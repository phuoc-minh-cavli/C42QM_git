/*
==============================================================================

FILE:         HALclkBLSP2QUP0.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   BLSP2QUP0 clocks.

   List of clock domains:
     - HAL_clk_mGCCBLSP2QUP0I2CAPPSClkDomain
     - HAL_clk_mGCCBLSP2QUP0SPIAPPSClkDomain


   List of power domains:



==============================================================================

$Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Library/HALclkLib/HALclkBLSP2QUP0.c#1 $

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
 *  HAL_clk_mBLSP2QUP0I2CAPPSClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mBLSP2QUP0I2CAPPSClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_blsp2_qup0_i2c_apps_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BLSP2_QUP0_I2C_APPS_CBCR), HWIO_OFFS(GCC_BLSP2_QUP0_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BLSP2_QUP0_I2C_APPS_CLK
  },
};


/*
 * HAL_clk_mGCCBLSP2QUP0I2CAPPSClkDomain
 *
 * BLSP2QUP0I2CAPPS clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP2QUP0I2CAPPSClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_BLSP2_QUP0_I2C_APPS_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mBLSP2QUP0I2CAPPSClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mBLSP2QUP0I2CAPPSClkDomainClks)/sizeof(HAL_clk_mBLSP2QUP0I2CAPPSClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mBLSP2QUP0SPIAPPSClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mBLSP2QUP0SPIAPPSClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_blsp2_qup0_spi_apps_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BLSP2_QUP0_SPI_APPS_CBCR), HWIO_OFFS(GCC_BLSP2_QUP0_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BLSP2_QUP0_SPI_APPS_CLK
  },
};


/*
 * HAL_clk_mGCCBLSP2QUP0SPIAPPSClkDomain
 *
 * BLSP2QUP0SPIAPPS clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP2QUP0SPIAPPSClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_BLSP2_QUP0_SPI_APPS_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mBLSP2QUP0SPIAPPSClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mBLSP2QUP0SPIAPPSClkDomainClks)/sizeof(HAL_clk_mBLSP2QUP0SPIAPPSClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};

