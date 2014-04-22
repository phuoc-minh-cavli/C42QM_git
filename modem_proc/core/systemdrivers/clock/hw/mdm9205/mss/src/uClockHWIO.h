#ifndef __UCLOCKHWIO_H__
#define __UCLOCKHWIO_H__
/*
===========================================================================
*/
/**
  @file uClockHWIO.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    MDM9205 (Twizy) [twizy_v1.0_p3q3r35_MTO]
 
  This file contains HWIO register definitions for the following modules:
    MSS_QDSP6V67SS_PUB
    GCC_CLK_CTL_REG

  'Include' filters applied: PLL[MSS_QDSP6V67SS_PUB] RCG[MSS_QDSP6V67SS_PUB] GPLL0_MODE[GCC_CLK_CTL_REG] UART[GCC_CLK_CTL_REG] BLSP1_AHB[GCC_CLK_CTL_REG] MSS_Q6_GPLL_ENA_VOTE[GCC_CLK_CTL_REG] 
  'Exclude' filters applied: PLL_RCG[MSS_QDSP6V67SS_PUB] UART_SIM[GCC_CLK_CTL_REG] 

  Generation parameters: 
  { 'filename': 'uClockHWIO.h',
    'header': '#include "msmhwiobase.h"',
    'module-filter-exclude': { 'GCC_CLK_CTL_REG': ['UART_SIM'],
                               'MSS_QDSP6V67SS_PUB': ['PLL_RCG']},
    'module-filter-include': { 'GCC_CLK_CTL_REG': [ 'GPLL0_MODE',
                                                    'UART',
                                                    'BLSP1_AHB',
                                                    'MSS_Q6_GPLL_ENA_VOTE'],
                               'MSS_QDSP6V67SS_PUB': ['PLL', 'RCG']},
    'modules': ['MSS_QDSP6V67SS_PUB', 'GCC_CLK_CTL_REG'],
    'rmsk-input': True}
*/
/*
  ===========================================================================

  Copyright (c) 2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  Export of this technology or software is regulated by the U.S. Government.
  Diversion contrary to U.S. law prohibited.

  All ideas, data and information contained in or disclosed by
  this document are confidential and proprietary information of
  Qualcomm Technologies, Inc. and all rights therein are expressly reserved.
  By accepting this material the recipient agrees that this material
  and the information contained therein are held in confidence and in
  trust and will not be used, copied, reproduced in whole or in part,
  nor its contents revealed in any manner to others without the express
  written permission of Qualcomm Technologies, Inc.

  ===========================================================================

  $Header: //components/rel/core.mpss/3.10/systemdrivers/clock/hw/mdm9205/mss/src/uClockHWIO.h#2 $
  $DateTime: 2019/09/19 05:28:49 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: MSS_QDSP6V67SS_PUB
 *--------------------------------------------------------------------------*/

#define MSS_QDSP6V67SS_PUB_REG_BASE                                               (MSS_TOP_BASE      + 0x00080000)
#define MSS_QDSP6V67SS_PUB_REG_BASE_SIZE                                          0x4040
#define MSS_QDSP6V67SS_PUB_REG_BASE_USED                                          0x2030

#define HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_ADDR                                       (MSS_QDSP6V67SS_PUB_REG_BASE      + 0x00000028)
#define HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_RMSK                                       0x80000013
#define HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_ADDR, HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_RMSK)
#define HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_ADDR, m)
#define HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_ADDR,v)
#define HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_ADDR,m,v,HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_IN)
#define HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_ROOT_OFF_BMSK                              0x80000000
#define HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_ROOT_OFF_SHFT                                    0x1f
#define HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                              0x10
#define HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                               0x4
#define HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_ROOT_EN_BMSK                                      0x2
#define HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_ROOT_EN_SHFT                                      0x1
#define HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_UPDATE_BMSK                                       0x1
#define HWIO_MSS_QDSP6SS_CORE_CMD_RCGR_UPDATE_SHFT                                       0x0

#define HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_ADDR                                       (MSS_QDSP6V67SS_PUB_REG_BASE      + 0x0000002c)
#define HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_RMSK                                         0x11071f
#define HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_ADDR, HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_RMSK)
#define HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_ADDR, m)
#define HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_ADDR,v)
#define HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_ADDR,m,v,HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_IN)
#define HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_HW_CLK_CONTROL_BMSK                          0x100000
#define HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_HW_CLK_CONTROL_SHFT                              0x14
#define HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_RCGLITE_DISABLE_BMSK                          0x10000
#define HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_RCGLITE_DISABLE_SHFT                             0x10
#define HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_SRC_SEL_BMSK                                    0x700
#define HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_SRC_SEL_SHFT                                      0x8
#define HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_SRC_DIV_BMSK                                     0x1f
#define HWIO_MSS_QDSP6SS_CORE_CFG_RCGR_SRC_DIV_SHFT                                      0x0

#define HWIO_MSS_QDSP6SS_PLL_MODE_ADDR                                            (MSS_QDSP6V67SS_PUB_REG_BASE      + 0x00000200)
#define HWIO_MSS_QDSP6SS_PLL_MODE_RMSK                                            0xffffffff
#define HWIO_MSS_QDSP6SS_PLL_MODE_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_MODE_ADDR, HWIO_MSS_QDSP6SS_PLL_MODE_RMSK)
#define HWIO_MSS_QDSP6SS_PLL_MODE_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_MODE_ADDR, m)
#define HWIO_MSS_QDSP6SS_PLL_MODE_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PLL_MODE_ADDR,v)
#define HWIO_MSS_QDSP6SS_PLL_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_PLL_MODE_ADDR,m,v,HWIO_MSS_QDSP6SS_PLL_MODE_IN)
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_LOCK_DET_BMSK                               0x80000000
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_LOCK_DET_SHFT                                     0x1f
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_ACTIVE_FLAG_BMSK                            0x40000000
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_ACTIVE_FLAG_SHFT                                  0x1e
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_ACK_LATCH_BMSK                              0x20000000
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_ACK_LATCH_SHFT                                    0x1d
#define HWIO_MSS_QDSP6SS_PLL_MODE_RESERVE_BITS28_24_BMSK                          0x1f000000
#define HWIO_MSS_QDSP6SS_PLL_MODE_RESERVE_BITS28_24_SHFT                                0x18
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_BMSK                   0x800000
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_SHFT                       0x17
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_UPDATE_BMSK                                   0x400000
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_UPDATE_SHFT                                       0x16
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_VOTE_FSM_RESET_BMSK                           0x200000
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_VOTE_FSM_RESET_SHFT                               0x15
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_VOTE_FSM_ENA_BMSK                             0x100000
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_VOTE_FSM_ENA_SHFT                                 0x14
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_BIAS_COUNT_BMSK                                0xfc000
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_BIAS_COUNT_SHFT                                    0xe
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_LOCK_COUNT_BMSK                                 0x3f00
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_LOCK_COUNT_SHFT                                    0x8
#define HWIO_MSS_QDSP6SS_PLL_MODE_RESERVE_BITS7_4_BMSK                                  0xf0
#define HWIO_MSS_QDSP6SS_PLL_MODE_RESERVE_BITS7_4_SHFT                                   0x4
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_PLLTEST_BMSK                                       0x8
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_PLLTEST_SHFT                                       0x3
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_RESET_N_BMSK                                       0x4
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_RESET_N_SHFT                                       0x2
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_BYPASSNL_BMSK                                      0x2
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_BYPASSNL_SHFT                                      0x1
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_OUTCTRL_BMSK                                       0x1
#define HWIO_MSS_QDSP6SS_PLL_MODE_PLL_OUTCTRL_SHFT                                       0x0

#define HWIO_MSS_QDSP6SS_PLL_L_VAL_ADDR                                           (MSS_QDSP6V67SS_PUB_REG_BASE      + 0x00000204)
#define HWIO_MSS_QDSP6SS_PLL_L_VAL_RMSK                                               0xffff
#define HWIO_MSS_QDSP6SS_PLL_L_VAL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_L_VAL_ADDR, HWIO_MSS_QDSP6SS_PLL_L_VAL_RMSK)
#define HWIO_MSS_QDSP6SS_PLL_L_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_L_VAL_ADDR, m)
#define HWIO_MSS_QDSP6SS_PLL_L_VAL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PLL_L_VAL_ADDR,v)
#define HWIO_MSS_QDSP6SS_PLL_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_PLL_L_VAL_ADDR,m,v,HWIO_MSS_QDSP6SS_PLL_L_VAL_IN)
#define HWIO_MSS_QDSP6SS_PLL_L_VAL_PLL_L_BMSK                                         0xffff
#define HWIO_MSS_QDSP6SS_PLL_L_VAL_PLL_L_SHFT                                            0x0

#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_ADDR                                       (MSS_QDSP6V67SS_PUB_REG_BASE      + 0x00000208)
#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_RMSK                                       0xffffffff
#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_ADDR, HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_RMSK)
#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_ADDR, m)
#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_ADDR,v)
#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_ADDR,m,v,HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_IN)
#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_PLL_ALPHA_31_0_BMSK                        0xffffffff
#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_PLL_ALPHA_31_0_SHFT                               0x0

#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_U_ADDR                                     (MSS_QDSP6V67SS_PUB_REG_BASE      + 0x0000020c)
#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_U_RMSK                                           0xff
#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_U_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_U_ADDR, HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_U_RMSK)
#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_U_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_U_ADDR, m)
#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_U_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_U_ADDR,v)
#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_U_ADDR,m,v,HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_U_IN)
#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_U_PLL_ALPHA_39_32_BMSK                           0xff
#define HWIO_MSS_QDSP6SS_PLL_ALPHA_VAL_U_PLL_ALPHA_39_32_SHFT                            0x0

#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_ADDR                                        (MSS_QDSP6V67SS_PUB_REG_BASE      + 0x00000210)
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_RMSK                                        0xffffffff
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_USER_CTL_ADDR, HWIO_MSS_QDSP6SS_PLL_USER_CTL_RMSK)
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_USER_CTL_ADDR, m)
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PLL_USER_CTL_ADDR,v)
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_PLL_USER_CTL_ADDR,m,v,HWIO_MSS_QDSP6SS_PLL_USER_CTL_IN)
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_RESERVE_BITS31_28_BMSK                      0xf0000000
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_RESERVE_BITS31_28_SHFT                            0x1c
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_SSC_MODE_CONTROL_BMSK                        0x8000000
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_SSC_MODE_CONTROL_SHFT                             0x1b
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_RESERVE_BITS26_25_BMSK                       0x6000000
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_RESERVE_BITS26_25_SHFT                            0x19
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_ALPHA_EN_BMSK                                0x1000000
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_ALPHA_EN_SHFT                                     0x18
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_RESERVE_BITS23_22_BMSK                        0xc00000
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_RESERVE_BITS23_22_SHFT                            0x16
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_VCO_SEL_BMSK                                  0x300000
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_VCO_SEL_SHFT                                      0x14
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_RESERVE_BITS19_15_BMSK                         0xf8000
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_RESERVE_BITS19_15_SHFT                             0xf
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_PRE_DIV_RATIO_BMSK                              0x7000
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_PRE_DIV_RATIO_SHFT                                 0xc
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_POST_DIV_RATIO_BMSK                              0xf00
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_POST_DIV_RATIO_SHFT                                0x8
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_OUTPUT_INV_BMSK                                   0x80
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_OUTPUT_INV_SHFT                                    0x7
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_RESERVE_BITS6_5_BMSK                              0x60
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_RESERVE_BITS6_5_SHFT                               0x5
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_TEST_BMSK                               0x10
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_TEST_SHFT                                0x4
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_EARLY_BMSK                               0x8
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_EARLY_SHFT                               0x3
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_AUX2_BMSK                                0x4
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_AUX2_SHFT                                0x2
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_AUX_BMSK                                 0x2
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_AUX_SHFT                                 0x1
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_MAIN_BMSK                                0x1
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_MAIN_SHFT                                0x0

#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_ADDR                                      (MSS_QDSP6V67SS_PUB_REG_BASE      + 0x00000214)
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_RMSK                                      0xffffffff
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_ADDR, HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_RMSK)
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_ADDR, m)
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_ADDR,v)
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_ADDR,m,v,HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_IN)
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_CALIBRATION_L_BMSK                        0xffff0000
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_CALIBRATION_L_SHFT                              0x10
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_RESERVE_BITS15_12_BMSK                        0xf000
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_RESERVE_BITS15_12_SHFT                           0xc
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_LATCH_INTERFACE_BYPASS_BMSK                    0x800
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_LATCH_INTERFACE_BYPASS_SHFT                      0xb
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_STATUS_REGISTER_BMSK                           0x700
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_STATUS_REGISTER_SHFT                             0x8
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_DSM_BMSK                                        0x80
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_DSM_SHFT                                         0x7
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_WRITE_STATE_BMSK                                0x40
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_WRITE_STATE_SHFT                                 0x6
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_TARGET_CTL_BMSK                                 0x38
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_TARGET_CTL_SHFT                                  0x3
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_LOCK_DET_BMSK                                    0x4
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_LOCK_DET_SHFT                                    0x2
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_FREEZE_PLL_BMSK                                  0x2
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_FREEZE_PLL_SHFT                                  0x1
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_TOGGLE_DET_BMSK                                  0x1
#define HWIO_MSS_QDSP6SS_PLL_USER_CTL_U_TOGGLE_DET_SHFT                                  0x0

#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_ADDR                                      (MSS_QDSP6V67SS_PUB_REG_BASE      + 0x00000218)
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_RMSK                                      0xffffffff
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_ADDR, HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_RMSK)
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_ADDR, m)
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_ADDR,v)
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_ADDR,m,v,HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_IN)
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_SINGLE_DMET_MODE_ENABLE_BMSK              0x80000000
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_SINGLE_DMET_MODE_ENABLE_SHFT                    0x1f
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_DMET_WINDOW_ENABLE_BMSK                   0x40000000
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_DMET_WINDOW_ENABLE_SHFT                         0x1e
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_TOGGLE_DET_SAMPLE_INTER_BMSK              0x3c000000
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_TOGGLE_DET_SAMPLE_INTER_SHFT                    0x1a
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_TOGGLE_DET_THRESHOLD_BMSK                  0x3800000
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_TOGGLE_DET_THRESHOLD_SHFT                       0x17
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_TOGGLE_DET_SAMPLE_BMSK                      0x700000
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_TOGGLE_DET_SAMPLE_SHFT                          0x14
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_LOCK_DET_THRESHOLD_BMSK                      0xff000
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_LOCK_DET_THRESHOLD_SHFT                          0xc
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_LOCK_DET_SAMPLE_SIZE_BMSK                      0xf00
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_LOCK_DET_SAMPLE_SIZE_SHFT                        0x8
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_GLITCH_THRESHOLD_BMSK                           0xc0
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_GLITCH_THRESHOLD_SHFT                            0x6
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_REF_CYCLE_BMSK                                  0x30
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_REF_CYCLE_SHFT                                   0x4
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_KFN_BMSK                                         0xf
#define HWIO_MSS_QDSP6SS_PLL_CONFIG_CTL_KFN_SHFT                                         0x0

#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_ADDR                                        (MSS_QDSP6V67SS_PUB_REG_BASE      + 0x0000021c)
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_RMSK                                        0xffffffff
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_TEST_CTL_ADDR, HWIO_MSS_QDSP6SS_PLL_TEST_CTL_RMSK)
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_TEST_CTL_ADDR, m)
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PLL_TEST_CTL_ADDR,v)
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_PLL_TEST_CTL_ADDR,m,v,HWIO_MSS_QDSP6SS_PLL_TEST_CTL_IN)
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_BIAS_GEN_TRIM_BMSK                          0xe0000000
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_BIAS_GEN_TRIM_SHFT                                0x1d
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_DCO_BMSK                                    0x10000000
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_DCO_SHFT                                          0x1c
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_PROCESS_CALB_BMSK                            0xc000000
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_PROCESS_CALB_SHFT                                 0x1a
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_OVERRIDE_PROCESS_CALB_BMSK                   0x2000000
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_OVERRIDE_PROCESS_CALB_SHFT                        0x19
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_FINE_FCW_BMSK                                0x1e00000
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_FINE_FCW_SHFT                                     0x15
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_OVERRIDE_FINE_FCW_BMSK                        0x100000
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_OVERRIDE_FINE_FCW_SHFT                            0x14
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_COARSE_FCW_BMSK                                0xfe000
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_COARSE_FCW_SHFT                                    0xd
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_OVERRIDE_COARSE_BMSK                            0x1000
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_OVERRIDE_COARSE_SHFT                               0xc
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_DISABLE_LFSR_BMSK                                0x800
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_DISABLE_LFSR_SHFT                                  0xb
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_DTEST_SEL_BMSK                                   0x700
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_DTEST_SEL_SHFT                                     0x8
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_DTEST_EN_BMSK                                     0x80
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_DTEST_EN_SHFT                                      0x7
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_BYP_TESTAMP_BMSK                                  0x40
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_BYP_TESTAMP_SHFT                                   0x6
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_ATEST1_SEL_BMSK                                   0x30
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_ATEST1_SEL_SHFT                                    0x4
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_ATEST0_SEL_BMSK                                    0xc
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_ATEST0_SEL_SHFT                                    0x2
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_ATEST1_EN_BMSK                                     0x2
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_ATEST1_EN_SHFT                                     0x1
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_ATEST0_EN_BMSK                                     0x1
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_ATEST0_EN_SHFT                                     0x0

#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_ADDR                                      (MSS_QDSP6V67SS_PUB_REG_BASE      + 0x00000220)
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_RMSK                                      0xffffffff
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_ADDR, HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_RMSK)
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_ADDR, m)
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_ADDR,v)
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_ADDR,m,v,HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_IN)
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_RESERVE_BITS31_14_BMSK                    0xffffc000
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_RESERVE_BITS31_14_SHFT                           0xe
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_OVERRIDE_FINE_FCW_MSB_BMSK                    0x2000
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_OVERRIDE_FINE_FCW_MSB_SHFT                       0xd
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_DTEST_MODE_SEL_BMSK                           0x1800
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_DTEST_MODE_SEL_SHFT                              0xb
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_NMO_OSC_SEL_BMSK                               0x600
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_NMO_OSC_SEL_SHFT                                 0x9
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_NMO_EN_BMSK                                    0x100
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_NMO_EN_SHFT                                      0x8
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_NOISE_MAG_BMSK                                  0xe0
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_NOISE_MAG_SHFT                                   0x5
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_NOISE_GEN_BMSK                                  0x10
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_NOISE_GEN_SHFT                                   0x4
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_OSC_BIAS_GND_BMSK                                0x8
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_OSC_BIAS_GND_SHFT                                0x3
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_PLL_TEST_OUT_SEL_BMSK                            0x6
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_PLL_TEST_OUT_SEL_SHFT                            0x1
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_CAL_CODE_UPDATE_BMSK                             0x1
#define HWIO_MSS_QDSP6SS_PLL_TEST_CTL_U_CAL_CODE_UPDATE_SHFT                             0x0

#define HWIO_MSS_QDSP6SS_PLL_STATUS_ADDR                                          (MSS_QDSP6V67SS_PUB_REG_BASE      + 0x00000224)
#define HWIO_MSS_QDSP6SS_PLL_STATUS_RMSK                                          0xffffffff
#define HWIO_MSS_QDSP6SS_PLL_STATUS_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_STATUS_ADDR, HWIO_MSS_QDSP6SS_PLL_STATUS_RMSK)
#define HWIO_MSS_QDSP6SS_PLL_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_STATUS_ADDR, m)
#define HWIO_MSS_QDSP6SS_PLL_STATUS_STATUS_31_0_BMSK                              0xffffffff
#define HWIO_MSS_QDSP6SS_PLL_STATUS_STATUS_31_0_SHFT                                     0x0

#define HWIO_MSS_QDSP6SS_PLL_FREQ_CTL_ADDR                                        (MSS_QDSP6V67SS_PUB_REG_BASE      + 0x00000228)
#define HWIO_MSS_QDSP6SS_PLL_FREQ_CTL_RMSK                                        0xffffffff
#define HWIO_MSS_QDSP6SS_PLL_FREQ_CTL_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_FREQ_CTL_ADDR, HWIO_MSS_QDSP6SS_PLL_FREQ_CTL_RMSK)
#define HWIO_MSS_QDSP6SS_PLL_FREQ_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_PLL_FREQ_CTL_ADDR, m)
#define HWIO_MSS_QDSP6SS_PLL_FREQ_CTL_OUT(v)      \
        out_dword(HWIO_MSS_QDSP6SS_PLL_FREQ_CTL_ADDR,v)
#define HWIO_MSS_QDSP6SS_PLL_FREQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_PLL_FREQ_CTL_ADDR,m,v,HWIO_MSS_QDSP6SS_PLL_FREQ_CTL_IN)
#define HWIO_MSS_QDSP6SS_PLL_FREQ_CTL_FREQUENCY_CTL_WORD_BMSK                     0xffffffff
#define HWIO_MSS_QDSP6SS_PLL_FREQ_CTL_FREQUENCY_CTL_WORD_SHFT                            0x0

/*----------------------------------------------------------------------------
 * MODULE: GCC_CLK_CTL_REG
 *--------------------------------------------------------------------------*/

#define GCC_CLK_CTL_REG_REG_BASE                                                              (CLK_CTL_BASE      + 0x00000000)
#define GCC_CLK_CTL_REG_REG_BASE_SIZE                                                         0x80000
#define GCC_CLK_CTL_REG_REG_BASE_USED                                                         0x7f000

#define HWIO_GCC_GPLL0_MODE_ADDR                                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00021000)
#define HWIO_GCC_GPLL0_MODE_RMSK                                                              0xe0ffff0f
#define HWIO_GCC_GPLL0_MODE_IN          \
        in_dword_masked(HWIO_GCC_GPLL0_MODE_ADDR, HWIO_GCC_GPLL0_MODE_RMSK)
#define HWIO_GCC_GPLL0_MODE_INM(m)      \
        in_dword_masked(HWIO_GCC_GPLL0_MODE_ADDR, m)
#define HWIO_GCC_GPLL0_MODE_OUT(v)      \
        out_dword(HWIO_GCC_GPLL0_MODE_ADDR,v)
#define HWIO_GCC_GPLL0_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_GPLL0_MODE_ADDR,m,v,HWIO_GCC_GPLL0_MODE_IN)
#define HWIO_GCC_GPLL0_MODE_PLL_LOCK_DET_BMSK                                                 0x80000000
#define HWIO_GCC_GPLL0_MODE_PLL_LOCK_DET_SHFT                                                       0x1f
#define HWIO_GCC_GPLL0_MODE_PLL_ACTIVE_FLAG_BMSK                                              0x40000000
#define HWIO_GCC_GPLL0_MODE_PLL_ACTIVE_FLAG_SHFT                                                    0x1e
#define HWIO_GCC_GPLL0_MODE_PLL_ACK_LATCH_BMSK                                                0x20000000
#define HWIO_GCC_GPLL0_MODE_PLL_ACK_LATCH_SHFT                                                      0x1d
#define HWIO_GCC_GPLL0_MODE_PLL_HW_UPADATE_LOGIC_BYPASS_BMSK                                    0x800000
#define HWIO_GCC_GPLL0_MODE_PLL_HW_UPADATE_LOGIC_BYPASS_SHFT                                        0x17
#define HWIO_GCC_GPLL0_MODE_PLL_UPDATE_BMSK                                                     0x400000
#define HWIO_GCC_GPLL0_MODE_PLL_UPDATE_SHFT                                                         0x16
#define HWIO_GCC_GPLL0_MODE_PLL_VOTE_FSM_RESET_BMSK                                             0x200000
#define HWIO_GCC_GPLL0_MODE_PLL_VOTE_FSM_RESET_SHFT                                                 0x15
#define HWIO_GCC_GPLL0_MODE_PLL_VOTE_FSM_ENA_BMSK                                               0x100000
#define HWIO_GCC_GPLL0_MODE_PLL_VOTE_FSM_ENA_SHFT                                                   0x14
#define HWIO_GCC_GPLL0_MODE_PLL_BIAS_COUNT_BMSK                                                  0xfc000
#define HWIO_GCC_GPLL0_MODE_PLL_BIAS_COUNT_SHFT                                                      0xe
#define HWIO_GCC_GPLL0_MODE_PLL_LOCK_COUNT_BMSK                                                   0x3f00
#define HWIO_GCC_GPLL0_MODE_PLL_LOCK_COUNT_SHFT                                                      0x8
#define HWIO_GCC_GPLL0_MODE_PLL_PLLTEST_BMSK                                                         0x8
#define HWIO_GCC_GPLL0_MODE_PLL_PLLTEST_SHFT                                                         0x3
#define HWIO_GCC_GPLL0_MODE_PLL_RESET_N_BMSK                                                         0x4
#define HWIO_GCC_GPLL0_MODE_PLL_RESET_N_SHFT                                                         0x2
#define HWIO_GCC_GPLL0_MODE_PLL_BYPASSNL_BMSK                                                        0x2
#define HWIO_GCC_GPLL0_MODE_PLL_BYPASSNL_SHFT                                                        0x1
#define HWIO_GCC_GPLL0_MODE_PLL_OUTCTRL_BMSK                                                         0x1
#define HWIO_GCC_GPLL0_MODE_PLL_OUTCTRL_SHFT                                                         0x0

#define HWIO_GCC_BLSP1_AHB_CBCR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x00001008)
#define HWIO_GCC_BLSP1_AHB_CBCR_RMSK                                                          0xf000fff0
#define HWIO_GCC_BLSP1_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_AHB_CBCR_ADDR, HWIO_GCC_BLSP1_AHB_CBCR_RMSK)
#define HWIO_GCC_BLSP1_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_AHB_CBCR_ADDR, m)
#define HWIO_GCC_BLSP1_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_AHB_CBCR_ADDR,v)
#define HWIO_GCC_BLSP1_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_AHB_CBCR_ADDR,m,v,HWIO_GCC_BLSP1_AHB_CBCR_IN)
#define HWIO_GCC_BLSP1_AHB_CBCR_CLK_OFF_BMSK                                                  0x80000000
#define HWIO_GCC_BLSP1_AHB_CBCR_CLK_OFF_SHFT                                                        0x1f
#define HWIO_GCC_BLSP1_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                 0x70000000
#define HWIO_GCC_BLSP1_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                       0x1c
#define HWIO_GCC_BLSP1_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                         0x8000
#define HWIO_GCC_BLSP1_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                            0xf
#define HWIO_GCC_BLSP1_AHB_CBCR_FORCE_MEM_CORE_ON_BMSK                                            0x4000
#define HWIO_GCC_BLSP1_AHB_CBCR_FORCE_MEM_CORE_ON_SHFT                                               0xe
#define HWIO_GCC_BLSP1_AHB_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                          0x2000
#define HWIO_GCC_BLSP1_AHB_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                             0xd
#define HWIO_GCC_BLSP1_AHB_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                         0x1000
#define HWIO_GCC_BLSP1_AHB_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                            0xc
#define HWIO_GCC_BLSP1_AHB_CBCR_WAKEUP_BMSK                                                        0xf00
#define HWIO_GCC_BLSP1_AHB_CBCR_WAKEUP_SHFT                                                          0x8
#define HWIO_GCC_BLSP1_AHB_CBCR_SLEEP_BMSK                                                          0xf0
#define HWIO_GCC_BLSP1_AHB_CBCR_SLEEP_SHFT                                                           0x4

#define HWIO_GCC_BLSP1_UART1_BCR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x00002038)
#define HWIO_GCC_BLSP1_UART1_BCR_RMSK                                                                0x1
#define HWIO_GCC_BLSP1_UART1_BCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_BCR_ADDR, HWIO_GCC_BLSP1_UART1_BCR_RMSK)
#define HWIO_GCC_BLSP1_UART1_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_BCR_ADDR, m)
#define HWIO_GCC_BLSP1_UART1_BCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART1_BCR_ADDR,v)
#define HWIO_GCC_BLSP1_UART1_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART1_BCR_ADDR,m,v,HWIO_GCC_BLSP1_UART1_BCR_IN)
#define HWIO_GCC_BLSP1_UART1_BCR_BLK_ARES_BMSK                                                       0x1
#define HWIO_GCC_BLSP1_UART1_BCR_BLK_ARES_SHFT                                                       0x0

#define HWIO_GCC_BLSP1_UART1_APPS_CBCR_ADDR                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x0000203c)
#define HWIO_GCC_BLSP1_UART1_APPS_CBCR_RMSK                                                   0x80000001
#define HWIO_GCC_BLSP1_UART1_APPS_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_APPS_CBCR_ADDR, HWIO_GCC_BLSP1_UART1_APPS_CBCR_RMSK)
#define HWIO_GCC_BLSP1_UART1_APPS_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_APPS_CBCR_ADDR, m)
#define HWIO_GCC_BLSP1_UART1_APPS_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART1_APPS_CBCR_ADDR,v)
#define HWIO_GCC_BLSP1_UART1_APPS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART1_APPS_CBCR_ADDR,m,v,HWIO_GCC_BLSP1_UART1_APPS_CBCR_IN)
#define HWIO_GCC_BLSP1_UART1_APPS_CBCR_CLK_OFF_BMSK                                           0x80000000
#define HWIO_GCC_BLSP1_UART1_APPS_CBCR_CLK_OFF_SHFT                                                 0x1f
#define HWIO_GCC_BLSP1_UART1_APPS_CBCR_CLK_ENABLE_BMSK                                               0x1
#define HWIO_GCC_BLSP1_UART1_APPS_CBCR_CLK_ENABLE_SHFT                                               0x0

#define HWIO_GCC_BLSP1_UART1_SIM_CBCR_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00002040)
#define HWIO_GCC_BLSP1_UART1_SIM_CBCR_RMSK                                                    0x80000001
#define HWIO_GCC_BLSP1_UART1_SIM_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_SIM_CBCR_ADDR, HWIO_GCC_BLSP1_UART1_SIM_CBCR_RMSK)
#define HWIO_GCC_BLSP1_UART1_SIM_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_SIM_CBCR_ADDR, m)
#define HWIO_GCC_BLSP1_UART1_SIM_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART1_SIM_CBCR_ADDR,v)
#define HWIO_GCC_BLSP1_UART1_SIM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART1_SIM_CBCR_ADDR,m,v,HWIO_GCC_BLSP1_UART1_SIM_CBCR_IN)
#define HWIO_GCC_BLSP1_UART1_SIM_CBCR_CLK_OFF_BMSK                                            0x80000000
#define HWIO_GCC_BLSP1_UART1_SIM_CBCR_CLK_OFF_SHFT                                                  0x1f
#define HWIO_GCC_BLSP1_UART1_SIM_CBCR_CLK_ENABLE_BMSK                                                0x1
#define HWIO_GCC_BLSP1_UART1_SIM_CBCR_CLK_ENABLE_SHFT                                                0x0

#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00002044)
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_RMSK                                               0x800000f3
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_ADDR, HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_RMSK)
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_IN)
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_ROOT_OFF_BMSK                                      0x80000000
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_ROOT_OFF_SHFT                                            0x1f
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_DIRTY_D_BMSK                                             0x80
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_DIRTY_D_SHFT                                              0x7
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_DIRTY_M_BMSK                                             0x40
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_DIRTY_M_SHFT                                              0x6
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_DIRTY_N_BMSK                                             0x20
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_DIRTY_N_SHFT                                              0x5
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                      0x10
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                       0x4
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_ROOT_EN_BMSK                                              0x2
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_ROOT_EN_SHFT                                              0x1
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_UPDATE_BMSK                                               0x1
#define HWIO_GCC_BLSP1_UART1_APPS_CMD_RCGR_UPDATE_SHFT                                               0x0

#define HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00002048)
#define HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_RMSK                                                   0x371f
#define HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_ADDR, HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_RMSK)
#define HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_IN)
#define HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_MODE_BMSK                                              0x3000
#define HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_MODE_SHFT                                                 0xc
#define HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_SRC_SEL_BMSK                                            0x700
#define HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_SRC_SEL_SHFT                                              0x8
#define HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_SRC_DIV_BMSK                                             0x1f
#define HWIO_GCC_BLSP1_UART1_APPS_CFG_RCGR_SRC_DIV_SHFT                                              0x0

#define HWIO_GCC_BLSP1_UART1_APPS_M_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x0000204c)
#define HWIO_GCC_BLSP1_UART1_APPS_M_RMSK                                                          0xffff
#define HWIO_GCC_BLSP1_UART1_APPS_M_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_APPS_M_ADDR, HWIO_GCC_BLSP1_UART1_APPS_M_RMSK)
#define HWIO_GCC_BLSP1_UART1_APPS_M_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_APPS_M_ADDR, m)
#define HWIO_GCC_BLSP1_UART1_APPS_M_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART1_APPS_M_ADDR,v)
#define HWIO_GCC_BLSP1_UART1_APPS_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART1_APPS_M_ADDR,m,v,HWIO_GCC_BLSP1_UART1_APPS_M_IN)
#define HWIO_GCC_BLSP1_UART1_APPS_M_M_BMSK                                                        0xffff
#define HWIO_GCC_BLSP1_UART1_APPS_M_M_SHFT                                                           0x0

#define HWIO_GCC_BLSP1_UART1_APPS_N_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00002050)
#define HWIO_GCC_BLSP1_UART1_APPS_N_RMSK                                                          0xffff
#define HWIO_GCC_BLSP1_UART1_APPS_N_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_APPS_N_ADDR, HWIO_GCC_BLSP1_UART1_APPS_N_RMSK)
#define HWIO_GCC_BLSP1_UART1_APPS_N_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_APPS_N_ADDR, m)
#define HWIO_GCC_BLSP1_UART1_APPS_N_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART1_APPS_N_ADDR,v)
#define HWIO_GCC_BLSP1_UART1_APPS_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART1_APPS_N_ADDR,m,v,HWIO_GCC_BLSP1_UART1_APPS_N_IN)
#define HWIO_GCC_BLSP1_UART1_APPS_N_NOT_N_MINUS_M_BMSK                                            0xffff
#define HWIO_GCC_BLSP1_UART1_APPS_N_NOT_N_MINUS_M_SHFT                                               0x0

#define HWIO_GCC_BLSP1_UART1_APPS_D_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00002054)
#define HWIO_GCC_BLSP1_UART1_APPS_D_RMSK                                                          0xffff
#define HWIO_GCC_BLSP1_UART1_APPS_D_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_APPS_D_ADDR, HWIO_GCC_BLSP1_UART1_APPS_D_RMSK)
#define HWIO_GCC_BLSP1_UART1_APPS_D_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART1_APPS_D_ADDR, m)
#define HWIO_GCC_BLSP1_UART1_APPS_D_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART1_APPS_D_ADDR,v)
#define HWIO_GCC_BLSP1_UART1_APPS_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART1_APPS_D_ADDR,m,v,HWIO_GCC_BLSP1_UART1_APPS_D_IN)
#define HWIO_GCC_BLSP1_UART1_APPS_D_NOT_2D_BMSK                                                   0xffff
#define HWIO_GCC_BLSP1_UART1_APPS_D_NOT_2D_SHFT                                                      0x0

#define HWIO_GCC_BLSP1_UART2_BCR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x00003028)
#define HWIO_GCC_BLSP1_UART2_BCR_RMSK                                                                0x1
#define HWIO_GCC_BLSP1_UART2_BCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_BCR_ADDR, HWIO_GCC_BLSP1_UART2_BCR_RMSK)
#define HWIO_GCC_BLSP1_UART2_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_BCR_ADDR, m)
#define HWIO_GCC_BLSP1_UART2_BCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART2_BCR_ADDR,v)
#define HWIO_GCC_BLSP1_UART2_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART2_BCR_ADDR,m,v,HWIO_GCC_BLSP1_UART2_BCR_IN)
#define HWIO_GCC_BLSP1_UART2_BCR_BLK_ARES_BMSK                                                       0x1
#define HWIO_GCC_BLSP1_UART2_BCR_BLK_ARES_SHFT                                                       0x0

#define HWIO_GCC_BLSP1_UART2_APPS_CBCR_ADDR                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x0000302c)
#define HWIO_GCC_BLSP1_UART2_APPS_CBCR_RMSK                                                   0x80000001
#define HWIO_GCC_BLSP1_UART2_APPS_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_APPS_CBCR_ADDR, HWIO_GCC_BLSP1_UART2_APPS_CBCR_RMSK)
#define HWIO_GCC_BLSP1_UART2_APPS_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_APPS_CBCR_ADDR, m)
#define HWIO_GCC_BLSP1_UART2_APPS_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART2_APPS_CBCR_ADDR,v)
#define HWIO_GCC_BLSP1_UART2_APPS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART2_APPS_CBCR_ADDR,m,v,HWIO_GCC_BLSP1_UART2_APPS_CBCR_IN)
#define HWIO_GCC_BLSP1_UART2_APPS_CBCR_CLK_OFF_BMSK                                           0x80000000
#define HWIO_GCC_BLSP1_UART2_APPS_CBCR_CLK_OFF_SHFT                                                 0x1f
#define HWIO_GCC_BLSP1_UART2_APPS_CBCR_CLK_ENABLE_BMSK                                               0x1
#define HWIO_GCC_BLSP1_UART2_APPS_CBCR_CLK_ENABLE_SHFT                                               0x0

#define HWIO_GCC_BLSP1_UART2_SIM_CBCR_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00003030)
#define HWIO_GCC_BLSP1_UART2_SIM_CBCR_RMSK                                                    0x80000001
#define HWIO_GCC_BLSP1_UART2_SIM_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_SIM_CBCR_ADDR, HWIO_GCC_BLSP1_UART2_SIM_CBCR_RMSK)
#define HWIO_GCC_BLSP1_UART2_SIM_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_SIM_CBCR_ADDR, m)
#define HWIO_GCC_BLSP1_UART2_SIM_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART2_SIM_CBCR_ADDR,v)
#define HWIO_GCC_BLSP1_UART2_SIM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART2_SIM_CBCR_ADDR,m,v,HWIO_GCC_BLSP1_UART2_SIM_CBCR_IN)
#define HWIO_GCC_BLSP1_UART2_SIM_CBCR_CLK_OFF_BMSK                                            0x80000000
#define HWIO_GCC_BLSP1_UART2_SIM_CBCR_CLK_OFF_SHFT                                                  0x1f
#define HWIO_GCC_BLSP1_UART2_SIM_CBCR_CLK_ENABLE_BMSK                                                0x1
#define HWIO_GCC_BLSP1_UART2_SIM_CBCR_CLK_ENABLE_SHFT                                                0x0

#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00003034)
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_RMSK                                               0x800000f3
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_ADDR, HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_RMSK)
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_IN)
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_ROOT_OFF_BMSK                                      0x80000000
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_ROOT_OFF_SHFT                                            0x1f
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_DIRTY_D_BMSK                                             0x80
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_DIRTY_D_SHFT                                              0x7
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_DIRTY_M_BMSK                                             0x40
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_DIRTY_M_SHFT                                              0x6
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_DIRTY_N_BMSK                                             0x20
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_DIRTY_N_SHFT                                              0x5
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                      0x10
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                       0x4
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_ROOT_EN_BMSK                                              0x2
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_ROOT_EN_SHFT                                              0x1
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_UPDATE_BMSK                                               0x1
#define HWIO_GCC_BLSP1_UART2_APPS_CMD_RCGR_UPDATE_SHFT                                               0x0

#define HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00003038)
#define HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_RMSK                                                   0x371f
#define HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_ADDR, HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_RMSK)
#define HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_IN)
#define HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_MODE_BMSK                                              0x3000
#define HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_MODE_SHFT                                                 0xc
#define HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_SRC_SEL_BMSK                                            0x700
#define HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_SRC_SEL_SHFT                                              0x8
#define HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_SRC_DIV_BMSK                                             0x1f
#define HWIO_GCC_BLSP1_UART2_APPS_CFG_RCGR_SRC_DIV_SHFT                                              0x0

#define HWIO_GCC_BLSP1_UART2_APPS_M_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x0000303c)
#define HWIO_GCC_BLSP1_UART2_APPS_M_RMSK                                                          0xffff
#define HWIO_GCC_BLSP1_UART2_APPS_M_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_APPS_M_ADDR, HWIO_GCC_BLSP1_UART2_APPS_M_RMSK)
#define HWIO_GCC_BLSP1_UART2_APPS_M_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_APPS_M_ADDR, m)
#define HWIO_GCC_BLSP1_UART2_APPS_M_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART2_APPS_M_ADDR,v)
#define HWIO_GCC_BLSP1_UART2_APPS_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART2_APPS_M_ADDR,m,v,HWIO_GCC_BLSP1_UART2_APPS_M_IN)
#define HWIO_GCC_BLSP1_UART2_APPS_M_M_BMSK                                                        0xffff
#define HWIO_GCC_BLSP1_UART2_APPS_M_M_SHFT                                                           0x0

#define HWIO_GCC_BLSP1_UART2_APPS_N_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00003040)
#define HWIO_GCC_BLSP1_UART2_APPS_N_RMSK                                                          0xffff
#define HWIO_GCC_BLSP1_UART2_APPS_N_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_APPS_N_ADDR, HWIO_GCC_BLSP1_UART2_APPS_N_RMSK)
#define HWIO_GCC_BLSP1_UART2_APPS_N_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_APPS_N_ADDR, m)
#define HWIO_GCC_BLSP1_UART2_APPS_N_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART2_APPS_N_ADDR,v)
#define HWIO_GCC_BLSP1_UART2_APPS_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART2_APPS_N_ADDR,m,v,HWIO_GCC_BLSP1_UART2_APPS_N_IN)
#define HWIO_GCC_BLSP1_UART2_APPS_N_NOT_N_MINUS_M_BMSK                                            0xffff
#define HWIO_GCC_BLSP1_UART2_APPS_N_NOT_N_MINUS_M_SHFT                                               0x0

#define HWIO_GCC_BLSP1_UART2_APPS_D_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00003044)
#define HWIO_GCC_BLSP1_UART2_APPS_D_RMSK                                                          0xffff
#define HWIO_GCC_BLSP1_UART2_APPS_D_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_APPS_D_ADDR, HWIO_GCC_BLSP1_UART2_APPS_D_RMSK)
#define HWIO_GCC_BLSP1_UART2_APPS_D_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART2_APPS_D_ADDR, m)
#define HWIO_GCC_BLSP1_UART2_APPS_D_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART2_APPS_D_ADDR,v)
#define HWIO_GCC_BLSP1_UART2_APPS_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART2_APPS_D_ADDR,m,v,HWIO_GCC_BLSP1_UART2_APPS_D_IN)
#define HWIO_GCC_BLSP1_UART2_APPS_D_NOT_2D_BMSK                                                   0xffff
#define HWIO_GCC_BLSP1_UART2_APPS_D_NOT_2D_SHFT                                                      0x0

#define HWIO_GCC_BLSP1_UART3_BCR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x00004038)
#define HWIO_GCC_BLSP1_UART3_BCR_RMSK                                                                0x1
#define HWIO_GCC_BLSP1_UART3_BCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART3_BCR_ADDR, HWIO_GCC_BLSP1_UART3_BCR_RMSK)
#define HWIO_GCC_BLSP1_UART3_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART3_BCR_ADDR, m)
#define HWIO_GCC_BLSP1_UART3_BCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART3_BCR_ADDR,v)
#define HWIO_GCC_BLSP1_UART3_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART3_BCR_ADDR,m,v,HWIO_GCC_BLSP1_UART3_BCR_IN)
#define HWIO_GCC_BLSP1_UART3_BCR_BLK_ARES_BMSK                                                       0x1
#define HWIO_GCC_BLSP1_UART3_BCR_BLK_ARES_SHFT                                                       0x0

#define HWIO_GCC_BLSP1_UART3_APPS_CBCR_ADDR                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x0000403c)
#define HWIO_GCC_BLSP1_UART3_APPS_CBCR_RMSK                                                   0x80000001
#define HWIO_GCC_BLSP1_UART3_APPS_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART3_APPS_CBCR_ADDR, HWIO_GCC_BLSP1_UART3_APPS_CBCR_RMSK)
#define HWIO_GCC_BLSP1_UART3_APPS_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART3_APPS_CBCR_ADDR, m)
#define HWIO_GCC_BLSP1_UART3_APPS_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART3_APPS_CBCR_ADDR,v)
#define HWIO_GCC_BLSP1_UART3_APPS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART3_APPS_CBCR_ADDR,m,v,HWIO_GCC_BLSP1_UART3_APPS_CBCR_IN)
#define HWIO_GCC_BLSP1_UART3_APPS_CBCR_CLK_OFF_BMSK                                           0x80000000
#define HWIO_GCC_BLSP1_UART3_APPS_CBCR_CLK_OFF_SHFT                                                 0x1f
#define HWIO_GCC_BLSP1_UART3_APPS_CBCR_CLK_ENABLE_BMSK                                               0x1
#define HWIO_GCC_BLSP1_UART3_APPS_CBCR_CLK_ENABLE_SHFT                                               0x0

#define HWIO_GCC_BLSP1_UART3_SIM_CBCR_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00004040)
#define HWIO_GCC_BLSP1_UART3_SIM_CBCR_RMSK                                                    0x80000001
#define HWIO_GCC_BLSP1_UART3_SIM_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART3_SIM_CBCR_ADDR, HWIO_GCC_BLSP1_UART3_SIM_CBCR_RMSK)
#define HWIO_GCC_BLSP1_UART3_SIM_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART3_SIM_CBCR_ADDR, m)
#define HWIO_GCC_BLSP1_UART3_SIM_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART3_SIM_CBCR_ADDR,v)
#define HWIO_GCC_BLSP1_UART3_SIM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART3_SIM_CBCR_ADDR,m,v,HWIO_GCC_BLSP1_UART3_SIM_CBCR_IN)
#define HWIO_GCC_BLSP1_UART3_SIM_CBCR_CLK_OFF_BMSK                                            0x80000000
#define HWIO_GCC_BLSP1_UART3_SIM_CBCR_CLK_OFF_SHFT                                                  0x1f
#define HWIO_GCC_BLSP1_UART3_SIM_CBCR_CLK_ENABLE_BMSK                                                0x1
#define HWIO_GCC_BLSP1_UART3_SIM_CBCR_CLK_ENABLE_SHFT                                                0x0

#define HWIO_GCC_BLSP1_UART3_APPS_CMD_RCGR_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00004044)
#define HWIO_GCC_BLSP1_UART3_APPS_CMD_RCGR_RMSK                                               0x800000f3
#define HWIO_GCC_BLSP1_UART3_APPS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART3_APPS_CMD_RCGR_ADDR, HWIO_GCC_BLSP1_UART3_APPS_CMD_RCGR_RMSK)
#define HWIO_GCC_BLSP1_UART3_APPS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART3_APPS_CMD_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_UART3_APPS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART3_APPS_CMD_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_UART3_APPS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART3_APPS_CMD_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_UART3_APPS_CMD_RCGR_IN)
#define HWIO_GCC_BLSP1_UART3_APPS_CMD_RCGR_ROOT_OFF_BMSK                                      0x80000000
#define HWIO_GCC_BLSP1_UART3_APPS_CMD_RCGR_ROOT_OFF_SHFT                                            0x1f
#define HWIO_GCC_BLSP1_UART3_APPS_CMD_RCGR_DIRTY_D_BMSK                                             0x80
#define HWIO_GCC_BLSP1_UART3_APPS_CMD_RCGR_DIRTY_D_SHFT                                              0x7
#define HWIO_GCC_BLSP1_UART3_APPS_CMD_RCGR_DIRTY_M_BMSK                                             0x40
#define HWIO_GCC_BLSP1_UART3_APPS_CMD_RCGR_DIRTY_M_SHFT                                              0x6
#define HWIO_GCC_BLSP1_UART3_APPS_CMD_RCGR_DIRTY_N_BMSK                                             0x20
#define HWIO_GCC_BLSP1_UART3_APPS_CMD_RCGR_DIRTY_N_SHFT                                              0x5
#define HWIO_GCC_BLSP1_UART3_APPS_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                      0x10
#define HWIO_GCC_BLSP1_UART3_APPS_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                       0x4
#define HWIO_GCC_BLSP1_UART3_APPS_CMD_RCGR_ROOT_EN_BMSK                                              0x2
#define HWIO_GCC_BLSP1_UART3_APPS_CMD_RCGR_ROOT_EN_SHFT                                              0x1
#define HWIO_GCC_BLSP1_UART3_APPS_CMD_RCGR_UPDATE_BMSK                                               0x1
#define HWIO_GCC_BLSP1_UART3_APPS_CMD_RCGR_UPDATE_SHFT                                               0x0

#define HWIO_GCC_BLSP1_UART3_APPS_CFG_RCGR_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00004048)
#define HWIO_GCC_BLSP1_UART3_APPS_CFG_RCGR_RMSK                                                   0x371f
#define HWIO_GCC_BLSP1_UART3_APPS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART3_APPS_CFG_RCGR_ADDR, HWIO_GCC_BLSP1_UART3_APPS_CFG_RCGR_RMSK)
#define HWIO_GCC_BLSP1_UART3_APPS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART3_APPS_CFG_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_UART3_APPS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART3_APPS_CFG_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_UART3_APPS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART3_APPS_CFG_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_UART3_APPS_CFG_RCGR_IN)
#define HWIO_GCC_BLSP1_UART3_APPS_CFG_RCGR_MODE_BMSK                                              0x3000
#define HWIO_GCC_BLSP1_UART3_APPS_CFG_RCGR_MODE_SHFT                                                 0xc
#define HWIO_GCC_BLSP1_UART3_APPS_CFG_RCGR_SRC_SEL_BMSK                                            0x700
#define HWIO_GCC_BLSP1_UART3_APPS_CFG_RCGR_SRC_SEL_SHFT                                              0x8
#define HWIO_GCC_BLSP1_UART3_APPS_CFG_RCGR_SRC_DIV_BMSK                                             0x1f
#define HWIO_GCC_BLSP1_UART3_APPS_CFG_RCGR_SRC_DIV_SHFT                                              0x0

#define HWIO_GCC_BLSP1_UART3_APPS_M_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x0000404c)
#define HWIO_GCC_BLSP1_UART3_APPS_M_RMSK                                                          0xffff
#define HWIO_GCC_BLSP1_UART3_APPS_M_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART3_APPS_M_ADDR, HWIO_GCC_BLSP1_UART3_APPS_M_RMSK)
#define HWIO_GCC_BLSP1_UART3_APPS_M_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART3_APPS_M_ADDR, m)
#define HWIO_GCC_BLSP1_UART3_APPS_M_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART3_APPS_M_ADDR,v)
#define HWIO_GCC_BLSP1_UART3_APPS_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART3_APPS_M_ADDR,m,v,HWIO_GCC_BLSP1_UART3_APPS_M_IN)
#define HWIO_GCC_BLSP1_UART3_APPS_M_M_BMSK                                                        0xffff
#define HWIO_GCC_BLSP1_UART3_APPS_M_M_SHFT                                                           0x0

#define HWIO_GCC_BLSP1_UART3_APPS_N_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00004050)
#define HWIO_GCC_BLSP1_UART3_APPS_N_RMSK                                                          0xffff
#define HWIO_GCC_BLSP1_UART3_APPS_N_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART3_APPS_N_ADDR, HWIO_GCC_BLSP1_UART3_APPS_N_RMSK)
#define HWIO_GCC_BLSP1_UART3_APPS_N_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART3_APPS_N_ADDR, m)
#define HWIO_GCC_BLSP1_UART3_APPS_N_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART3_APPS_N_ADDR,v)
#define HWIO_GCC_BLSP1_UART3_APPS_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART3_APPS_N_ADDR,m,v,HWIO_GCC_BLSP1_UART3_APPS_N_IN)
#define HWIO_GCC_BLSP1_UART3_APPS_N_NOT_N_MINUS_M_BMSK                                            0xffff
#define HWIO_GCC_BLSP1_UART3_APPS_N_NOT_N_MINUS_M_SHFT                                               0x0

#define HWIO_GCC_BLSP1_UART3_APPS_D_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00004054)
#define HWIO_GCC_BLSP1_UART3_APPS_D_RMSK                                                          0xffff
#define HWIO_GCC_BLSP1_UART3_APPS_D_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART3_APPS_D_ADDR, HWIO_GCC_BLSP1_UART3_APPS_D_RMSK)
#define HWIO_GCC_BLSP1_UART3_APPS_D_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART3_APPS_D_ADDR, m)
#define HWIO_GCC_BLSP1_UART3_APPS_D_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART3_APPS_D_ADDR,v)
#define HWIO_GCC_BLSP1_UART3_APPS_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART3_APPS_D_ADDR,m,v,HWIO_GCC_BLSP1_UART3_APPS_D_IN)
#define HWIO_GCC_BLSP1_UART3_APPS_D_NOT_2D_BMSK                                                   0xffff
#define HWIO_GCC_BLSP1_UART3_APPS_D_NOT_2D_SHFT                                                      0x0

#define HWIO_GCC_BLSP1_UART4_BCR_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x00005038)
#define HWIO_GCC_BLSP1_UART4_BCR_RMSK                                                                0x1
#define HWIO_GCC_BLSP1_UART4_BCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART4_BCR_ADDR, HWIO_GCC_BLSP1_UART4_BCR_RMSK)
#define HWIO_GCC_BLSP1_UART4_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART4_BCR_ADDR, m)
#define HWIO_GCC_BLSP1_UART4_BCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART4_BCR_ADDR,v)
#define HWIO_GCC_BLSP1_UART4_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART4_BCR_ADDR,m,v,HWIO_GCC_BLSP1_UART4_BCR_IN)
#define HWIO_GCC_BLSP1_UART4_BCR_BLK_ARES_BMSK                                                       0x1
#define HWIO_GCC_BLSP1_UART4_BCR_BLK_ARES_SHFT                                                       0x0

#define HWIO_GCC_BLSP1_UART4_APPS_CBCR_ADDR                                                   (GCC_CLK_CTL_REG_REG_BASE      + 0x0000503c)
#define HWIO_GCC_BLSP1_UART4_APPS_CBCR_RMSK                                                   0x80000001
#define HWIO_GCC_BLSP1_UART4_APPS_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART4_APPS_CBCR_ADDR, HWIO_GCC_BLSP1_UART4_APPS_CBCR_RMSK)
#define HWIO_GCC_BLSP1_UART4_APPS_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART4_APPS_CBCR_ADDR, m)
#define HWIO_GCC_BLSP1_UART4_APPS_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART4_APPS_CBCR_ADDR,v)
#define HWIO_GCC_BLSP1_UART4_APPS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART4_APPS_CBCR_ADDR,m,v,HWIO_GCC_BLSP1_UART4_APPS_CBCR_IN)
#define HWIO_GCC_BLSP1_UART4_APPS_CBCR_CLK_OFF_BMSK                                           0x80000000
#define HWIO_GCC_BLSP1_UART4_APPS_CBCR_CLK_OFF_SHFT                                                 0x1f
#define HWIO_GCC_BLSP1_UART4_APPS_CBCR_CLK_ENABLE_BMSK                                               0x1
#define HWIO_GCC_BLSP1_UART4_APPS_CBCR_CLK_ENABLE_SHFT                                               0x0

#define HWIO_GCC_BLSP1_UART4_SIM_CBCR_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x00005040)
#define HWIO_GCC_BLSP1_UART4_SIM_CBCR_RMSK                                                    0x80000001
#define HWIO_GCC_BLSP1_UART4_SIM_CBCR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART4_SIM_CBCR_ADDR, HWIO_GCC_BLSP1_UART4_SIM_CBCR_RMSK)
#define HWIO_GCC_BLSP1_UART4_SIM_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART4_SIM_CBCR_ADDR, m)
#define HWIO_GCC_BLSP1_UART4_SIM_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART4_SIM_CBCR_ADDR,v)
#define HWIO_GCC_BLSP1_UART4_SIM_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART4_SIM_CBCR_ADDR,m,v,HWIO_GCC_BLSP1_UART4_SIM_CBCR_IN)
#define HWIO_GCC_BLSP1_UART4_SIM_CBCR_CLK_OFF_BMSK                                            0x80000000
#define HWIO_GCC_BLSP1_UART4_SIM_CBCR_CLK_OFF_SHFT                                                  0x1f
#define HWIO_GCC_BLSP1_UART4_SIM_CBCR_CLK_ENABLE_BMSK                                                0x1
#define HWIO_GCC_BLSP1_UART4_SIM_CBCR_CLK_ENABLE_SHFT                                                0x0

#define HWIO_GCC_BLSP1_UART4_APPS_CMD_RCGR_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00005044)
#define HWIO_GCC_BLSP1_UART4_APPS_CMD_RCGR_RMSK                                               0x800000f3
#define HWIO_GCC_BLSP1_UART4_APPS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART4_APPS_CMD_RCGR_ADDR, HWIO_GCC_BLSP1_UART4_APPS_CMD_RCGR_RMSK)
#define HWIO_GCC_BLSP1_UART4_APPS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART4_APPS_CMD_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_UART4_APPS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART4_APPS_CMD_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_UART4_APPS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART4_APPS_CMD_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_UART4_APPS_CMD_RCGR_IN)
#define HWIO_GCC_BLSP1_UART4_APPS_CMD_RCGR_ROOT_OFF_BMSK                                      0x80000000
#define HWIO_GCC_BLSP1_UART4_APPS_CMD_RCGR_ROOT_OFF_SHFT                                            0x1f
#define HWIO_GCC_BLSP1_UART4_APPS_CMD_RCGR_DIRTY_D_BMSK                                             0x80
#define HWIO_GCC_BLSP1_UART4_APPS_CMD_RCGR_DIRTY_D_SHFT                                              0x7
#define HWIO_GCC_BLSP1_UART4_APPS_CMD_RCGR_DIRTY_M_BMSK                                             0x40
#define HWIO_GCC_BLSP1_UART4_APPS_CMD_RCGR_DIRTY_M_SHFT                                              0x6
#define HWIO_GCC_BLSP1_UART4_APPS_CMD_RCGR_DIRTY_N_BMSK                                             0x20
#define HWIO_GCC_BLSP1_UART4_APPS_CMD_RCGR_DIRTY_N_SHFT                                              0x5
#define HWIO_GCC_BLSP1_UART4_APPS_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                      0x10
#define HWIO_GCC_BLSP1_UART4_APPS_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                       0x4
#define HWIO_GCC_BLSP1_UART4_APPS_CMD_RCGR_ROOT_EN_BMSK                                              0x2
#define HWIO_GCC_BLSP1_UART4_APPS_CMD_RCGR_ROOT_EN_SHFT                                              0x1
#define HWIO_GCC_BLSP1_UART4_APPS_CMD_RCGR_UPDATE_BMSK                                               0x1
#define HWIO_GCC_BLSP1_UART4_APPS_CMD_RCGR_UPDATE_SHFT                                               0x0

#define HWIO_GCC_BLSP1_UART4_APPS_CFG_RCGR_ADDR                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00005048)
#define HWIO_GCC_BLSP1_UART4_APPS_CFG_RCGR_RMSK                                                   0x371f
#define HWIO_GCC_BLSP1_UART4_APPS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART4_APPS_CFG_RCGR_ADDR, HWIO_GCC_BLSP1_UART4_APPS_CFG_RCGR_RMSK)
#define HWIO_GCC_BLSP1_UART4_APPS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART4_APPS_CFG_RCGR_ADDR, m)
#define HWIO_GCC_BLSP1_UART4_APPS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART4_APPS_CFG_RCGR_ADDR,v)
#define HWIO_GCC_BLSP1_UART4_APPS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART4_APPS_CFG_RCGR_ADDR,m,v,HWIO_GCC_BLSP1_UART4_APPS_CFG_RCGR_IN)
#define HWIO_GCC_BLSP1_UART4_APPS_CFG_RCGR_MODE_BMSK                                              0x3000
#define HWIO_GCC_BLSP1_UART4_APPS_CFG_RCGR_MODE_SHFT                                                 0xc
#define HWIO_GCC_BLSP1_UART4_APPS_CFG_RCGR_SRC_SEL_BMSK                                            0x700
#define HWIO_GCC_BLSP1_UART4_APPS_CFG_RCGR_SRC_SEL_SHFT                                              0x8
#define HWIO_GCC_BLSP1_UART4_APPS_CFG_RCGR_SRC_DIV_BMSK                                             0x1f
#define HWIO_GCC_BLSP1_UART4_APPS_CFG_RCGR_SRC_DIV_SHFT                                              0x0

#define HWIO_GCC_BLSP1_UART4_APPS_M_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x0000504c)
#define HWIO_GCC_BLSP1_UART4_APPS_M_RMSK                                                          0xffff
#define HWIO_GCC_BLSP1_UART4_APPS_M_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART4_APPS_M_ADDR, HWIO_GCC_BLSP1_UART4_APPS_M_RMSK)
#define HWIO_GCC_BLSP1_UART4_APPS_M_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART4_APPS_M_ADDR, m)
#define HWIO_GCC_BLSP1_UART4_APPS_M_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART4_APPS_M_ADDR,v)
#define HWIO_GCC_BLSP1_UART4_APPS_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART4_APPS_M_ADDR,m,v,HWIO_GCC_BLSP1_UART4_APPS_M_IN)
#define HWIO_GCC_BLSP1_UART4_APPS_M_M_BMSK                                                        0xffff
#define HWIO_GCC_BLSP1_UART4_APPS_M_M_SHFT                                                           0x0

#define HWIO_GCC_BLSP1_UART4_APPS_N_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00005050)
#define HWIO_GCC_BLSP1_UART4_APPS_N_RMSK                                                          0xffff
#define HWIO_GCC_BLSP1_UART4_APPS_N_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART4_APPS_N_ADDR, HWIO_GCC_BLSP1_UART4_APPS_N_RMSK)
#define HWIO_GCC_BLSP1_UART4_APPS_N_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART4_APPS_N_ADDR, m)
#define HWIO_GCC_BLSP1_UART4_APPS_N_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART4_APPS_N_ADDR,v)
#define HWIO_GCC_BLSP1_UART4_APPS_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART4_APPS_N_ADDR,m,v,HWIO_GCC_BLSP1_UART4_APPS_N_IN)
#define HWIO_GCC_BLSP1_UART4_APPS_N_NOT_N_MINUS_M_BMSK                                            0xffff
#define HWIO_GCC_BLSP1_UART4_APPS_N_NOT_N_MINUS_M_SHFT                                               0x0

#define HWIO_GCC_BLSP1_UART4_APPS_D_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00005054)
#define HWIO_GCC_BLSP1_UART4_APPS_D_RMSK                                                          0xffff
#define HWIO_GCC_BLSP1_UART4_APPS_D_IN          \
        in_dword_masked(HWIO_GCC_BLSP1_UART4_APPS_D_ADDR, HWIO_GCC_BLSP1_UART4_APPS_D_RMSK)
#define HWIO_GCC_BLSP1_UART4_APPS_D_INM(m)      \
        in_dword_masked(HWIO_GCC_BLSP1_UART4_APPS_D_ADDR, m)
#define HWIO_GCC_BLSP1_UART4_APPS_D_OUT(v)      \
        out_dword(HWIO_GCC_BLSP1_UART4_APPS_D_ADDR,v)
#define HWIO_GCC_BLSP1_UART4_APPS_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_BLSP1_UART4_APPS_D_ADDR,m,v,HWIO_GCC_BLSP1_UART4_APPS_D_IN)
#define HWIO_GCC_BLSP1_UART4_APPS_D_NOT_2D_BMSK                                                   0xffff
#define HWIO_GCC_BLSP1_UART4_APPS_D_NOT_2D_SHFT                                                      0x0

#define HWIO_GCC_A7SS_BOOT_GPLL0_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x00046024)
#define HWIO_GCC_A7SS_BOOT_GPLL0_RMSK                                                                0x3
#define HWIO_GCC_A7SS_BOOT_GPLL0_IN          \
        in_dword_masked(HWIO_GCC_A7SS_BOOT_GPLL0_ADDR, HWIO_GCC_A7SS_BOOT_GPLL0_RMSK)
#define HWIO_GCC_A7SS_BOOT_GPLL0_INM(m)      \
        in_dword_masked(HWIO_GCC_A7SS_BOOT_GPLL0_ADDR, m)
#define HWIO_GCC_A7SS_BOOT_GPLL0_OUT(v)      \
        out_dword(HWIO_GCC_A7SS_BOOT_GPLL0_ADDR,v)
#define HWIO_GCC_A7SS_BOOT_GPLL0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_A7SS_BOOT_GPLL0_ADDR,m,v,HWIO_GCC_A7SS_BOOT_GPLL0_IN)
#define HWIO_GCC_A7SS_BOOT_GPLL0_CLK_DIV_BMSK                                                        0x3
#define HWIO_GCC_A7SS_BOOT_GPLL0_CLK_DIV_SHFT                                                        0x0

#define HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_ADDR                                                    (GCC_CLK_CTL_REG_REG_BASE      + 0x0001b000)
#define HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_RMSK                                                           0xf
#define HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_ADDR, HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_RMSK)
#define HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_ADDR, m)
#define HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_ADDR,v)
#define HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_ADDR,m,v,HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_IN)
#define HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_GPLL2_BMSK                                                     0x8
#define HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_GPLL2_SHFT                                                     0x3
#define HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_BIMC_PLL_BMSK                                                  0x4
#define HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_BIMC_PLL_SHFT                                                  0x2
#define HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_GPLL1_BMSK                                                     0x2
#define HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_GPLL1_SHFT                                                     0x1
#define HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_GPLL0_BMSK                                                     0x1
#define HWIO_GCC_MSS_Q6_GPLL_ENA_VOTE_GPLL0_SHFT                                                     0x0

#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_ADDR                                            (GCC_CLK_CTL_REG_REG_BASE      + 0x0001b004)
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_RMSK                                              0x1bf7ff
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_ADDR, HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_RMSK)
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_ADDR, m)
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_ADDR,v)
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_ADDR,m,v,HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_IN)
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_QDSS_CFG_AHB_CLK_ENA_BMSK                         0x100000
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_QDSS_CFG_AHB_CLK_ENA_SHFT                             0x14
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_QDSS_DAP_CLK_ENA_BMSK                              0x80000
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_QDSS_DAP_CLK_ENA_SHFT                                 0x13
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_IMEM_AXI_CLK_ENA_BMSK                              0x20000
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_IMEM_AXI_CLK_ENA_SHFT                                 0x11
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_PCNOC_APSS_AHB_CLK_ENA_BMSK                        0x10000
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_PCNOC_APSS_AHB_CLK_ENA_SHFT                           0x10
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_BIMC_APSS_AXI_CLK_ENA_BMSK                          0x8000
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_BIMC_APSS_AXI_CLK_ENA_SHFT                             0xf
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_APSS_AHB_CLK_ENA_BMSK                               0x4000
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_APSS_AHB_CLK_ENA_SHFT                                  0xe
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_APSS_AXI_CLK_ENA_BMSK                               0x2000
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_APSS_AXI_CLK_ENA_SHFT                                  0xd
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_MPM_AHB_CLK_ENA_BMSK                                0x1000
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_MPM_AHB_CLK_ENA_SHFT                                   0xc
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_BLSP1_AHB_CLK_ENA_BMSK                               0x400
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_BLSP1_AHB_CLK_ENA_SHFT                                 0xa
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_BLSP1_SLEEP_CLK_ENA_BMSK                             0x200
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_BLSP1_SLEEP_CLK_ENA_SHFT                               0x9
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_PRNG_AHB_CLK_ENA_BMSK                                0x100
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_PRNG_AHB_CLK_ENA_SHFT                                  0x8
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_BOOT_ROM_AHB_CLK_ENA_BMSK                             0x80
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_BOOT_ROM_AHB_CLK_ENA_SHFT                              0x7
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_MSG_RAM_AHB_CLK_ENA_BMSK                              0x40
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_MSG_RAM_AHB_CLK_ENA_SHFT                               0x6
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_TLMM_AHB_CLK_ENA_BMSK                                 0x20
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_TLMM_AHB_CLK_ENA_SHFT                                  0x5
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_TLMM_CLK_ENA_BMSK                                     0x10
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_TLMM_CLK_ENA_SHFT                                      0x4
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_SPMI_PCNOC_AHB_CLK_ENA_BMSK                            0x8
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_SPMI_PCNOC_AHB_CLK_ENA_SHFT                            0x3
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_CRYPTO_CLK_ENA_BMSK                                    0x4
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_CRYPTO_CLK_ENA_SHFT                                    0x2
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AXI_CLK_ENA_BMSK                                0x2
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AXI_CLK_ENA_SHFT                                0x1
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AHB_CLK_ENA_BMSK                                0x1
#define HWIO_GCC_MSS_Q6_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AHB_CLK_ENA_SHFT                                0x0

#endif /* __UCLOCKHWIO_H__ */
