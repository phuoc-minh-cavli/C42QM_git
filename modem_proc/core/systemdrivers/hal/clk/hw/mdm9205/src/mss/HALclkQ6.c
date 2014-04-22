/*
==============================================================================

FILE:         HALclkQ6.c

DESCRIPTION:
   This file contains the clock HAL code for the QDSP6 core clock.


==============================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.10/systemdrivers/hal/clk/hw/mdm9205/src/mss/HALclkQ6.c#2 $

when          who     what, where, why
--------      ---     ----------------------------------------------------------- 
12/06/18      shm     First version for 9205

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

static boolean HAL_clk_Q6CoreClkIsEnabled    ( HAL_clk_ClockDescType  *pmClockDesc );
static boolean HAL_clk_Q6CoreClkIsOn         ( HAL_clk_ClockDescType  *pmClockDesc );


/* ============================================================================
**    Externs
** ==========================================================================*/

extern HAL_clk_ClockDomainControlType  HAL_clk_mMSSCCClockDomainControl_2;

/* ============================================================================
**    Data
** ==========================================================================*/


/*
 * HAL_clk_mQ6ClockControl
 *
 * Functions for controlling Q6 clock functions.
 */
HAL_clk_ClockControlType HAL_clk_mQ6ClockControl =
{
  /* .Enable           = */ NULL,
  /* .Disable          = */ NULL,
  /* .IsEnabled        = */ HAL_clk_Q6CoreClkIsEnabled,
  /* .IsOn             = */ HAL_clk_Q6CoreClkIsOn,
  /* .Reset            = */ NULL,
  /* .IsReset          = */ NULL,
  /* .Config           = */ NULL,
  /* .DetectConfig     = */ NULL,
  /* .ConfigDivider    = */ NULL,
  /* .DetectDivider    = */ NULL,
  /* .ConfigFootswitch = */ NULL,
};


/*
 *  HAL_clk_mSDCC1APPSClkDomainClks
 *
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mQ6CoreClkDomainClks[] =
{
  {
    /* .szClockName      = */ "clk_q6",
    /* .mRegisters       = */ { 0, 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mQ6ClockControl,
    /* .nTestClock       = */ HAL_CLK_MSSCC_TEST_CLK_Q6
  },
};


/*
 * HAL_clk_mMSSQ6ClkDomain
 *
 * Q6 clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mMSSQ6ClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(MSS_QDSP6SS_CORE_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mQ6CoreClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mQ6CoreClkDomainClks)/sizeof(HAL_clk_mQ6CoreClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mMSSCCClockDomainControl_2,
  /* .pmNextClockDomain    = */ NULL
};


/*============================================================================

               FUNCTION DEFINITIONS FOR MODULE

============================================================================*/


/* ===========================================================================
**  HAL_clk_Q6CoreClkIsEnabled
**
** ======================================================================== */

boolean HAL_clk_Q6CoreClkIsEnabled
(
  HAL_clk_ClockDescType  *pmClockDesc
)
{
  /*
   * If this function is being called then Q6 core clock has to be enabled.
   * NOTE: We can't check ROOT_EN bit since enable is auto set by the HW.
   */
  return TRUE;

}  /* END HAL_clk_Q6CoreClkIsEnabled */


/* ===========================================================================
**  HAL_clk_Q6CoreClkIsOn
**
** ======================================================================== */

boolean HAL_clk_Q6CoreClkIsOn
(
  HAL_clk_ClockDescType  *pmClockDesc
)
{
  if (HWIO_INF(MSS_QDSP6SS_CORE_CMD_RCGR, ROOT_OFF))
  {
    return FALSE;
  }
  else
  {
    return TRUE;
  }
}  /* END HAL_clk_Q6CoreClkIsOn */
