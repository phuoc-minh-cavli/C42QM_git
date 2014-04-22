/*
==============================================================================

FILE:         HALclkADC.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the
   ADC clocks.

   List of clock domains:
    - HAL_clk_mMSSCCCLKADCDIVClkDomain


   List of power domains:



==============================================================================

$Header: //components/rel/core.mpss/3.10/systemdrivers/hal/clk/hw/mdm9205/src/mss/HALclkADC.c#1 $

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

extern HAL_clk_ClockDomainControlType  HAL_clk_mMSSCCClockDomainControl_1;


/* ============================================================================
**    Data
** ==========================================================================*/


/*
 *  HAL_clk_mCLKADCDIVClkDomainClks
 *
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mCLKADCDIVClkDomainClks[] =
{
  {
    /* .szClockName      = */ "clk_adc",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_ADC_CBCR), HWIO_OFFS(MSS_ADC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSSCC_TEST_CLK_ADC
  },
};


/*
 * HAL_clk_mMSSCCCLKADCDIVClkDomain
 *
 * CLKADCDIV clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mMSSCCCLKADCDIVClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(MSS_ADC_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mCLKADCDIVClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mCLKADCDIVClkDomainClks)/sizeof(HAL_clk_mCLKADCDIVClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mMSSCCClockDomainControl_1,
  /* .pmNextClockDomain    = */ NULL
};
