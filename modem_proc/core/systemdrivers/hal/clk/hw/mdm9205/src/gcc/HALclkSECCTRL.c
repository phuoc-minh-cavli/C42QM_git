/*
==============================================================================

FILE:         HALclkSECCTRL.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the
   SECCTRL clocks.

   List of clock domains:
    - HAL_clk_mGCCACCClkDomain
    - HAL_clk_mGCCSECCTRLACCClkDomain
    - HAL_clk_mGCCSECCTRLClkDomain


   List of power domains:



==============================================================================

$Header: //components/rel/core.mpss/3.10/systemdrivers/hal/clk/hw/mdm9205/src/gcc/HALclkSECCTRL.c#4 $

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
 *  HAL_clk_mSECCTRLClkDomainClks
 *
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mSECCTRLClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_sec_ctrl_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SEC_CTRL_CBCR), HWIO_OFFS(GCC_SEC_CTRL_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SEC_CTRL_CLK
  },
#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_sec_ctrl_sense_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SEC_CTRL_SENSE_CBCR), HWIO_OFFS(GCC_SEC_CTRL_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SEC_CTRL_SENSE_CLK
  },
#endif

};


/*
 * HAL_clk_mGCCSECCTRLClkDomain
 *
 * SECCTRL clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCSECCTRLClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_SEC_CTRL_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mSECCTRLClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mSECCTRLClkDomainClks)/sizeof(HAL_clk_mSECCTRLClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};
