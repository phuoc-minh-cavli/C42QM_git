/*
==============================================================================

FILE:         HALclkEMAC.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   EMAC clocks.

   List of clock domains:
     - HAL_clk_mGCCEMACClkDomain
     - HAL_clk_mGCCEMACPTPClkDomain


   List of power domains:



==============================================================================

$Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Library/HALclkLib/HALclkEMAC.c#1 $

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

extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControl_7;
extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControlRO_7;


/* ============================================================================
**    Data
** ==========================================================================*/


/*                           
 *  HAL_clk_mEMACClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mEMACClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_eth_rgmii_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_ETH_RGMII_CBCR), HWIO_OFFS(GCC_EMAC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_ETH_RGMII_CLK
  },
};


/*
 * HAL_clk_mGCCEMACClkDomain
 *
 * EMAC clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCEMACClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_EMAC_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mEMACClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mEMACClkDomainClks)/sizeof(HAL_clk_mEMACClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl_7,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mEMACPTPClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mEMACPTPClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_eth_ptp_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_ETH_PTP_CBCR), HWIO_OFFS(GCC_EMAC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_ETH_PTP_CLK
  },
};


/*
 * HAL_clk_mGCCEMACPTPClkDomain
 *
 * EMACPTP clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCEMACPTPClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_EMAC_PTP_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mEMACPTPClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mEMACPTPClkDomainClks)/sizeof(HAL_clk_mEMACPTPClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl_7,
  /* .pmNextClockDomain    = */ NULL
};

