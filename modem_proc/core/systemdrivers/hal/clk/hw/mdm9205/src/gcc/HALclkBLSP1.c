/*
==============================================================================

FILE:         HALclkBLSP1.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the
   BLSP1 clocks.

   List of clock domains:
    - HAL_clk_mGCCBLSPUARTSIMClkDomain


   List of power domains:



==============================================================================

$Header: //components/rel/core.mpss/3.10/systemdrivers/hal/clk/hw/mdm9205/src/gcc/HALclkBLSP1.c#2 $

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
 *  HAL_clk_mBLSPUARTSIMClkDomainClks
 *
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mBLSPUARTSIMClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_blsp1_uart1_sim_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BLSP1_UART1_SIM_CBCR), HWIO_OFFS(GCC_BLSP1_UART1_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BLSP1_UART1_SIM_CLK
  },
  {
    /* .szClockName      = */ "gcc_blsp1_uart2_sim_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BLSP1_UART2_SIM_CBCR), HWIO_OFFS(GCC_BLSP1_UART2_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BLSP1_UART2_SIM_CLK
  },
  {
    /* .szClockName      = */ "gcc_blsp1_uart3_sim_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BLSP1_UART3_SIM_CBCR), HWIO_OFFS(GCC_BLSP1_UART3_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BLSP1_UART3_SIM_CLK
  },
  {
    /* .szClockName      = */ "gcc_blsp1_uart4_sim_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BLSP1_UART4_SIM_CBCR), HWIO_OFFS(GCC_BLSP1_UART4_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BLSP1_UART4_SIM_CLK
  },
};


/*
 * HAL_clk_mGCCBLSPUARTSIMClkDomain
 *
 * BLSPUARTSIM clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSPUARTSIMClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_BLSP_UART_SIM_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mBLSPUARTSIMClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mBLSPUARTSIMClkDomainClks)/sizeof(HAL_clk_mBLSPUARTSIMClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};
