/*
==============================================================================

FILE:         HALclkMain.c

DESCRIPTION:
  This file contains the main platform initialization code for the clock
  HAL on the modem subsystem (MSS) processor.

==============================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.10/systemdrivers/hal/clk/hw/mdm9205/src/HALclkMain.c#2 $

when       who     what, where, why
--------   ---     -----------------------------------------------------------
12/06/18      shm     First version for 9205

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
#include "err.h"

/* ============================================================================
**    Prototypes
** ==========================================================================*/

/*
 * HAL_clk_RegDumpType
 *
 * Struct to contain register values of system clocks at the time of modem assert.
 *
 */
typedef struct
{
  uint32 gcc_bimc_ddr_cmd_rcgr;
  uint32 gcc_bimc_ddr_cfg_rcgr;
  
  uint32 gcc_bimc_pll_mode;
  uint32 gcc_bimc_pll_l_val;
  uint32 gcc_bimc_pll_alpha_val;
  uint32 gcc_bimc_pll_alpha_val_u;
  uint32 gcc_bimc_pll_status;
  
  uint32 gcc_pcnoc_bfdcd_cmd_rcgr;
  uint32 gcc_pcnoc_bfdcd_cfg_rcgr;
} HAL_clk_RegDumpType;


void HAL_clk_PlatformInitSources (void);
void HAL_clk_RegisterErrorCB     (void);
void HAL_clk_SystemClocksSnapshot(void);

/* ============================================================================
**    Externs
** ==========================================================================*/

extern void HAL_clk_PlatformInitGCCMain(void);
extern void HAL_clk_PlatformInitMSSCCMain(void);

extern void HAL_clk_InstallMPLL1DivSources(void);


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
   * MSS
   */
  HAL_clk_PlatformInitMSSCCMain,

  /*
   * Error callback for clock snapshot
   */
  HAL_clk_RegisterErrorCB,

  NULL
};


/*
 * Declare variable to store system clocks snapshot
 */
HAL_clk_RegDumpType HAL_clk_Snapshot;

/*
 * Declare the base pointers for HWIO access.
 */
uint32 HAL_clk_nHWIOBaseTop;
uint32 HAL_clk_nHWIOBaseMSS;


/*
 * HAL_clk_aHWIOBases
 *
 * Declare array of HWIO bases in use on this platform.
 */
static HAL_clk_HWIOBaseType HAL_clk_aHWIOBases[] =
{
  { CLK_CTL_BASE_PHYS,          CLK_CTL_BASE_SIZE,          &HAL_clk_nHWIOBaseTop      },
  { MSS_TOP_BASE_PHYS,          MSS_TOP_BASE_SIZE,          &HAL_clk_nHWIOBaseMSS      },
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
 * GPLL contexts
 */
static HAL_clk_PLLContextType HAL_clk_aPLLContextGPLL[] =
{
  {
    HWIO_OFFS(GCC_GPLL0_MODE),
    HAL_CLK_FMSK(GCC_MSS_Q6_GPLL_ENA_VOTE, GPLL0),
    HAL_CLK_PLL_STROMER
  },
  {
    HWIO_OFFS(GCC_GPLL1_MODE),
    HAL_CLK_FMSK(GCC_MSS_Q6_GPLL_ENA_VOTE, GPLL1),
    HAL_CLK_PLL_SR
  }
};


/*
 * Modem PLL contexts - no voting support.
 */
static HAL_clk_PLLContextType HAL_clk_aPLLContextMPLL[] =
{
  {
    HWIO_OFFS(MSS_MPLL1_MODE),
    {0},
    HAL_CLK_PLL_SR2
  },
  {
    HWIO_OFFS(MSS_QDSP6SS_PLL_MODE),
    {0},
    HAL_CLK_PLL_STROMER
  }
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
    HAL_CLK_SOURCE_GPLL0, &HAL_clk_aPLLContextGPLL[0], CLK_CTL_BASE);

  HAL_clk_InstallPLL(
    HAL_CLK_SOURCE_GPLL1, &HAL_clk_aPLLContextGPLL[1], CLK_CTL_BASE);

  HAL_clk_InstallPLL(
    HAL_CLK_SOURCE_MPLL1, &HAL_clk_aPLLContextMPLL[0], MSS_TOP_BASE);

  HAL_clk_InstallPLL(
    HAL_CLK_SOURCE_MPLL2, &HAL_clk_aPLLContextMPLL[1], MSS_TOP_BASE);

} /* END HAL_clk_PlatformInitSources */

void HAL_clk_RegisterErrorCB (void)
{
  err_crash_cb_register(HAL_clk_SystemClocksSnapshot);
  return;
}

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


/* ===========================================================================
**  HAL_clk_SystemClocksSnapshot
** ======================================================================== */

void HAL_clk_SystemClocksSnapshot (void)
{ 
  HAL_clk_Snapshot.gcc_bimc_ddr_cmd_rcgr =
    HWIO_IN(GCC_BIMC_DDR_CMD_RCGR);
  HAL_clk_Snapshot.gcc_bimc_ddr_cfg_rcgr =
    HWIO_IN(GCC_BIMC_DDR_CFG_RCGR);
  HAL_clk_Snapshot.gcc_bimc_pll_mode =
    HWIO_IN(GCC_BIMC_PLL_MODE);
  HAL_clk_Snapshot.gcc_bimc_pll_l_val =
    HWIO_IN(GCC_BIMC_PLL_L_VAL);
  HAL_clk_Snapshot.gcc_bimc_pll_alpha_val =
    HWIO_IN(GCC_BIMC_PLL_ALPHA_VAL);
  HAL_clk_Snapshot.gcc_bimc_pll_alpha_val_u =
    HWIO_IN(GCC_BIMC_PLL_ALPHA_VAL_U);
  HAL_clk_Snapshot.gcc_bimc_pll_status =
    HWIO_IN(GCC_BIMC_PLL_STATUS);
  HAL_clk_Snapshot.gcc_pcnoc_bfdcd_cmd_rcgr =
    HWIO_IN(GCC_PCNOC_BFDCD_CMD_RCGR);
  HAL_clk_Snapshot.gcc_pcnoc_bfdcd_cfg_rcgr =
    HWIO_IN(GCC_PCNOC_BFDCD_CFG_RCGR);
  
  return;
}
