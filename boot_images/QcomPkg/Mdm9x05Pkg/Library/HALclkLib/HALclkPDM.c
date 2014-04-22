/*
==============================================================================

FILE:         HALclkPDM.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   PDM clocks.

   List of clock domains:
     - HAL_clk_mGCCPDM2ClkDomain


   List of power domains:



==============================================================================

$Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Library/HALclkLib/HALclkPDM.c#1 $

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
 *  HAL_clk_mPDM2ClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mPDM2ClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_pdm2_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PDM2_CBCR), HWIO_OFFS(GCC_PDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PDM2_CLK
  },
};


/*
 * HAL_clk_mGCCPDM2ClkDomain
 *
 * PDM2 clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCPDM2ClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_PDM2_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mPDM2ClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mPDM2ClkDomainClks)/sizeof(HAL_clk_mPDM2ClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};

