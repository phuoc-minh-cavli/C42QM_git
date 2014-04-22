/*
==============================================================================

FILE:         HALclkMain.c

DESCRIPTION:
  This file contains the main platform initialization code for the clock
  HAL on the APCS processor.


==============================================================================

$Header: //components/rel/core.tx/6.0/systemdrivers/hal/clk/hw/mdm9205/src/HALclkMain.c#1 $

==============================================================================
            Copyright (c) 2018 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/


#include "HALclkInternal.h"
#include "HALclkGeneric.h"
#include "HALclkGenericPLL.h"
#include "HALhwio.h"
#include "HALclkHWIO.h"


/* ============================================================================
**    Prototypes
** ==========================================================================*/

void HAL_clk_PlatformInitSources(void);


/* ============================================================================
**    Externs
** ==========================================================================*/


extern void HAL_clk_PlatformInitAPCSMain(void);
extern void HAL_clk_PlatformInitGCCMain(void);
extern HAL_clk_SourceControlType HAL_clk_StromerAPCSPLLControl;


/* ============================================================================
**    Data
** ==========================================================================*/


/*
 * HAL_clk_aInitFuncs
 *
 * Declare array of module initialization functions.
 */
static HAL_clk_InitFuncType HAL_clk_afInitFuncs[] =
{
  /*
   * Sources
   */
  HAL_clk_PlatformInitSources,

  /*
   * APCS
   */
  HAL_clk_PlatformInitAPCSMain,

  /*
   * GCC
   */
  HAL_clk_PlatformInitGCCMain,

  NULL
};



uint32 HAL_clk_nHWIOBaseAPCS;
uint32 HAL_clk_nHWIOBaseGCC;


/*
 * HAL_clk_aHWIOBases
 *
 * Declare array of HWIO bases in use on this platform.
 */
static HAL_clk_HWIOBaseType HAL_clk_aHWIOBases[] =
{

  { A7SS_BASE_PHYS,           A7SS_BASE_SIZE,           &HAL_clk_nHWIOBaseAPCS       },
  { CLK_CTL_BASE_PHYS,        CLK_CTL_BASE_SIZE,        &HAL_clk_nHWIOBaseGCC        },

  { 0, 0, NULL }
};


/*
 * HAL_clk_Platform;
 * Platform data.
 */
HAL_clk_PlatformType HAL_clk_Platform =
{
  HAL_clk_afInitFuncs,
  HAL_clk_aHWIOBases
};


/*
 * GCC PLL contexts
 */
static HAL_clk_PLLContextType HAL_clk_aPLLContextGCCPLL[] =
{

  {
    HWIO_OFFS(GCC_GPLL0_MODE),
    HAL_CLK_FMSK(GCC_APCS_GPLL_ENA_VOTE, GPLL0),
    HAL_CLK_PLL_STROMER
  },

};


/*
 * APCS PLL contexts
 */
static HAL_clk_PLLContextType HAL_clk_aPLLContextAPCSPLL[] =
{

  {
    HWIO_OFFS(APCS_CPU_PLL_MODE),
    {0},
    HAL_CLK_PLL_STROMER
  },

};

/* ============================================================================
**    Functions
** ==========================================================================*/

/* ===========================================================================
**  HAL_clk_InstallPLL
**
** ======================================================================== */

void HAL_clk_InstallAPCSPLL
(
  HAL_clk_SourceType      eSource,
  HAL_clk_PLLContextType *pCtxt,
  uint32                  nBaseAddress
)
{
  /*
   * Add base address
   */
  pCtxt->nAddr += nBaseAddress;

  if (pCtxt->VoterRegister.nAddr != 0)
  {
    pCtxt->VoterRegister.nAddr += nBaseAddress;
  }

  /*
   * Install PLL handler
   */
  if (pCtxt->ePLLType == HAL_CLK_PLL_STROMER)
  {
    HAL_clk_InstallSource(eSource, &HAL_clk_StromerAPCSPLLControl, pCtxt);
  }
  else
  {
    // error but no return parameter to provide!
  }

} /* END HAL_clk_InstallAPCSPLL */


/* ===========================================================================
**  HAL_clk_PlatformInitSources
**
** ======================================================================== */

void HAL_clk_PlatformInitSources (void)
{
  /*
   * Install PLL handlers.
   */

  HAL_clk_InstallPLL(
      HAL_CLK_SOURCE_GPLL0, &HAL_clk_aPLLContextGCCPLL[0], CLK_CTL_BASE);

  HAL_clk_InstallAPCSPLL(
      HAL_CLK_SOURCE_APCSPLL0, &HAL_clk_aPLLContextAPCSPLL[0], A7SS_BASE);

} /* END HAL_clk_PlatformInitSources */


/* ===========================================================================
**  HAL_clk_Save
**
** ======================================================================== */

void HAL_clk_Save (void)
{
  /*
   * Nothing to save.
   */

} /* END HAL_clk_Save */


/* ===========================================================================
**  HAL_clk_Restore
**
** ======================================================================== */

void HAL_clk_Restore (void)
{
  /*
   * Nothing to restore.
   */

} /* END HAL_clk_Restore */
