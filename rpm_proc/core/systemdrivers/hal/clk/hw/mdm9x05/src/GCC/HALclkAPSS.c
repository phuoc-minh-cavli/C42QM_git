/*
==============================================================================

FILE:         HALclkAPSS.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   APSS clocks.

   List of clock domains:
     - HAL_clk_mGCCAPSSAHBClkDomain
     - HAL_clk_mGCCBIMCAPSSClkDomain


   List of power domains:



==============================================================================

$Header: //components/rel/rpm.bf/2.1.3/core/systemdrivers/hal/clk/hw/mdm9x05/src/GCC/HALclkAPSS.c#2 $

==============================================================================
            Copyright (c) 2018 QUALCOMM Technologies Incorporated.
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
extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControlRO;
extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControl_3;
extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControlRO_3;


/* ============================================================================
**    Data
** ==========================================================================*/


/*                           
 *  HAL_clk_mAPSSAHBClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mAPSSAHBClkDomainClks[] =
{

  {
    /* .szClockName      = */ "gcc_apss_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_APSS_AHB_CBCR), 0, HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, APSS_AHB_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_APSS_AHB_CLK
  },
  {
    /* .szClockName      = */ "gcc_pcnoc_apss_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_APSS_AHB_CBCR), HWIO_OFFS(GCC_PCNOC_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, PCNOC_APSS_AHB_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_APSS_AHB_CLK
  },
#if 0 /* not present on rpm */

  {
    /* .szClockName      = */ "gcc_pcnoc_bus_timeout4_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT4_AHB_CBCR), HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT4_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT4_AHB_CLK
  },
#endif

#if 0 /* not present on rpm */

  {
    /* .szClockName      = */ "gcc_snoc_bus_timeout0_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SNOC_BUS_TIMEOUT0_AHB_CBCR), HWIO_OFFS(GCC_SNOC_BUS_TIMEOUT0_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ 0
  },
#endif

};


/*
 * HAL_clk_mGCCAPSSAHBClkDomain
 *
 * APSSAHB clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCAPSSAHBClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_APSS_AHB_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mAPSSAHBClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mAPSSAHBClkDomainClks)/sizeof(HAL_clk_mAPSSAHBClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mBIMCAPSSClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mBIMCAPSSClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_apss_axi_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_APSS_AXI_CBCR), 0, HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, APSS_AXI_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_APSS_AXI_CLK
  },
  {
    /* .szClockName      = */ "gcc_bimc_apss_axi_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BIMC_APSS_AXI_CBCR), HWIO_OFFS(GCC_BIMC_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, BIMC_APSS_AXI_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIMC_APSS_AXI_CLK
  },
};


/*
 * HAL_clk_mGCCBIMCAPSSClkDomain
 *
 * BIMCAPSS clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCBIMCAPSSClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_APSS_AXI_CMD_RCGR), /* this domain does not have a cmd rcgr */
  /* .pmClocks             = */ HAL_clk_mBIMCAPSSClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mBIMCAPSSClkDomainClks)/sizeof(HAL_clk_mBIMCAPSSClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl_3,
  /* .pmNextClockDomain    = */ NULL
};

