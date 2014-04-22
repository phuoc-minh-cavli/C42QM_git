/*
==============================================================================

FILE:         HALclkBLSP2UART0.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   BLSP2UART0 clocks.

   List of clock domains:
     - HAL_clk_mGCCBLSP2UART0APPSClkDomain


   List of power domains:



==============================================================================

$Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Library/HALclkLib/HALclkBLSP2UART0.c#1 $

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

extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControl;
extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControlRO;


/* ============================================================================
**    Data
** ==========================================================================*/


/*                           
 *  HAL_clk_mBLSP2UART0APPSClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mBLSP2UART0APPSClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_blsp2_uart0_apps_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BLSP2_UART0_APPS_CBCR), HWIO_OFFS(GCC_BLSP2_UART0_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BLSP2_UART0_APPS_CLK
  },
};


/*
 * HAL_clk_mGCCBLSP2UART0APPSClkDomain
 *
 * BLSP2UART0APPS clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP2UART0APPSClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_BLSP2_UART0_APPS_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mBLSP2UART0APPSClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mBLSP2UART0APPSClkDomainClks)/sizeof(HAL_clk_mBLSP2UART0APPSClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};

