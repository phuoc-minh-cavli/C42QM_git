/*
==============================================================================

FILE:         HALclkGCCMain.c

DESCRIPTION:
   The main auto-generated file for GCC.


==============================================================================

$Header: //components/rel/rpm.bf/2.1.3/core/systemdrivers/hal/clk/hw/mdm9x05/src/GCC/HALclkGCCMain.c#4 $

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


/*
 * Clock domains
 */
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBIMCDDRClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBIMCDDRXOClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCCRYPTOClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCSLEEPClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCXOClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCPCNOCBFDCDClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSAPBTSCTRClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSATClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSSTMClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSTRACECLKINClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCQDSSTRIGClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCQPICClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCQPICIOMACROClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCRBCPRClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCRPMClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSPMIAHBClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCSPMISERClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCBIMCAPSSClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCDEBUGMUXClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCPRNGClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mGCCAPSSAHBClkDomain;

/*
 * Power domains
 */
extern HAL_clk_PowerDomainDescType HAL_clk_mGCCBIMCPowerDomain;


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
  { HAL_CLK_SOURCE_GPLL1,              2 },
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
  { HAL_CLK_SOURCE_GPLL0,              3 },
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
 * aGCCSourceMap_3
 *
 * GCC HW source mapping
 * 
 */
static HAL_clk_SourceMapType aGCCSourceMap_3[] =
{
  { HAL_CLK_SOURCE_XO,                 0 },
  { HAL_CLK_SOURCE_GPLL0,              1 },
  { HAL_CLK_SOURCE_BIMCPLL,            2 },
  { HAL_CLK_SOURCE_GPLL2,              3 },
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
  &HAL_clk_mGCCBIMCDDRXOClkDomain,
  &HAL_clk_mGCCCRYPTOClkDomain,
  &HAL_clk_mGCCGCCSLEEPClkDomain,
  &HAL_clk_mGCCGCCXOClkDomain,
  &HAL_clk_mGCCPCNOCBFDCDClkDomain,
  &HAL_clk_mGCCQDSSAPBTSCTRClkDomain,
  &HAL_clk_mGCCQDSSATClkDomain,
  &HAL_clk_mGCCQDSSSTMClkDomain,
  &HAL_clk_mGCCQDSSTRACECLKINClkDomain,
  &HAL_clk_mGCCQDSSTRIGClkDomain,
  &HAL_clk_mGCCQPICClkDomain,
  &HAL_clk_mGCCQPICIOMACROClkDomain,
  &HAL_clk_mGCCRBCPRClkDomain,
  &HAL_clk_mGCCRPMClkDomain,
  &HAL_clk_mGCCSPMIAHBClkDomain,
  &HAL_clk_mGCCSPMISERClkDomain,
  &HAL_clk_mGCCBIMCAPSSClkDomain,
  &HAL_clk_mGCCGCCDEBUGMUXClkDomain,
  &HAL_clk_mGCCPRNGClkDomain,
  &HAL_clk_mGCCAPSSAHBClkDomain,
  NULL
};


/*
 * HAL_clk_aGCCPowerDomainDesc
 *
 * List of GCC power domains
 */
static HAL_clk_PowerDomainDescType * HAL_clk_aGCCPowerDomainDesc [] =
{
  &HAL_clk_mGCCBIMCPowerDomain,
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

