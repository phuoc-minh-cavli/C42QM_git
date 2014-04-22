/*
==============================================================================

FILE:         HALclkULTAUDIOEXTMCLK2.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the
   ULTAUDIOEXTMCLK2 clocks.

   List of clock domains:
    - HAL_clk_mGCCGCCULTAUDIOEXTMCLK2ClkDomain


   List of power domains:



==============================================================================

$Header: //components/rel/core.mpss/3.10/systemdrivers/hal/clk/hw/mdm9205/src/gcc/HALclkULTAUDIOEXTMCLK2.c#2 $

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
 *  HAL_clk_mGCCULTAUDIOEXTMCLK2ClkDomainClks
 *
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mGCCULTAUDIOEXTMCLK2ClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_ultaudio_ext_mclk2_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_ULTAUDIO_EXT_MCLK2_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_ULTAUDIO_EXT_MCLK2_CLK
  },
};


/*
 * HAL_clk_mGCCGCCULTAUDIOEXTMCLK2ClkDomain
 *
 * GCCULTAUDIOEXTMCLK2 clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCULTAUDIOEXTMCLK2ClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_ULTAUDIO_EXT_MCLK2_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mGCCULTAUDIOEXTMCLK2ClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mGCCULTAUDIOEXTMCLK2ClkDomainClks)/sizeof(HAL_clk_mGCCULTAUDIOEXTMCLK2ClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl_2,
  /* .pmNextClockDomain    = */ NULL
};
