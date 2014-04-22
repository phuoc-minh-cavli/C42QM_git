/*
===========================================================================
*/
/**
  @file ClockMSSBSP.c

  BSP data for the MSS clock driver.
*/
/*
  ====================================================================

  Copyright (c) 2018 Qualcomm Technologies Incorporated.
  All rights reserved.
  QUALCOMM Proprietary and Confidential.

  ==================================================================== 
  $Header: //components/rel/core.mpss/3.10/systemdrivers/clock/config/mdm9205/ClockMSSBSP.c#2 $
  $DateTime: 2019/01/02 21:42:16 $
  $Author: pwbldsvc $
 
  when       who     what, where, why
  --------   ---     -------------------------------------------------
  04/12/18   shm     Clock driver changes
  09/10/18   shm     Initial version for 9205

  ====================================================================
*/


/*=========================================================================
      Include Files
==========================================================================*/


#include "ClockMSSBSP.h"
#include "comdef.h"
#include "VCSDefs.h"


/*=========================================================================
      Macros
==========================================================================*/


/*=========================================================================
      Type Definitions
==========================================================================*/

/*
 * Enumeration of QDSP6 performance levels.
 */
enum
{
  CLOCK_QDSP6_PERF_LEVEL_0,
  CLOCK_QDSP6_PERF_LEVEL_1,
  CLOCK_QDSP6_PERF_LEVEL_2,
  CLOCK_QDSP6_PERF_LEVEL_TOTAL
};


/*
 * Enumeration of QDSP6 configurations.
 */
enum
{
  CLOCK_QDSP6_CONFIG_326P40MHz,
  CLOCK_QDSP6_CONFIG_480MHz,
  CLOCK_QDSP6_CONFIG_576MHz,
  CLOCK_QDSP6_CONFIG_TOTAL
};


/*
 * Enumeration of MSS Config Bus performance levels.
 */
enum
{
  CLOCK_CONFIG_BUS_PERF_LEVEL_0,
  CLOCK_CONFIG_BUS_PERF_LEVEL_1,
  CLOCK_CONFIG_BUS_PERF_LEVEL_TOTAL
};


/*
 * Enumeration of MSS Config Bus configurations.
 */
enum
{
  CLOCK_CONFIG_BUS_CONFIG_XO,
  CLOCK_CONFIG_BUS_CONFIG_76P80MHz,
  CLOCK_CONFIG_BUS_CONFIG_TOTAL
};


/*=========================================================================
      Data
==========================================================================*/

/*
 * The MPLL2 source frequency configurations are defined in ClockBSP.c
 */
extern ClockSourceFreqConfigType SourceFreqConfig_MPLL2[];

/*
 * QDSP6 performance levels.
 */
static uint32 Clock_CPUPerfLevels[] =
{
  CLOCK_QDSP6_CONFIG_326P40MHz,
  CLOCK_QDSP6_CONFIG_480MHz,
  CLOCK_QDSP6_CONFIG_576MHz,
};

/*
 * Config Bus performance levels
 */
static uint32 Clock_ConfigBusPerfLevels [] =
{
  CLOCK_CONFIG_BUS_CONFIG_XO,
  CLOCK_CONFIG_BUS_CONFIG_76P80MHz,
};


/*
 * Mux configuration for different CPU frequencies.
 */
static ClockCPUConfigType Clock_CPUConfig[] =
{
  /* NOTE: Divider value is (2*Div) due to potential use of fractional values */
  {
    .CoreConfig     = HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX,
    .Mux            =
    {
      .nFreqHz           = 326400 * 1000,
      .HALConfig         = {HAL_CLK_SOURCE_MPLL2, 4},
      .HWVersion         = {{0x00, 0x00}, {0xFF, 0xFF}},
      .pSourceFreqConfig = &SourceFreqConfig_MPLL2[2] // 652.8 MHz
    },
    .eCornerMSS   = VCS_CORNER_LOW,
  },
  {
    .CoreConfig     = HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX,
    .Mux            =
    {
      .nFreqHz           = 480000 * 1000,
      .HALConfig         = {HAL_CLK_SOURCE_MPLL2, 2},
      .HWVersion         = {{0x00, 0x00}, {0xFF, 0xFF}},
      .pSourceFreqConfig = &SourceFreqConfig_MPLL2[0] // 480.0 MHz
    },
    .eCornerMSS   = VCS_CORNER_NOMINAL,
  },
  {
    .CoreConfig     = HAL_CLK_CONFIG_Q6SS_CORE_CLOCK_MUX,
    .Mux            =
    {
      .nFreqHz           = 576000 * 1000,
      .HALConfig         = {HAL_CLK_SOURCE_MPLL2, 2},
      .HWVersion         = {{0x00, 0x00}, {0xFF, 0xFF}},
      .pSourceFreqConfig = &SourceFreqConfig_MPLL2[1] // 576.0 MHz
    },
    .eCornerMSS   = VCS_CORNER_TURBO,
  },
};


/*
 * Performance level configuration data for the CPU clock.
 */
static ClockCPUPerfConfigType Clock_CPUPerfConfig[] =
{
  {
    .HWVersion      = {{0x00, 0x00}, {0xFF, 0xFF}},
    .nMinPerfLevel  = CLOCK_QDSP6_PERF_LEVEL_0,
    .nMaxPerfLevel  = CLOCK_QDSP6_PERF_LEVEL_2,
    .nInitPerfLevel = CLOCK_QDSP6_PERF_LEVEL_1,
    .anPerfLevel    = Clock_CPUPerfLevels,
    .nNumPerfLevels = ARR_SIZE(Clock_CPUPerfLevels)
  },
};


/*
 * Mux configuration for different MSS config bus frequencies.
 */
static ClockConfigBusConfigType Clock_ConfigBusConfig[] =
{
  /*  NOTE: Divider value is (2*Div) due to potential use of fractional values */
  {
    /* .Mux */   { 19200 * 1000, { HAL_CLK_SOURCE_XO, 2 }, VCS_CORNER_LOW }
  },
  {
    /* .Mux */   { 76800 * 1000, { HAL_CLK_SOURCE_MPLL1, 24 }, VCS_CORNER_LOW }
  },
};

/*
 * Performance level configuration data for the MSS config bus.
 */
static ClockConfigBusPerfConfigType Clock_ConfigBusPerfConfig =
{
  .nMinPerfLevel = CLOCK_CONFIG_BUS_PERF_LEVEL_0,
  .nMaxPerfLevel = CLOCK_CONFIG_BUS_PERF_LEVEL_1,
  .anPerfLevel   = Clock_ConfigBusPerfLevels
};


/*
 * Image BSP data
 */
const ClockImageBSPConfigType ClockImageBSPConfig =
{
  .bEnableDCS              = TRUE,
  .pCPUConfig              = Clock_CPUConfig,
  .pCPUPerfConfig          = Clock_CPUPerfConfig,
  .nNumCPUPerfLevelConfigs = ARR_SIZE(Clock_CPUPerfConfig),
  .pConfigBusConfig        = Clock_ConfigBusConfig,
  .pConfigBusPerfConfig    = &Clock_ConfigBusPerfConfig
};
