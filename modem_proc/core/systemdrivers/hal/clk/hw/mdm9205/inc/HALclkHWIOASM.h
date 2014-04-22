#ifndef __HALCLKHWIOASM_H__
#define __HALCLKHWIOASM_H__
/*
===========================================================================
*/
/**
  @file HALclkHWIOASM.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    MDM9205 (Twizy) [twizy_v1.0_p3q3r35_MTO]
 
  This file contains HWIO register definitions for the following modules:
    MSS_QDSP6V67SS_PUB
    MSS_QDSP6SS_QDSP6SS_QTMR_AC
    MSS_QDSP6SS_QTMR_F1_1
    MSS_PERPH
    TCSR_TCSR_REGS

  'Include' filters applied: PLL[MSS_QDSP6V67SS_PUB] RCG[MSS_QDSP6V67SS_PUB] CNTACR[MSS_QDSP6SS_QDSP6SS_QTMR_AC] MSS_MPLL1[MSS_PERPH] MSS_BUS.*RCGR[MSS_PERPH] CNTPCT_LO[MSS_QDSP6SS_QTMR_F1_1] TCSR_SOC_HW_VERSION[TCSR_TCSR_REGS] 
  'Exclude' filters applied: PLL_RCG[MSS_QDSP6V67SS_PUB] 

  Generation parameters: 
  { u'filename': u'HALclkHWIOASM.h',
    u'header': u'#define MSS_TOP_BASE_PHYS        0x04000000\n#define MSS_TOP_BASE             MSS_TOP_BASE_PHYS\n\n#define CORE_TOP_CSR_BASE_PHYS   0x01900000\n#define CORE_TOP_CSR_BASE        CORE_TOP_CSR_BASE_PHYS\n',
    u'module-filter-exclude': { u'MSS_QDSP6V67SS_PUB': [u'PLL_RCG']},
    u'module-filter-include': { u'MSS_PERPH': [u'MSS_MPLL1', u'MSS_BUS.*RCGR'],
                                u'MSS_QDSP6SS_QDSP6SS_QTMR_AC': [u'CNTACR'],
                                u'MSS_QDSP6SS_QTMR_F1_1': [u'CNTPCT_LO'],
                                u'MSS_QDSP6V67SS_PUB': [u'PLL', u'RCG'],
                                u'TCSR_TCSR_REGS': [u'TCSR_SOC_HW_VERSION']},
    u'modules': [ u'MSS_QDSP6V67SS_PUB',
                  u'MSS_QDSP6SS_QDSP6SS_QTMR_AC',
                  u'MSS_QDSP6SS_QTMR_F1_1',
                  u'MSS_PERPH',
                  u'TCSR_TCSR_REGS'],
    u'rmsk-input': True}
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

  $Header: //components/rel/core.mpss/3.10/systemdrivers/hal/clk/hw/mdm9205/inc/HALclkHWIOASM.h#3 $
  $DateTime: 2019/02/18 02:27:52 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#define MSS_TOP_BASE_PHYS        0x04000000
#define MSS_TOP_BASE             MSS_TOP_BASE_PHYS

#define CORE_TOP_CSR_BASE_PHYS   0x01900000
#define CORE_TOP_CSR_BASE        CORE_TOP_CSR_BASE_PHYS

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
 * MODULE: MSS_QDSP6SS_QDSP6SS_QTMR_AC
 *--------------------------------------------------------------------------*/

#define MSS_QDSP6SS_QDSP6SS_QTMR_AC_REG_BASE                           (MSS_TOP_BASE      + 0x00120000)
#define MSS_QDSP6SS_QDSP6SS_QTMR_AC_REG_BASE_SIZE                      0x1000
#define MSS_QDSP6SS_QDSP6SS_QTMR_AC_REG_BASE_USED                      0xfd0

#define HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_ADDR(n)                      (MSS_QDSP6SS_QDSP6SS_QTMR_AC_REG_BASE      + 0x00000040 + 0x4 * (n))
#define HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_RMSK                               0x3f
#define HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_MAXn                                  2
#define HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_INI(n)        \
        in_dword_masked(HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_ADDR(n), HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_RMSK)
#define HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_INMI(n,mask)    \
        in_dword_masked(HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_ADDR(n), mask)
#define HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_OUTI(n,val)    \
        out_dword(HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_ADDR(n),val)
#define HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_ADDR(n),mask,val,HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_INI(n))
#define HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_RWPT_BMSK                          0x20
#define HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_RWPT_SHFT                           0x5
#define HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_RWVT_BMSK                          0x10
#define HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_RWVT_SHFT                           0x4
#define HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_RVOFF_BMSK                          0x8
#define HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_RVOFF_SHFT                          0x3
#define HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_RFRQ_BMSK                           0x4
#define HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_RFRQ_SHFT                           0x2
#define HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_RPVCT_BMSK                          0x2
#define HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_RPVCT_SHFT                          0x1
#define HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_RPCT_BMSK                           0x1
#define HWIO_MSS_QDSP6SS_QTMR_AC_CNTACR_n_RPCT_SHFT                           0x0

/*----------------------------------------------------------------------------
 * MODULE: MSS_QDSP6SS_QTMR_F1_1
 *--------------------------------------------------------------------------*/

#define MSS_QDSP6SS_QTMR_F1_1_REG_BASE                                     (MSS_TOP_BASE      + 0x00122000)
#define MSS_QDSP6SS_QTMR_F1_1_REG_BASE_SIZE                                0x1000
#define MSS_QDSP6SS_QTMR_F1_1_REG_BASE_USED                                0xfd0

#define HWIO_MSS_QDSP6SS_QTMR_V1_CNTPCT_LO_1_ADDR                          (MSS_QDSP6SS_QTMR_F1_1_REG_BASE      + 0x00000000)
#define HWIO_MSS_QDSP6SS_QTMR_V1_CNTPCT_LO_1_RMSK                          0xffffffff
#define HWIO_MSS_QDSP6SS_QTMR_V1_CNTPCT_LO_1_IN          \
        in_dword_masked(HWIO_MSS_QDSP6SS_QTMR_V1_CNTPCT_LO_1_ADDR, HWIO_MSS_QDSP6SS_QTMR_V1_CNTPCT_LO_1_RMSK)
#define HWIO_MSS_QDSP6SS_QTMR_V1_CNTPCT_LO_1_INM(m)      \
        in_dword_masked(HWIO_MSS_QDSP6SS_QTMR_V1_CNTPCT_LO_1_ADDR, m)
#define HWIO_MSS_QDSP6SS_QTMR_V1_CNTPCT_LO_1_CNTPCT_LO_BMSK                0xffffffff
#define HWIO_MSS_QDSP6SS_QTMR_V1_CNTPCT_LO_1_CNTPCT_LO_SHFT                       0x0

/*----------------------------------------------------------------------------
 * MODULE: MSS_PERPH
 *--------------------------------------------------------------------------*/

#define MSS_PERPH_REG_BASE                                                             (MSS_TOP_BASE      + 0x00180000)
#define MSS_PERPH_REG_BASE_SIZE                                                        0xf020
#define MSS_PERPH_REG_BASE_USED                                                        0xf000

#define HWIO_MSS_MPLL1_MODE_ADDR                                                       (MSS_PERPH_REG_BASE      + 0x00001020)
#define HWIO_MSS_MPLL1_MODE_RMSK                                                         0x3fffff
#define HWIO_MSS_MPLL1_MODE_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_MODE_ADDR, HWIO_MSS_MPLL1_MODE_RMSK)
#define HWIO_MSS_MPLL1_MODE_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_MODE_ADDR, m)
#define HWIO_MSS_MPLL1_MODE_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_MODE_ADDR,v)
#define HWIO_MSS_MPLL1_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_MODE_ADDR,m,v,HWIO_MSS_MPLL1_MODE_IN)
#define HWIO_MSS_MPLL1_MODE_RESERVE_21_4_BMSK                                            0x3ffff0
#define HWIO_MSS_MPLL1_MODE_RESERVE_21_4_SHFT                                                 0x4
#define HWIO_MSS_MPLL1_MODE_PLL_PLLTEST_BMSK                                                  0x8
#define HWIO_MSS_MPLL1_MODE_PLL_PLLTEST_SHFT                                                  0x3
#define HWIO_MSS_MPLL1_MODE_PLL_RESET_N_BMSK                                                  0x4
#define HWIO_MSS_MPLL1_MODE_PLL_RESET_N_SHFT                                                  0x2
#define HWIO_MSS_MPLL1_MODE_PLL_BYPASSNL_BMSK                                                 0x2
#define HWIO_MSS_MPLL1_MODE_PLL_BYPASSNL_SHFT                                                 0x1
#define HWIO_MSS_MPLL1_MODE_PLL_OUTCTRL_BMSK                                                  0x1
#define HWIO_MSS_MPLL1_MODE_PLL_OUTCTRL_SHFT                                                  0x0

#define HWIO_MSS_MPLL1_L_VAL_ADDR                                                      (MSS_PERPH_REG_BASE      + 0x00001024)
#define HWIO_MSS_MPLL1_L_VAL_RMSK                                                            0x7f
#define HWIO_MSS_MPLL1_L_VAL_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_L_VAL_ADDR, HWIO_MSS_MPLL1_L_VAL_RMSK)
#define HWIO_MSS_MPLL1_L_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_L_VAL_ADDR, m)
#define HWIO_MSS_MPLL1_L_VAL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_L_VAL_ADDR,v)
#define HWIO_MSS_MPLL1_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_L_VAL_ADDR,m,v,HWIO_MSS_MPLL1_L_VAL_IN)
#define HWIO_MSS_MPLL1_L_VAL_PLL_L_BMSK                                                      0x7f
#define HWIO_MSS_MPLL1_L_VAL_PLL_L_SHFT                                                       0x0

#define HWIO_MSS_MPLL1_M_VAL_ADDR                                                      (MSS_PERPH_REG_BASE      + 0x00001028)
#define HWIO_MSS_MPLL1_M_VAL_RMSK                                                         0x7ffff
#define HWIO_MSS_MPLL1_M_VAL_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_M_VAL_ADDR, HWIO_MSS_MPLL1_M_VAL_RMSK)
#define HWIO_MSS_MPLL1_M_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_M_VAL_ADDR, m)
#define HWIO_MSS_MPLL1_M_VAL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_M_VAL_ADDR,v)
#define HWIO_MSS_MPLL1_M_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_M_VAL_ADDR,m,v,HWIO_MSS_MPLL1_M_VAL_IN)
#define HWIO_MSS_MPLL1_M_VAL_PLL_M_BMSK                                                   0x7ffff
#define HWIO_MSS_MPLL1_M_VAL_PLL_M_SHFT                                                       0x0

#define HWIO_MSS_MPLL1_N_VAL_ADDR                                                      (MSS_PERPH_REG_BASE      + 0x0000102c)
#define HWIO_MSS_MPLL1_N_VAL_RMSK                                                         0x7ffff
#define HWIO_MSS_MPLL1_N_VAL_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_N_VAL_ADDR, HWIO_MSS_MPLL1_N_VAL_RMSK)
#define HWIO_MSS_MPLL1_N_VAL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_N_VAL_ADDR, m)
#define HWIO_MSS_MPLL1_N_VAL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_N_VAL_ADDR,v)
#define HWIO_MSS_MPLL1_N_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_N_VAL_ADDR,m,v,HWIO_MSS_MPLL1_N_VAL_IN)
#define HWIO_MSS_MPLL1_N_VAL_PLL_N_BMSK                                                   0x7ffff
#define HWIO_MSS_MPLL1_N_VAL_PLL_N_SHFT                                                       0x0

#define HWIO_MSS_MPLL1_USER_CTL_ADDR                                                   (MSS_PERPH_REG_BASE      + 0x00001030)
#define HWIO_MSS_MPLL1_USER_CTL_RMSK                                                   0xffffffff
#define HWIO_MSS_MPLL1_USER_CTL_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_USER_CTL_ADDR, HWIO_MSS_MPLL1_USER_CTL_RMSK)
#define HWIO_MSS_MPLL1_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_USER_CTL_ADDR, m)
#define HWIO_MSS_MPLL1_USER_CTL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_USER_CTL_ADDR,v)
#define HWIO_MSS_MPLL1_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_USER_CTL_ADDR,m,v,HWIO_MSS_MPLL1_USER_CTL_IN)
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BITS_31_30_BMSK                                0xc0000000
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BITS_31_30_SHFT                                      0x1e
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BITS_29_28_BMSK                                0x30000000
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BITS_29_28_SHFT                                      0x1c
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BITS_27_25_BMSK                                 0xe000000
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BITS_27_25_SHFT                                      0x19
#define HWIO_MSS_MPLL1_USER_CTL_MN_EN_BMSK                                              0x1000000
#define HWIO_MSS_MPLL1_USER_CTL_MN_EN_SHFT                                                   0x18
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BITS_23_13_BMSK                                  0xffe000
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BITS_23_13_SHFT                                       0xd
#define HWIO_MSS_MPLL1_USER_CTL_PREDIV2_EN_BMSK                                            0x1000
#define HWIO_MSS_MPLL1_USER_CTL_PREDIV2_EN_SHFT                                               0xc
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BITS_11_10_BMSK                                     0xc00
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BITS_11_10_SHFT                                       0xa
#define HWIO_MSS_MPLL1_USER_CTL_POSTDIV_CTL_BMSK                                            0x300
#define HWIO_MSS_MPLL1_USER_CTL_POSTDIV_CTL_SHFT                                              0x8
#define HWIO_MSS_MPLL1_USER_CTL_INV_OUTPUT_BMSK                                              0x80
#define HWIO_MSS_MPLL1_USER_CTL_INV_OUTPUT_SHFT                                               0x7
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BIT_6_5_BMSK                                         0x60
#define HWIO_MSS_MPLL1_USER_CTL_RESERVE_BIT_6_5_SHFT                                          0x5
#define HWIO_MSS_MPLL1_USER_CTL_LVTEST_EN_BMSK                                               0x10
#define HWIO_MSS_MPLL1_USER_CTL_LVTEST_EN_SHFT                                                0x4
#define HWIO_MSS_MPLL1_USER_CTL_LVEARLY_EN_BMSK                                               0x8
#define HWIO_MSS_MPLL1_USER_CTL_LVEARLY_EN_SHFT                                               0x3
#define HWIO_MSS_MPLL1_USER_CTL_LVBIST_EN_BMSK                                                0x4
#define HWIO_MSS_MPLL1_USER_CTL_LVBIST_EN_SHFT                                                0x2
#define HWIO_MSS_MPLL1_USER_CTL_LVAUX_EN_BMSK                                                 0x2
#define HWIO_MSS_MPLL1_USER_CTL_LVAUX_EN_SHFT                                                 0x1
#define HWIO_MSS_MPLL1_USER_CTL_LVMAIN_EN_BMSK                                                0x1
#define HWIO_MSS_MPLL1_USER_CTL_LVMAIN_EN_SHFT                                                0x0

#define HWIO_MSS_MPLL1_CONFIG_CTL_ADDR                                                 (MSS_PERPH_REG_BASE      + 0x00001034)
#define HWIO_MSS_MPLL1_CONFIG_CTL_RMSK                                                 0xffffffff
#define HWIO_MSS_MPLL1_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_CONFIG_CTL_ADDR, HWIO_MSS_MPLL1_CONFIG_CTL_RMSK)
#define HWIO_MSS_MPLL1_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_CONFIG_CTL_ADDR, m)
#define HWIO_MSS_MPLL1_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_CONFIG_CTL_ADDR,v)
#define HWIO_MSS_MPLL1_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_CONFIG_CTL_ADDR,m,v,HWIO_MSS_MPLL1_CONFIG_CTL_IN)
#define HWIO_MSS_MPLL1_CONFIG_CTL_FRAC_N_MODE_CTL_BMSK                                 0x80000000
#define HWIO_MSS_MPLL1_CONFIG_CTL_FRAC_N_MODE_CTL_SHFT                                       0x1f
#define HWIO_MSS_MPLL1_CONFIG_CTL_CFG_LOCKDET_BMSK                                     0x60000000
#define HWIO_MSS_MPLL1_CONFIG_CTL_CFG_LOCKDET_SHFT                                           0x1d
#define HWIO_MSS_MPLL1_CONFIG_CTL_RESERVE_28_BMSK                                      0x10000000
#define HWIO_MSS_MPLL1_CONFIG_CTL_RESERVE_28_SHFT                                            0x1c
#define HWIO_MSS_MPLL1_CONFIG_CTL_OVER_VOLTAGE_DET_CFG_BMSK                             0xc000000
#define HWIO_MSS_MPLL1_CONFIG_CTL_OVER_VOLTAGE_DET_CFG_SHFT                                  0x1a
#define HWIO_MSS_MPLL1_CONFIG_CTL_OVER_VOLTAGE_DET_COUNTER_DIS_BMSK                     0x2000000
#define HWIO_MSS_MPLL1_CONFIG_CTL_OVER_VOLTAGE_DET_COUNTER_DIS_SHFT                          0x19
#define HWIO_MSS_MPLL1_CONFIG_CTL_MASH11_FRAC_N_MODE_CTL_BMSK                           0x1000000
#define HWIO_MSS_MPLL1_CONFIG_CTL_MASH11_FRAC_N_MODE_CTL_SHFT                                0x18
#define HWIO_MSS_MPLL1_CONFIG_CTL_RESERVE_23_BMSK                                        0x800000
#define HWIO_MSS_MPLL1_CONFIG_CTL_RESERVE_23_SHFT                                            0x17
#define HWIO_MSS_MPLL1_CONFIG_CTL_RESERVE_22_19_BMSK                                     0x780000
#define HWIO_MSS_MPLL1_CONFIG_CTL_RESERVE_22_19_SHFT                                         0x13
#define HWIO_MSS_MPLL1_CONFIG_CTL_FILTER_TIMING_CFG_BMSK                                  0x40000
#define HWIO_MSS_MPLL1_CONFIG_CTL_FILTER_TIMING_CFG_SHFT                                     0x12
#define HWIO_MSS_MPLL1_CONFIG_CTL_DOUBLE_SAMPLE_FILTER_EN_BMSK                            0x20000
#define HWIO_MSS_MPLL1_CONFIG_CTL_DOUBLE_SAMPLE_FILTER_EN_SHFT                               0x11
#define HWIO_MSS_MPLL1_CONFIG_CTL_OUTPUT_VOLT_SETTING_BMSK                                0x18000
#define HWIO_MSS_MPLL1_CONFIG_CTL_OUTPUT_VOLT_SETTING_SHFT                                    0xf
#define HWIO_MSS_MPLL1_CONFIG_CTL_ISEED_GATE_EN_BMSK                                       0x4000
#define HWIO_MSS_MPLL1_CONFIG_CTL_ISEED_GATE_EN_SHFT                                          0xe
#define HWIO_MSS_MPLL1_CONFIG_CTL_VCO_DECAP_DIS_BMSK                                       0x2000
#define HWIO_MSS_MPLL1_CONFIG_CTL_VCO_DECAP_DIS_SHFT                                          0xd
#define HWIO_MSS_MPLL1_CONFIG_CTL_STAGE1_REGULATOR_BYPASS_BMSK                             0x1000
#define HWIO_MSS_MPLL1_CONFIG_CTL_STAGE1_REGULATOR_BYPASS_SHFT                                0xc
#define HWIO_MSS_MPLL1_CONFIG_CTL_STAGE2_IREG_DIV_BMSK                                      0xc00
#define HWIO_MSS_MPLL1_CONFIG_CTL_STAGE2_IREG_DIV_SHFT                                        0xa
#define HWIO_MSS_MPLL1_CONFIG_CTL_STAGE2_OVER_VOLTAGE_DET_CFG_BMSK                          0x300
#define HWIO_MSS_MPLL1_CONFIG_CTL_STAGE2_OVER_VOLTAGE_DET_CFG_SHFT                            0x8
#define HWIO_MSS_MPLL1_CONFIG_CTL_FILTER_PHASE_OFFSET_CFG_BMSK                               0xc0
#define HWIO_MSS_MPLL1_CONFIG_CTL_FILTER_PHASE_OFFSET_CFG_SHFT                                0x6
#define HWIO_MSS_MPLL1_CONFIG_CTL_ISEED_CTRL_CFG_BMSK                                        0x30
#define HWIO_MSS_MPLL1_CONFIG_CTL_ISEED_CTRL_CFG_SHFT                                         0x4
#define HWIO_MSS_MPLL1_CONFIG_CTL_ICP_DIV_BMSK                                                0xc
#define HWIO_MSS_MPLL1_CONFIG_CTL_ICP_DIV_SHFT                                                0x2
#define HWIO_MSS_MPLL1_CONFIG_CTL_REF_PATH_DELAY_BMSK                                         0x3
#define HWIO_MSS_MPLL1_CONFIG_CTL_REF_PATH_DELAY_SHFT                                         0x0

#define HWIO_MSS_MPLL1_TEST_CTL_ADDR                                                   (MSS_PERPH_REG_BASE      + 0x00001038)
#define HWIO_MSS_MPLL1_TEST_CTL_RMSK                                                   0xffffffff
#define HWIO_MSS_MPLL1_TEST_CTL_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_TEST_CTL_ADDR, HWIO_MSS_MPLL1_TEST_CTL_RMSK)
#define HWIO_MSS_MPLL1_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_TEST_CTL_ADDR, m)
#define HWIO_MSS_MPLL1_TEST_CTL_OUT(v)      \
        out_dword(HWIO_MSS_MPLL1_TEST_CTL_ADDR,v)
#define HWIO_MSS_MPLL1_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_MPLL1_TEST_CTL_ADDR,m,v,HWIO_MSS_MPLL1_TEST_CTL_IN)
#define HWIO_MSS_MPLL1_TEST_CTL_RESERVE_31_23_BMSK                                     0xff800000
#define HWIO_MSS_MPLL1_TEST_CTL_RESERVE_31_23_SHFT                                           0x17
#define HWIO_MSS_MPLL1_TEST_CTL_NOISE_GEN_RES_MAX_I_BMSK                                 0x700000
#define HWIO_MSS_MPLL1_TEST_CTL_NOISE_GEN_RES_MAX_I_SHFT                                     0x14
#define HWIO_MSS_MPLL1_TEST_CTL_NOISE_GEN_EN_BMSK                                         0x80000
#define HWIO_MSS_MPLL1_TEST_CTL_NOISE_GEN_EN_SHFT                                            0x13
#define HWIO_MSS_MPLL1_TEST_CTL_OSC_DRIVER_EN_BMSK                                        0x60000
#define HWIO_MSS_MPLL1_TEST_CTL_OSC_DRIVER_EN_SHFT                                           0x11
#define HWIO_MSS_MPLL1_TEST_CTL_PLLOUT_LV_TEST_SEL_BMSK                                   0x10000
#define HWIO_MSS_MPLL1_TEST_CTL_PLLOUT_LV_TEST_SEL_SHFT                                      0x10
#define HWIO_MSS_MPLL1_TEST_CTL_RESERVE_15_BMSK                                            0x8000
#define HWIO_MSS_MPLL1_TEST_CTL_RESERVE_15_SHFT                                               0xf
#define HWIO_MSS_MPLL1_TEST_CTL_EARLY_DIV2_EN_BMSK                                         0x4000
#define HWIO_MSS_MPLL1_TEST_CTL_EARLY_DIV2_EN_SHFT                                            0xe
#define HWIO_MSS_MPLL1_TEST_CTL_BYP_TESTAMP_BMSK                                           0x2000
#define HWIO_MSS_MPLL1_TEST_CTL_BYP_TESTAMP_SHFT                                              0xd
#define HWIO_MSS_MPLL1_TEST_CTL_DTEST_SEL_BMSK                                             0x1000
#define HWIO_MSS_MPLL1_TEST_CTL_DTEST_SEL_SHFT                                                0xc
#define HWIO_MSS_MPLL1_TEST_CTL_RESERVE_11_8_BMSK                                           0xf00
#define HWIO_MSS_MPLL1_TEST_CTL_RESERVE_11_8_SHFT                                             0x8
#define HWIO_MSS_MPLL1_TEST_CTL_PLLOUT_HV_EN_BMSK                                            0x80
#define HWIO_MSS_MPLL1_TEST_CTL_PLLOUT_HV_EN_SHFT                                             0x7
#define HWIO_MSS_MPLL1_TEST_CTL_ATEST0_SEL_BMSK                                              0x40
#define HWIO_MSS_MPLL1_TEST_CTL_ATEST0_SEL_SHFT                                               0x6
#define HWIO_MSS_MPLL1_TEST_CTL_ICP_EXT_SEL_BMSK                                             0x20
#define HWIO_MSS_MPLL1_TEST_CTL_ICP_EXT_SEL_SHFT                                              0x5
#define HWIO_MSS_MPLL1_TEST_CTL_ATEST1_SEL_BMSK                                              0x10
#define HWIO_MSS_MPLL1_TEST_CTL_ATEST1_SEL_SHFT                                               0x4
#define HWIO_MSS_MPLL1_TEST_CTL_FILTER_TESTAMP_EN_BMSK                                        0x8
#define HWIO_MSS_MPLL1_TEST_CTL_FILTER_TESTAMP_EN_SHFT                                        0x3
#define HWIO_MSS_MPLL1_TEST_CTL_ATEST0_EXT_VOLT_FORCE_EN_BMSK                                 0x4
#define HWIO_MSS_MPLL1_TEST_CTL_ATEST0_EXT_VOLT_FORCE_EN_SHFT                                 0x2
#define HWIO_MSS_MPLL1_TEST_CTL_VINT_TESTAMP_ATEST0_BMSK                                      0x2
#define HWIO_MSS_MPLL1_TEST_CTL_VINT_TESTAMP_ATEST0_SHFT                                      0x1
#define HWIO_MSS_MPLL1_TEST_CTL_VREG_TESTAMP_ATEST0_BMSK                                      0x1
#define HWIO_MSS_MPLL1_TEST_CTL_VREG_TESTAMP_ATEST0_SHFT                                      0x0

#define HWIO_MSS_MPLL1_STATUS_ADDR                                                     (MSS_PERPH_REG_BASE      + 0x0000103c)
#define HWIO_MSS_MPLL1_STATUS_RMSK                                                        0x3ffff
#define HWIO_MSS_MPLL1_STATUS_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_STATUS_ADDR, HWIO_MSS_MPLL1_STATUS_RMSK)
#define HWIO_MSS_MPLL1_STATUS_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_STATUS_ADDR, m)
#define HWIO_MSS_MPLL1_STATUS_PLL_ACTIVE_FLAG_BMSK                                        0x20000
#define HWIO_MSS_MPLL1_STATUS_PLL_ACTIVE_FLAG_SHFT                                           0x11
#define HWIO_MSS_MPLL1_STATUS_PLL_LOCK_DET_BMSK                                           0x10000
#define HWIO_MSS_MPLL1_STATUS_PLL_LOCK_DET_SHFT                                              0x10
#define HWIO_MSS_MPLL1_STATUS_PLL_D_BMSK                                                   0xffff
#define HWIO_MSS_MPLL1_STATUS_PLL_D_SHFT                                                      0x0

#define HWIO_MSS_MPLL1_EARLY_BUS_CBCR_ADDR                                             (MSS_PERPH_REG_BASE      + 0x000010ec)
#define HWIO_MSS_MPLL1_EARLY_BUS_CBCR_RMSK                                             0x80000000
#define HWIO_MSS_MPLL1_EARLY_BUS_CBCR_IN          \
        in_dword_masked(HWIO_MSS_MPLL1_EARLY_BUS_CBCR_ADDR, HWIO_MSS_MPLL1_EARLY_BUS_CBCR_RMSK)
#define HWIO_MSS_MPLL1_EARLY_BUS_CBCR_INM(m)      \
        in_dword_masked(HWIO_MSS_MPLL1_EARLY_BUS_CBCR_ADDR, m)
#define HWIO_MSS_MPLL1_EARLY_BUS_CBCR_CLKOFF_BMSK                                      0x80000000
#define HWIO_MSS_MPLL1_EARLY_BUS_CBCR_CLKOFF_SHFT                                            0x1f

#define HWIO_MSS_BUS_CMD_RCGR_ADDR                                                     (MSS_PERPH_REG_BASE      + 0x00001108)
#define HWIO_MSS_BUS_CMD_RCGR_RMSK                                                     0x80000001
#define HWIO_MSS_BUS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CMD_RCGR_ADDR, HWIO_MSS_BUS_CMD_RCGR_RMSK)
#define HWIO_MSS_BUS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CMD_RCGR_ADDR, m)
#define HWIO_MSS_BUS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CMD_RCGR_ADDR,v)
#define HWIO_MSS_BUS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CMD_RCGR_ADDR,m,v,HWIO_MSS_BUS_CMD_RCGR_IN)
#define HWIO_MSS_BUS_CMD_RCGR_ROOT_OFF_BMSK                                            0x80000000
#define HWIO_MSS_BUS_CMD_RCGR_ROOT_OFF_SHFT                                                  0x1f
#define HWIO_MSS_BUS_CMD_RCGR_UPDATE_BMSK                                                     0x1
#define HWIO_MSS_BUS_CMD_RCGR_UPDATE_SHFT                                                     0x0

#define HWIO_MSS_BUS_CFG_RCGR_ADDR                                                     (MSS_PERPH_REG_BASE      + 0x0000110c)
#define HWIO_MSS_BUS_CFG_RCGR_RMSK                                                          0x71f
#define HWIO_MSS_BUS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_MSS_BUS_CFG_RCGR_ADDR, HWIO_MSS_BUS_CFG_RCGR_RMSK)
#define HWIO_MSS_BUS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_MSS_BUS_CFG_RCGR_ADDR, m)
#define HWIO_MSS_BUS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_MSS_BUS_CFG_RCGR_ADDR,v)
#define HWIO_MSS_BUS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_BUS_CFG_RCGR_ADDR,m,v,HWIO_MSS_BUS_CFG_RCGR_IN)
#define HWIO_MSS_BUS_CFG_RCGR_SRC_SEL_BMSK                                                  0x700
#define HWIO_MSS_BUS_CFG_RCGR_SRC_SEL_SHFT                                                    0x8
#define HWIO_MSS_BUS_CFG_RCGR_SRC_DIV_BMSK                                                   0x1f
#define HWIO_MSS_BUS_CFG_RCGR_SRC_DIV_SHFT                                                    0x0

/*----------------------------------------------------------------------------
 * MODULE: TCSR_TCSR_REGS
 *--------------------------------------------------------------------------*/

#define TCSR_TCSR_REGS_REG_BASE                                                                                            (CORE_TOP_CSR_BASE      + 0x00037000)
#define TCSR_TCSR_REGS_REG_BASE_SIZE                                                                                       0x21000
#define TCSR_TCSR_REGS_REG_BASE_USED                                                                                       0x20000

#define HWIO_TCSR_SOC_HW_VERSION_ADDR                                                                                      (TCSR_TCSR_REGS_REG_BASE      + 0x00016000)
#define HWIO_TCSR_SOC_HW_VERSION_RMSK                                                                                      0xffffffff
#define HWIO_TCSR_SOC_HW_VERSION_IN          \
        in_dword_masked(HWIO_TCSR_SOC_HW_VERSION_ADDR, HWIO_TCSR_SOC_HW_VERSION_RMSK)
#define HWIO_TCSR_SOC_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_TCSR_SOC_HW_VERSION_ADDR, m)
#define HWIO_TCSR_SOC_HW_VERSION_FAMILY_NUMBER_BMSK                                                                        0xf0000000
#define HWIO_TCSR_SOC_HW_VERSION_FAMILY_NUMBER_SHFT                                                                              0x1c
#define HWIO_TCSR_SOC_HW_VERSION_DEVICE_NUMBER_BMSK                                                                         0xfff0000
#define HWIO_TCSR_SOC_HW_VERSION_DEVICE_NUMBER_SHFT                                                                              0x10
#define HWIO_TCSR_SOC_HW_VERSION_MAJOR_VERSION_BMSK                                                                            0xff00
#define HWIO_TCSR_SOC_HW_VERSION_MAJOR_VERSION_SHFT                                                                               0x8
#define HWIO_TCSR_SOC_HW_VERSION_MINOR_VERSION_BMSK                                                                              0xff
#define HWIO_TCSR_SOC_HW_VERSION_MINOR_VERSION_SHFT                                                                               0x0


#endif /* __HALCLKHWIOASM_H__ */
