/*
==============================================================================

FILE:         HALclkSTANDALONEBCR.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   STANDALONEBCR clocks.

   List of clock domains:
     - HAL_clk_mGCCSTANDALONEBCRClkDomain


   List of power domains:



==============================================================================

$Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Library/HALclkLib/HALclkSTANDALONEBCR.c#1 $

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


/* ============================================================================
**    Data
** ==========================================================================*/


/*                           
 *  HAL_clk_mSTANDALONEBCRClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mSTANDALONEBCRClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_bias_pll_bcr",
    /* .mRegisters       = */ { 0, HWIO_OFFS(GCC_BIAS_PLL_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ 0
  },
  {
    /* .szClockName      = */ "gcc_mmss_bcr",
    /* .mRegisters       = */ { 0, HWIO_OFFS(GCC_MMSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ 0
  },
};


/*
 * HAL_clk_mGCCSTANDALONEBCRClkDomain
 *
 * STANDALONEBCR clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCSTANDALONEBCRClkDomain =
{
  /* .nCGRAddr             = */ 0, /* this domain does not have a cmd rcgr */
  /* .pmClocks             = */ HAL_clk_mSTANDALONEBCRClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mSTANDALONEBCRClkDomainClks)/sizeof(HAL_clk_mSTANDALONEBCRClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};

