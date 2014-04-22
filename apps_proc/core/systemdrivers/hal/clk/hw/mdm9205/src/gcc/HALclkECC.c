/*
==============================================================================

FILE:         HALclkECC.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the
   ECC clocks.

   List of clock domains:
    - HAL_clk_mGCCGCCECCClkDomain


   List of power domains:



==============================================================================

$Header: //components/rel/core.tx/6.0/systemdrivers/hal/clk/hw/mdm9205/src/gcc/HALclkECC.c#1 $

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


/* ============================================================================
**    Data
** ==========================================================================*/


/*
 *  HAL_clk_mGCCECCClkDomainClks
 *
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mGCCECCClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_ecc_core_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_ECC_CORE_CBCR), HWIO_OFFS(GCC_ECC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_ECC_CORE_CLK
  },
};


/*
 * HAL_clk_mGCCGCCECCClkDomain
 *
 * GCCECC clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCECCClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_ECC_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mGCCECCClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mGCCECCClkDomainClks)/sizeof(HAL_clk_mGCCECCClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};
