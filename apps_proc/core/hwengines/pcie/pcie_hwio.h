/*               P C I e   R E G I S T E R   M A P                            */
/**
 * @file pcie_hwio.h
 * @brief
 * Auto-generated PCIe HWIO interface include file.
 * This is the internal hardware IO layer for PCIe block
 */
/*
===============================================================================

Edit History

$Header:

Change revision history maintained in version system
===============================================================================
                   Copyright (c) 2015 QUALCOMM Technologies Incorporated.
                           All Rights Reserved.
                    Qualcomm Confidential and Proprietary.
===============================================================================
*/
#ifndef PCIE_HWIO_H
#define PCIE_HWIO_H

#include "pcie_target_hwio.h"

/*============================================================================
     PCIE_PARF
============================================================================*/
     
#define PCIE_PARF_BASE_PHYS                                                                            ( 0x80000 )
#define PCIE_PARF_SIZE                                                                                 ( 0x2000 )

/* PCIE20_PARF_SYS_CTRL */
#define PCIE20_PARF_SYS_CTRL                                                                           ( 0x0 )
#define PCIE20_PARF_SYS_CTRL_ECAM_BLOCKER_EN_RANGE2_MASK                                               0x40000000
#define PCIE20_PARF_SYS_CTRL_ECAM_BLOCKER_EN_RANGE2_SHFT                                               0x1E
#define PCIE20_PARF_SYS_CTRL_MAC_PHY_POWERDOWN_IN_P2_D_MUX_EN_MASK                                     0x20000000
#define PCIE20_PARF_SYS_CTRL_MAC_PHY_POWERDOWN_IN_P2_D_MUX_EN_SHFT                                     0x1D
#define PCIE20_PARF_SYS_CTRL_AUX_PIPE_MUX_OUTPUT_PIPE_DIS_MASK                                         0x10000000
#define PCIE20_PARF_SYS_CTRL_AUX_PIPE_MUX_OUTPUT_PIPE_DIS_SHFT                                         0x1C
#define PCIE20_PARF_SYS_CTRL_ECAM_REMOVE_OFFSET_EN_MASK                                                0x8000000
#define PCIE20_PARF_SYS_CTRL_ECAM_REMOVE_OFFSET_EN_SHFT                                                0x1B
#define PCIE20_PARF_SYS_CTRL_ECAM_BLOCKER_EN_MASK                                                      0x4000000
#define PCIE20_PARF_SYS_CTRL_ECAM_BLOCKER_EN_SHFT                                                      0x1A
#define PCIE20_PARF_SYS_CTRL_HW_EVENT_RESET_MASK                                                       0x2000000
#define PCIE20_PARF_SYS_CTRL_HW_EVENT_RESET_SHFT                                                       0x19
#define PCIE20_PARF_SYS_CTRL_HW_EVENT_CGC_OPEN_MASK                                                    0x1000000
#define PCIE20_PARF_SYS_CTRL_HW_EVENT_CGC_OPEN_SHFT                                                    0x18
#define PCIE20_PARF_SYS_CTRL_TESTBUS_SELECTOR_MASK                                                     0xFF0000
#define PCIE20_PARF_SYS_CTRL_TESTBUS_SELECTOR_SHFT                                                     0x10
#define PCIE20_PARF_SYS_CTRL_PHY_P2_IN_L1_DIS_MASK                                                     0x4000
#define PCIE20_PARF_SYS_CTRL_PHY_P2_IN_L1_DIS_SHFT                                                     0xE
#define PCIE20_PARF_SYS_CTRL_MST_WAKEUP_EN_MASK                                                        0x2000
#define PCIE20_PARF_SYS_CTRL_MST_WAKEUP_EN_SHFT                                                        0xD
#define PCIE20_PARF_SYS_CTRL_SLV_WAKEUP_EN_MASK                                                        0x1000
#define PCIE20_PARF_SYS_CTRL_SLV_WAKEUP_EN_SHFT                                                        0xC
#define PCIE20_PARF_SYS_CTRL_SLV_DBI_WAKE_DISABLE_MASK                                                 0x800
#define PCIE20_PARF_SYS_CTRL_SLV_DBI_WAKE_DISABLE_SHFT                                                 0xB
#define PCIE20_PARF_SYS_CTRL_MSTR_ACLK_CGC_DIS_MASK                                                    0x400
#define PCIE20_PARF_SYS_CTRL_MSTR_ACLK_CGC_DIS_SHFT                                                    0xA
#define PCIE20_PARF_SYS_CTRL_SLV_ACLK_CGC_DIS_MASK                                                     0x200
#define PCIE20_PARF_SYS_CTRL_SLV_ACLK_CGC_DIS_SHFT                                                     0x9
#define PCIE20_PARF_SYS_CTRL_ACLK_REQ_OVERRIDE_MASK                                                    0x100
#define PCIE20_PARF_SYS_CTRL_ACLK_REQ_OVERRIDE_SHFT                                                    0x8
#define PCIE20_PARF_SYS_CTRL_ACLK_REQ_MASK                                                             0x80
#define PCIE20_PARF_SYS_CTRL_ACLK_REQ_SHFT                                                             0x7
#define PCIE20_PARF_SYS_CTRL_CORE_CLK_CGC_DIS_MASK                                                     0x40
#define PCIE20_PARF_SYS_CTRL_CORE_CLK_CGC_DIS_SHFT                                                     0x6
#define PCIE20_PARF_SYS_CTRL_AUX_CLK_MUX_DIS_MASK                                                      0x20
#define PCIE20_PARF_SYS_CTRL_AUX_CLK_MUX_DIS_SHFT                                                      0x5
#define PCIE20_PARF_SYS_CTRL_AUX_PWR_DET_MASK                                                          0x10
#define PCIE20_PARF_SYS_CTRL_AUX_PWR_DET_SHFT                                                          0x4
#define PCIE20_PARF_SYS_CTRL_CORE_CLK_2AUX_CLK_MUX_DIS_MASK                                            0x8
#define PCIE20_PARF_SYS_CTRL_CORE_CLK_2AUX_CLK_MUX_DIS_SHFT                                            0x3
#define PCIE20_PARF_SYS_CTRL_L23_CLK_RMV_DIS_MASK                                                      0x4
#define PCIE20_PARF_SYS_CTRL_L23_CLK_RMV_DIS_SHFT                                                      0x2
#define PCIE20_PARF_SYS_CTRL_L1_CLK_RMV_DIS_MASK                                                       0x2
#define PCIE20_PARF_SYS_CTRL_L1_CLK_RMV_DIS_SHFT                                                       0x1
#define PCIE20_PARF_SYS_CTRL_CLK_RST_MDL_EN_MASK                                                       0x1
#define PCIE20_PARF_SYS_CTRL_CLK_RST_MDL_EN_SHFT                                                       0x0

/* PCIE20_PARF_DB_CTRL */
#define PCIE20_PARF_DB_CTRL                                                                            ( 0x10 )
#define PCIE20_PARF_DB_CTRL_MST_WKP_BLOCK_MASK                                                         0x40
#define PCIE20_PARF_DB_CTRL_MST_WKP_BLOCK_SHFT                                                         0x6
#define PCIE20_PARF_DB_CTRL_SLV_WKP_BLOCK_MASK                                                         0x20
#define PCIE20_PARF_DB_CTRL_SLV_WKP_BLOCK_SHFT                                                         0x5
#define PCIE20_PARF_DB_CTRL_DBI_WKP_BLOCK_MASK                                                         0x10
#define PCIE20_PARF_DB_CTRL_DBI_WKP_BLOCK_SHFT                                                         0x4
#define PCIE20_PARF_DB_CTRL_RMVL_DBNCR_BLOCK_MASK                                                      0x2
#define PCIE20_PARF_DB_CTRL_RMVL_DBNCR_BLOCK_SHFT                                                      0x1
#define PCIE20_PARF_DB_CTRL_INSR_DBNCR_BLOCK_MASK                                                      0x1
#define PCIE20_PARF_DB_CTRL_INSR_DBNCR_BLOCK_SHFT                                                      0x0

/* PCIE20_PARF_PREDET_DB_INSR */
#define PCIE20_PARF_PREDET_DB_INSR                                                                     ( 0x14 )
#define PCIE20_PARF_PREDET_DB_INSR_INSR_DBNCR_VAL_MASK                                                 0xFFFFFFFF
#define PCIE20_PARF_PREDET_DB_INSR_INSR_DBNCR_VAL_SHFT                                                 0x0

/* PCIE20_PARF_PREDET_DB_RMVL */
#define PCIE20_PARF_PREDET_DB_RMVL                                                                     ( 0x18 )
#define PCIE20_PARF_PREDET_DB_RMVL_RMVL_DBNCR_VAL_MASK                                                 0xFFFFFFFF
#define PCIE20_PARF_PREDET_DB_RMVL_RMVL_DBNCR_VAL_SHFT                                                 0x0

/* PCIE20_PARF_WKP_DB_RMVL */
#define PCIE20_PARF_WKP_DB_RMVL                                                                        ( 0x1C )
#define PCIE20_PARF_WKP_DB_RMVL_MST_DBNCR_VAL_MASK                                                     0xFF0000
#define PCIE20_PARF_WKP_DB_RMVL_MST_DBNCR_VAL_SHFT                                                     0x10
#define PCIE20_PARF_WKP_DB_RMVL_SLV_DBNCR_VAL_MASK                                                     0xFF00
#define PCIE20_PARF_WKP_DB_RMVL_SLV_DBNCR_VAL_SHFT                                                     0x8
#define PCIE20_PARF_WKP_DB_RMVL_DBI_DBNCR_VAL_MASK                                                     0xFF
#define PCIE20_PARF_WKP_DB_RMVL_DBI_DBNCR_VAL_SHFT                                                     0x0

/* PCIE20_PARF_PM_CTRL */
#define PCIE20_PARF_PM_CTRL                                                                            ( 0x20 )
#define PCIE20_PARF_PM_CTRL_REQ_NOT_ENTR_L1_MASK                                                       0x20
#define PCIE20_PARF_PM_CTRL_REQ_NOT_ENTR_L1_SHFT                                                       0x5
#define PCIE20_PARF_PM_CTRL_PM_XMT_PME_MASK                                                            0x10
#define PCIE20_PARF_PM_CTRL_PM_XMT_PME_SHFT                                                            0x4
#define PCIE20_PARF_PM_CTRL_OUTBAND_PWRUP_CMD_MASK                                                     0x8
#define PCIE20_PARF_PM_CTRL_OUTBAND_PWRUP_CMD_SHFT                                                     0x3
#define PCIE20_PARF_PM_CTRL_READY_ENTR_L23_MASK                                                        0x4
#define PCIE20_PARF_PM_CTRL_READY_ENTR_L23_SHFT                                                        0x2
#define PCIE20_PARF_PM_CTRL_REQ_EXIT_L1_MASK                                                           0x2
#define PCIE20_PARF_PM_CTRL_REQ_EXIT_L1_SHFT                                                           0x1
#define PCIE20_PARF_PM_CTRL_REQ_ENTR_L1_MASK                                                           0x1
#define PCIE20_PARF_PM_CTRL_REQ_ENTR_L1_SHFT                                                           0x0

/* PCIE20_PARF_PM_STTS */
#define PCIE20_PARF_PM_STTS                                                                            ( 0x24 )
#define PCIE20_PARF_PM_STTS_LINK_REQ_RST_NOT_MASK                                                      0x80000000
#define PCIE20_PARF_PM_STTS_LINK_REQ_RST_NOT_SHFT                                                      0x1F
#define PCIE20_PARF_PM_STTS_XMLH_LINK_UP_MASK                                                          0x40000000
#define PCIE20_PARF_PM_STTS_XMLH_LINK_UP_SHFT                                                          0x1E
#define PCIE20_PARF_PM_STTS_PM_DSTATE_0_MASK                                                           0x20000000
#define PCIE20_PARF_PM_STTS_PM_DSTATE_0_SHFT                                                           0x1D
#define PCIE20_PARF_PM_STTS_PHYSTATUS_MASK                                                             0x10000000
#define PCIE20_PARF_PM_STTS_PHYSTATUS_SHFT                                                             0x1C
#define PCIE20_PARF_PM_STTS_UNUSED_MASK                                                                0xFF80000
#define PCIE20_PARF_PM_STTS_UNUSED_SHFT                                                                0x13
#define PCIE20_PARF_PM_STTS_PM_DSTATE_MASK                                                             0x70000
#define PCIE20_PARF_PM_STTS_PM_DSTATE_SHFT                                                             0x10
#define PCIE20_PARF_PM_STTS_PM_PME_EN_MASK                                                             0xF000
#define PCIE20_PARF_PM_STTS_PM_PME_EN_SHFT                                                             0xC
#define PCIE20_PARF_PM_STTS_PHYCLK_REQ_N_MASK                                                          0x800
#define PCIE20_PARF_PM_STTS_PHYCLK_REQ_N_SHFT                                                          0xB
#define PCIE20_PARF_PM_STTS_L1SS_CLKREQN_OE_MASK                                                       0x400
#define PCIE20_PARF_PM_STTS_L1SS_CLKREQN_OE_SHFT                                                       0xA
#define PCIE20_PARF_PM_STTS_L1SS_CLKREQN_IN_MASK                                                       0x200
#define PCIE20_PARF_PM_STTS_L1SS_CLKREQN_IN_SHFT                                                       0x9
#define PCIE20_PARF_PM_STTS_PM_LINKST_IN_L1SUB_MASK                                                    0x100
#define PCIE20_PARF_PM_STTS_PM_LINKST_IN_L1SUB_SHFT                                                    0x8
#define PCIE20_PARF_PM_STTS_PM_LINKST_IN_L0S_MASK                                                      0x80
#define PCIE20_PARF_PM_STTS_PM_LINKST_IN_L0S_SHFT                                                      0x7
#define PCIE20_PARF_PM_STTS_PM_LINKST_L2_EXIT_MASK                                                     0x40
#define PCIE20_PARF_PM_STTS_PM_LINKST_L2_EXIT_SHFT                                                     0x6
#define PCIE20_PARF_PM_STTS_PM_LINKST_IN_L2_MASK                                                       0x20
#define PCIE20_PARF_PM_STTS_PM_LINKST_IN_L2_SHFT                                                       0x5
#define PCIE20_PARF_PM_STTS_PM_LINKST_IN_L1_MASK                                                       0x10
#define PCIE20_PARF_PM_STTS_PM_LINKST_IN_L1_SHFT                                                       0x4
#define PCIE20_PARF_PM_STTS_PM_STATUS_MASK                                                             0x8
#define PCIE20_PARF_PM_STTS_PM_STATUS_SHFT                                                             0x3

/* PCIE20_PARF_PM_STTS_1 */
#define PCIE20_PARF_PM_STTS_1                                                                          ( 0x28 )
#define PCIE20_PARF_PM_STTS_1_LINK_TIMEOUT_FLUSH_NOT_MASK                                              0x1000
#define PCIE20_PARF_PM_STTS_1_LINK_TIMEOUT_FLUSH_NOT_SHFT                                              0xC
#define PCIE20_PARF_PM_STTS_1_CFG_BUS_MASTER_EN_MASK                                                   0x800
#define PCIE20_PARF_PM_STTS_1_CFG_BUS_MASTER_EN_SHFT                                                   0xB
#define PCIE20_PARF_PM_STTS_1_RADM_PM_TURNOFF_MASK                                                     0x400
#define PCIE20_PARF_PM_STTS_1_RADM_PM_TURNOFF_SHFT                                                     0xA
#define PCIE20_PARF_PM_STTS_1_CURNT_STATE_MASK                                                         0x380
#define PCIE20_PARF_PM_STTS_1_CURNT_STATE_SHFT                                                         0x7
#define PCIE20_PARF_PM_STTS_1_AUX_PM_EN_MASK                                                           0x40
#define PCIE20_PARF_PM_STTS_1_AUX_PM_EN_SHFT                                                           0x6
#define PCIE20_PARF_PM_STTS_1_AUX_CLK_ACTIVE_MASK                                                      0x20
#define PCIE20_PARF_PM_STTS_1_AUX_CLK_ACTIVE_SHFT                                                      0x5
#define PCIE20_PARF_PM_STTS_1_MAC_PHY_PCLKREQ_N_MASK                                                   0x10
#define PCIE20_PARF_PM_STTS_1_MAC_PHY_PCLKREQ_N_SHFT                                                   0x4
#define PCIE20_PARF_PM_STTS_1_PHY_MAC_PCLKACK_N_MASK                                                   0x8
#define PCIE20_PARF_PM_STTS_1_PHY_MAC_PCLKACK_N_SHFT                                                   0x3
#define PCIE20_PARF_PM_STTS_1_MAC_PHY_RXELECIDLE_DIS_MASK                                              0x4
#define PCIE20_PARF_PM_STTS_1_MAC_PHY_RXELECIDLE_DIS_SHFT                                              0x2
#define PCIE20_PARF_PM_STTS_1_MAC_PHY_POWERDOWN_MASK                                                   0x3
#define PCIE20_PARF_PM_STTS_1_MAC_PHY_POWERDOWN_SHFT                                                   0x0

/* PCIE20_PARF_PCS_CTRL */
#define PCIE20_PARF_PCS_CTRL                                                                           ( 0x30 )
#define PCIE20_PARF_PCS_CTRL_TX2RX_LOOPBACK_EN_MASK                                                    0x2
#define PCIE20_PARF_PCS_CTRL_TX2RX_LOOPBACK_EN_SHFT                                                    0x1
#define PCIE20_PARF_PCS_CTRL_COMMON_CLOCKS_MASK                                                        0x1
#define PCIE20_PARF_PCS_CTRL_COMMON_CLOCKS_SHFT                                                        0x0

/* PCIE20_PARF_PCS_DEEMPH */
#define PCIE20_PARF_PCS_DEEMPH                                                                         ( 0x34 )
#define PCIE20_PARF_PCS_DEEMPH_TX_DEEMPH_GEN1_MASK                                                     0x3F0000
#define PCIE20_PARF_PCS_DEEMPH_TX_DEEMPH_GEN1_SHFT                                                     0x10
#define PCIE20_PARF_PCS_DEEMPH_TX_DEEMPH_GEN2_3_5DB_MASK                                               0x3F00
#define PCIE20_PARF_PCS_DEEMPH_TX_DEEMPH_GEN2_3_5DB_SHFT                                               0x8
#define PCIE20_PARF_PCS_DEEMPH_TX_DEEMPH_GEN2_6DB_MASK                                                 0x3F
#define PCIE20_PARF_PCS_DEEMPH_TX_DEEMPH_GEN2_6DB_SHFT                                                 0x0

/* PCIE20_PARF_PCS_SWING */
#define PCIE20_PARF_PCS_SWING                                                                          ( 0x38 )
#define PCIE20_PARF_PCS_SWING_TX_SWING_FULL_MASK                                                       0x7F00
#define PCIE20_PARF_PCS_SWING_TX_SWING_FULL_SHFT                                                       0x8
#define PCIE20_PARF_PCS_SWING_TX_SWING_LOW_MASK                                                        0x7F
#define PCIE20_PARF_PCS_SWING_TX_SWING_LOW_SHFT                                                        0x0

/* PCIE20_PARF_PHY_CTRL */
#define PCIE20_PARF_PHY_CTRL                                                                           ( 0x40 )
#define PCIE20_PARF_PHY_CTRL_PHY_TX0_TERM_OFFST_MASK                                                   0x1F0000
#define PCIE20_PARF_PHY_CTRL_PHY_TX0_TERM_OFFST_SHFT                                                   0x10
#define PCIE20_PARF_PHY_CTRL_PHY_LOS_LEVEL_MASK                                                        0x1F00
#define PCIE20_PARF_PHY_CTRL_PHY_LOS_LEVEL_SHFT                                                        0x8
#define PCIE20_PARF_PHY_CTRL_PHY_RTUNE_REQ_MASK                                                        0x10
#define PCIE20_PARF_PHY_CTRL_PHY_RTUNE_REQ_SHFT                                                        0x4
#define PCIE20_PARF_PHY_CTRL_PHY_TEST_BURNIN_MASK                                                      0x4
#define PCIE20_PARF_PHY_CTRL_PHY_TEST_BURNIN_SHFT                                                      0x2
#define PCIE20_PARF_PHY_CTRL_PHY_TEST_BYPASS_MASK                                                      0x2
#define PCIE20_PARF_PHY_CTRL_PHY_TEST_BYPASS_SHFT                                                      0x1
#define PCIE20_PARF_PHY_CTRL_PHY_TEST_PWR_DOWN_MASK                                                    0x1
#define PCIE20_PARF_PHY_CTRL_PHY_TEST_PWR_DOWN_SHFT                                                    0x0

/* PCIE20_PARF_PHY_STTS */
#define PCIE20_PARF_PHY_STTS                                                                           ( 0x44 )
#define PCIE20_PARF_PHY_STTS_PHY_RTUNE_ACK_MASK                                                        0x10
#define PCIE20_PARF_PHY_STTS_PHY_RTUNE_ACK_SHFT                                                        0x4

/* PCIE20_PARF_PHY_REFCLK */
#define PCIE20_PARF_PHY_REFCLK                                                                         ( 0x4C )
#define PCIE20_PARF_PHY_REFCLK_REF_SSP_EN_MASK                                                         0x10000
#define PCIE20_PARF_PHY_REFCLK_REF_SSP_EN_SHFT                                                         0x10
#define PCIE20_PARF_PHY_REFCLK_REF_USE_PAD_MASK                                                        0x1000
#define PCIE20_PARF_PHY_REFCLK_REF_USE_PAD_SHFT                                                        0xC
#define PCIE20_PARF_PHY_REFCLK_REFCLK_DIV2_MASK                                                        0x100
#define PCIE20_PARF_PHY_REFCLK_REFCLK_DIV2_SHFT                                                        0x8
#define PCIE20_PARF_PHY_REFCLK_MPLL_MULTIPLIER_MASK                                                    0x7F
#define PCIE20_PARF_PHY_REFCLK_MPLL_MULTIPLIER_SHFT                                                    0x0

/* PCIE20_PARF_AXI_CONFIG */
#define PCIE20_PARF_AXI_CONFIG                                                                         ( 0x54 )
#define PCIE20_PARF_AXI_CONFIG_NS_AWCACHE_MASK                                                         0xF0000000
#define PCIE20_PARF_AXI_CONFIG_NS_AWCACHE_SHFT                                                         0x1C
#define PCIE20_PARF_AXI_CONFIG_NS_ARCACHE_MASK                                                         0xF000000
#define PCIE20_PARF_AXI_CONFIG_NS_ARCACHE_SHFT                                                         0x18
#define PCIE20_PARF_AXI_CONFIG_NS_AWNOALLOCATE_MASK                                                    0x800000
#define PCIE20_PARF_AXI_CONFIG_NS_AWNOALLOCATE_SHFT                                                    0x17
#define PCIE20_PARF_AXI_CONFIG_NS_AWMEMTYPE_MASK                                                       0x700000
#define PCIE20_PARF_AXI_CONFIG_NS_AWMEMTYPE_SHFT                                                       0x14
#define PCIE20_PARF_AXI_CONFIG_NS_ARNOALLOCATE_MASK                                                    0x80000
#define PCIE20_PARF_AXI_CONFIG_NS_ARNOALLOCATE_SHFT                                                    0x13
#define PCIE20_PARF_AXI_CONFIG_NS_ARMEMTYPE_MASK                                                       0x70000
#define PCIE20_PARF_AXI_CONFIG_NS_ARMEMTYPE_SHFT                                                       0x10
#define PCIE20_PARF_AXI_CONFIG_AWCACHE_MASK                                                            0xF000
#define PCIE20_PARF_AXI_CONFIG_AWCACHE_SHFT                                                            0xC
#define PCIE20_PARF_AXI_CONFIG_ARCACHE_MASK                                                            0xF00
#define PCIE20_PARF_AXI_CONFIG_ARCACHE_SHFT                                                            0x8
#define PCIE20_PARF_AXI_CONFIG_AWNOALLOCATE_MASK                                                       0x80
#define PCIE20_PARF_AXI_CONFIG_AWNOALLOCATE_SHFT                                                       0x7
#define PCIE20_PARF_AXI_CONFIG_AWMEMTYPE_MASK                                                          0x70
#define PCIE20_PARF_AXI_CONFIG_AWMEMTYPE_SHFT                                                          0x4
#define PCIE20_PARF_AXI_CONFIG_ARNOALLOCATE_MASK                                                       0x8
#define PCIE20_PARF_AXI_CONFIG_ARNOALLOCATE_SHFT                                                       0x3
#define PCIE20_PARF_AXI_CONFIG_ARMEMTYPE_MASK                                                          0x7
#define PCIE20_PARF_AXI_CONFIG_ARMEMTYPE_SHFT                                                          0x0

/* PCIE20_PARF_HW_EV_n */
#define PCIE20_PARF_HW_EV_n(n)                                                                         ( 0x58 + 0x4 * (n) )
#define PCIE20_PARF_HW_EV_n_EN_MASK                                                                    0x80000000
#define PCIE20_PARF_HW_EV_n_EN_SHFT                                                                    0x1F
#define PCIE20_PARF_HW_EV_n_SEL_MASK                                                                   0x7F
#define PCIE20_PARF_HW_EV_n_SEL_SHFT                                                                   0x0

/* PCIE20_PARF_Q2A_BRIDGE_CTRL */
#define PCIE20_PARF_Q2A_BRIDGE_CTRL                                                                    ( 0xDC )
#define PCIE20_PARF_Q2A_BRIDGE_CTRL_BRIDGE_RESET_MASK                                                  0x40
#define PCIE20_PARF_Q2A_BRIDGE_CTRL_BRIDGE_RESET_SHFT                                                  0x6
#define PCIE20_PARF_Q2A_BRIDGE_CTRL_DBI_WR_STALL_RESET_MASK                                            0x20
#define PCIE20_PARF_Q2A_BRIDGE_CTRL_DBI_WR_STALL_RESET_SHFT                                            0x5
#define PCIE20_PARF_Q2A_BRIDGE_CTRL_DBI_WR_STALL_BYPASS_MASK                                           0x10
#define PCIE20_PARF_Q2A_BRIDGE_CTRL_DBI_WR_STALL_BYPASS_SHFT                                           0x4
#define PCIE20_PARF_Q2A_BRIDGE_CTRL_STATUS_MASK                                                        0xF
#define PCIE20_PARF_Q2A_BRIDGE_CTRL_STATUS_SHFT                                                        0x0

/* PCIE20_PARF_SLV_BASE_ADDR */
#define PCIE20_PARF_SLV_BASE_ADDR                                                                      ( 0xE0 )
#define PCIE20_PARF_SLV_BASE_ADDR_SLV_BASE_ADDR_MASK                                                   0xFFFFFFFF
#define PCIE20_PARF_SLV_BASE_ADDR_SLV_BASE_ADDR_SHFT                                                   0x0

/* PCIE20_PARF_TEST_BUS */
#define PCIE20_PARF_TEST_BUS                                                                           ( 0xE4 )
#define PCIE20_PARF_TEST_BUS_PCIE20_TEST_BUS_MASK                                                      0xFFFFFFFF
#define PCIE20_PARF_TEST_BUS_PCIE20_TEST_BUS_SHFT                                                      0x0

/* PCIE20_PARF_MSG_PARAMS_n */
#define PCIE20_PARF_MSG_PARAMS_n(n)                                                                    ( 0x128 + 0x4 * (n) )
#define PCIE20_PARF_MSG_PARAMS_n_EN_MASK                                                               0x80000000
#define PCIE20_PARF_MSG_PARAMS_n_EN_SHFT                                                               0x1F
#define PCIE20_PARF_MSG_PARAMS_n_MSG_MSGDNMSG_MASK                                                     0x1000000
#define PCIE20_PARF_MSG_PARAMS_n_MSG_MSGDNMSG_SHFT                                                     0x18
#define PCIE20_PARF_MSG_PARAMS_n_MSG_TC_MASK                                                           0x700000
#define PCIE20_PARF_MSG_PARAMS_n_MSG_TC_SHFT                                                           0x14
#define PCIE20_PARF_MSG_PARAMS_n_MSG_EP_MASK                                                           0x10000
#define PCIE20_PARF_MSG_PARAMS_n_MSG_EP_SHFT                                                           0x10
#define PCIE20_PARF_MSG_PARAMS_n_MSG_TYPE_MASK                                                         0x1F00
#define PCIE20_PARF_MSG_PARAMS_n_MSG_TYPE_SHFT                                                         0x8
#define PCIE20_PARF_MSG_PARAMS_n_MSG_CODE_MASK                                                         0xFF
#define PCIE20_PARF_MSG_PARAMS_n_MSG_CODE_SHFT                                                         0x0

/* PCIE20_PARF_VERSION */
#define PCIE20_PARF_VERSION                                                                            ( 0x170 )
#define PCIE20_PARF_VERSION_IP_CAT_VERSION_MASK                                                        0xFFF
#define PCIE20_PARF_VERSION_IP_CAT_VERSION_SHFT                                                        0x0

/* PCIE20_PARF_MHI_CLOCK_RESET_CTRL */
#define PCIE20_PARF_MHI_CLOCK_RESET_CTRL                                                               ( 0x174 )
#define PCIE20_PARF_MHI_CLOCK_RESET_CTRL_BYPASS_MASK                                                   0x10
#define PCIE20_PARF_MHI_CLOCK_RESET_CTRL_BYPASS_SHFT                                                   0x4
#define PCIE20_PARF_MHI_CLOCK_RESET_CTRL_MSTR_AXI_LOGIC_RESET_MASK                                     0x8
#define PCIE20_PARF_MHI_CLOCK_RESET_CTRL_MSTR_AXI_LOGIC_RESET_SHFT                                     0x3
#define PCIE20_PARF_MHI_CLOCK_RESET_CTRL_AHB_LOGIC_RESET_MASK                                          0x4
#define PCIE20_PARF_MHI_CLOCK_RESET_CTRL_AHB_LOGIC_RESET_SHFT                                          0x2
#define PCIE20_PARF_MHI_CLOCK_RESET_CTRL_MSTR_AXI_CLK_EN_MASK                                          0x2
#define PCIE20_PARF_MHI_CLOCK_RESET_CTRL_MSTR_AXI_CLK_EN_SHFT                                          0x1
#define PCIE20_PARF_MHI_CLOCK_RESET_CTRL_AHB_CLK_EN_MASK                                               0x1
#define PCIE20_PARF_MHI_CLOCK_RESET_CTRL_AHB_CLK_EN_SHFT                                               0x0

/* PCIE20_PARF_MHI_BASE_ADDR_LOWER */
#define PCIE20_PARF_MHI_BASE_ADDR_LOWER                                                                ( 0x178 )
#define PCIE20_PARF_MHI_BASE_ADDR_LOWER_VAL_MASK                                                       0xFFFFFFFF
#define PCIE20_PARF_MHI_BASE_ADDR_LOWER_VAL_SHFT                                                       0x0

/* PCIE20_PARF_MHI_BASE_ADDR_UPPER */
#define PCIE20_PARF_MHI_BASE_ADDR_UPPER                                                                ( 0x17C )
#define PCIE20_PARF_MHI_BASE_ADDR_UPPER_VAL_MASK                                                       0xFFFFFFFF
#define PCIE20_PARF_MHI_BASE_ADDR_UPPER_VAL_SHFT                                                       0x0

/* PCIE20_PARF_L1SUB_AHB_CLK_MAX_TIMER */
#define PCIE20_PARF_L1SUB_AHB_CLK_MAX_TIMER                                                            ( 0x180 )
#define PCIE20_PARF_L1SUB_AHB_CLK_MAX_TIMER_RESET_MASK                                                 0x80000000
#define PCIE20_PARF_L1SUB_AHB_CLK_MAX_TIMER_RESET_SHFT                                                 0x1F
#define PCIE20_PARF_L1SUB_AHB_CLK_MAX_TIMER_CNT_MAX_MASK                                               0x7FFFFFFF
#define PCIE20_PARF_L1SUB_AHB_CLK_MAX_TIMER_CNT_MAX_SHFT                                               0x0

/* PCIE20_PARF_LTR_MSG_GEN */
#define PCIE20_PARF_LTR_MSG_GEN                                                                        ( 0x184 )
#define PCIE20_PARF_LTR_MSG_GEN_LATENCY_MASK                                                           0xFFFFFFFF
#define PCIE20_PARF_LTR_MSG_GEN_LATENCY_SHFT                                                           0x0

/* PCIE20_PARF_MSI_GEN */
#define PCIE20_PARF_MSI_GEN                                                                            ( 0x188 )
#define PCIE20_PARF_MSI_GEN_VECTOR_MASK                                                                0x1F
#define PCIE20_PARF_MSI_GEN_VECTOR_SHFT                                                                0x0

/* PCIE20_PARF_AXI_MSTR_OUTST_LIMIT */
#define PCIE20_PARF_AXI_MSTR_OUTST_LIMIT                                                               ( 0x18C )
#define PCIE20_PARF_AXI_MSTR_OUTST_LIMIT_RD_LIMIT_MASK                                                 0x3E0
#define PCIE20_PARF_AXI_MSTR_OUTST_LIMIT_RD_LIMIT_SHFT                                                 0x5
#define PCIE20_PARF_AXI_MSTR_OUTST_LIMIT_WR_LIMIT_MASK                                                 0x1F
#define PCIE20_PARF_AXI_MSTR_OUTST_LIMIT_WR_LIMIT_SHFT                                                 0x0

/* PCIE20_PARF_DEBUG_INT_EN */
#define PCIE20_PARF_DEBUG_INT_EN                                                                       ( 0x190 )
#define PCIE20_PARF_DEBUG_INT_EN_L1SUB_TIMEOUT_SEL_BIT_MASK                                            0x10
#define PCIE20_PARF_DEBUG_INT_EN_L1SUB_TIMEOUT_SEL_BIT_SHFT                                            0x4
#define PCIE20_PARF_DEBUG_INT_EN_RADM_PM_TURNOFF_MASK                                                  0x8
#define PCIE20_PARF_DEBUG_INT_EN_RADM_PM_TURNOFF_SHFT                                                  0x3
#define PCIE20_PARF_DEBUG_INT_EN_CFG_BUS_MASTER_EN_MASK                                                0x4
#define PCIE20_PARF_DEBUG_INT_EN_CFG_BUS_MASTER_EN_SHFT                                                0x2
#define PCIE20_PARF_DEBUG_INT_EN_PM_DSTATE_CHANGE_BIT_MASK                                             0x2
#define PCIE20_PARF_DEBUG_INT_EN_PM_DSTATE_CHANGE_BIT_SHFT                                             0x1
#define PCIE20_PARF_DEBUG_INT_EN_L1SUB_TIMEOUT_BIT_MASK                                                0x1
#define PCIE20_PARF_DEBUG_INT_EN_L1SUB_TIMEOUT_BIT_SHFT                                                0x0

/* PCIE20_PARF_MHI_BAR_KB_SIZE */
#define PCIE20_PARF_MHI_BAR_KB_SIZE                                                                    ( 0x194 )
#define PCIE20_PARF_MHI_BAR_KB_SIZE_SIZE_KB_MASK                                                       0xFFFFFFFF
#define PCIE20_PARF_MHI_BAR_KB_SIZE_SIZE_KB_SHFT                                                       0x0

/* PCIE20_PARF_MHI_IPA_DBS */
#define PCIE20_PARF_MHI_IPA_DBS                                                                        ( 0x198 )
#define PCIE20_PARF_MHI_IPA_DBS_EDB_END_MASK                                                           0xFF000000
#define PCIE20_PARF_MHI_IPA_DBS_EDB_END_SHFT                                                           0x18
#define PCIE20_PARF_MHI_IPA_DBS_EDB_BASE_MASK                                                          0xFF0000
#define PCIE20_PARF_MHI_IPA_DBS_EDB_BASE_SHFT                                                          0x10
#define PCIE20_PARF_MHI_IPA_DBS_CDB_END_MASK                                                           0xFF00
#define PCIE20_PARF_MHI_IPA_DBS_CDB_END_SHFT                                                           0x8
#define PCIE20_PARF_MHI_IPA_DBS_CDB_BASE_MASK                                                          0xFF
#define PCIE20_PARF_MHI_IPA_DBS_CDB_BASE_SHFT                                                          0x0

/* PCIE20_PARF_MHI_IPA_CDB_TARGET_LOWER */
#define PCIE20_PARF_MHI_IPA_CDB_TARGET_LOWER                                                           ( 0x19C )
#define PCIE20_PARF_MHI_IPA_CDB_TARGET_LOWER_ADDR_LOWER_MASK                                           0xFFFFFFFF
#define PCIE20_PARF_MHI_IPA_CDB_TARGET_LOWER_ADDR_LOWER_SHFT                                           0x0

/* PCIE20_PARF_MHI_IPA_EDB_TARGET_LOWER */
#define PCIE20_PARF_MHI_IPA_EDB_TARGET_LOWER                                                           ( 0x1A0 )
#define PCIE20_PARF_MHI_IPA_EDB_TARGET_LOWER_ADDR_LOWER_MASK                                           0xFFFFFFFF
#define PCIE20_PARF_MHI_IPA_EDB_TARGET_LOWER_ADDR_LOWER_SHFT                                           0x0

/* PCIE20_PARF_AXI_MSTR_RD_HALT_NO_WRITES */
#define PCIE20_PARF_AXI_MSTR_RD_HALT_NO_WRITES                                                         ( 0x1A4 )
#define PCIE20_PARF_AXI_MSTR_RD_HALT_NO_WRITES_EN_MASK                                                 0x1
#define PCIE20_PARF_AXI_MSTR_RD_HALT_NO_WRITES_EN_SHFT                                                 0x0

/* PCIE20_PARF_AXI_MSTR_WR_ADDR_HALT */
#define PCIE20_PARF_AXI_MSTR_WR_ADDR_HALT                                                              ( 0x1A8 )
#define PCIE20_PARF_AXI_MSTR_WR_ADDR_HALT_EN_MASK                                                      0x80000000
#define PCIE20_PARF_AXI_MSTR_WR_ADDR_HALT_EN_SHFT                                                      0x1F
#define PCIE20_PARF_AXI_MSTR_WR_ADDR_HALT_ADDR_BIT_INDEX_MASK                                          0x3F
#define PCIE20_PARF_AXI_MSTR_WR_ADDR_HALT_ADDR_BIT_INDEX_SHFT                                          0x0

/* PCIE20_PARF_LTSSM */
#define PCIE20_PARF_LTSSM                                                                              ( 0x1B0 )
#define PCIE20_PARF_LTSSM_FLUSH_MODE_MASK                                                              0x800
#define PCIE20_PARF_LTSSM_FLUSH_MODE_SHFT                                                              0xB
#define PCIE20_PARF_LTSSM_SW_CLR_FLUSH_MODE_MASK                                                       0x400
#define PCIE20_PARF_LTSSM_SW_CLR_FLUSH_MODE_SHFT                                                       0xA
#define PCIE20_PARF_LTSSM_LINK_DOWN_AFFECT_LTSSM_DIS_MASK                                              0x200
#define PCIE20_PARF_LTSSM_LINK_DOWN_AFFECT_LTSSM_DIS_SHFT                                              0x9
#define PCIE20_PARF_LTSSM_LTSSM_EN_MASK                                                                0x100
#define PCIE20_PARF_LTSSM_LTSSM_EN_SHFT                                                                0x8
#define PCIE20_PARF_LTSSM_LTSSM_STATE_MASK                                                             0x3F
#define PCIE20_PARF_LTSSM_LTSSM_STATE_SHFT                                                             0x0

/* PCIE20_PARF_SLV_DBI_ELBI */
#define PCIE20_PARF_SLV_DBI_ELBI                                                                       ( 0x1B4 )
#define PCIE20_PARF_SLV_DBI_ELBI_ADDR_BASE_MASK                                                        0xFFF
#define PCIE20_PARF_SLV_DBI_ELBI_ADDR_BASE_SHFT                                                        0x0

/* PCIE20_PARF_BLOCK_SLV_AXI_DEBUG */
#define PCIE20_PARF_BLOCK_SLV_AXI_DEBUG                                                                ( 0x1C8 )
#define PCIE20_PARF_BLOCK_SLV_AXI_DEBUG_RD_STATE_MASK                                                  0x18
#define PCIE20_PARF_BLOCK_SLV_AXI_DEBUG_RD_STATE_SHFT                                                  0x3
#define PCIE20_PARF_BLOCK_SLV_AXI_DEBUG_WR_STATE_MASK                                                  0x7
#define PCIE20_PARF_BLOCK_SLV_AXI_DEBUG_WR_STATE_SHFT                                                  0x0

/* PCIE20_PARF_SPARE_0_1_BITS */
#define PCIE20_PARF_SPARE_0_1_BITS                                                                     ( 0x1D8 )
#define PCIE20_PARF_SPARE_0_1_BITS_SPARE_1_MASK                                                        0xFFFF0000
#define PCIE20_PARF_SPARE_0_1_BITS_SPARE_1_SHFT                                                        0x10
#define PCIE20_PARF_SPARE_0_1_BITS_SPARE_0_MASK                                                        0xFFFF
#define PCIE20_PARF_SPARE_0_1_BITS_SPARE_0_SHFT                                                        0x0

/* PCIE20_PARF_RELEASE_VERSION */
#define PCIE20_PARF_RELEASE_VERSION                                                                    ( 0x1DC )
#define PCIE20_PARF_RELEASE_VERSION_RELEASE_VER_MASK                                                   0xFFFFFFFF
#define PCIE20_PARF_RELEASE_VERSION_RELEASE_VER_SHFT                                                   0x0

/* PCIE20_PARF_CFG_MSI_PENDING_SET */
#define PCIE20_PARF_CFG_MSI_PENDING_SET                                                                ( 0x1E0 )
#define PCIE20_PARF_CFG_MSI_PENDING_SET_CFG_MSI_PENDING_SET_MASK                                       0xFFFFFFFF
#define PCIE20_PARF_CFG_MSI_PENDING_SET_CFG_MSI_PENDING_SET_SHFT                                       0x0

/* PCIE20_PARF_CFG_MSI_PENDING_CLEAR */
#define PCIE20_PARF_CFG_MSI_PENDING_CLEAR                                                              ( 0x1E4 )
#define PCIE20_PARF_CFG_MSI_PENDING_CLEAR_CFG_MSI_PENDING_CLEAR_MASK                                   0xFFFFFFFF
#define PCIE20_PARF_CFG_MSI_PENDING_CLEAR_CFG_MSI_PENDING_CLEAR_SHFT                                   0x0

/* PCIE20_PARF_CFG_MSI_PENDING */
#define PCIE20_PARF_CFG_MSI_PENDING                                                                    ( 0x1E8 )
#define PCIE20_PARF_CFG_MSI_PENDING_CFG_MSI_PENDING_MASK                                               0xFFFFFFFF
#define PCIE20_PARF_CFG_MSI_PENDING_CFG_MSI_PENDING_SHFT                                               0x0

/* PCIE20_PARF_CFG_MSI_MASK */
#define PCIE20_PARF_CFG_MSI_MASK                                                                       ( 0x1EC )
#define PCIE20_PARF_CFG_MSI_MASK_CFG_MSI_MASK_MASK                                                     0xFFFFFFFF
#define PCIE20_PARF_CFG_MSI_MASK_CFG_MSI_MASK_SHFT                                                     0x0

/* PCIE20_PARF_CFG_MSI_MASK_INT_STATUS */
#define PCIE20_PARF_CFG_MSI_MASK_INT_STATUS                                                            ( 0x1F0 )
#define PCIE20_PARF_CFG_MSI_MASK_INT_STATUS_CFG_MSI_MASK_INT_STATUS_MASK                               0xFFFFFFFF
#define PCIE20_PARF_CFG_MSI_MASK_INT_STATUS_CFG_MSI_MASK_INT_STATUS_SHFT                               0x0

/* PCIE20_PARF_CFG_MSI_MASK_INT_MASK */
#define PCIE20_PARF_CFG_MSI_MASK_INT_MASK                                                              ( 0x1F4 )
#define PCIE20_PARF_CFG_MSI_MASK_INT_MASK_CFG_MSI_MASK_INT_MASK_MASK                                   0xFFFFFFFF
#define PCIE20_PARF_CFG_MSI_MASK_INT_MASK_CFG_MSI_MASK_INT_MASK_SHFT                                   0x0

/* PCIE20_PARF_CFG_MSI_MASK_INT_CLEAR */
#define PCIE20_PARF_CFG_MSI_MASK_INT_CLEAR                                                             ( 0x1F8 )
#define PCIE20_PARF_CFG_MSI_MASK_INT_CLEAR_CFG_MSI_MASK_INT_CLEAR_MASK                                 0xFFFFFFFF
#define PCIE20_PARF_CFG_MSI_MASK_INT_CLEAR_CFG_MSI_MASK_INT_CLEAR_SHFT                                 0x0

/* PCIE20_PARF_CFG_BITS */
#define PCIE20_PARF_CFG_BITS                                                                           ( 0x210 )
#define PCIE20_PARF_CFG_BITS_PARF_SLV_SECOND_CGC_EN_OPTION_MASK                                        0x80000000
#define PCIE20_PARF_CFG_BITS_PARF_SLV_SECOND_CGC_EN_OPTION_SHFT                                        0x1F
#define PCIE20_PARF_CFG_BITS_PARF_MSTR_SECOND_CGC_EN_OPTION_MASK                                       0x40000000
#define PCIE20_PARF_CFG_BITS_PARF_MSTR_SECOND_CGC_EN_OPTION_SHFT                                       0x1E
#define PCIE20_PARF_CFG_BITS_PARF_ADD_APP_REQ_EXIT_L1_MASK                                             0x20000000
#define PCIE20_PARF_CFG_BITS_PARF_ADD_APP_REQ_EXIT_L1_SHFT                                             0x1D
#define PCIE20_PARF_CFG_BITS_PARF_ADD_APP_XFER_PENDING_MASK                                            0x10000000
#define PCIE20_PARF_CFG_BITS_PARF_ADD_APP_XFER_PENDING_SHFT                                            0x1C
#define PCIE20_PARF_CFG_BITS_CFG_AGGR_MSI_INT_MODE_MASK                                                0x2000000
#define PCIE20_PARF_CFG_BITS_CFG_AGGR_MSI_INT_MODE_SHFT                                                0x19
#define PCIE20_PARF_CFG_BITS_DIFF_MID_WAIT_BRESP_FIX_DIS_MASK                                          0x1000000
#define PCIE20_PARF_CFG_BITS_DIFF_MID_WAIT_BRESP_FIX_DIS_SHFT                                          0x18
#define PCIE20_PARF_CFG_BITS_FLUSH_INT_WAIT_QSB_CLK_FIX_DIS_MASK                                       0x800000
#define PCIE20_PARF_CFG_BITS_FLUSH_INT_WAIT_QSB_CLK_FIX_DIS_SHFT                                       0x17
#define PCIE20_PARF_CFG_BITS_MSG_NO_PAYLOAD_DATA_FIX_DIS_MASK                                          0x400000
#define PCIE20_PARF_CFG_BITS_MSG_NO_PAYLOAD_DATA_FIX_DIS_SHFT                                          0x16
#define PCIE20_PARF_CFG_BITS_LATCH_APP_REQ_EXIT_L1_FIX_EN_MASK                                         0x300000
#define PCIE20_PARF_CFG_BITS_LATCH_APP_REQ_EXIT_L1_FIX_EN_SHFT                                         0x14
#define PCIE20_PARF_CFG_BITS_INT_PME_LEGACY_OR_L1SUB_TIMEOUT_FIX_EN_MASK                               0x80000
#define PCIE20_PARF_CFG_BITS_INT_PME_LEGACY_OR_L1SUB_TIMEOUT_FIX_EN_SHFT                               0x13
#define PCIE20_PARF_CFG_BITS_UPD_MSTR_WR_HALT_ADDR_FIX_EN_MASK                                         0x40000
#define PCIE20_PARF_CFG_BITS_UPD_MSTR_WR_HALT_ADDR_FIX_EN_SHFT                                         0x12
#define PCIE20_PARF_CFG_BITS_FIRST_RAM_CLK_MASK_FIX_EN_MASK                                            0x20000
#define PCIE20_PARF_CFG_BITS_FIRST_RAM_CLK_MASK_FIX_EN_SHFT                                            0x11
#define PCIE20_PARF_CFG_BITS_NOC_MSTR_AWREADY_FIX_EN_MASK                                              0x10000
#define PCIE20_PARF_CFG_BITS_NOC_MSTR_AWREADY_FIX_EN_SHFT                                              0x10
#define PCIE20_PARF_CFG_BITS_SLV_WAKEUP_VALID_FIX_EN_MASK                                              0x8000
#define PCIE20_PARF_CFG_BITS_SLV_WAKEUP_VALID_FIX_EN_SHFT                                              0xF
#define PCIE20_PARF_CFG_BITS_TESTBUS_SWITCH_MASK                                                       0x4000
#define PCIE20_PARF_CFG_BITS_TESTBUS_SWITCH_SHFT                                                       0xE
#define PCIE20_PARF_CFG_BITS_BRIDGE_FLUSH_INT_CNT_EN_MASK                                              0x2000
#define PCIE20_PARF_CFG_BITS_BRIDGE_FLUSH_INT_CNT_EN_SHFT                                              0xD
#define PCIE20_PARF_CFG_BITS_MSTR_WR_DMA_NS_MASK                                                       0x1000
#define PCIE20_PARF_CFG_BITS_MSTR_WR_DMA_NS_SHFT                                                       0xC
#define PCIE20_PARF_CFG_BITS_MSTR_RD_DMA_NS_MASK                                                       0x800
#define PCIE20_PARF_CFG_BITS_MSTR_RD_DMA_NS_SHFT                                                       0xB
#define PCIE20_PARF_CFG_BITS_DEBUG_CNT_CLK_EN_MSTR_MASK                                                0x400
#define PCIE20_PARF_CFG_BITS_DEBUG_CNT_CLK_EN_MSTR_SHFT                                                0xA
#define PCIE20_PARF_CFG_BITS_DEBUG_CNT_CLK_EN_MASK                                                     0x200
#define PCIE20_PARF_CFG_BITS_DEBUG_CNT_CLK_EN_SHFT                                                     0x9
#define PCIE20_PARF_CFG_BITS_SLV_AMISC_INFO_ATU_BYPASS_MASK                                            0x180
#define PCIE20_PARF_CFG_BITS_SLV_AMISC_INFO_ATU_BYPASS_SHFT                                            0x7
#define PCIE20_PARF_CFG_BITS_USE_LANE_FLIP_EN_MASK                                                     0x60
#define PCIE20_PARF_CFG_BITS_USE_LANE_FLIP_EN_SHFT                                                     0x5
#define PCIE20_PARF_CFG_BITS_SNPS_CLKREQ_SECOND_FIX_MASK                                               0x10
#define PCIE20_PARF_CFG_BITS_SNPS_CLKREQ_SECOND_FIX_SHFT                                               0x4
#define PCIE20_PARF_CFG_BITS_ATU_ZERO_RESERVED_BITS_FIX_MASK                                           0x8
#define PCIE20_PARF_CFG_BITS_ATU_ZERO_RESERVED_BITS_FIX_SHFT                                           0x3
#define PCIE20_PARF_CFG_BITS_USE_CFG_L1SUB_EN_MASK                                                     0x4
#define PCIE20_PARF_CFG_BITS_USE_CFG_L1SUB_EN_SHFT                                                     0x2
#define PCIE20_PARF_CFG_BITS_REQ_EXIT_L1SS_MSI_LTR_EN_MASK                                             0x2
#define PCIE20_PARF_CFG_BITS_REQ_EXIT_L1SS_MSI_LTR_EN_SHFT                                             0x1
#define PCIE20_PARF_CFG_BITS_LTR_MSI_HREADY_LOW_EN_MASK                                                0x1
#define PCIE20_PARF_CFG_BITS_LTR_MSI_HREADY_LOW_EN_SHFT                                                0x0

/* PCIE20_PARF_LTR_MSI_EXIT_L1SS */
#define PCIE20_PARF_LTR_MSI_EXIT_L1SS                                                                  ( 0x214 )
#define PCIE20_PARF_LTR_MSI_EXIT_L1SS_MSI_EXIT_L1SS_MASK                                               0x2
#define PCIE20_PARF_LTR_MSI_EXIT_L1SS_MSI_EXIT_L1SS_SHFT                                               0x1
#define PCIE20_PARF_LTR_MSI_EXIT_L1SS_LTR_MSG_EXIT_L1SS_MASK                                           0x1
#define PCIE20_PARF_LTR_MSI_EXIT_L1SS_LTR_MSG_EXIT_L1SS_SHFT                                           0x0

/* PCIE20_PARF_INT_MASK */
#define PCIE20_PARF_INT_MASK                                                                           ( 0x218 )
#define PCIE20_PARF_INT_MASK_PHY_RESET_REQ_ASYNC_INT_MASK_MASK                                         0x10
#define PCIE20_PARF_INT_MASK_PHY_RESET_REQ_ASYNC_INT_MASK_SHFT                                         0x4
#define PCIE20_PARF_INT_MASK_PCIE20_INT_BRIDGE_FLUSH_MASK_MASK                                         0x4
#define PCIE20_PARF_INT_MASK_PCIE20_INT_BRIDGE_FLUSH_MASK_SHFT                                         0x2
#define PCIE20_PARF_INT_MASK_AMBA_ORDR_MGR_WDT_MASK_MASK                                               0x2
#define PCIE20_PARF_INT_MASK_AMBA_ORDR_MGR_WDT_MASK_SHFT                                               0x1
#define PCIE20_PARF_INT_MASK_RADM_QOVERFLOW_MASK_MASK                                                  0x1
#define PCIE20_PARF_INT_MASK_RADM_QOVERFLOW_MASK_SHFT                                                  0x0

/* PCIE20_PARF_INT_CLEAR */
#define PCIE20_PARF_INT_CLEAR                                                                          ( 0x21C )
#define PCIE20_PARF_INT_CLEAR_PHY_RESET_REQ_SYNC_INT_CLEAR_MASK                                        0x8
#define PCIE20_PARF_INT_CLEAR_PHY_RESET_REQ_SYNC_INT_CLEAR_SHFT                                        0x3
#define PCIE20_PARF_INT_CLEAR_AMBA_ORDR_MGR_WDT_CLEAR_MASK                                             0x2
#define PCIE20_PARF_INT_CLEAR_AMBA_ORDR_MGR_WDT_CLEAR_SHFT                                             0x1
#define PCIE20_PARF_INT_CLEAR_RADM_QOVERFLOW_CLEAR_MASK                                                0x1
#define PCIE20_PARF_INT_CLEAR_RADM_QOVERFLOW_CLEAR_SHFT                                                0x0

/* PCIE20_PARF_INT_STATUS */
#define PCIE20_PARF_INT_STATUS                                                                         ( 0x220 )
#define PCIE20_PARF_INT_STATUS_PHY_RESET_REQ_ASYNC_INT_STS_MASK                                        0x10
#define PCIE20_PARF_INT_STATUS_PHY_RESET_REQ_ASYNC_INT_STS_SHFT                                        0x4
#define PCIE20_PARF_INT_STATUS_PHY_RESET_REQ_SYNC_INT_STS_MASK                                         0x8
#define PCIE20_PARF_INT_STATUS_PHY_RESET_REQ_SYNC_INT_STS_SHFT                                         0x3
#define PCIE20_PARF_INT_STATUS_AMBA_ORDR_MGR_WDT_MASK                                                  0x2
#define PCIE20_PARF_INT_STATUS_AMBA_ORDR_MGR_WDT_SHFT                                                  0x1
#define PCIE20_PARF_INT_STATUS_RADM_QOVERFLOW_MASK                                                     0x1
#define PCIE20_PARF_INT_STATUS_RADM_QOVERFLOW_SHFT                                                     0x0

/* PCIE20_INT_ALL_STATUS */
#define PCIE20_INT_ALL_STATUS                                                                          ( 0x224 )
#define PCIE20_INT_ALL_STATUS_PCIE_INT_MSI_DEV_0_MASK                                                  0x40000000
#define PCIE20_INT_ALL_STATUS_PCIE_INT_MSI_DEV_0_SHFT                                                  0x1E
#define PCIE20_INT_ALL_STATUS_PCIE_INT_MSI_DEV_1_MASK                                                  0x20000000
#define PCIE20_INT_ALL_STATUS_PCIE_INT_MSI_DEV_1_SHFT                                                  0x1D
#define PCIE20_INT_ALL_STATUS_PCIE_INT_MSI_DEV_2_MASK                                                  0x10000000
#define PCIE20_INT_ALL_STATUS_PCIE_INT_MSI_DEV_2_SHFT                                                  0x1C
#define PCIE20_INT_ALL_STATUS_PCIE_INT_MSI_DEV_3_MASK                                                  0x8000000
#define PCIE20_INT_ALL_STATUS_PCIE_INT_MSI_DEV_3_SHFT                                                  0x1B
#define PCIE20_INT_ALL_STATUS_PCIE_INT_MSI_DEV_4_MASK                                                  0x4000000
#define PCIE20_INT_ALL_STATUS_PCIE_INT_MSI_DEV_4_SHFT                                                  0x1A
#define PCIE20_INT_ALL_STATUS_PCIE_INT_MSI_DEV_5_MASK                                                  0x2000000
#define PCIE20_INT_ALL_STATUS_PCIE_INT_MSI_DEV_5_SHFT                                                  0x19
#define PCIE20_INT_ALL_STATUS_PCIE_INT_MSI_DEV_6_MASK                                                  0x1000000
#define PCIE20_INT_ALL_STATUS_PCIE_INT_MSI_DEV_6_SHFT                                                  0x18
#define PCIE20_INT_ALL_STATUS_PCIE_INT_MSI_DEV_7_MASK                                                  0x800000
#define PCIE20_INT_ALL_STATUS_PCIE_INT_MSI_DEV_7_SHFT                                                  0x17
#define PCIE20_INT_ALL_STATUS_PCIE_INT_EDMA_INT_MASK                                                   0x400000
#define PCIE20_INT_ALL_STATUS_PCIE_INT_EDMA_INT_SHFT                                                   0x16
#define PCIE20_INT_ALL_STATUS_PCIE_INTA_MASK                                                           0x200000
#define PCIE20_INT_ALL_STATUS_PCIE_INTA_SHFT                                                           0x15
#define PCIE20_INT_ALL_STATUS_PCIE_INTB_MASK                                                           0x100000
#define PCIE20_INT_ALL_STATUS_PCIE_INTB_SHFT                                                           0x14
#define PCIE20_INT_ALL_STATUS_PCIE_INTC_MASK                                                           0x80000
#define PCIE20_INT_ALL_STATUS_PCIE_INTC_SHFT                                                           0x13
#define PCIE20_INT_ALL_STATUS_PCIE_INTD_MASK                                                           0x40000
#define PCIE20_INT_ALL_STATUS_PCIE_INTD_SHFT                                                           0x12
#define PCIE20_INT_ALL_STATUS_PCIE_INT_PLS_PME_MASK                                                    0x20000
#define PCIE20_INT_ALL_STATUS_PCIE_INT_PLS_PME_SHFT                                                    0x11
#define PCIE20_INT_ALL_STATUS_PCIE_INT_PME_LEGACY_MASK                                                 0x10000
#define PCIE20_INT_ALL_STATUS_PCIE_INT_PME_LEGACY_SHFT                                                 0x10
#define PCIE20_INT_ALL_STATUS_PCIE_INT_PLS_ERR_MASK                                                    0x8000
#define PCIE20_INT_ALL_STATUS_PCIE_INT_PLS_ERR_SHFT                                                    0xF
#define PCIE20_INT_ALL_STATUS_PCIE_INT_AER_LEGACY_MASK                                                 0x4000
#define PCIE20_INT_ALL_STATUS_PCIE_INT_AER_LEGACY_SHFT                                                 0xE
#define PCIE20_INT_ALL_STATUS_PCIE_INT_PLS_LINK_UP_MASK                                                0x2000
#define PCIE20_INT_ALL_STATUS_PCIE_INT_PLS_LINK_UP_SHFT                                                0xD
#define PCIE20_INT_ALL_STATUS_PCIE_INT_LINK_REQ_RST_FLUSH_MASK                                         0x1000
#define PCIE20_INT_ALL_STATUS_PCIE_INT_LINK_REQ_RST_FLUSH_SHFT                                         0xC
#define PCIE20_INT_ALL_STATUS_PCIE_INT_CFG_WRITE_MASK                                                  0x800
#define PCIE20_INT_ALL_STATUS_PCIE_INT_CFG_WRITE_SHFT                                                  0xB
#define PCIE20_INT_ALL_STATUS_PCIE_INT_MMIO_WRITE_MASK                                                 0x400
#define PCIE20_INT_ALL_STATUS_PCIE_INT_MMIO_WRITE_SHFT                                                 0xA
#define PCIE20_INT_ALL_STATUS_PCIE_INT_L1SS_IDLE_TO_MASK                                               0x200
#define PCIE20_INT_ALL_STATUS_PCIE_INT_L1SS_IDLE_TO_SHFT                                               0x9
#define PCIE20_INT_ALL_STATUS_PCIE_INT_DSTATE_CHANGE_MASK                                              0x100
#define PCIE20_INT_ALL_STATUS_PCIE_INT_DSTATE_CHANGE_SHFT                                              0x8
#define PCIE20_INT_ALL_STATUS_PCIE_INT_MHI_A7_MASK                                                     0x80
#define PCIE20_INT_ALL_STATUS_PCIE_INT_MHI_A7_SHFT                                                     0x7
#define PCIE20_INT_ALL_STATUS_PCIE_INT_MHI_Q6_MASK                                                     0x40
#define PCIE20_INT_ALL_STATUS_PCIE_INT_MHI_Q6_SHFT                                                     0x6
#define PCIE20_INT_ALL_STATUS_PCIE_INT_LTR_MASK                                                        0x20
#define PCIE20_INT_ALL_STATUS_PCIE_INT_LTR_SHFT                                                        0x5
#define PCIE20_INT_ALL_STATUS_PCIE_INT_DEBUG_MASK                                                      0x10
#define PCIE20_INT_ALL_STATUS_PCIE_INT_DEBUG_SHFT                                                      0x4
#define PCIE20_INT_ALL_STATUS_PCIE_RADM_TURNOFF_MASK                                                   0x8
#define PCIE20_INT_ALL_STATUS_PCIE_RADM_TURNOFF_SHFT                                                   0x3
#define PCIE20_INT_ALL_STATUS_PCIE_CFG_BUS_MASTER_EN_MASK                                              0x4
#define PCIE20_INT_ALL_STATUS_PCIE_CFG_BUS_MASTER_EN_SHFT                                              0x2
#define PCIE20_INT_ALL_STATUS_PCIE_SMLH_REQ_RST_LINK_DOWN_MASK                                         0x2
#define PCIE20_INT_ALL_STATUS_PCIE_SMLH_REQ_RST_LINK_DOWN_SHFT                                         0x1

/* PCIE20_INT_ALL_CLEAR */
#define PCIE20_INT_ALL_CLEAR                                                                           ( 0x228 )
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_MSI_DEV_0_MASK                                                   0x40000000
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_MSI_DEV_0_SHFT                                                   0x1E
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_MSI_DEV_1_MASK                                                   0x20000000
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_MSI_DEV_1_SHFT                                                   0x1D
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_MSI_DEV_2_MASK                                                   0x10000000
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_MSI_DEV_2_SHFT                                                   0x1C
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_MSI_DEV_3_MASK                                                   0x8000000
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_MSI_DEV_3_SHFT                                                   0x1B
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_MSI_DEV_4_MASK                                                   0x4000000
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_MSI_DEV_4_SHFT                                                   0x1A
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_MSI_DEV_5_MASK                                                   0x2000000
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_MSI_DEV_5_SHFT                                                   0x19
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_MSI_DEV_6_MASK                                                   0x1000000
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_MSI_DEV_6_SHFT                                                   0x18
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_MSI_DEV_7_MASK                                                   0x800000
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_MSI_DEV_7_SHFT                                                   0x17
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_EDMA_INT_MASK                                                    0x400000
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_EDMA_INT_SHFT                                                    0x16
#define PCIE20_INT_ALL_CLEAR_PCIE_INTA_MASK                                                            0x200000
#define PCIE20_INT_ALL_CLEAR_PCIE_INTA_SHFT                                                            0x15
#define PCIE20_INT_ALL_CLEAR_PCIE_INTB_MASK                                                            0x100000
#define PCIE20_INT_ALL_CLEAR_PCIE_INTB_SHFT                                                            0x14
#define PCIE20_INT_ALL_CLEAR_PCIE_INTC_MASK                                                            0x80000
#define PCIE20_INT_ALL_CLEAR_PCIE_INTC_SHFT                                                            0x13
#define PCIE20_INT_ALL_CLEAR_PCIE_INTD_MASK                                                            0x40000
#define PCIE20_INT_ALL_CLEAR_PCIE_INTD_SHFT                                                            0x12
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_PLS_PME_MASK                                                     0x20000
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_PLS_PME_SHFT                                                     0x11
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_PME_LEGACY_MASK                                                  0x10000
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_PME_LEGACY_SHFT                                                  0x10
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_PLS_ERR_MASK                                                     0x8000
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_PLS_ERR_SHFT                                                     0xF
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_AER_LEGACY_MASK                                                  0x4000
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_AER_LEGACY_SHFT                                                  0xE
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_PLS_LINK_UP_MASK                                                 0x2000
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_PLS_LINK_UP_SHFT                                                 0xD
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_LINK_REQ_RST_FLUSH_MASK                                          0x1000
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_LINK_REQ_RST_FLUSH_SHFT                                          0xC
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_CFG_WRITE_MASK                                                   0x800
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_CFG_WRITE_SHFT                                                   0xB
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_MMIO_WRITE_MASK                                                  0x400
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_MMIO_WRITE_SHFT                                                  0xA
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_L1SS_IDLE_TO_MASK                                                0x200
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_L1SS_IDLE_TO_SHFT                                                0x9
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_DSTATE_CHANGE_MASK                                               0x100
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_DSTATE_CHANGE_SHFT                                               0x8
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_MHI_A7_MASK                                                      0x80
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_MHI_A7_SHFT                                                      0x7
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_MHI_Q6_MASK                                                      0x40
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_MHI_Q6_SHFT                                                      0x6
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_LTR_MASK                                                         0x20
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_LTR_SHFT                                                         0x5
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_DEBUG_MASK                                                       0x10
#define PCIE20_INT_ALL_CLEAR_PCIE_INT_DEBUG_SHFT                                                       0x4
#define PCIE20_INT_ALL_CLEAR_PCIE_RADM_TURNOFF_MASK                                                    0x8
#define PCIE20_INT_ALL_CLEAR_PCIE_RADM_TURNOFF_SHFT                                                    0x3
#define PCIE20_INT_ALL_CLEAR_PCIE_CFG_BUS_MASTER_EN_MASK                                               0x4
#define PCIE20_INT_ALL_CLEAR_PCIE_CFG_BUS_MASTER_EN_SHFT                                               0x2
#define PCIE20_INT_ALL_CLEAR_PCIE_SMLH_REQ_RST_LINK_DOWN_MASK                                          0x2
#define PCIE20_INT_ALL_CLEAR_PCIE_SMLH_REQ_RST_LINK_DOWN_SHFT                                          0x1

/* PCIE20_INT_ALL_MASK */
#define PCIE20_INT_ALL_MASK                                                                            ( 0x22C )
#define PCIE20_INT_ALL_MASK_PCIE_INT_MSI_DEV_0_MASK                                                    0x40000000
#define PCIE20_INT_ALL_MASK_PCIE_INT_MSI_DEV_0_SHFT                                                    0x1E
#define PCIE20_INT_ALL_MASK_PCIE_INT_MSI_DEV_1_MASK                                                    0x20000000
#define PCIE20_INT_ALL_MASK_PCIE_INT_MSI_DEV_1_SHFT                                                    0x1D
#define PCIE20_INT_ALL_MASK_PCIE_INT_MSI_DEV_2_MASK                                                    0x10000000
#define PCIE20_INT_ALL_MASK_PCIE_INT_MSI_DEV_2_SHFT                                                    0x1C
#define PCIE20_INT_ALL_MASK_PCIE_INT_MSI_DEV_3_MASK                                                    0x8000000
#define PCIE20_INT_ALL_MASK_PCIE_INT_MSI_DEV_3_SHFT                                                    0x1B
#define PCIE20_INT_ALL_MASK_PCIE_INT_MSI_DEV_4_MASK                                                    0x4000000
#define PCIE20_INT_ALL_MASK_PCIE_INT_MSI_DEV_4_SHFT                                                    0x1A
#define PCIE20_INT_ALL_MASK_PCIE_INT_MSI_DEV_5_MASK                                                    0x2000000
#define PCIE20_INT_ALL_MASK_PCIE_INT_MSI_DEV_5_SHFT                                                    0x19
#define PCIE20_INT_ALL_MASK_PCIE_INT_MSI_DEV_6_MASK                                                    0x1000000
#define PCIE20_INT_ALL_MASK_PCIE_INT_MSI_DEV_6_SHFT                                                    0x18
#define PCIE20_INT_ALL_MASK_PCIE_INT_MSI_DEV_7_MASK                                                    0x800000
#define PCIE20_INT_ALL_MASK_PCIE_INT_MSI_DEV_7_SHFT                                                    0x17
#define PCIE20_INT_ALL_MASK_PCIE_INT_EDMA_INT_MASK                                                     0x400000
#define PCIE20_INT_ALL_MASK_PCIE_INT_EDMA_INT_SHFT                                                     0x16
#define PCIE20_INT_ALL_MASK_PCIE_INTA_MASK                                                             0x200000
#define PCIE20_INT_ALL_MASK_PCIE_INTA_SHFT                                                             0x15
#define PCIE20_INT_ALL_MASK_PCIE_INTB_MASK                                                             0x100000
#define PCIE20_INT_ALL_MASK_PCIE_INTB_SHFT                                                             0x14
#define PCIE20_INT_ALL_MASK_PCIE_INTC_MASK                                                             0x80000
#define PCIE20_INT_ALL_MASK_PCIE_INTC_SHFT                                                             0x13
#define PCIE20_INT_ALL_MASK_PCIE_INTD_MASK                                                             0x40000
#define PCIE20_INT_ALL_MASK_PCIE_INTD_SHFT                                                             0x12
#define PCIE20_INT_ALL_MASK_PCIE_INT_PLS_PME_MASK                                                      0x20000
#define PCIE20_INT_ALL_MASK_PCIE_INT_PLS_PME_SHFT                                                      0x11
#define PCIE20_INT_ALL_MASK_PCIE_INT_PME_LEGACY_MASK                                                   0x10000
#define PCIE20_INT_ALL_MASK_PCIE_INT_PME_LEGACY_SHFT                                                   0x10
#define PCIE20_INT_ALL_MASK_PCIE_INT_PLS_ERR_MASK                                                      0x8000
#define PCIE20_INT_ALL_MASK_PCIE_INT_PLS_ERR_SHFT                                                      0xF
#define PCIE20_INT_ALL_MASK_PCIE_INT_AER_LEGACY_MASK                                                   0x4000
#define PCIE20_INT_ALL_MASK_PCIE_INT_AER_LEGACY_SHFT                                                   0xE
#define PCIE20_INT_ALL_MASK_PCIE_INT_PLS_LINK_UP_MASK                                                  0x2000
#define PCIE20_INT_ALL_MASK_PCIE_INT_PLS_LINK_UP_SHFT                                                  0xD
#define PCIE20_INT_ALL_MASK_PCIE_INT_LINK_REQ_RST_FLUSH_MASK                                           0x1000
#define PCIE20_INT_ALL_MASK_PCIE_INT_LINK_REQ_RST_FLUSH_SHFT                                           0xC
#define PCIE20_INT_ALL_MASK_PCIE_INT_CFG_WRITE_MASK                                                    0x800
#define PCIE20_INT_ALL_MASK_PCIE_INT_CFG_WRITE_SHFT                                                    0xB
#define PCIE20_INT_ALL_MASK_PCIE_INT_MMIO_WRITE_MASK                                                   0x400
#define PCIE20_INT_ALL_MASK_PCIE_INT_MMIO_WRITE_SHFT                                                   0xA
#define PCIE20_INT_ALL_MASK_PCIE_INT_L1SS_IDLE_TO_MASK                                                 0x200
#define PCIE20_INT_ALL_MASK_PCIE_INT_L1SS_IDLE_TO_SHFT                                                 0x9
#define PCIE20_INT_ALL_MASK_PCIE_INT_DSTATE_CHANGE_MASK                                                0x100
#define PCIE20_INT_ALL_MASK_PCIE_INT_DSTATE_CHANGE_SHFT                                                0x8
#define PCIE20_INT_ALL_MASK_PCIE_INT_MHI_A7_MASK                                                       0x80
#define PCIE20_INT_ALL_MASK_PCIE_INT_MHI_A7_SHFT                                                       0x7
#define PCIE20_INT_ALL_MASK_PCIE_INT_MHI_Q6_MASK                                                       0x40
#define PCIE20_INT_ALL_MASK_PCIE_INT_MHI_Q6_SHFT                                                       0x6
#define PCIE20_INT_ALL_MASK_PCIE_INT_LTR_MASK                                                          0x20
#define PCIE20_INT_ALL_MASK_PCIE_INT_LTR_SHFT                                                          0x5
#define PCIE20_INT_ALL_MASK_PCIE_INT_DEBUG_MASK                                                        0x10
#define PCIE20_INT_ALL_MASK_PCIE_INT_DEBUG_SHFT                                                        0x4
#define PCIE20_INT_ALL_MASK_PCIE_RADM_TURNOFF_MASK                                                     0x8
#define PCIE20_INT_ALL_MASK_PCIE_RADM_TURNOFF_SHFT                                                     0x3
#define PCIE20_INT_ALL_MASK_PCIE_CFG_BUS_MASTER_EN_MASK                                                0x4
#define PCIE20_INT_ALL_MASK_PCIE_CFG_BUS_MASTER_EN_SHFT                                                0x2
#define PCIE20_INT_ALL_MASK_PCIE_SMLH_REQ_RST_LINK_DOWN_MASK                                           0x2
#define PCIE20_INT_ALL_MASK_PCIE_SMLH_REQ_RST_LINK_DOWN_SHFT                                           0x1

/* PCIE20_STATUS */
#define PCIE20_STATUS                                                                                  ( 0x230 )
#define PCIE20_STATUS_FLUSH_COMPLETED_MASK                                                             0x100
#define PCIE20_STATUS_FLUSH_COMPLETED_SHFT                                                             0x8
#define PCIE20_STATUS_RADM_Q_NOT_EMPTY_MASK                                                            0x80
#define PCIE20_STATUS_RADM_Q_NOT_EMPTY_SHFT                                                            0x7
#define PCIE20_STATUS_SMLH_REQ_RST_NOT_MASK                                                            0x40
#define PCIE20_STATUS_SMLH_REQ_RST_NOT_SHFT                                                            0x6
#define PCIE20_STATUS_RADM_XFER_PENDING_MASK                                                           0x20
#define PCIE20_STATUS_RADM_XFER_PENDING_SHFT                                                           0x5
#define PCIE20_STATUS_EDMA_XFER_PENDING_MASK                                                           0x10
#define PCIE20_STATUS_EDMA_XFER_PENDING_SHFT                                                           0x4
#define PCIE20_STATUS_BRIDGE_SLV_XFER_PENDING_MASK                                                     0x8
#define PCIE20_STATUS_BRIDGE_SLV_XFER_PENDING_SHFT                                                     0x3
#define PCIE20_STATUS_PHY_RESET_ACK_STS_MASK                                                           0x4
#define PCIE20_STATUS_PHY_RESET_ACK_STS_SHFT                                                           0x2
#define PCIE20_STATUS_PHY_RESET_REQ_STS_MASK                                                           0x2
#define PCIE20_STATUS_PHY_RESET_REQ_STS_SHFT                                                           0x1
#define PCIE20_STATUS_BRIDGE_FLUSH_NOT_MASK                                                            0x1
#define PCIE20_STATUS_BRIDGE_FLUSH_NOT_SHFT                                                            0x0

/* PCIE20_PARF_SID_OFFSET */
#define PCIE20_PARF_SID_OFFSET                                                                         ( 0x234 )
#define PCIE20_PARF_SID_OFFSET_OFFSET_MASK                                                             0xFFFF
#define PCIE20_PARF_SID_OFFSET_OFFSET_SHFT                                                             0x0

/* PCIE20_PARF_PHY_RST_REQ */
#define PCIE20_PARF_PHY_RST_REQ                                                                        ( 0x238 )
#define PCIE20_PARF_PHY_RST_REQ_PHY_RESET_REQ_INT_SEL_MASK                                             0x18
#define PCIE20_PARF_PHY_RST_REQ_PHY_RESET_REQ_INT_SEL_SHFT                                             0x3
#define PCIE20_PARF_PHY_RST_REQ_PHY_RESET_REQ_LINK_DOWN_MASK                                           0x4
#define PCIE20_PARF_PHY_RST_REQ_PHY_RESET_REQ_LINK_DOWN_SHFT                                           0x2
#define PCIE20_PARF_PHY_RST_REQ_PHY_RESET_ACK_SW_EN_MASK                                               0x2
#define PCIE20_PARF_PHY_RST_REQ_PHY_RESET_ACK_SW_EN_SHFT                                               0x1
#define PCIE20_PARF_PHY_RST_REQ_PHY_RESET_ACK_SW_MASK                                                  0x1
#define PCIE20_PARF_PHY_RST_REQ_PHY_RESET_ACK_SW_SHFT                                                  0x0

/* PCIE20_PARF_BDF_TRANSLATE_CFG */
#define PCIE20_PARF_BDF_TRANSLATE_CFG                                                                  ( 0x24C )
#define PCIE20_PARF_BDF_TRANSLATE_CFG_DISABLE_BDF_TRANSLATE_MASK                                       0x10
#define PCIE20_PARF_BDF_TRANSLATE_CFG_DISABLE_BDF_TRANSLATE_SHFT                                       0x4
#define PCIE20_PARF_BDF_TRANSLATE_CFG_DEFAULT_TRANSLATE_VALUE_MASK                                     0xF
#define PCIE20_PARF_BDF_TRANSLATE_CFG_DEFAULT_TRANSLATE_VALUE_SHFT                                     0x0

/* PCIE20_PARF_BDF_TRANSLATE_n */
#define PCIE20_PARF_BDF_TRANSLATE_n(n)                                                                 ( 0x250 + 0x4 * (n) )
#define PCIE20_PARF_BDF_TRANSLATE_n_BDF_VALUE_MASK                                                     0xFFFF
#define PCIE20_PARF_BDF_TRANSLATE_n_BDF_VALUE_SHFT                                                     0x0

/* PCIE20_PARF_CLKREQ_OVERRIDE */
#define PCIE20_PARF_CLKREQ_OVERRIDE                                                                    ( 0x2B0 )
#define PCIE20_PARF_CLKREQ_OVERRIDE_CLKREQ_IN_OVERRIDE_STS_MASK                                        0x20
#define PCIE20_PARF_CLKREQ_OVERRIDE_CLKREQ_IN_OVERRIDE_STS_SHFT                                        0x5
#define PCIE20_PARF_CLKREQ_OVERRIDE_CLKREQ_OE_OVERRIDE_STS_MASK                                        0x10
#define PCIE20_PARF_CLKREQ_OVERRIDE_CLKREQ_OE_OVERRIDE_STS_SHFT                                        0x4
#define PCIE20_PARF_CLKREQ_OVERRIDE_CLKREQ_IN_OVERRIDE_VALUE_MASK                                      0x8
#define PCIE20_PARF_CLKREQ_OVERRIDE_CLKREQ_IN_OVERRIDE_VALUE_SHFT                                      0x3
#define PCIE20_PARF_CLKREQ_OVERRIDE_CLKREQ_OE_OVERRIDE_VALUE_MASK                                      0x4
#define PCIE20_PARF_CLKREQ_OVERRIDE_CLKREQ_OE_OVERRIDE_VALUE_SHFT                                      0x2
#define PCIE20_PARF_CLKREQ_OVERRIDE_CLKREQ_IN_OVERRIDE_ENABLE_MASK                                     0x2
#define PCIE20_PARF_CLKREQ_OVERRIDE_CLKREQ_IN_OVERRIDE_ENABLE_SHFT                                     0x1
#define PCIE20_PARF_CLKREQ_OVERRIDE_CLKREQ_OE_OVERRIDE_ENABLE_MASK                                     0x1
#define PCIE20_PARF_CLKREQ_OVERRIDE_CLKREQ_OE_OVERRIDE_ENABLE_SHFT                                     0x0

/* PCIE20_PARF_MISC_INFO */
#define PCIE20_PARF_MISC_INFO                                                                          ( 0x2B4 )
#define PCIE20_PARF_MISC_INFO_MSTR_RMISC_INFO_MASK                                                     0x1FFF
#define PCIE20_PARF_MISC_INFO_MSTR_RMISC_INFO_SHFT                                                     0x0

/* PCIE20_PARF_MSTR1_REQ_NOT_ENTR_L1 */
#define PCIE20_PARF_MSTR1_REQ_NOT_ENTR_L1                                                              ( 0x2B8 )
#define PCIE20_PARF_MSTR1_REQ_NOT_ENTR_L1_REQ_NOT_ENTR_L1_MASK                                         0x1
#define PCIE20_PARF_MSTR1_REQ_NOT_ENTR_L1_REQ_NOT_ENTR_L1_SHFT                                         0x0

/* PCIE20_PARF_CFG_BITS_2 */
#define PCIE20_PARF_CFG_BITS_2                                                                         ( 0x2BC )
#define PCIE20_PARF_CFG_BITS_2_NOC_MSTR_AWREADY_FIX2_EN_MASK                                           0x4
#define PCIE20_PARF_CFG_BITS_2_NOC_MSTR_AWREADY_FIX2_EN_SHFT                                           0x2
#define PCIE20_PARF_CFG_BITS_2_RC_B_CHNL_WR_RESP_Q_OVERFLOW_FIX_DIS_MASK                               0x2
#define PCIE20_PARF_CFG_BITS_2_RC_B_CHNL_WR_RESP_Q_OVERFLOW_FIX_DIS_SHFT                               0x1
#define PCIE20_PARF_CFG_BITS_2_RC_L1_ASPM_ENTRY_DEADLOCK_FIX_DIS_MASK                                  0x1
#define PCIE20_PARF_CFG_BITS_2_RC_L1_ASPM_ENTRY_DEADLOCK_FIX_DIS_SHFT                                  0x0

/* PCIE20_PARF_SLV_ADDR_MSB_CTRL */
#define PCIE20_PARF_SLV_ADDR_MSB_CTRL                                                                  ( 0x2C0 )
#define PCIE20_PARF_SLV_ADDR_MSB_CTRL_SLV_ADDR_MSB_CTRL_MASK                                           0x3
#define PCIE20_PARF_SLV_ADDR_MSB_CTRL_SLV_ADDR_MSB_CTRL_SHFT                                           0x0

/* PCIE20_PARF_MSG_ADDR_n */
#define PCIE20_PARF_MSG_ADDR_n(n)                                                                      ( 0x2D0 + 0x4 * (n) )
#define PCIE20_PARF_MSG_ADDR_n_MSG_ADDR_MASK                                                           0xFFFFFFFF
#define PCIE20_PARF_MSG_ADDR_n_MSG_ADDR_SHFT                                                           0x0

/* PCIE20_PARF_MSG_ADDR_HI_n */
#define PCIE20_PARF_MSG_ADDR_HI_n(n)                                                                   ( 0x310 + 0x4 * (n) )
#define PCIE20_PARF_MSG_ADDR_HI_n_MSG_ADDR_HI_MASK                                                     0xFFFFFFFF
#define PCIE20_PARF_MSG_ADDR_HI_n_MSG_ADDR_HI_SHFT                                                     0x0

/* PCIE20_PARF_DBI_BASE_ADDR */
#define PCIE20_PARF_DBI_BASE_ADDR                                                                      ( 0x350 )
#define PCIE20_PARF_DBI_BASE_ADDR_DBI_BASE_ADDR_MASK                                                   0xFFFFFFFF
#define PCIE20_PARF_DBI_BASE_ADDR_DBI_BASE_ADDR_SHFT                                                   0x0

/* PCIE20_PARF_DBI_BASE_ADDR_HI */
#define PCIE20_PARF_DBI_BASE_ADDR_HI                                                                   ( 0x354 )
#define PCIE20_PARF_DBI_BASE_ADDR_HI_DBI_BASE_ADDR_HI_MASK                                             0xFFFFFFFF
#define PCIE20_PARF_DBI_BASE_ADDR_HI_DBI_BASE_ADDR_HI_SHFT                                             0x0

/* PCIE20_PARF_SLV_ADDR_SPACE_SIZE */
#define PCIE20_PARF_SLV_ADDR_SPACE_SIZE                                                                ( 0x358 )
#define PCIE20_PARF_SLV_ADDR_SPACE_SIZE_SLV_ADDR_SPACE_SIZE_MASK                                       0xFFFFFFFF
#define PCIE20_PARF_SLV_ADDR_SPACE_SIZE_SLV_ADDR_SPACE_SIZE_SHFT                                       0x0

/* PCIE20_PARF_SLV_ADDR_SPACE_SIZE_HI */
#define PCIE20_PARF_SLV_ADDR_SPACE_SIZE_HI                                                             ( 0x35C )
#define PCIE20_PARF_SLV_ADDR_SPACE_SIZE_HI_SLV_ADDR_SPACE_SIZE_HI_MASK                                 0xFFFFFFFF
#define PCIE20_PARF_SLV_ADDR_SPACE_SIZE_HI_SLV_ADDR_SPACE_SIZE_HI_SHFT                                 0x0

/* PCIE20_PARF_BLOCK_SLV_AXI_WR_BASE */
#define PCIE20_PARF_BLOCK_SLV_AXI_WR_BASE                                                              ( 0x360 )
#define PCIE20_PARF_BLOCK_SLV_AXI_WR_BASE_ADDR_BASE_MASK                                               0xFFFFFFFF
#define PCIE20_PARF_BLOCK_SLV_AXI_WR_BASE_ADDR_BASE_SHFT                                               0x0

/* PCIE20_PARF_BLOCK_SLV_AXI_WR_BASE_HI */
#define PCIE20_PARF_BLOCK_SLV_AXI_WR_BASE_HI                                                           ( 0x364 )
#define PCIE20_PARF_BLOCK_SLV_AXI_WR_BASE_HI_ADDR_BASE_HI_MASK                                         0xFFFFFFFF
#define PCIE20_PARF_BLOCK_SLV_AXI_WR_BASE_HI_ADDR_BASE_HI_SHFT                                         0x0

/* PCIE20_PARF_BLOCK_SLV_AXI_WR_LIMIT */
#define PCIE20_PARF_BLOCK_SLV_AXI_WR_LIMIT                                                             ( 0x368 )
#define PCIE20_PARF_BLOCK_SLV_AXI_WR_LIMIT_ADDR_BASE_MASK                                              0xFFFFFFFF
#define PCIE20_PARF_BLOCK_SLV_AXI_WR_LIMIT_ADDR_BASE_SHFT                                              0x0

/* PCIE20_PARF_BLOCK_SLV_AXI_WR_LIMIT_HI */
#define PCIE20_PARF_BLOCK_SLV_AXI_WR_LIMIT_HI                                                          ( 0x36C )
#define PCIE20_PARF_BLOCK_SLV_AXI_WR_LIMIT_HI_ADDR_BASE_HI_MASK                                        0xFFFFFFFF
#define PCIE20_PARF_BLOCK_SLV_AXI_WR_LIMIT_HI_ADDR_BASE_HI_SHFT                                        0x0

/* PCIE20_PARF_BLOCK_SLV_AXI_RD_BASE */
#define PCIE20_PARF_BLOCK_SLV_AXI_RD_BASE                                                              ( 0x370 )
#define PCIE20_PARF_BLOCK_SLV_AXI_RD_BASE_ADDR_BASE_MASK                                               0xFFFFFFFF
#define PCIE20_PARF_BLOCK_SLV_AXI_RD_BASE_ADDR_BASE_SHFT                                               0x0

/* PCIE20_PARF_BLOCK_SLV_AXI_RD_BASE_HI */
#define PCIE20_PARF_BLOCK_SLV_AXI_RD_BASE_HI                                                           ( 0x374 )
#define PCIE20_PARF_BLOCK_SLV_AXI_RD_BASE_HI_ADDR_BASE_HI_MASK                                         0xFFFFFFFF
#define PCIE20_PARF_BLOCK_SLV_AXI_RD_BASE_HI_ADDR_BASE_HI_SHFT                                         0x0

/* PCIE20_PARF_BLOCK_SLV_AXI_RD_LIMIT */
#define PCIE20_PARF_BLOCK_SLV_AXI_RD_LIMIT                                                             ( 0x378 )
#define PCIE20_PARF_BLOCK_SLV_AXI_RD_LIMIT_ADDR_BASE_MASK                                              0xFFFFFFFF
#define PCIE20_PARF_BLOCK_SLV_AXI_RD_LIMIT_ADDR_BASE_SHFT                                              0x0

/* PCIE20_PARF_BLOCK_SLV_AXI_RD_LIMIT_HI */
#define PCIE20_PARF_BLOCK_SLV_AXI_RD_LIMIT_HI                                                          ( 0x37C )
#define PCIE20_PARF_BLOCK_SLV_AXI_RD_LIMIT_HI_ADDR_BASE_HI_MASK                                        0xFFFFFFFF
#define PCIE20_PARF_BLOCK_SLV_AXI_RD_LIMIT_HI_ADDR_BASE_HI_SHFT                                        0x0

/* PCIE20_PARF_ECAM_BASE */
#define PCIE20_PARF_ECAM_BASE                                                                          ( 0x380 )
#define PCIE20_PARF_ECAM_BASE_BASE_MASK                                                                0xFFFFFFFF
#define PCIE20_PARF_ECAM_BASE_BASE_SHFT                                                                0x0

/* PCIE20_PARF_ECAM_BASE_HI */
#define PCIE20_PARF_ECAM_BASE_HI                                                                       ( 0x384 )
#define PCIE20_PARF_ECAM_BASE_HI_BASE_HI_MASK                                                          0xFFFFFFFF
#define PCIE20_PARF_ECAM_BASE_HI_BASE_HI_SHFT                                                          0x0

/* PCIE20_PARF_ECAM_OFFSET_REMOVAL_BASE */
#define PCIE20_PARF_ECAM_OFFSET_REMOVAL_BASE                                                           ( 0x388 )
#define PCIE20_PARF_ECAM_OFFSET_REMOVAL_BASE_ADDR_BASE_MASK                                            0xFFFFFFFF
#define PCIE20_PARF_ECAM_OFFSET_REMOVAL_BASE_ADDR_BASE_SHFT                                            0x0

/* PCIE20_PARF_ECAM_OFFSET_REMOVAL_BASE_HI */
#define PCIE20_PARF_ECAM_OFFSET_REMOVAL_BASE_HI                                                        ( 0x38C )
#define PCIE20_PARF_ECAM_OFFSET_REMOVAL_BASE_HI_ADDR_BASE_HI_MASK                                      0xFFFFFFFF
#define PCIE20_PARF_ECAM_OFFSET_REMOVAL_BASE_HI_ADDR_BASE_HI_SHFT                                      0x0

/* PCIE20_PARF_ECAM_OFFSET_REMOVAL_LIMIT */
#define PCIE20_PARF_ECAM_OFFSET_REMOVAL_LIMIT                                                          ( 0x390 )
#define PCIE20_PARF_ECAM_OFFSET_REMOVAL_LIMIT_ADDR_LIMIT_MASK                                          0xFFFFFFFF
#define PCIE20_PARF_ECAM_OFFSET_REMOVAL_LIMIT_ADDR_LIMIT_SHFT                                          0x0

/* PCIE20_PARF_ECAM_OFFSET_REMOVAL_LIMIT_HI */
#define PCIE20_PARF_ECAM_OFFSET_REMOVAL_LIMIT_HI                                                       ( 0x394 )
#define PCIE20_PARF_ECAM_OFFSET_REMOVAL_LIMIT_HI_ADDR_LIMIT_HI_MASK                                    0xFFFFFFFF
#define PCIE20_PARF_ECAM_OFFSET_REMOVAL_LIMIT_HI_ADDR_LIMIT_HI_SHFT                                    0x0

/* PCIE20_PARF_BLOCK_SLV_AXI_WR_BASE_2 */
#define PCIE20_PARF_BLOCK_SLV_AXI_WR_BASE_2                                                            ( 0x398 )
#define PCIE20_PARF_BLOCK_SLV_AXI_WR_BASE_2_ADDR_BASE_MASK                                             0xFFFFFFFF
#define PCIE20_PARF_BLOCK_SLV_AXI_WR_BASE_2_ADDR_BASE_SHFT                                             0x0

/* PCIE20_PARF_BLOCK_SLV_AXI_WR_BASE_2_HI */
#define PCIE20_PARF_BLOCK_SLV_AXI_WR_BASE_2_HI                                                         ( 0x39C )
#define PCIE20_PARF_BLOCK_SLV_AXI_WR_BASE_2_HI_ADDR_BASE_HI_MASK                                       0xFFFFFFFF
#define PCIE20_PARF_BLOCK_SLV_AXI_WR_BASE_2_HI_ADDR_BASE_HI_SHFT                                       0x0

/* PCIE20_PARF_BLOCK_SLV_AXI_WR_LIMIT_2 */
#define PCIE20_PARF_BLOCK_SLV_AXI_WR_LIMIT_2                                                           ( 0x3A0 )
#define PCIE20_PARF_BLOCK_SLV_AXI_WR_LIMIT_2_ADDR_LIMIT_MASK                                           0xFFFFFFFF
#define PCIE20_PARF_BLOCK_SLV_AXI_WR_LIMIT_2_ADDR_LIMIT_SHFT                                           0x0

/* PCIE20_PARF_BLOCK_SLV_AXI_WR_LIMIT_2_HI */
#define PCIE20_PARF_BLOCK_SLV_AXI_WR_LIMIT_2_HI                                                        ( 0x3A4 )
#define PCIE20_PARF_BLOCK_SLV_AXI_WR_LIMIT_2_HI_ADDR_LIMIT_HI_MASK                                     0xFFFFFFFF
#define PCIE20_PARF_BLOCK_SLV_AXI_WR_LIMIT_2_HI_ADDR_LIMIT_HI_SHFT                                     0x0

/* PCIE20_PARF_BLOCK_SLV_AXI_RD_BASE_2 */
#define PCIE20_PARF_BLOCK_SLV_AXI_RD_BASE_2                                                            ( 0x3A8 )
#define PCIE20_PARF_BLOCK_SLV_AXI_RD_BASE_2_ADDR_BASE_MASK                                             0xFFFFFFFF
#define PCIE20_PARF_BLOCK_SLV_AXI_RD_BASE_2_ADDR_BASE_SHFT                                             0x0

/* PCIE20_PARF_BLOCK_SLV_AXI_RD_BASE_2_HI */
#define PCIE20_PARF_BLOCK_SLV_AXI_RD_BASE_2_HI                                                         ( 0x3AC )
#define PCIE20_PARF_BLOCK_SLV_AXI_RD_BASE_2_HI_ADDR_BASE_HI_MASK                                       0xFFFFFFFF
#define PCIE20_PARF_BLOCK_SLV_AXI_RD_BASE_2_HI_ADDR_BASE_HI_SHFT                                       0x0

/* PCIE20_PARF_BLOCK_SLV_AXI_RD_LIMIT_2 */
#define PCIE20_PARF_BLOCK_SLV_AXI_RD_LIMIT_2                                                           ( 0x3B0 )
#define PCIE20_PARF_BLOCK_SLV_AXI_RD_LIMIT_2_ADDR_LIMIT_MASK                                           0xFFFFFFFF
#define PCIE20_PARF_BLOCK_SLV_AXI_RD_LIMIT_2_ADDR_LIMIT_SHFT                                           0x0

/* PCIE20_PARF_BLOCK_SLV_AXI_RD_LIMIT_2_HI */
#define PCIE20_PARF_BLOCK_SLV_AXI_RD_LIMIT_2_HI                                                        ( 0x3B4 )
#define PCIE20_PARF_BLOCK_SLV_AXI_RD_LIMIT_2_HI_ADDR_LIMIT_HI_MASK                                     0xFFFFFFFF
#define PCIE20_PARF_BLOCK_SLV_AXI_RD_LIMIT_2_HI_ADDR_LIMIT_HI_SHFT                                     0x0

/* PCIE20_PARF_CORE_ERRORS */
#define PCIE20_PARF_CORE_ERRORS                                                                        ( 0x3C0 )
#define PCIE20_PARF_CORE_ERRORS_CFG_AER_RC_ERR_MSI_MASK                                                0x20
#define PCIE20_PARF_CORE_ERRORS_CFG_AER_RC_ERR_MSI_SHFT                                                0x5
#define PCIE20_PARF_CORE_ERRORS_CFG_SYS_ERR_RC_MASK                                                    0x10
#define PCIE20_PARF_CORE_ERRORS_CFG_SYS_ERR_RC_SHFT                                                    0x4
#define PCIE20_PARF_CORE_ERRORS_RADM_CORRECTABLE_ERR_MASK                                              0x8
#define PCIE20_PARF_CORE_ERRORS_RADM_CORRECTABLE_ERR_SHFT                                              0x3
#define PCIE20_PARF_CORE_ERRORS_RADM_NONFATAL_ERR_MASK                                                 0x4
#define PCIE20_PARF_CORE_ERRORS_RADM_NONFATAL_ERR_SHFT                                                 0x2
#define PCIE20_PARF_CORE_ERRORS_RADM_FATAL_ERR_MASK                                                    0x2
#define PCIE20_PARF_CORE_ERRORS_RADM_FATAL_ERR_SHFT                                                    0x1
#define PCIE20_PARF_CORE_ERRORS_RADM_QOVERFLOW_MASK                                                    0x1
#define PCIE20_PARF_CORE_ERRORS_RADM_QOVERFLOW_SHFT                                                    0x0

/* PCIE20_PARF_SLAVE_AXI_ERROR_REPORT */
#define PCIE20_PARF_SLAVE_AXI_ERROR_REPORT                                                             ( 0x3C4 )
#define PCIE20_PARF_SLAVE_AXI_ERROR_REPORT_AXI_SLAVE_ERROR_CRS_BRESP_MASK                              0x18000
#define PCIE20_PARF_SLAVE_AXI_ERROR_REPORT_AXI_SLAVE_ERROR_CRS_BRESP_SHFT                              0xF
#define PCIE20_PARF_SLAVE_AXI_ERROR_REPORT_AXI_SLAVE_ERROR_CRS_BRESP_EN_MASK                           0x4000
#define PCIE20_PARF_SLAVE_AXI_ERROR_REPORT_AXI_SLAVE_ERROR_CRS_BRESP_EN_SHFT                           0xE
#define PCIE20_PARF_SLAVE_AXI_ERROR_REPORT_AXI_SLAVE_ERROR_POISONED_BRESP_MASK                         0x3000
#define PCIE20_PARF_SLAVE_AXI_ERROR_REPORT_AXI_SLAVE_ERROR_POISONED_BRESP_SHFT                         0xC
#define PCIE20_PARF_SLAVE_AXI_ERROR_REPORT_AXI_SLAVE_ERROR_POISONED_BRESP_EN_MASK                      0x800
#define PCIE20_PARF_SLAVE_AXI_ERROR_REPORT_AXI_SLAVE_ERROR_POISONED_BRESP_EN_SHFT                      0xB
#define PCIE20_PARF_SLAVE_AXI_ERROR_REPORT_AXI_SLAVE_ERROR_CRS_RDATA_EN_MASK                           0x400
#define PCIE20_PARF_SLAVE_AXI_ERROR_REPORT_AXI_SLAVE_ERROR_CRS_RDATA_EN_SHFT                           0xA
#define PCIE20_PARF_SLAVE_AXI_ERROR_REPORT_AXI_SLAVE_ERROR_POISONED_RDATA_EN_MASK                      0x200
#define PCIE20_PARF_SLAVE_AXI_ERROR_REPORT_AXI_SLAVE_ERROR_POISONED_RDATA_EN_SHFT                      0x9
#define PCIE20_PARF_SLAVE_AXI_ERROR_REPORT_CRS_VALUE_MASK                                              0x1C0
#define PCIE20_PARF_SLAVE_AXI_ERROR_REPORT_CRS_VALUE_SHFT                                              0x6
#define PCIE20_PARF_SLAVE_AXI_ERROR_REPORT_AXI_SLAVE_ERROR_CRS_RRESP_MASK                              0x30
#define PCIE20_PARF_SLAVE_AXI_ERROR_REPORT_AXI_SLAVE_ERROR_CRS_RRESP_SHFT                              0x4
#define PCIE20_PARF_SLAVE_AXI_ERROR_REPORT_AXI_SLAVE_ERROR_CRS_RRESP_EN_MASK                           0x8
#define PCIE20_PARF_SLAVE_AXI_ERROR_REPORT_AXI_SLAVE_ERROR_CRS_RRESP_EN_SHFT                           0x3
#define PCIE20_PARF_SLAVE_AXI_ERROR_REPORT_AXI_SLAVE_ERROR_POISONED_RRESP_MASK                         0x6
#define PCIE20_PARF_SLAVE_AXI_ERROR_REPORT_AXI_SLAVE_ERROR_POISONED_RRESP_SHFT                         0x1
#define PCIE20_PARF_SLAVE_AXI_ERROR_REPORT_AXI_SLAVE_ERROR_POISONED_RRESP_EN_MASK                      0x1
#define PCIE20_PARF_SLAVE_AXI_ERROR_REPORT_AXI_SLAVE_ERROR_POISONED_RRESP_EN_SHFT                      0x0

/* PCIE20_INT_ALL_TYPE */
#define PCIE20_INT_ALL_TYPE                                                                            ( 0x3C8 )
#define PCIE20_INT_ALL_TYPE_PCIE_INT_MSI_DEV_0_MASK                                                    0x40000000
#define PCIE20_INT_ALL_TYPE_PCIE_INT_MSI_DEV_0_SHFT                                                    0x1E
#define PCIE20_INT_ALL_TYPE_PCIE_INT_MSI_DEV_1_MASK                                                    0x20000000
#define PCIE20_INT_ALL_TYPE_PCIE_INT_MSI_DEV_1_SHFT                                                    0x1D
#define PCIE20_INT_ALL_TYPE_PCIE_INT_MSI_DEV_2_MASK                                                    0x10000000
#define PCIE20_INT_ALL_TYPE_PCIE_INT_MSI_DEV_2_SHFT                                                    0x1C
#define PCIE20_INT_ALL_TYPE_PCIE_INT_MSI_DEV_3_MASK                                                    0x8000000
#define PCIE20_INT_ALL_TYPE_PCIE_INT_MSI_DEV_3_SHFT                                                    0x1B
#define PCIE20_INT_ALL_TYPE_PCIE_INT_MSI_DEV_4_MASK                                                    0x4000000
#define PCIE20_INT_ALL_TYPE_PCIE_INT_MSI_DEV_4_SHFT                                                    0x1A
#define PCIE20_INT_ALL_TYPE_PCIE_INT_MSI_DEV_5_MASK                                                    0x2000000
#define PCIE20_INT_ALL_TYPE_PCIE_INT_MSI_DEV_5_SHFT                                                    0x19
#define PCIE20_INT_ALL_TYPE_PCIE_INT_MSI_DEV_6_MASK                                                    0x1000000
#define PCIE20_INT_ALL_TYPE_PCIE_INT_MSI_DEV_6_SHFT                                                    0x18
#define PCIE20_INT_ALL_TYPE_PCIE_INT_MSI_DEV_7_MASK                                                    0x800000
#define PCIE20_INT_ALL_TYPE_PCIE_INT_MSI_DEV_7_SHFT                                                    0x17
#define PCIE20_INT_ALL_TYPE_PCIE_INT_EDMA_INT_MASK                                                     0x400000
#define PCIE20_INT_ALL_TYPE_PCIE_INT_EDMA_INT_SHFT                                                     0x16
#define PCIE20_INT_ALL_TYPE_PCIE_INTA_MASK                                                             0x200000
#define PCIE20_INT_ALL_TYPE_PCIE_INTA_SHFT                                                             0x15
#define PCIE20_INT_ALL_TYPE_PCIE_INTB_MASK                                                             0x100000
#define PCIE20_INT_ALL_TYPE_PCIE_INTB_SHFT                                                             0x14
#define PCIE20_INT_ALL_TYPE_PCIE_INTC_MASK                                                             0x80000
#define PCIE20_INT_ALL_TYPE_PCIE_INTC_SHFT                                                             0x13
#define PCIE20_INT_ALL_TYPE_PCIE_INTD_MASK                                                             0x40000
#define PCIE20_INT_ALL_TYPE_PCIE_INTD_SHFT                                                             0x12
#define PCIE20_INT_ALL_TYPE_PCIE_INT_PLS_PME_MASK                                                      0x20000
#define PCIE20_INT_ALL_TYPE_PCIE_INT_PLS_PME_SHFT                                                      0x11
#define PCIE20_INT_ALL_TYPE_PCIE_INT_PME_LEGACY_MASK                                                   0x10000
#define PCIE20_INT_ALL_TYPE_PCIE_INT_PME_LEGACY_SHFT                                                   0x10
#define PCIE20_INT_ALL_TYPE_PCIE_INT_PLS_ERR_MASK                                                      0x8000
#define PCIE20_INT_ALL_TYPE_PCIE_INT_PLS_ERR_SHFT                                                      0xF
#define PCIE20_INT_ALL_TYPE_PCIE_INT_AER_LEGACY_MASK                                                   0x4000
#define PCIE20_INT_ALL_TYPE_PCIE_INT_AER_LEGACY_SHFT                                                   0xE
#define PCIE20_INT_ALL_TYPE_PCIE_INT_PLS_LINK_UP_MASK                                                  0x2000
#define PCIE20_INT_ALL_TYPE_PCIE_INT_PLS_LINK_UP_SHFT                                                  0xD
#define PCIE20_INT_ALL_TYPE_PCIE_INT_LINK_REQ_RST_FLUSH_MASK                                           0x1000
#define PCIE20_INT_ALL_TYPE_PCIE_INT_LINK_REQ_RST_FLUSH_SHFT                                           0xC
#define PCIE20_INT_ALL_TYPE_PCIE_INT_CFG_WRITE_MASK                                                    0x800
#define PCIE20_INT_ALL_TYPE_PCIE_INT_CFG_WRITE_SHFT                                                    0xB
#define PCIE20_INT_ALL_TYPE_PCIE_INT_MMIO_WRITE_MASK                                                   0x400
#define PCIE20_INT_ALL_TYPE_PCIE_INT_MMIO_WRITE_SHFT                                                   0xA
#define PCIE20_INT_ALL_TYPE_PCIE_INT_L1SS_IDLE_TO_MASK                                                 0x200
#define PCIE20_INT_ALL_TYPE_PCIE_INT_L1SS_IDLE_TO_SHFT                                                 0x9
#define PCIE20_INT_ALL_TYPE_PCIE_INT_DSTATE_CHANGE_MASK                                                0x100
#define PCIE20_INT_ALL_TYPE_PCIE_INT_DSTATE_CHANGE_SHFT                                                0x8
#define PCIE20_INT_ALL_TYPE_PCIE_INT_MHI_A7_MASK                                                       0x80
#define PCIE20_INT_ALL_TYPE_PCIE_INT_MHI_A7_SHFT                                                       0x7
#define PCIE20_INT_ALL_TYPE_PCIE_INT_MHI_Q6_MASK                                                       0x40
#define PCIE20_INT_ALL_TYPE_PCIE_INT_MHI_Q6_SHFT                                                       0x6
#define PCIE20_INT_ALL_TYPE_PCIE_INT_LTR_MASK                                                          0x20
#define PCIE20_INT_ALL_TYPE_PCIE_INT_LTR_SHFT                                                          0x5
#define PCIE20_INT_ALL_TYPE_PCIE_INT_DEBUG_MASK                                                        0x10
#define PCIE20_INT_ALL_TYPE_PCIE_INT_DEBUG_SHFT                                                        0x4
#define PCIE20_INT_ALL_TYPE_PCIE_RADM_TURNOFF_MASK                                                     0x8
#define PCIE20_INT_ALL_TYPE_PCIE_RADM_TURNOFF_SHFT                                                     0x3
#define PCIE20_INT_ALL_TYPE_PCIE_CFG_BUS_MASTER_EN_MASK                                                0x4
#define PCIE20_INT_ALL_TYPE_PCIE_CFG_BUS_MASTER_EN_SHFT                                                0x2
#define PCIE20_INT_ALL_TYPE_PCIE_SMLH_REQ_RST_LINK_DOWN_MASK                                           0x2
#define PCIE20_INT_ALL_TYPE_PCIE_SMLH_REQ_RST_LINK_DOWN_SHFT                                           0x1

/* PCIE20_PARF_INT_ALL_SOURCE */
#define PCIE20_PARF_INT_ALL_SOURCE                                                                     ( 0x3CC )
#define PCIE20_PARF_INT_ALL_SOURCE_INT_SRC_MASK                                                        0xFFFFFFFF
#define PCIE20_PARF_INT_ALL_SOURCE_INT_SRC_SHFT                                                        0x0

/* PCIE20_PARF_DEVICE_TYPE */
#define PCIE20_PARF_DEVICE_TYPE                                                                        ( 0x1000 )
#define PCIE20_PARF_DEVICE_TYPE_DEVICE_TYPE_MASK                                                       0xF
#define PCIE20_PARF_DEVICE_TYPE_DEVICE_TYPE_SHFT                                                       0x0


/*============================================================================
     PCIE_PHY
============================================================================*/
     
#define PCIE_PHY_BASE_PHYS   ( 0x86000 )
#define PCIE_PHY_SIZE        ( 0x1000 )

/*============================================================================
     QSERDES_COM
============================================================================*/

/* ATB_SEL1 */
#define QSERDES_COM_ATB_SEL1                                                                   ( 0x0 )
#define QSERDES_COM_ATB_SEL1_ATB_SEL_7_0_MASK                                                  0xFF
#define QSERDES_COM_ATB_SEL1_ATB_SEL_7_0_SHFT                                                  0x0

/* ATB_SEL2 */
#define QSERDES_COM_ATB_SEL2                                                                   ( 0x4 )
#define QSERDES_COM_ATB_SEL2_ATB_SEL_11_8_MASK                                                 0xF
#define QSERDES_COM_ATB_SEL2_ATB_SEL_11_8_SHFT                                                 0x0

/* FREQ_UPDATE */
#define QSERDES_COM_FREQ_UPDATE                                                                ( 0x8 )
#define QSERDES_COM_FREQ_UPDATE_PLL_FREQ_UPDATE_MASK                                           0x1
#define QSERDES_COM_FREQ_UPDATE_PLL_FREQ_UPDATE_SHFT                                           0x0

/* BG_TIMER */
#define QSERDES_COM_BG_TIMER                                                                   ( 0xC )
#define QSERDES_COM_BG_TIMER_SYSCLK_WAKE_BYPASS_MASK                                           0x80
#define QSERDES_COM_BG_TIMER_SYSCLK_WAKE_BYPASS_SHFT                                           0x7
#define QSERDES_COM_BG_TIMER_SYSCLK_WAKE_TIME_MASK                                             0x70
#define QSERDES_COM_BG_TIMER_SYSCLK_WAKE_TIME_SHFT                                             0x4
#define QSERDES_COM_BG_TIMER_REFCLK_FREQ_SEL_MASK                                              0x8
#define QSERDES_COM_BG_TIMER_REFCLK_FREQ_SEL_SHFT                                              0x3
#define QSERDES_COM_BG_TIMER_BG_WAIT_TIME_MASK                                                 0x7
#define QSERDES_COM_BG_TIMER_BG_WAIT_TIME_SHFT                                                 0x0

/* SSC_EN_CENTER */
#define QSERDES_COM_SSC_EN_CENTER                                                              ( 0x10 )
#define QSERDES_COM_SSC_EN_CENTER_SSC_CENTER_MASK                                              0x2
#define QSERDES_COM_SSC_EN_CENTER_SSC_CENTER_SHFT                                              0x1
#define QSERDES_COM_SSC_EN_CENTER_SSC_EN_MASK                                                  0x1
#define QSERDES_COM_SSC_EN_CENTER_SSC_EN_SHFT                                                  0x0

/* SSC_ADJ_PER1 */
#define QSERDES_COM_SSC_ADJ_PER1                                                               ( 0x14 )
#define QSERDES_COM_SSC_ADJ_PER1_SSC_ADJPER_7_0_MASK                                           0xFF
#define QSERDES_COM_SSC_ADJ_PER1_SSC_ADJPER_7_0_SHFT                                           0x0

/* SSC_ADJ_PER2 */
#define QSERDES_COM_SSC_ADJ_PER2                                                               ( 0x18 )
#define QSERDES_COM_SSC_ADJ_PER2_SSC_ADJPER_9_8_MASK                                           0x3
#define QSERDES_COM_SSC_ADJ_PER2_SSC_ADJPER_9_8_SHFT                                           0x0

/* SSC_PER1 */
#define QSERDES_COM_SSC_PER1                                                                   ( 0x1C )
#define QSERDES_COM_SSC_PER1_SSC_PER_7_0_MASK                                                  0xFF
#define QSERDES_COM_SSC_PER1_SSC_PER_7_0_SHFT                                                  0x0

/* SSC_PER2 */
#define QSERDES_COM_SSC_PER2                                                                   ( 0x20 )
#define QSERDES_COM_SSC_PER2_SSC_PER_15_8_MASK                                                 0xFF
#define QSERDES_COM_SSC_PER2_SSC_PER_15_8_SHFT                                                 0x0

/* SSC_STEP_SIZE1 */
#define QSERDES_COM_SSC_STEP_SIZE1                                                             ( 0x24 )
#define QSERDES_COM_SSC_STEP_SIZE1_SSC_STEPSIZE_7_0_MASK                                       0xFF
#define QSERDES_COM_SSC_STEP_SIZE1_SSC_STEPSIZE_7_0_SHFT                                       0x0

/* SSC_STEP_SIZE2 */
#define QSERDES_COM_SSC_STEP_SIZE2                                                             ( 0x28 )
#define QSERDES_COM_SSC_STEP_SIZE2_SSC_STEPSIZE_15_8_MASK                                      0xFF
#define QSERDES_COM_SSC_STEP_SIZE2_SSC_STEPSIZE_15_8_SHFT                                      0x0

/* POST_DIV */
#define QSERDES_COM_POST_DIV                                                                   ( 0x2C )
#define QSERDES_COM_POST_DIV_PLL_POSTDIV_MASK                                                  0xFF
#define QSERDES_COM_POST_DIV_PLL_POSTDIV_SHFT                                                  0x0

/* POST_DIV_MUX */
#define QSERDES_COM_POST_DIV_MUX                                                               ( 0x30 )
#define QSERDES_COM_POST_DIV_MUX_PLL_POSTDIV_MUX_MASK                                          0x1
#define QSERDES_COM_POST_DIV_MUX_PLL_POSTDIV_MUX_SHFT                                          0x0

/* BIAS_EN_CLKBUFLR_EN */
#define QSERDES_COM_BIAS_EN_CLKBUFLR_EN                                                        ( 0x34 )
#define QSERDES_COM_BIAS_EN_CLKBUFLR_EN_CLKBUF_RX_DRIVE_R_MASK                                 0x40
#define QSERDES_COM_BIAS_EN_CLKBUFLR_EN_CLKBUF_RX_DRIVE_R_SHFT                                 0x6
#define QSERDES_COM_BIAS_EN_CLKBUFLR_EN_CLKBUF_RX_DRIVE_L_MASK                                 0x20
#define QSERDES_COM_BIAS_EN_CLKBUFLR_EN_CLKBUF_RX_DRIVE_L_SHFT                                 0x5
#define QSERDES_COM_BIAS_EN_CLKBUFLR_EN_EN_SYSCLK_TX_SEL_MASK                                  0x10
#define QSERDES_COM_BIAS_EN_CLKBUFLR_EN_EN_SYSCLK_TX_SEL_SHFT                                  0x4
#define QSERDES_COM_BIAS_EN_CLKBUFLR_EN_CLKBUF_L_EN_MASK                                       0x8
#define QSERDES_COM_BIAS_EN_CLKBUFLR_EN_CLKBUF_L_EN_SHFT                                       0x3
#define QSERDES_COM_BIAS_EN_CLKBUFLR_EN_CLKBUF_R_EN_MASK                                       0x4
#define QSERDES_COM_BIAS_EN_CLKBUFLR_EN_CLKBUF_R_EN_SHFT                                       0x2
#define QSERDES_COM_BIAS_EN_CLKBUFLR_EN_BIAS_EN_MUX_MASK                                       0x2
#define QSERDES_COM_BIAS_EN_CLKBUFLR_EN_BIAS_EN_MUX_SHFT                                       0x1
#define QSERDES_COM_BIAS_EN_CLKBUFLR_EN_BIAS_EN_MASK                                           0x1
#define QSERDES_COM_BIAS_EN_CLKBUFLR_EN_BIAS_EN_SHFT                                           0x0

/* CLK_ENABLE1 */
#define QSERDES_COM_CLK_ENABLE1                                                                ( 0x38 )
#define QSERDES_COM_CLK_ENABLE1_CLKDRVIDLE_EN_MASK                                             0x80
#define QSERDES_COM_CLK_ENABLE1_CLKDRVIDLE_EN_SHFT                                             0x7
#define QSERDES_COM_CLK_ENABLE1_EN_SYSCLK_TX_PS3PS4_MASK                                       0x40
#define QSERDES_COM_CLK_ENABLE1_EN_SYSCLK_TX_PS3PS4_SHFT                                       0x6
#define QSERDES_COM_CLK_ENABLE1_EN_SYSCLK_TX_PS0PS1PS2_MASK                                    0x20
#define QSERDES_COM_CLK_ENABLE1_EN_SYSCLK_TX_PS0PS1PS2_SHFT                                    0x5
#define QSERDES_COM_CLK_ENABLE1_CLK_EP_EN_MASK                                                 0x10
#define QSERDES_COM_CLK_ENABLE1_CLK_EP_EN_SHFT                                                 0x4
#define QSERDES_COM_CLK_ENABLE1_PLL_TXCLK_EN_MUX_MASK                                          0x8
#define QSERDES_COM_CLK_ENABLE1_PLL_TXCLK_EN_MUX_SHFT                                          0x3
#define QSERDES_COM_CLK_ENABLE1_PLL_TXCLK_EN_MASK                                              0x4
#define QSERDES_COM_CLK_ENABLE1_PLL_TXCLK_EN_SHFT                                              0x2
#define QSERDES_COM_CLK_ENABLE1_PLL_RXCLK_EN_MUX_MASK                                          0x2
#define QSERDES_COM_CLK_ENABLE1_PLL_RXCLK_EN_MUX_SHFT                                          0x1
#define QSERDES_COM_CLK_ENABLE1_PLL_RXCLK_EN_MASK                                              0x1
#define QSERDES_COM_CLK_ENABLE1_PLL_RXCLK_EN_SHFT                                              0x0

/* SYS_CLK_CTRL */
#define QSERDES_COM_SYS_CLK_CTRL                                                               ( 0x3C )
#define QSERDES_COM_SYS_CLK_CTRL_SYSCLK_DIV2_SEL_MODE1_MASK                                    0x10
#define QSERDES_COM_SYS_CLK_CTRL_SYSCLK_DIV2_SEL_MODE1_SHFT                                    0x4
#define QSERDES_COM_SYS_CLK_CTRL_SYSCLK_DIV2_SEL_MODE0_MASK                                    0x8
#define QSERDES_COM_SYS_CLK_CTRL_SYSCLK_DIV2_SEL_MODE0_SHFT                                    0x3
#define QSERDES_COM_SYS_CLK_CTRL_SYSCLK_CM_MASK                                                0x4
#define QSERDES_COM_SYS_CLK_CTRL_SYSCLK_CM_SHFT                                                0x2
#define QSERDES_COM_SYS_CLK_CTRL_SYSCLK_AC_COUPLE_MASK                                         0x2
#define QSERDES_COM_SYS_CLK_CTRL_SYSCLK_AC_COUPLE_SHFT                                         0x1
#define QSERDES_COM_SYS_CLK_CTRL_SYSCLK_GNDTERM_MASK                                           0x1
#define QSERDES_COM_SYS_CLK_CTRL_SYSCLK_GNDTERM_SHFT                                           0x0

/* SYSCLK_BUF_ENABLE */
#define QSERDES_COM_SYSCLK_BUF_ENABLE                                                          ( 0x40 )
#define QSERDES_COM_SYSCLK_BUF_ENABLE_SYSCLK_EN_PS5_MASK                                       0x10
#define QSERDES_COM_SYSCLK_BUF_ENABLE_SYSCLK_EN_PS5_SHFT                                       0x4
#define QSERDES_COM_SYSCLK_BUF_ENABLE_SYSCLK_EN_PS3PS4_MASK                                    0x8
#define QSERDES_COM_SYSCLK_BUF_ENABLE_SYSCLK_EN_PS3PS4_SHFT                                    0x3
#define QSERDES_COM_SYSCLK_BUF_ENABLE_SYSCLK_EN_PS0PS1PS2_MASK                                 0x4
#define QSERDES_COM_SYSCLK_BUF_ENABLE_SYSCLK_EN_PS0PS1PS2_SHFT                                 0x2
#define QSERDES_COM_SYSCLK_BUF_ENABLE_SYSCLK_BUF_EN_MUX_MASK                                   0x2
#define QSERDES_COM_SYSCLK_BUF_ENABLE_SYSCLK_BUF_EN_MUX_SHFT                                   0x1
#define QSERDES_COM_SYSCLK_BUF_ENABLE_SW_SYSCLK_BUF_EN_MASK                                    0x1
#define QSERDES_COM_SYSCLK_BUF_ENABLE_SW_SYSCLK_BUF_EN_SHFT                                    0x0

/* PLL_EN */
#define QSERDES_COM_PLL_EN                                                                     ( 0x44 )
#define QSERDES_COM_PLL_EN_PLL_EN_MUX_MASK                                                     0x2
#define QSERDES_COM_PLL_EN_PLL_EN_MUX_SHFT                                                     0x1
#define QSERDES_COM_PLL_EN_PLL_EN_MASK                                                         0x1
#define QSERDES_COM_PLL_EN_PLL_EN_SHFT                                                         0x0

/* PLL_IVCO */
#define QSERDES_COM_PLL_IVCO                                                                   ( 0x48 )
#define QSERDES_COM_PLL_IVCO_PLL_IVCO_MASK                                                     0xFF
#define QSERDES_COM_PLL_IVCO_PLL_IVCO_SHFT                                                     0x0

/* LOCK_CMP1_MODE0 */
#define QSERDES_COM_LOCK_CMP1_MODE0                                                            ( 0x4C )
#define QSERDES_COM_LOCK_CMP1_MODE0_PLLLOCK_CMP_MODE0_7_0_MASK                                 0xFF
#define QSERDES_COM_LOCK_CMP1_MODE0_PLLLOCK_CMP_MODE0_7_0_SHFT                                 0x0

/* LOCK_CMP2_MODE0 */
#define QSERDES_COM_LOCK_CMP2_MODE0                                                            ( 0x50 )
#define QSERDES_COM_LOCK_CMP2_MODE0_PLLLOCK_CMP_MODE0_15_8_MASK                                0xFF
#define QSERDES_COM_LOCK_CMP2_MODE0_PLLLOCK_CMP_MODE0_15_8_SHFT                                0x0

/* LOCK_CMP3_MODE0 */
#define QSERDES_COM_LOCK_CMP3_MODE0                                                            ( 0x54 )
#define QSERDES_COM_LOCK_CMP3_MODE0_PLLLOCK_CMP_MODE0_17_16_MASK                               0x3
#define QSERDES_COM_LOCK_CMP3_MODE0_PLLLOCK_CMP_MODE0_17_16_SHFT                               0x0

/* LOCK_CMP1_MODE1 */
#define QSERDES_COM_LOCK_CMP1_MODE1                                                            ( 0x58 )
#define QSERDES_COM_LOCK_CMP1_MODE1_PLLLOCK_CMP_MODE1_7_0_MASK                                 0xFF
#define QSERDES_COM_LOCK_CMP1_MODE1_PLLLOCK_CMP_MODE1_7_0_SHFT                                 0x0

/* LOCK_CMP2_MODE1 */
#define QSERDES_COM_LOCK_CMP2_MODE1                                                            ( 0x5C )
#define QSERDES_COM_LOCK_CMP2_MODE1_PLLLOCK_CMP_MODE1_15_8_MASK                                0xFF
#define QSERDES_COM_LOCK_CMP2_MODE1_PLLLOCK_CMP_MODE1_15_8_SHFT                                0x0

/* LOCK_CMP3_MODE1 */
#define QSERDES_COM_LOCK_CMP3_MODE1                                                            ( 0x60 )
#define QSERDES_COM_LOCK_CMP3_MODE1_PLLLOCK_CMP_MODE1_17_16_MASK                               0x3
#define QSERDES_COM_LOCK_CMP3_MODE1_PLLLOCK_CMP_MODE1_17_16_SHFT                               0x0

/* CMN_RSVD0 */
#define QSERDES_COM_CMN_RSVD0                                                                  ( 0x64 )
#define QSERDES_COM_CMN_RSVD0_RSVD_MASK                                                        0x1
#define QSERDES_COM_CMN_RSVD0_RSVD_SHFT                                                        0x0

/* EP_CLOCK_DETECT_CTRL */
#define QSERDES_COM_EP_CLOCK_DETECT_CTRL                                                       ( 0x68 )
#define QSERDES_COM_EP_CLOCK_DETECT_CTRL_SYSCLK_CLKDET_DIVSEL_MUX_MASK                         0x20
#define QSERDES_COM_EP_CLOCK_DETECT_CTRL_SYSCLK_CLKDET_DIVSEL_MUX_SHFT                         0x5
#define QSERDES_COM_EP_CLOCK_DETECT_CTRL_SYSCLK_CLKDET_DIVSEL_MASK                             0x18
#define QSERDES_COM_EP_CLOCK_DETECT_CTRL_SYSCLK_CLKDET_DIVSEL_SHFT                             0x3
#define QSERDES_COM_EP_CLOCK_DETECT_CTRL_SYSCLK_CLKDET_FORCE_MASK                              0x4
#define QSERDES_COM_EP_CLOCK_DETECT_CTRL_SYSCLK_CLKDET_FORCE_SHFT                              0x2
#define QSERDES_COM_EP_CLOCK_DETECT_CTRL_SYSCLK_CLKDET_EN_MUX_MASK                             0x2
#define QSERDES_COM_EP_CLOCK_DETECT_CTRL_SYSCLK_CLKDET_EN_MUX_SHFT                             0x1
#define QSERDES_COM_EP_CLOCK_DETECT_CTRL_SYSCLK_CLKDET_EN_MASK                                 0x1
#define QSERDES_COM_EP_CLOCK_DETECT_CTRL_SYSCLK_CLKDET_EN_SHFT                                 0x0

/* SYSCLK_DET_COMP_STATUS */
#define QSERDES_COM_SYSCLK_DET_COMP_STATUS                                                     ( 0x6C )
#define QSERDES_COM_SYSCLK_DET_COMP_STATUS_SYSCLK_CLKDET_COMP_MASK                             0x1
#define QSERDES_COM_SYSCLK_DET_COMP_STATUS_SYSCLK_CLKDET_COMP_SHFT                             0x0

/* BG_TRIM */
#define QSERDES_COM_BG_TRIM                                                                    ( 0x70 )
#define QSERDES_COM_BG_TRIM_BG_TRIM_MASK                                                       0xFF
#define QSERDES_COM_BG_TRIM_BG_TRIM_SHFT                                                       0x0

/* CLK_EP_DIV */
#define QSERDES_COM_CLK_EP_DIV                                                                 ( 0x74 )
#define QSERDES_COM_CLK_EP_DIV_CLK_EP_DIV_MASK                                                 0xFF
#define QSERDES_COM_CLK_EP_DIV_CLK_EP_DIV_SHFT                                                 0x0

/* CP_CTRL_MODE0 */
#define QSERDES_COM_CP_CTRL_MODE0                                                              ( 0x78 )
#define QSERDES_COM_CP_CTRL_MODE0_PLL_CPCTRL_MODE0_MASK                                        0x3F
#define QSERDES_COM_CP_CTRL_MODE0_PLL_CPCTRL_MODE0_SHFT                                        0x0

/* CP_CTRL_MODE1 */
#define QSERDES_COM_CP_CTRL_MODE1                                                              ( 0x7C )
#define QSERDES_COM_CP_CTRL_MODE1_PLL_CPCTRL_MODE1_MASK                                        0x3F
#define QSERDES_COM_CP_CTRL_MODE1_PLL_CPCTRL_MODE1_SHFT                                        0x0

/* CMN_RSVD1 */
#define QSERDES_COM_CMN_RSVD1                                                                  ( 0x80 )
#define QSERDES_COM_CMN_RSVD1_RSVD_MASK                                                        0x1
#define QSERDES_COM_CMN_RSVD1_RSVD_SHFT                                                        0x0

/* PLL_RCTRL_MODE0 */
#define QSERDES_COM_PLL_RCTRL_MODE0                                                            ( 0x84 )
#define QSERDES_COM_PLL_RCTRL_MODE0_PLL_RCTRL_MODE0_MASK                                       0x1F
#define QSERDES_COM_PLL_RCTRL_MODE0_PLL_RCTRL_MODE0_SHFT                                       0x0

/* PLL_RCTRL_MODE1 */
#define QSERDES_COM_PLL_RCTRL_MODE1                                                            ( 0x88 )
#define QSERDES_COM_PLL_RCTRL_MODE1_PLL_RCTRL_MODE1_MASK                                       0x1F
#define QSERDES_COM_PLL_RCTRL_MODE1_PLL_RCTRL_MODE1_SHFT                                       0x0

/* CMN_RSVD2 */
#define QSERDES_COM_CMN_RSVD2                                                                  ( 0x8C )
#define QSERDES_COM_CMN_RSVD2_RSVD_MASK                                                        0x1
#define QSERDES_COM_CMN_RSVD2_RSVD_SHFT                                                        0x0

/* PLL_CCTRL_MODE0 */
#define QSERDES_COM_PLL_CCTRL_MODE0                                                            ( 0x90 )
#define QSERDES_COM_PLL_CCTRL_MODE0_PLL_CCTRL_MODE0_MASK                                       0x3F
#define QSERDES_COM_PLL_CCTRL_MODE0_PLL_CCTRL_MODE0_SHFT                                       0x0

/* PLL_CCTRL_MODE1 */
#define QSERDES_COM_PLL_CCTRL_MODE1                                                            ( 0x94 )
#define QSERDES_COM_PLL_CCTRL_MODE1_PLL_CCTRL_MODE1_MASK                                       0x3F
#define QSERDES_COM_PLL_CCTRL_MODE1_PLL_CCTRL_MODE1_SHFT                                       0x0

/* CMN_RSVD3 */
#define QSERDES_COM_CMN_RSVD3                                                                  ( 0x98 )
#define QSERDES_COM_CMN_RSVD3_RSVD_MASK                                                        0x1
#define QSERDES_COM_CMN_RSVD3_RSVD_SHFT                                                        0x0

/* PLL_CNTRL */
#define QSERDES_COM_PLL_CNTRL                                                                  ( 0x9C )
#define QSERDES_COM_PLL_CNTRL_PLL_CPLIN_MASK                                                   0x4
#define QSERDES_COM_PLL_CNTRL_PLL_CPLIN_SHFT                                                   0x2
#define QSERDES_COM_PLL_CNTRL_PLL_DIV_FFEN_MASK                                                0x2
#define QSERDES_COM_PLL_CNTRL_PLL_DIV_FFEN_SHFT                                                0x1
#define QSERDES_COM_PLL_CNTRL_PLL_DIV_ORD_MASK                                                 0x1
#define QSERDES_COM_PLL_CNTRL_PLL_DIV_ORD_SHFT                                                 0x0

/* PHASE_SEL_CTRL */
#define QSERDES_COM_PHASE_SEL_CTRL                                                             ( 0xA0 )
#define QSERDES_COM_PHASE_SEL_CTRL_PLL_PHSEL_FLIPDIR_MASK                                      0x2
#define QSERDES_COM_PHASE_SEL_CTRL_PLL_PHSEL_FLIPDIR_SHFT                                      0x1
#define QSERDES_COM_PHASE_SEL_CTRL_PLL_PHSEL_EN_MASK                                           0x1
#define QSERDES_COM_PHASE_SEL_CTRL_PLL_PHSEL_EN_SHFT                                           0x0

/* PHASE_SEL_DC */
#define QSERDES_COM_PHASE_SEL_DC                                                               ( 0xA4 )
#define QSERDES_COM_PHASE_SEL_DC_PLL_PHSEL_DC_MASK                                             0xFF
#define QSERDES_COM_PHASE_SEL_DC_PLL_PHSEL_DC_SHFT                                             0x0

/* BIAS_EN_CTRL_BY_PSM */
#define QSERDES_COM_BIAS_EN_CTRL_BY_PSM                                                        ( 0xA8 )
#define QSERDES_COM_BIAS_EN_CTRL_BY_PSM_BIAS_EN_CTRL_BY_PSM_MASK                               0x1
#define QSERDES_COM_BIAS_EN_CTRL_BY_PSM_BIAS_EN_CTRL_BY_PSM_SHFT                               0x0

/* SYSCLK_EN_SEL */
#define QSERDES_COM_SYSCLK_EN_SEL                                                              ( 0xAC )
#define QSERDES_COM_SYSCLK_EN_SEL_CML_SYSCLK_SEL_MASK                                          0xE0
#define QSERDES_COM_SYSCLK_EN_SEL_CML_SYSCLK_SEL_SHFT                                          0x5
#define QSERDES_COM_SYSCLK_EN_SEL_SYSCLK_TERM_SEL_MASK                                         0x10
#define QSERDES_COM_SYSCLK_EN_SEL_SYSCLK_TERM_SEL_SHFT                                         0x4
#define QSERDES_COM_SYSCLK_EN_SEL_SYSCLK_SEL_MASK                                              0xC
#define QSERDES_COM_SYSCLK_EN_SEL_SYSCLK_SEL_SHFT                                              0x2
#define QSERDES_COM_SYSCLK_EN_SEL_SYSCLK_EN_MUX_MASK                                           0x2
#define QSERDES_COM_SYSCLK_EN_SEL_SYSCLK_EN_MUX_SHFT                                           0x1
#define QSERDES_COM_SYSCLK_EN_SEL_SYSCLK_EN_MASK                                               0x1
#define QSERDES_COM_SYSCLK_EN_SEL_SYSCLK_EN_SHFT                                               0x0

/* CML_SYSCLK_SEL */
#define QSERDES_COM_CML_SYSCLK_SEL                                                             ( 0xB0 )
#define QSERDES_COM_CML_SYSCLK_SEL_CML_SYSCLK_SEL_MUX_MASK                                     0x1
#define QSERDES_COM_CML_SYSCLK_SEL_CML_SYSCLK_SEL_MUX_SHFT                                     0x0

/* RESETSM_CNTRL */
#define QSERDES_COM_RESETSM_CNTRL                                                              ( 0xB4 )
#define QSERDES_COM_RESETSM_CNTRL_FORCE_C_READY_MASK                                           0x40
#define QSERDES_COM_RESETSM_CNTRL_FORCE_C_READY_SHFT                                           0x6
#define QSERDES_COM_RESETSM_CNTRL_TSYNC_SEL_MASK                                               0x20
#define QSERDES_COM_RESETSM_CNTRL_TSYNC_SEL_SHFT                                               0x5
#define QSERDES_COM_RESETSM_CNTRL_DISABLE_CLKGATE_MASK                                         0x10
#define QSERDES_COM_RESETSM_CNTRL_DISABLE_CLKGATE_SHFT                                         0x4
#define QSERDES_COM_RESETSM_CNTRL_PLL_UNLOCK_DISABLE_MASK                                      0x8
#define QSERDES_COM_RESETSM_CNTRL_PLL_UNLOCK_DISABLE_SHFT                                      0x3
#define QSERDES_COM_RESETSM_CNTRL_RSM_START_MUX_MASK                                           0x4
#define QSERDES_COM_RESETSM_CNTRL_RSM_START_MUX_SHFT                                           0x2
#define QSERDES_COM_RESETSM_CNTRL_RSM_START_MASK                                               0x2
#define QSERDES_COM_RESETSM_CNTRL_RSM_START_SHFT                                               0x1
#define QSERDES_COM_RESETSM_CNTRL_FORCE_PLLLOCK_MASK                                           0x1
#define QSERDES_COM_RESETSM_CNTRL_FORCE_PLLLOCK_SHFT                                           0x0

/* RESETSM_CNTRL2 */
#define QSERDES_COM_RESETSM_CNTRL2                                                             ( 0xB8 )
#define QSERDES_COM_RESETSM_CNTRL2_BYPASS_RSM_INTEGLOOP_CAL_MASK                               0x40
#define QSERDES_COM_RESETSM_CNTRL2_BYPASS_RSM_INTEGLOOP_CAL_SHFT                               0x6
#define QSERDES_COM_RESETSM_CNTRL2_BYPASS_RSM_RESTRIM_MASK                                     0x20
#define QSERDES_COM_RESETSM_CNTRL2_BYPASS_RSM_RESTRIM_SHFT                                     0x5
#define QSERDES_COM_RESETSM_CNTRL2_BYPASS_RSM_BANDGAP_MASK                                     0x10
#define QSERDES_COM_RESETSM_CNTRL2_BYPASS_RSM_BANDGAP_SHFT                                     0x4
#define QSERDES_COM_RESETSM_CNTRL2_TSYNC_PULSE_WIDTH_MASK                                      0xC
#define QSERDES_COM_RESETSM_CNTRL2_TSYNC_PULSE_WIDTH_SHFT                                      0x2
#define QSERDES_COM_RESETSM_CNTRL2_TSYNC_MUX_MASK                                              0x2
#define QSERDES_COM_RESETSM_CNTRL2_TSYNC_MUX_SHFT                                              0x1
#define QSERDES_COM_RESETSM_CNTRL2_TSYNC_MASK                                                  0x1
#define QSERDES_COM_RESETSM_CNTRL2_TSYNC_SHFT                                                  0x0

/* RESTRIM_CTRL */
#define QSERDES_COM_RESTRIM_CTRL                                                               ( 0xBC )
#define QSERDES_COM_RESTRIM_CTRL_RESTRIM_CMN_BYPASS_CODE_MASK                                  0xFE
#define QSERDES_COM_RESTRIM_CTRL_RESTRIM_CMN_BYPASS_CODE_SHFT                                  0x1
#define QSERDES_COM_RESTRIM_CTRL_BYPASS_CMN_RESTRIM_MASK                                       0x1
#define QSERDES_COM_RESTRIM_CTRL_BYPASS_CMN_RESTRIM_SHFT                                       0x0

/* RESTRIM_CTRL2 */
#define QSERDES_COM_RESTRIM_CTRL2                                                              ( 0xC0 )
#define QSERDES_COM_RESTRIM_CTRL2_RESTRIM_OFFSET_CODE_MASK                                     0x7F
#define QSERDES_COM_RESTRIM_CTRL2_RESTRIM_OFFSET_CODE_SHFT                                     0x0

/* RESCODE_DIV_NUM */
#define QSERDES_COM_RESCODE_DIV_NUM                                                            ( 0xC4 )
#define QSERDES_COM_RESCODE_DIV_NUM_RESCODE_DIV_NUMERATOR_MASK                                 0x7F
#define QSERDES_COM_RESCODE_DIV_NUM_RESCODE_DIV_NUMERATOR_SHFT                                 0x0

/* LOCK_CMP_EN */
#define QSERDES_COM_LOCK_CMP_EN                                                                ( 0xC8 )
#define QSERDES_COM_LOCK_CMP_EN_PLLLOCK_RNG_2_MASK                                             0x80
#define QSERDES_COM_LOCK_CMP_EN_PLLLOCK_RNG_2_SHFT                                             0x7
#define QSERDES_COM_LOCK_CMP_EN_PLLLOCK_CNT_2_MASK                                             0x40
#define QSERDES_COM_LOCK_CMP_EN_PLLLOCK_CNT_2_SHFT                                             0x6
#define QSERDES_COM_LOCK_CMP_EN_PLLLOCK_EN_MUX_MASK                                            0x20
#define QSERDES_COM_LOCK_CMP_EN_PLLLOCK_EN_MUX_SHFT                                            0x5
#define QSERDES_COM_LOCK_CMP_EN_PLLLOCK_EN_MASK                                                0x10
#define QSERDES_COM_LOCK_CMP_EN_PLLLOCK_EN_SHFT                                                0x4
#define QSERDES_COM_LOCK_CMP_EN_PLLLOCK_RNG_MASK                                               0xC
#define QSERDES_COM_LOCK_CMP_EN_PLLLOCK_RNG_SHFT                                               0x2
#define QSERDES_COM_LOCK_CMP_EN_PLLLOCK_CNT_MASK                                               0x3
#define QSERDES_COM_LOCK_CMP_EN_PLLLOCK_CNT_SHFT                                               0x0

/* LOCK_CMP_CFG */
#define QSERDES_COM_LOCK_CMP_CFG                                                               ( 0xCC )
#define QSERDES_COM_LOCK_CMP_CFG_PLLLOCK_STOP_COUNT_MASK                                       0x2
#define QSERDES_COM_LOCK_CMP_CFG_PLLLOCK_STOP_COUNT_SHFT                                       0x1
#define QSERDES_COM_LOCK_CMP_CFG_BYP_PLLLOCK_MASK                                              0x1
#define QSERDES_COM_LOCK_CMP_CFG_BYP_PLLLOCK_SHFT                                              0x0

/* DEC_START_MODE0 */
#define QSERDES_COM_DEC_START_MODE0                                                            ( 0xD0 )
#define QSERDES_COM_DEC_START_MODE0_DEC_START_MODE0_MASK                                       0xFF
#define QSERDES_COM_DEC_START_MODE0_DEC_START_MODE0_SHFT                                       0x0

/* DEC_START_MODE1 */
#define QSERDES_COM_DEC_START_MODE1                                                            ( 0xD4 )
#define QSERDES_COM_DEC_START_MODE1_DEC_START_MODE1_MASK                                       0xFF
#define QSERDES_COM_DEC_START_MODE1_DEC_START_MODE1_SHFT                                       0x0

/* VCOCAL_DEADMAN_CTRL */
#define QSERDES_COM_VCOCAL_DEADMAN_CTRL                                                        ( 0xD8 )
#define QSERDES_COM_VCOCAL_DEADMAN_CTRL_FBCLK_TOO_SLOW_CNTSEL_MASK                             0x18
#define QSERDES_COM_VCOCAL_DEADMAN_CTRL_FBCLK_TOO_SLOW_CNTSEL_SHFT                             0x3
#define QSERDES_COM_VCOCAL_DEADMAN_CTRL_FORCE_CORRECT_POL_MASK                                 0x4
#define QSERDES_COM_VCOCAL_DEADMAN_CTRL_FORCE_CORRECT_POL_SHFT                                 0x2
#define QSERDES_COM_VCOCAL_DEADMAN_CTRL_FBCLK_DEAD_CORRECT_EN_MASK                             0x2
#define QSERDES_COM_VCOCAL_DEADMAN_CTRL_FBCLK_DEAD_CORRECT_EN_SHFT                             0x1
#define QSERDES_COM_VCOCAL_DEADMAN_CTRL_FBCLK_DEAD_TIME_MASK                                   0x1
#define QSERDES_COM_VCOCAL_DEADMAN_CTRL_FBCLK_DEAD_TIME_SHFT                                   0x0

/* DIV_FRAC_START1_MODE0 */
#define QSERDES_COM_DIV_FRAC_START1_MODE0                                                      ( 0xDC )
#define QSERDES_COM_DIV_FRAC_START1_MODE0_DIV_FRAC_START_MODE0_7_0_MASK                        0xFF
#define QSERDES_COM_DIV_FRAC_START1_MODE0_DIV_FRAC_START_MODE0_7_0_SHFT                        0x0

/* DIV_FRAC_START2_MODE0 */
#define QSERDES_COM_DIV_FRAC_START2_MODE0                                                      ( 0xE0 )
#define QSERDES_COM_DIV_FRAC_START2_MODE0_DIV_FRAC_START_MODE0_15_8_MASK                       0xFF
#define QSERDES_COM_DIV_FRAC_START2_MODE0_DIV_FRAC_START_MODE0_15_8_SHFT                       0x0

/* DIV_FRAC_START3_MODE0 */
#define QSERDES_COM_DIV_FRAC_START3_MODE0                                                      ( 0xE4 )
#define QSERDES_COM_DIV_FRAC_START3_MODE0_DIV_FRAC_START_MODE0_19_16_MASK                      0xF
#define QSERDES_COM_DIV_FRAC_START3_MODE0_DIV_FRAC_START_MODE0_19_16_SHFT                      0x0

/* DIV_FRAC_START1_MODE1 */
#define QSERDES_COM_DIV_FRAC_START1_MODE1                                                      ( 0xE8 )
#define QSERDES_COM_DIV_FRAC_START1_MODE1_DIV_FRAC_START_MODE1_7_0_MASK                        0xFF
#define QSERDES_COM_DIV_FRAC_START1_MODE1_DIV_FRAC_START_MODE1_7_0_SHFT                        0x0

/* DIV_FRAC_START2_MODE1 */
#define QSERDES_COM_DIV_FRAC_START2_MODE1                                                      ( 0xEC )
#define QSERDES_COM_DIV_FRAC_START2_MODE1_DIV_FRAC_START_MODE1_15_8_MASK                       0xFF
#define QSERDES_COM_DIV_FRAC_START2_MODE1_DIV_FRAC_START_MODE1_15_8_SHFT                       0x0

/* DIV_FRAC_START3_MODE1 */
#define QSERDES_COM_DIV_FRAC_START3_MODE1                                                      ( 0xF0 )
#define QSERDES_COM_DIV_FRAC_START3_MODE1_DIV_FRAC_START_MODE1_19_16_MASK                      0xF
#define QSERDES_COM_DIV_FRAC_START3_MODE1_DIV_FRAC_START_MODE1_19_16_SHFT                      0x0

/* VCO_TUNE_MINVAL1 */
#define QSERDES_COM_VCO_TUNE_MINVAL1                                                           ( 0xF4 )
#define QSERDES_COM_VCO_TUNE_MINVAL1_VCO_TUNE_MINVAL_7_0_MASK                                  0xFF
#define QSERDES_COM_VCO_TUNE_MINVAL1_VCO_TUNE_MINVAL_7_0_SHFT                                  0x0

/* VCO_TUNE_MINVAL2 */
#define QSERDES_COM_VCO_TUNE_MINVAL2                                                           ( 0xF8 )
#define QSERDES_COM_VCO_TUNE_MINVAL2_VCO_TUNE_MINVAL_9_8_MASK                                  0x3
#define QSERDES_COM_VCO_TUNE_MINVAL2_VCO_TUNE_MINVAL_9_8_SHFT                                  0x0

/* CMN_RSVD4 */
#define QSERDES_COM_CMN_RSVD4                                                                  ( 0xFC )
#define QSERDES_COM_CMN_RSVD4_RSVD_MASK                                                        0x1
#define QSERDES_COM_CMN_RSVD4_RSVD_SHFT                                                        0x0

/* INTEGLOOP_INITVAL */
#define QSERDES_COM_INTEGLOOP_INITVAL                                                          ( 0x100 )
#define QSERDES_COM_INTEGLOOP_INITVAL_INTEGLOOP_INITVAL_MASK                                   0xFF
#define QSERDES_COM_INTEGLOOP_INITVAL_INTEGLOOP_INITVAL_SHFT                                   0x0

/* INTEGLOOP_EN */
#define QSERDES_COM_INTEGLOOP_EN                                                               ( 0x104 )
#define QSERDES_COM_INTEGLOOP_EN_INTEGLOOP_CODE_BYPASS_MASK                                    0x4
#define QSERDES_COM_INTEGLOOP_EN_INTEGLOOP_CODE_BYPASS_SHFT                                    0x2
#define QSERDES_COM_INTEGLOOP_EN_INTEGLOOP_EN_MUX_MASK                                         0x2
#define QSERDES_COM_INTEGLOOP_EN_INTEGLOOP_EN_MUX_SHFT                                         0x1
#define QSERDES_COM_INTEGLOOP_EN_INTEGLOOP_EN_MASK                                             0x1
#define QSERDES_COM_INTEGLOOP_EN_INTEGLOOP_EN_SHFT                                             0x0

/* INTEGLOOP_GAIN0_MODE0 */
#define QSERDES_COM_INTEGLOOP_GAIN0_MODE0                                                      ( 0x108 )
#define QSERDES_COM_INTEGLOOP_GAIN0_MODE0_INTEGLOOP_GAIN_MODE0_7_0_MASK                        0xFF
#define QSERDES_COM_INTEGLOOP_GAIN0_MODE0_INTEGLOOP_GAIN_MODE0_7_0_SHFT                        0x0

/* INTEGLOOP_GAIN1_MODE0 */
#define QSERDES_COM_INTEGLOOP_GAIN1_MODE0                                                      ( 0x10C )
#define QSERDES_COM_INTEGLOOP_GAIN1_MODE0_INTEGLOOP_GAIN_MODE0_11_8_MASK                       0xF
#define QSERDES_COM_INTEGLOOP_GAIN1_MODE0_INTEGLOOP_GAIN_MODE0_11_8_SHFT                       0x0

/* INTEGLOOP_GAIN0_MODE1 */
#define QSERDES_COM_INTEGLOOP_GAIN0_MODE1                                                      ( 0x110 )
#define QSERDES_COM_INTEGLOOP_GAIN0_MODE1_INTEGLOOP_GAIN_MODE1_7_0_MASK                        0xFF
#define QSERDES_COM_INTEGLOOP_GAIN0_MODE1_INTEGLOOP_GAIN_MODE1_7_0_SHFT                        0x0

/* INTEGLOOP_GAIN1_MODE1 */
#define QSERDES_COM_INTEGLOOP_GAIN1_MODE1                                                      ( 0x114 )
#define QSERDES_COM_INTEGLOOP_GAIN1_MODE1_INTEGLOOP_GAIN_MODE1_11_8_MASK                       0xF
#define QSERDES_COM_INTEGLOOP_GAIN1_MODE1_INTEGLOOP_GAIN_MODE1_11_8_SHFT                       0x0

/* VCO_TUNE_MAXVAL1 */
#define QSERDES_COM_VCO_TUNE_MAXVAL1                                                           ( 0x118 )
#define QSERDES_COM_VCO_TUNE_MAXVAL1_VCO_TUNE_MAXVAL_7_0_MASK                                  0xFF
#define QSERDES_COM_VCO_TUNE_MAXVAL1_VCO_TUNE_MAXVAL_7_0_SHFT                                  0x0

/* VCO_TUNE_MAXVAL2 */
#define QSERDES_COM_VCO_TUNE_MAXVAL2                                                           ( 0x11C )
#define QSERDES_COM_VCO_TUNE_MAXVAL2_VCO_TUNE_MAXVAL_9_8_MASK                                  0x3
#define QSERDES_COM_VCO_TUNE_MAXVAL2_VCO_TUNE_MAXVAL_9_8_SHFT                                  0x0

/* RES_TRIM_CONTROL2 */
#define QSERDES_COM_RES_TRIM_CONTROL2                                                          ( 0x120 )
#define QSERDES_COM_RES_TRIM_CONTROL2_RESTRIM_EN_MUX_MASK                                      0x2
#define QSERDES_COM_RES_TRIM_CONTROL2_RESTRIM_EN_MUX_SHFT                                      0x1
#define QSERDES_COM_RES_TRIM_CONTROL2_RESTRIM_EN_MASK                                          0x1
#define QSERDES_COM_RES_TRIM_CONTROL2_RESTRIM_EN_SHFT                                          0x0

/* VCO_TUNE_CTRL */
#define QSERDES_COM_VCO_TUNE_CTRL                                                              ( 0x124 )
#define QSERDES_COM_VCO_TUNE_CTRL_VCOCAL_POS_SLOPE_MASK                                        0x20
#define QSERDES_COM_VCO_TUNE_CTRL_VCOCAL_POS_SLOPE_SHFT                                        0x5
#define QSERDES_COM_VCO_TUNE_CTRL_BYP_VCOCAL_MODE1_MASK                                        0x8
#define QSERDES_COM_VCO_TUNE_CTRL_BYP_VCOCAL_MODE1_SHFT                                        0x3
#define QSERDES_COM_VCO_TUNE_CTRL_BYP_VCOCAL_MODE0_MASK                                        0x4
#define QSERDES_COM_VCO_TUNE_CTRL_BYP_VCOCAL_MODE0_SHFT                                        0x2
#define QSERDES_COM_VCO_TUNE_CTRL_VCO_TUNE_EN_MUX_MASK                                         0x2
#define QSERDES_COM_VCO_TUNE_CTRL_VCO_TUNE_EN_MUX_SHFT                                         0x1
#define QSERDES_COM_VCO_TUNE_CTRL_VCO_TUNE_EN_MASK                                             0x1
#define QSERDES_COM_VCO_TUNE_CTRL_VCO_TUNE_EN_SHFT                                             0x0

/* VCO_TUNE_MAP */
#define QSERDES_COM_VCO_TUNE_MAP                                                               ( 0x128 )
#define QSERDES_COM_VCO_TUNE_MAP_VCO_DEFAULT_FREQ_MASK                                         0xC0
#define QSERDES_COM_VCO_TUNE_MAP_VCO_DEFAULT_FREQ_SHFT                                         0x6
#define QSERDES_COM_VCO_TUNE_MAP_MAP_LANE_PS0B_MASK                                            0xC
#define QSERDES_COM_VCO_TUNE_MAP_MAP_LANE_PS0B_SHFT                                            0x2
#define QSERDES_COM_VCO_TUNE_MAP_MAP_LANE_PS0A_MASK                                            0x3
#define QSERDES_COM_VCO_TUNE_MAP_MAP_LANE_PS0A_SHFT                                            0x0

/* VCO_TUNE1_MODE0 */
#define QSERDES_COM_VCO_TUNE1_MODE0                                                            ( 0x12C )
#define QSERDES_COM_VCO_TUNE1_MODE0_PLL_VCOTUNE_MODE0_7_0_MASK                                 0xFF
#define QSERDES_COM_VCO_TUNE1_MODE0_PLL_VCOTUNE_MODE0_7_0_SHFT                                 0x0

/* VCO_TUNE2_MODE0 */
#define QSERDES_COM_VCO_TUNE2_MODE0                                                            ( 0x130 )
#define QSERDES_COM_VCO_TUNE2_MODE0_PLL_VCOTUNE_MODE0_9_8_MASK                                 0x3
#define QSERDES_COM_VCO_TUNE2_MODE0_PLL_VCOTUNE_MODE0_9_8_SHFT                                 0x0

/* VCO_TUNE1_MODE1 */
#define QSERDES_COM_VCO_TUNE1_MODE1                                                            ( 0x134 )
#define QSERDES_COM_VCO_TUNE1_MODE1_PLL_VCOTUNE_MODE1_7_0_MASK                                 0xFF
#define QSERDES_COM_VCO_TUNE1_MODE1_PLL_VCOTUNE_MODE1_7_0_SHFT                                 0x0

/* VCO_TUNE2_MODE1 */
#define QSERDES_COM_VCO_TUNE2_MODE1                                                            ( 0x138 )
#define QSERDES_COM_VCO_TUNE2_MODE1_PLL_VCOTUNE_MODE1_9_8_MASK                                 0x3
#define QSERDES_COM_VCO_TUNE2_MODE1_PLL_VCOTUNE_MODE1_9_8_SHFT                                 0x0

/* VCO_TUNE_INITVAL1 */
#define QSERDES_COM_VCO_TUNE_INITVAL1                                                          ( 0x13C )
#define QSERDES_COM_VCO_TUNE_INITVAL1_VCO_TUNE_INITVAL_7_0_MASK                                0xFF
#define QSERDES_COM_VCO_TUNE_INITVAL1_VCO_TUNE_INITVAL_7_0_SHFT                                0x0

/* VCO_TUNE_INITVAL2 */
#define QSERDES_COM_VCO_TUNE_INITVAL2                                                          ( 0x140 )
#define QSERDES_COM_VCO_TUNE_INITVAL2_VCO_TUNE_INITVAL_9_8_MASK                                0x3
#define QSERDES_COM_VCO_TUNE_INITVAL2_VCO_TUNE_INITVAL_9_8_SHFT                                0x0

/* VCO_TUNE_TIMER1 */
#define QSERDES_COM_VCO_TUNE_TIMER1                                                            ( 0x144 )
#define QSERDES_COM_VCO_TUNE_TIMER1_PLL_VCOTUNE_TIMER_7_0_MASK                                 0xFF
#define QSERDES_COM_VCO_TUNE_TIMER1_PLL_VCOTUNE_TIMER_7_0_SHFT                                 0x0

/* VCO_TUNE_TIMER2 */
#define QSERDES_COM_VCO_TUNE_TIMER2                                                            ( 0x148 )
#define QSERDES_COM_VCO_TUNE_TIMER2_PLL_VCOTUNE_TIMER_15_8_MASK                                0xFF
#define QSERDES_COM_VCO_TUNE_TIMER2_PLL_VCOTUNE_TIMER_15_8_SHFT                                0x0

/* SAR */
#define QSERDES_COM_SAR                                                                        ( 0x14C )
#define QSERDES_COM_SAR_SAR_FROM_ATB_MASK                                                      0x40
#define QSERDES_COM_SAR_SAR_FROM_ATB_SHFT                                                      0x6
#define QSERDES_COM_SAR_SAR_ATB_MUXSEL_MASK                                                    0x20
#define QSERDES_COM_SAR_SAR_ATB_MUXSEL_SHFT                                                    0x5
#define QSERDES_COM_SAR_SAR_CTUNE_MASK                                                         0x1E
#define QSERDES_COM_SAR_SAR_CTUNE_SHFT                                                         0x1
#define QSERDES_COM_SAR_SAR_EN_MASK                                                            0x1
#define QSERDES_COM_SAR_SAR_EN_SHFT                                                            0x0

/* SAR_CLK */
#define QSERDES_COM_SAR_CLK                                                                    ( 0x150 )
#define QSERDES_COM_SAR_CLK_SAR_CLK_DIV_MASK                                                   0x1F
#define QSERDES_COM_SAR_CLK_SAR_CLK_DIV_SHFT                                                   0x0

/* SAR_CODE_OUT_STATUS */
#define QSERDES_COM_SAR_CODE_OUT_STATUS                                                        ( 0x154 )
#define QSERDES_COM_SAR_CODE_OUT_STATUS_SAR_CODE_OUT_MASK                                      0xFF
#define QSERDES_COM_SAR_CODE_OUT_STATUS_SAR_CODE_OUT_SHFT                                      0x0

/* SAR_CODE_READY_STATUS */
#define QSERDES_COM_SAR_CODE_READY_STATUS                                                      ( 0x158 )
#define QSERDES_COM_SAR_CODE_READY_STATUS_SAR_CODEREADY_MASK                                   0x1
#define QSERDES_COM_SAR_CODE_READY_STATUS_SAR_CODEREADY_SHFT                                   0x0

/* CMN_STATUS */
#define QSERDES_COM_CMN_STATUS                                                                 ( 0x15C )
#define QSERDES_COM_CMN_STATUS_SWITCH_TO_REFCLK_DONE_MASK                                      0x20
#define QSERDES_COM_CMN_STATUS_SWITCH_TO_REFCLK_DONE_SHFT                                      0x5
#define QSERDES_COM_CMN_STATUS_SWITCH_TO_CORECLK_DONE_MASK                                     0x10
#define QSERDES_COM_CMN_STATUS_SWITCH_TO_CORECLK_DONE_SHFT                                     0x4
#define QSERDES_COM_CMN_STATUS_SWITCH_TO_AUXCLK_DONE_MASK                                      0x8
#define QSERDES_COM_CMN_STATUS_SWITCH_TO_AUXCLK_DONE_SHFT                                      0x3
#define QSERDES_COM_CMN_STATUS_PLL_UNLOCKED_MASK                                               0x4
#define QSERDES_COM_CMN_STATUS_PLL_UNLOCKED_SHFT                                               0x2
#define QSERDES_COM_CMN_STATUS_PLL_LOCKED_MASK                                                 0x2
#define QSERDES_COM_CMN_STATUS_PLL_LOCKED_SHFT                                                 0x1
#define QSERDES_COM_CMN_STATUS_PLL_FREQ_DONE_MASK                                              0x1
#define QSERDES_COM_CMN_STATUS_PLL_FREQ_DONE_SHFT                                              0x0

/* RESET_SM_STATUS */
#define QSERDES_COM_RESET_SM_STATUS                                                            ( 0x160 )
#define QSERDES_COM_RESET_SM_STATUS_RESETSM_MASK                                               0xFF
#define QSERDES_COM_RESET_SM_STATUS_RESETSM_SHFT                                               0x0

/* RESTRIM_CODE_STATUS */
#define QSERDES_COM_RESTRIM_CODE_STATUS                                                        ( 0x164 )
#define QSERDES_COM_RESTRIM_CODE_STATUS_RESTRIM_CODE_MASK                                      0x7F
#define QSERDES_COM_RESTRIM_CODE_STATUS_RESTRIM_CODE_SHFT                                      0x0

/* PLLCAL_CODE1_STATUS */
#define QSERDES_COM_PLLCAL_CODE1_STATUS                                                        ( 0x168 )
#define QSERDES_COM_PLLCAL_CODE1_STATUS_VCO_CTUNE_BINCODE_7_0_MASK                             0xFF
#define QSERDES_COM_PLLCAL_CODE1_STATUS_VCO_CTUNE_BINCODE_7_0_SHFT                             0x0

/* PLLCAL_CODE2_STATUS */
#define QSERDES_COM_PLLCAL_CODE2_STATUS                                                        ( 0x16C )
#define QSERDES_COM_PLLCAL_CODE2_STATUS_VCO_CTUNE_BINCODE_9_8_MASK                             0x3
#define QSERDES_COM_PLLCAL_CODE2_STATUS_VCO_CTUNE_BINCODE_9_8_SHFT                             0x0

/* BG_CTRL */
#define QSERDES_COM_BG_CTRL                                                                    ( 0x170 )
#define QSERDES_COM_BG_CTRL_BANDGAP_EN_MUX_MASK                                                0x4
#define QSERDES_COM_BG_CTRL_BANDGAP_EN_MUX_SHFT                                                0x2
#define QSERDES_COM_BG_CTRL_BANDGAP_EN_MASK                                                    0x2
#define QSERDES_COM_BG_CTRL_BANDGAP_EN_SHFT                                                    0x1
#define QSERDES_COM_BG_CTRL_BANDGAP_BYPASS_MASK                                                0x1
#define QSERDES_COM_BG_CTRL_BANDGAP_BYPASS_SHFT                                                0x0

/* CLK_SELECT */
#define QSERDES_COM_CLK_SELECT                                                                 ( 0x174 )
#define QSERDES_COM_CLK_SELECT_TSYNC_EN_MUX_MASK                                               0x20
#define QSERDES_COM_CLK_SELECT_TSYNC_EN_MUX_SHFT                                               0x5
#define QSERDES_COM_CLK_SELECT_TSYNC_EN_MASK                                                   0x10
#define QSERDES_COM_CLK_SELECT_TSYNC_EN_SHFT                                                   0x4
#define QSERDES_COM_CLK_SELECT_SYSCLK_CMOS_PAD_SEL_MASK                                        0x8
#define QSERDES_COM_CLK_SELECT_SYSCLK_CMOS_PAD_SEL_SHFT                                        0x3
#define QSERDES_COM_CLK_SELECT_SYSCLK_TX_SWINGSEL_MASK                                         0x6
#define QSERDES_COM_CLK_SELECT_SYSCLK_TX_SWINGSEL_SHFT                                         0x1
#define QSERDES_COM_CLK_SELECT_SYSCLK_TX_SYSCLKSEL_MASK                                        0x1
#define QSERDES_COM_CLK_SELECT_SYSCLK_TX_SYSCLKSEL_SHFT                                        0x0

/* HSCLK_SEL */
#define QSERDES_COM_HSCLK_SEL                                                                  ( 0x178 )
#define QSERDES_COM_HSCLK_SEL_HSCLK_DIVSEL_MODE1_MASK                                          0xF0
#define QSERDES_COM_HSCLK_SEL_HSCLK_DIVSEL_MODE1_SHFT                                          0x4
#define QSERDES_COM_HSCLK_SEL_HSCLK_DIVSEL_MODE0_MASK                                          0xF
#define QSERDES_COM_HSCLK_SEL_HSCLK_DIVSEL_MODE0_SHFT                                          0x0

/* INTEGLOOP_BINCODE_STATUS */
#define QSERDES_COM_INTEGLOOP_BINCODE_STATUS                                                   ( 0x17C )
#define QSERDES_COM_INTEGLOOP_BINCODE_STATUS_INTEGLOOP_BINCODE_MASK                            0xFF
#define QSERDES_COM_INTEGLOOP_BINCODE_STATUS_INTEGLOOP_BINCODE_SHFT                            0x0

/* PLL_ANALOG */
#define QSERDES_COM_PLL_ANALOG                                                                 ( 0x180 )
#define QSERDES_COM_PLL_ANALOG_PLL_ANALOG_MASK                                                 0x1
#define QSERDES_COM_PLL_ANALOG_PLL_ANALOG_SHFT                                                 0x0

/* CORECLK_DIV */
#define QSERDES_COM_CORECLK_DIV                                                                ( 0x184 )
#define QSERDES_COM_CORECLK_DIV_CORE_CLK_DIV_MASK                                              0xFF
#define QSERDES_COM_CORECLK_DIV_CORE_CLK_DIV_SHFT                                              0x0

/* SW_RESET */
#define QSERDES_COM_SW_RESET                                                                   ( 0x188 )
#define QSERDES_COM_SW_RESET_SW_RESET_MASK                                                     0x1
#define QSERDES_COM_SW_RESET_SW_RESET_SHFT                                                     0x0

/* CORE_CLK_EN */
#define QSERDES_COM_CORE_CLK_EN                                                                ( 0x18C )
#define QSERDES_COM_CORE_CLK_EN_CLKS_PLL_DIVSEL_MODE1_MASK                                     0x20
#define QSERDES_COM_CORE_CLK_EN_CLKS_PLL_DIVSEL_MODE1_SHFT                                     0x5
#define QSERDES_COM_CORE_CLK_EN_CLKS_PLL_DIVSEL_MODE0_MASK                                     0x10
#define QSERDES_COM_CORE_CLK_EN_CLKS_PLL_DIVSEL_MODE0_SHFT                                     0x4
#define QSERDES_COM_CORE_CLK_EN_CORECLK_EN_MUX_MASK                                            0x8
#define QSERDES_COM_CORE_CLK_EN_CORECLK_EN_MUX_SHFT                                            0x3
#define QSERDES_COM_CORE_CLK_EN_CORECLK_EN_MASK                                                0x4
#define QSERDES_COM_CORE_CLK_EN_CORECLK_EN_SHFT                                                0x2
#define QSERDES_COM_CORE_CLK_EN_AUXCLK_EN_MUX_MASK                                             0x2
#define QSERDES_COM_CORE_CLK_EN_AUXCLK_EN_MUX_SHFT                                             0x1
#define QSERDES_COM_CORE_CLK_EN_AUXCLK_EN_MASK                                                 0x1
#define QSERDES_COM_CORE_CLK_EN_AUXCLK_EN_SHFT                                                 0x0

/* C_READY_STATUS */
#define QSERDES_COM_C_READY_STATUS                                                             ( 0x190 )
#define QSERDES_COM_C_READY_STATUS_C_READY_MASK                                                0x1
#define QSERDES_COM_C_READY_STATUS_C_READY_SHFT                                                0x0

/* CMN_CONFIG */
#define QSERDES_COM_CMN_CONFIG                                                                 ( 0x194 )
#define QSERDES_COM_CMN_CONFIG_LANE_SYNC_MODE_MASK                                             0x10
#define QSERDES_COM_CMN_CONFIG_LANE_SYNC_MODE_SHFT                                             0x4
#define QSERDES_COM_CMN_CONFIG_PSM_SWITCH_CORECLK_MASK                                         0x8
#define QSERDES_COM_CMN_CONFIG_PSM_SWITCH_CORECLK_SHFT                                         0x3
#define QSERDES_COM_CMN_CONFIG_PSM_SWITCH_AUXCLK_MASK                                          0x4
#define QSERDES_COM_CMN_CONFIG_PSM_SWITCH_AUXCLK_SHFT                                          0x2
#define QSERDES_COM_CMN_CONFIG_CLKGEN_MODE_MASK                                                0x3
#define QSERDES_COM_CMN_CONFIG_CLKGEN_MODE_SHFT                                                0x0

/* CMN_RATE_OVERRIDE */
#define QSERDES_COM_CMN_RATE_OVERRIDE                                                          ( 0x198 )
#define QSERDES_COM_CMN_RATE_OVERRIDE_CMN_RATE_SW_UPDATE_MASK                                  0x8
#define QSERDES_COM_CMN_RATE_OVERRIDE_CMN_RATE_SW_UPDATE_SHFT                                  0x3
#define QSERDES_COM_CMN_RATE_OVERRIDE_CMN_RATE_SW_SEL_MASK                                     0x4
#define QSERDES_COM_CMN_RATE_OVERRIDE_CMN_RATE_SW_SEL_SHFT                                     0x2
#define QSERDES_COM_CMN_RATE_OVERRIDE_CMN_RATE_CODE_MASK                                       0x3
#define QSERDES_COM_CMN_RATE_OVERRIDE_CMN_RATE_CODE_SHFT                                       0x0

/* SVS_MODE_CLK_SEL */
#define QSERDES_COM_SVS_MODE_CLK_SEL                                                           ( 0x19C )
#define QSERDES_COM_SVS_MODE_CLK_SEL_PLL_DIGCLK_DIVSEL_MODE1_MASK                              0xC
#define QSERDES_COM_SVS_MODE_CLK_SEL_PLL_DIGCLK_DIVSEL_MODE1_SHFT                              0x2
#define QSERDES_COM_SVS_MODE_CLK_SEL_PLL_DIGCLK_DIVSEL_MODE0_MASK                              0x3
#define QSERDES_COM_SVS_MODE_CLK_SEL_PLL_DIGCLK_DIVSEL_MODE0_SHFT                              0x0

/* DEBUG_BUS0 */
#define QSERDES_COM_DEBUG_BUS0                                                                 ( 0x1A0 )
#define QSERDES_COM_DEBUG_BUS0_DEBUG_BUS_7_0_MASK                                              0xFF
#define QSERDES_COM_DEBUG_BUS0_DEBUG_BUS_7_0_SHFT                                              0x0

/* DEBUG_BUS1 */
#define QSERDES_COM_DEBUG_BUS1                                                                 ( 0x1A4 )
#define QSERDES_COM_DEBUG_BUS1_DEBUG_BUS_15_8_MASK                                             0xFF
#define QSERDES_COM_DEBUG_BUS1_DEBUG_BUS_15_8_SHFT                                             0x0

/* DEBUG_BUS2 */
#define QSERDES_COM_DEBUG_BUS2                                                                 ( 0x1A8 )
#define QSERDES_COM_DEBUG_BUS2_DEBUG_BUS_23_16_MASK                                            0xFF
#define QSERDES_COM_DEBUG_BUS2_DEBUG_BUS_23_16_SHFT                                            0x0

/* DEBUG_BUS3 */
#define QSERDES_COM_DEBUG_BUS3                                                                 ( 0x1AC )
#define QSERDES_COM_DEBUG_BUS3_DEBUG_BUS_31_24_MASK                                            0xFF
#define QSERDES_COM_DEBUG_BUS3_DEBUG_BUS_31_24_SHFT                                            0x0

/* DEBUG_BUS_SEL */
#define QSERDES_COM_DEBUG_BUS_SEL                                                              ( 0x1B0 )
#define QSERDES_COM_DEBUG_BUS_SEL_DEBUG_BUS_SEL_MASK                                           0xF
#define QSERDES_COM_DEBUG_BUS_SEL_DEBUG_BUS_SEL_SHFT                                           0x0

/* CMN_MISC1 */
#define QSERDES_COM_CMN_MISC1                                                                  ( 0x1B4 )
#define QSERDES_COM_CMN_MISC1_DIS_CG_RATE_CHANGE_MASK                                          0x40
#define QSERDES_COM_CMN_MISC1_DIS_CG_RATE_CHANGE_SHFT                                          0x6
#define QSERDES_COM_CMN_MISC1_DIS_CG_SYSGLITCH_MASK                                            0x20
#define QSERDES_COM_CMN_MISC1_DIS_CG_SYSGLITCH_SHFT                                            0x5
#define QSERDES_COM_CMN_MISC1_PLL_RESET_MUX_MASK                                               0x10
#define QSERDES_COM_CMN_MISC1_PLL_RESET_MUX_SHFT                                               0x4
#define QSERDES_COM_CMN_MISC1_PLL_RESET_MASK                                                   0x8
#define QSERDES_COM_CMN_MISC1_PLL_RESET_SHFT                                                   0x3
#define QSERDES_COM_CMN_MISC1_PLL_VREG_READY_MUX_MASK                                          0x4
#define QSERDES_COM_CMN_MISC1_PLL_VREG_READY_MUX_SHFT                                          0x2
#define QSERDES_COM_CMN_MISC1_PLL_VREG_READY_MASK                                              0x2
#define QSERDES_COM_CMN_MISC1_PLL_VREG_READY_SHFT                                              0x1
#define QSERDES_COM_CMN_MISC1_DISABLE_B2T_INTEGLOOP_CLKGATE_MASK                               0x1
#define QSERDES_COM_CMN_MISC1_DISABLE_B2T_INTEGLOOP_CLKGATE_SHFT                               0x0

/* CMN_MISC2 */
#define QSERDES_COM_CMN_MISC2                                                                  ( 0x1B8 )
#define QSERDES_COM_CMN_MISC2_RSVD_MASK                                                        0x80
#define QSERDES_COM_CMN_MISC2_RSVD_SHFT                                                        0x7
#define QSERDES_COM_CMN_MISC2_RESCODE_MAX_CAP_MASK                                             0x7F
#define QSERDES_COM_CMN_MISC2_RESCODE_MAX_CAP_SHFT                                             0x0

/* CORECLK_DIV_MODE1 */
#define QSERDES_COM_CORECLK_DIV_MODE1                                                          ( 0x1BC )
#define QSERDES_COM_CORECLK_DIV_MODE1_CORE_CLK_DIV_MODE1_MASK                                  0xFF
#define QSERDES_COM_CORECLK_DIV_MODE1_CORE_CLK_DIV_MODE1_SHFT                                  0x0

/* CMN_RSVD5 */
#define QSERDES_COM_CMN_RSVD5                                                                  ( 0x1C0 )
#define QSERDES_COM_CMN_RSVD5_RSVD_MASK                                                        0x3F
#define QSERDES_COM_CMN_RSVD5_RSVD_SHFT                                                        0x0

/*============================================================================
     QSERDES_TX
============================================================================*/

/* BIST_MODE_LANENO */
#define QSERDES_TX_BIST_MODE_LANENO                                                                   ( 0x200 )
#define QSERDES_TX_BIST_MODE_LANENO_BIST_ERROR_INJECT_MASK                                            0x80
#define QSERDES_TX_BIST_MODE_LANENO_BIST_ERROR_INJECT_SHFT                                            0x7
#define QSERDES_TX_BIST_MODE_LANENO_BIST_LANE_NUMBER_MASK                                             0x60
#define QSERDES_TX_BIST_MODE_LANENO_BIST_LANE_NUMBER_SHFT                                             0x5
#define QSERDES_TX_BIST_MODE_LANENO_BISTMODE_MASK                                                     0x1F
#define QSERDES_TX_BIST_MODE_LANENO_BISTMODE_SHFT                                                     0x0

/* BIST_INVERT */
#define QSERDES_TX_BIST_INVERT                                                                        ( 0x204 )
#define QSERDES_TX_BIST_INVERT_BIST_INVERT_RX_MASK                                                    0x2
#define QSERDES_TX_BIST_INVERT_BIST_INVERT_RX_SHFT                                                    0x1
#define QSERDES_TX_BIST_INVERT_BIST_INVERT_TX_MASK                                                    0x1
#define QSERDES_TX_BIST_INVERT_BIST_INVERT_TX_SHFT                                                    0x0

/* CLKBUF_ENABLE */
#define QSERDES_TX_CLKBUF_ENABLE                                                                      ( 0x208 )
#define QSERDES_TX_CLKBUF_ENABLE_TXONLY_CLOCK_EDGE_MASK                                               0x8
#define QSERDES_TX_CLKBUF_ENABLE_TXONLY_CLOCK_EDGE_SHFT                                               0x3
#define QSERDES_TX_CLKBUF_ENABLE_GATED_CLK_EN_MASK                                                    0x4
#define QSERDES_TX_CLKBUF_ENABLE_GATED_CLK_EN_SHFT                                                    0x2
#define QSERDES_TX_CLKBUF_ENABLE_CLKBUF_EN_MUX_MASK                                                   0x2
#define QSERDES_TX_CLKBUF_ENABLE_CLKBUF_EN_MUX_SHFT                                                   0x1
#define QSERDES_TX_CLKBUF_ENABLE_CLKBUF_EN_MASK                                                       0x1
#define QSERDES_TX_CLKBUF_ENABLE_CLKBUF_EN_SHFT                                                       0x0

/* CMN_CONTROL_ONE */
#define QSERDES_TX_CMN_CONTROL_ONE                                                                    ( 0x20C )
#define QSERDES_TX_CMN_CONTROL_ONE_CMN_CNTRL3_MUX_MASK                                                0x80
#define QSERDES_TX_CMN_CONTROL_ONE_CMN_CNTRL3_MUX_SHFT                                                0x7
#define QSERDES_TX_CMN_CONTROL_ONE_CMN_CNTRL3_MASK                                                    0x40
#define QSERDES_TX_CMN_CONTROL_ONE_CMN_CNTRL3_SHFT                                                    0x6
#define QSERDES_TX_CMN_CONTROL_ONE_CMN_CNTRL2_MUX_MASK                                                0x20
#define QSERDES_TX_CMN_CONTROL_ONE_CMN_CNTRL2_MUX_SHFT                                                0x5
#define QSERDES_TX_CMN_CONTROL_ONE_CMN_CNTRL2_MASK                                                    0x10
#define QSERDES_TX_CMN_CONTROL_ONE_CMN_CNTRL2_SHFT                                                    0x4
#define QSERDES_TX_CMN_CONTROL_ONE_CMN_CNTRL1_MUX_MASK                                                0x8
#define QSERDES_TX_CMN_CONTROL_ONE_CMN_CNTRL1_MUX_SHFT                                                0x3
#define QSERDES_TX_CMN_CONTROL_ONE_CMN_CNTRL1_MASK                                                    0x4
#define QSERDES_TX_CMN_CONTROL_ONE_CMN_CNTRL1_SHFT                                                    0x2
#define QSERDES_TX_CMN_CONTROL_ONE_CMN_CNTRL0_MUX_MASK                                                0x2
#define QSERDES_TX_CMN_CONTROL_ONE_CMN_CNTRL0_MUX_SHFT                                                0x1
#define QSERDES_TX_CMN_CONTROL_ONE_CMN_CNTRL0_MASK                                                    0x1
#define QSERDES_TX_CMN_CONTROL_ONE_CMN_CNTRL0_SHFT                                                    0x0

/* CMN_CONTROL_TWO */
#define QSERDES_TX_CMN_CONTROL_TWO                                                                    ( 0x210 )
#define QSERDES_TX_CMN_CONTROL_TWO_CMN_CNTRL7_MUX_MASK                                                0x80
#define QSERDES_TX_CMN_CONTROL_TWO_CMN_CNTRL7_MUX_SHFT                                                0x7
#define QSERDES_TX_CMN_CONTROL_TWO_CMN_CNTRL7_MASK                                                    0x40
#define QSERDES_TX_CMN_CONTROL_TWO_CMN_CNTRL7_SHFT                                                    0x6
#define QSERDES_TX_CMN_CONTROL_TWO_CMN_CNTRL6_MUX_MASK                                                0x20
#define QSERDES_TX_CMN_CONTROL_TWO_CMN_CNTRL6_MUX_SHFT                                                0x5
#define QSERDES_TX_CMN_CONTROL_TWO_CMN_CNTRL6_MASK                                                    0x10
#define QSERDES_TX_CMN_CONTROL_TWO_CMN_CNTRL6_SHFT                                                    0x4
#define QSERDES_TX_CMN_CONTROL_TWO_CMN_CNTRL5_MUX_MASK                                                0x8
#define QSERDES_TX_CMN_CONTROL_TWO_CMN_CNTRL5_MUX_SHFT                                                0x3
#define QSERDES_TX_CMN_CONTROL_TWO_CMN_CNTRL5_MASK                                                    0x4
#define QSERDES_TX_CMN_CONTROL_TWO_CMN_CNTRL5_SHFT                                                    0x2
#define QSERDES_TX_CMN_CONTROL_TWO_CMN_CNTRL4_MUX_MASK                                                0x2
#define QSERDES_TX_CMN_CONTROL_TWO_CMN_CNTRL4_MUX_SHFT                                                0x1
#define QSERDES_TX_CMN_CONTROL_TWO_CMN_CNTRL4_MASK                                                    0x1
#define QSERDES_TX_CMN_CONTROL_TWO_CMN_CNTRL4_SHFT                                                    0x0

/* CMN_CONTROL_THREE */
#define QSERDES_TX_CMN_CONTROL_THREE                                                                  ( 0x214 )
#define QSERDES_TX_CMN_CONTROL_THREE_CORE_CMN_ACK_OUT_MUX_MASK                                        0x80
#define QSERDES_TX_CMN_CONTROL_THREE_CORE_CMN_ACK_OUT_MUX_SHFT                                        0x7
#define QSERDES_TX_CMN_CONTROL_THREE_CORE_CMN_ACK_OUT_MASK                                            0x40
#define QSERDES_TX_CMN_CONTROL_THREE_CORE_CMN_ACK_OUT_SHFT                                            0x6
#define QSERDES_TX_CMN_CONTROL_THREE_CMN_CNTRL10_MUX_MASK                                             0x20
#define QSERDES_TX_CMN_CONTROL_THREE_CMN_CNTRL10_MUX_SHFT                                             0x5
#define QSERDES_TX_CMN_CONTROL_THREE_CMN_CNTRL10_MASK                                                 0x10
#define QSERDES_TX_CMN_CONTROL_THREE_CMN_CNTRL10_SHFT                                                 0x4
#define QSERDES_TX_CMN_CONTROL_THREE_CMN_CNTRL9_MUX_MASK                                              0x8
#define QSERDES_TX_CMN_CONTROL_THREE_CMN_CNTRL9_MUX_SHFT                                              0x3
#define QSERDES_TX_CMN_CONTROL_THREE_CMN_CNTRL9_MASK                                                  0x4
#define QSERDES_TX_CMN_CONTROL_THREE_CMN_CNTRL9_SHFT                                                  0x2
#define QSERDES_TX_CMN_CONTROL_THREE_CMN_CNTRL8_MUX_MASK                                              0x2
#define QSERDES_TX_CMN_CONTROL_THREE_CMN_CNTRL8_MUX_SHFT                                              0x1
#define QSERDES_TX_CMN_CONTROL_THREE_CMN_CNTRL8_MASK                                                  0x1
#define QSERDES_TX_CMN_CONTROL_THREE_CMN_CNTRL8_SHFT                                                  0x0

/* TX_EMP_POST1_LVL */
#define QSERDES_TX_TX_EMP_POST1_LVL                                                                   ( 0x218 )
#define QSERDES_TX_TX_EMP_POST1_LVL_TX_EMP_POST1_LVL_MUX_MASK                                         0x20
#define QSERDES_TX_TX_EMP_POST1_LVL_TX_EMP_POST1_LVL_MUX_SHFT                                         0x5
#define QSERDES_TX_TX_EMP_POST1_LVL_TX_EMP_POST1_LVL_MASK                                             0x1F
#define QSERDES_TX_TX_EMP_POST1_LVL_TX_EMP_POST1_LVL_SHFT                                             0x0

/* TX_POST2_EMPH */
#define QSERDES_TX_TX_POST2_EMPH                                                                      ( 0x21C )
#define QSERDES_TX_TX_POST2_EMPH_TX_POST2_EMPH_MASK                                                   0x7
#define QSERDES_TX_TX_POST2_EMPH_TX_POST2_EMPH_SHFT                                                   0x0

/* TX_BOOST_LVL_UP_DN */
#define QSERDES_TX_TX_BOOST_LVL_UP_DN                                                                 ( 0x220 )
#define QSERDES_TX_TX_BOOST_LVL_UP_DN_TX_BOOST_LVL_DN_MASK                                            0x38
#define QSERDES_TX_TX_BOOST_LVL_UP_DN_TX_BOOST_LVL_DN_SHFT                                            0x3
#define QSERDES_TX_TX_BOOST_LVL_UP_DN_TX_BOOST_LVL_UP_MASK                                            0x7
#define QSERDES_TX_TX_BOOST_LVL_UP_DN_TX_BOOST_LVL_UP_SHFT                                            0x0

/* HP_PD_ENABLES */
#define QSERDES_TX_HP_PD_ENABLES                                                                      ( 0x224 )
#define QSERDES_TX_HP_PD_ENABLES_TX_PD_TERM_MUX_MASK                                                  0x20
#define QSERDES_TX_HP_PD_ENABLES_TX_PD_TERM_MUX_SHFT                                                  0x5
#define QSERDES_TX_HP_PD_ENABLES_TX_PD_TERM_MASK                                                      0x10
#define QSERDES_TX_HP_PD_ENABLES_TX_PD_TERM_SHFT                                                      0x4
#define QSERDES_TX_HP_PD_ENABLES_TX_PD_SWING_MUX_MASK                                                 0x8
#define QSERDES_TX_HP_PD_ENABLES_TX_PD_SWING_MUX_SHFT                                                 0x3
#define QSERDES_TX_HP_PD_ENABLES_TX_PD_SWING_MASK                                                     0x4
#define QSERDES_TX_HP_PD_ENABLES_TX_PD_SWING_SHFT                                                     0x2
#define QSERDES_TX_HP_PD_ENABLES_TX_HPEN_MUX_MASK                                                     0x2
#define QSERDES_TX_HP_PD_ENABLES_TX_HPEN_MUX_SHFT                                                     0x1
#define QSERDES_TX_HP_PD_ENABLES_TX_HPEN_MASK                                                         0x1
#define QSERDES_TX_HP_PD_ENABLES_TX_HPEN_SHFT                                                         0x0

/* TX_IDLE_LVL_LARGE_AMP */
#define QSERDES_TX_TX_IDLE_LVL_LARGE_AMP                                                              ( 0x228 )
#define QSERDES_TX_TX_IDLE_LVL_LARGE_AMP_TX_LARGE_AMP_MUX_MASK                                        0x10
#define QSERDES_TX_TX_IDLE_LVL_LARGE_AMP_TX_LARGE_AMP_MUX_SHFT                                        0x4
#define QSERDES_TX_TX_IDLE_LVL_LARGE_AMP_TX_LARGE_AMP_MASK                                            0x8
#define QSERDES_TX_TX_IDLE_LVL_LARGE_AMP_TX_LARGE_AMP_SHFT                                            0x3
#define QSERDES_TX_TX_IDLE_LVL_LARGE_AMP_TX_IDLE_LVL_MUX_MASK                                         0x4
#define QSERDES_TX_TX_IDLE_LVL_LARGE_AMP_TX_IDLE_LVL_MUX_SHFT                                         0x2
#define QSERDES_TX_TX_IDLE_LVL_LARGE_AMP_TX_IDLE_LVL_MASK                                             0x3
#define QSERDES_TX_TX_IDLE_LVL_LARGE_AMP_TX_IDLE_LVL_SHFT                                             0x0

/* TX_DRV_LVL */
#define QSERDES_TX_TX_DRV_LVL                                                                         ( 0x22C )
#define QSERDES_TX_TX_DRV_LVL_TX_DRV_LVL_MUX_MASK                                                     0x20
#define QSERDES_TX_TX_DRV_LVL_TX_DRV_LVL_MUX_SHFT                                                     0x5
#define QSERDES_TX_TX_DRV_LVL_TX_DRV_LVL_MASK                                                         0x1F
#define QSERDES_TX_TX_DRV_LVL_TX_DRV_LVL_SHFT                                                         0x0

/* TX_DRV_LVL_OFFSET */
#define QSERDES_TX_TX_DRV_LVL_OFFSET                                                                  ( 0x230 )
#define QSERDES_TX_TX_DRV_LVL_OFFSET_TX_DRV_LVL_OFFSET_MASK                                           0xF
#define QSERDES_TX_TX_DRV_LVL_OFFSET_TX_DRV_LVL_OFFSET_SHFT                                           0x0

/* RESET_TSYNC_EN */
#define QSERDES_TX_RESET_TSYNC_EN                                                                     ( 0x234 )
#define QSERDES_TX_RESET_TSYNC_EN_RESET_TSYNC_EN_MUX_MASK                                             0x2
#define QSERDES_TX_RESET_TSYNC_EN_RESET_TSYNC_EN_MUX_SHFT                                             0x1
#define QSERDES_TX_RESET_TSYNC_EN_RESET_TSYNC_EN_MASK                                                 0x1
#define QSERDES_TX_RESET_TSYNC_EN_RESET_TSYNC_EN_SHFT                                                 0x0

/* PRE_STALL_LDO_BOOST_EN */
#define QSERDES_TX_PRE_STALL_LDO_BOOST_EN                                                             ( 0x238 )
#define QSERDES_TX_PRE_STALL_LDO_BOOST_EN_BOOST_EN_MUX_MASK                                           0x80
#define QSERDES_TX_PRE_STALL_LDO_BOOST_EN_BOOST_EN_MUX_SHFT                                           0x7
#define QSERDES_TX_PRE_STALL_LDO_BOOST_EN_BOOST_EN_MASK                                               0x40
#define QSERDES_TX_PRE_STALL_LDO_BOOST_EN_BOOST_EN_SHFT                                               0x6
#define QSERDES_TX_PRE_STALL_LDO_BOOST_EN_LDO_EN_MUX_MASK                                             0x20
#define QSERDES_TX_PRE_STALL_LDO_BOOST_EN_LDO_EN_MUX_SHFT                                             0x5
#define QSERDES_TX_PRE_STALL_LDO_BOOST_EN_LDO_EN_MASK                                                 0x10
#define QSERDES_TX_PRE_STALL_LDO_BOOST_EN_LDO_EN_SHFT                                                 0x4
#define QSERDES_TX_PRE_STALL_LDO_BOOST_EN_STALL_EN_MUX_MASK                                           0x8
#define QSERDES_TX_PRE_STALL_LDO_BOOST_EN_STALL_EN_MUX_SHFT                                           0x3
#define QSERDES_TX_PRE_STALL_LDO_BOOST_EN_STALL_EN_MASK                                               0x4
#define QSERDES_TX_PRE_STALL_LDO_BOOST_EN_STALL_EN_SHFT                                               0x2
#define QSERDES_TX_PRE_STALL_LDO_BOOST_EN_PRE_EN_MUX_MASK                                             0x2
#define QSERDES_TX_PRE_STALL_LDO_BOOST_EN_PRE_EN_MUX_SHFT                                             0x1
#define QSERDES_TX_PRE_STALL_LDO_BOOST_EN_PRE_EN_MASK                                                 0x1
#define QSERDES_TX_PRE_STALL_LDO_BOOST_EN_PRE_EN_SHFT                                                 0x0

/* TX_BAND */
#define QSERDES_TX_TX_BAND                                                                            ( 0x23C )
#define QSERDES_TX_TX_BAND_TX_BAND_MUX_MASK                                                           0x4
#define QSERDES_TX_TX_BAND_TX_BAND_MUX_SHFT                                                           0x2
#define QSERDES_TX_TX_BAND_TX_BAND_MASK                                                               0x3
#define QSERDES_TX_TX_BAND_TX_BAND_SHFT                                                               0x0

/* SLEW_CNTL */
#define QSERDES_TX_SLEW_CNTL                                                                          ( 0x240 )
#define QSERDES_TX_SLEW_CNTL_SLEW_CNTL_MUX_MASK                                                       0x8
#define QSERDES_TX_SLEW_CNTL_SLEW_CNTL_MUX_SHFT                                                       0x3
#define QSERDES_TX_SLEW_CNTL_SLEW_CNTL_MASK                                                           0x7
#define QSERDES_TX_SLEW_CNTL_SLEW_CNTL_SHFT                                                           0x0

/* INTERFACE_SELECT */
#define QSERDES_TX_INTERFACE_SELECT                                                                   ( 0x244 )
#define QSERDES_TX_INTERFACE_SELECT_TX_CLKDIV_PHASE_MASK                                              0x30
#define QSERDES_TX_INTERFACE_SELECT_TX_CLKDIV_PHASE_SHFT                                              0x4
#define QSERDES_TX_INTERFACE_SELECT_TX_PWM_PHASE_MASK                                                 0xC
#define QSERDES_TX_INTERFACE_SELECT_TX_PWM_PHASE_SHFT                                                 0x2
#define QSERDES_TX_INTERFACE_SELECT_SEL_20B_10B_MASK                                                  0x2
#define QSERDES_TX_INTERFACE_SELECT_SEL_20B_10B_SHFT                                                  0x1
#define QSERDES_TX_INTERFACE_SELECT_SEL_10B_8B_MASK                                                   0x1
#define QSERDES_TX_INTERFACE_SELECT_SEL_10B_8B_SHFT                                                   0x0

/* LPB_EN */
#define QSERDES_TX_LPB_EN                                                                             ( 0x248 )
#define QSERDES_TX_LPB_EN_PLACEHOLDER_CHAIN_INPUT_MASK                                                0x80
#define QSERDES_TX_LPB_EN_PLACEHOLDER_CHAIN_INPUT_SHFT                                                0x7
#define QSERDES_TX_LPB_EN_LINE_LPB_EN_MUX_MASK                                                        0x40
#define QSERDES_TX_LPB_EN_LINE_LPB_EN_MUX_SHFT                                                        0x6
#define QSERDES_TX_LPB_EN_LINE_LPB_EN_MASK                                                            0x20
#define QSERDES_TX_LPB_EN_LINE_LPB_EN_SHFT                                                            0x5
#define QSERDES_TX_LPB_EN_TX_RCLK_LPB_EN_MASK                                                         0x10
#define QSERDES_TX_LPB_EN_TX_RCLK_LPB_EN_SHFT                                                         0x4
#define QSERDES_TX_LPB_EN_SER_LPB_EN_MUX_MASK                                                         0x8
#define QSERDES_TX_LPB_EN_SER_LPB_EN_MUX_SHFT                                                         0x3
#define QSERDES_TX_LPB_EN_SER_LPB_EN_MASK                                                             0x4
#define QSERDES_TX_LPB_EN_SER_LPB_EN_SHFT                                                             0x2
#define QSERDES_TX_LPB_EN_PAR_LPB_EN_MASK                                                             0x2
#define QSERDES_TX_LPB_EN_PAR_LPB_EN_SHFT                                                             0x1
#define QSERDES_TX_LPB_EN_RCLK_LPB_EN_MASK                                                            0x1
#define QSERDES_TX_LPB_EN_RCLK_LPB_EN_SHFT                                                            0x0

/* RES_CODE_LANE_TX */
#define QSERDES_TX_RES_CODE_LANE_TX                                                                   ( 0x24C )
#define QSERDES_TX_RES_CODE_LANE_TX_RES_CODE_LANE_TX_MUX_MASK                                         0x20
#define QSERDES_TX_RES_CODE_LANE_TX_RES_CODE_LANE_TX_MUX_SHFT                                         0x5
#define QSERDES_TX_RES_CODE_LANE_TX_RES_CODE_LANE_TX_MASK                                             0x1F
#define QSERDES_TX_RES_CODE_LANE_TX_RES_CODE_LANE_TX_SHFT                                             0x0

/* RES_CODE_LANE_RX */
#define QSERDES_TX_RES_CODE_LANE_RX                                                                   ( 0x250 )
#define QSERDES_TX_RES_CODE_LANE_RX_RES_CODE_LANE_RX_MUX_MASK                                         0x20
#define QSERDES_TX_RES_CODE_LANE_RX_RES_CODE_LANE_RX_MUX_SHFT                                         0x5
#define QSERDES_TX_RES_CODE_LANE_RX_RES_CODE_LANE_RX_MASK                                             0x1F
#define QSERDES_TX_RES_CODE_LANE_RX_RES_CODE_LANE_RX_SHFT                                             0x0

/* RES_CODE_LANE_OFFSET */
#define QSERDES_TX_RES_CODE_LANE_OFFSET                                                               ( 0x254 )
#define QSERDES_TX_RES_CODE_LANE_OFFSET_RES_CODE_LANE_OFFSET_RX_MASK                                  0xF0
#define QSERDES_TX_RES_CODE_LANE_OFFSET_RES_CODE_LANE_OFFSET_RX_SHFT                                  0x4
#define QSERDES_TX_RES_CODE_LANE_OFFSET_RES_CODE_LANE_OFFSET_TX_MASK                                  0xF
#define QSERDES_TX_RES_CODE_LANE_OFFSET_RES_CODE_LANE_OFFSET_TX_SHFT                                  0x0

/* PERL_LENGTH1 */
#define QSERDES_TX_PERL_LENGTH1                                                                       ( 0x258 )
#define QSERDES_TX_PERL_LENGTH1_PERL_LENGTH1_MASK                                                     0xFF
#define QSERDES_TX_PERL_LENGTH1_PERL_LENGTH1_SHFT                                                     0x0

/* PERL_LENGTH2 */
#define QSERDES_TX_PERL_LENGTH2                                                                       ( 0x25C )
#define QSERDES_TX_PERL_LENGTH2_PERL_LENGTH2_MASK                                                     0xFF
#define QSERDES_TX_PERL_LENGTH2_PERL_LENGTH2_SHFT                                                     0x0

/* SERDES_BYP_EN_OUT */
#define QSERDES_TX_SERDES_BYP_EN_OUT                                                                  ( 0x260 )
#define QSERDES_TX_SERDES_BYP_EN_OUT_SERDES_BYP_OUT_MUX_MASK                                          0x20
#define QSERDES_TX_SERDES_BYP_EN_OUT_SERDES_BYP_OUT_MUX_SHFT                                          0x5
#define QSERDES_TX_SERDES_BYP_EN_OUT_SERDES_BYP_OUT_MASK                                              0x10
#define QSERDES_TX_SERDES_BYP_EN_OUT_SERDES_BYP_OUT_SHFT                                              0x4
#define QSERDES_TX_SERDES_BYP_EN_OUT_SERDES_BYP_OUT_EN_MUX_MASK                                       0x8
#define QSERDES_TX_SERDES_BYP_EN_OUT_SERDES_BYP_OUT_EN_MUX_SHFT                                       0x3
#define QSERDES_TX_SERDES_BYP_EN_OUT_SERDES_BYP_OUT_EN_MASK                                           0x4
#define QSERDES_TX_SERDES_BYP_EN_OUT_SERDES_BYP_OUT_EN_SHFT                                           0x2
#define QSERDES_TX_SERDES_BYP_EN_OUT_SERDES_BYP_IN_EN_MUX_MASK                                        0x2
#define QSERDES_TX_SERDES_BYP_EN_OUT_SERDES_BYP_IN_EN_MUX_SHFT                                        0x1
#define QSERDES_TX_SERDES_BYP_EN_OUT_SERDES_BYP_IN_EN_MASK                                            0x1
#define QSERDES_TX_SERDES_BYP_EN_OUT_SERDES_BYP_IN_EN_SHFT                                            0x0

/* DEBUG_BUS_SEL */
#define QSERDES_TX_DEBUG_BUS_SEL                                                                      ( 0x264 )
#define QSERDES_TX_DEBUG_BUS_SEL_DEBUGBUS_SEL_MASK                                                    0xF
#define QSERDES_TX_DEBUG_BUS_SEL_DEBUGBUS_SEL_SHFT                                                    0x0

/* HIGHZ_TRANSCEIVEREN_BIAS_DRVR_EN */
#define QSERDES_TX_HIGHZ_TRANSCEIVEREN_BIAS_DRVR_EN                                                   ( 0x268 )
#define QSERDES_TX_HIGHZ_TRANSCEIVEREN_BIAS_DRVR_EN_DRVR_EN_MASK                                      0x40
#define QSERDES_TX_HIGHZ_TRANSCEIVEREN_BIAS_DRVR_EN_DRVR_EN_SHFT                                      0x6
#define QSERDES_TX_HIGHZ_TRANSCEIVEREN_BIAS_DRVR_EN_TX_HIGHZ_MUX_MASK                                 0x20
#define QSERDES_TX_HIGHZ_TRANSCEIVEREN_BIAS_DRVR_EN_TX_HIGHZ_MUX_SHFT                                 0x5
#define QSERDES_TX_HIGHZ_TRANSCEIVEREN_BIAS_DRVR_EN_TX_HIGHZ_MASK                                     0x10
#define QSERDES_TX_HIGHZ_TRANSCEIVEREN_BIAS_DRVR_EN_TX_HIGHZ_SHFT                                     0x4
#define QSERDES_TX_HIGHZ_TRANSCEIVEREN_BIAS_DRVR_EN_TRANSCEIVER_BIAS_EN_MUX_MASK                      0x8
#define QSERDES_TX_HIGHZ_TRANSCEIVEREN_BIAS_DRVR_EN_TRANSCEIVER_BIAS_EN_MUX_SHFT                      0x3
#define QSERDES_TX_HIGHZ_TRANSCEIVEREN_BIAS_DRVR_EN_TRANSCEIVER_BIAS_EN_MASK                          0x4
#define QSERDES_TX_HIGHZ_TRANSCEIVEREN_BIAS_DRVR_EN_TRANSCEIVER_BIAS_EN_SHFT                          0x2
#define QSERDES_TX_HIGHZ_TRANSCEIVEREN_BIAS_DRVR_EN_TX_EN_MUX_MASK                                    0x2
#define QSERDES_TX_HIGHZ_TRANSCEIVEREN_BIAS_DRVR_EN_TX_EN_MUX_SHFT                                    0x1
#define QSERDES_TX_HIGHZ_TRANSCEIVEREN_BIAS_DRVR_EN_TX_EN_MASK                                        0x1
#define QSERDES_TX_HIGHZ_TRANSCEIVEREN_BIAS_DRVR_EN_TX_EN_SHFT                                        0x0

/* TX_POL_INV */
#define QSERDES_TX_TX_POL_INV                                                                         ( 0x26C )
#define QSERDES_TX_TX_POL_INV_TX_POL_INV_MUX_MASK                                                     0x2
#define QSERDES_TX_TX_POL_INV_TX_POL_INV_MUX_SHFT                                                     0x1
#define QSERDES_TX_TX_POL_INV_TX_POL_INV_MASK                                                         0x1
#define QSERDES_TX_TX_POL_INV_TX_POL_INV_SHFT                                                         0x0

/* PARRATE_REC_DETECT_IDLE_EN */
#define QSERDES_TX_PARRATE_REC_DETECT_IDLE_EN                                                         ( 0x270 )
#define QSERDES_TX_PARRATE_REC_DETECT_IDLE_EN_PAR_RATE_MUX_MASK                                       0x40
#define QSERDES_TX_PARRATE_REC_DETECT_IDLE_EN_PAR_RATE_MUX_SHFT                                       0x6
#define QSERDES_TX_PARRATE_REC_DETECT_IDLE_EN_PAR_RATE_MASK                                           0x30
#define QSERDES_TX_PARRATE_REC_DETECT_IDLE_EN_PAR_RATE_SHFT                                           0x4
#define QSERDES_TX_PARRATE_REC_DETECT_IDLE_EN_IDLE_EN_MUX_MASK                                        0x8
#define QSERDES_TX_PARRATE_REC_DETECT_IDLE_EN_IDLE_EN_MUX_SHFT                                        0x3
#define QSERDES_TX_PARRATE_REC_DETECT_IDLE_EN_IDLE_EN_MASK                                            0x4
#define QSERDES_TX_PARRATE_REC_DETECT_IDLE_EN_IDLE_EN_SHFT                                            0x2
#define QSERDES_TX_PARRATE_REC_DETECT_IDLE_EN_RCV_DET_EN_MUX_MASK                                     0x2
#define QSERDES_TX_PARRATE_REC_DETECT_IDLE_EN_RCV_DET_EN_MUX_SHFT                                     0x1
#define QSERDES_TX_PARRATE_REC_DETECT_IDLE_EN_RCV_DET_EN_MASK                                         0x1
#define QSERDES_TX_PARRATE_REC_DETECT_IDLE_EN_RCV_DET_EN_SHFT                                         0x0

/* BIST_PATTERN1 */
#define QSERDES_TX_BIST_PATTERN1                                                                      ( 0x274 )
#define QSERDES_TX_BIST_PATTERN1_BIST_PATTERN1_MASK                                                   0xFF
#define QSERDES_TX_BIST_PATTERN1_BIST_PATTERN1_SHFT                                                   0x0

/* BIST_PATTERN2 */
#define QSERDES_TX_BIST_PATTERN2                                                                      ( 0x278 )
#define QSERDES_TX_BIST_PATTERN2_BIST_PATTERN2_MASK                                                   0xFF
#define QSERDES_TX_BIST_PATTERN2_BIST_PATTERN2_SHFT                                                   0x0

/* BIST_PATTERN3 */
#define QSERDES_TX_BIST_PATTERN3                                                                      ( 0x27C )
#define QSERDES_TX_BIST_PATTERN3_BIST_PATTERN3_MASK                                                   0xFF
#define QSERDES_TX_BIST_PATTERN3_BIST_PATTERN3_SHFT                                                   0x0

/* BIST_PATTERN4 */
#define QSERDES_TX_BIST_PATTERN4                                                                      ( 0x280 )
#define QSERDES_TX_BIST_PATTERN4_BIST_PATTERN4_MASK                                                   0xFF
#define QSERDES_TX_BIST_PATTERN4_BIST_PATTERN4_SHFT                                                   0x0

/* BIST_PATTERN5 */
#define QSERDES_TX_BIST_PATTERN5                                                                      ( 0x284 )
#define QSERDES_TX_BIST_PATTERN5_BIST_PATTERN5_MASK                                                   0xFF
#define QSERDES_TX_BIST_PATTERN5_BIST_PATTERN5_SHFT                                                   0x0

/* BIST_PATTERN6 */
#define QSERDES_TX_BIST_PATTERN6                                                                      ( 0x288 )
#define QSERDES_TX_BIST_PATTERN6_BIST_PATTERN6_MASK                                                   0xFF
#define QSERDES_TX_BIST_PATTERN6_BIST_PATTERN6_SHFT                                                   0x0

/* BIST_PATTERN7 */
#define QSERDES_TX_BIST_PATTERN7                                                                      ( 0x28C )
#define QSERDES_TX_BIST_PATTERN7_BIST_PATTERN7_MASK                                                   0xFF
#define QSERDES_TX_BIST_PATTERN7_BIST_PATTERN7_SHFT                                                   0x0

/* BIST_PATTERN8 */
#define QSERDES_TX_BIST_PATTERN8                                                                      ( 0x290 )
#define QSERDES_TX_BIST_PATTERN8_BIST_PATTERN8_MASK                                                   0xFF
#define QSERDES_TX_BIST_PATTERN8_BIST_PATTERN8_SHFT                                                   0x0

/* LANE_MODE */
#define QSERDES_TX_LANE_MODE                                                                          ( 0x294 )
#define QSERDES_TX_LANE_MODE_LANE_MODE_MASK                                                           0xFF
#define QSERDES_TX_LANE_MODE_LANE_MODE_SHFT                                                           0x0

/* IDAC_CAL_LANE_MODE */
#define QSERDES_TX_IDAC_CAL_LANE_MODE                                                                 ( 0x298 )
#define QSERDES_TX_IDAC_CAL_LANE_MODE_IDAC_CAL_LANE_MODE_MASK                                         0xFF
#define QSERDES_TX_IDAC_CAL_LANE_MODE_IDAC_CAL_LANE_MODE_SHFT                                         0x0

/* IDAC_CAL_LANE_MODE_CONFIGURATION */
#define QSERDES_TX_IDAC_CAL_LANE_MODE_CONFIGURATION                                                   ( 0x29C )
#define QSERDES_TX_IDAC_CAL_LANE_MODE_CONFIGURATION_LANE_MODE_MUX_MASK                                0x1
#define QSERDES_TX_IDAC_CAL_LANE_MODE_CONFIGURATION_LANE_MODE_MUX_SHFT                                0x0

/* ATB_SEL1 */
#define QSERDES_TX_ATB_SEL1                                                                           ( 0x2A0 )
#define QSERDES_TX_ATB_SEL1_ATB_SEL1_MASK                                                             0xFF
#define QSERDES_TX_ATB_SEL1_ATB_SEL1_SHFT                                                             0x0

/* ATB_SEL2 */
#define QSERDES_TX_ATB_SEL2                                                                           ( 0x2A4 )
#define QSERDES_TX_ATB_SEL2_ATB_SEL2_MASK                                                             0x1
#define QSERDES_TX_ATB_SEL2_ATB_SEL2_SHFT                                                             0x0

/* RCV_DETECT_LVL */
#define QSERDES_TX_RCV_DETECT_LVL                                                                     ( 0x2A8 )
#define QSERDES_TX_RCV_DETECT_LVL_RCV_DET_LVL_MUX_MASK                                                0x8
#define QSERDES_TX_RCV_DETECT_LVL_RCV_DET_LVL_MUX_SHFT                                                0x3
#define QSERDES_TX_RCV_DETECT_LVL_RCV_DET_LVL_MASK                                                    0x7
#define QSERDES_TX_RCV_DETECT_LVL_RCV_DET_LVL_SHFT                                                    0x0

/* RCV_DETECT_LVL_2 */
#define QSERDES_TX_RCV_DETECT_LVL_2                                                                   ( 0x2AC )
#define QSERDES_TX_RCV_DETECT_LVL_2_RCV_DET_LVL_2_MASK                                                0x38
#define QSERDES_TX_RCV_DETECT_LVL_2_RCV_DET_LVL_2_SHFT                                                0x3
#define QSERDES_TX_RCV_DETECT_LVL_2_RCV_DET_LVL_1_MASK                                                0x7
#define QSERDES_TX_RCV_DETECT_LVL_2_RCV_DET_LVL_1_SHFT                                                0x0

/* PRBS_SEED1 */
#define QSERDES_TX_PRBS_SEED1                                                                         ( 0x2B0 )
#define QSERDES_TX_PRBS_SEED1_PRBS_SEED1_MASK                                                         0xFF
#define QSERDES_TX_PRBS_SEED1_PRBS_SEED1_SHFT                                                         0x0

/* PRBS_SEED2 */
#define QSERDES_TX_PRBS_SEED2                                                                         ( 0x2B4 )
#define QSERDES_TX_PRBS_SEED2_PRBS_SEED2_MASK                                                         0xFF
#define QSERDES_TX_PRBS_SEED2_PRBS_SEED2_SHFT                                                         0x0

/* PRBS_SEED3 */
#define QSERDES_TX_PRBS_SEED3                                                                         ( 0x2B8 )
#define QSERDES_TX_PRBS_SEED3_PRBS_SEED3_MASK                                                         0xFF
#define QSERDES_TX_PRBS_SEED3_PRBS_SEED3_SHFT                                                         0x0

/* PRBS_SEED4 */
#define QSERDES_TX_PRBS_SEED4                                                                         ( 0x2BC )
#define QSERDES_TX_PRBS_SEED4_PRBS_SEED4_MASK                                                         0xFF
#define QSERDES_TX_PRBS_SEED4_PRBS_SEED4_SHFT                                                         0x0

/* RESET_GEN */
#define QSERDES_TX_RESET_GEN                                                                          ( 0x2C0 )
#define QSERDES_TX_RESET_GEN_RESET_GEN_MASK                                                           0x7F
#define QSERDES_TX_RESET_GEN_RESET_GEN_SHFT                                                           0x0

/* RESET_GEN_MUXES */
#define QSERDES_TX_RESET_GEN_MUXES                                                                    ( 0x2C4 )
#define QSERDES_TX_RESET_GEN_MUXES_TSYNC_MUX_MASK                                                     0x2
#define QSERDES_TX_RESET_GEN_MUXES_TSYNC_MUX_SHFT                                                     0x1
#define QSERDES_TX_RESET_GEN_MUXES_RSYNC_MUX_MASK                                                     0x1
#define QSERDES_TX_RESET_GEN_MUXES_RSYNC_MUX_SHFT                                                     0x0

/* TRAN_DRVR_EMP_EN */
#define QSERDES_TX_TRAN_DRVR_EMP_EN                                                                   ( 0x2C8 )
#define QSERDES_TX_TRAN_DRVR_EMP_EN_EMP_EN_MUX_MASK                                                   0x2
#define QSERDES_TX_TRAN_DRVR_EMP_EN_EMP_EN_MUX_SHFT                                                   0x1
#define QSERDES_TX_TRAN_DRVR_EMP_EN_EMP_EN_MASK                                                       0x1
#define QSERDES_TX_TRAN_DRVR_EMP_EN_EMP_EN_SHFT                                                       0x0

/* TX_INTERFACE_MODE */
#define QSERDES_TX_TX_INTERFACE_MODE                                                                  ( 0x2CC )
#define QSERDES_TX_TX_INTERFACE_MODE_TXINTERFACE_MODE_MASK                                            0xF
#define QSERDES_TX_TX_INTERFACE_MODE_TXINTERFACE_MODE_SHFT                                            0x0

/* PWM_CTRL */
#define QSERDES_TX_PWM_CTRL                                                                           ( 0x2D0 )
#define QSERDES_TX_PWM_CTRL_TX_PWM_GEAR_MUX_MASK                                                      0x20
#define QSERDES_TX_PWM_CTRL_TX_PWM_GEAR_MUX_SHFT                                                      0x5
#define QSERDES_TX_PWM_CTRL_TX_PWM_GEAR_MASK                                                          0x1C
#define QSERDES_TX_PWM_CTRL_TX_PWM_GEAR_SHFT                                                          0x2
#define QSERDES_TX_PWM_CTRL_TX_PWM_EN_MUX_MASK                                                        0x2
#define QSERDES_TX_PWM_CTRL_TX_PWM_EN_MUX_SHFT                                                        0x1
#define QSERDES_TX_PWM_CTRL_TX_PWM_EN_MASK                                                            0x1
#define QSERDES_TX_PWM_CTRL_TX_PWM_EN_SHFT                                                            0x0

/* PWM_ENCODED_OR_DATA */
#define QSERDES_TX_PWM_ENCODED_OR_DATA                                                                ( 0x2D4 )
#define QSERDES_TX_PWM_ENCODED_OR_DATA_TX_PWM_NDATA_MUX_MASK                                          0x2
#define QSERDES_TX_PWM_ENCODED_OR_DATA_TX_PWM_NDATA_MUX_SHFT                                          0x1
#define QSERDES_TX_PWM_ENCODED_OR_DATA_TX_PWM_NDATA_MASK                                              0x1
#define QSERDES_TX_PWM_ENCODED_OR_DATA_TX_PWM_NDATA_SHFT                                              0x0

/* PWM_GEAR_1_DIVIDER_BAND2 */
#define QSERDES_TX_PWM_GEAR_1_DIVIDER_BAND2                                                           ( 0x2D8 )
#define QSERDES_TX_PWM_GEAR_1_DIVIDER_BAND2_TX_PWM_DIVGEAR1_MASK                                      0xFF
#define QSERDES_TX_PWM_GEAR_1_DIVIDER_BAND2_TX_PWM_DIVGEAR1_SHFT                                      0x0

/* PWM_GEAR_2_DIVIDER_BAND2 */
#define QSERDES_TX_PWM_GEAR_2_DIVIDER_BAND2                                                           ( 0x2DC )
#define QSERDES_TX_PWM_GEAR_2_DIVIDER_BAND2_TX_PWM_DIVGEAR2_MASK                                      0xFF
#define QSERDES_TX_PWM_GEAR_2_DIVIDER_BAND2_TX_PWM_DIVGEAR2_SHFT                                      0x0

/* PWM_GEAR_3_DIVIDER_BAND2 */
#define QSERDES_TX_PWM_GEAR_3_DIVIDER_BAND2                                                           ( 0x2E0 )
#define QSERDES_TX_PWM_GEAR_3_DIVIDER_BAND2_TX_PWM_DIVGEAR3_MASK                                      0xFF
#define QSERDES_TX_PWM_GEAR_3_DIVIDER_BAND2_TX_PWM_DIVGEAR3_SHFT                                      0x0

/* PWM_GEAR_4_DIVIDER_BAND2 */
#define QSERDES_TX_PWM_GEAR_4_DIVIDER_BAND2                                                           ( 0x2E4 )
#define QSERDES_TX_PWM_GEAR_4_DIVIDER_BAND2_TX_PWM_DIVGEAR4_MASK                                      0xFF
#define QSERDES_TX_PWM_GEAR_4_DIVIDER_BAND2_TX_PWM_DIVGEAR4_SHFT                                      0x0

/* PWM_GEAR_1_DIVIDER_BAND0_1 */
#define QSERDES_TX_PWM_GEAR_1_DIVIDER_BAND0_1                                                         ( 0x2E8 )
#define QSERDES_TX_PWM_GEAR_1_DIVIDER_BAND0_1_TX_PWM_DIVGEAR1A_MASK                                   0xFF
#define QSERDES_TX_PWM_GEAR_1_DIVIDER_BAND0_1_TX_PWM_DIVGEAR1A_SHFT                                   0x0

/* PWM_GEAR_2_DIVIDER_BAND0_1 */
#define QSERDES_TX_PWM_GEAR_2_DIVIDER_BAND0_1                                                         ( 0x2EC )
#define QSERDES_TX_PWM_GEAR_2_DIVIDER_BAND0_1_TX_PWM_DIVGEAR2A_MASK                                   0xFF
#define QSERDES_TX_PWM_GEAR_2_DIVIDER_BAND0_1_TX_PWM_DIVGEAR2A_SHFT                                   0x0

/* PWM_GEAR_3_DIVIDER_BAND0_1 */
#define QSERDES_TX_PWM_GEAR_3_DIVIDER_BAND0_1                                                         ( 0x2F0 )
#define QSERDES_TX_PWM_GEAR_3_DIVIDER_BAND0_1_TX_PWM_DIVGEAR3A_MASK                                   0xFF
#define QSERDES_TX_PWM_GEAR_3_DIVIDER_BAND0_1_TX_PWM_DIVGEAR3A_SHFT                                   0x0

/* PWM_GEAR_4_DIVIDER_BAND0_1 */
#define QSERDES_TX_PWM_GEAR_4_DIVIDER_BAND0_1                                                         ( 0x2F4 )
#define QSERDES_TX_PWM_GEAR_4_DIVIDER_BAND0_1_TX_PWM_DIVGEAR4A_MASK                                   0xFF
#define QSERDES_TX_PWM_GEAR_4_DIVIDER_BAND0_1_TX_PWM_DIVGEAR4A_SHFT                                   0x0

/* VMODE_CTRL1 */
#define QSERDES_TX_VMODE_CTRL1                                                                        ( 0x2F8 )
#define QSERDES_TX_VMODE_CTRL1_VMODE_CTRL1_MASK                                                       0xFF
#define QSERDES_TX_VMODE_CTRL1_VMODE_CTRL1_SHFT                                                       0x0

/* VMODE_CTRL2 */
#define QSERDES_TX_VMODE_CTRL2                                                                        ( 0x2FC )
#define QSERDES_TX_VMODE_CTRL2_VMODE_CTRL2_MASK                                                       0xFF
#define QSERDES_TX_VMODE_CTRL2_VMODE_CTRL2_SHFT                                                       0x0

/* TX_ALOG_INTF_OBSV_CNTL */
#define QSERDES_TX_TX_ALOG_INTF_OBSV_CNTL                                                             ( 0x300 )
#define QSERDES_TX_TX_ALOG_INTF_OBSV_CNTL_TX_ALOG_INTF_OBSV_CNTL_MASK                                 0x1F
#define QSERDES_TX_TX_ALOG_INTF_OBSV_CNTL_TX_ALOG_INTF_OBSV_CNTL_SHFT                                 0x0

/* BIST_STATUS */
#define QSERDES_TX_BIST_STATUS                                                                        ( 0x304 )
#define QSERDES_TX_BIST_STATUS_PLACEHOLDER_CHAIN_OUTPUT_MASK                                          0x8
#define QSERDES_TX_BIST_STATUS_PLACEHOLDER_CHAIN_OUTPUT_SHFT                                          0x3
#define QSERDES_TX_BIST_STATUS_BIST_STATUS_RO_MASK                                                    0x7
#define QSERDES_TX_BIST_STATUS_BIST_STATUS_RO_SHFT                                                    0x0

/* BIST_ERROR_COUNT1 */
#define QSERDES_TX_BIST_ERROR_COUNT1                                                                  ( 0x308 )
#define QSERDES_TX_BIST_ERROR_COUNT1_BIST_ERROR_COUNT1_STATUS_MASK                                    0xFF
#define QSERDES_TX_BIST_ERROR_COUNT1_BIST_ERROR_COUNT1_STATUS_SHFT                                    0x0

/* BIST_ERROR_COUNT2 */
#define QSERDES_TX_BIST_ERROR_COUNT2                                                                  ( 0x30C )
#define QSERDES_TX_BIST_ERROR_COUNT2_BIST_ERROR_COUNT2_STATUS_MASK                                    0xFF
#define QSERDES_TX_BIST_ERROR_COUNT2_BIST_ERROR_COUNT2_STATUS_SHFT                                    0x0

/* TX_ALOG_INTF_OBSV */
#define QSERDES_TX_TX_ALOG_INTF_OBSV                                                                  ( 0x310 )
#define QSERDES_TX_TX_ALOG_INTF_OBSV_TX_ALOG_INTF_OBSV_STATUS_MASK                                    0xFF
#define QSERDES_TX_TX_ALOG_INTF_OBSV_TX_ALOG_INTF_OBSV_STATUS_SHFT                                    0x0


/*============================================================================
     QSERDES_RX
============================================================================*/

/* UCDR_FO_GAIN_HALF */
#define QSERDES_RX_UCDR_FO_GAIN_HALF                                                             ( 0x400 )
#define QSERDES_RX_UCDR_FO_GAIN_HALF_UCDR_FO_GAIN_HALF_MASK                                      0x7F
#define QSERDES_RX_UCDR_FO_GAIN_HALF_UCDR_FO_GAIN_HALF_SHFT                                      0x0

/* UCDR_FO_GAIN_QUARTER */
#define QSERDES_RX_UCDR_FO_GAIN_QUARTER                                                          ( 0x404 )
#define QSERDES_RX_UCDR_FO_GAIN_QUARTER_UCDR_FO_GAIN_QUARTER_MASK                                0x7F
#define QSERDES_RX_UCDR_FO_GAIN_QUARTER_UCDR_FO_GAIN_QUARTER_SHFT                                0x0

/* UCDR_FO_GAIN_EIGHTH */
#define QSERDES_RX_UCDR_FO_GAIN_EIGHTH                                                           ( 0x408 )
#define QSERDES_RX_UCDR_FO_GAIN_EIGHTH_UCDR_FO_GAIN_EIGHTH_MASK                                  0x7F
#define QSERDES_RX_UCDR_FO_GAIN_EIGHTH_UCDR_FO_GAIN_EIGHTH_SHFT                                  0x0

/* UCDR_FO_GAIN */
#define QSERDES_RX_UCDR_FO_GAIN                                                                  ( 0x40C )
#define QSERDES_RX_UCDR_FO_GAIN_UCDR_FO_GAIN_MASK                                                0x7F
#define QSERDES_RX_UCDR_FO_GAIN_UCDR_FO_GAIN_SHFT                                                0x0

/* UCDR_SO_GAIN_HALF */
#define QSERDES_RX_UCDR_SO_GAIN_HALF                                                             ( 0x410 )
#define QSERDES_RX_UCDR_SO_GAIN_HALF_UCDR_SO_GAIN_HALF_MASK                                      0x7F
#define QSERDES_RX_UCDR_SO_GAIN_HALF_UCDR_SO_GAIN_HALF_SHFT                                      0x0

/* UCDR_SO_GAIN_QUARTER */
#define QSERDES_RX_UCDR_SO_GAIN_QUARTER                                                          ( 0x414 )
#define QSERDES_RX_UCDR_SO_GAIN_QUARTER_UCDR_SO_GAIN_QUARTER_MASK                                0x7F
#define QSERDES_RX_UCDR_SO_GAIN_QUARTER_UCDR_SO_GAIN_QUARTER_SHFT                                0x0

/* UCDR_SO_GAIN_EIGHTH */
#define QSERDES_RX_UCDR_SO_GAIN_EIGHTH                                                           ( 0x418 )
#define QSERDES_RX_UCDR_SO_GAIN_EIGHTH_UCDR_SO_GAIN_EIGHTH_MASK                                  0x7F
#define QSERDES_RX_UCDR_SO_GAIN_EIGHTH_UCDR_SO_GAIN_EIGHTH_SHFT                                  0x0

/* UCDR_SO_GAIN */
#define QSERDES_RX_UCDR_SO_GAIN                                                                  ( 0x41C )
#define QSERDES_RX_UCDR_SO_GAIN_UCDR_SO_GAIN_MASK                                                0x7F
#define QSERDES_RX_UCDR_SO_GAIN_UCDR_SO_GAIN_SHFT                                                0x0

/* UCDR_SVS_FO_GAIN_HALF */
#define QSERDES_RX_UCDR_SVS_FO_GAIN_HALF                                                         ( 0x420 )
#define QSERDES_RX_UCDR_SVS_FO_GAIN_HALF_UCDR_SVS_FO_GAIN_HALF_MASK                              0x7F
#define QSERDES_RX_UCDR_SVS_FO_GAIN_HALF_UCDR_SVS_FO_GAIN_HALF_SHFT                              0x0

/* UCDR_SVS_FO_GAIN_QUARTER */
#define QSERDES_RX_UCDR_SVS_FO_GAIN_QUARTER                                                      ( 0x424 )
#define QSERDES_RX_UCDR_SVS_FO_GAIN_QUARTER_UCDR_SVS_FO_GAIN_QUARTER_MASK                        0x7F
#define QSERDES_RX_UCDR_SVS_FO_GAIN_QUARTER_UCDR_SVS_FO_GAIN_QUARTER_SHFT                        0x0

/* UCDR_SVS_FO_GAIN_EIGHTH */
#define QSERDES_RX_UCDR_SVS_FO_GAIN_EIGHTH                                                       ( 0x428 )
#define QSERDES_RX_UCDR_SVS_FO_GAIN_EIGHTH_UCDR_SVS_FO_GAIN_EIGHTH_MASK                          0x7F
#define QSERDES_RX_UCDR_SVS_FO_GAIN_EIGHTH_UCDR_SVS_FO_GAIN_EIGHTH_SHFT                          0x0

/* UCDR_SVS_FO_GAIN */
#define QSERDES_RX_UCDR_SVS_FO_GAIN                                                              ( 0x42C )
#define QSERDES_RX_UCDR_SVS_FO_GAIN_UCDR_SVS_FO_GAIN_MASK                                        0x7F
#define QSERDES_RX_UCDR_SVS_FO_GAIN_UCDR_SVS_FO_GAIN_SHFT                                        0x0

/* UCDR_SVS_SO_GAIN_HALF */
#define QSERDES_RX_UCDR_SVS_SO_GAIN_HALF                                                         ( 0x430 )
#define QSERDES_RX_UCDR_SVS_SO_GAIN_HALF_UCDR_SVS_SO_GAIN_HALF_MASK                              0x7F
#define QSERDES_RX_UCDR_SVS_SO_GAIN_HALF_UCDR_SVS_SO_GAIN_HALF_SHFT                              0x0

/* UCDR_SVS_SO_GAIN_QUARTER */
#define QSERDES_RX_UCDR_SVS_SO_GAIN_QUARTER                                                      ( 0x434 )
#define QSERDES_RX_UCDR_SVS_SO_GAIN_QUARTER_UCDR_SVS_SO_GAIN_QUARTER_MASK                        0x7F
#define QSERDES_RX_UCDR_SVS_SO_GAIN_QUARTER_UCDR_SVS_SO_GAIN_QUARTER_SHFT                        0x0

/* UCDR_SVS_SO_GAIN_EIGHTH */
#define QSERDES_RX_UCDR_SVS_SO_GAIN_EIGHTH                                                       ( 0x438 )
#define QSERDES_RX_UCDR_SVS_SO_GAIN_EIGHTH_UCDR_SVS_SO_GAIN_EIGHTH_MASK                          0x7F
#define QSERDES_RX_UCDR_SVS_SO_GAIN_EIGHTH_UCDR_SVS_SO_GAIN_EIGHTH_SHFT                          0x0

/* UCDR_SVS_SO_GAIN */
#define QSERDES_RX_UCDR_SVS_SO_GAIN                                                              ( 0x43C )
#define QSERDES_RX_UCDR_SVS_SO_GAIN_UCDR_SVS_SO_GAIN_MASK                                        0x7F
#define QSERDES_RX_UCDR_SVS_SO_GAIN_UCDR_SVS_SO_GAIN_SHFT                                        0x0

/* UCDR_FASTLOCK_FO_GAIN */
#define QSERDES_RX_UCDR_FASTLOCK_FO_GAIN                                                         ( 0x440 )
#define QSERDES_RX_UCDR_FASTLOCK_FO_GAIN_UCDR_FASTLOCK_FO_GAIN_MASK                              0x7F
#define QSERDES_RX_UCDR_FASTLOCK_FO_GAIN_UCDR_FASTLOCK_FO_GAIN_SHFT                              0x0

/* UCDR_FD_GAIN */
#define QSERDES_RX_UCDR_FD_GAIN                                                                  ( 0x444 )
#define QSERDES_RX_UCDR_FD_GAIN_UCDR_FD_GAIN_MASK                                                0x7F
#define QSERDES_RX_UCDR_FD_GAIN_UCDR_FD_GAIN_SHFT                                                0x0

/* UCDR_SO_SATURATION_AND_ENABLE */
#define QSERDES_RX_UCDR_SO_SATURATION_AND_ENABLE                                                 ( 0x448 )
#define QSERDES_RX_UCDR_SO_SATURATION_AND_ENABLE_UCDR_ENABLE_MASK                                0x40
#define QSERDES_RX_UCDR_SO_SATURATION_AND_ENABLE_UCDR_ENABLE_SHFT                                0x6
#define QSERDES_RX_UCDR_SO_SATURATION_AND_ENABLE_UCDR_SO_SATURATION_MASK                         0x3F
#define QSERDES_RX_UCDR_SO_SATURATION_AND_ENABLE_UCDR_SO_SATURATION_SHFT                         0x0

/* UCDR_FO_TO_SO_DELAY */
#define QSERDES_RX_UCDR_FO_TO_SO_DELAY                                                           ( 0x44C )
#define QSERDES_RX_UCDR_FO_TO_SO_DELAY_UCDR_FO_TO_SO_DELAY_MASK                                  0xFF
#define QSERDES_RX_UCDR_FO_TO_SO_DELAY_UCDR_FO_TO_SO_DELAY_SHFT                                  0x0

/* UCDR_FASTLOCK_COUNT_LOW */
#define QSERDES_RX_UCDR_FASTLOCK_COUNT_LOW                                                       ( 0x450 )
#define QSERDES_RX_UCDR_FASTLOCK_COUNT_LOW_UCDR_FASTLOCK_COUNTER_LOW_MASK                        0xFF
#define QSERDES_RX_UCDR_FASTLOCK_COUNT_LOW_UCDR_FASTLOCK_COUNTER_LOW_SHFT                        0x0

/* UCDR_FASTLOCK_COUNT_HIGH */
#define QSERDES_RX_UCDR_FASTLOCK_COUNT_HIGH                                                      ( 0x454 )
#define QSERDES_RX_UCDR_FASTLOCK_COUNT_HIGH_UCDR_FASTLOCK_COUNTER_HIGH_MASK                      0xF
#define QSERDES_RX_UCDR_FASTLOCK_COUNT_HIGH_UCDR_FASTLOCK_COUNTER_HIGH_SHFT                      0x0

/* UCDR_MODULATE */
#define QSERDES_RX_UCDR_MODULATE                                                                 ( 0x458 )
#define QSERDES_RX_UCDR_MODULATE_UCDR_MODULATE_PERIOD_MASK                                       0xFF
#define QSERDES_RX_UCDR_MODULATE_UCDR_MODULATE_PERIOD_SHFT                                       0x0

/* UCDR_PI_CONTROLS */
#define QSERDES_RX_UCDR_PI_CONTROLS                                                              ( 0x45C )
#define QSERDES_RX_UCDR_PI_CONTROLS_FD_MODE_MASK                                                 0x80
#define QSERDES_RX_UCDR_PI_CONTROLS_FD_MODE_SHFT                                                 0x7
#define QSERDES_RX_UCDR_PI_CONTROLS_UCDR_MODULATE_GAIN_MASK                                      0x40
#define QSERDES_RX_UCDR_PI_CONTROLS_UCDR_MODULATE_GAIN_SHFT                                      0x6
#define QSERDES_RX_UCDR_PI_CONTROLS_UCDR_BLOCK_STEP_BY_TWO_MASK                                  0x20
#define QSERDES_RX_UCDR_PI_CONTROLS_UCDR_BLOCK_STEP_BY_TWO_SHFT                                  0x5
#define QSERDES_RX_UCDR_PI_CONTROLS_UCDR_ADD_ONE_FO_GAIN_COMPLEMENT_MASK                         0x10
#define QSERDES_RX_UCDR_PI_CONTROLS_UCDR_ADD_ONE_FO_GAIN_COMPLEMENT_SHFT                         0x4
#define QSERDES_RX_UCDR_PI_CONTROLS_UCDR_ADD_ONE_FO_GAIN_MASK                                    0x8
#define QSERDES_RX_UCDR_PI_CONTROLS_UCDR_ADD_ONE_FO_GAIN_SHFT                                    0x3
#define QSERDES_RX_UCDR_PI_CONTROLS_UCDR_STEP_BY_TWO_MUX_MASK                                    0x4
#define QSERDES_RX_UCDR_PI_CONTROLS_UCDR_STEP_BY_TWO_MUX_SHFT                                    0x2
#define QSERDES_RX_UCDR_PI_CONTROLS_UCDR_STEP_BY_TWO_MASK                                        0x2
#define QSERDES_RX_UCDR_PI_CONTROLS_UCDR_STEP_BY_TWO_SHFT                                        0x1
#define QSERDES_RX_UCDR_PI_CONTROLS_UCDR_FASTLOCK_ENABLE_MASK                                    0x1
#define QSERDES_RX_UCDR_PI_CONTROLS_UCDR_FASTLOCK_ENABLE_SHFT                                    0x0

/* RBIST_CONTROL */
#define QSERDES_RX_RBIST_CONTROL                                                                 ( 0x460 )
#define QSERDES_RX_RBIST_CONTROL_RBIST_ENABLE_MASK                                               0x10
#define QSERDES_RX_RBIST_CONTROL_RBIST_ENABLE_SHFT                                               0x4
#define QSERDES_RX_RBIST_CONTROL_RBIST_SELECT_MASK                                               0xF
#define QSERDES_RX_RBIST_CONTROL_RBIST_SELECT_SHFT                                               0x0

/* AUX_CONTROL */
#define QSERDES_RX_AUX_CONTROL                                                                   ( 0x464 )
#define QSERDES_RX_AUX_CONTROL_AUXDATA_EN_MASK                                                   0x80
#define QSERDES_RX_AUX_CONTROL_AUXDATA_EN_SHFT                                                   0x7
#define QSERDES_RX_AUX_CONTROL_AUXCLK_EN_MASK                                                    0x40
#define QSERDES_RX_AUX_CONTROL_AUXCLK_EN_SHFT                                                    0x6
#define QSERDES_RX_AUX_CONTROL_AUX_OFFSET_MASK                                                   0x3F
#define QSERDES_RX_AUX_CONTROL_AUX_OFFSET_SHFT                                                   0x0

/* AUX_DATA_TCOARSE */
#define QSERDES_RX_AUX_DATA_TCOARSE                                                              ( 0x468 )
#define QSERDES_RX_AUX_DATA_TCOARSE_AUXDATA_TCOARSE_MASK                                         0xFF
#define QSERDES_RX_AUX_DATA_TCOARSE_AUXDATA_TCOARSE_SHFT                                         0x0

/* AUX_DATA_TFINE_LSB */
#define QSERDES_RX_AUX_DATA_TFINE_LSB                                                            ( 0x46C )
#define QSERDES_RX_AUX_DATA_TFINE_LSB_AUXDATA_TFINE_LSB_MASK                                     0xFF
#define QSERDES_RX_AUX_DATA_TFINE_LSB_AUXDATA_TFINE_LSB_SHFT                                     0x0

/* AUX_DATA_TFINE_MSB */
#define QSERDES_RX_AUX_DATA_TFINE_MSB                                                            ( 0x470 )
#define QSERDES_RX_AUX_DATA_TFINE_MSB_AUXDATA_TFINE_MSB_MASK                                     0xFF
#define QSERDES_RX_AUX_DATA_TFINE_MSB_AUXDATA_TFINE_MSB_SHFT                                     0x0

/* RCLK_AUXDATA_SEL */
#define QSERDES_RX_RCLK_AUXDATA_SEL                                                              ( 0x474 )
#define QSERDES_RX_RCLK_AUXDATA_SEL_FD_EN_MUX_MASK                                               0x80
#define QSERDES_RX_RCLK_AUXDATA_SEL_FD_EN_MUX_SHFT                                               0x7
#define QSERDES_RX_RCLK_AUXDATA_SEL_FD_EN_MASK                                                   0x40
#define QSERDES_RX_RCLK_AUXDATA_SEL_FD_EN_SHFT                                                   0x6
#define QSERDES_RX_RCLK_AUXDATA_SEL_AUXDATA_EN_MUX_MASK                                          0x20
#define QSERDES_RX_RCLK_AUXDATA_SEL_AUXDATA_EN_MUX_SHFT                                          0x5
#define QSERDES_RX_RCLK_AUXDATA_SEL_AUXCLK_EN_MUX_MASK                                           0x10
#define QSERDES_RX_RCLK_AUXDATA_SEL_AUXCLK_EN_MUX_SHFT                                           0x4
#define QSERDES_RX_RCLK_AUXDATA_SEL_IACOMPARE_CLEAR_MASK                                         0x8
#define QSERDES_RX_RCLK_AUXDATA_SEL_IACOMPARE_CLEAR_SHFT                                         0x3
#define QSERDES_RX_RCLK_AUXDATA_SEL_IACOMPARE_ENABLE_MASK                                        0x4
#define QSERDES_RX_RCLK_AUXDATA_SEL_IACOMPARE_ENABLE_SHFT                                        0x2
#define QSERDES_RX_RCLK_AUXDATA_SEL_AUXDATA_SEL_MASK                                             0x2
#define QSERDES_RX_RCLK_AUXDATA_SEL_AUXDATA_SEL_SHFT                                             0x1
#define QSERDES_RX_RCLK_AUXDATA_SEL_RCLK_SEL_MASK                                                0x1
#define QSERDES_RX_RCLK_AUXDATA_SEL_RCLK_SEL_SHFT                                                0x0

/* AC_JTAG_ENABLE */
#define QSERDES_RX_AC_JTAG_ENABLE                                                                ( 0x478 )
#define QSERDES_RX_AC_JTAG_ENABLE_AC_JTAG_EN_MASK                                                0x1
#define QSERDES_RX_AC_JTAG_ENABLE_AC_JTAG_EN_SHFT                                                0x0

/* AC_JTAG_INITP */
#define QSERDES_RX_AC_JTAG_INITP                                                                 ( 0x47C )
#define QSERDES_RX_AC_JTAG_INITP_AC_JTAG_INITP_MUX_MASK                                          0x2
#define QSERDES_RX_AC_JTAG_INITP_AC_JTAG_INITP_MUX_SHFT                                          0x1
#define QSERDES_RX_AC_JTAG_INITP_AC_JTAG_INITP_MASK                                              0x1
#define QSERDES_RX_AC_JTAG_INITP_AC_JTAG_INITP_SHFT                                              0x0

/* AC_JTAG_INITN */
#define QSERDES_RX_AC_JTAG_INITN                                                                 ( 0x480 )
#define QSERDES_RX_AC_JTAG_INITN_AC_JTAG_INITN_MUX_MASK                                          0x2
#define QSERDES_RX_AC_JTAG_INITN_AC_JTAG_INITN_MUX_SHFT                                          0x1
#define QSERDES_RX_AC_JTAG_INITN_AC_JTAG_INITN_MASK                                              0x1
#define QSERDES_RX_AC_JTAG_INITN_AC_JTAG_INITN_SHFT                                              0x0

/* AC_JTAG_LVL */
#define QSERDES_RX_AC_JTAG_LVL                                                                   ( 0x484 )
#define QSERDES_RX_AC_JTAG_LVL_AC_JTAG_LVL_MASK                                                  0x1
#define QSERDES_RX_AC_JTAG_LVL_AC_JTAG_LVL_SHFT                                                  0x0

/* AC_JTAG_MODE */
#define QSERDES_RX_AC_JTAG_MODE                                                                  ( 0x488 )
#define QSERDES_RX_AC_JTAG_MODE_AC_JTAG_MODE_MASK                                                0x1
#define QSERDES_RX_AC_JTAG_MODE_AC_JTAG_MODE_SHFT                                                0x0

/* AC_JTAG_RESET */
#define QSERDES_RX_AC_JTAG_RESET                                                                 ( 0x48C )
#define QSERDES_RX_AC_JTAG_RESET_AC_JTAG_RESET_MASK                                              0x1
#define QSERDES_RX_AC_JTAG_RESET_AC_JTAG_RESET_SHFT                                              0x0

/* RX_TERM_BW */
#define QSERDES_RX_RX_TERM_BW                                                                    ( 0x490 )
#define QSERDES_RX_RX_TERM_BW_RX_TERM_BW_BAND3_MASK                                              0xC0
#define QSERDES_RX_RX_TERM_BW_RX_TERM_BW_BAND3_SHFT                                              0x6
#define QSERDES_RX_RX_TERM_BW_RX_TERM_BW_BAND2_MASK                                              0x30
#define QSERDES_RX_RX_TERM_BW_RX_TERM_BW_BAND2_SHFT                                              0x4
#define QSERDES_RX_RX_TERM_BW_RX_TERM_BW_BAND1_MASK                                              0xC
#define QSERDES_RX_RX_TERM_BW_RX_TERM_BW_BAND1_SHFT                                              0x2
#define QSERDES_RX_RX_TERM_BW_RX_TERM_BW_BAND0_MASK                                              0x3
#define QSERDES_RX_RX_TERM_BW_RX_TERM_BW_BAND0_SHFT                                              0x0

/* RX_RCVR_IQ_EN */
#define QSERDES_RX_RX_RCVR_IQ_EN                                                                 ( 0x494 )
#define QSERDES_RX_RX_RCVR_IQ_EN_RX_POL_INV_MASK                                                 0x40
#define QSERDES_RX_RX_RCVR_IQ_EN_RX_POL_INV_SHFT                                                 0x6
#define QSERDES_RX_RX_RCVR_IQ_EN_RX_Q_EN_MUX_MASK                                                0x20
#define QSERDES_RX_RX_RCVR_IQ_EN_RX_Q_EN_MUX_SHFT                                                0x5
#define QSERDES_RX_RX_RCVR_IQ_EN_RX_Q_EN_MASK                                                    0x10
#define QSERDES_RX_RX_RCVR_IQ_EN_RX_Q_EN_SHFT                                                    0x4
#define QSERDES_RX_RX_RCVR_IQ_EN_RX_I_EN_MUX_MASK                                                0x8
#define QSERDES_RX_RX_RCVR_IQ_EN_RX_I_EN_MUX_SHFT                                                0x3
#define QSERDES_RX_RX_RCVR_IQ_EN_RX_I_EN_MASK                                                    0x4
#define QSERDES_RX_RX_RCVR_IQ_EN_RX_I_EN_SHFT                                                    0x2
#define QSERDES_RX_RX_RCVR_IQ_EN_RCVR_EN_MUX_MASK                                                0x2
#define QSERDES_RX_RX_RCVR_IQ_EN_RCVR_EN_MUX_SHFT                                                0x1
#define QSERDES_RX_RX_RCVR_IQ_EN_RCVR_EN_MASK                                                    0x1
#define QSERDES_RX_RX_RCVR_IQ_EN_RCVR_EN_SHFT                                                    0x0

/* RX_IDAC_I_DC_OFFSETS */
#define QSERDES_RX_RX_IDAC_I_DC_OFFSETS                                                          ( 0x498 )
#define QSERDES_RX_RX_IDAC_I_DC_OFFSETS_IDAC_I_MASK                                              0x1F
#define QSERDES_RX_RX_IDAC_I_DC_OFFSETS_IDAC_I_SHFT                                              0x0

/* RX_IDAC_IBAR_DC_OFFSETS */
#define QSERDES_RX_RX_IDAC_IBAR_DC_OFFSETS                                                       ( 0x49C )
#define QSERDES_RX_RX_IDAC_IBAR_DC_OFFSETS_IDAC_IB_MASK                                          0x1F
#define QSERDES_RX_RX_IDAC_IBAR_DC_OFFSETS_IDAC_IB_SHFT                                          0x0

/* RX_IDAC_Q_DC_OFFSETS */
#define QSERDES_RX_RX_IDAC_Q_DC_OFFSETS                                                          ( 0x4A0 )
#define QSERDES_RX_RX_IDAC_Q_DC_OFFSETS_IDAC_Q_MASK                                              0x1F
#define QSERDES_RX_RX_IDAC_Q_DC_OFFSETS_IDAC_Q_SHFT                                              0x0

/* RX_IDAC_QBAR_DC_OFFSETS */
#define QSERDES_RX_RX_IDAC_QBAR_DC_OFFSETS                                                       ( 0x4A4 )
#define QSERDES_RX_RX_IDAC_QBAR_DC_OFFSETS_IDAC_QB_MASK                                          0x1F
#define QSERDES_RX_RX_IDAC_QBAR_DC_OFFSETS_IDAC_QB_SHFT                                          0x0

/* RX_IDAC_A_DC_OFFSETS */
#define QSERDES_RX_RX_IDAC_A_DC_OFFSETS                                                          ( 0x4A8 )
#define QSERDES_RX_RX_IDAC_A_DC_OFFSETS_IDAC_A_MASK                                              0x1F
#define QSERDES_RX_RX_IDAC_A_DC_OFFSETS_IDAC_A_SHFT                                              0x0

/* RX_IDAC_ABAR_DC_OFFSETS */
#define QSERDES_RX_RX_IDAC_ABAR_DC_OFFSETS                                                       ( 0x4AC )
#define QSERDES_RX_RX_IDAC_ABAR_DC_OFFSETS_IDAC_AB_MASK                                          0x1F
#define QSERDES_RX_RX_IDAC_ABAR_DC_OFFSETS_IDAC_AB_SHFT                                          0x0

/* RX_IDAC_EN */
#define QSERDES_RX_RX_IDAC_EN                                                                    ( 0x4B0 )
#define QSERDES_RX_RX_IDAC_EN_CORE_IDAC_FORCE_MASK_MASK                                          0x10
#define QSERDES_RX_RX_IDAC_EN_CORE_IDAC_FORCE_MASK_SHFT                                          0x4
#define QSERDES_RX_RX_IDAC_EN_IDAC_MUX_MASK                                                      0x8
#define QSERDES_RX_RX_IDAC_EN_IDAC_MUX_SHFT                                                      0x3
#define QSERDES_RX_RX_IDAC_EN_IDAC_CAL_BYPASS_MASK                                               0x4
#define QSERDES_RX_RX_IDAC_EN_IDAC_CAL_BYPASS_SHFT                                               0x2
#define QSERDES_RX_RX_IDAC_EN_IDAC_CAL_EN_MUX_MASK                                               0x2
#define QSERDES_RX_RX_IDAC_EN_IDAC_CAL_EN_MUX_SHFT                                               0x1
#define QSERDES_RX_RX_IDAC_EN_IDAC_CAL_EN_MASK                                                   0x1
#define QSERDES_RX_RX_IDAC_EN_IDAC_CAL_EN_SHFT                                                   0x0

/* RX_IDAC_ENABLES */
#define QSERDES_RX_RX_IDAC_ENABLES                                                               ( 0x4B4 )
#define QSERDES_RX_RX_IDAC_ENABLES_IDAC_ENABLE_MASK                                              0x3F
#define QSERDES_RX_RX_IDAC_ENABLES_IDAC_ENABLE_SHFT                                              0x0

/* RX_IDAC_SIGN */
#define QSERDES_RX_RX_IDAC_SIGN                                                                  ( 0x4B8 )
#define QSERDES_RX_RX_IDAC_SIGN_IDAC_SIGN_MASK                                                   0x3F
#define QSERDES_RX_RX_IDAC_SIGN_IDAC_SIGN_SHFT                                                   0x0

/* RX_HIGHZ_HIGHRATE */
#define QSERDES_RX_RX_HIGHZ_HIGHRATE                                                             ( 0x4BC )
#define QSERDES_RX_RX_HIGHZ_HIGHRATE_RX_HIGHZ_MUX_MASK                                           0x2
#define QSERDES_RX_RX_HIGHZ_HIGHRATE_RX_HIGHZ_MUX_SHFT                                           0x1
#define QSERDES_RX_RX_HIGHZ_HIGHRATE_RX_HIGHZ_MASK                                               0x1
#define QSERDES_RX_RX_HIGHZ_HIGHRATE_RX_HIGHZ_SHFT                                               0x0

/* RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET */
#define QSERDES_RX_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET                                            ( 0x4C0 )
#define QSERDES_RX_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_RX_AC_COUPLE_MUX_MASK                      0x20
#define QSERDES_RX_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_RX_AC_COUPLE_MUX_SHFT                      0x5
#define QSERDES_RX_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_RX_AC_COUPLE_MASK                          0x10
#define QSERDES_RX_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_RX_AC_COUPLE_SHFT                          0x4
#define QSERDES_RX_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_RX_DC_COUPLE_MUX_MASK                      0x8
#define QSERDES_RX_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_RX_DC_COUPLE_MUX_SHFT                      0x3
#define QSERDES_RX_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_RX_DC_COUPLE_MASK                          0x4
#define QSERDES_RX_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_RX_DC_COUPLE_SHFT                          0x2
#define QSERDES_RX_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_RX_DC_OFFSET_MUX_MASK                      0x2
#define QSERDES_RX_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_RX_DC_OFFSET_MUX_SHFT                      0x1
#define QSERDES_RX_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_RX_DC_OFFSET_MASK                          0x1
#define QSERDES_RX_RX_TERM_AC_BYPASS_DC_COUPLE_OFFSET_RX_DC_OFFSET_SHFT                          0x0

/* RX_EQ_GAIN1_LSB */
#define QSERDES_RX_RX_EQ_GAIN1_LSB                                                               ( 0x4C4 )
#define QSERDES_RX_RX_EQ_GAIN1_LSB_RX_EQ_GAIN1_LSB_MASK                                          0xFF
#define QSERDES_RX_RX_EQ_GAIN1_LSB_RX_EQ_GAIN1_LSB_SHFT                                          0x0

/* RX_EQ_GAIN1_MSB */
#define QSERDES_RX_RX_EQ_GAIN1_MSB                                                               ( 0x4C8 )
#define QSERDES_RX_RX_EQ_GAIN1_MSB_RX_EQ_GAIN1_MSB_MASK                                          0xFF
#define QSERDES_RX_RX_EQ_GAIN1_MSB_RX_EQ_GAIN1_MSB_SHFT                                          0x0

/* RX_EQ_GAIN2_LSB */
#define QSERDES_RX_RX_EQ_GAIN2_LSB                                                               ( 0x4CC )
#define QSERDES_RX_RX_EQ_GAIN2_LSB_RX_EQ_GAIN2_LSB_MASK                                          0xFF
#define QSERDES_RX_RX_EQ_GAIN2_LSB_RX_EQ_GAIN2_LSB_SHFT                                          0x0

/* RX_EQ_GAIN2_MSB */
#define QSERDES_RX_RX_EQ_GAIN2_MSB                                                               ( 0x4D0 )
#define QSERDES_RX_RX_EQ_GAIN2_MSB_RX_EQ_GAIN2_MSB_MASK                                          0xFF
#define QSERDES_RX_RX_EQ_GAIN2_MSB_RX_EQ_GAIN2_MSB_SHFT                                          0x0

/* RX_EQU_ADAPTOR_CNTRL1 */
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL1                                                         ( 0x4D4 )
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL1_DES_EDGE_EN_MUX_MASK                                    0x10
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL1_DES_EDGE_EN_MUX_SHFT                                    0x4
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL1_DES_EDGE_EN_MASK                                        0x8
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL1_DES_EDGE_EN_SHFT                                        0x3
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL1_RX_DCOFF_OBSV_MASK                                      0x4
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL1_RX_DCOFF_OBSV_SHFT                                      0x2
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL1_RX_EQ_BYPASS2_MASK                                      0x2
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL1_RX_EQ_BYPASS2_SHFT                                      0x1
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL1_RX_EQ_BYPASS1_MASK                                      0x1
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL1_RX_EQ_BYPASS1_SHFT                                      0x0

/* RX_EQU_ADAPTOR_CNTRL2 */
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL2                                                         ( 0x4D8 )
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL2_CORE_EQADP_FORCE_MASK_MASK                              0x40
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL2_CORE_EQADP_FORCE_MASK_SHFT                              0x6
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL2_EQADP_EN_MUX_MASK                                       0x20
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL2_EQADP_EN_MUX_SHFT                                       0x5
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL2_EQADP_EN_MASK                                           0x10
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL2_EQADP_EN_SHFT                                           0x4
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL2_EQ_GAIN2_MUX_MASK                                       0x8
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL2_EQ_GAIN2_MUX_SHFT                                       0x3
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL2_EQ_GAIN1_MUX_MASK                                       0x4
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL2_EQ_GAIN1_MUX_SHFT                                       0x2
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL2_EQDEGCAP_MASK                                           0x3
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL2_EQDEGCAP_SHFT                                           0x0

/* RX_EQU_ADAPTOR_CNTRL3 */
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL3                                                         ( 0x4DC )
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL3_KEQ_MASK                                                0x70
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL3_KEQ_SHFT                                                0x4
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL3_EQADP_FLIP_SIGN_MASK                                    0x8
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL3_EQADP_FLIP_SIGN_SHFT                                    0x3
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL3_EQADP_MODE_MASK                                         0x6
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL3_EQADP_MODE_SHFT                                         0x1
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL3_EQCODE_SEL_MASK                                         0x1
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL3_EQCODE_SEL_SHFT                                         0x0

/* RX_EQU_ADAPTOR_CNTRL4 */
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL4                                                         ( 0x4E0 )
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL4_EQCODE_FIXED_MASK                                       0xF0
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL4_EQCODE_FIXED_SHFT                                       0x4
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL4_EQCODE_MAN_VAL_MASK                                     0xF
#define QSERDES_RX_RX_EQU_ADAPTOR_CNTRL4_EQCODE_MAN_VAL_SHFT                                     0x0

/* RX_IDAC_CAL_CONFIGURATION */
#define QSERDES_RX_RX_IDAC_CAL_CONFIGURATION                                                     ( 0x4E4 )
#define QSERDES_RX_RX_IDAC_CAL_CONFIGURATION_RX_EQ_BYPASS_IDAC_CAL_MUX_MASK                      0x4
#define QSERDES_RX_RX_IDAC_CAL_CONFIGURATION_RX_EQ_BYPASS_IDAC_CAL_MUX_SHFT                      0x2
#define QSERDES_RX_RX_IDAC_CAL_CONFIGURATION_RX_EQ_BYPASS2_IDAC_CAL_MASK                         0x2
#define QSERDES_RX_RX_IDAC_CAL_CONFIGURATION_RX_EQ_BYPASS2_IDAC_CAL_SHFT                         0x1
#define QSERDES_RX_RX_IDAC_CAL_CONFIGURATION_RX_EQ_BYPASS1_IDAC_CAL_MASK                         0x1
#define QSERDES_RX_RX_IDAC_CAL_CONFIGURATION_RX_EQ_BYPASS1_IDAC_CAL_SHFT                         0x0

/* RX_IDAC_TSETTLE_LOW */
#define QSERDES_RX_RX_IDAC_TSETTLE_LOW                                                           ( 0x4E8 )
#define QSERDES_RX_RX_IDAC_TSETTLE_LOW_TSETTLE_LOW_MASK                                          0xFF
#define QSERDES_RX_RX_IDAC_TSETTLE_LOW_TSETTLE_LOW_SHFT                                          0x0

/* RX_IDAC_TSETTLE_HIGH */
#define QSERDES_RX_RX_IDAC_TSETTLE_HIGH                                                          ( 0x4EC )
#define QSERDES_RX_RX_IDAC_TSETTLE_HIGH_TSETTLE_HIGH_MASK                                        0x3
#define QSERDES_RX_RX_IDAC_TSETTLE_HIGH_TSETTLE_HIGH_SHFT                                        0x0

/* RX_IDAC_ENDSAMP_LOW */
#define QSERDES_RX_RX_IDAC_ENDSAMP_LOW                                                           ( 0x4F0 )
#define QSERDES_RX_RX_IDAC_ENDSAMP_LOW_ENDSAMP_LOW_MASK                                          0xFF
#define QSERDES_RX_RX_IDAC_ENDSAMP_LOW_ENDSAMP_LOW_SHFT                                          0x0

/* RX_IDAC_ENDSAMP_HIGH */
#define QSERDES_RX_RX_IDAC_ENDSAMP_HIGH                                                          ( 0x4F4 )
#define QSERDES_RX_RX_IDAC_ENDSAMP_HIGH_ENDSAMP_HIGH_MASK                                        0x3
#define QSERDES_RX_RX_IDAC_ENDSAMP_HIGH_ENDSAMP_HIGH_SHFT                                        0x0

/* RX_IDAC_MIDPOINT_LOW */
#define QSERDES_RX_RX_IDAC_MIDPOINT_LOW                                                          ( 0x4F8 )
#define QSERDES_RX_RX_IDAC_MIDPOINT_LOW_MIDPOINT_LOW_MASK                                        0xFF
#define QSERDES_RX_RX_IDAC_MIDPOINT_LOW_MIDPOINT_LOW_SHFT                                        0x0

/* RX_IDAC_MIDPOINT_HIGH */
#define QSERDES_RX_RX_IDAC_MIDPOINT_HIGH                                                         ( 0x4FC )
#define QSERDES_RX_RX_IDAC_MIDPOINT_HIGH_MIDPOINT_HIGH_MASK                                      0x3
#define QSERDES_RX_RX_IDAC_MIDPOINT_HIGH_MIDPOINT_HIGH_SHFT                                      0x0

/* RX_EQ_OFFSET_LSB */
#define QSERDES_RX_RX_EQ_OFFSET_LSB                                                              ( 0x500 )
#define QSERDES_RX_RX_EQ_OFFSET_LSB_RX_EQ_OFFSET_LSB_MASK                                        0xFF
#define QSERDES_RX_RX_EQ_OFFSET_LSB_RX_EQ_OFFSET_LSB_SHFT                                        0x0

/* RX_EQ_OFFSET_MSB */
#define QSERDES_RX_RX_EQ_OFFSET_MSB                                                              ( 0x504 )
#define QSERDES_RX_RX_EQ_OFFSET_MSB_RX_EQ_OFFSET_MSB_MASK                                        0xFF
#define QSERDES_RX_RX_EQ_OFFSET_MSB_RX_EQ_OFFSET_MSB_SHFT                                        0x0

/* RX_EQ_OFFSET_ADAPTOR_CNTRL1 */
#define QSERDES_RX_RX_EQ_OFFSET_ADAPTOR_CNTRL1                                                   ( 0x508 )
#define QSERDES_RX_RX_EQ_OFFSET_ADAPTOR_CNTRL1_RX_EQ_OFFSET_MUX_MASK                             0x80
#define QSERDES_RX_RX_EQ_OFFSET_ADAPTOR_CNTRL1_RX_EQ_OFFSET_MUX_SHFT                             0x7
#define QSERDES_RX_RX_EQ_OFFSET_ADAPTOR_CNTRL1_KOFFSET_MASK                                      0x70
#define QSERDES_RX_RX_EQ_OFFSET_ADAPTOR_CNTRL1_KOFFSET_SHFT                                      0x4
#define QSERDES_RX_RX_EQ_OFFSET_ADAPTOR_CNTRL1_OFFSETADP_FLIP_SIGN_MASK                          0x8
#define QSERDES_RX_RX_EQ_OFFSET_ADAPTOR_CNTRL1_OFFSETADP_FLIP_SIGN_SHFT                          0x3
#define QSERDES_RX_RX_EQ_OFFSET_ADAPTOR_CNTRL1_OFFSETADP_MODE_MASK                               0x4
#define QSERDES_RX_RX_EQ_OFFSET_ADAPTOR_CNTRL1_OFFSETADP_MODE_SHFT                               0x2
#define QSERDES_RX_RX_EQ_OFFSET_ADAPTOR_CNTRL1_OFFSETADP_EN_MUX_MASK                             0x2
#define QSERDES_RX_RX_EQ_OFFSET_ADAPTOR_CNTRL1_OFFSETADP_EN_MUX_SHFT                             0x1
#define QSERDES_RX_RX_EQ_OFFSET_ADAPTOR_CNTRL1_OFFSETADP_EN_MASK                                 0x1
#define QSERDES_RX_RX_EQ_OFFSET_ADAPTOR_CNTRL1_OFFSETADP_EN_SHFT                                 0x0

/* RX_OFFSET_ADAPTOR_CNTRL2 */
#define QSERDES_RX_RX_OFFSET_ADAPTOR_CNTRL2                                                      ( 0x50C )
#define QSERDES_RX_RX_OFFSET_ADAPTOR_CNTRL2_CORE_DCOFFSET_FORCE_MASK_MASK                        0x80
#define QSERDES_RX_RX_OFFSET_ADAPTOR_CNTRL2_CORE_DCOFFSET_FORCE_MASK_SHFT                        0x7
#define QSERDES_RX_RX_OFFSET_ADAPTOR_CNTRL2_CORE_DCOFFSET_MASK_MASK                              0x40
#define QSERDES_RX_RX_OFFSET_ADAPTOR_CNTRL2_CORE_DCOFFSET_MASK_SHFT                              0x6
#define QSERDES_RX_RX_OFFSET_ADAPTOR_CNTRL2_CORE_EQADP_MASK_MASK                                 0x20
#define QSERDES_RX_RX_OFFSET_ADAPTOR_CNTRL2_CORE_EQADP_MASK_SHFT                                 0x5
#define QSERDES_RX_RX_OFFSET_ADAPTOR_CNTRL2_OFFSETCODE_MAN_VAL_MASK                              0x1F
#define QSERDES_RX_RX_OFFSET_ADAPTOR_CNTRL2_OFFSETCODE_MAN_VAL_SHFT                              0x0

/* SIGDET_ENABLES */
#define QSERDES_RX_SIGDET_ENABLES                                                                ( 0x510 )
#define QSERDES_RX_SIGDET_ENABLES_SIGDET_FLT_BYP_MASK                                            0x10
#define QSERDES_RX_SIGDET_ENABLES_SIGDET_FLT_BYP_SHFT                                            0x4
#define QSERDES_RX_SIGDET_ENABLES_SIGDET_LP_EN_MUX_MASK                                          0x8
#define QSERDES_RX_SIGDET_ENABLES_SIGDET_LP_EN_MUX_SHFT                                          0x3
#define QSERDES_RX_SIGDET_ENABLES_SIGDET_LP_EN_MASK                                              0x4
#define QSERDES_RX_SIGDET_ENABLES_SIGDET_LP_EN_SHFT                                              0x2
#define QSERDES_RX_SIGDET_ENABLES_SIGDET_EN_MUX_MASK                                             0x2
#define QSERDES_RX_SIGDET_ENABLES_SIGDET_EN_MUX_SHFT                                             0x1
#define QSERDES_RX_SIGDET_ENABLES_SIGDET_EN_MASK                                                 0x1
#define QSERDES_RX_SIGDET_ENABLES_SIGDET_EN_SHFT                                                 0x0

/* SIGDET_CNTRL */
#define QSERDES_RX_SIGDET_CNTRL                                                                  ( 0x514 )
#define QSERDES_RX_SIGDET_CNTRL_SIGDET_BW_CTRL_MASK                                              0xF
#define QSERDES_RX_SIGDET_CNTRL_SIGDET_BW_CTRL_SHFT                                              0x0

/* SIGDET_LVL */
#define QSERDES_RX_SIGDET_LVL                                                                    ( 0x518 )
#define QSERDES_RX_SIGDET_LVL_SIGDET_UFS_MODE_MASK                                               0x20
#define QSERDES_RX_SIGDET_LVL_SIGDET_UFS_MODE_SHFT                                               0x5
#define QSERDES_RX_SIGDET_LVL_SIGDET_LVL_MUX_MASK                                                0x10
#define QSERDES_RX_SIGDET_LVL_SIGDET_LVL_MUX_SHFT                                                0x4
#define QSERDES_RX_SIGDET_LVL_SIGDET_LVL_MASK                                                    0xF
#define QSERDES_RX_SIGDET_LVL_SIGDET_LVL_SHFT                                                    0x0

/* SIGDET_DEGLITCH_CNTRL */
#define QSERDES_RX_SIGDET_DEGLITCH_CNTRL                                                         ( 0x51C )
#define QSERDES_RX_SIGDET_DEGLITCH_CNTRL_SIGDET_DEGLITCH_CTRL_MASK                               0x1E
#define QSERDES_RX_SIGDET_DEGLITCH_CNTRL_SIGDET_DEGLITCH_CTRL_SHFT                               0x1
#define QSERDES_RX_SIGDET_DEGLITCH_CNTRL_SIGDET_DEGLITCH_BYP_MASK                                0x1
#define QSERDES_RX_SIGDET_DEGLITCH_CNTRL_SIGDET_DEGLITCH_BYP_SHFT                                0x0

/* RX_BAND */
#define QSERDES_RX_RX_BAND                                                                       ( 0x520 )
#define QSERDES_RX_RX_BAND_RX_BAND_UNMODIFIED_MUX_MASK                                           0x20
#define QSERDES_RX_RX_BAND_RX_BAND_UNMODIFIED_MUX_SHFT                                           0x5
#define QSERDES_RX_RX_BAND_RX_BAND_10_HALF_MASK                                                  0x10
#define QSERDES_RX_RX_BAND_RX_BAND_10_HALF_SHFT                                                  0x4
#define QSERDES_RX_RX_BAND_RX_BAND_01_HALF_MASK                                                  0x8
#define QSERDES_RX_RX_BAND_RX_BAND_01_HALF_SHFT                                                  0x3
#define QSERDES_RX_RX_BAND_RX_BAND_MUX_MASK                                                      0x4
#define QSERDES_RX_RX_BAND_RX_BAND_MUX_SHFT                                                      0x2
#define QSERDES_RX_RX_BAND_RX_BAND_MASK                                                          0x3
#define QSERDES_RX_RX_BAND_RX_BAND_SHFT                                                          0x0

/* CDR_FREEZE_UP_DN */
#define QSERDES_RX_CDR_FREEZE_UP_DN                                                              ( 0x524 )
#define QSERDES_RX_CDR_FREEZE_UP_DN_CDR_DN_MASK                                                  0x4
#define QSERDES_RX_CDR_FREEZE_UP_DN_CDR_DN_SHFT                                                  0x2
#define QSERDES_RX_CDR_FREEZE_UP_DN_CDR_UP_MASK                                                  0x2
#define QSERDES_RX_CDR_FREEZE_UP_DN_CDR_UP_SHFT                                                  0x1
#define QSERDES_RX_CDR_FREEZE_UP_DN_CDR_FREEZE_MASK                                              0x1
#define QSERDES_RX_CDR_FREEZE_UP_DN_CDR_FREEZE_SHFT                                              0x0

/* CDR_RESET_OVERRIDE */
#define QSERDES_RX_CDR_RESET_OVERRIDE                                                            ( 0x528 )
#define QSERDES_RX_CDR_RESET_OVERRIDE_CDR_RESET_OVERRIDE_MUX_MASK                                0x2
#define QSERDES_RX_CDR_RESET_OVERRIDE_CDR_RESET_OVERRIDE_MUX_SHFT                                0x1
#define QSERDES_RX_CDR_RESET_OVERRIDE_CDR_RESET_OVERRIDE_MASK                                    0x1
#define QSERDES_RX_CDR_RESET_OVERRIDE_CDR_RESET_OVERRIDE_SHFT                                    0x0

/* RX_INTERFACE_MODE */
#define QSERDES_RX_RX_INTERFACE_MODE                                                             ( 0x52C )
#define QSERDES_RX_RX_INTERFACE_MODE_SVS_MODE_MUX_MASK                                           0x80
#define QSERDES_RX_RX_INTERFACE_MODE_SVS_MODE_MUX_SHFT                                           0x7
#define QSERDES_RX_RX_INTERFACE_MODE_SVS_MODE_MASK                                               0x40
#define QSERDES_RX_RX_INTERFACE_MODE_SVS_MODE_SHFT                                               0x6
#define QSERDES_RX_RX_INTERFACE_MODE_RX_CLOCK_EDGE_MASK                                          0x20
#define QSERDES_RX_RX_INTERFACE_MODE_RX_CLOCK_EDGE_SHFT                                          0x5
#define QSERDES_RX_RX_INTERFACE_MODE_RXINTERFACE_MODE_MASK                                       0x1F
#define QSERDES_RX_RX_INTERFACE_MODE_RXINTERFACE_MODE_SHFT                                       0x0

/* JITTER_GEN_MODE */
#define QSERDES_RX_JITTER_GEN_MODE                                                               ( 0x530 )
#define QSERDES_RX_JITTER_GEN_MODE_JITTERGENMODE_MASK                                            0xFF
#define QSERDES_RX_JITTER_GEN_MODE_JITTERGENMODE_SHFT                                            0x0

/* BUJ_AMP */
#define QSERDES_RX_BUJ_AMP                                                                       ( 0x534 )
#define QSERDES_RX_BUJ_AMP_BUJ_AMPLITUDE_MASK                                                    0x1F
#define QSERDES_RX_BUJ_AMP_BUJ_AMPLITUDE_SHFT                                                    0x0

/* SJ_AMP1 */
#define QSERDES_RX_SJ_AMP1                                                                       ( 0x538 )
#define QSERDES_RX_SJ_AMP1_SJ_AMPLITUDE1_MASK                                                    0xFF
#define QSERDES_RX_SJ_AMP1_SJ_AMPLITUDE1_SHFT                                                    0x0

/* SJ_AMP2 */
#define QSERDES_RX_SJ_AMP2                                                                       ( 0x53C )
#define QSERDES_RX_SJ_AMP2_SJ_AMPLITUDE2_MASK                                                    0xFF
#define QSERDES_RX_SJ_AMP2_SJ_AMPLITUDE2_SHFT                                                    0x0

/* SJ_PER1 */
#define QSERDES_RX_SJ_PER1                                                                       ( 0x540 )
#define QSERDES_RX_SJ_PER1_SJ_PERIOD1_MASK                                                       0xFF
#define QSERDES_RX_SJ_PER1_SJ_PERIOD1_SHFT                                                       0x0

/* SJ_PER2 */
#define QSERDES_RX_SJ_PER2                                                                       ( 0x544 )
#define QSERDES_RX_SJ_PER2_SJ_PERIOD2_MASK                                                       0xFF
#define QSERDES_RX_SJ_PER2_SJ_PERIOD2_SHFT                                                       0x0

/* BUJ_STEP_FREQ1 */
#define QSERDES_RX_BUJ_STEP_FREQ1                                                                ( 0x548 )
#define QSERDES_RX_BUJ_STEP_FREQ1_BUJ_STEPFREQ1_MASK                                             0xFF
#define QSERDES_RX_BUJ_STEP_FREQ1_BUJ_STEPFREQ1_SHFT                                             0x0

/* BUJ_STEP_FREQ2 */
#define QSERDES_RX_BUJ_STEP_FREQ2                                                                ( 0x54C )
#define QSERDES_RX_BUJ_STEP_FREQ2_BUJ_STEPFREQ2_MASK                                             0xFF
#define QSERDES_RX_BUJ_STEP_FREQ2_BUJ_STEPFREQ2_SHFT                                             0x0

/* PPM_OFFSET1 */
#define QSERDES_RX_PPM_OFFSET1                                                                   ( 0x550 )
#define QSERDES_RX_PPM_OFFSET1_PPMOFFSET1_MASK                                                   0xFF
#define QSERDES_RX_PPM_OFFSET1_PPMOFFSET1_SHFT                                                   0x0

/* PPM_OFFSET2 */
#define QSERDES_RX_PPM_OFFSET2                                                                   ( 0x554 )
#define QSERDES_RX_PPM_OFFSET2_PPMOFFSET2_MASK                                                   0xFF
#define QSERDES_RX_PPM_OFFSET2_PPMOFFSET2_SHFT                                                   0x0

/* SIGN_PPM_PERIOD1 */
#define QSERDES_RX_SIGN_PPM_PERIOD1                                                              ( 0x558 )
#define QSERDES_RX_SIGN_PPM_PERIOD1_SIGN_PPM_PERIOD1_MASK                                        0xFF
#define QSERDES_RX_SIGN_PPM_PERIOD1_SIGN_PPM_PERIOD1_SHFT                                        0x0

/* SIGN_PPM_PERIOD2 */
#define QSERDES_RX_SIGN_PPM_PERIOD2                                                              ( 0x55C )
#define QSERDES_RX_SIGN_PPM_PERIOD2_SIGN_PPM_PERIOD2_MASK                                        0xFF
#define QSERDES_RX_SIGN_PPM_PERIOD2_SI3GN_PPM_PERIOD2_SHFT                                        0x0

/* SSC_CTRL */
#define QSERDES_RX_SSC_CTRL                                                                      ( 0x560 )
#define QSERDES_RX_SSC_CTRL_SSCDIRECTION_MASK                                                    0x2
#define QSERDES_RX_SSC_CTRL_SSCDIRECTION_SHFT                                                    0x1
#define QSERDES_RX_SSC_CTRL_SSCON_MASK                                                           0x1
#define QSERDES_RX_SSC_CTRL_SSCON_SHFT                                                           0x0

/* SSC_COUNT1 */
#define QSERDES_RX_SSC_COUNT1                                                                    ( 0x564 )
#define QSERDES_RX_SSC_COUNT1_SSCCOUNT1_MASK                                                     0xFF
#define QSERDES_RX_SSC_COUNT1_SSCCOUNT1_SHFT                                                     0x0

/* SSC_COUNT2 */
#define QSERDES_RX_SSC_COUNT2                                                                    ( 0x568 )
#define QSERDES_RX_SSC_COUNT2_SSCCOUNT2_MASK                                                     0xFF
#define QSERDES_RX_SSC_COUNT2_SSCCOUNT2_SHFT                                                     0x0

/* RX_ALOG_INTF_OBSV_CNTL */
#define QSERDES_RX_RX_ALOG_INTF_OBSV_CNTL                                                        ( 0x56C )
#define QSERDES_RX_RX_ALOG_INTF_OBSV_CNTL_RX_ALOG_INTF_OBSV_CNTL_MASK                            0x1F
#define QSERDES_RX_RX_ALOG_INTF_OBSV_CNTL_RX_ALOG_INTF_OBSV_CNTL_SHFT                            0x0

/* RX_PWM_ENABLE_AND_DATA */
#define QSERDES_RX_RX_PWM_ENABLE_AND_DATA                                                        ( 0x570 )
#define QSERDES_RX_RX_PWM_ENABLE_AND_DATA_RX_PWM_GEAR_MUX_MASK                                   0x20
#define QSERDES_RX_RX_PWM_ENABLE_AND_DATA_RX_PWM_GEAR_MUX_SHFT                                   0x5
#define QSERDES_RX_RX_PWM_ENABLE_AND_DATA_RX_PWM_GEAR_MASK                                       0x1C
#define QSERDES_RX_RX_PWM_ENABLE_AND_DATA_RX_PWM_GEAR_SHFT                                       0x2
#define QSERDES_RX_RX_PWM_ENABLE_AND_DATA_RX_PWM_EN_MUX_MASK                                     0x2
#define QSERDES_RX_RX_PWM_ENABLE_AND_DATA_RX_PWM_EN_MUX_SHFT                                     0x1
#define QSERDES_RX_RX_PWM_ENABLE_AND_DATA_RX_PWM_EN_MASK                                         0x1
#define QSERDES_RX_RX_PWM_ENABLE_AND_DATA_RX_PWM_EN_SHFT                                         0x0

/* RX_PWM_GEAR1_TIMEOUT_COUNT */
#define QSERDES_RX_RX_PWM_GEAR1_TIMEOUT_COUNT                                                    ( 0x574 )
#define QSERDES_RX_RX_PWM_GEAR1_TIMEOUT_COUNT_RX_PWM_GEAR1_COUNT_MASK                            0xFF
#define QSERDES_RX_RX_PWM_GEAR1_TIMEOUT_COUNT_RX_PWM_GEAR1_COUNT_SHFT                            0x0

/* RX_PWM_GEAR2_TIMEOUT_COUNT */
#define QSERDES_RX_RX_PWM_GEAR2_TIMEOUT_COUNT                                                    ( 0x578 )
#define QSERDES_RX_RX_PWM_GEAR2_TIMEOUT_COUNT_RX_PWM_GEAR2_COUNT_MASK                            0xFF
#define QSERDES_RX_RX_PWM_GEAR2_TIMEOUT_COUNT_RX_PWM_GEAR2_COUNT_SHFT                            0x0

/* RX_PWM_GEAR3_TIMEOUT_COUNT */
#define QSERDES_RX_RX_PWM_GEAR3_TIMEOUT_COUNT                                                    ( 0x57C )
#define QSERDES_RX_RX_PWM_GEAR3_TIMEOUT_COUNT_RX_PWM_GEAR3_COUNT_MASK                            0xFF
#define QSERDES_RX_RX_PWM_GEAR3_TIMEOUT_COUNT_RX_PWM_GEAR3_COUNT_SHFT                            0x0

/* RX_PWM_GEAR4_TIMEOUT_COUNT */
#define QSERDES_RX_RX_PWM_GEAR4_TIMEOUT_COUNT                                                    ( 0x580 )
#define QSERDES_RX_RX_PWM_GEAR4_TIMEOUT_COUNT_RX_PWM_GEAR4_COUNT_MASK                            0xFF
#define QSERDES_RX_RX_PWM_GEAR4_TIMEOUT_COUNT_RX_PWM_GEAR4_COUNT_SHFT                            0x0

/* PI_CTRL1 */
#define QSERDES_RX_PI_CTRL1                                                                      ( 0x584 )
#define QSERDES_RX_PI_CTRL1_PI_CTRL1_STATUS_MASK                                                 0xFF
#define QSERDES_RX_PI_CTRL1_PI_CTRL1_STATUS_SHFT                                                 0x0

/* PI_CTRL2 */
#define QSERDES_RX_PI_CTRL2                                                                      ( 0x588 )
#define QSERDES_RX_PI_CTRL2_PI_CTRL2_STATUS_MASK                                                 0xFF
#define QSERDES_RX_PI_CTRL2_PI_CTRL2_STATUS_SHFT                                                 0x0

/* PI_QUAD */
#define QSERDES_RX_PI_QUAD                                                                       ( 0x58C )
#define QSERDES_RX_PI_QUAD_RSVD_STATUS_MASK                                                      0xFC
#define QSERDES_RX_PI_QUAD_RSVD_STATUS_SHFT                                                      0x2
#define QSERDES_RX_PI_QUAD_PI_QUAD_STATUS_MASK                                                   0x3
#define QSERDES_RX_PI_QUAD_PI_QUAD_STATUS_SHFT                                                   0x0

/* IDATA1 */
#define QSERDES_RX_IDATA1                                                                        ( 0x590 )
#define QSERDES_RX_IDATA1_IDATA1_STATUS_MASK                                                     0xFF
#define QSERDES_RX_IDATA1_IDATA1_STATUS_SHFT                                                     0x0

/* IDATA2 */
#define QSERDES_RX_IDATA2                                                                        ( 0x594 )
#define QSERDES_RX_IDATA2_IDATA2_STATUS_MASK                                                     0xFF
#define QSERDES_RX_IDATA2_IDATA2_STATUS_SHFT                                                     0x0

/* AUX_DATA1 */
#define QSERDES_RX_AUX_DATA1                                                                     ( 0x598 )
#define QSERDES_RX_AUX_DATA1_AUXDATA1_STATUS_MASK                                                0xFF
#define QSERDES_RX_AUX_DATA1_AUXDATA1_STATUS_SHFT                                                0x0

/* AUX_DATA2 */
#define QSERDES_RX_AUX_DATA2                                                                     ( 0x59C )
#define QSERDES_RX_AUX_DATA2_RSVD_STATUS_MASK                                                    0xFC
#define QSERDES_RX_AUX_DATA2_RSVD_STATUS_SHFT                                                    0x2
#define QSERDES_RX_AUX_DATA2_AUXDATA2_STATUS_MASK                                                0x3
#define QSERDES_RX_AUX_DATA2_AUXDATA2_STATUS_SHFT                                                0x0

/* AC_JTAG_OUTP */
#define QSERDES_RX_AC_JTAG_OUTP                                                                  ( 0x5A0 )
#define QSERDES_RX_AC_JTAG_OUTP_RSVD_STATUS_MASK                                                 0xFE
#define QSERDES_RX_AC_JTAG_OUTP_RSVD_STATUS_SHFT                                                 0x1
#define QSERDES_RX_AC_JTAG_OUTP_AC_JTAG_OUTP_STATUS_MASK                                         0x1
#define QSERDES_RX_AC_JTAG_OUTP_AC_JTAG_OUTP_STATUS_SHFT                                         0x0

/* AC_JTAG_OUTN */
#define QSERDES_RX_AC_JTAG_OUTN                                                                  ( 0x5A4 )
#define QSERDES_RX_AC_JTAG_OUTN_RSVD_STATUS_MASK                                                 0xFE
#define QSERDES_RX_AC_JTAG_OUTN_RSVD_STATUS_SHFT                                                 0x1
#define QSERDES_RX_AC_JTAG_OUTN_AC_JTAG_OUTN_STATUS_MASK                                         0x1
#define QSERDES_RX_AC_JTAG_OUTN_AC_JTAG_OUTN_STATUS_SHFT                                         0x0

/* RX_SIGDET */
#define QSERDES_RX_RX_SIGDET                                                                     ( 0x5A8 )
#define QSERDES_RX_RX_SIGDET_RSVD_STATUS_MASK                                                    0xFC
#define QSERDES_RX_RX_SIGDET_RSVD_STATUS_SHFT                                                    0x2
#define QSERDES_RX_RX_SIGDET_RX_SIGDET_OUT_STATUS_MASK                                           0x3
#define QSERDES_RX_RX_SIGDET_RX_SIGDET_OUT_STATUS_SHFT                                           0x0

/* RX_VDCOFF */
#define QSERDES_RX_RX_VDCOFF                                                                     ( 0x5AC )
#define QSERDES_RX_RX_VDCOFF_RSVD_STATUS_MASK                                                    0xFE
#define QSERDES_RX_RX_VDCOFF_RSVD_STATUS_SHFT                                                    0x1
#define QSERDES_RX_RX_VDCOFF_VDCOFF_STATUS_MASK                                                  0x1
#define QSERDES_RX_RX_VDCOFF_VDCOFF_STATUS_SHFT                                                  0x0

/* IDAC_CAL_ON */
#define QSERDES_RX_IDAC_CAL_ON                                                                   ( 0x5B0 )
#define QSERDES_RX_IDAC_CAL_ON_RSVD_STATUS_MASK                                                  0xFC
#define QSERDES_RX_IDAC_CAL_ON_RSVD_STATUS_SHFT                                                  0x2
#define QSERDES_RX_IDAC_CAL_ON_IDAC_CAL_DONE_STATUS_MASK                                         0x2
#define QSERDES_RX_IDAC_CAL_ON_IDAC_CAL_DONE_STATUS_SHFT                                         0x1
#define QSERDES_RX_IDAC_CAL_ON_IDAC_CAL_ON_STATUS_MASK                                           0x1
#define QSERDES_RX_IDAC_CAL_ON_IDAC_CAL_ON_STATUS_SHFT                                           0x0

/* IDAC_STATUS_I */
#define QSERDES_RX_IDAC_STATUS_I                                                                 ( 0x5B4 )
#define QSERDES_RX_IDAC_STATUS_I_IDAC_I_STATUS_MASK                                              0x1F
#define QSERDES_RX_IDAC_STATUS_I_IDAC_I_STATUS_SHFT                                              0x0

/* IDAC_STATUS_IBAR */
#define QSERDES_RX_IDAC_STATUS_IBAR                                                              ( 0x5B8 )
#define QSERDES_RX_IDAC_STATUS_IBAR_IDAC_IB_STATUS_MASK                                          0x1F
#define QSERDES_RX_IDAC_STATUS_IBAR_IDAC_IB_STATUS_SHFT                                          0x0

/* IDAC_STATUS_Q */
#define QSERDES_RX_IDAC_STATUS_Q                                                                 ( 0x5BC )
#define QSERDES_RX_IDAC_STATUS_Q_IDAC_Q_STATUS_MASK                                              0x1F
#define QSERDES_RX_IDAC_STATUS_Q_IDAC_Q_STATUS_SHFT                                              0x0

/* IDAC_STATUS_QBAR */
#define QSERDES_RX_IDAC_STATUS_QBAR                                                              ( 0x5C0 )
#define QSERDES_RX_IDAC_STATUS_QBAR_IDAC_QB_STATUS_MASK                                          0x1F
#define QSERDES_RX_IDAC_STATUS_QBAR_IDAC_QB_STATUS_SHFT                                          0x0

/* IDAC_STATUS_A */
#define QSERDES_RX_IDAC_STATUS_A                                                                 ( 0x5C4 )
#define QSERDES_RX_IDAC_STATUS_A_IDAC_A_STATUS_MASK                                              0x1F
#define QSERDES_RX_IDAC_STATUS_A_IDAC_A_STATUS_SHFT                                              0x0

/* IDAC_STATUS_ABAR */
#define QSERDES_RX_IDAC_STATUS_ABAR                                                              ( 0x5C8 )
#define QSERDES_RX_IDAC_STATUS_ABAR_IDAC_AB_STATUS_MASK                                          0x1F
#define QSERDES_RX_IDAC_STATUS_ABAR_IDAC_AB_STATUS_SHFT                                          0x0

/* CALST_STATUS_I */
#define QSERDES_RX_CALST_STATUS_I                                                                ( 0x5CC )
#define QSERDES_RX_CALST_STATUS_I_RSVD_STATUS_MASK                                               0xC0
#define QSERDES_RX_CALST_STATUS_I_RSVD_STATUS_SHFT                                               0x6
#define QSERDES_RX_CALST_STATUS_I_CALST_IB_STATUS_MASK                                           0x38
#define QSERDES_RX_CALST_STATUS_I_CALST_IB_STATUS_SHFT                                           0x3
#define QSERDES_RX_CALST_STATUS_I_CALST_I_STATUS_MASK                                            0x7
#define QSERDES_RX_CALST_STATUS_I_CALST_I_STATUS_SHFT                                            0x0

/* CALST_STATUS_Q */
#define QSERDES_RX_CALST_STATUS_Q                                                                ( 0x5D0 )
#define QSERDES_RX_CALST_STATUS_Q_RSVD_STATUS_MASK                                               0xC0
#define QSERDES_RX_CALST_STATUS_Q_RSVD_STATUS_SHFT                                               0x6
#define QSERDES_RX_CALST_STATUS_Q_CALST_QB_STATUS_MASK                                           0x38
#define QSERDES_RX_CALST_STATUS_Q_CALST_QB_STATUS_SHFT                                           0x3
#define QSERDES_RX_CALST_STATUS_Q_CALST_Q_STATUS_MASK                                            0x7
#define QSERDES_RX_CALST_STATUS_Q_CALST_Q_STATUS_SHFT                                            0x0

/* CALST_STATUS_A */
#define QSERDES_RX_CALST_STATUS_A                                                                ( 0x5D4 )
#define QSERDES_RX_CALST_STATUS_A_RSVD_STATUS_MASK                                               0xC0
#define QSERDES_RX_CALST_STATUS_A_RSVD_STATUS_SHFT                                               0x6
#define QSERDES_RX_CALST_STATUS_A_CALST_AB_STATUS_MASK                                           0x38
#define QSERDES_RX_CALST_STATUS_A_CALST_AB_STATUS_SHFT                                           0x3
#define QSERDES_RX_CALST_STATUS_A_CALST_A_STATUS_MASK                                            0x7
#define QSERDES_RX_CALST_STATUS_A_CALST_A_STATUS_SHFT                                            0x0

/* RX_ALOG_INTF_OBSV */
#define QSERDES_RX_RX_ALOG_INTF_OBSV                                                             ( 0x5D8 )
#define QSERDES_RX_RX_ALOG_INTF_OBSV_RX_ALOG_INTF_OBSV_STATUS_MASK                               0xFF
#define QSERDES_RX_RX_ALOG_INTF_OBSV_RX_ALOG_INTF_OBSV_STATUS_SHFT                               0x0

/* READ_EQCODE */
#define QSERDES_RX_READ_EQCODE                                                                   ( 0x5DC )
#define QSERDES_RX_READ_EQCODE_READ_EQCODE_STATUS_MASK                                           0xFF
#define QSERDES_RX_READ_EQCODE_READ_EQCODE_STATUS_SHFT                                           0x0

/* READ_OFFSETCODE */
#define QSERDES_RX_READ_OFFSETCODE                                                               ( 0x5E0 )
#define QSERDES_RX_READ_OFFSETCODE_RSVD_STATUS_MASK                                              0xE0
#define QSERDES_RX_READ_OFFSETCODE_RSVD_STATUS_SHFT                                              0x5
#define QSERDES_RX_READ_OFFSETCODE_READ_OFFSETCODE_STATUS_MASK                                   0x1F
#define QSERDES_RX_READ_OFFSETCODE_READ_OFFSETCODE_STATUS_SHFT                                   0x0

/* IA_ERROR_COUNTER_LOW */
#define QSERDES_RX_IA_ERROR_COUNTER_LOW                                                          ( 0x5E4 )
#define QSERDES_RX_IA_ERROR_COUNTER_LOW_IACOMPARE_COUNT_LOW_MASK                                 0xFF
#define QSERDES_RX_IA_ERROR_COUNTER_LOW_IACOMPARE_COUNT_LOW_SHFT                                 0x0

/* IA_ERROR_COUNTER_HIGH */
#define QSERDES_RX_IA_ERROR_COUNTER_HIGH                                                         ( 0x5E8 )
#define QSERDES_RX_IA_ERROR_COUNTER_HIGH_IACOMPARE_COUNT_HIGH_MASK                               0xFF
#define QSERDES_RX_IA_ERROR_COUNTER_HIGH_IACOMPARE_COUNT_HIGH_SHFT                               0x0

/* PCIE_USB3_PCS_MISC */

/* DEBUG_BUS_BYTE0_INDEX */
#define PCIE_USB3_PCS_MISC_DEBUG_BUS_BYTE0_INDEX                                                   ( 0x600 )
#define PCIE_USB3_PCS_MISC_DEBUG_BUS_BYTE0_INDEX_DEBUG_BUS_IDX0_MASK                               0xF
#define PCIE_USB3_PCS_MISC_DEBUG_BUS_BYTE0_INDEX_DEBUG_BUS_IDX0_SHFT                               0x0

/* DEBUG_BUS_BYTE1_INDEX */
#define PCIE_USB3_PCS_MISC_DEBUG_BUS_BYTE1_INDEX                                                   ( 0x604 )
#define PCIE_USB3_PCS_MISC_DEBUG_BUS_BYTE1_INDEX_DEBUG_BUS_IDX1_MASK                               0xF
#define PCIE_USB3_PCS_MISC_DEBUG_BUS_BYTE1_INDEX_DEBUG_BUS_IDX1_SHFT                               0x0

/* DEBUG_BUS_BYTE2_INDEX */
#define PCIE_USB3_PCS_MISC_DEBUG_BUS_BYTE2_INDEX                                                   ( 0x608 )
#define PCIE_USB3_PCS_MISC_DEBUG_BUS_BYTE2_INDEX_DEBUG_BUS_IDX2_MASK                               0xF
#define PCIE_USB3_PCS_MISC_DEBUG_BUS_BYTE2_INDEX_DEBUG_BUS_IDX2_SHFT                               0x0

/* DEBUG_BUS_BYTE3_INDEX */
#define PCIE_USB3_PCS_MISC_DEBUG_BUS_BYTE3_INDEX                                                   ( 0x60C )
#define PCIE_USB3_PCS_MISC_DEBUG_BUS_BYTE3_INDEX_DEBUG_BUS_IDX3_MASK                               0xF
#define PCIE_USB3_PCS_MISC_DEBUG_BUS_BYTE3_INDEX_DEBUG_BUS_IDX3_SHFT                               0x0

/* PLACEHOLDER_STATUS */
#define PCIE_USB3_PCS_MISC_PLACEHOLDER_STATUS                                                      ( 0x610 )
#define PCIE_USB3_PCS_MISC_PLACEHOLDER_STATUS_PLACEHOLDER_STATUS_MASK                              0x1
#define PCIE_USB3_PCS_MISC_PLACEHOLDER_STATUS_PLACEHOLDER_STATUS_SHFT                              0x0

/* DEBUG_BUS_0_STATUS */
#define PCIE_USB3_PCS_MISC_DEBUG_BUS_0_STATUS                                                      ( 0x614 )
#define PCIE_USB3_PCS_MISC_DEBUG_BUS_0_STATUS_DEBUG_BUS_0_STATUS_MASK                              0xFF
#define PCIE_USB3_PCS_MISC_DEBUG_BUS_0_STATUS_DEBUG_BUS_0_STATUS_SHFT                              0x0

/* DEBUG_BUS_1_STATUS */
#define PCIE_USB3_PCS_MISC_DEBUG_BUS_1_STATUS                                                      ( 0x618 )
#define PCIE_USB3_PCS_MISC_DEBUG_BUS_1_STATUS_DEBUG_BUS_1_STATUS_MASK                              0xFF
#define PCIE_USB3_PCS_MISC_DEBUG_BUS_1_STATUS_DEBUG_BUS_1_STATUS_SHFT                              0x0

/* DEBUG_BUS_2_STATUS */
#define PCIE_USB3_PCS_MISC_DEBUG_BUS_2_STATUS                                                      ( 0x61C )
#define PCIE_USB3_PCS_MISC_DEBUG_BUS_2_STATUS_DEBUG_BUS_2_STATUS_MASK                              0xFF
#define PCIE_USB3_PCS_MISC_DEBUG_BUS_2_STATUS_DEBUG_BUS_2_STATUS_SHFT                              0x0

/* DEBUG_BUS_3_STATUS */
#define PCIE_USB3_PCS_MISC_DEBUG_BUS_3_STATUS                                                      ( 0x620 )
#define PCIE_USB3_PCS_MISC_DEBUG_BUS_3_STATUS_DEBUG_BUS_3_STATUS_MASK                              0xFF
#define PCIE_USB3_PCS_MISC_DEBUG_BUS_3_STATUS_DEBUG_BUS_3_STATUS_SHFT                              0x0

/* OSC_DTCT_STATUS */
#define PCIE_USB3_PCS_MISC_OSC_DTCT_STATUS                                                         ( 0x624 )
#define PCIE_USB3_PCS_MISC_OSC_DTCT_STATUS_OSC_DTCT_STATUS_MASK                                    0x1
#define PCIE_USB3_PCS_MISC_OSC_DTCT_STATUS_OSC_DTCT_STATUS_SHFT                                    0x0

/* OSC_DTCT_CONFIG1 */
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG1                                                        ( 0x628 )
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG1_OSC_DTCT_TEST_ANALOG_EN_MASK                           0x40
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG1_OSC_DTCT_TEST_ANALOG_EN_SHFT                           0x6
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG1_OSC_DTCT_OSC_CLK_DIG_DIV_SEL_MASK                      0x30
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG1_OSC_DTCT_OSC_CLK_DIG_DIV_SEL_SHFT                      0x4
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG1_OSC_DTCT_FREQ_MATCH_MODE_MASK                          0xC
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG1_OSC_DTCT_FREQ_MATCH_MODE_SHFT                          0x2
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG1_OSC_DTCT_MODE_MASK                                     0x2
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG1_OSC_DTCT_MODE_SHFT                                     0x1
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG1_RSVD2_MASK                                             0x1
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG1_RSVD2_SHFT                                             0x0

/* OSC_DTCT_CONFIG2 */
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG2                                                        ( 0x62C )
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG2_OSC_DTCT_OSC_CLK_ANA_DIV_SEL_MASK                      0xC0
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG2_OSC_DTCT_OSC_CLK_ANA_DIV_SEL_SHFT                      0x6
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG2_OSC_DTCT_INT_CLK_DIV_SEL_MASK                          0x3F
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG2_OSC_DTCT_INT_CLK_DIV_SEL_SHFT                          0x0

/* OSC_DTCT_CONFIG3 */
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG3                                                        ( 0x630 )
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG3_OSC_DTCT_STARTUP_TIME_MASK                             0xFF
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG3_OSC_DTCT_STARTUP_TIME_SHFT                             0x0

/* OSC_DTCT_CONFIG4 */
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG4                                                        ( 0x634 )
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG4_OSC_DTCT_COUNT_MATCH_VAL_7_0_MASK                      0xFF
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG4_OSC_DTCT_COUNT_MATCH_VAL_7_0_SHFT                      0x0

/* OSC_DTCT_CONFIG5 */
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG5                                                        ( 0x638 )
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG5_OSC_DTCT_COUNT_MATCH_RANGE_MASK                        0x3F
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG5_OSC_DTCT_COUNT_MATCH_RANGE_SHFT                        0x0

/* OSC_DTCT_CONFIG6 */
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG6                                                        ( 0x63C )
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG6_OSC_DTCT_LOCK_EVAL_TIME_MASK                           0xFF
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG6_OSC_DTCT_LOCK_EVAL_TIME_SHFT                           0x0

/* OSC_DTCT_CONFIG7 */
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG7                                                        ( 0x640 )
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG7_OSC_DTCT_COUNT_MATCH_VAL_8_8_MASK                      0x1
#define PCIE_USB3_PCS_MISC_OSC_DTCT_CONFIG7_OSC_DTCT_COUNT_MATCH_VAL_8_8_SHFT                      0x0


/*============================================================================
     PCIE_USB3_PCS
============================================================================*/

/* SW_RESET */
#define PCIE_USB3_PCS_SW_RESET                                                                                   ( 0x800 )
#define PCIE_USB3_PCS_SW_RESET_RSVD_MASK                                                                         0xFE
#define PCIE_USB3_PCS_SW_RESET_RSVD_SHFT                                                                         0x1
#define PCIE_USB3_PCS_SW_RESET_SW_RESET_MASK                                                                     0x1
#define PCIE_USB3_PCS_SW_RESET_SW_RESET_SHFT                                                                     0x0

/* POWER_DOWN_CONTROL */
#define PCIE_USB3_PCS_POWER_DOWN_CONTROL                                                                         ( 0x804 )
#define PCIE_USB3_PCS_POWER_DOWN_CONTROL_RSVD_MASK                                                               0xFC
#define PCIE_USB3_PCS_POWER_DOWN_CONTROL_RSVD_SHFT                                                               0x2
#define PCIE_USB3_PCS_POWER_DOWN_CONTROL_REFCLK_DRV_DSBL_B_MASK                                                  0x2
#define PCIE_USB3_PCS_POWER_DOWN_CONTROL_REFCLK_DRV_DSBL_B_SHFT                                                  0x1
#define PCIE_USB3_PCS_POWER_DOWN_CONTROL_SW_PWRDN_B_MASK                                                         0x1
#define PCIE_USB3_PCS_POWER_DOWN_CONTROL_SW_PWRDN_B_SHFT                                                         0x0

/* START_CONTROL */
#define PCIE_USB3_PCS_START_CONTROL                                                                              ( 0x808 )
#define PCIE_USB3_PCS_START_CONTROL_FAST_CAL_DIS_MASK                                                            0x80
#define PCIE_USB3_PCS_START_CONTROL_FAST_CAL_DIS_SHFT                                                            0x7
#define PCIE_USB3_PCS_START_CONTROL_RSVD_MASK                                                                    0x78
#define PCIE_USB3_PCS_START_CONTROL_RSVD_SHFT                                                                    0x3
#define PCIE_USB3_PCS_START_CONTROL_SYSCLK_EN_MASK                                                               0x4
#define PCIE_USB3_PCS_START_CONTROL_SYSCLK_EN_SHFT                                                               0x2
#define PCIE_USB3_PCS_START_CONTROL_PCS_START_MASK                                                               0x2
#define PCIE_USB3_PCS_START_CONTROL_PCS_START_SHFT                                                               0x1
#define PCIE_USB3_PCS_START_CONTROL_SERDES_START_MASK                                                            0x1
#define PCIE_USB3_PCS_START_CONTROL_SERDES_START_SHFT                                                            0x0

/* TXMGN_V0 */
#define PCIE_USB3_PCS_TXMGN_V0                                                                                   ( 0x80C )
#define PCIE_USB3_PCS_TXMGN_V0_TX_LARGE_AMP_V0_MASK                                                              0x80
#define PCIE_USB3_PCS_TXMGN_V0_TX_LARGE_AMP_V0_SHFT                                                              0x7
#define PCIE_USB3_PCS_TXMGN_V0_TX_IDLE_LVL_V0_MASK                                                               0x60
#define PCIE_USB3_PCS_TXMGN_V0_TX_IDLE_LVL_V0_SHFT                                                               0x5
#define PCIE_USB3_PCS_TXMGN_V0_TXMGN_V0_MASK                                                                     0x1F
#define PCIE_USB3_PCS_TXMGN_V0_TXMGN_V0_SHFT                                                                     0x0

/* TXMGN_V1 */
#define PCIE_USB3_PCS_TXMGN_V1                                                                                   ( 0x810 )
#define PCIE_USB3_PCS_TXMGN_V1_TX_LARGE_AMP_V1_MASK                                                              0x80
#define PCIE_USB3_PCS_TXMGN_V1_TX_LARGE_AMP_V1_SHFT                                                              0x7
#define PCIE_USB3_PCS_TXMGN_V1_TX_IDLE_LVL_V1_MASK                                                               0x60
#define PCIE_USB3_PCS_TXMGN_V1_TX_IDLE_LVL_V1_SHFT                                                               0x5
#define PCIE_USB3_PCS_TXMGN_V1_TXMGN_V1_MASK                                                                     0x1F
#define PCIE_USB3_PCS_TXMGN_V1_TXMGN_V1_SHFT                                                                     0x0

/* TXMGN_V2 */
#define PCIE_USB3_PCS_TXMGN_V2                                                                                   ( 0x814 )
#define PCIE_USB3_PCS_TXMGN_V2_TX_LARGE_AMP_V2_MASK                                                              0x80
#define PCIE_USB3_PCS_TXMGN_V2_TX_LARGE_AMP_V2_SHFT                                                              0x7
#define PCIE_USB3_PCS_TXMGN_V2_TX_IDLE_LVL_V2_MASK                                                               0x60
#define PCIE_USB3_PCS_TXMGN_V2_TX_IDLE_LVL_V2_SHFT                                                               0x5
#define PCIE_USB3_PCS_TXMGN_V2_TXMGN_V2_MASK                                                                     0x1F
#define PCIE_USB3_PCS_TXMGN_V2_TXMGN_V2_SHFT                                                                     0x0

/* TXMGN_V3 */
#define PCIE_USB3_PCS_TXMGN_V3                                                                                   ( 0x818 )
#define PCIE_USB3_PCS_TXMGN_V3_TX_LARGE_AMP_V3_MASK                                                              0x80
#define PCIE_USB3_PCS_TXMGN_V3_TX_LARGE_AMP_V3_SHFT                                                              0x7
#define PCIE_USB3_PCS_TXMGN_V3_TX_IDLE_LVL_V3_MASK                                                               0x60
#define PCIE_USB3_PCS_TXMGN_V3_TX_IDLE_LVL_V3_SHFT                                                               0x5
#define PCIE_USB3_PCS_TXMGN_V3_TXMGN_V3_MASK                                                                     0x1F
#define PCIE_USB3_PCS_TXMGN_V3_TXMGN_V3_SHFT                                                                     0x0

/* TXMGN_V4 */
#define PCIE_USB3_PCS_TXMGN_V4                                                                                   ( 0x81C )
#define PCIE_USB3_PCS_TXMGN_V4_TX_LARGE_AMP_V4_MASK                                                              0x80
#define PCIE_USB3_PCS_TXMGN_V4_TX_LARGE_AMP_V4_SHFT                                                              0x7
#define PCIE_USB3_PCS_TXMGN_V4_TX_IDLE_LVL_V4_MASK                                                               0x60
#define PCIE_USB3_PCS_TXMGN_V4_TX_IDLE_LVL_V4_SHFT                                                               0x5
#define PCIE_USB3_PCS_TXMGN_V4_TXMGN_V4_MASK                                                                     0x1F
#define PCIE_USB3_PCS_TXMGN_V4_TXMGN_V4_SHFT                                                                     0x0

/* TXMGN_LS */
#define PCIE_USB3_PCS_TXMGN_LS                                                                                   ( 0x820 )
#define PCIE_USB3_PCS_TXMGN_LS_TX_LARGE_AMP_LS_MASK                                                              0x80
#define PCIE_USB3_PCS_TXMGN_LS_TX_LARGE_AMP_LS_SHFT                                                              0x7
#define PCIE_USB3_PCS_TXMGN_LS_TX_IDLE_LVL_LS_MASK                                                               0x60
#define PCIE_USB3_PCS_TXMGN_LS_TX_IDLE_LVL_LS_SHFT                                                               0x5
#define PCIE_USB3_PCS_TXMGN_LS_TXMGN_LS_MASK                                                                     0x1F
#define PCIE_USB3_PCS_TXMGN_LS_TXMGN_LS_SHFT                                                                     0x0

/* TXDEEMPH_M6DB_V0 */
#define PCIE_USB3_PCS_TXDEEMPH_M6DB_V0                                                                           ( 0x824 )
#define PCIE_USB3_PCS_TXDEEMPH_M6DB_V0_TXDEEMPH_M6DB_V0_MASK                                                     0x1F
#define PCIE_USB3_PCS_TXDEEMPH_M6DB_V0_TXDEEMPH_M6DB_V0_SHFT                                                     0x0

/* TXDEEMPH_M3P5DB_V0 */
#define PCIE_USB3_PCS_TXDEEMPH_M3P5DB_V0                                                                         ( 0x828 )
#define PCIE_USB3_PCS_TXDEEMPH_M3P5DB_V0_TXDEEMPH_M3P5DB_V0_MASK                                                 0x1F
#define PCIE_USB3_PCS_TXDEEMPH_M3P5DB_V0_TXDEEMPH_M3P5DB_V0_SHFT                                                 0x0

/* TXDEEMPH_M6DB_V1 */
#define PCIE_USB3_PCS_TXDEEMPH_M6DB_V1                                                                           ( 0x82C )
#define PCIE_USB3_PCS_TXDEEMPH_M6DB_V1_TXDEEMPH_M6DB_V1_MASK                                                     0x1F
#define PCIE_USB3_PCS_TXDEEMPH_M6DB_V1_TXDEEMPH_M6DB_V1_SHFT                                                     0x0

/* TXDEEMPH_M3P5DB_V1 */
#define PCIE_USB3_PCS_TXDEEMPH_M3P5DB_V1                                                                         ( 0x830 )
#define PCIE_USB3_PCS_TXDEEMPH_M3P5DB_V1_TXDEEMPH_M3P5DB_V1_MASK                                                 0x1F
#define PCIE_USB3_PCS_TXDEEMPH_M3P5DB_V1_TXDEEMPH_M3P5DB_V1_SHFT                                                 0x0

/* TXDEEMPH_M6DB_V2 */
#define PCIE_USB3_PCS_TXDEEMPH_M6DB_V2                                                                           ( 0x834 )
#define PCIE_USB3_PCS_TXDEEMPH_M6DB_V2_TXDEEMPH_M6DB_V2_MASK                                                     0x1F
#define PCIE_USB3_PCS_TXDEEMPH_M6DB_V2_TXDEEMPH_M6DB_V2_SHFT                                                     0x0

/* TXDEEMPH_M3P5DB_V2 */
#define PCIE_USB3_PCS_TXDEEMPH_M3P5DB_V2                                                                         ( 0x838 )
#define PCIE_USB3_PCS_TXDEEMPH_M3P5DB_V2_TXDEEMPH_M3P5DB_V2_MASK                                                 0x1F
#define PCIE_USB3_PCS_TXDEEMPH_M3P5DB_V2_TXDEEMPH_M3P5DB_V2_SHFT                                                 0x0

/* TXDEEMPH_M6DB_V3 */
#define PCIE_USB3_PCS_TXDEEMPH_M6DB_V3                                                                           ( 0x83C )
#define PCIE_USB3_PCS_TXDEEMPH_M6DB_V3_TXDEEMPH_M6DB_V3_MASK                                                     0x1F
#define PCIE_USB3_PCS_TXDEEMPH_M6DB_V3_TXDEEMPH_M6DB_V3_SHFT                                                     0x0

/* TXDEEMPH_M3P5DB_V3 */
#define PCIE_USB3_PCS_TXDEEMPH_M3P5DB_V3                                                                         ( 0x840 )
#define PCIE_USB3_PCS_TXDEEMPH_M3P5DB_V3_TXDEEMPH_M3P5DB_V3_MASK                                                 0x1F
#define PCIE_USB3_PCS_TXDEEMPH_M3P5DB_V3_TXDEEMPH_M3P5DB_V3_SHFT                                                 0x0

/* TXDEEMPH_M6DB_V4 */
#define PCIE_USB3_PCS_TXDEEMPH_M6DB_V4                                                                           ( 0x844 )
#define PCIE_USB3_PCS_TXDEEMPH_M6DB_V4_TXDEEMPH_M6DB_V4_MASK                                                     0x1F
#define PCIE_USB3_PCS_TXDEEMPH_M6DB_V4_TXDEEMPH_M6DB_V4_SHFT                                                     0x0

/* TXDEEMPH_M3P5DB_V4 */
#define PCIE_USB3_PCS_TXDEEMPH_M3P5DB_V4                                                                         ( 0x848 )
#define PCIE_USB3_PCS_TXDEEMPH_M3P5DB_V4_TXDEEMPH_M3P5DB_V4_MASK                                                 0x1F
#define PCIE_USB3_PCS_TXDEEMPH_M3P5DB_V4_TXDEEMPH_M3P5DB_V4_SHFT                                                 0x0

/* TXDEEMPH_M6DB_LS */
#define PCIE_USB3_PCS_TXDEEMPH_M6DB_LS                                                                           ( 0x84C )
#define PCIE_USB3_PCS_TXDEEMPH_M6DB_LS_TXDEEMPH_M6DB_LS_MASK                                                     0x1F
#define PCIE_USB3_PCS_TXDEEMPH_M6DB_LS_TXDEEMPH_M6DB_LS_SHFT                                                     0x0

/* TXDEEMPH_M3P5DB_LS */
#define PCIE_USB3_PCS_TXDEEMPH_M3P5DB_LS                                                                         ( 0x850 )
#define PCIE_USB3_PCS_TXDEEMPH_M3P5DB_LS_TXDEEMPH_M3P5DB_LS_MASK                                                 0x1F
#define PCIE_USB3_PCS_TXDEEMPH_M3P5DB_LS_TXDEEMPH_M3P5DB_LS_SHFT                                                 0x0

/* ENDPOINT_REFCLK_DRIVE */
#define PCIE_USB3_PCS_ENDPOINT_REFCLK_DRIVE                                                                      ( 0x854 )
#define PCIE_USB3_PCS_ENDPOINT_REFCLK_DRIVE_REFCLK_DRV_OFF_MODE_SEL_MASK                                         0x20
#define PCIE_USB3_PCS_ENDPOINT_REFCLK_DRIVE_REFCLK_DRV_OFF_MODE_SEL_SHFT                                         0x5
#define PCIE_USB3_PCS_ENDPOINT_REFCLK_DRIVE_REFCLK_CAPSEL_MASK                                                   0x18
#define PCIE_USB3_PCS_ENDPOINT_REFCLK_DRIVE_REFCLK_CAPSEL_SHFT                                                   0x3
#define PCIE_USB3_PCS_ENDPOINT_REFCLK_DRIVE_REFCLK_DRV_SW_EN_MASK                                                0x4
#define PCIE_USB3_PCS_ENDPOINT_REFCLK_DRIVE_REFCLK_DRV_SW_EN_SHFT                                                0x2
#define PCIE_USB3_PCS_ENDPOINT_REFCLK_DRIVE_REFCLK_DRV_LVL_MASK                                                  0x3
#define PCIE_USB3_PCS_ENDPOINT_REFCLK_DRIVE_REFCLK_DRV_LVL_SHFT                                                  0x0

/* RX_IDLE_DTCT_CNTRL */
#define PCIE_USB3_PCS_RX_IDLE_DTCT_CNTRL                                                                         ( 0x858 )
#define PCIE_USB3_PCS_RX_IDLE_DTCT_CNTRL_RX_IDLE_LOW_COUNT_VAL_MASK                                              0xF0
#define PCIE_USB3_PCS_RX_IDLE_DTCT_CNTRL_RX_IDLE_LOW_COUNT_VAL_SHFT                                              0x4
#define PCIE_USB3_PCS_RX_IDLE_DTCT_CNTRL_EIOS_DTCT_CFG_MASK                                                      0x8
#define PCIE_USB3_PCS_RX_IDLE_DTCT_CNTRL_EIOS_DTCT_CFG_SHFT                                                      0x3
#define PCIE_USB3_PCS_RX_IDLE_DTCT_CNTRL_RX_IDLE_ON_EIOS_EN_MASK                                                 0x4
#define PCIE_USB3_PCS_RX_IDLE_DTCT_CNTRL_RX_IDLE_ON_EIOS_EN_SHFT                                                 0x2
#define PCIE_USB3_PCS_RX_IDLE_DTCT_CNTRL_RX_IDLE_DTCT_G2S_EN_MASK                                                0x2
#define PCIE_USB3_PCS_RX_IDLE_DTCT_CNTRL_RX_IDLE_DTCT_G2S_EN_SHFT                                                0x1
#define PCIE_USB3_PCS_RX_IDLE_DTCT_CNTRL_RX_IDLE_DTCT_G1S_EN_MASK                                                0x1
#define PCIE_USB3_PCS_RX_IDLE_DTCT_CNTRL_RX_IDLE_DTCT_G1S_EN_SHFT                                                0x0

/* RATE_SLEW_CNTRL */
#define PCIE_USB3_PCS_RATE_SLEW_CNTRL                                                                            ( 0x85C )
#define PCIE_USB3_PCS_RATE_SLEW_CNTRL_RATE1_SLEW_CNTRL_MASK                                                      0x38
#define PCIE_USB3_PCS_RATE_SLEW_CNTRL_RATE1_SLEW_CNTRL_SHFT                                                      0x3
#define PCIE_USB3_PCS_RATE_SLEW_CNTRL_RATE0_SLEW_CNTRL_MASK                                                      0x7
#define PCIE_USB3_PCS_RATE_SLEW_CNTRL_RATE0_SLEW_CNTRL_SHFT                                                      0x0

/* POWER_STATE_CONFIG1 */
#define PCIE_USB3_PCS_POWER_STATE_CONFIG1                                                                        ( 0x860 )
#define PCIE_USB3_PCS_POWER_STATE_CONFIG1_SYSCLK_OFF_IN_L1SS_EN_MASK                                             0x80
#define PCIE_USB3_PCS_POWER_STATE_CONFIG1_SYSCLK_OFF_IN_L1SS_EN_SHFT                                             0x7
#define PCIE_USB3_PCS_POWER_STATE_CONFIG1_PLL_ON_IN_L1P1_EN_MASK                                                 0x40
#define PCIE_USB3_PCS_POWER_STATE_CONFIG1_PLL_ON_IN_L1P1_EN_SHFT                                                 0x6
#define PCIE_USB3_PCS_POWER_STATE_CONFIG1_RXELECIDLE_LOW_ON_L1SS_EXIT_EN_MASK                                    0x20
#define PCIE_USB3_PCS_POWER_STATE_CONFIG1_RXELECIDLE_LOW_ON_L1SS_EXIT_EN_SHFT                                    0x5
#define PCIE_USB3_PCS_POWER_STATE_CONFIG1_PU0_P0SU1_RXCLKS_ON_MASK                                               0x10
#define PCIE_USB3_PCS_POWER_STATE_CONFIG1_PU0_P0SU1_RXCLKS_ON_SHFT                                               0x4
#define PCIE_USB3_PCS_POWER_STATE_CONFIG1_SW_BEACON_RX_EN_MASK                                                   0x8
#define PCIE_USB3_PCS_POWER_STATE_CONFIG1_SW_BEACON_RX_EN_SHFT                                                   0x3
#define PCIE_USB3_PCS_POWER_STATE_CONFIG1_SW_BEACON_TX_EN_MASK                                                   0x4
#define PCIE_USB3_PCS_POWER_STATE_CONFIG1_SW_BEACON_TX_EN_SHFT                                                   0x2
#define PCIE_USB3_PCS_POWER_STATE_CONFIG1_P2U3_CM_CTRL_MASK                                                      0x2
#define PCIE_USB3_PCS_POWER_STATE_CONFIG1_P2U3_CM_CTRL_SHFT                                                      0x1
#define PCIE_USB3_PCS_POWER_STATE_CONFIG1_PU0_P1U2P2U3_WO_IDLE_EN_MASK                                           0x1
#define PCIE_USB3_PCS_POWER_STATE_CONFIG1_PU0_P1U2P2U3_WO_IDLE_EN_SHFT                                           0x0

/* POWER_STATE_CONFIG2 */
#define PCIE_USB3_PCS_POWER_STATE_CONFIG2                                                                        ( 0x864 )
#define PCIE_USB3_PCS_POWER_STATE_CONFIG2_EP_REFCLK_WAKEUP_IN_P2_EN_MASK                                         0x80
#define PCIE_USB3_PCS_POWER_STATE_CONFIG2_EP_REFCLK_WAKEUP_IN_P2_EN_SHFT                                         0x7
#define PCIE_USB3_PCS_POWER_STATE_CONFIG2_PHYCLK_REQ_N_CFG_MASK                                                  0x70
#define PCIE_USB3_PCS_POWER_STATE_CONFIG2_PHYCLK_REQ_N_CFG_SHFT                                                  0x4
#define PCIE_USB3_PCS_POWER_STATE_CONFIG2_SYSCLK_OFF_IN_P2U3_EN_MASK                                             0x8
#define PCIE_USB3_PCS_POWER_STATE_CONFIG2_SYSCLK_OFF_IN_P2U3_EN_SHFT                                             0x3
#define PCIE_USB3_PCS_POWER_STATE_CONFIG2_PLL_ON_IN_P2_EN_MASK                                                   0x4
#define PCIE_USB3_PCS_POWER_STATE_CONFIG2_PLL_ON_IN_P2_EN_SHFT                                                   0x2
#define PCIE_USB3_PCS_POWER_STATE_CONFIG2_P2U3_CLKREQ_SEL_MASK                                                   0x3
#define PCIE_USB3_PCS_POWER_STATE_CONFIG2_P2U3_CLKREQ_SEL_SHFT                                                   0x0

/* POWER_STATE_CONFIG3 */
#define PCIE_USB3_PCS_POWER_STATE_CONFIG3                                                                        ( 0x868 )
#define PCIE_USB3_PCS_POWER_STATE_CONFIG3_EBM1_SKP_ADD_STS_EN_MASK                                               0x80
#define PCIE_USB3_PCS_POWER_STATE_CONFIG3_EBM1_SKP_ADD_STS_EN_SHFT                                               0x7
#define PCIE_USB3_PCS_POWER_STATE_CONFIG3_RX_CM_DSBL_ON_RXTERM_LOW_MASK                                          0x40
#define PCIE_USB3_PCS_POWER_STATE_CONFIG3_RX_CM_DSBL_ON_RXTERM_LOW_SHFT                                          0x6
#define PCIE_USB3_PCS_POWER_STATE_CONFIG3_U2_CM_CTRL_MASK                                                        0x20
#define PCIE_USB3_PCS_POWER_STATE_CONFIG3_U2_CM_CTRL_SHFT                                                        0x5
#define PCIE_USB3_PCS_POWER_STATE_CONFIG3_RX_SS_DET_EN_MASK                                                      0x10
#define PCIE_USB3_PCS_POWER_STATE_CONFIG3_RX_SS_DET_EN_SHFT                                                      0x4
#define PCIE_USB3_PCS_POWER_STATE_CONFIG3_RX_SS_WD_TIME_MASK                                                     0xF
#define PCIE_USB3_PCS_POWER_STATE_CONFIG3_RX_SS_WD_TIME_SHFT                                                     0x0

/* POWER_STATE_CONFIG4 */
#define PCIE_USB3_PCS_POWER_STATE_CONFIG4                                                                        ( 0x86C )
#define PCIE_USB3_PCS_POWER_STATE_CONFIG4_PCLK_ON_IN_P2_EN_MASK                                                  0x8
#define PCIE_USB3_PCS_POWER_STATE_CONFIG4_PCLK_ON_IN_P2_EN_SHFT                                                  0x3
#define PCIE_USB3_PCS_POWER_STATE_CONFIG4_PCLK_ON_IN_L1SS_EN_MASK                                                0x4
#define PCIE_USB3_PCS_POWER_STATE_CONFIG4_PCLK_ON_IN_L1SS_EN_SHFT                                                0x2
#define PCIE_USB3_PCS_POWER_STATE_CONFIG4_PLL_ON_IN_L1P2_EN_MASK                                                 0x2
#define PCIE_USB3_PCS_POWER_STATE_CONFIG4_PLL_ON_IN_L1P2_EN_SHFT                                                 0x1
#define PCIE_USB3_PCS_POWER_STATE_CONFIG4_RXELECIDLE_DSBL_CAPTURE_EN_MASK                                        0x1
#define PCIE_USB3_PCS_POWER_STATE_CONFIG4_RXELECIDLE_DSBL_CAPTURE_EN_SHFT                                        0x0

/* RCVR_DTCT_DLY_P1U2_L */
#define PCIE_USB3_PCS_RCVR_DTCT_DLY_P1U2_L                                                                       ( 0x870 )
#define PCIE_USB3_PCS_RCVR_DTCT_DLY_P1U2_L_RCVR_DTCT_DLY_P1U2_L_MASK                                             0xFF
#define PCIE_USB3_PCS_RCVR_DTCT_DLY_P1U2_L_RCVR_DTCT_DLY_P1U2_L_SHFT                                             0x0

/* RCVR_DTCT_DLY_P1U2_H */
#define PCIE_USB3_PCS_RCVR_DTCT_DLY_P1U2_H                                                                       ( 0x874 )
#define PCIE_USB3_PCS_RCVR_DTCT_DLY_P1U2_H_RSVD_MASK                                                             0xF0
#define PCIE_USB3_PCS_RCVR_DTCT_DLY_P1U2_H_RSVD_SHFT                                                             0x4
#define PCIE_USB3_PCS_RCVR_DTCT_DLY_P1U2_H_RCVR_DTCT_DLY_P1U2_H_MASK                                             0xF
#define PCIE_USB3_PCS_RCVR_DTCT_DLY_P1U2_H_RCVR_DTCT_DLY_P1U2_H_SHFT                                             0x0

/* RCVR_DTCT_DLY_U3_L */
#define PCIE_USB3_PCS_RCVR_DTCT_DLY_U3_L                                                                         ( 0x878 )
#define PCIE_USB3_PCS_RCVR_DTCT_DLY_U3_L_RCVR_DTCT_DLY_U3_L_MASK                                                 0xFF
#define PCIE_USB3_PCS_RCVR_DTCT_DLY_U3_L_RCVR_DTCT_DLY_U3_L_SHFT                                                 0x0

/* RCVR_DTCT_DLY_U3_H */
#define PCIE_USB3_PCS_RCVR_DTCT_DLY_U3_H                                                                         ( 0x87C )
#define PCIE_USB3_PCS_RCVR_DTCT_DLY_U3_H_RSVD_MASK                                                               0xFC
#define PCIE_USB3_PCS_RCVR_DTCT_DLY_U3_H_RSVD_SHFT                                                               0x2
#define PCIE_USB3_PCS_RCVR_DTCT_DLY_U3_H_RCVR_DTCT_DLY_U3_H_MASK                                                 0x3
#define PCIE_USB3_PCS_RCVR_DTCT_DLY_U3_H_RCVR_DTCT_DLY_U3_H_SHFT                                                 0x0

/* LOCK_DETECT_CONFIG1 */
#define PCIE_USB3_PCS_LOCK_DETECT_CONFIG1                                                                        ( 0x880 )
#define PCIE_USB3_PCS_LOCK_DETECT_CONFIG1_LOCK_HOLDOFF_TIME_L_MASK                                               0xFF
#define PCIE_USB3_PCS_LOCK_DETECT_CONFIG1_LOCK_HOLDOFF_TIME_L_SHFT                                               0x0

/* LOCK_DETECT_CONFIG2 */
#define PCIE_USB3_PCS_LOCK_DETECT_CONFIG2                                                                        ( 0x884 )
#define PCIE_USB3_PCS_LOCK_DETECT_CONFIG2_LOCK_COUNT_VAL_L_MASK                                                  0xFF
#define PCIE_USB3_PCS_LOCK_DETECT_CONFIG2_LOCK_COUNT_VAL_L_SHFT                                                  0x0

/* LOCK_DETECT_CONFIG3 */
#define PCIE_USB3_PCS_LOCK_DETECT_CONFIG3                                                                        ( 0x888 )
#define PCIE_USB3_PCS_LOCK_DETECT_CONFIG3_LOCK_KEEP_VAL_MASK                                                     0xC0
#define PCIE_USB3_PCS_LOCK_DETECT_CONFIG3_LOCK_KEEP_VAL_SHFT                                                     0x6
#define PCIE_USB3_PCS_LOCK_DETECT_CONFIG3_LOCK_COUNT_VAL_H_MASK                                                  0x30
#define PCIE_USB3_PCS_LOCK_DETECT_CONFIG3_LOCK_COUNT_VAL_H_SHFT                                                  0x4
#define PCIE_USB3_PCS_LOCK_DETECT_CONFIG3_LOCK_HOLDOFF_TIME_H_MASK                                               0xF
#define PCIE_USB3_PCS_LOCK_DETECT_CONFIG3_LOCK_HOLDOFF_TIME_H_SHFT                                               0x0

/* TSYNC_RSYNC_TIME */
#define PCIE_USB3_PCS_TSYNC_RSYNC_TIME                                                                           ( 0x88C )
#define PCIE_USB3_PCS_TSYNC_RSYNC_TIME_TSYNC_DLY_TIME_MASK                                                       0xF0
#define PCIE_USB3_PCS_TSYNC_RSYNC_TIME_TSYNC_DLY_TIME_SHFT                                                       0x4
#define PCIE_USB3_PCS_TSYNC_RSYNC_TIME_ACTIVE_RSYNC_TIME_MASK                                                    0xF
#define PCIE_USB3_PCS_TSYNC_RSYNC_TIME_ACTIVE_RSYNC_TIME_SHFT                                                    0x0

/* SIGDET_LOW_2_IDLE_TIME */
#define PCIE_USB3_PCS_SIGDET_LOW_2_IDLE_TIME                                                                     ( 0x890 )
#define PCIE_USB3_PCS_SIGDET_LOW_2_IDLE_TIME_SIGDET_LOW_2_IDLE_TIME_MASK                                         0xFF
#define PCIE_USB3_PCS_SIGDET_LOW_2_IDLE_TIME_SIGDET_LOW_2_IDLE_TIME_SHFT                                         0x0

/* BEACON_2_IDLE_TIME_L */
#define PCIE_USB3_PCS_BEACON_2_IDLE_TIME_L                                                                       ( 0x894 )
#define PCIE_USB3_PCS_BEACON_2_IDLE_TIME_L_BEACON_2_IDLE_TIME_L_MASK                                             0xFF
#define PCIE_USB3_PCS_BEACON_2_IDLE_TIME_L_BEACON_2_IDLE_TIME_L_SHFT                                             0x0

/* BEACON_2_IDLE_TIME_H */
#define PCIE_USB3_PCS_BEACON_2_IDLE_TIME_H                                                                       ( 0x898 )
#define PCIE_USB3_PCS_BEACON_2_IDLE_TIME_H_RSVD_MASK                                                             0xF0
#define PCIE_USB3_PCS_BEACON_2_IDLE_TIME_H_RSVD_SHFT                                                             0x4
#define PCIE_USB3_PCS_BEACON_2_IDLE_TIME_H_BEACON_2_IDLE_TIME_H_MASK                                             0xF
#define PCIE_USB3_PCS_BEACON_2_IDLE_TIME_H_BEACON_2_IDLE_TIME_H_SHFT                                             0x0

/* PWRUP_RESET_DLY_TIME_SYSCLK */
#define PCIE_USB3_PCS_PWRUP_RESET_DLY_TIME_SYSCLK                                                                ( 0x89C )
#define PCIE_USB3_PCS_PWRUP_RESET_DLY_TIME_SYSCLK_PWRUP_RESET_DLY_TIME_SYSCLK_MASK                               0xFF
#define PCIE_USB3_PCS_PWRUP_RESET_DLY_TIME_SYSCLK_PWRUP_RESET_DLY_TIME_SYSCLK_SHFT                               0x0

/* PWRUP_RESET_DLY_TIME_AUXCLK */
#define PCIE_USB3_PCS_PWRUP_RESET_DLY_TIME_AUXCLK                                                                ( 0x8A0 )
#define PCIE_USB3_PCS_PWRUP_RESET_DLY_TIME_AUXCLK_PWRUP_RESET_DLY_TIME_AUXCLK_MASK                               0xFF
#define PCIE_USB3_PCS_PWRUP_RESET_DLY_TIME_AUXCLK_PWRUP_RESET_DLY_TIME_AUXCLK_SHFT                               0x0

/* LP_WAKEUP_DLY_TIME_AUXCLK */
#define PCIE_USB3_PCS_LP_WAKEUP_DLY_TIME_AUXCLK                                                                  ( 0x8A4 )
#define PCIE_USB3_PCS_LP_WAKEUP_DLY_TIME_AUXCLK_LP_WAKEUP_DLY_TIME_AUXCLK_MASK                                   0xFF
#define PCIE_USB3_PCS_LP_WAKEUP_DLY_TIME_AUXCLK_LP_WAKEUP_DLY_TIME_AUXCLK_SHFT                                   0x0

/* PLL_LOCK_CHK_DLY_TIME */
#define PCIE_USB3_PCS_PLL_LOCK_CHK_DLY_TIME                                                                      ( 0x8A8 )
#define PCIE_USB3_PCS_PLL_LOCK_CHK_DLY_TIME_PLL_LOCK_CHK_DLY_TIME_MASK                                           0xFF
#define PCIE_USB3_PCS_PLL_LOCK_CHK_DLY_TIME_PLL_LOCK_CHK_DLY_TIME_SHFT                                           0x0

/* LFPS_DET_HIGH_COUNT_VAL */
#define PCIE_USB3_PCS_LFPS_DET_HIGH_COUNT_VAL                                                                    ( 0x8AC )
#define PCIE_USB3_PCS_LFPS_DET_HIGH_COUNT_VAL_RSVD_MASK                                                          0xF0
#define PCIE_USB3_PCS_LFPS_DET_HIGH_COUNT_VAL_RSVD_SHFT                                                          0x4
#define PCIE_USB3_PCS_LFPS_DET_HIGH_COUNT_VAL_LFPS_DET_HIGH_COUNT_VAL_MASK                                       0xF
#define PCIE_USB3_PCS_LFPS_DET_HIGH_COUNT_VAL_LFPS_DET_HIGH_COUNT_VAL_SHFT                                       0x0

/* LFPS_TX_ECSTART_EQTLOCK */
#define PCIE_USB3_PCS_LFPS_TX_ECSTART_EQTLOCK                                                                    ( 0x8B0 )
#define PCIE_USB3_PCS_LFPS_TX_ECSTART_EQTLOCK_RXEQTRAINING_LOCK_TIME_MASK                                        0xF0
#define PCIE_USB3_PCS_LFPS_TX_ECSTART_EQTLOCK_RXEQTRAINING_LOCK_TIME_SHFT                                        0x4
#define PCIE_USB3_PCS_LFPS_TX_ECSTART_EQTLOCK_LFPS_TX_END_CNT_START_MASK                                         0xF
#define PCIE_USB3_PCS_LFPS_TX_ECSTART_EQTLOCK_LFPS_TX_END_CNT_START_SHFT                                         0x0

/* LFPS_TX_END_CNT_P2U3_START */
#define PCIE_USB3_PCS_LFPS_TX_END_CNT_P2U3_START                                                                 ( 0x8B4 )
#define PCIE_USB3_PCS_LFPS_TX_END_CNT_P2U3_START_LFPS_TX_END_CNT_U3_START_MASK                                   0xFF
#define PCIE_USB3_PCS_LFPS_TX_END_CNT_P2U3_START_LFPS_TX_END_CNT_U3_START_SHFT                                   0x0

/* RXEQTRAINING_WAIT_TIME */
#define PCIE_USB3_PCS_RXEQTRAINING_WAIT_TIME                                                                     ( 0x8B8 )
#define PCIE_USB3_PCS_RXEQTRAINING_WAIT_TIME_RXEQTRAINING_WAIT_TIME_MASK                                         0xFF
#define PCIE_USB3_PCS_RXEQTRAINING_WAIT_TIME_RXEQTRAINING_WAIT_TIME_SHFT                                         0x0

/* RXEQTRAINING_RUN_TIME */
#define PCIE_USB3_PCS_RXEQTRAINING_RUN_TIME                                                                      ( 0x8BC )
#define PCIE_USB3_PCS_RXEQTRAINING_RUN_TIME_RXEQTRAINING_RUN_TIME_MASK                                           0xFF
#define PCIE_USB3_PCS_RXEQTRAINING_RUN_TIME_RXEQTRAINING_RUN_TIME_SHFT                                           0x0

/* TXONESZEROS_RUN_LENGTH */
#define PCIE_USB3_PCS_TXONESZEROS_RUN_LENGTH                                                                     ( 0x8C0 )
#define PCIE_USB3_PCS_TXONESZEROS_RUN_LENGTH_RSVD_MASK                                                           0xF0
#define PCIE_USB3_PCS_TXONESZEROS_RUN_LENGTH_RSVD_SHFT                                                           0x4
#define PCIE_USB3_PCS_TXONESZEROS_RUN_LENGTH_TXONESZEROS_RUN_LENGTH_MASK                                         0xF
#define PCIE_USB3_PCS_TXONESZEROS_RUN_LENGTH_TXONESZEROS_RUN_LENGTH_SHFT                                         0x0

/* FLL_CNTRL1 */
#define PCIE_USB3_PCS_FLL_CNTRL1                                                                                 ( 0x8C4 )
#define PCIE_USB3_PCS_FLL_CNTRL1_FLL_ANA_CTRL_MASK                                                               0xF0
#define PCIE_USB3_PCS_FLL_CNTRL1_FLL_ANA_CTRL_SHFT                                                               0x4
#define PCIE_USB3_PCS_FLL_CNTRL1_FLL_BYPASS_EN_MASK                                                              0x8
#define PCIE_USB3_PCS_FLL_CNTRL1_FLL_BYPASS_EN_SHFT                                                              0x3
#define PCIE_USB3_PCS_FLL_CNTRL1_FLL_BKGRND_CAL_EN_MASK                                                          0x4
#define PCIE_USB3_PCS_FLL_CNTRL1_FLL_BKGRND_CAL_EN_SHFT                                                          0x2
#define PCIE_USB3_PCS_FLL_CNTRL1_FLL_CAL_ON_STRTUP_EN_MASK                                                       0x2
#define PCIE_USB3_PCS_FLL_CNTRL1_FLL_CAL_ON_STRTUP_EN_SHFT                                                       0x1
#define PCIE_USB3_PCS_FLL_CNTRL1_FLL_MAN_MODE_MASK                                                               0x1
#define PCIE_USB3_PCS_FLL_CNTRL1_FLL_MAN_MODE_SHFT                                                               0x0

/* FLL_CNTRL2 */
#define PCIE_USB3_PCS_FLL_CNTRL2                                                                                 ( 0x8C8 )
#define PCIE_USB3_PCS_FLL_CNTRL2_FLL_PER_AUTO_MODE_MASK                                                          0x80
#define PCIE_USB3_PCS_FLL_CNTRL2_FLL_PER_AUTO_MODE_SHFT                                                          0x7
#define PCIE_USB3_PCS_FLL_CNTRL2_FLL_AO_AUTO_MODE_MASK                                                           0x40
#define PCIE_USB3_PCS_FLL_CNTRL2_FLL_AO_AUTO_MODE_SHFT                                                           0x6
#define PCIE_USB3_PCS_FLL_CNTRL2_FLL_CAL_DONE_MASK_MASK                                                          0x20
#define PCIE_USB3_PCS_FLL_CNTRL2_FLL_CAL_DONE_MASK_SHFT                                                          0x5
#define PCIE_USB3_PCS_FLL_CNTRL2_FLL_ANA_EN_FORCE_SET_MASK                                                       0x10
#define PCIE_USB3_PCS_FLL_CNTRL2_FLL_ANA_EN_FORCE_SET_SHFT                                                       0x4
#define PCIE_USB3_PCS_FLL_CNTRL2_FLL_DIG_EN_FORCE_CLR_MASK                                                       0x8
#define PCIE_USB3_PCS_FLL_CNTRL2_FLL_DIG_EN_FORCE_CLR_SHFT                                                       0x3
#define PCIE_USB3_PCS_FLL_CNTRL2_FLL_DIV_MASK                                                                    0x7
#define PCIE_USB3_PCS_FLL_CNTRL2_FLL_DIV_SHFT                                                                    0x0

/* FLL_CNT_VAL_L */
#define PCIE_USB3_PCS_FLL_CNT_VAL_L                                                                              ( 0x8CC )
#define PCIE_USB3_PCS_FLL_CNT_VAL_L_FLL_CNT_VAL_L_MASK                                                           0xFF
#define PCIE_USB3_PCS_FLL_CNT_VAL_L_FLL_CNT_VAL_L_SHFT                                                           0x0

/* FLL_CNT_VAL_H_TOL */
#define PCIE_USB3_PCS_FLL_CNT_VAL_H_TOL                                                                          ( 0x8D0 )
#define PCIE_USB3_PCS_FLL_CNT_VAL_H_TOL_USE_CNT_VAL_TOL_FORGND_MASK                                              0x80
#define PCIE_USB3_PCS_FLL_CNT_VAL_H_TOL_USE_CNT_VAL_TOL_FORGND_SHFT                                              0x7
#define PCIE_USB3_PCS_FLL_CNT_VAL_H_TOL_FLL_CNT_VAL_TOL_MASK                                                     0x70
#define PCIE_USB3_PCS_FLL_CNT_VAL_H_TOL_FLL_CNT_VAL_TOL_SHFT                                                     0x4
#define PCIE_USB3_PCS_FLL_CNT_VAL_H_TOL_FLL_CNT_VAL_H_MASK                                                       0xF
#define PCIE_USB3_PCS_FLL_CNT_VAL_H_TOL_FLL_CNT_VAL_H_SHFT                                                       0x0

/* FLL_MAN_CODE */
#define PCIE_USB3_PCS_FLL_MAN_CODE                                                                               ( 0x8D4 )
#define PCIE_USB3_PCS_FLL_MAN_CODE_FLL_MAN_CODE_MASK                                                             0xFF
#define PCIE_USB3_PCS_FLL_MAN_CODE_FLL_MAN_CODE_SHFT                                                             0x0

/* AUTONOMOUS_MODE_CTRL */
#define PCIE_USB3_PCS_AUTONOMOUS_MODE_CTRL                                                                       ( 0x8D8 )
#define PCIE_USB3_PCS_AUTONOMOUS_MODE_CTRL_AUTO_RX_SIGDET_PER_EN_MASK                                            0x80
#define PCIE_USB3_PCS_AUTONOMOUS_MODE_CTRL_AUTO_RX_SIGDET_PER_EN_SHFT                                            0x7
#define PCIE_USB3_PCS_AUTONOMOUS_MODE_CTRL_SIGDET_ON_WITH_NO_AUTO_LFPS_P2U3_MASK                                 0x40
#define PCIE_USB3_PCS_AUTONOMOUS_MODE_CTRL_SIGDET_ON_WITH_NO_AUTO_LFPS_P2U3_SHFT                                 0x6
#define PCIE_USB3_PCS_AUTONOMOUS_MODE_CTRL_AUTO_RX_SIGDET_AO_MASK                                                0x20
#define PCIE_USB3_PCS_AUTONOMOUS_MODE_CTRL_AUTO_RX_SIGDET_AO_SHFT                                                0x5
#define PCIE_USB3_PCS_AUTONOMOUS_MODE_CTRL_ARCVR_DTCT_EVENT_SEL_MASK                                             0x10
#define PCIE_USB3_PCS_AUTONOMOUS_MODE_CTRL_ARCVR_DTCT_EVENT_SEL_SHFT                                             0x4
#define PCIE_USB3_PCS_AUTONOMOUS_MODE_CTRL_ARCVR_DTCT_CM_CTRL_MASK                                               0x8
#define PCIE_USB3_PCS_AUTONOMOUS_MODE_CTRL_ARCVR_DTCT_CM_CTRL_SHFT                                               0x3
#define PCIE_USB3_PCS_AUTONOMOUS_MODE_CTRL_ALFPS_RCVR_DTCT_TEST_EN_MASK                                          0x4
#define PCIE_USB3_PCS_AUTONOMOUS_MODE_CTRL_ALFPS_RCVR_DTCT_TEST_EN_SHFT                                          0x2
#define PCIE_USB3_PCS_AUTONOMOUS_MODE_CTRL_ALFPS_DTCT_EN_MASK                                                    0x2
#define PCIE_USB3_PCS_AUTONOMOUS_MODE_CTRL_ALFPS_DTCT_EN_SHFT                                                    0x1
#define PCIE_USB3_PCS_AUTONOMOUS_MODE_CTRL_ARCVR_DTCT_EN_MASK                                                    0x1
#define PCIE_USB3_PCS_AUTONOMOUS_MODE_CTRL_ARCVR_DTCT_EN_SHFT                                                    0x0

/* LFPS_RXTERM_IRQ_CLEAR */
#define PCIE_USB3_PCS_LFPS_RXTERM_IRQ_CLEAR                                                                      ( 0x8DC )
#define PCIE_USB3_PCS_LFPS_RXTERM_IRQ_CLEAR_RSVD_MASK                                                            0x2
#define PCIE_USB3_PCS_LFPS_RXTERM_IRQ_CLEAR_RSVD_SHFT                                                            0x1
#define PCIE_USB3_PCS_LFPS_RXTERM_IRQ_CLEAR_LFPS_RXTERM_IRQ_CLEAR_MASK                                           0x1
#define PCIE_USB3_PCS_LFPS_RXTERM_IRQ_CLEAR_LFPS_RXTERM_IRQ_CLEAR_SHFT                                           0x0

/* ARCVR_DTCT_EN_PERIOD */
#define PCIE_USB3_PCS_ARCVR_DTCT_EN_PERIOD                                                                       ( 0x8E0 )
#define PCIE_USB3_PCS_ARCVR_DTCT_EN_PERIOD_ARCVR_DTCT_EN_PERIOD_MASK                                             0xFF
#define PCIE_USB3_PCS_ARCVR_DTCT_EN_PERIOD_ARCVR_DTCT_EN_PERIOD_SHFT                                             0x0

/* ARCVR_DTCT_CM_DLY */
#define PCIE_USB3_PCS_ARCVR_DTCT_CM_DLY                                                                          ( 0x8E4 )
#define PCIE_USB3_PCS_ARCVR_DTCT_CM_DLY_RSVD_MASK                                                                0xF0
#define PCIE_USB3_PCS_ARCVR_DTCT_CM_DLY_RSVD_SHFT                                                                0x4
#define PCIE_USB3_PCS_ARCVR_DTCT_CM_DLY_ARCVR_DTCT_CM_DLY_MASK                                                   0xF
#define PCIE_USB3_PCS_ARCVR_DTCT_CM_DLY_ARCVR_DTCT_CM_DLY_SHFT                                                   0x0

/* ALFPS_DEGLITCH_VAL */
#define PCIE_USB3_PCS_ALFPS_DEGLITCH_VAL                                                                         ( 0x8E8 )
#define PCIE_USB3_PCS_ALFPS_DEGLITCH_VAL_RSVD_MASK                                                               0xC0
#define PCIE_USB3_PCS_ALFPS_DEGLITCH_VAL_RSVD_SHFT                                                               0x6
#define PCIE_USB3_PCS_ALFPS_DEGLITCH_VAL_ALFPS_DEGLITCH_VAL_MASK                                                 0x3F
#define PCIE_USB3_PCS_ALFPS_DEGLITCH_VAL_ALFPS_DEGLITCH_VAL_SHFT                                                 0x0

/* INSIG_SW_CTRL1 */
#define PCIE_USB3_PCS_INSIG_SW_CTRL1                                                                             ( 0x8EC )
#define PCIE_USB3_PCS_INSIG_SW_CTRL1_SW_TXSWING_MASK                                                             0x80
#define PCIE_USB3_PCS_INSIG_SW_CTRL1_SW_TXSWING_SHFT                                                             0x7
#define PCIE_USB3_PCS_INSIG_SW_CTRL1_SW_TXMARGIN_MASK                                                            0x70
#define PCIE_USB3_PCS_INSIG_SW_CTRL1_SW_TXMARGIN_SHFT                                                            0x4
#define PCIE_USB3_PCS_INSIG_SW_CTRL1_SW_TXDEEMPH_MASK                                                            0xC
#define PCIE_USB3_PCS_INSIG_SW_CTRL1_SW_TXDEEMPH_SHFT                                                            0x2
#define PCIE_USB3_PCS_INSIG_SW_CTRL1_SW_POWERDOWN_MASK                                                           0x3
#define PCIE_USB3_PCS_INSIG_SW_CTRL1_SW_POWERDOWN_SHFT                                                           0x0

/* INSIG_SW_CTRL2 */
#define PCIE_USB3_PCS_INSIG_SW_CTRL2                                                                             ( 0x8F0 )
#define PCIE_USB3_PCS_INSIG_SW_CTRL2_SW_RATE_MASK                                                                0x80
#define PCIE_USB3_PCS_INSIG_SW_CTRL2_SW_RATE_SHFT                                                                0x7
#define PCIE_USB3_PCS_INSIG_SW_CTRL2_SW_TXCM_DISABLE_MASK                                                        0x40
#define PCIE_USB3_PCS_INSIG_SW_CTRL2_SW_TXCM_DISABLE_SHFT                                                        0x6
#define PCIE_USB3_PCS_INSIG_SW_CTRL2_SW_RXELECIDLE_DISABLE_MASK                                                  0x20
#define PCIE_USB3_PCS_INSIG_SW_CTRL2_SW_RXELECIDLE_DISABLE_SHFT                                                  0x5
#define PCIE_USB3_PCS_INSIG_SW_CTRL2_SW_PCLKREQ_N_MASK                                                           0x10
#define PCIE_USB3_PCS_INSIG_SW_CTRL2_SW_PCLKREQ_N_SHFT                                                           0x4
#define PCIE_USB3_PCS_INSIG_SW_CTRL2_SW_TXCOMPLIANCE_MASK                                                        0x8
#define PCIE_USB3_PCS_INSIG_SW_CTRL2_SW_TXCOMPLIANCE_SHFT                                                        0x3
#define PCIE_USB3_PCS_INSIG_SW_CTRL2_SW_TXDETECTRX_LOOPBACK_MASK                                                 0x4
#define PCIE_USB3_PCS_INSIG_SW_CTRL2_SW_TXDETECTRX_LOOPBACK_SHFT                                                 0x2
#define PCIE_USB3_PCS_INSIG_SW_CTRL2_SW_RXPOLARITY_MASK                                                          0x2
#define PCIE_USB3_PCS_INSIG_SW_CTRL2_SW_RXPOLARITY_SHFT                                                          0x1
#define PCIE_USB3_PCS_INSIG_SW_CTRL2_SW_TXELECIDLE_MASK                                                          0x1
#define PCIE_USB3_PCS_INSIG_SW_CTRL2_SW_TXELECIDLE_SHFT                                                          0x0

/* INSIG_SW_CTRL3 */
#define PCIE_USB3_PCS_INSIG_SW_CTRL3                                                                             ( 0x8F4 )
#define PCIE_USB3_PCS_INSIG_SW_CTRL3_RSVD_MASK                                                                   0xE0
#define PCIE_USB3_PCS_INSIG_SW_CTRL3_RSVD_SHFT                                                                   0x5
#define PCIE_USB3_PCS_INSIG_SW_CTRL3_SW_RXTERMINATION_MASK                                                       0x10
#define PCIE_USB3_PCS_INSIG_SW_CTRL3_SW_RXTERMINATION_SHFT                                                       0x4
#define PCIE_USB3_PCS_INSIG_SW_CTRL3_SW_RXEQTRAINING_MASK                                                        0x8
#define PCIE_USB3_PCS_INSIG_SW_CTRL3_SW_RXEQTRAINING_SHFT                                                        0x3
#define PCIE_USB3_PCS_INSIG_SW_CTRL3_SW_TXONESZEROS_MASK                                                         0x4
#define PCIE_USB3_PCS_INSIG_SW_CTRL3_SW_TXONESZEROS_SHFT                                                         0x2
#define PCIE_USB3_PCS_INSIG_SW_CTRL3_SW_EBUFMODE_MASK                                                            0x2
#define PCIE_USB3_PCS_INSIG_SW_CTRL3_SW_EBUFMODE_SHFT                                                            0x1
#define PCIE_USB3_PCS_INSIG_SW_CTRL3_SW_PHYMODE_MASK                                                             0x1
#define PCIE_USB3_PCS_INSIG_SW_CTRL3_SW_PHYMODE_SHFT                                                             0x0

/* INSIG_MX_CTRL1 */
#define PCIE_USB3_PCS_INSIG_MX_CTRL1                                                                             ( 0x8F8 )
#define PCIE_USB3_PCS_INSIG_MX_CTRL1_SW_TXSWING_MX_MASK                                                          0x80
#define PCIE_USB3_PCS_INSIG_MX_CTRL1_SW_TXSWING_MX_SHFT                                                          0x7
#define PCIE_USB3_PCS_INSIG_MX_CTRL1_SW_TXMGN_MX_MASK                                                            0x10
#define PCIE_USB3_PCS_INSIG_MX_CTRL1_SW_TXMGN_MX_SHFT                                                            0x4
#define PCIE_USB3_PCS_INSIG_MX_CTRL1_SW_TXDEEMPH_MX_MASK                                                         0x4
#define PCIE_USB3_PCS_INSIG_MX_CTRL1_SW_TXDEEMPH_MX_SHFT                                                         0x2
#define PCIE_USB3_PCS_INSIG_MX_CTRL1_SW_POWERDOWN_MX_MASK                                                        0x1
#define PCIE_USB3_PCS_INSIG_MX_CTRL1_SW_POWERDOWN_MX_SHFT                                                        0x0

/* INSIG_MX_CTRL2 */
#define PCIE_USB3_PCS_INSIG_MX_CTRL2                                                                             ( 0x8FC )
#define PCIE_USB3_PCS_INSIG_MX_CTRL2_SW_RATE_MX_MASK                                                             0x80
#define PCIE_USB3_PCS_INSIG_MX_CTRL2_SW_RATE_MX_SHFT                                                             0x7
#define PCIE_USB3_PCS_INSIG_MX_CTRL2_SW_TXCM_DISABLE_MX_MASK                                                     0x40
#define PCIE_USB3_PCS_INSIG_MX_CTRL2_SW_TXCM_DISABLE_MX_SHFT                                                     0x6
#define PCIE_USB3_PCS_INSIG_MX_CTRL2_SW_RXELECIDLE_DISABLE_MX_MASK                                               0x20
#define PCIE_USB3_PCS_INSIG_MX_CTRL2_SW_RXELECIDLE_DISABLE_MX_SHFT                                               0x5
#define PCIE_USB3_PCS_INSIG_MX_CTRL2_SW_PCLKREQ_N_MX_MASK                                                        0x10
#define PCIE_USB3_PCS_INSIG_MX_CTRL2_SW_PCLKREQ_N_MX_SHFT                                                        0x4
#define PCIE_USB3_PCS_INSIG_MX_CTRL2_SW_TXCOMPLIANCE_MX_MASK                                                     0x8
#define PCIE_USB3_PCS_INSIG_MX_CTRL2_SW_TXCOMPLIANCE_MX_SHFT                                                     0x3
#define PCIE_USB3_PCS_INSIG_MX_CTRL2_SW_TXDTCTRX_LPB_MX_MASK                                                     0x4
#define PCIE_USB3_PCS_INSIG_MX_CTRL2_SW_TXDTCTRX_LPB_MX_SHFT                                                     0x2
#define PCIE_USB3_PCS_INSIG_MX_CTRL2_SW_RXPOLARITY_MX_MASK                                                       0x2
#define PCIE_USB3_PCS_INSIG_MX_CTRL2_SW_RXPOLARITY_MX_SHFT                                                       0x1
#define PCIE_USB3_PCS_INSIG_MX_CTRL2_SW_TXELECIDLE_MX_MASK                                                       0x1
#define PCIE_USB3_PCS_INSIG_MX_CTRL2_SW_TXELECIDLE_MX_SHFT                                                       0x0

/* INSIG_MX_CTRL3 */
#define PCIE_USB3_PCS_INSIG_MX_CTRL3                                                                             ( 0x900 )
#define PCIE_USB3_PCS_INSIG_MX_CTRL3_RSVD_MASK                                                                   0xE0
#define PCIE_USB3_PCS_INSIG_MX_CTRL3_RSVD_SHFT                                                                   0x5
#define PCIE_USB3_PCS_INSIG_MX_CTRL3_SW_RXTERMINATION_MX_MASK                                                    0x10
#define PCIE_USB3_PCS_INSIG_MX_CTRL3_SW_RXTERMINATION_MX_SHFT                                                    0x4
#define PCIE_USB3_PCS_INSIG_MX_CTRL3_SW_RXEQTRAINING_MX_MASK                                                     0x8
#define PCIE_USB3_PCS_INSIG_MX_CTRL3_SW_RXEQTRAINING_MX_SHFT                                                     0x3
#define PCIE_USB3_PCS_INSIG_MX_CTRL3_SW_TXONESZEROS_MX_MASK                                                      0x4
#define PCIE_USB3_PCS_INSIG_MX_CTRL3_SW_TXONESZEROS_MX_SHFT                                                      0x2
#define PCIE_USB3_PCS_INSIG_MX_CTRL3_SW_EBUFMODE_MX_MASK                                                         0x2
#define PCIE_USB3_PCS_INSIG_MX_CTRL3_SW_EBUFMODE_MX_SHFT                                                         0x1
#define PCIE_USB3_PCS_INSIG_MX_CTRL3_SW_PHYMODE_MX_MASK                                                          0x1
#define PCIE_USB3_PCS_INSIG_MX_CTRL3_SW_PHYMODE_MX_SHFT                                                          0x0

/* OUTSIG_SW_CTRL1 */
#define PCIE_USB3_PCS_OUTSIG_SW_CTRL1                                                                            ( 0x904 )
#define PCIE_USB3_PCS_OUTSIG_SW_CTRL1_SW_RXELECIDLE_MASK                                                         0x8
#define PCIE_USB3_PCS_OUTSIG_SW_CTRL1_SW_RXELECIDLE_SHFT                                                         0x3
#define PCIE_USB3_PCS_OUTSIG_SW_CTRL1_SW_PHYCLK_REQ_N_MASK                                                       0x4
#define PCIE_USB3_PCS_OUTSIG_SW_CTRL1_SW_PHYCLK_REQ_N_SHFT                                                       0x2
#define PCIE_USB3_PCS_OUTSIG_SW_CTRL1_SW_PCLKACK_N_MASK                                                          0x2
#define PCIE_USB3_PCS_OUTSIG_SW_CTRL1_SW_PCLKACK_N_SHFT                                                          0x1
#define PCIE_USB3_PCS_OUTSIG_SW_CTRL1_SW_PHYSTATUS_MASK                                                          0x1
#define PCIE_USB3_PCS_OUTSIG_SW_CTRL1_SW_PHYSTATUS_SHFT                                                          0x0

/* OUTSIG_MX_CTRL1 */
#define PCIE_USB3_PCS_OUTSIG_MX_CTRL1                                                                            ( 0x908 )
#define PCIE_USB3_PCS_OUTSIG_MX_CTRL1_SW_RXELECIDLE_MX_MASK                                                      0x8
#define PCIE_USB3_PCS_OUTSIG_MX_CTRL1_SW_RXELECIDLE_MX_SHFT                                                      0x3
#define PCIE_USB3_PCS_OUTSIG_MX_CTRL1_SW_PHYCLK_REQ_N_MX_MASK                                                    0x4
#define PCIE_USB3_PCS_OUTSIG_MX_CTRL1_SW_PHYCLK_REQ_N_MX_SHFT                                                    0x2
#define PCIE_USB3_PCS_OUTSIG_MX_CTRL1_SW_PCLKACK_N_MX_MASK                                                       0x2
#define PCIE_USB3_PCS_OUTSIG_MX_CTRL1_SW_PCLKACK_N_MX_SHFT                                                       0x1
#define PCIE_USB3_PCS_OUTSIG_MX_CTRL1_SW_PHYSTATUS_MX_MASK                                                       0x1
#define PCIE_USB3_PCS_OUTSIG_MX_CTRL1_SW_PHYSTATUS_MX_SHFT                                                       0x0

/* CLK_DEBUG_BYPASS_CTRL */
#define PCIE_USB3_PCS_CLK_DEBUG_BYPASS_CTRL                                                                      ( 0x90C )
#define PCIE_USB3_PCS_CLK_DEBUG_BYPASS_CTRL_CLK_DEBUG_BYPASS_SEL_MASK                                            0x1E
#define PCIE_USB3_PCS_CLK_DEBUG_BYPASS_CTRL_CLK_DEBUG_BYPASS_SEL_SHFT                                            0x1
#define PCIE_USB3_PCS_CLK_DEBUG_BYPASS_CTRL_CLK_DEBUG_BYPASS_EN_MASK                                             0x1
#define PCIE_USB3_PCS_CLK_DEBUG_BYPASS_CTRL_CLK_DEBUG_BYPASS_EN_SHFT                                             0x0

/* TEST_CONTROL */
#define PCIE_USB3_PCS_TEST_CONTROL                                                                               ( 0x910 )
#define PCIE_USB3_PCS_TEST_CONTROL_FORCE_REC_DETECTED_MASK                                                       0x8
#define PCIE_USB3_PCS_TEST_CONTROL_FORCE_REC_DETECTED_SHFT                                                       0x3
#define PCIE_USB3_PCS_TEST_CONTROL_PCS_PCLK_SEL_MASK                                                             0x4
#define PCIE_USB3_PCS_TEST_CONTROL_PCS_PCLK_SEL_SHFT                                                             0x2
#define PCIE_USB3_PCS_TEST_CONTROL_BIST_MODE_MASK                                                                0x2
#define PCIE_USB3_PCS_TEST_CONTROL_BIST_MODE_SHFT                                                                0x1
#define PCIE_USB3_PCS_TEST_CONTROL_FORCE_RX_SIGDET_MASK                                                          0x1
#define PCIE_USB3_PCS_TEST_CONTROL_FORCE_RX_SIGDET_SHFT                                                          0x0

/* TEST_CONTROL2 */
#define PCIE_USB3_PCS_TEST_CONTROL2                                                                              ( 0x914 )
#define PCIE_USB3_PCS_TEST_CONTROL2_FORCE_RX_SIGDET_LFPS_MASK                                                    0x4
#define PCIE_USB3_PCS_TEST_CONTROL2_FORCE_RX_SIGDET_LFPS_SHFT                                                    0x2
#define PCIE_USB3_PCS_TEST_CONTROL2_FORCE_LFPS_DET_LPBCK_MASK                                                    0x2
#define PCIE_USB3_PCS_TEST_CONTROL2_FORCE_LFPS_DET_LPBCK_SHFT                                                    0x1
#define PCIE_USB3_PCS_TEST_CONTROL2_DSBL_EBUF_QUAL_MASK                                                          0x1
#define PCIE_USB3_PCS_TEST_CONTROL2_DSBL_EBUF_QUAL_SHFT                                                          0x0

/* TEST_CONTROL3 */
#define PCIE_USB3_PCS_TEST_CONTROL3                                                                              ( 0x918 )
#define PCIE_USB3_PCS_TEST_CONTROL3_DEBUG_CLK_IDX3_MASK                                                          0x80
#define PCIE_USB3_PCS_TEST_CONTROL3_DEBUG_CLK_IDX3_SHFT                                                          0x7
#define PCIE_USB3_PCS_TEST_CONTROL3_DEBUG_CLK_IDX2_MASK                                                          0x40
#define PCIE_USB3_PCS_TEST_CONTROL3_DEBUG_CLK_IDX2_SHFT                                                          0x6
#define PCIE_USB3_PCS_TEST_CONTROL3_DEBUG_CLK_IDX1_MASK                                                          0x20
#define PCIE_USB3_PCS_TEST_CONTROL3_DEBUG_CLK_IDX1_SHFT                                                          0x5
#define PCIE_USB3_PCS_TEST_CONTROL3_DEBUG_CLK_IDX0_MASK                                                          0x10
#define PCIE_USB3_PCS_TEST_CONTROL3_DEBUG_CLK_IDX0_SHFT                                                          0x4
#define PCIE_USB3_PCS_TEST_CONTROL3_USE_CLK_DEBUG_BUS3_MASK                                                      0x8
#define PCIE_USB3_PCS_TEST_CONTROL3_USE_CLK_DEBUG_BUS3_SHFT                                                      0x3
#define PCIE_USB3_PCS_TEST_CONTROL3_USE_CLK_DEBUG_BUS2_MASK                                                      0x4
#define PCIE_USB3_PCS_TEST_CONTROL3_USE_CLK_DEBUG_BUS2_SHFT                                                      0x2
#define PCIE_USB3_PCS_TEST_CONTROL3_USE_CLK_DEBUG_BUS1_MASK                                                      0x2
#define PCIE_USB3_PCS_TEST_CONTROL3_USE_CLK_DEBUG_BUS1_SHFT                                                      0x1
#define PCIE_USB3_PCS_TEST_CONTROL3_USE_CLK_DEBUG_BUS0_MASK                                                      0x1
#define PCIE_USB3_PCS_TEST_CONTROL3_USE_CLK_DEBUG_BUS0_SHFT                                                      0x0

/* TEST_CONTROL4 */
#define PCIE_USB3_PCS_TEST_CONTROL4                                                                              ( 0x91C )
#define PCIE_USB3_PCS_TEST_CONTROL4_DEBUG_BUS_IDX0_MASK                                                          0x7F
#define PCIE_USB3_PCS_TEST_CONTROL4_DEBUG_BUS_IDX0_SHFT                                                          0x0

/* TEST_CONTROL5 */
#define PCIE_USB3_PCS_TEST_CONTROL5                                                                              ( 0x920 )
#define PCIE_USB3_PCS_TEST_CONTROL5_DEBUG_BUS_IDX1_MASK                                                          0x7F
#define PCIE_USB3_PCS_TEST_CONTROL5_DEBUG_BUS_IDX1_SHFT                                                          0x0

/* TEST_CONTROL6 */
#define PCIE_USB3_PCS_TEST_CONTROL6                                                                              ( 0x924 )
#define PCIE_USB3_PCS_TEST_CONTROL6_DEBUG_BUS_IDX2_MASK                                                          0x7F
#define PCIE_USB3_PCS_TEST_CONTROL6_DEBUG_BUS_IDX2_SHFT                                                          0x0

/* TEST_CONTROL7 */
#define PCIE_USB3_PCS_TEST_CONTROL7                                                                              ( 0x928 )
#define PCIE_USB3_PCS_TEST_CONTROL7_DEBUG_BUS_IDX3_MASK                                                          0x7F
#define PCIE_USB3_PCS_TEST_CONTROL7_DEBUG_BUS_IDX3_SHFT                                                          0x0

/* COM_RESET_CONTROL */
#define PCIE_USB3_PCS_COM_RESET_CONTROL                                                                          ( 0x92C )
#define PCIE_USB3_PCS_COM_RESET_CONTROL_COMRST_FORCE_CLKREQ_LOW_L1SS_MASK                                        0x8
#define PCIE_USB3_PCS_COM_RESET_CONTROL_COMRST_FORCE_CLKREQ_LOW_L1SS_SHFT                                        0x3
#define PCIE_USB3_PCS_COM_RESET_CONTROL_COMRST_AUTO_ENTER_P2_MASK                                                0x4
#define PCIE_USB3_PCS_COM_RESET_CONTROL_COMRST_AUTO_ENTER_P2_SHFT                                                0x2
#define PCIE_USB3_PCS_COM_RESET_CONTROL_FORCE_COM_RESET_ACK_MASK                                                 0x2
#define PCIE_USB3_PCS_COM_RESET_CONTROL_FORCE_COM_RESET_ACK_SHFT                                                 0x1
#define PCIE_USB3_PCS_COM_RESET_CONTROL_FORCE_COM_RESET_REQ_MASK                                                 0x1
#define PCIE_USB3_PCS_COM_RESET_CONTROL_FORCE_COM_RESET_REQ_SHFT                                                 0x0

/* BIST_CTRL */
#define PCIE_USB3_PCS_BIST_CTRL                                                                                  ( 0x930 )
#define PCIE_USB3_PCS_BIST_CTRL_RSVD_MASK                                                                        0xC0
#define PCIE_USB3_PCS_BIST_CTRL_RSVD_SHFT                                                                        0x6
#define PCIE_USB3_PCS_BIST_CTRL_BIST_CHK_ERR_CNT_CLR_MASK                                                        0x20
#define PCIE_USB3_PCS_BIST_CTRL_BIST_CHK_ERR_CNT_CLR_SHFT                                                        0x5
#define PCIE_USB3_PCS_BIST_CTRL_BIST_RX_ERR_EN_MASK                                                              0x10
#define PCIE_USB3_PCS_BIST_CTRL_BIST_RX_ERR_EN_SHFT                                                              0x4
#define PCIE_USB3_PCS_BIST_CTRL_BIST_GEN_SHORT_PATTERN_MASK                                                      0x8
#define PCIE_USB3_PCS_BIST_CTRL_BIST_GEN_SHORT_PATTERN_SHFT                                                      0x3
#define PCIE_USB3_PCS_BIST_CTRL_BIST_PAT_SEL_MASK                                                                0x4
#define PCIE_USB3_PCS_BIST_CTRL_BIST_PAT_SEL_SHFT                                                                0x2
#define PCIE_USB3_PCS_BIST_CTRL_BIST_TXRX_EN_MASK                                                                0x2
#define PCIE_USB3_PCS_BIST_CTRL_BIST_TXRX_EN_SHFT                                                                0x1
#define PCIE_USB3_PCS_BIST_CTRL_BIST_TX_EN_MASK                                                                  0x1
#define PCIE_USB3_PCS_BIST_CTRL_BIST_TX_EN_SHFT                                                                  0x0

/* PRBS_POLY0 */
#define PCIE_USB3_PCS_PRBS_POLY0                                                                                 ( 0x934 )
#define PCIE_USB3_PCS_PRBS_POLY0_PRBS_POLY0_MASK                                                                 0xFF
#define PCIE_USB3_PCS_PRBS_POLY0_PRBS_POLY0_SHFT                                                                 0x0

/* PRBS_POLY1 */
#define PCIE_USB3_PCS_PRBS_POLY1                                                                                 ( 0x938 )
#define PCIE_USB3_PCS_PRBS_POLY1_PRBS_POLY1_MASK                                                                 0xFF
#define PCIE_USB3_PCS_PRBS_POLY1_PRBS_POLY1_SHFT                                                                 0x0

/* PRBS_SEED0 */
#define PCIE_USB3_PCS_PRBS_SEED0                                                                                 ( 0x93C )
#define PCIE_USB3_PCS_PRBS_SEED0_PRBS_SEED0_MASK                                                                 0xFF
#define PCIE_USB3_PCS_PRBS_SEED0_PRBS_SEED0_SHFT                                                                 0x0

/* PRBS_SEED1 */
#define PCIE_USB3_PCS_PRBS_SEED1                                                                                 ( 0x940 )
#define PCIE_USB3_PCS_PRBS_SEED1_PRBS_SEED1_MASK                                                                 0xFF
#define PCIE_USB3_PCS_PRBS_SEED1_PRBS_SEED1_SHFT                                                                 0x0

/* FIXED_PAT_CTRL */
#define PCIE_USB3_PCS_FIXED_PAT_CTRL                                                                             ( 0x944 )
#define PCIE_USB3_PCS_FIXED_PAT_CTRL_RSVD_MASK                                                                   0xF0
#define PCIE_USB3_PCS_FIXED_PAT_CTRL_RSVD_SHFT                                                                   0x4
#define PCIE_USB3_PCS_FIXED_PAT_CTRL_FIXED_PAT3_K_MASK                                                           0x8
#define PCIE_USB3_PCS_FIXED_PAT_CTRL_FIXED_PAT3_K_SHFT                                                           0x3
#define PCIE_USB3_PCS_FIXED_PAT_CTRL_FIXED_PAT2_K_MASK                                                           0x4
#define PCIE_USB3_PCS_FIXED_PAT_CTRL_FIXED_PAT2_K_SHFT                                                           0x2
#define PCIE_USB3_PCS_FIXED_PAT_CTRL_FIXED_PAT1_K_MASK                                                           0x2
#define PCIE_USB3_PCS_FIXED_PAT_CTRL_FIXED_PAT1_K_SHFT                                                           0x1
#define PCIE_USB3_PCS_FIXED_PAT_CTRL_FIXED_PAT0_K_MASK                                                           0x1
#define PCIE_USB3_PCS_FIXED_PAT_CTRL_FIXED_PAT0_K_SHFT                                                           0x0

/* FIXED_PAT0 */
#define PCIE_USB3_PCS_FIXED_PAT0                                                                                 ( 0x948 )
#define PCIE_USB3_PCS_FIXED_PAT0_FIXED_PAT0_MASK                                                                 0xFF
#define PCIE_USB3_PCS_FIXED_PAT0_FIXED_PAT0_SHFT                                                                 0x0

/* FIXED_PAT1 */
#define PCIE_USB3_PCS_FIXED_PAT1                                                                                 ( 0x94C )
#define PCIE_USB3_PCS_FIXED_PAT1_FIXED_PAT1_MASK                                                                 0xFF
#define PCIE_USB3_PCS_FIXED_PAT1_FIXED_PAT1_SHFT                                                                 0x0

/* FIXED_PAT2 */
#define PCIE_USB3_PCS_FIXED_PAT2                                                                                 ( 0x950 )
#define PCIE_USB3_PCS_FIXED_PAT2_FIXED_PAT2_MASK                                                                 0xFF
#define PCIE_USB3_PCS_FIXED_PAT2_FIXED_PAT2_SHFT                                                                 0x0

/* FIXED_PAT3 */
#define PCIE_USB3_PCS_FIXED_PAT3                                                                                 ( 0x954 )
#define PCIE_USB3_PCS_FIXED_PAT3_FIXED_PAT3_MASK                                                                 0xFF
#define PCIE_USB3_PCS_FIXED_PAT3_FIXED_PAT3_SHFT                                                                 0x0

/* COM_CLK_SWITCH_CTRL */
#define PCIE_USB3_PCS_COM_CLK_SWITCH_CTRL                                                                        ( 0x958 )
#define PCIE_USB3_PCS_COM_CLK_SWITCH_CTRL_COM_CLK_SWITCH_EN_MASK                                                 0x1
#define PCIE_USB3_PCS_COM_CLK_SWITCH_CTRL_COM_CLK_SWITCH_EN_SHFT                                                 0x0

/* ELECIDLE_DLY_SEL */
#define PCIE_USB3_PCS_ELECIDLE_DLY_SEL                                                                           ( 0x95C )
#define PCIE_USB3_PCS_ELECIDLE_DLY_SEL_ELECIDLE_DLY_SEL_MASK                                                     0x3
#define PCIE_USB3_PCS_ELECIDLE_DLY_SEL_ELECIDLE_DLY_SEL_SHFT                                                     0x0

/* SPARE1 */
#define PCIE_USB3_PCS_SPARE1                                                                                     ( 0x960 )
#define PCIE_USB3_PCS_SPARE1_RSVD_MASK                                                                           0xFF
#define PCIE_USB3_PCS_SPARE1_RSVD_SHFT                                                                           0x0

/* BIST_CHK_ERR_CNT_L_STATUS */
#define PCIE_USB3_PCS_BIST_CHK_ERR_CNT_L_STATUS                                                                  ( 0x964 )
#define PCIE_USB3_PCS_BIST_CHK_ERR_CNT_L_STATUS_BIST_CHK_ERR_CNT_L_MASK                                          0xFF
#define PCIE_USB3_PCS_BIST_CHK_ERR_CNT_L_STATUS_BIST_CHK_ERR_CNT_L_SHFT                                          0x0

/* BIST_CHK_ERR_CNT_H_STATUS */
#define PCIE_USB3_PCS_BIST_CHK_ERR_CNT_H_STATUS                                                                  ( 0x968 )
#define PCIE_USB3_PCS_BIST_CHK_ERR_CNT_H_STATUS_BIST_CHK_ERR_CNT_H_MASK                                          0xFF
#define PCIE_USB3_PCS_BIST_CHK_ERR_CNT_H_STATUS_BIST_CHK_ERR_CNT_H_SHFT                                          0x0

/* BIST_CHK_STATUS */
#define PCIE_USB3_PCS_BIST_CHK_STATUS                                                                            ( 0x96C )
#define PCIE_USB3_PCS_BIST_CHK_STATUS_BIST_CHK_HEADER_SEL_MASK                                                   0x8
#define PCIE_USB3_PCS_BIST_CHK_STATUS_BIST_CHK_HEADER_SEL_SHFT                                                   0x3
#define PCIE_USB3_PCS_BIST_CHK_STATUS_BIST_HEADER_NOT_FOUND_MASK                                                 0x4
#define PCIE_USB3_PCS_BIST_CHK_STATUS_BIST_HEADER_NOT_FOUND_SHFT                                                 0x2
#define PCIE_USB3_PCS_BIST_CHK_STATUS_BIST_PAT_DATA_NOT_FOUND_MASK                                               0x2
#define PCIE_USB3_PCS_BIST_CHK_STATUS_BIST_PAT_DATA_NOT_FOUND_SHFT                                               0x1
#define PCIE_USB3_PCS_BIST_CHK_STATUS_BIST_CHECK_DONE_MASK                                                       0x1
#define PCIE_USB3_PCS_BIST_CHK_STATUS_BIST_CHECK_DONE_SHFT                                                       0x0

/* LFPS_RXTERM_IRQ_SOURCE_STATUS */
#define PCIE_USB3_PCS_LFPS_RXTERM_IRQ_SOURCE_STATUS                                                              ( 0x970 )
#define PCIE_USB3_PCS_LFPS_RXTERM_IRQ_SOURCE_STATUS_CLR_IRQ_BUSY_MASK                                            0x4
#define PCIE_USB3_PCS_LFPS_RXTERM_IRQ_SOURCE_STATUS_CLR_IRQ_BUSY_SHFT                                            0x2
#define PCIE_USB3_PCS_LFPS_RXTERM_IRQ_SOURCE_STATUS_LFPS_DETECT_IRQ_MASK                                         0x2
#define PCIE_USB3_PCS_LFPS_RXTERM_IRQ_SOURCE_STATUS_LFPS_DETECT_IRQ_SHFT                                         0x1
#define PCIE_USB3_PCS_LFPS_RXTERM_IRQ_SOURCE_STATUS_RCVR_DETECT_IRQ_MASK                                         0x1
#define PCIE_USB3_PCS_LFPS_RXTERM_IRQ_SOURCE_STATUS_RCVR_DETECT_IRQ_SHFT                                         0x0

/* PCS_STATUS */
#define PCIE_USB3_PCS_PCS_STATUS                                                                                 ( 0x974 )
#define PCIE_USB3_PCS_PCS_STATUS_ACTIVE_STATE_STATUS_MASK                                                        0x80
#define PCIE_USB3_PCS_PCS_STATUS_ACTIVE_STATE_STATUS_SHFT                                                        0x7
#define PCIE_USB3_PCS_PCS_STATUS_PHYSTATUS_MASK                                                                  0x40
#define PCIE_USB3_PCS_PCS_STATUS_PHYSTATUS_SHFT                                                                  0x6
#define PCIE_USB3_PCS_PCS_STATUS_PCS_POWERDOWN_MASK                                                              0x30
#define PCIE_USB3_PCS_PCS_STATUS_PCS_POWERDOWN_SHFT                                                              0x4
#define PCIE_USB3_PCS_PCS_STATUS_FREEZE_POWERDOWN_MASK                                                           0x8
#define PCIE_USB3_PCS_PCS_STATUS_FREEZE_POWERDOWN_SHFT                                                           0x3
#define PCIE_USB3_PCS_PCS_STATUS_POWER_STATE_MASK                                                                0x7
#define PCIE_USB3_PCS_PCS_STATUS_POWER_STATE_SHFT                                                                0x0

/* PCS_STATUS2 */
#define PCIE_USB3_PCS_PCS_STATUS2                                                                                ( 0x978 )
#define PCIE_USB3_PCS_PCS_STATUS2_POWERPRESENT_MASK                                                              0x80
#define PCIE_USB3_PCS_PCS_STATUS2_POWERPRESENT_SHFT                                                              0x7
#define PCIE_USB3_PCS_PCS_STATUS2_RX_EQUALIZATION_IN_PROGRESS_MASK                                               0x40
#define PCIE_USB3_PCS_PCS_STATUS2_RX_EQUALIZATION_IN_PROGRESS_SHFT                                               0x6
#define PCIE_USB3_PCS_PCS_STATUS2_PCS_LFPS_DET_MASK                                                              0x20
#define PCIE_USB3_PCS_PCS_STATUS2_PCS_LFPS_DET_SHFT                                                              0x5
#define PCIE_USB3_PCS_PCS_STATUS2_FLL_CLK_EN_MASK                                                                0x10
#define PCIE_USB3_PCS_PCS_STATUS2_FLL_CLK_EN_SHFT                                                                0x4
#define PCIE_USB3_PCS_PCS_STATUS2_L1SS_MODE_MASK                                                                 0x8
#define PCIE_USB3_PCS_PCS_STATUS2_L1SS_MODE_SHFT                                                                 0x3
#define PCIE_USB3_PCS_PCS_STATUS2_RX_SAMPLER_CAL_IN_PROGRESS_MASK                                                0x4
#define PCIE_USB3_PCS_PCS_STATUS2_RX_SAMPLER_CAL_IN_PROGRESS_SHFT                                                0x2
#define PCIE_USB3_PCS_PCS_STATUS2_REC_DETECT_DONE_MASK                                                           0x2
#define PCIE_USB3_PCS_PCS_STATUS2_REC_DETECT_DONE_SHFT                                                           0x1
#define PCIE_USB3_PCS_PCS_STATUS2_REC_DETECT_OUTCOME_MASK                                                        0x1
#define PCIE_USB3_PCS_PCS_STATUS2_REC_DETECT_OUTCOME_SHFT                                                        0x0

/* PCS_STATUS3 */
#define PCIE_USB3_PCS_PCS_STATUS3                                                                                ( 0x97C )
#define PCIE_USB3_PCS_PCS_STATUS3_RX_SAMPLER_CAL_DONE_MASK                                                       0x8
#define PCIE_USB3_PCS_PCS_STATUS3_RX_SAMPLER_CAL_DONE_SHFT                                                       0x3
#define PCIE_USB3_PCS_PCS_STATUS3_PIPE_CLK_EN_MASK                                                               0x4
#define PCIE_USB3_PCS_PCS_STATUS3_PIPE_CLK_EN_SHFT                                                               0x2
#define PCIE_USB3_PCS_PCS_STATUS3_PCS_RATE_MASK                                                                  0x2
#define PCIE_USB3_PCS_PCS_STATUS3_PCS_RATE_SHFT                                                                  0x1
#define PCIE_USB3_PCS_PCS_STATUS3_SYSCLK_ENABLED_MASK                                                            0x1
#define PCIE_USB3_PCS_PCS_STATUS3_SYSCLK_ENABLED_SHFT                                                            0x0

/* COM_RESET_STATUS */
#define PCIE_USB3_PCS_COM_RESET_STATUS                                                                           ( 0x980 )
#define PCIE_USB3_PCS_COM_RESET_STATUS_COM_RESET_INITIATED_MASK                                                  0x4
#define PCIE_USB3_PCS_COM_RESET_STATUS_COM_RESET_INITIATED_SHFT                                                  0x2
#define PCIE_USB3_PCS_COM_RESET_STATUS_COM_RESET_ACK_MASK                                                        0x2
#define PCIE_USB3_PCS_COM_RESET_STATUS_COM_RESET_ACK_SHFT                                                        0x1
#define PCIE_USB3_PCS_COM_RESET_STATUS_COM_RESET_REQ_MASK                                                        0x1
#define PCIE_USB3_PCS_COM_RESET_STATUS_COM_RESET_REQ_SHFT                                                        0x0

/* OSC_DTCT_STATUS */
#define PCIE_USB3_PCS_OSC_DTCT_STATUS                                                                            ( 0x984 )
#define PCIE_USB3_PCS_OSC_DTCT_STATUS_SYSCLK_ACTIVE_MASK                                                         0x1
#define PCIE_USB3_PCS_OSC_DTCT_STATUS_SYSCLK_ACTIVE_SHFT                                                         0x0

/* REVISION_ID0 */
#define PCIE_USB3_PCS_REVISION_ID0                                                                               ( 0x988 )
#define PCIE_USB3_PCS_REVISION_ID0_STEP_7_0_MASK                                                                 0xFF
#define PCIE_USB3_PCS_REVISION_ID0_STEP_7_0_SHFT                                                                 0x0

/* REVISION_ID1 */
#define PCIE_USB3_PCS_REVISION_ID1                                                                               ( 0x98C )
#define PCIE_USB3_PCS_REVISION_ID1_STEP_15_8_MASK                                                                0xFF
#define PCIE_USB3_PCS_REVISION_ID1_STEP_15_8_SHFT                                                                0x0

/* REVISION_ID2 */
#define PCIE_USB3_PCS_REVISION_ID2                                                                               ( 0x990 )
#define PCIE_USB3_PCS_REVISION_ID2_MINOR_7_0_MASK                                                                0xFF
#define PCIE_USB3_PCS_REVISION_ID2_MINOR_7_0_SHFT                                                                0x0

/* REVISION_ID3 */
#define PCIE_USB3_PCS_REVISION_ID3                                                                               ( 0x994 )
#define PCIE_USB3_PCS_REVISION_ID3_MAJOR_MASK                                                                    0xF0
#define PCIE_USB3_PCS_REVISION_ID3_MAJOR_SHFT                                                                    0x4
#define PCIE_USB3_PCS_REVISION_ID3_MINOR_11_8_MASK                                                               0xF
#define PCIE_USB3_PCS_REVISION_ID3_MINOR_11_8_SHFT                                                               0x0

/* DEBUG_BUS_0_STATUS */
#define PCIE_USB3_PCS_DEBUG_BUS_0_STATUS                                                                         ( 0x998 )
#define PCIE_USB3_PCS_DEBUG_BUS_0_STATUS_DEBUG_BUS_0_STATUS_MASK                                                 0xFF
#define PCIE_USB3_PCS_DEBUG_BUS_0_STATUS_DEBUG_BUS_0_STATUS_SHFT                                                 0x0

/* DEBUG_BUS_1_STATUS */
#define PCIE_USB3_PCS_DEBUG_BUS_1_STATUS                                                                         ( 0x99C )
#define PCIE_USB3_PCS_DEBUG_BUS_1_STATUS_DEBUG_BUS_1_STATUS_MASK                                                 0xFF
#define PCIE_USB3_PCS_DEBUG_BUS_1_STATUS_DEBUG_BUS_1_STATUS_SHFT                                                 0x0

/* DEBUG_BUS_2_STATUS */
#define PCIE_USB3_PCS_DEBUG_BUS_2_STATUS                                                                         ( 0x9A0 )
#define PCIE_USB3_PCS_DEBUG_BUS_2_STATUS_DEBUG_BUS_2_STATUS_MASK                                                 0xFF
#define PCIE_USB3_PCS_DEBUG_BUS_2_STATUS_DEBUG_BUS_2_STATUS_SHFT                                                 0x0

/* DEBUG_BUS_3_STATUS */
#define PCIE_USB3_PCS_DEBUG_BUS_3_STATUS                                                                         ( 0x9A4 )
#define PCIE_USB3_PCS_DEBUG_BUS_3_STATUS_DEBUG_BUS_3_STATUS_MASK                                                 0xFF
#define PCIE_USB3_PCS_DEBUG_BUS_3_STATUS_DEBUG_BUS_3_STATUS_SHFT                                                 0x0

/* LP_WAKEUP_DLY_TIME_AUXCLK_MSB */
#define PCIE_USB3_PCS_LP_WAKEUP_DLY_TIME_AUXCLK_MSB                                                              ( 0x9A8 )
#define PCIE_USB3_PCS_LP_WAKEUP_DLY_TIME_AUXCLK_MSB_LP_WAKEUP_DLY_TIME_AUXCLK_MSB_MASK                           0x1F
#define PCIE_USB3_PCS_LP_WAKEUP_DLY_TIME_AUXCLK_MSB_LP_WAKEUP_DLY_TIME_AUXCLK_MSB_SHFT                           0x0

/* OSC_DTCT_ACTIONS */
#define PCIE_USB3_PCS_OSC_DTCT_ACTIONS                                                                           ( 0x9AC )
#define PCIE_USB3_PCS_OSC_DTCT_ACTIONS_P2U3_EXIT_PLL_EN_WAIT_MASK                                                0x4
#define PCIE_USB3_PCS_OSC_DTCT_ACTIONS_P2U3_EXIT_PLL_EN_WAIT_SHFT                                                0x2
#define PCIE_USB3_PCS_OSC_DTCT_ACTIONS_L1SS_EXIT_PLL_EN_WAIT_MASK                                                0x2
#define PCIE_USB3_PCS_OSC_DTCT_ACTIONS_L1SS_EXIT_PLL_EN_WAIT_SHFT                                                0x1
#define PCIE_USB3_PCS_OSC_DTCT_ACTIONS_PWRUP_PLL_EN_WAIT_MASK                                                    0x1
#define PCIE_USB3_PCS_OSC_DTCT_ACTIONS_PWRUP_PLL_EN_WAIT_SHFT                                                    0x0

/* SIGDET_CNTRL */
#define PCIE_USB3_PCS_SIGDET_CNTRL                                                                               ( 0x9B0 )
#define PCIE_USB3_PCS_SIGDET_CNTRL_PRE_L1SS_MASK_SIGDET_EN_MASK                                                  0x8
#define PCIE_USB3_PCS_SIGDET_CNTRL_PRE_L1SS_MASK_SIGDET_EN_SHFT                                                  0x3
#define PCIE_USB3_PCS_SIGDET_CNTRL_P2_MASK_DSBL_SIGDET_MASK                                                      0x4
#define PCIE_USB3_PCS_SIGDET_CNTRL_P2_MASK_DSBL_SIGDET_SHFT                                                      0x2
#define PCIE_USB3_PCS_SIGDET_CNTRL_PRE_P2_MASK_SIGDET_EN_MASK                                                    0x2
#define PCIE_USB3_PCS_SIGDET_CNTRL_PRE_P2_MASK_SIGDET_EN_SHFT                                                    0x1
#define PCIE_USB3_PCS_SIGDET_CNTRL_P2_MASK_SIGDET_EN_MASK                                                        0x1
#define PCIE_USB3_PCS_SIGDET_CNTRL_P2_MASK_SIGDET_EN_SHFT                                                        0x0

/* IDAC_CAL_CNTRL */
#define PCIE_USB3_PCS_IDAC_CAL_CNTRL                                                                             ( 0x9B4 )
#define PCIE_USB3_PCS_IDAC_CAL_CNTRL_FORCE_IDAC_INITIAL_CAL_MASK                                                 0x2
#define PCIE_USB3_PCS_IDAC_CAL_CNTRL_FORCE_IDAC_INITIAL_CAL_SHFT                                                 0x1
#define PCIE_USB3_PCS_IDAC_CAL_CNTRL_FORCE_IDAC_RECAL_MASK                                                       0x1
#define PCIE_USB3_PCS_IDAC_CAL_CNTRL_FORCE_IDAC_RECAL_SHFT                                                       0x0

/* CMN_ACK_OUT_SEL */
#define PCIE_USB3_PCS_CMN_ACK_OUT_SEL                                                                            ( 0x9B8 )
#define PCIE_USB3_PCS_CMN_ACK_OUT_SEL_CMN_ACK_OUT_SEL_MASK                                                       0x3
#define PCIE_USB3_PCS_CMN_ACK_OUT_SEL_CMN_ACK_OUT_SEL_SHFT                                                       0x0

/* PLL_LOCK_CHK_DLY_TIME_SYSCLK */
#define PCIE_USB3_PCS_PLL_LOCK_CHK_DLY_TIME_SYSCLK                                                               ( 0x9BC )
#define PCIE_USB3_PCS_PLL_LOCK_CHK_DLY_TIME_SYSCLK_PLL_LOCK_CHK_DLY_TIME_SYSCLK_MASK                             0xFF
#define PCIE_USB3_PCS_PLL_LOCK_CHK_DLY_TIME_SYSCLK_PLL_LOCK_CHK_DLY_TIME_SYSCLK_SHFT                             0x0

/* AUTONOMOUS_MODE_STATUS */
#define PCIE_USB3_PCS_AUTONOMOUS_MODE_STATUS                                                                     ( 0x9C0 )
#define PCIE_USB3_PCS_AUTONOMOUS_MODE_STATUS_ALFPS_DTCT_EN_MASK                                                  0x2
#define PCIE_USB3_PCS_AUTONOMOUS_MODE_STATUS_ALFPS_DTCT_EN_SHFT                                                  0x1
#define PCIE_USB3_PCS_AUTONOMOUS_MODE_STATUS_ARCVR_DTCT_EN_MASK                                                  0x1
#define PCIE_USB3_PCS_AUTONOMOUS_MODE_STATUS_ARCVR_DTCT_EN_SHFT                                                  0x0

/* ENDPOINT_REFCLK_CNTRL */
#define PCIE_USB3_PCS_ENDPOINT_REFCLK_CNTRL                                                                      ( 0x9C4 )
#define PCIE_USB3_PCS_ENDPOINT_REFCLK_CNTRL_EPCLK_ALWAYS_ON_EN_MASK                                              0x40
#define PCIE_USB3_PCS_ENDPOINT_REFCLK_CNTRL_EPCLK_ALWAYS_ON_EN_SHFT                                              0x6
#define PCIE_USB3_PCS_ENDPOINT_REFCLK_CNTRL_EPCLK_P2_DLY_EN_MASK                                                 0x20
#define PCIE_USB3_PCS_ENDPOINT_REFCLK_CNTRL_EPCLK_P2_DLY_EN_SHFT                                                 0x5
#define PCIE_USB3_PCS_ENDPOINT_REFCLK_CNTRL_EPCLK_P2_PRE_PLL_LOCK_EN_MASK                                        0x10
#define PCIE_USB3_PCS_ENDPOINT_REFCLK_CNTRL_EPCLK_P2_PRE_PLL_LOCK_EN_SHFT                                        0x4
#define PCIE_USB3_PCS_ENDPOINT_REFCLK_CNTRL_EPCLK_L1P2_DLY_EN_MASK                                               0x8
#define PCIE_USB3_PCS_ENDPOINT_REFCLK_CNTRL_EPCLK_L1P2_DLY_EN_SHFT                                               0x3
#define PCIE_USB3_PCS_ENDPOINT_REFCLK_CNTRL_EPCLK_L1P2_PRE_PLL_LOCK_EN_MASK                                      0x4
#define PCIE_USB3_PCS_ENDPOINT_REFCLK_CNTRL_EPCLK_L1P2_PRE_PLL_LOCK_EN_SHFT                                      0x2
#define PCIE_USB3_PCS_ENDPOINT_REFCLK_CNTRL_EPCLK_L1P1_DLY_EN_MASK                                               0x2
#define PCIE_USB3_PCS_ENDPOINT_REFCLK_CNTRL_EPCLK_L1P1_DLY_EN_SHFT                                               0x1
#define PCIE_USB3_PCS_ENDPOINT_REFCLK_CNTRL_EPCLK_L1P1_PRE_PLL_LOCK_EN_MASK                                      0x1
#define PCIE_USB3_PCS_ENDPOINT_REFCLK_CNTRL_EPCLK_L1P1_PRE_PLL_LOCK_EN_SHFT                                      0x0

/* EPCLK_PRE_PLL_LOCK_DLY_SYSCLK */
#define PCIE_USB3_PCS_EPCLK_PRE_PLL_LOCK_DLY_SYSCLK                                                              ( 0x9C8 )
#define PCIE_USB3_PCS_EPCLK_PRE_PLL_LOCK_DLY_SYSCLK_EPCLK_PRE_PLL_LOCK_DLY_SYSCLK_MASK                           0xFF
#define PCIE_USB3_PCS_EPCLK_PRE_PLL_LOCK_DLY_SYSCLK_EPCLK_PRE_PLL_LOCK_DLY_SYSCLK_SHFT                           0x0

/* EPCLK_PRE_PLL_LOCK_DLY_AUXCLK */
#define PCIE_USB3_PCS_EPCLK_PRE_PLL_LOCK_DLY_AUXCLK                                                              ( 0x9CC )
#define PCIE_USB3_PCS_EPCLK_PRE_PLL_LOCK_DLY_AUXCLK_EPCLK_PRE_PLL_LOCK_DLY_AUXCLK_MASK                           0xFF
#define PCIE_USB3_PCS_EPCLK_PRE_PLL_LOCK_DLY_AUXCLK_EPCLK_PRE_PLL_LOCK_DLY_AUXCLK_SHFT                           0x0

/* EPCLK_DLY_COUNT_VAL_L */
#define PCIE_USB3_PCS_EPCLK_DLY_COUNT_VAL_L                                                                      ( 0x9D0 )
#define PCIE_USB3_PCS_EPCLK_DLY_COUNT_VAL_L_EPCLK_DLY_COUNT_VAL_7_0_MASK                                         0xFF
#define PCIE_USB3_PCS_EPCLK_DLY_COUNT_VAL_L_EPCLK_DLY_COUNT_VAL_7_0_SHFT                                         0x0

/* EPCLK_DLY_COUNT_VAL_H */
#define PCIE_USB3_PCS_EPCLK_DLY_COUNT_VAL_H                                                                      ( 0x9D4 )
#define PCIE_USB3_PCS_EPCLK_DLY_COUNT_VAL_H_EPCLK_DLY_COUNT_VAL_11_8_MASK                                        0xF
#define PCIE_USB3_PCS_EPCLK_DLY_COUNT_VAL_H_EPCLK_DLY_COUNT_VAL_11_8_SHFT                                        0x0

/* RX_SIGDET_LVL */
#define PCIE_USB3_PCS_RX_SIGDET_LVL                                                                              ( 0x9D8 )
#define PCIE_USB3_PCS_RX_SIGDET_LVL_RX_SIGDET_LVL_LP_MASK                                                        0xF0
#define PCIE_USB3_PCS_RX_SIGDET_LVL_RX_SIGDET_LVL_LP_SHFT                                                        0x4
#define PCIE_USB3_PCS_RX_SIGDET_LVL_RX_SIGDET_LVL_NORMAL_MASK                                                    0xF
#define PCIE_USB3_PCS_RX_SIGDET_LVL_RX_SIGDET_LVL_NORMAL_SHFT                                                    0x0

/* L1SS_WAKEUP_DLY_TIME_AUXCLK_LSB */
#define PCIE_USB3_PCS_L1SS_WAKEUP_DLY_TIME_AUXCLK_LSB                                                            ( 0x9DC )
#define PCIE_USB3_PCS_L1SS_WAKEUP_DLY_TIME_AUXCLK_LSB_L1SS_WAKEUP_DLY_TIME_AUXCLK_7_0_MASK                       0xFF
#define PCIE_USB3_PCS_L1SS_WAKEUP_DLY_TIME_AUXCLK_LSB_L1SS_WAKEUP_DLY_TIME_AUXCLK_7_0_SHFT                       0x0

/* L1SS_WAKEUP_DLY_TIME_AUXCLK_MSB */
#define PCIE_USB3_PCS_L1SS_WAKEUP_DLY_TIME_AUXCLK_MSB                                                            ( 0x9E0 )
#define PCIE_USB3_PCS_L1SS_WAKEUP_DLY_TIME_AUXCLK_MSB_L1SS_WAKEUP_DLY_TIME_AUXCLK_12_8_MASK                      0x1F
#define PCIE_USB3_PCS_L1SS_WAKEUP_DLY_TIME_AUXCLK_MSB_L1SS_WAKEUP_DLY_TIME_AUXCLK_12_8_SHFT                      0x0

/* AUTONOMOUS_MODE_CTRL2 */
#define PCIE_USB3_PCS_AUTONOMOUS_MODE_CTRL2                                                                      ( 0x9E4 )
#define PCIE_USB3_PCS_AUTONOMOUS_MODE_CTRL2_AUTO_MODE_MASK_RXELECIDLE_MASK                                       0x2
#define PCIE_USB3_PCS_AUTONOMOUS_MODE_CTRL2_AUTO_MODE_MASK_RXELECIDLE_SHFT                                       0x1
#define PCIE_USB3_PCS_AUTONOMOUS_MODE_CTRL2_AUTO_MODE_IGNORE_PIPE_SIGS_MASK                                      0x1
#define PCIE_USB3_PCS_AUTONOMOUS_MODE_CTRL2_AUTO_MODE_IGNORE_PIPE_SIGS_SHFT                                      0x0

/* RXTERMINATION_DLY_SEL */
#define PCIE_USB3_PCS_RXTERMINATION_DLY_SEL                                                                      ( 0x9E8 )
#define PCIE_USB3_PCS_RXTERMINATION_DLY_SEL_RXTERMINATION_DLY_SEL_MASK                                           0x3
#define PCIE_USB3_PCS_RXTERMINATION_DLY_SEL_RXTERMINATION_DLY_SEL_SHFT                                           0x0

/* LFPS_PER_TIMER_VAL */
#define PCIE_USB3_PCS_LFPS_PER_TIMER_VAL                                                                         ( 0x9EC )
#define PCIE_USB3_PCS_LFPS_PER_TIMER_VAL_LFPS_PER_TIMER_VAL_MASK                                                 0xFF
#define PCIE_USB3_PCS_LFPS_PER_TIMER_VAL_LFPS_PER_TIMER_VAL_SHFT                                                 0x0

/* SIGDET_STARTUP_TIMER_VAL */
#define PCIE_USB3_PCS_SIGDET_STARTUP_TIMER_VAL                                                                   ( 0x9F0 )
#define PCIE_USB3_PCS_SIGDET_STARTUP_TIMER_VAL_SIGDET_STARTUP_TIMER_VAL_MASK                                     0xFF
#define PCIE_USB3_PCS_SIGDET_STARTUP_TIMER_VAL_SIGDET_STARTUP_TIMER_VAL_SHFT                                     0x0

/* LOCK_DETECT_CONFIG4 */
#define PCIE_USB3_PCS_LOCK_DETECT_CONFIG4                                                                        ( 0x9F4 )
#define PCIE_USB3_PCS_LOCK_DETECT_CONFIG4_GEN2_LOCK_WDT_EN_MASK                                                  0x80
#define PCIE_USB3_PCS_LOCK_DETECT_CONFIG4_GEN2_LOCK_WDT_EN_SHFT                                                  0x7
#define PCIE_USB3_PCS_LOCK_DETECT_CONFIG4_GEN1_LOCK_WDT_EN_MASK                                                  0x40
#define PCIE_USB3_PCS_LOCK_DETECT_CONFIG4_GEN1_LOCK_WDT_EN_SHFT                                                  0x6
#define PCIE_USB3_PCS_LOCK_DETECT_CONFIG4_GEN12_LOCK_WDT_VAL_MASK                                                0x3F
#define PCIE_USB3_PCS_LOCK_DETECT_CONFIG4_GEN12_LOCK_WDT_VAL_SHFT                                                0x0


/*============================================================================
     PCIE_MHI
============================================================================*/
     
#define PCIE_MHI_BASE_PHYS                                                              ( 0x87000 )
#define PCIE_MHI_SIZE                                                                   ( 0x1000 )

/* PCIE20_MHI_MMIO_CTRL_INT_STATUS_Q6 */
#define PCIE20_MHI_MMIO_CTRL_INT_STATUS_Q6                                              ( 0x0 )
#define PCIE20_MHI_MMIO_CTRL_INT_STATUS_Q6_CRDB_STATUS_MASK                             0x2
#define PCIE20_MHI_MMIO_CTRL_INT_STATUS_Q6_CRDB_STATUS_SHFT                             0x1
#define PCIE20_MHI_MMIO_CTRL_INT_STATUS_Q6_MHICTRL_STATUS_MASK                          0x1
#define PCIE20_MHI_MMIO_CTRL_INT_STATUS_Q6_MHICTRL_STATUS_SHFT                          0x0

/* PCIE20_MHI_MMIO_CTRL_INT_STATUS_A7 */
#define PCIE20_MHI_MMIO_CTRL_INT_STATUS_A7                                              ( 0x4 )
#define PCIE20_MHI_MMIO_CTRL_INT_STATUS_A7_CRDB_STATUS_MASK                             0x2
#define PCIE20_MHI_MMIO_CTRL_INT_STATUS_A7_CRDB_STATUS_SHFT                             0x1
#define PCIE20_MHI_MMIO_CTRL_INT_STATUS_A7_MHICTRL_STATUS_MASK                          0x1
#define PCIE20_MHI_MMIO_CTRL_INT_STATUS_A7_MHICTRL_STATUS_SHFT                          0x0

/* PCIE20_MHI_CHDB_INT_STATUS_Q6_n */
#define PCIE20_MHI_CHDB_INT_STATUS_Q6_n(n)                                              ( 0x8 + 0x4 * (n) )
#define PCIE20_MHI_CHDB_INT_STATUS_Q6_n_STATUS_MASK                                     0xFFFFFFFF
#define PCIE20_MHI_CHDB_INT_STATUS_Q6_n_STATUS_SHFT                                     0x0

/* PCIE20_MHI_ERDB_INT_STATUS_Q6_n */
#define PCIE20_MHI_ERDB_INT_STATUS_Q6_n(n)                                              ( 0x18 + 0x4 * (n) )
#define PCIE20_MHI_ERDB_INT_STATUS_Q6_n_STATUS_MASK                                     0xFFFFFFFF
#define PCIE20_MHI_ERDB_INT_STATUS_Q6_n_STATUS_SHFT                                     0x0

/* PCIE20_MHI_CHDB_INT_STATUS_A7_n */
#define PCIE20_MHI_CHDB_INT_STATUS_A7_n(n)                                              ( 0x28 + 0x4 * (n) )
#define PCIE20_MHI_CHDB_INT_STATUS_A7_n_STATUS_MASK                                     0xFFFFFFFF
#define PCIE20_MHI_CHDB_INT_STATUS_A7_n_STATUS_SHFT                                     0x0

/* PCIE20_MHI_ERDB_INT_STATUS_A7_n */
#define PCIE20_MHI_ERDB_INT_STATUS_A7_n(n)                                              ( 0x38 + 0x4 * (n) )
#define PCIE20_MHI_ERDB_INT_STATUS_A7_n_STATUS_MASK                                     0xFFFFFFFF
#define PCIE20_MHI_ERDB_INT_STATUS_A7_n_STATUS_SHFT                                     0x0

/* PCIE20_MHI_MMIO_CTRL_INT_CLEAR_Q6 */
#define PCIE20_MHI_MMIO_CTRL_INT_CLEAR_Q6                                               ( 0x48 )
#define PCIE20_MHI_MMIO_CTRL_INT_CLEAR_Q6_CRDB_CLEAR_MASK                               0x2
#define PCIE20_MHI_MMIO_CTRL_INT_CLEAR_Q6_CRDB_CLEAR_SHFT                               0x1
#define PCIE20_MHI_MMIO_CTRL_INT_CLEAR_Q6_MHICTRL_CLEAR_MASK                            0x1
#define PCIE20_MHI_MMIO_CTRL_INT_CLEAR_Q6_MHICTRL_CLEAR_SHFT                            0x0

/* PCIE20_MHI_MMIO_CTRL_INT_CLEAR_A7 */
#define PCIE20_MHI_MMIO_CTRL_INT_CLEAR_A7                                               ( 0x4C )
#define PCIE20_MHI_MMIO_CTRL_INT_CLEAR_A7_CRDB_CLEAR_MASK                               0x2
#define PCIE20_MHI_MMIO_CTRL_INT_CLEAR_A7_CRDB_CLEAR_SHFT                               0x1
#define PCIE20_MHI_MMIO_CTRL_INT_CLEAR_A7_MHICTRL_CLEAR_MASK                            0x1
#define PCIE20_MHI_MMIO_CTRL_INT_CLEAR_A7_MHICTRL_CLEAR_SHFT                            0x0

/* PCIE20_MHI_CHDB_INT_CLEAR_Q6_n */
#define PCIE20_MHI_CHDB_INT_CLEAR_Q6_n(n)                                               ( 0x50 + 0x4 * (n) )
#define PCIE20_MHI_CHDB_INT_CLEAR_Q6_n_CLEAR_MASK                                       0xFFFFFFFF
#define PCIE20_MHI_CHDB_INT_CLEAR_Q6_n_CLEAR_SHFT                                       0x0

/* PCIE20_MHI_ERDB_INT_CLEAR_Q6_n */
#define PCIE20_MHI_ERDB_INT_CLEAR_Q6_n(n)                                               ( 0x60 + 0x4 * (n) )
#define PCIE20_MHI_ERDB_INT_CLEAR_Q6_n_CLEAR_MASK                                       0xFFFFFFFF
#define PCIE20_MHI_ERDB_INT_CLEAR_Q6_n_CLEAR_SHFT                                       0x0

/* PCIE20_MHI_CHDB_INT_CLEAR_A7_n */
#define PCIE20_MHI_CHDB_INT_CLEAR_A7_n(n)                                               ( 0x70 + 0x4 * (n) )
#define PCIE20_MHI_CHDB_INT_CLEAR_A7_n_CLEAR_MASK                                       0xFFFFFFFF
#define PCIE20_MHI_CHDB_INT_CLEAR_A7_n_CLEAR_SHFT                                       0x0

/* PCIE20_MHI_ERDB_INT_CLEAR_A7_n */
#define PCIE20_MHI_ERDB_INT_CLEAR_A7_n(n)                                               ( 0x80 + 0x4 * (n) )
#define PCIE20_MHI_ERDB_INT_CLEAR_A7_n_CLEAR_MASK                                       0xFFFFFFFF
#define PCIE20_MHI_ERDB_INT_CLEAR_A7_n_CLEAR_SHFT                                       0x0

/* PCIE20_MHI_MMIO_CTRL_INT_MASK_Q6 */
#define PCIE20_MHI_MMIO_CTRL_INT_MASK_Q6                                                ( 0x90 )
#define PCIE20_MHI_MMIO_CTRL_INT_MASK_Q6_CRDB_MASK_MASK                                 0x2
#define PCIE20_MHI_MMIO_CTRL_INT_MASK_Q6_CRDB_MASK_SHFT                                 0x1
#define PCIE20_MHI_MMIO_CTRL_INT_MASK_Q6_MHICTRL_MASK_MASK                              0x1
#define PCIE20_MHI_MMIO_CTRL_INT_MASK_Q6_MHICTRL_MASK_SHFT                              0x0

/* PCIE20_MHI_MMIO_CTRL_INT_MASK_A7 */
#define PCIE20_MHI_MMIO_CTRL_INT_MASK_A7                                                ( 0x94 )
#define PCIE20_MHI_MMIO_CTRL_INT_MASK_A7_CRDB_MASK_MASK                                 0x2
#define PCIE20_MHI_MMIO_CTRL_INT_MASK_A7_CRDB_MASK_SHFT                                 0x1
#define PCIE20_MHI_MMIO_CTRL_INT_MASK_A7_MHICTRL_MASK_MASK                              0x1
#define PCIE20_MHI_MMIO_CTRL_INT_MASK_A7_MHICTRL_MASK_SHFT                              0x0

/* PCIE20_MHI_CHDB_INT_MASK_Q6_n */
#define PCIE20_MHI_CHDB_INT_MASK_Q6_n(n)                                                ( 0x98 + 0x4 * (n) )
#define PCIE20_MHI_CHDB_INT_MASK_Q6_n_MASK_MASK                                         0xFFFFFFFF
#define PCIE20_MHI_CHDB_INT_MASK_Q6_n_MASK_SHFT                                         0x0

/* PCIE20_MHI_ERDB_INT_MASK_Q6_n */
#define PCIE20_MHI_ERDB_INT_MASK_Q6_n(n)                                                ( 0xA8 + 0x4 * (n) )
#define PCIE20_MHI_ERDB_INT_MASK_Q6_n_MASK_MASK                                         0xFFFFFFFF
#define PCIE20_MHI_ERDB_INT_MASK_Q6_n_MASK_SHFT                                         0x0

/* PCIE20_MHI_CHDB_INT_MASK_A7_n */
#define PCIE20_MHI_CHDB_INT_MASK_A7_n(n)                                                ( 0xB8 + 0x4 * (n) )
#define PCIE20_MHI_CHDB_INT_MASK_A7_n_MASK_MASK                                         0xFFFFFFFF
#define PCIE20_MHI_CHDB_INT_MASK_A7_n_MASK_SHFT                                         0x0

/* PCIE20_MHI_ERDB_INT_MASK_A7_n */
#define PCIE20_MHI_ERDB_INT_MASK_A7_n(n)                                                ( 0xC8 + 0x4 * (n) )
#define PCIE20_MHI_ERDB_INT_MASK_A7_n_MASK_MASK                                         0xFFFFFFFF
#define PCIE20_MHI_ERDB_INT_MASK_A7_n_MASK_SHFT                                         0x0

/* PCIE20_MHIREGLEN */
#define PCIE20_MHIREGLEN                                                                ( 0x100 )
#define PCIE20_MHIREGLEN_MHIREGLEN_MASK                                                 0xFFFFFFFF
#define PCIE20_MHIREGLEN_MHIREGLEN_SHFT                                                 0x0

/* PCIE20_MHIVER */
#define PCIE20_MHIVER                                                                   ( 0x108 )
#define PCIE20_MHIVER_MHIVER_MASK                                                       0xFFFFFFFF
#define PCIE20_MHIVER_MHIVER_SHFT                                                       0x0

/* PCIE20_MHICFG */
#define PCIE20_MHICFG                                                                   ( 0x110 )
#define PCIE20_MHICFG_NER_MASK                                                          0xFF0000
#define PCIE20_MHICFG_NER_SHFT                                                          0x10
#define PCIE20_MHICFG_NCH_MASK                                                          0xFF
#define PCIE20_MHICFG_NCH_SHFT                                                          0x0

/* PCIE20_CHDBOFF */
#define PCIE20_CHDBOFF                                                                  ( 0x118 )
#define PCIE20_CHDBOFF_CHDBOFF_MASK                                                     0xFFFFFFFF
#define PCIE20_CHDBOFF_CHDBOFF_SHFT                                                     0x0

/* PCIE20_ERDBOFF */
#define PCIE20_ERDBOFF                                                                  ( 0x120 )
#define PCIE20_ERDBOFF_ERDBOFF_MASK                                                     0xFFFFFFFF
#define PCIE20_ERDBOFF_ERDBOFF_SHFT                                                     0x0

/* PCIE20_BHIOFF */
#define PCIE20_BHIOFF                                                                   ( 0x128 )
#define PCIE20_BHIOFF_BHIOFF_MASK                                                       0xFFFFFFFF
#define PCIE20_BHIOFF_BHIOFF_SHFT                                                       0x0

/* PCIE20_DEBUGOFF */
#define PCIE20_DEBUGOFF                                                                 ( 0x130 )
#define PCIE20_DEBUGOFF_DEBUGOFF_MASK                                                   0xFFFFFFFF
#define PCIE20_DEBUGOFF_DEBUGOFF_SHFT                                                   0x0

/* PCIE20_MHICTRL */
#define PCIE20_MHICTRL                                                                  ( 0x138 )
#define PCIE20_MHICTRL_MHISTATE_MASK                                                    0xFF00
#define PCIE20_MHICTRL_MHISTATE_SHFT                                                    0x8
#define PCIE20_MHICTRL_RESET_MASK                                                       0x2
#define PCIE20_MHICTRL_RESET_SHFT                                                       0x1

/* PCIE20_MHISTATUS */
#define PCIE20_MHISTATUS                                                                ( 0x148 )
#define PCIE20_MHISTATUS_MHISTATE_MASK                                                  0xFF00
#define PCIE20_MHISTATUS_MHISTATE_SHFT                                                  0x8
#define PCIE20_MHISTATUS_SYSERR_MASK                                                    0x4
#define PCIE20_MHISTATUS_SYSERR_SHFT                                                    0x2
#define PCIE20_MHISTATUS_READY_MASK                                                     0x1
#define PCIE20_MHISTATUS_READY_SHFT                                                     0x0

/* PCIE20_MHIERROR */
#define PCIE20_MHIERROR                                                                 ( 0x150 )
#define PCIE20_MHIERROR_MHIERROR_MASK                                                   0xFFFFFFFF
#define PCIE20_MHIERROR_MHIERROR_SHFT                                                   0x0

/* PCIE20_CCABAP_LOWER */
#define PCIE20_CCABAP_LOWER                                                             ( 0x158 )
#define PCIE20_CCABAP_LOWER_CCABAP_LOWER_MASK                                           0xFFFFFFFF
#define PCIE20_CCABAP_LOWER_CCABAP_LOWER_SHFT                                           0x0

/* PCIE20_CCABAP_UPPER */
#define PCIE20_CCABAP_UPPER                                                             ( 0x15C )
#define PCIE20_CCABAP_UPPER_CCABAP_UPPER_MASK                                           0xFFFFFFFF
#define PCIE20_CCABAP_UPPER_CCABAP_UPPER_SHFT                                           0x0

/* PCIE20_ECABAP_LOWER */
#define PCIE20_ECABAP_LOWER                                                             ( 0x160 )
#define PCIE20_ECABAP_LOWER_ECABAP_LOWER_MASK                                           0xFFFFFFFF
#define PCIE20_ECABAP_LOWER_ECABAP_LOWER_SHFT                                           0x0

/* PCIE20_ECABAP_UPPER */
#define PCIE20_ECABAP_UPPER                                                             ( 0x164 )
#define PCIE20_ECABAP_UPPER_ECABAP_UPPER_MASK                                           0xFFFFFFFF
#define PCIE20_ECABAP_UPPER_ECABAP_UPPER_SHFT                                           0x0

/* PCIE20_CRCBAP_LOWER */
#define PCIE20_CRCBAP_LOWER                                                             ( 0x168 )
#define PCIE20_CRCBAP_LOWER_CRCBAP_LOWER_MASK                                           0xFFFFFFFF
#define PCIE20_CRCBAP_LOWER_CRCBAP_LOWER_SHFT                                           0x0

/* PCIE20_CRCBAP_UPPER */
#define PCIE20_CRCBAP_UPPER                                                             ( 0x16C )
#define PCIE20_CRCBAP_UPPER_CRCBAP_UPPER_MASK                                           0xFFFFFFFF
#define PCIE20_CRCBAP_UPPER_CRCBAP_UPPER_SHFT                                           0x0

/* PCIE20_CRDB_LOWER */
#define PCIE20_CRDB_LOWER                                                               ( 0x170 )
#define PCIE20_CRDB_LOWER_CRDB_LOWER_MASK                                               0xFFFFFFFF
#define PCIE20_CRDB_LOWER_CRDB_LOWER_SHFT                                               0x0

/* PCIE20_CRDB_UPPER */
#define PCIE20_CRDB_UPPER                                                               ( 0x174 )
#define PCIE20_CRDB_UPPER_CRDB_UPPER_MASK                                               0xFFFFFFFF
#define PCIE20_CRDB_UPPER_CRDB_UPPER_SHFT                                               0x0

/* PCIE20_MHICTRLBASE_LOWER */
#define PCIE20_MHICTRLBASE_LOWER                                                        ( 0x180 )
#define PCIE20_MHICTRLBASE_LOWER_MHICTRLBASE_LOWER_MASK                                 0xFFFFFFFF
#define PCIE20_MHICTRLBASE_LOWER_MHICTRLBASE_LOWER_SHFT                                 0x0

/* PCIE20_MHICTRLBASE_UPPER */
#define PCIE20_MHICTRLBASE_UPPER                                                        ( 0x184 )
#define PCIE20_MHICTRLBASE_UPPER_MHICTRLBASE_UPPER_MASK                                 0xFFFFFFFF
#define PCIE20_MHICTRLBASE_UPPER_MHICTRLBASE_UPPER_SHFT                                 0x0

/* PCIE20_MHICTRLLIMIT_LOWER */
#define PCIE20_MHICTRLLIMIT_LOWER                                                       ( 0x188 )
#define PCIE20_MHICTRLLIMIT_LOWER_MHICTRLLIMIT_LOWER_MASK                               0xFFFFFFFF
#define PCIE20_MHICTRLLIMIT_LOWER_MHICTRLLIMIT_LOWER_SHFT                               0x0

/* PCIE20_MHICTRLLIMIT_UPPER */
#define PCIE20_MHICTRLLIMIT_UPPER                                                       ( 0x18C )
#define PCIE20_MHICTRLLIMIT_UPPER_MHICTRLLIMIT_UPPER_MASK                               0xFFFFFFFF
#define PCIE20_MHICTRLLIMIT_UPPER_MHICTRLLIMIT_UPPER_SHFT                               0x0

/* PCIE20_MHIDATABASE_LOWER */
#define PCIE20_MHIDATABASE_LOWER                                                        ( 0x198 )
#define PCIE20_MHIDATABASE_LOWER_MHIDATABASE_LOWER_MASK                                 0xFFFFFFFF
#define PCIE20_MHIDATABASE_LOWER_MHIDATABASE_LOWER_SHFT                                 0x0

/* PCIE20_MHIDATABASE_UPPER */
#define PCIE20_MHIDATABASE_UPPER                                                        ( 0x19C )
#define PCIE20_MHIDATABASE_UPPER_MHIDATABASE_UPPER_MASK                                 0xFFFFFFFF
#define PCIE20_MHIDATABASE_UPPER_MHIDATABASE_UPPER_SHFT                                 0x0

/* PCIE20_MHIDATALIMIT_LOWER */
#define PCIE20_MHIDATALIMIT_LOWER                                                       ( 0x1A0 )
#define PCIE20_MHIDATALIMIT_LOWER_MHIDATALIMIT_LOWER_MASK                               0xFFFFFFFF
#define PCIE20_MHIDATALIMIT_LOWER_MHIDATALIMIT_LOWER_SHFT                               0x0

/* PCIE20_MHIDATALIMIT_UPPER */
#define PCIE20_MHIDATALIMIT_UPPER                                                       ( 0x1A4 )
#define PCIE20_MHIDATALIMIT_UPPER_MHIDATALIMIT_UPPER_MASK                               0xFFFFFFFF
#define PCIE20_MHIDATALIMIT_UPPER_MHIDATALIMIT_UPPER_SHFT                               0x0

/* PCIE20_BHI_VERSION_LOWER */
#define PCIE20_BHI_VERSION_LOWER                                                        ( 0x200 )
#define PCIE20_BHI_VERSION_LOWER_BHI_VERSION_LOWER_MASK                                 0xFFFFFFFF
#define PCIE20_BHI_VERSION_LOWER_BHI_VERSION_LOWER_SHFT                                 0x0

/* PCIE20_BHI_VERSION_UPPER */
#define PCIE20_BHI_VERSION_UPPER                                                        ( 0x204 )
#define PCIE20_BHI_VERSION_UPPER_BHI_VERSION_UPPER_MASK                                 0xFFFFFFFF
#define PCIE20_BHI_VERSION_UPPER_BHI_VERSION_UPPER_SHFT                                 0x0

/* PCIE20_BHI_IMGADDR_LOWER */
#define PCIE20_BHI_IMGADDR_LOWER                                                        ( 0x208 )
#define PCIE20_BHI_IMGADDR_LOWER_BHI_IMGADDR_LOWER_MASK                                 0xFFFFFFFF
#define PCIE20_BHI_IMGADDR_LOWER_BHI_IMGADDR_LOWER_SHFT                                 0x0

/* PCIE20_BHI_IMGADDR_UPPER */
#define PCIE20_BHI_IMGADDR_UPPER                                                        ( 0x20C )
#define PCIE20_BHI_IMGADDR_UPPER_BHI_IMGADDR_UPPER_MASK                                 0xFFFFFFFF
#define PCIE20_BHI_IMGADDR_UPPER_BHI_IMGADDR_UPPER_SHFT                                 0x0

/* PCIE20_BHI_IMGSIZE */
#define PCIE20_BHI_IMGSIZE                                                              ( 0x210 )
#define PCIE20_BHI_IMGSIZE_BHI_IMGSIZE_MASK                                             0xFFFFFFFF
#define PCIE20_BHI_IMGSIZE_BHI_IMGSIZE_SHFT                                             0x0

/* PCIE20_BHI_IMGTXDB */
#define PCIE20_BHI_IMGTXDB                                                              ( 0x218 )
#define PCIE20_BHI_IMGTXDB_BHI_IMGTXDB_MASK                                             0xFFFFFFFF
#define PCIE20_BHI_IMGTXDB_BHI_IMGTXDB_SHFT                                             0x0

/* PCIE20_BHI_INTVEC */
#define PCIE20_BHI_INTVEC                                                               ( 0x220 )
#define PCIE20_BHI_INTVEC_BHI_INTVEC_MASK                                               0xFFFFFFFF
#define PCIE20_BHI_INTVEC_BHI_INTVEC_SHFT                                               0x0

/* PCIE20_BHI_EXECENV */
#define PCIE20_BHI_EXECENV                                                              ( 0x228 )
#define PCIE20_BHI_EXECENV_BHI_EXECENV_MASK                                             0xFFFFFFFF
#define PCIE20_BHI_EXECENV_BHI_EXECENV_SHFT                                             0x0

/* PCIE20_BHI_STATUS */
#define PCIE20_BHI_STATUS                                                               ( 0x22C )
#define PCIE20_BHI_STATUS_BHI_STATUS_MASK                                               0xFFFFFFFF
#define PCIE20_BHI_STATUS_BHI_STATUS_SHFT                                               0x0

/* PCIE20_BHI_ERRCODE */
#define PCIE20_BHI_ERRCODE                                                              ( 0x230 )
#define PCIE20_BHI_ERRCODE_BHI_ERRCODE_MASK                                             0xFFFFFFFF
#define PCIE20_BHI_ERRCODE_BHI_ERRCODE_SHFT                                             0x0

/* PCIE20_BHI_ERRDBG1 */
#define PCIE20_BHI_ERRDBG1                                                              ( 0x234 )
#define PCIE20_BHI_ERRDBG1_BHI_ERRDBG_MASK                                              0xFFFFFFFF
#define PCIE20_BHI_ERRDBG1_BHI_ERRDBG_SHFT                                              0x0

/* PCIE20_BHI_ERRDBG2 */
#define PCIE20_BHI_ERRDBG2                                                              ( 0x238 )
#define PCIE20_BHI_ERRDBG2_BHI_ERRDBG_MASK                                              0xFFFFFFFF
#define PCIE20_BHI_ERRDBG2_BHI_ERRDBG_SHFT                                              0x0

/* PCIE20_BHI_ERRDBG3 */
#define PCIE20_BHI_ERRDBG3                                                              ( 0x23C )
#define PCIE20_BHI_ERRDBG3_BHI_ERRDBG_MASK                                              0xFFFFFFFF
#define PCIE20_BHI_ERRDBG3_BHI_ERRDBG_SHFT                                              0x0

/* PCIE20_BHI_SERIALNUM */
#define PCIE20_BHI_SERIALNUM                                                            ( 0x240 )
#define PCIE20_BHI_SERIALNUM_BHI_SERIALNUM_MASK                                         0xFFFFFFFF
#define PCIE20_BHI_SERIALNUM_BHI_SERIALNUM_SHFT                                         0x0

/* PCIE20_BHI_SBLANTIROLLVER */
#define PCIE20_BHI_SBLANTIROLLVER                                                       ( 0x244 )
#define PCIE20_BHI_SBLANTIROLLVER_BHI_SBLANTIROLLVER_MASK                               0xFFFFFFFF
#define PCIE20_BHI_SBLANTIROLLVER_BHI_SBLANTIROLLVER_SHFT                               0x0

/* PCIE20_BHI_NUMSEG */
#define PCIE20_BHI_NUMSEG                                                               ( 0x248 )
#define PCIE20_BHI_NUMSEG_BHI_NUMSEG_MASK                                               0xFFFFFFFF
#define PCIE20_BHI_NUMSEG_BHI_NUMSEG_SHFT                                               0x0

/* PCIE20_BHI_MSMHWID_n */
#define PCIE20_BHI_MSMHWID_n(n)                                                         ( 0x24C + 0x4 * (n) )
#define PCIE20_BHI_MSMHWID_n_BHI_MSMHWID_MASK                                           0xFFFFFFFF
#define PCIE20_BHI_MSMHWID_n_BHI_MSMHWID_SHFT                                           0x0

/* PCIE20_BHI_OEMPKHASH_n */
#define PCIE20_BHI_OEMPKHASH_n(n)                                                       ( 0x264 + 0x4 * (n) )
#define PCIE20_BHI_OEMPKHASH_n_BHI_OEMPKHASH_MASK                                       0xFFFFFFFF
#define PCIE20_BHI_OEMPKHASH_n_BHI_OEMPKHASH_SHFT                                       0x0

/* PCIE20_CHDB_LOWER_n */
#define PCIE20_CHDB_LOWER_n(n)                                                          ( 0x400 + 0x8 * (n) )
#define PCIE20_CHDB_LOWER_n_CHDB_LOWER_MASK                                             0xFFFFFFFF
#define PCIE20_CHDB_LOWER_n_CHDB_LOWER_SHFT                                             0x0

/* PCIE20_CHDB_UPPER_n */
#define PCIE20_CHDB_UPPER_n(n)                                                          ( 0x404 + 0x8 * (n) )
#define PCIE20_CHDB_UPPER_n_CHDB_UPPER_MASK                                             0xFFFFFFFF
#define PCIE20_CHDB_UPPER_n_CHDB_UPPER_SHFT                                             0x0

/* PCIE20_ERDB_LOWER_n */
#define PCIE20_ERDB_LOWER_n(n)                                                          ( 0x800 + 0x8 * (n) )
#define PCIE20_ERDB_LOWER_n_ERDB_LOWER_MASK                                             0xFFFFFFFF
#define PCIE20_ERDB_LOWER_n_ERDB_LOWER_SHFT                                             0x0

/* PCIE20_ERDB_UPPER_n */
#define PCIE20_ERDB_UPPER_n(n)                                                          ( 0x804 + 0x8 * (n) )
#define PCIE20_ERDB_UPPER_n_ERDB_UPPER_MASK                                             0xFFFFFFFF
#define PCIE20_ERDB_UPPER_n_ERDB_UPPER_SHFT                                             0x0

/* PCIE20_PARF_DEBUG_CNT_INT_SLV_XPU2_MSA */
#define PCIE20_PARF_DEBUG_CNT_INT_SLV_XPU2_MSA                                          ( 0xC00 )
#define PCIE20_PARF_DEBUG_CNT_INT_SLV_XPU2_MSA_CNT_MASK                                 0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_INT_SLV_XPU2_MSA_CNT_SHFT                                 0x0

/* PCIE20_PARF_DEBUG_CNT_PM_LINKST_IN_L2 */
#define PCIE20_PARF_DEBUG_CNT_PM_LINKST_IN_L2                                           ( 0xC04 )
#define PCIE20_PARF_DEBUG_CNT_PM_LINKST_IN_L2_CNT_MASK                                  0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_PM_LINKST_IN_L2_CNT_SHFT                                  0x0

/* PCIE20_PARF_DEBUG_CNT_PM_LINKST_IN_L1SUB */
#define PCIE20_PARF_DEBUG_CNT_PM_LINKST_IN_L1SUB                                        ( 0xC08 )
#define PCIE20_PARF_DEBUG_CNT_PM_LINKST_IN_L1SUB_CNT_MASK                               0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_PM_LINKST_IN_L1SUB_CNT_SHFT                               0x0

/* PCIE20_PARF_DEBUG_CNT_PM_LINKST_IN_L1 */
#define PCIE20_PARF_DEBUG_CNT_PM_LINKST_IN_L1                                           ( 0xC0C )
#define PCIE20_PARF_DEBUG_CNT_PM_LINKST_IN_L1_CNT_MASK                                  0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_PM_LINKST_IN_L1_CNT_SHFT                                  0x0

/* PCIE20_PARF_DEBUG_CNT_PM_LINKST_IN_L0S */
#define PCIE20_PARF_DEBUG_CNT_PM_LINKST_IN_L0S                                          ( 0xC10 )
#define PCIE20_PARF_DEBUG_CNT_PM_LINKST_IN_L0S_CNT_MASK                                 0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_PM_LINKST_IN_L0S_CNT_SHFT                                 0x0

/* PCIE20_PARF_DEBUG_CNT_AUX_CLK_APP_REQ_EXIT_L1 */
#define PCIE20_PARF_DEBUG_CNT_AUX_CLK_APP_REQ_EXIT_L1                                   ( 0xC14 )
#define PCIE20_PARF_DEBUG_CNT_AUX_CLK_APP_REQ_EXIT_L1_CNT_MASK                          0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_AUX_CLK_APP_REQ_EXIT_L1_CNT_SHFT                          0x0

/* PCIE20_PARF_DEBUG_CNT_AUX_CLK_APP_READY_ENTER_L23 */
#define PCIE20_PARF_DEBUG_CNT_AUX_CLK_APP_READY_ENTER_L23                               ( 0xC18 )
#define PCIE20_PARF_DEBUG_CNT_AUX_CLK_APP_READY_ENTER_L23_CNT_MASK                      0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_AUX_CLK_APP_READY_ENTER_L23_CNT_SHFT                      0x0

/* PCIE20_PARF_DEBUG_CNT_AUX_CLK_APP_READY_ENTER_L1 */
#define PCIE20_PARF_DEBUG_CNT_AUX_CLK_APP_READY_ENTER_L1                                ( 0xC1C )
#define PCIE20_PARF_DEBUG_CNT_AUX_CLK_APP_READY_ENTER_L1_CNT_MASK                       0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_AUX_CLK_APP_READY_ENTER_L1_CNT_SHFT                       0x0

/* PCIE20_PARF_DEBUG_CNT_INT_BRIDGE_FLUSH */
#define PCIE20_PARF_DEBUG_CNT_INT_BRIDGE_FLUSH                                          ( 0xC20 )
#define PCIE20_PARF_DEBUG_CNT_INT_BRIDGE_FLUSH_CNT_MASK                                 0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_INT_BRIDGE_FLUSH_CNT_SHFT                                 0x0

/* PCIE20_PARF_DEBUG_CNT_ECRC_ERR_ASSERTED */
#define PCIE20_PARF_DEBUG_CNT_ECRC_ERR_ASSERTED                                         ( 0xC24 )
#define PCIE20_PARF_DEBUG_CNT_ECRC_ERR_ASSERTED_CNT_MASK                                0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_ECRC_ERR_ASSERTED_CNT_SHFT                                0x0

/* PCIE20_PARF_DEBUG_CNT_LCRC_ERR_ASSERTED */
#define PCIE20_PARF_DEBUG_CNT_LCRC_ERR_ASSERTED                                         ( 0xC28 )
#define PCIE20_PARF_DEBUG_CNT_LCRC_ERR_ASSERTED_CNT_MASK                                0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_LCRC_ERR_ASSERTED_CNT_SHFT                                0x0

/* PCIE20_PARF_DEBUG_CNT_RMLH_RDLH_PKTERR_1 */
#define PCIE20_PARF_DEBUG_CNT_RMLH_RDLH_PKTERR_1                                        ( 0xC2C )
#define PCIE20_PARF_DEBUG_CNT_RMLH_RDLH_PKTERR_1_CNT_MASK                               0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_RMLH_RDLH_PKTERR_1_CNT_SHFT                               0x0

/* PCIE20_PARF_DEBUG_CNT_RMLH_RDLH_PKTERR_0 */
#define PCIE20_PARF_DEBUG_CNT_RMLH_RDLH_PKTERR_0                                        ( 0xC30 )
#define PCIE20_PARF_DEBUG_CNT_RMLH_RDLH_PKTERR_0_CNT_MASK                               0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_RMLH_RDLH_PKTERR_0_CNT_SHFT                               0x0

/* PCIE20_PARF_DEBUG_CNT_RMLH_DESKEW_ALIGN_ERR */
#define PCIE20_PARF_DEBUG_CNT_RMLH_DESKEW_ALIGN_ERR                                     ( 0xC34 )
#define PCIE20_PARF_DEBUG_CNT_RMLH_DESKEW_ALIGN_ERR_CNT_MASK                            0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_RMLH_DESKEW_ALIGN_ERR_CNT_SHFT                            0x0

/* PCIE20_PARF_DEBUG_CNT_RMLK_TS_RCV_ERR */
#define PCIE20_PARF_DEBUG_CNT_RMLK_TS_RCV_ERR                                           ( 0xC38 )
#define PCIE20_PARF_DEBUG_CNT_RMLK_TS_RCV_ERR_CNT_MASK                                  0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_RMLK_TS_RCV_ERR_CNT_SHFT                                  0x0

/* PCIE20_PARF_DEBUG_CNT_UNEXP_CPL_ERR */
#define PCIE20_PARF_DEBUG_CNT_UNEXP_CPL_ERR                                             ( 0xC3C )
#define PCIE20_PARF_DEBUG_CNT_UNEXP_CPL_ERR_CNT_MASK                                    0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_UNEXP_CPL_ERR_CNT_SHFT                                    0x0

/* PCIE20_PARF_DEBUG_CNT_CPL_CA_ERR */
#define PCIE20_PARF_DEBUG_CNT_CPL_CA_ERR                                                ( 0xC40 )
#define PCIE20_PARF_DEBUG_CNT_CPL_CA_ERR_CNT_MASK                                       0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_CPL_CA_ERR_CNT_SHFT                                       0x0

/* PCIE20_PARF_DEBUG_CNT_CPL_UR_ERR */
#define PCIE20_PARF_DEBUG_CNT_CPL_UR_ERR                                                ( 0xC44 )
#define PCIE20_PARF_DEBUG_CNT_CPL_UR_ERR_CNT_MASK                                       0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_CPL_UR_ERR_CNT_SHFT                                       0x0

/* PCIE20_PARF_DEBUG_CNT_CPL_MLF_ERR */
#define PCIE20_PARF_DEBUG_CNT_CPL_MLF_ERR                                               ( 0xC48 )
#define PCIE20_PARF_DEBUG_CNT_CPL_MLF_ERR_CNT_MASK                                      0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_CPL_MLF_ERR_CNT_SHFT                                      0x0

/* PCIE20_PARF_DEBUG_CNT_FORM_FILT_ECRC_ERR */
#define PCIE20_PARF_DEBUG_CNT_FORM_FILT_ECRC_ERR                                        ( 0xC4C )
#define PCIE20_PARF_DEBUG_CNT_FORM_FILT_ECRC_ERR_CNT_MASK                               0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_FORM_FILT_ECRC_ERR_CNT_SHFT                               0x0

/* PCIE20_PARF_DEBUG_CNT_FORM_FILT_MALFORM_TLP_ERR */
#define PCIE20_PARF_DEBUG_CNT_FORM_FILT_MALFORM_TLP_ERR                                 ( 0xC50 )
#define PCIE20_PARF_DEBUG_CNT_FORM_FILT_MALFORM_TLP_ERR_CNT_MASK                        0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_FORM_FILT_MALFORM_TLP_ERR_CNT_SHFT                        0x0

/* PCIE20_PARF_DEBUG_CNT_FORM_FILT_DLLP_ERR */
#define PCIE20_PARF_DEBUG_CNT_FORM_FILT_DLLP_ERR                                        ( 0xC54 )
#define PCIE20_PARF_DEBUG_CNT_FORM_FILT_DLLP_ERR_CNT_MASK                               0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_FORM_FILT_DLLP_ERR_CNT_SHFT                               0x0

/* PCIE20_PARF_DEBUG_CNT_XDLH_REPLAY_TIMEOUT_ERR */
#define PCIE20_PARF_DEBUG_CNT_XDLH_REPLAY_TIMEOUT_ERR                                   ( 0xC58 )
#define PCIE20_PARF_DEBUG_CNT_XDLH_REPLAY_TIMEOUT_ERR_CNT_MASK                          0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_XDLH_REPLAY_TIMEOUT_ERR_CNT_SHFT                          0x0

/* PCIE20_PARF_DEBUG_CNT_XDLH_REPLAY_NUM_RLOVER_ERR */
#define PCIE20_PARF_DEBUG_CNT_XDLH_REPLAY_NUM_RLOVER_ERR                                ( 0xC5C )
#define PCIE20_PARF_DEBUG_CNT_XDLH_REPLAY_NUM_RLOVER_ERR_CNT_MASK                       0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_XDLH_REPLAY_NUM_RLOVER_ERR_CNT_SHFT                       0x0

/* PCIE20_PARF_DEBUG_CNT_RDLH_BAD_DLLP_ERR */
#define PCIE20_PARF_DEBUG_CNT_RDLH_BAD_DLLP_ERR                                         ( 0xC60 )
#define PCIE20_PARF_DEBUG_CNT_RDLH_BAD_DLLP_ERR_CNT_MASK                                0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_RDLH_BAD_DLLP_ERR_CNT_SHFT                                0x0

/* PCIE20_PARF_DEBUG_CNT_RDLH_BAD_TLP_ERR */
#define PCIE20_PARF_DEBUG_CNT_RDLH_BAD_TLP_ERR                                          ( 0xC64 )
#define PCIE20_PARF_DEBUG_CNT_RDLH_BAD_TLP_ERR_CNT_MASK                                 0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_RDLH_BAD_TLP_ERR_CNT_SHFT                                 0x0

/* PCIE20_PARF_DEBUG_CNT_RDLH_PROT_ERR */
#define PCIE20_PARF_DEBUG_CNT_RDLH_PROT_ERR                                             ( 0xC68 )
#define PCIE20_PARF_DEBUG_CNT_RDLH_PROT_ERR_CNT_MASK                                    0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_RDLH_PROT_ERR_CNT_SHFT                                    0x0

/* PCIE20_PARF_DEBUG_CNT_RMLH_RCVD_ERR */
#define PCIE20_PARF_DEBUG_CNT_RMLH_RCVD_ERR                                             ( 0xC6C )
#define PCIE20_PARF_DEBUG_CNT_RMLH_RCVD_ERR_CNT_MASK                                    0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_RMLH_RCVD_ERR_CNT_SHFT                                    0x0

/* PCIE20_PARF_DEBUG_CNT_RADM_CPL_TIMEOUT */
#define PCIE20_PARF_DEBUG_CNT_RADM_CPL_TIMEOUT                                          ( 0xC70 )
#define PCIE20_PARF_DEBUG_CNT_RADM_CPL_TIMEOUT_CNT_MASK                                 0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_RADM_CPL_TIMEOUT_CNT_SHFT                                 0x0

/* PCIE20_PARF_DEBUG_CNT_RADM_QOVERFLOW */
#define PCIE20_PARF_DEBUG_CNT_RADM_QOVERFLOW                                            ( 0xC74 )
#define PCIE20_PARF_DEBUG_CNT_RADM_QOVERFLOW_CNT_MASK                                   0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_RADM_QOVERFLOW_CNT_SHFT                                   0x0

/* PCIE20_PARF_DEBUG_CNT_RADM_UNEXP_CPL_ERR */
#define PCIE20_PARF_DEBUG_CNT_RADM_UNEXP_CPL_ERR                                        ( 0xC78 )
#define PCIE20_PARF_DEBUG_CNT_RADM_UNEXP_CPL_ERR_CNT_MASK                               0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_RADM_UNEXP_CPL_ERR_CNT_SHFT                               0x0

/* PCIE20_PARF_DEBUG_CNT_RADM_ECRC_ERR */
#define PCIE20_PARF_DEBUG_CNT_RADM_ECRC_ERR                                             ( 0xC7C )
#define PCIE20_PARF_DEBUG_CNT_RADM_ECRC_ERR_CNT_MASK                                    0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_RADM_ECRC_ERR_CNT_SHFT                                    0x0

/* PCIE20_PARF_DEBUG_CNT_RADM_MLF_TLP_ERR */
#define PCIE20_PARF_DEBUG_CNT_RADM_MLF_TLP_ERR                                          ( 0xC80 )
#define PCIE20_PARF_DEBUG_CNT_RADM_MLF_TLP_ERR_CNT_MASK                                 0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_RADM_MLF_TLP_ERR_CNT_SHFT                                 0x0

/* PCIE20_PARF_DEBUG_CNT_AUX_CLK_IN_L1SUB_L1 */
#define PCIE20_PARF_DEBUG_CNT_AUX_CLK_IN_L1SUB_L1                                       ( 0xC84 )
#define PCIE20_PARF_DEBUG_CNT_AUX_CLK_IN_L1SUB_L1_CNT_MASK                              0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_AUX_CLK_IN_L1SUB_L1_CNT_SHFT                              0x0

/* PCIE20_PARF_DEBUG_CNT_AUX_CLK_IN_L1SUB_L2 */
#define PCIE20_PARF_DEBUG_CNT_AUX_CLK_IN_L1SUB_L2                                       ( 0xC88 )
#define PCIE20_PARF_DEBUG_CNT_AUX_CLK_IN_L1SUB_L2_CNT_MASK                              0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_AUX_CLK_IN_L1SUB_L2_CNT_SHFT                              0x0

/* PCIE20_PARF_DEBUG_CNT_AUX_CLK_D_STATE_CHANGE */
#define PCIE20_PARF_DEBUG_CNT_AUX_CLK_D_STATE_CHANGE                                    ( 0xC8C )
#define PCIE20_PARF_DEBUG_CNT_AUX_CLK_D_STATE_CHANGE_CNT_MASK                           0xFFFFFFFF
#define PCIE20_PARF_DEBUG_CNT_AUX_CLK_D_STATE_CHANGE_CNT_SHFT                           0x0


/*============================================================================
     PCIE_DBI
============================================================================*/
     
#define PCIE_DBI_BASE_PHYS                                                                                            ( 0x7FFFE000 )
#define PCIE_DBI_SIZE                                                                                                 ( 0xF1D )

/* TYPE1_DEV_ID_VEND_ID_REG */
#define TYPE1_DEV_ID_VEND_ID_REG                                                                                      ( 0x0 )
#define TYPE1_DEV_ID_VEND_ID_REG_DEVICE_ID_MASK                                                                       0xFFFF0000
#define TYPE1_DEV_ID_VEND_ID_REG_DEVICE_ID_SHFT                                                                       0x10
#define TYPE1_DEV_ID_VEND_ID_REG_VENDOR_ID_MASK                                                                       0xFFFF
#define TYPE1_DEV_ID_VEND_ID_REG_VENDOR_ID_SHFT                                                                       0x0

/* TYPE0_DEVICE_ID_VENDOR_ID_REG_1 */
#define TYPE0_DEVICE_ID_VENDOR_ID_REG_1                                                                               ( 0x0 )
#define TYPE0_DEVICE_ID_VENDOR_ID_REG_1_PCI_TYPE0_DEVICE_ID_MASK                                                      0xFFFF0000
#define TYPE0_DEVICE_ID_VENDOR_ID_REG_1_PCI_TYPE0_DEVICE_ID_SHFT                                                      0x10
#define TYPE0_DEVICE_ID_VENDOR_ID_REG_1_PCI_TYPE0_VENDOR_ID_MASK                                                      0xFFFF
#define TYPE0_DEVICE_ID_VENDOR_ID_REG_1_PCI_TYPE0_VENDOR_ID_SHFT                                                      0x0

/* TYPE1_STATUS_COMMAND_REG */
#define TYPE1_STATUS_COMMAND_REG                                                                                      ( 0x4 )
#define TYPE1_STATUS_COMMAND_REG_DETECTED_PARITY_ERROR_MASK                                                           0x80000000
#define TYPE1_STATUS_COMMAND_REG_DETECTED_PARITY_ERROR_SHFT                                                           0x1F
#define TYPE1_STATUS_COMMAND_REG_SIGNALED_SYS_ERROR_MASK                                                              0x40000000
#define TYPE1_STATUS_COMMAND_REG_SIGNALED_SYS_ERROR_SHFT                                                              0x1E
#define TYPE1_STATUS_COMMAND_REG_RCVD_MASTER_ABORT_MASK                                                               0x20000000
#define TYPE1_STATUS_COMMAND_REG_RCVD_MASTER_ABORT_SHFT                                                               0x1D
#define TYPE1_STATUS_COMMAND_REG_RCVD_TARGET_ABORT_MASK                                                               0x10000000
#define TYPE1_STATUS_COMMAND_REG_RCVD_TARGET_ABORT_SHFT                                                               0x1C
#define TYPE1_STATUS_COMMAND_REG_SIGNALED_TARGET_ABORT_MASK                                                           0x8000000
#define TYPE1_STATUS_COMMAND_REG_SIGNALED_TARGET_ABORT_SHFT                                                           0x1B
#define TYPE1_STATUS_COMMAND_REG_DEV_SEL_TIMING_MASK                                                                  0x6000000
#define TYPE1_STATUS_COMMAND_REG_DEV_SEL_TIMING_SHFT                                                                  0x19
#define TYPE1_STATUS_COMMAND_REG_MASTER_DPE_MASK                                                                      0x1000000
#define TYPE1_STATUS_COMMAND_REG_MASTER_DPE_SHFT                                                                      0x18
#define TYPE1_STATUS_COMMAND_REG_FAST_B2B_CAP_MASK                                                                    0x800000
#define TYPE1_STATUS_COMMAND_REG_FAST_B2B_CAP_SHFT                                                                    0x17
#define TYPE1_STATUS_COMMAND_REG_FAST_66MHZ_CAP_MASK                                                                  0x200000
#define TYPE1_STATUS_COMMAND_REG_FAST_66MHZ_CAP_SHFT                                                                  0x15
#define TYPE1_STATUS_COMMAND_REG_CAP_LIST_MASK                                                                        0x100000
#define TYPE1_STATUS_COMMAND_REG_CAP_LIST_SHFT                                                                        0x14
#define TYPE1_STATUS_COMMAND_REG_INT_STATUS_MASK                                                                      0x80000
#define TYPE1_STATUS_COMMAND_REG_INT_STATUS_SHFT                                                                      0x13
#define TYPE1_STATUS_COMMAND_REG_RESERV_MASK                                                                          0x1F800
#define TYPE1_STATUS_COMMAND_REG_RESERV_SHFT                                                                          0xB
#define TYPE1_STATUS_COMMAND_REG_INT_EN_MASK                                                                          0x400
#define TYPE1_STATUS_COMMAND_REG_INT_EN_SHFT                                                                          0xA
#define TYPE1_STATUS_COMMAND_REG_SERREN_MASK                                                                          0x100
#define TYPE1_STATUS_COMMAND_REG_SERREN_SHFT                                                                          0x8
#define TYPE1_STATUS_COMMAND_REG_IDSEL_MASK                                                                           0x80
#define TYPE1_STATUS_COMMAND_REG_IDSEL_SHFT                                                                           0x7
#define TYPE1_STATUS_COMMAND_REG_PERREN_MASK                                                                          0x40
#define TYPE1_STATUS_COMMAND_REG_PERREN_SHFT                                                                          0x6
#define TYPE1_STATUS_COMMAND_REG_VGAPS_MASK                                                                           0x20
#define TYPE1_STATUS_COMMAND_REG_VGAPS_SHFT                                                                           0x5
#define TYPE1_STATUS_COMMAND_REG_MWI_EN_MASK                                                                          0x10
#define TYPE1_STATUS_COMMAND_REG_MWI_EN_SHFT                                                                          0x4
#define TYPE1_STATUS_COMMAND_REG_SCO_MASK                                                                             0x8
#define TYPE1_STATUS_COMMAND_REG_SCO_SHFT                                                                             0x3
#define TYPE1_STATUS_COMMAND_REG_BME_MASK                                                                             0x4
#define TYPE1_STATUS_COMMAND_REG_BME_SHFT                                                                             0x2
#define TYPE1_STATUS_COMMAND_REG_MSE_MASK                                                                             0x2
#define TYPE1_STATUS_COMMAND_REG_MSE_SHFT                                                                             0x1
#define TYPE1_STATUS_COMMAND_REG_IO_EN_MASK                                                                           0x1
#define TYPE1_STATUS_COMMAND_REG_IO_EN_SHFT                                                                           0x0

/* TYPE0_STATUS_COMMAND_REG_1 */
#define TYPE0_STATUS_COMMAND_REG_1                                                                                    ( 0x4 )
#define TYPE0_STATUS_COMMAND_REG_1_DETECTED_PARITY_ERR_MASK                                                           0x80000000
#define TYPE0_STATUS_COMMAND_REG_1_DETECTED_PARITY_ERR_SHFT                                                           0x1F
#define TYPE0_STATUS_COMMAND_REG_1_SIGNALED_SYS_ERR_MASK                                                              0x40000000
#define TYPE0_STATUS_COMMAND_REG_1_SIGNALED_SYS_ERR_SHFT                                                              0x1E
#define TYPE0_STATUS_COMMAND_REG_1_RCVD_MASTER_ABORT_MASK                                                             0x20000000
#define TYPE0_STATUS_COMMAND_REG_1_RCVD_MASTER_ABORT_SHFT                                                             0x1D
#define TYPE0_STATUS_COMMAND_REG_1_RCVD_TARGET_ABORT_MASK                                                             0x10000000
#define TYPE0_STATUS_COMMAND_REG_1_RCVD_TARGET_ABORT_SHFT                                                             0x1C
#define TYPE0_STATUS_COMMAND_REG_1_SIGNALED_TARGET_ABORT_MASK                                                         0x8000000
#define TYPE0_STATUS_COMMAND_REG_1_SIGNALED_TARGET_ABORT_SHFT                                                         0x1B
#define TYPE0_STATUS_COMMAND_REG_1_DEV_SEL_TIMING_MASK                                                                0x6000000
#define TYPE0_STATUS_COMMAND_REG_1_DEV_SEL_TIMING_SHFT                                                                0x19
#define TYPE0_STATUS_COMMAND_REG_1_MASTER_DPE_MASK                                                                    0x1000000
#define TYPE0_STATUS_COMMAND_REG_1_MASTER_DPE_SHFT                                                                    0x18
#define TYPE0_STATUS_COMMAND_REG_1_FAST_B2B_CAP_MASK                                                                  0x800000
#define TYPE0_STATUS_COMMAND_REG_1_FAST_B2B_CAP_SHFT                                                                  0x17
#define TYPE0_STATUS_COMMAND_REG_1_FAST_66MHZ_CAP_MASK                                                                0x200000
#define TYPE0_STATUS_COMMAND_REG_1_FAST_66MHZ_CAP_SHFT                                                                0x15
#define TYPE0_STATUS_COMMAND_REG_1_CAP_LIST_MASK                                                                      0x100000
#define TYPE0_STATUS_COMMAND_REG_1_CAP_LIST_SHFT                                                                      0x14
#define TYPE0_STATUS_COMMAND_REG_1_INT_STATUS_MASK                                                                    0x80000
#define TYPE0_STATUS_COMMAND_REG_1_INT_STATUS_SHFT                                                                    0x13
#define TYPE0_STATUS_COMMAND_REG_1_PCI_TYPE_RESERV_MASK                                                               0x1F800
#define TYPE0_STATUS_COMMAND_REG_1_PCI_TYPE_RESERV_SHFT                                                               0xB
#define TYPE0_STATUS_COMMAND_REG_1_PCI_TYPE0_INT_EN_MASK                                                              0x400
#define TYPE0_STATUS_COMMAND_REG_1_PCI_TYPE0_INT_EN_SHFT                                                              0xA
#define TYPE0_STATUS_COMMAND_REG_1_PCI_TYPE0_SERREN_MASK                                                              0x100
#define TYPE0_STATUS_COMMAND_REG_1_PCI_TYPE0_SERREN_SHFT                                                              0x8
#define TYPE0_STATUS_COMMAND_REG_1_PCI_TYPE_IDSEL_STEPPING_MASK                                                       0x80
#define TYPE0_STATUS_COMMAND_REG_1_PCI_TYPE_IDSEL_STEPPING_SHFT                                                       0x7
#define TYPE0_STATUS_COMMAND_REG_1_PCI_TYPE0_PARITY_ERR_EN_MASK                                                       0x40
#define TYPE0_STATUS_COMMAND_REG_1_PCI_TYPE0_PARITY_ERR_EN_SHFT                                                       0x6
#define TYPE0_STATUS_COMMAND_REG_1_PCI_TYPE_VGA_PALETTE_SNOOP_MASK                                                    0x20
#define TYPE0_STATUS_COMMAND_REG_1_PCI_TYPE_VGA_PALETTE_SNOOP_SHFT                                                    0x5
#define TYPE0_STATUS_COMMAND_REG_1_PCI_TYPE_MWI_ENABLE_MASK                                                           0x10
#define TYPE0_STATUS_COMMAND_REG_1_PCI_TYPE_MWI_ENABLE_SHFT                                                           0x4
#define TYPE0_STATUS_COMMAND_REG_1_PCI_TYPE0_SPECIAL_CYCLE_OPERATION_MASK                                             0x8
#define TYPE0_STATUS_COMMAND_REG_1_PCI_TYPE0_SPECIAL_CYCLE_OPERATION_SHFT                                             0x3
#define TYPE0_STATUS_COMMAND_REG_1_PCI_TYPE0_BUS_MASTER_EN_MASK                                                       0x4
#define TYPE0_STATUS_COMMAND_REG_1_PCI_TYPE0_BUS_MASTER_EN_SHFT                                                       0x2
#define TYPE0_STATUS_COMMAND_REG_1_PCI_TYPE0_MEM_SPACE_EN_MASK                                                        0x2
#define TYPE0_STATUS_COMMAND_REG_1_PCI_TYPE0_MEM_SPACE_EN_SHFT                                                        0x1
#define TYPE0_STATUS_COMMAND_REG_1_PCI_TYPE0_IO_EN_MASK                                                               0x1
#define TYPE0_STATUS_COMMAND_REG_1_PCI_TYPE0_IO_EN_SHFT                                                               0x0

/* TYPE1_CLASS_CODE_REV_ID_REG */
#define TYPE1_CLASS_CODE_REV_ID_REG                                                                                   ( 0x8 )
#define TYPE1_CLASS_CODE_REV_ID_REG_BASE_CLASS_CODE_MASK                                                              0xFF000000
#define TYPE1_CLASS_CODE_REV_ID_REG_BASE_CLASS_CODE_SHFT                                                              0x18
#define TYPE1_CLASS_CODE_REV_ID_REG_SUBCLASS_CODE_MASK                                                                0xFF0000
#define TYPE1_CLASS_CODE_REV_ID_REG_SUBCLASS_CODE_SHFT                                                                0x10
#define TYPE1_CLASS_CODE_REV_ID_REG_PROGRAM_INTERFACE_MASK                                                            0xFF00
#define TYPE1_CLASS_CODE_REV_ID_REG_PROGRAM_INTERFACE_SHFT                                                            0x8
#define TYPE1_CLASS_CODE_REV_ID_REG_REVISION_ID_MASK                                                                  0xFF
#define TYPE1_CLASS_CODE_REV_ID_REG_REVISION_ID_SHFT                                                                  0x0

/* TYPE0_CLASS_CODE_REVISION_ID_1 */
#define TYPE0_CLASS_CODE_REVISION_ID_1                                                                                ( 0x8 )
#define TYPE0_CLASS_CODE_REVISION_ID_1_BASE_CLASS_CODE_MASK                                                           0xFF000000
#define TYPE0_CLASS_CODE_REVISION_ID_1_BASE_CLASS_CODE_SHFT                                                           0x18
#define TYPE0_CLASS_CODE_REVISION_ID_1_SUBCLASS_CODE_MASK                                                             0xFF0000
#define TYPE0_CLASS_CODE_REVISION_ID_1_SUBCLASS_CODE_SHFT                                                             0x10
#define TYPE0_CLASS_CODE_REVISION_ID_1_PROGRAM_INTERFACE_MASK                                                         0xFF00
#define TYPE0_CLASS_CODE_REVISION_ID_1_PROGRAM_INTERFACE_SHFT                                                         0x8
#define TYPE0_CLASS_CODE_REVISION_ID_1_REVISION_ID_MASK                                                               0xFF
#define TYPE0_CLASS_CODE_REVISION_ID_1_REVISION_ID_SHFT                                                               0x0

/* TYPE1_BIST_HDR_TYPE_LAT_CACHE_LINE_SIZE_REG */
#define TYPE1_BIST_HDR_TYPE_LAT_CACHE_LINE_SIZE_REG                                                                   ( 0xC )
#define TYPE1_BIST_HDR_TYPE_LAT_CACHE_LINE_SIZE_REG_BIST_MASK                                                         0xFF000000
#define TYPE1_BIST_HDR_TYPE_LAT_CACHE_LINE_SIZE_REG_BIST_SHFT                                                         0x18
#define TYPE1_BIST_HDR_TYPE_LAT_CACHE_LINE_SIZE_REG_MULTI_FUNC_MASK                                                   0x800000
#define TYPE1_BIST_HDR_TYPE_LAT_CACHE_LINE_SIZE_REG_MULTI_FUNC_SHFT                                                   0x17
#define TYPE1_BIST_HDR_TYPE_LAT_CACHE_LINE_SIZE_REG_HEADER_TYPE_MASK                                                  0x7F0000
#define TYPE1_BIST_HDR_TYPE_LAT_CACHE_LINE_SIZE_REG_HEADER_TYPE_SHFT                                                  0x10
#define TYPE1_BIST_HDR_TYPE_LAT_CACHE_LINE_SIZE_REG_LATENCY_MASTER_TIMER_MASK                                         0xFF00
#define TYPE1_BIST_HDR_TYPE_LAT_CACHE_LINE_SIZE_REG_LATENCY_MASTER_TIMER_SHFT                                         0x8
#define TYPE1_BIST_HDR_TYPE_LAT_CACHE_LINE_SIZE_REG_CACHE_LINE_SIZE_MASK                                              0xFF
#define TYPE1_BIST_HDR_TYPE_LAT_CACHE_LINE_SIZE_REG_CACHE_LINE_SIZE_SHFT                                              0x0

/* TYPE0_BIST_HEADER_TYPE_LATENCY_CACHE_LINE_SIZE_REG_1 */
#define TYPE0_BIST_HEADER_TYPE_LATENCY_CACHE_LINE_SIZE_REG_1                                                          ( 0xC )
#define TYPE0_BIST_HEADER_TYPE_LATENCY_CACHE_LINE_SIZE_REG_1_BIST_MASK                                                0xFF000000
#define TYPE0_BIST_HEADER_TYPE_LATENCY_CACHE_LINE_SIZE_REG_1_BIST_SHFT                                                0x18
#define TYPE0_BIST_HEADER_TYPE_LATENCY_CACHE_LINE_SIZE_REG_1_MULTI_FUNC_MASK                                          0x800000
#define TYPE0_BIST_HEADER_TYPE_LATENCY_CACHE_LINE_SIZE_REG_1_MULTI_FUNC_SHFT                                          0x17
#define TYPE0_BIST_HEADER_TYPE_LATENCY_CACHE_LINE_SIZE_REG_1_HEADER_TYPE_MASK                                         0x7F0000
#define TYPE0_BIST_HEADER_TYPE_LATENCY_CACHE_LINE_SIZE_REG_1_HEADER_TYPE_SHFT                                         0x10
#define TYPE0_BIST_HEADER_TYPE_LATENCY_CACHE_LINE_SIZE_REG_1_LATENCY_MASTER_TIMER_MASK                                0xFF00
#define TYPE0_BIST_HEADER_TYPE_LATENCY_CACHE_LINE_SIZE_REG_1_LATENCY_MASTER_TIMER_SHFT                                0x8
#define TYPE0_BIST_HEADER_TYPE_LATENCY_CACHE_LINE_SIZE_REG_1_CACHE_LINE_SIZE_MASK                                     0xFF
#define TYPE0_BIST_HEADER_TYPE_LATENCY_CACHE_LINE_SIZE_REG_1_CACHE_LINE_SIZE_SHFT                                     0x0

/* BAR0_REG */
#define BAR0_REG                                                                                                      ( 0x10 )
#define BAR0_REG_BAR0_START_MASK                                                                                      0xFFFFFFF0
#define BAR0_REG_BAR0_START_SHFT                                                                                      0x4
#define BAR0_REG_BAR0_PREFETCH_MASK                                                                                   0x8
#define BAR0_REG_BAR0_PREFETCH_SHFT                                                                                   0x3
#define BAR0_REG_BAR0_TYPE_MASK                                                                                       0x6
#define BAR0_REG_BAR0_TYPE_SHFT                                                                                       0x1
#define BAR0_REG_BAR0_MEM_IO_MASK                                                                                     0x1
#define BAR0_REG_BAR0_MEM_IO_SHFT                                                                                     0x0

/* TYPE0_BAR0_REG_1 */
#define TYPE0_BAR0_REG_1                                                                                              ( 0x10 )
#define TYPE0_BAR0_REG_1_BAR0_START_MASK                                                                              0xFFFFFFF0
#define TYPE0_BAR0_REG_1_BAR0_START_SHFT                                                                              0x4
#define TYPE0_BAR0_REG_1_BAR0_PREFETCH_MASK                                                                           0x8
#define TYPE0_BAR0_REG_1_BAR0_PREFETCH_SHFT                                                                           0x3
#define TYPE0_BAR0_REG_1_BAR0_TYPE_MASK                                                                               0x6
#define TYPE0_BAR0_REG_1_BAR0_TYPE_SHFT                                                                               0x1
#define TYPE0_BAR0_REG_1_BAR0_MEM_IO_MASK                                                                             0x1
#define TYPE0_BAR0_REG_1_BAR0_MEM_IO_SHFT                                                                             0x0

/* BAR1_REG */
#define BAR1_REG                                                                                                      ( 0x14 )
#define BAR1_REG_BAR1_START_MASK                                                                                      0xFFFFFFF0
#define BAR1_REG_BAR1_START_SHFT                                                                                      0x4
#define BAR1_REG_BAR1_PREFETCH_MASK                                                                                   0x8
#define BAR1_REG_BAR1_PREFETCH_SHFT                                                                                   0x3
#define BAR1_REG_BAR1_TYPE_MASK                                                                                       0x6
#define BAR1_REG_BAR1_TYPE_SHFT                                                                                       0x1
#define BAR1_REG_BAR1_MEM_IO_MASK                                                                                     0x1
#define BAR1_REG_BAR1_MEM_IO_SHFT                                                                                     0x0

/* TYPE0_BAR1_REG_1 */
#define TYPE0_BAR1_REG_1                                                                                              ( 0x14 )
#define TYPE0_BAR1_REG_1_BAR1_START_MASK                                                                              0xFFFFFFF0
#define TYPE0_BAR1_REG_1_BAR1_START_SHFT                                                                              0x4
#define TYPE0_BAR1_REG_1_BAR1_PREFETCH_MASK                                                                           0x8
#define TYPE0_BAR1_REG_1_BAR1_PREFETCH_SHFT                                                                           0x3
#define TYPE0_BAR1_REG_1_BAR1_TYPE_MASK                                                                               0x6
#define TYPE0_BAR1_REG_1_BAR1_TYPE_SHFT                                                                               0x1
#define TYPE0_BAR1_REG_1_BAR1_MEM_IO_MASK                                                                             0x1
#define TYPE0_BAR1_REG_1_BAR1_MEM_IO_SHFT                                                                             0x0

/* SEC_LAT_TIMER_SUB_BUS_SEC_BUS_PRI_BUS_REG */
#define SEC_LAT_TIMER_SUB_BUS_SEC_BUS_PRI_BUS_REG                                                                     ( 0x18 )
#define SEC_LAT_TIMER_SUB_BUS_SEC_BUS_PRI_BUS_REG_SEC_LAT_TIMER_MASK                                                  0xFF000000
#define SEC_LAT_TIMER_SUB_BUS_SEC_BUS_PRI_BUS_REG_SEC_LAT_TIMER_SHFT                                                  0x18
#define SEC_LAT_TIMER_SUB_BUS_SEC_BUS_PRI_BUS_REG_SUB_BUS_MASK                                                        0xFF0000
#define SEC_LAT_TIMER_SUB_BUS_SEC_BUS_PRI_BUS_REG_SUB_BUS_SHFT                                                        0x10
#define SEC_LAT_TIMER_SUB_BUS_SEC_BUS_PRI_BUS_REG_SEC_BUS_MASK                                                        0xFF00
#define SEC_LAT_TIMER_SUB_BUS_SEC_BUS_PRI_BUS_REG_SEC_BUS_SHFT                                                        0x8
#define SEC_LAT_TIMER_SUB_BUS_SEC_BUS_PRI_BUS_REG_PRIM_BUS_MASK                                                       0xFF
#define SEC_LAT_TIMER_SUB_BUS_SEC_BUS_PRI_BUS_REG_PRIM_BUS_SHFT                                                       0x0

/* TYPE0_BAR2_REG_1 */
#define TYPE0_BAR2_REG_1                                                                                              ( 0x18 )
#define TYPE0_BAR2_REG_1_BAR2_START_MASK                                                                              0xFFFFFFF0
#define TYPE0_BAR2_REG_1_BAR2_START_SHFT                                                                              0x4
#define TYPE0_BAR2_REG_1_BAR2_PREFETCH_MASK                                                                           0x8
#define TYPE0_BAR2_REG_1_BAR2_PREFETCH_SHFT                                                                           0x3
#define TYPE0_BAR2_REG_1_BAR2_TYPE_MASK                                                                               0x6
#define TYPE0_BAR2_REG_1_BAR2_TYPE_SHFT                                                                               0x1
#define TYPE0_BAR2_REG_1_BAR2_MEM_IO_MASK                                                                             0x1
#define TYPE0_BAR2_REG_1_BAR2_MEM_IO_SHFT                                                                             0x0

/* SEC_STAT_IO_LIMIT_IO_BASE_REG */
#define SEC_STAT_IO_LIMIT_IO_BASE_REG                                                                                 ( 0x1C )
#define SEC_STAT_IO_LIMIT_IO_BASE_REG_SEC_STAT_DPE_MASK                                                               0x80000000
#define SEC_STAT_IO_LIMIT_IO_BASE_REG_SEC_STAT_DPE_SHFT                                                               0x1F
#define SEC_STAT_IO_LIMIT_IO_BASE_REG_SEC_STAT_RCVD_SYS_ERR_MASK                                                      0x40000000
#define SEC_STAT_IO_LIMIT_IO_BASE_REG_SEC_STAT_RCVD_SYS_ERR_SHFT                                                      0x1E
#define SEC_STAT_IO_LIMIT_IO_BASE_REG_SEC_STAT_RCVD_MSTR_ABRT_MASK                                                    0x20000000
#define SEC_STAT_IO_LIMIT_IO_BASE_REG_SEC_STAT_RCVD_MSTR_ABRT_SHFT                                                    0x1D
#define SEC_STAT_IO_LIMIT_IO_BASE_REG_SEC_STAT_RCVD_TRGT_ABRT_MASK                                                    0x10000000
#define SEC_STAT_IO_LIMIT_IO_BASE_REG_SEC_STAT_RCVD_TRGT_ABRT_SHFT                                                    0x1C
#define SEC_STAT_IO_LIMIT_IO_BASE_REG_SEC_STAT_SIG_TRGT_ABRT_MASK                                                     0x8000000
#define SEC_STAT_IO_LIMIT_IO_BASE_REG_SEC_STAT_SIG_TRGT_ABRT_SHFT                                                     0x1B
#define SEC_STAT_IO_LIMIT_IO_BASE_REG_SEC_STAT_MDPE_MASK                                                              0x1000000
#define SEC_STAT_IO_LIMIT_IO_BASE_REG_SEC_STAT_MDPE_SHFT                                                              0x18
#define SEC_STAT_IO_LIMIT_IO_BASE_REG_SEC_STAT_RESERV_MASK                                                            0x7F0000
#define SEC_STAT_IO_LIMIT_IO_BASE_REG_SEC_STAT_RESERV_SHFT                                                            0x10
#define SEC_STAT_IO_LIMIT_IO_BASE_REG_IO_LIMIT_MASK                                                                   0xF000
#define SEC_STAT_IO_LIMIT_IO_BASE_REG_IO_LIMIT_SHFT                                                                   0xC
#define SEC_STAT_IO_LIMIT_IO_BASE_REG_IO_RESERV1_MASK                                                                 0xE00
#define SEC_STAT_IO_LIMIT_IO_BASE_REG_IO_RESERV1_SHFT                                                                 0x9
#define SEC_STAT_IO_LIMIT_IO_BASE_REG_IO_DECODE_BIT8_MASK                                                             0x100
#define SEC_STAT_IO_LIMIT_IO_BASE_REG_IO_DECODE_BIT8_SHFT                                                             0x8
#define SEC_STAT_IO_LIMIT_IO_BASE_REG_IO_BASE_MASK                                                                    0xF0
#define SEC_STAT_IO_LIMIT_IO_BASE_REG_IO_BASE_SHFT                                                                    0x4
#define SEC_STAT_IO_LIMIT_IO_BASE_REG_IO_RESERV_MASK                                                                  0xE
#define SEC_STAT_IO_LIMIT_IO_BASE_REG_IO_RESERV_SHFT                                                                  0x1
#define SEC_STAT_IO_LIMIT_IO_BASE_REG_IO_DECODE_MASK                                                                  0x1
#define SEC_STAT_IO_LIMIT_IO_BASE_REG_IO_DECODE_SHFT                                                                  0x0

/* TYPE0_BAR3_REG_1 */
#define TYPE0_BAR3_REG_1                                                                                              ( 0x1C )
#define TYPE0_BAR3_REG_1_BAR3_START_MASK                                                                              0xFFFFFFF0
#define TYPE0_BAR3_REG_1_BAR3_START_SHFT                                                                              0x4
#define TYPE0_BAR3_REG_1_BAR3_PREFETCH_MASK                                                                           0x8
#define TYPE0_BAR3_REG_1_BAR3_PREFETCH_SHFT                                                                           0x3
#define TYPE0_BAR3_REG_1_BAR3_TYPE_MASK                                                                               0x6
#define TYPE0_BAR3_REG_1_BAR3_TYPE_SHFT                                                                               0x1
#define TYPE0_BAR3_REG_1_BAR3_MEM_IO_MASK                                                                             0x1
#define TYPE0_BAR3_REG_1_BAR3_MEM_IO_SHFT                                                                             0x0

/* MEM_LIMIT_MEM_BASE_REG */
#define MEM_LIMIT_MEM_BASE_REG                                                                                        ( 0x20 )
#define MEM_LIMIT_MEM_BASE_REG_MEM_LIMIT_MASK                                                                         0xFFF00000
#define MEM_LIMIT_MEM_BASE_REG_MEM_LIMIT_SHFT                                                                         0x14
#define MEM_LIMIT_MEM_BASE_REG_MEM_LIMIT_RESERV_MASK                                                                  0xF0000
#define MEM_LIMIT_MEM_BASE_REG_MEM_LIMIT_RESERV_SHFT                                                                  0x10
#define MEM_LIMIT_MEM_BASE_REG_MEM_BASE_MASK                                                                          0xFFF0
#define MEM_LIMIT_MEM_BASE_REG_MEM_BASE_SHFT                                                                          0x4
#define MEM_LIMIT_MEM_BASE_REG_MEM_BASE_RESERV_MASK                                                                   0xF
#define MEM_LIMIT_MEM_BASE_REG_MEM_BASE_RESERV_SHFT                                                                   0x0

/* TYPE0_BAR4_REG_1 */
#define TYPE0_BAR4_REG_1                                                                                              ( 0x20 )
#define TYPE0_BAR4_REG_1_BAR4_START_MASK                                                                              0xFFFFFFF0
#define TYPE0_BAR4_REG_1_BAR4_START_SHFT                                                                              0x4
#define TYPE0_BAR4_REG_1_BAR4_PREFETCH_MASK                                                                           0x8
#define TYPE0_BAR4_REG_1_BAR4_PREFETCH_SHFT                                                                           0x3
#define TYPE0_BAR4_REG_1_BAR4_TYPE_MASK                                                                               0x6
#define TYPE0_BAR4_REG_1_BAR4_TYPE_SHFT                                                                               0x1
#define TYPE0_BAR4_REG_1_BAR4_MEM_IO_MASK                                                                             0x1
#define TYPE0_BAR4_REG_1_BAR4_MEM_IO_SHFT                                                                             0x0

/* PREF_MEM_LIMIT_PREF_MEM_BASE_REG */
#define PREF_MEM_LIMIT_PREF_MEM_BASE_REG                                                                              ( 0x24 )
#define PREF_MEM_LIMIT_PREF_MEM_BASE_REG_PREF_MEM_LIMIT_MASK                                                          0xFFF00000
#define PREF_MEM_LIMIT_PREF_MEM_BASE_REG_PREF_MEM_LIMIT_SHFT                                                          0x14
#define PREF_MEM_LIMIT_PREF_MEM_BASE_REG_PREF_RESERV1_MASK                                                            0xE0000
#define PREF_MEM_LIMIT_PREF_MEM_BASE_REG_PREF_RESERV1_SHFT                                                            0x11
#define PREF_MEM_LIMIT_PREF_MEM_BASE_REG_PREF_MEM_LIMIT_DECODE_MASK                                                   0x10000
#define PREF_MEM_LIMIT_PREF_MEM_BASE_REG_PREF_MEM_LIMIT_DECODE_SHFT                                                   0x10
#define PREF_MEM_LIMIT_PREF_MEM_BASE_REG_PREF_MEM_BASE_MASK                                                           0xFFF0
#define PREF_MEM_LIMIT_PREF_MEM_BASE_REG_PREF_MEM_BASE_SHFT                                                           0x4
#define PREF_MEM_LIMIT_PREF_MEM_BASE_REG_PREF_RESERV_MASK                                                             0xE
#define PREF_MEM_LIMIT_PREF_MEM_BASE_REG_PREF_RESERV_SHFT                                                             0x1
#define PREF_MEM_LIMIT_PREF_MEM_BASE_REG_PREF_MEM_DECODE_MASK                                                         0x1
#define PREF_MEM_LIMIT_PREF_MEM_BASE_REG_PREF_MEM_DECODE_SHFT                                                         0x0

/* TYPE0_BAR5_REG_1 */
#define TYPE0_BAR5_REG_1                                                                                              ( 0x24 )
#define TYPE0_BAR5_REG_1_BAR5_START_MASK                                                                              0xFFFFFFF0
#define TYPE0_BAR5_REG_1_BAR5_START_SHFT                                                                              0x4
#define TYPE0_BAR5_REG_1_BAR5_PREFETCH_MASK                                                                           0x8
#define TYPE0_BAR5_REG_1_BAR5_PREFETCH_SHFT                                                                           0x3
#define TYPE0_BAR5_REG_1_BAR5_TYPE_MASK                                                                               0x6
#define TYPE0_BAR5_REG_1_BAR5_TYPE_SHFT                                                                               0x1
#define TYPE0_BAR5_REG_1_BAR5_MEM_IO_MASK                                                                             0x1
#define TYPE0_BAR5_REG_1_BAR5_MEM_IO_SHFT                                                                             0x0

/* PREF_BASE_UPPER_REG */
#define PREF_BASE_UPPER_REG                                                                                           ( 0x28 )
#define PREF_BASE_UPPER_REG_PREF_MEM_BASE_UPPER_MASK                                                                  0xFFFFFFFF
#define PREF_BASE_UPPER_REG_PREF_MEM_BASE_UPPER_SHFT                                                                  0x0

/* TYPE0_CARDBUS_CIS_PTR_REG_1 */
#define TYPE0_CARDBUS_CIS_PTR_REG_1                                                                                   ( 0x28 )
#define TYPE0_CARDBUS_CIS_PTR_REG_1_CARDBUS_CIS_POINTER_MASK                                                          0xFFFFFFFF
#define TYPE0_CARDBUS_CIS_PTR_REG_1_CARDBUS_CIS_POINTER_SHFT                                                          0x0

/* PREF_LIMIT_UPPER_REG */
#define PREF_LIMIT_UPPER_REG                                                                                          ( 0x2C )
#define PREF_LIMIT_UPPER_REG_PREF_MEM_LIMIT_UPPER_MASK                                                                0xFFFFFFFF
#define PREF_LIMIT_UPPER_REG_PREF_MEM_LIMIT_UPPER_SHFT                                                                0x0

/* TYPE0_SUBSYSTEM_ID_SUBSYSTEM_VENDOR_ID_REG_1 */
#define TYPE0_SUBSYSTEM_ID_SUBSYSTEM_VENDOR_ID_REG_1                                                                  ( 0x2C )
#define TYPE0_SUBSYSTEM_ID_SUBSYSTEM_VENDOR_ID_REG_1_SUBSYS_DEV_ID_MASK                                               0xFFFF0000
#define TYPE0_SUBSYSTEM_ID_SUBSYSTEM_VENDOR_ID_REG_1_SUBSYS_DEV_ID_SHFT                                               0x10
#define TYPE0_SUBSYSTEM_ID_SUBSYSTEM_VENDOR_ID_REG_1_SUBSYS_VENDOR_ID_MASK                                            0xFFFF
#define TYPE0_SUBSYSTEM_ID_SUBSYSTEM_VENDOR_ID_REG_1_SUBSYS_VENDOR_ID_SHFT                                            0x0

/* IO_LIMIT_UPPER_IO_BASE_UPPER_REG */
#define IO_LIMIT_UPPER_IO_BASE_UPPER_REG                                                                              ( 0x30 )
#define IO_LIMIT_UPPER_IO_BASE_UPPER_REG_IO_LIMIT_UPPER_MASK                                                          0xFFFF0000
#define IO_LIMIT_UPPER_IO_BASE_UPPER_REG_IO_LIMIT_UPPER_SHFT                                                          0x10
#define IO_LIMIT_UPPER_IO_BASE_UPPER_REG_IO_BASE_UPPER_MASK                                                           0xFFFF
#define IO_LIMIT_UPPER_IO_BASE_UPPER_REG_IO_BASE_UPPER_SHFT                                                           0x0

/* TYPE0_EXP_ROM_BASE_ADDR_REG_1 */
#define TYPE0_EXP_ROM_BASE_ADDR_REG_1                                                                                 ( 0x30 )
#define TYPE0_EXP_ROM_BASE_ADDR_REG_1_EXP_ROM_BASE_ADDRESS_MASK                                                       0xFFFFF800
#define TYPE0_EXP_ROM_BASE_ADDR_REG_1_EXP_ROM_BASE_ADDRESS_SHFT                                                       0xB
#define TYPE0_EXP_ROM_BASE_ADDR_REG_1_ROM_BAR_ENABLE_MASK                                                             0x1
#define TYPE0_EXP_ROM_BASE_ADDR_REG_1_ROM_BAR_ENABLE_SHFT                                                             0x0

/* TYPE1_CAP_PTR_REG */
#define TYPE1_CAP_PTR_REG                                                                                             ( 0x34 )
#define TYPE1_CAP_PTR_REG_CAP_POINTER_MASK                                                                            0xFF
#define TYPE1_CAP_PTR_REG_CAP_POINTER_SHFT                                                                            0x0

/* TYPE0_PCI_CAP_PTR_REG_1 */
#define TYPE0_PCI_CAP_PTR_REG_1                                                                                       ( 0x34 )
#define TYPE0_PCI_CAP_PTR_REG_1_CAP_POINTER_MASK                                                                      0xFF
#define TYPE0_PCI_CAP_PTR_REG_1_CAP_POINTER_SHFT                                                                      0x0

/* TYPE1_EXP_ROM_BASE_REG */
#define TYPE1_EXP_ROM_BASE_REG                                                                                        ( 0x38 )
#define TYPE1_EXP_ROM_BASE_REG_EXP_ROM_BASE_ADDRESS_MASK                                                              0xFFFFF800
#define TYPE1_EXP_ROM_BASE_REG_EXP_ROM_BASE_ADDRESS_SHFT                                                              0xB
#define TYPE1_EXP_ROM_BASE_REG_ROM_BAR_ENABLE_MASK                                                                    0x1
#define TYPE1_EXP_ROM_BASE_REG_ROM_BAR_ENABLE_SHFT                                                                    0x0

/* TYPE0_DUMMY_TYPE1_EXP_ROM_BASE_REG */
#define TYPE0_DUMMY_TYPE1_EXP_ROM_BASE_REG                                                                            ( 0x38 )
#define TYPE0_DUMMY_TYPE1_EXP_ROM_BASE_REG_EXP_ROM_BASE_ADDRESS_MASK                                                  0xFFFFF800
#define TYPE0_DUMMY_TYPE1_EXP_ROM_BASE_REG_EXP_ROM_BASE_ADDRESS_SHFT                                                  0xB
#define TYPE0_DUMMY_TYPE1_EXP_ROM_BASE_REG_ROM_BAR_ENABLE_MASK                                                        0x1
#define TYPE0_DUMMY_TYPE1_EXP_ROM_BASE_REG_ROM_BAR_ENABLE_SHFT                                                        0x0

/* BRIDGE_CTRL_INT_PIN_INT_LINE_REG */
#define BRIDGE_CTRL_INT_PIN_INT_LINE_REG                                                                              ( 0x3C )
#define BRIDGE_CTRL_INT_PIN_INT_LINE_REG_BRIDGE_CTRL_RESERV_MASK                                                      0xFF800000
#define BRIDGE_CTRL_INT_PIN_INT_LINE_REG_BRIDGE_CTRL_RESERV_SHFT                                                      0x17
#define BRIDGE_CTRL_INT_PIN_INT_LINE_REG_SBR_MASK                                                                     0x400000
#define BRIDGE_CTRL_INT_PIN_INT_LINE_REG_SBR_SHFT                                                                     0x16
#define BRIDGE_CTRL_INT_PIN_INT_LINE_REG_MSTR_ABORT_MODE_MASK                                                         0x200000
#define BRIDGE_CTRL_INT_PIN_INT_LINE_REG_MSTR_ABORT_MODE_SHFT                                                         0x15
#define BRIDGE_CTRL_INT_PIN_INT_LINE_REG_VGA_16B_DEC_MASK                                                             0x100000
#define BRIDGE_CTRL_INT_PIN_INT_LINE_REG_VGA_16B_DEC_SHFT                                                             0x14
#define BRIDGE_CTRL_INT_PIN_INT_LINE_REG_VGA_EN_MASK                                                                  0x80000
#define BRIDGE_CTRL_INT_PIN_INT_LINE_REG_VGA_EN_SHFT                                                                  0x13
#define BRIDGE_CTRL_INT_PIN_INT_LINE_REG_ISA_EN_MASK                                                                  0x40000
#define BRIDGE_CTRL_INT_PIN_INT_LINE_REG_ISA_EN_SHFT                                                                  0x12
#define BRIDGE_CTRL_INT_PIN_INT_LINE_REG_SERR_EN_MASK                                                                 0x20000
#define BRIDGE_CTRL_INT_PIN_INT_LINE_REG_SERR_EN_SHFT                                                                 0x11
#define BRIDGE_CTRL_INT_PIN_INT_LINE_REG_PERE_MASK                                                                    0x10000
#define BRIDGE_CTRL_INT_PIN_INT_LINE_REG_PERE_SHFT                                                                    0x10
#define BRIDGE_CTRL_INT_PIN_INT_LINE_REG_INT_PIN_MASK                                                                 0xFF00
#define BRIDGE_CTRL_INT_PIN_INT_LINE_REG_INT_PIN_SHFT                                                                 0x8
#define BRIDGE_CTRL_INT_PIN_INT_LINE_REG_INT_LINE_MASK                                                                0xFF
#define BRIDGE_CTRL_INT_PIN_INT_LINE_REG_INT_LINE_SHFT                                                                0x0

/* TYPE0_MAX_LATENCY_MIN_GRANT_INTERRUPT_PIN_INTERRUPT_LINE_REG_1 */
#define TYPE0_MAX_LATENCY_MIN_GRANT_INTERRUPT_PIN_INTERRUPT_LINE_REG_1                                                ( 0x3C )
#define TYPE0_MAX_LATENCY_MIN_GRANT_INTERRUPT_PIN_INTERRUPT_LINE_REG_1_INT_PIN_MASK                                   0xFF00
#define TYPE0_MAX_LATENCY_MIN_GRANT_INTERRUPT_PIN_INTERRUPT_LINE_REG_1_INT_PIN_SHFT                                   0x8
#define TYPE0_MAX_LATENCY_MIN_GRANT_INTERRUPT_PIN_INTERRUPT_LINE_REG_1_INT_LINE_MASK                                  0xFF
#define TYPE0_MAX_LATENCY_MIN_GRANT_INTERRUPT_PIN_INTERRUPT_LINE_REG_1_INT_LINE_SHFT                                  0x0

/* CAP_ID_NXT_PTR_REG */
#define CAP_ID_NXT_PTR_REG                                                                                            ( 0x40 )
#define CAP_ID_NXT_PTR_REG_PME_SUPPORT_MASK                                                                           0xF8000000
#define CAP_ID_NXT_PTR_REG_PME_SUPPORT_SHFT                                                                           0x1B
#define CAP_ID_NXT_PTR_REG_D2_SUPPORT_MASK                                                                            0x4000000
#define CAP_ID_NXT_PTR_REG_D2_SUPPORT_SHFT                                                                            0x1A
#define CAP_ID_NXT_PTR_REG_D1_SUPPORT_MASK                                                                            0x2000000
#define CAP_ID_NXT_PTR_REG_D1_SUPPORT_SHFT                                                                            0x19
#define CAP_ID_NXT_PTR_REG_AUX_CURR_MASK                                                                              0x1C00000
#define CAP_ID_NXT_PTR_REG_AUX_CURR_SHFT                                                                              0x16
#define CAP_ID_NXT_PTR_REG_DSI_MASK                                                                                   0x200000
#define CAP_ID_NXT_PTR_REG_DSI_SHFT                                                                                   0x15
#define CAP_ID_NXT_PTR_REG_PME_CLK_MASK                                                                               0x80000
#define CAP_ID_NXT_PTR_REG_PME_CLK_SHFT                                                                               0x13
#define CAP_ID_NXT_PTR_REG_PM_SPEC_VER_MASK                                                                           0x70000
#define CAP_ID_NXT_PTR_REG_PM_SPEC_VER_SHFT                                                                           0x10
#define CAP_ID_NXT_PTR_REG_PM_NEXT_POINTER_MASK                                                                       0xFF00
#define CAP_ID_NXT_PTR_REG_PM_NEXT_POINTER_SHFT                                                                       0x8
#define CAP_ID_NXT_PTR_REG_PM_CAP_ID_MASK                                                                             0xFF
#define CAP_ID_NXT_PTR_REG_PM_CAP_ID_SHFT                                                                             0x0

/* TYPE0_CAP_ID_NXT_PTR_REG_1 */
#define TYPE0_CAP_ID_NXT_PTR_REG_1                                                                                    ( 0x40 )
#define TYPE0_CAP_ID_NXT_PTR_REG_1_PME_SUPPORT_MASK                                                                   0xF8000000
#define TYPE0_CAP_ID_NXT_PTR_REG_1_PME_SUPPORT_SHFT                                                                   0x1B
#define TYPE0_CAP_ID_NXT_PTR_REG_1_D2_SUPPORT_MASK                                                                    0x4000000
#define TYPE0_CAP_ID_NXT_PTR_REG_1_D2_SUPPORT_SHFT                                                                    0x1A
#define TYPE0_CAP_ID_NXT_PTR_REG_1_D1_SUPPORT_MASK                                                                    0x2000000
#define TYPE0_CAP_ID_NXT_PTR_REG_1_D1_SUPPORT_SHFT                                                                    0x19
#define TYPE0_CAP_ID_NXT_PTR_REG_1_AUX_CURR_MASK                                                                      0x1C00000
#define TYPE0_CAP_ID_NXT_PTR_REG_1_AUX_CURR_SHFT                                                                      0x16
#define TYPE0_CAP_ID_NXT_PTR_REG_1_DSI_MASK                                                                           0x200000
#define TYPE0_CAP_ID_NXT_PTR_REG_1_DSI_SHFT                                                                           0x15
#define TYPE0_CAP_ID_NXT_PTR_REG_1_PME_CLK_MASK                                                                       0x80000
#define TYPE0_CAP_ID_NXT_PTR_REG_1_PME_CLK_SHFT                                                                       0x13
#define TYPE0_CAP_ID_NXT_PTR_REG_1_PM_SPEC_VER_MASK                                                                   0x70000
#define TYPE0_CAP_ID_NXT_PTR_REG_1_PM_SPEC_VER_SHFT                                                                   0x10
#define TYPE0_CAP_ID_NXT_PTR_REG_1_PM_NEXT_POINTER_MASK                                                               0xFF00
#define TYPE0_CAP_ID_NXT_PTR_REG_1_PM_NEXT_POINTER_SHFT                                                               0x8
#define TYPE0_CAP_ID_NXT_PTR_REG_1_PM_CAP_ID_MASK                                                                     0xFF
#define TYPE0_CAP_ID_NXT_PTR_REG_1_PM_CAP_ID_SHFT                                                                     0x0

/* CON_STATUS_REG */
#define CON_STATUS_REG                                                                                                ( 0x44 )
#define CON_STATUS_REG_DATA_REG_ADD_INFO_MASK                                                                         0xFF000000
#define CON_STATUS_REG_DATA_REG_ADD_INFO_SHFT                                                                         0x18
#define CON_STATUS_REG_BUS_PWR_CLK_CON_EN_MASK                                                                        0x800000
#define CON_STATUS_REG_BUS_PWR_CLK_CON_EN_SHFT                                                                        0x17
#define CON_STATUS_REG_B2_B3_SUPPORT_MASK                                                                             0x400000
#define CON_STATUS_REG_B2_B3_SUPPORT_SHFT                                                                             0x16
#define CON_STATUS_REG_PME_STATUS_MASK                                                                                0x8000
#define CON_STATUS_REG_PME_STATUS_SHFT                                                                                0xF
#define CON_STATUS_REG_DATA_SCALE_MASK                                                                                0x6000
#define CON_STATUS_REG_DATA_SCALE_SHFT                                                                                0xD
#define CON_STATUS_REG_DATA_SELECT_MASK                                                                               0x1E00
#define CON_STATUS_REG_DATA_SELECT_SHFT                                                                               0x9
#define CON_STATUS_REG_PME_ENABLE_MASK                                                                                0x100
#define CON_STATUS_REG_PME_ENABLE_SHFT                                                                                0x8
#define CON_STATUS_REG_NO_SOFT_RST_MASK                                                                               0x8
#define CON_STATUS_REG_NO_SOFT_RST_SHFT                                                                               0x3
#define CON_STATUS_REG_POWER_STATE_MASK                                                                               0x3
#define CON_STATUS_REG_POWER_STATE_SHFT                                                                               0x0

/* TYPE0_CON_STATUS_REG_1 */
#define TYPE0_CON_STATUS_REG_1                                                                                        ( 0x44 )
#define TYPE0_CON_STATUS_REG_1_DATA_REG_ADD_INFO_MASK                                                                 0xFF000000
#define TYPE0_CON_STATUS_REG_1_DATA_REG_ADD_INFO_SHFT                                                                 0x18
#define TYPE0_CON_STATUS_REG_1_BUS_PWR_CLK_CON_EN_MASK                                                                0x800000
#define TYPE0_CON_STATUS_REG_1_BUS_PWR_CLK_CON_EN_SHFT                                                                0x17
#define TYPE0_CON_STATUS_REG_1_B2_B3_SUPPORT_MASK                                                                     0x400000
#define TYPE0_CON_STATUS_REG_1_B2_B3_SUPPORT_SHFT                                                                     0x16
#define TYPE0_CON_STATUS_REG_1_PME_STATUS_MASK                                                                        0x8000
#define TYPE0_CON_STATUS_REG_1_PME_STATUS_SHFT                                                                        0xF
#define TYPE0_CON_STATUS_REG_1_DATA_SCALE_MASK                                                                        0x6000
#define TYPE0_CON_STATUS_REG_1_DATA_SCALE_SHFT                                                                        0xD
#define TYPE0_CON_STATUS_REG_1_DATA_SELECT_MASK                                                                       0x1E00
#define TYPE0_CON_STATUS_REG_1_DATA_SELECT_SHFT                                                                       0x9
#define TYPE0_CON_STATUS_REG_1_PME_ENABLE_MASK                                                                        0x100
#define TYPE0_CON_STATUS_REG_1_PME_ENABLE_SHFT                                                                        0x8
#define TYPE0_CON_STATUS_REG_1_NO_SOFT_RST_MASK                                                                       0x8
#define TYPE0_CON_STATUS_REG_1_NO_SOFT_RST_SHFT                                                                       0x3
#define TYPE0_CON_STATUS_REG_1_POWER_STATE_MASK                                                                       0x3
#define TYPE0_CON_STATUS_REG_1_POWER_STATE_SHFT                                                                       0x0

/* PCI_MSI_CAP_ID_NEXT_CTRL_REG */
#define PCI_MSI_CAP_ID_NEXT_CTRL_REG                                                                                  ( 0x50 )
#define PCI_MSI_CAP_ID_NEXT_CTRL_REG_PCI_PVM_SUPPORT_MASK                                                             0x1000000
#define PCI_MSI_CAP_ID_NEXT_CTRL_REG_PCI_PVM_SUPPORT_SHFT                                                             0x18
#define PCI_MSI_CAP_ID_NEXT_CTRL_REG_PCI_MSI_64_BIT_ADDR_CAP_MASK                                                     0x800000
#define PCI_MSI_CAP_ID_NEXT_CTRL_REG_PCI_MSI_64_BIT_ADDR_CAP_SHFT                                                     0x17
#define PCI_MSI_CAP_ID_NEXT_CTRL_REG_PCI_MSI_MULTIPLE_MSG_EN_MASK                                                     0x700000
#define PCI_MSI_CAP_ID_NEXT_CTRL_REG_PCI_MSI_MULTIPLE_MSG_EN_SHFT                                                     0x14
#define PCI_MSI_CAP_ID_NEXT_CTRL_REG_PCI_MSI_MULTIPLE_MSG_CAP_MASK                                                    0xE0000
#define PCI_MSI_CAP_ID_NEXT_CTRL_REG_PCI_MSI_MULTIPLE_MSG_CAP_SHFT                                                    0x11
#define PCI_MSI_CAP_ID_NEXT_CTRL_REG_PCI_MSI_ENABLE_MASK                                                              0x10000
#define PCI_MSI_CAP_ID_NEXT_CTRL_REG_PCI_MSI_ENABLE_SHFT                                                              0x10
#define PCI_MSI_CAP_ID_NEXT_CTRL_REG_PCI_MSI_CAP_NEXT_OFFSET_MASK                                                     0xFF00
#define PCI_MSI_CAP_ID_NEXT_CTRL_REG_PCI_MSI_CAP_NEXT_OFFSET_SHFT                                                     0x8
#define PCI_MSI_CAP_ID_NEXT_CTRL_REG_PCI_MSI_CAP_ID_MASK                                                              0xFF
#define PCI_MSI_CAP_ID_NEXT_CTRL_REG_PCI_MSI_CAP_ID_SHFT                                                              0x0

/* TYPE0_PCI_MSI_CAP_ID_NEXT_CTRL_REG_1 */
#define TYPE0_PCI_MSI_CAP_ID_NEXT_CTRL_REG_1                                                                          ( 0x50 )
#define TYPE0_PCI_MSI_CAP_ID_NEXT_CTRL_REG_1_PCI_PVM_SUPPORT_MASK                                                     0x1000000
#define TYPE0_PCI_MSI_CAP_ID_NEXT_CTRL_REG_1_PCI_PVM_SUPPORT_SHFT                                                     0x18
#define TYPE0_PCI_MSI_CAP_ID_NEXT_CTRL_REG_1_PCI_MSI_64_BIT_ADDR_CAP_MASK                                             0x800000
#define TYPE0_PCI_MSI_CAP_ID_NEXT_CTRL_REG_1_PCI_MSI_64_BIT_ADDR_CAP_SHFT                                             0x17
#define TYPE0_PCI_MSI_CAP_ID_NEXT_CTRL_REG_1_PCI_MSI_MULTIPLE_MSG_EN_MASK                                             0x700000
#define TYPE0_PCI_MSI_CAP_ID_NEXT_CTRL_REG_1_PCI_MSI_MULTIPLE_MSG_EN_SHFT                                             0x14
#define TYPE0_PCI_MSI_CAP_ID_NEXT_CTRL_REG_1_PCI_MSI_MULTIPLE_MSG_CAP_MASK                                            0xE0000
#define TYPE0_PCI_MSI_CAP_ID_NEXT_CTRL_REG_1_PCI_MSI_MULTIPLE_MSG_CAP_SHFT                                            0x11
#define TYPE0_PCI_MSI_CAP_ID_NEXT_CTRL_REG_1_PCI_MSI_ENABLE_MASK                                                      0x10000
#define TYPE0_PCI_MSI_CAP_ID_NEXT_CTRL_REG_1_PCI_MSI_ENABLE_SHFT                                                      0x10
#define TYPE0_PCI_MSI_CAP_ID_NEXT_CTRL_REG_1_PCI_MSI_CAP_NEXT_OFFSET_MASK                                             0xFF00
#define TYPE0_PCI_MSI_CAP_ID_NEXT_CTRL_REG_1_PCI_MSI_CAP_NEXT_OFFSET_SHFT                                             0x8
#define TYPE0_PCI_MSI_CAP_ID_NEXT_CTRL_REG_1_PCI_MSI_CAP_ID_MASK                                                      0xFF
#define TYPE0_PCI_MSI_CAP_ID_NEXT_CTRL_REG_1_PCI_MSI_CAP_ID_SHFT                                                      0x0

/* MSI_LOWER_32 */
#define MSI_LOWER_32                                                                                                  ( 0x54 )
#define MSI_LOWER_32_PCI_MSI_LOWER_32_MASK                                                                            0xFFFFFFFC
#define MSI_LOWER_32_PCI_MSI_LOWER_32_SHFT                                                                            0x2

/* TYPE0_MSI_LOWER_32_1 */
#define TYPE0_MSI_LOWER_32_1                                                                                          ( 0x54 )
#define TYPE0_MSI_LOWER_32_1_PCI_MSI_LOWER_32_MASK                                                                    0xFFFFFFFC
#define TYPE0_MSI_LOWER_32_1_PCI_MSI_LOWER_32_SHFT                                                                    0x2

/* MSI_UPPER_32 */
#define MSI_UPPER_32                                                                                                  ( 0x58 )
#define MSI_UPPER_32_PCI_MSI_UPPER_32_MASK                                                                            0xFFFFFFFF
#define MSI_UPPER_32_PCI_MSI_UPPER_32_SHFT                                                                            0x0

/* TYPE0_MSI_UPPER_32_1 */
#define TYPE0_MSI_UPPER_32_1                                                                                          ( 0x58 )
#define TYPE0_MSI_UPPER_32_1_PCI_MSI_UPPER_32_MASK                                                                    0xFFFFFFFF
#define TYPE0_MSI_UPPER_32_1_PCI_MSI_UPPER_32_SHFT                                                                    0x0

/* MSI_DATA_REG */
#define MSI_DATA_REG                                                                                                  ( 0x5C )
#define MSI_DATA_REG_MSI_DATA_REG_MASK                                                                                0xFFFFFFFF
#define MSI_DATA_REG_MSI_DATA_REG_SHFT                                                                                0x0

/* TYPE0_MSI_DATA_REG_1 */
#define TYPE0_MSI_DATA_REG_1                                                                                          ( 0x5C )
#define TYPE0_MSI_DATA_REG_1_MSI_DATA_REG_MASK                                                                        0xFFFFFFFF
#define TYPE0_MSI_DATA_REG_1_MSI_DATA_REG_SHFT                                                                        0x0

/* MSI_MASK_REG */
#define MSI_MASK_REG                                                                                                  ( 0x60 )
#define MSI_MASK_REG_MSI_MASK_REG_MASK                                                                                0xFFFFFFFF
#define MSI_MASK_REG_MSI_MASK_REG_SHFT                                                                                0x0

/* TYPE0_MSI_MASK_REG_1 */
#define TYPE0_MSI_MASK_REG_1                                                                                          ( 0x60 )
#define TYPE0_MSI_MASK_REG_1_MSI_MASK_REG_MASK                                                                        0xFFFFFFFF
#define TYPE0_MSI_MASK_REG_1_MSI_MASK_REG_SHFT                                                                        0x0

/* MSI_PENDING_BIT_REG */
#define MSI_PENDING_BIT_REG                                                                                           ( 0x64 )
#define MSI_PENDING_BIT_REG_PCI_MSI_PENDING_BIT_MASK                                                                  0xFFFFFFFF
#define MSI_PENDING_BIT_REG_PCI_MSI_PENDING_BIT_SHFT                                                                  0x0

/* TYPE0_MSI_PENDING_BIT_REG_1 */
#define TYPE0_MSI_PENDING_BIT_REG_1                                                                                   ( 0x64 )
#define TYPE0_MSI_PENDING_BIT_REG_1_PCI_MSI_PENDING_BIT_MASK                                                          0xFFFFFFFF
#define TYPE0_MSI_PENDING_BIT_REG_1_PCI_MSI_PENDING_BIT_SHFT                                                          0x0

/* PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG */
#define PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG                                                                    ( 0x70 )
#define PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_RSVD_MASK                                                          0x40000000
#define PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_RSVD_SHFT                                                          0x1E
#define PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_PCIE_INT_MSG_NUM_MASK                                              0x3E000000
#define PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_PCIE_INT_MSG_NUM_SHFT                                              0x19
#define PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_PCIE_SLOT_IMP_MASK                                                 0x1000000
#define PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_PCIE_SLOT_IMP_SHFT                                                 0x18
#define PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_PCIE_DEV_PORT_TYPE_MASK                                            0xF00000
#define PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_PCIE_DEV_PORT_TYPE_SHFT                                            0x14
#define PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_PCIE_CAP_REG_MASK                                                  0xF0000
#define PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_PCIE_CAP_REG_SHFT                                                  0x10
#define PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_PCIE_CAP_NEXT_PTR_MASK                                             0xFF00
#define PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_PCIE_CAP_NEXT_PTR_SHFT                                             0x8
#define PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_PCIE_CAP_ID_MASK                                                   0xFF
#define PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_PCIE_CAP_ID_SHFT                                                   0x0

/* TYPE0_PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_1 */
#define TYPE0_PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_1                                                            ( 0x70 )
#define TYPE0_PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_1_RSVD_MASK                                                  0x40000000
#define TYPE0_PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_1_RSVD_SHFT                                                  0x1E
#define TYPE0_PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_1_PCIE_INT_MSG_NUM_MASK                                      0x3E000000
#define TYPE0_PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_1_PCIE_INT_MSG_NUM_SHFT                                      0x19
#define TYPE0_PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_1_PCIE_SLOT_IMP_MASK                                         0x1000000
#define TYPE0_PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_1_PCIE_SLOT_IMP_SHFT                                         0x18
#define TYPE0_PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_1_PCIE_DEV_PORT_TYPE_MASK                                    0xF00000
#define TYPE0_PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_1_PCIE_DEV_PORT_TYPE_SHFT                                    0x14
#define TYPE0_PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_1_PCIE_CAP_REG_MASK                                          0xF0000
#define TYPE0_PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_1_PCIE_CAP_REG_SHFT                                          0x10
#define TYPE0_PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_1_PCIE_CAP_NEXT_PTR_MASK                                     0xFF00
#define TYPE0_PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_1_PCIE_CAP_NEXT_PTR_SHFT                                     0x8
#define TYPE0_PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_1_PCIE_CAP_ID_MASK                                           0xFF
#define TYPE0_PCIE_CAP_ID_PCIE_NEXT_CAP_PTR_PCIE_CAP_REG_1_PCIE_CAP_ID_SHFT                                           0x0

/* DEVICE_CAPABILITIES_REG */
#define DEVICE_CAPABILITIES_REG                                                                                       ( 0x74 )
#define DEVICE_CAPABILITIES_REG_PCIE_CAP_FLR_CAP_MASK                                                                 0x10000000
#define DEVICE_CAPABILITIES_REG_PCIE_CAP_FLR_CAP_SHFT                                                                 0x1C
#define DEVICE_CAPABILITIES_REG_PCIE_CAP_ROLE_BASED_ERR_REPORT_MASK                                                   0x8000
#define DEVICE_CAPABILITIES_REG_PCIE_CAP_ROLE_BASED_ERR_REPORT_SHFT                                                   0xF
#define DEVICE_CAPABILITIES_REG_PCIE_CAP_EP_L1_ACCPT_LATENCY_MASK                                                     0xE00
#define DEVICE_CAPABILITIES_REG_PCIE_CAP_EP_L1_ACCPT_LATENCY_SHFT                                                     0x9
#define DEVICE_CAPABILITIES_REG_PCIE_CAP_EP_L0S_ACCPT_LATENCY_MASK                                                    0x1C0
#define DEVICE_CAPABILITIES_REG_PCIE_CAP_EP_L0S_ACCPT_LATENCY_SHFT                                                    0x6
#define DEVICE_CAPABILITIES_REG_PCIE_CAP_EXT_TAG_SUPP_MASK                                                            0x20
#define DEVICE_CAPABILITIES_REG_PCIE_CAP_EXT_TAG_SUPP_SHFT                                                            0x5
#define DEVICE_CAPABILITIES_REG_PCIE_CAP_PHANTOM_FUNC_SUPPORT_MASK                                                    0x18
#define DEVICE_CAPABILITIES_REG_PCIE_CAP_PHANTOM_FUNC_SUPPORT_SHFT                                                    0x3
#define DEVICE_CAPABILITIES_REG_PCIE_CAP_MAX_PAYLOAD_SIZE_MASK                                                        0x7
#define DEVICE_CAPABILITIES_REG_PCIE_CAP_MAX_PAYLOAD_SIZE_SHFT                                                        0x0

/* TYPE0_DEVICE_CAPABILITIES_REG_1 */
#define TYPE0_DEVICE_CAPABILITIES_REG_1                                                                               ( 0x74 )
#define TYPE0_DEVICE_CAPABILITIES_REG_1_PCIE_CAP_FLR_CAP_MASK                                                         0x10000000
#define TYPE0_DEVICE_CAPABILITIES_REG_1_PCIE_CAP_FLR_CAP_SHFT                                                         0x1C
#define TYPE0_DEVICE_CAPABILITIES_REG_1_PCIE_CAP_ROLE_BASED_ERR_REPORT_MASK                                           0x8000
#define TYPE0_DEVICE_CAPABILITIES_REG_1_PCIE_CAP_ROLE_BASED_ERR_REPORT_SHFT                                           0xF
#define TYPE0_DEVICE_CAPABILITIES_REG_1_PCIE_CAP_EP_L1_ACCPT_LATENCY_MASK                                             0xE00
#define TYPE0_DEVICE_CAPABILITIES_REG_1_PCIE_CAP_EP_L1_ACCPT_LATENCY_SHFT                                             0x9
#define TYPE0_DEVICE_CAPABILITIES_REG_1_PCIE_CAP_EP_L0S_ACCPT_LATENCY_MASK                                            0x1C0
#define TYPE0_DEVICE_CAPABILITIES_REG_1_PCIE_CAP_EP_L0S_ACCPT_LATENCY_SHFT                                            0x6
#define TYPE0_DEVICE_CAPABILITIES_REG_1_PCIE_CAP_EXT_TAG_SUPP_MASK                                                    0x20
#define TYPE0_DEVICE_CAPABILITIES_REG_1_PCIE_CAP_EXT_TAG_SUPP_SHFT                                                    0x5
#define TYPE0_DEVICE_CAPABILITIES_REG_1_PCIE_CAP_PHANTOM_FUNC_SUPPORT_MASK                                            0x18
#define TYPE0_DEVICE_CAPABILITIES_REG_1_PCIE_CAP_PHANTOM_FUNC_SUPPORT_SHFT                                            0x3
#define TYPE0_DEVICE_CAPABILITIES_REG_1_PCIE_CAP_MAX_PAYLOAD_SIZE_MASK                                                0x7
#define TYPE0_DEVICE_CAPABILITIES_REG_1_PCIE_CAP_MAX_PAYLOAD_SIZE_SHFT                                                0x0

/* DEVICE_CONTROL_DEVICE_STATUS */
#define DEVICE_CONTROL_DEVICE_STATUS                                                                                  ( 0x78 )
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_TRANS_PENDING_MASK                                                      0x200000
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_TRANS_PENDING_SHFT                                                      0x15
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_AUX_POWER_DETECTED_MASK                                                 0x100000
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_AUX_POWER_DETECTED_SHFT                                                 0x14
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_UNSUPPORTED_REQ_DETECTED_MASK                                           0x80000
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_UNSUPPORTED_REQ_DETECTED_SHFT                                           0x13
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_FATAL_ERR_DETECTED_MASK                                                 0x40000
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_FATAL_ERR_DETECTED_SHFT                                                 0x12
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_NON_FATAL_ERR_DETECTED_MASK                                             0x20000
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_NON_FATAL_ERR_DETECTED_SHFT                                             0x11
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_CORR_ERR_DETECTED_MASK                                                  0x10000
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_CORR_ERR_DETECTED_SHFT                                                  0x10
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_INITIATE_FLR_MASK                                                       0x8000
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_INITIATE_FLR_SHFT                                                       0xF
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_MAX_READ_REQ_SIZE_MASK                                                  0x7000
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_MAX_READ_REQ_SIZE_SHFT                                                  0xC
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_EN_NO_SNOOP_MASK                                                        0x800
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_EN_NO_SNOOP_SHFT                                                        0xB
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_AUX_POWER_PM_EN_MASK                                                    0x400
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_AUX_POWER_PM_EN_SHFT                                                    0xA
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_PHANTOM_FUNC_EN_MASK                                                    0x200
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_PHANTOM_FUNC_EN_SHFT                                                    0x9
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_EXT_TAG_EN_MASK                                                         0x100
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_EXT_TAG_EN_SHFT                                                         0x8
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_MAX_PAYLOAD_SIZE_CS_MASK                                                0xE0
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_MAX_PAYLOAD_SIZE_CS_SHFT                                                0x5
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_EN_REL_ORDER_MASK                                                       0x10
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_EN_REL_ORDER_SHFT                                                       0x4
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_UNSUPPORT_REQ_REP_EN_MASK                                               0x8
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_UNSUPPORT_REQ_REP_EN_SHFT                                               0x3
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_FATAL_ERR_REPORT_EN_MASK                                                0x4
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_FATAL_ERR_REPORT_EN_SHFT                                                0x2
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_NON_FATAL_ERR_REPORT_EN_MASK                                            0x2
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_NON_FATAL_ERR_REPORT_EN_SHFT                                            0x1
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_CORR_ERR_REPORT_EN_MASK                                                 0x1
#define DEVICE_CONTROL_DEVICE_STATUS_PCIE_CAP_CORR_ERR_REPORT_EN_SHFT                                                 0x0

/* TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1 */
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1                                                                          ( 0x78 )
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_TRANS_PENDING_MASK                                              0x200000
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_TRANS_PENDING_SHFT                                              0x15
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_AUX_POWER_DETECTED_MASK                                         0x100000
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_AUX_POWER_DETECTED_SHFT                                         0x14
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_UNSUPPORTED_REQ_DETECTED_MASK                                   0x80000
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_UNSUPPORTED_REQ_DETECTED_SHFT                                   0x13
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_FATAL_ERR_DETECTED_MASK                                         0x40000
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_FATAL_ERR_DETECTED_SHFT                                         0x12
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_NON_FATAL_ERR_DETECTED_MASK                                     0x20000
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_NON_FATAL_ERR_DETECTED_SHFT                                     0x11
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_CORR_ERR_DETECTED_MASK                                          0x10000
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_CORR_ERR_DETECTED_SHFT                                          0x10
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_INITIATE_FLR_MASK                                               0x8000
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_INITIATE_FLR_SHFT                                               0xF
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_MAX_READ_REQ_SIZE_MASK                                          0x7000
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_MAX_READ_REQ_SIZE_SHFT                                          0xC
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_EN_NO_SNOOP_MASK                                                0x800
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_EN_NO_SNOOP_SHFT                                                0xB
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_AUX_POWER_PM_EN_MASK                                            0x400
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_AUX_POWER_PM_EN_SHFT                                            0xA
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_PHANTOM_FUNC_EN_MASK                                            0x200
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_PHANTOM_FUNC_EN_SHFT                                            0x9
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_EXT_TAG_EN_MASK                                                 0x100
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_EXT_TAG_EN_SHFT                                                 0x8
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_MAX_PAYLOAD_SIZE_CS_MASK                                        0xE0
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_MAX_PAYLOAD_SIZE_CS_SHFT                                        0x5
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_EN_REL_ORDER_MASK                                               0x10
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_EN_REL_ORDER_SHFT                                               0x4
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_UNSUPPORT_REQ_REP_EN_MASK                                       0x8
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_UNSUPPORT_REQ_REP_EN_SHFT                                       0x3
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_FATAL_ERR_REPORT_EN_MASK                                        0x4
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_FATAL_ERR_REPORT_EN_SHFT                                        0x2
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_NON_FATAL_ERR_REPORT_EN_MASK                                    0x2
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_NON_FATAL_ERR_REPORT_EN_SHFT                                    0x1
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_CORR_ERR_REPORT_EN_MASK                                         0x1
#define TYPE0_DEVICE_CONTROL_DEVICE_STATUS_1_PCIE_CAP_CORR_ERR_REPORT_EN_SHFT                                         0x0

/* LINK_CAPABILITIES_REG */
#define LINK_CAPABILITIES_REG                                                                                         ( 0x7C )
#define LINK_CAPABILITIES_REG_PCIE_CAP_PORT_NUM_MASK                                                                  0xFF000000
#define LINK_CAPABILITIES_REG_PCIE_CAP_PORT_NUM_SHFT                                                                  0x18
#define LINK_CAPABILITIES_REG_PCIE_CAP_ASPM_OPT_COMPLIANCE_MASK                                                       0x400000
#define LINK_CAPABILITIES_REG_PCIE_CAP_ASPM_OPT_COMPLIANCE_SHFT                                                       0x16
#define LINK_CAPABILITIES_REG_PCIE_CAP_LINK_BW_NOT_CAP_MASK                                                           0x200000
#define LINK_CAPABILITIES_REG_PCIE_CAP_LINK_BW_NOT_CAP_SHFT                                                           0x15
#define LINK_CAPABILITIES_REG_PCIE_CAP_DLL_ACTIVE_REP_CAP_MASK                                                        0x100000
#define LINK_CAPABILITIES_REG_PCIE_CAP_DLL_ACTIVE_REP_CAP_SHFT                                                        0x14
#define LINK_CAPABILITIES_REG_PCIE_CAP_SURPRISE_DOWN_ERR_REP_CAP_MASK                                                 0x80000
#define LINK_CAPABILITIES_REG_PCIE_CAP_SURPRISE_DOWN_ERR_REP_CAP_SHFT                                                 0x13
#define LINK_CAPABILITIES_REG_PCIE_CAP_CLOCK_POWER_MAN_MASK                                                           0x40000
#define LINK_CAPABILITIES_REG_PCIE_CAP_CLOCK_POWER_MAN_SHFT                                                           0x12
#define LINK_CAPABILITIES_REG_PCIE_CAP_L1_EXIT_LATENCY_MASK                                                           0x38000
#define LINK_CAPABILITIES_REG_PCIE_CAP_L1_EXIT_LATENCY_SHFT                                                           0xF
#define LINK_CAPABILITIES_REG_PCIE_CAP_L0S_EXIT_LATENCY_MASK                                                          0x7000
#define LINK_CAPABILITIES_REG_PCIE_CAP_L0S_EXIT_LATENCY_SHFT                                                          0xC
#define LINK_CAPABILITIES_REG_PCIE_CAP_ACTIVE_STATE_LINK_PM_SUPPORT_MASK                                              0xC00
#define LINK_CAPABILITIES_REG_PCIE_CAP_ACTIVE_STATE_LINK_PM_SUPPORT_SHFT                                              0xA
#define LINK_CAPABILITIES_REG_PCIE_CAP_MAX_LINK_WIDTH_MASK                                                            0x3F0
#define LINK_CAPABILITIES_REG_PCIE_CAP_MAX_LINK_WIDTH_SHFT                                                            0x4
#define LINK_CAPABILITIES_REG_PCIE_CAP_MAX_LINK_SPEED_MASK                                                            0xF
#define LINK_CAPABILITIES_REG_PCIE_CAP_MAX_LINK_SPEED_SHFT                                                            0x0

/* TYPE0_LINK_CAPABILITIES_REG_1 */
#define TYPE0_LINK_CAPABILITIES_REG_1                                                                                 ( 0x7C )
#define TYPE0_LINK_CAPABILITIES_REG_1_PCIE_CAP_PORT_NUM_MASK                                                          0xFF000000
#define TYPE0_LINK_CAPABILITIES_REG_1_PCIE_CAP_PORT_NUM_SHFT                                                          0x18
#define TYPE0_LINK_CAPABILITIES_REG_1_PCIE_CAP_ASPM_OPT_COMPLIANCE_MASK                                               0x400000
#define TYPE0_LINK_CAPABILITIES_REG_1_PCIE_CAP_ASPM_OPT_COMPLIANCE_SHFT                                               0x16
#define TYPE0_LINK_CAPABILITIES_REG_1_PCIE_CAP_LINK_BW_NOT_CAP_MASK                                                   0x200000
#define TYPE0_LINK_CAPABILITIES_REG_1_PCIE_CAP_LINK_BW_NOT_CAP_SHFT                                                   0x15
#define TYPE0_LINK_CAPABILITIES_REG_1_PCIE_CAP_DLL_ACTIVE_REP_CAP_MASK                                                0x100000
#define TYPE0_LINK_CAPABILITIES_REG_1_PCIE_CAP_DLL_ACTIVE_REP_CAP_SHFT                                                0x14
#define TYPE0_LINK_CAPABILITIES_REG_1_PCIE_CAP_SURPRISE_DOWN_ERR_REP_CAP_MASK                                         0x80000
#define TYPE0_LINK_CAPABILITIES_REG_1_PCIE_CAP_SURPRISE_DOWN_ERR_REP_CAP_SHFT                                         0x13
#define TYPE0_LINK_CAPABILITIES_REG_1_PCIE_CAP_CLOCK_POWER_MAN_MASK                                                   0x40000
#define TYPE0_LINK_CAPABILITIES_REG_1_PCIE_CAP_CLOCK_POWER_MAN_SHFT                                                   0x12
#define TYPE0_LINK_CAPABILITIES_REG_1_PCIE_CAP_L1_EXIT_LATENCY_MASK                                                   0x38000
#define TYPE0_LINK_CAPABILITIES_REG_1_PCIE_CAP_L1_EXIT_LATENCY_SHFT                                                   0xF
#define TYPE0_LINK_CAPABILITIES_REG_1_PCIE_CAP_L0S_EXIT_LATENCY_MASK                                                  0x7000
#define TYPE0_LINK_CAPABILITIES_REG_1_PCIE_CAP_L0S_EXIT_LATENCY_SHFT                                                  0xC
#define TYPE0_LINK_CAPABILITIES_REG_1_PCIE_CAP_ACTIVE_STATE_LINK_PM_SUPPORT_MASK                                      0xC00
#define TYPE0_LINK_CAPABILITIES_REG_1_PCIE_CAP_ACTIVE_STATE_LINK_PM_SUPPORT_SHFT                                      0xA
#define TYPE0_LINK_CAPABILITIES_REG_1_PCIE_CAP_MAX_LINK_WIDTH_MASK                                                    0x3F0
#define TYPE0_LINK_CAPABILITIES_REG_1_PCIE_CAP_MAX_LINK_WIDTH_SHFT                                                    0x4
#define TYPE0_LINK_CAPABILITIES_REG_1_PCIE_CAP_MAX_LINK_SPEED_MASK                                                    0xF
#define TYPE0_LINK_CAPABILITIES_REG_1_PCIE_CAP_MAX_LINK_SPEED_SHFT                                                    0x0

/* LINK_CONTROL_LINK_STATUS_REG */
#define LINK_CONTROL_LINK_STATUS_REG                                                                                  ( 0x80 )
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_LINK_AUOT_BW_STATUS_MASK                                                0x80000000
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_LINK_AUOT_BW_STATUS_SHFT                                                0x1F
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_LINK_BW_MAN_STATUS_MASK                                                 0x40000000
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_LINK_BW_MAN_STATUS_SHFT                                                 0x1E
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_DLL_ACTIVE_MASK                                                         0x20000000
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_DLL_ACTIVE_SHFT                                                         0x1D
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_SLOT_CLK_CONFIG_MASK                                                    0x10000000
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_SLOT_CLK_CONFIG_SHFT                                                    0x1C
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_LINK_TRAINING_MASK                                                      0x8000000
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_LINK_TRAINING_SHFT                                                      0x1B
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_NEGO_LINK_WIDTH_MASK                                                    0x1F00000
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_NEGO_LINK_WIDTH_SHFT                                                    0x14
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_LINK_SPEED_MASK                                                         0xF0000
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_LINK_SPEED_SHFT                                                         0x10
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_LINK_AUTO_BW_INT_EN_MASK                                                0x800
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_LINK_AUTO_BW_INT_EN_SHFT                                                0xB
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_LINK_BW_MAN_INT_EN_MASK                                                 0x400
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_LINK_BW_MAN_INT_EN_SHFT                                                 0xA
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_EN_CLK_POWER_MAN_MASK                                                   0x100
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_EN_CLK_POWER_MAN_SHFT                                                   0x8
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_EXTENDED_SYNCH_MASK                                                     0x80
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_EXTENDED_SYNCH_SHFT                                                     0x7
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_COMMON_CLK_CONFIG_MASK                                                  0x40
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_COMMON_CLK_CONFIG_SHFT                                                  0x6
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_RETRAIN_LINK_MASK                                                       0x20
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_RETRAIN_LINK_SHFT                                                       0x5
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_LINK_DISABLE_MASK                                                       0x10
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_LINK_DISABLE_SHFT                                                       0x4
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_RCB_MASK                                                                0x8
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_RCB_SHFT                                                                0x3
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL_MASK                                       0x3
#define LINK_CONTROL_LINK_STATUS_REG_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL_SHFT                                       0x0

/* TYPE0_LINK_CONTROL_LINK_STATUS_REG_1 */
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1                                                                          ( 0x80 )
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_LINK_AUOT_BW_STATUS_MASK                                        0x80000000
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_LINK_AUOT_BW_STATUS_SHFT                                        0x1F
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_LINK_BW_MAN_STATUS_MASK                                         0x40000000
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_LINK_BW_MAN_STATUS_SHFT                                         0x1E
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_DLL_ACTIVE_MASK                                                 0x20000000
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_DLL_ACTIVE_SHFT                                                 0x1D
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_SLOT_CLK_CONFIG_MASK                                            0x10000000
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_SLOT_CLK_CONFIG_SHFT                                            0x1C
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_LINK_TRAINING_MASK                                              0x8000000
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_LINK_TRAINING_SHFT                                              0x1B
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_NEGO_LINK_WIDTH_MASK                                            0x1F00000
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_NEGO_LINK_WIDTH_SHFT                                            0x14
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_LINK_SPEED_MASK                                                 0xF0000
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_LINK_SPEED_SHFT                                                 0x10
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_LINK_AUTO_BW_INT_EN_MASK                                        0x800
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_LINK_AUTO_BW_INT_EN_SHFT                                        0xB
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_LINK_BW_MAN_INT_EN_MASK                                         0x400
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_LINK_BW_MAN_INT_EN_SHFT                                         0xA
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_EN_CLK_POWER_MAN_MASK                                           0x100
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_EN_CLK_POWER_MAN_SHFT                                           0x8
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_EXTENDED_SYNCH_MASK                                             0x80
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_EXTENDED_SYNCH_SHFT                                             0x7
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_COMMON_CLK_CONFIG_MASK                                          0x40
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_COMMON_CLK_CONFIG_SHFT                                          0x6
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_RETRAIN_LINK_MASK                                               0x20
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_RETRAIN_LINK_SHFT                                               0x5
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_LINK_DISABLE_MASK                                               0x10
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_LINK_DISABLE_SHFT                                               0x4
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_RCB_MASK                                                        0x8
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_RCB_SHFT                                                        0x3
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL_MASK                               0x3
#define TYPE0_LINK_CONTROL_LINK_STATUS_REG_1_PCIE_CAP_ACTIVE_STATE_LINK_PM_CONTROL_SHFT                               0x0

/* ROOT_CONTROL_ROOT_CAPABILITIES_REG */
#define ROOT_CONTROL_ROOT_CAPABILITIES_REG                                                                            ( 0x8C )
#define ROOT_CONTROL_ROOT_CAPABILITIES_REG_PCIE_CAP_CRS_SW_VISIBILITY_MASK                                            0x10000
#define ROOT_CONTROL_ROOT_CAPABILITIES_REG_PCIE_CAP_CRS_SW_VISIBILITY_SHFT                                            0x10
#define ROOT_CONTROL_ROOT_CAPABILITIES_REG_PCIE_CAP_CRS_SW_VISIBILITY_EN_MASK                                         0x10
#define ROOT_CONTROL_ROOT_CAPABILITIES_REG_PCIE_CAP_CRS_SW_VISIBILITY_EN_SHFT                                         0x4
#define ROOT_CONTROL_ROOT_CAPABILITIES_REG_PCIE_CAP_PME_INT_EN_MASK                                                   0x8
#define ROOT_CONTROL_ROOT_CAPABILITIES_REG_PCIE_CAP_PME_INT_EN_SHFT                                                   0x3
#define ROOT_CONTROL_ROOT_CAPABILITIES_REG_PCIE_CAP_SYS_ERR_ON_FATAL_ERR_EN_MASK                                      0x4
#define ROOT_CONTROL_ROOT_CAPABILITIES_REG_PCIE_CAP_SYS_ERR_ON_FATAL_ERR_EN_SHFT                                      0x2
#define ROOT_CONTROL_ROOT_CAPABILITIES_REG_PCIE_CAP_SYS_ERR_ON_NON_FATAL_ERR_EN_MASK                                  0x2
#define ROOT_CONTROL_ROOT_CAPABILITIES_REG_PCIE_CAP_SYS_ERR_ON_NON_FATAL_ERR_EN_SHFT                                  0x1
#define ROOT_CONTROL_ROOT_CAPABILITIES_REG_PCIE_CAP_SYS_ERR_ON_CORR_ERR_EN_MASK                                       0x1
#define ROOT_CONTROL_ROOT_CAPABILITIES_REG_PCIE_CAP_SYS_ERR_ON_CORR_ERR_EN_SHFT                                       0x0

/* TYPE0_DUMMY_ROOT_CONTROL_ROOT_CAPABILITIES_REG */
#define TYPE0_DUMMY_ROOT_CONTROL_ROOT_CAPABILITIES_REG                                                                ( 0x8C )
#define TYPE0_DUMMY_ROOT_CONTROL_ROOT_CAPABILITIES_REG_PCIE_CAP_CRS_SW_VISIBILITY_MASK                                0x10000
#define TYPE0_DUMMY_ROOT_CONTROL_ROOT_CAPABILITIES_REG_PCIE_CAP_CRS_SW_VISIBILITY_SHFT                                0x10
#define TYPE0_DUMMY_ROOT_CONTROL_ROOT_CAPABILITIES_REG_PCIE_CAP_CRS_SW_VISIBILITY_EN_MASK                             0x10
#define TYPE0_DUMMY_ROOT_CONTROL_ROOT_CAPABILITIES_REG_PCIE_CAP_CRS_SW_VISIBILITY_EN_SHFT                             0x4
#define TYPE0_DUMMY_ROOT_CONTROL_ROOT_CAPABILITIES_REG_PCIE_CAP_PME_INT_EN_MASK                                       0x8
#define TYPE0_DUMMY_ROOT_CONTROL_ROOT_CAPABILITIES_REG_PCIE_CAP_PME_INT_EN_SHFT                                       0x3
#define TYPE0_DUMMY_ROOT_CONTROL_ROOT_CAPABILITIES_REG_PCIE_CAP_SYS_ERR_ON_FATAL_ERR_EN_MASK                          0x4
#define TYPE0_DUMMY_ROOT_CONTROL_ROOT_CAPABILITIES_REG_PCIE_CAP_SYS_ERR_ON_FATAL_ERR_EN_SHFT                          0x2
#define TYPE0_DUMMY_ROOT_CONTROL_ROOT_CAPABILITIES_REG_PCIE_CAP_SYS_ERR_ON_NON_FATAL_ERR_EN_MASK                      0x2
#define TYPE0_DUMMY_ROOT_CONTROL_ROOT_CAPABILITIES_REG_PCIE_CAP_SYS_ERR_ON_NON_FATAL_ERR_EN_SHFT                      0x1
#define TYPE0_DUMMY_ROOT_CONTROL_ROOT_CAPABILITIES_REG_PCIE_CAP_SYS_ERR_ON_CORR_ERR_EN_MASK                           0x1
#define TYPE0_DUMMY_ROOT_CONTROL_ROOT_CAPABILITIES_REG_PCIE_CAP_SYS_ERR_ON_CORR_ERR_EN_SHFT                           0x0

/* ROOT_STATUS_REG */
#define ROOT_STATUS_REG                                                                                               ( 0x90 )
#define ROOT_STATUS_REG_PCIE_CAP_PME_PENDING_MASK                                                                     0x20000
#define ROOT_STATUS_REG_PCIE_CAP_PME_PENDING_SHFT                                                                     0x11
#define ROOT_STATUS_REG_PCIE_CAP_PME_STATUS_MASK                                                                      0x10000
#define ROOT_STATUS_REG_PCIE_CAP_PME_STATUS_SHFT                                                                      0x10
#define ROOT_STATUS_REG_PCIE_CAP_PME_REQ_ID_MASK                                                                      0xFFFF
#define ROOT_STATUS_REG_PCIE_CAP_PME_REQ_ID_SHFT                                                                      0x0

/* TYPE0_DUMMY_ROOT_STATUS_REG */
#define TYPE0_DUMMY_ROOT_STATUS_REG                                                                                   ( 0x90 )
#define TYPE0_DUMMY_ROOT_STATUS_REG_PCIE_CAP_PME_PENDING_MASK                                                         0x20000
#define TYPE0_DUMMY_ROOT_STATUS_REG_PCIE_CAP_PME_PENDING_SHFT                                                         0x11
#define TYPE0_DUMMY_ROOT_STATUS_REG_PCIE_CAP_PME_STATUS_MASK                                                          0x10000
#define TYPE0_DUMMY_ROOT_STATUS_REG_PCIE_CAP_PME_STATUS_SHFT                                                          0x10
#define TYPE0_DUMMY_ROOT_STATUS_REG_PCIE_CAP_PME_REQ_ID_MASK                                                          0xFFFF
#define TYPE0_DUMMY_ROOT_STATUS_REG_PCIE_CAP_PME_REQ_ID_SHFT                                                          0x0

/* DEVICE_CAPABILITIES2_REG */
#define DEVICE_CAPABILITIES2_REG                                                                                      ( 0x94 )
#define DEVICE_CAPABILITIES2_REG_PCIE_CAP_OBFF_SUPPORT_MASK                                                           0xC0000
#define DEVICE_CAPABILITIES2_REG_PCIE_CAP_OBFF_SUPPORT_SHFT                                                           0x12
#define DEVICE_CAPABILITIES2_REG_PCIE_CAP_TPH_CMPLT_SUPPORT_1_MASK                                                    0x2000
#define DEVICE_CAPABILITIES2_REG_PCIE_CAP_TPH_CMPLT_SUPPORT_1_SHFT                                                    0xD
#define DEVICE_CAPABILITIES2_REG_PCIE_CAP_TPH_CMPLT_SUPPORT_0_MASK                                                    0x1000
#define DEVICE_CAPABILITIES2_REG_PCIE_CAP_TPH_CMPLT_SUPPORT_0_SHFT                                                    0xC
#define DEVICE_CAPABILITIES2_REG_PCIE_CAP_LTR_SUPP_MASK                                                               0x800
#define DEVICE_CAPABILITIES2_REG_PCIE_CAP_LTR_SUPP_SHFT                                                               0xB
#define DEVICE_CAPABILITIES2_REG_PCIE_CAP_NO_RO_EN_PR2PR_PAR_MASK                                                     0x400
#define DEVICE_CAPABILITIES2_REG_PCIE_CAP_NO_RO_EN_PR2PR_PAR_SHFT                                                     0xA
#define DEVICE_CAPABILITIES2_REG_PCIE_CAP_128_CAS_CPL_SUPP_MASK                                                       0x200
#define DEVICE_CAPABILITIES2_REG_PCIE_CAP_128_CAS_CPL_SUPP_SHFT                                                       0x9
#define DEVICE_CAPABILITIES2_REG_PCIE_CAP_64_ATOMIC_CPL_SUPP_MASK                                                     0x100
#define DEVICE_CAPABILITIES2_REG_PCIE_CAP_64_ATOMIC_CPL_SUPP_SHFT                                                     0x8
#define DEVICE_CAPABILITIES2_REG_PCIE_CAP_32_ATOMIC_CPL_SUPP_MASK                                                     0x80
#define DEVICE_CAPABILITIES2_REG_PCIE_CAP_32_ATOMIC_CPL_SUPP_SHFT                                                     0x7
#define DEVICE_CAPABILITIES2_REG_PCIE_CAP_ATOMIC_ROUTING_SUPP_MASK                                                    0x40
#define DEVICE_CAPABILITIES2_REG_PCIE_CAP_ATOMIC_ROUTING_SUPP_SHFT                                                    0x6
#define DEVICE_CAPABILITIES2_REG_PCIE_CAP_ARI_FORWARD_SUPPORT_MASK                                                    0x20
#define DEVICE_CAPABILITIES2_REG_PCIE_CAP_ARI_FORWARD_SUPPORT_SHFT                                                    0x5
#define DEVICE_CAPABILITIES2_REG_PCIE_CAP_CPL_TIMEOUT_DISABLE_SUPPORT_MASK                                            0x10
#define DEVICE_CAPABILITIES2_REG_PCIE_CAP_CPL_TIMEOUT_DISABLE_SUPPORT_SHFT                                            0x4
#define DEVICE_CAPABILITIES2_REG_PCIE_CAP_CPL_TIMEOUT_RANGE_MASK                                                      0xF
#define DEVICE_CAPABILITIES2_REG_PCIE_CAP_CPL_TIMEOUT_RANGE_SHFT                                                      0x0

/* TYPE0_DEVICE_CAPABILITIES2_REG_1 */
#define TYPE0_DEVICE_CAPABILITIES2_REG_1                                                                              ( 0x94 )
#define TYPE0_DEVICE_CAPABILITIES2_REG_1_PCIE_CAP_OBFF_SUPPORT_MASK                                                   0xC0000
#define TYPE0_DEVICE_CAPABILITIES2_REG_1_PCIE_CAP_OBFF_SUPPORT_SHFT                                                   0x12
#define TYPE0_DEVICE_CAPABILITIES2_REG_1_PCIE_CAP_TPH_CMPLT_SUPPORT_1_MASK                                            0x2000
#define TYPE0_DEVICE_CAPABILITIES2_REG_1_PCIE_CAP_TPH_CMPLT_SUPPORT_1_SHFT                                            0xD
#define TYPE0_DEVICE_CAPABILITIES2_REG_1_PCIE_CAP_TPH_CMPLT_SUPPORT_0_MASK                                            0x1000
#define TYPE0_DEVICE_CAPABILITIES2_REG_1_PCIE_CAP_TPH_CMPLT_SUPPORT_0_SHFT                                            0xC
#define TYPE0_DEVICE_CAPABILITIES2_REG_1_PCIE_CAP_LTR_SUPP_MASK                                                       0x800
#define TYPE0_DEVICE_CAPABILITIES2_REG_1_PCIE_CAP_LTR_SUPP_SHFT                                                       0xB
#define TYPE0_DEVICE_CAPABILITIES2_REG_1_PCIE_CAP_NO_RO_EN_PR2PR_PAR_MASK                                             0x400
#define TYPE0_DEVICE_CAPABILITIES2_REG_1_PCIE_CAP_NO_RO_EN_PR2PR_PAR_SHFT                                             0xA
#define TYPE0_DEVICE_CAPABILITIES2_REG_1_PCIE_CAP_128_CAS_CPL_SUPP_MASK                                               0x200
#define TYPE0_DEVICE_CAPABILITIES2_REG_1_PCIE_CAP_128_CAS_CPL_SUPP_SHFT                                               0x9
#define TYPE0_DEVICE_CAPABILITIES2_REG_1_PCIE_CAP_64_ATOMIC_CPL_SUPP_MASK                                             0x100
#define TYPE0_DEVICE_CAPABILITIES2_REG_1_PCIE_CAP_64_ATOMIC_CPL_SUPP_SHFT                                             0x8
#define TYPE0_DEVICE_CAPABILITIES2_REG_1_PCIE_CAP_32_ATOMIC_CPL_SUPP_MASK                                             0x80
#define TYPE0_DEVICE_CAPABILITIES2_REG_1_PCIE_CAP_32_ATOMIC_CPL_SUPP_SHFT                                             0x7
#define TYPE0_DEVICE_CAPABILITIES2_REG_1_PCIE_CAP_ATOMIC_ROUTING_SUPP_MASK                                            0x40
#define TYPE0_DEVICE_CAPABILITIES2_REG_1_PCIE_CAP_ATOMIC_ROUTING_SUPP_SHFT                                            0x6
#define TYPE0_DEVICE_CAPABILITIES2_REG_1_PCIE_CAP_ARI_FORWARD_SUPPORT_MASK                                            0x20
#define TYPE0_DEVICE_CAPABILITIES2_REG_1_PCIE_CAP_ARI_FORWARD_SUPPORT_SHFT                                            0x5
#define TYPE0_DEVICE_CAPABILITIES2_REG_1_PCIE_CAP_CPL_TIMEOUT_DISABLE_SUPPORT_MASK                                    0x10
#define TYPE0_DEVICE_CAPABILITIES2_REG_1_PCIE_CAP_CPL_TIMEOUT_DISABLE_SUPPORT_SHFT                                    0x4
#define TYPE0_DEVICE_CAPABILITIES2_REG_1_PCIE_CAP_CPL_TIMEOUT_RANGE_MASK                                              0xF
#define TYPE0_DEVICE_CAPABILITIES2_REG_1_PCIE_CAP_CPL_TIMEOUT_RANGE_SHFT                                              0x0

/* DEVICE_CONTROL2_DEVICE_STATUS2_REG */
#define DEVICE_CONTROL2_DEVICE_STATUS2_REG                                                                            ( 0x98 )
#define DEVICE_CONTROL2_DEVICE_STATUS2_REG_PCIE_CAP_LTR_EN_MASK                                                       0x400
#define DEVICE_CONTROL2_DEVICE_STATUS2_REG_PCIE_CAP_LTR_EN_SHFT                                                       0xA
#define DEVICE_CONTROL2_DEVICE_STATUS2_REG_PCIE_CAP_ARI_FORWARD_SUPPORT_CS_MASK                                       0x20
#define DEVICE_CONTROL2_DEVICE_STATUS2_REG_PCIE_CAP_ARI_FORWARD_SUPPORT_CS_SHFT                                       0x5
#define DEVICE_CONTROL2_DEVICE_STATUS2_REG_PCIE_CAP_CPL_TIMEOUT_DISABLE_MASK                                          0x10
#define DEVICE_CONTROL2_DEVICE_STATUS2_REG_PCIE_CAP_CPL_TIMEOUT_DISABLE_SHFT                                          0x4
#define DEVICE_CONTROL2_DEVICE_STATUS2_REG_PCIE_CAP_CPL_TIMEOUT_VALUE_MASK                                            0xF
#define DEVICE_CONTROL2_DEVICE_STATUS2_REG_PCIE_CAP_CPL_TIMEOUT_VALUE_SHFT                                            0x0

/* TYPE0_DEVICE_CONTROL2_DEVICE_STATUS2_REG_1 */
#define TYPE0_DEVICE_CONTROL2_DEVICE_STATUS2_REG_1                                                                    ( 0x98 )
#define TYPE0_DEVICE_CONTROL2_DEVICE_STATUS2_REG_1_PCIE_CAP_LTR_EN_MASK                                               0x400
#define TYPE0_DEVICE_CONTROL2_DEVICE_STATUS2_REG_1_PCIE_CAP_LTR_EN_SHFT                                               0xA
#define TYPE0_DEVICE_CONTROL2_DEVICE_STATUS2_REG_1_PCIE_CAP_ARI_FORWARD_SUPPORT_CS_MASK                               0x20
#define TYPE0_DEVICE_CONTROL2_DEVICE_STATUS2_REG_1_PCIE_CAP_ARI_FORWARD_SUPPORT_CS_SHFT                               0x5
#define TYPE0_DEVICE_CONTROL2_DEVICE_STATUS2_REG_1_PCIE_CAP_CPL_TIMEOUT_DISABLE_MASK                                  0x10
#define TYPE0_DEVICE_CONTROL2_DEVICE_STATUS2_REG_1_PCIE_CAP_CPL_TIMEOUT_DISABLE_SHFT                                  0x4
#define TYPE0_DEVICE_CONTROL2_DEVICE_STATUS2_REG_1_PCIE_CAP_CPL_TIMEOUT_VALUE_MASK                                    0xF
#define TYPE0_DEVICE_CONTROL2_DEVICE_STATUS2_REG_1_PCIE_CAP_CPL_TIMEOUT_VALUE_SHFT                                    0x0

/* LINK_CAPABILITIES2_REG */
#define LINK_CAPABILITIES2_REG                                                                                        ( 0x9C )
#define LINK_CAPABILITIES2_REG_PCIE_CAP_CROSS_LINK_SUPPORT_MASK                                                       0x100
#define LINK_CAPABILITIES2_REG_PCIE_CAP_CROSS_LINK_SUPPORT_SHFT                                                       0x8
#define LINK_CAPABILITIES2_REG_PCIE_CAP_SUPPORT_LINK_SPEED_VECTOR_MASK                                                0xFE
#define LINK_CAPABILITIES2_REG_PCIE_CAP_SUPPORT_LINK_SPEED_VECTOR_SHFT                                                0x1

/* TYPE0_LINK_CAPABILITIES2_REG_1 */
#define TYPE0_LINK_CAPABILITIES2_REG_1                                                                                ( 0x9C )
#define TYPE0_LINK_CAPABILITIES2_REG_1_PCIE_CAP_CROSS_LINK_SUPPORT_MASK                                               0x100
#define TYPE0_LINK_CAPABILITIES2_REG_1_PCIE_CAP_CROSS_LINK_SUPPORT_SHFT                                               0x8
#define TYPE0_LINK_CAPABILITIES2_REG_1_PCIE_CAP_SUPPORT_LINK_SPEED_VECTOR_MASK                                        0xFE
#define TYPE0_LINK_CAPABILITIES2_REG_1_PCIE_CAP_SUPPORT_LINK_SPEED_VECTOR_SHFT                                        0x1

/* LINK_CONTROL2_LINK_STATUS2_REG */
#define LINK_CONTROL2_LINK_STATUS2_REG                                                                                ( 0xA0 )
#define LINK_CONTROL2_LINK_STATUS2_REG_PCIE_CAP_LINK_EQ_REQ_MASK                                                      0x200000
#define LINK_CONTROL2_LINK_STATUS2_REG_PCIE_CAP_LINK_EQ_REQ_SHFT                                                      0x15
#define LINK_CONTROL2_LINK_STATUS2_REG_PCIE_CAP_EQ_CPL_P3_MASK                                                        0x100000
#define LINK_CONTROL2_LINK_STATUS2_REG_PCIE_CAP_EQ_CPL_P3_SHFT                                                        0x14
#define LINK_CONTROL2_LINK_STATUS2_REG_PCIE_CAP_EQ_CPL_P2_MASK                                                        0x80000
#define LINK_CONTROL2_LINK_STATUS2_REG_PCIE_CAP_EQ_CPL_P2_SHFT                                                        0x13
#define LINK_CONTROL2_LINK_STATUS2_REG_PCIE_CAP_EQ_CPL_P1_MASK                                                        0x40000
#define LINK_CONTROL2_LINK_STATUS2_REG_PCIE_CAP_EQ_CPL_P1_SHFT                                                        0x12
#define LINK_CONTROL2_LINK_STATUS2_REG_PCIE_CAP_EQ_CPL_MASK                                                           0x20000
#define LINK_CONTROL2_LINK_STATUS2_REG_PCIE_CAP_EQ_CPL_SHFT                                                           0x11
#define LINK_CONTROL2_LINK_STATUS2_REG_PCIE_CAP_CURR_DEEMPHASIS_MASK                                                  0x10000
#define LINK_CONTROL2_LINK_STATUS2_REG_PCIE_CAP_CURR_DEEMPHASIS_SHFT                                                  0x10
#define LINK_CONTROL2_LINK_STATUS2_REG_PCIE_CAP_COMPLIANCE_PRESET_MASK                                                0xF000
#define LINK_CONTROL2_LINK_STATUS2_REG_PCIE_CAP_COMPLIANCE_PRESET_SHFT                                                0xC
#define LINK_CONTROL2_LINK_STATUS2_REG_PCIE_CAP_COMPLIANCE_SOS_MASK                                                   0x800
#define LINK_CONTROL2_LINK_STATUS2_REG_PCIE_CAP_COMPLIANCE_SOS_SHFT                                                   0xB
#define LINK_CONTROL2_LINK_STATUS2_REG_PCIE_CAP_ENTER_MODIFIED_COMPLIANCE_MASK                                        0x400
#define LINK_CONTROL2_LINK_STATUS2_REG_PCIE_CAP_ENTER_MODIFIED_COMPLIANCE_SHFT                                        0xA
#define LINK_CONTROL2_LINK_STATUS2_REG_PCIE_CAP_TX_MARGIN_MASK                                                        0x380
#define LINK_CONTROL2_LINK_STATUS2_REG_PCIE_CAP_TX_MARGIN_SHFT                                                        0x7
#define LINK_CONTROL2_LINK_STATUS2_REG_PCIE_CAP_SEL_DEEMPHASIS_MASK                                                   0x40
#define LINK_CONTROL2_LINK_STATUS2_REG_PCIE_CAP_SEL_DEEMPHASIS_SHFT                                                   0x6
#define LINK_CONTROL2_LINK_STATUS2_REG_PCIE_CAP_HW_AUTO_SPEED_DISABLE_MASK                                            0x20
#define LINK_CONTROL2_LINK_STATUS2_REG_PCIE_CAP_HW_AUTO_SPEED_DISABLE_SHFT                                            0x5
#define LINK_CONTROL2_LINK_STATUS2_REG_PCIE_CAP_ENTER_COMPLIANCE_MASK                                                 0x10
#define LINK_CONTROL2_LINK_STATUS2_REG_PCIE_CAP_ENTER_COMPLIANCE_SHFT                                                 0x4
#define LINK_CONTROL2_LINK_STATUS2_REG_PCIE_CAP_TARGET_LINK_SPEED_MASK                                                0xF
#define LINK_CONTROL2_LINK_STATUS2_REG_PCIE_CAP_TARGET_LINK_SPEED_SHFT                                                0x0

/* TYPE0_LINK_CONTROL2_LINK_STATUS2_REG_1 */
#define TYPE0_LINK_CONTROL2_LINK_STATUS2_REG_1                                                                        ( 0xA0 )
#define TYPE0_LINK_CONTROL2_LINK_STATUS2_REG_1_PCIE_CAP_LINK_EQ_REQ_MASK                                              0x200000
#define TYPE0_LINK_CONTROL2_LINK_STATUS2_REG_1_PCIE_CAP_LINK_EQ_REQ_SHFT                                              0x15
#define TYPE0_LINK_CONTROL2_LINK_STATUS2_REG_1_PCIE_CAP_EQ_CPL_P3_MASK                                                0x100000
#define TYPE0_LINK_CONTROL2_LINK_STATUS2_REG_1_PCIE_CAP_EQ_CPL_P3_SHFT                                                0x14
#define TYPE0_LINK_CONTROL2_LINK_STATUS2_REG_1_PCIE_CAP_EQ_CPL_P2_MASK                                                0x80000
#define TYPE0_LINK_CONTROL2_LINK_STATUS2_REG_1_PCIE_CAP_EQ_CPL_P2_SHFT                                                0x13
#define TYPE0_LINK_CONTROL2_LINK_STATUS2_REG_1_PCIE_CAP_EQ_CPL_P1_MASK                                                0x40000
#define TYPE0_LINK_CONTROL2_LINK_STATUS2_REG_1_PCIE_CAP_EQ_CPL_P1_SHFT                                                0x12
#define TYPE0_LINK_CONTROL2_LINK_STATUS2_REG_1_PCIE_CAP_EQ_CPL_MASK                                                   0x20000
#define TYPE0_LINK_CONTROL2_LINK_STATUS2_REG_1_PCIE_CAP_EQ_CPL_SHFT                                                   0x11
#define TYPE0_LINK_CONTROL2_LINK_STATUS2_REG_1_PCIE_CAP_CURR_DEEMPHASIS_MASK                                          0x10000
#define TYPE0_LINK_CONTROL2_LINK_STATUS2_REG_1_PCIE_CAP_CURR_DEEMPHASIS_SHFT                                          0x10
#define TYPE0_LINK_CONTROL2_LINK_STATUS2_REG_1_PCIE_CAP_COMPLIANCE_PRESET_MASK                                        0xF000
#define TYPE0_LINK_CONTROL2_LINK_STATUS2_REG_1_PCIE_CAP_COMPLIANCE_PRESET_SHFT                                        0xC
#define TYPE0_LINK_CONTROL2_LINK_STATUS2_REG_1_PCIE_CAP_COMPLIANCE_SOS_MASK                                           0x800
#define TYPE0_LINK_CONTROL2_LINK_STATUS2_REG_1_PCIE_CAP_COMPLIANCE_SOS_SHFT                                           0xB
#define TYPE0_LINK_CONTROL2_LINK_STATUS2_REG_1_PCIE_CAP_ENTER_MODIFIED_COMPLIANCE_MASK                                0x400
#define TYPE0_LINK_CONTROL2_LINK_STATUS2_REG_1_PCIE_CAP_ENTER_MODIFIED_COMPLIANCE_SHFT                                0xA
#define TYPE0_LINK_CONTROL2_LINK_STATUS2_REG_1_PCIE_CAP_TX_MARGIN_MASK                                                0x380
#define TYPE0_LINK_CONTROL2_LINK_STATUS2_REG_1_PCIE_CAP_TX_MARGIN_SHFT                                                0x7
#define TYPE0_LINK_CONTROL2_LINK_STATUS2_REG_1_PCIE_CAP_SEL_DEEMPHASIS_MASK                                           0x40
#define TYPE0_LINK_CONTROL2_LINK_STATUS2_REG_1_PCIE_CAP_SEL_DEEMPHASIS_SHFT                                           0x6
#define TYPE0_LINK_CONTROL2_LINK_STATUS2_REG_1_PCIE_CAP_HW_AUTO_SPEED_DISABLE_MASK                                    0x20
#define TYPE0_LINK_CONTROL2_LINK_STATUS2_REG_1_PCIE_CAP_HW_AUTO_SPEED_DISABLE_SHFT                                    0x5
#define TYPE0_LINK_CONTROL2_LINK_STATUS2_REG_1_PCIE_CAP_ENTER_COMPLIANCE_MASK                                         0x10
#define TYPE0_LINK_CONTROL2_LINK_STATUS2_REG_1_PCIE_CAP_ENTER_COMPLIANCE_SHFT                                         0x4
#define TYPE0_LINK_CONTROL2_LINK_STATUS2_REG_1_PCIE_CAP_TARGET_LINK_SPEED_MASK                                        0xF
#define TYPE0_LINK_CONTROL2_LINK_STATUS2_REG_1_PCIE_CAP_TARGET_LINK_SPEED_SHFT                                        0x0

/* AER_EXT_CAP_HDR_REG */
#define AER_EXT_CAP_HDR_REG                                                                                           ( 0x100 )
#define AER_EXT_CAP_HDR_REG_NEXT_OFFSET_MASK                                                                          0xFFF00000
#define AER_EXT_CAP_HDR_REG_NEXT_OFFSET_SHFT                                                                          0x14
#define AER_EXT_CAP_HDR_REG_CAP_VERSION_MASK                                                                          0xF0000
#define AER_EXT_CAP_HDR_REG_CAP_VERSION_SHFT                                                                          0x10
#define AER_EXT_CAP_HDR_REG_CAP_ID_MASK                                                                               0xFFFF
#define AER_EXT_CAP_HDR_REG_CAP_ID_SHFT                                                                               0x0

/* TYPE0_AER_EXT_CAP_HDR_REG_1 */
#define TYPE0_AER_EXT_CAP_HDR_REG_1                                                                                   ( 0x100 )
#define TYPE0_AER_EXT_CAP_HDR_REG_1_NEXT_OFFSET_MASK                                                                  0xFFF00000
#define TYPE0_AER_EXT_CAP_HDR_REG_1_NEXT_OFFSET_SHFT                                                                  0x14
#define TYPE0_AER_EXT_CAP_HDR_REG_1_CAP_VERSION_MASK                                                                  0xF0000
#define TYPE0_AER_EXT_CAP_HDR_REG_1_CAP_VERSION_SHFT                                                                  0x10
#define TYPE0_AER_EXT_CAP_HDR_REG_1_CAP_ID_MASK                                                                       0xFFFF
#define TYPE0_AER_EXT_CAP_HDR_REG_1_CAP_ID_SHFT                                                                       0x0

/* UNCORR_ERR_STATUS_REG */
#define UNCORR_ERR_STATUS_REG                                                                                         ( 0x104 )
#define UNCORR_ERR_STATUS_REG_ATOMIC_EGRESS_BLOCKED_ERR_STATUS_MASK                                                   0x1000000
#define UNCORR_ERR_STATUS_REG_ATOMIC_EGRESS_BLOCKED_ERR_STATUS_SHFT                                                   0x18
#define UNCORR_ERR_STATUS_REG_INTERNAL_ERR_STATUS_MASK                                                                0x400000
#define UNCORR_ERR_STATUS_REG_INTERNAL_ERR_STATUS_SHFT                                                                0x16
#define UNCORR_ERR_STATUS_REG_UNSUPPORTED_REQ_ERR_STATUS_MASK                                                         0x100000
#define UNCORR_ERR_STATUS_REG_UNSUPPORTED_REQ_ERR_STATUS_SHFT                                                         0x14
#define UNCORR_ERR_STATUS_REG_ECRC_ERR_STATUS_MASK                                                                    0x80000
#define UNCORR_ERR_STATUS_REG_ECRC_ERR_STATUS_SHFT                                                                    0x13
#define UNCORR_ERR_STATUS_REG_MALF_TLP_ERR_STATUS_MASK                                                                0x40000
#define UNCORR_ERR_STATUS_REG_MALF_TLP_ERR_STATUS_SHFT                                                                0x12
#define UNCORR_ERR_STATUS_REG_REC_OVERFLOW_ERR_STATUS_MASK                                                            0x20000
#define UNCORR_ERR_STATUS_REG_REC_OVERFLOW_ERR_STATUS_SHFT                                                            0x11
#define UNCORR_ERR_STATUS_REG_UNEXP_CMPLT_ERR_STATUS_MASK                                                             0x10000
#define UNCORR_ERR_STATUS_REG_UNEXP_CMPLT_ERR_STATUS_SHFT                                                             0x10
#define UNCORR_ERR_STATUS_REG_CMPLT_ABORT_ERR_STATUS_MASK                                                             0x8000
#define UNCORR_ERR_STATUS_REG_CMPLT_ABORT_ERR_STATUS_SHFT                                                             0xF
#define UNCORR_ERR_STATUS_REG_CMPLT_TIMEOUT_ERR_STATUS_MASK                                                           0x4000
#define UNCORR_ERR_STATUS_REG_CMPLT_TIMEOUT_ERR_STATUS_SHFT                                                           0xE
#define UNCORR_ERR_STATUS_REG_FC_PROTOCOL_ERR_STATUS_MASK                                                             0x2000
#define UNCORR_ERR_STATUS_REG_FC_PROTOCOL_ERR_STATUS_SHFT                                                             0xD
#define UNCORR_ERR_STATUS_REG_POIS_TLP_ERR_STATUS_MASK                                                                0x1000
#define UNCORR_ERR_STATUS_REG_POIS_TLP_ERR_STATUS_SHFT                                                                0xC
#define UNCORR_ERR_STATUS_REG_SUR_DWN_ERR_STATUS_MASK                                                                 0x20
#define UNCORR_ERR_STATUS_REG_SUR_DWN_ERR_STATUS_SHFT                                                                 0x5
#define UNCORR_ERR_STATUS_REG_DL_PROTOCOL_ERR_STATUS_MASK                                                             0x10
#define UNCORR_ERR_STATUS_REG_DL_PROTOCOL_ERR_STATUS_SHFT                                                             0x4

/* TYPE0_UNCORR_ERR_STATUS_REG_1 */
#define TYPE0_UNCORR_ERR_STATUS_REG_1                                                                                 ( 0x104 )
#define TYPE0_UNCORR_ERR_STATUS_REG_1_ATOMIC_EGRESS_BLOCKED_ERR_STATUS_MASK                                           0x1000000
#define TYPE0_UNCORR_ERR_STATUS_REG_1_ATOMIC_EGRESS_BLOCKED_ERR_STATUS_SHFT                                           0x18
#define TYPE0_UNCORR_ERR_STATUS_REG_1_INTERNAL_ERR_STATUS_MASK                                                        0x400000
#define TYPE0_UNCORR_ERR_STATUS_REG_1_INTERNAL_ERR_STATUS_SHFT                                                        0x16
#define TYPE0_UNCORR_ERR_STATUS_REG_1_UNSUPPORTED_REQ_ERR_STATUS_MASK                                                 0x100000
#define TYPE0_UNCORR_ERR_STATUS_REG_1_UNSUPPORTED_REQ_ERR_STATUS_SHFT                                                 0x14
#define TYPE0_UNCORR_ERR_STATUS_REG_1_ECRC_ERR_STATUS_MASK                                                            0x80000
#define TYPE0_UNCORR_ERR_STATUS_REG_1_ECRC_ERR_STATUS_SHFT                                                            0x13
#define TYPE0_UNCORR_ERR_STATUS_REG_1_MALF_TLP_ERR_STATUS_MASK                                                        0x40000
#define TYPE0_UNCORR_ERR_STATUS_REG_1_MALF_TLP_ERR_STATUS_SHFT                                                        0x12
#define TYPE0_UNCORR_ERR_STATUS_REG_1_REC_OVERFLOW_ERR_STATUS_MASK                                                    0x20000
#define TYPE0_UNCORR_ERR_STATUS_REG_1_REC_OVERFLOW_ERR_STATUS_SHFT                                                    0x11
#define TYPE0_UNCORR_ERR_STATUS_REG_1_UNEXP_CMPLT_ERR_STATUS_MASK                                                     0x10000
#define TYPE0_UNCORR_ERR_STATUS_REG_1_UNEXP_CMPLT_ERR_STATUS_SHFT                                                     0x10
#define TYPE0_UNCORR_ERR_STATUS_REG_1_CMPLT_ABORT_ERR_STATUS_MASK                                                     0x8000
#define TYPE0_UNCORR_ERR_STATUS_REG_1_CMPLT_ABORT_ERR_STATUS_SHFT                                                     0xF
#define TYPE0_UNCORR_ERR_STATUS_REG_1_CMPLT_TIMEOUT_ERR_STATUS_MASK                                                   0x4000
#define TYPE0_UNCORR_ERR_STATUS_REG_1_CMPLT_TIMEOUT_ERR_STATUS_SHFT                                                   0xE
#define TYPE0_UNCORR_ERR_STATUS_REG_1_FC_PROTOCOL_ERR_STATUS_MASK                                                     0x2000
#define TYPE0_UNCORR_ERR_STATUS_REG_1_FC_PROTOCOL_ERR_STATUS_SHFT                                                     0xD
#define TYPE0_UNCORR_ERR_STATUS_REG_1_POIS_TLP_ERR_STATUS_MASK                                                        0x1000
#define TYPE0_UNCORR_ERR_STATUS_REG_1_POIS_TLP_ERR_STATUS_SHFT                                                        0xC
#define TYPE0_UNCORR_ERR_STATUS_REG_1_SUR_DWN_ERR_STATUS_MASK                                                         0x20
#define TYPE0_UNCORR_ERR_STATUS_REG_1_SUR_DWN_ERR_STATUS_SHFT                                                         0x5
#define TYPE0_UNCORR_ERR_STATUS_REG_1_DL_PROTOCOL_ERR_STATUS_MASK                                                     0x10
#define TYPE0_UNCORR_ERR_STATUS_REG_1_DL_PROTOCOL_ERR_STATUS_SHFT                                                     0x4

/* UNCORR_ERR_MASK_REG */
#define UNCORR_ERR_MASK_REG                                                                                           ( 0x108 )
#define UNCORR_ERR_MASK_REG_ATOMIC_EGRESS_BLOCKED_ERR_MASK_MASK                                                       0x1000000
#define UNCORR_ERR_MASK_REG_ATOMIC_EGRESS_BLOCKED_ERR_MASK_SHFT                                                       0x18
#define UNCORR_ERR_MASK_REG_INTERNAL_ERR_MASK_MASK                                                                    0x400000
#define UNCORR_ERR_MASK_REG_INTERNAL_ERR_MASK_SHFT                                                                    0x16
#define UNCORR_ERR_MASK_REG_UNSUPPORTED_REQ_ERR_MASK_MASK                                                             0x100000
#define UNCORR_ERR_MASK_REG_UNSUPPORTED_REQ_ERR_MASK_SHFT                                                             0x14
#define UNCORR_ERR_MASK_REG_ECRC_ERR_MASK_MASK                                                                        0x80000
#define UNCORR_ERR_MASK_REG_ECRC_ERR_MASK_SHFT                                                                        0x13
#define UNCORR_ERR_MASK_REG_MALF_TLP_ERR_MASK_MASK                                                                    0x40000
#define UNCORR_ERR_MASK_REG_MALF_TLP_ERR_MASK_SHFT                                                                    0x12
#define UNCORR_ERR_MASK_REG_REC_OVERFLOW_ERR_MASK_MASK                                                                0x20000
#define UNCORR_ERR_MASK_REG_REC_OVERFLOW_ERR_MASK_SHFT                                                                0x11
#define UNCORR_ERR_MASK_REG_UNEXP_CMPLT_ERR_MASK_MASK                                                                 0x10000
#define UNCORR_ERR_MASK_REG_UNEXP_CMPLT_ERR_MASK_SHFT                                                                 0x10
#define UNCORR_ERR_MASK_REG_CMPLT_ABORT_ERR_MASK_MASK                                                                 0x8000
#define UNCORR_ERR_MASK_REG_CMPLT_ABORT_ERR_MASK_SHFT                                                                 0xF
#define UNCORR_ERR_MASK_REG_CMPLT_TIMEOUT_ERR_MASK_MASK                                                               0x4000
#define UNCORR_ERR_MASK_REG_CMPLT_TIMEOUT_ERR_MASK_SHFT                                                               0xE
#define UNCORR_ERR_MASK_REG_FC_PROTOCOL_ERR_MASK_MASK                                                                 0x2000
#define UNCORR_ERR_MASK_REG_FC_PROTOCOL_ERR_MASK_SHFT                                                                 0xD
#define UNCORR_ERR_MASK_REG_POIS_TLP_ERR_MASK_MASK                                                                    0x1000
#define UNCORR_ERR_MASK_REG_POIS_TLP_ERR_MASK_SHFT                                                                    0xC
#define UNCORR_ERR_MASK_REG_SUR_DWN_ERR_MASK_MASK                                                                     0x20
#define UNCORR_ERR_MASK_REG_SUR_DWN_ERR_MASK_SHFT                                                                     0x5
#define UNCORR_ERR_MASK_REG_DL_PROTOCOL_ERR_MASK_MASK                                                                 0x10
#define UNCORR_ERR_MASK_REG_DL_PROTOCOL_ERR_MASK_SHFT                                                                 0x4

/* TYPE0_UNCORR_ERR_MASK_REG_1 */
#define TYPE0_UNCORR_ERR_MASK_REG_1                                                                                   ( 0x108 )
#define TYPE0_UNCORR_ERR_MASK_REG_1_ATOMIC_EGRESS_BLOCKED_ERR_MASK_MASK                                               0x1000000
#define TYPE0_UNCORR_ERR_MASK_REG_1_ATOMIC_EGRESS_BLOCKED_ERR_MASK_SHFT                                               0x18
#define TYPE0_UNCORR_ERR_MASK_REG_1_INTERNAL_ERR_MASK_MASK                                                            0x400000
#define TYPE0_UNCORR_ERR_MASK_REG_1_INTERNAL_ERR_MASK_SHFT                                                            0x16
#define TYPE0_UNCORR_ERR_MASK_REG_1_UNSUPPORTED_REQ_ERR_MASK_MASK                                                     0x100000
#define TYPE0_UNCORR_ERR_MASK_REG_1_UNSUPPORTED_REQ_ERR_MASK_SHFT                                                     0x14
#define TYPE0_UNCORR_ERR_MASK_REG_1_ECRC_ERR_MASK_MASK                                                                0x80000
#define TYPE0_UNCORR_ERR_MASK_REG_1_ECRC_ERR_MASK_SHFT                                                                0x13
#define TYPE0_UNCORR_ERR_MASK_REG_1_MALF_TLP_ERR_MASK_MASK                                                            0x40000
#define TYPE0_UNCORR_ERR_MASK_REG_1_MALF_TLP_ERR_MASK_SHFT                                                            0x12
#define TYPE0_UNCORR_ERR_MASK_REG_1_REC_OVERFLOW_ERR_MASK_MASK                                                        0x20000
#define TYPE0_UNCORR_ERR_MASK_REG_1_REC_OVERFLOW_ERR_MASK_SHFT                                                        0x11
#define TYPE0_UNCORR_ERR_MASK_REG_1_UNEXP_CMPLT_ERR_MASK_MASK                                                         0x10000
#define TYPE0_UNCORR_ERR_MASK_REG_1_UNEXP_CMPLT_ERR_MASK_SHFT                                                         0x10
#define TYPE0_UNCORR_ERR_MASK_REG_1_CMPLT_ABORT_ERR_MASK_MASK                                                         0x8000
#define TYPE0_UNCORR_ERR_MASK_REG_1_CMPLT_ABORT_ERR_MASK_SHFT                                                         0xF
#define TYPE0_UNCORR_ERR_MASK_REG_1_CMPLT_TIMEOUT_ERR_MASK_MASK                                                       0x4000
#define TYPE0_UNCORR_ERR_MASK_REG_1_CMPLT_TIMEOUT_ERR_MASK_SHFT                                                       0xE
#define TYPE0_UNCORR_ERR_MASK_REG_1_FC_PROTOCOL_ERR_MASK_MASK                                                         0x2000
#define TYPE0_UNCORR_ERR_MASK_REG_1_FC_PROTOCOL_ERR_MASK_SHFT                                                         0xD
#define TYPE0_UNCORR_ERR_MASK_REG_1_POIS_TLP_ERR_MASK_MASK                                                            0x1000
#define TYPE0_UNCORR_ERR_MASK_REG_1_POIS_TLP_ERR_MASK_SHFT                                                            0xC
#define TYPE0_UNCORR_ERR_MASK_REG_1_SUR_DWN_ERR_MASK_MASK                                                             0x20
#define TYPE0_UNCORR_ERR_MASK_REG_1_SUR_DWN_ERR_MASK_SHFT                                                             0x5
#define TYPE0_UNCORR_ERR_MASK_REG_1_DL_PROTOCOL_ERR_MASK_MASK                                                         0x10
#define TYPE0_UNCORR_ERR_MASK_REG_1_DL_PROTOCOL_ERR_MASK_SHFT                                                         0x4

/* UNCORR_ERR_SEV_REG */
#define UNCORR_ERR_SEV_REG                                                                                            ( 0x10C )
#define UNCORR_ERR_SEV_REG_ATOMIC_EGRESS_BLOCKED_ERR_SEVERITY_MASK                                                    0x1000000
#define UNCORR_ERR_SEV_REG_ATOMIC_EGRESS_BLOCKED_ERR_SEVERITY_SHFT                                                    0x18
#define UNCORR_ERR_SEV_REG_INTERNAL_ERR_SEVERITY_MASK                                                                 0x400000
#define UNCORR_ERR_SEV_REG_INTERNAL_ERR_SEVERITY_SHFT                                                                 0x16
#define UNCORR_ERR_SEV_REG_UNSUPPORTED_REQ_ERR_SEVERITY_MASK                                                          0x100000
#define UNCORR_ERR_SEV_REG_UNSUPPORTED_REQ_ERR_SEVERITY_SHFT                                                          0x14
#define UNCORR_ERR_SEV_REG_ECRC_ERR_SEVERITY_MASK                                                                     0x80000
#define UNCORR_ERR_SEV_REG_ECRC_ERR_SEVERITY_SHFT                                                                     0x13
#define UNCORR_ERR_SEV_REG_MALF_TLP_ERR_SEVERITY_MASK                                                                 0x40000
#define UNCORR_ERR_SEV_REG_MALF_TLP_ERR_SEVERITY_SHFT                                                                 0x12
#define UNCORR_ERR_SEV_REG_REC_OVERFLOW_ERR_SEVERITY_MASK                                                             0x20000
#define UNCORR_ERR_SEV_REG_REC_OVERFLOW_ERR_SEVERITY_SHFT                                                             0x11
#define UNCORR_ERR_SEV_REG_UNEXP_CMPLT_ERR_SEVERITY_MASK                                                              0x10000
#define UNCORR_ERR_SEV_REG_UNEXP_CMPLT_ERR_SEVERITY_SHFT                                                              0x10
#define UNCORR_ERR_SEV_REG_CMPLT_ABORT_ERR_SEVERITY_MASK                                                              0x8000
#define UNCORR_ERR_SEV_REG_CMPLT_ABORT_ERR_SEVERITY_SHFT                                                              0xF
#define UNCORR_ERR_SEV_REG_CMPLT_TIMEOUT_ERR_SEVERITY_MASK                                                            0x4000
#define UNCORR_ERR_SEV_REG_CMPLT_TIMEOUT_ERR_SEVERITY_SHFT                                                            0xE
#define UNCORR_ERR_SEV_REG_FC_PROTOCOL_ERR_SEVERITY_MASK                                                              0x2000
#define UNCORR_ERR_SEV_REG_FC_PROTOCOL_ERR_SEVERITY_SHFT                                                              0xD
#define UNCORR_ERR_SEV_REG_POIS_TLP_ERR_SEVERITY_MASK                                                                 0x1000
#define UNCORR_ERR_SEV_REG_POIS_TLP_ERR_SEVERITY_SHFT                                                                 0xC
#define UNCORR_ERR_SEV_REG_SUR_DWN_ERR_SEVERITY_MASK                                                                  0x20
#define UNCORR_ERR_SEV_REG_SUR_DWN_ERR_SEVERITY_SHFT                                                                  0x5
#define UNCORR_ERR_SEV_REG_DL_PROTOCOL_ERR_SEVERITY_MASK                                                              0x10
#define UNCORR_ERR_SEV_REG_DL_PROTOCOL_ERR_SEVERITY_SHFT                                                              0x4

/* TYPE0_UNCORR_ERR_SEV_REG_1 */
#define TYPE0_UNCORR_ERR_SEV_REG_1                                                                                    ( 0x10C )
#define TYPE0_UNCORR_ERR_SEV_REG_1_ATOMIC_EGRESS_BLOCKED_ERR_SEVERITY_MASK                                            0x1000000
#define TYPE0_UNCORR_ERR_SEV_REG_1_ATOMIC_EGRESS_BLOCKED_ERR_SEVERITY_SHFT                                            0x18
#define TYPE0_UNCORR_ERR_SEV_REG_1_INTERNAL_ERR_SEVERITY_MASK                                                         0x400000
#define TYPE0_UNCORR_ERR_SEV_REG_1_INTERNAL_ERR_SEVERITY_SHFT                                                         0x16
#define TYPE0_UNCORR_ERR_SEV_REG_1_UNSUPPORTED_REQ_ERR_SEVERITY_MASK                                                  0x100000
#define TYPE0_UNCORR_ERR_SEV_REG_1_UNSUPPORTED_REQ_ERR_SEVERITY_SHFT                                                  0x14
#define TYPE0_UNCORR_ERR_SEV_REG_1_ECRC_ERR_SEVERITY_MASK                                                             0x80000
#define TYPE0_UNCORR_ERR_SEV_REG_1_ECRC_ERR_SEVERITY_SHFT                                                             0x13
#define TYPE0_UNCORR_ERR_SEV_REG_1_MALF_TLP_ERR_SEVERITY_MASK                                                         0x40000
#define TYPE0_UNCORR_ERR_SEV_REG_1_MALF_TLP_ERR_SEVERITY_SHFT                                                         0x12
#define TYPE0_UNCORR_ERR_SEV_REG_1_REC_OVERFLOW_ERR_SEVERITY_MASK                                                     0x20000
#define TYPE0_UNCORR_ERR_SEV_REG_1_REC_OVERFLOW_ERR_SEVERITY_SHFT                                                     0x11
#define TYPE0_UNCORR_ERR_SEV_REG_1_UNEXP_CMPLT_ERR_SEVERITY_MASK                                                      0x10000
#define TYPE0_UNCORR_ERR_SEV_REG_1_UNEXP_CMPLT_ERR_SEVERITY_SHFT                                                      0x10
#define TYPE0_UNCORR_ERR_SEV_REG_1_CMPLT_ABORT_ERR_SEVERITY_MASK                                                      0x8000
#define TYPE0_UNCORR_ERR_SEV_REG_1_CMPLT_ABORT_ERR_SEVERITY_SHFT                                                      0xF
#define TYPE0_UNCORR_ERR_SEV_REG_1_CMPLT_TIMEOUT_ERR_SEVERITY_MASK                                                    0x4000
#define TYPE0_UNCORR_ERR_SEV_REG_1_CMPLT_TIMEOUT_ERR_SEVERITY_SHFT                                                    0xE
#define TYPE0_UNCORR_ERR_SEV_REG_1_FC_PROTOCOL_ERR_SEVERITY_MASK                                                      0x2000
#define TYPE0_UNCORR_ERR_SEV_REG_1_FC_PROTOCOL_ERR_SEVERITY_SHFT                                                      0xD
#define TYPE0_UNCORR_ERR_SEV_REG_1_POIS_TLP_ERR_SEVERITY_MASK                                                         0x1000
#define TYPE0_UNCORR_ERR_SEV_REG_1_POIS_TLP_ERR_SEVERITY_SHFT                                                         0xC
#define TYPE0_UNCORR_ERR_SEV_REG_1_SUR_DWN_ERR_SEVERITY_MASK                                                          0x20
#define TYPE0_UNCORR_ERR_SEV_REG_1_SUR_DWN_ERR_SEVERITY_SHFT                                                          0x5
#define TYPE0_UNCORR_ERR_SEV_REG_1_DL_PROTOCOL_ERR_SEVERITY_MASK                                                      0x10
#define TYPE0_UNCORR_ERR_SEV_REG_1_DL_PROTOCOL_ERR_SEVERITY_SHFT                                                      0x4

/* CORR_ERR_STATUS_REG */
#define CORR_ERR_STATUS_REG                                                                                           ( 0x110 )
#define CORR_ERR_STATUS_REG_CORRECTED_INT_ERR_STATUS_MASK                                                             0x4000
#define CORR_ERR_STATUS_REG_CORRECTED_INT_ERR_STATUS_SHFT                                                             0xE
#define CORR_ERR_STATUS_REG_ADVISORY_NON_FATAL_ERR_STATUS_MASK                                                        0x2000
#define CORR_ERR_STATUS_REG_ADVISORY_NON_FATAL_ERR_STATUS_SHFT                                                        0xD
#define CORR_ERR_STATUS_REG_RPL_TIMER_TIMEOUT_STATUS_MASK                                                             0x1000
#define CORR_ERR_STATUS_REG_RPL_TIMER_TIMEOUT_STATUS_SHFT                                                             0xC
#define CORR_ERR_STATUS_REG_REPLAY_NO_ROLEOVER_STATUS_MASK                                                            0x100
#define CORR_ERR_STATUS_REG_REPLAY_NO_ROLEOVER_STATUS_SHFT                                                            0x8
#define CORR_ERR_STATUS_REG_BAD_DLLP_STATUS_MASK                                                                      0x80
#define CORR_ERR_STATUS_REG_BAD_DLLP_STATUS_SHFT                                                                      0x7
#define CORR_ERR_STATUS_REG_BAD_TLP_STATUS_MASK                                                                       0x40
#define CORR_ERR_STATUS_REG_BAD_TLP_STATUS_SHFT                                                                       0x6
#define CORR_ERR_STATUS_REG_RX_ERR_STATUS_MASK                                                                        0x1
#define CORR_ERR_STATUS_REG_RX_ERR_STATUS_SHFT                                                                        0x0

/* TYPE0_CORR_ERR_STATUS_REG_1 */
#define TYPE0_CORR_ERR_STATUS_REG_1                                                                                   ( 0x110 )
#define TYPE0_CORR_ERR_STATUS_REG_1_CORRECTED_INT_ERR_STATUS_MASK                                                     0x4000
#define TYPE0_CORR_ERR_STATUS_REG_1_CORRECTED_INT_ERR_STATUS_SHFT                                                     0xE
#define TYPE0_CORR_ERR_STATUS_REG_1_ADVISORY_NON_FATAL_ERR_STATUS_MASK                                                0x2000
#define TYPE0_CORR_ERR_STATUS_REG_1_ADVISORY_NON_FATAL_ERR_STATUS_SHFT                                                0xD
#define TYPE0_CORR_ERR_STATUS_REG_1_RPL_TIMER_TIMEOUT_STATUS_MASK                                                     0x1000
#define TYPE0_CORR_ERR_STATUS_REG_1_RPL_TIMER_TIMEOUT_STATUS_SHFT                                                     0xC
#define TYPE0_CORR_ERR_STATUS_REG_1_REPLAY_NO_ROLEOVER_STATUS_MASK                                                    0x100
#define TYPE0_CORR_ERR_STATUS_REG_1_REPLAY_NO_ROLEOVER_STATUS_SHFT                                                    0x8
#define TYPE0_CORR_ERR_STATUS_REG_1_BAD_DLLP_STATUS_MASK                                                              0x80
#define TYPE0_CORR_ERR_STATUS_REG_1_BAD_DLLP_STATUS_SHFT                                                              0x7
#define TYPE0_CORR_ERR_STATUS_REG_1_BAD_TLP_STATUS_MASK                                                               0x40
#define TYPE0_CORR_ERR_STATUS_REG_1_BAD_TLP_STATUS_SHFT                                                               0x6
#define TYPE0_CORR_ERR_STATUS_REG_1_RX_ERR_STATUS_MASK                                                                0x1
#define TYPE0_CORR_ERR_STATUS_REG_1_RX_ERR_STATUS_SHFT                                                                0x0

/* CORR_ERR_MASK_REG */
#define CORR_ERR_MASK_REG                                                                                             ( 0x114 )
#define CORR_ERR_MASK_REG_CORRECTED_INT_ERR_MASK_MASK                                                                 0x4000
#define CORR_ERR_MASK_REG_CORRECTED_INT_ERR_MASK_SHFT                                                                 0xE
#define CORR_ERR_MASK_REG_ADVISORY_NON_FATAL_ERR_MASK_MASK                                                            0x2000
#define CORR_ERR_MASK_REG_ADVISORY_NON_FATAL_ERR_MASK_SHFT                                                            0xD
#define CORR_ERR_MASK_REG_RPL_TIMER_TIMEOUT_MASK_MASK                                                                 0x1000
#define CORR_ERR_MASK_REG_RPL_TIMER_TIMEOUT_MASK_SHFT                                                                 0xC
#define CORR_ERR_MASK_REG_REPLAY_NO_ROLEOVER_MASK_MASK                                                                0x100
#define CORR_ERR_MASK_REG_REPLAY_NO_ROLEOVER_MASK_SHFT                                                                0x8
#define CORR_ERR_MASK_REG_BAD_DLLP_MASK_MASK                                                                          0x80
#define CORR_ERR_MASK_REG_BAD_DLLP_MASK_SHFT                                                                          0x7
#define CORR_ERR_MASK_REG_BAD_TLP_MASK_MASK                                                                           0x40
#define CORR_ERR_MASK_REG_BAD_TLP_MASK_SHFT                                                                           0x6
#define CORR_ERR_MASK_REG_RX_ERR_MASK_MASK                                                                            0x1
#define CORR_ERR_MASK_REG_RX_ERR_MASK_SHFT                                                                            0x0

/* TYPE0_CORR_ERR_MASK_REG_1 */
#define TYPE0_CORR_ERR_MASK_REG_1                                                                                     ( 0x114 )
#define TYPE0_CORR_ERR_MASK_REG_1_CORRECTED_INT_ERR_MASK_MASK                                                         0x4000
#define TYPE0_CORR_ERR_MASK_REG_1_CORRECTED_INT_ERR_MASK_SHFT                                                         0xE
#define TYPE0_CORR_ERR_MASK_REG_1_ADVISORY_NON_FATAL_ERR_MASK_MASK                                                    0x2000
#define TYPE0_CORR_ERR_MASK_REG_1_ADVISORY_NON_FATAL_ERR_MASK_SHFT                                                    0xD
#define TYPE0_CORR_ERR_MASK_REG_1_RPL_TIMER_TIMEOUT_MASK_MASK                                                         0x1000
#define TYPE0_CORR_ERR_MASK_REG_1_RPL_TIMER_TIMEOUT_MASK_SHFT                                                         0xC
#define TYPE0_CORR_ERR_MASK_REG_1_REPLAY_NO_ROLEOVER_MASK_MASK                                                        0x100
#define TYPE0_CORR_ERR_MASK_REG_1_REPLAY_NO_ROLEOVER_MASK_SHFT                                                        0x8
#define TYPE0_CORR_ERR_MASK_REG_1_BAD_DLLP_MASK_MASK                                                                  0x80
#define TYPE0_CORR_ERR_MASK_REG_1_BAD_DLLP_MASK_SHFT                                                                  0x7
#define TYPE0_CORR_ERR_MASK_REG_1_BAD_TLP_MASK_MASK                                                                   0x40
#define TYPE0_CORR_ERR_MASK_REG_1_BAD_TLP_MASK_SHFT                                                                   0x6
#define TYPE0_CORR_ERR_MASK_REG_1_RX_ERR_MASK_MASK                                                                    0x1
#define TYPE0_CORR_ERR_MASK_REG_1_RX_ERR_MASK_SHFT                                                                    0x0

/* ADV_ERR_CAP_CTRL_REG */
#define ADV_ERR_CAP_CTRL_REG                                                                                          ( 0x118 )
#define ADV_ERR_CAP_CTRL_REG_ECRC_CHECK_EN_MASK                                                                       0x100
#define ADV_ERR_CAP_CTRL_REG_ECRC_CHECK_EN_SHFT                                                                       0x8
#define ADV_ERR_CAP_CTRL_REG_ECRC_CHECK_CAP_MASK                                                                      0x80
#define ADV_ERR_CAP_CTRL_REG_ECRC_CHECK_CAP_SHFT                                                                      0x7
#define ADV_ERR_CAP_CTRL_REG_ECRC_GEN_EN_MASK                                                                         0x40
#define ADV_ERR_CAP_CTRL_REG_ECRC_GEN_EN_SHFT                                                                         0x6
#define ADV_ERR_CAP_CTRL_REG_ECRC_GEN_CAP_MASK                                                                        0x20
#define ADV_ERR_CAP_CTRL_REG_ECRC_GEN_CAP_SHFT                                                                        0x5
#define ADV_ERR_CAP_CTRL_REG_FIRST_ERR_POINTER_MASK                                                                   0x1F
#define ADV_ERR_CAP_CTRL_REG_FIRST_ERR_POINTER_SHFT                                                                   0x0

/* TYPE0_ADV_ERR_CAP_CTRL_REG_1 */
#define TYPE0_ADV_ERR_CAP_CTRL_REG_1                                                                                  ( 0x118 )
#define TYPE0_ADV_ERR_CAP_CTRL_REG_1_ECRC_CHECK_EN_MASK                                                               0x100
#define TYPE0_ADV_ERR_CAP_CTRL_REG_1_ECRC_CHECK_EN_SHFT                                                               0x8
#define TYPE0_ADV_ERR_CAP_CTRL_REG_1_ECRC_CHECK_CAP_MASK                                                              0x80
#define TYPE0_ADV_ERR_CAP_CTRL_REG_1_ECRC_CHECK_CAP_SHFT                                                              0x7
#define TYPE0_ADV_ERR_CAP_CTRL_REG_1_ECRC_GEN_EN_MASK                                                                 0x40
#define TYPE0_ADV_ERR_CAP_CTRL_REG_1_ECRC_GEN_EN_SHFT                                                                 0x6
#define TYPE0_ADV_ERR_CAP_CTRL_REG_1_ECRC_GEN_CAP_MASK                                                                0x20
#define TYPE0_ADV_ERR_CAP_CTRL_REG_1_ECRC_GEN_CAP_SHFT                                                                0x5
#define TYPE0_ADV_ERR_CAP_CTRL_REG_1_FIRST_ERR_POINTER_MASK                                                           0x1F
#define TYPE0_ADV_ERR_CAP_CTRL_REG_1_FIRST_ERR_POINTER_SHFT                                                           0x0

/* HDR_LOG_REG_0 */
#define HDR_LOG_REG_0                                                                                                 ( 0x11C )
#define HDR_LOG_REG_0_FIRST_DWORD_MASK                                                                                0xFFFFFFFF
#define HDR_LOG_REG_0_FIRST_DWORD_SHFT                                                                                0x0

/* TYPE0_HDR_LOG_REG_0_1 */
#define TYPE0_HDR_LOG_REG_0_1                                                                                         ( 0x11C )
#define TYPE0_HDR_LOG_REG_0_1_FIRST_DWORD_MASK                                                                        0xFFFFFFFF
#define TYPE0_HDR_LOG_REG_0_1_FIRST_DWORD_SHFT                                                                        0x0

/* HDR_LOG_REG_1 */
#define HDR_LOG_REG_1                                                                                                 ( 0x120 )
#define HDR_LOG_REG_1_SECOND_DWORD_MASK                                                                               0xFFFFFFFF
#define HDR_LOG_REG_1_SECOND_DWORD_SHFT                                                                               0x0

/* TYPE0_HDR_LOG_REG_1_1 */
#define TYPE0_HDR_LOG_REG_1_1                                                                                         ( 0x120 )
#define TYPE0_HDR_LOG_REG_1_1_SECOND_DWORD_MASK                                                                       0xFFFFFFFF
#define TYPE0_HDR_LOG_REG_1_1_SECOND_DWORD_SHFT                                                                       0x0

/* HDR_LOG_REG_2 */
#define HDR_LOG_REG_2                                                                                                 ( 0x124 )
#define HDR_LOG_REG_2_THIRD_DWORD_MASK                                                                                0xFFFFFFFF
#define HDR_LOG_REG_2_THIRD_DWORD_SHFT                                                                                0x0

/* TYPE0_HDR_LOG_REG_2_1 */
#define TYPE0_HDR_LOG_REG_2_1                                                                                         ( 0x124 )
#define TYPE0_HDR_LOG_REG_2_1_THIRD_DWORD_MASK                                                                        0xFFFFFFFF
#define TYPE0_HDR_LOG_REG_2_1_THIRD_DWORD_SHFT                                                                        0x0

/* HDR_LOG_REG_3 */
#define HDR_LOG_REG_3                                                                                                 ( 0x128 )
#define HDR_LOG_REG_3_FOURTH_DWORD_MASK                                                                               0xFFFFFFFF
#define HDR_LOG_REG_3_FOURTH_DWORD_SHFT                                                                               0x0

/* TYPE0_HDR_LOG_REG_3_1 */
#define TYPE0_HDR_LOG_REG_3_1                                                                                         ( 0x128 )
#define TYPE0_HDR_LOG_REG_3_1_FOURTH_DWORD_MASK                                                                       0xFFFFFFFF
#define TYPE0_HDR_LOG_REG_3_1_FOURTH_DWORD_SHFT                                                                       0x0

/* ROOT_ERR_CMD_REG */
#define ROOT_ERR_CMD_REG                                                                                              ( 0x12C )
#define ROOT_ERR_CMD_REG_FATAL_ERR_REPORTING_EN_MASK                                                                  0x4
#define ROOT_ERR_CMD_REG_FATAL_ERR_REPORTING_EN_SHFT                                                                  0x2
#define ROOT_ERR_CMD_REG_NON_FATAL_ERR_REPORTING_EN_MASK                                                              0x2
#define ROOT_ERR_CMD_REG_NON_FATAL_ERR_REPORTING_EN_SHFT                                                              0x1
#define ROOT_ERR_CMD_REG_CORR_ERR_REPORTING_EN_MASK                                                                   0x1
#define ROOT_ERR_CMD_REG_CORR_ERR_REPORTING_EN_SHFT                                                                   0x0

/* TYPE0_DUMMY_ROOT_ERR_CMD_REG */
#define TYPE0_DUMMY_ROOT_ERR_CMD_REG                                                                                  ( 0x12C )
#define TYPE0_DUMMY_ROOT_ERR_CMD_REG_FATAL_ERR_REPORTING_EN_MASK                                                      0x4
#define TYPE0_DUMMY_ROOT_ERR_CMD_REG_FATAL_ERR_REPORTING_EN_SHFT                                                      0x2
#define TYPE0_DUMMY_ROOT_ERR_CMD_REG_NON_FATAL_ERR_REPORTING_EN_MASK                                                  0x2
#define TYPE0_DUMMY_ROOT_ERR_CMD_REG_NON_FATAL_ERR_REPORTING_EN_SHFT                                                  0x1
#define TYPE0_DUMMY_ROOT_ERR_CMD_REG_CORR_ERR_REPORTING_EN_MASK                                                       0x1
#define TYPE0_DUMMY_ROOT_ERR_CMD_REG_CORR_ERR_REPORTING_EN_SHFT                                                       0x0

/* ROOT_ERR_STATUS_REG */
#define ROOT_ERR_STATUS_REG                                                                                           ( 0x130 )
#define ROOT_ERR_STATUS_REG_ADV_ERR_INT_MSG_NUM_MASK                                                                  0xF8000000
#define ROOT_ERR_STATUS_REG_ADV_ERR_INT_MSG_NUM_SHFT                                                                  0x1B
#define ROOT_ERR_STATUS_REG_FATAL_ERR_MSG_RX_MASK                                                                     0x40
#define ROOT_ERR_STATUS_REG_FATAL_ERR_MSG_RX_SHFT                                                                     0x6
#define ROOT_ERR_STATUS_REG_NON_FATAL_ERR_MSG_RX_MASK                                                                 0x20
#define ROOT_ERR_STATUS_REG_NON_FATAL_ERR_MSG_RX_SHFT                                                                 0x5
#define ROOT_ERR_STATUS_REG_FIRST_UNCORR_FATAL_MASK                                                                   0x10
#define ROOT_ERR_STATUS_REG_FIRST_UNCORR_FATAL_SHFT                                                                   0x4
#define ROOT_ERR_STATUS_REG_MUL_ERR_FATAL_NON_FATAL_RX_MASK                                                           0x8
#define ROOT_ERR_STATUS_REG_MUL_ERR_FATAL_NON_FATAL_RX_SHFT                                                           0x3
#define ROOT_ERR_STATUS_REG_ERR_FATAL_NON_FATAL_RX_MASK                                                               0x4
#define ROOT_ERR_STATUS_REG_ERR_FATAL_NON_FATAL_RX_SHFT                                                               0x2
#define ROOT_ERR_STATUS_REG_MUL_ERR_COR_RX_MASK                                                                       0x2
#define ROOT_ERR_STATUS_REG_MUL_ERR_COR_RX_SHFT                                                                       0x1
#define ROOT_ERR_STATUS_REG_ERR_COR_RX_MASK                                                                           0x1
#define ROOT_ERR_STATUS_REG_ERR_COR_RX_SHFT                                                                           0x0

/* TYPE0_DUMMY_ROOT_ERR_STATUS_REG */
#define TYPE0_DUMMY_ROOT_ERR_STATUS_REG                                                                               ( 0x130 )
#define TYPE0_DUMMY_ROOT_ERR_STATUS_REG_ADV_ERR_INT_MSG_NUM_MASK                                                      0xF8000000
#define TYPE0_DUMMY_ROOT_ERR_STATUS_REG_ADV_ERR_INT_MSG_NUM_SHFT                                                      0x1B
#define TYPE0_DUMMY_ROOT_ERR_STATUS_REG_FATAL_ERR_MSG_RX_MASK                                                         0x40
#define TYPE0_DUMMY_ROOT_ERR_STATUS_REG_FATAL_ERR_MSG_RX_SHFT                                                         0x6
#define TYPE0_DUMMY_ROOT_ERR_STATUS_REG_NON_FATAL_ERR_MSG_RX_MASK                                                     0x20
#define TYPE0_DUMMY_ROOT_ERR_STATUS_REG_NON_FATAL_ERR_MSG_RX_SHFT                                                     0x5
#define TYPE0_DUMMY_ROOT_ERR_STATUS_REG_FIRST_UNCORR_FATAL_MASK                                                       0x10
#define TYPE0_DUMMY_ROOT_ERR_STATUS_REG_FIRST_UNCORR_FATAL_SHFT                                                       0x4
#define TYPE0_DUMMY_ROOT_ERR_STATUS_REG_MUL_ERR_FATAL_NON_FATAL_RX_MASK                                               0x8
#define TYPE0_DUMMY_ROOT_ERR_STATUS_REG_MUL_ERR_FATAL_NON_FATAL_RX_SHFT                                               0x3
#define TYPE0_DUMMY_ROOT_ERR_STATUS_REG_ERR_FATAL_NON_FATAL_RX_MASK                                                   0x4
#define TYPE0_DUMMY_ROOT_ERR_STATUS_REG_ERR_FATAL_NON_FATAL_RX_SHFT                                                   0x2
#define TYPE0_DUMMY_ROOT_ERR_STATUS_REG_MUL_ERR_COR_RX_MASK                                                           0x2
#define TYPE0_DUMMY_ROOT_ERR_STATUS_REG_MUL_ERR_COR_RX_SHFT                                                           0x1
#define TYPE0_DUMMY_ROOT_ERR_STATUS_REG_ERR_COR_RX_MASK                                                               0x1
#define TYPE0_DUMMY_ROOT_ERR_STATUS_REG_ERR_COR_RX_SHFT                                                               0x0

/* ERR_SRC_ID_REG */
#define ERR_SRC_ID_REG                                                                                                ( 0x134 )
#define ERR_SRC_ID_REG_ERR_FATAL_NON_FATAL_SOURCE_ID_MASK                                                             0xFFFF0000
#define ERR_SRC_ID_REG_ERR_FATAL_NON_FATAL_SOURCE_ID_SHFT                                                             0x10
#define ERR_SRC_ID_REG_ERR_COR_SOURCE_ID_MASK                                                                         0xFFFF
#define ERR_SRC_ID_REG_ERR_COR_SOURCE_ID_SHFT                                                                         0x0

/* TYPE0_DUMMY_ERR_SRC_ID_REG */
#define TYPE0_DUMMY_ERR_SRC_ID_REG                                                                                    ( 0x134 )
#define TYPE0_DUMMY_ERR_SRC_ID_REG_ERR_FATAL_NON_FATAL_SOURCE_ID_MASK                                                 0xFFFF0000
#define TYPE0_DUMMY_ERR_SRC_ID_REG_ERR_FATAL_NON_FATAL_SOURCE_ID_SHFT                                                 0x10
#define TYPE0_DUMMY_ERR_SRC_ID_REG_ERR_COR_SOURCE_ID_MASK                                                             0xFFFF
#define TYPE0_DUMMY_ERR_SRC_ID_REG_ERR_COR_SOURCE_ID_SHFT                                                             0x0

/* TLP_PREFIX_LOG_REG */
#define TLP_PREFIX_LOG_REG                                                                                            ( 0x138 )
#define TLP_PREFIX_LOG_REG_TLP_PREFIX_LOG_REG_MASK                                                                    0xFFFFFFFF
#define TLP_PREFIX_LOG_REG_TLP_PREFIX_LOG_REG_SHFT                                                                    0x0

/* TYPE0_TLP_PREFIX_LOG_REG_1 */
#define TYPE0_TLP_PREFIX_LOG_REG_1                                                                                    ( 0x138 )
#define TYPE0_TLP_PREFIX_LOG_REG_1_TLP_PREFIX_LOG_REG_MASK                                                            0xFFFFFFFF
#define TYPE0_TLP_PREFIX_LOG_REG_1_TLP_PREFIX_LOG_REG_SHFT                                                            0x0

/* LTR_CAP_HDR_REG */
#define LTR_CAP_HDR_REG                                                                                               ( 0x148 )
#define LTR_CAP_HDR_REG_NEXT_OFFSET_MASK                                                                              0xFFF00000
#define LTR_CAP_HDR_REG_NEXT_OFFSET_SHFT                                                                              0x14
#define LTR_CAP_HDR_REG_CAP_VERSION_MASK                                                                              0xF0000
#define LTR_CAP_HDR_REG_CAP_VERSION_SHFT                                                                              0x10
#define LTR_CAP_HDR_REG_CAP_ID_MASK                                                                                   0xFFFF
#define LTR_CAP_HDR_REG_CAP_ID_SHFT                                                                                   0x0

/* TYPE0_LTR_CAP_HDR_REG_1 */
#define TYPE0_LTR_CAP_HDR_REG_1                                                                                       ( 0x148 )
#define TYPE0_LTR_CAP_HDR_REG_1_NEXT_OFFSET_MASK                                                                      0xFFF00000
#define TYPE0_LTR_CAP_HDR_REG_1_NEXT_OFFSET_SHFT                                                                      0x14
#define TYPE0_LTR_CAP_HDR_REG_1_CAP_VERSION_MASK                                                                      0xF0000
#define TYPE0_LTR_CAP_HDR_REG_1_CAP_VERSION_SHFT                                                                      0x10
#define TYPE0_LTR_CAP_HDR_REG_1_CAP_ID_MASK                                                                           0xFFFF
#define TYPE0_LTR_CAP_HDR_REG_1_CAP_ID_SHFT                                                                           0x0

/* LTR_LATENCY_REG */
#define LTR_LATENCY_REG                                                                                               ( 0x14C )
#define LTR_LATENCY_REG_MAX_NO_SNOOP_LAT_SCALE_MASK                                                                   0x1C000000
#define LTR_LATENCY_REG_MAX_NO_SNOOP_LAT_SCALE_SHFT                                                                   0x1A
#define LTR_LATENCY_REG_MAX_NO_SNOOP_LAT_MASK                                                                         0x3FF0000
#define LTR_LATENCY_REG_MAX_NO_SNOOP_LAT_SHFT                                                                         0x10
#define LTR_LATENCY_REG_MAX_SNOOP_LAT_SCALE_MASK                                                                      0x1C00
#define LTR_LATENCY_REG_MAX_SNOOP_LAT_SCALE_SHFT                                                                      0xA
#define LTR_LATENCY_REG_MAX_SNOOP_LAT_MASK                                                                            0x3FF
#define LTR_LATENCY_REG_MAX_SNOOP_LAT_SHFT                                                                            0x0

/* TYPE0_LTR_LATENCY_REG_1 */
#define TYPE0_LTR_LATENCY_REG_1                                                                                       ( 0x14C )
#define TYPE0_LTR_LATENCY_REG_1_MAX_NO_SNOOP_LAT_SCALE_MASK                                                           0x1C000000
#define TYPE0_LTR_LATENCY_REG_1_MAX_NO_SNOOP_LAT_SCALE_SHFT                                                           0x1A
#define TYPE0_LTR_LATENCY_REG_1_MAX_NO_SNOOP_LAT_MASK                                                                 0x3FF0000
#define TYPE0_LTR_LATENCY_REG_1_MAX_NO_SNOOP_LAT_SHFT                                                                 0x10
#define TYPE0_LTR_LATENCY_REG_1_MAX_SNOOP_LAT_SCALE_MASK                                                              0x1C00
#define TYPE0_LTR_LATENCY_REG_1_MAX_SNOOP_LAT_SCALE_SHFT                                                              0xA
#define TYPE0_LTR_LATENCY_REG_1_MAX_SNOOP_LAT_MASK                                                                    0x3FF
#define TYPE0_LTR_LATENCY_REG_1_MAX_SNOOP_LAT_SHFT                                                                    0x0

/* L1SUB_CAP_HEADER_REG */
#define L1SUB_CAP_HEADER_REG                                                                                          ( 0x150 )
#define L1SUB_CAP_HEADER_REG_NEXT_OFFSET_MASK                                                                         0xFFF00000
#define L1SUB_CAP_HEADER_REG_NEXT_OFFSET_SHFT                                                                         0x14
#define L1SUB_CAP_HEADER_REG_CAP_VERSION_MASK                                                                         0xF0000
#define L1SUB_CAP_HEADER_REG_CAP_VERSION_SHFT                                                                         0x10
#define L1SUB_CAP_HEADER_REG_EXTENDED_CAP_ID_MASK                                                                     0xFFFF
#define L1SUB_CAP_HEADER_REG_EXTENDED_CAP_ID_SHFT                                                                     0x0

/* TYPE0_L1SUB_CAP_HEADER_REG_1 */
#define TYPE0_L1SUB_CAP_HEADER_REG_1                                                                                  ( 0x150 )
#define TYPE0_L1SUB_CAP_HEADER_REG_1_NEXT_OFFSET_MASK                                                                 0xFFF00000
#define TYPE0_L1SUB_CAP_HEADER_REG_1_NEXT_OFFSET_SHFT                                                                 0x14
#define TYPE0_L1SUB_CAP_HEADER_REG_1_CAP_VERSION_MASK                                                                 0xF0000
#define TYPE0_L1SUB_CAP_HEADER_REG_1_CAP_VERSION_SHFT                                                                 0x10
#define TYPE0_L1SUB_CAP_HEADER_REG_1_EXTENDED_CAP_ID_MASK                                                             0xFFFF
#define TYPE0_L1SUB_CAP_HEADER_REG_1_EXTENDED_CAP_ID_SHFT                                                             0x0

/* L1SUB_CAPABILITY_REG */
#define L1SUB_CAPABILITY_REG                                                                                          ( 0x154 )
#define L1SUB_CAPABILITY_REG_PWR_ON_VALUE_SUPPORT_MASK                                                                0xF80000
#define L1SUB_CAPABILITY_REG_PWR_ON_VALUE_SUPPORT_SHFT                                                                0x13
#define L1SUB_CAPABILITY_REG_PWR_ON_SCALE_SUPPORT_MASK                                                                0x30000
#define L1SUB_CAPABILITY_REG_PWR_ON_SCALE_SUPPORT_SHFT                                                                0x10
#define L1SUB_CAPABILITY_REG_COMM_MODE_SUPPORT_MASK                                                                   0xFF00
#define L1SUB_CAPABILITY_REG_COMM_MODE_SUPPORT_SHFT                                                                   0x8
#define L1SUB_CAPABILITY_REG_L1_PMSUB_SUPPORT_MASK                                                                    0x10
#define L1SUB_CAPABILITY_REG_L1_PMSUB_SUPPORT_SHFT                                                                    0x4
#define L1SUB_CAPABILITY_REG_L1_1_ASPM_SUPPORT_MASK                                                                   0x8
#define L1SUB_CAPABILITY_REG_L1_1_ASPM_SUPPORT_SHFT                                                                   0x3
#define L1SUB_CAPABILITY_REG_L1_2_ASPM_SUPPORT_MASK                                                                   0x4
#define L1SUB_CAPABILITY_REG_L1_2_ASPM_SUPPORT_SHFT                                                                   0x2
#define L1SUB_CAPABILITY_REG_L1_1_PCIPM_SUPPORT_MASK                                                                  0x2
#define L1SUB_CAPABILITY_REG_L1_1_PCIPM_SUPPORT_SHFT                                                                  0x1
#define L1SUB_CAPABILITY_REG_L1_2_PCIPM_SUPPORT_MASK                                                                  0x1
#define L1SUB_CAPABILITY_REG_L1_2_PCIPM_SUPPORT_SHFT                                                                  0x0

/* TYPE0_L1SUB_CAPABILITY_REG_1 */
#define TYPE0_L1SUB_CAPABILITY_REG_1                                                                                  ( 0x154 )
#define TYPE0_L1SUB_CAPABILITY_REG_1_PWR_ON_VALUE_SUPPORT_MASK                                                        0xF80000
#define TYPE0_L1SUB_CAPABILITY_REG_1_PWR_ON_VALUE_SUPPORT_SHFT                                                        0x13
#define TYPE0_L1SUB_CAPABILITY_REG_1_PWR_ON_SCALE_SUPPORT_MASK                                                        0x30000
#define TYPE0_L1SUB_CAPABILITY_REG_1_PWR_ON_SCALE_SUPPORT_SHFT                                                        0x10
#define TYPE0_L1SUB_CAPABILITY_REG_1_COMM_MODE_SUPPORT_MASK                                                           0xFF00
#define TYPE0_L1SUB_CAPABILITY_REG_1_COMM_MODE_SUPPORT_SHFT                                                           0x8
#define TYPE0_L1SUB_CAPABILITY_REG_1_L1_PMSUB_SUPPORT_MASK                                                            0x10
#define TYPE0_L1SUB_CAPABILITY_REG_1_L1_PMSUB_SUPPORT_SHFT                                                            0x4
#define TYPE0_L1SUB_CAPABILITY_REG_1_L1_1_ASPM_SUPPORT_MASK                                                           0x8
#define TYPE0_L1SUB_CAPABILITY_REG_1_L1_1_ASPM_SUPPORT_SHFT                                                           0x3
#define TYPE0_L1SUB_CAPABILITY_REG_1_L1_2_ASPM_SUPPORT_MASK                                                           0x4
#define TYPE0_L1SUB_CAPABILITY_REG_1_L1_2_ASPM_SUPPORT_SHFT                                                           0x2
#define TYPE0_L1SUB_CAPABILITY_REG_1_L1_1_PCIPM_SUPPORT_MASK                                                          0x2
#define TYPE0_L1SUB_CAPABILITY_REG_1_L1_1_PCIPM_SUPPORT_SHFT                                                          0x1
#define TYPE0_L1SUB_CAPABILITY_REG_1_L1_2_PCIPM_SUPPORT_MASK                                                          0x1
#define TYPE0_L1SUB_CAPABILITY_REG_1_L1_2_PCIPM_SUPPORT_SHFT                                                          0x0

/* L1SUB_CONTROL1_REG */
#define L1SUB_CONTROL1_REG                                                                                            ( 0x158 )
#define L1SUB_CONTROL1_REG_L1_2_TH_SCA_MASK                                                                           0xE0000000
#define L1SUB_CONTROL1_REG_L1_2_TH_SCA_SHFT                                                                           0x1D
#define L1SUB_CONTROL1_REG_L1_2_TH_VAL_MASK                                                                           0x3FF0000
#define L1SUB_CONTROL1_REG_L1_2_TH_VAL_SHFT                                                                           0x10
#define L1SUB_CONTROL1_REG_T_COMMON_MODE_MASK                                                                         0xFF00
#define L1SUB_CONTROL1_REG_T_COMMON_MODE_SHFT                                                                         0x8
#define L1SUB_CONTROL1_REG_L1_1_ASPM_EN_MASK                                                                          0x8
#define L1SUB_CONTROL1_REG_L1_1_ASPM_EN_SHFT                                                                          0x3
#define L1SUB_CONTROL1_REG_L1_2_ASPM_EN_MASK                                                                          0x4
#define L1SUB_CONTROL1_REG_L1_2_ASPM_EN_SHFT                                                                          0x2
#define L1SUB_CONTROL1_REG_L1_1_PCIPM_EN_MASK                                                                         0x2
#define L1SUB_CONTROL1_REG_L1_1_PCIPM_EN_SHFT                                                                         0x1
#define L1SUB_CONTROL1_REG_L1_2_PCIPM_EN_MASK                                                                         0x1
#define L1SUB_CONTROL1_REG_L1_2_PCIPM_EN_SHFT                                                                         0x0

/* TYPE0_L1SUB_CONTROL1_REG_1 */
#define TYPE0_L1SUB_CONTROL1_REG_1                                                                                    ( 0x158 )
#define TYPE0_L1SUB_CONTROL1_REG_1_L1_2_TH_SCA_MASK                                                                   0xE0000000
#define TYPE0_L1SUB_CONTROL1_REG_1_L1_2_TH_SCA_SHFT                                                                   0x1D
#define TYPE0_L1SUB_CONTROL1_REG_1_L1_2_TH_VAL_MASK                                                                   0x3FF0000
#define TYPE0_L1SUB_CONTROL1_REG_1_L1_2_TH_VAL_SHFT                                                                   0x10
#define TYPE0_L1SUB_CONTROL1_REG_1_T_COMMON_MODE_MASK                                                                 0xFF00
#define TYPE0_L1SUB_CONTROL1_REG_1_T_COMMON_MODE_SHFT                                                                 0x8
#define TYPE0_L1SUB_CONTROL1_REG_1_L1_1_ASPM_EN_MASK                                                                  0x8
#define TYPE0_L1SUB_CONTROL1_REG_1_L1_1_ASPM_EN_SHFT                                                                  0x3
#define TYPE0_L1SUB_CONTROL1_REG_1_L1_2_ASPM_EN_MASK                                                                  0x4
#define TYPE0_L1SUB_CONTROL1_REG_1_L1_2_ASPM_EN_SHFT                                                                  0x2
#define TYPE0_L1SUB_CONTROL1_REG_1_L1_1_PCIPM_EN_MASK                                                                 0x2
#define TYPE0_L1SUB_CONTROL1_REG_1_L1_1_PCIPM_EN_SHFT                                                                 0x1
#define TYPE0_L1SUB_CONTROL1_REG_1_L1_2_PCIPM_EN_MASK                                                                 0x1
#define TYPE0_L1SUB_CONTROL1_REG_1_L1_2_PCIPM_EN_SHFT                                                                 0x0

/* L1SUB_CONTROL2_REG */
#define L1SUB_CONTROL2_REG                                                                                            ( 0x15C )
#define L1SUB_CONTROL2_REG_T_POWER_ON_VALUE_MASK                                                                      0xF8
#define L1SUB_CONTROL2_REG_T_POWER_ON_VALUE_SHFT                                                                      0x3
#define L1SUB_CONTROL2_REG_T_POWER_ON_SCALE_MASK                                                                      0x3
#define L1SUB_CONTROL2_REG_T_POWER_ON_SCALE_SHFT                                                                      0x0

/* TYPE0_L1SUB_CONTROL2_REG_1 */
#define TYPE0_L1SUB_CONTROL2_REG_1                                                                                    ( 0x15C )
#define TYPE0_L1SUB_CONTROL2_REG_1_T_POWER_ON_VALUE_MASK                                                              0xF8
#define TYPE0_L1SUB_CONTROL2_REG_1_T_POWER_ON_VALUE_SHFT                                                              0x3
#define TYPE0_L1SUB_CONTROL2_REG_1_T_POWER_ON_SCALE_MASK                                                              0x3
#define TYPE0_L1SUB_CONTROL2_REG_1_T_POWER_ON_SCALE_SHFT                                                              0x0

/* ACK_LATENCY_TIMER */
#define ACK_LATENCY_TIMER                                                                                             ( 0x700 )
#define ACK_LATENCY_TIMER_REPLAY_TIME_LIMIT_MASK                                                                      0xFFFF0000
#define ACK_LATENCY_TIMER_REPLAY_TIME_LIMIT_SHFT                                                                      0x10
#define ACK_LATENCY_TIMER_ROUND_TRIP_LATENCY_TIME_LIMIT_MASK                                                          0xFFFF
#define ACK_LATENCY_TIMER_ROUND_TRIP_LATENCY_TIME_LIMIT_SHFT                                                          0x0

/* TYPE0_ACK_LATENCY_TIMER */
#define TYPE0_ACK_LATENCY_TIMER                                                                                       ( 0x700 )
#define TYPE0_ACK_LATENCY_TIMER_REPLAY_TIME_LIMIT_MASK                                                                0xFFFF0000
#define TYPE0_ACK_LATENCY_TIMER_REPLAY_TIME_LIMIT_SHFT                                                                0x10
#define TYPE0_ACK_LATENCY_TIMER_ROUND_TRIP_LATENCY_TIME_LIMIT_MASK                                                    0xFFFF
#define TYPE0_ACK_LATENCY_TIMER_ROUND_TRIP_LATENCY_TIME_LIMIT_SHFT                                                    0x0

/* VENDOR_SPEC_DLLP_REG */
#define VENDOR_SPEC_DLLP_REG                                                                                          ( 0x704 )
#define VENDOR_SPEC_DLLP_REG_VENDOR_SPEC_DLLP_MASK                                                                    0xFFFFFFFF
#define VENDOR_SPEC_DLLP_REG_VENDOR_SPEC_DLLP_SHFT                                                                    0x0

/* TYPE0_VENDOR_SPEC_DLLP_REG */
#define TYPE0_VENDOR_SPEC_DLLP_REG                                                                                    ( 0x704 )
#define TYPE0_VENDOR_SPEC_DLLP_REG_VENDOR_SPEC_DLLP_MASK                                                              0xFFFFFFFF
#define TYPE0_VENDOR_SPEC_DLLP_REG_VENDOR_SPEC_DLLP_SHFT                                                              0x0

/* PORT_FORCE_REG */
#define PORT_FORCE_REG                                                                                                ( 0x708 )
#define PORT_FORCE_REG_CPL_SENT_COUNT_MASK                                                                            0xFF000000
#define PORT_FORCE_REG_CPL_SENT_COUNT_SHFT                                                                            0x18
#define PORT_FORCE_REG_LINK_STATE_MASK                                                                                0x3F0000
#define PORT_FORCE_REG_LINK_STATE_SHFT                                                                                0x10
#define PORT_FORCE_REG_FORCED_LTSSM_MASK                                                                              0xF00
#define PORT_FORCE_REG_FORCED_LTSSM_SHFT                                                                              0x8
#define PORT_FORCE_REG_LINK_NUM_MASK                                                                                  0xFF
#define PORT_FORCE_REG_LINK_NUM_SHFT                                                                                  0x0

/* TYPE0_PORT_FORCE_REG */
#define TYPE0_PORT_FORCE_REG                                                                                          ( 0x708 )
#define TYPE0_PORT_FORCE_REG_CPL_SENT_COUNT_MASK                                                                      0xFF000000
#define TYPE0_PORT_FORCE_REG_CPL_SENT_COUNT_SHFT                                                                      0x18
#define TYPE0_PORT_FORCE_REG_LINK_STATE_MASK                                                                          0x3F0000
#define TYPE0_PORT_FORCE_REG_LINK_STATE_SHFT                                                                          0x10
#define TYPE0_PORT_FORCE_REG_FORCED_LTSSM_MASK                                                                        0xF00
#define TYPE0_PORT_FORCE_REG_FORCED_LTSSM_SHFT                                                                        0x8
#define TYPE0_PORT_FORCE_REG_LINK_NUM_MASK                                                                            0xFF
#define TYPE0_PORT_FORCE_REG_LINK_NUM_SHFT                                                                            0x0

/* ACK_F_ASPM_CTRL_REG */
#define ACK_F_ASPM_CTRL_REG                                                                                           ( 0x70C )
#define ACK_F_ASPM_CTRL_REG_ENTER_ASPM_MASK                                                                           0x40000000
#define ACK_F_ASPM_CTRL_REG_ENTER_ASPM_SHFT                                                                           0x1E
#define ACK_F_ASPM_CTRL_REG_L1_ENTRANCE_LATENCY_MASK                                                                  0x38000000
#define ACK_F_ASPM_CTRL_REG_L1_ENTRANCE_LATENCY_SHFT                                                                  0x1B
#define ACK_F_ASPM_CTRL_REG_L0_ENTRANCE_LATENCY_MASK                                                                  0x7000000
#define ACK_F_ASPM_CTRL_REG_L0_ENTRANCE_LATENCY_SHFT                                                                  0x18
#define ACK_F_ASPM_CTRL_REG_COMMON_CLK_N_FTS_MASK                                                                     0xFF0000
#define ACK_F_ASPM_CTRL_REG_COMMON_CLK_N_FTS_SHFT                                                                     0x10
#define ACK_F_ASPM_CTRL_REG_ACK_N_FTS_MASK                                                                            0xFF00
#define ACK_F_ASPM_CTRL_REG_ACK_N_FTS_SHFT                                                                            0x8
#define ACK_F_ASPM_CTRL_REG_ACK_FREQ_MASK                                                                             0xFF
#define ACK_F_ASPM_CTRL_REG_ACK_FREQ_SHFT                                                                             0x0

/* TYPE0_ACK_F_ASPM_CTRL_REG */
#define TYPE0_ACK_F_ASPM_CTRL_REG                                                                                     ( 0x70C )
#define TYPE0_ACK_F_ASPM_CTRL_REG_ENTER_ASPM_MASK                                                                     0x40000000
#define TYPE0_ACK_F_ASPM_CTRL_REG_ENTER_ASPM_SHFT                                                                     0x1E
#define TYPE0_ACK_F_ASPM_CTRL_REG_L1_ENTRANCE_LATENCY_MASK                                                            0x38000000
#define TYPE0_ACK_F_ASPM_CTRL_REG_L1_ENTRANCE_LATENCY_SHFT                                                            0x1B
#define TYPE0_ACK_F_ASPM_CTRL_REG_L0_ENTRANCE_LATENCY_MASK                                                            0x7000000
#define TYPE0_ACK_F_ASPM_CTRL_REG_L0_ENTRANCE_LATENCY_SHFT                                                            0x18
#define TYPE0_ACK_F_ASPM_CTRL_REG_COMMON_CLK_N_FTS_MASK                                                               0xFF0000
#define TYPE0_ACK_F_ASPM_CTRL_REG_COMMON_CLK_N_FTS_SHFT                                                               0x10
#define TYPE0_ACK_F_ASPM_CTRL_REG_ACK_N_FTS_MASK                                                                      0xFF00
#define TYPE0_ACK_F_ASPM_CTRL_REG_ACK_N_FTS_SHFT                                                                      0x8
#define TYPE0_ACK_F_ASPM_CTRL_REG_ACK_FREQ_MASK                                                                       0xFF
#define TYPE0_ACK_F_ASPM_CTRL_REG_ACK_FREQ_SHFT                                                                       0x0

/* PORT_LINK_CTRL_REG */
#define PORT_LINK_CTRL_REG                                                                                            ( 0x710 )
#define PORT_LINK_CTRL_REG_TRANSMIT_LANE_REVERSALE_ENABLE_MASK                                                        0x8000000
#define PORT_LINK_CTRL_REG_TRANSMIT_LANE_REVERSALE_ENABLE_SHFT                                                        0x1B
#define PORT_LINK_CTRL_REG_EXTENDED_SYNCH_MASK                                                                        0x4000000
#define PORT_LINK_CTRL_REG_EXTENDED_SYNCH_SHFT                                                                        0x1A
#define PORT_LINK_CTRL_REG_CORRUPT_LCRC_ENABLE_MASK                                                                   0x2000000
#define PORT_LINK_CTRL_REG_CORRUPT_LCRC_ENABLE_SHFT                                                                   0x19
#define PORT_LINK_CTRL_REG_BEACON_ENABLE_MASK                                                                         0x1000000
#define PORT_LINK_CTRL_REG_BEACON_ENABLE_SHFT                                                                         0x18
#define PORT_LINK_CTRL_REG_LINK_CAPABLE_MASK                                                                          0x3F0000
#define PORT_LINK_CTRL_REG_LINK_CAPABLE_SHFT                                                                          0x10
#define PORT_LINK_CTRL_REG_LINK_RATE_MASK                                                                             0xF00
#define PORT_LINK_CTRL_REG_LINK_RATE_SHFT                                                                             0x8
#define PORT_LINK_CTRL_REG_FAST_LINK_MODE_MASK                                                                        0x80
#define PORT_LINK_CTRL_REG_FAST_LINK_MODE_SHFT                                                                        0x7
#define PORT_LINK_CTRL_REG_LINK_DISABLE_MASK                                                                          0x40
#define PORT_LINK_CTRL_REG_LINK_DISABLE_SHFT                                                                          0x6
#define PORT_LINK_CTRL_REG_DLL_LINK_EN_MASK                                                                           0x20
#define PORT_LINK_CTRL_REG_DLL_LINK_EN_SHFT                                                                           0x5
#define PORT_LINK_CTRL_REG_RESET_ASSERT_MASK                                                                          0x8
#define PORT_LINK_CTRL_REG_RESET_ASSERT_SHFT                                                                          0x3
#define PORT_LINK_CTRL_REG_LOOPBACK_ENABLE_MASK                                                                       0x4
#define PORT_LINK_CTRL_REG_LOOPBACK_ENABLE_SHFT                                                                       0x2
#define PORT_LINK_CTRL_REG_SCRAMBLE_DISABLE_MASK                                                                      0x2
#define PORT_LINK_CTRL_REG_SCRAMBLE_DISABLE_SHFT                                                                      0x1
#define PORT_LINK_CTRL_REG_VENDOR_SPECIFIC_DLLP_REQ_MASK                                                              0x1
#define PORT_LINK_CTRL_REG_VENDOR_SPECIFIC_DLLP_REQ_SHFT                                                              0x0

/* TYPE0_PORT_LINK_CTRL_REG */
#define TYPE0_PORT_LINK_CTRL_REG                                                                                      ( 0x710 )
#define TYPE0_PORT_LINK_CTRL_REG_TRANSMIT_LANE_REVERSALE_ENABLE_MASK                                                  0x8000000
#define TYPE0_PORT_LINK_CTRL_REG_TRANSMIT_LANE_REVERSALE_ENABLE_SHFT                                                  0x1B
#define TYPE0_PORT_LINK_CTRL_REG_EXTENDED_SYNCH_MASK                                                                  0x4000000
#define TYPE0_PORT_LINK_CTRL_REG_EXTENDED_SYNCH_SHFT                                                                  0x1A
#define TYPE0_PORT_LINK_CTRL_REG_CORRUPT_LCRC_ENABLE_MASK                                                             0x2000000
#define TYPE0_PORT_LINK_CTRL_REG_CORRUPT_LCRC_ENABLE_SHFT                                                             0x19
#define TYPE0_PORT_LINK_CTRL_REG_BEACON_ENABLE_MASK                                                                   0x1000000
#define TYPE0_PORT_LINK_CTRL_REG_BEACON_ENABLE_SHFT                                                                   0x18
#define TYPE0_PORT_LINK_CTRL_REG_LINK_CAPABLE_MASK                                                                    0x3F0000
#define TYPE0_PORT_LINK_CTRL_REG_LINK_CAPABLE_SHFT                                                                    0x10
#define TYPE0_PORT_LINK_CTRL_REG_LINK_RATE_MASK                                                                       0xF00
#define TYPE0_PORT_LINK_CTRL_REG_LINK_RATE_SHFT                                                                       0x8
#define TYPE0_PORT_LINK_CTRL_REG_FAST_LINK_MODE_MASK                                                                  0x80
#define TYPE0_PORT_LINK_CTRL_REG_FAST_LINK_MODE_SHFT                                                                  0x7
#define TYPE0_PORT_LINK_CTRL_REG_LINK_DISABLE_MASK                                                                    0x40
#define TYPE0_PORT_LINK_CTRL_REG_LINK_DISABLE_SHFT                                                                    0x6
#define TYPE0_PORT_LINK_CTRL_REG_DLL_LINK_EN_MASK                                                                     0x20
#define TYPE0_PORT_LINK_CTRL_REG_DLL_LINK_EN_SHFT                                                                     0x5
#define TYPE0_PORT_LINK_CTRL_REG_RESET_ASSERT_MASK                                                                    0x8
#define TYPE0_PORT_LINK_CTRL_REG_RESET_ASSERT_SHFT                                                                    0x3
#define TYPE0_PORT_LINK_CTRL_REG_LOOPBACK_ENABLE_MASK                                                                 0x4
#define TYPE0_PORT_LINK_CTRL_REG_LOOPBACK_ENABLE_SHFT                                                                 0x2
#define TYPE0_PORT_LINK_CTRL_REG_SCRAMBLE_DISABLE_MASK                                                                0x2
#define TYPE0_PORT_LINK_CTRL_REG_SCRAMBLE_DISABLE_SHFT                                                                0x1
#define TYPE0_PORT_LINK_CTRL_REG_VENDOR_SPECIFIC_DLLP_REQ_MASK                                                        0x1
#define TYPE0_PORT_LINK_CTRL_REG_VENDOR_SPECIFIC_DLLP_REQ_SHFT                                                        0x0

/* LANE_SKEW_REG */
#define LANE_SKEW_REG                                                                                                 ( 0x714 )
#define LANE_SKEW_REG_DISABLE_LANE_TO_LANE_DESKEW_MASK                                                                0x80000000
#define LANE_SKEW_REG_DISABLE_LANE_TO_LANE_DESKEW_SHFT                                                                0x1F
#define LANE_SKEW_REG_ACK_NAK_DISABLE_MASK                                                                            0x2000000
#define LANE_SKEW_REG_ACK_NAK_DISABLE_SHFT                                                                            0x19
#define LANE_SKEW_REG_FLOW_CTRL_DISABLE_MASK                                                                          0x1000000
#define LANE_SKEW_REG_FLOW_CTRL_DISABLE_SHFT                                                                          0x18
#define LANE_SKEW_REG_INSERT_LANE_SKEW_MASK                                                                           0xFFFFFF
#define LANE_SKEW_REG_INSERT_LANE_SKEW_SHFT                                                                           0x0

/* TYPE0_LANE_SKEW_REG */
#define TYPE0_LANE_SKEW_REG                                                                                           ( 0x714 )
#define TYPE0_LANE_SKEW_REG_DISABLE_LANE_TO_LANE_DESKEW_MASK                                                          0x80000000
#define TYPE0_LANE_SKEW_REG_DISABLE_LANE_TO_LANE_DESKEW_SHFT                                                          0x1F
#define TYPE0_LANE_SKEW_REG_ACK_NAK_DISABLE_MASK                                                                      0x2000000
#define TYPE0_LANE_SKEW_REG_ACK_NAK_DISABLE_SHFT                                                                      0x19
#define TYPE0_LANE_SKEW_REG_FLOW_CTRL_DISABLE_MASK                                                                    0x1000000
#define TYPE0_LANE_SKEW_REG_FLOW_CTRL_DISABLE_SHFT                                                                    0x18
#define TYPE0_LANE_SKEW_REG_INSERT_LANE_SKEW_MASK                                                                     0xFFFFFF
#define TYPE0_LANE_SKEW_REG_INSERT_LANE_SKEW_SHFT                                                                     0x0

/* TIMER_CTRL_MAX_FUNC_NUM */
#define TIMER_CTRL_MAX_FUNC_NUM                                                                                       ( 0x718 )
#define TIMER_CTRL_MAX_FUNC_NUM_UPDATE_FREQ_TIMER_MASK                                                                0x1F000000
#define TIMER_CTRL_MAX_FUNC_NUM_UPDATE_FREQ_TIMER_SHFT                                                                0x18
#define TIMER_CTRL_MAX_FUNC_NUM_TIMER_MOD_ACK_NAK_MASK                                                                0xF80000
#define TIMER_CTRL_MAX_FUNC_NUM_TIMER_MOD_ACK_NAK_SHFT                                                                0x13
#define TIMER_CTRL_MAX_FUNC_NUM_TIMER_MOD_REPLAY_TIMER_MASK                                                           0x7C000
#define TIMER_CTRL_MAX_FUNC_NUM_TIMER_MOD_REPLAY_TIMER_SHFT                                                           0xE
#define TIMER_CTRL_MAX_FUNC_NUM_MAX_FUNC_NUM_MASK                                                                     0xFF
#define TIMER_CTRL_MAX_FUNC_NUM_MAX_FUNC_NUM_SHFT                                                                     0x0

/* TYPE0_TIMER_CTRL_MAX_FUNC_NUM */
#define TYPE0_TIMER_CTRL_MAX_FUNC_NUM                                                                                 ( 0x718 )
#define TYPE0_TIMER_CTRL_MAX_FUNC_NUM_UPDATE_FREQ_TIMER_MASK                                                          0x1F000000
#define TYPE0_TIMER_CTRL_MAX_FUNC_NUM_UPDATE_FREQ_TIMER_SHFT                                                          0x18
#define TYPE0_TIMER_CTRL_MAX_FUNC_NUM_TIMER_MOD_ACK_NAK_MASK                                                          0xF80000
#define TYPE0_TIMER_CTRL_MAX_FUNC_NUM_TIMER_MOD_ACK_NAK_SHFT                                                          0x13
#define TYPE0_TIMER_CTRL_MAX_FUNC_NUM_TIMER_MOD_REPLAY_TIMER_MASK                                                     0x7C000
#define TYPE0_TIMER_CTRL_MAX_FUNC_NUM_TIMER_MOD_REPLAY_TIMER_SHFT                                                     0xE
#define TYPE0_TIMER_CTRL_MAX_FUNC_NUM_MAX_FUNC_NUM_MASK                                                               0xFF
#define TYPE0_TIMER_CTRL_MAX_FUNC_NUM_MAX_FUNC_NUM_SHFT                                                               0x0

/* SYMBOL_TIMER_FILTER_1_REG */
#define SYMBOL_TIMER_FILTER_1_REG                                                                                     ( 0x71C )
#define SYMBOL_TIMER_FILTER_1_REG_MASK_RADM_1_MASK                                                                    0xFFFF0000
#define SYMBOL_TIMER_FILTER_1_REG_MASK_RADM_1_SHFT                                                                    0x10
#define SYMBOL_TIMER_FILTER_1_REG_DISABLE_FC_WD_TIMER_MASK                                                            0x8000
#define SYMBOL_TIMER_FILTER_1_REG_DISABLE_FC_WD_TIMER_SHFT                                                            0xF
#define SYMBOL_TIMER_FILTER_1_REG_EIDLE_TIMER_MASK                                                                    0x7800
#define SYMBOL_TIMER_FILTER_1_REG_EIDLE_TIMER_SHFT                                                                    0xB
#define SYMBOL_TIMER_FILTER_1_REG_SKP_INT_VAL_MASK                                                                    0x7FF
#define SYMBOL_TIMER_FILTER_1_REG_SKP_INT_VAL_SHFT                                                                    0x0

/* TYPE0_SYMBOL_TIMER_FILTER_1_REG */
#define TYPE0_SYMBOL_TIMER_FILTER_1_REG                                                                               ( 0x71C )
#define TYPE0_SYMBOL_TIMER_FILTER_1_REG_MASK_RADM_1_MASK                                                              0xFFFF0000
#define TYPE0_SYMBOL_TIMER_FILTER_1_REG_MASK_RADM_1_SHFT                                                              0x10
#define TYPE0_SYMBOL_TIMER_FILTER_1_REG_DISABLE_FC_WD_TIMER_MASK                                                      0x8000
#define TYPE0_SYMBOL_TIMER_FILTER_1_REG_DISABLE_FC_WD_TIMER_SHFT                                                      0xF
#define TYPE0_SYMBOL_TIMER_FILTER_1_REG_EIDLE_TIMER_MASK                                                              0x7800
#define TYPE0_SYMBOL_TIMER_FILTER_1_REG_EIDLE_TIMER_SHFT                                                              0xB
#define TYPE0_SYMBOL_TIMER_FILTER_1_REG_SKP_INT_VAL_MASK                                                              0x7FF
#define TYPE0_SYMBOL_TIMER_FILTER_1_REG_SKP_INT_VAL_SHFT                                                              0x0

/* FILTER_MASK_REG_2 */
#define FILTER_MASK_REG_2                                                                                             ( 0x720 )
#define FILTER_MASK_REG_2_MASK_RADM_2_MASK                                                                            0xFFFFFFFF
#define FILTER_MASK_REG_2_MASK_RADM_2_SHFT                                                                            0x0

/* TYPE0_FILTER_MASK_REG_2 */
#define TYPE0_FILTER_MASK_REG_2                                                                                       ( 0x720 )
#define TYPE0_FILTER_MASK_REG_2_MASK_RADM_2_MASK                                                                      0xFFFFFFFF
#define TYPE0_FILTER_MASK_REG_2_MASK_RADM_2_SHFT                                                                      0x0

/* AMBA_MUL_OB_DECOMP_NP_SUB_REQ_CTRL_REG */
#define AMBA_MUL_OB_DECOMP_NP_SUB_REQ_CTRL_REG                                                                        ( 0x724 )
#define AMBA_MUL_OB_DECOMP_NP_SUB_REQ_CTRL_REG_RSVDP_1_MASK                                                           0xFFFFFFFE
#define AMBA_MUL_OB_DECOMP_NP_SUB_REQ_CTRL_REG_RSVDP_1_SHFT                                                           0x1
#define AMBA_MUL_OB_DECOMP_NP_SUB_REQ_CTRL_REG_OB_RD_SPLIT_BURST_EN_MASK                                              0x1
#define AMBA_MUL_OB_DECOMP_NP_SUB_REQ_CTRL_REG_OB_RD_SPLIT_BURST_EN_SHFT                                              0x0

/* TYPE0_AMBA_MUL_OB_DECOMP_NP_SUB_REQ_CTRL_REG */
#define TYPE0_AMBA_MUL_OB_DECOMP_NP_SUB_REQ_CTRL_REG                                                                  ( 0x724 )
#define TYPE0_AMBA_MUL_OB_DECOMP_NP_SUB_REQ_CTRL_REG_RSVDP_1_MASK                                                     0xFFFFFFFE
#define TYPE0_AMBA_MUL_OB_DECOMP_NP_SUB_REQ_CTRL_REG_RSVDP_1_SHFT                                                     0x1
#define TYPE0_AMBA_MUL_OB_DECOMP_NP_SUB_REQ_CTRL_REG_OB_RD_SPLIT_BURST_EN_MASK                                        0x1
#define TYPE0_AMBA_MUL_OB_DECOMP_NP_SUB_REQ_CTRL_REG_OB_RD_SPLIT_BURST_EN_SHFT                                        0x0

/* DEBUG_REG_0 */
#define DEBUG_REG_0                                                                                                   ( 0x728 )
#define DEBUG_REG_0_DEB_REG_0_MASK                                                                                    0xFFFFFFFF
#define DEBUG_REG_0_DEB_REG_0_SHFT                                                                                    0x0

/* TYPE0_DEBUG_REG_0 */
#define TYPE0_DEBUG_REG_0                                                                                             ( 0x728 )
#define TYPE0_DEBUG_REG_0_DEB_REG_0_MASK                                                                              0xFFFFFFFF
#define TYPE0_DEBUG_REG_0_DEB_REG_0_SHFT                                                                              0x0

/* DEBUG_REG_1 */
#define DEBUG_REG_1                                                                                                   ( 0x72C )
#define DEBUG_REG_1_DEB_REG_1_MASK                                                                                    0xFFFFFFFF
#define DEBUG_REG_1_DEB_REG_1_SHFT                                                                                    0x0

/* TYPE0_DEBUG_REG_1 */
#define TYPE0_DEBUG_REG_1                                                                                             ( 0x72C )
#define TYPE0_DEBUG_REG_1_DEB_REG_1_MASK                                                                              0xFFFFFFFF
#define TYPE0_DEBUG_REG_1_DEB_REG_1_SHFT                                                                              0x0

/* TX_P_FC_CREDIT_STATUS */
#define TX_P_FC_CREDIT_STATUS                                                                                         ( 0x730 )
#define TX_P_FC_CREDIT_STATUS_TX_P_HEADER_FC_CREDIT_MASK                                                              0xFF000
#define TX_P_FC_CREDIT_STATUS_TX_P_HEADER_FC_CREDIT_SHFT                                                              0xC
#define TX_P_FC_CREDIT_STATUS_TX_P_DATA_FC_CREDIT_MASK                                                                0xFFF
#define TX_P_FC_CREDIT_STATUS_TX_P_DATA_FC_CREDIT_SHFT                                                                0x0

/* TYPE0_TX_P_FC_CREDIT_STATUS */
#define TYPE0_TX_P_FC_CREDIT_STATUS                                                                                   ( 0x730 )
#define TYPE0_TX_P_FC_CREDIT_STATUS_TX_P_HEADER_FC_CREDIT_MASK                                                        0xFF000
#define TYPE0_TX_P_FC_CREDIT_STATUS_TX_P_HEADER_FC_CREDIT_SHFT                                                        0xC
#define TYPE0_TX_P_FC_CREDIT_STATUS_TX_P_DATA_FC_CREDIT_MASK                                                          0xFFF
#define TYPE0_TX_P_FC_CREDIT_STATUS_TX_P_DATA_FC_CREDIT_SHFT                                                          0x0

/* TX_NP_FC_CREDIT_STATUS */
#define TX_NP_FC_CREDIT_STATUS                                                                                        ( 0x734 )
#define TX_NP_FC_CREDIT_STATUS_TX_NP_HEADER_FC_CREDIT_MASK                                                            0xFF000
#define TX_NP_FC_CREDIT_STATUS_TX_NP_HEADER_FC_CREDIT_SHFT                                                            0xC
#define TX_NP_FC_CREDIT_STATUS_TX_NP_DATA_FC_CREDIT_MASK                                                              0xFFF
#define TX_NP_FC_CREDIT_STATUS_TX_NP_DATA_FC_CREDIT_SHFT                                                              0x0

/* TYPE0_TX_NP_FC_CREDIT_STATUS */
#define TYPE0_TX_NP_FC_CREDIT_STATUS                                                                                  ( 0x734 )
#define TYPE0_TX_NP_FC_CREDIT_STATUS_TX_NP_HEADER_FC_CREDIT_MASK                                                      0xFF000
#define TYPE0_TX_NP_FC_CREDIT_STATUS_TX_NP_HEADER_FC_CREDIT_SHFT                                                      0xC
#define TYPE0_TX_NP_FC_CREDIT_STATUS_TX_NP_DATA_FC_CREDIT_MASK                                                        0xFFF
#define TYPE0_TX_NP_FC_CREDIT_STATUS_TX_NP_DATA_FC_CREDIT_SHFT                                                        0x0

/* TX_CPL_FC_CREDIT_STATUS */
#define TX_CPL_FC_CREDIT_STATUS                                                                                       ( 0x738 )
#define TX_CPL_FC_CREDIT_STATUS_TX_CPL_HEADER_FC_CREDIT_MASK                                                          0xFF000
#define TX_CPL_FC_CREDIT_STATUS_TX_CPL_HEADER_FC_CREDIT_SHFT                                                          0xC
#define TX_CPL_FC_CREDIT_STATUS_TX_CPL_DATA_FC_CREDIT_MASK                                                            0xFFF
#define TX_CPL_FC_CREDIT_STATUS_TX_CPL_DATA_FC_CREDIT_SHFT                                                            0x0

/* TYPE0_TX_CPL_FC_CREDIT_STATUS */
#define TYPE0_TX_CPL_FC_CREDIT_STATUS                                                                                 ( 0x738 )
#define TYPE0_TX_CPL_FC_CREDIT_STATUS_TX_CPL_HEADER_FC_CREDIT_MASK                                                    0xFF000
#define TYPE0_TX_CPL_FC_CREDIT_STATUS_TX_CPL_HEADER_FC_CREDIT_SHFT                                                    0xC
#define TYPE0_TX_CPL_FC_CREDIT_STATUS_TX_CPL_DATA_FC_CREDIT_MASK                                                      0xFFF
#define TYPE0_TX_CPL_FC_CREDIT_STATUS_TX_CPL_DATA_FC_CREDIT_SHFT                                                      0x0

/* Q_STATUS */
#define Q_STATUS                                                                                                      ( 0x73C )
#define Q_STATUS_TIMER_MOD_FLOW_CONTROL_EN_MASK                                                                       0x80000000
#define Q_STATUS_TIMER_MOD_FLOW_CONTROL_EN_SHFT                                                                       0x1F
#define Q_STATUS_TIMER_MOD_FLOW_CONTROL_MASK                                                                          0x1FFF0000
#define Q_STATUS_TIMER_MOD_FLOW_CONTROL_SHFT                                                                          0x10
#define Q_STATUS_RX_QUEUE_NON_EMPTY_MASK                                                                              0x4
#define Q_STATUS_RX_QUEUE_NON_EMPTY_SHFT                                                                              0x2
#define Q_STATUS_TX_RETRY_BUFFER_NE_MASK                                                                              0x2
#define Q_STATUS_TX_RETRY_BUFFER_NE_SHFT                                                                              0x1
#define Q_STATUS_RX_TLP_FC_CREDIT_NON_RETURN_MASK                                                                     0x1
#define Q_STATUS_RX_TLP_FC_CREDIT_NON_RETURN_SHFT                                                                     0x0

/* TYPE0_Q_STATUS */
#define TYPE0_Q_STATUS                                                                                                ( 0x73C )
#define TYPE0_Q_STATUS_TIMER_MOD_FLOW_CONTROL_EN_MASK                                                                 0x80000000
#define TYPE0_Q_STATUS_TIMER_MOD_FLOW_CONTROL_EN_SHFT                                                                 0x1F
#define TYPE0_Q_STATUS_TIMER_MOD_FLOW_CONTROL_MASK                                                                    0x1FFF0000
#define TYPE0_Q_STATUS_TIMER_MOD_FLOW_CONTROL_SHFT                                                                    0x10
#define TYPE0_Q_STATUS_RX_QUEUE_NON_EMPTY_MASK                                                                        0x4
#define TYPE0_Q_STATUS_RX_QUEUE_NON_EMPTY_SHFT                                                                        0x2
#define TYPE0_Q_STATUS_TX_RETRY_BUFFER_NE_MASK                                                                        0x2
#define TYPE0_Q_STATUS_TX_RETRY_BUFFER_NE_SHFT                                                                        0x1
#define TYPE0_Q_STATUS_RX_TLP_FC_CREDIT_NON_RETURN_MASK                                                               0x1
#define TYPE0_Q_STATUS_RX_TLP_FC_CREDIT_NON_RETURN_SHFT                                                               0x0

/* VC_TX_ARBI_REG_1 */
#define VC_TX_ARBI_REG_1                                                                                              ( 0x740 )
#define VC_TX_ARBI_REG_1_WRR_WEIGHT_VC_3_MASK                                                                         0xFF000000
#define VC_TX_ARBI_REG_1_WRR_WEIGHT_VC_3_SHFT                                                                         0x18
#define VC_TX_ARBI_REG_1_WRR_WEIGHT_VC_2_MASK                                                                         0xFF0000
#define VC_TX_ARBI_REG_1_WRR_WEIGHT_VC_2_SHFT                                                                         0x10
#define VC_TX_ARBI_REG_1_WRR_WEIGHT_VC_1_MASK                                                                         0xFF00
#define VC_TX_ARBI_REG_1_WRR_WEIGHT_VC_1_SHFT                                                                         0x8
#define VC_TX_ARBI_REG_1_WRR_WEIGHT_VC_0_MASK                                                                         0xFF
#define VC_TX_ARBI_REG_1_WRR_WEIGHT_VC_0_SHFT                                                                         0x0

/* TYPE0_VC_TX_ARBI_REG_1 */
#define TYPE0_VC_TX_ARBI_REG_1                                                                                        ( 0x740 )
#define TYPE0_VC_TX_ARBI_REG_1_WRR_WEIGHT_VC_3_MASK                                                                   0xFF000000
#define TYPE0_VC_TX_ARBI_REG_1_WRR_WEIGHT_VC_3_SHFT                                                                   0x18
#define TYPE0_VC_TX_ARBI_REG_1_WRR_WEIGHT_VC_2_MASK                                                                   0xFF0000
#define TYPE0_VC_TX_ARBI_REG_1_WRR_WEIGHT_VC_2_SHFT                                                                   0x10
#define TYPE0_VC_TX_ARBI_REG_1_WRR_WEIGHT_VC_1_MASK                                                                   0xFF00
#define TYPE0_VC_TX_ARBI_REG_1_WRR_WEIGHT_VC_1_SHFT                                                                   0x8
#define TYPE0_VC_TX_ARBI_REG_1_WRR_WEIGHT_VC_0_MASK                                                                   0xFF
#define TYPE0_VC_TX_ARBI_REG_1_WRR_WEIGHT_VC_0_SHFT                                                                   0x0

/* VC_TX_ARBI_REG_2 */
#define VC_TX_ARBI_REG_2                                                                                              ( 0x744 )
#define VC_TX_ARBI_REG_2_WRR_WEIGHT_VC_7_MASK                                                                         0xFF000000
#define VC_TX_ARBI_REG_2_WRR_WEIGHT_VC_7_SHFT                                                                         0x18
#define VC_TX_ARBI_REG_2_WRR_WEIGHT_VC_6_MASK                                                                         0xFF0000
#define VC_TX_ARBI_REG_2_WRR_WEIGHT_VC_6_SHFT                                                                         0x10
#define VC_TX_ARBI_REG_2_WRR_WEIGHT_VC_5_MASK                                                                         0xFF00
#define VC_TX_ARBI_REG_2_WRR_WEIGHT_VC_5_SHFT                                                                         0x8
#define VC_TX_ARBI_REG_2_WRR_WEIGHT_VC_4_MASK                                                                         0xFF
#define VC_TX_ARBI_REG_2_WRR_WEIGHT_VC_4_SHFT                                                                         0x0

/* TYPE0_VC_TX_ARBI_REG_2 */
#define TYPE0_VC_TX_ARBI_REG_2                                                                                        ( 0x744 )
#define TYPE0_VC_TX_ARBI_REG_2_WRR_WEIGHT_VC_7_MASK                                                                   0xFF000000
#define TYPE0_VC_TX_ARBI_REG_2_WRR_WEIGHT_VC_7_SHFT                                                                   0x18
#define TYPE0_VC_TX_ARBI_REG_2_WRR_WEIGHT_VC_6_MASK                                                                   0xFF0000
#define TYPE0_VC_TX_ARBI_REG_2_WRR_WEIGHT_VC_6_SHFT                                                                   0x10
#define TYPE0_VC_TX_ARBI_REG_2_WRR_WEIGHT_VC_5_MASK                                                                   0xFF00
#define TYPE0_VC_TX_ARBI_REG_2_WRR_WEIGHT_VC_5_SHFT                                                                   0x8
#define TYPE0_VC_TX_ARBI_REG_2_WRR_WEIGHT_VC_4_MASK                                                                   0xFF
#define TYPE0_VC_TX_ARBI_REG_2_WRR_WEIGHT_VC_4_SHFT                                                                   0x0

/* VC0_P_RX_Q_CTRL */
#define VC0_P_RX_Q_CTRL                                                                                               ( 0x748 )
#define VC0_P_RX_Q_CTRL_VC_ORDERING_RX_Q_MASK                                                                         0x80000000
#define VC0_P_RX_Q_CTRL_VC_ORDERING_RX_Q_SHFT                                                                         0x1F
#define VC0_P_RX_Q_CTRL_TLP_TYPE_ORDERING_VC0_MASK                                                                    0x40000000
#define VC0_P_RX_Q_CTRL_TLP_TYPE_ORDERING_VC0_SHFT                                                                    0x1E
#define VC0_P_RX_Q_CTRL_VC0_P_TLP_Q_MODE_MASK                                                                         0xE00000
#define VC0_P_RX_Q_CTRL_VC0_P_TLP_Q_MODE_SHFT                                                                         0x15
#define VC0_P_RX_Q_CTRL_VC0_P_HEADER_CREDIT_MASK                                                                      0xFF000
#define VC0_P_RX_Q_CTRL_VC0_P_HEADER_CREDIT_SHFT                                                                      0xC
#define VC0_P_RX_Q_CTRL_VC0_P_DATA_CREDIT_MASK                                                                        0xFFF
#define VC0_P_RX_Q_CTRL_VC0_P_DATA_CREDIT_SHFT                                                                        0x0

/* TYPE0_VC0_P_RX_Q_CTRL */
#define TYPE0_VC0_P_RX_Q_CTRL                                                                                         ( 0x748 )
#define TYPE0_VC0_P_RX_Q_CTRL_VC_ORDERING_RX_Q_MASK                                                                   0x80000000
#define TYPE0_VC0_P_RX_Q_CTRL_VC_ORDERING_RX_Q_SHFT                                                                   0x1F
#define TYPE0_VC0_P_RX_Q_CTRL_TLP_TYPE_ORDERING_VC0_MASK                                                              0x40000000
#define TYPE0_VC0_P_RX_Q_CTRL_TLP_TYPE_ORDERING_VC0_SHFT                                                              0x1E
#define TYPE0_VC0_P_RX_Q_CTRL_VC0_P_TLP_Q_MODE_MASK                                                                   0xE00000
#define TYPE0_VC0_P_RX_Q_CTRL_VC0_P_TLP_Q_MODE_SHFT                                                                   0x15
#define TYPE0_VC0_P_RX_Q_CTRL_VC0_P_HEADER_CREDIT_MASK                                                                0xFF000
#define TYPE0_VC0_P_RX_Q_CTRL_VC0_P_HEADER_CREDIT_SHFT                                                                0xC
#define TYPE0_VC0_P_RX_Q_CTRL_VC0_P_DATA_CREDIT_MASK                                                                  0xFFF
#define TYPE0_VC0_P_RX_Q_CTRL_VC0_P_DATA_CREDIT_SHFT                                                                  0x0

/* VC0_NP_RX_Q_CTRL */
#define VC0_NP_RX_Q_CTRL                                                                                              ( 0x74C )
#define VC0_NP_RX_Q_CTRL_VC0_NP_TLP_Q_MODE_MASK                                                                       0xE00000
#define VC0_NP_RX_Q_CTRL_VC0_NP_TLP_Q_MODE_SHFT                                                                       0x15
#define VC0_NP_RX_Q_CTRL_VC0_NP_HEADER_CREDIT_MASK                                                                    0xFF000
#define VC0_NP_RX_Q_CTRL_VC0_NP_HEADER_CREDIT_SHFT                                                                    0xC
#define VC0_NP_RX_Q_CTRL_VC0_NP_DATA_CREDIT_MASK                                                                      0xFFF
#define VC0_NP_RX_Q_CTRL_VC0_NP_DATA_CREDIT_SHFT                                                                      0x0

/* TYPE0_VC0_NP_RX_Q_CTRL */
#define TYPE0_VC0_NP_RX_Q_CTRL                                                                                        ( 0x74C )
#define TYPE0_VC0_NP_RX_Q_CTRL_VC0_NP_TLP_Q_MODE_MASK                                                                 0xE00000
#define TYPE0_VC0_NP_RX_Q_CTRL_VC0_NP_TLP_Q_MODE_SHFT                                                                 0x15
#define TYPE0_VC0_NP_RX_Q_CTRL_VC0_NP_HEADER_CREDIT_MASK                                                              0xFF000
#define TYPE0_VC0_NP_RX_Q_CTRL_VC0_NP_HEADER_CREDIT_SHFT                                                              0xC
#define TYPE0_VC0_NP_RX_Q_CTRL_VC0_NP_DATA_CREDIT_MASK                                                                0xFFF
#define TYPE0_VC0_NP_RX_Q_CTRL_VC0_NP_DATA_CREDIT_SHFT                                                                0x0

/* VC0_CPL_RX_Q_CTRL */
#define VC0_CPL_RX_Q_CTRL                                                                                             ( 0x750 )
#define VC0_CPL_RX_Q_CTRL_VC0_CPL_TLP_Q_MODE_MASK                                                                     0xE00000
#define VC0_CPL_RX_Q_CTRL_VC0_CPL_TLP_Q_MODE_SHFT                                                                     0x15
#define VC0_CPL_RX_Q_CTRL_VC0_CPL_HEADER_CREDIT_MASK                                                                  0xFF000
#define VC0_CPL_RX_Q_CTRL_VC0_CPL_HEADER_CREDIT_SHFT                                                                  0xC
#define VC0_CPL_RX_Q_CTRL_VC0_CPL_DATA_CREDIT_MASK                                                                    0xFFF
#define VC0_CPL_RX_Q_CTRL_VC0_CPL_DATA_CREDIT_SHFT                                                                    0x0

/* TYPE0_VC0_CPL_RX_Q_CTRL */
#define TYPE0_VC0_CPL_RX_Q_CTRL                                                                                       ( 0x750 )
#define TYPE0_VC0_CPL_RX_Q_CTRL_VC0_CPL_TLP_Q_MODE_MASK                                                               0xE00000
#define TYPE0_VC0_CPL_RX_Q_CTRL_VC0_CPL_TLP_Q_MODE_SHFT                                                               0x15
#define TYPE0_VC0_CPL_RX_Q_CTRL_VC0_CPL_HEADER_CREDIT_MASK                                                            0xFF000
#define TYPE0_VC0_CPL_RX_Q_CTRL_VC0_CPL_HEADER_CREDIT_SHFT                                                            0xC
#define TYPE0_VC0_CPL_RX_Q_CTRL_VC0_CPL_DATA_CREDIT_MASK                                                              0xFFF
#define TYPE0_VC0_CPL_RX_Q_CTRL_VC0_CPL_DATA_CREDIT_SHFT                                                              0x0

/* VC1_P_RX_Q_CTRL */
#define VC1_P_RX_Q_CTRL                                                                                               ( 0x754 )
#define VC1_P_RX_Q_CTRL_VC1_P_RX_Q_CTRL_MASK                                                                          0xFFFFFFFF
#define VC1_P_RX_Q_CTRL_VC1_P_RX_Q_CTRL_SHFT                                                                          0x0

/* TYPE0_VC1_P_RX_Q_CTRL */
#define TYPE0_VC1_P_RX_Q_CTRL                                                                                         ( 0x754 )
#define TYPE0_VC1_P_RX_Q_CTRL_VC1_P_RX_Q_CTRL_MASK                                                                    0xFFFFFFFF
#define TYPE0_VC1_P_RX_Q_CTRL_VC1_P_RX_Q_CTRL_SHFT                                                                    0x0

/* VC1_NP_RX_Q_CTRL */
#define VC1_NP_RX_Q_CTRL                                                                                              ( 0x758 )
#define VC1_NP_RX_Q_CTRL_VC1_NP_RX_Q_CTRL_MASK                                                                        0xFFFFFFFF
#define VC1_NP_RX_Q_CTRL_VC1_NP_RX_Q_CTRL_SHFT                                                                        0x0

/* TYPE0_VC1_NP_RX_Q_CTRL */
#define TYPE0_VC1_NP_RX_Q_CTRL                                                                                        ( 0x758 )
#define TYPE0_VC1_NP_RX_Q_CTRL_VC1_NP_RX_Q_CTRL_MASK                                                                  0xFFFFFFFF
#define TYPE0_VC1_NP_RX_Q_CTRL_VC1_NP_RX_Q_CTRL_SHFT                                                                  0x0

/* VC1_CPL_RX_Q_CTRL */
#define VC1_CPL_RX_Q_CTRL                                                                                             ( 0x75C )
#define VC1_CPL_RX_Q_CTRL_VC1_CPL_RX_Q_CTRL_MASK                                                                      0xFFFFFFFF
#define VC1_CPL_RX_Q_CTRL_VC1_CPL_RX_Q_CTRL_SHFT                                                                      0x0

/* TYPE0_VC1_CPL_RX_Q_CTRL */
#define TYPE0_VC1_CPL_RX_Q_CTRL                                                                                       ( 0x75C )
#define TYPE0_VC1_CPL_RX_Q_CTRL_VC1_CPL_RX_Q_CTRL_MASK                                                                0xFFFFFFFF
#define TYPE0_VC1_CPL_RX_Q_CTRL_VC1_CPL_RX_Q_CTRL_SHFT                                                                0x0

/* VC2_P_RX_Q_CTRL */
#define VC2_P_RX_Q_CTRL                                                                                               ( 0x760 )
#define VC2_P_RX_Q_CTRL_VC2_P_RX_Q_CTRL_MASK                                                                          0xFFFFFFFF
#define VC2_P_RX_Q_CTRL_VC2_P_RX_Q_CTRL_SHFT                                                                          0x0

/* TYPE0_VC2_P_RX_Q_CTRL */
#define TYPE0_VC2_P_RX_Q_CTRL                                                                                         ( 0x760 )
#define TYPE0_VC2_P_RX_Q_CTRL_VC2_P_RX_Q_CTRL_MASK                                                                    0xFFFFFFFF
#define TYPE0_VC2_P_RX_Q_CTRL_VC2_P_RX_Q_CTRL_SHFT                                                                    0x0

/* VC2_NP_RX_Q_CTRL */
#define VC2_NP_RX_Q_CTRL                                                                                              ( 0x764 )
#define VC2_NP_RX_Q_CTRL_VC2_NP_RX_Q_CTRL_MASK                                                                        0xFFFFFFFF
#define VC2_NP_RX_Q_CTRL_VC2_NP_RX_Q_CTRL_SHFT                                                                        0x0

/* TYPE0_VC2_NP_RX_Q_CTRL */
#define TYPE0_VC2_NP_RX_Q_CTRL                                                                                        ( 0x764 )
#define TYPE0_VC2_NP_RX_Q_CTRL_VC2_NP_RX_Q_CTRL_MASK                                                                  0xFFFFFFFF
#define TYPE0_VC2_NP_RX_Q_CTRL_VC2_NP_RX_Q_CTRL_SHFT                                                                  0x0

/* VC2_CPL_RX_Q_CTRL */
#define VC2_CPL_RX_Q_CTRL                                                                                             ( 0x768 )
#define VC2_CPL_RX_Q_CTRL_VC2_CPL_RX_Q_CTRL_MASK                                                                      0xFFFFFFFF
#define VC2_CPL_RX_Q_CTRL_VC2_CPL_RX_Q_CTRL_SHFT                                                                      0x0

/* TYPE0_VC2_CPL_RX_Q_CTRL */
#define TYPE0_VC2_CPL_RX_Q_CTRL                                                                                       ( 0x768 )
#define TYPE0_VC2_CPL_RX_Q_CTRL_VC2_CPL_RX_Q_CTRL_MASK                                                                0xFFFFFFFF
#define TYPE0_VC2_CPL_RX_Q_CTRL_VC2_CPL_RX_Q_CTRL_SHFT                                                                0x0

/* VC3_P_RX_Q_CTRL */
#define VC3_P_RX_Q_CTRL                                                                                               ( 0x76C )
#define VC3_P_RX_Q_CTRL_VC3_P_RX_Q_CTRL_MASK                                                                          0xFFFFFFFF
#define VC3_P_RX_Q_CTRL_VC3_P_RX_Q_CTRL_SHFT                                                                          0x0

/* TYPE0_VC3_P_RX_Q_CTRL */
#define TYPE0_VC3_P_RX_Q_CTRL                                                                                         ( 0x76C )
#define TYPE0_VC3_P_RX_Q_CTRL_VC3_P_RX_Q_CTRL_MASK                                                                    0xFFFFFFFF
#define TYPE0_VC3_P_RX_Q_CTRL_VC3_P_RX_Q_CTRL_SHFT                                                                    0x0

/* VC3_NP_RX_Q_CTRL */
#define VC3_NP_RX_Q_CTRL                                                                                              ( 0x770 )
#define VC3_NP_RX_Q_CTRL_VC3_NP_RX_Q_CTRL_MASK                                                                        0xFFFFFFFF
#define VC3_NP_RX_Q_CTRL_VC3_NP_RX_Q_CTRL_SHFT                                                                        0x0

/* TYPE0_VC3_NP_RX_Q_CTRL */
#define TYPE0_VC3_NP_RX_Q_CTRL                                                                                        ( 0x770 )
#define TYPE0_VC3_NP_RX_Q_CTRL_VC3_NP_RX_Q_CTRL_MASK                                                                  0xFFFFFFFF
#define TYPE0_VC3_NP_RX_Q_CTRL_VC3_NP_RX_Q_CTRL_SHFT                                                                  0x0

/* VC3_CPL_RX_Q_CTRL */
#define VC3_CPL_RX_Q_CTRL                                                                                             ( 0x774 )
#define VC3_CPL_RX_Q_CTRL_VC3_CPL_RX_Q_CTRL_MASK                                                                      0xFFFFFFFF
#define VC3_CPL_RX_Q_CTRL_VC3_CPL_RX_Q_CTRL_SHFT                                                                      0x0

/* TYPE0_VC3_CPL_RX_Q_CTRL */
#define TYPE0_VC3_CPL_RX_Q_CTRL                                                                                       ( 0x774 )
#define TYPE0_VC3_CPL_RX_Q_CTRL_VC3_CPL_RX_Q_CTRL_MASK                                                                0xFFFFFFFF
#define TYPE0_VC3_CPL_RX_Q_CTRL_VC3_CPL_RX_Q_CTRL_SHFT                                                                0x0

/* VC4_P_RX_Q_CTRL */
#define VC4_P_RX_Q_CTRL                                                                                               ( 0x778 )
#define VC4_P_RX_Q_CTRL_VC4_P_RX_Q_CTRL_MASK                                                                          0xFFFFFFFF
#define VC4_P_RX_Q_CTRL_VC4_P_RX_Q_CTRL_SHFT                                                                          0x0

/* TYPE0_VC4_P_RX_Q_CTRL */
#define TYPE0_VC4_P_RX_Q_CTRL                                                                                         ( 0x778 )
#define TYPE0_VC4_P_RX_Q_CTRL_VC4_P_RX_Q_CTRL_MASK                                                                    0xFFFFFFFF
#define TYPE0_VC4_P_RX_Q_CTRL_VC4_P_RX_Q_CTRL_SHFT                                                                    0x0

/* VC4_NP_RX_Q_CTRL */
#define VC4_NP_RX_Q_CTRL                                                                                              ( 0x77C )
#define VC4_NP_RX_Q_CTRL_VC4_NP_RX_Q_CTRL_MASK                                                                        0xFFFFFFFF
#define VC4_NP_RX_Q_CTRL_VC4_NP_RX_Q_CTRL_SHFT                                                                        0x0

/* TYPE0_VC4_NP_RX_Q_CTRL */
#define TYPE0_VC4_NP_RX_Q_CTRL                                                                                        ( 0x77C )
#define TYPE0_VC4_NP_RX_Q_CTRL_VC4_NP_RX_Q_CTRL_MASK                                                                  0xFFFFFFFF
#define TYPE0_VC4_NP_RX_Q_CTRL_VC4_NP_RX_Q_CTRL_SHFT                                                                  0x0

/* VC4_CPL_RX_Q_CTRL */
#define VC4_CPL_RX_Q_CTRL                                                                                             ( 0x780 )
#define VC4_CPL_RX_Q_CTRL_VC4_CPL_RX_Q_CTRL_MASK                                                                      0xFFFFFFFF
#define VC4_CPL_RX_Q_CTRL_VC4_CPL_RX_Q_CTRL_SHFT                                                                      0x0

/* TYPE0_VC4_CPL_RX_Q_CTRL */
#define TYPE0_VC4_CPL_RX_Q_CTRL                                                                                       ( 0x780 )
#define TYPE0_VC4_CPL_RX_Q_CTRL_VC4_CPL_RX_Q_CTRL_MASK                                                                0xFFFFFFFF
#define TYPE0_VC4_CPL_RX_Q_CTRL_VC4_CPL_RX_Q_CTRL_SHFT                                                                0x0

/* VC5_P_RX_Q_CTRL */
#define VC5_P_RX_Q_CTRL                                                                                               ( 0x784 )
#define VC5_P_RX_Q_CTRL_VC5_P_RX_Q_CTRL_MASK                                                                          0xFFFFFFFF
#define VC5_P_RX_Q_CTRL_VC5_P_RX_Q_CTRL_SHFT                                                                          0x0

/* TYPE0_VC5_P_RX_Q_CTRL */
#define TYPE0_VC5_P_RX_Q_CTRL                                                                                         ( 0x784 )
#define TYPE0_VC5_P_RX_Q_CTRL_VC5_P_RX_Q_CTRL_MASK                                                                    0xFFFFFFFF
#define TYPE0_VC5_P_RX_Q_CTRL_VC5_P_RX_Q_CTRL_SHFT                                                                    0x0

/* VC5_NP_RX_Q_CTRL */
#define VC5_NP_RX_Q_CTRL                                                                                              ( 0x788 )
#define VC5_NP_RX_Q_CTRL_VC5_NP_RX_Q_CTRL_MASK                                                                        0xFFFFFFFF
#define VC5_NP_RX_Q_CTRL_VC5_NP_RX_Q_CTRL_SHFT                                                                        0x0

/* TYPE0_VC5_NP_RX_Q_CTRL */
#define TYPE0_VC5_NP_RX_Q_CTRL                                                                                        ( 0x788 )
#define TYPE0_VC5_NP_RX_Q_CTRL_VC5_NP_RX_Q_CTRL_MASK                                                                  0xFFFFFFFF
#define TYPE0_VC5_NP_RX_Q_CTRL_VC5_NP_RX_Q_CTRL_SHFT                                                                  0x0

/* VC5_CPL_RX_Q_CTRL */
#define VC5_CPL_RX_Q_CTRL                                                                                             ( 0x78C )
#define VC5_CPL_RX_Q_CTRL_VC5_CPL_RX_Q_CTRL_MASK                                                                      0xFFFFFFFF
#define VC5_CPL_RX_Q_CTRL_VC5_CPL_RX_Q_CTRL_SHFT                                                                      0x0

/* TYPE0_VC5_CPL_RX_Q_CTRL */
#define TYPE0_VC5_CPL_RX_Q_CTRL                                                                                       ( 0x78C )
#define TYPE0_VC5_CPL_RX_Q_CTRL_VC5_CPL_RX_Q_CTRL_MASK                                                                0xFFFFFFFF
#define TYPE0_VC5_CPL_RX_Q_CTRL_VC5_CPL_RX_Q_CTRL_SHFT                                                                0x0

/* VC6_P_RX_Q_CTRL */
#define VC6_P_RX_Q_CTRL                                                                                               ( 0x790 )
#define VC6_P_RX_Q_CTRL_VC6_P_RX_Q_CTRL_MASK                                                                          0xFFFFFFFF
#define VC6_P_RX_Q_CTRL_VC6_P_RX_Q_CTRL_SHFT                                                                          0x0

/* TYPE0_VC6_P_RX_Q_CTRL */
#define TYPE0_VC6_P_RX_Q_CTRL                                                                                         ( 0x790 )
#define TYPE0_VC6_P_RX_Q_CTRL_VC6_P_RX_Q_CTRL_MASK                                                                    0xFFFFFFFF
#define TYPE0_VC6_P_RX_Q_CTRL_VC6_P_RX_Q_CTRL_SHFT                                                                    0x0

/* VC6_NP_RX_Q_CTRL */
#define VC6_NP_RX_Q_CTRL                                                                                              ( 0x794 )
#define VC6_NP_RX_Q_CTRL_VC6_NP_RX_Q_CTRL_MASK                                                                        0xFFFFFFFF
#define VC6_NP_RX_Q_CTRL_VC6_NP_RX_Q_CTRL_SHFT                                                                        0x0

/* TYPE0_VC6_NP_RX_Q_CTRL */
#define TYPE0_VC6_NP_RX_Q_CTRL                                                                                        ( 0x794 )
#define TYPE0_VC6_NP_RX_Q_CTRL_VC6_NP_RX_Q_CTRL_MASK                                                                  0xFFFFFFFF
#define TYPE0_VC6_NP_RX_Q_CTRL_VC6_NP_RX_Q_CTRL_SHFT                                                                  0x0

/* VC6_CPL_RX_Q_CTRL */
#define VC6_CPL_RX_Q_CTRL                                                                                             ( 0x798 )
#define VC6_CPL_RX_Q_CTRL_VC6_CPL_RX_Q_CTRL_MASK                                                                      0xFFFFFFFF
#define VC6_CPL_RX_Q_CTRL_VC6_CPL_RX_Q_CTRL_SHFT                                                                      0x0

/* TYPE0_VC6_CPL_RX_Q_CTRL */
#define TYPE0_VC6_CPL_RX_Q_CTRL                                                                                       ( 0x798 )
#define TYPE0_VC6_CPL_RX_Q_CTRL_VC6_CPL_RX_Q_CTRL_MASK                                                                0xFFFFFFFF
#define TYPE0_VC6_CPL_RX_Q_CTRL_VC6_CPL_RX_Q_CTRL_SHFT                                                                0x0

/* VC7_P_RX_Q_CTRL */
#define VC7_P_RX_Q_CTRL                                                                                               ( 0x79C )
#define VC7_P_RX_Q_CTRL_VC7_P_RX_Q_CTRL_MASK                                                                          0xFFFFFFFF
#define VC7_P_RX_Q_CTRL_VC7_P_RX_Q_CTRL_SHFT                                                                          0x0

/* TYPE0_VC7_P_RX_Q_CTRL */
#define TYPE0_VC7_P_RX_Q_CTRL                                                                                         ( 0x79C )
#define TYPE0_VC7_P_RX_Q_CTRL_VC7_P_RX_Q_CTRL_MASK                                                                    0xFFFFFFFF
#define TYPE0_VC7_P_RX_Q_CTRL_VC7_P_RX_Q_CTRL_SHFT                                                                    0x0

/* VC7_NP_RX_Q_CTRL */
#define VC7_NP_RX_Q_CTRL                                                                                              ( 0x7A0 )
#define VC7_NP_RX_Q_CTRL_VC7_NP_RX_Q_CTRL_MASK                                                                        0xFFFFFFFF
#define VC7_NP_RX_Q_CTRL_VC7_NP_RX_Q_CTRL_SHFT                                                                        0x0

/* TYPE0_VC7_NP_RX_Q_CTRL */
#define TYPE0_VC7_NP_RX_Q_CTRL                                                                                        ( 0x7A0 )
#define TYPE0_VC7_NP_RX_Q_CTRL_VC7_NP_RX_Q_CTRL_MASK                                                                  0xFFFFFFFF
#define TYPE0_VC7_NP_RX_Q_CTRL_VC7_NP_RX_Q_CTRL_SHFT                                                                  0x0

/* VC7_CPL_RX_Q_CTRL */
#define VC7_CPL_RX_Q_CTRL                                                                                             ( 0x7A4 )
#define VC7_CPL_RX_Q_CTRL_VC7_CPL_RX_Q_CTRL_MASK                                                                      0xFFFFFFFF
#define VC7_CPL_RX_Q_CTRL_VC7_CPL_RX_Q_CTRL_SHFT                                                                      0x0

/* TYPE0_VC7_CPL_RX_Q_CTRL */
#define TYPE0_VC7_CPL_RX_Q_CTRL                                                                                       ( 0x7A4 )
#define TYPE0_VC7_CPL_RX_Q_CTRL_VC7_CPL_RX_Q_CTRL_MASK                                                                0xFFFFFFFF
#define TYPE0_VC7_CPL_RX_Q_CTRL_VC7_CPL_RX_Q_CTRL_SHFT                                                                0x0

/* GEN2_CTRL_REG */
#define GEN2_CTRL_REG                                                                                                 ( 0x80C )
#define GEN2_CTRL_REG_SEL_DEEMPHASIS_MASK                                                                             0x100000
#define GEN2_CTRL_REG_SEL_DEEMPHASIS_SHFT                                                                             0x14
#define GEN2_CTRL_REG_CONFIG_TX_COMP_RX_MASK                                                                          0x80000
#define GEN2_CTRL_REG_CONFIG_TX_COMP_RX_SHFT                                                                          0x13
#define GEN2_CTRL_REG_CONFIG_PHY_TX_CHANGE_MASK                                                                       0x40000
#define GEN2_CTRL_REG_CONFIG_PHY_TX_CHANGE_SHFT                                                                       0x12
#define GEN2_CTRL_REG_DIRECT_SPEED_CHANGE_MASK                                                                        0x20000
#define GEN2_CTRL_REG_DIRECT_SPEED_CHANGE_SHFT                                                                        0x11
#define GEN2_CTRL_REG_NUM_OF_LANES_MASK                                                                               0x1FF00
#define GEN2_CTRL_REG_NUM_OF_LANES_SHFT                                                                               0x8
#define GEN2_CTRL_REG_FAST_TRAINING_SEQ_MASK                                                                          0xFF
#define GEN2_CTRL_REG_FAST_TRAINING_SEQ_SHFT                                                                          0x0

/* TYPE0_GEN2_CTRL_REG */
#define TYPE0_GEN2_CTRL_REG                                                                                           ( 0x80C )
#define TYPE0_GEN2_CTRL_REG_GEN1_EI_INFERENCE_MASK                                                                    0x200000
#define TYPE0_GEN2_CTRL_REG_GEN1_EI_INFERENCE_SHFT                                                                    0x15
#define TYPE0_GEN2_CTRL_REG_SEL_DEEMPHASIS_MASK                                                                       0x100000
#define TYPE0_GEN2_CTRL_REG_SEL_DEEMPHASIS_SHFT                                                                       0x14
#define TYPE0_GEN2_CTRL_REG_CONFIG_TX_COMP_RX_MASK                                                                    0x80000
#define TYPE0_GEN2_CTRL_REG_CONFIG_TX_COMP_RX_SHFT                                                                    0x13
#define TYPE0_GEN2_CTRL_REG_CONFIG_PHY_TX_CHANGE_MASK                                                                 0x40000
#define TYPE0_GEN2_CTRL_REG_CONFIG_PHY_TX_CHANGE_SHFT                                                                 0x12
#define TYPE0_GEN2_CTRL_REG_DIRECT_SPEED_CHANGE_MASK                                                                  0x20000
#define TYPE0_GEN2_CTRL_REG_DIRECT_SPEED_CHANGE_SHFT                                                                  0x11
#define TYPE0_GEN2_CTRL_REG_NUM_OF_LANES_MASK                                                                         0x1FF00
#define TYPE0_GEN2_CTRL_REG_NUM_OF_LANES_SHFT                                                                         0x8
#define TYPE0_GEN2_CTRL_REG_FAST_TRAINING_SEQ_MASK                                                                    0xFF
#define TYPE0_GEN2_CTRL_REG_FAST_TRAINING_SEQ_SHFT                                                                    0x0

/* PHY_STATUS_REG */
#define PHY_STATUS_REG                                                                                                ( 0x810 )
#define PHY_STATUS_REG_PHY_STATUS_MASK                                                                                0xFFFFFFFF
#define PHY_STATUS_REG_PHY_STATUS_SHFT                                                                                0x0

/* TYPE0_PHY_STATUS_REG */
#define TYPE0_PHY_STATUS_REG                                                                                          ( 0x810 )
#define TYPE0_PHY_STATUS_REG_PHY_STATUS_MASK                                                                          0xFFFFFFFF
#define TYPE0_PHY_STATUS_REG_PHY_STATUS_SHFT                                                                          0x0

/* PHY_CONTROL_REG */
#define PHY_CONTROL_REG                                                                                               ( 0x814 )
#define PHY_CONTROL_REG_PHY_CONTROL_MASK                                                                              0xFFFFFFFF
#define PHY_CONTROL_REG_PHY_CONTROL_SHFT                                                                              0x0

/* TYPE0_PHY_CONTROL_REG */
#define TYPE0_PHY_CONTROL_REG                                                                                         ( 0x814 )
#define TYPE0_PHY_CONTROL_REG_PHY_CONTROL_MASK                                                                        0xFFFFFFFF
#define TYPE0_PHY_CONTROL_REG_PHY_CONTROL_SHFT                                                                        0x0

/* AXI_MASTER_CTRL_REG_0 */
#define AXI_MASTER_CTRL_REG_0                                                                                         ( 0x818 )
#define AXI_MASTER_CTRL_REG_0_REMOTE_MAX_BRIDGE_TAG_MASK                                                              0xFF00
#define AXI_MASTER_CTRL_REG_0_REMOTE_MAX_BRIDGE_TAG_SHFT                                                              0x8
#define AXI_MASTER_CTRL_REG_0_REMOTE_READ_REQ_SIZE_MASK                                                               0x7
#define AXI_MASTER_CTRL_REG_0_REMOTE_READ_REQ_SIZE_SHFT                                                               0x0

/* TYPE0_AXI_MASTER_CTRL_REG_0 */
#define TYPE0_AXI_MASTER_CTRL_REG_0                                                                                   ( 0x818 )
#define TYPE0_AXI_MASTER_CTRL_REG_0_REMOTE_MAX_BRIDGE_TAG_MASK                                                        0xFF00
#define TYPE0_AXI_MASTER_CTRL_REG_0_REMOTE_MAX_BRIDGE_TAG_SHFT                                                        0x8
#define TYPE0_AXI_MASTER_CTRL_REG_0_REMOTE_READ_REQ_SIZE_MASK                                                         0x7
#define TYPE0_AXI_MASTER_CTRL_REG_0_REMOTE_READ_REQ_SIZE_SHFT                                                         0x0

/* AXI_MASTER_CTRL_REG_1 */
#define AXI_MASTER_CTRL_REG_1                                                                                         ( 0x81C )
#define AXI_MASTER_CTRL_REG_1_RESIZE_MASTER_RESPONSE_MASK                                                             0x1
#define AXI_MASTER_CTRL_REG_1_RESIZE_MASTER_RESPONSE_SHFT                                                             0x0

/* TYPE0_AXI_MASTER_CTRL_REG_1 */
#define TYPE0_AXI_MASTER_CTRL_REG_1                                                                                   ( 0x81C )
#define TYPE0_AXI_MASTER_CTRL_REG_1_RESIZE_MASTER_RESPONSE_MASK                                                       0x1
#define TYPE0_AXI_MASTER_CTRL_REG_1_RESIZE_MASTER_RESPONSE_SHFT                                                       0x0

/* MSI_CTRL_ADDR_REG */
#define MSI_CTRL_ADDR_REG                                                                                             ( 0x820 )
#define MSI_CTRL_ADDR_REG_MSI_CTRL_ADDR_MASK                                                                          0xFFFFFFFF
#define MSI_CTRL_ADDR_REG_MSI_CTRL_ADDR_SHFT                                                                          0x0

/* TYPE0_MSI_CTRL_ADDR_REG */
#define TYPE0_MSI_CTRL_ADDR_REG                                                                                       ( 0x820 )
#define TYPE0_MSI_CTRL_ADDR_REG_MSI_CTRL_ADDR_MASK                                                                    0xFFFFFFFF
#define TYPE0_MSI_CTRL_ADDR_REG_MSI_CTRL_ADDR_SHFT                                                                    0x0

/* MSI_CTRL_UPPER_ADDR_REG */
#define MSI_CTRL_UPPER_ADDR_REG                                                                                       ( 0x824 )
#define MSI_CTRL_UPPER_ADDR_REG_MSI_CTRL_UPPER_ADDR_MASK                                                              0xFFFFFFFF
#define MSI_CTRL_UPPER_ADDR_REG_MSI_CTRL_UPPER_ADDR_SHFT                                                              0x0

/* TYPE0_MSI_CTRL_UPPER_ADDR_REG */
#define TYPE0_MSI_CTRL_UPPER_ADDR_REG                                                                                 ( 0x824 )
#define TYPE0_MSI_CTRL_UPPER_ADDR_REG_MSI_CTRL_UPPER_ADDR_MASK                                                        0xFFFFFFFF
#define TYPE0_MSI_CTRL_UPPER_ADDR_REG_MSI_CTRL_UPPER_ADDR_SHFT                                                        0x0

/* MSI_CTRL_INT_0_EN_REG */
#define MSI_CTRL_INT_0_EN_REG                                                                                         ( 0x828 )
#define MSI_CTRL_INT_0_EN_REG_MSI_CTRL_INT_0_EN_MASK                                                                  0xFFFFFFFF
#define MSI_CTRL_INT_0_EN_REG_MSI_CTRL_INT_0_EN_SHFT                                                                  0x0

/* TYPE0_MSI_CTRL_INT_0_EN_REG */
#define TYPE0_MSI_CTRL_INT_0_EN_REG                                                                                   ( 0x828 )
#define TYPE0_MSI_CTRL_INT_0_EN_REG_MSI_CTRL_INT_0_EN_MASK                                                            0xFFFFFFFF
#define TYPE0_MSI_CTRL_INT_0_EN_REG_MSI_CTRL_INT_0_EN_SHFT                                                            0x0

/* MSI_CTRL_INT_0_MASK_REG */
#define MSI_CTRL_INT_0_MASK_REG                                                                                       ( 0x82C )
#define MSI_CTRL_INT_0_MASK_REG_MSI_CTRL_INT_0_MASK_MASK                                                              0xFFFFFFFF
#define MSI_CTRL_INT_0_MASK_REG_MSI_CTRL_INT_0_MASK_SHFT                                                              0x0

/* TYPE0_MSI_CTRL_INT_0_MASK_REG */
#define TYPE0_MSI_CTRL_INT_0_MASK_REG                                                                                 ( 0x82C )
#define TYPE0_MSI_CTRL_INT_0_MASK_REG_MSI_CTRL_INT_0_MASK_MASK                                                        0xFFFFFFFF
#define TYPE0_MSI_CTRL_INT_0_MASK_REG_MSI_CTRL_INT_0_MASK_SHFT                                                        0x0

/* MSI_CTRL_INT_0_STATUS_REG */
#define MSI_CTRL_INT_0_STATUS_REG                                                                                     ( 0x830 )
#define MSI_CTRL_INT_0_STATUS_REG_MSI_CTRL_INT_0_STATUS_MASK                                                          0xFFFFFFFF
#define MSI_CTRL_INT_0_STATUS_REG_MSI_CTRL_INT_0_STATUS_SHFT                                                          0x0

/* TYPE0_MSI_CTRL_INT_0_STATUS_REG */
#define TYPE0_MSI_CTRL_INT_0_STATUS_REG                                                                               ( 0x830 )
#define TYPE0_MSI_CTRL_INT_0_STATUS_REG_MSI_CTRL_INT_0_STATUS_MASK                                                    0xFFFFFFFF
#define TYPE0_MSI_CTRL_INT_0_STATUS_REG_MSI_CTRL_INT_0_STATUS_SHFT                                                    0x0

/* MSI_CTRL_INT_1_EN_REG */
#define MSI_CTRL_INT_1_EN_REG                                                                                         ( 0x834 )
#define MSI_CTRL_INT_1_EN_REG_MSI_CTRL_INT_1_EN_MASK                                                                  0xFFFFFFFF
#define MSI_CTRL_INT_1_EN_REG_MSI_CTRL_INT_1_EN_SHFT                                                                  0x0

/* TYPE0_MSI_CTRL_INT_1_EN_REG */
#define TYPE0_MSI_CTRL_INT_1_EN_REG                                                                                   ( 0x834 )
#define TYPE0_MSI_CTRL_INT_1_EN_REG_MSI_CTRL_INT_1_EN_MASK                                                            0xFFFFFFFF
#define TYPE0_MSI_CTRL_INT_1_EN_REG_MSI_CTRL_INT_1_EN_SHFT                                                            0x0

/* MSI_CTRL_INT_1_MASK_REG */
#define MSI_CTRL_INT_1_MASK_REG                                                                                       ( 0x838 )
#define MSI_CTRL_INT_1_MASK_REG_MSI_CTRL_INT_1_MASK_MASK                                                              0xFFFFFFFF
#define MSI_CTRL_INT_1_MASK_REG_MSI_CTRL_INT_1_MASK_SHFT                                                              0x0

/* TYPE0_MSI_CTRL_INT_1_MASK_REG */
#define TYPE0_MSI_CTRL_INT_1_MASK_REG                                                                                 ( 0x838 )
#define TYPE0_MSI_CTRL_INT_1_MASK_REG_MSI_CTRL_INT_1_MASK_MASK                                                        0xFFFFFFFF
#define TYPE0_MSI_CTRL_INT_1_MASK_REG_MSI_CTRL_INT_1_MASK_SHFT                                                        0x0

/* MSI_CTRL_INT_1_STATUS_REG */
#define MSI_CTRL_INT_1_STATUS_REG                                                                                     ( 0x83C )
#define MSI_CTRL_INT_1_STATUS_REG_MSI_CTRL_INT_1_STATUS_MASK                                                          0xFFFFFFFF
#define MSI_CTRL_INT_1_STATUS_REG_MSI_CTRL_INT_1_STATUS_SHFT                                                          0x0

/* TYPE0_MSI_CTRL_INT_1_STATUS_REG */
#define TYPE0_MSI_CTRL_INT_1_STATUS_REG                                                                               ( 0x83C )
#define TYPE0_MSI_CTRL_INT_1_STATUS_REG_MSI_CTRL_INT_1_STATUS_MASK                                                    0xFFFFFFFF
#define TYPE0_MSI_CTRL_INT_1_STATUS_REG_MSI_CTRL_INT_1_STATUS_SHFT                                                    0x0

/* MSI_CTRL_INT_2_EN_REG */
#define MSI_CTRL_INT_2_EN_REG                                                                                         ( 0x840 )
#define MSI_CTRL_INT_2_EN_REG_MSI_CTRL_INT_2_EN_MASK                                                                  0xFFFFFFFF
#define MSI_CTRL_INT_2_EN_REG_MSI_CTRL_INT_2_EN_SHFT                                                                  0x0

/* TYPE0_MSI_CTRL_INT_2_EN_REG */
#define TYPE0_MSI_CTRL_INT_2_EN_REG                                                                                   ( 0x840 )
#define TYPE0_MSI_CTRL_INT_2_EN_REG_MSI_CTRL_INT_2_EN_MASK                                                            0xFFFFFFFF
#define TYPE0_MSI_CTRL_INT_2_EN_REG_MSI_CTRL_INT_2_EN_SHFT                                                            0x0

/* MSI_CTRL_INT_2_MASK_REG */
#define MSI_CTRL_INT_2_MASK_REG                                                                                       ( 0x844 )
#define MSI_CTRL_INT_2_MASK_REG_MSI_CTRL_INT_2_MASK_MASK                                                              0xFFFFFFFF
#define MSI_CTRL_INT_2_MASK_REG_MSI_CTRL_INT_2_MASK_SHFT                                                              0x0

/* TYPE0_MSI_CTRL_INT_2_MASK_REG */
#define TYPE0_MSI_CTRL_INT_2_MASK_REG                                                                                 ( 0x844 )
#define TYPE0_MSI_CTRL_INT_2_MASK_REG_MSI_CTRL_INT_2_MASK_MASK                                                        0xFFFFFFFF
#define TYPE0_MSI_CTRL_INT_2_MASK_REG_MSI_CTRL_INT_2_MASK_SHFT                                                        0x0

/* MSI_CTRL_INT_2_STATUS_REG */
#define MSI_CTRL_INT_2_STATUS_REG                                                                                     ( 0x848 )
#define MSI_CTRL_INT_2_STATUS_REG_MSI_CTRL_INT_2_STATUS_MASK                                                          0xFFFFFFFF
#define MSI_CTRL_INT_2_STATUS_REG_MSI_CTRL_INT_2_STATUS_SHFT                                                          0x0

/* TYPE0_MSI_CTRL_INT_2_STATUS_REG */
#define TYPE0_MSI_CTRL_INT_2_STATUS_REG                                                                               ( 0x848 )
#define TYPE0_MSI_CTRL_INT_2_STATUS_REG_MSI_CTRL_INT_2_STATUS_MASK                                                    0xFFFFFFFF
#define TYPE0_MSI_CTRL_INT_2_STATUS_REG_MSI_CTRL_INT_2_STATUS_SHFT                                                    0x0

/* MSI_CTRL_INT_3_EN_REG */
#define MSI_CTRL_INT_3_EN_REG                                                                                         ( 0x84C )
#define MSI_CTRL_INT_3_EN_REG_MSI_CTRL_INT_3_EN_MASK                                                                  0xFFFFFFFF
#define MSI_CTRL_INT_3_EN_REG_MSI_CTRL_INT_3_EN_SHFT                                                                  0x0

/* TYPE0_MSI_CTRL_INT_3_EN_REG */
#define TYPE0_MSI_CTRL_INT_3_EN_REG                                                                                   ( 0x84C )
#define TYPE0_MSI_CTRL_INT_3_EN_REG_MSI_CTRL_INT_3_EN_MASK                                                            0xFFFFFFFF
#define TYPE0_MSI_CTRL_INT_3_EN_REG_MSI_CTRL_INT_3_EN_SHFT                                                            0x0

/* MSI_CTRL_INT_3_MASK_REG */
#define MSI_CTRL_INT_3_MASK_REG                                                                                       ( 0x850 )
#define MSI_CTRL_INT_3_MASK_REG_MSI_CTRL_INT_3_MASK_MASK                                                              0xFFFFFFFF
#define MSI_CTRL_INT_3_MASK_REG_MSI_CTRL_INT_3_MASK_SHFT                                                              0x0

/* TYPE0_MSI_CTRL_INT_3_MASK_REG */
#define TYPE0_MSI_CTRL_INT_3_MASK_REG                                                                                 ( 0x850 )
#define TYPE0_MSI_CTRL_INT_3_MASK_REG_MSI_CTRL_INT_3_MASK_MASK                                                        0xFFFFFFFF
#define TYPE0_MSI_CTRL_INT_3_MASK_REG_MSI_CTRL_INT_3_MASK_SHFT                                                        0x0

/* MSI_CTRL_INT_3_STATUS_REG */
#define MSI_CTRL_INT_3_STATUS_REG                                                                                     ( 0x854 )
#define MSI_CTRL_INT_3_STATUS_REG_MSI_CTRL_INT_3_STATUS_MASK                                                          0xFFFFFFFF
#define MSI_CTRL_INT_3_STATUS_REG_MSI_CTRL_INT_3_STATUS_SHFT                                                          0x0

/* TYPE0_MSI_CTRL_INT_3_STATUS_REG */
#define TYPE0_MSI_CTRL_INT_3_STATUS_REG                                                                               ( 0x854 )
#define TYPE0_MSI_CTRL_INT_3_STATUS_REG_MSI_CTRL_INT_3_STATUS_MASK                                                    0xFFFFFFFF
#define TYPE0_MSI_CTRL_INT_3_STATUS_REG_MSI_CTRL_INT_3_STATUS_SHFT                                                    0x0

/* MSI_CTRL_INT_4_EN_REG */
#define MSI_CTRL_INT_4_EN_REG                                                                                         ( 0x858 )
#define MSI_CTRL_INT_4_EN_REG_MSI_CTRL_INT_4_EN_MASK                                                                  0xFFFFFFFF
#define MSI_CTRL_INT_4_EN_REG_MSI_CTRL_INT_4_EN_SHFT                                                                  0x0

/* TYPE0_MSI_CTRL_INT_4_EN_REG */
#define TYPE0_MSI_CTRL_INT_4_EN_REG                                                                                   ( 0x858 )
#define TYPE0_MSI_CTRL_INT_4_EN_REG_MSI_CTRL_INT_4_EN_MASK                                                            0xFFFFFFFF
#define TYPE0_MSI_CTRL_INT_4_EN_REG_MSI_CTRL_INT_4_EN_SHFT                                                            0x0

/* MSI_CTRL_INT_4_MASK_REG */
#define MSI_CTRL_INT_4_MASK_REG                                                                                       ( 0x85C )
#define MSI_CTRL_INT_4_MASK_REG_MSI_CTRL_INT_4_MASK_MASK                                                              0xFFFFFFFF
#define MSI_CTRL_INT_4_MASK_REG_MSI_CTRL_INT_4_MASK_SHFT                                                              0x0

/* TYPE0_MSI_CTRL_INT_4_MASK_REG */
#define TYPE0_MSI_CTRL_INT_4_MASK_REG                                                                                 ( 0x85C )
#define TYPE0_MSI_CTRL_INT_4_MASK_REG_MSI_CTRL_INT_4_MASK_MASK                                                        0xFFFFFFFF
#define TYPE0_MSI_CTRL_INT_4_MASK_REG_MSI_CTRL_INT_4_MASK_SHFT                                                        0x0

/* MSI_CTRL_INT_4_STATUS_REG */
#define MSI_CTRL_INT_4_STATUS_REG                                                                                     ( 0x860 )
#define MSI_CTRL_INT_4_STATUS_REG_MSI_CTRL_INT_4_STATUS_MASK                                                          0xFFFFFFFF
#define MSI_CTRL_INT_4_STATUS_REG_MSI_CTRL_INT_4_STATUS_SHFT                                                          0x0

/* TYPE0_MSI_CTRL_INT_4_STATUS_REG */
#define TYPE0_MSI_CTRL_INT_4_STATUS_REG                                                                               ( 0x860 )
#define TYPE0_MSI_CTRL_INT_4_STATUS_REG_MSI_CTRL_INT_4_STATUS_MASK                                                    0xFFFFFFFF
#define TYPE0_MSI_CTRL_INT_4_STATUS_REG_MSI_CTRL_INT_4_STATUS_SHFT                                                    0x0

/* MSI_CTRL_INT_5_EN_REG */
#define MSI_CTRL_INT_5_EN_REG                                                                                         ( 0x864 )
#define MSI_CTRL_INT_5_EN_REG_MSI_CTRL_INT_5_EN_MASK                                                                  0xFFFFFFFF
#define MSI_CTRL_INT_5_EN_REG_MSI_CTRL_INT_5_EN_SHFT                                                                  0x0

/* TYPE0_MSI_CTRL_INT_5_EN_REG */
#define TYPE0_MSI_CTRL_INT_5_EN_REG                                                                                   ( 0x864 )
#define TYPE0_MSI_CTRL_INT_5_EN_REG_MSI_CTRL_INT_5_EN_MASK                                                            0xFFFFFFFF
#define TYPE0_MSI_CTRL_INT_5_EN_REG_MSI_CTRL_INT_5_EN_SHFT                                                            0x0

/* MSI_CTRL_INT_5_MASK_REG */
#define MSI_CTRL_INT_5_MASK_REG                                                                                       ( 0x868 )
#define MSI_CTRL_INT_5_MASK_REG_MSI_CTRL_INT_5_MASK_MASK                                                              0xFFFFFFFF
#define MSI_CTRL_INT_5_MASK_REG_MSI_CTRL_INT_5_MASK_SHFT                                                              0x0

/* TYPE0_MSI_CTRL_INT_5_MASK_REG */
#define TYPE0_MSI_CTRL_INT_5_MASK_REG                                                                                 ( 0x868 )
#define TYPE0_MSI_CTRL_INT_5_MASK_REG_MSI_CTRL_INT_5_MASK_MASK                                                        0xFFFFFFFF
#define TYPE0_MSI_CTRL_INT_5_MASK_REG_MSI_CTRL_INT_5_MASK_SHFT                                                        0x0

/* MSI_CTRL_INT_5_STATUS_REG */
#define MSI_CTRL_INT_5_STATUS_REG                                                                                     ( 0x86C )
#define MSI_CTRL_INT_5_STATUS_REG_MSI_CTRL_INT_5_STATUS_MASK                                                          0xFFFFFFFF
#define MSI_CTRL_INT_5_STATUS_REG_MSI_CTRL_INT_5_STATUS_SHFT                                                          0x0

/* TYPE0_MSI_CTRL_INT_5_STATUS_REG */
#define TYPE0_MSI_CTRL_INT_5_STATUS_REG                                                                               ( 0x86C )
#define TYPE0_MSI_CTRL_INT_5_STATUS_REG_MSI_CTRL_INT_5_STATUS_MASK                                                    0xFFFFFFFF
#define TYPE0_MSI_CTRL_INT_5_STATUS_REG_MSI_CTRL_INT_5_STATUS_SHFT                                                    0x0

/* MSI_CTRL_INT_6_EN_REG */
#define MSI_CTRL_INT_6_EN_REG                                                                                         ( 0x870 )
#define MSI_CTRL_INT_6_EN_REG_MSI_CTRL_INT_6_EN_MASK                                                                  0xFFFFFFFF
#define MSI_CTRL_INT_6_EN_REG_MSI_CTRL_INT_6_EN_SHFT                                                                  0x0

/* TYPE0_MSI_CTRL_INT_6_EN_REG */
#define TYPE0_MSI_CTRL_INT_6_EN_REG                                                                                   ( 0x870 )
#define TYPE0_MSI_CTRL_INT_6_EN_REG_MSI_CTRL_INT_6_EN_MASK                                                            0xFFFFFFFF
#define TYPE0_MSI_CTRL_INT_6_EN_REG_MSI_CTRL_INT_6_EN_SHFT                                                            0x0

/* MSI_CTRL_INT_6_MASK_REG */
#define MSI_CTRL_INT_6_MASK_REG                                                                                       ( 0x874 )
#define MSI_CTRL_INT_6_MASK_REG_MSI_CTRL_INT_6_MASK_MASK                                                              0xFFFFFFFF
#define MSI_CTRL_INT_6_MASK_REG_MSI_CTRL_INT_6_MASK_SHFT                                                              0x0

/* TYPE0_MSI_CTRL_INT_6_MASK_REG */
#define TYPE0_MSI_CTRL_INT_6_MASK_REG                                                                                 ( 0x874 )
#define TYPE0_MSI_CTRL_INT_6_MASK_REG_MSI_CTRL_INT_6_MASK_MASK                                                        0xFFFFFFFF
#define TYPE0_MSI_CTRL_INT_6_MASK_REG_MSI_CTRL_INT_6_MASK_SHFT                                                        0x0

/* MSI_CTRL_INT_6_STATUS_REG */
#define MSI_CTRL_INT_6_STATUS_REG                                                                                     ( 0x878 )
#define MSI_CTRL_INT_6_STATUS_REG_MSI_CTRL_INT_6_STATUS_MASK                                                          0xFFFFFFFF
#define MSI_CTRL_INT_6_STATUS_REG_MSI_CTRL_INT_6_STATUS_SHFT                                                          0x0

/* TYPE0_MSI_CTRL_INT_6_STATUS_REG */
#define TYPE0_MSI_CTRL_INT_6_STATUS_REG                                                                               ( 0x878 )
#define TYPE0_MSI_CTRL_INT_6_STATUS_REG_MSI_CTRL_INT_6_STATUS_MASK                                                    0xFFFFFFFF
#define TYPE0_MSI_CTRL_INT_6_STATUS_REG_MSI_CTRL_INT_6_STATUS_SHFT                                                    0x0

/* MSI_CTRL_INT_7_EN_REG */
#define MSI_CTRL_INT_7_EN_REG                                                                                         ( 0x87C )
#define MSI_CTRL_INT_7_EN_REG_MSI_CTRL_INT_7_EN_MASK                                                                  0xFFFFFFFF
#define MSI_CTRL_INT_7_EN_REG_MSI_CTRL_INT_7_EN_SHFT                                                                  0x0

/* TYPE0_MSI_CTRL_INT_7_EN_REG */
#define TYPE0_MSI_CTRL_INT_7_EN_REG                                                                                   ( 0x87C )
#define TYPE0_MSI_CTRL_INT_7_EN_REG_MSI_CTRL_INT_7_EN_MASK                                                            0xFFFFFFFF
#define TYPE0_MSI_CTRL_INT_7_EN_REG_MSI_CTRL_INT_7_EN_SHFT                                                            0x0

/* MSI_CTRL_INT_7_MASK_REG */
#define MSI_CTRL_INT_7_MASK_REG                                                                                       ( 0x880 )
#define MSI_CTRL_INT_7_MASK_REG_MSI_CTRL_INT_7_MASK_MASK                                                              0xFFFFFFFF
#define MSI_CTRL_INT_7_MASK_REG_MSI_CTRL_INT_7_MASK_SHFT                                                              0x0

/* TYPE0_MSI_CTRL_INT_7_MASK_REG */
#define TYPE0_MSI_CTRL_INT_7_MASK_REG                                                                                 ( 0x880 )
#define TYPE0_MSI_CTRL_INT_7_MASK_REG_MSI_CTRL_INT_7_MASK_MASK                                                        0xFFFFFFFF
#define TYPE0_MSI_CTRL_INT_7_MASK_REG_MSI_CTRL_INT_7_MASK_SHFT                                                        0x0

/* MSI_CTRL_INT_7_STATUS_REG */
#define MSI_CTRL_INT_7_STATUS_REG                                                                                     ( 0x884 )
#define MSI_CTRL_INT_7_STATUS_REG_MSI_CTRL_INT_7_STATUS_MASK                                                          0xFFFFFFFF
#define MSI_CTRL_INT_7_STATUS_REG_MSI_CTRL_INT_7_STATUS_SHFT                                                          0x0

/* TYPE0_MSI_CTRL_INT_7_STATUS_REG */
#define TYPE0_MSI_CTRL_INT_7_STATUS_REG                                                                               ( 0x884 )
#define TYPE0_MSI_CTRL_INT_7_STATUS_REG_MSI_CTRL_INT_7_STATUS_MASK                                                    0xFFFFFFFF
#define TYPE0_MSI_CTRL_INT_7_STATUS_REG_MSI_CTRL_INT_7_STATUS_SHFT                                                    0x0

/* MSI_GPIO_IO_REG */
#define MSI_GPIO_IO_REG                                                                                               ( 0x888 )
#define MSI_GPIO_IO_REG_MSI_GPIO_REG_MASK                                                                             0xFFFFFFFF
#define MSI_GPIO_IO_REG_MSI_GPIO_REG_SHFT                                                                             0x0

/* TYPE0_MSI_GPIO_IO_REG */
#define TYPE0_MSI_GPIO_IO_REG                                                                                         ( 0x888 )
#define TYPE0_MSI_GPIO_IO_REG_MSI_GPIO_REG_MASK                                                                       0xFFFFFFFF
#define TYPE0_MSI_GPIO_IO_REG_MSI_GPIO_REG_SHFT                                                                       0x0

/* GEN3_RELATED_REG */
#define GEN3_RELATED_REG                                                                                              ( 0x890 )
#define GEN3_RELATED_REG_GEN3_RELATED_REG_MASK                                                                        0xFFFFFFFF
#define GEN3_RELATED_REG_GEN3_RELATED_REG_SHFT                                                                        0x0

/* TYPE0_GEN3_RELATED_REG */
#define TYPE0_GEN3_RELATED_REG                                                                                        ( 0x890 )
#define TYPE0_GEN3_RELATED_REG_GEN3_RELATED_REG_MASK                                                                  0xFFFFFFFF
#define TYPE0_GEN3_RELATED_REG_GEN3_RELATED_REG_SHFT                                                                  0x0

/* GEN3_EQ_LOCAL_FS_LF_REG */
#define GEN3_EQ_LOCAL_FS_LF_REG                                                                                       ( 0x894 )
#define GEN3_EQ_LOCAL_FS_LF_REG_GEN3_EQ_LOCAL_FS_LF_REG_MASK                                                          0xFFFFFFFF
#define GEN3_EQ_LOCAL_FS_LF_REG_GEN3_EQ_LOCAL_FS_LF_REG_SHFT                                                          0x0

/* TYPE0_GEN3_EQ_LOCAL_FS_LF_REG */
#define TYPE0_GEN3_EQ_LOCAL_FS_LF_REG                                                                                 ( 0x894 )
#define TYPE0_GEN3_EQ_LOCAL_FS_LF_REG_GEN3_EQ_LOCAL_FS_LF_REG_MASK                                                    0xFFFFFFFF
#define TYPE0_GEN3_EQ_LOCAL_FS_LF_REG_GEN3_EQ_LOCAL_FS_LF_REG_SHFT                                                    0x0

/* GEN3_EQ_PSET_COEF_MAP_0 */
#define GEN3_EQ_PSET_COEF_MAP_0                                                                                       ( 0x898 )
#define GEN3_EQ_PSET_COEF_MAP_0_GEN3_EQ_PSET_COEF_MAP_0_MASK                                                          0xFFFFFFFF
#define GEN3_EQ_PSET_COEF_MAP_0_GEN3_EQ_PSET_COEF_MAP_0_SHFT                                                          0x0

/* TYPE0_GEN3_EQ_PSET_COEF_MAP_0 */
#define TYPE0_GEN3_EQ_PSET_COEF_MAP_0                                                                                 ( 0x898 )
#define TYPE0_GEN3_EQ_PSET_COEF_MAP_0_GEN3_EQ_PSET_COEF_MAP_0_MASK                                                    0xFFFFFFFF
#define TYPE0_GEN3_EQ_PSET_COEF_MAP_0_GEN3_EQ_PSET_COEF_MAP_0_SHFT                                                    0x0

/* GEN3_EQ_PSET_INDEX_REG */
#define GEN3_EQ_PSET_INDEX_REG                                                                                        ( 0x89C )
#define GEN3_EQ_PSET_INDEX_REG_GEN3_EQ_PSET_INDEX_REG_MASK                                                            0xFFFFFFFF
#define GEN3_EQ_PSET_INDEX_REG_GEN3_EQ_PSET_INDEX_REG_SHFT                                                            0x0

/* TYPE0_GEN3_EQ_PSET_INDEX_REG */
#define TYPE0_GEN3_EQ_PSET_INDEX_REG                                                                                  ( 0x89C )
#define TYPE0_GEN3_EQ_PSET_INDEX_REG_GEN3_EQ_PSET_INDEX_REG_MASK                                                      0xFFFFFFFF
#define TYPE0_GEN3_EQ_PSET_INDEX_REG_GEN3_EQ_PSET_INDEX_REG_SHFT                                                      0x0

/* DUMMY_TYPE1_PF_HIDDEN_REG */
#define DUMMY_TYPE1_PF_HIDDEN_REG                                                                                     ( 0x8A0 )
#define DUMMY_TYPE1_PF_HIDDEN_REG_PF_HIDDEN_REG_MASK                                                                  0xFFFFFFFF
#define DUMMY_TYPE1_PF_HIDDEN_REG_PF_HIDDEN_REG_SHFT                                                                  0x0

/* TYPE0_PF_HIDDEN_REG */
#define TYPE0_PF_HIDDEN_REG                                                                                           ( 0x8A0 )
#define TYPE0_PF_HIDDEN_REG_PF_HIDDEN_REG_MASK                                                                        0xFFFFFFFF
#define TYPE0_PF_HIDDEN_REG_PF_HIDDEN_REG_SHFT                                                                        0x0

/* GEN3_EQ_COEFF_LEGALITY_STATUS_REG */
#define GEN3_EQ_COEFF_LEGALITY_STATUS_REG                                                                             ( 0x8A4 )
#define GEN3_EQ_COEFF_LEGALITY_STATUS_REG_GEN3_EQ_COEFF_LEGALITY_STATUS_REG_MASK                                      0xFFFFFFFF
#define GEN3_EQ_COEFF_LEGALITY_STATUS_REG_GEN3_EQ_COEFF_LEGALITY_STATUS_REG_SHFT                                      0x0

/* TYPE0_GEN3_EQ_COEFF_LEGALITY_STATUS_REG */
#define TYPE0_GEN3_EQ_COEFF_LEGALITY_STATUS_REG                                                                       ( 0x8A4 )
#define TYPE0_GEN3_EQ_COEFF_LEGALITY_STATUS_REG_GEN3_EQ_COEFF_LEGALITY_STATUS_REG_MASK                                0xFFFFFFFF
#define TYPE0_GEN3_EQ_COEFF_LEGALITY_STATUS_REG_GEN3_EQ_COEFF_LEGALITY_STATUS_REG_SHFT                                0x0

/* GEN3_EQ_CONTROL_REG */
#define GEN3_EQ_CONTROL_REG                                                                                           ( 0x8A8 )
#define GEN3_EQ_CONTROL_REG_GEN3_EQ_CONTROL_REG_MASK                                                                  0xFFFFFFFF
#define GEN3_EQ_CONTROL_REG_GEN3_EQ_CONTROL_REG_SHFT                                                                  0x0

/* TYPE0_GEN3_EQ_CONTROL_REG */
#define TYPE0_GEN3_EQ_CONTROL_REG                                                                                     ( 0x8A8 )
#define TYPE0_GEN3_EQ_CONTROL_REG_GEN3_EQ_CONTROL_REG_MASK                                                            0xFFFFFFFF
#define TYPE0_GEN3_EQ_CONTROL_REG_GEN3_EQ_CONTROL_REG_SHFT                                                            0x0

/* GEN3_EQ_FB_MODE_DIR_CHANGE_REG */
#define GEN3_EQ_FB_MODE_DIR_CHANGE_REG                                                                                ( 0x8AC )
#define GEN3_EQ_FB_MODE_DIR_CHANGE_REG_GEN3_EQ_FB_MODE_DIR_CHANGE_REG_MASK                                            0xFFFFFFFF
#define GEN3_EQ_FB_MODE_DIR_CHANGE_REG_GEN3_EQ_FB_MODE_DIR_CHANGE_REG_SHFT                                            0x0

/* TYPE0_GEN3_EQ_FB_MODE_DIR_CHANGE_REG */
#define TYPE0_GEN3_EQ_FB_MODE_DIR_CHANGE_REG                                                                          ( 0x8AC )
#define TYPE0_GEN3_EQ_FB_MODE_DIR_CHANGE_REG_GEN3_EQ_FB_MODE_DIR_CHANGE_REG_MASK                                      0xFFFFFFFF
#define TYPE0_GEN3_EQ_FB_MODE_DIR_CHANGE_REG_GEN3_EQ_FB_MODE_DIR_CHANGE_REG_SHFT                                      0x0

/* PIPE_LOOPBACK_CONTROL */
#define PIPE_LOOPBACK_CONTROL                                                                                         ( 0x8B8 )
#define PIPE_LOOPBACK_CONTROL_PIPE_LOOPBACK_MASK                                                                      0x80000000
#define PIPE_LOOPBACK_CONTROL_PIPE_LOOPBACK_SHFT                                                                      0x1F
#define PIPE_LOOPBACK_CONTROL_RXSTATUS_VALUE_MASK                                                                     0x7000000
#define PIPE_LOOPBACK_CONTROL_RXSTATUS_VALUE_SHFT                                                                     0x18
#define PIPE_LOOPBACK_CONTROL_RXSTATUS_LANE_MASK                                                                      0x3F0000
#define PIPE_LOOPBACK_CONTROL_RXSTATUS_LANE_SHFT                                                                      0x10
#define PIPE_LOOPBACK_CONTROL_LPBK_RXVALID_MASK                                                                       0xFFFF
#define PIPE_LOOPBACK_CONTROL_LPBK_RXVALID_SHFT                                                                       0x0

/* TYPE0_PIPE_LOOPBACK_CONTROL */
#define TYPE0_PIPE_LOOPBACK_CONTROL                                                                                   ( 0x8B8 )
#define TYPE0_PIPE_LOOPBACK_CONTROL_PIPE_LOOPBACK_MASK                                                                0x80000000
#define TYPE0_PIPE_LOOPBACK_CONTROL_PIPE_LOOPBACK_SHFT                                                                0x1F
#define TYPE0_PIPE_LOOPBACK_CONTROL_RXSTATUS_VALUE_MASK                                                               0x7000000
#define TYPE0_PIPE_LOOPBACK_CONTROL_RXSTATUS_VALUE_SHFT                                                               0x18
#define TYPE0_PIPE_LOOPBACK_CONTROL_RXSTATUS_LANE_MASK                                                                0x3F0000
#define TYPE0_PIPE_LOOPBACK_CONTROL_RXSTATUS_LANE_SHFT                                                                0x10
#define TYPE0_PIPE_LOOPBACK_CONTROL_LPBK_RXVALID_MASK                                                                 0xFFFF
#define TYPE0_PIPE_LOOPBACK_CONTROL_LPBK_RXVALID_SHFT                                                                 0x0

/* MISC_CONTROL_1_REG */
#define MISC_CONTROL_1_REG                                                                                            ( 0x8BC )
#define MISC_CONTROL_1_REG_DBI_RO_WR_EN_MASK                                                                          0x1
#define MISC_CONTROL_1_REG_DBI_RO_WR_EN_SHFT                                                                          0x0

/* TYPE0_MISC_CONTROL_1_REG */
#define TYPE0_MISC_CONTROL_1_REG                                                                                      ( 0x8BC )
#define TYPE0_MISC_CONTROL_1_REG_DBI_RO_WR_EN_MASK                                                                    0x1
#define TYPE0_MISC_CONTROL_1_REG_DBI_RO_WR_EN_SHFT                                                                    0x0

/* LINK_FLUSH_CONTROL_REG */
#define LINK_FLUSH_CONTROL_REG                                                                                        ( 0x8CC )
#define LINK_FLUSH_CONTROL_REG_RSVD_I_8_MASK                                                                          0xFF000000
#define LINK_FLUSH_CONTROL_REG_RSVD_I_8_SHFT                                                                          0x18
#define LINK_FLUSH_CONTROL_REG_RSVDP_1_MASK                                                                           0xFFFFFE
#define LINK_FLUSH_CONTROL_REG_RSVDP_1_SHFT                                                                           0x1
#define LINK_FLUSH_CONTROL_REG_AUTO_FLUSH_EN_MASK                                                                     0x1
#define LINK_FLUSH_CONTROL_REG_AUTO_FLUSH_EN_SHFT                                                                     0x0

/* TYPE0_LINK_FLUSH_CONTROL_REG */
#define TYPE0_LINK_FLUSH_CONTROL_REG                                                                                  ( 0x8CC )
#define TYPE0_LINK_FLUSH_CONTROL_REG_RSVD_I_8_MASK                                                                    0xFF000000
#define TYPE0_LINK_FLUSH_CONTROL_REG_RSVD_I_8_SHFT                                                                    0x18
#define TYPE0_LINK_FLUSH_CONTROL_REG_RSVDP_1_MASK                                                                     0xFFFFFE
#define TYPE0_LINK_FLUSH_CONTROL_REG_RSVDP_1_SHFT                                                                     0x1
#define TYPE0_LINK_FLUSH_CONTROL_REG_AUTO_FLUSH_EN_MASK                                                               0x1
#define TYPE0_LINK_FLUSH_CONTROL_REG_AUTO_FLUSH_EN_SHFT                                                               0x0

/* AMBA_ERROR_RESPONSE_DEFAULT_REG */
#define AMBA_ERROR_RESPONSE_DEFAULT_REG                                                                               ( 0x8D0 )
#define AMBA_ERROR_RESPONSE_DEFAULT_REG_RSVDP_16_MASK                                                                 0xFFFF0000
#define AMBA_ERROR_RESPONSE_DEFAULT_REG_RSVDP_16_SHFT                                                                 0x10
#define AMBA_ERROR_RESPONSE_DEFAULT_REG_AMBA_ERROR_RESPONSE_MAP_MASK                                                  0xFC00
#define AMBA_ERROR_RESPONSE_DEFAULT_REG_AMBA_ERROR_RESPONSE_MAP_SHFT                                                  0xA
#define AMBA_ERROR_RESPONSE_DEFAULT_REG_RSVDP_3_MASK                                                                  0x3F8
#define AMBA_ERROR_RESPONSE_DEFAULT_REG_RSVDP_3_SHFT                                                                  0x3
#define AMBA_ERROR_RESPONSE_DEFAULT_REG_AMBA_ERROR_RESPONSE_VENDORID_MASK                                             0x4
#define AMBA_ERROR_RESPONSE_DEFAULT_REG_AMBA_ERROR_RESPONSE_VENDORID_SHFT                                             0x2
#define AMBA_ERROR_RESPONSE_DEFAULT_REG_RSVDP_1_MASK                                                                  0x2
#define AMBA_ERROR_RESPONSE_DEFAULT_REG_RSVDP_1_SHFT                                                                  0x1
#define AMBA_ERROR_RESPONSE_DEFAULT_REG_AMBA_ERROR_RESPONSE_GLOBAL_MASK                                               0x1
#define AMBA_ERROR_RESPONSE_DEFAULT_REG_AMBA_ERROR_RESPONSE_GLOBAL_SHFT                                               0x0

/* TYPE0_AMBA_ERROR_RESPONSE_DEFAULT_REG */
#define TYPE0_AMBA_ERROR_RESPONSE_DEFAULT_REG                                                                         ( 0x8D0 )
#define TYPE0_AMBA_ERROR_RESPONSE_DEFAULT_REG_RSVDP_16_MASK                                                           0xFFFF0000
#define TYPE0_AMBA_ERROR_RESPONSE_DEFAULT_REG_RSVDP_16_SHFT                                                           0x10
#define TYPE0_AMBA_ERROR_RESPONSE_DEFAULT_REG_AMBA_ERROR_RESPONSE_MAP_MASK                                            0xFC00
#define TYPE0_AMBA_ERROR_RESPONSE_DEFAULT_REG_AMBA_ERROR_RESPONSE_MAP_SHFT                                            0xA
#define TYPE0_AMBA_ERROR_RESPONSE_DEFAULT_REG_RSVDP_3_MASK                                                            0x3F8
#define TYPE0_AMBA_ERROR_RESPONSE_DEFAULT_REG_RSVDP_3_SHFT                                                            0x3
#define TYPE0_AMBA_ERROR_RESPONSE_DEFAULT_REG_AMBA_ERROR_RESPONSE_VENDORID_MASK                                       0x4
#define TYPE0_AMBA_ERROR_RESPONSE_DEFAULT_REG_AMBA_ERROR_RESPONSE_VENDORID_SHFT                                       0x2
#define TYPE0_AMBA_ERROR_RESPONSE_DEFAULT_REG_RSVDP_1_MASK                                                            0x2
#define TYPE0_AMBA_ERROR_RESPONSE_DEFAULT_REG_RSVDP_1_SHFT                                                            0x1
#define TYPE0_AMBA_ERROR_RESPONSE_DEFAULT_REG_AMBA_ERROR_RESPONSE_GLOBAL_MASK                                         0x1
#define TYPE0_AMBA_ERROR_RESPONSE_DEFAULT_REG_AMBA_ERROR_RESPONSE_GLOBAL_SHFT                                         0x0

/* AMBA_LINK_TIMEOUT_REG */
#define AMBA_LINK_TIMEOUT_REG                                                                                         ( 0x8D4 )
#define AMBA_LINK_TIMEOUT_REG_LINK_TIMEOUT_ENABLE_DEFAULT_MASK                                                        0x100
#define AMBA_LINK_TIMEOUT_REG_LINK_TIMEOUT_ENABLE_DEFAULT_SHFT                                                        0x8
#define AMBA_LINK_TIMEOUT_REG_LINK_TIMEOUT_PERIOD_DEFAULT_MASK                                                        0xFF
#define AMBA_LINK_TIMEOUT_REG_LINK_TIMEOUT_PERIOD_DEFAULT_SHFT                                                        0x0

/* TYPE0_DUMMY_AMBA_LINK_TIMEOUT_REG */
#define TYPE0_DUMMY_AMBA_LINK_TIMEOUT_REG                                                                             ( 0x8D4 )
#define TYPE0_DUMMY_AMBA_LINK_TIMEOUT_REG_LINK_TIMEOUT_ENABLE_DEFAULT_MASK                                            0x100
#define TYPE0_DUMMY_AMBA_LINK_TIMEOUT_REG_LINK_TIMEOUT_ENABLE_DEFAULT_SHFT                                            0x8
#define TYPE0_DUMMY_AMBA_LINK_TIMEOUT_REG_LINK_TIMEOUT_PERIOD_DEFAULT_MASK                                            0xFF
#define TYPE0_DUMMY_AMBA_LINK_TIMEOUT_REG_LINK_TIMEOUT_PERIOD_DEFAULT_SHFT                                            0x0

/* AMBA_ORDERING_CTRL_OFF */
#define AMBA_ORDERING_CTRL_OFF                                                                                        ( 0x8D8 )
#define AMBA_ORDERING_CTRL_OFF_RSVDP_2_MASK                                                                           0xFFFFFFFC
#define AMBA_ORDERING_CTRL_OFF_RSVDP_2_SHFT                                                                           0x2
#define AMBA_ORDERING_CTRL_OFF_AX_SNP_EN_MASK                                                                         0x2
#define AMBA_ORDERING_CTRL_OFF_AX_SNP_EN_SHFT                                                                         0x1
#define AMBA_ORDERING_CTRL_OFF_AX_MSTR_NP_PASS_P_MASK                                                                 0x1
#define AMBA_ORDERING_CTRL_OFF_AX_MSTR_NP_PASS_P_SHFT                                                                 0x0

/* TYPE0_AMBA_ORDERING_CTRL_OFF */
#define TYPE0_AMBA_ORDERING_CTRL_OFF                                                                                  ( 0x8D8 )
#define TYPE0_AMBA_ORDERING_CTRL_OFF_RSVDP_2_MASK                                                                     0xFFFFFFFC
#define TYPE0_AMBA_ORDERING_CTRL_OFF_RSVDP_2_SHFT                                                                     0x2
#define TYPE0_AMBA_ORDERING_CTRL_OFF_AX_SNP_EN_MASK                                                                   0x2
#define TYPE0_AMBA_ORDERING_CTRL_OFF_AX_SNP_EN_SHFT                                                                   0x1
#define TYPE0_AMBA_ORDERING_CTRL_OFF_AX_MSTR_NP_PASS_P_MASK                                                           0x1
#define TYPE0_AMBA_ORDERING_CTRL_OFF_AX_MSTR_NP_PASS_P_SHFT                                                           0x0

/* AMBA_ORDRMGR_WDOG_OFF */
#define AMBA_ORDRMGR_WDOG_OFF                                                                                         ( 0x8DC )
#define AMBA_ORDRMGR_WDOG_OFF_AMBA_ORDRMGR_WDOG_EN_MASK                                                               0x80000000
#define AMBA_ORDRMGR_WDOG_OFF_AMBA_ORDRMGR_WDOG_EN_SHFT                                                               0x1F
#define AMBA_ORDRMGR_WDOG_OFF_RSVDP_16_MASK                                                                           0x7FFF0000
#define AMBA_ORDRMGR_WDOG_OFF_RSVDP_16_SHFT                                                                           0x10
#define AMBA_ORDRMGR_WDOG_OFF_AMBA_ORDRMGR_WDOG_MASK                                                                  0xFFFF
#define AMBA_ORDRMGR_WDOG_OFF_AMBA_ORDRMGR_WDOG_SHFT                                                                  0x0

/* TYPE0_AMBA_ORDRMGR_WDOG_OFF */
#define TYPE0_AMBA_ORDRMGR_WDOG_OFF                                                                                   ( 0x8DC )
#define TYPE0_AMBA_ORDRMGR_WDOG_OFF_AMBA_ORDRMGR_WDOG_EN_MASK                                                         0x80000000
#define TYPE0_AMBA_ORDRMGR_WDOG_OFF_AMBA_ORDRMGR_WDOG_EN_SHFT                                                         0x1F
#define TYPE0_AMBA_ORDRMGR_WDOG_OFF_RSVDP_16_MASK                                                                     0x7FFF0000
#define TYPE0_AMBA_ORDRMGR_WDOG_OFF_RSVDP_16_SHFT                                                                     0x10
#define TYPE0_AMBA_ORDRMGR_WDOG_OFF_AMBA_ORDRMGR_WDOG_MASK                                                            0xFFFF
#define TYPE0_AMBA_ORDRMGR_WDOG_OFF_AMBA_ORDRMGR_WDOG_SHFT                                                            0x0

/* PL_LAST_REG */
#define PL_LAST_REG                                                                                                   ( 0x8FC )
#define PL_LAST_REG_PL_LAST_REG_MASK                                                                                  0xFFFFFFFF
#define PL_LAST_REG_PL_LAST_REG_SHFT                                                                                  0x0

/* TYPE0_PL_LAST_REG */
#define TYPE0_PL_LAST_REG                                                                                             ( 0x8FC )
#define TYPE0_PL_LAST_REG_PL_LAST_REG_MASK                                                                            0xFFFFFFFF
#define TYPE0_PL_LAST_REG_PL_LAST_REG_SHFT                                                                            0x0

/* IATU_VIEWPORT_REG */
#define IATU_VIEWPORT_REG                                                                                             ( 0x900 )
#define IATU_VIEWPORT_REG_REGION_DIR_MASK                                                                             0x80000000
#define IATU_VIEWPORT_REG_REGION_DIR_SHFT                                                                             0x1F
#define IATU_VIEWPORT_REG_VP_RSVD_MASK                                                                                0x7FFFFFE0
#define IATU_VIEWPORT_REG_VP_RSVD_SHFT                                                                                0x5
#define IATU_VIEWPORT_REG_REGION_INDEX_MASK                                                                           0x1F
#define IATU_VIEWPORT_REG_REGION_INDEX_SHFT                                                                           0x0

/* TYPE0_IATU_VIEWPORT_REG */
#define TYPE0_IATU_VIEWPORT_REG                                                                                       ( 0x900 )
#define TYPE0_IATU_VIEWPORT_REG_REGION_DIR_MASK                                                                       0x80000000
#define TYPE0_IATU_VIEWPORT_REG_REGION_DIR_SHFT                                                                       0x1F
#define TYPE0_IATU_VIEWPORT_REG_VP_RSVD_MASK                                                                          0x7FFFFFE0
#define TYPE0_IATU_VIEWPORT_REG_VP_RSVD_SHFT                                                                          0x5
#define TYPE0_IATU_VIEWPORT_REG_REGION_INDEX_MASK                                                                     0x1F
#define TYPE0_IATU_VIEWPORT_REG_REGION_INDEX_SHFT                                                                     0x0

/* PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0 */
#define PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0                                                                          ( 0x904 )
#define PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0_FUNC_NUM_MASK                                                            0x1F00000
#define PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0_FUNC_NUM_SHFT                                                            0x14
#define PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0_AT_MASK                                                                  0x30000
#define PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0_AT_SHFT                                                                  0x10
#define PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0_ATTR_MASK                                                                0x600
#define PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0_ATTR_SHFT                                                                0x9
#define PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0_TD_MASK                                                                  0x100
#define PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0_TD_SHFT                                                                  0x8
#define PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0_TC_MASK                                                                  0xE0
#define PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0_TC_SHFT                                                                  0x5
#define PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0_TYPE_MASK                                                                0x1F
#define PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0_TYPE_SHFT                                                                0x0

/* TYPE0_PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0 */
#define TYPE0_PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0                                                                    ( 0x904 )
#define TYPE0_PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0_FUNC_NUM_MASK                                                      0x1F00000
#define TYPE0_PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0_FUNC_NUM_SHFT                                                      0x14
#define TYPE0_PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0_AT_MASK                                                            0x30000
#define TYPE0_PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0_AT_SHFT                                                            0x10
#define TYPE0_PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0_ATTR_MASK                                                          0x600
#define TYPE0_PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0_ATTR_SHFT                                                          0x9
#define TYPE0_PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0_TD_MASK                                                            0x100
#define TYPE0_PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0_TD_SHFT                                                            0x8
#define TYPE0_PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0_TC_MASK                                                            0xE0
#define TYPE0_PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0_TC_SHFT                                                            0x5
#define TYPE0_PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0_TYPE_MASK                                                          0x1F
#define TYPE0_PL_IATU_REGION_CTRL_REG_1_OUTBOUND_0_TYPE_SHFT                                                          0x0

/* PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0 */
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0                                                                          ( 0x908 )
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_REGION_EN_MASK                                                           0x80000000
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_REGION_EN_SHFT                                                           0x1F
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_MATCH_MODE_MASK                                                          0x40000000
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_MATCH_MODE_SHFT                                                          0x1E
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_INVERT_MODE_MASK                                                         0x20000000
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_INVERT_MODE_SHFT                                                         0x1D
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_CFG_SHIFT_MODE_MASK                                                      0x10000000
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_CFG_SHIFT_MODE_SHFT                                                      0x1C
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_DMA_BYPASSS_MODE_MASK                                                    0x8000000
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_DMA_BYPASSS_MODE_SHFT                                                    0x1B
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_RESP_CODE_MASK                                                           0x3000000
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_RESP_CODE_SHFT                                                           0x18
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_MC_MATCH_EN_MASK                                                         0x200000
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_MC_MATCH_EN_SHFT                                                         0x15
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_VF_MATCH_EN_MASK                                                         0x100000
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_VF_MATCH_EN_SHFT                                                         0x14
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_FUNC_BYPASS_MASK                                                         0x80000
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_FUNC_BYPASS_SHFT                                                         0x13
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_AT_MATCH_EN_MASK                                                         0x40000
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_AT_MATCH_EN_SHFT                                                         0x12
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_ATTR_MATCH_EN_MASK                                                       0x10000
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_ATTR_MATCH_EN_SHFT                                                       0x10
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_TD_MATCH_EN_MASK                                                         0x8000
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_TD_MATCH_EN_SHFT                                                         0xF
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_TC_MATCH_EN_MASK                                                         0x4000
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_TC_MATCH_EN_SHFT                                                         0xE
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_BAR_NUMBER_MASK                                                          0x700
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_BAR_NUMBER_SHFT                                                          0x8
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_MSG_CODE_MASK                                                            0xFF
#define PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_MSG_CODE_SHFT                                                            0x0

/* TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0 */
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0                                                                    ( 0x908 )
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_REGION_EN_MASK                                                     0x80000000
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_REGION_EN_SHFT                                                     0x1F
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_MATCH_MODE_MASK                                                    0x40000000
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_MATCH_MODE_SHFT                                                    0x1E
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_INVERT_MODE_MASK                                                   0x20000000
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_INVERT_MODE_SHFT                                                   0x1D
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_CFG_SHIFT_MODE_MASK                                                0x10000000
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_CFG_SHIFT_MODE_SHFT                                                0x1C
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_DMA_BYPASSS_MODE_MASK                                              0x8000000
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_DMA_BYPASSS_MODE_SHFT                                              0x1B
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_RESP_CODE_MASK                                                     0x3000000
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_RESP_CODE_SHFT                                                     0x18
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_MC_MATCH_EN_MASK                                                   0x200000
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_MC_MATCH_EN_SHFT                                                   0x15
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_VF_MATCH_EN_MASK                                                   0x100000
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_VF_MATCH_EN_SHFT                                                   0x14
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_FUNC_BYPASS_MASK                                                   0x80000
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_FUNC_BYPASS_SHFT                                                   0x13
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_AT_MATCH_EN_MASK                                                   0x40000
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_AT_MATCH_EN_SHFT                                                   0x12
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_ATTR_MATCH_EN_MASK                                                 0x10000
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_ATTR_MATCH_EN_SHFT                                                 0x10
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_TD_MATCH_EN_MASK                                                   0x8000
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_TD_MATCH_EN_SHFT                                                   0xF
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_TC_MATCH_EN_MASK                                                   0x4000
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_TC_MATCH_EN_SHFT                                                   0xE
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_BAR_NUMBER_MASK                                                    0x700
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_BAR_NUMBER_SHFT                                                    0x8
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_MSG_CODE_MASK                                                      0xFF
#define TYPE0_PL_IATU_REGION_CTRL_REG_2_OUTBOUND_0_MSG_CODE_SHFT                                                      0x0

/* PL_IATU_LWR_BASE_ADDR_REG_OUTBOUND_0 */
#define PL_IATU_LWR_BASE_ADDR_REG_OUTBOUND_0                                                                          ( 0x90C )
#define PL_IATU_LWR_BASE_ADDR_REG_OUTBOUND_0_LWR_BASE_RW_MASK                                                         0xFFFFF000
#define PL_IATU_LWR_BASE_ADDR_REG_OUTBOUND_0_LWR_BASE_RW_SHFT                                                         0xC
#define PL_IATU_LWR_BASE_ADDR_REG_OUTBOUND_0_LWR_BASE_HW_MASK                                                         0xFFF
#define PL_IATU_LWR_BASE_ADDR_REG_OUTBOUND_0_LWR_BASE_HW_SHFT                                                         0x0

/* TYPE0_PL_IATU_LWR_BASE_ADDR_REG_OUTBOUND_0 */
#define TYPE0_PL_IATU_LWR_BASE_ADDR_REG_OUTBOUND_0                                                                    ( 0x90C )
#define TYPE0_PL_IATU_LWR_BASE_ADDR_REG_OUTBOUND_0_LWR_BASE_RW_MASK                                                   0xFFFFF000
#define TYPE0_PL_IATU_LWR_BASE_ADDR_REG_OUTBOUND_0_LWR_BASE_RW_SHFT                                                   0xC
#define TYPE0_PL_IATU_LWR_BASE_ADDR_REG_OUTBOUND_0_LWR_BASE_HW_MASK                                                   0xFFF
#define TYPE0_PL_IATU_LWR_BASE_ADDR_REG_OUTBOUND_0_LWR_BASE_HW_SHFT                                                   0x0

/* PL_IATU_UPPER_BASE_ADDR_REG_OUTBOUND_0 */
#define PL_IATU_UPPER_BASE_ADDR_REG_OUTBOUND_0                                                                        ( 0x910 )
#define PL_IATU_UPPER_BASE_ADDR_REG_OUTBOUND_0_UPPER_BASE_RW_MASK                                                     0xFFFFFFFF
#define PL_IATU_UPPER_BASE_ADDR_REG_OUTBOUND_0_UPPER_BASE_RW_SHFT                                                     0x0

/* TYPE0_PL_IATU_UPPER_BASE_ADDR_REG_OUTBOUND_0 */
#define TYPE0_PL_IATU_UPPER_BASE_ADDR_REG_OUTBOUND_0                                                                  ( 0x910 )
#define TYPE0_PL_IATU_UPPER_BASE_ADDR_REG_OUTBOUND_0_UPPER_BASE_RW_MASK                                               0xFFFFFFFF
#define TYPE0_PL_IATU_UPPER_BASE_ADDR_REG_OUTBOUND_0_UPPER_BASE_RW_SHFT                                               0x0

/* PL_IATU_LIMIT_ADDR_REG_OUTBOUND_0 */
#define PL_IATU_LIMIT_ADDR_REG_OUTBOUND_0                                                                             ( 0x914 )
#define PL_IATU_LIMIT_ADDR_REG_OUTBOUND_0_LIMIT_ADDR_HW_MASK                                                          0xFFFFFFFF
#define PL_IATU_LIMIT_ADDR_REG_OUTBOUND_0_LIMIT_ADDR_HW_SHFT                                                          0x0

/* TYPE0_PL_IATU_LIMIT_ADDR_REG_OUTBOUND_0 */
#define TYPE0_PL_IATU_LIMIT_ADDR_REG_OUTBOUND_0                                                                       ( 0x914 )
#define TYPE0_PL_IATU_LIMIT_ADDR_REG_OUTBOUND_0_LIMIT_ADDR_HW_MASK                                                    0xFFFFFFFF
#define TYPE0_PL_IATU_LIMIT_ADDR_REG_OUTBOUND_0_LIMIT_ADDR_HW_SHFT                                                    0x0

/* PL_IATU_LWR_TARGET_ADDR_REG_OUTBOUND_0 */
#define PL_IATU_LWR_TARGET_ADDR_REG_OUTBOUND_0                                                                        ( 0x918 )
#define PL_IATU_LWR_TARGET_ADDR_REG_OUTBOUND_0_LWR_TARGET_RW_MASK                                                     0xFFFFF000
#define PL_IATU_LWR_TARGET_ADDR_REG_OUTBOUND_0_LWR_TARGET_RW_SHFT                                                     0xC
#define PL_IATU_LWR_TARGET_ADDR_REG_OUTBOUND_0_LWR_TARGET_HW_MASK                                                     0xFFF
#define PL_IATU_LWR_TARGET_ADDR_REG_OUTBOUND_0_LWR_TARGET_HW_SHFT                                                     0x0

/* TYPE0_PL_IATU_LWR_TARGET_ADDR_REG_OUTBOUND_0 */
#define TYPE0_PL_IATU_LWR_TARGET_ADDR_REG_OUTBOUND_0                                                                  ( 0x918 )
#define TYPE0_PL_IATU_LWR_TARGET_ADDR_REG_OUTBOUND_0_LWR_TARGET_RW_MASK                                               0xFFFFF000
#define TYPE0_PL_IATU_LWR_TARGET_ADDR_REG_OUTBOUND_0_LWR_TARGET_RW_SHFT                                               0xC
#define TYPE0_PL_IATU_LWR_TARGET_ADDR_REG_OUTBOUND_0_LWR_TARGET_HW_MASK                                               0xFFF
#define TYPE0_PL_IATU_LWR_TARGET_ADDR_REG_OUTBOUND_0_LWR_TARGET_HW_SHFT                                               0x0

/* PL_IATU_UPPER_TARGET_ADDR_REG_OUTBOUND_0 */
#define PL_IATU_UPPER_TARGET_ADDR_REG_OUTBOUND_0                                                                      ( 0x91C )
#define PL_IATU_UPPER_TARGET_ADDR_REG_OUTBOUND_0_UPPER_TARGET_RW_MASK                                                 0xFFFFFFFF
#define PL_IATU_UPPER_TARGET_ADDR_REG_OUTBOUND_0_UPPER_TARGET_RW_SHFT                                                 0x0

/* TYPE0_PL_IATU_UPPER_TARGET_ADDR_REG_OUTBOUND_0 */
#define TYPE0_PL_IATU_UPPER_TARGET_ADDR_REG_OUTBOUND_0                                                                ( 0x91C )
#define TYPE0_PL_IATU_UPPER_TARGET_ADDR_REG_OUTBOUND_0_UPPER_TARGET_RW_MASK                                           0xFFFFFFFF
#define TYPE0_PL_IATU_UPPER_TARGET_ADDR_REG_OUTBOUND_0_UPPER_TARGET_RW_SHFT                                           0x0

/* PL_IATU_REGION_CTRL_REG_3_OUTBOUND_0 */
#define PL_IATU_REGION_CTRL_REG_3_OUTBOUND_0                                                                          ( 0x920 )
#define PL_IATU_REGION_CTRL_REG_3_OUTBOUND_0_VF_ACTIVE_MASK                                                           0x80000000
#define PL_IATU_REGION_CTRL_REG_3_OUTBOUND_0_VF_ACTIVE_SHFT                                                           0x1F
#define PL_IATU_REGION_CTRL_REG_3_OUTBOUND_0_VF_NUMBER_MASK                                                           0x1
#define PL_IATU_REGION_CTRL_REG_3_OUTBOUND_0_VF_NUMBER_SHFT                                                           0x0

/* TYPE0_PL_IATU_REGION_CTRL_REG_3_OUTBOUND_0 */
#define TYPE0_PL_IATU_REGION_CTRL_REG_3_OUTBOUND_0                                                                    ( 0x920 )
#define TYPE0_PL_IATU_REGION_CTRL_REG_3_OUTBOUND_0_VF_ACTIVE_MASK                                                     0x80000000
#define TYPE0_PL_IATU_REGION_CTRL_REG_3_OUTBOUND_0_VF_ACTIVE_SHFT                                                     0x1F
#define TYPE0_PL_IATU_REGION_CTRL_REG_3_OUTBOUND_0_VF_NUMBER_MASK                                                     0x1
#define TYPE0_PL_IATU_REGION_CTRL_REG_3_OUTBOUND_0_VF_NUMBER_SHFT                                                     0x0

/* DMA_CTRL_REG */
#define DMA_CTRL_REG                                                                                                  ( 0x978 )
#define DMA_CTRL_REG_NUM_DMA_RD_CHAN_MASK                                                                             0xF0000
#define DMA_CTRL_REG_NUM_DMA_RD_CHAN_SHFT                                                                             0x10
#define DMA_CTRL_REG_NUM_DMA_WR_CHAN_MASK                                                                             0xF
#define DMA_CTRL_REG_NUM_DMA_WR_CHAN_SHFT                                                                             0x0

/* TYPE0_DMA_CTRL_REG */
#define TYPE0_DMA_CTRL_REG                                                                                            ( 0x978 )
#define TYPE0_DMA_CTRL_REG_NUM_DMA_RD_CHAN_MASK                                                                       0xF0000
#define TYPE0_DMA_CTRL_REG_NUM_DMA_RD_CHAN_SHFT                                                                       0x10
#define TYPE0_DMA_CTRL_REG_NUM_DMA_WR_CHAN_MASK                                                                       0xF
#define TYPE0_DMA_CTRL_REG_NUM_DMA_WR_CHAN_SHFT                                                                       0x0

/* DMA_WRITE_ENGINE_EN */
#define DMA_WRITE_ENGINE_EN                                                                                           ( 0x97C )
#define DMA_WRITE_ENGINE_EN_DMA_WRITE_ENGINE_MASK                                                                     0x1
#define DMA_WRITE_ENGINE_EN_DMA_WRITE_ENGINE_SHFT                                                                     0x0

/* TYPE0_DMA_WRITE_ENGINE_EN */
#define TYPE0_DMA_WRITE_ENGINE_EN                                                                                     ( 0x97C )
#define TYPE0_DMA_WRITE_ENGINE_EN_DMA_WRITE_ENGINE_MASK                                                               0x1
#define TYPE0_DMA_WRITE_ENGINE_EN_DMA_WRITE_ENGINE_SHFT                                                               0x0

/* DMA_WRITE_DOORBELL_REG */
#define DMA_WRITE_DOORBELL_REG                                                                                        ( 0x980 )
#define DMA_WRITE_DOORBELL_REG_WR_STOP_MASK                                                                           0x80000000
#define DMA_WRITE_DOORBELL_REG_WR_STOP_SHFT                                                                           0x1F
#define DMA_WRITE_DOORBELL_REG_WR_DOORBELL_NUM_MASK                                                                   0x7
#define DMA_WRITE_DOORBELL_REG_WR_DOORBELL_NUM_SHFT                                                                   0x0

/* TYPE0_DMA_WRITE_DOORBELL_REG */
#define TYPE0_DMA_WRITE_DOORBELL_REG                                                                                  ( 0x980 )
#define TYPE0_DMA_WRITE_DOORBELL_REG_WR_STOP_MASK                                                                     0x80000000
#define TYPE0_DMA_WRITE_DOORBELL_REG_WR_STOP_SHFT                                                                     0x1F
#define TYPE0_DMA_WRITE_DOORBELL_REG_WR_DOORBELL_NUM_MASK                                                             0x7
#define TYPE0_DMA_WRITE_DOORBELL_REG_WR_DOORBELL_NUM_SHFT                                                             0x0

/* DMA_WRITE_CHANNEL_ARB_WEIGHT_LOW_REG */
#define DMA_WRITE_CHANNEL_ARB_WEIGHT_LOW_REG                                                                          ( 0x988 )
#define DMA_WRITE_CHANNEL_ARB_WEIGHT_LOW_REG_WRITE_CHANNEL3_WEIGHT_MASK                                               0xF8000
#define DMA_WRITE_CHANNEL_ARB_WEIGHT_LOW_REG_WRITE_CHANNEL3_WEIGHT_SHFT                                               0xF
#define DMA_WRITE_CHANNEL_ARB_WEIGHT_LOW_REG_WRITE_CHANNEL2_WEIGHT_MASK                                               0x7C00
#define DMA_WRITE_CHANNEL_ARB_WEIGHT_LOW_REG_WRITE_CHANNEL2_WEIGHT_SHFT                                               0xA
#define DMA_WRITE_CHANNEL_ARB_WEIGHT_LOW_REG_WRITE_CHANNEL1_WEIGHT_MASK                                               0x3E0
#define DMA_WRITE_CHANNEL_ARB_WEIGHT_LOW_REG_WRITE_CHANNEL1_WEIGHT_SHFT                                               0x5
#define DMA_WRITE_CHANNEL_ARB_WEIGHT_LOW_REG_WRITE_CHANNEL0_WEIGHT_MASK                                               0x1F
#define DMA_WRITE_CHANNEL_ARB_WEIGHT_LOW_REG_WRITE_CHANNEL0_WEIGHT_SHFT                                               0x0

/* TYPE0_DMA_WRITE_CHANNEL_ARB_WEIGHT_LOW_REG */
#define TYPE0_DMA_WRITE_CHANNEL_ARB_WEIGHT_LOW_REG                                                                    ( 0x988 )
#define TYPE0_DMA_WRITE_CHANNEL_ARB_WEIGHT_LOW_REG_WRITE_CHANNEL3_WEIGHT_MASK                                         0xF8000
#define TYPE0_DMA_WRITE_CHANNEL_ARB_WEIGHT_LOW_REG_WRITE_CHANNEL3_WEIGHT_SHFT                                         0xF
#define TYPE0_DMA_WRITE_CHANNEL_ARB_WEIGHT_LOW_REG_WRITE_CHANNEL2_WEIGHT_MASK                                         0x7C00
#define TYPE0_DMA_WRITE_CHANNEL_ARB_WEIGHT_LOW_REG_WRITE_CHANNEL2_WEIGHT_SHFT                                         0xA
#define TYPE0_DMA_WRITE_CHANNEL_ARB_WEIGHT_LOW_REG_WRITE_CHANNEL1_WEIGHT_MASK                                         0x3E0
#define TYPE0_DMA_WRITE_CHANNEL_ARB_WEIGHT_LOW_REG_WRITE_CHANNEL1_WEIGHT_SHFT                                         0x5
#define TYPE0_DMA_WRITE_CHANNEL_ARB_WEIGHT_LOW_REG_WRITE_CHANNEL0_WEIGHT_MASK                                         0x1F
#define TYPE0_DMA_WRITE_CHANNEL_ARB_WEIGHT_LOW_REG_WRITE_CHANNEL0_WEIGHT_SHFT                                         0x0

/* DMA_WRITE_CHANNEL_ARB_WEIGHT_HIGH_REG */
#define DMA_WRITE_CHANNEL_ARB_WEIGHT_HIGH_REG                                                                         ( 0x98C )
#define DMA_WRITE_CHANNEL_ARB_WEIGHT_HIGH_REG_WRITE_CHANNEL7_WEIGHT_MASK                                              0xF8000
#define DMA_WRITE_CHANNEL_ARB_WEIGHT_HIGH_REG_WRITE_CHANNEL7_WEIGHT_SHFT                                              0xF
#define DMA_WRITE_CHANNEL_ARB_WEIGHT_HIGH_REG_WRITE_CHANNEL6_WEIGHT_MASK                                              0x7C00
#define DMA_WRITE_CHANNEL_ARB_WEIGHT_HIGH_REG_WRITE_CHANNEL6_WEIGHT_SHFT                                              0xA
#define DMA_WRITE_CHANNEL_ARB_WEIGHT_HIGH_REG_WRITE_CHANNEL5_WEIGHT_MASK                                              0x3E0
#define DMA_WRITE_CHANNEL_ARB_WEIGHT_HIGH_REG_WRITE_CHANNEL5_WEIGHT_SHFT                                              0x5
#define DMA_WRITE_CHANNEL_ARB_WEIGHT_HIGH_REG_WRITE_CHANNEL4_WEIGHT_MASK                                              0x1F
#define DMA_WRITE_CHANNEL_ARB_WEIGHT_HIGH_REG_WRITE_CHANNEL4_WEIGHT_SHFT                                              0x0

/* TYPE0_DMA_WRITE_CHANNEL_ARB_WEIGHT_HIGH_REG */
#define TYPE0_DMA_WRITE_CHANNEL_ARB_WEIGHT_HIGH_REG                                                                   ( 0x98C )
#define TYPE0_DMA_WRITE_CHANNEL_ARB_WEIGHT_HIGH_REG_WRITE_CHANNEL7_WEIGHT_MASK                                        0xF8000
#define TYPE0_DMA_WRITE_CHANNEL_ARB_WEIGHT_HIGH_REG_WRITE_CHANNEL7_WEIGHT_SHFT                                        0xF
#define TYPE0_DMA_WRITE_CHANNEL_ARB_WEIGHT_HIGH_REG_WRITE_CHANNEL6_WEIGHT_MASK                                        0x7C00
#define TYPE0_DMA_WRITE_CHANNEL_ARB_WEIGHT_HIGH_REG_WRITE_CHANNEL6_WEIGHT_SHFT                                        0xA
#define TYPE0_DMA_WRITE_CHANNEL_ARB_WEIGHT_HIGH_REG_WRITE_CHANNEL5_WEIGHT_MASK                                        0x3E0
#define TYPE0_DMA_WRITE_CHANNEL_ARB_WEIGHT_HIGH_REG_WRITE_CHANNEL5_WEIGHT_SHFT                                        0x5
#define TYPE0_DMA_WRITE_CHANNEL_ARB_WEIGHT_HIGH_REG_WRITE_CHANNEL4_WEIGHT_MASK                                        0x1F
#define TYPE0_DMA_WRITE_CHANNEL_ARB_WEIGHT_HIGH_REG_WRITE_CHANNEL4_WEIGHT_SHFT                                        0x0

/* DMA_WRITE_P_REQ_TIMER_REG */
#define DMA_WRITE_P_REQ_TIMER_REG                                                                                     ( 0x998 )
#define DMA_WRITE_P_REQ_TIMER_REG_DMA_WR_P_TIMER_DISABLE_MASK                                                         0x8000
#define DMA_WRITE_P_REQ_TIMER_REG_DMA_WR_P_TIMER_DISABLE_SHFT                                                         0xF
#define DMA_WRITE_P_REQ_TIMER_REG_DMA_WR_P_TIMER_LIMIT_MASK                                                           0xFFF
#define DMA_WRITE_P_REQ_TIMER_REG_DMA_WR_P_TIMER_LIMIT_SHFT                                                           0x0

/* TYPE0_DMA_WRITE_P_REQ_TIMER_REG */
#define TYPE0_DMA_WRITE_P_REQ_TIMER_REG                                                                               ( 0x998 )
#define TYPE0_DMA_WRITE_P_REQ_TIMER_REG_DMA_WR_P_TIMER_DISABLE_MASK                                                   0x8000
#define TYPE0_DMA_WRITE_P_REQ_TIMER_REG_DMA_WR_P_TIMER_DISABLE_SHFT                                                   0xF
#define TYPE0_DMA_WRITE_P_REQ_TIMER_REG_DMA_WR_P_TIMER_LIMIT_MASK                                                     0xFFF
#define TYPE0_DMA_WRITE_P_REQ_TIMER_REG_DMA_WR_P_TIMER_LIMIT_SHFT                                                     0x0

/* DMA_READ_ENGINE_EN */
#define DMA_READ_ENGINE_EN                                                                                            ( 0x99C )
#define DMA_READ_ENGINE_EN_DMA_READ_ENGINE_MASK                                                                       0x1
#define DMA_READ_ENGINE_EN_DMA_READ_ENGINE_SHFT                                                                       0x0

/* TYPE0_DMA_READ_ENGINE_EN */
#define TYPE0_DMA_READ_ENGINE_EN                                                                                      ( 0x99C )
#define TYPE0_DMA_READ_ENGINE_EN_DMA_READ_ENGINE_MASK                                                                 0x1
#define TYPE0_DMA_READ_ENGINE_EN_DMA_READ_ENGINE_SHFT                                                                 0x0

/* DMA_READ_DOORBELL_REG */
#define DMA_READ_DOORBELL_REG                                                                                         ( 0x9A0 )
#define DMA_READ_DOORBELL_REG_RD_STOP_MASK                                                                            0x80000000
#define DMA_READ_DOORBELL_REG_RD_STOP_SHFT                                                                            0x1F
#define DMA_READ_DOORBELL_REG_RD_DOORBELL_NUM_MASK                                                                    0x7
#define DMA_READ_DOORBELL_REG_RD_DOORBELL_NUM_SHFT                                                                    0x0

/* TYPE0_DMA_READ_DOORBELL_REG */
#define TYPE0_DMA_READ_DOORBELL_REG                                                                                   ( 0x9A0 )
#define TYPE0_DMA_READ_DOORBELL_REG_RD_STOP_MASK                                                                      0x80000000
#define TYPE0_DMA_READ_DOORBELL_REG_RD_STOP_SHFT                                                                      0x1F
#define TYPE0_DMA_READ_DOORBELL_REG_RD_DOORBELL_NUM_MASK                                                              0x7
#define TYPE0_DMA_READ_DOORBELL_REG_RD_DOORBELL_NUM_SHFT                                                              0x0

/* DMA_READ_CHANNEL_ARB_WEIGHT_LOW_REG */
#define DMA_READ_CHANNEL_ARB_WEIGHT_LOW_REG                                                                           ( 0x9A8 )
#define DMA_READ_CHANNEL_ARB_WEIGHT_LOW_REG_READ_CHANNEL3_WEIGHT_MASK                                                 0xF8000
#define DMA_READ_CHANNEL_ARB_WEIGHT_LOW_REG_READ_CHANNEL3_WEIGHT_SHFT                                                 0xF
#define DMA_READ_CHANNEL_ARB_WEIGHT_LOW_REG_READ_CHANNEL2_WEIGHT_MASK                                                 0x7C00
#define DMA_READ_CHANNEL_ARB_WEIGHT_LOW_REG_READ_CHANNEL2_WEIGHT_SHFT                                                 0xA
#define DMA_READ_CHANNEL_ARB_WEIGHT_LOW_REG_READ_CHANNEL1_WEIGHT_MASK                                                 0x3E0
#define DMA_READ_CHANNEL_ARB_WEIGHT_LOW_REG_READ_CHANNEL1_WEIGHT_SHFT                                                 0x5
#define DMA_READ_CHANNEL_ARB_WEIGHT_LOW_REG_READ_CHANNEL0_WEIGHT_MASK                                                 0x1F
#define DMA_READ_CHANNEL_ARB_WEIGHT_LOW_REG_READ_CHANNEL0_WEIGHT_SHFT                                                 0x0

/* TYPE0_DMA_READ_CHANNEL_ARB_WEIGHT_LOW_REG */
#define TYPE0_DMA_READ_CHANNEL_ARB_WEIGHT_LOW_REG                                                                     ( 0x9A8 )
#define TYPE0_DMA_READ_CHANNEL_ARB_WEIGHT_LOW_REG_READ_CHANNEL3_WEIGHT_MASK                                           0xF8000
#define TYPE0_DMA_READ_CHANNEL_ARB_WEIGHT_LOW_REG_READ_CHANNEL3_WEIGHT_SHFT                                           0xF
#define TYPE0_DMA_READ_CHANNEL_ARB_WEIGHT_LOW_REG_READ_CHANNEL2_WEIGHT_MASK                                           0x7C00
#define TYPE0_DMA_READ_CHANNEL_ARB_WEIGHT_LOW_REG_READ_CHANNEL2_WEIGHT_SHFT                                           0xA
#define TYPE0_DMA_READ_CHANNEL_ARB_WEIGHT_LOW_REG_READ_CHANNEL1_WEIGHT_MASK                                           0x3E0
#define TYPE0_DMA_READ_CHANNEL_ARB_WEIGHT_LOW_REG_READ_CHANNEL1_WEIGHT_SHFT                                           0x5
#define TYPE0_DMA_READ_CHANNEL_ARB_WEIGHT_LOW_REG_READ_CHANNEL0_WEIGHT_MASK                                           0x1F
#define TYPE0_DMA_READ_CHANNEL_ARB_WEIGHT_LOW_REG_READ_CHANNEL0_WEIGHT_SHFT                                           0x0

/* DMA_READ_CHANNEL_ARB_WEIGHT_HIGH_REG */
#define DMA_READ_CHANNEL_ARB_WEIGHT_HIGH_REG                                                                          ( 0x9AC )
#define DMA_READ_CHANNEL_ARB_WEIGHT_HIGH_REG_READ_CHANNEL7_WEIGHT_MASK                                                0xF8000
#define DMA_READ_CHANNEL_ARB_WEIGHT_HIGH_REG_READ_CHANNEL7_WEIGHT_SHFT                                                0xF
#define DMA_READ_CHANNEL_ARB_WEIGHT_HIGH_REG_READ_CHANNEL6_WEIGHT_MASK                                                0x7C00
#define DMA_READ_CHANNEL_ARB_WEIGHT_HIGH_REG_READ_CHANNEL6_WEIGHT_SHFT                                                0xA
#define DMA_READ_CHANNEL_ARB_WEIGHT_HIGH_REG_READ_CHANNEL5_WEIGHT_MASK                                                0x3E0
#define DMA_READ_CHANNEL_ARB_WEIGHT_HIGH_REG_READ_CHANNEL5_WEIGHT_SHFT                                                0x5
#define DMA_READ_CHANNEL_ARB_WEIGHT_HIGH_REG_READ_CHANNEL4_WEIGHT_MASK                                                0x1F
#define DMA_READ_CHANNEL_ARB_WEIGHT_HIGH_REG_READ_CHANNEL4_WEIGHT_SHFT                                                0x0

/* TYPE0_DMA_READ_CHANNEL_ARB_WEIGHT_HIGH_REG */
#define TYPE0_DMA_READ_CHANNEL_ARB_WEIGHT_HIGH_REG                                                                    ( 0x9AC )
#define TYPE0_DMA_READ_CHANNEL_ARB_WEIGHT_HIGH_REG_READ_CHANNEL7_WEIGHT_MASK                                          0xF8000
#define TYPE0_DMA_READ_CHANNEL_ARB_WEIGHT_HIGH_REG_READ_CHANNEL7_WEIGHT_SHFT                                          0xF
#define TYPE0_DMA_READ_CHANNEL_ARB_WEIGHT_HIGH_REG_READ_CHANNEL6_WEIGHT_MASK                                          0x7C00
#define TYPE0_DMA_READ_CHANNEL_ARB_WEIGHT_HIGH_REG_READ_CHANNEL6_WEIGHT_SHFT                                          0xA
#define TYPE0_DMA_READ_CHANNEL_ARB_WEIGHT_HIGH_REG_READ_CHANNEL5_WEIGHT_MASK                                          0x3E0
#define TYPE0_DMA_READ_CHANNEL_ARB_WEIGHT_HIGH_REG_READ_CHANNEL5_WEIGHT_SHFT                                          0x5
#define TYPE0_DMA_READ_CHANNEL_ARB_WEIGHT_HIGH_REG_READ_CHANNEL4_WEIGHT_MASK                                          0x1F
#define TYPE0_DMA_READ_CHANNEL_ARB_WEIGHT_HIGH_REG_READ_CHANNEL4_WEIGHT_SHFT                                          0x0

/* DMA_WRITE_INT_STATUS_REG */
#define DMA_WRITE_INT_STATUS_REG                                                                                      ( 0x9BC )
#define DMA_WRITE_INT_STATUS_REG_WR_ABORT_INT_STATUS_MASK                                                             0xFF0000
#define DMA_WRITE_INT_STATUS_REG_WR_ABORT_INT_STATUS_SHFT                                                             0x10
#define DMA_WRITE_INT_STATUS_REG_WR_DONE_INT_STATUS_MASK                                                              0xFF
#define DMA_WRITE_INT_STATUS_REG_WR_DONE_INT_STATUS_SHFT                                                              0x0

/* TYPE0_DMA_WRITE_INT_STATUS_REG */
#define TYPE0_DMA_WRITE_INT_STATUS_REG                                                                                ( 0x9BC )
#define TYPE0_DMA_WRITE_INT_STATUS_REG_WR_ABORT_INT_STATUS_MASK                                                       0xFF0000
#define TYPE0_DMA_WRITE_INT_STATUS_REG_WR_ABORT_INT_STATUS_SHFT                                                       0x10
#define TYPE0_DMA_WRITE_INT_STATUS_REG_WR_DONE_INT_STATUS_MASK                                                        0xFF
#define TYPE0_DMA_WRITE_INT_STATUS_REG_WR_DONE_INT_STATUS_SHFT                                                        0x0

/* DMA_WRITE_INT_MASK_REG */
#define DMA_WRITE_INT_MASK_REG                                                                                        ( 0x9C4 )
#define DMA_WRITE_INT_MASK_REG_WR_ABORT_INT_MASK_MASK                                                                 0x10000
#define DMA_WRITE_INT_MASK_REG_WR_ABORT_INT_MASK_SHFT                                                                 0x10
#define DMA_WRITE_INT_MASK_REG_WR_DONE_INT_MASK_MASK                                                                  0x1
#define DMA_WRITE_INT_MASK_REG_WR_DONE_INT_MASK_SHFT                                                                  0x0

/* TYPE0_DMA_WRITE_INT_MASK_REG */
#define TYPE0_DMA_WRITE_INT_MASK_REG                                                                                  ( 0x9C4 )
#define TYPE0_DMA_WRITE_INT_MASK_REG_WR_ABORT_INT_MASK_MASK                                                           0x10000
#define TYPE0_DMA_WRITE_INT_MASK_REG_WR_ABORT_INT_MASK_SHFT                                                           0x10
#define TYPE0_DMA_WRITE_INT_MASK_REG_WR_DONE_INT_MASK_MASK                                                            0x1
#define TYPE0_DMA_WRITE_INT_MASK_REG_WR_DONE_INT_MASK_SHFT                                                            0x0

/* DMA_WRITE_INT_CLEAR_REG */
#define DMA_WRITE_INT_CLEAR_REG                                                                                       ( 0x9C8 )
#define DMA_WRITE_INT_CLEAR_REG_WR_ABORT_INT_CLEAR_MASK                                                               0x10000
#define DMA_WRITE_INT_CLEAR_REG_WR_ABORT_INT_CLEAR_SHFT                                                               0x10
#define DMA_WRITE_INT_CLEAR_REG_WR_DONE_INT_CLEAR_MASK                                                                0x1
#define DMA_WRITE_INT_CLEAR_REG_WR_DONE_INT_CLEAR_SHFT                                                                0x0

/* TYPE0_DMA_WRITE_INT_CLEAR_REG */
#define TYPE0_DMA_WRITE_INT_CLEAR_REG                                                                                 ( 0x9C8 )
#define TYPE0_DMA_WRITE_INT_CLEAR_REG_WR_ABORT_INT_CLEAR_MASK                                                         0x10000
#define TYPE0_DMA_WRITE_INT_CLEAR_REG_WR_ABORT_INT_CLEAR_SHFT                                                         0x10
#define TYPE0_DMA_WRITE_INT_CLEAR_REG_WR_DONE_INT_CLEAR_MASK                                                          0x1
#define TYPE0_DMA_WRITE_INT_CLEAR_REG_WR_DONE_INT_CLEAR_SHFT                                                          0x0

/* DMA_WRITE_ERR_STATUS_REG */
#define DMA_WRITE_ERR_STATUS_REG                                                                                      ( 0x9CC )
#define DMA_WRITE_ERR_STATUS_REG_LINKLIST_ELEMENT_FETCH_ERR_DETECT_MASK                                               0x10000
#define DMA_WRITE_ERR_STATUS_REG_LINKLIST_ELEMENT_FETCH_ERR_DETECT_SHFT                                               0x10
#define DMA_WRITE_ERR_STATUS_REG_APP_READ_ERR_DETECT_MASK                                                             0x1
#define DMA_WRITE_ERR_STATUS_REG_APP_READ_ERR_DETECT_SHFT                                                             0x0

/* TYPE0_DMA_WRITE_ERR_STATUS_REG */
#define TYPE0_DMA_WRITE_ERR_STATUS_REG                                                                                ( 0x9CC )
#define TYPE0_DMA_WRITE_ERR_STATUS_REG_LINKLIST_ELEMENT_FETCH_ERR_DETECT_MASK                                         0x10000
#define TYPE0_DMA_WRITE_ERR_STATUS_REG_LINKLIST_ELEMENT_FETCH_ERR_DETECT_SHFT                                         0x10
#define TYPE0_DMA_WRITE_ERR_STATUS_REG_APP_READ_ERR_DETECT_MASK                                                       0x1
#define TYPE0_DMA_WRITE_ERR_STATUS_REG_APP_READ_ERR_DETECT_SHFT                                                       0x0

/* DMA_WRITE_DONE_IMWR_LOW_REG */
#define DMA_WRITE_DONE_IMWR_LOW_REG                                                                                   ( 0x9D0 )
#define DMA_WRITE_DONE_IMWR_LOW_REG_DMA_WRITE_DONE_LOW_REG_MASK                                                       0xFFFFFFFF
#define DMA_WRITE_DONE_IMWR_LOW_REG_DMA_WRITE_DONE_LOW_REG_SHFT                                                       0x0

/* TYPE0_DMA_WRITE_DONE_IMWR_LOW_REG */
#define TYPE0_DMA_WRITE_DONE_IMWR_LOW_REG                                                                             ( 0x9D0 )
#define TYPE0_DMA_WRITE_DONE_IMWR_LOW_REG_DMA_WRITE_DONE_LOW_REG_MASK                                                 0xFFFFFFFF
#define TYPE0_DMA_WRITE_DONE_IMWR_LOW_REG_DMA_WRITE_DONE_LOW_REG_SHFT                                                 0x0

/* DMA_WRITE_DONE_IMWR_HIGH_REG */
#define DMA_WRITE_DONE_IMWR_HIGH_REG                                                                                  ( 0x9D4 )
#define DMA_WRITE_DONE_IMWR_HIGH_REG_DMA_WRITE_DONE_HIGH_REG_MASK                                                     0xFFFFFFFF
#define DMA_WRITE_DONE_IMWR_HIGH_REG_DMA_WRITE_DONE_HIGH_REG_SHFT                                                     0x0

/* TYPE0_DMA_WRITE_DONE_IMWR_HIGH_REG */
#define TYPE0_DMA_WRITE_DONE_IMWR_HIGH_REG                                                                            ( 0x9D4 )
#define TYPE0_DMA_WRITE_DONE_IMWR_HIGH_REG_DMA_WRITE_DONE_HIGH_REG_MASK                                               0xFFFFFFFF
#define TYPE0_DMA_WRITE_DONE_IMWR_HIGH_REG_DMA_WRITE_DONE_HIGH_REG_SHFT                                               0x0

/* DMA_WRITE_ABORT_IMWR_LOW_REG */
#define DMA_WRITE_ABORT_IMWR_LOW_REG                                                                                  ( 0x9D8 )
#define DMA_WRITE_ABORT_IMWR_LOW_REG_DMA_WRITE_ABORT_LOW_REG_MASK                                                     0xFFFFFFFF
#define DMA_WRITE_ABORT_IMWR_LOW_REG_DMA_WRITE_ABORT_LOW_REG_SHFT                                                     0x0

/* TYPE0_DMA_WRITE_ABORT_IMWR_LOW_REG */
#define TYPE0_DMA_WRITE_ABORT_IMWR_LOW_REG                                                                            ( 0x9D8 )
#define TYPE0_DMA_WRITE_ABORT_IMWR_LOW_REG_DMA_WRITE_ABORT_LOW_REG_MASK                                               0xFFFFFFFF
#define TYPE0_DMA_WRITE_ABORT_IMWR_LOW_REG_DMA_WRITE_ABORT_LOW_REG_SHFT                                               0x0

/* DMA_WRITE_ABORT_IMWR_HIGH_REG */
#define DMA_WRITE_ABORT_IMWR_HIGH_REG                                                                                 ( 0x9DC )
#define DMA_WRITE_ABORT_IMWR_HIGH_REG_DMA_WRITE_ABORT_HIGH_REG_MASK                                                   0xFFFFFFFF
#define DMA_WRITE_ABORT_IMWR_HIGH_REG_DMA_WRITE_ABORT_HIGH_REG_SHFT                                                   0x0

/* TYPE0_DMA_WRITE_ABORT_IMWR_HIGH_REG */
#define TYPE0_DMA_WRITE_ABORT_IMWR_HIGH_REG                                                                           ( 0x9DC )
#define TYPE0_DMA_WRITE_ABORT_IMWR_HIGH_REG_DMA_WRITE_ABORT_HIGH_REG_MASK                                             0xFFFFFFFF
#define TYPE0_DMA_WRITE_ABORT_IMWR_HIGH_REG_DMA_WRITE_ABORT_HIGH_REG_SHFT                                             0x0

/* DMA_WRITE_CH01_IMWR_DATA_REG */
#define DMA_WRITE_CH01_IMWR_DATA_REG                                                                                  ( 0x9E0 )
#define DMA_WRITE_CH01_IMWR_DATA_REG_WR_CHANNEL_0_DATA_MASK                                                           0xFFFF
#define DMA_WRITE_CH01_IMWR_DATA_REG_WR_CHANNEL_0_DATA_SHFT                                                           0x0

/* TYPE0_DMA_WRITE_CH01_IMWR_DATA_REG */
#define TYPE0_DMA_WRITE_CH01_IMWR_DATA_REG                                                                            ( 0x9E0 )
#define TYPE0_DMA_WRITE_CH01_IMWR_DATA_REG_WR_CHANNEL_0_DATA_MASK                                                     0xFFFF
#define TYPE0_DMA_WRITE_CH01_IMWR_DATA_REG_WR_CHANNEL_0_DATA_SHFT                                                     0x0

/* DMA_WRITE_LINKED_LIST_ERR_EN_REG */
#define DMA_WRITE_LINKED_LIST_ERR_EN_REG                                                                              ( 0xA00 )
#define DMA_WRITE_LINKED_LIST_ERR_EN_REG_WR_CHANNEL_LLLAIE_MASK                                                       0x10000
#define DMA_WRITE_LINKED_LIST_ERR_EN_REG_WR_CHANNEL_LLLAIE_SHFT                                                       0x10
#define DMA_WRITE_LINKED_LIST_ERR_EN_REG_WR_CHANNEL_LLRAIE_MASK                                                       0x1
#define DMA_WRITE_LINKED_LIST_ERR_EN_REG_WR_CHANNEL_LLRAIE_SHFT                                                       0x0

/* TYPE0_DMA_WRITE_LINKED_LIST_ERR_EN_REG */
#define TYPE0_DMA_WRITE_LINKED_LIST_ERR_EN_REG                                                                        ( 0xA00 )
#define TYPE0_DMA_WRITE_LINKED_LIST_ERR_EN_REG_WR_CHANNEL_LLLAIE_MASK                                                 0x10000
#define TYPE0_DMA_WRITE_LINKED_LIST_ERR_EN_REG_WR_CHANNEL_LLLAIE_SHFT                                                 0x10
#define TYPE0_DMA_WRITE_LINKED_LIST_ERR_EN_REG_WR_CHANNEL_LLRAIE_MASK                                                 0x1
#define TYPE0_DMA_WRITE_LINKED_LIST_ERR_EN_REG_WR_CHANNEL_LLRAIE_SHFT                                                 0x0

/* DMA_READ_INT_STATUS_REG */
#define DMA_READ_INT_STATUS_REG                                                                                       ( 0xA10 )
#define DMA_READ_INT_STATUS_REG_RD_ABORT_INT_STATUS_MASK                                                              0xFF0000
#define DMA_READ_INT_STATUS_REG_RD_ABORT_INT_STATUS_SHFT                                                              0x10
#define DMA_READ_INT_STATUS_REG_RD_DONE_INT_STATUS_MASK                                                               0xFF
#define DMA_READ_INT_STATUS_REG_RD_DONE_INT_STATUS_SHFT                                                               0x0

/* TYPE0_DMA_READ_INT_STATUS_REG */
#define TYPE0_DMA_READ_INT_STATUS_REG                                                                                 ( 0xA10 )
#define TYPE0_DMA_READ_INT_STATUS_REG_RD_ABORT_INT_STATUS_MASK                                                        0xFF0000
#define TYPE0_DMA_READ_INT_STATUS_REG_RD_ABORT_INT_STATUS_SHFT                                                        0x10
#define TYPE0_DMA_READ_INT_STATUS_REG_RD_DONE_INT_STATUS_MASK                                                         0xFF
#define TYPE0_DMA_READ_INT_STATUS_REG_RD_DONE_INT_STATUS_SHFT                                                         0x0

/* DMA_READ_INT_MASK_REG */
#define DMA_READ_INT_MASK_REG                                                                                         ( 0xA18 )
#define DMA_READ_INT_MASK_REG_RD_ABORT_INT_MASK_MASK                                                                  0x10000
#define DMA_READ_INT_MASK_REG_RD_ABORT_INT_MASK_SHFT                                                                  0x10
#define DMA_READ_INT_MASK_REG_RD_DONE_INT_MASK_MASK                                                                   0x1
#define DMA_READ_INT_MASK_REG_RD_DONE_INT_MASK_SHFT                                                                   0x0

/* TYPE0_DMA_READ_INT_MASK_REG */
#define TYPE0_DMA_READ_INT_MASK_REG                                                                                   ( 0xA18 )
#define TYPE0_DMA_READ_INT_MASK_REG_RD_ABORT_INT_MASK_MASK                                                            0x10000
#define TYPE0_DMA_READ_INT_MASK_REG_RD_ABORT_INT_MASK_SHFT                                                            0x10
#define TYPE0_DMA_READ_INT_MASK_REG_RD_DONE_INT_MASK_MASK                                                             0x1
#define TYPE0_DMA_READ_INT_MASK_REG_RD_DONE_INT_MASK_SHFT                                                             0x0

/* DMA_READ_INT_CLEAR_REG */
#define DMA_READ_INT_CLEAR_REG                                                                                        ( 0xA1C )
#define DMA_READ_INT_CLEAR_REG_RD_ABORT_INT_CLEAR_MASK                                                                0x10000
#define DMA_READ_INT_CLEAR_REG_RD_ABORT_INT_CLEAR_SHFT                                                                0x10
#define DMA_READ_INT_CLEAR_REG_RD_DONE_INT_CLEAR_MASK                                                                 0x1
#define DMA_READ_INT_CLEAR_REG_RD_DONE_INT_CLEAR_SHFT                                                                 0x0

/* TYPE0_DMA_READ_INT_CLEAR_REG */
#define TYPE0_DMA_READ_INT_CLEAR_REG                                                                                  ( 0xA1C )
#define TYPE0_DMA_READ_INT_CLEAR_REG_RD_ABORT_INT_CLEAR_MASK                                                          0x10000
#define TYPE0_DMA_READ_INT_CLEAR_REG_RD_ABORT_INT_CLEAR_SHFT                                                          0x10
#define TYPE0_DMA_READ_INT_CLEAR_REG_RD_DONE_INT_CLEAR_MASK                                                           0x1
#define TYPE0_DMA_READ_INT_CLEAR_REG_RD_DONE_INT_CLEAR_SHFT                                                           0x0

/* DMA_READ_ERR_STATUS_LOW_REG */
#define DMA_READ_ERR_STATUS_LOW_REG                                                                                   ( 0xA24 )
#define DMA_READ_ERR_STATUS_LOW_REG_LINK_LIST_ELEMENT_FETCH_ERR_DETECT_MASK                                           0x10000
#define DMA_READ_ERR_STATUS_LOW_REG_LINK_LIST_ELEMENT_FETCH_ERR_DETECT_SHFT                                           0x10
#define DMA_READ_ERR_STATUS_LOW_REG_APP_WR_ERR_DETECT_MASK                                                            0x1
#define DMA_READ_ERR_STATUS_LOW_REG_APP_WR_ERR_DETECT_SHFT                                                            0x0

/* TYPE0_DMA_READ_ERR_STATUS_LOW_REG */
#define TYPE0_DMA_READ_ERR_STATUS_LOW_REG                                                                             ( 0xA24 )
#define TYPE0_DMA_READ_ERR_STATUS_LOW_REG_LINK_LIST_ELEMENT_FETCH_ERR_DETECT_MASK                                     0x10000
#define TYPE0_DMA_READ_ERR_STATUS_LOW_REG_LINK_LIST_ELEMENT_FETCH_ERR_DETECT_SHFT                                     0x10
#define TYPE0_DMA_READ_ERR_STATUS_LOW_REG_APP_WR_ERR_DETECT_MASK                                                      0x1
#define TYPE0_DMA_READ_ERR_STATUS_LOW_REG_APP_WR_ERR_DETECT_SHFT                                                      0x0

/* DMA_READ_ERR_STATUS_HIGH_REG */
#define DMA_READ_ERR_STATUS_HIGH_REG                                                                                  ( 0xA28 )
#define DMA_READ_ERR_STATUS_HIGH_REG_DATA_POISIONING_MASK                                                             0x1000000
#define DMA_READ_ERR_STATUS_HIGH_REG_DATA_POISIONING_SHFT                                                             0x18
#define DMA_READ_ERR_STATUS_HIGH_REG_CPL_TIMEOUT_MASK                                                                 0x10000
#define DMA_READ_ERR_STATUS_HIGH_REG_CPL_TIMEOUT_SHFT                                                                 0x10
#define DMA_READ_ERR_STATUS_HIGH_REG_CPL_ABORT_MASK                                                                   0x200
#define DMA_READ_ERR_STATUS_HIGH_REG_CPL_ABORT_SHFT                                                                   0x9
#define DMA_READ_ERR_STATUS_HIGH_REG_UNSUPPORTED_REQ_MASK                                                             0x1
#define DMA_READ_ERR_STATUS_HIGH_REG_UNSUPPORTED_REQ_SHFT                                                             0x0

/* TYPE0_DMA_READ_ERR_STATUS_HIGH_REG */
#define TYPE0_DMA_READ_ERR_STATUS_HIGH_REG                                                                            ( 0xA28 )
#define TYPE0_DMA_READ_ERR_STATUS_HIGH_REG_DATA_POISIONING_MASK                                                       0x1000000
#define TYPE0_DMA_READ_ERR_STATUS_HIGH_REG_DATA_POISIONING_SHFT                                                       0x18
#define TYPE0_DMA_READ_ERR_STATUS_HIGH_REG_CPL_TIMEOUT_MASK                                                           0x10000
#define TYPE0_DMA_READ_ERR_STATUS_HIGH_REG_CPL_TIMEOUT_SHFT                                                           0x10
#define TYPE0_DMA_READ_ERR_STATUS_HIGH_REG_CPL_ABORT_MASK                                                             0x200
#define TYPE0_DMA_READ_ERR_STATUS_HIGH_REG_CPL_ABORT_SHFT                                                             0x9
#define TYPE0_DMA_READ_ERR_STATUS_HIGH_REG_UNSUPPORTED_REQ_MASK                                                       0x1
#define TYPE0_DMA_READ_ERR_STATUS_HIGH_REG_UNSUPPORTED_REQ_SHFT                                                       0x0

/* DMA_READ_LINKED_LIST_ERR_EN_REG */
#define DMA_READ_LINKED_LIST_ERR_EN_REG                                                                               ( 0xA34 )
#define DMA_READ_LINKED_LIST_ERR_EN_REG_RD_CHANNEL_LLLAIE_MASK                                                        0x10000
#define DMA_READ_LINKED_LIST_ERR_EN_REG_RD_CHANNEL_LLLAIE_SHFT                                                        0x10
#define DMA_READ_LINKED_LIST_ERR_EN_REG_RD_CHANNEL_LLRAIE_MASK                                                        0x1
#define DMA_READ_LINKED_LIST_ERR_EN_REG_RD_CHANNEL_LLRAIE_SHFT                                                        0x0

/* TYPE0_DMA_READ_LINKED_LIST_ERR_EN_REG */
#define TYPE0_DMA_READ_LINKED_LIST_ERR_EN_REG                                                                         ( 0xA34 )
#define TYPE0_DMA_READ_LINKED_LIST_ERR_EN_REG_RD_CHANNEL_LLLAIE_MASK                                                  0x10000
#define TYPE0_DMA_READ_LINKED_LIST_ERR_EN_REG_RD_CHANNEL_LLLAIE_SHFT                                                  0x10
#define TYPE0_DMA_READ_LINKED_LIST_ERR_EN_REG_RD_CHANNEL_LLRAIE_MASK                                                  0x1
#define TYPE0_DMA_READ_LINKED_LIST_ERR_EN_REG_RD_CHANNEL_LLRAIE_SHFT                                                  0x0

/* DMA_READ_DONE_IMWR_LOW_REG */
#define DMA_READ_DONE_IMWR_LOW_REG                                                                                    ( 0xA3C )
#define DMA_READ_DONE_IMWR_LOW_REG_DMA_READ_DONE_LOW_REG_MASK                                                         0xFFFFFFFF
#define DMA_READ_DONE_IMWR_LOW_REG_DMA_READ_DONE_LOW_REG_SHFT                                                         0x0

/* TYPE0_DMA_READ_DONE_IMWR_LOW_REG */
#define TYPE0_DMA_READ_DONE_IMWR_LOW_REG                                                                              ( 0xA3C )
#define TYPE0_DMA_READ_DONE_IMWR_LOW_REG_DMA_READ_DONE_LOW_REG_MASK                                                   0xFFFFFFFF
#define TYPE0_DMA_READ_DONE_IMWR_LOW_REG_DMA_READ_DONE_LOW_REG_SHFT                                                   0x0

/* DMA_READ_DONE_IMWR_HIGH_REG */
#define DMA_READ_DONE_IMWR_HIGH_REG                                                                                   ( 0xA40 )
#define DMA_READ_DONE_IMWR_HIGH_REG_DMA_READ_DONE_HIGH_REG_MASK                                                       0xFFFFFFFF
#define DMA_READ_DONE_IMWR_HIGH_REG_DMA_READ_DONE_HIGH_REG_SHFT                                                       0x0

/* TYPE0_DMA_READ_DONE_IMWR_HIGH_REG */
#define TYPE0_DMA_READ_DONE_IMWR_HIGH_REG                                                                             ( 0xA40 )
#define TYPE0_DMA_READ_DONE_IMWR_HIGH_REG_DMA_READ_DONE_HIGH_REG_MASK                                                 0xFFFFFFFF
#define TYPE0_DMA_READ_DONE_IMWR_HIGH_REG_DMA_READ_DONE_HIGH_REG_SHFT                                                 0x0

/* DMA_READ_ABORT_IMWR_LOW_REG */
#define DMA_READ_ABORT_IMWR_LOW_REG                                                                                   ( 0xA44 )
#define DMA_READ_ABORT_IMWR_LOW_REG_DMA_READ_ABORT_LOW_REG_MASK                                                       0xFFFFFFFF
#define DMA_READ_ABORT_IMWR_LOW_REG_DMA_READ_ABORT_LOW_REG_SHFT                                                       0x0

/* TYPE0_DMA_READ_ABORT_IMWR_LOW_REG */
#define TYPE0_DMA_READ_ABORT_IMWR_LOW_REG                                                                             ( 0xA44 )
#define TYPE0_DMA_READ_ABORT_IMWR_LOW_REG_DMA_READ_ABORT_LOW_REG_MASK                                                 0xFFFFFFFF
#define TYPE0_DMA_READ_ABORT_IMWR_LOW_REG_DMA_READ_ABORT_LOW_REG_SHFT                                                 0x0

/* DMA_READ_ABORT_IMWR_HIGH_REG */
#define DMA_READ_ABORT_IMWR_HIGH_REG                                                                                  ( 0xA48 )
#define DMA_READ_ABORT_IMWR_HIGH_REG_DMA_READ_ABORT_HIGH_REG_MASK                                                     0xFFFFFFFF
#define DMA_READ_ABORT_IMWR_HIGH_REG_DMA_READ_ABORT_HIGH_REG_SHFT                                                     0x0

/* TYPE0_DMA_READ_ABORT_IMWR_HIGH_REG */
#define TYPE0_DMA_READ_ABORT_IMWR_HIGH_REG                                                                            ( 0xA48 )
#define TYPE0_DMA_READ_ABORT_IMWR_HIGH_REG_DMA_READ_ABORT_HIGH_REG_MASK                                               0xFFFFFFFF
#define TYPE0_DMA_READ_ABORT_IMWR_HIGH_REG_DMA_READ_ABORT_HIGH_REG_SHFT                                               0x0

/* DMA_READ_CH01_IMWR_DATA_REG */
#define DMA_READ_CH01_IMWR_DATA_REG                                                                                   ( 0xA4C )
#define DMA_READ_CH01_IMWR_DATA_REG_RD_CHANNEL_0_DATA_MASK                                                            0xFFFF
#define DMA_READ_CH01_IMWR_DATA_REG_RD_CHANNEL_0_DATA_SHFT                                                            0x0

/* TYPE0_DMA_READ_CH01_IMWR_DATA_REG */
#define TYPE0_DMA_READ_CH01_IMWR_DATA_REG                                                                             ( 0xA4C )
#define TYPE0_DMA_READ_CH01_IMWR_DATA_REG_RD_CHANNEL_0_DATA_MASK                                                      0xFFFF
#define TYPE0_DMA_READ_CH01_IMWR_DATA_REG_RD_CHANNEL_0_DATA_SHFT                                                      0x0

/* DMA_VIEWPORT_SEL_REG */
#define DMA_VIEWPORT_SEL_REG                                                                                          ( 0xA6C )
#define DMA_VIEWPORT_SEL_REG_CHANNEL_DIR_MASK                                                                         0x80000000
#define DMA_VIEWPORT_SEL_REG_CHANNEL_DIR_SHFT                                                                         0x1F
#define DMA_VIEWPORT_SEL_REG_CHANNEL_NUM_MASK                                                                         0x7
#define DMA_VIEWPORT_SEL_REG_CHANNEL_NUM_SHFT                                                                         0x0

/* TYPE0_DMA_VIEWPORT_SEL_REG */
#define TYPE0_DMA_VIEWPORT_SEL_REG                                                                                    ( 0xA6C )
#define TYPE0_DMA_VIEWPORT_SEL_REG_CHANNEL_DIR_MASK                                                                   0x80000000
#define TYPE0_DMA_VIEWPORT_SEL_REG_CHANNEL_DIR_SHFT                                                                   0x1F
#define TYPE0_DMA_VIEWPORT_SEL_REG_CHANNEL_NUM_MASK                                                                   0x7
#define TYPE0_DMA_VIEWPORT_SEL_REG_CHANNEL_NUM_SHFT                                                                   0x0

/* DMA_CH_CONTROL1_REG_WRCH_0 */
#define DMA_CH_CONTROL1_REG_WRCH_0                                                                                    ( 0xA70 )
#define DMA_CH_CONTROL1_REG_WRCH_0_DMA_AT_MASK                                                                        0xC0000000
#define DMA_CH_CONTROL1_REG_WRCH_0_DMA_AT_SHFT                                                                        0x1E
#define DMA_CH_CONTROL1_REG_WRCH_0_DMA_TC_MASK                                                                        0x38000000
#define DMA_CH_CONTROL1_REG_WRCH_0_DMA_TC_SHFT                                                                        0x1B
#define DMA_CH_CONTROL1_REG_WRCH_0_DMA_TD_MASK                                                                        0x4000000
#define DMA_CH_CONTROL1_REG_WRCH_0_DMA_TD_SHFT                                                                        0x1A
#define DMA_CH_CONTROL1_REG_WRCH_0_DMA_RO_MASK                                                                        0x2000000
#define DMA_CH_CONTROL1_REG_WRCH_0_DMA_RO_SHFT                                                                        0x19
#define DMA_CH_CONTROL1_REG_WRCH_0_NS_MASK                                                                            0x1000000
#define DMA_CH_CONTROL1_REG_WRCH_0_NS_SHFT                                                                            0x18
#define DMA_CH_CONTROL1_REG_WRCH_0_DMA_FUNC_NUM_MASK                                                                  0x1F000
#define DMA_CH_CONTROL1_REG_WRCH_0_DMA_FUNC_NUM_SHFT                                                                  0xC
#define DMA_CH_CONTROL1_REG_WRCH_0_LLE_MASK                                                                           0x200
#define DMA_CH_CONTROL1_REG_WRCH_0_LLE_SHFT                                                                           0x9
#define DMA_CH_CONTROL1_REG_WRCH_0_CCS_MASK                                                                           0x100
#define DMA_CH_CONTROL1_REG_WRCH_0_CCS_SHFT                                                                           0x8
#define DMA_CH_CONTROL1_REG_WRCH_0_CS_MASK                                                                            0x60
#define DMA_CH_CONTROL1_REG_WRCH_0_CS_SHFT                                                                            0x5
#define DMA_CH_CONTROL1_REG_WRCH_0_RIE_MASK                                                                           0x10
#define DMA_CH_CONTROL1_REG_WRCH_0_RIE_SHFT                                                                           0x4
#define DMA_CH_CONTROL1_REG_WRCH_0_LIE_MASK                                                                           0x8
#define DMA_CH_CONTROL1_REG_WRCH_0_LIE_SHFT                                                                           0x3
#define DMA_CH_CONTROL1_REG_WRCH_0_LLP_MASK                                                                           0x4
#define DMA_CH_CONTROL1_REG_WRCH_0_LLP_SHFT                                                                           0x2
#define DMA_CH_CONTROL1_REG_WRCH_0_TCB_MASK                                                                           0x2
#define DMA_CH_CONTROL1_REG_WRCH_0_TCB_SHFT                                                                           0x1
#define DMA_CH_CONTROL1_REG_WRCH_0_CB_MASK                                                                            0x1
#define DMA_CH_CONTROL1_REG_WRCH_0_CB_SHFT                                                                            0x0

/* TYPE0_DMA_CH_CONTROL1_REG_WRCH_0 */
#define TYPE0_DMA_CH_CONTROL1_REG_WRCH_0                                                                              ( 0xA70 )
#define TYPE0_DMA_CH_CONTROL1_REG_WRCH_0_DMA_AT_MASK                                                                  0xC0000000
#define TYPE0_DMA_CH_CONTROL1_REG_WRCH_0_DMA_AT_SHFT                                                                  0x1E
#define TYPE0_DMA_CH_CONTROL1_REG_WRCH_0_DMA_TC_MASK                                                                  0x38000000
#define TYPE0_DMA_CH_CONTROL1_REG_WRCH_0_DMA_TC_SHFT                                                                  0x1B
#define TYPE0_DMA_CH_CONTROL1_REG_WRCH_0_DMA_TD_MASK                                                                  0x4000000
#define TYPE0_DMA_CH_CONTROL1_REG_WRCH_0_DMA_TD_SHFT                                                                  0x1A
#define TYPE0_DMA_CH_CONTROL1_REG_WRCH_0_DMA_RO_MASK                                                                  0x2000000
#define TYPE0_DMA_CH_CONTROL1_REG_WRCH_0_DMA_RO_SHFT                                                                  0x19
#define TYPE0_DMA_CH_CONTROL1_REG_WRCH_0_NS_MASK                                                                      0x1000000
#define TYPE0_DMA_CH_CONTROL1_REG_WRCH_0_NS_SHFT                                                                      0x18
#define TYPE0_DMA_CH_CONTROL1_REG_WRCH_0_DMA_FUNC_NUM_MASK                                                            0x1F000
#define TYPE0_DMA_CH_CONTROL1_REG_WRCH_0_DMA_FUNC_NUM_SHFT                                                            0xC
#define TYPE0_DMA_CH_CONTROL1_REG_WRCH_0_LLE_MASK                                                                     0x200
#define TYPE0_DMA_CH_CONTROL1_REG_WRCH_0_LLE_SHFT                                                                     0x9
#define TYPE0_DMA_CH_CONTROL1_REG_WRCH_0_CCS_MASK                                                                     0x100
#define TYPE0_DMA_CH_CONTROL1_REG_WRCH_0_CCS_SHFT                                                                     0x8
#define TYPE0_DMA_CH_CONTROL1_REG_WRCH_0_CS_MASK                                                                      0x60
#define TYPE0_DMA_CH_CONTROL1_REG_WRCH_0_CS_SHFT                                                                      0x5
#define TYPE0_DMA_CH_CONTROL1_REG_WRCH_0_RIE_MASK                                                                     0x10
#define TYPE0_DMA_CH_CONTROL1_REG_WRCH_0_RIE_SHFT                                                                     0x4
#define TYPE0_DMA_CH_CONTROL1_REG_WRCH_0_LIE_MASK                                                                     0x8
#define TYPE0_DMA_CH_CONTROL1_REG_WRCH_0_LIE_SHFT                                                                     0x3
#define TYPE0_DMA_CH_CONTROL1_REG_WRCH_0_LLP_MASK                                                                     0x4
#define TYPE0_DMA_CH_CONTROL1_REG_WRCH_0_LLP_SHFT                                                                     0x2
#define TYPE0_DMA_CH_CONTROL1_REG_WRCH_0_TCB_MASK                                                                     0x2
#define TYPE0_DMA_CH_CONTROL1_REG_WRCH_0_TCB_SHFT                                                                     0x1
#define TYPE0_DMA_CH_CONTROL1_REG_WRCH_0_CB_MASK                                                                      0x1
#define TYPE0_DMA_CH_CONTROL1_REG_WRCH_0_CB_SHFT                                                                      0x0

/* DMA_CH_CONTROL2_REG_WRCH_0 */
#define DMA_CH_CONTROL2_REG_WRCH_0                                                                                    ( 0xA74 )
#define DMA_CH_CONTROL2_REG_WRCH_0_DMA_VF_MASK                                                                        0x1FE
#define DMA_CH_CONTROL2_REG_WRCH_0_DMA_VF_SHFT                                                                        0x1
#define DMA_CH_CONTROL2_REG_WRCH_0_DMA_VF_EN_MASK                                                                     0x1
#define DMA_CH_CONTROL2_REG_WRCH_0_DMA_VF_EN_SHFT                                                                     0x0

/* TYPE0_DMA_CH_CONTROL2_REG_WRCH_0 */
#define TYPE0_DMA_CH_CONTROL2_REG_WRCH_0                                                                              ( 0xA74 )
#define TYPE0_DMA_CH_CONTROL2_REG_WRCH_0_DMA_VF_MASK                                                                  0x1FE
#define TYPE0_DMA_CH_CONTROL2_REG_WRCH_0_DMA_VF_SHFT                                                                  0x1
#define TYPE0_DMA_CH_CONTROL2_REG_WRCH_0_DMA_VF_EN_MASK                                                               0x1
#define TYPE0_DMA_CH_CONTROL2_REG_WRCH_0_DMA_VF_EN_SHFT                                                               0x0

/* DMA_TRANSFER_SIZE_REG_WRCH_0 */
#define DMA_TRANSFER_SIZE_REG_WRCH_0                                                                                  ( 0xA78 )
#define DMA_TRANSFER_SIZE_REG_WRCH_0_DMA_TRANSFER_SIZE_MASK                                                           0xFFFFFFFF
#define DMA_TRANSFER_SIZE_REG_WRCH_0_DMA_TRANSFER_SIZE_SHFT                                                           0x0

/* TYPE0_DMA_TRANSFER_SIZE_REG_WRCH_0 */
#define TYPE0_DMA_TRANSFER_SIZE_REG_WRCH_0                                                                            ( 0xA78 )
#define TYPE0_DMA_TRANSFER_SIZE_REG_WRCH_0_DMA_TRANSFER_SIZE_MASK                                                     0xFFFFFFFF
#define TYPE0_DMA_TRANSFER_SIZE_REG_WRCH_0_DMA_TRANSFER_SIZE_SHFT                                                     0x0

/* DMA_SAR_LOW_REG_WRCH_0 */
#define DMA_SAR_LOW_REG_WRCH_0                                                                                        ( 0xA7C )
#define DMA_SAR_LOW_REG_WRCH_0_SRC_ADDR_REG_LOW_MASK                                                                  0xFFFFFFFF
#define DMA_SAR_LOW_REG_WRCH_0_SRC_ADDR_REG_LOW_SHFT                                                                  0x0

/* TYPE0_DMA_SAR_LOW_REG_WRCH_0 */
#define TYPE0_DMA_SAR_LOW_REG_WRCH_0                                                                                  ( 0xA7C )
#define TYPE0_DMA_SAR_LOW_REG_WRCH_0_SRC_ADDR_REG_LOW_MASK                                                            0xFFFFFFFF
#define TYPE0_DMA_SAR_LOW_REG_WRCH_0_SRC_ADDR_REG_LOW_SHFT                                                            0x0

/* DMA_SAR_HIGH_REG_WRCH_0 */
#define DMA_SAR_HIGH_REG_WRCH_0                                                                                       ( 0xA80 )
#define DMA_SAR_HIGH_REG_WRCH_0_SRC_ADDR_REG_HIGH_MASK                                                                0xFFFFFFFF
#define DMA_SAR_HIGH_REG_WRCH_0_SRC_ADDR_REG_HIGH_SHFT                                                                0x0

/* TYPE0_DMA_SAR_HIGH_REG_WRCH_0 */
#define TYPE0_DMA_SAR_HIGH_REG_WRCH_0                                                                                 ( 0xA80 )
#define TYPE0_DMA_SAR_HIGH_REG_WRCH_0_SRC_ADDR_REG_HIGH_MASK                                                          0xFFFFFFFF
#define TYPE0_DMA_SAR_HIGH_REG_WRCH_0_SRC_ADDR_REG_HIGH_SHFT                                                          0x0

/* DMA_DAR_LOW_REG_WRCH_0 */
#define DMA_DAR_LOW_REG_WRCH_0                                                                                        ( 0xA84 )
#define DMA_DAR_LOW_REG_WRCH_0_SRC_ADDR_REG_LOW_MASK                                                                  0xFFFFFFFF
#define DMA_DAR_LOW_REG_WRCH_0_SRC_ADDR_REG_LOW_SHFT                                                                  0x0

/* TYPE0_DMA_DAR_LOW_REG_WRCH_0 */
#define TYPE0_DMA_DAR_LOW_REG_WRCH_0                                                                                  ( 0xA84 )
#define TYPE0_DMA_DAR_LOW_REG_WRCH_0_SRC_ADDR_REG_LOW_MASK                                                            0xFFFFFFFF
#define TYPE0_DMA_DAR_LOW_REG_WRCH_0_SRC_ADDR_REG_LOW_SHFT                                                            0x0

/* DMA_DAR_HIGH_REG_WRCH_0 */
#define DMA_DAR_HIGH_REG_WRCH_0                                                                                       ( 0xA88 )
#define DMA_DAR_HIGH_REG_WRCH_0_SRC_ADDR_REG_HIGH_MASK                                                                0xFFFFFFFF
#define DMA_DAR_HIGH_REG_WRCH_0_SRC_ADDR_REG_HIGH_SHFT                                                                0x0

/* TYPE0_DMA_DAR_HIGH_REG_WRCH_0 */
#define TYPE0_DMA_DAR_HIGH_REG_WRCH_0                                                                                 ( 0xA88 )
#define TYPE0_DMA_DAR_HIGH_REG_WRCH_0_SRC_ADDR_REG_HIGH_MASK                                                          0xFFFFFFFF
#define TYPE0_DMA_DAR_HIGH_REG_WRCH_0_SRC_ADDR_REG_HIGH_SHFT                                                          0x0

/* DMA_LLP_LOW_REG_WRCH_0 */
#define DMA_LLP_LOW_REG_WRCH_0                                                                                        ( 0xA8C )
#define DMA_LLP_LOW_REG_WRCH_0_LLP_LOW_MASK                                                                           0xFFFFFFFF
#define DMA_LLP_LOW_REG_WRCH_0_LLP_LOW_SHFT                                                                           0x0

/* TYPE0_DMA_LLP_LOW_REG_WRCH_0 */
#define TYPE0_DMA_LLP_LOW_REG_WRCH_0                                                                                  ( 0xA8C )
#define TYPE0_DMA_LLP_LOW_REG_WRCH_0_LLP_LOW_MASK                                                                     0xFFFFFFFF
#define TYPE0_DMA_LLP_LOW_REG_WRCH_0_LLP_LOW_SHFT                                                                     0x0

/* DMA_LLP_HIGH_REG_WRCH_0 */
#define DMA_LLP_HIGH_REG_WRCH_0                                                                                       ( 0xA90 )
#define DMA_LLP_HIGH_REG_WRCH_0_LLP_HIGH_MASK                                                                         0xFFFFFFFF
#define DMA_LLP_HIGH_REG_WRCH_0_LLP_HIGH_SHFT                                                                         0x0

/* TYPE0_DMA_LLP_HIGH_REG_WRCH_0 */
#define TYPE0_DMA_LLP_HIGH_REG_WRCH_0                                                                                 ( 0xA90 )
#define TYPE0_DMA_LLP_HIGH_REG_WRCH_0_LLP_HIGH_MASK                                                                   0xFFFFFFFF
#define TYPE0_DMA_LLP_HIGH_REG_WRCH_0_LLP_HIGH_SHFT                                                                   0x0

/* PL_LTR_LATENCY_REG */
#define PL_LTR_LATENCY_REG                                                                                            ( 0xB30 )
#define PL_LTR_LATENCY_REG_NO_SNOOP_LATENCY_REQUIRE_MASK                                                              0x80000000
#define PL_LTR_LATENCY_REG_NO_SNOOP_LATENCY_REQUIRE_SHFT                                                              0x1F
#define PL_LTR_LATENCY_REG_NO_SNOOP_LATENCY_SCALE_MASK                                                                0x1C000000
#define PL_LTR_LATENCY_REG_NO_SNOOP_LATENCY_SCALE_SHFT                                                                0x1A
#define PL_LTR_LATENCY_REG_NO_SNOOP_LATENCY_VALUE_MASK                                                                0x3FF0000
#define PL_LTR_LATENCY_REG_NO_SNOOP_LATENCY_VALUE_SHFT                                                                0x10
#define PL_LTR_LATENCY_REG_SNOOP_LATENCY_REQUIRE_MASK                                                                 0x8000
#define PL_LTR_LATENCY_REG_SNOOP_LATENCY_REQUIRE_SHFT                                                                 0xF
#define PL_LTR_LATENCY_REG_SNOOP_LATENCY_SCALE_MASK                                                                   0x1C00
#define PL_LTR_LATENCY_REG_SNOOP_LATENCY_SCALE_SHFT                                                                   0xA
#define PL_LTR_LATENCY_REG_SNOOP_LATENCY_VALUE_MASK                                                                   0x3FF
#define PL_LTR_LATENCY_REG_SNOOP_LATENCY_VALUE_SHFT                                                                   0x0

/* TYPE0_PL_LTR_LATENCY_REG */
#define TYPE0_PL_LTR_LATENCY_REG                                                                                      ( 0xB30 )
#define TYPE0_PL_LTR_LATENCY_REG_NO_SNOOP_LATENCY_REQUIRE_MASK                                                        0x80000000
#define TYPE0_PL_LTR_LATENCY_REG_NO_SNOOP_LATENCY_REQUIRE_SHFT                                                        0x1F
#define TYPE0_PL_LTR_LATENCY_REG_NO_SNOOP_LATENCY_SCALE_MASK                                                          0x1C000000
#define TYPE0_PL_LTR_LATENCY_REG_NO_SNOOP_LATENCY_SCALE_SHFT                                                          0x1A
#define TYPE0_PL_LTR_LATENCY_REG_NO_SNOOP_LATENCY_VALUE_MASK                                                          0x3FF0000
#define TYPE0_PL_LTR_LATENCY_REG_NO_SNOOP_LATENCY_VALUE_SHFT                                                          0x10
#define TYPE0_PL_LTR_LATENCY_REG_SNOOP_LATENCY_REQUIRE_MASK                                                           0x8000
#define TYPE0_PL_LTR_LATENCY_REG_SNOOP_LATENCY_REQUIRE_SHFT                                                           0xF
#define TYPE0_PL_LTR_LATENCY_REG_SNOOP_LATENCY_SCALE_MASK                                                             0x1C00
#define TYPE0_PL_LTR_LATENCY_REG_SNOOP_LATENCY_SCALE_SHFT                                                             0xA
#define TYPE0_PL_LTR_LATENCY_REG_SNOOP_LATENCY_VALUE_MASK                                                             0x3FF
#define TYPE0_PL_LTR_LATENCY_REG_SNOOP_LATENCY_VALUE_SHFT                                                             0x0

/* AUX_CLK_FREQ_REG */
#define AUX_CLK_FREQ_REG                                                                                              ( 0xB40 )
#define AUX_CLK_FREQ_REG_AUX_CLK_FREQ_MASK                                                                            0x3FF
#define AUX_CLK_FREQ_REG_AUX_CLK_FREQ_SHFT                                                                            0x0

/* TYPE0_AUX_CLK_FREQ_REG */
#define TYPE0_AUX_CLK_FREQ_REG                                                                                        ( 0xB40 )
#define TYPE0_AUX_CLK_FREQ_REG_AUX_CLK_FREQ_MASK                                                                      0x3FF
#define TYPE0_AUX_CLK_FREQ_REG_AUX_CLK_FREQ_SHFT                                                                      0x0

/* L1_SUBSTATES_REG */
#define L1_SUBSTATES_REG                                                                                              ( 0xB44 )
#define L1_SUBSTATES_REG_L1SUB_T_PCLKACK_MASK                                                                         0xC0
#define L1_SUBSTATES_REG_L1SUB_T_PCLKACK_SHFT                                                                         0x6
#define L1_SUBSTATES_REG_L1SUB_T_L1_2_MASK                                                                            0x3C
#define L1_SUBSTATES_REG_L1SUB_T_L1_2_SHFT                                                                            0x2
#define L1_SUBSTATES_REG_L1SUB_T_POWER_OFF_MASK                                                                       0x3
#define L1_SUBSTATES_REG_L1SUB_T_POWER_OFF_SHFT                                                                       0x0

/* TYPE0_L1_SUBSTATES_REG */
#define TYPE0_L1_SUBSTATES_REG                                                                                        ( 0xB44 )
#define TYPE0_L1_SUBSTATES_REG_L1SUB_T_PCLKACK_MASK                                                                   0xC0
#define TYPE0_L1_SUBSTATES_REG_L1SUB_T_PCLKACK_SHFT                                                                   0x6
#define TYPE0_L1_SUBSTATES_REG_L1SUB_T_L1_2_MASK                                                                      0x3C
#define TYPE0_L1_SUBSTATES_REG_L1SUB_T_L1_2_SHFT                                                                      0x2
#define TYPE0_L1_SUBSTATES_REG_L1SUB_T_POWER_OFF_MASK                                                                 0x3
#define TYPE0_L1_SUBSTATES_REG_L1SUB_T_POWER_OFF_SHFT                                                                 0x0


/*============================================================================
     PCIE_ELBI
============================================================================*/
     
#define PCIE_ELBI_BASE_PHYS                                                ( 0x7FFFEF20 )
#define PCIE_ELBI_SIZE                                                     ( 0xA8 )

/* PCIE20_ELBI_VERSION */
#define PCIE20_ELBI_VERSION                                                ( 0x0 )
#define PCIE20_ELBI_VERSION_IP_CAT_VERSION_MASK                            0xFFF
#define PCIE20_ELBI_VERSION_IP_CAT_VERSION_SHFT                            0x0

/* PCIE20_ELBI_SYS_CTRL */
#define PCIE20_ELBI_SYS_CTRL                                               ( 0x4 )
#define PCIE20_ELBI_SYS_CTRL_APPS_PM_XMT_PME_MASK                          0xF000
#define PCIE20_ELBI_SYS_CTRL_APPS_PM_XMT_PME_SHFT                          0xC
#define PCIE20_ELBI_SYS_CTRL_OUTBAND_PWRUP_CMD_MASK                        0xF00
#define PCIE20_ELBI_SYS_CTRL_OUTBAND_PWRUP_CMD_SHFT                        0x8
#define PCIE20_ELBI_SYS_CTRL_CLK_PM_EN_MASK                                0x80
#define PCIE20_ELBI_SYS_CTRL_CLK_PM_EN_SHFT                                0x7
#define PCIE20_ELBI_SYS_CTRL_INIT_RST_MASK                                 0x40
#define PCIE20_ELBI_SYS_CTRL_INIT_RST_SHFT                                 0x6
#define PCIE20_ELBI_SYS_CTRL_UNLOCK_MSG_MASK                               0x20
#define PCIE20_ELBI_SYS_CTRL_UNLOCK_MSG_SHFT                               0x5
#define PCIE20_ELBI_SYS_CTRL_PME_TURNOFF_MSG_MASK                          0x10
#define PCIE20_ELBI_SYS_CTRL_PME_TURNOFF_MSG_SHFT                          0x4

/* PCIE20_ELBI_SYS_STTS */
#define PCIE20_ELBI_SYS_STTS                                               ( 0x8 )
#define PCIE20_ELBI_SYS_STTS_PM_CURNT_STATE_MASK                           0x1C0000
#define PCIE20_ELBI_SYS_STTS_PM_CURNT_STATE_SHFT                           0x12
#define PCIE20_ELBI_SYS_STTS_XMLH_LTSSM_STATE_MASK                         0x3F000
#define PCIE20_ELBI_SYS_STTS_XMLH_LTSSM_STATE_SHFT                         0xC
#define PCIE20_ELBI_SYS_STTS_XMLH_IN_RL0S_MASK                             0x800
#define PCIE20_ELBI_SYS_STTS_XMLH_IN_RL0S_SHFT                             0xB
#define PCIE20_ELBI_SYS_STTS_XMLH_LINK_UP_MASK                             0x400
#define PCIE20_ELBI_SYS_STTS_XMLH_LINK_UP_SHFT                             0xA
#define PCIE20_ELBI_SYS_STTS_CFG_HW_AUTO_SP_DIS_MASK                       0x200
#define PCIE20_ELBI_SYS_STTS_CFG_HW_AUTO_SP_DIS_SHFT                       0x9
#define PCIE20_ELBI_SYS_STTS_CPL_TIMEOUT_MASK                              0x100
#define PCIE20_ELBI_SYS_STTS_CPL_TIMEOUT_SHFT                              0x8
#define PCIE20_ELBI_SYS_STTS_RADMX_RSP_CMPSR_ERR_MASK                      0x10
#define PCIE20_ELBI_SYS_STTS_RADMX_RSP_CMPSR_ERR_SHFT                      0x4
#define PCIE20_ELBI_SYS_STTS_BLK_TLP_MASK                                  0x4
#define PCIE20_ELBI_SYS_STTS_BLK_TLP_SHFT                                  0x2
#define PCIE20_ELBI_SYS_STTS_PME_TO_ACK_MASK                               0x2
#define PCIE20_ELBI_SYS_STTS_PME_TO_ACK_SHFT                               0x1
#define PCIE20_ELBI_SYS_STTS_LINK_REQ_RST_NOT_MASK                         0x1
#define PCIE20_ELBI_SYS_STTS_LINK_REQ_RST_NOT_SHFT                         0x0

/* PCIE20_ELBI_SYS_CLR */
#define PCIE20_ELBI_SYS_CLR                                                ( 0xC )
#define PCIE20_ELBI_SYS_CLR_CPL_TIMEOUT_MASK                               0x100
#define PCIE20_ELBI_SYS_CLR_CPL_TIMEOUT_SHFT                               0x8
#define PCIE20_ELBI_SYS_CLR_GM_RSP_CMPSR_ERR_MASK                          0x20
#define PCIE20_ELBI_SYS_CLR_GM_RSP_CMPSR_ERR_SHFT                          0x5
#define PCIE20_ELBI_SYS_CLR_RADMX_RSP_CMPSR_ERR_MASK                       0x10
#define PCIE20_ELBI_SYS_CLR_RADMX_RSP_CMPSR_ERR_SHFT                       0x4
#define PCIE20_ELBI_SYS_CLR_PME_TO_ACK_MASK                                0x2
#define PCIE20_ELBI_SYS_CLR_PME_TO_ACK_SHFT                                0x1

/* PCIE20_ELBI_TESTBUS_CTRL */
#define PCIE20_ELBI_TESTBUS_CTRL                                           ( 0x20 )
#define PCIE20_ELBI_TESTBUS_CTRL_DIAG_CTRL_BUS_MASK                        0x7
#define PCIE20_ELBI_TESTBUS_CTRL_DIAG_CTRL_BUS_SHFT                        0x0

/* PCIE20_ELBI_DEBUG_INFO_n */
#define PCIE20_ELBI_DEBUG_INFO_n(n)                                        ( 0x30 + 0x4 * (n) )
#define PCIE20_ELBI_DEBUG_INFO_n_BUS_MASK                                  0xFFFFFFFF
#define PCIE20_ELBI_DEBUG_INFO_n_BUS_SHFT                                  0x0

/* PCIE20_ELBI_TB_DIAG_n */
#define PCIE20_ELBI_TB_DIAG_n(n)                                           ( 0x40 + 0x4 * (n) )
#define PCIE20_ELBI_TB_DIAG_n_BUS_MASK                                     0xFFFFFFFF
#define PCIE20_ELBI_TB_DIAG_n_BUS_SHFT                                     0x0

/* PCIE20_ELBI_DEBUG_INFO_EI */
#define PCIE20_ELBI_DEBUG_INFO_EI                                          ( 0xA0 )
#define PCIE20_ELBI_DEBUG_INFO_EI_BUS_MASK                                 0xFFFF
#define PCIE20_ELBI_DEBUG_INFO_EI_BUS_SHFT                                 0x0

/* PCIE20_ELBI_CS2_ENABLE */
#define PCIE20_ELBI_CS2_ENABLE                                             ( 0xA4 )
#define PCIE20_ELBI_CS2_ENABLE_ENABLE_MASK                                 0x1
#define PCIE20_ELBI_CS2_ENABLE_ENABLE_SHFT                                 0x0

/*============================================================================
     PCIE_EP
============================================================================*/

#define PCIE_EP_BASE_PHYS    ( 0x40000000 )
#define PCIE_EP_SIZE         ( 0x3FFFE000 )

#endif /* #ifndef PCIE_HWIO_H */

