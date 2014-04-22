/*
==============================================================================

FILE:         HALclkCLOCKDRIVERGENNODOMAIN.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the
   CLOCKDRIVERGENNODOMAIN clocks.

   List of clock domains:
    - HAL_clk_mMSSCCCLOCKDRIVERGENNODOMAINClkDomain


   List of power domains:



==============================================================================

$Header: //components/rel/core.mpss/3.10/systemdrivers/hal/clk/hw/mdm9205/src/mss/HALclkCLOCKDRIVERGENNODOMAIN.c#1 $

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

extern HAL_clk_ClockDomainControlType  HAL_clk_mMSSCCClockDomainControlRO;


/* ============================================================================
**    Data
** ==========================================================================*/


/*
 *  HAL_clk_mCLOCKDRIVERGENNODOMAINClkDomainClks
 *
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mCLOCKDRIVERGENNODOMAINClkDomainClks[] =
{
  {
    /* .szClockName      = */ "clk_bus_modem",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_BUS_MODEM_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSSCC_TEST_CLK_BUS_MODEM
  },
  {
    /* .szClockName      = */ "clk_modem_cfg_ahb",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_MODEM_CFG_AHB_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSSCC_TEST_CLK_MODEM_CFG_AHB
  },
  {
    /* .szClockName      = */ "clk_modem_snoc_axi",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_MODEM_SNOC_AXI_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSSCC_TEST_CLK_MODEM_SNOC_AXI
  },
  {
    /* .szClockName      = */ "clk_nav_dbe",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_NAV_DBE_CBCR), HWIO_OFFS(MSS_NAV_DBE_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSSCC_TEST_CLK_NAV_DBE
  },
  {
    /* .szClockName      = */ "clk_nav_snoc_axi",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_NAV_SNOC_AXI_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSSCC_TEST_CLK_NAV_SNOC_AXI
  },
  {
    /* .szClockName      = */ "clk_shared_mem",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_SHARED_MEM_CBCR), HWIO_OFFS(MSS_SHARED_MEM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSSCC_TEST_CLK_SHARED_MEM
  },
  {
    /* .szClockName      = */ "clk_silver",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_SILVER_CBCR), HWIO_OFFS(MSS_SILVER_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSSCC_TEST_CLK_SILVER
  },
  {
    /* .szClockName      = */ "clk_xo_modem",
    /* .mRegisters       = */ { HWIO_OFFS(MSS_XO_MODEM_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_MSSCC_TEST_CLK_XO_MODEM
  },
};


/*
 * HAL_clk_mMSSCCCLOCKDRIVERGENNODOMAINClkDomain
 *
 * CLOCKDRIVERGENNODOMAIN clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mMSSCCCLOCKDRIVERGENNODOMAINClkDomain =
{
  /* .nCGRAddr             = */ 0,
  /* .pmClocks             = */ HAL_clk_mCLOCKDRIVERGENNODOMAINClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mCLOCKDRIVERGENNODOMAINClkDomainClks)/sizeof(HAL_clk_mCLOCKDRIVERGENNODOMAINClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mMSSCCClockDomainControlRO,
  /* .pmNextClockDomain    = */ NULL
};
