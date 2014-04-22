/*
==============================================================================

FILE:         HALclkBIMC.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the
   BIMC clocks.

   List of clock domains:
    - HAL_clk_mGCCBIMCDDRClkDomain


   List of power domains:



==============================================================================

$Header: //components/rel/core.mpss/3.10/systemdrivers/hal/clk/hw/mdm9205/src/gcc/HALclkBIMC.c#2 $

==============================================================================
            Copyright (c) 2018 Qualcomm Technologies Incorporated.
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

extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControlRO;


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
#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_bimc_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BIMC_CBCR), HWIO_OFFS(GCC_BIMC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIMC_CLK
  },
#endif

  {
    /* .szClockName      = */ "gcc_mss_q6_bimc_axi_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_MSS_Q6_BIMC_AXI_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_MSS_Q6_BIMC_AXI_CLK
  },
#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_spdm_bimc_cy_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SPDM_BIMC_CY_CBCR), HWIO_OFFS(GCC_SPDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SPDM_BIMC_CY_CLK
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
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControlRO,
  /* .pmNextClockDomain    = */ NULL
};
