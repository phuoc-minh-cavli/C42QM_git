#ifndef __CLOCKAPPSBSP_H__
#define __CLOCKAPPSBSP_H__
/*
===========================================================================
*/
/**
  @file ClockAppsBSP.h

  Header description for the clock driver BSP format.
*/
/*
  ====================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary/GTDR

  ==================================================================== 
  $Header: //components/rel/core.tx/6.0/systemdrivers/clock/hw/mdm9205/apps/inc/ClockAPCSBSP.h#1 $ 
  $DateTime: 2020/01/30 22:49:35 $ 
  $Author: pwbldsvc $

  when       who     what, where, why
  --------   ---     -------------------------------------------------
  10/02/18   shm     First version for 9205

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockBSP.h"


/*=========================================================================
      Type Definitions
==========================================================================*/


/*
 * ClockCPUConfigType
 *
 * Configuration parameters for a CPU performance level.
 *
 *  Mux - General mux configuration.
 */
typedef struct
{
  ClockMuxConfigType Mux;
} ClockCPUConfigType;


/*
 * ClockCPUPerfConfigType
 *
 * Actual format for the data stored in the BSP.
 *
 *  HWVersion      - Version of the chip HW this configuration is for.
 *  nMinPerfLevel       - Minimum performance level
 *  nMaxPerfLevel       - Maximum performance level
 *  anPerfLevel         - Array of performance level indices
 *  nNumPerfLevels - Number of PerfLevels in anPerfLevel
 */
typedef struct
{
  ClockHWVersionType  HWVersion;
  uint32              nMinPerfLevel;
  uint32              nMaxPerfLevel;
  uint32             *anPerfLevel;
  uint32              nNumPerfLevels;
} ClockCPUPerfConfigType;

/*
 * ClockImageBSPConfigType
 *
 * Image BSP data configuration.
 *
 *  bEnableDCS               - Enable boolean for dynamic clock scaling.
 *  pCPUConfig               - Array of CPU configurations.
 *  pCPUPerfConfig           - Array of CPU perf configurations.
 *  nNumCPUPerfLevelConfigs  - Number of CPU perf levels
 *  eCPU                     - Identifier of CPU.
 *  eRail                    - Identifier of rail powering the CPU.
 *  szRailName               - String name of NPA resource for CPU power rail.
 *  nSleepPerfLevel          - Performance level for processor sleep
 *  nPLLCalibrationPerfLevel - Performance level to use when recalibrating the APCS PLL.
 *  nInitCPUPerfLevel        - Performance level to use while initializing the CPU.
 */
typedef struct
{
  boolean                  bEnableDCS;
  ClockCPUConfigType      *pCPUConfig;
  ClockCPUPerfConfigType  *pCPUPerfConfig;
  uint32                   nNumCPUPerfLevelConfigs;
  ClockCPUType             eCPU;
  VCSRailType              eRail;
  const char              *szRailName;
  uint32                   nSleepPerfLevel;
  uint32                   nPLLCalibrationPerfLevel;
  uint32                   nInitCPUPerfLevel;
} ClockImageBSPConfigType;


/*=========================================================================
      Extern Definitions
==========================================================================*/


#endif  /* __CLOCKAPPSBSP_H__ */ 
