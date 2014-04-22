#ifndef __HWIO_GCC_SDCC_H__
#define __HWIO_GCC_SDCC_H__
/*
===========================================================================
*/
/**
  @file hwio_gcc_sdcc.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    QCS405 (Vipertooth) [vipertooth_prj_1.0_p3q3r103]
 
  This file contains HWIO register definitions for the following modules:
    GCC_CLK_CTL_REG


  Generation parameters: 
  { u'filename': u'hwio_gcc_sdcc.h',
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [u'GCC_CLK_CTL_REG']}
*/
/*
  ===========================================================================

  Copyright (c) 2018 Qualcomm Technologies, Inc.
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

  $Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Settings/SDCC/inc/hwio_gcc_sdcc.h#1 $
  $DateTime: 2018/08/08 05:31:55 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: GCC_CLK_CTL_REG
 *--------------------------------------------------------------------------*/

#define GCC_CLK_CTL_REG_REG_BASE                                                               (CLK_CTL_BASE      + 0x00000000)
#define GCC_CLK_CTL_REG_REG_BASE_SIZE                                                          0x80000
#define GCC_CLK_CTL_REG_REG_BASE_USED                                                          0x7f000

#define HWIO_GCC_SDCC1_BCR_ADDR                                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x00042000)
#define HWIO_GCC_SDCC1_BCR_RMSK                                                                       0x1
#define HWIO_GCC_SDCC1_BCR_IN          \
        in_dword_masked(HWIO_GCC_SDCC1_BCR_ADDR, HWIO_GCC_SDCC1_BCR_RMSK)
#define HWIO_GCC_SDCC1_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC1_BCR_ADDR, m)
#define HWIO_GCC_SDCC1_BCR_OUT(v)      \
        out_dword(HWIO_GCC_SDCC1_BCR_ADDR,v)
#define HWIO_GCC_SDCC1_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC1_BCR_ADDR,m,v,HWIO_GCC_SDCC1_BCR_IN)
#define HWIO_GCC_SDCC1_BCR_BLK_ARES_BMSK                                                              0x1
#define HWIO_GCC_SDCC1_BCR_BLK_ARES_SHFT                                                              0x0

#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00042004)
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_RMSK                                                      0x800000f3
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_SDCC1_APPS_CMD_RCGR_ADDR, HWIO_GCC_SDCC1_APPS_CMD_RCGR_RMSK)
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC1_APPS_CMD_RCGR_ADDR, m)
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_SDCC1_APPS_CMD_RCGR_ADDR,v)
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC1_APPS_CMD_RCGR_ADDR,m,v,HWIO_GCC_SDCC1_APPS_CMD_RCGR_IN)
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_ROOT_OFF_BMSK                                             0x80000000
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_ROOT_OFF_SHFT                                                   0x1f
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_DIRTY_D_BMSK                                                    0x80
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_DIRTY_D_SHFT                                                     0x7
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_DIRTY_M_BMSK                                                    0x40
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_DIRTY_M_SHFT                                                     0x6
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_DIRTY_N_BMSK                                                    0x20
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_DIRTY_N_SHFT                                                     0x5
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                             0x10
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                              0x4
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_ROOT_EN_BMSK                                                     0x2
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_ROOT_EN_SHFT                                                     0x1
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_UPDATE_BMSK                                                      0x1
#define HWIO_GCC_SDCC1_APPS_CMD_RCGR_UPDATE_SHFT                                                      0x0

#define HWIO_GCC_SDCC1_APPS_CFG_RCGR_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00042008)
#define HWIO_GCC_SDCC1_APPS_CFG_RCGR_RMSK                                                          0x371f
#define HWIO_GCC_SDCC1_APPS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_SDCC1_APPS_CFG_RCGR_ADDR, HWIO_GCC_SDCC1_APPS_CFG_RCGR_RMSK)
#define HWIO_GCC_SDCC1_APPS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC1_APPS_CFG_RCGR_ADDR, m)
#define HWIO_GCC_SDCC1_APPS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_SDCC1_APPS_CFG_RCGR_ADDR,v)
#define HWIO_GCC_SDCC1_APPS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC1_APPS_CFG_RCGR_ADDR,m,v,HWIO_GCC_SDCC1_APPS_CFG_RCGR_IN)
#define HWIO_GCC_SDCC1_APPS_CFG_RCGR_MODE_BMSK                                                     0x3000
#define HWIO_GCC_SDCC1_APPS_CFG_RCGR_MODE_SHFT                                                        0xc
#define HWIO_GCC_SDCC1_APPS_CFG_RCGR_SRC_SEL_BMSK                                                   0x700
#define HWIO_GCC_SDCC1_APPS_CFG_RCGR_SRC_SEL_SHFT                                                     0x8
#define HWIO_GCC_SDCC1_APPS_CFG_RCGR_SRC_DIV_BMSK                                                    0x1f
#define HWIO_GCC_SDCC1_APPS_CFG_RCGR_SRC_DIV_SHFT                                                     0x0

#define HWIO_GCC_SDCC1_APPS_M_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0004200c)
#define HWIO_GCC_SDCC1_APPS_M_RMSK                                                                   0xff
#define HWIO_GCC_SDCC1_APPS_M_IN          \
        in_dword_masked(HWIO_GCC_SDCC1_APPS_M_ADDR, HWIO_GCC_SDCC1_APPS_M_RMSK)
#define HWIO_GCC_SDCC1_APPS_M_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC1_APPS_M_ADDR, m)
#define HWIO_GCC_SDCC1_APPS_M_OUT(v)      \
        out_dword(HWIO_GCC_SDCC1_APPS_M_ADDR,v)
#define HWIO_GCC_SDCC1_APPS_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC1_APPS_M_ADDR,m,v,HWIO_GCC_SDCC1_APPS_M_IN)
#define HWIO_GCC_SDCC1_APPS_M_M_BMSK                                                                 0xff
#define HWIO_GCC_SDCC1_APPS_M_M_SHFT                                                                  0x0

#define HWIO_GCC_SDCC1_APPS_N_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00042010)
#define HWIO_GCC_SDCC1_APPS_N_RMSK                                                                   0xff
#define HWIO_GCC_SDCC1_APPS_N_IN          \
        in_dword_masked(HWIO_GCC_SDCC1_APPS_N_ADDR, HWIO_GCC_SDCC1_APPS_N_RMSK)
#define HWIO_GCC_SDCC1_APPS_N_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC1_APPS_N_ADDR, m)
#define HWIO_GCC_SDCC1_APPS_N_OUT(v)      \
        out_dword(HWIO_GCC_SDCC1_APPS_N_ADDR,v)
#define HWIO_GCC_SDCC1_APPS_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC1_APPS_N_ADDR,m,v,HWIO_GCC_SDCC1_APPS_N_IN)
#define HWIO_GCC_SDCC1_APPS_N_NOT_N_MINUS_M_BMSK                                                     0xff
#define HWIO_GCC_SDCC1_APPS_N_NOT_N_MINUS_M_SHFT                                                      0x0

#define HWIO_GCC_SDCC1_APPS_D_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00042014)
#define HWIO_GCC_SDCC1_APPS_D_RMSK                                                                   0xff
#define HWIO_GCC_SDCC1_APPS_D_IN          \
        in_dword_masked(HWIO_GCC_SDCC1_APPS_D_ADDR, HWIO_GCC_SDCC1_APPS_D_RMSK)
#define HWIO_GCC_SDCC1_APPS_D_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC1_APPS_D_ADDR, m)
#define HWIO_GCC_SDCC1_APPS_D_OUT(v)      \
        out_dword(HWIO_GCC_SDCC1_APPS_D_ADDR,v)
#define HWIO_GCC_SDCC1_APPS_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC1_APPS_D_ADDR,m,v,HWIO_GCC_SDCC1_APPS_D_IN)
#define HWIO_GCC_SDCC1_APPS_D_NOT_2D_BMSK                                                            0xff
#define HWIO_GCC_SDCC1_APPS_D_NOT_2D_SHFT                                                             0x0

#define HWIO_GCC_SDCC1_APPS_CBCR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x00042018)
#define HWIO_GCC_SDCC1_APPS_CBCR_RMSK                                                          0x80007ff1
#define HWIO_GCC_SDCC1_APPS_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SDCC1_APPS_CBCR_ADDR, HWIO_GCC_SDCC1_APPS_CBCR_RMSK)
#define HWIO_GCC_SDCC1_APPS_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC1_APPS_CBCR_ADDR, m)
#define HWIO_GCC_SDCC1_APPS_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SDCC1_APPS_CBCR_ADDR,v)
#define HWIO_GCC_SDCC1_APPS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC1_APPS_CBCR_ADDR,m,v,HWIO_GCC_SDCC1_APPS_CBCR_IN)
#define HWIO_GCC_SDCC1_APPS_CBCR_CLK_OFF_BMSK                                                  0x80000000
#define HWIO_GCC_SDCC1_APPS_CBCR_CLK_OFF_SHFT                                                        0x1f
#define HWIO_GCC_SDCC1_APPS_CBCR_FORCE_MEM_CORE_ON_BMSK                                            0x4000
#define HWIO_GCC_SDCC1_APPS_CBCR_FORCE_MEM_CORE_ON_SHFT                                               0xe
#define HWIO_GCC_SDCC1_APPS_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                          0x2000
#define HWIO_GCC_SDCC1_APPS_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                             0xd
#define HWIO_GCC_SDCC1_APPS_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                         0x1000
#define HWIO_GCC_SDCC1_APPS_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                            0xc
#define HWIO_GCC_SDCC1_APPS_CBCR_WAKEUP_BMSK                                                        0xf00
#define HWIO_GCC_SDCC1_APPS_CBCR_WAKEUP_SHFT                                                          0x8
#define HWIO_GCC_SDCC1_APPS_CBCR_SLEEP_BMSK                                                          0xf0
#define HWIO_GCC_SDCC1_APPS_CBCR_SLEEP_SHFT                                                           0x4
#define HWIO_GCC_SDCC1_APPS_CBCR_CLK_ENABLE_BMSK                                                      0x1
#define HWIO_GCC_SDCC1_APPS_CBCR_CLK_ENABLE_SHFT                                                      0x0

#define HWIO_GCC_SDCC1_AHB_CBCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x0004201c)
#define HWIO_GCC_SDCC1_AHB_CBCR_RMSK                                                           0xf000fff1
#define HWIO_GCC_SDCC1_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SDCC1_AHB_CBCR_ADDR, HWIO_GCC_SDCC1_AHB_CBCR_RMSK)
#define HWIO_GCC_SDCC1_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC1_AHB_CBCR_ADDR, m)
#define HWIO_GCC_SDCC1_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SDCC1_AHB_CBCR_ADDR,v)
#define HWIO_GCC_SDCC1_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC1_AHB_CBCR_ADDR,m,v,HWIO_GCC_SDCC1_AHB_CBCR_IN)
#define HWIO_GCC_SDCC1_AHB_CBCR_CLK_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_SDCC1_AHB_CBCR_CLK_OFF_SHFT                                                         0x1f
#define HWIO_GCC_SDCC1_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                  0x70000000
#define HWIO_GCC_SDCC1_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                        0x1c
#define HWIO_GCC_SDCC1_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                          0x8000
#define HWIO_GCC_SDCC1_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                             0xf
#define HWIO_GCC_SDCC1_AHB_CBCR_FORCE_MEM_CORE_ON_BMSK                                             0x4000
#define HWIO_GCC_SDCC1_AHB_CBCR_FORCE_MEM_CORE_ON_SHFT                                                0xe
#define HWIO_GCC_SDCC1_AHB_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                           0x2000
#define HWIO_GCC_SDCC1_AHB_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                              0xd
#define HWIO_GCC_SDCC1_AHB_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                          0x1000
#define HWIO_GCC_SDCC1_AHB_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                             0xc
#define HWIO_GCC_SDCC1_AHB_CBCR_WAKEUP_BMSK                                                         0xf00
#define HWIO_GCC_SDCC1_AHB_CBCR_WAKEUP_SHFT                                                           0x8
#define HWIO_GCC_SDCC1_AHB_CBCR_SLEEP_BMSK                                                           0xf0
#define HWIO_GCC_SDCC1_AHB_CBCR_SLEEP_SHFT                                                            0x4
#define HWIO_GCC_SDCC1_AHB_CBCR_CLK_ENABLE_BMSK                                                       0x1
#define HWIO_GCC_SDCC1_AHB_CBCR_CLK_ENABLE_SHFT                                                       0x0

#define HWIO_GCC_SDCC1_MISC_ADDR                                                               (GCC_CLK_CTL_REG_REG_BASE      + 0x00042020)
#define HWIO_GCC_SDCC1_MISC_RMSK                                                                      0x1
#define HWIO_GCC_SDCC1_MISC_IN          \
        in_dword_masked(HWIO_GCC_SDCC1_MISC_ADDR, HWIO_GCC_SDCC1_MISC_RMSK)
#define HWIO_GCC_SDCC1_MISC_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC1_MISC_ADDR, m)
#define HWIO_GCC_SDCC1_MISC_OUT(v)      \
        out_dword(HWIO_GCC_SDCC1_MISC_ADDR,v)
#define HWIO_GCC_SDCC1_MISC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC1_MISC_ADDR,m,v,HWIO_GCC_SDCC1_MISC_IN)
#define HWIO_GCC_SDCC1_MISC_CDC_ARES_BMSK                                                             0x1
#define HWIO_GCC_SDCC1_MISC_CDC_ARES_SHFT                                                             0x0

#define HWIO_GCC_SDCC2_BCR_ADDR                                                                (GCC_CLK_CTL_REG_REG_BASE      + 0x00043000)
#define HWIO_GCC_SDCC2_BCR_RMSK                                                                       0x1
#define HWIO_GCC_SDCC2_BCR_IN          \
        in_dword_masked(HWIO_GCC_SDCC2_BCR_ADDR, HWIO_GCC_SDCC2_BCR_RMSK)
#define HWIO_GCC_SDCC2_BCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC2_BCR_ADDR, m)
#define HWIO_GCC_SDCC2_BCR_OUT(v)      \
        out_dword(HWIO_GCC_SDCC2_BCR_ADDR,v)
#define HWIO_GCC_SDCC2_BCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC2_BCR_ADDR,m,v,HWIO_GCC_SDCC2_BCR_IN)
#define HWIO_GCC_SDCC2_BCR_BLK_ARES_BMSK                                                              0x1
#define HWIO_GCC_SDCC2_BCR_BLK_ARES_SHFT                                                              0x0

#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00043004)
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_RMSK                                                      0x800000f3
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_SDCC2_APPS_CMD_RCGR_ADDR, HWIO_GCC_SDCC2_APPS_CMD_RCGR_RMSK)
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC2_APPS_CMD_RCGR_ADDR, m)
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_SDCC2_APPS_CMD_RCGR_ADDR,v)
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC2_APPS_CMD_RCGR_ADDR,m,v,HWIO_GCC_SDCC2_APPS_CMD_RCGR_IN)
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_ROOT_OFF_BMSK                                             0x80000000
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_ROOT_OFF_SHFT                                                   0x1f
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_DIRTY_D_BMSK                                                    0x80
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_DIRTY_D_SHFT                                                     0x7
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_DIRTY_M_BMSK                                                    0x40
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_DIRTY_M_SHFT                                                     0x6
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_DIRTY_N_BMSK                                                    0x20
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_DIRTY_N_SHFT                                                     0x5
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                             0x10
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                              0x4
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_ROOT_EN_BMSK                                                     0x2
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_ROOT_EN_SHFT                                                     0x1
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_UPDATE_BMSK                                                      0x1
#define HWIO_GCC_SDCC2_APPS_CMD_RCGR_UPDATE_SHFT                                                      0x0

#define HWIO_GCC_SDCC2_APPS_CFG_RCGR_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x00043008)
#define HWIO_GCC_SDCC2_APPS_CFG_RCGR_RMSK                                                          0x371f
#define HWIO_GCC_SDCC2_APPS_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_SDCC2_APPS_CFG_RCGR_ADDR, HWIO_GCC_SDCC2_APPS_CFG_RCGR_RMSK)
#define HWIO_GCC_SDCC2_APPS_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC2_APPS_CFG_RCGR_ADDR, m)
#define HWIO_GCC_SDCC2_APPS_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_SDCC2_APPS_CFG_RCGR_ADDR,v)
#define HWIO_GCC_SDCC2_APPS_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC2_APPS_CFG_RCGR_ADDR,m,v,HWIO_GCC_SDCC2_APPS_CFG_RCGR_IN)
#define HWIO_GCC_SDCC2_APPS_CFG_RCGR_MODE_BMSK                                                     0x3000
#define HWIO_GCC_SDCC2_APPS_CFG_RCGR_MODE_SHFT                                                        0xc
#define HWIO_GCC_SDCC2_APPS_CFG_RCGR_SRC_SEL_BMSK                                                   0x700
#define HWIO_GCC_SDCC2_APPS_CFG_RCGR_SRC_SEL_SHFT                                                     0x8
#define HWIO_GCC_SDCC2_APPS_CFG_RCGR_SRC_DIV_BMSK                                                    0x1f
#define HWIO_GCC_SDCC2_APPS_CFG_RCGR_SRC_DIV_SHFT                                                     0x0

#define HWIO_GCC_SDCC2_APPS_M_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x0004300c)
#define HWIO_GCC_SDCC2_APPS_M_RMSK                                                                   0xff
#define HWIO_GCC_SDCC2_APPS_M_IN          \
        in_dword_masked(HWIO_GCC_SDCC2_APPS_M_ADDR, HWIO_GCC_SDCC2_APPS_M_RMSK)
#define HWIO_GCC_SDCC2_APPS_M_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC2_APPS_M_ADDR, m)
#define HWIO_GCC_SDCC2_APPS_M_OUT(v)      \
        out_dword(HWIO_GCC_SDCC2_APPS_M_ADDR,v)
#define HWIO_GCC_SDCC2_APPS_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC2_APPS_M_ADDR,m,v,HWIO_GCC_SDCC2_APPS_M_IN)
#define HWIO_GCC_SDCC2_APPS_M_M_BMSK                                                                 0xff
#define HWIO_GCC_SDCC2_APPS_M_M_SHFT                                                                  0x0

#define HWIO_GCC_SDCC2_APPS_N_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00043010)
#define HWIO_GCC_SDCC2_APPS_N_RMSK                                                                   0xff
#define HWIO_GCC_SDCC2_APPS_N_IN          \
        in_dword_masked(HWIO_GCC_SDCC2_APPS_N_ADDR, HWIO_GCC_SDCC2_APPS_N_RMSK)
#define HWIO_GCC_SDCC2_APPS_N_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC2_APPS_N_ADDR, m)
#define HWIO_GCC_SDCC2_APPS_N_OUT(v)      \
        out_dword(HWIO_GCC_SDCC2_APPS_N_ADDR,v)
#define HWIO_GCC_SDCC2_APPS_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC2_APPS_N_ADDR,m,v,HWIO_GCC_SDCC2_APPS_N_IN)
#define HWIO_GCC_SDCC2_APPS_N_NOT_N_MINUS_M_BMSK                                                     0xff
#define HWIO_GCC_SDCC2_APPS_N_NOT_N_MINUS_M_SHFT                                                      0x0

#define HWIO_GCC_SDCC2_APPS_D_ADDR                                                             (GCC_CLK_CTL_REG_REG_BASE      + 0x00043014)
#define HWIO_GCC_SDCC2_APPS_D_RMSK                                                                   0xff
#define HWIO_GCC_SDCC2_APPS_D_IN          \
        in_dword_masked(HWIO_GCC_SDCC2_APPS_D_ADDR, HWIO_GCC_SDCC2_APPS_D_RMSK)
#define HWIO_GCC_SDCC2_APPS_D_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC2_APPS_D_ADDR, m)
#define HWIO_GCC_SDCC2_APPS_D_OUT(v)      \
        out_dword(HWIO_GCC_SDCC2_APPS_D_ADDR,v)
#define HWIO_GCC_SDCC2_APPS_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC2_APPS_D_ADDR,m,v,HWIO_GCC_SDCC2_APPS_D_IN)
#define HWIO_GCC_SDCC2_APPS_D_NOT_2D_BMSK                                                            0xff
#define HWIO_GCC_SDCC2_APPS_D_NOT_2D_SHFT                                                             0x0

#define HWIO_GCC_SDCC2_APPS_CBCR_ADDR                                                          (GCC_CLK_CTL_REG_REG_BASE      + 0x00043018)
#define HWIO_GCC_SDCC2_APPS_CBCR_RMSK                                                          0x80007ff1
#define HWIO_GCC_SDCC2_APPS_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SDCC2_APPS_CBCR_ADDR, HWIO_GCC_SDCC2_APPS_CBCR_RMSK)
#define HWIO_GCC_SDCC2_APPS_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC2_APPS_CBCR_ADDR, m)
#define HWIO_GCC_SDCC2_APPS_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SDCC2_APPS_CBCR_ADDR,v)
#define HWIO_GCC_SDCC2_APPS_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC2_APPS_CBCR_ADDR,m,v,HWIO_GCC_SDCC2_APPS_CBCR_IN)
#define HWIO_GCC_SDCC2_APPS_CBCR_CLK_OFF_BMSK                                                  0x80000000
#define HWIO_GCC_SDCC2_APPS_CBCR_CLK_OFF_SHFT                                                        0x1f
#define HWIO_GCC_SDCC2_APPS_CBCR_FORCE_MEM_CORE_ON_BMSK                                            0x4000
#define HWIO_GCC_SDCC2_APPS_CBCR_FORCE_MEM_CORE_ON_SHFT                                               0xe
#define HWIO_GCC_SDCC2_APPS_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                          0x2000
#define HWIO_GCC_SDCC2_APPS_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                             0xd
#define HWIO_GCC_SDCC2_APPS_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                         0x1000
#define HWIO_GCC_SDCC2_APPS_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                            0xc
#define HWIO_GCC_SDCC2_APPS_CBCR_WAKEUP_BMSK                                                        0xf00
#define HWIO_GCC_SDCC2_APPS_CBCR_WAKEUP_SHFT                                                          0x8
#define HWIO_GCC_SDCC2_APPS_CBCR_SLEEP_BMSK                                                          0xf0
#define HWIO_GCC_SDCC2_APPS_CBCR_SLEEP_SHFT                                                           0x4
#define HWIO_GCC_SDCC2_APPS_CBCR_CLK_ENABLE_BMSK                                                      0x1
#define HWIO_GCC_SDCC2_APPS_CBCR_CLK_ENABLE_SHFT                                                      0x0

#define HWIO_GCC_SDCC2_AHB_CBCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x0004301c)
#define HWIO_GCC_SDCC2_AHB_CBCR_RMSK                                                           0xf000fff1
#define HWIO_GCC_SDCC2_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SDCC2_AHB_CBCR_ADDR, HWIO_GCC_SDCC2_AHB_CBCR_RMSK)
#define HWIO_GCC_SDCC2_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC2_AHB_CBCR_ADDR, m)
#define HWIO_GCC_SDCC2_AHB_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SDCC2_AHB_CBCR_ADDR,v)
#define HWIO_GCC_SDCC2_AHB_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC2_AHB_CBCR_ADDR,m,v,HWIO_GCC_SDCC2_AHB_CBCR_IN)
#define HWIO_GCC_SDCC2_AHB_CBCR_CLK_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_SDCC2_AHB_CBCR_CLK_OFF_SHFT                                                         0x1f
#define HWIO_GCC_SDCC2_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_BMSK                                  0x70000000
#define HWIO_GCC_SDCC2_AHB_CBCR_NOC_HANDSHAKE_FSM_STATUS_SHFT                                        0x1c
#define HWIO_GCC_SDCC2_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_BMSK                                          0x8000
#define HWIO_GCC_SDCC2_AHB_CBCR_NOC_HANDSHAKE_FSM_EN_SHFT                                             0xf
#define HWIO_GCC_SDCC2_AHB_CBCR_FORCE_MEM_CORE_ON_BMSK                                             0x4000
#define HWIO_GCC_SDCC2_AHB_CBCR_FORCE_MEM_CORE_ON_SHFT                                                0xe
#define HWIO_GCC_SDCC2_AHB_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                           0x2000
#define HWIO_GCC_SDCC2_AHB_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                              0xd
#define HWIO_GCC_SDCC2_AHB_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                          0x1000
#define HWIO_GCC_SDCC2_AHB_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                             0xc
#define HWIO_GCC_SDCC2_AHB_CBCR_WAKEUP_BMSK                                                         0xf00
#define HWIO_GCC_SDCC2_AHB_CBCR_WAKEUP_SHFT                                                           0x8
#define HWIO_GCC_SDCC2_AHB_CBCR_SLEEP_BMSK                                                           0xf0
#define HWIO_GCC_SDCC2_AHB_CBCR_SLEEP_SHFT                                                            0x4
#define HWIO_GCC_SDCC2_AHB_CBCR_CLK_ENABLE_BMSK                                                       0x1
#define HWIO_GCC_SDCC2_AHB_CBCR_CLK_ENABLE_SHFT                                                       0x0

#define HWIO_GCC_SDCC1_ICE_CORE_CMD_RCGR_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0005d000)
#define HWIO_GCC_SDCC1_ICE_CORE_CMD_RCGR_RMSK                                                  0x800000f3
#define HWIO_GCC_SDCC1_ICE_CORE_CMD_RCGR_IN          \
        in_dword_masked(HWIO_GCC_SDCC1_ICE_CORE_CMD_RCGR_ADDR, HWIO_GCC_SDCC1_ICE_CORE_CMD_RCGR_RMSK)
#define HWIO_GCC_SDCC1_ICE_CORE_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC1_ICE_CORE_CMD_RCGR_ADDR, m)
#define HWIO_GCC_SDCC1_ICE_CORE_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_SDCC1_ICE_CORE_CMD_RCGR_ADDR,v)
#define HWIO_GCC_SDCC1_ICE_CORE_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC1_ICE_CORE_CMD_RCGR_ADDR,m,v,HWIO_GCC_SDCC1_ICE_CORE_CMD_RCGR_IN)
#define HWIO_GCC_SDCC1_ICE_CORE_CMD_RCGR_ROOT_OFF_BMSK                                         0x80000000
#define HWIO_GCC_SDCC1_ICE_CORE_CMD_RCGR_ROOT_OFF_SHFT                                               0x1f
#define HWIO_GCC_SDCC1_ICE_CORE_CMD_RCGR_DIRTY_D_BMSK                                                0x80
#define HWIO_GCC_SDCC1_ICE_CORE_CMD_RCGR_DIRTY_D_SHFT                                                 0x7
#define HWIO_GCC_SDCC1_ICE_CORE_CMD_RCGR_DIRTY_M_BMSK                                                0x40
#define HWIO_GCC_SDCC1_ICE_CORE_CMD_RCGR_DIRTY_M_SHFT                                                 0x6
#define HWIO_GCC_SDCC1_ICE_CORE_CMD_RCGR_DIRTY_N_BMSK                                                0x20
#define HWIO_GCC_SDCC1_ICE_CORE_CMD_RCGR_DIRTY_N_SHFT                                                 0x5
#define HWIO_GCC_SDCC1_ICE_CORE_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                                         0x10
#define HWIO_GCC_SDCC1_ICE_CORE_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                                          0x4
#define HWIO_GCC_SDCC1_ICE_CORE_CMD_RCGR_ROOT_EN_BMSK                                                 0x2
#define HWIO_GCC_SDCC1_ICE_CORE_CMD_RCGR_ROOT_EN_SHFT                                                 0x1
#define HWIO_GCC_SDCC1_ICE_CORE_CMD_RCGR_UPDATE_BMSK                                                  0x1
#define HWIO_GCC_SDCC1_ICE_CORE_CMD_RCGR_UPDATE_SHFT                                                  0x0

#define HWIO_GCC_SDCC1_ICE_CORE_CFG_RCGR_ADDR                                                  (GCC_CLK_CTL_REG_REG_BASE      + 0x0005d004)
#define HWIO_GCC_SDCC1_ICE_CORE_CFG_RCGR_RMSK                                                      0x371f
#define HWIO_GCC_SDCC1_ICE_CORE_CFG_RCGR_IN          \
        in_dword_masked(HWIO_GCC_SDCC1_ICE_CORE_CFG_RCGR_ADDR, HWIO_GCC_SDCC1_ICE_CORE_CFG_RCGR_RMSK)
#define HWIO_GCC_SDCC1_ICE_CORE_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC1_ICE_CORE_CFG_RCGR_ADDR, m)
#define HWIO_GCC_SDCC1_ICE_CORE_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_GCC_SDCC1_ICE_CORE_CFG_RCGR_ADDR,v)
#define HWIO_GCC_SDCC1_ICE_CORE_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC1_ICE_CORE_CFG_RCGR_ADDR,m,v,HWIO_GCC_SDCC1_ICE_CORE_CFG_RCGR_IN)
#define HWIO_GCC_SDCC1_ICE_CORE_CFG_RCGR_MODE_BMSK                                                 0x3000
#define HWIO_GCC_SDCC1_ICE_CORE_CFG_RCGR_MODE_SHFT                                                    0xc
#define HWIO_GCC_SDCC1_ICE_CORE_CFG_RCGR_SRC_SEL_BMSK                                               0x700
#define HWIO_GCC_SDCC1_ICE_CORE_CFG_RCGR_SRC_SEL_SHFT                                                 0x8
#define HWIO_GCC_SDCC1_ICE_CORE_CFG_RCGR_SRC_DIV_BMSK                                                0x1f
#define HWIO_GCC_SDCC1_ICE_CORE_CFG_RCGR_SRC_DIV_SHFT                                                 0x0

#define HWIO_GCC_SDCC1_ICE_CORE_M_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x0005d008)
#define HWIO_GCC_SDCC1_ICE_CORE_M_RMSK                                                               0xff
#define HWIO_GCC_SDCC1_ICE_CORE_M_IN          \
        in_dword_masked(HWIO_GCC_SDCC1_ICE_CORE_M_ADDR, HWIO_GCC_SDCC1_ICE_CORE_M_RMSK)
#define HWIO_GCC_SDCC1_ICE_CORE_M_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC1_ICE_CORE_M_ADDR, m)
#define HWIO_GCC_SDCC1_ICE_CORE_M_OUT(v)      \
        out_dword(HWIO_GCC_SDCC1_ICE_CORE_M_ADDR,v)
#define HWIO_GCC_SDCC1_ICE_CORE_M_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC1_ICE_CORE_M_ADDR,m,v,HWIO_GCC_SDCC1_ICE_CORE_M_IN)
#define HWIO_GCC_SDCC1_ICE_CORE_M_M_BMSK                                                             0xff
#define HWIO_GCC_SDCC1_ICE_CORE_M_M_SHFT                                                              0x0

#define HWIO_GCC_SDCC1_ICE_CORE_N_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x0005d00c)
#define HWIO_GCC_SDCC1_ICE_CORE_N_RMSK                                                               0xff
#define HWIO_GCC_SDCC1_ICE_CORE_N_IN          \
        in_dword_masked(HWIO_GCC_SDCC1_ICE_CORE_N_ADDR, HWIO_GCC_SDCC1_ICE_CORE_N_RMSK)
#define HWIO_GCC_SDCC1_ICE_CORE_N_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC1_ICE_CORE_N_ADDR, m)
#define HWIO_GCC_SDCC1_ICE_CORE_N_OUT(v)      \
        out_dword(HWIO_GCC_SDCC1_ICE_CORE_N_ADDR,v)
#define HWIO_GCC_SDCC1_ICE_CORE_N_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC1_ICE_CORE_N_ADDR,m,v,HWIO_GCC_SDCC1_ICE_CORE_N_IN)
#define HWIO_GCC_SDCC1_ICE_CORE_N_NOT_N_MINUS_M_BMSK                                                 0xff
#define HWIO_GCC_SDCC1_ICE_CORE_N_NOT_N_MINUS_M_SHFT                                                  0x0

#define HWIO_GCC_SDCC1_ICE_CORE_D_ADDR                                                         (GCC_CLK_CTL_REG_REG_BASE      + 0x0005d010)
#define HWIO_GCC_SDCC1_ICE_CORE_D_RMSK                                                               0xff
#define HWIO_GCC_SDCC1_ICE_CORE_D_IN          \
        in_dword_masked(HWIO_GCC_SDCC1_ICE_CORE_D_ADDR, HWIO_GCC_SDCC1_ICE_CORE_D_RMSK)
#define HWIO_GCC_SDCC1_ICE_CORE_D_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC1_ICE_CORE_D_ADDR, m)
#define HWIO_GCC_SDCC1_ICE_CORE_D_OUT(v)      \
        out_dword(HWIO_GCC_SDCC1_ICE_CORE_D_ADDR,v)
#define HWIO_GCC_SDCC1_ICE_CORE_D_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC1_ICE_CORE_D_ADDR,m,v,HWIO_GCC_SDCC1_ICE_CORE_D_IN)
#define HWIO_GCC_SDCC1_ICE_CORE_D_NOT_2D_BMSK                                                        0xff
#define HWIO_GCC_SDCC1_ICE_CORE_D_NOT_2D_SHFT                                                         0x0

#define HWIO_GCC_SDCC1_ICE_CORE_CBCR_ADDR                                                      (GCC_CLK_CTL_REG_REG_BASE      + 0x0005d014)
#define HWIO_GCC_SDCC1_ICE_CORE_CBCR_RMSK                                                      0x80007ff1
#define HWIO_GCC_SDCC1_ICE_CORE_CBCR_IN          \
        in_dword_masked(HWIO_GCC_SDCC1_ICE_CORE_CBCR_ADDR, HWIO_GCC_SDCC1_ICE_CORE_CBCR_RMSK)
#define HWIO_GCC_SDCC1_ICE_CORE_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_SDCC1_ICE_CORE_CBCR_ADDR, m)
#define HWIO_GCC_SDCC1_ICE_CORE_CBCR_OUT(v)      \
        out_dword(HWIO_GCC_SDCC1_ICE_CORE_CBCR_ADDR,v)
#define HWIO_GCC_SDCC1_ICE_CORE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_SDCC1_ICE_CORE_CBCR_ADDR,m,v,HWIO_GCC_SDCC1_ICE_CORE_CBCR_IN)
#define HWIO_GCC_SDCC1_ICE_CORE_CBCR_CLK_OFF_BMSK                                              0x80000000
#define HWIO_GCC_SDCC1_ICE_CORE_CBCR_CLK_OFF_SHFT                                                    0x1f
#define HWIO_GCC_SDCC1_ICE_CORE_CBCR_FORCE_MEM_CORE_ON_BMSK                                        0x4000
#define HWIO_GCC_SDCC1_ICE_CORE_CBCR_FORCE_MEM_CORE_ON_SHFT                                           0xe
#define HWIO_GCC_SDCC1_ICE_CORE_CBCR_FORCE_MEM_PERIPH_ON_BMSK                                      0x2000
#define HWIO_GCC_SDCC1_ICE_CORE_CBCR_FORCE_MEM_PERIPH_ON_SHFT                                         0xd
#define HWIO_GCC_SDCC1_ICE_CORE_CBCR_FORCE_MEM_PERIPH_OFF_BMSK                                     0x1000
#define HWIO_GCC_SDCC1_ICE_CORE_CBCR_FORCE_MEM_PERIPH_OFF_SHFT                                        0xc
#define HWIO_GCC_SDCC1_ICE_CORE_CBCR_WAKEUP_BMSK                                                    0xf00
#define HWIO_GCC_SDCC1_ICE_CORE_CBCR_WAKEUP_SHFT                                                      0x8
#define HWIO_GCC_SDCC1_ICE_CORE_CBCR_SLEEP_BMSK                                                      0xf0
#define HWIO_GCC_SDCC1_ICE_CORE_CBCR_SLEEP_SHFT                                                       0x4
#define HWIO_GCC_SDCC1_ICE_CORE_CBCR_CLK_ENABLE_BMSK                                                  0x1
#define HWIO_GCC_SDCC1_ICE_CORE_CBCR_CLK_ENABLE_SHFT                                                  0x0

#endif /* __HWIO_GCC_SDCC_H__ */
