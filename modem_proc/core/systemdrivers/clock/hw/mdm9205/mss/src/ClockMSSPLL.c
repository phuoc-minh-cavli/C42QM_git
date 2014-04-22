/*
===========================================================================
*/
/**
  @file ClockMSSPLL.c

  Modem PLL NPA node definitions for the MSS clock driver.
*/
/*
  ====================================================================

  Copyright (c) 2018 Qualcomm Technologies Incorporated.
  All rights reserved.
  QUALCOMM Proprietary and Confidential.

  ==================================================================== 
  $Header: //components/rel/core.mpss/3.10/systemdrivers/clock/hw/mdm9205/mss/src/ClockMSSPLL.c#2 $
  $DateTime: 2019/01/02 21:42:16 $
  $Author: pwbldsvc $
 
  when       who     what, where, why
  --------   ---     -------------------------------------------------
  12/11/18   shm     First version for 9205

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockDriver.h"
#include "ClockMSS.h"
#include "DALDeviceId.h"
#include "DDIVCS.h"

#include <DALSys.h>
#include <npa.h>
#include <npa_resource.h>
#include <npa_remote.h>
#include <npa_remote_resource.h>
#include "pmapp_npa.h"


/*=========================================================================
      Macros
==========================================================================*/

/*
 * PMIC PLL SR2 client and resource names
 */
#define CLOCK_PMIC_NPA_GROUP_ID_PLL_SR2            "/pmic/client/pll_sr2"
#define CLOCK_PMIC_RESOURCE_PLL_SR2_CLIENT         "/clkregim/pll_sr2"

/*
 * Generic active value for appropriate PLL node.
 */
#define CLOCK_PMIC_NPA_MODE_ID_GENERIC_ACTIVE      1


/*=========================================================================
      Type Definitions
==========================================================================*/

/*
 * Enumeration of modem PLLs.
 */
enum
{
  CLOCK_MODEM_PLL1,
  CLOCK_MODEM_PLL2,
  CLOCK_MODEM_PLL_TOTAL
};

/*=========================================================================
      Prototypes
==========================================================================*/

static void Clock_PLLSR2ReadyCallback
(
  void *pContext
);

/*=========================================================================
      Data
==========================================================================*/



/*=========================================================================
      Functions
==========================================================================*/

/* =========================================================================
**  Function : Clock_PLLSR2ReadyCallback
** =========================================================================*/
/**
  Callback after /pmic/client/pll_sr2 node is created.

  This function is called by the NPA framework after the /pmic/client/pll_sr2
  node is created.  The creation is delayed until all dependencies are also
  created.

  @param *pContext [in] -- Context pointer, the driver context in this case.

  @return
  None.

  @dependencies
  None.
 */

static void Clock_PLLSR2ReadyCallback
(
  void *pContext
)
{
  npa_client_handle hNPAPMICPLLSR2;

  /*-----------------------------------------------------------------------*/
  /* Get a handle to PMIC_NPA_GROUP_ID_PLL_SR2 node                        */
  /*-----------------------------------------------------------------------*/

  hNPAPMICPLLSR2 = npa_create_sync_client(
                     CLOCK_PMIC_NPA_GROUP_ID_PLL_SR2,
                     CLOCK_PMIC_RESOURCE_PLL_SR2_CLIENT,
                     NPA_CLIENT_SUPPRESSIBLE);

  if (hNPAPMICPLLSR2 == NULL)
  {
    DALSYS_LogEvent (
      DALDEVICEID_CLOCK,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to create NPA sync client %s.",
      CLOCK_PMIC_RESOURCE_PLL_SR2_CLIENT);

    return;
  }

  /*-----------------------------------------------------------------------*/
  /* Vote for SR2 PLL active state                                         */
  /*-----------------------------------------------------------------------*/

  npa_issue_scalar_request(hNPAPMICPLLSR2, CLOCK_PMIC_NPA_MODE_ID_GENERIC_ACTIVE);

} /*Clock_PLLSR2ReadyCallback */

/* =========================================================================
**  Function : Clock_InitPLL
** =========================================================================*/
/*
  See ClockMSS.h
*/ 

DALResult Clock_InitPLL
(
  ClockDrvCtxt *pDrvCtxt
)
{
  DALResult           eResult;
  ClockCPUConfigType    *pConfig;
  ClockSourceNodeType    *pSource;
  HAL_clk_PLLConfigType  *pHALConfig;
  ClockImageCtxtType    *pImageCtxt;
  uint32                 nPL, nConfig;
  uint32                 nSourceIndex;
  boolean                bResult;

  pImageCtxt = (ClockImageCtxtType *)pDrvCtxt->pImageCtxt;

  /*-----------------------------------------------------------------------*/
  /* Register a callback for the appropriate PLL node.                     */
  /*-----------------------------------------------------------------------*/

  eResult =
    Clock_RegisterResourceCallback(
      pDrvCtxt,
                                        CLOCK_PMIC_NPA_GROUP_ID_PLL_SR2,
                                        Clock_PLLSR2ReadyCallback,
                                        pDrvCtxt);
  if (eResult != DAL_SUCCESS)
  {
    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Find the init performance level.                                      */
  /*-----------------------------------------------------------------------*/

  nPL = pImageCtxt->CPUCtxt.PerfConfig.nInitPerfLevel;
  nConfig = pImageCtxt->CPUCtxt.PerfConfig.anPerfLevel[nPL];
  pConfig = &pImageCtxt->pBSPConfig->pCPUConfig[nConfig];

  /*-----------------------------------------------------------------------*/
  /* Satisfy voltage dependency for the init performance level.            */
  /*-----------------------------------------------------------------------*/

  eResult =
    DalVCS_SetCPUCorner(
      pImageCtxt->hVCS,
      CLOCK_CPU_MSS_Q6,
      pConfig->eCornerMSS);
  if (eResult != DAL_SUCCESS)
  {
    DALSYS_LogEvent(
      DALDEVICEID_CLOCK,
      DALSYS_LOGEVENT_FATAL_ERROR,
      "Unable to set voltage for init performance level.");

    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Get source node.                                                      */
  /*-----------------------------------------------------------------------*/

  nSourceIndex = pDrvCtxt->anSourceIndex[pConfig->Mux.HALConfig.eSource];
  if (nSourceIndex == 0xFF)
  {
    return DAL_ERROR;
  }

  pSource = &pDrvCtxt->aSources[nSourceIndex];
  if (pSource == NULL)
    {
    return DAL_ERROR;
  }

  /*-----------------------------------------------------------------------*/
  /* Mark the Q6 PLL as having already been calibrated - done in ASM.      */
  /*-----------------------------------------------------------------------*/

  pSource->nFlags |= CLOCK_FLAG_SOURCE_CALIBRATED;

  /*-----------------------------------------------------------------------*/
  /* Slew MPLL2 (Q6PLL) to the init freq config                            */
  /*-----------------------------------------------------------------------*/

  pHALConfig = &pConfig->Mux.pSourceFreqConfig->HALConfig;
  bResult =
    HAL_clk_ConfigPLL(
      pSource->eSource,
      pHALConfig,
      HAL_CLK_SOURCE_CONFIG_MODE_SLEW);
  if (bResult != TRUE)
  {
    return DAL_ERROR_INTERNAL;
  }

  /*-----------------------------------------------------------------------*/
  /* Set the active config for Q6PLL.                                      */
  /*-----------------------------------------------------------------------*/

  pSource->pActiveFreqConfig = pConfig->Mux.pSourceFreqConfig;

  /*-----------------------------------------------------------------------*/
  /* Configure MPLL1 to the default performance level.                     */
  /*-----------------------------------------------------------------------*/

  eResult = Clock_InitSource(pDrvCtxt, HAL_CLK_SOURCE_MPLL1, NULL);
  if (eResult != DAL_SUCCESS)
  {
    return eResult;
  }

  /*-----------------------------------------------------------------------*/
  /* Good to go.                                                           */
  /*-----------------------------------------------------------------------*/

  return DAL_SUCCESS;

} /* END Clock_InitPLL */
