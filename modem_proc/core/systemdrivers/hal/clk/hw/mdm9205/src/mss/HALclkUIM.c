/*
==============================================================================

FILE:         HALclkUIM.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the
   UIM clocks.

   List of clock domains:
    - HAL_clk_mMSSCCXOMNDUIM0ClkDomain


   List of power domains:



==============================================================================

$Header: //components/rel/core.mpss/3.10/systemdrivers/hal/clk/hw/mdm9205/src/mss/HALclkUIM.c#2 $

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
extern HAL_clk_ClockDomainControlType  HAL_clk_mMSSCCClockDomainControl_1;


/* ============================================================================
**    Data
** ==========================================================================*/


/*
 *  HAL_clk_mXOMNDUIM0ClkDomainClks
 *
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mXOMNDUIM0ClkDomainClks[] =
{
  {
    /* .szClockName      = */ "clk_xo_uim0",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_XO_UIM0_CBCR), HWIO_OFFS(MSS_UIM0_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSSCC_TEST_CLK_XO_UIM0
  },
};


/*
 * HAL_clk_mMSSCCXOMNDUIM0ClkDomain
 *
 * XOMNDUIM0 clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mMSSCCXOMNDUIM0ClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(MSS_UIM0_MND_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mXOMNDUIM0ClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mXOMNDUIM0ClkDomainClks)/sizeof(HAL_clk_mXOMNDUIM0ClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mMSSCCClockDomainControl_1,
  /* .pmNextClockDomain    = */ NULL
};

/*
 *  HAL_clk_mMSSCCUIMClkDomainClks
 *
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mMSSCCUIMClkDomainClks[] =
{
  {
    /* .szClockName      = */ "clk_uim0",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_UIM0_CBCR), HWIO_OFFS(MSS_UIM0_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSSCC_TEST_CLK_UIM0
  },
};


/*
 * HAL_clk_mMSSCCUIMClkDomain
 *
 * CXO UIM clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mMSSCCUIMClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(MSS_UIM_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mMSSCCUIMClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mMSSCCUIMClkDomainClks)/sizeof(HAL_clk_mMSSCCUIMClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mMSSCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};
