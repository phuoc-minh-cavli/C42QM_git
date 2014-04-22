/*
==============================================================================

FILE:         HALclkPCIE.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   PCIE clocks.

   List of clock domains:
     - HAL_clk_mGCCPCIE0AUXClkDomain
     - HAL_clk_mGCCPCIE0PIPEClkDomain


   List of power domains:



==============================================================================

$Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Library/HALclkLib/HALclkPCIE.c#1 $

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

extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControl_2;
extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControlRO_2;
extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControl_4;
extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControlRO_4;


/* ============================================================================
**    Data
** ==========================================================================*/


/*                           
 *  HAL_clk_mPCIE0AUXClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mPCIE0AUXClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_pcie_0_aux_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCIE_0_AUX_CBCR), HWIO_OFFS(GCC_PCIE_0_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, PCIE_0_AUX_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCIE_0_AUX_CLK
  },
};


/*
 * HAL_clk_mGCCPCIE0AUXClkDomain
 *
 * PCIE0AUX clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCPCIE0AUXClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_PCIE_0_AUX_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mPCIE0AUXClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mPCIE0AUXClkDomainClks)/sizeof(HAL_clk_mPCIE0AUXClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl_4,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mPCIE0PIPEClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mPCIE0PIPEClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_pcie_0_pipe_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCIE_0_PIPE_CBCR), HWIO_OFFS(GCC_PCIE_0_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, PCIE_0_PIPE_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCIE_0_PIPE_CLK
  },
};


/*
 * HAL_clk_mGCCPCIE0PIPEClkDomain
 *
 * PCIE0PIPE clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCPCIE0PIPEClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_PCIE_0_PIPE_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mPCIE0PIPEClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mPCIE0PIPEClkDomainClks)/sizeof(HAL_clk_mPCIE0PIPEClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl_2,
  /* .pmNextClockDomain    = */ NULL
};

