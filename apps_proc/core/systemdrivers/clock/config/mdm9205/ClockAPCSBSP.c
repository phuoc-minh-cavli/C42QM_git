/*
==============================================================================

FILE:         ClockAPCSBSP.c

DESCRIPTION:
  This file contains clock BSP data for the DAL based driver.


==============================================================================

$Header: //components/rel/core.tx/6.0/systemdrivers/clock/config/mdm9205/ClockAPCSBSP.c#1 $

==============================================================================
            Copyright (c) 2019 Qualcomm Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockAPCSBSP.h"
#include "comdef.h"
#include "VCSDefs.h"


/*=========================================================================
      Data Declarations
==========================================================================*/


/*
 * Enumeration of SPARROWCLKINTERNAL performance levels.
 */
enum
{
  CLOCK_SPARROWCLKINTERNAL_PERF_LEVEL_0,
  CLOCK_SPARROWCLKINTERNAL_PERF_LEVEL_1,
  CLOCK_SPARROWCLKINTERNAL_PERF_LEVEL_2,
  CLOCK_SPARROWCLKINTERNAL_PERF_LEVEL_3,
  CLOCK_SPARROWCLKINTERNAL_PERF_LEVEL_TOTAL
};


/*
 * Enumeration of SPARROWCLKINTERNAL configurations.
 */
enum
{
  CLOCK_SPARROWCLKINTERNAL_CONFIG_400P0MHz,
  CLOCK_SPARROWCLKINTERNAL_CONFIG_403P2MHz,
  CLOCK_SPARROWCLKINTERNAL_CONFIG_499P2MHz,
  CLOCK_SPARROWCLKINTERNAL_CONFIG_806P4MHz,
  CLOCK_SPARROWCLKINTERNAL_CONFIG_TOTAL
};


/*
 * SPARROWCLKINTERNAL performance levels mappings
 */
static uint32 Clock_SPARROWCLKINTERNALPerfLevels [] =
{
  CLOCK_SPARROWCLKINTERNAL_CONFIG_400P0MHz,
  CLOCK_SPARROWCLKINTERNAL_CONFIG_403P2MHz,
  CLOCK_SPARROWCLKINTERNAL_CONFIG_499P2MHz,
  CLOCK_SPARROWCLKINTERNAL_CONFIG_806P4MHz,
};


/*=========================================================================
      Data
==========================================================================*/


/*
 * The APCSPLL0 source frequency configurations are defined in ClockBSP.c
 */
extern ClockSourceFreqConfigType SourceFreqConfig_APCSPLL0[];

/*
 * SPARROWCLKINTERNAL clock configurations
 */
static ClockCPUConfigType Clock_SPARROWCLKINTERNALConfig[] =
{
  /* NOTE: Divider value is (2*Div) due to potential use of fractional values */
  {
    /* .Mux             = */
    {
      /* .nFreq             = */  400000000,
      /* .HALConfig         = */  { HAL_CLK_SOURCE_GPLL0, 4, 0, 0, 0 },
      /* .eVRegLevel        = */  VCS_CORNER_LOW,
      /* .HWVersion         = */  { {0x00, 0x00}, {0xFF, 0xFF} },
      /* .pSourceFreqConfig = */  NULL
    },
  },
  {
    /* .Mux             = */
    {
      /* .nFreq             = */  403200000,
      /* .HALConfig         = */  { HAL_CLK_SOURCE_APCSPLL0, 4, 0, 0, 0 },
      /* .eVRegLevel        = */  VCS_CORNER_LOW,
      /* .HWVersion         = */  { {0x00, 0x00}, {0xFF, 0xFF} },
      /* .pSourceFreqConfig = */  &SourceFreqConfig_APCSPLL0[1] // 806.4 MHz
    },
  },
  {
    /* .Mux             = */
    {
      /* .nFreq             = */  499200000,
      /* .HALConfig         = */  { HAL_CLK_SOURCE_APCSPLL0, 2, 0, 0, 0 },
      /* .eVRegLevel        = */  VCS_CORNER_NOMINAL,
      /* .HWVersion         = */  { {0x00, 0x00}, {0xFF, 0xFF} },
      /* .pSourceFreqConfig = */  &SourceFreqConfig_APCSPLL0[0] // 499.2 MHz
    },
  },
  {
    /* .Mux             = */
    {
      /* .nFreq             = */  806400000,
      /* .HALConfig         = */  { HAL_CLK_SOURCE_APCSPLL0, 2, 0, 0, 0 },
      /* .eVRegLevel        = */  VCS_CORNER_TURBO,
      /* .HWVersion         = */  { {0x00, 0x00}, {0xFF, 0xFF} },
      /* .pSourceFreqConfig = */  &SourceFreqConfig_APCSPLL0[1] // 806.4 MHz
    },
  },
  { 0 }
};

/*
 * Performance level configuration data for the CPU clock.
 */
static ClockCPUPerfConfigType Clock_CPUPerfConfig[] =
{
  {
    /*
     * v1 performance configuration
     */
    { {0x00, 0x00}, {0xFF, 0xFF} },

    /*
     * Minimum performance level
     */
    CLOCK_SPARROWCLKINTERNAL_PERF_LEVEL_1,

    /*
     * Maximum performance level
     */
    CLOCK_SPARROWCLKINTERNAL_PERF_LEVEL_3,

    /*
     * Define default performance level mappings.
     */
    Clock_SPARROWCLKINTERNALPerfLevels,
    
    /*
     * Number of Performance Levels
     */
    ARR_SIZE(Clock_SPARROWCLKINTERNALPerfLevels)
  },
};

/*
 * Image BSP data
 */
const ClockImageBSPConfigType ClockImageBSPConfig =
{
  /*
   * EnableDCS flag
   */
  TRUE,

  /*
   * CPU config
   */
  Clock_SPARROWCLKINTERNALConfig,

  /*
   * CPU performance config
   */
  Clock_CPUPerfConfig,

  /*
   * Total number of performance configurations
   */
  ARR_SIZE(Clock_CPUPerfConfig),

  /*
   * eCPU
   */
  CLOCK_CPU_APPS_C0,

  /*
   * eRail
   */
  VCS_RAIL_CX,

  /*
   * szRailName
   */
  VCS_NPA_RESOURCE_VDD_CX,

  /*
   * APPS Sleep Index
   *
   * Note: There is a fundamental assumption that we will go to sleep at
   *       a frequency which only requires SVS voltage. The reason being
   *       when ProcessorSleep function is called, we can no longer talk
   *       to the RPM to vote for a higher voltage level. The sleep index
   *       must correspond to a frequency level at SVS.
   */
  CLOCK_SPARROWCLKINTERNAL_CONFIG_403P2MHz,

 /*
  * nPLLCalibrationPerfLevel
  */
  CLOCK_SPARROWCLKINTERNAL_CONFIG_400P0MHz,

 /*
  * nInitCPUPerfLevel
  */
  CLOCK_SPARROWCLKINTERNAL_CONFIG_806P4MHz,
};
