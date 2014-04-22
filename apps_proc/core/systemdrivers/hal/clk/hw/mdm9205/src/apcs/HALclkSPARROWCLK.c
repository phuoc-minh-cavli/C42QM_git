/*
==============================================================================

FILE:         HALclkSPARROWCLK.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the
   SPARROWCLK clocks.

   List of clock domains:


   List of power domains:



==============================================================================

$Header: //components/rel/core.tx/6.0/systemdrivers/hal/clk/hw/mdm9205/src/apcs/HALclkSPARROWCLK.c#1 $

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

static boolean HAL_clk_SPARROWCLKINTERNALIsEnabled(HAL_clk_ClockDescType  *pmClockDesc);
static boolean HAL_clk_SPARROWCLKINTERNALIsOn(HAL_clk_ClockDescType  *pmClockDesc);


/* ============================================================================
**    Externs
** ==========================================================================*/



/* ============================================================================
**    Data
** ==========================================================================*/


/*
 * aSPARROWCLKINTERNALSourceMap
 *
 * SPARROWCLKINTERNAL HW source mapping
 * 
 * NOTES:
 * - HAL_clk_SourceMapType is an array of mapped sources
 *   - see HALclkInternal.h.
 *
 * - If source index is reserved/not used in a clock diagram, please tie that
 *   to HAL_CLK_SOURCE_GROUND.
 *
 * - {HAL_CLK_SOURCE_NULL, HAL_CLK_SOURCE_INDEX_INVALID} is used to indicate
 *   the end of the mapping array. If we reach this element during our lookup,
 *   we'll know we could not find the matching source enum for the register
 *   value, or vice versa.
 * 
 */
static HAL_clk_SourceMapType aSPARROWCLKINTERNALSourceMap[] =
{
  {HAL_CLK_SOURCE_XO,          0},
  {HAL_CLK_SOURCE_GPLL0,       1},
  {HAL_CLK_SOURCE_GPLL1,       2},
  {HAL_CLK_SOURCE_GPLL2,       3},
  {HAL_CLK_SOURCE_GPLL3,       4},
  {HAL_CLK_SOURCE_APCSPLL0,    5},
  {HAL_CLK_SOURCE_GROUND,      6},
  {HAL_CLK_SOURCE_PLLTEST,     7},
  {HAL_CLK_SOURCE_NULL,        HAL_CLK_SOURCE_INDEX_INVALID}
};


/*
 * HAL_clk_mSPARROWCLKINTERNALClockDomainControl
 *
 * Functions for controlling SPARROWCLKINTERNAL clock domains
 */
static HAL_clk_ClockDomainControlType HAL_clk_mSPARROWCLKINTERNALClockDomainControl =
{
   /* .ConfigMux          = */ HAL_clk_GenericConfigMux,
   /* .DetectMuxConfig    = */ HAL_clk_GenericDetectMuxConfig,
   /* .IsOn               = */ HAL_clk_GenericDomainIsOn,
   /* .pmSourceMap        = */ aSPARROWCLKINTERNALSourceMap
};

/*
 * HAL_clk_mSPARROWCLKINTERNALClockControl
 *
 * Functions for controlling SPARROWCLKINTERNAL clock functions.
 * Please implement each function or NULL out.
 */
static HAL_clk_ClockControlType HAL_clk_mSPARROWCLKINTERNALClockControl =
{
  /* .Enable           = */ NULL,
  /* .Disable          = */ NULL,
  /* .IsEnabled        = */ HAL_clk_SPARROWCLKINTERNALIsEnabled,
  /* .IsOn             = */ HAL_clk_SPARROWCLKINTERNALIsOn,
  /* .Reset            = */ NULL,
  /* .IsReset          = */ NULL,
  /* .Config           = */ NULL,
  /* .DetectConfig     = */ NULL,
  /* .ConfigDivider    = */ NULL,
  /* .DetectDivider    = */ NULL,
  /* .ConfigFootswitch = */ NULL,
};


/*
 *  HAL_clk_mSPARROWCLKINTERNALIMAGEClkDomainClks
 *
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mSPARROWCLKINTERNALIMAGEClkDomainClks[] =
{
  {
    /* .szClockName      = */ "sparrow_clk_internal",
    /* .mRegisters       = */ { 0, 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mSPARROWCLKINTERNALClockControl,
    /* .nTestClock       = */ 0
  },
};


/*
 * HAL_clk_mAPCSSPARROWCLKINTERNALIMAGEClkDomain
 *
 * SPARROWCLKINTERNALIMAGE clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mAPCSSPARROWCLKINTERNALIMAGEClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(APCS_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mSPARROWCLKINTERNALIMAGEClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mSPARROWCLKINTERNALIMAGEClkDomainClks)/sizeof(HAL_clk_mSPARROWCLKINTERNALIMAGEClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mSPARROWCLKINTERNALClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


/* ============================================================================
**    FUNCTION DEFINITIONS FOR THE MODULE
** ==========================================================================*/

/*
 *  HAL_clk_SPARROWCLKINTERNALIsEnabled
 */
static boolean HAL_clk_SPARROWCLKINTERNALIsEnabled
(
  HAL_clk_ClockDescType  *pmClockDesc
)
{
  /*
   * returning TRUE because the core clock must be ENABLED if we are executing code
   */
  return TRUE;
}  /* END HAL_clk_SPARROWCLKINTERNALIsEnabled */


/*
 *  HAL_clk_SPARROWCLKINTERNALIsOn
 */
static boolean HAL_clk_SPARROWCLKINTERNALIsOn
(
  HAL_clk_ClockDescType  *pmClockDesc
)
{
  /*
   * returning TRUE because the core clock must be ON if we are executing code
   */
  return TRUE;
}  /* END HAL_clk_SPARROWCLKINTERNALIsOn */
