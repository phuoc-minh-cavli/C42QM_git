/*
===========================================================================
  @file ClockCPU.c

  This file provides clock initialization for the Apps.
===========================================================================

                             Edit History


when      who           what, where, why
--------  ------------  ------------------------------------------------------
03/02/18  avk           Initial QCS405 revision, branched from SDM660.

==============================================================================
            Copyright (c) 2018 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================

*/

/*=========================================================================
      Include Files
==========================================================================*/
#include "ClockCommon.h"
#include "ClockHWIO.h"
#include "ClockBSP.h"
#include "busywait.h"
#include "boot_error_if.h"
/*=========================================================================
      Macro Definitions
==========================================================================*/

/*=========================================================================
     Externs
==========================================================================*/

/*=========================================================================
      Function Prototypes
==========================================================================*/

/*=========================================================================
      Function Definitions
==========================================================================*/

/*=========================================================================
      Data
==========================================================================*/
/* variable that holds the configured apps clock freq */
uint32 clock_apps_speed_khz = 0;

/*=========================================================================
      Function Definitions
=========================================================================*/

/* ============================================================================
**  Function : Clock_SetCPUPerfLevel
** ============================================================================
*/
/**
  Configure Krait CPU to a specific perf level.

  @param eCPUPerfLevel [in] - CPU performance level.

  @return
  TRUE -- CPU was configured to perf level successful.
  FALSE -- Configuration failed.

  @dependencies
  None.

  @sa
  Clock_Init.
*/

boolean Clock_SetCPUPerfLevel(ClockBootPerfLevelType eCPUPerfLevel)
{
  Clock_ConfigType *cfg = Clock_Config();
  const ClockConfigMuxType *CPU_mux, *CXO_cfg;

  if( eCPUPerfLevel >= CLOCK_BOOT_PERF_NUM ) return FALSE;

  CPU_mux = &cfg->CPU_Cfg[eCPUPerfLevel].ApcsMux;

  /* Check to see if the switch is from A53PLL to A53PLL */
  if( CPU_mux->eSource == SRC_A53PERFPLL )
  {
    if(HWIO_INF(APCS_CPU_PLL_MODE, PLL_OUTCTRL))
    {
      /* Switch away from A53PERF PLL so that it can be re-tuned */
      CXO_cfg = &cfg->CPU_Cfg[CLOCK_BOOT_PERF_SVS].ApcsMux;
      if( ! Clock_EnableSource( CXO_cfg->eSource )) return FALSE;
      if( ! Clock_ConfigMux(CXO_cfg)) return FALSE;

      /* Disable the A53PLL */
      HWIO_OUTF(APCS_CPU_PLL_MODE, PLL_OUTCTRL, 0x0);
    }
  }

  /* Now switch */
  if( ! Clock_EnableSource( CPU_mux->eSource )) return FALSE;
  if( ! Clock_ConfigMux(CPU_mux)) return FALSE;

  clock_apps_speed_khz = cfg->CPU_Cfg[eCPUPerfLevel].nFrequency;

  return TRUE;
}

/* ============================================================================
**  Function : Clock_SetL2PerfLevel
** ============================================================================
*/
/*!
    Configure L2 cache to a specific perf level.

    @param eL2PerfLevel   -  [IN] CPU performance level

    @return
    TRUE -- Initialization was successful.
    FALSE -- Initialization failed.

    @dependencies
    None.

    @sa None
*/
boolean Clock_SetL2PerfLevel(ClockBootPerfLevelType eL2PerfLevel)
{
  return FALSE;
}
boolean Clock_LMhDeInit( void )
{
	return FALSE;
}
boolean Clock_LMhInit( uint32 nA57uV )
{
	return FALSE;
}
uint32 Clock_LMhPreCalibration( uint32 nFreqKHz )
{
	return FALSE;
}