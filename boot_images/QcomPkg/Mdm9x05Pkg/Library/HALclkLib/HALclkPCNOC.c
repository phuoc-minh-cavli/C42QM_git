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

$Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Library/HALclkLib/HALclkPCNOC.c#1 $

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
#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_GCC_AHB_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_bimc_cfg_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BIMC_CFG_AHB_CBCR), HWIO_OFFS(GCC_BIMC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIMC_CFG_AHB_CLK
  },
#endif

  {
    /* .szClockName      = */ "gcc_blsp1_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BLSP1_AHB_CBCR), HWIO_OFFS(GCC_BLSP1_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, BLSP1_AHB_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BLSP1_AHB_CLK
  },
  {
    /* .szClockName      = */ "gcc_blsp2_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BLSP2_AHB_CBCR), HWIO_OFFS(GCC_BLSP2_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, BLSP2_AHB_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BLSP2_AHB_CLK
  },
  {
    /* .szClockName      = */ "gcc_boot_rom_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BOOT_ROM_AHB_CBCR), HWIO_OFFS(GCC_BOOT_ROM_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, BOOT_ROM_AHB_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BOOT_ROM_AHB_CLK
  },
#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_cdsp_cfg_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_CDSP_CFG_AHB_CBCR), HWIO_OFFS(GCC_CDSP_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_CDSP_CFG_AHB_CLK
  },
#endif

  {
    /* .szClockName      = */ "gcc_crypto_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_CRYPTO_AHB_CBCR), HWIO_OFFS(GCC_CRYPTO_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, CRYPTO_AHB_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControlRO,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_CRYPTO_AHB_CLK
  },
  {
    /* .szClockName      = */ "gcc_crypto_axi_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_CRYPTO_AXI_CBCR), HWIO_OFFS(GCC_CRYPTO_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, CRYPTO_AXI_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControlRO,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_CRYPTO_AXI_CLK
  },
  {
    /* .szClockName      = */ "gcc_dcc_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_DCC_CBCR), HWIO_OFFS(GCC_DCC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_DCC_CLK
  },
  {
    /* .szClockName      = */ "gcc_eth_slave_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_ETH_SLAVE_AHB_CBCR), HWIO_OFFS(GCC_EMAC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_ETH_SLAVE_AHB_CLK
  },
  {
    /* .szClockName      = */ "gcc_geni_ir_h_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_GENI_IR_AHB_CBCR), HWIO_OFFS(GCC_GENI_IR_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_GENI_IR_H_CLK
  },
#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_geni_ir_xpu_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_GENI_IR_XPU_CBCR), HWIO_OFFS(GCC_GENI_IR_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_GENI_IR_XPU_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_gtcu_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_GTCU_AHB_CBCR), HWIO_OFFS(GCC_SMMU_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_GTCU_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_imem_cfg_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_IMEM_CFG_AHB_CBCR), HWIO_OFFS(GCC_IMEM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_IMEM_CFG_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_lpass_agnoc_cfg_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_LPASS_AGNOC_CFG_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_LPASS_AGNOC_CFG_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_lpass_agnoc_mpu_cfg_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_LPASS_AGNOC_MPU_CFG_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_LPASS_AGNOC_MPU_CFG_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_lpass_mpu_cfg_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_LPASS_MPU_CFG_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_LPASS_MPU_CFG_CLK
  },
#endif

  {
    /* .szClockName      = */ "gcc_mdss_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_MDSS_AHB_CBCR), HWIO_OFFS(GCC_MDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_MDSS_AHB_CLK
  },
#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_mpm_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_MPM_AHB_CBCR), HWIO_OFFS(GCC_MPM_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, MPM_AHB_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_MPM_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_msg_ram_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_MSG_RAM_AHB_CBCR), HWIO_OFFS(GCC_MSG_RAM_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, MSG_RAM_AHB_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_MSG_RAM_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_noc_conf_xpu_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_NOC_CONF_XPU_AHB_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_NOC_CONF_XPU_AHB_CLK
  },
#endif

  {
    /* .szClockName      = */ "gcc_oxili_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_OXILI_AHB_CBCR), HWIO_OFFS(GCC_OXILI_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_OXILI_AHB_CLK
  },
  {
    /* .szClockName      = */ "gcc_pcie_0_cfg_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCIE_0_CFG_AHB_CBCR), HWIO_OFFS(GCC_PCIE_0_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, PCIE_0_CFG_AHB_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCIE_0_CFG_AHB_CLK
  },
  {
    /* .szClockName      = */ "gcc_pcnoc_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_AHB_CBCR), HWIO_OFFS(GCC_PCNOC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControlRO,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_AHB_CLK
  },
#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_pcnoc_bus_timeout0_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT0_AHB_CBCR), HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT0_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT0_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_pcnoc_bus_timeout1_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT1_AHB_CBCR), HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT1_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT1_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_pcnoc_bus_timeout2_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT2_AHB_CBCR), HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT2_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT2_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_pcnoc_bus_timeout3_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT3_AHB_CBCR), HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT3_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT3_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_pcnoc_bus_timeout4_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT4_AHB_CBCR), HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT4_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT4_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_pcnoc_bus_timeout6_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT6_AHB_CBCR), HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT6_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT6_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_pcnoc_bus_timeout7_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT7_AHB_CBCR), HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT7_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT7_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_pcnoc_bus_timeout8_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT8_AHB_CBCR), HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT8_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT8_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_pcnoc_bus_timeout9_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT9_AHB_CBCR), HWIO_OFFS(GCC_PCNOC_BUS_TIMEOUT9_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_BUS_TIMEOUT9_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_pcnoc_ddr_cfg_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_DDR_CFG_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_DDR_CFG_CLK
  },
#endif

  {
    /* .szClockName      = */ "gcc_pcnoc_east_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_EAST_AHB_CBCR), HWIO_OFFS(GCC_PCNOC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControlRO,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_EAST_AHB_CLK
  },
#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_pcnoc_mpu_cfg_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_MPU_CFG_AHB_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_MPU_CFG_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_pcnoc_mst_rpm_bridge_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_MST_RPM_BRIDGE_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_MST_RPM_BRIDGE_CLK
  },
#endif

  {
    /* .szClockName      = */ "gcc_pcnoc_north_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_NORTH_AHB_CBCR), HWIO_OFFS(GCC_PCNOC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControlRO,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_NORTH_AHB_CLK
  },
#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_pcnoc_slv_rpm_bridge_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_SLV_RPM_BRIDGE_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_SLV_RPM_BRIDGE_CLK
  },
#endif

  {
    /* .szClockName      = */ "gcc_pcnoc_south_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_SOUTH_AHB_CBCR), HWIO_OFFS(GCC_PCNOC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControlRO,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_SOUTH_AHB_CLK
  },
#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_pcnoc_tbu_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_TBU_CBCR), HWIO_OFFS(GCC_SMMU_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_TBU_CLK
  },
#endif

  {
    /* .szClockName      = */ "gcc_pcnoc_west_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCNOC_WEST_AHB_CBCR), HWIO_OFFS(GCC_PCNOC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControlRO,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCNOC_WEST_AHB_CLK
  },
  {
    /* .szClockName      = */ "gcc_pdm_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PDM_AHB_CBCR), HWIO_OFFS(GCC_PDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PDM_AHB_CLK
  },
  {
    /* .szClockName      = */ "gcc_prng_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PRNG_AHB_CBCR), HWIO_OFFS(GCC_PRNG_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, PRNG_AHB_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PRNG_AHB_CLK
  },
#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_qdss_cfg_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QDSS_CFG_AHB_CBCR), HWIO_OFFS(GCC_QDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QDSS_CFG_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_qdss_dap_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QDSS_DAP_AHB_CBCR), HWIO_OFFS(GCC_QDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QDSS_DAP_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_qpic_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QPIC_AHB_CBCR), HWIO_OFFS(GCC_QPIC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QPIC_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_rbcpr_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_RBCPR_AHB_CBCR), HWIO_OFFS(GCC_RBCPR_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_RBCPR_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_rpm_cfg_xpu_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_RPM_CFG_XPU_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_RPM_CFG_XPU_CLK
  },
#endif

  {
    /* .szClockName      = */ "gcc_sdcc1_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SDCC1_AHB_CBCR), HWIO_OFFS(GCC_SDCC1_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SDCC1_AHB_CLK
  },
  {
    /* .szClockName      = */ "gcc_sdcc2_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SDCC2_AHB_CBCR), HWIO_OFFS(GCC_SDCC2_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SDCC2_AHB_CLK
  },
#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_sec_ctrl_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SEC_CTRL_AHB_CBCR), HWIO_OFFS(GCC_SEC_CTRL_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SEC_CTRL_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_sec_ctrl_boot_rom_patch_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SEC_CTRL_BOOT_ROM_PATCH_CBCR), HWIO_OFFS(GCC_SEC_CTRL_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SEC_CTRL_BOOT_ROM_PATCH_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_smmu_cfg_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SMMU_CFG_CBCR), HWIO_OFFS(GCC_SMMU_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SMMU_CFG_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_smmu_xpu_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SMMU_XPU_CBCR), HWIO_OFFS(GCC_SMMU_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SMMU_XPU_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_snoc_pcnoc_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SNOC_PCNOC_AHB_CBCR), HWIO_OFFS(GCC_SYSTEM_NOC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SNOC_PCNOC_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_spdm_cfg_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SPDM_CFG_AHB_CBCR), HWIO_OFFS(GCC_SPDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SPDM_CFG_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_spdm_mstr_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SPDM_MSTR_AHB_CBCR), HWIO_OFFS(GCC_SPDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SPDM_MSTR_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_spdm_pcnoc_cy_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SPDM_PCNOC_CY_CBCR), HWIO_OFFS(GCC_SPDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SPDM_PCNOC_CY_CLK
  },
#endif

  {
    /* .szClockName      = */ "gcc_spmi_pcnoc_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SPMI_PCNOC_AHB_CBCR), HWIO_OFFS(GCC_SPMI_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, SPMI_PCNOC_AHB_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControlRO,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SPMI_PCNOC_AHB_CLK
  },
#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_tcsr_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_TCSR_AHB_CBCR), HWIO_OFFS(GCC_TCSR_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_TCSR_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_tic_cfg_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_TIC_CFG_AHB_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_TIC_CFG_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_tlmm_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_TLMM_CBCR), HWIO_OFFS(GCC_TLMM_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, TLMM_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_TLMM_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_tlmm_east_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_TLMM_EAST_AHB_CBCR), HWIO_OFFS(GCC_TLMM_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, TLMM_EAST_AHB_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_TLMM_EAST_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_tlmm_east_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_TLMM_EAST_CBCR), HWIO_OFFS(GCC_TLMM_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, TLMM_EAST_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_TLMM_EAST_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_tlmm_north_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_TLMM_NORTH_AHB_CBCR), HWIO_OFFS(GCC_TLMM_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, TLMM_NORTH_AHB_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_TLMM_NORTH_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_tlmm_north_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_TLMM_NORTH_CBCR), HWIO_OFFS(GCC_TLMM_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, TLMM_NORTH_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_TLMM_NORTH_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_tlmm_south_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_TLMM_SOUTH_AHB_CBCR), HWIO_OFFS(GCC_TLMM_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, TLMM_SOUTH_AHB_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_TLMM_SOUTH_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_tlmm_south_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_TLMM_SOUTH_CBCR), HWIO_OFFS(GCC_TLMM_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, TLMM_SOUTH_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_TLMM_SOUTH_CLK
  },
#endif

  {
    /* .szClockName      = */ "gcc_usb_hs_phy_cfg_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_USB_HS_PHY_CFG_AHB_CBCR), HWIO_OFFS(GCC_USB2A_PHY_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_USB_HS_PHY_CFG_AHB_CLK
  },
#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_wdsp_q6ss_ahbs_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_WDSP_Q6SS_AHBS_CBCR), HWIO_OFFS(GCC_WDSP_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_WDSP_Q6SS_AHBS_CLK
  },
#endif

  {
    /* .szClockName      = */ "gcc_bias_pll_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BIAS_PLL_AHB_CBCR), HWIO_OFFS(GCC_BIAS_PLL_MISC_RESET), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIAS_PLL_AHB_CLK
  },


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

