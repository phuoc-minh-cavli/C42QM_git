#ifndef __CLOCKMSSBSP_H__
#define __CLOCKMSSBSP_H__
/*
===========================================================================
*/
/**
  @file ClockMSSBSP.h

  Header description for the MSS clock driver BSP format.
*/
/*
  ====================================================================

  Copyright (c) 2018 Qualcomm Technologies Incorporated.
  All rights reserved.
  QUALCOMM Proprietary and Confidential.

  ==================================================================== 
  $Header: //components/rel/core.mpss/3.10/systemdrivers/clock/hw/mdm9205/mss/inc/ClockMSSBSP.h#2 $
  $DateTime: 2019/01/02 21:42:16 $
  $Author: pwbldsvc $
 
  when       who     what, where, why
  --------   ---     -------------------------------------------------
  12/11/18   shm     First version for 9205

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
 * CoreConfig     - QDSP6 core configuration
 * Mux            - General mux configuration
 * eCornerMSS     - VDD_MSS voltage level for this performance level
 * nStrapACCVal   - 4 byte value for STRAP_ACC register at this level.
 */
typedef struct
{
  HAL_clk_ClockConfigType CoreConfig;
  ClockMuxConfigType      Mux;
  VCSCornerType           eCornerMSS;
  uint32                  nStrapACCVal;
} ClockCPUConfigType;


/*
 * ClockCPUPerfConfigType
 *
 * Actual format for the data stored in the BSP.
 *
 *  HWVersion      - Version of the chip HW this configuration is for.
 *  nMinPerfLevel  - Minimum performance level
 *  nMaxPerfLevel  - Maximum performance level
 *  nInitPerfLevel - Initial performance level
 *  anPerfLevel    - Array of performance level indices
 *  nNumPerfLevels - Number of PerfLevels in anPerfLevel
 */
typedef struct
{
  ClockHWVersionType  HWVersion;
  uint32              nMinPerfLevel;
  uint32              nMaxPerfLevel;
  uint32              nInitPerfLevel;
  uint32             *anPerfLevel;
  uint32              nNumPerfLevels;
} ClockCPUPerfConfigType;


/*
 * ClockConfigBusConfigType
 *
 * Parameters for an MSS config bus configuration.
 *
 * Mux            - General mux configuration
 */
typedef struct
{
  ClockMuxConfigType     Mux;
} ClockConfigBusConfigType;


/*
 * ClockConfigBusPerfConfigType
 *
 * Actual format for the data stored in the BSP for the MSS Config Bus.
 *
 *  nMinPerfLevel - Minimum performance level
 *  nMaxPerfLevel - Maximum performance level
 *  anPerfLevel   - Array of performance level indices
 */
typedef struct
{
  uint32  nMinPerfLevel;
  uint32  nMaxPerfLevel;
  uint32 *anPerfLevel;
} ClockConfigBusPerfConfigType;



/*
 * ClockImageBSPConfigType
 *
 * Image BSP data configuration.
 *
 *  bEnableDCS              - Enable boolean for dynamic clock scaling.
 *  pCPUConfig              - Array of CPU configurations.
 *  pCPUPerfConfig          - Array of CPU perf configurations.
 *  nNumCPUPerfLevelConfigs - Number of CPU perf levels
 *  pConfigBusConfig        - Array of Config Bus configurations.
 *  pConfigBusPerfConfig    - Array of Config Bus perf levels.
 */
typedef struct
{
  boolean                       bEnableDCS;
  ClockCPUConfigType           *pCPUConfig;
  ClockCPUPerfConfigType       *pCPUPerfConfig;
  uint32                        nNumCPUPerfLevelConfigs;
  ClockConfigBusConfigType     *pConfigBusConfig;
  ClockConfigBusPerfConfigType *pConfigBusPerfConfig;
} ClockImageBSPConfigType;


/*=========================================================================
      Extern Definitions
==========================================================================*/


#endif  /* __CLOCKMSSBSP_H__ */
