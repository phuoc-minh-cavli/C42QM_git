/*
==============================================================================

FILE:         HALclkSYSTEMNOC.c

DESCRIPTION:
   This auto-generated file contains the clock HAL code for the 
   SYSTEMNOC clocks.

   List of clock domains:
     - HAL_clk_mGCCSYSTEMNOCBFDCDClkDomain


   List of power domains:



==============================================================================

$Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Library/HALclkLib/HALclkSYSTEMNOC.c#1 $

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

extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControl_1;
extern HAL_clk_ClockDomainControlType  HAL_clk_mGCCClockDomainControlRO_1;


/* ============================================================================
**    Data
** ==========================================================================*/


/*                           
 *  HAL_clk_mSYSTEMNOCBFDCDClkDomainClks
 *                  
 *  List of clocks supported by this domain.
 */
static HAL_clk_ClockDescType HAL_clk_mSYSTEMNOCBFDCDClkDomainClks[] =
{
#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_bimc_sysnoc_axi_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_BIMC_SYSNOC_AXI_CBCR), HWIO_OFFS(GCC_BIMC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_BIMC_SYSNOC_AXI_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_emac_tbu_axi_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_EMAC_TBU_AXI_CBCR), HWIO_OFFS(GCC_SMMU_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_EMAC_TBU_AXI_CLK
  },
#endif

  {
    /* .szClockName      = */ "gcc_eth_axi_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_ETH_AXI_CBCR), HWIO_OFFS(GCC_EMAC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_ETH_AXI_CLK
  },
#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_imem_axi_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_IMEM_AXI_CBCR), HWIO_OFFS(GCC_IMEM_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, IMEM_AXI_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_IMEM_AXI_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_lpass_q6_axis_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_LPASS_Q6_AXIS_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_LPASS_Q6_AXIS_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_lpass_sway_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_LPASS_SWAY_CBCR), 0, {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_LPASS_SWAY_CLK
  },
#endif

  {
    /* .szClockName      = */ "gcc_pcie_0_mstr_axi_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCIE_0_MSTR_AXI_CBCR), HWIO_OFFS(GCC_PCIE_0_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, PCIE_0_MSTR_AXI_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCIE_0_MSTR_AXI_CLK
  },
  {
    /* .szClockName      = */ "gcc_pcie_0_slv_axi_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCIE_0_SLV_AXI_CBCR), HWIO_OFFS(GCC_PCIE_0_BCR), HAL_CLK_FMSK(PROC_CLK_BRANCH_ENA_VOTE, PCIE_0_SLV_AXI_CLK_ENA) },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCIE_0_SLV_AXI_CLK
  },
#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_pcie_tbu_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_PCIE_TBU_CBCR), HWIO_OFFS(GCC_SMMU_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_PCIE_TBU_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_qdss_etr_usb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_QDSS_ETR_USB_CBCR), HWIO_OFFS(GCC_QDSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_QDSS_ETR_USB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_smmu_cats_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SMMU_CATS_CBCR), HWIO_OFFS(GCC_SMMU_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SMMU_CATS_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_snoc_bus_timeout1_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SNOC_BUS_TIMEOUT1_AHB_CBCR), HWIO_OFFS(GCC_SNOC_BUS_TIMEOUT1_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SNOC_BUS_TIMEOUT1_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_snoc_bus_timeout2_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SNOC_BUS_TIMEOUT2_AHB_CBCR), HWIO_OFFS(GCC_SNOC_BUS_TIMEOUT2_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SNOC_BUS_TIMEOUT2_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_spdm_snoc_cy_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SPDM_SNOC_CY_CBCR), HWIO_OFFS(GCC_SPDM_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SPDM_SNOC_CY_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_sys_noc_axi_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SYS_NOC_AXI_CBCR), HWIO_OFFS(GCC_SYSTEM_NOC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SYS_NOC_AXI_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_sys_noc_lpass_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SYS_NOC_LPASS_CBCR), HWIO_OFFS(GCC_SYSTEM_NOC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SYS_NOC_LPASS_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_sys_noc_tprb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SYS_NOC_TPRB_CBCR), HWIO_OFFS(GCC_SYSTEM_NOC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SYS_NOC_TPRB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_sys_noc_wcss_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_SYS_NOC_WCSS_CBCR), HWIO_OFFS(GCC_SYSTEM_NOC_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_SYS_NOC_WCSS_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_wcss_ahb_s0_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_WCSS_AHB_S0_CBCR), HWIO_OFFS(GCC_WCSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_WCSS_AHB_S0_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_wcss_axi_m_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_WCSS_AXI_M_CBCR), HWIO_OFFS(GCC_WCSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_WCSS_AXI_M_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_wcss_ecahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_WCSS_ECAHB_CBCR), HWIO_OFFS(GCC_WCSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_WCSS_ECAHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_wcss_shdreg_ahb_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_WCSS_SHDREG_AHB_CBCR), HWIO_OFFS(GCC_WCSS_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_WCSS_SHDREG_AHB_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_wcss_tbu_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_WCSS_TBU_CBCR), HWIO_OFFS(GCC_SMMU_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_WCSS_TBU_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_wdsp_q6ss_axim_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_WDSP_Q6SS_AXIM_CBCR), HWIO_OFFS(GCC_WDSP_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_WDSP_Q6SS_AXIM_CLK
  },
#endif

#if 0 /* not present on apcs */

  {
    /* .szClockName      = */ "gcc_wdsp_xpu_clk",
    /* .mRegisters       = */ { HWIO_OFFS(GCC_WDSP_XPU_CBCR), HWIO_OFFS(GCC_SMMU_BCR), {0, 0} },
    /* .pmControl        = */ &HAL_clk_mGenericClockControl,
    /* .nTestClock       = */ HAL_CLK_GCC_TEST_GCC_WDSP_XPU_CLK
  },
#endif

};


/*
 * HAL_clk_mGCCSYSTEMNOCBFDCDClkDomain
 *
 * SYSTEMNOCBFDCD clock domain.
 */
HAL_clk_ClockDomainDescType HAL_clk_mGCCSYSTEMNOCBFDCDClkDomain =
{
  /* .nCGRAddr             = */ HWIO_OFFS(GCC_SYSTEM_NOC_BFDCD_CMD_RCGR),
  /* .pmClocks             = */ HAL_clk_mSYSTEMNOCBFDCDClkDomainClks,
  /* .nClockCount          = */ sizeof(HAL_clk_mSYSTEMNOCBFDCDClkDomainClks)/sizeof(HAL_clk_mSYSTEMNOCBFDCDClkDomainClks[0]),
  /* .pmControl            = */ &HAL_clk_mGCCClockDomainControlRO_1,
  /* .pmNextClockDomain    = */ NULL
};

