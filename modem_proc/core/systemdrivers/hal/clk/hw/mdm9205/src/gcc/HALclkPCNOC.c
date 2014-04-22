/*
==============================================================================

FILE:         HALclkPCNOC.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the
   PCNOC clocks.

   List of clock domains:
    - HAL_clk_mGCCPCNOCBFDCDClkDomain


   List of power domains:



==============================================================================

$Header: //components/rel/core.mpss/3.10/systemdrivers/hal/clk/hw/mdm9205/src/gcc/HALclkPCNOC.c#3 $

==============================================================================
            Copyright (c) 2019 Qualcomm Technologies Incorporated.
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


/* ============================================================================
**    Data
** ==========================================================================*/


/*
 *  HAL_clk_mPCNOCBFDCDClkDomainClks
 *
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mPCNOCBFDCDClkDomainClks[] =
{
#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_GCC_AHB_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_bimc_cfg_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BIMC_CFG_AHB_CBCR), HWIO_OFFS(GCC_BIMC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIMC_CFG_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_bimc_noc_mpu_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BIMC_NOC_MPU_AHB_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIMC_NOC_MPU_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_bimc_pcnoc_axi_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BIMC_PCNOC_AXI_CBCR), HWIO_OFFS(GCC_BIMC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIMC_PCNOC_AXI_CLK
  },
#endif

  {
    /* .szClockName      = */ "gcc_blsp1_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BLSP1_AHB_CBCR), HWIO_OFFS(GCC_BLSP1_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, BLSP1_AHB_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BLSP1_AHB_CLK
  },
  {
    /* .szClockName      = */ "gcc_prng_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PRNG_AHB_CBCR), HWIO_OFFS(GCC_PRNG_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, PRNG_AHB_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PRNG_AHB_CLK
  },
#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_boot_rom_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BOOT_ROM_AHB_CBCR), HWIO_OFFS(GCC_BOOT_ROM_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, BOOT_ROM_AHB_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BOOT_ROM_AHB_CLK
  },
#endif

  {
    /* .szClockName      = */ "gcc_crypto_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_CRYPTO_AHB_CBCR), HWIO_OFFS(GCC_CRYPTO_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControlRO,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_CRYPTO_AHB_CLK
  },
  {
    /* .szClockName      = */ "gcc_crypto_axi_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_CRYPTO_AXI_CBCR), HWIO_OFFS(GCC_CRYPTO_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, CRYPTO_AXI_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControlRO,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_CRYPTO_AXI_CLK
  },
#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_dcc_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_DCC_CBCR), HWIO_OFFS(GCC_DCC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_DCC_CLK
  },
#endif

  {
    /* .szClockName      = */ "gcc_dehr_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_DEHR_CBCR), HWIO_OFFS(GCC_DEHR_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_DEHR_CLK
  },

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_ecc_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_ECC_CBCR), HWIO_OFFS(GCC_ECC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_ECC_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_imem_axi_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_IMEM_AXI_CBCR), HWIO_OFFS(GCC_IMEM_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, IMEM_AXI_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_IMEM_AXI_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_imem_cfg_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_IMEM_CFG_AHB_CBCR), HWIO_OFFS(GCC_IMEM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_IMEM_CFG_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_mpm_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_MPM_AHB_CBCR), HWIO_OFFS(GCC_MPM_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, MPM_AHB_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_MPM_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_msg_ram_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_MSG_RAM_AHB_CBCR), HWIO_OFFS(GCC_MSG_RAM_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, MSG_RAM_AHB_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_MSG_RAM_AHB_CLK
  },
#endif

  {
    /* .szClockName      = */ "gcc_mss_cfg_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_MSS_CFG_AHB_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_MSS_CFG_AHB_CLK
  },
#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_mss_nav_ce_mpu_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_MSS_NAV_CE_MPU_AHB_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_MSS_NAV_CE_MPU_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_mss_nav_ce_vmidmt_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_MSS_NAV_CE_VMIDMT_AHB_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_MSS_NAV_CE_VMIDMT_AHB_CLK
  },
#endif

  {
    /* .szClockName      = */ "gcc_mss_q6ss_axis_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_MSS_Q6SS_AXIS_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_MSS_Q6SS_AXIS_CLK
  },
#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_noc_conf_xpu_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_NOC_CONF_XPU_AHB_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_NOC_CONF_XPU_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_pcnoc_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_AHB_CBCR), HWIO_OFFS(GCC_PCNOC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_pcnoc_bus_timeout0_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT0_AHB_CBCR), HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT0_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT0_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_pcnoc_bus_timeout1_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT1_AHB_CBCR), HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT1_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT1_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_pcnoc_bus_timeout2_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT2_AHB_CBCR), HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT2_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT2_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_pcnoc_bus_timeout3_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT3_AHB_CBCR), HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT3_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT3_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_pcnoc_bus_timeout5_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT5_AHB_CBCR), HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT5_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT5_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_pcnoc_bus_timeout7_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT7_AHB_CBCR), HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT7_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT7_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_pcnoc_bus_timeout8_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT8_AHB_CBCR), HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT8_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT8_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_pcnoc_bus_timeout9_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT9_AHB_CBCR), HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT9_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT9_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_pcnoc_ddr_cfg_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_DDR_CFG_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_DDR_CFG_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_pcnoc_mpu_cfg_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_MPU_CFG_AHB_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_MPU_CFG_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_pcnoc_snoc_mpu_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_SNOC_MPU_AHB_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_SNOC_MPU_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_pcnoc_snoc_vmidmt1_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_SNOC_VMIDMT1_AHB_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_SNOC_VMIDMT1_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_pcnoc_snoc_vmidmt2_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_SNOC_VMIDMT2_AHB_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_SNOC_VMIDMT2_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_pcnoc_snoc_vmidmt3_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_SNOC_VMIDMT3_AHB_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_SNOC_VMIDMT3_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_pcnoc_snoc_vmidmt4_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_SNOC_VMIDMT4_AHB_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_SNOC_VMIDMT4_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_pdm_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PDM_AHB_CBCR), HWIO_OFFS(GCC_PDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PDM_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_prng_ahb_bridge_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PRNG_AHB_BRIDGE_CBCR), HWIO_OFFS(GCC_PRNG_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PRNG_AHB_BRIDGE_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_prng_xpu_cfg_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PRNG_XPU_CFG_AHB_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PRNG_XPU_CFG_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_q6_mpu_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_Q6_MPU_AHB_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_Q6_MPU_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_q6_vmidmt_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_Q6_VMIDMT_AHB_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_Q6_VMIDMT_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_qdss_cfg_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QDSS_CFG_AHB_CBCR), HWIO_OFFS(GCC_QDSS_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, QDSS_CFG_AHB_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QDSS_CFG_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_qdss_dap_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QDSS_DAP_AHB_CBCR), HWIO_OFFS(GCC_QDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QDSS_DAP_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_qdss_etr_usb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QDSS_ETR_USB_CBCR), HWIO_OFFS(GCC_QDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QDSS_ETR_USB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_qpic_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QPIC_AHB_CBCR), HWIO_OFFS(GCC_QPIC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QPIC_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_qzip_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QZIP_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QZIP_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_rbcpr_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_RBCPR_AHB_CBCR), HWIO_OFFS(GCC_RBCPR_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_RBCPR_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_sec_ctrl_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SEC_CTRL_AHB_CBCR), HWIO_OFFS(GCC_SEC_CTRL_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SEC_CTRL_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_sec_ctrl_boot_rom_patch_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SEC_CTRL_BOOT_ROM_PATCH_CBCR), HWIO_OFFS(GCC_SEC_CTRL_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SEC_CTRL_BOOT_ROM_PATCH_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_snoc_boot_imem_mpu_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SNOC_BOOT_IMEM_MPU_AHB_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SNOC_BOOT_IMEM_MPU_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_spdm_cfg_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SPDM_CFG_AHB_CBCR), HWIO_OFFS(GCC_SPDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SPDM_CFG_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_spdm_mstr_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SPDM_MSTR_AHB_CBCR), HWIO_OFFS(GCC_SPDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SPDM_MSTR_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_spmi_pcnoc_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SPMI_PCNOC_AHB_CBCR), HWIO_OFFS(GCC_SPMI_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, SPMI_PCNOC_AHB_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SPMI_PCNOC_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_tcsr_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_TCSR_AHB_CBCR), HWIO_OFFS(GCC_TCSR_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_TCSR_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_tlmm_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_TLMM_AHB_CBCR), HWIO_OFFS(GCC_TLMM_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, TLMM_AHB_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_TLMM_AHB_CLK
  },
#endif

#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_tlmm_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_TLMM_CBCR), HWIO_OFFS(GCC_TLMM_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, TLMM_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_TLMM_CLK
  },
#endif

  {
    /* .szClockName      = */ "gcc_ultaudio_pcnoc_mport_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_ULTAUDIO_PCNOC_MPORT_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_ULTAUDIO_PCNOC_MPORT_CLK
  },
  {
    /* .szClockName      = */ "gcc_ultaudio_pcnoc_sway_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_ULTAUDIO_PCNOC_SWAY_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_ULTAUDIO_PCNOC_SWAY_CLK
  },
#if 0 /* not present on mss */

  {
    /* .szClockName      = */ "gcc_usb_hs_phy_cfg_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_USB_HS_PHY_CFG_AHB_CBCR), HWIO_OFFS(GCC_USB2A_PHY_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_USB_HS_PHY_CFG_AHB_CLK
  },
#endif

};


/*
 * HAL_clk_mGCCPCNOCBFDCDClkDomain
 *
 * PCNOCBFDCD clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCPCNOCBFDCDClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_PCNOC_BFDCD_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mPCNOCBFDCDClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mPCNOCBFDCDClkDomainClks)/sizeof(HAL_clk_mPCNOCBFDCDClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControlRO,
  /* .pmNextClockDomain    = */ NULL
};