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

$Header: //components/rel/core.tx/6.0/systemdrivers/hal/clk/hw/mdm9205/src/gcc/HALclkBLSP1UART3.c#1 $

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
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};
