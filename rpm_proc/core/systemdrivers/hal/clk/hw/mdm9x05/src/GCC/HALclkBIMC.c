/*
==============================================================================

FILE:         HALclkBIMC.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   BIMC clocks.

   List of clock domains:
     - HAL_clk_mGCCBIMCDDRClkDomain
     - HAL_clk_mGCCBIMCDDRXOClkDomain


   List of power domains:
     - HAL_clk_mGCCBIMCPowerDomain



==============================================================================

$Header: //components/rel/rpm.bf/2.1.3/core/systemdrivers/hal/clk/hw/mdm9x05/src/GCC/HALclkBIMC.c#4 $

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

extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControl_1;
extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControlRO_1;
extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControl_3;
extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControlRO_3;


/* ============================================================================
**    Data
** ==========================================================================*/

/*                           
 *  HAL_clk_mBIMCDDRClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mBIMCDDRClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_bimc_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BIMC_CBCR), HWIO_OFFS(GCC_BIMC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIMC_CLK
  },
  {
    /* .szClockName      = */ "gcc_mss_q6_bimc_axi_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_MSS_Q6_BIMC_AXI_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_MSS_Q6_BIMC_AXI_CLK
  },
  {
    /* .szClockName      = */ "gcc_spdm_bimc_cy_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SPDM_BIMC_CY_CBCR), HWIO_OFFS(GCC_SPDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SPDM_BIMC_CY_CLK
  },
};


/*
 * HAL_clk_mGCCBIMCDDRClkDomain
 *
 * BIMCDDR clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCBIMCDDRClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_BIMC_DDR_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mBIMCDDRClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mBIMCDDRClkDomainClks)/sizeof(HAL_clk_mBIMCDDRClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl_3,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mBIMCDDRXOClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mBIMCDDRXOClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_bimc_xo_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BIMC_XO_CBCR), HWIO_OFFS(GCC_BIMC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIMC_XO_CLK
  },
  {
    /* .szClockName      = */ "gcc_ddr_dim_cfg_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_DDR_DIM_CFG_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_DDR_DIM_CFG_CLK
  },
};


/*
 * HAL_clk_mGCCBIMCDDRXOClkDomain
 *
 * BIMCDDRXO clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCBIMCDDRXOClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_BIMC_DDR_XO_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mBIMCDDRXOClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mBIMCDDRXOClkDomainClks)/sizeof(HAL_clk_mBIMCDDRXOClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl_1,
  /* .pmNextClockDomain    = */ NULL
};


/*
 * HAL_clk_mGCCBIMCPowerDomain
 *
 * BIMC power domain.
 */
HAL_clk_PowerDomainDescType HAL_clk_mGCCBIMCPowerDomain =
{
  /* .szPowerDomainName       = */ "VDD_BIMC",
  /* .nGDSCRAddr              = */ HWIO_OFFS(GCC_BIMC_GDSCR),
  /* .pmControl               = */ &HAL_clk_mGenericPowerDomainControl,
  /* .pmNextPowerDomain       = */ NULL
};

/* ===========================================================================
**  HAL_clk_BIMCGetMiscInfo
**
** ======================================================================== */

uint32 HAL_clk_BIMCGetMiscInfo( void )
{
  return HWIO_IN(GCC_BIMC_MISC);
}


/* ===========================================================================
**  HAL_clk_GetDEHRStatus
**
** ======================================================================== */

uint32 HAL_clk_GetDEHRStatus( void )
{
  return HWIO_IN(DEHR_BIMC_DEHR_DMA_STATUS);
}

/* ===========================================================================
**  HAL_clk_IsDEHREnable
**
** ======================================================================== */

boolean HAL_clk_IsDEHREnable( void )
{
  return ( HWIO_INF(DEHR_BIMC_DEHR_SEC_CFG, DEHR_ENA) != 0 );
}

void HAL_clk_SPMAPPSEnableBIMCPorts( boolean bEnable )
{

  /*  gcc_apss_axi_clk is vote-able clock and default on from rpm clk branch */
  HWIO_OUTF(GCC_RPM_CLOCK_BRANCH_ENA_VOTE, APSS_AXI_CLK_ENA, bEnable);

}

void HAL_clk_EnableQ6BIMCPorts( boolean bEnable )
{
  HWIO_OUTF( GCC_MSS_Q6_BIMC_AXI_CBCR, CLK_ENABLE, bEnable);
}

