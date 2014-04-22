/*
==============================================================================

FILE:         HALclkMSSCCMain.c

DESCRIPTION:
   The main auto-generated file for MSSCC.


==============================================================================

$Header: //components/rel/core.mpss/3.10/systemdrivers/hal/clk/hw/mdm9205/src/mss/HALclkMSSCCMain.c#1 $

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
extern HAL_clk_ClockDomainDescType HAL_clk_mMSSCCCLKADCDIVClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mMSSCCCLKBUSPOSTMUXClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mMSSCCCLKMODEMAXIClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mMSSCCCOXMBITClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mMSSCCXOMNDUIM0ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mMSSCCUIMClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mMSSCCCLKTWOXOClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mMSSCCCLOCKDRIVERGENNODOMAINClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mMSSCCCLOCKDRIVERGENSTANDALONEBCRClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mMSSQ6ClkDomain;
extern HAL_clk_ClockDomainDescType HAL_clk_mMSSConfigBusClkDomain;

/*
 * Power domains
 */


/* ============================================================================
**    Data
** ==========================================================================*/


/*
 * aMSSCCSourceMap
 *
 * MSSCC HW source mapping
 *
 */
static HAL_clk_SourceMapType aMSSCCSourceMap[] =
{
  { HAL_CLK_SOURCE_XO,                 0 },
  { HAL_CLK_SOURCE_MPLL1,              1 },
  { HAL_CLK_SOURCE_MPLL1,              2 }, // EARLY
  { HAL_CLK_SOURCE_MPLL2,              3 },
  { HAL_CLK_SOURCE_GROUND,             4 },
  { HAL_CLK_SOURCE_GROUND,             5 },
  { HAL_CLK_SOURCE_GROUND,             6 },
  { HAL_CLK_SOURCE_PLLTEST,            7 },
  { HAL_CLK_SOURCE_NULL,               HAL_CLK_SOURCE_INDEX_INVALID }
};


/*
 * HAL_clk_mMSSCCClockDomainControl
 *
 * Functions for controlling MSSCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mMSSCCClockDomainControl =
{
   /* .ConfigMux          = */ HAL_clk_GenericConfigMux,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aMSSCCSourceMap
};


/*
 * HAL_clk_mMSSCCClockDomainControlRO
 *
 * Read-only functions for MSSCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mMSSCCClockDomainControlRO =
{
   /* .ConfigMux          = */ NULL,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aMSSCCSourceMap
};


/*
 * aMSSCCSourceMap_1
 *
 * MSSCC HW source mapping
 *
 */
static HAL_clk_SourceMapType aMSSCCSourceMap_1[] =
{
  { HAL_CLK_SOURCE_XO,                 0 },
  { HAL_CLK_SOURCE_MPLL1,              1 },  // AUX
  { HAL_CLK_SOURCE_MPLL1,              2 },  // AUX
  { HAL_CLK_SOURCE_GROUND,             3 },
  { HAL_CLK_SOURCE_GROUND,             4 },
  { HAL_CLK_SOURCE_GROUND,             5 },
  { HAL_CLK_SOURCE_GROUND,             6 },
  { HAL_CLK_SOURCE_PLLTEST,            7 },
  { HAL_CLK_SOURCE_NULL,               HAL_CLK_SOURCE_INDEX_INVALID }
};


/*
 * HAL_clk_mMSSCCClockDomainControl_1
 *
 * Functions for controlling MSSCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mMSSCCClockDomainControl_1 =
{
   /* .ConfigMux          = */ HAL_clk_GenericConfigMux,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aMSSCCSourceMap_1
};


/*
 * HAL_clk_mMSSCCClockDomainControlRO_1
 *
 * Read-only functions for MSSCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mMSSCCClockDomainControlRO_1 =
{
   /* .ConfigMux          = */ NULL,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aMSSCCSourceMap_1
};


/*
 * aMSSCCSourceMap_2
 *
 * MSSCC HW source mapping
 *
 */
static HAL_clk_SourceMapType aMSSCCSourceMap_2[] =
{
  { HAL_CLK_SOURCE_GROUND,             0 },
  { HAL_CLK_SOURCE_MPLL2,              1 },
  { HAL_CLK_SOURCE_GROUND,             2 },
  { HAL_CLK_SOURCE_GROUND,             3 },
  { HAL_CLK_SOURCE_GROUND,             4 },
  { HAL_CLK_SOURCE_GROUND,             5 },
  { HAL_CLK_SOURCE_GROUND,             6 },
  { HAL_CLK_SOURCE_PLLTEST,            7 },
  { HAL_CLK_SOURCE_NULL,               HAL_CLK_SOURCE_INDEX_INVALID }
};


/*
 * HAL_clk_mMSSCCClockDomainControl_2
 *
 * Functions for controlling MSSCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mMSSCCClockDomainControl_2 =
{
   /* .ConfigMux          = */ HAL_clk_GenericConfigMux,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aMSSCCSourceMap_2
};


/*
 * HAL_clk_mMSSCCClockDomainControlRO_2
 *
 * Read-only functions for MSSCC clock domains
 */
HAL_clk_ClockDomainControlType HAL_clk_mMSSCCClockDomainControlRO_2 =
{
   /* .ConfigMux          = */ NULL,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .pmSourceMap        = */ aMSSCCSourceMap_2
};


/*
 * HAL_clk_aMSSCCClockDomainDesc
 *
 * List of MSSCC clock domains
*/
static HAL_clk_ClockDomainDescType * HAL_clk_aMSSCCClockDomainDesc [] =
{
  &HAL_clk_mMSSCCCLKADCDIVClkDomain,
  &HAL_clk_mMSSCCCLKBUSPOSTMUXClkDomain,
  &HAL_clk_mMSSCCCLKMODEMAXIClkDomain,
  &HAL_clk_mMSSCCCOXMBITClkDomain,
  &HAL_clk_mMSSCCXOMNDUIM0ClkDomain,
  &HAL_clk_mMSSCCUIMClkDomain,
  &HAL_clk_mMSSCCCLKTWOXOClkDomain,
  &HAL_clk_mMSSCCCLOCKDRIVERGENNODOMAINClkDomain,
  &HAL_clk_mMSSQ6ClkDomain,
  &HAL_clk_mMSSConfigBusClkDomain,
  NULL
};


/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/


/* ===========================================================================
**  HAL_clk_PlatformInitMSSCCMain
**
** ======================================================================== */

void HAL_clk_PlatformInitMSSCCMain (void)
{

  /*
   * Install all clock domains
   */
  HAL_clk_InstallClockDomains(HAL_clk_aMSSCCClockDomainDesc, MSS_TOP_BASE);

} /* END HAL_clk_PlatformInitMSSCCMain */
