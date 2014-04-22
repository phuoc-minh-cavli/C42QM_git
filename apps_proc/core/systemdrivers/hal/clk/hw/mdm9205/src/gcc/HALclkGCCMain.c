/*
==============================================================================

FILE:         HALclkGCCMain.c

DESCRIPTION:
   The main auto-generated file for GCC.


==============================================================================

$Header: //components/rel/core.tx/6.0/systemdrivers/hal/clk/hw/mdm9205/src/gcc/HALclkGCCMain.c#1 $

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



/*
 * Clock domains
 */
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP1I2CAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP1SPIAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP2I2CAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP2SPIAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP3I2CAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP3SPIAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP4I2CAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP4SPIAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1UART1APPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1UART2APPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1UART3APPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1UART4APPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCCRYPTOClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCECCClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCSLEEPClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCXOClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGP1ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGP2ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGP3ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCPCNOCBFDCDClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCPDM2ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCPRNGClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSPMIAHBClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSPMISERClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCUSBHSSYSTEMClkDomain;

/*
 * Power domains
 */


/* ============================================================================
**    Data
** ==========================================================================*/


/*
 * aGCCSourceMap
 *
 * GCC HW source mapping
 *
 */
static HAL_clk_SourceMapType aGCCSourceMap[] =
{
  { HAL_CLK_SOURCE_XO,                 0 },
  { HAL_CLK_SOURCE_GPLL0,              1 },
  { HAL_CLK_SOURCE_GROUND,             2 },
  { HAL_CLK_SOURCE_GPLL2,              3 },
  { HAL_CLK_SOURCE_GROUND,             4 },
  { HAL_CLK_SOURCE_GROUND,             5 },
  { HAL_CLK_SOURCE_SLEEPCLK,           6 },
  { HAL_CLK_SOURCE_PLLTEST,            7 },
  { HAL_CLK_SOURCE_NULL,               HAL_CLK_SOURCE_INDEX_INVALID }
};


/*
 * HAL_clk_mGCCClockDomainControl
 *
 * Functions for controlling GCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mGCCClockDomainControl =
{
   /* .ConfigMux          = */ HAL_clk_GenericConfigMux,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .IsOn               = */ HAL_clk_GenericDomainIsOn,
   /* .pmSourceMap        = */ aGCCSourceMap
};


/*
 * HAL_clk_mGCCClockDomainControlRO
 *
 * Read-only functions for GCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mGCCClockDomainControlRO =
{
   /* .ConfigMux          = */ NULL,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .IsOn               = */ HAL_clk_GenericDomainIsOn,
   /* .pmSourceMap        = */ aGCCSourceMap
};


/*
 * HAL_clk_aGCCClockDomainDesc
 *
 * List of GCC clock domains
*/
static HAL_clk_ClockDomainDescType * HAL_clk_aGCCClockDomainDesc [] =
{
  &HAL_clk_mGCCBLSP1QUP1I2CAPPSClkDomain,
  &HAL_clk_mGCCBLSP1QUP1SPIAPPSClkDomain,
  &HAL_clk_mGCCBLSP1QUP2I2CAPPSClkDomain,
  &HAL_clk_mGCCBLSP1QUP2SPIAPPSClkDomain,
  &HAL_clk_mGCCBLSP1QUP3I2CAPPSClkDomain,
  &HAL_clk_mGCCBLSP1QUP3SPIAPPSClkDomain,
  &HAL_clk_mGCCBLSP1QUP4I2CAPPSClkDomain,
  &HAL_clk_mGCCBLSP1QUP4SPIAPPSClkDomain,
  &HAL_clk_mGCCBLSP1UART1APPSClkDomain,
  &HAL_clk_mGCCBLSP1UART2APPSClkDomain,
  &HAL_clk_mGCCBLSP1UART3APPSClkDomain,
  &HAL_clk_mGCCBLSP1UART4APPSClkDomain,
  &HAL_clk_mGCCCRYPTOClkDomain,
  &HAL_clk_mGCCGCCECCClkDomain,
  &HAL_clk_mGCCGCCSLEEPClkDomain,
  &HAL_clk_mGCCGCCXOClkDomain,
  &HAL_clk_mGCCGP1ClkDomain,
  &HAL_clk_mGCCGP2ClkDomain,
  &HAL_clk_mGCCGP3ClkDomain,
  &HAL_clk_mGCCPCNOCBFDCDClkDomain,
  &HAL_clk_mGCCPDM2ClkDomain,
  &HAL_clk_mGCCPRNGClkDomain,
  &HAL_clk_mGCCSPMIAHBClkDomain,
  &HAL_clk_mGCCSPMISERClkDomain,
  &HAL_clk_mGCCUSBHSSYSTEMClkDomain,
  NULL
};


/*
 * HAL_clk_aGCCPowerDomainDesc
 *
 * List of GCC power domains
 */
static HAL_clk_PowerDomainDescType * HAL_clk_aGCCPowerDomainDesc [] =
{
  NULL
};



/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/


/* ===========================================================================
**  HAL_clk_PlatformInitGCCMain
**
** ======================================================================== */

void HAL_clk_PlatformInitGCCMain (void)
{

  /*
   * Install all clock domains
   */
  HAL_clk_InstallClockDomains(HAL_clk_aGCCClockDomainDesc, CLK_CTL_BASE);

  /*
   * Install all power domains
   */
  HAL_clk_InstallPowerDomains(HAL_clk_aGCCPowerDomainDesc, CLK_CTL_BASE);

} /* END HAL_clk_PlatformInitGCCMain */
