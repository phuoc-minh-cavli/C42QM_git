/*
==============================================================================

FILE:         HALclkCOXM.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the
   COXM clocks.

   List of clock domains:
    - HAL_clk_mMSSCCCOXMBITClkDomain


   List of power domains:



==============================================================================

$Header: //components/rel/core.mpss/3.10/systemdrivers/hal/clk/hw/mdm9205/src/mss/HALclkCOXM.c#2 $

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

extern HAL_clk_ClockDomainControlType  HAL_clk_mMSSCCClockDomainControl;
extern HAL_clk_ClockDomainControlType  HAL_clk_mMSSCCClockDomainControlRO;


/* ============================================================================
**    Data
** ==========================================================================*/


/*
 *  HAL_clk_mCOXMBITClkDomainClks
 *
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mCOXMBITClkDomainClks[] =
{
  {
    /* .szClockName      = */ "clk_coxm_bit",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_BIT_COXM_CBCR), HWIO_OFFS(MSS_COXM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSSCC_TEST_CLK_COXM_BIT
  },
};


/*
 * HAL_clk_mMSSCCCOXMBITClkDomain
 *
 * COXMBIT clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mMSSCCCOXMBITClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(MSS_BIT_COXM_MND_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mCOXMBITClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mCOXMBITClkDomainClks)/sizeof(HAL_clk_mCOXMBITClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mMSSCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};
