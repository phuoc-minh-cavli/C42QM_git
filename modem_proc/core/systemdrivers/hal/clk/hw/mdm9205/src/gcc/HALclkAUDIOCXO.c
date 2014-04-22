/*
==============================================================================

FILE:         HALclkAUDIOCXO.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the
   AUDIOCXO clocks.

   List of clock domains:
    - HAL_clk_mGCCGCCULTAUDIOXOClkDomain


   List of power domains:



==============================================================================

$Header: //components/rel/core.mpss/3.10/systemdrivers/hal/clk/hw/mdm9205/src/gcc/HALclkAUDIOCXO.c#2 $

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

extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControl_2;


/* ============================================================================
**    Data
** ==========================================================================*/


/*
 *  HAL_clk_mGCCULTAUDIOXOClkDomainClks
 *
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mGCCULTAUDIOXOClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_ultaudio_avsync_xo_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_ULTAUDIO_AVSYNC_XO_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_ULTAUDIO_AVSYNC_XO_CLK
  },
  {
    /* .szClockName      = */ "gcc_ultaudio_stc_xo_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_ULTAUDIO_STC_XO_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_ULTAUDIO_STC_XO_CLK
  },
};


/*
 * HAL_clk_mGCCGCCULTAUDIOXOClkDomain
 *
 * GCCULTAUDIOXO clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCULTAUDIOXOClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_ULTAUDIO_XO_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mGCCULTAUDIOXOClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mGCCULTAUDIOXOClkDomainClks)/sizeof(HAL_clk_mGCCULTAUDIOXOClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl_2,
  /* .pmNextClockDomain    = */ NULL
};
