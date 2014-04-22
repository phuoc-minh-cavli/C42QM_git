/**
 * @file pcie_target_hwio.h
 * @brief Auto-generated HWIO interface include file.
 * This file contains HWIO register definitions for the following modules outside PCIe:
 * SECURITY_CONTROL_CORE
 * Also contains other hardware dependent #defines
 * 'Include' filters applied: BOOT_CONFIG[SECURITY_CONTROL_CORE]
 * 'Exclude' filters applied: RESERVED DUMMY
 */
/*
===============================================================================

Edit History

$Header:

Change revision history maintained in version system
===============================================================================
Copyright (c) 2014-2015 QUALCOMM Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===============================================================================

*/
#ifndef __PCIE_TARGET_HWIO_H__
#define __PCIE_TARGET_HWIO_H__

#include "msmhwiobase.h"
#include "HALhwio.h"

#define PCIE_PCIE20_WRAPPER_AHB_BASE                                          PCIE_0_PCIE20_WRAPPER_AHB_BASE
#define PCIE_PCIE20_WRAPPER_AHB_BASE_SIZE                                     PCIE_0_PCIE20_WRAPPER_AHB_BASE_SIZE
#define PCIE_PCIE20_WRAPPER_AHB_BASE_PHYS                                     PCIE_0_PCIE20_WRAPPER_AHB_BASE_PHYS

#define PCIE_PCIE20_WRAPPER_AXI_BASE                                          PCIE_0_PCIE20_WRAPPER_AXI_BASE
#define PCIE_PCIE20_WRAPPER_AXI_BASE_SIZE                                     PCIE_0_PCIE20_WRAPPER_AXI_BASE_SIZE
#define PCIE_PCIE20_WRAPPER_AXI_BASE_PHYS                                     PCIE_0_PCIE20_WRAPPER_AXI_BASE_PHYS

#define PCIE_CM_SIZE  PCIE_PHY_SIZE

#define PCIE_0_QSERDES_COM_QSERDES_COM_PCIE_USB3_COMMON_REG_BASE              (PCIE_0_PCIE20_WRAPPER_AHB_BASE      + 0x00006000)
#define PCIE_0_QSERDES_COM_QSERDES_COM_PCIE_USB3_COMMON_REG_BASE_OFFS         0x00006000


#define PCIE_QSERDES_COM_QSERDES_COM_PCIE_USB3_COMMON_REG_BASE                PCIE_0_QSERDES_COM_QSERDES_COM_PCIE_USB3_COMMON_REG_BASE
#define PCIE_QSERDES_COM_QSERDES_COM_PCIE_USB3_COMMON_REG_BASE_PHYS           (PCIE_0_PCIE20_WRAPPER_AHB_BASE_PHYS + 0x00006000)
#define PCIE_QSERDES_COM_QSERDES_COM_PCIE_USB3_COMMON_REG_BASE_OFFS           PCIE_0_QSERDES_COM_QSERDES_COM_PCIE_USB3_COMMON_REG_BASE_OFFS


/* Define offset */
#define PCIE20_ELBI_REG_BASE_OFFS                                        0x3FFFEF20
#define PCIE20_PARF_REG_BASE_OFFS                                        0x00000000
#define PCIE20_MHI_REG_BASE_OFFS                                         0x00007000
#define DWC_PCIE_DM_REG_BASE_OFFS                                        0x3FFFE000
#define PCIE20_PHY_REG_BASE_OFFS                                         PCIE_QSERDES_COM_QSERDES_COM_PCIE_USB3_COMMON_REG_BASE_OFFS

/*----------------------------------------------------------------------------
 * MODULE: TCSR_TCSR_REGS
 *--------------------------------------------------------------------------*/

#define TCSR_TCSR_REGS_REG_BASE                                                                                     (CORE_TOP_CSR_BASE      + 0x00037000)
#define TCSR_TCSR_REGS_REG_BASE_PHYS                                                                                (CORE_TOP_CSR_BASE_PHYS + 0x00037000)
#define TCSR_TCSR_REGS_REG_BASE_OFFS                                                                                0x00037000

#define HWIO_TCSR_PCIE_PERST_EN_ADDR                                                                                (TCSR_TCSR_REGS_REG_BASE      + 0x00015020)
#define HWIO_TCSR_PCIE_PERST_EN_PHYS                                                                                (TCSR_TCSR_REGS_REG_BASE_PHYS + 0x00015020)
#define HWIO_TCSR_PCIE_PERST_EN_OFFS                                                                                (TCSR_TCSR_REGS_REG_BASE_OFFS + 0x00015020)
#define HWIO_TCSR_PCIE_PERST_EN_RMSK                                                                                       0x1
#define HWIO_TCSR_PCIE_PERST_EN_IN          \
        in_dword_masked(HWIO_TCSR_PCIE_PERST_EN_ADDR, HWIO_TCSR_PCIE_PERST_EN_RMSK)
#define HWIO_TCSR_PCIE_PERST_EN_INM(m)      \
        in_dword_masked(HWIO_TCSR_PCIE_PERST_EN_ADDR, m)
#define HWIO_TCSR_PCIE_PERST_EN_OUT(v)      \
        out_dword(HWIO_TCSR_PCIE_PERST_EN_ADDR,v)
#define HWIO_TCSR_PCIE_PERST_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_PCIE_PERST_EN_ADDR,m,v,HWIO_TCSR_PCIE_PERST_EN_IN)
#define HWIO_TCSR_PCIE_PERST_EN_PCIE_PERST_EN_BMSK                                                                         0x1
#define HWIO_TCSR_PCIE_PERST_EN_PCIE_PERST_EN_SHFT                                                                         0x0

/*----------------------------------------------------------------------------
 * MODULE: SECURITY_CONTROL_CORE
 *--------------------------------------------------------------------------*/

#define SECURITY_CONTROL_CORE_REG_BASE                                                        (SECURITY_CONTROL_BASE      + 0x00000000)
#define SECURITY_CONTROL_CORE_REG_BASE_OFFS                                                   0x00000000

#define HWIO_BOOT_CONFIG_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000602c)
#define HWIO_BOOT_CONFIG_OFFS                                                                 (SECURITY_CONTROL_CORE_REG_BASE_OFFS + 0x0000602c)
#define HWIO_BOOT_CONFIG_RMSK                                                                      0x3ff
#define HWIO_BOOT_CONFIG_IN          \
        in_dword_masked(HWIO_BOOT_CONFIG_ADDR, HWIO_BOOT_CONFIG_RMSK)
#define HWIO_BOOT_CONFIG_INM(m)      \
        in_dword_masked(HWIO_BOOT_CONFIG_ADDR, m)
#define HWIO_BOOT_CONFIG_USB_SS_ENABLE_BMSK                                                        0x200
#define HWIO_BOOT_CONFIG_USB_SS_ENABLE_SHFT                                                          0x9
#define HWIO_BOOT_CONFIG_USB_SS_ENABLE_DISABLE_FVAL                                                  0x0
#define HWIO_BOOT_CONFIG_USB_SS_ENABLE_ENABLE_FVAL                                                   0x1
#define HWIO_BOOT_CONFIG_FORCE_MSA_AUTH_EN_BMSK                                                    0x100
#define HWIO_BOOT_CONFIG_FORCE_MSA_AUTH_EN_SHFT                                                      0x8
#define HWIO_BOOT_CONFIG_FORCE_MSA_AUTH_EN_DOES_NOT_FORCE_CODE_AUTHENTICATION_FOR_SECURE_BOOT_FVAL   0x0
#define HWIO_BOOT_CONFIG_FORCE_MSA_AUTH_EN_FORCE_CODE_AUTHENTICATION_FOR_SECURE_BOOT_FVAL            0x1
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_BMSK                                                   0xc0
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_SHFT                                                    0x6
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_XO_FVAL                                                 0x0
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_ENUM_384_MHZ_FVAL                                       0x1
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_ENUM_614_4_MHZ_FVAL                                     0x2
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_ENUM_998_4_MHZ_FVAL                                     0x3
#define HWIO_BOOT_CONFIG_APPS_BOOT_FROM_ROM_BMSK                                                    0x20
#define HWIO_BOOT_CONFIG_APPS_BOOT_FROM_ROM_SHFT                                                     0x5
#define HWIO_BOOT_CONFIG_FAST_BOOT_BMSK                                                             0x1e
#define HWIO_BOOT_CONFIG_FAST_BOOT_SHFT                                                              0x1
#define HWIO_BOOT_CONFIG_FAST_BOOT_DEFAULT_FVAL                                                      0x0
#define HWIO_BOOT_CONFIG_FAST_BOOT_SD_SDC2_EMMC_SDC1_FVAL                                            0x1
#define HWIO_BOOT_CONFIG_FAST_BOOT_EMMC_SDC1_FVAL                                                    0x2
#define HWIO_BOOT_CONFIG_FAST_BOOT_USB_FVAL                                                          0x3
#define HWIO_BOOT_CONFIG_FAST_BOOT_HSIC_FVAL                                                         0x4
#define HWIO_BOOT_CONFIG_FAST_BOOT_NAND_FVAL                                                         0x5
#define HWIO_BOOT_CONFIG_FAST_BOOT_SPI_ON_BLSP1_QUP1_FVAL                                            0x6
#define HWIO_BOOT_CONFIG_FAST_BOOT_SPI_ON_BLSP1_QUP2_FVAL                                            0x7
#define HWIO_BOOT_CONFIG_WDOG_EN_BMSK                                                                0x1
#define HWIO_BOOT_CONFIG_WDOG_EN_SHFT                                                                0x0
#define HWIO_BOOT_CONFIG_WDOG_EN_GPIO_CONTROLLED_FVAL                                                0x0
#define HWIO_BOOT_CONFIG_WDOG_EN_ENABLE_WDOG_FVAL                                                    0x1

/*----------------------------------------------------------------------------
 * MODULE: BIMC_GLOBAL2
 *--------------------------------------------------------------------------*/

#define BIMC_GLOBAL2_REG_BASE                                              (BIMC_BASE      + 0x00002000)

#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_ADDR(n)                  (BIMC_GLOBAL2_REG_BASE      + 0x00000220 + 0x80 * (n))
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_RMSK                     0xfff0040f
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_MAXn                              2
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_INI(n)        \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_ADDR(n), HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_RMSK)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_ADDR(n), mask)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_OUTI(n,val)    \
        out_dword(HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_ADDR(n),val)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_ADDR(n),mask,val,HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_INI(n))
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_BASE_31_20_BMSK          0xfff00000
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_BASE_31_20_SHFT                0x14
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_BASE_10_BMSK                  0x400
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_BASE_10_SHFT                    0xa
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_RFU_BMSK                        0xc
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_RFU_SHFT                        0x2
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_TYPE_BMSK                       0x2
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_TYPE_SHFT                       0x1
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_ENABLE_BMSK                     0x1
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_BASE_C_LOWER_ENABLE_SHFT                     0x0

#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_ADDR(n)                  (BIMC_GLOBAL2_REG_BASE      + 0x00000228 + 0x80 * (n))
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_RMSK                     0xfff00400
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_MAXn                              2
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_INI(n)        \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_ADDR(n), HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_RMSK)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_INMI(n,mask)    \
        in_dword_masked(HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_ADDR(n), mask)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_OUTI(n,val)    \
        out_dword(HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_ADDR(n),val)
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_ADDR(n),mask,val,HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_INI(n))
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_MASK_31_20_BMSK          0xfff00000
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_MASK_31_20_SHFT                0x14
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_MASK_10_BMSK                  0x400
#define HWIO_BIMC_BRIC_SEGMENTn_ADDR_MASK_C_LOWER_MASK_10_SHFT                    0xa

/*----------------------------------------------------------------------------
 * MODULE: TLMM_CSR
 *--------------------------------------------------------------------------*/

#define TLMM_CSR_REG_BASE                                                                   (TLMM_BASE      + 0x00000000)


#define HWIO_TLMM_GPIO_OE_0_ADDR                                                            (TLMM_CSR_REG_BASE      + 0x00200080)
#define HWIO_TLMM_GPIO_OE_0_RMSK                                                            0xffffffff
#define HWIO_TLMM_GPIO_OE_0_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OE_0_ADDR, HWIO_TLMM_GPIO_OE_0_RMSK)
#define HWIO_TLMM_GPIO_OE_0_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_0_ADDR, m)
#define HWIO_TLMM_GPIO_OE_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_0_ADDR,v)
#define HWIO_TLMM_GPIO_OE_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OE_0_ADDR,m,v,HWIO_TLMM_GPIO_OE_0_IN)
#define HWIO_TLMM_GPIO_OE_0_GPIO_OE_BMSK                                                    0xffffffff
#define HWIO_TLMM_GPIO_OE_0_GPIO_OE_SHFT                                                           0x0


#define HWIO_TLMM_GPIO_OUT_0_ADDR                                                           (TLMM_CSR_REG_BASE      + 0x00200000)
#define HWIO_TLMM_GPIO_OUT_0_RMSK                                                           0xffffffff
#define HWIO_TLMM_GPIO_OUT_0_IN          \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_0_ADDR, HWIO_TLMM_GPIO_OUT_0_RMSK)
#define HWIO_TLMM_GPIO_OUT_0_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_0_ADDR, m)
#define HWIO_TLMM_GPIO_OUT_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_0_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OUT_0_ADDR,m,v,HWIO_TLMM_GPIO_OUT_0_IN)
#define HWIO_TLMM_GPIO_OUT_0_GPIO_OUT_BMSK                                                  0xffffffff
#define HWIO_TLMM_GPIO_OUT_0_GPIO_OUT_SHFT                                                         0x0

/*----------------------------------------------------------------------------
 * MODULE: SECURITY_CONTROL_CORE
 *--------------------------------------------------------------------------*/

#define SECURITY_CONTROL_CORE_REG_BASE                                                                             (SECURITY_CONTROL_BASE      + 0x00000000)
#define SECURITY_CONTROL_CORE_REG_BASE_OFFS                                                                        0x00000000

#define HWIO_FEATURE_CONFIG2_ADDR(x)                                                           ((x) + 0x0000600c)
#define HWIO_FEATURE_CONFIG2_PHYS(x)                                                           ((x) + 0x0000600c)
#define HWIO_FEATURE_CONFIG2_OFFS                                                              (0x0000600c)
#define HWIO_FEATURE_CONFIG2_RMSK                                                              0xffffffff
#define HWIO_FEATURE_CONFIG2_IN(x)      \
        in_dword_masked(HWIO_FEATURE_CONFIG2_ADDR(x), HWIO_FEATURE_CONFIG2_RMSK)
#define HWIO_FEATURE_CONFIG2_INM(x, m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG2_ADDR(x), m)
#define HWIO_FEATURE_CONFIG2_ACC_DISABLE_BMSK                                                  0x80000000
#define HWIO_FEATURE_CONFIG2_ACC_DISABLE_SHFT                                                        0x1f
#define HWIO_FEATURE_CONFIG2_ACC_DISABLE_ENABLE_FVAL                                                  0x0
#define HWIO_FEATURE_CONFIG2_ACC_DISABLE_DISABLE_FVAL                                                 0x1
#define HWIO_FEATURE_CONFIG2_SHADOW_READ_DISABLE_BMSK                                          0x40000000
#define HWIO_FEATURE_CONFIG2_SHADOW_READ_DISABLE_SHFT                                                0x1e
#define HWIO_FEATURE_CONFIG2_SPARE1_BMSK                                                       0x30000000
#define HWIO_FEATURE_CONFIG2_SPARE1_SHFT                                                             0x1c
#define HWIO_FEATURE_CONFIG2_FUSE_SMT_PERM_ENABLE_BMSK                                          0x8000000
#define HWIO_FEATURE_CONFIG2_FUSE_SMT_PERM_ENABLE_SHFT                                               0x1b
#define HWIO_FEATURE_CONFIG2_FUSE_RCP_BYPASS_ENABLE_BMSK                                        0x4000000
#define HWIO_FEATURE_CONFIG2_FUSE_RCP_BYPASS_ENABLE_SHFT                                             0x1a
#define HWIO_FEATURE_CONFIG2_MSMC_SMMU_BYPASS_DISABLE_BMSK                                      0x2000000
#define HWIO_FEATURE_CONFIG2_MSMC_SMMU_BYPASS_DISABLE_SHFT                                           0x19
#define HWIO_FEATURE_CONFIG2_MSMC_GPIO_SLIMBUS_PD_DISABLE_BMSK                                  0x1000000
#define HWIO_FEATURE_CONFIG2_MSMC_GPIO_SLIMBUS_PD_DISABLE_SHFT                                       0x18
#define HWIO_FEATURE_CONFIG2_FUSE_PCIE20_RC_DISABLE_BMSK                                         0x800000
#define HWIO_FEATURE_CONFIG2_FUSE_PCIE20_RC_DISABLE_SHFT                                             0x17
#define HWIO_FEATURE_CONFIG2_MSMC_PCIE_DISABLE_BMSK                                              0x400000
#define HWIO_FEATURE_CONFIG2_MSMC_PCIE_DISABLE_SHFT                                                  0x16
#define HWIO_FEATURE_CONFIG2_NDINT_DISABLE_BMSK                                                  0x200000
#define HWIO_FEATURE_CONFIG2_NDINT_DISABLE_SHFT                                                      0x15
#define HWIO_FEATURE_CONFIG2_QC_DAP_DEVICEEN_DISABLE_BMSK                                        0x100000
#define HWIO_FEATURE_CONFIG2_QC_DAP_DEVICEEN_DISABLE_SHFT                                            0x14
#define HWIO_FEATURE_CONFIG2_QC_DAP_DEVICEEN_DISABLE_ENABLE_FVAL                                      0x0
#define HWIO_FEATURE_CONFIG2_QC_DAP_DEVICEEN_DISABLE_DISABLE_FVAL                                     0x1
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPNIDEN_DISABLE_BMSK                                          0x80000
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPNIDEN_DISABLE_SHFT                                             0x13
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPNIDEN_DISABLE_ENABLE_FVAL                                       0x0
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPNIDEN_DISABLE_DISABLE_FVAL                                      0x1
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPIDEN_DISABLE_BMSK                                           0x40000
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPIDEN_DISABLE_SHFT                                              0x12
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPIDEN_DISABLE_ENABLE_FVAL                                        0x0
#define HWIO_FEATURE_CONFIG2_QC_DAP_SPIDEN_DISABLE_DISABLE_FVAL                                       0x1
#define HWIO_FEATURE_CONFIG2_QC_DAP_NIDEN_DISABLE_BMSK                                            0x20000
#define HWIO_FEATURE_CONFIG2_QC_DAP_NIDEN_DISABLE_SHFT                                               0x11
#define HWIO_FEATURE_CONFIG2_QC_DAP_NIDEN_DISABLE_ENABLE_FVAL                                         0x0
#define HWIO_FEATURE_CONFIG2_QC_DAP_NIDEN_DISABLE_DISABLE_FVAL                                        0x1
#define HWIO_FEATURE_CONFIG2_QC_DAP_DBGEN_DISABLE_BMSK                                            0x10000
#define HWIO_FEATURE_CONFIG2_QC_DAP_DBGEN_DISABLE_SHFT                                               0x10
#define HWIO_FEATURE_CONFIG2_QC_DAP_DBGEN_DISABLE_ENABLE_FVAL                                         0x0
#define HWIO_FEATURE_CONFIG2_QC_DAP_DBGEN_DISABLE_DISABLE_FVAL                                        0x1
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPNIDEN_DISABLE_BMSK                                          0x8000
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPNIDEN_DISABLE_SHFT                                             0xf
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPNIDEN_DISABLE_ENABLE_FVAL                                      0x0
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPNIDEN_DISABLE_DISABLE_FVAL                                     0x1
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPIDEN_DISABLE_BMSK                                           0x4000
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPIDEN_DISABLE_SHFT                                              0xe
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPIDEN_DISABLE_ENABLE_FVAL                                       0x0
#define HWIO_FEATURE_CONFIG2_QC_APPS_SPIDEN_DISABLE_DISABLE_FVAL                                      0x1
#define HWIO_FEATURE_CONFIG2_QC_APPS_NIDEN_DISABLE_BMSK                                            0x2000
#define HWIO_FEATURE_CONFIG2_QC_APPS_NIDEN_DISABLE_SHFT                                               0xd
#define HWIO_FEATURE_CONFIG2_QC_APPS_NIDEN_DISABLE_ENABLE_FVAL                                        0x0
#define HWIO_FEATURE_CONFIG2_QC_APPS_NIDEN_DISABLE_DISABLE_FVAL                                       0x1
#define HWIO_FEATURE_CONFIG2_QC_APPS_DBGEN_DISABLE_BMSK                                            0x1000
#define HWIO_FEATURE_CONFIG2_QC_APPS_DBGEN_DISABLE_SHFT                                               0xc
#define HWIO_FEATURE_CONFIG2_QC_APPS_DBGEN_DISABLE_ENABLE_FVAL                                        0x0
#define HWIO_FEATURE_CONFIG2_QC_APPS_DBGEN_DISABLE_DISABLE_FVAL                                       0x1
#define HWIO_FEATURE_CONFIG2_QC_SPARE1_DISABLE_BMSK                                                 0x800
#define HWIO_FEATURE_CONFIG2_QC_SPARE1_DISABLE_SHFT                                                   0xb
#define HWIO_FEATURE_CONFIG2_QC_SPARE0_DISABLE_BMSK                                                 0x400
#define HWIO_FEATURE_CONFIG2_QC_SPARE0_DISABLE_SHFT                                                   0xa
#define HWIO_FEATURE_CONFIG2_QC_VENUS_0_DBGEN_DISABLE_BMSK                                          0x200
#define HWIO_FEATURE_CONFIG2_QC_VENUS_0_DBGEN_DISABLE_SHFT                                            0x9
#define HWIO_FEATURE_CONFIG2_QC_VENUS_0_DBGEN_DISABLE_ENABLE_FVAL                                     0x0
#define HWIO_FEATURE_CONFIG2_QC_VENUS_0_DBGEN_DISABLE_DISABLE_FVAL                                    0x1
#define HWIO_FEATURE_CONFIG2_QC_RPM_DAPEN_DISABLE_BMSK                                              0x100
#define HWIO_FEATURE_CONFIG2_QC_RPM_DAPEN_DISABLE_SHFT                                                0x8
#define HWIO_FEATURE_CONFIG2_QC_RPM_DAPEN_DISABLE_ENABLE_FVAL                                         0x0
#define HWIO_FEATURE_CONFIG2_QC_RPM_DAPEN_DISABLE_DISABLE_FVAL                                        0x1
#define HWIO_FEATURE_CONFIG2_QC_RPM_WCSS_NIDEN_DISABLE_BMSK                                          0x80
#define HWIO_FEATURE_CONFIG2_QC_RPM_WCSS_NIDEN_DISABLE_SHFT                                           0x7
#define HWIO_FEATURE_CONFIG2_QC_RPM_WCSS_NIDEN_DISABLE_ENABLE_FVAL                                    0x0
#define HWIO_FEATURE_CONFIG2_QC_RPM_WCSS_NIDEN_DISABLE_DISABLE_FVAL                                   0x1
#define HWIO_FEATURE_CONFIG2_QC_RPM_DBGEN_DISABLE_BMSK                                               0x40
#define HWIO_FEATURE_CONFIG2_QC_RPM_DBGEN_DISABLE_SHFT                                                0x6
#define HWIO_FEATURE_CONFIG2_QC_RPM_DBGEN_DISABLE_ENABLE_FVAL                                         0x0
#define HWIO_FEATURE_CONFIG2_QC_RPM_DBGEN_DISABLE_DISABLE_FVAL                                        0x1
#define HWIO_FEATURE_CONFIG2_QC_WCSS_DBGEN_DISABLE_BMSK                                              0x20
#define HWIO_FEATURE_CONFIG2_QC_WCSS_DBGEN_DISABLE_SHFT                                               0x5
#define HWIO_FEATURE_CONFIG2_QC_WCSS_DBGEN_DISABLE_ENABLE_FVAL                                        0x0
#define HWIO_FEATURE_CONFIG2_QC_WCSS_DBGEN_DISABLE_DISABLE_FVAL                                       0x1
#define HWIO_FEATURE_CONFIG2_QC_MSS_NIDEN_DISABLE_BMSK                                               0x10
#define HWIO_FEATURE_CONFIG2_QC_MSS_NIDEN_DISABLE_SHFT                                                0x4
#define HWIO_FEATURE_CONFIG2_QC_MSS_NIDEN_DISABLE_ENABLE_FVAL                                         0x0
#define HWIO_FEATURE_CONFIG2_QC_MSS_NIDEN_DISABLE_DISABLE_FVAL                                        0x1
#define HWIO_FEATURE_CONFIG2_QC_MSS_DBGEN_DISABLE_BMSK                                                0x8
#define HWIO_FEATURE_CONFIG2_QC_MSS_DBGEN_DISABLE_SHFT                                                0x3
#define HWIO_FEATURE_CONFIG2_QC_MSS_DBGEN_DISABLE_ENABLE_FVAL                                         0x0
#define HWIO_FEATURE_CONFIG2_QC_MSS_DBGEN_DISABLE_DISABLE_FVAL                                        0x1
#define HWIO_FEATURE_CONFIG2_QDI_SPMI_DISABLE_BMSK                                                    0x4
#define HWIO_FEATURE_CONFIG2_QDI_SPMI_DISABLE_SHFT                                                    0x2
#define HWIO_FEATURE_CONFIG2_QDI_SPMI_DISABLE_ENABLE_FVAL                                             0x0
#define HWIO_FEATURE_CONFIG2_QDI_SPMI_DISABLE_DISABLE_FVAL                                            0x1
#define HWIO_FEATURE_CONFIG2_SM_BIST_DISABLE_BMSK                                                     0x2
#define HWIO_FEATURE_CONFIG2_SM_BIST_DISABLE_SHFT                                                     0x1
#define HWIO_FEATURE_CONFIG2_SM_BIST_DISABLE_ENABLE_FVAL                                              0x0
#define HWIO_FEATURE_CONFIG2_SM_BIST_DISABLE_DISABLE_FVAL                                             0x1
#define HWIO_FEATURE_CONFIG2_TIC_DISABLE_BMSK                                                         0x1
#define HWIO_FEATURE_CONFIG2_TIC_DISABLE_SHFT                                                         0x0
#define HWIO_FEATURE_CONFIG2_TIC_DISABLE_ENABLE_FVAL                                                  0x0
#define HWIO_FEATURE_CONFIG2_TIC_DISABLE_DISABLE_FVAL                                                 0x1

/*----------------------------------------------------------------------------
 * MODULE: GCC_CLK_CTL_REG
 *--------------------------------------------------------------------------*/



#define GCC_CLK_CTL_REG_REG_BASE                                                                      (CLK_CTL_BASE      + 0x00000000)
#define GCC_CLK_CTL_REG_REG_BASE_PHYS                                                                 (CLK_CTL_BASE_PHYS + 0x00000000)
#define GCC_CLK_CTL_REG_REG_BASE_OFFS                                                                 0x00000000

#define HWIO_GCC_PCIE_BOOT_CLOCK_CTL_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x0005d000)
#define HWIO_GCC_PCIE_BOOT_CLOCK_CTL_RMSK                                                                0x1
#define HWIO_GCC_PCIE_BOOT_CLOCK_CTL_IN          \
        in_dword_masked(HWIO_GCC_PCIE_BOOT_CLOCK_CTL_ADDR, HWIO_GCC_PCIE_BOOT_CLOCK_CTL_RMSK)
#define HWIO_GCC_PCIE_BOOT_CLOCK_CTL_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_BOOT_CLOCK_CTL_ADDR, m)
#define HWIO_GCC_PCIE_BOOT_CLOCK_CTL_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_BOOT_CLOCK_CTL_ADDR,v)
#define HWIO_GCC_PCIE_BOOT_CLOCK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_BOOT_CLOCK_CTL_ADDR,m,v,HWIO_GCC_PCIE_BOOT_CLOCK_CTL_IN)
#define HWIO_GCC_PCIE_BOOT_CLOCK_CTL_CLK_ENABLE_BMSK                                                     0x1
#define HWIO_GCC_PCIE_BOOT_CLOCK_CTL_CLK_ENABLE_SHFT                                                     0x0

#define HWIO_GCC_PCIE_BCR_ADDR                                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x0005d004)
#define HWIO_GCC_PCIE_BCR_RMSK                                                                           0x1
#define HWIO_GCC_PCIE_BCR_IN          \
        in_dword_masked(HWIO_GCC_PCIE_BCR_ADDR, HWIO_GCC_PCIE_BCR_RMSK)
#define HWIO_GCC_PCIE_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_BCR_ADDR, m)
#define HWIO_GCC_PCIE_BCR_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_BCR_ADDR,v)
#define HWIO_GCC_PCIE_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_BCR_ADDR,m,v,HWIO_GCC_PCIE_BCR_IN)
#define HWIO_GCC_PCIE_BCR_BLK_ARES_BMSK                                                                  0x1
#define HWIO_GCC_PCIE_BCR_BLK_ARES_SHFT                                                                  0x0

#define HWIO_GCC_PCIE_CFG_AHB_CBCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x0005d008)
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_RMSK                                                           0xf0008001
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PCIE_CFG_AHB_CBCR_ADDR, HWIO_GCC_PCIE_CFG_AHB_CBCR_RMSK)
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_CFG_AHB_CBCR_ADDR, m)
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_CFG_AHB_CBCR_ADDR,v)
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_CFG_AHB_CBCR_ADDR,m,v,HWIO_GCC_PCIE_CFG_AHB_CBCR_IN)
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_CLK_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_CLK_OFF_SHFT                                                         0x1f
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                  0x70000000
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                        0x1c
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                          0x8000
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                             0xf
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_CLK_ENABLE_BMSK                                                       0x1
#define HWIO_GCC_PCIE_CFG_AHB_CBCR_CLK_ENABLE_SHFT                                                       0x0

#define HWIO_GCC_PCIE_PIPE_CBCR_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x0005d00c)
#define HWIO_GCC_PCIE_PIPE_CBCR_RMSK                                                              0x80007ff1
#define HWIO_GCC_PCIE_PIPE_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PCIE_PIPE_CBCR_ADDR, HWIO_GCC_PCIE_PIPE_CBCR_RMSK)
#define HWIO_GCC_PCIE_PIPE_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_PIPE_CBCR_ADDR, m)
#define HWIO_GCC_PCIE_PIPE_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_PIPE_CBCR_ADDR,v)
#define HWIO_GCC_PCIE_PIPE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_PIPE_CBCR_ADDR,m,v,HWIO_GCC_PCIE_PIPE_CBCR_IN)
#define HWIO_GCC_PCIE_PIPE_CBCR_CLK_OFF_BMSK                                                      0x80000000
#define HWIO_GCC_PCIE_PIPE_CBCR_CLK_OFF_SHFT                                                            0x1f
#define HWIO_GCC_PCIE_PIPE_CBCR_FORCE_MEM_CORE_ON_BMSK                                                0x4000
#define HWIO_GCC_PCIE_PIPE_CBCR_FORCE_MEM_CORE_ON_SHFT                                                   0xe
#define HWIO_GCC_PCIE_PIPE_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                              0x2000
#define HWIO_GCC_PCIE_PIPE_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                                 0xd
#define HWIO_GCC_PCIE_PIPE_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                             0x1000
#define HWIO_GCC_PCIE_PIPE_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                                0xc
#define HWIO_GCC_PCIE_PIPE_CBCR_WAKEUP_BMSK                                                            0xf00
#define HWIO_GCC_PCIE_PIPE_CBCR_WAKEUP_SHFT                                                              0x8
#define HWIO_GCC_PCIE_PIPE_CBCR_SLEEP_BMSK                                                              0xf0
#define HWIO_GCC_PCIE_PIPE_CBCR_SLEEP_SHFT                                                               0x4
#define HWIO_GCC_PCIE_PIPE_CBCR_CLK_ENABLE_BMSK                                                          0x1
#define HWIO_GCC_PCIE_PIPE_CBCR_CLK_ENABLE_SHFT                                                          0x0

#define HWIO_GCC_PCIE_AXI_CBCR_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0005d010)
#define HWIO_GCC_PCIE_AXI_CBCR_RMSK                                                               0xf000fff1
#define HWIO_GCC_PCIE_AXI_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PCIE_AXI_CBCR_ADDR, HWIO_GCC_PCIE_AXI_CBCR_RMSK)
#define HWIO_GCC_PCIE_AXI_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_AXI_CBCR_ADDR, m)
#define HWIO_GCC_PCIE_AXI_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_AXI_CBCR_ADDR,v)
#define HWIO_GCC_PCIE_AXI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_AXI_CBCR_ADDR,m,v,HWIO_GCC_PCIE_AXI_CBCR_IN)
#define HWIO_GCC_PCIE_AXI_CBCR_CLK_OFF_BMSK                                                       0x80000000
#define HWIO_GCC_PCIE_AXI_CBCR_CLK_OFF_SHFT                                                             0x1f
#define HWIO_GCC_PCIE_AXI_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                      0x70000000
#define HWIO_GCC_PCIE_AXI_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                            0x1c
#define HWIO_GCC_PCIE_AXI_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                              0x8000
#define HWIO_GCC_PCIE_AXI_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                                 0xf
#define HWIO_GCC_PCIE_AXI_CBCR_FORCE_MEM_CORE_ON_BMSK                                                 0x4000
#define HWIO_GCC_PCIE_AXI_CBCR_FORCE_MEM_CORE_ON_SHFT                                                    0xe
#define HWIO_GCC_PCIE_AXI_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                               0x2000
#define HWIO_GCC_PCIE_AXI_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                                  0xd
#define HWIO_GCC_PCIE_AXI_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                              0x1000
#define HWIO_GCC_PCIE_AXI_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                                 0xc
#define HWIO_GCC_PCIE_AXI_CBCR_WAKEUP_BMSK                                                             0xf00
#define HWIO_GCC_PCIE_AXI_CBCR_WAKEUP_SHFT                                                               0x8
#define HWIO_GCC_PCIE_AXI_CBCR_SLEEP_BMSK                                                               0xf0
#define HWIO_GCC_PCIE_AXI_CBCR_SLEEP_SHFT                                                                0x4
#define HWIO_GCC_PCIE_AXI_CBCR_CLK_ENABLE_BMSK                                                           0x1
#define HWIO_GCC_PCIE_AXI_CBCR_CLK_ENABLE_SHFT                                                           0x0

#define HWIO_GCC_PCIE_SLEEP_CBCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0005d014)
#define HWIO_GCC_PCIE_SLEEP_CBCR_RMSK                                                             0x80000001
#define HWIO_GCC_PCIE_SLEEP_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PCIE_SLEEP_CBCR_ADDR, HWIO_GCC_PCIE_SLEEP_CBCR_RMSK)
#define HWIO_GCC_PCIE_SLEEP_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_SLEEP_CBCR_ADDR, m)
#define HWIO_GCC_PCIE_SLEEP_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_SLEEP_CBCR_ADDR,v)
#define HWIO_GCC_PCIE_SLEEP_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_SLEEP_CBCR_ADDR,m,v,HWIO_GCC_PCIE_SLEEP_CBCR_IN)
#define HWIO_GCC_PCIE_SLEEP_CBCR_CLK_OFF_BMSK                                                     0x80000000
#define HWIO_GCC_PCIE_SLEEP_CBCR_CLK_OFF_SHFT                                                           0x1f
#define HWIO_GCC_PCIE_SLEEP_CBCR_CLK_ENABLE_BMSK                                                         0x1
#define HWIO_GCC_PCIE_SLEEP_CBCR_CLK_ENABLE_SHFT                                                         0x0

#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x0005d018)
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_RMSK                                                          0x80007ff1
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PCIE_AXI_MSTR_CBCR_ADDR, HWIO_GCC_PCIE_AXI_MSTR_CBCR_RMSK)
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_AXI_MSTR_CBCR_ADDR, m)
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_AXI_MSTR_CBCR_ADDR,v)
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_AXI_MSTR_CBCR_ADDR,m,v,HWIO_GCC_PCIE_AXI_MSTR_CBCR_IN)
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_CLK_OFF_BMSK                                                  0x80000000
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_CLK_OFF_SHFT                                                        0x1f
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_FORCE_MEM_CORE_ON_BMSK                                            0x4000
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_FORCE_MEM_CORE_ON_SHFT                                               0xe
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                          0x2000
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                             0xd
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                         0x1000
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                            0xc
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_WAKEUP_BMSK                                                        0xf00
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_WAKEUP_SHFT                                                          0x8
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_SLEEP_BMSK                                                          0xf0
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_SLEEP_SHFT                                                           0x4
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_CLK_ENABLE_BMSK                                                      0x1
#define HWIO_GCC_PCIE_AXI_MSTR_CBCR_CLK_ENABLE_SHFT                                                      0x0

#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x0005d01c)
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_RMSK                                                          0x80000013
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_PCIE_PIPE_CMD_RCGR_ADDR, HWIO_GCC_PCIE_PIPE_CMD_RCGR_RMSK)
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_PIPE_CMD_RCGR_ADDR, m)
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_PIPE_CMD_RCGR_ADDR,v)
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_PIPE_CMD_RCGR_ADDR,m,v,HWIO_GCC_PCIE_PIPE_CMD_RCGR_IN)
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_ROOT_OFF_BMSK                                                 0x80000000
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_ROOT_OFF_SHFT                                                       0x1f
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                 0x10
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                  0x4
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_ROOT_EN_BMSK                                                         0x2
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_ROOT_EN_SHFT                                                         0x1
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_UPDATE_BMSK                                                          0x1
#define HWIO_GCC_PCIE_PIPE_CMD_RCGR_UPDATE_SHFT                                                          0x0

#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x0005d020)
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_RMSK                                                               0x71f
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_PCIE_PIPE_CFG_RCGR_ADDR, HWIO_GCC_PCIE_PIPE_CFG_RCGR_RMSK)
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_PIPE_CFG_RCGR_ADDR, m)
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_PIPE_CFG_RCGR_ADDR,v)
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_PIPE_CFG_RCGR_ADDR,m,v,HWIO_GCC_PCIE_PIPE_CFG_RCGR_IN)
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_SEL_BMSK                                                       0x700
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_SEL_SHFT                                                         0x8
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_BMSK                                                        0x1f
#define HWIO_GCC_PCIE_PIPE_CFG_RCGR_SRC_DIV_SHFT                                                         0x0

#define HWIO_GCC_PCIE_AUX_CMD_RCGR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x0005d030)
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_RMSK                                                           0x800000f3
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_PCIE_AUX_CMD_RCGR_ADDR, HWIO_GCC_PCIE_AUX_CMD_RCGR_RMSK)
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_AUX_CMD_RCGR_ADDR, m)
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_AUX_CMD_RCGR_ADDR,v)
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_AUX_CMD_RCGR_ADDR,m,v,HWIO_GCC_PCIE_AUX_CMD_RCGR_IN)
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_ROOT_OFF_BMSK                                                  0x80000000
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_ROOT_OFF_SHFT                                                        0x1f
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_DIRTY_D_BMSK                                                         0x80
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_DIRTY_D_SHFT                                                          0x7
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_DIRTY_M_BMSK                                                         0x40
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_DIRTY_M_SHFT                                                          0x6
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_DIRTY_N_BMSK                                                         0x20
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_DIRTY_N_SHFT                                                          0x5
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                                  0x10
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                                   0x4
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_ROOT_EN_BMSK                                                          0x2
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_ROOT_EN_SHFT                                                          0x1
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_UPDATE_BMSK                                                           0x1
#define HWIO_GCC_PCIE_AUX_CMD_RCGR_UPDATE_SHFT                                                           0x0

#define HWIO_GCC_PCIE_AUX_CFG_RCGR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x0005d034)
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_RMSK                                                               0x371f
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_PCIE_AUX_CFG_RCGR_ADDR, HWIO_GCC_PCIE_AUX_CFG_RCGR_RMSK)
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_AUX_CFG_RCGR_ADDR, m)
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_AUX_CFG_RCGR_ADDR,v)
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_AUX_CFG_RCGR_ADDR,m,v,HWIO_GCC_PCIE_AUX_CFG_RCGR_IN)
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_MODE_BMSK                                                          0x3000
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_MODE_SHFT                                                             0xc
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_SEL_BMSK                                                        0x700
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_SEL_SHFT                                                          0x8
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_BMSK                                                         0x1f
#define HWIO_GCC_PCIE_AUX_CFG_RCGR_SRC_DIV_SHFT                                                          0x0

#define HWIO_GCC_PCIE_AUX_M_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0005d038)
#define HWIO_GCC_PCIE_AUX_M_RMSK                                                                      0xffff
#define HWIO_GCC_PCIE_AUX_M_IN          \
        in_dword_masked(HWIO_GCC_PCIE_AUX_M_ADDR, HWIO_GCC_PCIE_AUX_M_RMSK)
#define HWIO_GCC_PCIE_AUX_M_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_AUX_M_ADDR, m)
#define HWIO_GCC_PCIE_AUX_M_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_AUX_M_ADDR,v)
#define HWIO_GCC_PCIE_AUX_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_AUX_M_ADDR,m,v,HWIO_GCC_PCIE_AUX_M_IN)
#define HWIO_GCC_PCIE_AUX_M_M_BMSK                                                                    0xffff
#define HWIO_GCC_PCIE_AUX_M_M_SHFT                                                                       0x0

#define HWIO_GCC_PCIE_AUX_N_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0005d03c)
#define HWIO_GCC_PCIE_AUX_N_RMSK                                                                      0xffff
#define HWIO_GCC_PCIE_AUX_N_IN          \
        in_dword_masked(HWIO_GCC_PCIE_AUX_N_ADDR, HWIO_GCC_PCIE_AUX_N_RMSK)
#define HWIO_GCC_PCIE_AUX_N_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_AUX_N_ADDR, m)
#define HWIO_GCC_PCIE_AUX_N_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_AUX_N_ADDR,v)
#define HWIO_GCC_PCIE_AUX_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_AUX_N_ADDR,m,v,HWIO_GCC_PCIE_AUX_N_IN)
#define HWIO_GCC_PCIE_AUX_N_NOT_N_MINUS_M_BMSK                                                        0xffff
#define HWIO_GCC_PCIE_AUX_N_NOT_N_MINUS_M_SHFT                                                           0x0

#define HWIO_GCC_PCIE_AUX_D_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0005d040)
#define HWIO_GCC_PCIE_AUX_D_RMSK                                                                      0xffff
#define HWIO_GCC_PCIE_AUX_D_IN          \
        in_dword_masked(HWIO_GCC_PCIE_AUX_D_ADDR, HWIO_GCC_PCIE_AUX_D_RMSK)
#define HWIO_GCC_PCIE_AUX_D_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_AUX_D_ADDR, m)
#define HWIO_GCC_PCIE_AUX_D_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_AUX_D_ADDR,v)
#define HWIO_GCC_PCIE_AUX_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_AUX_D_ADDR,m,v,HWIO_GCC_PCIE_AUX_D_IN)
#define HWIO_GCC_PCIE_AUX_D_NOT_2D_BMSK                                                               0xffff
#define HWIO_GCC_PCIE_AUX_D_NOT_2D_SHFT                                                                  0x0

#define HWIO_GCC_PCIE_GDSCR_ADDR                                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0005d044)
#define HWIO_GCC_PCIE_GDSCR_RMSK                                                                  0xf8ffffff
#define HWIO_GCC_PCIE_GDSCR_IN          \
        in_dword_masked(HWIO_GCC_PCIE_GDSCR_ADDR, HWIO_GCC_PCIE_GDSCR_RMSK)
#define HWIO_GCC_PCIE_GDSCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_GDSCR_ADDR, m)
#define HWIO_GCC_PCIE_GDSCR_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_GDSCR_ADDR,v)
#define HWIO_GCC_PCIE_GDSCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_GDSCR_ADDR,m,v,HWIO_GCC_PCIE_GDSCR_IN)
#define HWIO_GCC_PCIE_GDSCR_PWR_ON_BMSK                                                           0x80000000
#define HWIO_GCC_PCIE_GDSCR_PWR_ON_SHFT                                                                 0x1f
#define HWIO_GCC_PCIE_GDSCR_GDSC_STATE_BMSK                                                       0x78000000
#define HWIO_GCC_PCIE_GDSCR_GDSC_STATE_SHFT                                                             0x1b
#define HWIO_GCC_PCIE_GDSCR_EN_REST_WAIT_BMSK                                                       0xf00000
#define HWIO_GCC_PCIE_GDSCR_EN_REST_WAIT_SHFT                                                           0x14
#define HWIO_GCC_PCIE_GDSCR_EN_FEW_WAIT_BMSK                                                         0xf0000
#define HWIO_GCC_PCIE_GDSCR_EN_FEW_WAIT_SHFT                                                            0x10
#define HWIO_GCC_PCIE_GDSCR_CLK_DIS_WAIT_BMSK                                                         0xf000
#define HWIO_GCC_PCIE_GDSCR_CLK_DIS_WAIT_SHFT                                                            0xc
#define HWIO_GCC_PCIE_GDSCR_RETAIN_FF_ENABLE_BMSK                                                      0x800
#define HWIO_GCC_PCIE_GDSCR_RETAIN_FF_ENABLE_SHFT                                                        0xb
#define HWIO_GCC_PCIE_GDSCR_RESTORE_BMSK                                                               0x400
#define HWIO_GCC_PCIE_GDSCR_RESTORE_SHFT                                                                 0xa
#define HWIO_GCC_PCIE_GDSCR_SAVE_BMSK                                                                  0x200
#define HWIO_GCC_PCIE_GDSCR_SAVE_SHFT                                                                    0x9
#define HWIO_GCC_PCIE_GDSCR_RETAIN_BMSK                                                                0x100
#define HWIO_GCC_PCIE_GDSCR_RETAIN_SHFT                                                                  0x8
#define HWIO_GCC_PCIE_GDSCR_EN_REST_BMSK                                                                0x80
#define HWIO_GCC_PCIE_GDSCR_EN_REST_SHFT                                                                 0x7
#define HWIO_GCC_PCIE_GDSCR_EN_FEW_BMSK                                                                 0x40
#define HWIO_GCC_PCIE_GDSCR_EN_FEW_SHFT                                                                  0x6
#define HWIO_GCC_PCIE_GDSCR_CLAMP_IO_BMSK                                                               0x20
#define HWIO_GCC_PCIE_GDSCR_CLAMP_IO_SHFT                                                                0x5
#define HWIO_GCC_PCIE_GDSCR_CLK_DISABLE_BMSK                                                            0x10
#define HWIO_GCC_PCIE_GDSCR_CLK_DISABLE_SHFT                                                             0x4
#define HWIO_GCC_PCIE_GDSCR_PD_ARES_BMSK                                                                 0x8
#define HWIO_GCC_PCIE_GDSCR_PD_ARES_SHFT                                                                 0x3
#define HWIO_GCC_PCIE_GDSCR_SW_OVERRIDE_BMSK                                                             0x4
#define HWIO_GCC_PCIE_GDSCR_SW_OVERRIDE_SHFT                                                             0x2
#define HWIO_GCC_PCIE_GDSCR_HW_CONTROL_BMSK                                                              0x2
#define HWIO_GCC_PCIE_GDSCR_HW_CONTROL_SHFT                                                              0x1
#define HWIO_GCC_PCIE_GDSCR_SW_COLLAPSE_BMSK                                                             0x1
#define HWIO_GCC_PCIE_GDSCR_SW_COLLAPSE_SHFT                                                             0x0

#define HWIO_GCC_PCIEPHY_PHY_BCR_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0005d048)
#define HWIO_GCC_PCIEPHY_PHY_BCR_RMSK                                                                    0x1
#define HWIO_GCC_PCIEPHY_PHY_BCR_IN          \
        in_dword_masked(HWIO_GCC_PCIEPHY_PHY_BCR_ADDR, HWIO_GCC_PCIEPHY_PHY_BCR_RMSK)
#define HWIO_GCC_PCIEPHY_PHY_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIEPHY_PHY_BCR_ADDR, m)
#define HWIO_GCC_PCIEPHY_PHY_BCR_OUT(v)      \
        out_dword(HWIO_GCC_PCIEPHY_PHY_BCR_ADDR,v)
#define HWIO_GCC_PCIEPHY_PHY_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIEPHY_PHY_BCR_ADDR,m,v,HWIO_GCC_PCIEPHY_PHY_BCR_IN)
#define HWIO_GCC_PCIEPHY_PHY_BCR_BLK_ARES_BMSK                                                           0x1
#define HWIO_GCC_PCIEPHY_PHY_BCR_BLK_ARES_SHFT                                                           0x0

#define HWIO_GCC_PCIE_GPIO_LDO_EN_ADDR                                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0005d04c)
#define HWIO_GCC_PCIE_GPIO_LDO_EN_RMSK                                                                   0x1
#define HWIO_GCC_PCIE_GPIO_LDO_EN_IN          \
        in_dword_masked(HWIO_GCC_PCIE_GPIO_LDO_EN_ADDR, HWIO_GCC_PCIE_GPIO_LDO_EN_RMSK)
#define HWIO_GCC_PCIE_GPIO_LDO_EN_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_GPIO_LDO_EN_ADDR, m)
#define HWIO_GCC_PCIE_GPIO_LDO_EN_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_GPIO_LDO_EN_ADDR,v)
#define HWIO_GCC_PCIE_GPIO_LDO_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_GPIO_LDO_EN_ADDR,m,v,HWIO_GCC_PCIE_GPIO_LDO_EN_IN)
#define HWIO_GCC_PCIE_GPIO_LDO_EN_LDO_ENABLE_BMSK                                                        0x1
#define HWIO_GCC_PCIE_GPIO_LDO_EN_LDO_ENABLE_SHFT                                                        0x0

#define HWIO_GCC_PCIE_PHY_BCR_ADDR                                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x0005d050)
#define HWIO_GCC_PCIE_PHY_BCR_RMSK                                                                       0x1
#define HWIO_GCC_PCIE_PHY_BCR_IN          \
        in_dword_masked(HWIO_GCC_PCIE_PHY_BCR_ADDR, HWIO_GCC_PCIE_PHY_BCR_RMSK)
#define HWIO_GCC_PCIE_PHY_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_PHY_BCR_ADDR, m)
#define HWIO_GCC_PCIE_PHY_BCR_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_PHY_BCR_ADDR,v)
#define HWIO_GCC_PCIE_PHY_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_PHY_BCR_ADDR,m,v,HWIO_GCC_PCIE_PHY_BCR_IN)
#define HWIO_GCC_PCIE_PHY_BCR_BLK_ARES_BMSK                                                              0x1
#define HWIO_GCC_PCIE_PHY_BCR_BLK_ARES_SHFT                                                              0x0

#define HWIO_GCC_PCIE_MISC_RESET_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0005d054)
#define HWIO_GCC_PCIE_MISC_RESET_RMSK                                                                   0x7f
#define HWIO_GCC_PCIE_MISC_RESET_IN          \
        in_dword_masked(HWIO_GCC_PCIE_MISC_RESET_ADDR, HWIO_GCC_PCIE_MISC_RESET_RMSK)
#define HWIO_GCC_PCIE_MISC_RESET_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_MISC_RESET_ADDR, m)
#define HWIO_GCC_PCIE_MISC_RESET_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_MISC_RESET_ADDR,v)
#define HWIO_GCC_PCIE_MISC_RESET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_MISC_RESET_ADDR,m,v,HWIO_GCC_PCIE_MISC_RESET_IN)
#define HWIO_GCC_PCIE_MISC_RESET_AXI_MASTER_STICKY_ARES_BMSK                                            0x40
#define HWIO_GCC_PCIE_MISC_RESET_AXI_MASTER_STICKY_ARES_SHFT                                             0x6
#define HWIO_GCC_PCIE_MISC_RESET_AHB_ARES_BMSK                                                          0x20
#define HWIO_GCC_PCIE_MISC_RESET_AHB_ARES_SHFT                                                           0x5
#define HWIO_GCC_PCIE_MISC_RESET_AXI_SLAVE_ARES_BMSK                                                    0x10
#define HWIO_GCC_PCIE_MISC_RESET_AXI_SLAVE_ARES_SHFT                                                     0x4
#define HWIO_GCC_PCIE_MISC_RESET_AXI_MASTER_ARES_BMSK                                                    0x8
#define HWIO_GCC_PCIE_MISC_RESET_AXI_MASTER_ARES_SHFT                                                    0x3
#define HWIO_GCC_PCIE_MISC_RESET_CORE_STICKY_ARES_BMSK                                                   0x4
#define HWIO_GCC_PCIE_MISC_RESET_CORE_STICKY_ARES_SHFT                                                   0x2
#define HWIO_GCC_PCIE_MISC_RESET_SLEEP_ARES_BMSK                                                         0x2
#define HWIO_GCC_PCIE_MISC_RESET_SLEEP_ARES_SHFT                                                         0x1
#define HWIO_GCC_PCIE_MISC_RESET_PIPE_ARES_BMSK                                                          0x1
#define HWIO_GCC_PCIE_MISC_RESET_PIPE_ARES_SHFT                                                          0x0

#define HWIO_GCC_PCIE_LINK_DOWN_BCR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x0005d05c)
#define HWIO_GCC_PCIE_LINK_DOWN_BCR_RMSK                                                                 0x1
#define HWIO_GCC_PCIE_LINK_DOWN_BCR_IN          \
        in_dword_masked(HWIO_GCC_PCIE_LINK_DOWN_BCR_ADDR, HWIO_GCC_PCIE_LINK_DOWN_BCR_RMSK)
#define HWIO_GCC_PCIE_LINK_DOWN_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PCIE_LINK_DOWN_BCR_ADDR, m)
#define HWIO_GCC_PCIE_LINK_DOWN_BCR_OUT(v)      \
        out_dword(HWIO_GCC_PCIE_LINK_DOWN_BCR_ADDR,v)
#define HWIO_GCC_PCIE_LINK_DOWN_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_PCIE_LINK_DOWN_BCR_ADDR,m,v,HWIO_GCC_PCIE_LINK_DOWN_BCR_IN)
#define HWIO_GCC_PCIE_LINK_DOWN_BCR_BLK_ARES_BMSK                                                        0x1
#define HWIO_GCC_PCIE_LINK_DOWN_BCR_BLK_ARES_SHFT                                                        0x0

#define HWIO_GCC_USB_SS_LDO_EN_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x0005e07c)
#define HWIO_GCC_USB_SS_LDO_EN_RMSK                                                                      0x1
#define HWIO_GCC_USB_SS_LDO_EN_IN          \
        in_dword_masked(HWIO_GCC_USB_SS_LDO_EN_ADDR, HWIO_GCC_USB_SS_LDO_EN_RMSK)
#define HWIO_GCC_USB_SS_LDO_EN_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_SS_LDO_EN_ADDR, m)
#define HWIO_GCC_USB_SS_LDO_EN_OUT(v)      \
        out_dword(HWIO_GCC_USB_SS_LDO_EN_ADDR,v)
#define HWIO_GCC_USB_SS_LDO_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_SS_LDO_EN_ADDR,m,v,HWIO_GCC_USB_SS_LDO_EN_IN)
#define HWIO_GCC_USB_SS_LDO_EN_LDO_ENABLE_BMSK                                                           0x1
#define HWIO_GCC_USB_SS_LDO_EN_LDO_ENABLE_SHFT                                                           0x0

#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_ADDR                                                        (GCC_CLK_CTL_REG_REG_BASE      + 0x0005e080)
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_RMSK                                                        0xf0008001
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_USB_PHY_CFG_AHB_CBCR_ADDR, HWIO_GCC_USB_PHY_CFG_AHB_CBCR_RMSK)
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_USB_PHY_CFG_AHB_CBCR_ADDR, m)
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_USB_PHY_CFG_AHB_CBCR_ADDR,v)
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_USB_PHY_CFG_AHB_CBCR_ADDR,m,v,HWIO_GCC_USB_PHY_CFG_AHB_CBCR_IN)
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_CLK_OFF_BMSK                                                0x80000000
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_CLK_OFF_SHFT                                                      0x1f
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                               0x70000000
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                     0x1c
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                       0x8000
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                          0xf
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_CLK_ENABLE_BMSK                                                    0x1
#define HWIO_GCC_USB_PHY_CFG_AHB_CBCR_CLK_ENABLE_SHFT                                                    0x0

#define HWIO_GCC_SYS_NOC_USB3_AXI_CBCR_ADDR                                                       (GCC_CLK_CTL_REG_REG_BASE      + 0x0005e084)
#define HWIO_GCC_SYS_NOC_USB3_AXI_CBCR_RMSK                                                       0x80000001
#define HWIO_GCC_SYS_NOC_USB3_AXI_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SYS_NOC_USB3_AXI_CBCR_ADDR, HWIO_GCC_SYS_NOC_USB3_AXI_CBCR_RMSK)
#define HWIO_GCC_SYS_NOC_USB3_AXI_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SYS_NOC_USB3_AXI_CBCR_ADDR, m)
#define HWIO_GCC_SYS_NOC_USB3_AXI_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SYS_NOC_USB3_AXI_CBCR_ADDR,v)
#define HWIO_GCC_SYS_NOC_USB3_AXI_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SYS_NOC_USB3_AXI_CBCR_ADDR,m,v,HWIO_GCC_SYS_NOC_USB3_AXI_CBCR_IN)
#define HWIO_GCC_SYS_NOC_USB3_AXI_CBCR_CLK_OFF_BMSK                                               0x80000000
#define HWIO_GCC_SYS_NOC_USB3_AXI_CBCR_CLK_OFF_SHFT                                                     0x1f
#define HWIO_GCC_SYS_NOC_USB3_AXI_CBCR_CLK_ENABLE_BMSK                                                   0x1
#define HWIO_GCC_SYS_NOC_USB3_AXI_CBCR_CLK_ENABLE_SHFT                                                   0x0

#ifdef PCIE_BOOT
/*----------------------------------------------------------------------------
 * BASE: IPA_WRAPPER
 *--------------------------------------------------------------------------*/

#define IPA_WRAPPER_BASE                                                       0x07920000
#define IPA_WRAPPER_BASE_PHYS                                                  0x07920000

#define IPA_REG_BASE                                                           (IPA_WRAPPER_BASE      + 0x00020000)
#define IPA_REG_BASE_PHYS                                                      (IPA_WRAPPER_BASE_PHYS + 0x00020000)
#define IPA_REG_BASE_OFFS                                                      0x00020000

#define HWIO_IPA_COMP_SW_RESET_ADDR                                            (IPA_REG_BASE      + 0x0000003c)
#define HWIO_IPA_COMP_SW_RESET_PHYS                                            (IPA_REG_BASE_PHYS + 0x0000003c)
#define HWIO_IPA_COMP_SW_RESET_OFFS                                            (IPA_REG_BASE_OFFS + 0x0000003c)
#define HWIO_IPA_COMP_SW_RESET_RMSK                                                   0x1
#define HWIO_IPA_COMP_SW_RESET_POR                                             0x00000000
#define HWIO_IPA_COMP_SW_RESET_ATTR                                                   0x2
#define HWIO_IPA_COMP_SW_RESET_OUT(v)      \
        out_dword(HWIO_IPA_COMP_SW_RESET_ADDR,v)
#define HWIO_IPA_COMP_SW_RESET_SW_RESET_BMSK                                          0x1
#define HWIO_IPA_COMP_SW_RESET_SW_RESET_SHFT                                          0x0


#define HWIO_IPA_ENDP_INIT_MODE_n_ADDR(n)                                      (IPA_REG_BASE      + 0x000002c0 + 0x4 * (n))
#define HWIO_IPA_ENDP_INIT_MODE_n_PHYS(n)                                      (IPA_REG_BASE_PHYS + 0x000002c0 + 0x4 * (n))
#define HWIO_IPA_ENDP_INIT_MODE_n_OFFS(n)                                      (IPA_REG_BASE_OFFS + 0x000002c0 + 0x4 * (n))
#define HWIO_IPA_ENDP_INIT_MODE_n_RMSK                                          0xffff1f3
#define HWIO_IPA_ENDP_INIT_MODE_n_MAXn                                                 19
#define HWIO_IPA_ENDP_INIT_MODE_n_POR                                          0x00000000
#define HWIO_IPA_ENDP_INIT_MODE_n_ATTR                                                0x3
#define HWIO_IPA_ENDP_INIT_MODE_n_INI(n)        \
        in_dword_masked(HWIO_IPA_ENDP_INIT_MODE_n_ADDR(n), HWIO_IPA_ENDP_INIT_MODE_n_RMSK)
#define HWIO_IPA_ENDP_INIT_MODE_n_INMI(n,mask)    \
        in_dword_masked(HWIO_IPA_ENDP_INIT_MODE_n_ADDR(n), mask)
#define HWIO_IPA_ENDP_INIT_MODE_n_OUTI(n,val)    \
        out_dword(HWIO_IPA_ENDP_INIT_MODE_n_ADDR(n),val)
#define HWIO_IPA_ENDP_INIT_MODE_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_IPA_ENDP_INIT_MODE_n_ADDR(n),mask,val,HWIO_IPA_ENDP_INIT_MODE_n_INI(n))
#define HWIO_IPA_ENDP_INIT_MODE_n_BYTE_THRESHOLD_BMSK                           0xffff000
#define HWIO_IPA_ENDP_INIT_MODE_n_BYTE_THRESHOLD_SHFT                                 0xc
#define HWIO_IPA_ENDP_INIT_MODE_n_DEST_PIPE_INDEX_BMSK                              0x1f0
#define HWIO_IPA_ENDP_INIT_MODE_n_DEST_PIPE_INDEX_SHFT                                0x4
#define HWIO_IPA_ENDP_INIT_MODE_n_MODE_BMSK                                           0x3
#define HWIO_IPA_ENDP_INIT_MODE_n_MODE_SHFT                                           0x0


#define HWIO_IPA_COMP_CFG_ADDR                                                 (IPA_REG_BASE      + 0x00000038)
#define HWIO_IPA_COMP_CFG_PHYS                                                 (IPA_REG_BASE_PHYS + 0x00000038)
#define HWIO_IPA_COMP_CFG_OFFS                                                 (IPA_REG_BASE_OFFS + 0x00000038)
#define HWIO_IPA_COMP_CFG_RMSK                                                        0x3
#define HWIO_IPA_COMP_CFG_POR                                                  0x00000000
#define HWIO_IPA_COMP_CFG_ATTR                                                        0x3
#define HWIO_IPA_COMP_CFG_IN          \
        in_dword_masked(HWIO_IPA_COMP_CFG_ADDR, HWIO_IPA_COMP_CFG_RMSK)
#define HWIO_IPA_COMP_CFG_INM(m)      \
        in_dword_masked(HWIO_IPA_COMP_CFG_ADDR, m)
#define HWIO_IPA_COMP_CFG_OUT(v)      \
        out_dword(HWIO_IPA_COMP_CFG_ADDR,v)
#define HWIO_IPA_COMP_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_IPA_COMP_CFG_ADDR,m,v,HWIO_IPA_COMP_CFG_IN)
#define HWIO_IPA_COMP_CFG_BAM_SNOC_BYPASS_DIS_BMSK                                    0x2
#define HWIO_IPA_COMP_CFG_BAM_SNOC_BYPASS_DIS_SHFT                                    0x1
#define HWIO_IPA_COMP_CFG_ENABLE_BMSK                                                 0x1
#define HWIO_IPA_COMP_CFG_ENABLE_SHFT                                                 0x0
#endif /* PCIE_BOOT */

#endif /* __PCIE_TARGET_HWIO_H__ */
