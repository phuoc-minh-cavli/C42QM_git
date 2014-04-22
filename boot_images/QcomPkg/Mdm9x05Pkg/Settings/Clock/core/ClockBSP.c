/*
==============================================================================

FILE:         ClockBSP.c 

DESCRIPTION:
  This file contains clock bsp data for the DAL based driver.


==============================================================================

$Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Settings/Clock/core/ClockBSP.c#1 $

==============================================================================
            Copyright (c) 2018 QUALCOMM Technologies Incorporated.
                    All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
==============================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/

#include "Drivers/ClockDxe/ClockBSP.h"

/*=========================================================================
      Data Declarations
==========================================================================*/


/*
 *  SourceFreqConfig_SLEEPCLK
 *
 *  Set of source frequency configurations.
 */
static ClockSourceFreqConfigType SourceFreqConfig_SLEEPCLK[] =
{
  {
    /* .nFreqHz    = */  32768,
    /* .HALConfig  = */  { HAL_CLK_SOURCE_NULL },
    /* .eVRegLevel = */  CLOCK_VREG_LEVEL_OFF,
    /* .HWVersion  = */  { {0x00, 0x00}, {0x00, 0x00} },
  },
  /* last entry */
  { 0 }
};


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
    /* .eVRegLevel = */  CLOCK_VREG_LEVEL_OFF,
    /* .HWVersion  = */  { {0x00, 0x00}, {0x00, 0x00} },
  },
  /* last entry */
  { 0 }
};

/*
 *  SourceFreqConfig_GPLL6_AUX
 *
 *  Set of source frequency configurations.
 */
static ClockSourceFreqConfigType SourceFreqConfig_GPLL6_AUX[] =
{
  {
    /* .nFreqHz    = */  1080000 * 1000,
    /* .HALConfig  = */  { HAL_CLK_SOURCE_GPLL6 },
    /* .eVRegLevel = */  CLOCK_VREG_LEVEL_LOW,
    /* .HWVersion  = */  { {0x00, 0x00}, {0x00, 0x00} },
  },
  /* last entry */
  { 0 }
};


/*
 *  SourceFreqConfig_GPLL0
 *
 *  Set of source frequency configurations.
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
    /* .eVRegLevel = */  CLOCK_VREG_LEVEL_LOW,
    /* .HWVersion  = */  { {0x00, 0x00}, {0x00, 0x00} },
  },
  /* last entry */
  { 0 }
};


/*
 *  SourceFreqConfig_GPLL1
 *
 *  Set of source frequency configurations.
 */
static ClockSourceFreqConfigType SourceFreqConfig_GPLL1[] =
{
  {
    /* .nFreqHz    = */  500000 * 1000,
    /* .HALConfig  = */
    {
      /* .eSource        = */  HAL_CLK_SOURCE_XO,
      /* .eVCO           = */  HAL_CLK_PLL_VCO3,
      /* .nPreDiv        = */  1,
      /* .nPostDiv       = */  1,
      /* .nL             = */  26,
      /* .nM             = */  0,
      /* .nN             = */  0,
      /* .nVCOMultiplier = */  0,
      /* .nAlpha         = */  0xAAAAAAAA,
      /* .nAlphaU        = */  0x0A,
    },
    /* .eVRegLevel = */  CLOCK_VREG_LEVEL_LOW,
    /* .HWVersion  = */  { {0x00, 0x00}, {0x00, 0x00} },
  },
  /* last entry */
  { 0 }
};


/*
 *  SourceFreqConfig_GPLL3
 *
 *  Set of source frequency configurations.
 */
static ClockSourceFreqConfigType SourceFreqConfig_GPLL3[] =
{
  {
    /* .nFreqHz    = */  484800 * 1000,
    /* .HALConfig  = */
    {
      /* .eSource        = */  HAL_CLK_SOURCE_XO,
      /* .eVCO           = */  HAL_CLK_PLL_VCO1,
      /* .nPreDiv        = */  1,
      /* .nPostDiv       = */  2,
      /* .nL             = */  50,
      /* .nM             = */  0,
      /* .nN             = */  0,
      /* .nVCOMultiplier = */  0,
      /* .nAlpha         = */  0x00000000,
      /* .nAlphaU        = */  0x80,
    },
    /* .eVRegLevel = */  CLOCK_VREG_LEVEL_NOMINAL,/*LOW, VCO2 can do the job here, but for slewing works in the same VCO mode*/
    /* .HWVersion  = */  { {0x00, 0x00}, {0x00, 0x00} },
  },
  {
    /* .nFreqHz    = */  523200 * 1000,
    /* .HALConfig  = */
    {
      /* .eSource        = */  HAL_CLK_SOURCE_XO,
      /* .eVCO           = */  HAL_CLK_PLL_VCO1,
      /* .nPreDiv        = */  1,
      /* .nPostDiv       = */  2,
      /* .nL             = */  54,
      /* .nM             = */  0,
      /* .nN             = */  0,
      /* .nVCOMultiplier = */  0,
      /* .nAlpha         = */  0x00000000,
      /* .nAlphaU        = */  0x80,
    },
    /* .eVRegLevel = */  CLOCK_VREG_LEVEL_NOMINAL, /*LOW, VCO2 can do the job here, but for slewing works in the same VCO mode*/
    /* .HWVersion  = */  { {0x00, 0x00}, {0x00, 0x00} },
  },
  {
    /* .nFreqHz    = */  550000 * 1000,
    /* .HALConfig  = */
    {
      /* .eSource        = */  HAL_CLK_SOURCE_XO,
      /* .eVCO           = */  HAL_CLK_PLL_VCO1,
      /* .nPreDiv        = */  1,
      /* .nPostDiv       = */  2,
      /* .nL             = */  57,
      /* .nM             = */  0,
      /* .nN             = */  0,
      /* .nVCOMultiplier = */  0,
      /* .nAlpha         = */  0xAAAAAAAA,
      /* .nAlphaU        = */  0x4A,
    },
    /* .eVRegLevel = */  CLOCK_VREG_LEVEL_NOMINAL, /*LOW, VCO2 can do the job here, but for slewing works in the same VCO mode*/
    /* .HWVersion  = */  { {0x00, 0x00}, {0x00, 0x00} },
  },
  {
    /* .nFreqHz    = */  598000 * 1000,
    /* .HALConfig  = */
    {
      /* .eSource        = */  HAL_CLK_SOURCE_XO,
      /* .eVCO           = */  HAL_CLK_PLL_VCO1,
      /* .nPreDiv        = */  1,
      /* .nPostDiv       = */  2,
      /* .nL             = */  62,
      /* .nM             = */  0,
      /* .nN             = */  0,
      /* .nVCOMultiplier = */  0,
      /* .nAlpha         = */  0xAA000000,
      /* .nAlphaU        = */  0x4A,
    },
    /* .eVRegLevel = */  CLOCK_VREG_LEVEL_NOMINAL,
    /* .HWVersion  = */  { {0x00, 0x00}, {0x00, 0x00} },
  },
  /* last entry */
  { 0 }
};


/*
 *  SourceFreqConfig_GPLL4
 *
 *  Set of source frequency configurations.
 */
static ClockSourceFreqConfigType SourceFreqConfig_GPLL4[] =
{
  {
    /* .nFreqHz    = */  1152000 * 1000,
    /* .HALConfig  = */
    {
      /* .eSource        = */  HAL_CLK_SOURCE_XO,
      /* .eVCO           = */  HAL_CLK_PLL_VCO1,
      /* .nPreDiv        = */  1,
      /* .nPostDiv       = */  1,
      /* .nL             = */  60,
      /* .nM             = */  0,
      /* .nN             = */  0,
      /* .nVCOMultiplier = */  0,
      /* .nAlpha         = */  0x00000000,
      /* .nAlphaU        = */  0x00,
    },
    /* .eVRegLevel = */  CLOCK_VREG_LEVEL_NOMINAL,
    /* .HWVersion  = */  { {0x00, 0x00}, {0x00, 0x00} },
  },
  /* last entry */
  { 0 }
};

/*
 *  SourceFreqConfig_GPLL6
 *
 *  Set of source frequency configurations.
 */
static ClockSourceFreqConfigType SourceFreqConfig_GPLL6[] =
{
  {
    /* .nFreqHz    = */  1080000 * 1000,
    /* .HALConfig  = */
    {
      /* .eSource        = */  HAL_CLK_SOURCE_XO,
      /* .eVCO           = */  HAL_CLK_PLL_VCO1,
      /* .nPreDiv        = */  1,
      /* .nPostDiv       = */  1,
      /* .nL             = */  56,
      /* .nM             = */  1,
      /* .nN             = */  4,
      /* .nVCOMultiplier = */  0,
      /* .nAlpha         = */  0x00000000,
      /* .nAlphaU        = */  0x00,
    },
    /* .eVRegLevel = */  CLOCK_VREG_LEVEL_LOW,
    /* .HWVersion  = */  { {0x00, 0x00}, {0x00, 0x00} },
  },
  /* last entry */
  { 0 }
};


/*
 *  SourceFreqConfig_APCSPLL1_PLL
 *
 *  Set of source frequency configurations.
 */
static ClockSourceFreqConfigType SourceFreqConfig_APCSPLL1_PLL[] =
{
  {
    /* .nFreqHz    = */  1113600 * 1000,
    /* .HALConfig  = */
    {
      /* .eSource        = */  HAL_CLK_SOURCE_XO,
      /* .eVCO           = */  HAL_CLK_PLL_VCO1,
      /* .nPreDiv        = */  1,
      /* .nPostDiv       = */  1,
      /* .nL             = */  58,
      /* .nM             = */  0,
      /* .nN             = */  0,
      /* .nVCOMultiplier = */  0,
      /* .nAlpha         = */  0x00000000,
      /* .nAlphaU        = */  0x00,
    },
    /* .eVRegLevel = */  CLOCK_VREG_LEVEL_NOMINAL,
    /* .HWVersion  = */  { {0x00, 0x00}, {0x00, 0x00} },
    /* .bLastEntry = */  0
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
    SOURCE_NAME(SLEEPCLK),

    /* .nConfigMask       = */  0,
    /* .pSourceFreqConfig = */  SourceFreqConfig_SLEEPCLK,
  },
  {
    SOURCE_NAME(XO),

    /* .nConfigMask       = */  0,
    /* .pSourceFreqConfig = */  SourceFreqConfig_XO,
  },
  {
    SOURCE_NAME(GPLL6_AUX),

    /* .nConfigMask       = */  0,
    /* .pSourceFreqConfig = */  SourceFreqConfig_GPLL6_AUX,
  },
  {
    SOURCE_NAME(GPLL0),

    /* .nConfigMask       = */  CLOCK_CONFIG_PLL_FSM_MODE_ENABLE,
    /* .pSourceFreqConfig = */  SourceFreqConfig_GPLL0,
  },
  {
    SOURCE_NAME(GPLL1),

    /* .nConfigMask       = */  CLOCK_CONFIG_PLL_FSM_MODE_ENABLE,
    /* .pSourceFreqConfig = */  SourceFreqConfig_GPLL1,
  },
  {
    SOURCE_NAME(GPLL3),

    /* .nConfigMask       = */  0,
    /* .pSourceFreqConfig = */  SourceFreqConfig_GPLL3,
    /* .pCalibrationFreqConfig  = */  &SourceFreqConfig_GPLL3[3]
  },
  {
    SOURCE_NAME(GPLL4),

    /* .nConfigMask       = */  CLOCK_CONFIG_PLL_FSM_MODE_ENABLE,
    /* .pSourceFreqConfig = */  SourceFreqConfig_GPLL4,
  },
  {
    SOURCE_NAME(GPLL6),

    /* .nConfigMask       = */  CLOCK_CONFIG_PLL_FSM_MODE_ENABLE,
    /* .pSourceFreqConfig = */  SourceFreqConfig_GPLL6,
  },
  {
    SOURCE_NAME(APCSPLL1),

    /* .nConfigMask             = */  0,
    /* .pSourceFreqConfig       = */  SourceFreqConfig_APCSPLL1_PLL,
    /* .pCalibrationFreqConfig  = */  SourceFreqConfig_APCSPLL1_PLL,
    /* .eDisableMode            = */  HAL_CLK_SOURCE_DISABLE_MODE_NORMAL
  },
  { 0 }
};


/*----------------------------------------------------------------------*/
/* GCC   Clock Configurations                                           */
/*----------------------------------------------------------------------*/


/*
 * APSSAHB clock configurations
 */
const ClockMuxConfigType APSSAHBClockConfig[] =
{
  {   19200000, { HAL_CLK_SOURCE_XO,             2,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {   50000000, { HAL_CLK_SOURCE_GPLL0,          32,     0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  100000000, { HAL_CLK_SOURCE_GPLL0,          16,     0,      0,      0       }, CLOCK_VREG_LEVEL_NOMINAL,     },
  {  133333333, { HAL_CLK_SOURCE_GPLL0,          12,     0,      0,      0       }, CLOCK_VREG_LEVEL_HIGH,        },
  { 0 }
};


/*
 * BLSP1QUP0I2CAPPS clock configurations
 */
const ClockMuxConfigType BLSP1QUP0I2CAPPSClockConfig[] =
{
  {   19200000, { HAL_CLK_SOURCE_XO,             2,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {   50000000, { HAL_CLK_SOURCE_GPLL0,          32,     0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  { 0 }
};


/*
 * BLSP1QUP0SPIAPPS clock configurations
 */
const ClockMuxConfigType BLSP1QUP0SPIAPPSClockConfig[] =
{
  {     960000, { HAL_CLK_SOURCE_XO,             20,     1,      2,      2       }, CLOCK_VREG_LEVEL_LOW,         },
  {    4800000, { HAL_CLK_SOURCE_XO,             8,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {    9600000, { HAL_CLK_SOURCE_XO,             4,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {   16000000, { HAL_CLK_SOURCE_GPLL0,          20,     1,      5,      5       }, CLOCK_VREG_LEVEL_LOW,         },
  {   19200000, { HAL_CLK_SOURCE_XO,             2,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {   25000000, { HAL_CLK_SOURCE_GPLL0,          32,     1,      2,      2       }, CLOCK_VREG_LEVEL_LOW,         },
  {   50000000, { HAL_CLK_SOURCE_GPLL0,          32,     0,      0,      0       }, CLOCK_VREG_LEVEL_NOMINAL,     },
  { 0 }
};


/*
 * BLSP1QUP1SPIAPPS clock configurations
 */
const ClockMuxConfigType BLSP1QUP1SPIAPPSClockConfig[] =
{
  {     960000, { HAL_CLK_SOURCE_XO,             20,     1,      2,      2       }, CLOCK_VREG_LEVEL_LOW,         },
  {    4800000, { HAL_CLK_SOURCE_XO,             8,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {    9600000, { HAL_CLK_SOURCE_XO,             4,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {   16000000, { HAL_CLK_SOURCE_GPLL0,          20,     1,      5,      5       }, CLOCK_VREG_LEVEL_NOMINAL,     },
  {   19200000, { HAL_CLK_SOURCE_XO,             2,      0,      0,      0       }, CLOCK_VREG_LEVEL_NOMINAL,     },
  {   20960000, { HAL_CLK_SOURCE_GPLL0,          2,      6,    229,    229       }, CLOCK_VREG_LEVEL_NOMINAL,     }, /* HW team recommended to limit 21 MHz */
  { 0 }
};


/*
 * BLSP1QUP2SPIAPPS clock configurations
 */
const ClockMuxConfigType BLSP1QUP2SPIAPPSClockConfig[] =
{
  {     960000, { HAL_CLK_SOURCE_XO,             20,     1,      2,      2       }, CLOCK_VREG_LEVEL_LOW,         },
  {    4800000, { HAL_CLK_SOURCE_XO,             8,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {    9600000, { HAL_CLK_SOURCE_XO,             4,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {   16000000, { HAL_CLK_SOURCE_GPLL0,          20,     1,      5,      5       }, CLOCK_VREG_LEVEL_NOMINAL,     },
  {   19200000, { HAL_CLK_SOURCE_XO,             2,      0,      0,      0       }, CLOCK_VREG_LEVEL_NOMINAL,     },
  {   25000000, { HAL_CLK_SOURCE_GPLL0,          32,     1,      2,      2       }, CLOCK_VREG_LEVEL_NOMINAL,     },
  {   30000000, { HAL_CLK_SOURCE_GPLL0,          2,      3,      80,     80      }, CLOCK_VREG_LEVEL_NOMINAL,     },
  { 0 }
};


/*
 * BLSP1UART0APPS clock configurations
 */
const ClockMuxConfigType BLSP1UART0APPSClockConfig[] =
{
  {    3686400, { HAL_CLK_SOURCE_GPLL0,          2,      72,     15625,  15625   }, CLOCK_VREG_LEVEL_LOW,         },
  {    7372800, { HAL_CLK_SOURCE_GPLL0,          2,      144,    15625,  15625   }, CLOCK_VREG_LEVEL_LOW,         },
  {   14745600, { HAL_CLK_SOURCE_GPLL0,          2,      288,    15625,  15625   }, CLOCK_VREG_LEVEL_LOW,         },
  {   16000000, { HAL_CLK_SOURCE_GPLL0,          20,     1,      5,      5       }, CLOCK_VREG_LEVEL_LOW,         },
  {   19200000, { HAL_CLK_SOURCE_XO,             2,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {   24000000, { HAL_CLK_SOURCE_GPLL0,          2,      3,      100,    100     }, CLOCK_VREG_LEVEL_LOW,         },
  {   25000000, { HAL_CLK_SOURCE_GPLL0,          32,     1,      2,      2       }, CLOCK_VREG_LEVEL_LOW,         },
  {   32000000, { HAL_CLK_SOURCE_GPLL0,          2,      1,      25,     25      }, CLOCK_VREG_LEVEL_LOW,         },
  {   40000000, { HAL_CLK_SOURCE_GPLL0,          2,      1,      20,     20      }, CLOCK_VREG_LEVEL_NOMINAL,     },
  {   46400000, { HAL_CLK_SOURCE_GPLL0,          2,      29,     500,    500     }, CLOCK_VREG_LEVEL_NOMINAL,     },
  {   48000000, { HAL_CLK_SOURCE_GPLL0,          2,      3,      50,     50      }, CLOCK_VREG_LEVEL_NOMINAL,     },
  {   51200000, { HAL_CLK_SOURCE_GPLL0,          2,      8,      125,    125     }, CLOCK_VREG_LEVEL_NOMINAL,     },
  {   56000000, { HAL_CLK_SOURCE_GPLL0,          2,      7,      100,    100     }, CLOCK_VREG_LEVEL_NOMINAL,     },
  {   58982400, { HAL_CLK_SOURCE_GPLL0,          2,      1152,   15625,  15625   }, CLOCK_VREG_LEVEL_NOMINAL,     },
  {   60000000, { HAL_CLK_SOURCE_GPLL0,          2,      3,      40,     40      }, CLOCK_VREG_LEVEL_NOMINAL,     },
  {   64000000, { HAL_CLK_SOURCE_GPLL0,          2,      2,      25,     25      }, CLOCK_VREG_LEVEL_NOMINAL,     },
  { 0 }
};


/*
 * BYTE0 clock configurations
 */
const ClockMuxConfigType BYTE0ClockConfig[] =
{
  {   19200000, { HAL_CLK_SOURCE_XO,             2,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  125000000, { HAL_CLK_SOURCE_EXTERNAL,       2,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  187500000, { HAL_CLK_SOURCE_EXTERNAL,       2,      0,      0,      0       }, CLOCK_VREG_LEVEL_NOMINAL,     },
  { 0 }
};


/*
 * EMAC clock configurations
 */
const ClockMuxConfigType EMACClockConfig[] =
{
  {  125000000, { HAL_CLK_SOURCE_GPLL1,          8,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  250000000, { HAL_CLK_SOURCE_GPLL1,          4,      0,      0,      0       }, CLOCK_VREG_LEVEL_NOMINAL,     },
  { 0 }
};


/*
 * ESC0 clock configurations
 */
const ClockMuxConfigType ESC0ClockConfig[] =
{
  {   19200000, { HAL_CLK_SOURCE_XO,             2,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {   19200000, { HAL_CLK_SOURCE_EXTERNAL,       2,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  { 0 }
};


/*
 * GFX3D clock configurations
 */
const ClockMuxConfigType GFX3DClockConfig[] =
{
  {   19200000, { HAL_CLK_SOURCE_XO,             2,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {   50000000, { HAL_CLK_SOURCE_GPLL0,          32,     0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {   80000000, { HAL_CLK_SOURCE_GPLL0,          20,     0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  100000000, { HAL_CLK_SOURCE_GPLL0,          16,     0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  160000000, { HAL_CLK_SOURCE_GPLL0,          10,     0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  200000000, { HAL_CLK_SOURCE_GPLL0,          8,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  228571429, { HAL_CLK_SOURCE_GPLL0,          7,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  240000000, { HAL_CLK_SOURCE_GPLL6_AUX,      9,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  266666667, { HAL_CLK_SOURCE_GPLL0,          6,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  270000000, { HAL_CLK_SOURCE_GPLL6_AUX,      8,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  320000000, { HAL_CLK_SOURCE_GPLL0,          5,      0,      0,      0       }, CLOCK_VREG_LEVEL_NOMINAL,     },
  {  400000000, { HAL_CLK_SOURCE_GPLL0,          4,      0,      0,      0       }, CLOCK_VREG_LEVEL_NOMINAL,     },
  {  484800000, { HAL_CLK_SOURCE_GPLL3,          2,      0,      0,      0       }, CLOCK_VREG_LEVEL_NOMINAL,     { { 0 }                      }, &SourceFreqConfig_GPLL3[0] /* 484.8 MHz */ },
  {  523200000, { HAL_CLK_SOURCE_GPLL3,          2,      0,      0,      0       }, CLOCK_VREG_LEVEL_HIGH,        { { 0 }                      }, &SourceFreqConfig_GPLL3[1] /* 523.2 MHz */ },
  {  598000000, { HAL_CLK_SOURCE_GPLL3,          2,      0,      0,      0       }, CLOCK_VREG_LEVEL_HIGH,        { { 0 }                      }, &SourceFreqConfig_GPLL3[3] /* 598.0 MHz */ },
  { 0 }
};


/*
 * GP1 clock configurations
 */
const ClockMuxConfigType GP1ClockConfig[] =
{
  {   19200000, { HAL_CLK_SOURCE_XO,             2,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  100000000, { HAL_CLK_SOURCE_GPLL0,          16,     0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  200000000, { HAL_CLK_SOURCE_GPLL0,          8,      0,      0,      0       }, CLOCK_VREG_LEVEL_NOMINAL,     },
  { 0 }
};


/*
 * HDMIAPP clock configurations
 */
const ClockMuxConfigType HDMIAPPClockConfig[] =
{
  {   19200000, { HAL_CLK_SOURCE_XO,             2,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  { 0 }
};


/*
 * HDMIPCLK clock configurations
 */
const ClockMuxConfigType HDMIPCLKClockConfig[] =
{
  {   19200000, { HAL_CLK_SOURCE_XO,             2,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {   75000000, { HAL_CLK_SOURCE_EXTERNAL,       2,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  150000000, { HAL_CLK_SOURCE_EXTERNAL,       2,      0,      0,      0       }, CLOCK_VREG_LEVEL_NOMINAL,     },
  { 0 }
};


/*
 * MDP clock configurations
 */
const ClockMuxConfigType MDPClockConfig[] =
{
  {   50000000, { HAL_CLK_SOURCE_GPLL0,          32,     0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {   80000000, { HAL_CLK_SOURCE_GPLL0,          20,     0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  100000000, { HAL_CLK_SOURCE_GPLL0,          16,     0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  145454545, { HAL_CLK_SOURCE_GPLL0,          11,     0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  160000000, { HAL_CLK_SOURCE_GPLL0,          10,     0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  160000000, { HAL_CLK_SOURCE_EXTERNAL,       2,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  177777778, { HAL_CLK_SOURCE_GPLL0,          9,      0,      0,      0       }, CLOCK_VREG_LEVEL_NOMINAL,     },
  {  200000000, { HAL_CLK_SOURCE_GPLL0,          8,      0,      0,      0       }, CLOCK_VREG_LEVEL_NOMINAL,     },
  {  266666667, { HAL_CLK_SOURCE_GPLL0,          6,      0,      0,      0       }, CLOCK_VREG_LEVEL_NOMINAL,     },
  {  266666667, { HAL_CLK_SOURCE_EXTERNAL,       2,      0,      0,      0       }, CLOCK_VREG_LEVEL_NOMINAL,     },
  {  320000000, { HAL_CLK_SOURCE_GPLL0,          5,      0,      0,      0       }, CLOCK_VREG_LEVEL_HIGH,        },
  {  320000000, { HAL_CLK_SOURCE_EXTERNAL,       2,      0,      0,      0       }, CLOCK_VREG_LEVEL_HIGH,        },
  { 0 }
};


/*
 * PCIE0AUX clock configurations
 */
const ClockMuxConfigType PCIE0AUXClockConfig[] =
{
  {    1200000, { HAL_CLK_SOURCE_XO,             32,     0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  { 0 }
};


/*
 * PCIE0PIPE clock configurations
 */
const ClockMuxConfigType PCIE0PIPEClockConfig[] =
{
  {   19200000, { HAL_CLK_SOURCE_XO,             2,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  125000000, { HAL_CLK_SOURCE_EXTERNAL,       2,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  250000000, { HAL_CLK_SOURCE_EXTERNAL,       2,      0,      0,      0       }, CLOCK_VREG_LEVEL_NOMINAL,     },
  { 0 }
};


/*
 * PCLK0 clock configurations
 */
const ClockMuxConfigType PCLK0ClockConfig[] =
{
  {   19200000, { HAL_CLK_SOURCE_XO,             2,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  166666667, { HAL_CLK_SOURCE_EXTERNAL,       2,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  250000000, { HAL_CLK_SOURCE_EXTERNAL,       2,      0,      0,      0       }, CLOCK_VREG_LEVEL_NOMINAL,     },
  { 0 }
};


/*
 * PDM2 clock configurations
 */
const ClockMuxConfigType PDM2ClockConfig[] =
{
  {   19200000, { HAL_CLK_SOURCE_XO,             2,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {   64000000, { HAL_CLK_SOURCE_GPLL0,          25,     0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  { 0 }
};


/*
 * SDCC1APPS clock configurations
 */
const ClockMuxConfigType SDCC1APPSClockConfig[] =
{
  {     144000, { HAL_CLK_SOURCE_XO,             32,     3,      25,     25      }, CLOCK_VREG_LEVEL_LOW,         },
  {     400000, { HAL_CLK_SOURCE_XO,             24,     1,      4,      4       }, CLOCK_VREG_LEVEL_LOW,         },
  {   20000000, { HAL_CLK_SOURCE_GPLL0,          20,     1,      4,      4       }, CLOCK_VREG_LEVEL_LOW,         },
  {   25000000, { HAL_CLK_SOURCE_GPLL0,          32,     1,      2,      2       }, CLOCK_VREG_LEVEL_LOW,         },
  {   50000000, { HAL_CLK_SOURCE_GPLL0,          32,     0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  100000000, { HAL_CLK_SOURCE_GPLL0,          16,     0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  177777778, { HAL_CLK_SOURCE_GPLL0,          9,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  192000000, { HAL_CLK_SOURCE_GPLL4,          12,     0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  200000000, { HAL_CLK_SOURCE_GPLL0,          8,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  384000000, { HAL_CLK_SOURCE_GPLL4,          6,      0,      0,      0       }, CLOCK_VREG_LEVEL_NOMINAL,     },
  { 0 }
};


/*
 * SDCC1ICECORE clock configurations
 */
const ClockMuxConfigType SDCC1ICECOREClockConfig[] =
{
  {  160000000, { HAL_CLK_SOURCE_GPLL0,          10,     0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  266666667, { HAL_CLK_SOURCE_GPLL0,          6,      0,      0,      0       }, CLOCK_VREG_LEVEL_NOMINAL,     },
  { 0 }
};


/*
 * SDCC2APPS clock configurations
 */
const ClockMuxConfigType SDCC2APPSClockConfig[] =
{
  {     144000, { HAL_CLK_SOURCE_XO,             32,     3,      25,     25      }, CLOCK_VREG_LEVEL_LOW,         },
  {     400000, { HAL_CLK_SOURCE_XO,             24,     1,      4,      4       }, CLOCK_VREG_LEVEL_LOW,         },
  {   20000000, { HAL_CLK_SOURCE_GPLL0,          20,     1,      4,      4       }, CLOCK_VREG_LEVEL_LOW,         },
  {   25000000, { HAL_CLK_SOURCE_GPLL0,          32,     1,      2,      2       }, CLOCK_VREG_LEVEL_LOW,         },
  {   50000000, { HAL_CLK_SOURCE_GPLL0,          32,     0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  100000000, { HAL_CLK_SOURCE_GPLL0,          16,     0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  177777778, { HAL_CLK_SOURCE_GPLL0,          9,      0,      0,      0       }, CLOCK_VREG_LEVEL_NOMINAL,     },
  {  200000000, { HAL_CLK_SOURCE_GPLL0,          8,      0,      0,      0       }, CLOCK_VREG_LEVEL_NOMINAL,     },
  { 0 }
};


/*
 * USB20MOCKUTMI clock configurations
 */
const ClockMuxConfigType USB20MOCKUTMIClockConfig[] =
{
  {   19200000, { HAL_CLK_SOURCE_XO,             2,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  { 0 }
};

/*
 * USB3PHYPIPE clock configurations
 */
const ClockMuxConfigType USB3PHYPIPEClockConfig[] =
{
  {   19200000, { HAL_CLK_SOURCE_XO,             2,     0,      0,      0       }, CLOCK_VREG_LEVEL_LOW_MINUS, },
  {   62500000, { HAL_CLK_SOURCE_EXTERNAL,       2,     0,      0,      0       }, CLOCK_VREG_LEVEL_LOW_MINUS, },
  {  125000000, { HAL_CLK_SOURCE_EXTERNAL,       2,     0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,       },
  { 0 }
};


/*
 * USB30MASTER clock configurations
 */
const ClockMuxConfigType USB30MASTERClockConfig[] =
{
  {   19200000, { HAL_CLK_SOURCE_XO,             2,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  100000000, { HAL_CLK_SOURCE_GPLL0,          16,     0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  200000000, { HAL_CLK_SOURCE_GPLL0,          8,      0,      0,      0       }, CLOCK_VREG_LEVEL_NOMINAL,     },
  {  266666667, { HAL_CLK_SOURCE_GPLL0,          6,      0,      0,      0       }, CLOCK_VREG_LEVEL_HIGH,        },
  { 0 }
};


/*
 * USB3PHYAUX clock configurations
 */
const ClockMuxConfigType USB3PHYAUXClockConfig[] =
{
  {    1200000, { HAL_CLK_SOURCE_XO,             32,     0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  { 0 }
};


/*
 * USBHSSYSTEM clock configurations
 */
const ClockMuxConfigType USBHSSYSTEMClockConfig[] =
{
  {   19200000, { HAL_CLK_SOURCE_XO,             2,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {   80000000, { HAL_CLK_SOURCE_GPLL0,          20,     0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
  {  100000000, { HAL_CLK_SOURCE_GPLL0,          16,     0,      0,      0       }, CLOCK_VREG_LEVEL_NOMINAL,     },
  {  133333333, { HAL_CLK_SOURCE_GPLL0,          12,     0,      0,      0       }, CLOCK_VREG_LEVEL_NOMINAL,     },
  {  177777778, { HAL_CLK_SOURCE_GPLL0,          9,      0,      0,      0       }, CLOCK_VREG_LEVEL_HIGH,        },
  { 0 }
};


/*
 * VSYNC clock configurations
 */
const ClockMuxConfigType VSYNCClockConfig[] =
{
  {   19200000, { HAL_CLK_SOURCE_XO,             2,      0,      0,      0       }, CLOCK_VREG_LEVEL_LOW,         },
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
    /* .nLogSize        = */ 4096,
    /* .nGlobalLogFlags = */ 0x90ff
  }
};


/*
 * Clock Flag Init Config.
 */
const ClockFlagInitType ClockFlagInitConfig[] =
{
  {
    CLOCK_FLAG_NODE_TYPE_SOURCE,
    (void *)"GPLL3",
    CLOCK_FLAG_SUPPORTS_SLEWING
  },
  {
    CLOCK_FLAG_NODE_TYPE_SOURCE,
    (void *)"SLEEPCLK",
    CLOCK_FLAG_SOURCE_NOT_CONFIGURABLE
  },
  {
    CLOCK_FLAG_NODE_TYPE_SOURCE,
    (void *)"XO",
    CLOCK_FLAG_SOURCE_NOT_CONFIGURABLE
  },
  {
    CLOCK_FLAG_NODE_TYPE_SOURCE,
    (void *)"GPLL6_AUX",
    CLOCK_FLAG_SOURCE_NOT_CONFIGURABLE
  },
  {
    CLOCK_FLAG_NODE_TYPE_SOURCE,
    (void *)"GPLL0",
    CLOCK_FLAG_SOURCE_NOT_CONFIGURABLE
  },
  {
    CLOCK_FLAG_NODE_TYPE_CLOCK_DOMAIN,
    (void *)"apcs_cluster1_clk",
    CLOCK_FLAG_SUPPRESSIBLE
  },
  {
    CLOCK_FLAG_NODE_TYPE_NONE,
    (void *)0,
    0
  }
};


const ClockAVSCoreConfigType ClockAVSData[] =
{
  {
    /* .nPhysicalBase    = */ 0x0B012000,             /* Base address: 0x0B012000 - 0x1000 */
    /* .eCore            = */ HAL_AVS_CORE_APC1_L2,   /* Cluster1 L2: APCS_CLUS1_L2_SAW2 */
    /* .ePMIC            = */ HAL_AVS_PMQcs405_SUPPORT, //check
    /* .eStepSize        = */ HAL_AVS_STEP_SIZE_25MV, //check
    /* .nVddRequestDelay = */ 300, //check
    /* .nClockDiv        = */ 1, //check
    /* .HWVersion        = */ { {0, 0}, {0xFF, 0xFF}, DALCHIPINFO_FAMILY_UNKNOWN, NULL },
  },

  { 0 }
};

/*
 * List of clocks allowed to be suppressible.
 */
ClockNameListType ClockSuppressibleList[] =
{
  // Serial Debugger:
  { "gcc_blsp1_uart1_apps_clk"   },  
  { "gcc_blsp1_ahb_clk"          },

  // USB 2.0 Debugger:
  { "gcc_usb_hs_system_clk"      },
  { "gcc_usb20_mock_utmi_clk"    },
  { "gcc_usb_hs_phy_cfg_ahb_clk" },
  { "gcc_usb2a_phy_sleep_clk"    },
  //{ "gcc_usb_hs_inactivity_timers_clk"    },
  //{ "gcc_pcnoc_usb2_clk"        },

  

  // USB 3.0 Debugger:
  { "gcc_usb30_master_clk"       },
  { "gcc_usb30_mock_utmi_clk"    },
  { "gcc_sys_noc_usb3_clk"       },
  { "gcc_usb30_sleep_clk"        },
  { "gcc_usb30_phy_pipe_clk"      },
  { "gcc_usb3_phy_aux_clk"       },
  //{ "gcc_usb3_tbu_axi_clk"       },
  //{ "gcc_pcnoc_usb3_clk"   },
  // Check this and below lists with Pranav
  { NULL }
};

/*
 * List of clocks allowed to be always on.
 */
ClockNameListType ClockAlwaysOnList[] =
{
  { "gcc_usb2a_phy_sleep_clk"        },
  { "gcc_usb30_sleep_clk"        },
  { "gcc_usb3_phy_aux_clk"       },
  { "gcc_usb_hs_phy_cfg_ahb_clk"       },
  
  { NULL }
};

/*
 * List of clocks allowed to be reference counter disabled.
 */
ClockNameListType ClockRefCountSuspendedList[] =
{
  // USB 2.0
  { "gcc_usb20_mock_utmi_clk"    },

  // USB 3.0
  { "gcc_usb30_master_clk"       },
  { "gcc_usb30_mock_utmi_clk"    },
  { "gcc_sys_noc_usb3_axi_clk"   },
  { "gcc_usb3_phy_aux_clk"       },

  { NULL }
};

/*
 * List of clocks allowed to be configured with the
 * FORCE_MEM_CORE and FORCE_MEM_PERIPH parameters.
 */
ClockNameListType ClockForceMemCoreAndPeriphList[] =
{
  { NULL }
};

/*
 * List of clocks anticipated to fail the BIST.
 */
ClockNameListType ClockBistExceptions[] =
{
  {"gcc_usb30_phy_pipe_clk"},
  {"gcc_bias_pll_bcr"},
  {"gcc_mmss_bcr"},
  { NULL }
};

/*
 * List of clocks excluded from the BIST.
 */
ClockNameListType ClockExcludeFromBist[] =
{
  { NULL }
};
