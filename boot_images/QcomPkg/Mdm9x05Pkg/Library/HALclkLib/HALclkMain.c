/*
==============================================================================

FILE:         HALclkMain.c

DESCRIPTION:
  This file contains the main platform initialization code for the clock
  HAL on the apcs processor.


==============================================================================

$Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Library/HALclkLib/HALclkMain.c#1 $

==============================================================================
            Copyright (c) 2018 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*============================================================================

                     INCLUDE FILES FOR MODULE

============================================================================*/


#include "HALclkInternal.h"
#include "HALclkGeneric.h"
#include "Library/HALclkSharedLib/HALclkGenericPLL.h"
#include "HALhwio.h"
#include "HALclkHWIO.h"
#include "DDIPlatformInfo.h"
#include "Drivers/ClockDxe/ClockDriver.h"

/* ============================================================================
**    Prototypes
** ==========================================================================*/

void HAL_clk_PlatformInitSources(void);


/* ============================================================================
**    Externs
** ==========================================================================*/


extern void HAL_clk_PlatformInitGCCMain(void);
extern void HAL_clk_PlatformInitAPCSMain(void);



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
   * GCC
   */
  HAL_clk_PlatformInitGCCMain,

  /*
   * APCS
   */
  HAL_clk_PlatformInitAPCSMain,

  NULL
};



size_t HAL_clk_nHWIOBaseGCC;
size_t HAL_clk_nHWIOBaseHMSS;
size_t HAL_clk_nHWIOBaseSecCtrl;
size_t HAL_clk_nHWIOBaseCoreTopCsr;


/*
 * HAL_clk_aHWIOBases
 *
 * Declare array of HWIO bases in use on this platform.
 */
static HAL_clk_HWIOBaseType HAL_clk_aHWIOBases[] =
{

  { CLK_CTL_BASE_PHYS,          CLK_CTL_BASE_SIZE,          &HAL_clk_nHWIOBaseGCC        },
  { HMSS_BASE_PHYS,             HMSS_BASE_SIZE,             &HAL_clk_nHWIOBaseHMSS       },
  { SECURITY_CONTROL_BASE_PHYS, SECURITY_CONTROL_BASE_SIZE, &HAL_clk_nHWIOBaseSecCtrl    },
  { CORE_TOP_CSR_BASE_PHYS,     CORE_TOP_CSR_BASE_SIZE,     &HAL_clk_nHWIOBaseCoreTopCsr },

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

  {
    HWIO_OFFS(GCC_GPLL1_MODE),
    HAL_CLK_FMSK(GCC_APCS_GPLL_ENA_VOTE, GPLL1),
    HAL_CLK_PLL_STROMER
  },

  {
    HWIO_OFFS(GCC_GPLL3_MODE),
    {0,0},
    HAL_CLK_PLL_STROMER
  },

  {
    HWIO_OFFS(GCC_GPLL4_MODE),
    HAL_CLK_FMSK(GCC_APCS_GPLL_ENA_VOTE, GPLL4),
    HAL_CLK_PLL_STROMER
  },

  {
    HWIO_OFFS(GCC_GPLL6_MODE),
    HAL_CLK_FMSK(GCC_APCS_GPLL_ENA_VOTE, GPLL6),
    HAL_CLK_PLL_SR
  },

};

/*
 * HMSS PLL contexts
 */
static HAL_clk_PLLContextType HAL_clk_aPLLContextAPCSPLL[] =
{

  {
    HWIO_OFFS(APCS_C1_PLL_MODE),
    {0},
    HAL_CLK_PLL_HF
  },

};


/* ============================================================================
**    Functions
** ==========================================================================*/


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

  HAL_clk_InstallPLL(
      HAL_CLK_SOURCE_GPLL1, &HAL_clk_aPLLContextGCCPLL[1], CLK_CTL_BASE);

  HAL_clk_InstallPLL(
      HAL_CLK_SOURCE_GPLL3, &HAL_clk_aPLLContextGCCPLL[2], CLK_CTL_BASE);

  HAL_clk_InstallPLL(
      HAL_CLK_SOURCE_GPLL4, &HAL_clk_aPLLContextGCCPLL[3], CLK_CTL_BASE);

  HAL_clk_InstallPLL(
      HAL_CLK_SOURCE_GPLL6, &HAL_clk_aPLLContextGCCPLL[4], CLK_CTL_BASE);

  HAL_clk_InstallPLL(
    HAL_CLK_SOURCE_APCSPLL1, &HAL_clk_aPLLContextAPCSPLL[0], HMSS_BASE);
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
