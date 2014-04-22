/*
==============================================================================

FILE:         HALclkPRNG.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the
   PRNG clocks.

   List of clock domains:
    - HAL_clk_mGCCPRNGClkDomain


   List of power domains:



==============================================================================

$Header: //components/rel/core.tx/6.0/systemdrivers/hal/clk/hw/mdm9205/src/gcc/HALclkPRNG.c#1 $

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

extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControl;
extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControlRO;


/* ============================================================================
**    Data
** ==========================================================================*/


/*
 *  HAL_clk_mPRNGClkDomainClks
 *
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mPRNGClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_prng_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PRNG_AHB_CBCR), HWIO_OFFS(GCC_PRNG_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, PRNG_AHB_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PRNG_AHB_CLK
  },
};


/*
 * HAL_clk_mGCCPRNGClkDomain
 *
 * PRNG clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCPRNGClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_PRNG_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mPRNGClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mPRNGClkDomainClks)/sizeof(HAL_clk_mPRNGClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControlRO,
  /* .pmNextClockDomain    = */ NULL
};
