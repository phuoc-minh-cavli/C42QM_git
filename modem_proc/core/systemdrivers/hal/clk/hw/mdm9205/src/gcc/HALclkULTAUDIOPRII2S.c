/*
==============================================================================

FILE:         HALclkULTAUDIOPRII2S.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the
   ULTAUDIOPRII2S clocks.

   List of clock domains:
    - HAL_clk_mGCCGCCULTAUDIOLPAIFPRII2SClkDomain


   List of power domains:



==============================================================================

$Header: //components/rel/core.mpss/3.10/systemdrivers/hal/clk/hw/mdm9205/src/gcc/HALclkULTAUDIOPRII2S.c#2 $

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
 *  HAL_clk_mGCCULTAUDIOLPAIFPRII2SClkDomainClks
 *
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mGCCULTAUDIOLPAIFPRII2SClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_ultaudio_lpaif_pri_i2s_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_ULTAUDIO_LPAIF_PRI_I2S_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_ULTAUDIO_LPAIF_PRI_I2S_CLK
  },
};


/*
 * HAL_clk_mGCCGCCULTAUDIOLPAIFPRII2SClkDomain
 *
 * GCCULTAUDIOLPAIFPRII2S clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCULTAUDIOLPAIFPRII2SClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_ULTAUDIO_LPAIF_PRI_I2S_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mGCCULTAUDIOLPAIFPRII2SClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mGCCULTAUDIOLPAIFPRII2SClkDomainClks)/sizeof(HAL_clk_mGCCULTAUDIOLPAIFPRII2SClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl_2,
  /* .pmNextClockDomain    = */ NULL
};
