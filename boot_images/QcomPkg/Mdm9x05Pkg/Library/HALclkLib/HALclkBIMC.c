/*
==============================================================================

FILE:         HALclkBIMC.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   BIMC clocks.

   List of clock domains:
     - HAL_clk_mGCCBIMCDDRClkDomain
     - HAL_clk_mGCCBIMCGPUClkDomain


   List of power domains:
     - HAL_clk_mGCCBIMCPowerDomain



==============================================================================

$Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Library/HALclkLib/HALclkBIMC.c#1 $

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

extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControl_5;
extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControlRO_5;


/* ============================================================================
**    Data
** ==========================================================================*/


/*                           
 *  HAL_clk_mBIMCDDRClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mBIMCDDRClkDomainClks[] =
{
#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_bimc_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BIMC_CBCR), HWIO_OFFS(GCC_BIMC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIMC_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_spdm_bimc_cy_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SPDM_BIMC_CY_CBCR), HWIO_OFFS(GCC_SPDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SPDM_BIMC_CY_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_spdm_debug_cy_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SPDM_DEBUG_CY_CBCR), HWIO_OFFS(GCC_SPDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ 0
  },
#endif

};


/*
 * HAL_clk_mGCCBIMCDDRClkDomain
 *
 * BIMCDDR clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCBIMCDDRClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_BIMC_DDR_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mBIMCDDRClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mBIMCDDRClkDomainClks)/sizeof(HAL_clk_mBIMCDDRClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControlRO_5,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mBIMCGPUClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mBIMCGPUClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_bimc_gfx_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BIMC_GFX_CBCR), HWIO_OFFS(GCC_OXILI_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIMC_GFX_CLK
  },
#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_bimc_gpu_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BIMC_GPU_CBCR), HWIO_OFFS(GCC_BIMC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIMC_GPU_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_gfx_tbu_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_GFX_TBU_CBCR), HWIO_OFFS(GCC_SMMU_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_GFX_TBU_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_gfx_tcu_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_GFX_TCU_CBCR), HWIO_OFFS(GCC_SMMU_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_GFX_TCU_CLK
  },
#endif

};


/*
 * HAL_clk_mGCCBIMCGPUClkDomain
 *
 * BIMCGPU clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCBIMCGPUClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_BIMC_GPU_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mBIMCGPUClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mBIMCGPUClkDomainClks)/sizeof(HAL_clk_mBIMCGPUClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControlRO_5,
  /* .pmNextClockDomain    = */ NULL
};

