/*
==============================================================================

FILE:         HALclkAPSS.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   APSS clocks.

   List of clock domains:
     - HAL_clk_mGCCAPSSAHBClkDomain
     - HAL_clk_mGCCAPSSAXIClkDomain


   List of power domains:



==============================================================================

$Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Library/HALclkLib/HALclkAPSS.c#1 $

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
#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_snoc_bus_timeout0_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SNOC_BUS_TIMEOUT0_AHB_CBCR), HWIO_OFFS(GCC_SNOC_BUS_TIMEOUT0_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SNOC_BUS_TIMEOUT0_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_sys_noc_apss_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SYS_NOC_APSS_AHB_CBCR), HWIO_OFFS(GCC_SYSTEM_NOC_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, SYS_NOC_APSS_AHB_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SYS_NOC_APSS_AHB_CLK
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
 *  HAL_clk_mAPSSAXIClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mAPSSAXIClkDomainClks[] =
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
 * HAL_clk_mGCCAPSSAXIClkDomain
 *
 * APSSAXI clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCAPSSAXIClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_APSS_AXI_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mAPSSAXIClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mAPSSAXIClkDomainClks)/sizeof(HAL_clk_mAPSSAXIClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControlRO_3,
  /* .pmNextClockDomain    = */ NULL
};

