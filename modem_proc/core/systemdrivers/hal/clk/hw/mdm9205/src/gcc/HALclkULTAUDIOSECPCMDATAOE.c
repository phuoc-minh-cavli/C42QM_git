/*
==============================================================================

FILE:         HALclkULTAUDIOSECPCMDATAOE.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the
   ULTAUDIOSECPCMDATAOE clocks.

   List of clock domains:
    - HAL_clk_mGCCGCCULTAUDIOSECPCMDATAOEClkDomain


   List of power domains:



==============================================================================

$Header: //components/rel/core.mpss/3.10/systemdrivers/hal/clk/hw/mdm9205/src/gcc/HALclkULTAUDIOSECPCMDATAOE.c#2 $

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
 *  HAL_clk_mGCCULTAUDIOSECPCMDATAOEClkDomainClks
 *
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mGCCULTAUDIOSECPCMDATAOEClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_ultaudio_sec_pcm_data_oe_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_ULTAUDIO_SEC_PCM_DATA_OE_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_ULTAUDIO_SEC_PCM_DATA_OE_CLK
  },
};


/*
 * HAL_clk_mGCCGCCULTAUDIOSECPCMDATAOEClkDomain
 *
 * GCCULTAUDIOSECPCMDATAOE clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCULTAUDIOSECPCMDATAOEClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_ULTAUDIO_SEC_PCM_DATA_OE_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mGCCULTAUDIOSECPCMDATAOEClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mGCCULTAUDIOSECPCMDATAOEClkDomainClks)/sizeof(HAL_clk_mGCCULTAUDIOSECPCMDATAOEClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl_2,
  /* .pmNextClockDomain    = */ NULL
};
