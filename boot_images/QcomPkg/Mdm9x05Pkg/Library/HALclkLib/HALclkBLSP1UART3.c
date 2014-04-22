/*
==============================================================================

FILE:         HALclkBLSP1UART3.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   BLSP1UART3 clocks.

   List of clock domains:
     - HAL_clk_mGCCBLSP1UART3APPSClkDomain


   List of power domains:



==============================================================================

$Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Library/HALclkLib/HALclkBLSP1UART3.c#1 $

==============================================================================
            Copyright (c) 2018 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/


#include <HALhwio.h>

#include "HALclkInternal.h"
#include "HALclkTest.h"
#include "HALclkGeneric.h"
#include "HALclkHWIO.h"


/*============================================================================

             DEFINITIONS AND DECLARATIONS FOR MODULE

=============================================================================*/


/* ============================================================================
**    Prototypes
** ==========================================================================*/


/* ============================================================================
**    Externs
** ==========================================================================*/

extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCUART3ClockDomainControl;

/* ============================================================================
**    Data
** ==========================================================================*/

/* ===========================================================================
**  HAL_clk_UART3ConfigMux
**  This custom function is implemented to handle BLSP1_UART3 clock configs
**  This blsp1_uart3 clock registers are not aligned properly in SWI
**  so using direct register addresses instead of offsets
**
** ======================================================================== */

void HAL_clk_UART3ConfigMux
(
  HAL_clk_ClockDomainDescType      *pmClockDomainDesc,
  const HAL_clk_ClockMuxConfigType *pmConfig
)
{
  size_t                 nCmdCGRAddr, nCfgCGRAddr;
  uint32                 nCmdCGRVal, nCfgCGRVal;
  size_t                 nMAddr, nNAddr, nDAddr;
  uint32                 nSourceIndex = 0;
  HAL_clk_SourceMapType *pmSourceMap;

  /*
   * Sanity check
   */
  if ((pmConfig                                    == NULL) ||
      (pmClockDomainDesc                           == NULL) ||
      (pmClockDomainDesc->nCGRAddr                 == 0   ) ||
      (pmClockDomainDesc->pmControl                == NULL) ||
      (pmClockDomainDesc->pmControl->pmSourceMap   == NULL) )
  {
    return;
  }

  /*
   * Get current CMD and CFG register values
   */
  nCmdCGRAddr = pmClockDomainDesc->nCGRAddr;
  nCmdCGRVal  = inpdw(nCmdCGRAddr);
  nCfgCGRAddr = HWIO_ADDR(GCC_BLSP1_UART3_APPS_CFG_RCGR);
  nCfgCGRVal  = inpdw(nCfgCGRAddr);

  /*
   * Clear the fields
   */
  nCfgCGRVal &= ~(HAL_CLK_CFG_CGR_SRC_SEL_FMSK |
                  HAL_CLK_CFG_CGR_SRC_DIV_FMSK |
                  HAL_CLK_CFG_CGR_MODE_FMSK);

  /*
   * Get source index from source enum
   */
  pmSourceMap = pmClockDomainDesc->pmControl->pmSourceMap;
  nSourceIndex = HAL_clk_GenericSourceMapToHW(pmSourceMap, pmConfig->eSource);

  /*
   * Bail if could not find matching source index
   */
  if (nSourceIndex == HAL_CLK_SOURCE_INDEX_INVALID)
  {
    return;
  }

  /*
   * Program the source and divider values.
   */
  nCfgCGRVal |= ((nSourceIndex << HAL_CLK_CFG_CGR_SRC_SEL_SHFT) & HAL_CLK_CFG_CGR_SRC_SEL_FMSK);
  nCfgCGRVal |= ((HALF_DIVIDER(pmConfig) << HAL_CLK_CFG_CGR_SRC_DIV_SHFT)
                  & HAL_CLK_CFG_CGR_SRC_DIV_FMSK);

  /*
   * Set MND counter mode depending on if it is in use
   */
  if (pmConfig->nM != 0 && (pmConfig->nM < pmConfig->nN))
  {
    /*
     * Get M, N and D addresses
     */
    nMAddr = HWIO_ADDR(GCC_BLSP1_UART3_APPS_M);//pmClockDomainDesc->nCGRAddr + HAL_CLK_M_REG_OFFSET;
    nNAddr = HWIO_ADDR(GCC_BLSP1_UART3_APPS_N);//pmClockDomainDesc->nCGRAddr + HAL_CLK_N_REG_OFFSET;
    nDAddr = HWIO_ADDR(GCC_BLSP1_UART3_APPS_D);//pmClockDomainDesc->nCGRAddr + HAL_CLK_D_REG_OFFSET;

    /*
     * Set M value
     */
    outpdw(nMAddr, pmConfig->nM);

    /*
     * Set N value
     */
    outpdw(nNAddr, NOT_N_MINUS_M(pmConfig));

    /*
     * Set D value
     */
    outpdw(nDAddr, NOT_2D(pmConfig));

    /*
     * Dual-edge mode
     */
    nCfgCGRVal |= ((HAL_CLK_CFG_CFG_DUAL_EDGE_MODE_VAL << HAL_CLK_CFG_CGR_MODE_SHFT)
                    & HAL_CLK_CFG_CGR_MODE_FMSK);
  }

  /*
   * Write the final CFG register value
   */
  outpdw(nCfgCGRAddr, nCfgCGRVal);

  /*
   * Trigger the update
   */
  nCmdCGRVal |= HAL_CLK_CMD_CFG_UPDATE_FMSK;
  outpdw(nCmdCGRAddr, nCmdCGRVal);

  /*
   * Wait until update finishes
   */
  while(inpdw(nCmdCGRAddr) & HAL_CLK_CMD_CFG_UPDATE_FMSK);

} /* HAL_clk_UART3ConfigMux */


/* ===========================================================================
**  HAL_clk_UART3DetectMuxConfig
**
** ======================================================================== */

void HAL_clk_UART3DetectMuxConfig
(
  HAL_clk_ClockDomainDescType *pmClockDomainDesc,
  HAL_clk_ClockMuxConfigType  *pmConfig
)
{
  size_t                 nCfgCGRAddr;
  uint32                 nCfgCGRVal, nSourceIndex;
  HAL_clk_SourceType     eSource;
  HAL_clk_SourceMapType *pmSourceMap;

  if (pmConfig == NULL)
  {
    return;
  }

  pmConfig->eSource = HAL_CLK_SOURCE_NULL;
  pmConfig->nDiv2x  = 0;
  pmConfig->n2D     = 0;
  pmConfig->nM      = 0;
  pmConfig->nN      = 0;

  /*
   * Sanity check
   */
  if((pmClockDomainDesc                           == NULL) ||
     (pmClockDomainDesc->nCGRAddr                 == 0   ) ||
     (pmClockDomainDesc->pmControl                == NULL) ||
     (pmClockDomainDesc->pmControl->pmSourceMap   == NULL) )
  {
    return;
  }

  /*
   * NOTES:
   *
   * -- pmSourceMap returns value based on the "common" mapping.  Individual
   *    HAL file may need to implement its own mapping if common one is too
   *    generic.
   *
   * -- Generic DetectMuxConfig only detects source and divider values.
   *    Individual SS HAL file will need to implement its own DetectMuxConfig
   *    for a proper detection of M, N, and D values.
   */

  /*
   * Get current CMD and CFG register values
   */
  nCfgCGRAddr = HWIO_ADDR(GCC_BLSP1_UART3_APPS_CFG_RCGR);//pmClockDomainDesc->nCGRAddr + HAL_CLK_CFG_REG_OFFSET;
  nCfgCGRVal  = inpdw(nCfgCGRAddr);

  /*
   * Get the source
   */
  nSourceIndex = (nCfgCGRVal & HAL_CLK_CFG_CGR_SRC_SEL_FMSK) >> HAL_CLK_CFG_CGR_SRC_SEL_SHFT;

  /*
   * Get source enum from source index
   */
  pmSourceMap = pmClockDomainDesc->pmControl->pmSourceMap;
  eSource = HAL_clk_GenericSourceMapFromHW(pmSourceMap, nSourceIndex);

  /*
   * Bail if could not find matching source enum
   */
  if (eSource == HAL_CLK_SOURCE_NULL)
  {
    return;
  }

  pmConfig->eSource = eSource;

  /*
   * Get the divider
   */
  pmConfig->nDiv2x =
    ((nCfgCGRVal & HAL_CLK_CFG_CGR_SRC_DIV_FMSK) >> HAL_CLK_CFG_CGR_SRC_DIV_SHFT) + 1;

} /* END HAL_clk_UART3DetectMuxConfig */



/*                           
 *  HAL_clk_mBLSP1UART3APPSClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mBLSP1UART3APPSClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_blsp1_uart3_apps_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BLSP1_UART3_APPS_CBCR), HWIO_OFFS(GCC_BLSP1_UART3_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BLSP1_UART3_APPS_CLK
  },
};


/*
 * HAL_clk_mGCCBLSP1UART3APPSClkDomain
 *
 * BLSP1UART3APPS clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1UART3APPSClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_BLSP1_UART3_APPS_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mBLSP1UART3APPSClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mBLSP1UART3APPSClkDomainClks)/sizeof(HAL_clk_mBLSP1UART3APPSClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCUART3ClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};

