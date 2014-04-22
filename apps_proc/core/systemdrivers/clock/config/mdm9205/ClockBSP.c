/*
==============================================================================

FILE:         ClockBSP.c

DESCRIPTION:
  This file contains clock BSP data for the DAL based driver.


==============================================================================

$Header: //components/rel/core.tx/6.0/systemdrivers/clock/config/mdm9205/ClockBSP.c#2 $

==============================================================================
  Copyright (c) 2019, 2020 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/

#include "ClockBSP.h"


/*=========================================================================
      Data Declarations
==========================================================================*/


/*
 *  SourceFreqConfig_XO
 *
 *  Set of source frequency configurations.
 */
static ClockSourceFreqConfigType SourceFreqConfig_XO[] =
{
  {
    /* .nFreqHz    = */  19200 * 1000,
    /* .HALConfig  = */  { HAL_CLK_SOURCE_NULL },
    /* .eVRegLevel = */  VCS_CORNER_OFF,
    /* .HWVersion  = */  { {0x00, 0x00}, {0xFF, 0xFF} },
  },
  /* last entry */
  { 0 }
};


/*
 * SourceFreqCalibrationConfig_APCSPLL0
 *
 * Used by:
 *   APCSPLL0
 *
 *  Set of source frequency calibration configurations.
 */
static ClockSourceFreqConfigType SourceFreqCalibrationConfig_APCSPLL0[] =
{
  {
    /* .nFreqHz    = */  787200 * 1000,
    /* .HALConfig  = */
    {
      /* .eSource        = */  HAL_CLK_SOURCE_XO,
      /* .eVCO           = */  HAL_CLK_PLL_VCO2,
      /* .nPreDiv        = */  1,
      /* .nPostDiv       = */  1,
      /* .nL             = */  41,
      /* .nM             = */  0,
      /* .nN             = */  0,
      /* .nVCOMultiplier = */  0,
      /* .nAlpha         = */  0x00000000,
      /* .nAlphaU        = */  0x00,
    },
    /* .eVRegLevel = */  VCS_CORNER_NOMINAL,
    /* .HWVersion  = */  { {0x00, 0x00}, {0xFF, 0xFF} },
  },
  /* last entry */
  { 0 }
};


/*
 * SourceFreqConfig_GPLL0
 *
 * Used by:
 *   GPLL0
 */
static ClockSourceFreqConfigType SourceFreqConfig_GPLL0[] =
{
  {
    /* .nFreqHz    = */  800000 * 1000,
    /* .HALConfig  = */
    {
      /* .eSource        = */  HAL_CLK_SOURCE_XO,
      /* .eVCO           = */  HAL_CLK_PLL_VCO2,
      /* .nPreDiv        = */  1,
      /* .nPostDiv       = */  1,
      /* .nL             = */  41,
      /* .nM             = */  0,
      /* .nN             = */  0,
      /* .nVCOMultiplier = */  0,
      /* .nAlpha         = */  0xAA000000,
      /* .nAlphaU        = */  0xAA,
    },
    /* .eVRegLevel = */  VCS_CORNER_LOW,
    /* .HWVersion = */   { {0x00, 0x00}, {0xFF, 0xFF} },
  },
  /* last entry */
  { 0 }
};

/*
 *  SourceFreqConfig_APCSPLL0
 *
 *  Set of source frequency configurations.
 */
ClockSourceFreqConfigType SourceFreqConfig_APCSPLL0[] =
{
  {
    /* .nFreqHz    = */  499200 * 1000,
    /* .HALConfig  = */
    {
      /* .eSource        = */  HAL_CLK_SOURCE_XO,
      /* .eVCO           = */  HAL_CLK_PLL_VCO2,
      /* .nPreDiv        = */  1,
      /* .nPostDiv       = */  2,
      /* .nL             = */  52,
      /* .nM             = */  0,
      /* .nN             = */  0,
      /* .nVCOMultiplier = */  0,
      /* .nAlpha         = */  0x00000000,
      /* .nAlphaU        = */  0x00,
    },
    /* .eVRegLevel = */  VCS_CORNER_NOMINAL,
    /* .HWVersion  = */  { {0x00, 0x00}, {0xFF, 0xFF} },
  },
  {
    /* .nFreqHz    = */  806400 * 1000,
    /* .HALConfig  = */
    {
      /* .eSource        = */  HAL_CLK_SOURCE_XO,
      /* .eVCO           = */  HAL_CLK_PLL_VCO2,
      /* .nPreDiv        = */  1,
      /* .nPostDiv       = */  1,
      /* .nL             = */  42,
      /* .nM             = */  0,
      /* .nN             = */  0,
      /* .nVCOMultiplier = */  0,
      /* .nAlpha         = */  0x00000000,
      /* .nAlphaU        = */  0x00,
    },
    /*
     * NOTE: Although Stromer PLL datasheet states that SVS can support only
     *       till 800 MHz, PLL & MSIP teams have confirmed that 806.4 MHz can
     *       also be supported.
     */
    /* .eVRegLevel = */  VCS_CORNER_LOW,
    /* .HWVersion  = */  { {0x00, 0x00}, {0xFF, 0xFF} },
  },
  /* last entry */
  { 0 }
};

/*
 * SourceConfig
 *
 * Clock source configuration data.
 */
const ClockSourceConfigType SourceConfig[] =
{
  {
    SOURCE_NAME(XO),

    /* .nConfigMask       = */  0,
    /* .pSourceFreqConfig = */  SourceFreqConfig_XO,
  },
  {
    SOURCE_NAME(GPLL0),

    /* .nConfigMask       = */  CLOCK_CONFIG_PLL_FSM_MODE_ENABLE | CLOCK_CONFIG_PLL_AUX_OUTPUT_ENABLE,
    /* .pSourceFreqConfig = */  SourceFreqConfig_GPLL0,
  },
  {
    SOURCE_NAME(APCSPLL0),

    /* .nConfigMask            = */ CLOCK_CONFIG_PLL_EARLY_OUTPUT_ENABLE,
    /* .pSourceFreqConfig      = */ SourceFreqConfig_APCSPLL0,
    /* .pCalibrationFreqConfig = */ SourceFreqCalibrationConfig_APCSPLL0,
    /* .eDisableMode           = */ HAL_CLK_SOURCE_DISABLE_MODE_FREEZE
  },
   { SOURCE_NAME(NULL) }
};


/*----------------------------------------------------------------------*/
/* GCC Clock Configurations                                             */
/*----------------------------------------------------------------------*/


/*
 * BLSP1QUP1I2CAPPS clock configurations
 *
 * Used by:
 *   BLSP1QUP1I2CAPPS
 *   BLSP1QUP2I2CAPPS
 *   BLSP1QUP3I2CAPPS
 *   BLSP1QUP4I2CAPPS
 */
const ClockMuxConfigType BLSP1QUP1I2CAPPSClockConfig[] =
{
  {   19200000, { HAL_CLK_SOURCE_XO,             2,     0,     0,     0      }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {   50000000, { HAL_CLK_SOURCE_GPLL0,          32,    0,     0,     0      }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  { 0 }
};


/*
 * BLSP1QUP1SPIAPPS clock configurations
 *
 * Used by:
 *   BLSP1QUP1SPIAPPS
 *   BLSP1QUP2SPIAPPS
 *   BLSP1QUP3SPIAPPS
 *   BLSP1QUP4SPIAPPS
 */
const ClockMuxConfigType BLSP1QUP1SPIAPPSClockConfig[] =
{
  {     960000, { HAL_CLK_SOURCE_XO,             20,    1,     2,     2      }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    4800000, { HAL_CLK_SOURCE_XO,             8,     0,     0,     0      }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    9600000, { HAL_CLK_SOURCE_XO,             4,     0,     0,     0      }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {   16000000, { HAL_CLK_SOURCE_GPLL0,          20,    1,     5,     5      }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  {   19200000, { HAL_CLK_SOURCE_XO,             2,     0,     0,     0      }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {   25000000, { HAL_CLK_SOURCE_GPLL0,          32,    1,     2,     2      }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  {   50000000, { HAL_CLK_SOURCE_GPLL0,          32,    0,     0,     0      }, VCS_CORNER_NOMINAL,             },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  { 0 }
};


/*
 * BLSP1UART1APPS clock configurations
 *
 * Used by:
 *   BLSP1UART1APPS
 *   BLSP1UART2APPS
 *   BLSP1UART3APPS
 *   BLSP1UART4APPS
 */
const ClockMuxConfigType BLSP1UART1APPSClockConfig[] =
{
  {    3686400, { HAL_CLK_SOURCE_GPLL0,          2,     72,    15625, 15625  }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  {    7372800, { HAL_CLK_SOURCE_GPLL0,          2,     144,   15625, 15625  }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  {   14745600, { HAL_CLK_SOURCE_GPLL0,          2,     288,   15625, 15625  }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  {   16000000, { HAL_CLK_SOURCE_GPLL0,          20,    1,     5,     5      }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  {   19200000, { HAL_CLK_SOURCE_XO,             2,     0,     0,     0      }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {   24000000, { HAL_CLK_SOURCE_GPLL0,          2,     3,     100,   100    }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  {   25000000, { HAL_CLK_SOURCE_GPLL0,          32,    1,     2,     2      }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  {   32000000, { HAL_CLK_SOURCE_GPLL0,          2,     1,     25,    25     }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  {   40000000, { HAL_CLK_SOURCE_GPLL0,          2,     1,     20,    20     }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  {   46400000, { HAL_CLK_SOURCE_GPLL0,          2,     29,    500,   500    }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  {   48000000, { HAL_CLK_SOURCE_GPLL0,          2,     3,     50,    50     }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  {   51200000, { HAL_CLK_SOURCE_GPLL0,          2,     8,     125,   125    }, VCS_CORNER_NOMINAL,             },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  {   56000000, { HAL_CLK_SOURCE_GPLL0,          2,     7,     100,   100    }, VCS_CORNER_NOMINAL,             },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  {   58982400, { HAL_CLK_SOURCE_GPLL0,          2,     1152,  15625, 15625  }, VCS_CORNER_NOMINAL,             },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  {   60000000, { HAL_CLK_SOURCE_GPLL0,          2,     3,     40,    40     }, VCS_CORNER_NOMINAL,             },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  {   64000000, { HAL_CLK_SOURCE_GPLL0,          2,     2,     25,    25     }, VCS_CORNER_NOMINAL,             },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  { 0 }
};


/*
 * GCCECC clock configurations
 *
 * Used by:
 *   GCCECC
 */
const ClockMuxConfigType GCCECCClockConfig[] =
{
  {   19200000, { HAL_CLK_SOURCE_XO,             2,     0,     0,     0      }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {  160000000, { HAL_CLK_SOURCE_GPLL0,          10,    0,     0,     0      }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  {  320000000, { HAL_CLK_SOURCE_GPLL0,          5,     0,     0,     0      }, VCS_CORNER_NOMINAL,             },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  { 0 }
};


/*
 * GP1 clock configurations
 *
 * Used by:
 *   GP1
 *   GP2
 *   GP3
 */
const ClockMuxConfigType GP1ClockConfig[] =
{
  {   19200000, { HAL_CLK_SOURCE_XO,             2,     0,     0,     0      }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {  100000000, { HAL_CLK_SOURCE_GPLL0,          16,    0,     0,     0      }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  {  200000000, { HAL_CLK_SOURCE_GPLL0,          8,     0,     0,     0      }, VCS_CORNER_NOMINAL,             },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  { 0 }
};


/*
 * PDM2 clock configurations
 *
 * Used by:
 *   PDM2
 */
const ClockMuxConfigType PDM2ClockConfig[] =
{
  {   64000000, { HAL_CLK_SOURCE_GPLL0,          25,    0,     0,     0      }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  { 0 }
};


/*
 * USBHSSYSTEM clock configurations
 *
 * Used by:
 *   USBHSSYSTEM
 */
const ClockMuxConfigType USBHSSYSTEMClockConfig[] =
{
  {   19200000, { HAL_CLK_SOURCE_XO,             2,     0,     0,     0      }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {   69565217, { HAL_CLK_SOURCE_GPLL0,          23,    0,     0,     0      }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  {  133333333, { HAL_CLK_SOURCE_GPLL0,          12,    0,     0,     0      }, VCS_CORNER_NOMINAL,             },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  {  177777778, { HAL_CLK_SOURCE_GPLL0,          9,     0,     0,     0      }, VCS_CORNER_TURBO,               },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  { 0 }
};


/*
 * Clock Log Default Configuration.
 *
 * NOTE: An .nGlobalLogFlags value of 0x12 will log only clock frequency
 *       changes and source state changes by default.
 */
const ClockLogType ClockLogDefaultConfig[] =
{
  {
    /* .nLogSize        = */ 1024,
    /* .nGlobalLogFlags = */ 0x12
  }
};


/*
 * Clock Flag Init Config.
 */
const ClockFlagInitType ClockFlagInitConfig[] =
{
  {
    /* .eNodeType = */ CLOCK_FLAG_NODE_TYPE_SOURCE,
    /* .pID       = */ (void *)"XO",
    /* .nFlags    = */ CLOCK_FLAG_SOURCE_NOT_CONFIGURABLE
  },
  {
    /* .eNodeType = */ CLOCK_FLAG_NODE_TYPE_SOURCE,
    /* .pID       = */ (void *)"APCSPLL0",
    /* .nFlags    = */ CLOCK_FLAG_SUPPORTS_SLEWING
  },
  {
    /* .eNodeType = */ CLOCK_FLAG_NODE_TYPE_SOURCE,
    /* .pID       = */ (void *)"GPLL0_AUX",
    /* .nFlags    = */ CLOCK_FLAG_SOURCE_NOT_CONFIGURABLE
  },
  {
    /* .eNodeType = */ CLOCK_FLAG_NODE_TYPE_CLOCK_DOMAIN,
    /* .pID       = */ (void *)"sparrow_clk_internal",
    /* .nFlags    = */ CLOCK_FLAG_SUPPRESSIBLE | CLOCK_FLAG_DOMAIN_HOLD_VREG_REQUEST
  },
  {
    /* .eNodeType = */ CLOCK_FLAG_NODE_TYPE_CLOCK,
    /* .pID       = */ (void *)"gcc_pdm_ahb_clk",
    /* .nFlags    = */ CLOCK_FLAG_SUPPRESSIBLE
  },
  {
    /* .eNodeType = */ CLOCK_FLAG_NODE_TYPE_CLOCK,
    /* .pID       = */ (void *)"gcc_usb2a_phy_sleep_clk",
    /* .nFlags    = */ CLOCK_FLAG_SUPPRESSIBLE
  },
  {
    /* .eNodeType = */ CLOCK_FLAG_NODE_TYPE_CLOCK,
    /* .pID       = */ (void *)"gcc_spmi_ahb_clk",
    /* .nFlags    = */ CLOCK_FLAG_INTERNAL_CONTROL
  },
  {
    /* .eNodeType = */ CLOCK_FLAG_NODE_TYPE_CLOCK,
    /* .pID       = */ (void *)"gcc_spmi_ser_clk",
    /* .nFlags    = */ CLOCK_FLAG_INTERNAL_CONTROL
  },
  {
    /* .eNodeType = */ CLOCK_FLAG_NODE_TYPE_NONE,
    /* .pID       = */ (void *)0,
    /* .nFlags    = */ 0
  }
};

#ifdef CLOCK_STUB_FWK_ENABLE
/*
 * Stub flags.
 */
const ClockStubType ClockStubConfig =
{
  /* .bRUMI   */ FALSE
};
#endif
