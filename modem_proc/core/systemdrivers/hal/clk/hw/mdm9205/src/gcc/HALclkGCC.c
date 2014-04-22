/*
==============================================================================

FILE:         HALclkGCC.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the
   GCC clocks.

   List of clock domains:
    - HAL_clk_mGCCGCCSLEEPClkDomain
    - HAL_clk_mGCCGCCXOClkDomain


   List of power domains:



==============================================================================

$Header: //components/rel/core.mpss/3.10/systemdrivers/hal/clk/hw/mdm9205/src/gcc/HALclkGCC.c#2 $

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


/* ============================================================================
**    Externs
** ==========================================================================*/

extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControlRO;


/* ============================================================================
**    Data
** ==========================================================================*/


/*
 *  HAL_clk_mGCCSLEEPClkDomainClks
 *
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mGCCSLEEPClkDomainClks[] =
{
#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_bimc_sleep_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BIMC_SLEEP_CBCR), HWIO_OFFS(GCC_BIMC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIMC_SLEEP_CLK
  },
#endif

  {
    /* .szClockName      = */ "gcc_blsp1_sleep_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BLSP1_SLEEP_CBCR), HWIO_OFFS(GCC_BLSP1_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, BLSP1_SLEEP_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BLSP1_SLEEP_CLK
  },
#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_ddr_dim_sleep_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_DDR_DIM_SLEEP_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_DDR_DIM_SLEEP_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_im_sleep_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_GCC_IM_SLEEP_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_IM_SLEEP_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_qpic_system_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QPIC_SYSTEM_CBCR), HWIO_OFFS(GCC_QPIC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QPIC_SYSTEM_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_rpm_sleep_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_RPM_SLEEP_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_RPM_SLEEP_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_usb2a_phy_sleep_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_USB2A_PHY_SLEEP_CBCR), HWIO_OFFS(GCC_USB2A_PHY_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_USB2A_PHY_SLEEP_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_usb_hs_inactivity_timers_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_USB_HS_INACTIVITY_TIMERS_CBCR), HWIO_OFFS(GCC_USB_HS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_USB_HS_INACTIVITY_TIMERS_CLK
  },
#endif

};


/*
 * HAL_clk_mGCCGCCSLEEPClkDomain
 *
 * GCCSLEEP clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCSLEEPClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_GCC_SLEEP_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mGCCSLEEPClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mGCCSLEEPClkDomainClks)/sizeof(HAL_clk_mGCCSLEEPClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControlRO,
  /* .pmNextClockDomain    = */ NULL
};


/*
 *  HAL_clk_mGCCXOClkDomainClks
 *
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mGCCXOClkDomainClks[] =
{
#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_dcc_xo_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_DCC_XO_CBCR), HWIO_OFFS(GCC_DCC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_DCC_XO_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_pcnoc_xo_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_XO_CBCR), HWIO_OFFS(GCC_PCNOC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_XO_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_pdm_xo4_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PDM_XO4_CBCR), HWIO_OFFS(GCC_PDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PDM_XO4_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_pwm0_xo512_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PWM0_XO512_CBCR), HWIO_OFFS(GCC_PDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PWM0_XO512_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_rpm_timer_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_RPM_TIMER_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_RPM_TIMER_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_spdm_ff_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SPDM_FF_CBCR), HWIO_OFFS(GCC_SPDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SPDM_FF_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_spdm_pcnoc_cy_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SPDM_PCNOC_CY_CBCR), HWIO_OFFS(GCC_SPDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SPDM_PCNOC_CY_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_xo_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_GCC_XO_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_XO_CLK
  },
#endif

  {
    /* .szClockName      = */ "gcc_xo_div4_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_GCC_XO_DIV4_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_XO_DIV4_CLK
  },
};


/*
 * HAL_clk_mGCCGCCXOClkDomain
 *
 * GCCXO clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCGCCXOClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_GCC_XO_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mGCCXOClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mGCCXOClkDomainClks)/sizeof(HAL_clk_mGCCXOClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControlRO,
  /* .pmNextClockDomain    = */ NULL
};
