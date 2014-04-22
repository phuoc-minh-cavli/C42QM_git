/*
==============================================================================

FILE:         HALclkGCCMain.c

DESCRIPTION:
   The main auto-generated file for GCC.


==============================================================================

$Header: //components/rel/core.mpss/3.10/systemdrivers/hal/clk/hw/mdm9205/src/gcc/HALclkGCCMain.c#3 $

==============================================================================
            Copyright (c) 2019 Qualcomm Technologies Incorporated.
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
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBIMCDDRClkDomain;
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
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSPUARTSIMClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCSLEEPClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCULTAUDIOEXTMCLK2ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCULTAUDIOLPAIFAUXI2SClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCULTAUDIOLPAIFPRII2SClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCULTAUDIOLPAIFSECI2SClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCULTAUDIOPCMDATAOEClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCULTAUDIOSECPCMDATAOEClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCULTAUDIOTERPCMDATAOEClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCULTAUDIOXOClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCXOClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCPCNOCBFDCDClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSECCTRLClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSPMIAHBClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSPMISERClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCULTAUDIOAHBFABRICClkDomain;


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
  { HAL_CLK_SOURCE_XO,          0 },
  { HAL_CLK_SOURCE_GPLL0,       1 },
  { HAL_CLK_SOURCE_GROUND,      2 },
  { HAL_CLK_SOURCE_GPLL2,       3 },
  { HAL_CLK_SOURCE_GROUND,      4 },
  { HAL_CLK_SOURCE_GROUND,      5 },
  { HAL_CLK_SOURCE_SLEEPCLK,    6 },
  { HAL_CLK_SOURCE_PLLTEST,     7 },
  { HAL_CLK_SOURCE_NULL,        HAL_CLK_SOURCE_INDEX_INVALID }
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
   /* .pmSourceMap        = */ aGCCSourceMap
};


/*
 * aGCCSourceMap_1
 *
 * GCC HW source mapping
 *
 */
static HAL_clk_SourceMapType aGCCSourceMap_1[] =
{
  { HAL_CLK_SOURCE_XO,                 0 },
  { HAL_CLK_SOURCE_GPLL1,              1 },
  { HAL_CLK_SOURCE_GROUND,             2 },
  { HAL_CLK_SOURCE_GPLL0,              3 },
  { HAL_CLK_SOURCE_EXTERNAL,           4 },
  { HAL_CLK_SOURCE_GROUND,             5 },
  { HAL_CLK_SOURCE_SLEEPCLK,           6 },
  { HAL_CLK_SOURCE_PLLTEST,            7 },
  { HAL_CLK_SOURCE_NULL,               HAL_CLK_SOURCE_INDEX_INVALID }
};


/*
 * HAL_clk_mGCCClockDomainControl_1
 *
 * Functions for controlling GCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mGCCClockDomainControl_1 =
{
   /* .ConfigMux          = */ HAL_clk_GenericConfigMux,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aGCCSourceMap_1
};


/*
 * HAL_clk_mGCCClockDomainControlRO_1
 *
 * Read-only functions for GCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mGCCClockDomainControlRO_1 =
{
   /* .ConfigMux          = */ NULL,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aGCCSourceMap_1
};


/*
 * aGCCSourceMap_2
 *
 * GCC HW source mapping
 *
 */
static HAL_clk_SourceMapType aGCCSourceMap_2[] =
{
  { HAL_CLK_SOURCE_XO,                 0 },
  { HAL_CLK_SOURCE_GPLL1,              1 },
  { HAL_CLK_SOURCE_GROUND,             2 },
  { HAL_CLK_SOURCE_EXTERNAL,           3 },
  { HAL_CLK_SOURCE_GPLL0,              4 },
  { HAL_CLK_SOURCE_GROUND,             5 },
  { HAL_CLK_SOURCE_SLEEPCLK,           6 },
  { HAL_CLK_SOURCE_PLLTEST,            7 },
  { HAL_CLK_SOURCE_NULL,               HAL_CLK_SOURCE_INDEX_INVALID }
};


/*
 * HAL_clk_mGCCClockDomainControl_2
 *
 * Functions for controlling GCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mGCCClockDomainControl_2 =
{
   /* .ConfigMux          = */ HAL_clk_GenericConfigMux,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aGCCSourceMap_2,
   /* .IsOn               = */ HAL_clk_GenericDomainIsOn
};


/*
 * HAL_clk_mGCCClockDomainControlRO_2
 *
 * Read-only functions for GCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mGCCClockDomainControlRO_2 =
{
   /* .ConfigMux          = */ NULL,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aGCCSourceMap_2
};


/*
 * aGCCSourceMap_3
 *
 * GCC HW source mapping
 *
 */
static HAL_clk_SourceMapType aGCCSourceMap_3[] =
{
  { HAL_CLK_SOURCE_XO,                 0 },
  { HAL_CLK_SOURCE_GPLL0,              1 },
  { HAL_CLK_SOURCE_GPLL1,              2 },  // AUX
  { HAL_CLK_SOURCE_EXTERNAL,           3 },
  { HAL_CLK_SOURCE_GROUND,             4 },
  { HAL_CLK_SOURCE_GROUND,             5 },
  { HAL_CLK_SOURCE_SLEEPCLK,           6 },
  { HAL_CLK_SOURCE_PLLTEST,            7 },
  { HAL_CLK_SOURCE_NULL,               HAL_CLK_SOURCE_INDEX_INVALID }
};


/*
 * HAL_clk_mGCCClockDomainControl_3
 *
 * Functions for controlling GCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mGCCClockDomainControl_3 =
{
   /* .ConfigMux          = */ HAL_clk_GenericConfigMux,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aGCCSourceMap_3
};


/*
 * HAL_clk_mGCCClockDomainControlRO_3
 *
 * Read-only functions for GCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mGCCClockDomainControlRO_3 =
{
   /* .ConfigMux          = */ NULL,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aGCCSourceMap_3
};


/*
 * HAL_clk_aGCCClockDomainDesc
 *
 * List of GCC clock domains
*/
static HAL_clk_ClockDomainDescType * HAL_clk_aGCCClockDomainDesc [] =
{
  &HAL_clk_mGCCBIMCDDRClkDomain,
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
  &HAL_clk_mGCCBLSPUARTSIMClkDomain,
  &HAL_clk_mGCCGCCSLEEPClkDomain,
  &HAL_clk_mGCCGCCULTAUDIOEXTMCLK2ClkDomain,
  &HAL_clk_mGCCGCCULTAUDIOLPAIFAUXI2SClkDomain,
  &HAL_clk_mGCCGCCULTAUDIOLPAIFPRII2SClkDomain,
  &HAL_clk_mGCCGCCULTAUDIOLPAIFSECI2SClkDomain,
  &HAL_clk_mGCCGCCULTAUDIOPCMDATAOEClkDomain,
  &HAL_clk_mGCCGCCULTAUDIOSECPCMDATAOEClkDomain,
  &HAL_clk_mGCCGCCULTAUDIOTERPCMDATAOEClkDomain,
  &HAL_clk_mGCCGCCULTAUDIOXOClkDomain,
  &HAL_clk_mGCCGCCXOClkDomain,
  &HAL_clk_mGCCPCNOCBFDCDClkDomain,
  &HAL_clk_mGCCSECCTRLClkDomain,
  &HAL_clk_mGCCSPMIAHBClkDomain,
  &HAL_clk_mGCCSPMISERClkDomain,
  &HAL_clk_mGCCULTAUDIOAHBFABRICClkDomain,
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

} /* END HAL_clk_PlatformInitGCCMain */
