/*
==============================================================================

FILE:         HALclkAPCSMain.c

DESCRIPTION:
   The main auto-generated file for APCS.


==============================================================================

$Header: //components/rel/core.tx/6.0/systemdrivers/hal/clk/hw/mdm9205/src/apcs/HALclkAPCSMain.c#1 $

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
extern HAL_clk_ClockDomainDescType HAL_clk_mAPCSSPARROWCLKINTERNALIMAGEClkDomain;

/*
 * Power domains
 */


/* ============================================================================
**    Data
** ==========================================================================*/


/*
 * HAL_clk_aAPCSClockDomainDesc
 *
 * List of APCS clock domains
*/
static HAL_clk_ClockDomainDescType * HAL_clk_aAPCSClockDomainDesc [] =
{
  &HAL_clk_mAPCSSPARROWCLKINTERNALIMAGEClkDomain,
  NULL
};


/*
 * HAL_clk_aAPCSPowerDomainDesc
 *
 * List of APCS power domains
 */
static HAL_clk_PowerDomainDescType * HAL_clk_aAPCSPowerDomainDesc [] =
{
  NULL
};


/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/


/* ===========================================================================
**  HAL_clk_PlatformInitAPCSMain
**
** ======================================================================== */

void HAL_clk_PlatformInitAPCSMain (void)
{

  /*
   * Install all clock domains
   */
  HAL_clk_InstallClockDomains(HAL_clk_aAPCSClockDomainDesc, A7SS_BASE);

  /*
   * Install all power domains
   */
  HAL_clk_InstallPowerDomains(HAL_clk_aAPCSPowerDomainDesc, A7SS_BASE);

} /* END HAL_clk_PlatformInitAPCSMain */
