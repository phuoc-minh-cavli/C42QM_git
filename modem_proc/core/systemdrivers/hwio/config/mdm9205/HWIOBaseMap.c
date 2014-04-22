
/*
===========================================================================
*/
/**
  @file HWIOBaseMap.c
  @brief Auto-generated HWIO Device Configuration base file.

  DESCRIPTION:
    This file contains Device Configuration data structures for mapping
    physical and virtual memory for HWIO blocks.
*/
/*
  ===========================================================================

  Copyright (c) 2019 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies Incorporated and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies Incorporated.

  ===========================================================================

  $Header: //components/rel/core.mpss/3.10/systemdrivers/hwio/config/mdm9205/HWIOBaseMap.c#6 $
  $DateTime: 2019/01/30 02:50:00 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*=========================================================================
      Include Files
==========================================================================*/

#include "DalHWIO.h"


/*=========================================================================
      Data Definitions
==========================================================================*/

static HWIOModuleType HWIOModules_XPU_CFG_PCNOC_CFG_MPU1132_4_M19L12_AHB[] =
{
  { "XPU_CFG_PCNOC_CFG_MPU1132_4_M19L12_AHB",      0x00000000, 0x00000400 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_BIMC_MPU_MPU1132_32_M31L10_AHB[] =
{
  { "BIMC_MPU_MPU1132_32_M31L10_AHB",              0x00000000, 0x00001200 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_XPU_CFG_RPM_CFG_MPU1132_2_M21L12_AHB[] =
{
  { "XPU_CFG_RPM_CFG_MPU1132_2_M21L12_AHB",        0x00000000, 0x00000300 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_OCIMEM_WRAPPER_CSR[] =
{
  { "OCIMEM_CSR",                                  0x00000000, 0x00000800 },
  { "OCIMEM_MPU",                                  0x00003000, 0x00000600 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_RPM_SS_MSG_RAM_START_ADDRESS[] =
{
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_MPU_BOOT_IMEM_CFG_MPU1132A_4_M31L12_AHB[] =
{
  { "MPU_BOOT_IMEM_CFG_MPU1132A_4_M31L12_AHB",     0x00000000, 0x00000400 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_MPU_MSS_Q6_CFG_MPU1132A_20_M31L12_AXI[] =
{
  { "MPU_MSS_Q6_CFG_MPU1132A_20_M31L12_AXI",       0x00000000, 0x00000c00 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_PCNOC_SNOC_MPU_MPU1132_32_M31L10_AHB[] =
{
  { "PCNOC_SNOC_MPU_MPU1132_32_M31L10_AHB",        0x00000000, 0x00001200 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_VMIDMT_PCNOC_SNOC_2_CFG_VMIDMT_SMR_128_SSD7_SID7_MA128[] =
{
  { "VMIDMT_PCNOC_SNOC_2_CFG_VMIDMT_SMR_128_SSD7_SID7_MA128", 0x00000000, 0x00001000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_SECURITY_CONTROL[] =
{
  { "SECURITY_CONTROL_CORE",                       0x00000000, 0x00007000 },
  { "SECURE_CHANNEL",                              0x00008000, 0x00003000 },
  { "SEC_CTRL_APU_APU1132_19",                     0x0000e000, 0x00000b80 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_QC_DCC_TWIZY_2KB2LL[] =
{
  { "DCC_APU",                                     0x00000000, 0x00000400 },
  { "DCC_CFG",                                     0x00002000, 0x00001000 },
  { "DCC_RAM",                                     0x0000f800, 0x00000800 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_PDM_PERPH_WEB[] =
{
  { "PDM_WEB_PWM",                                 0x00000000, 0x0000c000 },
  { "PDM_WEB_TCXO4",                               0x00010000, 0x00004000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_PRNG_PRNG_TOP[] =
{
  { "PRNG_CM_CM_PRNG_CM",                          0x00000000, 0x00001000 },
  { "PRNG_TZ_TZ_PRNG_TZ",                          0x00001000, 0x00001000 },
  { "PRNG_MSA_MSA_PRNG_SUB",                       0x00002000, 0x00001000 },
  { "PRNG_EE2_EE2_PRNG_SUB",                       0x00003000, 0x00001000 },
  { "PRNG_EE3_EE3_PRNG_SUB",                       0x00004000, 0x00001000 },
  { "PRNG_EE4_EE4_PRNG_SUB",                       0x00005000, 0x00001000 },
  { "PRNG_EE5_EE5_PRNG_SUB",                       0x00006000, 0x00001000 },
  { "PRNG_EE6_EE6_PRNG_SUB",                       0x00007000, 0x00001000 },
  { "PRNG_EE7_EE7_PRNG_SUB",                       0x00008000, 0x00001000 },
  { "PRNG_EE8_EE8_PRNG_SUB",                       0x00009000, 0x00001000 },
  { "PRNG_EE9_EE9_PRNG_SUB",                       0x0000a000, 0x00001000 },
  { "PRNG_EE10_EE10_PRNG_SUB",                     0x0000b000, 0x00001000 },
  { "PRNG_EE11_EE11_PRNG_SUB",                     0x0000c000, 0x00001000 },
  { "PRNG_EE12_EE12_PRNG_SUB",                     0x0000d000, 0x00001000 },
  { "PRNG_EE13_EE13_PRNG_SUB",                     0x0000e000, 0x00001000 },
  { "PRNG_EE14_EE14_PRNG_SUB",                     0x0000f000, 0x00001000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_BOOT_ROM[] =
{
  { "BOOT_ROM_MPU1032_3_M20L12_AHB",               0x000ff000, 0x00000380 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_RPM[] =
{
  { "RPM_CODE",                                    0x00000000, 0x00024000 },
  { "RPM_DATA",                                    0x00090000, 0x00010000 },
  { "RPM_DEC",                                     0x00080000, 0x00002000 },
  { "RPM_QTMR_AC",                                 0x00082000, 0x00001000 },
  { "RPM_F0_QTMR_V1_F0",                           0x00083000, 0x00001000 },
  { "RPM_F1_QTMR_V1_F1",                           0x00084000, 0x00001000 },
  { "RPM_APU",                                     0x00087000, 0x00000300 },
  { "RPM_VMIDMT",                                  0x00088000, 0x00001000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_BIMC[] =
{
  { "BIMC_GLOBAL0",                                0x00000000, 0x00001000 },
  { "BIMC_DTE",                                    0x00006000, 0x0000005c },
  { "BIMC_GLOBAL1",                                0x00001000, 0x00001000 },
  { "BIMC_GLOBAL2",                                0x00002000, 0x00001000 },
  { "BIMC_PERFMON",                                0x00003000, 0x00001000 },
  { "BIMC_M_APP_MPORT",                            0x00008000, 0x00001000 },
  { "BIMC_M_APP_PROF",                             0x00009000, 0x00001000 },
  { "BIMC_M_DSP_MPORT",                            0x0000c000, 0x00001000 },
  { "BIMC_M_DSP_PROF",                             0x0000d000, 0x00001000 },
  { "BIMC_M_MCDMA_MPORT",                          0x00010000, 0x00001000 },
  { "BIMC_M_MCDMA_PROF",                           0x00011000, 0x00001000 },
  { "BIMC_M_SYS_MPORT",                            0x00014000, 0x00001000 },
  { "BIMC_M_SYS_PROF",                             0x00015000, 0x00001000 },
  { "BIMC_S_DDR0_SCMO",                            0x00030000, 0x00001000 },
  { "BIMC_S_DDR0_ARB",                             0x00031000, 0x00001000 },
  { "BIMC_S_SYS_SWAY",                             0x0003c000, 0x00001000 },
  { "BIMC_S_SYS_ARB",                              0x0003d000, 0x00001000 },
  { "BIMC_S_DEFAULT_SWAY",                         0x00044000, 0x00001000 },
  { "BIMC_S_DEFAULT_ARB",                          0x00045000, 0x00001000 },
  { "BIMC_S_DDR0",                                 0x00032000, 0x00001580 },
  { "BIMC_S_DDR0_DPE",                             0x00034000, 0x00001000 },
  { "BIMC_S_DDR0_SHKE",                            0x00035000, 0x00001000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_MPM2_MPM[] =
{
  { "MPM2_MPM",                                    0x00000000, 0x00001000 },
  { "MPM2_G_CTRL_CNTR",                            0x00001000, 0x00001000 },
  { "MPM2_G_RD_CNTR",                              0x00002000, 0x00001000 },
  { "MPM2_SLP_CNTR",                               0x00003000, 0x00001000 },
  { "MPM2_QTIMR_AC",                               0x00004000, 0x00001000 },
  { "MPM2_QTIMR_V1",                               0x00005000, 0x00001000 },
  { "MPM2_TSYNC",                                  0x00006000, 0x00001000 },
  { "MPM2_APU",                                    0x00007000, 0x00000780 },
  { "MPM2_TSENS",                                  0x00008000, 0x00001000 },
  { "MPM2_TSENS_TM",                               0x00009000, 0x00001000 },
  { "MPM2_WDOG",                                   0x0000a000, 0x00000020 },
  { "MPM2_PSHOLD",                                 0x0000b000, 0x00001000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_DEHR_WRAPPER_8K_R01[] =
{
  { "DEHR_BIMC",                                   0x00002000, 0x00002000 },
  { "DEHR_XPU",                                    0x00000000, 0x00000300 },
  { "DEHR_VMIDMT",                                 0x00001000, 0x00001000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_PC_NOC[] =
{
  { "PC_NOC",                                      0x00000000, 0x00017080 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_CRYPTO0_CRYPTO_TOP[] =
{
  { "CRYPTO0_CRYPTO",                              0x0003a000, 0x00006000 },
  { "CRYPTO0_CRYPTO_BAM",                          0x00004000, 0x00024000 },
  { "CRYPTO0_CRYPTO_BAM_XPU2_BAM",                 0x00002000, 0x00002000 },
  { "CRYPTO0_CRYPTO_BAM_VMIDMT_BAM",               0x00000000, 0x00001000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_TLMM[] =
{
  { "TLMM_MPU1132_16_M22L12_AHB",                  0x00300000, 0x00000a00 },
  { "TLMM_CSR",                                    0x00000000, 0x00300000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_CLK_CTL[] =
{
  { "GCC_CLK_CTL_REG",                             0x00000000, 0x00080000 },
  { "GCC_RPU_RPU1132_32_L12",                      0x00080000, 0x00001200 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_CORE_TOP_CSR[] =
{
  { "TCSR_TCSR_MUTEX",                             0x00005000, 0x00020000 },
  { "TCSR_REGS_APU1132_16",                        0x00036000, 0x00000a00 },
  { "TCSR_TCSR_REGS",                              0x00037000, 0x00021000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_PMIC_ARB[] =
{
  { "SPMI_CFG_TOP",                                0x00000000, 0x0000d000 },
  { "SPMI_GENI_CFG",                               0x0000a000, 0x00000700 },
  { "SPMI_CFG",                                    0x0000a700, 0x00001a00 },
  { "SPMI_PIC",                                    0x01800000, 0x00200000 },
  { "PMIC_ARB_MPU1132_25_M25L12_AHB",              0x0000e000, 0x00000e6d },
  { "PMIC_ARB_CORE",                               0x0000f000, 0x00001000 },
  { "PMIC_ARB_CORE_REGISTERS",                     0x00400000, 0x00800000 },
  { "PMIC_ARB_CORE_REGISTERS_OBS",                 0x00c00000, 0x00800000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_MSS_TOP[] =
{
  { "MSS_APU0132_5",                               0x00000000, 0x00000480 },
  { "MSS_RMB",                                     0x00020000, 0x00010000 },
  { "MSS_BAM_NDP_AUTO_SCALE_V2_0",                 0x00044000, 0x00019000 },
  { "MSS_QDSP6V67SS_PUB",                          0x00080000, 0x00004040 },
  { "MSS_QDSP6V67SS_PRIVATE",                      0x00100000, 0x00080000 },
  { "MSS_QDSP6V67SS_CSR",                          0x00100000, 0x0000d000 },
  { "MSS_QDSP6V67SS_L2VIC",                        0x00110000, 0x00001000 },
  { "MSS_QDSP6SS_QDSP6SS_QTMR_AC",                 0x00120000, 0x00001000 },
  { "MSS_QDSP6SS_QTMR_F0_0",                       0x00121000, 0x00001000 },
  { "MSS_QDSP6SS_QTMR_F1_1",                       0x00122000, 0x00001000 },
  { "MSS_QDSP6SS_QTMR_F2_2",                       0x00123000, 0x00001000 },
  { "MSS_QDSP6SS_QDSP6V67SS_RSCC",                 0x00130000, 0x00010000 },
  { "MSS_QDSP6SS_RSCC_RSCC_RSC",                   0x00130000, 0x00010000 },
  { "MSS_PERPH",                                   0x00180000, 0x0000f020 },
  { "MSS_UIM0_UART_DM",                            0x00190000, 0x000001c0 },
  { "MSS_CXM_UART_DM",                             0x001a0000, 0x00000200 },
  { "MSS_CM_MANTICORE_ADC",                        0x001a1000, 0x00000100 },
  { "MSS_CONF_BUS_TIMEOUT",                        0x001b0000, 0x00001000 },
  { "MSS_TXDAC_COMP",                              0x001b2000, 0x00001000 },
  { "MODEM_DTR_DAC_CALIB_0",                       0x001b2000, 0x00000400 },
  { "DAC_REGARRAY_0",                              0x001b2400, 0x00000400 },
  { "MSS_MGPI",                                    0x001b3000, 0x00000128 },
  { "MSS_CRYPTO_TOP",                              0x001c0000, 0x00040000 },
  { "MSS_CRYPTO",                                  0x001fa000, 0x00006000 },
  { "MSS_CRYPTO_BAM",                              0x001c4000, 0x00024000 },
  { "MSS_NAV_SS",                                  0x00200000, 0x000f888d },
  { "MSS_NAV_BASE_REGS",                           0x00200000, 0x000f888d },
  { "MSS_NAV_WTR_BASE",                            0x00200000, 0x00001900 },
  { "MSS_NAV_TOP_BASE",                            0x00201900, 0x00000700 },
  { "MSS_NAV_BB_BASE",                             0x00202000, 0x000c6000 },
  { "MODEM_TOP",                                   0x00300000, 0x00090000 },
  { "TX_TOP",                                      0x00380000, 0x00010000 },
  { "TX",                                          0x00380000, 0x00001000 },
  { "TX_MEM",                                      0x00382000, 0x00001000 },
  { "TX_BRDG",                                     0x00383000, 0x00000050 },
  { "TXR_A0",                                      0x00385000, 0x00001000 },
  { "TXC_A0",                                      0x00384000, 0x00001000 },
  { "TXC_MEM",                                     0x00388000, 0x00001e00 },
  { "O_TX",                                        0x0038a000, 0x00000200 },
  { "O_TX_WMORE_TS_TRIF",                          0x0038a200, 0x00000400 },
  { "O_TX_ENC_TS_TRIF",                            0x0038a600, 0x00000200 },
  { "O_TX_MOD_TS_TRIF",                            0x0038a800, 0x00000700 },
  { "RXFE",                                        0x00360000, 0x00020000 },
  { "RXFE_TOP_CFG",                                0x00360000, 0x00001000 },
  { "RXFE_WB_WB0",                                 0x00364000, 0x00001000 },
  { "RXFE_NB_NB0",                                 0x00370000, 0x00001000 },
  { "RXFE_NB_NB1",                                 0x00371000, 0x00001000 },
  { "RXFE_BRDG",                                   0x0037f000, 0x00000100 },
  { "DEMBACK_TOP",                                 0x00340000, 0x00020000 },
  { "DEMBACK_COMMON",                              0x00340000, 0x00000100 },
  { "DEMBACK_BRDG",                                0x00340100, 0x00000100 },
  { "LTE_DEMBACK",                                 0x00341000, 0x00000050 },
  { "CCHP_TOP_RIF_MMAP",                           0x00342000, 0x0000002c },
  { "DB_BUF",                                      0x00344000, 0x00004000 },
  { "LTE_DEMBACK_SCH_TRIF",                        0x00349000, 0x00000300 },
  { "DECOB",                                       0x00350500, 0x000000c1 },
  { "DB_BUF_PAGE",                                 0x0035c000, 0x00004000 },
  { "TDEC_TOP",                                    0x0033c000, 0x00004000 },
  { "TDEC",                                        0x0033c000, 0x000000a8 },
  { "TD_CFG_TRIF",                                 0x0033d000, 0x00000400 },
  { "TD_TRIF",                                     0x0033e000, 0x00000300 },
  { "TDECIB_MEM",                                  0x0033f000, 0x00001000 },
  { "MTC_TOP",                                     0x00300000, 0x0002fffd },
  { "CCS",                                         0x00300000, 0x00020000 },
  { "PDMEM",                                       0x00300000, 0x0000c000 },
  { "CONTROL",                                     0x0031fc00, 0x00000400 },
  { "MTC_CLK",                                     0x00320000, 0x00000400 },
  { "MCDMA",                                       0x00320400, 0x00000400 },
  { "A2",                                          0x00320800, 0x00000400 },
  { "DBG",                                         0x00320c00, 0x00000400 },
  { "MTC_BRDG",                                    0x00321000, 0x00000400 },
  { "A2_MEM",                                      0x00322000, 0x00002000 },
  { "MCDMA_TS_TRIF",                               0x00324000, 0x00000600 },
  { "DBG_TS_TRIF",                                 0x00324800, 0x00000200 },
  { "STMR_EXPY",                                   0x00324c00, 0x00000400 },
  { "ENCRYPT",                                     0x00325000, 0x000000fd },
  { "UNIV_STMR",                                   0x00328800, 0x00000400 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_QDSS_SOC_DBG[] =
{
  { "QDSS_APSS_APSS_A7SS_DEBUG",                   0x00d00000, 0x00004000 },
  { "QDSS_APSS_APCS_APB_APCS_APB_A7CORE_DEBUG",    0x00d00000, 0x00003000 },
  { "QDSS_APSS_APCS_CTI_APCS_CTI_CSCTI",           0x00d03000, 0x00001000 },
  { "QDSS_TPDM_TPDM_TPDM_ATB64_APCLK_DSB256",      0x00c28000, 0x00001000 },
  { "QDSS_TPDM_TPDM_ATB64_APCLK_DSB256_SUB",       0x00c28280, 0x00000d80 },
  { "QDSS_TPDM_TPDM_ATB64_APCLK_DSB256_GPR",       0x00c28000, 0x0000027d },
  { "QDSS_RPM_RPM_CSCTI",                          0x00b00000, 0x00001000 },
  { "QDSS_DCC_DCC_QC_DCC_TWIZY_2KB2LL_APB",        0x00870000, 0x00001000 },
  { "QDSS_DCC_DCC_TPDM_DCC_TPDM_TPDM_ATB8_ATCLK_CMB32_CS6DCF0636", 0x00870000, 0x00001000 },
  { "QDSS_DCC_DCC_TPDM_TPDM_ATB8_ATCLK_CMB32_CS6DCF0636_SUB", 0x00870280, 0x00000d80 },
  { "QDSS_DCC_DCC_TPDM_TPDM_ATB8_ATCLK_CMB32_CS6DCF0636_GPR", 0x00870000, 0x0000027d },
  { "QDSS_MSS_Q6_MSS_Q6_QDSP6V67SS_MSS_APB",       0x00800000, 0x00008000 },
  { "QDSS_MSS_Q6_QDSP6SS_QDSP6SS_Q6V67X_ETM",      0x00800000, 0x00000800 },
  { "QDSS_MSS_Q6_QDSP6SS_QDSP6SS_Q6V67X_ISDB",     0x00801000, 0x00001000 },
  { "QDSS_MSS_Q6_QDSP6SS_QDSP6SS_CSCTI",           0x00803000, 0x00001000 },
  { "QDSS_QDSS",                                   0x00000000, 0x00800000 },
  { "QDSS_QDSS_QDSS_APB",                          0x00000000, 0x00080000 },
  { "QDSS_QDSS_QDSS_CSR",                          0x00001000, 0x00001000 },
  { "QDSS_QDSS_CXSTM_8_32_32_TRUE",                0x00002000, 0x00001000 },
  { "QDSS_QDSS_QDSS_DL_SLV_QDSS_DL_SLV_DRAGONLINK_SLV_NICOBAR_QDSS", 0x00004000, 0x00002000 },
  { "QDSS_QDSS_QDSS_DL_SLV_TPDA_TPDA_TPDA_S16_W64_D8_M64", 0x00004000, 0x00001000 },
  { "QDSS_QDSS_QDSS_DL_SLV_QATBFUNNEL_QATBFUNNEL_QATBFUNNEL_64W8SP", 0x00005000, 0x00001000 },
  { "QDSS_QDSS_CTI0_CTI0_CSCTI",                   0x00010000, 0x00001000 },
  { "QDSS_QDSS_CTI1_CTI1_CSCTI",                   0x00011000, 0x00001000 },
  { "QDSS_QDSS_CTI2_CTI2_CSCTI",                   0x00012000, 0x00001000 },
  { "QDSS_QDSS_CTI3_CTI3_CSCTI",                   0x00013000, 0x00001000 },
  { "QDSS_QDSS_CTI4_CTI4_CSCTI",                   0x00014000, 0x00001000 },
  { "QDSS_QDSS_CTI5_CTI5_CSCTI",                   0x00015000, 0x00001000 },
  { "QDSS_QDSS_CTI6_CTI6_CSCTI",                   0x00016000, 0x00001000 },
  { "QDSS_QDSS_CTI7_CTI7_CSCTI",                   0x00017000, 0x00001000 },
  { "QDSS_QDSS_CTI8_CTI8_CSCTI",                   0x00018000, 0x00001000 },
  { "QDSS_QDSS_CTI9_CTI9_CSCTI",                   0x00019000, 0x00001000 },
  { "QDSS_QDSS_CTI10_CTI10_CSCTI",                 0x0001a000, 0x00001000 },
  { "QDSS_QDSS_CTI11_CTI11_CSCTI",                 0x0001b000, 0x00001000 },
  { "QDSS_QDSS_CTI12_CTI12_CSCTI",                 0x0001c000, 0x00001000 },
  { "QDSS_QDSS_CTI13_CTI13_CSCTI",                 0x0001d000, 0x00001000 },
  { "QDSS_QDSS_CTI14_CTI14_CSCTI",                 0x0001e000, 0x00001000 },
  { "QDSS_QDSS_CTI15_CTI15_CSCTI",                 0x0001f000, 0x00001000 },
  { "QDSS_QDSS_CSTPIU_CSTPIU_CSTPIU",              0x00040000, 0x00001000 },
  { "QDSS_QDSS_IN_FUN0_IN_FUN0_CXATBFUNNEL_128W8SP", 0x00041000, 0x00001000 },
  { "QDSS_QDSS_REPL64_REPL64_CXATBREPLICATOR_64WP", 0x00046000, 0x00001000 },
  { "QDSS_QDSS_ETR_ETR_CXTMC_R64W32D",             0x00048000, 0x00001000 },
  { "QDSS_QDSS_IN_FUN1_IN_FUN1_CXATBFUNNEL_128W8SP", 0x00042000, 0x00001000 },
  { "QDSS_QDSS_MERG_FUN_MERG_FUN_CXATBFUNNEL_128W2SP", 0x00045000, 0x00001000 },
  { "QDSS_QDSS_ETFETB_ETFETB_CXTMC_F128W4K",       0x00047000, 0x00001000 },
  { "QDSS_QDSS_VMIDETR_VMIDETR_VMIDMT_IDX_2_SSD1", 0x00049000, 0x00001000 },
  { "QDSS_QDSS_NDPBAM_NDPBAM_BAM_NDP_TOP_AUTO_SCALE_V2_0", 0x00060000, 0x00019000 },
  { "QDSS_QDSS_NDPBAM_BAM",                        0x00064000, 0x00015000 },
  { "QDSS_QDSS_QDSS_AHB",                          0x00080000, 0x00009000 },
  { "QDSS_QDSS_VMIDDAP_VMIDDAP_VMIDMT_IDX_2_SSD1", 0x00088000, 0x00001000 },
  { "QDSS_QDSS_SPDM_SPDM_SPDM_WRAPPER_TOP",        0x00080000, 0x00008000 },
  { "QDSS_QDSS_SPDM_SPDM_SPDM_SPDM_CREG",          0x00080000, 0x00000120 },
  { "QDSS_QDSS_SPDM_SPDM_SPDM_SPDM_OLEM",          0x00081000, 0x0000015c },
  { "QDSS_QDSS_SPDM_SPDM_SPDM_SPDM_RTEM",          0x00082000, 0x00000318 },
  { "QDSS_QDSS_SPDM_SPDM_SPDM_SPDM_SREG",          0x00084000, 0x00000120 },
  { "QDSS_QDSS_SPDM_SPDM_SPDM_APU0132_1",          0x00086000, 0x00000280 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_ULTAUDIO_CORE[] =
{
  { "ULTAUDIO_CSR",                                0x00000000, 0x00003000 },
  { "ULTAUDIO_LPASS_TCSR",                         0x00000000, 0x00000100 },
  { "LPASS_LPASS_CSR",                             0x00000100, 0x00000f00 },
  { "LPASS_LPASS_CSR_VFR",                         0x00001000, 0x00001000 },
  { "LPASS_LPASS_CSR_IO",                          0x00002000, 0x00001000 },
  { "LPASS_AHBI_TIME",                             0x00005000, 0x00001000 },
  { "LPASS_LPASS_SYNC_WRAPPER",                    0x00006000, 0x00000ffd },
  { "LPASS_AVTIMER",                               0x00006000, 0x00000100 },
  { "LPASS_LPA_IF_ULTAUDIO",                       0x00008000, 0x00010000 },
  { "LPASS_LPASS_LPM",                             0x00018000, 0x00002000 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_BLSP1_BLSP[] =
{
  { "BLSP1_BLSP_BAM",                              0x00004000, 0x00023000 },
  { "BLSP1_BLSP_BAM_XPU2",                         0x00002000, 0x00002000 },
  { "BLSP1_BLSP_BAM_VMIDMT",                       0x00000000, 0x00001000 },
  { "BLSP1_BLSP_UART0_UART0_DM",                   0x0002f000, 0x00000200 },
  { "BLSP1_BLSP_UART1_UART1_DM",                   0x00030000, 0x00000200 },
  { "BLSP1_BLSP_UART2_UART2_DM",                   0x00031000, 0x00000200 },
  { "BLSP1_BLSP_UART3_UART3_DM",                   0x00032000, 0x00000200 },
  { "BLSP1_BLSP_QUP0",                             0x00035000, 0x00000600 },
  { "BLSP1_BLSP_QUP1",                             0x00036000, 0x00000600 },
  { "BLSP1_BLSP_QUP2",                             0x00037000, 0x00000600 },
  { "BLSP1_BLSP_QUP3",                             0x00038000, 0x00000600 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_PKA_WRAPPER[] =
{
  { "PKA_WRAPPER_REGS",                            0x00000000, 0x00048040 },
  { "PKA_WRAPPER_XPU2",                            0x00070000, 0x00000480 },
  { NULL, 0, 0 }
};

static HWIOModuleType HWIOModules_QPIC_QPIC_XPU2[] =
{
  { "QPIC_QPIC_BAM_LITE_TOP_QPIC",                 0x00000000, 0x0001e001 },
  { "QPIC_QPIC_BAM",                               0x00004000, 0x0001c000 },
  { "QPIC_QPIC_XPU2",                              0x00002000, 0x00002000 },
  { "QPIC_QPIC_VMIDMT",                            0x00000000, 0x00001000 },
  { "QPIC_EBI2CR",                                 0x00020000, 0x00008000 },
  { "QPIC_EBI2ND",                                 0x00030000, 0x00010000 },
  { "QPIC_QPIC_MPU_MPU1132_8_M39L16_AHB_40",       0x00031000, 0x00000600 },
  { NULL, 0, 0 }
};

HWIOPhysRegionType HWIOBaseMap[] =
{
  {
    "XPU_CFG_PCNOC_CFG_MPU1132_4_M19L12_AHB",
    (DALSYSMemAddr)0x0002e000,
    0x00000400,
    (DALSYSMemAddr)0xe002e000,
    HWIOModules_XPU_CFG_PCNOC_CFG_MPU1132_4_M19L12_AHB
  },
  {
    "BIMC_MPU_MPU1132_32_M31L10_AHB",
    (DALSYSMemAddr)0x00031000,
    0x00001200,
    (DALSYSMemAddr)0xe0131000,
    HWIOModules_BIMC_MPU_MPU1132_32_M31L10_AHB
  },
  {
    "XPU_CFG_RPM_CFG_MPU1132_2_M21L12_AHB",
    (DALSYSMemAddr)0x00033000,
    0x00000300,
    (DALSYSMemAddr)0xe0233000,
    HWIOModules_XPU_CFG_RPM_CFG_MPU1132_2_M21L12_AHB
  },
  {
    "OCIMEM_WRAPPER_CSR",
    (DALSYSMemAddr)0x00050000,
    0x00004000,
    (DALSYSMemAddr)0xe0350000,
    HWIOModules_OCIMEM_WRAPPER_CSR
  },
  {
    "RPM_SS_MSG_RAM_START_ADDRESS",
    (DALSYSMemAddr)0x00060000,
    0x00003000,
    (DALSYSMemAddr)0xe0460000,
    HWIOModules_RPM_SS_MSG_RAM_START_ADDRESS
  },
  {
    "MPU_BOOT_IMEM_CFG_MPU1132A_4_M31L12_AHB",
    (DALSYSMemAddr)0x00088000,
    0x00000400,
    (DALSYSMemAddr)0xe0588000,
    HWIOModules_MPU_BOOT_IMEM_CFG_MPU1132A_4_M31L12_AHB
  },
  {
    "MPU_MSS_Q6_CFG_MPU1132A_20_M31L12_AXI",
    (DALSYSMemAddr)0x0008e000,
    0x00000c00,
    (DALSYSMemAddr)0xe068e000,
    HWIOModules_MPU_MSS_Q6_CFG_MPU1132A_20_M31L12_AXI
  },
  {
    "PCNOC_SNOC_MPU_MPU1132_32_M31L10_AHB",
    (DALSYSMemAddr)0x00092000,
    0x00001200,
    (DALSYSMemAddr)0xe0792000,
    HWIOModules_PCNOC_SNOC_MPU_MPU1132_32_M31L10_AHB
  },
  {
    "VMIDMT_PCNOC_SNOC_2_CFG_VMIDMT_SMR_128_SSD7_SID7_MA128",
    (DALSYSMemAddr)0x00096000,
    0x00001000,
    (DALSYSMemAddr)0xe0896000,
    HWIOModules_VMIDMT_PCNOC_SNOC_2_CFG_VMIDMT_SMR_128_SSD7_SID7_MA128
  },
  {
    "SECURITY_CONTROL",
    (DALSYSMemAddr)0x000a0000,
    0x00010000,
    (DALSYSMemAddr)0xe09a0000,
    HWIOModules_SECURITY_CONTROL
  },
  {
    "QC_DCC_TWIZY_2KB2LL",
    (DALSYSMemAddr)0x000b0000,
    0x00010000,
    (DALSYSMemAddr)0xe0ab0000,
    HWIOModules_QC_DCC_TWIZY_2KB2LL
  },
  {
    "PDM_PERPH_WEB",
    (DALSYSMemAddr)0x000c0000,
    0x00014000,
    (DALSYSMemAddr)0xe0bc0000,
    HWIOModules_PDM_PERPH_WEB
  },
  {
    "PRNG_PRNG_TOP",
    (DALSYSMemAddr)0x000e0000,
    0x00010000,
    (DALSYSMemAddr)0xe0ce0000,
    HWIOModules_PRNG_PRNG_TOP
  },
  {
    "BOOT_ROM",
    (DALSYSMemAddr)0x00100000,
    0x00100000,
    (DALSYSMemAddr)0xe0d00000,
    HWIOModules_BOOT_ROM
  },
  {
    "RPM",
    (DALSYSMemAddr)0x00200000,
    0x00100000,
    (DALSYSMemAddr)0xe0e00000,
    HWIOModules_RPM
  },
  {
    "BIMC",
    (DALSYSMemAddr)0x00400000,
    0x00080000,
    (DALSYSMemAddr)0xe0f00000,
    HWIOModules_BIMC
  },
  {
    "MPM2_MPM",
    (DALSYSMemAddr)0x004a0000,
    0x00010000,
    (DALSYSMemAddr)0xe10a0000,
    HWIOModules_MPM2_MPM
  },
  {
    "DEHR_WRAPPER_8K_R01",
    (DALSYSMemAddr)0x004b0000,
    0x00008000,
    (DALSYSMemAddr)0xe11b0000,
    HWIOModules_DEHR_WRAPPER_8K_R01
  },
  {
    "PC_NOC",
    (DALSYSMemAddr)0x00500000,
    0x00017080,
    (DALSYSMemAddr)0xe1200000,
    HWIOModules_PC_NOC
  },
  {
    "CRYPTO0_CRYPTO_TOP",
    (DALSYSMemAddr)0x00700000,
    0x00040000,
    (DALSYSMemAddr)0xe1300000,
    HWIOModules_CRYPTO0_CRYPTO_TOP
  },
  {
    "TLMM",
    (DALSYSMemAddr)0x01000000,
    0x00400000,
    (DALSYSMemAddr)0xe1400000,
    HWIOModules_TLMM
  },
  {
    "CLK_CTL",
    (DALSYSMemAddr)0x01800000,
    0x00100000,
    (DALSYSMemAddr)0xe1800000,
    HWIOModules_CLK_CTL
  },
  {
    "CORE_TOP_CSR",
    (DALSYSMemAddr)0x01900000,
    0x00100000,
    (DALSYSMemAddr)0xe1900000,
    HWIOModules_CORE_TOP_CSR
  },
  {
    "PMIC_ARB",
    (DALSYSMemAddr)0x02000000,
    0x01c00000,
    (DALSYSMemAddr)0xe1a00000,
    HWIOModules_PMIC_ARB
  },
  {
    "MSS_TOP",
    (DALSYSMemAddr)0x04000000,
    0x00800000,
    (DALSYSMemAddr)0xec000000,
    HWIOModules_MSS_TOP
  },
  {
    "QDSS_SOC_DBG",
    (DALSYSMemAddr)0x06000000,
    0x01000000,
    (DALSYSMemAddr)0xe3a00000,
    HWIOModules_QDSS_SOC_DBG
  },
  {
    "ULTAUDIO_CORE",
    (DALSYSMemAddr)0x07700000,
    0x00020000,
    (DALSYSMemAddr)0xe4a00000,
    HWIOModules_ULTAUDIO_CORE
  },
  {
    "BLSP1_BLSP",
    (DALSYSMemAddr)0x07880000,
    0x00040000,
    (DALSYSMemAddr)0xe4b80000,
    HWIOModules_BLSP1_BLSP
  },
  {
    "PKA_WRAPPER",
    (DALSYSMemAddr)0x07900000,
    0x00071100,
    (DALSYSMemAddr)0xe4c00000,
    HWIOModules_PKA_WRAPPER
  },
  {
    "QPIC_QPIC_XPU2",
    (DALSYSMemAddr)0x07980000,
    0x00040000,
    (DALSYSMemAddr)0xe4d80000,
    HWIOModules_QPIC_QPIC_XPU2
  },
  { NULL, 0, 0, 0, NULL }
};

