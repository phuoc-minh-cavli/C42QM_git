/*
==============================================================================

FILE:         HALclkMODEM.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the
   MODEM clocks.

   List of clock domains:
    - HAL_clk_mMSSCCCLKMODEMAXIClkDomain
    - HAL_clk_mMSSCCCLKTDECClkDomain
    - HAL_clk_mMSSCCCLKTWOXOClkDomain


   List of power domains:



==============================================================================

$Header: //components/rel/core.mpss/3.10/systemdrivers/hal/clk/hw/mdm9205/src/mss/HALclkMODEM.c#2 $

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
 *  HAL_clk_mCLKMODEMAXIClkDomainClks
 *
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mCLKMODEMAXIClkDomainClks[] =
{
  {
    /* .szClockName      = */ "clk_modem_axi_cbc",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_MODEM_AXI_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSSCC_TEST_CLK_MODEM_AXI_CBC
  },
};


/*
 * HAL_clk_mMSSCCCLKMODEMAXIClkDomain
 *
 * CLKMODEMAXI clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mMSSCCCLKMODEMAXIClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(MSS_MODEM_AXI_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mCLKMODEMAXIClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mCLKMODEMAXIClkDomainClks)/sizeof(HAL_clk_mCLKMODEMAXIClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mMSSCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


/*
 *  HAL_clk_mCLKTWOXOClkDomainClks
 *
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mCLKTWOXOClkDomainClks[] =
{
  {
    /* .szClockName      = */ "clk_half_xo_cbc",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_HALF_XO_RFFE_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSSCC_TEST_CLK_HALF_XO_CBC
  },
  {
    /* .szClockName      = */ "clk_one_xo_cbc",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_ONE_XO_RFFE_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSSCC_TEST_CLK_ONE_XO_CBC
  },
  {
    /* .szClockName      = */ "clk_two_xo_cbc",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_TWO_XO_RFFE_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSSCC_TEST_CLK_TWO_XO_CBC
  },
};


/*
 * HAL_clk_mMSSCCCLKTWOXOClkDomain
 *
 * CLKTWOXO clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mMSSCCCLKTWOXOClkDomain =
{
  /* .nCGRAddr             = */ 0,
  /* .pmClocks             = */ HAL_clk_mCLKTWOXOClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mCLKTWOXOClkDomainClks)/sizeof(HAL_clk_mCLKTWOXOClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mMSSCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};
