/*
==============================================================================

FILE:         HALclkGCCMain.c

DESCRIPTION:
   The main auto-generated file for GCC.


==============================================================================

$Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Library/HALclkLib/HALclkGCCMain.c#1 $

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

void HAL_clk_UART3ConfigMux
(
  HAL_clk_ClockDomainDescType      *pmClockDomainDesc,
  const HAL_clk_ClockMuxConfigType *pmConfig
);

void HAL_clk_UART3DetectMuxConfig
(
  HAL_clk_ClockDomainDescType *pmClockDomainDesc,
  HAL_clk_ClockMuxConfigType  *pmConfig
)
;


/*
 * Clock domains
 */
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCAPSSAHBClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCAPSSAXIClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBIMCDDRClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBIMCGPUClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP0I2CAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP0SPIAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP1I2CAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP1SPIAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP2I2CAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP2SPIAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP3I2CAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP3SPIAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP4I2CAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1QUP4SPIAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1UART0APPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1UART1APPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1UART2APPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP1UART3APPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP2QUP0I2CAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP2QUP0SPIAPPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBLSP2UART0APPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBYTE0ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCCRYPTOClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCEMACClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCEMACPTPClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCESC0ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCSLEEPClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCXOClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGFX3DClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGP1ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGP2ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGP3ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCHDMIAPPClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCHDMIPCLKClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCMDPClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCMDSSBIMCClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCPCIE0AUXClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCPCIE0PIPEClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCPCLK0ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCPCNOCBFDCDClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCPDM2ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSDCC1APPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSDCC1ICECOREClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSDCC2APPSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSPMIAHBClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSPMISERClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSTANDALONEBCRClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSYSTEMNOCBFDCDClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCUSB20MOCKUTMIClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCUSB30MASTERClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCUSB30MOCKUTMIClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCUSB3PHYAUXClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCUSBHSSYSTEMClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCUSB3PHYPIPEClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCVSYNCClkDomain;

/*
 * Power domains
 */
extern HAL_clk_PowerDomainDescType HAL_clk_mGCCMDSSPowerDomain;
extern HAL_clk_PowerDomainDescType HAL_clk_mGCCOXILIPowerDomain;


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
  //{ HAL_CLK_SOURCE_GPLL4_AUX,          2 },
  { HAL_CLK_SOURCE_GPLL6_AUX,          3 },
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
   /* .pmSourceMap        = */ aGCCSourceMap
};


/*
 * HAL_clk_mGCCClockDomainControl
 *
 * Functions for controlling GCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mGCCUART3ClockDomainControl =
{
   /* .ConfigMux          = */ HAL_clk_UART3ConfigMux,
   /* .DetectMuxConfig    = */ HAL_clk_UART3DetectMuxConfig,
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
  { HAL_CLK_SOURCE_GPLL0,              1 },
  { HAL_CLK_SOURCE_GPLL6_AUX,          2 },
  { HAL_CLK_SOURCE_GROUND,             3 },
  { HAL_CLK_SOURCE_GROUND,             4 },
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
  { HAL_CLK_SOURCE_EXTERNAL1,          1 },
  { HAL_CLK_SOURCE_GPLL0_AUX,          2 },
  { HAL_CLK_SOURCE_GROUND,             3 },
  { HAL_CLK_SOURCE_GROUND,             4 },
  { HAL_CLK_SOURCE_GROUND,             5 },
  { HAL_CLK_SOURCE_GROUND,             6 },
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
   /* .pmSourceMap        = */ aGCCSourceMap_2
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
  { HAL_CLK_SOURCE_GPLL6,              2 },
  //{ HAL_CLK_SOURCE_GPLL5_AUX,          3 },
  //{ HAL_CLK_SOURCE_BIMC_PLL_AUX,       4 },
  { HAL_CLK_SOURCE_GROUND,             5 },
  { HAL_CLK_SOURCE_GROUND,             6 },
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
 * aGCCSourceMap_4
 *
 * GCC HW source mapping
 * 
 */
static HAL_clk_SourceMapType aGCCSourceMap_4[] =
{
  { HAL_CLK_SOURCE_XO,                 0 },
  { HAL_CLK_SOURCE_SLEEPCLK,           1 },
  { HAL_CLK_SOURCE_EXTERNAL2,          2 },
  { HAL_CLK_SOURCE_GPLL0_AUX,          3 },
  { HAL_CLK_SOURCE_GROUND,             4 },
  { HAL_CLK_SOURCE_GROUND,             5 },
  { HAL_CLK_SOURCE_GROUND,             6 },
  { HAL_CLK_SOURCE_PLLTEST,            7 },
  { HAL_CLK_SOURCE_NULL,               HAL_CLK_SOURCE_INDEX_INVALID }
};


/*
 * HAL_clk_mGCCClockDomainControl_4
 *
 * Functions for controlling GCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mGCCClockDomainControl_4 =
{
   /* .ConfigMux          = */ HAL_clk_GenericConfigMux,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aGCCSourceMap_4
};


/*
 * HAL_clk_mGCCClockDomainControlRO_4
 *
 * Read-only functions for GCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mGCCClockDomainControlRO_4 =
{
   /* .ConfigMux          = */ NULL,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aGCCSourceMap_4
};


/*
 * aGCCSourceMap_5
 *
 * GCC HW source mapping
 * 
 */
static HAL_clk_SourceMapType aGCCSourceMap_5[] =
{
  { HAL_CLK_SOURCE_XO,                 0 },
  { HAL_CLK_SOURCE_GPLL0,              1 },
  //{ HAL_CLK_SOURCE_BIMC_PLL,           2 },
  //{ HAL_CLK_SOURCE_GPLL5_EARLY,        3 },
  //{ HAL_CLK_SOURCE_BIMC_PLL_EARLY,     4 },
  { HAL_CLK_SOURCE_GPLL5,              5 },
  { HAL_CLK_SOURCE_GROUND,             6 },
  { HAL_CLK_SOURCE_PLLTEST,            7 },
  { HAL_CLK_SOURCE_NULL,               HAL_CLK_SOURCE_INDEX_INVALID }
};


/*
 * HAL_clk_mGCCClockDomainControl_5
 *
 * Functions for controlling GCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mGCCClockDomainControl_5 =
{
   /* .ConfigMux          = */ HAL_clk_GenericConfigMux,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aGCCSourceMap_5
};


/*
 * HAL_clk_mGCCClockDomainControlRO_5
 *
 * Read-only functions for GCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mGCCClockDomainControlRO_5 =
{
   /* .ConfigMux          = */ NULL,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aGCCSourceMap_5
};


/*
 * aGCCSourceMap_7
 *
 * GCC HW source mapping
 * 
 */
static HAL_clk_SourceMapType aGCCSourceMap_7[] =
{
  { HAL_CLK_SOURCE_XO,                 0 },
  { HAL_CLK_SOURCE_GPLL1,              1 },
  { HAL_CLK_SOURCE_GROUND,             2 },
  { HAL_CLK_SOURCE_GROUND,             3 },
  { HAL_CLK_SOURCE_GROUND,             4 },
  { HAL_CLK_SOURCE_GROUND,             5 },
  { HAL_CLK_SOURCE_GROUND,             6 },
  { HAL_CLK_SOURCE_PLLTEST,            7 },
  { HAL_CLK_SOURCE_NULL,               HAL_CLK_SOURCE_INDEX_INVALID }
};


/*
 * HAL_clk_mGCCClockDomainControl_7
 *
 * Functions for controlling GCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mGCCClockDomainControl_7 =
{
   /* .ConfigMux          = */ HAL_clk_GenericConfigMux,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aGCCSourceMap_7
};


/*
 * HAL_clk_mGCCClockDomainControlRO_7
 *
 * Read-only functions for GCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mGCCClockDomainControlRO_7 =
{
   /* .ConfigMux          = */ NULL,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aGCCSourceMap_7
};


/*
 * aGCCSourceMap_9
 *
 * GCC HW source mapping
 * 
 */
static HAL_clk_SourceMapType aGCCSourceMap_9[] =
{
  { HAL_CLK_SOURCE_XO,                 0 },
  { HAL_CLK_SOURCE_GPLL0,              1 },
  { HAL_CLK_SOURCE_GPLL3,              2 },
  { HAL_CLK_SOURCE_GPLL6_AUX,          3 },
  //{ HAL_CLK_SOURCE_GPLL4_AUX,          4 },
  { HAL_CLK_SOURCE_GROUND,             5 },
  { HAL_CLK_SOURCE_GROUND,             6 },
  { HAL_CLK_SOURCE_PLLTEST,            7 },
  { HAL_CLK_SOURCE_NULL,               HAL_CLK_SOURCE_INDEX_INVALID }
};


/*
 * HAL_clk_mGCCClockDomainControl_9
 *
 * Functions for controlling GCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mGCCClockDomainControl_9 =
{
   /* .ConfigMux          = */ HAL_clk_GenericConfigMux,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aGCCSourceMap_9
};


/*
 * HAL_clk_mGCCClockDomainControlRO_9
 *
 * Read-only functions for GCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mGCCClockDomainControlRO_9 =
{
   /* .ConfigMux          = */ NULL,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aGCCSourceMap_9
};


/*
 * aGCCSourceMap_10
 *
 * GCC HW source mapping
 * 
 */
static HAL_clk_SourceMapType aGCCSourceMap_10[] =
{
  { HAL_CLK_SOURCE_XO,                 0 },
  { HAL_CLK_SOURCE_GPLL0,              1 },
  { HAL_CLK_SOURCE_EXTERNAL2,          2 },
  { HAL_CLK_SOURCE_GPLL6_AUX,          3 },
  { HAL_CLK_SOURCE_GROUND,             4 },
  { HAL_CLK_SOURCE_GROUND,             5 },
  { HAL_CLK_SOURCE_GROUND,             6 },
  { HAL_CLK_SOURCE_PLLTEST,            7 },
  { HAL_CLK_SOURCE_NULL,               HAL_CLK_SOURCE_INDEX_INVALID }
};


/*
 * HAL_clk_mGCCClockDomainControl_10
 *
 * Functions for controlling GCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mGCCClockDomainControl_10 =
{
   /* .ConfigMux          = */ HAL_clk_GenericConfigMux,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aGCCSourceMap_10
};


/*
 * HAL_clk_mGCCClockDomainControlRO_10
 *
 * Read-only functions for GCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mGCCClockDomainControlRO_10 =
{
   /* .ConfigMux          = */ NULL,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aGCCSourceMap_10
};


/*
 * aGCCSourceMap_11
 *
 * GCC HW source mapping
 * 
 */
static HAL_clk_SourceMapType aGCCSourceMap_11[] =
{
  { HAL_CLK_SOURCE_XO,                 0 },
  { HAL_CLK_SOURCE_GPLL0,              1 },
  { HAL_CLK_SOURCE_GPLL4,              2 },
  { HAL_CLK_SOURCE_GPLL6_AUX,          3 },
  { HAL_CLK_SOURCE_GROUND,             4 },
  { HAL_CLK_SOURCE_GROUND,             5 },
  { HAL_CLK_SOURCE_GROUND,             6 },
  { HAL_CLK_SOURCE_PLLTEST,            7 },
  { HAL_CLK_SOURCE_NULL,               HAL_CLK_SOURCE_INDEX_INVALID }
};


/*
 * HAL_clk_mGCCClockDomainControl_11
 *
 * Functions for controlling GCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mGCCClockDomainControl_11 =
{
   /* .ConfigMux          = */ HAL_clk_GenericConfigMux,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aGCCSourceMap_11
};


/*
 * HAL_clk_mGCCClockDomainControlRO_11
 *
 * Read-only functions for GCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mGCCClockDomainControlRO_11 =
{
   /* .ConfigMux          = */ NULL,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aGCCSourceMap_11
};


/*
 * HAL_clk_aGCCClockDomainDesc
 *
 * List of GCC clock domains
*/
static HAL_clk_ClockDomainDescType * HAL_clk_aGCCClockDomainDesc [] =
{
  &HAL_clk_mGCCAPSSAHBClkDomain,
  &HAL_clk_mGCCAPSSAXIClkDomain,
  &HAL_clk_mGCCBIMCDDRClkDomain,
  &HAL_clk_mGCCBIMCGPUClkDomain,
  &HAL_clk_mGCCBLSP1QUP0I2CAPPSClkDomain,
  &HAL_clk_mGCCBLSP1QUP0SPIAPPSClkDomain,
  &HAL_clk_mGCCBLSP1QUP1I2CAPPSClkDomain,
  &HAL_clk_mGCCBLSP1QUP1SPIAPPSClkDomain,
  &HAL_clk_mGCCBLSP1QUP2I2CAPPSClkDomain,
  &HAL_clk_mGCCBLSP1QUP2SPIAPPSClkDomain,
  &HAL_clk_mGCCBLSP1QUP3I2CAPPSClkDomain,
  &HAL_clk_mGCCBLSP1QUP3SPIAPPSClkDomain,
  &HAL_clk_mGCCBLSP1QUP4I2CAPPSClkDomain,
  &HAL_clk_mGCCBLSP1QUP4SPIAPPSClkDomain,
  &HAL_clk_mGCCBLSP1UART0APPSClkDomain,
  &HAL_clk_mGCCBLSP1UART1APPSClkDomain,
  &HAL_clk_mGCCBLSP1UART2APPSClkDomain,
  &HAL_clk_mGCCBLSP1UART3APPSClkDomain,
  &HAL_clk_mGCCBLSP2QUP0I2CAPPSClkDomain,
  &HAL_clk_mGCCBLSP2QUP0SPIAPPSClkDomain,
  &HAL_clk_mGCCBLSP2UART0APPSClkDomain,
  &HAL_clk_mGCCBYTE0ClkDomain,
  &HAL_clk_mGCCCRYPTOClkDomain,
  &HAL_clk_mGCCEMACClkDomain,
  &HAL_clk_mGCCEMACPTPClkDomain,
  &HAL_clk_mGCCESC0ClkDomain,
  &HAL_clk_mGCCGCCSLEEPClkDomain,
  &HAL_clk_mGCCGCCXOClkDomain,
  &HAL_clk_mGCCGFX3DClkDomain,
  &HAL_clk_mGCCGP1ClkDomain,
  &HAL_clk_mGCCGP2ClkDomain,
  &HAL_clk_mGCCGP3ClkDomain,
  &HAL_clk_mGCCHDMIAPPClkDomain,
  &HAL_clk_mGCCHDMIPCLKClkDomain,
  &HAL_clk_mGCCMDPClkDomain,
  &HAL_clk_mGCCMDSSBIMCClkDomain,
  &HAL_clk_mGCCPCIE0AUXClkDomain,
  &HAL_clk_mGCCPCIE0PIPEClkDomain,
  &HAL_clk_mGCCPCLK0ClkDomain,
  &HAL_clk_mGCCPCNOCBFDCDClkDomain,
  &HAL_clk_mGCCPDM2ClkDomain,
  &HAL_clk_mGCCSDCC1APPSClkDomain,
  &HAL_clk_mGCCSDCC1ICECOREClkDomain,
  &HAL_clk_mGCCSDCC2APPSClkDomain,
  &HAL_clk_mGCCSPMIAHBClkDomain,
  &HAL_clk_mGCCSPMISERClkDomain,
  &HAL_clk_mGCCSTANDALONEBCRClkDomain,
  &HAL_clk_mGCCSYSTEMNOCBFDCDClkDomain,
  &HAL_clk_mGCCUSB20MOCKUTMIClkDomain,
  &HAL_clk_mGCCUSB30MASTERClkDomain,
  &HAL_clk_mGCCUSB30MOCKUTMIClkDomain,
  &HAL_clk_mGCCUSB3PHYAUXClkDomain,
  &HAL_clk_mGCCUSBHSSYSTEMClkDomain,
  &HAL_clk_mGCCUSB3PHYPIPEClkDomain,
  &HAL_clk_mGCCVSYNCClkDomain,
  NULL
};


/*
 * HAL_clk_aGCCPowerDomainDesc
 *
 * List of GCC power domains
 */
static HAL_clk_PowerDomainDescType * HAL_clk_aGCCPowerDomainDesc [] =
{
  &HAL_clk_mGCCMDSSPowerDomain,
  &HAL_clk_mGCCOXILIPowerDomain,
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

