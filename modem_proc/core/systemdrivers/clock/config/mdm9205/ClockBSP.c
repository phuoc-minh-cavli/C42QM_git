/*
==============================================================================

FILE:         ClockBSP.c

DESCRIPTION:
  This file contains clock BSP data for the DAL based driver.


==============================================================================

                             Edit History

$Header: //components/rel/core.mpss/3.10/systemdrivers/clock/config/mdm9205/ClockBSP.c#6 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------- 
08/22/19   shm     Added WAIT_FOR_ROOT_OFF for I2S clocks
02/18/19   shm     Marking clk_bus_nc_hm_bridge as suppressible
04/12/18   shm     Clock driver changes
09/10/18   shm     Initial version for 9205

==============================================================================
  Copyright (c) 2019 Qualcomm Technologies Incorporated.
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
    .nFreqHz    = 19200 * 1000,
    .HALConfig  = { HAL_CLK_SOURCE_NULL },
    .eVRegLevel = VCS_CORNER_LOW,
    .HWVersion  = { {0, 0}, {0xFF, 0xFF} },
  },
  /* last entry */
  { 0 }
};

/*
 *  SourceFreqConfig_GPLL0
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
 *  SourceFreqConfig_GPLL1
 *
 * Used by:
 *   GPLL1
 */
static ClockSourceFreqConfigType SourceFreqConfig_GPLL1[] =
{
  {
    /* .nFreqHz    = */  614400 * 1000,
    /* .HALConfig  = */
    {
      /* .eSource        = */  HAL_CLK_SOURCE_XO,
      /* .eVCO           = */  HAL_CLK_PLL_VCO1,
      /* .nPreDiv        = */  1,
      /* .nPostDiv       = */  1,
      /* .nL             = */  32,
      /* .nM             = */  0,
      /* .nN             = */  1,
      /* .nVCOMultiplier = */  0,
      /* .nAlpha         = */  0x00000000,
      /* .nAlphaU        = */  0x00,
    },
    /* .eVRegLevel = */  VCS_CORNER_LOW,
    /* .HWVersion = */   { {0x00, 0x00}, {0xFF, 0xFF} },
  },
  /* last entry */
  { 0 }
};

/*
 *  SourceFreqConfig_MPLL1
 *
 * Used by:
 *   MPLL1
 */
static ClockSourceFreqConfigType SourceFreqConfig_MPLL1[] =
{
  {
    /* .nFreqHz    = */  460800 * 1000,
    /* .HALConfig  = */
    {
      /* .eSource        = */  HAL_CLK_SOURCE_XO,
      /* .eVCO           = */  HAL_CLK_PLL_VCO1,
      /* .nPreDiv        = */  1,
      /* .nPostDiv       = */  2,
      /* .nL             = */  48,
      /* .nM             = */  0,
      /* .nN             = */  0,
      /* .nVCOMultiplier = */  4, /* Quadrupler mode for SR2 PLL */
      /* .nAlpha         = */  0x00000000,
      /* .nAlphaU        = */  0x00,
    },
    /* .eVRegLevel = */  VCS_CORNER_LOW,
    /* .HWVersion = */   { {0x00, 0xFF}, {0xFF, 0xFF} },
  },
  /* last entry */
  { 0 }
};

/*
 *  SourceFreqConfig_MPLL2
 *
 * Used by:
 *   MPLL2
 */
ClockSourceFreqConfigType SourceFreqConfig_MPLL2[] =
{
  {
    /* .nFreqHz    = */  480000 * 1000,
    /* .HALConfig  = */
    {
      /* .eSource        = */  HAL_CLK_SOURCE_XO,
      /* .eVCO           = */  HAL_CLK_PLL_VCO3,
      /* .nPreDiv        = */  1,
      /* .nPostDiv       = */  1,
      /* .nL             = */  25,
      /* .nM             = */  0,
      /* .nN             = */  0,
      /* .nVCOMultiplier = */  0,
      /* .nAlpha         = */  0x00000000,
      /* .nAlphaU        = */  0x00,
    },
    /* .eVRegLevel = */  VCS_CORNER_LOW,
    /* .HWVersion = */   { {0x00, 0x00}, {0xFF, 0xFF} },
  },
  {
    /* .nFreqHz    = */  576000 * 1000,
    /* .HALConfig  = */
    {
      /* .eSource        = */  HAL_CLK_SOURCE_XO,
      /* .eVCO           = */  HAL_CLK_PLL_VCO3,
      /* .nPreDiv        = */  1,
      /* .nPostDiv       = */  1,
      /* .nL             = */  30,
      /* .nM             = */  0,
      /* .nN             = */  0,
      /* .nVCOMultiplier = */  0,
      /* .nAlpha         = */  0x00000000,
      /* .nAlphaU        = */  0x00,
    },
    /* .eVRegLevel = */  VCS_CORNER_LOW,
    /* .HWVersion = */   { {0x00, 0x00}, {0xFF, 0xFF} },
  },
  {
    /* .nFreqHz    = */  652800 * 1000,
    /* .HALConfig  = */
    {
      /* .eSource        = */  HAL_CLK_SOURCE_XO,
      /* .eVCO           = */  HAL_CLK_PLL_VCO3,
      /* .nPreDiv        = */  1,
      /* .nPostDiv       = */  1,
      /* .nL             = */  34,
      /* .nM             = */  0,
      /* .nN             = */  0,
      /* .nVCOMultiplier = */  0,
      /* .nAlpha         = */  0x00000000,
      /* .nAlphaU        = */  0x00,
    },
    /* .eVRegLevel = */  VCS_CORNER_LOW,
    /* .HWVersion = */   { {0x00, 0xFF}, {0xFF, 0xFF} },
  },
  /* last entry */
  { 0 }
};

/*
 *  SourceCalibrationFreqConfig_MPLL2
 *
 * Used by:
 *   MPLL2
 */
ClockSourceFreqConfigType SourceCalibrationFreqConfig_MPLL2 =
{
  .nFreqHz    = 537600 * 1000,
  .HALConfig  =
  {
    .eSource        = HAL_CLK_SOURCE_XO,
    .eVCO           = HAL_CLK_PLL_VCO3,
    .nPreDiv        = 1,
    .nPostDiv       = 1,
    .nL             = 28,
    .nM             = 0,
    .nN             = 0,
    .nVCOMultiplier = 0,
    .nAlpha         = 0,
    .nAlphaU        = 0,
  },
  .eVRegLevel = VCS_CORNER_LOW,
  .HWVersion  = { {0x00, 0x00}, {0xFF, 0xFF} },
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

    /* .nConfigMask            = */  0,
    /* .pSourceFreqConfig      = */  SourceFreqConfig_XO,
  },
  {
    SOURCE_NAME(GPLL0),

    /* .nConfigMask            = */  CLOCK_CONFIG_PLL_FSM_MODE_ENABLE,
    /* .pSourceFreqConfig      = */  SourceFreqConfig_GPLL0,
  },
  {
    SOURCE_NAME(GPLL1),

    /* .nConfigMask            = */  CLOCK_CONFIG_PLL_FSM_MODE_ENABLE |
                                     CLOCK_CONFIG_PLL_AUX_OUTPUT_ENABLE,
    /* .pSourceFreqConfig      = */  SourceFreqConfig_GPLL1,
  },
  {
    SOURCE_NAME(MPLL1),

    /* .nConfigMask            = */  CLOCK_CONFIG_PLL_EARLY_OUTPUT_ENABLE |
                                     CLOCK_CONFIG_PLL_AUX_OUTPUT_ENABLE,
    /* .pSourceFreqConfig      = */  SourceFreqConfig_MPLL1,
  },
  {
    SOURCE_NAME(MPLL2),
     /* TODO: Confirm with Q6 team on which outputs are used */
    /* .nConfigMask            = */  CLOCK_CONFIG_PLL_EARLY_OUTPUT_ENABLE |
                                     CLOCK_CONFIG_PLL_AUX2_OUTPUT_ENABLE,
    /* .pSourceFreqConfig      = */  SourceFreqConfig_MPLL2,
    /* .pCalibrationFreqConfig = */  &SourceCalibrationFreqConfig_MPLL2,
  },
  /* last entry */
  { SOURCE_NAME(NULL) }
};


/*----------------------------------------------------------------------*/
/* GCC clock configurations                                             */
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
 * BLSPUARTSIM clock configurations
 *
 * Used by:
 *   BLSPUARTSIM
 */
const ClockMuxConfigType BLSPUARTSIMClockConfig[] =
{
  {    3840000, { HAL_CLK_SOURCE_XO,             10,    0,     0,     0      }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  { 0 }
};


/*
 * GCCULTAUDIOEXTMCLK2 clock configurations
 *
 * Used by:
 *   GCCULTAUDIOEXTMCLK2
 */
const ClockMuxConfigType GCCULTAUDIOEXTMCLK2ClockConfig[] =
{
  {    9600000, { HAL_CLK_SOURCE_XO,             4,     0,     0,     0      }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {   12288000, { HAL_CLK_SOURCE_GPLL1,          4,     1,     25,    25     }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL1_OUT_MAIN */
  { 0 }
};


/*
 * GCCULTAUDIOLPAIFAUXI2S clock configurations
 *
 * Used by:
 *   GCCULTAUDIOLPAIFAUXI2S
 *   GCCULTAUDIOLPAIFPRII2S
 *   GCCULTAUDIOLPAIFSECI2S
 */
const ClockMuxConfigType GCCULTAUDIOLPAIFAUXI2SClockConfig[] =
{
  {     256000, { HAL_CLK_SOURCE_GPLL1,          32,    1,     150,   150    }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL1_OUT_MAIN */
  {     512000, { HAL_CLK_SOURCE_GPLL1,          32,    1,     75,    75     }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL1_OUT_MAIN */
  {     768000, { HAL_CLK_SOURCE_GPLL1,          32,    1,     50,    50     }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL1_OUT_MAIN */
  {    1024000, { HAL_CLK_SOURCE_GPLL1,          32,    2,     75,    75     }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL1_OUT_MAIN */
  {    1536000, { HAL_CLK_SOURCE_GPLL1,          32,    1,     25,    25     }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL1_OUT_MAIN */
  {    2048000, { HAL_CLK_SOURCE_GPLL1,          16,    2,     75,    75     }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL1_OUT_MAIN */
  {    2304000, { HAL_CLK_SOURCE_GPLL1,          16,    3,     100,   100    }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL1_OUT_MAIN */
  {    2400000, { HAL_CLK_SOURCE_XO,             16,    0,     0,     0      }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    3072000, { HAL_CLK_SOURCE_GPLL1,          16,    1,     25,    25     }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL1_OUT_MAIN */
  {    4096000, { HAL_CLK_SOURCE_GPLL1,          8,     2,     75,    75     }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL1_OUT_MAIN */
  {    6144000, { HAL_CLK_SOURCE_GPLL1,          8,     1,     25,    25     }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL1_OUT_MAIN */
  {    8192000, { HAL_CLK_SOURCE_GPLL1,          4,     2,     75,    75     }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL1_OUT_MAIN */
  {   12288000, { HAL_CLK_SOURCE_GPLL1,          4,     1,     25,    25     }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL1_OUT_MAIN */
  { 0 }
};


/*
 * GCCULTAUDIOPCMDATAOE clock configurations
 *
 * Used by:
 *   GCCULTAUDIOPCMDATAOE
 *   GCCULTAUDIOSECPCMDATAOE
 *   GCCULTAUDIOTERPCMDATAOE
 */
const ClockMuxConfigType GCCULTAUDIOPCMDATAOEClockConfig[] =
{
  {    9600000, { HAL_CLK_SOURCE_XO,             4,     0,     0,     0      }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {   12288000, { HAL_CLK_SOURCE_GPLL1,          2,     1,     50,    50     }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL1_OUT_MAIN */
  {   19200000, { HAL_CLK_SOURCE_XO,             2,     0,     0,     0      }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {   24576000, { HAL_CLK_SOURCE_GPLL1,          2,     1,     25,    25     }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL1_OUT_MAIN */
  {   72282000, { HAL_CLK_SOURCE_GPLL1,          17,    0,     0,     0      }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL1_OUT_MAIN */
  { 0 }
};


/*
 * GCCULTAUDIOXO clock configurations
 *
 * Used by:
 *   GCCULTAUDIOXO
 */
const ClockMuxConfigType GCCULTAUDIOXOClockConfig[] =
{
  {   19200000, { HAL_CLK_SOURCE_XO,             2,     0,     0,     0      }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  { 0 }
};


/*
 * SECCTRL clock configurations
 *
 * Used by:
 *   SECCTRL
 */
const ClockMuxConfigType SECCTRLClockConfig[] =
{
  {    2400000, { HAL_CLK_SOURCE_XO,             16,    0,     0,     0      }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {   19200000, { HAL_CLK_SOURCE_XO,             2,     0,     0,     0      }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  { 0 }
};


/*
 * ULTAUDIOAHBFABRIC clock configurations
 *
 * Used by:
 *   ULTAUDIOAHBFABRIC
 */
const ClockMuxConfigType ULTAUDIOAHBFABRICClockConfig[] =
{
  {    3200000, { HAL_CLK_SOURCE_XO,             12,    0,     0,     0      }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    6400000, { HAL_CLK_SOURCE_XO,             6,     0,     0,     0      }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    9600000, { HAL_CLK_SOURCE_XO,             4,     0,     0,     0      }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {   19200000, { HAL_CLK_SOURCE_XO,             2,     0,     0,     0      }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {   40000000, { HAL_CLK_SOURCE_GPLL0,          20,    1,     2,     2      }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  {   50000000, { HAL_CLK_SOURCE_GPLL0,          32,    0,     0,     0      }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  {   66670000, { HAL_CLK_SOURCE_GPLL0,          24,    0,     0,     0      }, VCS_CORNER_NOMINAL,             },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  {   80000000, { HAL_CLK_SOURCE_GPLL0,          20,    0,     0,     0      }, VCS_CORNER_NOMINAL,             },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  {  100000000, { HAL_CLK_SOURCE_GPLL0,          16,    0,     0,     0      }, VCS_CORNER_NOMINAL,             },                                                    /* Mux[1] = GPLL0_OUT_MAIN */
  { 0 }
};


/*----------------------------------------------------------------------*/
/* MSSCC clock configurations                                           */
/*----------------------------------------------------------------------*/

/*
 * COXMBIT clock configurations
 *
 * Used by:
 *   COXMBIT
 */
const ClockMuxConfigType COXMBITClockConfig[] =
{
  {   46080000, { HAL_CLK_SOURCE_MPLL1,          20,    0,     0,     0      }, VCS_CORNER_LOW,                 },                                                    /* Mux[1] = MPLL1_OUT_MAIN */
  { 0 }
};

/*
 * UIMSIM clock configurations.
 *
 * Used by:
 *   UIMSIM
 */
const ClockMuxConfigType  UIMSIMClockConfig[] =
{
  { 3840000, { HAL_CLK_SOURCE_XO,  10,  0,  0,  0 }, VCS_CORNER_LOW },
  { 4800000, { HAL_CLK_SOURCE_XO,   8,  0,  0,  0 }, VCS_CORNER_LOW },
  { 0 }
};

/*
 * XOMNDUIM0 clock configurations
 *
 * Used by:
 *   XOMNDUIM0
 */
const ClockMuxConfigType XOMNDUIM0ClockConfig[] =
{
  {    4807680, { HAL_CLK_SOURCE_XO,             2,     313,   1250,  1250   }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    4953600, { HAL_CLK_SOURCE_XO,             2,     129,   500,   500    }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    4961280, { HAL_CLK_SOURCE_XO,             2,     323,   1250,  1250   }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    5120000, { HAL_CLK_SOURCE_XO,             2,     4,     15,    15     }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    5283840, { HAL_CLK_SOURCE_XO,             2,     172,   625,   625    }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    5285120, { HAL_CLK_SOURCE_XO,             2,     4129,  15000, 15000  }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    5760000, { HAL_CLK_SOURCE_XO,             2,     3,     10,    10     }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    5775360, { HAL_CLK_SOURCE_XO,             2,     188,   625,   625    }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    6328320, { HAL_CLK_SOURCE_XO,             2,     206,   625,   625    }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    6343680, { HAL_CLK_SOURCE_XO,             2,     413,   1250,  1250   }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    6400000, { HAL_CLK_SOURCE_XO,             2,     1,     3,     3      }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    6604800, { HAL_CLK_SOURCE_XO,             2,     43,    125,   125    }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    6606080, { HAL_CLK_SOURCE_XO,             2,     5161,  15000, 15000  }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    6606400, { HAL_CLK_SOURCE_XO,             2,     4129,  12000, 12000  }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    6606720, { HAL_CLK_SOURCE_XO,             2,     3441,  10000, 10000  }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    6607360, { HAL_CLK_SOURCE_XO,             2,     2581,  7500,  7500   }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    7045120, { HAL_CLK_SOURCE_XO,             2,     688,   1875,  1875   }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    7046400, { HAL_CLK_SOURCE_XO,             2,     367,   1000,  1000   }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    7046880, { HAL_CLK_SOURCE_XO,             2,     14681, 40000, 40000  }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    7047040, { HAL_CLK_SOURCE_XO,             2,     11011, 30000, 30000  }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    7047680, { HAL_CLK_SOURCE_XO,             2,     2753,  7500,  7500   }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    7188480, { HAL_CLK_SOURCE_XO,             2,     234,   625,   625    }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    7200000, { HAL_CLK_SOURCE_XO,             2,     3,     8,     8      }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    7201280, { HAL_CLK_SOURCE_XO,             2,     2813,  7500,  7500   }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    7203840, { HAL_CLK_SOURCE_XO,             2,     469,   1250,  1250   }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    7432320, { HAL_CLK_SOURCE_XO,             2,     3871,  10000, 10000  }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    7680000, { HAL_CLK_SOURCE_XO,             2,     2,     5,     5      }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    7925760, { HAL_CLK_SOURCE_XO,             2,     258,   625,   625    }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    7927680, { HAL_CLK_SOURCE_XO,             2,     4129,  10000, 10000  }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    7928320, { HAL_CLK_SOURCE_XO,             2,     3097,  7500,  7500   }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    7929600, { HAL_CLK_SOURCE_XO,             2,     413,   1000,  1000   }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    8000000, { HAL_CLK_SOURCE_XO,             2,     5,     12,    12     }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    8257920, { HAL_CLK_SOURCE_XO,             2,     4301,  10000, 10000  }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    8258560, { HAL_CLK_SOURCE_XO,             2,     1613,  3750,  3750   }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    8455680, { HAL_CLK_SOURCE_XO,             2,     1101,  2500,  2500   }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    8456320, { HAL_CLK_SOURCE_XO,             2,     13213, 30000, 30000  }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    8458240, { HAL_CLK_SOURCE_XO,             2,     826,   1875,  1875   }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    8640000, { HAL_CLK_SOURCE_XO,             2,     9,     20,    20     }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    8806400, { HAL_CLK_SOURCE_XO,             2,     172,   375,   375    }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    8808320, { HAL_CLK_SOURCE_XO,             2,     13763, 30000, 30000  }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    8808640, { HAL_CLK_SOURCE_XO,             2,     27527, 60000, 60000  }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    8808960, { HAL_CLK_SOURCE_XO,             2,     1147,  2500,  2500   }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    9000000, { HAL_CLK_SOURCE_XO,             2,     15,    32,    32     }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    9000960, { HAL_CLK_SOURCE_XO,             2,     293,   625,   625    }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    9511680, { HAL_CLK_SOURCE_XO,             2,     2477,  5000,  5000   }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {    9600000, { HAL_CLK_SOURCE_XO,             2,     1,     2,     2      }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  {   19200000, { HAL_CLK_SOURCE_XO,             2,     0,     0,     0      }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
  { 0 }
};

/*
 * LPASS CXO clock configurations.
 */
const ClockMuxConfigType AudioCXOClockConfig[] =
{
  {   19200000, { HAL_CLK_SOURCE_XO,             2,     0,     0,     0      }, VCS_CORNER_LOW,                 },                                                    /* Mux[0] = XO */
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
    /* .nGlobalLogFlags = */ 0x12
  }
};


/*
 * Clock Flag Init Config.
 */
const ClockFlagInitType ClockFlagInitConfig[] =
{
  {
    /* .eNodeType = */ CLOCK_FLAG_NODE_TYPE_CLOCK_DOMAIN,
    /* .pID       = */ (void *)"clk_q6",
    /* .nFlags    = */ CLOCK_FLAG_SUPPRESSIBLE
  },
  {
    /* .eNodeType = */ CLOCK_FLAG_NODE_TYPE_CLOCK,
    /* .pID       = */ (void *)"clk_bus_slave_timeout",
    /* .nFlags    = */ CLOCK_FLAG_SUPPRESSIBLE
  },
  {
    /* .eNodeType = */ CLOCK_FLAG_NODE_TYPE_CLOCK,
    /* .pID       = */ (void *)"clk_mss_config_bus",
    /* .nFlags    = */ CLOCK_FLAG_SUPPRESSIBLE
  },
  {
    /* .eNodeType = */ CLOCK_FLAG_NODE_TYPE_CLOCK,
    /* .pID       = */ (void *)"clk_bus_uim0",
    /* .nFlags    = */ CLOCK_FLAG_SUPPRESSIBLE
  },
  {
    /* .eNodeType = */ CLOCK_FLAG_NODE_TYPE_CLOCK,
    /* .pID       = */ (void *)"clk_bus_nc_hm_bridge",
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
    /* .eNodeType = */ CLOCK_FLAG_NODE_TYPE_CLOCK,
    /* .pID       = */ (void *)"gcc_ultaudio_ahbfabric_efabric_spdm_clk",
    /* .nFlags    = */ CLOCK_FLAG_SUPPRESSIBLE
  },
  {
    /* .eNodeType = */ CLOCK_FLAG_NODE_TYPE_CLOCK,
    /* .pID       = */ (void *)"gcc_ultaudio_ahbfabric_ixfabric_clk",
    /* .nFlags    = */ CLOCK_FLAG_SUPPRESSIBLE
  },
  {
    /* .eNodeType = */ CLOCK_FLAG_NODE_TYPE_CLOCK,
    /* .pID       = */ (void *)"gcc_ultaudio_ahbfabric_ixfabric_lpm_clk",
    /* .nFlags    = */ CLOCK_FLAG_SUPPRESSIBLE
  },
  {
    /* .eNodeType = */ CLOCK_FLAG_NODE_TYPE_CLOCK,
    /* .pID       = */ (void *)"gcc_ultaudio_pcnoc_mport_clk",
    /* .nFlags    = */ CLOCK_FLAG_SUPPRESSIBLE
  },
  {
    /* .eNodeType = */ CLOCK_FLAG_NODE_TYPE_CLOCK,
    /* .pID       = */ (void *)"gcc_ultaudio_pcnoc_sway_clk",
    /* .nFlags    = */ CLOCK_FLAG_SUPPRESSIBLE
  },
  {
    /* .eNodeType = */ CLOCK_FLAG_NODE_TYPE_SOURCE,
    /* .pID       = */ (void *)"MPLL1",
    /* .nFlags    = */ CLOCK_FLAG_SUPPRESSIBLE
  },
  {
    /* .eNodeType = */ CLOCK_FLAG_NODE_TYPE_CLOCK_DOMAIN,
    /* .pID       = */ (void *)"gcc_ultaudio_lpaif_pri_i2s_clk",
    /* .nFlags    = */ CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF
  },
  {
    /* .eNodeType = */ CLOCK_FLAG_NODE_TYPE_CLOCK_DOMAIN,
    /* .pID       = */ (void *)"gcc_ultaudio_lpaif_sec_i2s_clk",
    /* .nFlags    = */ CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF
  },
  {
    /* .eNodeType = */ CLOCK_FLAG_NODE_TYPE_CLOCK_DOMAIN,
    /* .pID       = */ (void *)"gcc_ultaudio_lpaif_aux_i2s_clk",
    /* .nFlags    = */ CLOCK_FLAG_DOMAIN_WAIT_FOR_ROOT_OFF
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
  .bRUMI   = FALSE,
};
#endif
