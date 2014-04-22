/*
==============================================================================

FILE:         HALclkMDSS.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   MDSS clocks.

   List of clock domains:
     - HAL_clk_mGCCBYTE0ClkDomain
     - HAL_clk_mGCCESC0ClkDomain
     - HAL_clk_mGCCHDMIAPPClkDomain
     - HAL_clk_mGCCHDMIPCLKClkDomain
     - HAL_clk_mGCCMDPClkDomain
     - HAL_clk_mGCCMDSSBIMCClkDomain
     - HAL_clk_mGCCPCLK0ClkDomain
     - HAL_clk_mGCCVSYNCClkDomain


   List of power domains:
     - HAL_clk_mGCCMDSSPowerDomain



==============================================================================

$Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Library/HALclkLib/HALclkMDSS.c#1 $

==============================================================================
            Copyright (c) 2018 QUALCOMM Technologies Incorporated.
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

extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControl;
extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControlRO;
extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControl_10;
extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControlRO_10;
extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControl_2;
extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControlRO_2;
extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControl_4;
extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControlRO_4;


/* ============================================================================
**    Data
** ==========================================================================*/


/*                           
 *  HAL_clk_mBYTE0ClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mBYTE0ClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_mdss_byte0_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_MDSS_BYTE0_CBCR), HWIO_OFFS(GCC_MDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_MDSS_BYTE0_CLK
  },
};


/*
 * HAL_clk_mGCCBYTE0ClkDomain
 *
 * BYTE0 clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCBYTE0ClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_BYTE0_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mBYTE0ClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mBYTE0ClkDomainClks)/sizeof(HAL_clk_mBYTE0ClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl_2,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mESC0ClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mESC0ClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_mdss_esc0_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_MDSS_ESC0_CBCR), HWIO_OFFS(GCC_MDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_MDSS_ESC0_CLK
  },
};


/*
 * HAL_clk_mGCCESC0ClkDomain
 *
 * ESC0 clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCESC0ClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_ESC0_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mESC0ClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mESC0ClkDomainClks)/sizeof(HAL_clk_mESC0ClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl_4,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mHDMIAPPClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mHDMIAPPClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_mdss_hdmi_app_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_MDSS_HDMI_APP_CBCR), HWIO_OFFS(GCC_MDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_MDSS_HDMI_APP_CLK
  },
};


/*
 * HAL_clk_mGCCHDMIAPPClkDomain
 *
 * HDMIAPP clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCHDMIAPPClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_HDMI_APP_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mHDMIAPPClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mHDMIAPPClkDomainClks)/sizeof(HAL_clk_mHDMIAPPClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mHDMIPCLKClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mHDMIPCLKClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_mdss_hdmi_pclk_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_MDSS_HDMI_PCLK_CBCR), HWIO_OFFS(GCC_MDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_MDSS_HDMI_PCLK_CLK
  },
};


/*
 * HAL_clk_mGCCHDMIPCLKClkDomain
 *
 * HDMIPCLK clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCHDMIPCLKClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_HDMI_PCLK_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mHDMIPCLKClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mHDMIPCLKClkDomainClks)/sizeof(HAL_clk_mHDMIPCLKClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl_2,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mMDPClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mMDPClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_mdss_mdp_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_MDSS_MDP_CBCR), HWIO_OFFS(GCC_MDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_MDSS_MDP_CLK
  },
#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_spdm_mdp_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SPDM_MDP_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SPDM_MDP_CLK
  },
#endif

};


/*
 * HAL_clk_mGCCMDPClkDomain
 *
 * MDP clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCMDPClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_MDP_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mMDPClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mMDPClkDomainClks)/sizeof(HAL_clk_mMDPClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl_10,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mMDSSBIMCClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mMDSSBIMCClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_bimc_mdss_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BIMC_MDSS_CBCR), HWIO_OFFS(GCC_BIMC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIMC_MDSS_CLK
  },
#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_mdp_tbu_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_MDP_TBU_CBCR), HWIO_OFFS(GCC_SMMU_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_MDP_TBU_CLK
  },
#endif

  {
    /* .szClockName      = */ "gcc_mdss_axi_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_MDSS_AXI_CBCR), HWIO_OFFS(GCC_MDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_MDSS_AXI_CLK
  },
};


/*
 * HAL_clk_mGCCMDSSBIMCClkDomain
 *
 * MDSSBIMC clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCMDSSBIMCClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_MDSS_BIMC_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mMDSSBIMCClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mMDSSBIMCClkDomainClks)/sizeof(HAL_clk_mMDSSBIMCClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControlRO,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mPCLK0ClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mPCLK0ClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_mdss_pclk0_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_MDSS_PCLK0_CBCR), HWIO_OFFS(GCC_MDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_MDSS_PCLK0_CLK
  },
#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_spdm_pclk0_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SPDM_PCLK0_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SPDM_PCLK0_CLK
  },
#endif

};


/*
 * HAL_clk_mGCCPCLK0ClkDomain
 *
 * PCLK0 clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCPCLK0ClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_PCLK0_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mPCLK0ClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mPCLK0ClkDomainClks)/sizeof(HAL_clk_mPCLK0ClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl_2,
  /* .pmNextClockDomain    = */ NULL
};


/*                           
 *  HAL_clk_mVSYNCClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mVSYNCClkDomainClks[] =
{
  {
    /* .szClockName      = */ "gcc_mdss_vsync_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_MDSS_VSYNC_CBCR), HWIO_OFFS(GCC_MDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_MDSS_VSYNC_CLK
  },
};


/*
 * HAL_clk_mGCCVSYNCClkDomain
 *
 * VSYNC clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCVSYNCClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_VSYNC_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mVSYNCClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mVSYNCClkDomainClks)/sizeof(HAL_clk_mVSYNCClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControl_2,
  /* .pmNextClockDomain    = */ NULL
};


/*
 * HAL_clk_mGCCMDSSPowerDomain
 *
 * MDSS power domain.
 */
HAL_clk_PowerDomainDescType HAL_clk_mGCCMDSSPowerDomain =
{
  /* .szPowerDomainName       = */ "VDD_MDSS",
  /* .nGDSCRAddr              = */ HWIO_OFFS(GCC_MDSS_GDSCR),
  /* .pmControl               = */ &HAL_clk_mGenericPowerDomainControl,
  /* .pmNextPowerDomain       = */ NULL
};


/* ===========================================================================
**  HAL_clk_SetExternalSourceDiv
**
** ======================================================================== */
void HAL_clk_SetExternalSourceDiv
(
  const char   *szClock,
  uint32        nExtDivider
)
{
/* This is used for settings the external source Dividers, and is used by Clock_SetExternalSourceDiv API in the ClockDriver.c file
 *  to avoid Linking errors added in vipertooth target
*/

}/* END HAL_clk_SetExternalSourceDiv */