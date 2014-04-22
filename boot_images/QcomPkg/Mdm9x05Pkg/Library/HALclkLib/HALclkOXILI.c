/*
==============================================================================

FILE:         HALclkOXILI.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   OXILI clocks.

   List of clock domains:
     - HAL_clk_mGCCGFX3DClkDomain


   List of power domains:
     - HAL_clk_mGCCOXILIPowerDomain



==============================================================================

$Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Library/HALclkLib/HALclkOXILI.c#1 $

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

extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControl_9;
extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControlRO_9;


/* ============================================================================
**    Data
** ==========================================================================*/


/*                           
 *  HAL_clk_mGFX3DClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mGFX3DClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_oxili_gfx3d_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_OXILI_GFX3D_CBCR), HWIO_OFFS(GCC_OXILI_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_OXILI_GFX3D_CLK
  },
#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_spdm_gfx3d_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SPDM_GFX3D_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SPDM_GFX3D_CLK
  },
#endif

};


/*
 * HAL_clk_mGCCGFX3DClkDomain
 *
 * GFX3D clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCGFX3DClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_GFX3D_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mGFX3DClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mGFX3DClkDomainClks)/sizeof(HAL_clk_mGFX3DClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl_9,
  /* .pmNextClockDomain    = */ NULL
};


/*
 * HAL_clk_mGCCOXILIPowerDomain
 *
 * OXILI power domain.
 */
HAL_clk_PowerDomainDescType HAL_clk_mGCCOXILIPowerDomain =
{
  /* .szPowerDomainName       = */ "VDD_OXILI",
  /* .nGDSCRAddr              = */ HWIO_OFFS(GCC_OXILI_GDSCR),
  /* .pmControl               = */ &HAL_clk_mGenericPowerDomainControl,
  /* .pmNextPowerDomain       = */ NULL
};

