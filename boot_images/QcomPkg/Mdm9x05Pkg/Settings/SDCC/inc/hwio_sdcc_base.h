#ifndef __HWIO_SDCC_BASE_H__
#define __HWIO_SDCC_BASE_H__
/*
===========================================================================
*/
/**
  @file hwio_sdcc_base.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    QCS405 (Vipertooth) [vipertooth_prj_1.0_p3q3r103]
 
  This file contains HWIO register definitions for the following modules:
    SDC1_SDCC_SDCC5_HC
    SDC2_SDCC_SDCC5_HC


  Generation parameters: 
  { u'filename': u'hwio_sdcc_base.h',
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [u'SDC1_SDCC_SDCC5_HC', u'SDC2_SDCC_SDCC5_HC'],
    u'output-attrs': True,
    u'output-offsets': True,
    u'output-resets': True}

  Attribute definitions for the HWIO_*_ATTR macros are as follows:
    0x0: Command register
    0x1: Read-Only
    0x2: Write-Only
    0x3: Read/Write
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

  $Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Settings/SDCC/inc/hwio_sdcc_base.h#1 $
  $DateTime: 2018/08/08 05:31:55 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: SDC1_SDCC_SDCC5_HC
 *--------------------------------------------------------------------------*/

#define SDC1_SDCC_SDCC5_HC_REG_BASE                                                                          (SDC1_SDCC5_TOP_BASE      + 0x00004000)
#define SDC1_SDCC_SDCC5_HC_REG_BASE_SIZE                                                                     0x1000
#define SDC1_SDCC_SDCC5_HC_REG_BASE_USED                                                                     0x800
#define SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS                                                                     0x00004000

#define HWIO_SDC1_SDCC_HC_REG_0_2_ADDR                                                                       (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000000)
#define HWIO_SDC1_SDCC_HC_REG_0_2_OFFS                                                                       (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000000)
#define HWIO_SDC1_SDCC_HC_REG_0_2_RMSK                                                                       0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_0_2_POR                                                                        0x00000000
#define HWIO_SDC1_SDCC_HC_REG_0_2_POR_RMSK                                                                   0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_0_2_ATTR                                                                              0x3
#define HWIO_SDC1_SDCC_HC_REG_0_2_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_0_2_ADDR, HWIO_SDC1_SDCC_HC_REG_0_2_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_0_2_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_0_2_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_0_2_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_REG_0_2_ADDR,v)
#define HWIO_SDC1_SDCC_HC_REG_0_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_REG_0_2_ADDR,m,v,HWIO_SDC1_SDCC_HC_REG_0_2_IN)
#define HWIO_SDC1_SDCC_HC_REG_0_2_ARG_2_BMSK                                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_0_2_ARG_2_SHFT                                                                        0x0

#define HWIO_SDC1_SDCC_HC_REG_4_6_ADDR                                                                       (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000004)
#define HWIO_SDC1_SDCC_HC_REG_4_6_OFFS                                                                       (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000004)
#define HWIO_SDC1_SDCC_HC_REG_4_6_RMSK                                                                       0xffff7fff
#define HWIO_SDC1_SDCC_HC_REG_4_6_POR                                                                        0x00000000
#define HWIO_SDC1_SDCC_HC_REG_4_6_POR_RMSK                                                                   0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_4_6_ATTR                                                                              0x3
#define HWIO_SDC1_SDCC_HC_REG_4_6_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_4_6_ADDR, HWIO_SDC1_SDCC_HC_REG_4_6_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_4_6_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_4_6_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_4_6_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_REG_4_6_ADDR,v)
#define HWIO_SDC1_SDCC_HC_REG_4_6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_REG_4_6_ADDR,m,v,HWIO_SDC1_SDCC_HC_REG_4_6_IN)
#define HWIO_SDC1_SDCC_HC_REG_4_6_BLK_CNT_FOR_CUR_TRANS_BMSK                                                 0xffff0000
#define HWIO_SDC1_SDCC_HC_REG_4_6_BLK_CNT_FOR_CUR_TRANS_SHFT                                                       0x10
#define HWIO_SDC1_SDCC_HC_REG_4_6_BLK_SIZE_HST_SDMA_BUF_BMSK                                                     0x7000
#define HWIO_SDC1_SDCC_HC_REG_4_6_BLK_SIZE_HST_SDMA_BUF_SHFT                                                        0xc
#define HWIO_SDC1_SDCC_HC_REG_4_6_BLK_SIZE_TRANS_BMSK                                                             0xfff
#define HWIO_SDC1_SDCC_HC_REG_4_6_BLK_SIZE_TRANS_SHFT                                                               0x0

#define HWIO_SDC1_SDCC_HC_REG_8_A_ADDR                                                                       (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000008)
#define HWIO_SDC1_SDCC_HC_REG_8_A_OFFS                                                                       (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000008)
#define HWIO_SDC1_SDCC_HC_REG_8_A_RMSK                                                                       0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_8_A_POR                                                                        0x00000000
#define HWIO_SDC1_SDCC_HC_REG_8_A_POR_RMSK                                                                   0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_8_A_ATTR                                                                              0x3
#define HWIO_SDC1_SDCC_HC_REG_8_A_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_8_A_ADDR, HWIO_SDC1_SDCC_HC_REG_8_A_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_8_A_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_8_A_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_8_A_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_REG_8_A_ADDR,v)
#define HWIO_SDC1_SDCC_HC_REG_8_A_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_REG_8_A_ADDR,m,v,HWIO_SDC1_SDCC_HC_REG_8_A_IN)
#define HWIO_SDC1_SDCC_HC_REG_8_A_CMD_ARG_1_BMSK                                                             0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_8_A_CMD_ARG_1_SHFT                                                                    0x0

#define HWIO_SDC1_SDCC_HC_REG_C_E_ADDR                                                                       (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x0000000c)
#define HWIO_SDC1_SDCC_HC_REG_C_E_OFFS                                                                       (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000000c)
#define HWIO_SDC1_SDCC_HC_REG_C_E_RMSK                                                                       0x3ffb003f
#define HWIO_SDC1_SDCC_HC_REG_C_E_POR                                                                        0x00000000
#define HWIO_SDC1_SDCC_HC_REG_C_E_POR_RMSK                                                                   0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_C_E_ATTR                                                                              0x3
#define HWIO_SDC1_SDCC_HC_REG_C_E_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_C_E_ADDR, HWIO_SDC1_SDCC_HC_REG_C_E_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_C_E_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_C_E_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_C_E_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_REG_C_E_ADDR,v)
#define HWIO_SDC1_SDCC_HC_REG_C_E_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_REG_C_E_ADDR,m,v,HWIO_SDC1_SDCC_HC_REG_C_E_IN)
#define HWIO_SDC1_SDCC_HC_REG_C_E_CMD_INDX_BMSK                                                              0x3f000000
#define HWIO_SDC1_SDCC_HC_REG_C_E_CMD_INDX_SHFT                                                                    0x18
#define HWIO_SDC1_SDCC_HC_REG_C_E_CMD_TYPE_BMSK                                                                0xc00000
#define HWIO_SDC1_SDCC_HC_REG_C_E_CMD_TYPE_SHFT                                                                    0x16
#define HWIO_SDC1_SDCC_HC_REG_C_E_CMD_DATA_PRESENT_SEL_BMSK                                                    0x200000
#define HWIO_SDC1_SDCC_HC_REG_C_E_CMD_DATA_PRESENT_SEL_SHFT                                                        0x15
#define HWIO_SDC1_SDCC_HC_REG_C_E_CMD_INDX_CHECK_EN_BMSK                                                       0x100000
#define HWIO_SDC1_SDCC_HC_REG_C_E_CMD_INDX_CHECK_EN_SHFT                                                           0x14
#define HWIO_SDC1_SDCC_HC_REG_C_E_CMD_CRC_CHECK_EN_BMSK                                                         0x80000
#define HWIO_SDC1_SDCC_HC_REG_C_E_CMD_CRC_CHECK_EN_SHFT                                                            0x13
#define HWIO_SDC1_SDCC_HC_REG_C_E_CMD_RESP_TYPE_SEL_BMSK                                                        0x30000
#define HWIO_SDC1_SDCC_HC_REG_C_E_CMD_RESP_TYPE_SEL_SHFT                                                           0x10
#define HWIO_SDC1_SDCC_HC_REG_C_E_TRANS_MODE_MULTI_SINGLE_BLK_SEL_BMSK                                             0x20
#define HWIO_SDC1_SDCC_HC_REG_C_E_TRANS_MODE_MULTI_SINGLE_BLK_SEL_SHFT                                              0x5
#define HWIO_SDC1_SDCC_HC_REG_C_E_TRANS_MODE_DATA_DIRECTION_SEL_BMSK                                               0x10
#define HWIO_SDC1_SDCC_HC_REG_C_E_TRANS_MODE_DATA_DIRECTION_SEL_SHFT                                                0x4
#define HWIO_SDC1_SDCC_HC_REG_C_E_TRANS_MODE_AUTO_CMD_EN_BMSK                                                       0xc
#define HWIO_SDC1_SDCC_HC_REG_C_E_TRANS_MODE_AUTO_CMD_EN_SHFT                                                       0x2
#define HWIO_SDC1_SDCC_HC_REG_C_E_TRANS_MODE_BLK_CNT_EN_BMSK                                                        0x2
#define HWIO_SDC1_SDCC_HC_REG_C_E_TRANS_MODE_BLK_CNT_EN_SHFT                                                        0x1
#define HWIO_SDC1_SDCC_HC_REG_C_E_TRANS_MODE_DMA_EN_BMSK                                                            0x1
#define HWIO_SDC1_SDCC_HC_REG_C_E_TRANS_MODE_DMA_EN_SHFT                                                            0x0

#define HWIO_SDC1_SDCC_HC_REG_10_12_ADDR                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000010)
#define HWIO_SDC1_SDCC_HC_REG_10_12_OFFS                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000010)
#define HWIO_SDC1_SDCC_HC_REG_10_12_RMSK                                                                     0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_10_12_POR                                                                      0x00000000
#define HWIO_SDC1_SDCC_HC_REG_10_12_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_10_12_ATTR                                                                            0x1
#define HWIO_SDC1_SDCC_HC_REG_10_12_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_10_12_ADDR, HWIO_SDC1_SDCC_HC_REG_10_12_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_10_12_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_10_12_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_10_12_CMD_RESP_BMSK                                                            0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_10_12_CMD_RESP_SHFT                                                                   0x0

#define HWIO_SDC1_SDCC_HC_REG_14_16_ADDR                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000014)
#define HWIO_SDC1_SDCC_HC_REG_14_16_OFFS                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000014)
#define HWIO_SDC1_SDCC_HC_REG_14_16_RMSK                                                                     0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_14_16_POR                                                                      0x00000000
#define HWIO_SDC1_SDCC_HC_REG_14_16_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_14_16_ATTR                                                                            0x1
#define HWIO_SDC1_SDCC_HC_REG_14_16_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_14_16_ADDR, HWIO_SDC1_SDCC_HC_REG_14_16_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_14_16_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_14_16_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_14_16_CMD_RESP_BMSK                                                            0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_14_16_CMD_RESP_SHFT                                                                   0x0

#define HWIO_SDC1_SDCC_HC_REG_18_1A_ADDR                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000018)
#define HWIO_SDC1_SDCC_HC_REG_18_1A_OFFS                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000018)
#define HWIO_SDC1_SDCC_HC_REG_18_1A_RMSK                                                                     0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_18_1A_POR                                                                      0x00000000
#define HWIO_SDC1_SDCC_HC_REG_18_1A_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_18_1A_ATTR                                                                            0x1
#define HWIO_SDC1_SDCC_HC_REG_18_1A_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_18_1A_ADDR, HWIO_SDC1_SDCC_HC_REG_18_1A_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_18_1A_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_18_1A_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_18_1A_CMD_RESP_BMSK                                                            0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_18_1A_CMD_RESP_SHFT                                                                   0x0

#define HWIO_SDC1_SDCC_HC_REG_1C_1E_ADDR                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x0000001c)
#define HWIO_SDC1_SDCC_HC_REG_1C_1E_OFFS                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000001c)
#define HWIO_SDC1_SDCC_HC_REG_1C_1E_RMSK                                                                     0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_1C_1E_POR                                                                      0x00000000
#define HWIO_SDC1_SDCC_HC_REG_1C_1E_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_1C_1E_ATTR                                                                            0x1
#define HWIO_SDC1_SDCC_HC_REG_1C_1E_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_1C_1E_ADDR, HWIO_SDC1_SDCC_HC_REG_1C_1E_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_1C_1E_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_1C_1E_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_1C_1E_CMD_RESP_BMSK                                                            0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_1C_1E_CMD_RESP_SHFT                                                                   0x0

#define HWIO_SDC1_SDCC_HC_REG_20_22_ADDR                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000020)
#define HWIO_SDC1_SDCC_HC_REG_20_22_OFFS                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000020)
#define HWIO_SDC1_SDCC_HC_REG_20_22_RMSK                                                                     0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_20_22_POR                                                                      0x00000000
#define HWIO_SDC1_SDCC_HC_REG_20_22_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_20_22_ATTR                                                                            0x3
#define HWIO_SDC1_SDCC_HC_REG_20_22_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_20_22_ADDR, HWIO_SDC1_SDCC_HC_REG_20_22_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_20_22_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_20_22_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_20_22_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_REG_20_22_ADDR,v)
#define HWIO_SDC1_SDCC_HC_REG_20_22_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_REG_20_22_ADDR,m,v,HWIO_SDC1_SDCC_HC_REG_20_22_IN)
#define HWIO_SDC1_SDCC_HC_REG_20_22_BUF_DATA_PORT_3_BMSK                                                     0xff000000
#define HWIO_SDC1_SDCC_HC_REG_20_22_BUF_DATA_PORT_3_SHFT                                                           0x18
#define HWIO_SDC1_SDCC_HC_REG_20_22_BUF_DATA_PORT_2_BMSK                                                       0xff0000
#define HWIO_SDC1_SDCC_HC_REG_20_22_BUF_DATA_PORT_2_SHFT                                                           0x10
#define HWIO_SDC1_SDCC_HC_REG_20_22_BUF_DATA_PORT_1_BMSK                                                         0xff00
#define HWIO_SDC1_SDCC_HC_REG_20_22_BUF_DATA_PORT_1_SHFT                                                            0x8
#define HWIO_SDC1_SDCC_HC_REG_20_22_BUF_DATA_PORT_0_BMSK                                                           0xff
#define HWIO_SDC1_SDCC_HC_REG_20_22_BUF_DATA_PORT_0_SHFT                                                            0x0

#define HWIO_SDC1_SDCC_HC_REG_24_26_ADDR                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000024)
#define HWIO_SDC1_SDCC_HC_REG_24_26_OFFS                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000024)
#define HWIO_SDC1_SDCC_HC_REG_24_26_RMSK                                                                      0x3ff0fff
#define HWIO_SDC1_SDCC_HC_REG_24_26_POR                                                                      0x01f800f0
#define HWIO_SDC1_SDCC_HC_REG_24_26_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_24_26_ATTR                                                                            0x1
#define HWIO_SDC1_SDCC_HC_REG_24_26_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_24_26_ADDR, HWIO_SDC1_SDCC_HC_REG_24_26_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_24_26_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_24_26_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_24_26_SIGANLING_18_SWITCHING_STS_BMSK                                           0x2000000
#define HWIO_SDC1_SDCC_HC_REG_24_26_SIGANLING_18_SWITCHING_STS_SHFT                                                0x19
#define HWIO_SDC1_SDCC_HC_REG_24_26_PRESENT_STATE_CMD_LINE_SIGNAL_LEVEL_BMSK                                  0x1000000
#define HWIO_SDC1_SDCC_HC_REG_24_26_PRESENT_STATE_CMD_LINE_SIGNAL_LEVEL_SHFT                                       0x18
#define HWIO_SDC1_SDCC_HC_REG_24_26_PRESENT_STATE_DAT_3_0_LINE_SIGNAL_LEVEL_BMSK                               0xf00000
#define HWIO_SDC1_SDCC_HC_REG_24_26_PRESENT_STATE_DAT_3_0_LINE_SIGNAL_LEVEL_SHFT                                   0x14
#define HWIO_SDC1_SDCC_HC_REG_24_26_PRESENT_STATE_WR_PROTECT_SWITCH_PIN_LEVEL_BMSK                              0x80000
#define HWIO_SDC1_SDCC_HC_REG_24_26_PRESENT_STATE_WR_PROTECT_SWITCH_PIN_LEVEL_SHFT                                 0x13
#define HWIO_SDC1_SDCC_HC_REG_24_26_PRESENT_STATE_CARD_DETECT_PIN_LEVEL_BMSK                                    0x40000
#define HWIO_SDC1_SDCC_HC_REG_24_26_PRESENT_STATE_CARD_DETECT_PIN_LEVEL_SHFT                                       0x12
#define HWIO_SDC1_SDCC_HC_REG_24_26_PRESENT_STATE_CARD_STATE_STABLE_BMSK                                        0x20000
#define HWIO_SDC1_SDCC_HC_REG_24_26_PRESENT_STATE_CARD_STATE_STABLE_SHFT                                           0x11
#define HWIO_SDC1_SDCC_HC_REG_24_26_PRESENT_STATE_CARD_INSERTED_BMSK                                            0x10000
#define HWIO_SDC1_SDCC_HC_REG_24_26_PRESENT_STATE_CARD_INSERTED_SHFT                                               0x10
#define HWIO_SDC1_SDCC_HC_REG_24_26_PRESENT_STATE_BUF_RD_EN_BMSK                                                  0x800
#define HWIO_SDC1_SDCC_HC_REG_24_26_PRESENT_STATE_BUF_RD_EN_SHFT                                                    0xb
#define HWIO_SDC1_SDCC_HC_REG_24_26_PRESENT_STATE_BUF_WR_EN_BMSK                                                  0x400
#define HWIO_SDC1_SDCC_HC_REG_24_26_PRESENT_STATE_BUF_WR_EN_SHFT                                                    0xa
#define HWIO_SDC1_SDCC_HC_REG_24_26_PRESENT_STATE_RD_TRANS_ACT_BMSK                                               0x200
#define HWIO_SDC1_SDCC_HC_REG_24_26_PRESENT_STATE_RD_TRANS_ACT_SHFT                                                 0x9
#define HWIO_SDC1_SDCC_HC_REG_24_26_PRESENT_STATE_WR_TRANS_ACT_BMSK                                               0x100
#define HWIO_SDC1_SDCC_HC_REG_24_26_PRESENT_STATE_WR_TRANS_ACT_SHFT                                                 0x8
#define HWIO_SDC1_SDCC_HC_REG_24_26_PRESENT_STATE_DAT_7_4_LINE_SIGNAL_LEVEL_BMSK                                   0xf0
#define HWIO_SDC1_SDCC_HC_REG_24_26_PRESENT_STATE_DAT_7_4_LINE_SIGNAL_LEVEL_SHFT                                    0x4
#define HWIO_SDC1_SDCC_HC_REG_24_26_PRESENT_STATE_RETUNING_REQ_BMSK                                                 0x8
#define HWIO_SDC1_SDCC_HC_REG_24_26_PRESENT_STATE_RETUNING_REQ_SHFT                                                 0x3
#define HWIO_SDC1_SDCC_HC_REG_24_26_PRESENT_STATE_DAT_LINE_ACT_BMSK                                                 0x4
#define HWIO_SDC1_SDCC_HC_REG_24_26_PRESENT_STATE_DAT_LINE_ACT_SHFT                                                 0x2
#define HWIO_SDC1_SDCC_HC_REG_24_26_PRESENT_STATE_CMD_INHIBIT_DAT_BMSK                                              0x2
#define HWIO_SDC1_SDCC_HC_REG_24_26_PRESENT_STATE_CMD_INHIBIT_DAT_SHFT                                              0x1
#define HWIO_SDC1_SDCC_HC_REG_24_26_PRESENT_STATE_CMD_INHIBIT_CMD_BMSK                                              0x1
#define HWIO_SDC1_SDCC_HC_REG_24_26_PRESENT_STATE_CMD_INHIBIT_CMD_SHFT                                              0x0

#define HWIO_SDC1_SDCC_HC_REG_28_2A_ADDR                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000028)
#define HWIO_SDC1_SDCC_HC_REG_28_2A_OFFS                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000028)
#define HWIO_SDC1_SDCC_HC_REG_28_2A_RMSK                                                                      0x70f0fff
#define HWIO_SDC1_SDCC_HC_REG_28_2A_POR                                                                      0x00000000
#define HWIO_SDC1_SDCC_HC_REG_28_2A_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_28_2A_ATTR                                                                            0x3
#define HWIO_SDC1_SDCC_HC_REG_28_2A_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_28_2A_ADDR, HWIO_SDC1_SDCC_HC_REG_28_2A_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_28_2A_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_28_2A_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_28_2A_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_REG_28_2A_ADDR,v)
#define HWIO_SDC1_SDCC_HC_REG_28_2A_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_REG_28_2A_ADDR,m,v,HWIO_SDC1_SDCC_HC_REG_28_2A_IN)
#define HWIO_SDC1_SDCC_HC_REG_28_2A_WKUP_EVENT_EN_ON_SD_CARD_REMOVAL_BMSK                                     0x4000000
#define HWIO_SDC1_SDCC_HC_REG_28_2A_WKUP_EVENT_EN_ON_SD_CARD_REMOVAL_SHFT                                          0x1a
#define HWIO_SDC1_SDCC_HC_REG_28_2A_WKUP_EVENT_EN_ON_SD_CARD_INSERTION_BMSK                                   0x2000000
#define HWIO_SDC1_SDCC_HC_REG_28_2A_WKUP_EVENT_EN_ON_SD_CARD_INSERTION_SHFT                                        0x19
#define HWIO_SDC1_SDCC_HC_REG_28_2A_WKUP_EVENT_EN_ON_SD_CARD_INT_BMSK                                         0x1000000
#define HWIO_SDC1_SDCC_HC_REG_28_2A_WKUP_EVENT_EN_ON_SD_CARD_INT_SHFT                                              0x18
#define HWIO_SDC1_SDCC_HC_REG_28_2A_BLK_GAP_CTL_INT_BMSK                                                        0x80000
#define HWIO_SDC1_SDCC_HC_REG_28_2A_BLK_GAP_CTL_INT_SHFT                                                           0x13
#define HWIO_SDC1_SDCC_HC_REG_28_2A_BLK_GAP_CTL_RD_WAIT_BMSK                                                    0x40000
#define HWIO_SDC1_SDCC_HC_REG_28_2A_BLK_GAP_CTL_RD_WAIT_SHFT                                                       0x12
#define HWIO_SDC1_SDCC_HC_REG_28_2A_BLK_GAP_CTL_CONTINUE_REQ_BMSK                                               0x20000
#define HWIO_SDC1_SDCC_HC_REG_28_2A_BLK_GAP_CTL_CONTINUE_REQ_SHFT                                                  0x11
#define HWIO_SDC1_SDCC_HC_REG_28_2A_BLK_GAP_CTL_STOP_GAP_REQ_BMSK                                               0x10000
#define HWIO_SDC1_SDCC_HC_REG_28_2A_BLK_GAP_CTL_STOP_GAP_REQ_SHFT                                                  0x10
#define HWIO_SDC1_SDCC_HC_REG_28_2A_PWR_CTL_SD_BUS_VOLTAGE_SEL_BMSK                                               0xe00
#define HWIO_SDC1_SDCC_HC_REG_28_2A_PWR_CTL_SD_BUS_VOLTAGE_SEL_SHFT                                                 0x9
#define HWIO_SDC1_SDCC_HC_REG_28_2A_PWR_CTL_SD_BUS_PWR_BMSK                                                       0x100
#define HWIO_SDC1_SDCC_HC_REG_28_2A_PWR_CTL_SD_BUS_PWR_SHFT                                                         0x8
#define HWIO_SDC1_SDCC_HC_REG_28_2A_HST_CTL1_CARD_DETECT_SIGNAL_SEL_BMSK                                           0x80
#define HWIO_SDC1_SDCC_HC_REG_28_2A_HST_CTL1_CARD_DETECT_SIGNAL_SEL_SHFT                                            0x7
#define HWIO_SDC1_SDCC_HC_REG_28_2A_HST_CTL1_CARD_DETECT_TEST_LEVEL_BMSK                                           0x40
#define HWIO_SDC1_SDCC_HC_REG_28_2A_HST_CTL1_CARD_DETECT_TEST_LEVEL_SHFT                                            0x6
#define HWIO_SDC1_SDCC_HC_REG_28_2A_HST_CTL1_EXTENDED_DATA_TRANS_WIDTH_BMSK                                        0x20
#define HWIO_SDC1_SDCC_HC_REG_28_2A_HST_CTL1_EXTENDED_DATA_TRANS_WIDTH_SHFT                                         0x5
#define HWIO_SDC1_SDCC_HC_REG_28_2A_HST_CTL1_DMA_SEL_BMSK                                                          0x18
#define HWIO_SDC1_SDCC_HC_REG_28_2A_HST_CTL1_DMA_SEL_SHFT                                                           0x3
#define HWIO_SDC1_SDCC_HC_REG_28_2A_HST_CTL1_HS_EN_BMSK                                                             0x4
#define HWIO_SDC1_SDCC_HC_REG_28_2A_HST_CTL1_HS_EN_SHFT                                                             0x2
#define HWIO_SDC1_SDCC_HC_REG_28_2A_HST_CTL1_DATA_TRANS_WIDTH_BMSK                                                  0x2
#define HWIO_SDC1_SDCC_HC_REG_28_2A_HST_CTL1_DATA_TRANS_WIDTH_SHFT                                                  0x1
#define HWIO_SDC1_SDCC_HC_REG_28_2A_HST_CTL1_LED_CTL_BMSK                                                           0x1
#define HWIO_SDC1_SDCC_HC_REG_28_2A_HST_CTL1_LED_CTL_SHFT                                                           0x0

#define HWIO_SDC1_SDCC_HC_REG_2C_2E_ADDR                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x0000002c)
#define HWIO_SDC1_SDCC_HC_REG_2C_2E_OFFS                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000002c)
#define HWIO_SDC1_SDCC_HC_REG_2C_2E_RMSK                                                                      0x70fffe7
#define HWIO_SDC1_SDCC_HC_REG_2C_2E_POR                                                                      0x00000001
#define HWIO_SDC1_SDCC_HC_REG_2C_2E_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_2C_2E_ATTR                                                                            0x3
#define HWIO_SDC1_SDCC_HC_REG_2C_2E_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_2C_2E_ADDR, HWIO_SDC1_SDCC_HC_REG_2C_2E_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_2C_2E_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_2C_2E_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_2C_2E_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_REG_2C_2E_ADDR,v)
#define HWIO_SDC1_SDCC_HC_REG_2C_2E_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_REG_2C_2E_ADDR,m,v,HWIO_SDC1_SDCC_HC_REG_2C_2E_IN)
#define HWIO_SDC1_SDCC_HC_REG_2C_2E_SW_RST_DAT_LINE_BMSK                                                      0x4000000
#define HWIO_SDC1_SDCC_HC_REG_2C_2E_SW_RST_DAT_LINE_SHFT                                                           0x1a
#define HWIO_SDC1_SDCC_HC_REG_2C_2E_SW_RST_CMD_LINE_BMSK                                                      0x2000000
#define HWIO_SDC1_SDCC_HC_REG_2C_2E_SW_RST_CMD_LINE_SHFT                                                           0x19
#define HWIO_SDC1_SDCC_HC_REG_2C_2E_SW_RST_FOR_ALL_BMSK                                                       0x1000000
#define HWIO_SDC1_SDCC_HC_REG_2C_2E_SW_RST_FOR_ALL_SHFT                                                            0x18
#define HWIO_SDC1_SDCC_HC_REG_2C_2E_DATA_TIMEOUT_COUNTER_BMSK                                                   0xf0000
#define HWIO_SDC1_SDCC_HC_REG_2C_2E_DATA_TIMEOUT_COUNTER_SHFT                                                      0x10
#define HWIO_SDC1_SDCC_HC_REG_2C_2E_CLK_CTL_SDCLK_FREQ_SEL_BMSK                                                  0xff00
#define HWIO_SDC1_SDCC_HC_REG_2C_2E_CLK_CTL_SDCLK_FREQ_SEL_SHFT                                                     0x8
#define HWIO_SDC1_SDCC_HC_REG_2C_2E_CLK_CTL_SDCLK_FREQ_SEL_MSB_BMSK                                                0xc0
#define HWIO_SDC1_SDCC_HC_REG_2C_2E_CLK_CTL_SDCLK_FREQ_SEL_MSB_SHFT                                                 0x6
#define HWIO_SDC1_SDCC_HC_REG_2C_2E_CLK_CTL_GEN_SEL_BMSK                                                           0x20
#define HWIO_SDC1_SDCC_HC_REG_2C_2E_CLK_CTL_GEN_SEL_SHFT                                                            0x5
#define HWIO_SDC1_SDCC_HC_REG_2C_2E_CLK_CTL_SDCLK_EN_BMSK                                                           0x4
#define HWIO_SDC1_SDCC_HC_REG_2C_2E_CLK_CTL_SDCLK_EN_SHFT                                                           0x2
#define HWIO_SDC1_SDCC_HC_REG_2C_2E_CLK_CTL_INTERNAL_CLK_STABLE_BMSK                                                0x2
#define HWIO_SDC1_SDCC_HC_REG_2C_2E_CLK_CTL_INTERNAL_CLK_STABLE_SHFT                                                0x1
#define HWIO_SDC1_SDCC_HC_REG_2C_2E_CLK_CTL_INTERNAL_CLK_EN_BMSK                                                    0x1
#define HWIO_SDC1_SDCC_HC_REG_2C_2E_CLK_CTL_INTERNAL_CLK_EN_SHFT                                                    0x0

#define HWIO_SDC1_SDCC_HC_REG_30_32_ADDR                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000030)
#define HWIO_SDC1_SDCC_HC_REG_30_32_OFFS                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000030)
#define HWIO_SDC1_SDCC_HC_REG_30_32_RMSK                                                                     0xc7ffdfff
#define HWIO_SDC1_SDCC_HC_REG_30_32_POR                                                                      0x00000000
#define HWIO_SDC1_SDCC_HC_REG_30_32_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_30_32_ATTR                                                                            0x3
#define HWIO_SDC1_SDCC_HC_REG_30_32_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_30_32_ADDR, HWIO_SDC1_SDCC_HC_REG_30_32_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_30_32_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_30_32_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_30_32_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_REG_30_32_ADDR,v)
#define HWIO_SDC1_SDCC_HC_REG_30_32_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_REG_30_32_ADDR,m,v,HWIO_SDC1_SDCC_HC_REG_30_32_IN)
#define HWIO_SDC1_SDCC_HC_REG_30_32_ERR_INT_STS_VENDOR_SPECIFIC_ERR_BMSK                                     0x80000000
#define HWIO_SDC1_SDCC_HC_REG_30_32_ERR_INT_STS_VENDOR_SPECIFIC_ERR_SHFT                                           0x1f
#define HWIO_SDC1_SDCC_HC_REG_30_32_ERR_INT_STS_VENDOR_SPECIFIC_STAT_BMSK                                    0x40000000
#define HWIO_SDC1_SDCC_HC_REG_30_32_ERR_INT_STS_VENDOR_SPECIFIC_STAT_SHFT                                          0x1e
#define HWIO_SDC1_SDCC_HC_REG_30_32_ERR_INT_STS_TUNING_ERR_BMSK                                               0x4000000
#define HWIO_SDC1_SDCC_HC_REG_30_32_ERR_INT_STS_TUNING_ERR_SHFT                                                    0x1a
#define HWIO_SDC1_SDCC_HC_REG_30_32_ERR_INT_STS_ADMA_ERR_BMSK                                                 0x2000000
#define HWIO_SDC1_SDCC_HC_REG_30_32_ERR_INT_STS_ADMA_ERR_SHFT                                                      0x19
#define HWIO_SDC1_SDCC_HC_REG_30_32_ERR_INT_STS_AUTO_CMD_ERR_BMSK                                             0x1000000
#define HWIO_SDC1_SDCC_HC_REG_30_32_ERR_INT_STS_AUTO_CMD_ERR_SHFT                                                  0x18
#define HWIO_SDC1_SDCC_HC_REG_30_32_ERR_INT_STS_CURRENT_LIMIT_ERR_BMSK                                         0x800000
#define HWIO_SDC1_SDCC_HC_REG_30_32_ERR_INT_STS_CURRENT_LIMIT_ERR_SHFT                                             0x17
#define HWIO_SDC1_SDCC_HC_REG_30_32_ERR_INT_STS_DATA_END_BIT_ERR_BMSK                                          0x400000
#define HWIO_SDC1_SDCC_HC_REG_30_32_ERR_INT_STS_DATA_END_BIT_ERR_SHFT                                              0x16
#define HWIO_SDC1_SDCC_HC_REG_30_32_ERR_INT_STS_DATA_CRC_ERR_BMSK                                              0x200000
#define HWIO_SDC1_SDCC_HC_REG_30_32_ERR_INT_STS_DATA_CRC_ERR_SHFT                                                  0x15
#define HWIO_SDC1_SDCC_HC_REG_30_32_ERR_INT_STS_DATA_TIMEOUT_ERR_BMSK                                          0x100000
#define HWIO_SDC1_SDCC_HC_REG_30_32_ERR_INT_STS_DATA_TIMEOUT_ERR_SHFT                                              0x14
#define HWIO_SDC1_SDCC_HC_REG_30_32_ERR_INT_STS_CMD_INDX_ERR_BMSK                                               0x80000
#define HWIO_SDC1_SDCC_HC_REG_30_32_ERR_INT_STS_CMD_INDX_ERR_SHFT                                                  0x13
#define HWIO_SDC1_SDCC_HC_REG_30_32_ERR_INT_STS_CMD_END_BIT_ERR_BMSK                                            0x40000
#define HWIO_SDC1_SDCC_HC_REG_30_32_ERR_INT_STS_CMD_END_BIT_ERR_SHFT                                               0x12
#define HWIO_SDC1_SDCC_HC_REG_30_32_ERR_INT_STS_CMD_CRC_ERR_BMSK                                                0x20000
#define HWIO_SDC1_SDCC_HC_REG_30_32_ERR_INT_STS_CMD_CRC_ERR_SHFT                                                   0x11
#define HWIO_SDC1_SDCC_HC_REG_30_32_ERR_INT_STS_CMD_TIMEOUT_ERR_BMSK                                            0x10000
#define HWIO_SDC1_SDCC_HC_REG_30_32_ERR_INT_STS_CMD_TIMEOUT_ERR_SHFT                                               0x10
#define HWIO_SDC1_SDCC_HC_REG_30_32_NORMAL_INT_STS_ERR_INT_BMSK                                                  0x8000
#define HWIO_SDC1_SDCC_HC_REG_30_32_NORMAL_INT_STS_ERR_INT_SHFT                                                     0xf
#define HWIO_SDC1_SDCC_HC_REG_30_32_NORMAL_INT_STS_CMD_QUEUE_BMSK                                                0x4000
#define HWIO_SDC1_SDCC_HC_REG_30_32_NORMAL_INT_STS_CMD_QUEUE_SHFT                                                   0xe
#define HWIO_SDC1_SDCC_HC_REG_30_32_NORMAL_INT_STS_RETUNING_EVENT_BMSK                                           0x1000
#define HWIO_SDC1_SDCC_HC_REG_30_32_NORMAL_INT_STS_RETUNING_EVENT_SHFT                                              0xc
#define HWIO_SDC1_SDCC_HC_REG_30_32_NORMAL_INT_STS_INT_C_BMSK                                                     0x800
#define HWIO_SDC1_SDCC_HC_REG_30_32_NORMAL_INT_STS_INT_C_SHFT                                                       0xb
#define HWIO_SDC1_SDCC_HC_REG_30_32_NORMAL_INT_STS_INT_B_BMSK                                                     0x400
#define HWIO_SDC1_SDCC_HC_REG_30_32_NORMAL_INT_STS_INT_B_SHFT                                                       0xa
#define HWIO_SDC1_SDCC_HC_REG_30_32_NORMAL_INT_STS_INT_A_BMSK                                                     0x200
#define HWIO_SDC1_SDCC_HC_REG_30_32_NORMAL_INT_STS_INT_A_SHFT                                                       0x9
#define HWIO_SDC1_SDCC_HC_REG_30_32_NORMAL_INT_STS_CARD_INT_BMSK                                                  0x100
#define HWIO_SDC1_SDCC_HC_REG_30_32_NORMAL_INT_STS_CARD_INT_SHFT                                                    0x8
#define HWIO_SDC1_SDCC_HC_REG_30_32_NORMAL_INT_STS_CARD_REMOVAL_BMSK                                               0x80
#define HWIO_SDC1_SDCC_HC_REG_30_32_NORMAL_INT_STS_CARD_REMOVAL_SHFT                                                0x7
#define HWIO_SDC1_SDCC_HC_REG_30_32_NORMAL_INT_STS_CARD_INSERTION_BMSK                                             0x40
#define HWIO_SDC1_SDCC_HC_REG_30_32_NORMAL_INT_STS_CARD_INSERTION_SHFT                                              0x6
#define HWIO_SDC1_SDCC_HC_REG_30_32_NORMAL_INT_STS_BUF_RD_READY_BMSK                                               0x20
#define HWIO_SDC1_SDCC_HC_REG_30_32_NORMAL_INT_STS_BUF_RD_READY_SHFT                                                0x5
#define HWIO_SDC1_SDCC_HC_REG_30_32_NORMAL_INT_STS_BUF_WR_READY_BMSK                                               0x10
#define HWIO_SDC1_SDCC_HC_REG_30_32_NORMAL_INT_STS_BUF_WR_READY_SHFT                                                0x4
#define HWIO_SDC1_SDCC_HC_REG_30_32_NORMAL_INT_STS_DMA_INT_BMSK                                                     0x8
#define HWIO_SDC1_SDCC_HC_REG_30_32_NORMAL_INT_STS_DMA_INT_SHFT                                                     0x3
#define HWIO_SDC1_SDCC_HC_REG_30_32_NORMAL_INT_STS_BLK_GAP_EVENT_BMSK                                               0x4
#define HWIO_SDC1_SDCC_HC_REG_30_32_NORMAL_INT_STS_BLK_GAP_EVENT_SHFT                                               0x2
#define HWIO_SDC1_SDCC_HC_REG_30_32_NORMAL_INT_STS_TRANS_COMPLETE_BMSK                                              0x2
#define HWIO_SDC1_SDCC_HC_REG_30_32_NORMAL_INT_STS_TRANS_COMPLETE_SHFT                                              0x1
#define HWIO_SDC1_SDCC_HC_REG_30_32_NORMAL_INT_STS_CMD_COMPLETE_BMSK                                                0x1
#define HWIO_SDC1_SDCC_HC_REG_30_32_NORMAL_INT_STS_CMD_COMPLETE_SHFT                                                0x0

#define HWIO_SDC1_SDCC_HC_REG_34_36_ADDR                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000034)
#define HWIO_SDC1_SDCC_HC_REG_34_36_OFFS                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000034)
#define HWIO_SDC1_SDCC_HC_REG_34_36_RMSK                                                                     0xc7ff5fff
#define HWIO_SDC1_SDCC_HC_REG_34_36_POR                                                                      0x00000000
#define HWIO_SDC1_SDCC_HC_REG_34_36_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_34_36_ATTR                                                                            0x3
#define HWIO_SDC1_SDCC_HC_REG_34_36_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_34_36_ADDR, HWIO_SDC1_SDCC_HC_REG_34_36_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_34_36_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_34_36_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_34_36_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_REG_34_36_ADDR,v)
#define HWIO_SDC1_SDCC_HC_REG_34_36_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_REG_34_36_ADDR,m,v,HWIO_SDC1_SDCC_HC_REG_34_36_IN)
#define HWIO_SDC1_SDCC_HC_REG_34_36_ERR_INT_STS_EN_VENDOR_SPECIFIC_ERR_BMSK                                  0x80000000
#define HWIO_SDC1_SDCC_HC_REG_34_36_ERR_INT_STS_EN_VENDOR_SPECIFIC_ERR_SHFT                                        0x1f
#define HWIO_SDC1_SDCC_HC_REG_34_36_ERR_INT_STS_EN_VENDOR_SPECIFIC_STAT_BMSK                                 0x40000000
#define HWIO_SDC1_SDCC_HC_REG_34_36_ERR_INT_STS_EN_VENDOR_SPECIFIC_STAT_SHFT                                       0x1e
#define HWIO_SDC1_SDCC_HC_REG_34_36_ERR_INT_STS_EN_TUNING_ERR_BMSK                                            0x4000000
#define HWIO_SDC1_SDCC_HC_REG_34_36_ERR_INT_STS_EN_TUNING_ERR_SHFT                                                 0x1a
#define HWIO_SDC1_SDCC_HC_REG_34_36_ERR_INT_STS_EN_ADMA_ERR_BMSK                                              0x2000000
#define HWIO_SDC1_SDCC_HC_REG_34_36_ERR_INT_STS_EN_ADMA_ERR_SHFT                                                   0x19
#define HWIO_SDC1_SDCC_HC_REG_34_36_ERR_INT_STS_EN_AUTO_CMD_ERR_BMSK                                          0x1000000
#define HWIO_SDC1_SDCC_HC_REG_34_36_ERR_INT_STS_EN_AUTO_CMD_ERR_SHFT                                               0x18
#define HWIO_SDC1_SDCC_HC_REG_34_36_ERR_INT_STS_EN_CURRENT_LIMIT_ERR_BMSK                                      0x800000
#define HWIO_SDC1_SDCC_HC_REG_34_36_ERR_INT_STS_EN_CURRENT_LIMIT_ERR_SHFT                                          0x17
#define HWIO_SDC1_SDCC_HC_REG_34_36_ERR_INT_STS_EN_DATA_END_BIT_ERR_BMSK                                       0x400000
#define HWIO_SDC1_SDCC_HC_REG_34_36_ERR_INT_STS_EN_DATA_END_BIT_ERR_SHFT                                           0x16
#define HWIO_SDC1_SDCC_HC_REG_34_36_ERR_INT_STS_EN_DATA_CRC_ERR_BMSK                                           0x200000
#define HWIO_SDC1_SDCC_HC_REG_34_36_ERR_INT_STS_EN_DATA_CRC_ERR_SHFT                                               0x15
#define HWIO_SDC1_SDCC_HC_REG_34_36_ERR_INT_STS_EN_DATA_TIMEOUT_BMSK                                           0x100000
#define HWIO_SDC1_SDCC_HC_REG_34_36_ERR_INT_STS_EN_DATA_TIMEOUT_SHFT                                               0x14
#define HWIO_SDC1_SDCC_HC_REG_34_36_ERR_INT_STS_EN_CMD_INDX_ERR_BMSK                                            0x80000
#define HWIO_SDC1_SDCC_HC_REG_34_36_ERR_INT_STS_EN_CMD_INDX_ERR_SHFT                                               0x13
#define HWIO_SDC1_SDCC_HC_REG_34_36_ERR_INT_STS_EN_CMD_END_BIT_ERR_BMSK                                         0x40000
#define HWIO_SDC1_SDCC_HC_REG_34_36_ERR_INT_STS_EN_CMD_END_BIT_ERR_SHFT                                            0x12
#define HWIO_SDC1_SDCC_HC_REG_34_36_ERR_INT_STS_EN_CMD_CRC_ERR_BMSK                                             0x20000
#define HWIO_SDC1_SDCC_HC_REG_34_36_ERR_INT_STS_EN_CMD_CRC_ERR_SHFT                                                0x11
#define HWIO_SDC1_SDCC_HC_REG_34_36_ERR_INT_STS_EN_CMD_TIMEOUT_BMSK                                             0x10000
#define HWIO_SDC1_SDCC_HC_REG_34_36_ERR_INT_STS_EN_CMD_TIMEOUT_SHFT                                                0x10
#define HWIO_SDC1_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_CMD_QUEUE_BMSK                                             0x4000
#define HWIO_SDC1_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_CMD_QUEUE_SHFT                                                0xe
#define HWIO_SDC1_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_RETUNING_EVENT_BMSK                                        0x1000
#define HWIO_SDC1_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_RETUNING_EVENT_SHFT                                           0xc
#define HWIO_SDC1_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_INT_C_BMSK                                                  0x800
#define HWIO_SDC1_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_INT_C_SHFT                                                    0xb
#define HWIO_SDC1_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_INT_B_BMSK                                                  0x400
#define HWIO_SDC1_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_INT_B_SHFT                                                    0xa
#define HWIO_SDC1_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_INT_A_BMSK                                                  0x200
#define HWIO_SDC1_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_INT_A_SHFT                                                    0x9
#define HWIO_SDC1_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_CARD_INT_BMSK                                               0x100
#define HWIO_SDC1_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_CARD_INT_SHFT                                                 0x8
#define HWIO_SDC1_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_CARD_REMOVAL_BMSK                                            0x80
#define HWIO_SDC1_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_CARD_REMOVAL_SHFT                                             0x7
#define HWIO_SDC1_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_CARD_INSERTION_BMSK                                          0x40
#define HWIO_SDC1_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_CARD_INSERTION_SHFT                                           0x6
#define HWIO_SDC1_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_BUF_RD_READY_BMSK                                            0x20
#define HWIO_SDC1_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_BUF_RD_READY_SHFT                                             0x5
#define HWIO_SDC1_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_BUF_WR_READY_BMSK                                            0x10
#define HWIO_SDC1_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_BUF_WR_READY_SHFT                                             0x4
#define HWIO_SDC1_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_DMA_INT_BMSK                                                  0x8
#define HWIO_SDC1_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_DMA_INT_SHFT                                                  0x3
#define HWIO_SDC1_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_BLK_GAP_EVENT_BMSK                                            0x4
#define HWIO_SDC1_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_BLK_GAP_EVENT_SHFT                                            0x2
#define HWIO_SDC1_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_TRANS_COMPLETE_BMSK                                           0x2
#define HWIO_SDC1_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_TRANS_COMPLETE_SHFT                                           0x1
#define HWIO_SDC1_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_CMD_COMPLETE_BMSK                                             0x1
#define HWIO_SDC1_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_CMD_COMPLETE_SHFT                                             0x0

#define HWIO_SDC1_SDCC_HC_REG_38_3A_ADDR                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000038)
#define HWIO_SDC1_SDCC_HC_REG_38_3A_OFFS                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000038)
#define HWIO_SDC1_SDCC_HC_REG_38_3A_RMSK                                                                     0xc7ffdfff
#define HWIO_SDC1_SDCC_HC_REG_38_3A_POR                                                                      0x00000000
#define HWIO_SDC1_SDCC_HC_REG_38_3A_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_38_3A_ATTR                                                                            0x3
#define HWIO_SDC1_SDCC_HC_REG_38_3A_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_38_3A_ADDR, HWIO_SDC1_SDCC_HC_REG_38_3A_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_38_3A_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_38_3A_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_38_3A_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_REG_38_3A_ADDR,v)
#define HWIO_SDC1_SDCC_HC_REG_38_3A_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_REG_38_3A_ADDR,m,v,HWIO_SDC1_SDCC_HC_REG_38_3A_IN)
#define HWIO_SDC1_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_VENDOR_SPECIFIC_ERR_BMSK                               0x80000000
#define HWIO_SDC1_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_VENDOR_SPECIFIC_ERR_SHFT                                     0x1f
#define HWIO_SDC1_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_VENDOR_SPECIFIC_STAT_BMSK                              0x40000000
#define HWIO_SDC1_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_VENDOR_SPECIFIC_STAT_SHFT                                    0x1e
#define HWIO_SDC1_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_TUNING_ERR_BMSK                                         0x4000000
#define HWIO_SDC1_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_TUNING_ERR_SHFT                                              0x1a
#define HWIO_SDC1_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_ADMA_ERR_BMSK                                           0x2000000
#define HWIO_SDC1_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_ADMA_ERR_SHFT                                                0x19
#define HWIO_SDC1_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_AUTO_CMD_ERR_BMSK                                       0x1000000
#define HWIO_SDC1_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_AUTO_CMD_ERR_SHFT                                            0x18
#define HWIO_SDC1_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_CURRENT_LIMIT_ERR_BMSK                                   0x800000
#define HWIO_SDC1_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_CURRENT_LIMIT_ERR_SHFT                                       0x17
#define HWIO_SDC1_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_DATA_END_BIT_ERR_BMSK                                    0x400000
#define HWIO_SDC1_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_DATA_END_BIT_ERR_SHFT                                        0x16
#define HWIO_SDC1_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_DATA_CRC_ERR_BMSK                                        0x200000
#define HWIO_SDC1_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_DATA_CRC_ERR_SHFT                                            0x15
#define HWIO_SDC1_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_DATA_TIMEOUT_BMSK                                        0x100000
#define HWIO_SDC1_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_DATA_TIMEOUT_SHFT                                            0x14
#define HWIO_SDC1_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_CMD_INDX_ERR_BMSK                                         0x80000
#define HWIO_SDC1_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_CMD_INDX_ERR_SHFT                                            0x13
#define HWIO_SDC1_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_CMD_END_BIT_ERR_BMSK                                      0x40000
#define HWIO_SDC1_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_CMD_END_BIT_ERR_SHFT                                         0x12
#define HWIO_SDC1_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_CMD_CRC_ERR_BMSK                                          0x20000
#define HWIO_SDC1_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_CMD_CRC_ERR_SHFT                                             0x11
#define HWIO_SDC1_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_CMD_TIMEOUT_BMSK                                          0x10000
#define HWIO_SDC1_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_CMD_TIMEOUT_SHFT                                             0x10
#define HWIO_SDC1_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_ERR_INT_BMSK                                            0x8000
#define HWIO_SDC1_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_ERR_INT_SHFT                                               0xf
#define HWIO_SDC1_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_CMD_QUEUE_BMSK                                          0x4000
#define HWIO_SDC1_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_CMD_QUEUE_SHFT                                             0xe
#define HWIO_SDC1_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_RETUNING_EVENT_BMSK                                     0x1000
#define HWIO_SDC1_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_RETUNING_EVENT_SHFT                                        0xc
#define HWIO_SDC1_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_INT_C_BMSK                                               0x800
#define HWIO_SDC1_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_INT_C_SHFT                                                 0xb
#define HWIO_SDC1_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_INT_B_BMSK                                               0x400
#define HWIO_SDC1_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_INT_B_SHFT                                                 0xa
#define HWIO_SDC1_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_INT_A_BMSK                                               0x200
#define HWIO_SDC1_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_INT_A_SHFT                                                 0x9
#define HWIO_SDC1_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_CARD_INT_BMSK                                            0x100
#define HWIO_SDC1_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_CARD_INT_SHFT                                              0x8
#define HWIO_SDC1_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_CARD_REMOVAL_BMSK                                         0x80
#define HWIO_SDC1_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_CARD_REMOVAL_SHFT                                          0x7
#define HWIO_SDC1_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_CARD_INSERTION_BMSK                                       0x40
#define HWIO_SDC1_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_CARD_INSERTION_SHFT                                        0x6
#define HWIO_SDC1_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_BUF_RD_READY_BMSK                                         0x20
#define HWIO_SDC1_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_BUF_RD_READY_SHFT                                          0x5
#define HWIO_SDC1_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_BUF_WR_READY_BMSK                                         0x10
#define HWIO_SDC1_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_BUF_WR_READY_SHFT                                          0x4
#define HWIO_SDC1_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_DMA_INT_BMSK                                               0x8
#define HWIO_SDC1_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_DMA_INT_SHFT                                               0x3
#define HWIO_SDC1_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_BLK_GAP_EVENT_BMSK                                         0x4
#define HWIO_SDC1_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_BLK_GAP_EVENT_SHFT                                         0x2
#define HWIO_SDC1_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_TRANS_COMPLETE_BMSK                                        0x2
#define HWIO_SDC1_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_TRANS_COMPLETE_SHFT                                        0x1
#define HWIO_SDC1_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_CMD_COMPLETE_BMSK                                          0x1
#define HWIO_SDC1_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_CMD_COMPLETE_SHFT                                          0x0

#define HWIO_SDC1_SDCC_HC_REG_3C_3E_ADDR                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x0000003c)
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_OFFS                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000003c)
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_RMSK                                                                     0xc0ff009f
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_POR                                                                      0x00000000
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_ATTR                                                                            0x3
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_3C_3E_ADDR, HWIO_SDC1_SDCC_HC_REG_3C_3E_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_3C_3E_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_REG_3C_3E_ADDR,v)
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_REG_3C_3E_ADDR,m,v,HWIO_SDC1_SDCC_HC_REG_3C_3E_IN)
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_HST_CTL2_PRESET_VALUE_EN_BMSK                                            0x80000000
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_HST_CTL2_PRESET_VALUE_EN_SHFT                                                  0x1f
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_HST_CTL2_ASYNC_INT_EN_BMSK                                               0x40000000
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_HST_CTL2_ASYNC_INT_EN_SHFT                                                     0x1e
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_HST_CTL2_SAMPL_CLK_SEL_BMSK                                                0x800000
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_HST_CTL2_SAMPL_CLK_SEL_SHFT                                                    0x17
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_HST_CTL2_EXEC_TUNING_BMSK                                                  0x400000
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_HST_CTL2_EXEC_TUNING_SHFT                                                      0x16
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_HST_CTL2_DRIVER_STRENGTH_SEL_BMSK                                          0x300000
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_HST_CTL2_DRIVER_STRENGTH_SEL_SHFT                                              0x14
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_HST_CTL2_SIGNALING_1_8_EN_BMSK                                              0x80000
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_HST_CTL2_SIGNALING_1_8_EN_SHFT                                                 0x13
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_HST_CTL2_UHS_MODE_SEL_BMSK                                                  0x70000
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_HST_CTL2_UHS_MODE_SEL_SHFT                                                     0x10
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_AUTO_CMD_NOT_ISSUED_BY_AUTO_CMD12_BMSK                                         0x80
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_AUTO_CMD_NOT_ISSUED_BY_AUTO_CMD12_SHFT                                          0x7
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_AUTO_CMD_INDX_ERR_BMSK                                                         0x10
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_AUTO_CMD_INDX_ERR_SHFT                                                          0x4
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_AUTO_CMD_END_BIT_ERR_BMSK                                                       0x8
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_AUTO_CMD_END_BIT_ERR_SHFT                                                       0x3
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_AUTO_CMD_CRC_ERR_BMSK                                                           0x4
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_AUTO_CMD_CRC_ERR_SHFT                                                           0x2
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_AUTO_CMD_TIMEOUT_BMSK                                                           0x2
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_AUTO_CMD_TIMEOUT_SHFT                                                           0x1
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_AUTO_CMD12_NOT_EXEC_BMSK                                                        0x1
#define HWIO_SDC1_SDCC_HC_REG_3C_3E_AUTO_CMD12_NOT_EXEC_SHFT                                                        0x0

#define HWIO_SDC1_SDCC_HC_REG_40_42_ADDR                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000040)
#define HWIO_SDC1_SDCC_HC_REG_40_42_OFFS                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000040)
#define HWIO_SDC1_SDCC_HC_REG_40_42_RMSK                                                                     0xf7efffbf
#define HWIO_SDC1_SDCC_HC_REG_40_42_POR                                                                      0x3029c8b2
#define HWIO_SDC1_SDCC_HC_REG_40_42_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_40_42_ATTR                                                                            0x1
#define HWIO_SDC1_SDCC_HC_REG_40_42_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_40_42_ADDR, HWIO_SDC1_SDCC_HC_REG_40_42_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_40_42_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_40_42_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_40_42_CAPABILITIES_SLOT_TYPE_BMSK                                              0xc0000000
#define HWIO_SDC1_SDCC_HC_REG_40_42_CAPABILITIES_SLOT_TYPE_SHFT                                                    0x1e
#define HWIO_SDC1_SDCC_HC_REG_40_42_CAPABILITIES_ASYNC_INT_SUPPORT_BMSK                                      0x20000000
#define HWIO_SDC1_SDCC_HC_REG_40_42_CAPABILITIES_ASYNC_INT_SUPPORT_SHFT                                            0x1d
#define HWIO_SDC1_SDCC_HC_REG_40_42_CAPABILITIES_SYS_BUS_SUPPORT_64_BIT_BMSK                                 0x10000000
#define HWIO_SDC1_SDCC_HC_REG_40_42_CAPABILITIES_SYS_BUS_SUPPORT_64_BIT_SHFT                                       0x1c
#define HWIO_SDC1_SDCC_HC_REG_40_42_CAPABILITIES_VOLTAGE_SUPPORT_1_8V_BMSK                                    0x4000000
#define HWIO_SDC1_SDCC_HC_REG_40_42_CAPABILITIES_VOLTAGE_SUPPORT_1_8V_SHFT                                         0x1a
#define HWIO_SDC1_SDCC_HC_REG_40_42_CAPABILITIES_VOLTAGE_SUPPORT_3_0V_BMSK                                    0x2000000
#define HWIO_SDC1_SDCC_HC_REG_40_42_CAPABILITIES_VOLTAGE_SUPPORT_3_0V_SHFT                                         0x19
#define HWIO_SDC1_SDCC_HC_REG_40_42_CAPABILITIES_VOLTAGE_SUPPORT_3_3V_BMSK                                    0x1000000
#define HWIO_SDC1_SDCC_HC_REG_40_42_CAPABILITIES_VOLTAGE_SUPPORT_3_3V_SHFT                                         0x18
#define HWIO_SDC1_SDCC_HC_REG_40_42_CAPABILITIES_SUSPEND_RESUME_SUPPORT_BMSK                                   0x800000
#define HWIO_SDC1_SDCC_HC_REG_40_42_CAPABILITIES_SUSPEND_RESUME_SUPPORT_SHFT                                       0x17
#define HWIO_SDC1_SDCC_HC_REG_40_42_CAPABILITIES_SDMA_SUPPORT_BMSK                                             0x400000
#define HWIO_SDC1_SDCC_HC_REG_40_42_CAPABILITIES_SDMA_SUPPORT_SHFT                                                 0x16
#define HWIO_SDC1_SDCC_HC_REG_40_42_CAPABILITIES_HS_SUPPORT_BMSK                                               0x200000
#define HWIO_SDC1_SDCC_HC_REG_40_42_CAPABILITIES_HS_SUPPORT_SHFT                                                   0x15
#define HWIO_SDC1_SDCC_HC_REG_40_42_CAPABILITIES_ADMA2_SUPPORT_BMSK                                             0x80000
#define HWIO_SDC1_SDCC_HC_REG_40_42_CAPABILITIES_ADMA2_SUPPORT_SHFT                                                0x13
#define HWIO_SDC1_SDCC_HC_REG_40_42_CAPABILITIES_SUPPORT_8_BIT_BMSK                                             0x40000
#define HWIO_SDC1_SDCC_HC_REG_40_42_CAPABILITIES_SUPPORT_8_BIT_SHFT                                                0x12
#define HWIO_SDC1_SDCC_HC_REG_40_42_CAPABILITIES_MAX_BLK_LENGTH_BMSK                                            0x30000
#define HWIO_SDC1_SDCC_HC_REG_40_42_CAPABILITIES_MAX_BLK_LENGTH_SHFT                                               0x10
#define HWIO_SDC1_SDCC_HC_REG_40_42_CAPABILITIES_BASE_SDCLK_FREQ_BMSK                                            0xff00
#define HWIO_SDC1_SDCC_HC_REG_40_42_CAPABILITIES_BASE_SDCLK_FREQ_SHFT                                               0x8
#define HWIO_SDC1_SDCC_HC_REG_40_42_CAPABILITIES_TIMEOUT_CLK_UNIT_BMSK                                             0x80
#define HWIO_SDC1_SDCC_HC_REG_40_42_CAPABILITIES_TIMEOUT_CLK_UNIT_SHFT                                              0x7
#define HWIO_SDC1_SDCC_HC_REG_40_42_CAPABILITIES_TIMEOUT_CLK_FREQ_BMSK                                             0x3f
#define HWIO_SDC1_SDCC_HC_REG_40_42_CAPABILITIES_TIMEOUT_CLK_FREQ_SHFT                                              0x0

#define HWIO_SDC1_SDCC_HC_REG_44_46_ADDR                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000044)
#define HWIO_SDC1_SDCC_HC_REG_44_46_OFFS                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000044)
#define HWIO_SDC1_SDCC_HC_REG_44_46_RMSK                                                                       0xffef77
#define HWIO_SDC1_SDCC_HC_REG_44_46_POR                                                                      0x00008007
#define HWIO_SDC1_SDCC_HC_REG_44_46_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_44_46_ATTR                                                                            0x1
#define HWIO_SDC1_SDCC_HC_REG_44_46_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_44_46_ADDR, HWIO_SDC1_SDCC_HC_REG_44_46_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_44_46_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_44_46_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_44_46_CAPABILITIES_CLK_MULTIPLIER_BMSK                                           0xff0000
#define HWIO_SDC1_SDCC_HC_REG_44_46_CAPABILITIES_CLK_MULTIPLIER_SHFT                                               0x10
#define HWIO_SDC1_SDCC_HC_REG_44_46_CAPABILITIES_RETUNING_MODE_BMSK                                              0xc000
#define HWIO_SDC1_SDCC_HC_REG_44_46_CAPABILITIES_RETUNING_MODE_SHFT                                                 0xe
#define HWIO_SDC1_SDCC_HC_REG_44_46_CAPABILITIES_USE_TUNING_FOR_SDR50_BMSK                                       0x2000
#define HWIO_SDC1_SDCC_HC_REG_44_46_CAPABILITIES_USE_TUNING_FOR_SDR50_SHFT                                          0xd
#define HWIO_SDC1_SDCC_HC_REG_44_46_CAPABILITIES_TIMER_CNT_FOR_RETUNING_BMSK                                      0xf00
#define HWIO_SDC1_SDCC_HC_REG_44_46_CAPABILITIES_TIMER_CNT_FOR_RETUNING_SHFT                                        0x8
#define HWIO_SDC1_SDCC_HC_REG_44_46_CAPABILITIES_DRIVER_TYPE_D_SUPPORT_BMSK                                        0x40
#define HWIO_SDC1_SDCC_HC_REG_44_46_CAPABILITIES_DRIVER_TYPE_D_SUPPORT_SHFT                                         0x6
#define HWIO_SDC1_SDCC_HC_REG_44_46_CAPABILITIES_DRIVER_TYPE_C_SUPPORT_BMSK                                        0x20
#define HWIO_SDC1_SDCC_HC_REG_44_46_CAPABILITIES_DRIVER_TYPE_C_SUPPORT_SHFT                                         0x5
#define HWIO_SDC1_SDCC_HC_REG_44_46_CAPABILITIES_DRIVER_TYPE_A_SUPPORT_BMSK                                        0x10
#define HWIO_SDC1_SDCC_HC_REG_44_46_CAPABILITIES_DRIVER_TYPE_A_SUPPORT_SHFT                                         0x4
#define HWIO_SDC1_SDCC_HC_REG_44_46_CAPABILITIES_DDR_50_SUPPORT_BMSK                                                0x4
#define HWIO_SDC1_SDCC_HC_REG_44_46_CAPABILITIES_DDR_50_SUPPORT_SHFT                                                0x2
#define HWIO_SDC1_SDCC_HC_REG_44_46_CAPABILITIES_SDR_104_SUPPORT_BMSK                                               0x2
#define HWIO_SDC1_SDCC_HC_REG_44_46_CAPABILITIES_SDR_104_SUPPORT_SHFT                                               0x1
#define HWIO_SDC1_SDCC_HC_REG_44_46_CAPABILITIES_SDR_50_SUPPORT_BMSK                                                0x1
#define HWIO_SDC1_SDCC_HC_REG_44_46_CAPABILITIES_SDR_50_SUPPORT_SHFT                                                0x0

#define HWIO_SDC1_SDCC_HC_REG_48_4A_ADDR                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000048)
#define HWIO_SDC1_SDCC_HC_REG_48_4A_OFFS                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000048)
#define HWIO_SDC1_SDCC_HC_REG_48_4A_RMSK                                                                       0xffffff
#define HWIO_SDC1_SDCC_HC_REG_48_4A_POR                                                                      0x00000000
#define HWIO_SDC1_SDCC_HC_REG_48_4A_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_48_4A_ATTR                                                                            0x3
#define HWIO_SDC1_SDCC_HC_REG_48_4A_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_48_4A_ADDR, HWIO_SDC1_SDCC_HC_REG_48_4A_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_48_4A_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_48_4A_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_48_4A_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_REG_48_4A_ADDR,v)
#define HWIO_SDC1_SDCC_HC_REG_48_4A_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_REG_48_4A_ADDR,m,v,HWIO_SDC1_SDCC_HC_REG_48_4A_IN)
#define HWIO_SDC1_SDCC_HC_REG_48_4A_MAX_CURRENT_1_8V_BMSK                                                      0xff0000
#define HWIO_SDC1_SDCC_HC_REG_48_4A_MAX_CURRENT_1_8V_SHFT                                                          0x10
#define HWIO_SDC1_SDCC_HC_REG_48_4A_MAX_CURRENT_3_0V_BMSK                                                        0xff00
#define HWIO_SDC1_SDCC_HC_REG_48_4A_MAX_CURRENT_3_0V_SHFT                                                           0x8
#define HWIO_SDC1_SDCC_HC_REG_48_4A_MAX_CURRENT_3_3V_BMSK                                                          0xff
#define HWIO_SDC1_SDCC_HC_REG_48_4A_MAX_CURRENT_3_3V_SHFT                                                           0x0

#define HWIO_SDC1_SDCC_HC_REG_50_52_ADDR                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000050)
#define HWIO_SDC1_SDCC_HC_REG_50_52_OFFS                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000050)
#define HWIO_SDC1_SDCC_HC_REG_50_52_RMSK                                                                     0xf3ff009f
#define HWIO_SDC1_SDCC_HC_REG_50_52_POR                                                                      0x00000000
#define HWIO_SDC1_SDCC_HC_REG_50_52_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_50_52_ATTR                                                                            0x2
#define HWIO_SDC1_SDCC_HC_REG_50_52_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_REG_50_52_ADDR,v)
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_VENDOR_SPECIFIC_ERR_STS_BMSK                         0xf0000000
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_VENDOR_SPECIFIC_ERR_STS_SHFT                               0x1c
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_ADMA_BMSK                                             0x2000000
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_ADMA_SHFT                                                  0x19
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_AUTO_CMD_BMSK                                         0x1000000
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_AUTO_CMD_SHFT                                              0x18
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_CURRENT_LIMIT_BMSK                                     0x800000
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_CURRENT_LIMIT_SHFT                                         0x17
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_DATA_END_BIT_BMSK                                      0x400000
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_DATA_END_BIT_SHFT                                          0x16
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_DATA_CRC_BMSK                                          0x200000
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_DATA_CRC_SHFT                                              0x15
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_DATA_TIMEOUT_BMSK                                      0x100000
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_DATA_TIMEOUT_SHFT                                          0x14
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_CMD_INDX_BMSK                                           0x80000
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_CMD_INDX_SHFT                                              0x13
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_CMD_END_BIT_BMSK                                        0x40000
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_CMD_END_BIT_SHFT                                           0x12
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_CMD_CRC_BMSK                                            0x20000
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_CMD_CRC_SHFT                                               0x11
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_CMD_TIMEOUT_BMSK                                        0x10000
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_CMD_TIMEOUT_SHFT                                           0x10
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_CMD_NOT_ISSUED_BY_AUTO_CMD12_BMSK                              0x80
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_CMD_NOT_ISSUED_BY_AUTO_CMD12_SHFT                               0x7
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_AUTO_CMD_INDX_ERR_BMSK                                         0x10
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_AUTO_CMD_INDX_ERR_SHFT                                          0x4
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_AUTO_CMD_END_BIT_ERR_BMSK                                       0x8
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_AUTO_CMD_END_BIT_ERR_SHFT                                       0x3
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_AUTO_CMD_CRC_ERR_BMSK                                           0x4
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_AUTO_CMD_CRC_ERR_SHFT                                           0x2
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_AUTO_CMD_TIMEOUT_ERR_BMSK                                       0x2
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_AUTO_CMD_TIMEOUT_ERR_SHFT                                       0x1
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_AUTO_CMD12_NOT_EXEC_BMSK                                        0x1
#define HWIO_SDC1_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_AUTO_CMD12_NOT_EXEC_SHFT                                        0x0

#define HWIO_SDC1_SDCC_HC_REG_54_56_ADDR                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000054)
#define HWIO_SDC1_SDCC_HC_REG_54_56_OFFS                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000054)
#define HWIO_SDC1_SDCC_HC_REG_54_56_RMSK                                                                            0x7
#define HWIO_SDC1_SDCC_HC_REG_54_56_POR                                                                      0x00000000
#define HWIO_SDC1_SDCC_HC_REG_54_56_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_54_56_ATTR                                                                            0x1
#define HWIO_SDC1_SDCC_HC_REG_54_56_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_54_56_ADDR, HWIO_SDC1_SDCC_HC_REG_54_56_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_54_56_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_54_56_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_54_56_ADMA_LENGTH_MISMATCH_ERR_BMSK                                                   0x4
#define HWIO_SDC1_SDCC_HC_REG_54_56_ADMA_LENGTH_MISMATCH_ERR_SHFT                                                   0x2
#define HWIO_SDC1_SDCC_HC_REG_54_56_ADMA_ERR_STATE_BMSK                                                             0x3
#define HWIO_SDC1_SDCC_HC_REG_54_56_ADMA_ERR_STATE_SHFT                                                             0x0

#define HWIO_SDC1_SDCC_HC_REG_58_5A_ADDR                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000058)
#define HWIO_SDC1_SDCC_HC_REG_58_5A_OFFS                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000058)
#define HWIO_SDC1_SDCC_HC_REG_58_5A_RMSK                                                                     0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_58_5A_POR                                                                      0x00000000
#define HWIO_SDC1_SDCC_HC_REG_58_5A_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_58_5A_ATTR                                                                            0x3
#define HWIO_SDC1_SDCC_HC_REG_58_5A_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_58_5A_ADDR, HWIO_SDC1_SDCC_HC_REG_58_5A_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_58_5A_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_58_5A_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_58_5A_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_REG_58_5A_ADDR,v)
#define HWIO_SDC1_SDCC_HC_REG_58_5A_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_REG_58_5A_ADDR,m,v,HWIO_SDC1_SDCC_HC_REG_58_5A_IN)
#define HWIO_SDC1_SDCC_HC_REG_58_5A_ADMA_SYS_ADDRESS_BMSK                                                    0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_58_5A_ADMA_SYS_ADDRESS_SHFT                                                           0x0

#define HWIO_SDC1_SDCC_HC_REG_5C_5E_ADDR                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x0000005c)
#define HWIO_SDC1_SDCC_HC_REG_5C_5E_OFFS                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000005c)
#define HWIO_SDC1_SDCC_HC_REG_5C_5E_RMSK                                                                     0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_5C_5E_POR                                                                      0x00000000
#define HWIO_SDC1_SDCC_HC_REG_5C_5E_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_5C_5E_ATTR                                                                            0x3
#define HWIO_SDC1_SDCC_HC_REG_5C_5E_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_5C_5E_ADDR, HWIO_SDC1_SDCC_HC_REG_5C_5E_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_5C_5E_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_5C_5E_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_5C_5E_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_REG_5C_5E_ADDR,v)
#define HWIO_SDC1_SDCC_HC_REG_5C_5E_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_REG_5C_5E_ADDR,m,v,HWIO_SDC1_SDCC_HC_REG_5C_5E_IN)
#define HWIO_SDC1_SDCC_HC_REG_5C_5E_ADMA_SYS_ADDRESS_64_BMSK                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_5C_5E_ADMA_SYS_ADDRESS_64_SHFT                                                        0x0

#define HWIO_SDC1_SDCC_HC_REG_60_62_ADDR                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000060)
#define HWIO_SDC1_SDCC_HC_REG_60_62_OFFS                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000060)
#define HWIO_SDC1_SDCC_HC_REG_60_62_RMSK                                                                     0xc7ffc7ff
#define HWIO_SDC1_SDCC_HC_REG_60_62_POR                                                                      0x00040000
#define HWIO_SDC1_SDCC_HC_REG_60_62_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_60_62_ATTR                                                                            0x1
#define HWIO_SDC1_SDCC_HC_REG_60_62_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_60_62_ADDR, HWIO_SDC1_SDCC_HC_REG_60_62_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_60_62_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_60_62_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_60_62_DEFAULT_SPEED_DRIVER_STRENGTH_SEL_BMSK                                   0xc0000000
#define HWIO_SDC1_SDCC_HC_REG_60_62_DEFAULT_SPEED_DRIVER_STRENGTH_SEL_SHFT                                         0x1e
#define HWIO_SDC1_SDCC_HC_REG_60_62_DEFAULT_SPEED_CLK_GEN_SEL_BMSK                                            0x4000000
#define HWIO_SDC1_SDCC_HC_REG_60_62_DEFAULT_SPEED_CLK_GEN_SEL_SHFT                                                 0x1a
#define HWIO_SDC1_SDCC_HC_REG_60_62_DEFAULT_SPEED_SDCLK_FREQ_SEL_BMSK                                         0x3ff0000
#define HWIO_SDC1_SDCC_HC_REG_60_62_DEFAULT_SPEED_SDCLK_FREQ_SEL_SHFT                                              0x10
#define HWIO_SDC1_SDCC_HC_REG_60_62_INIT_DRIVER_STRENGTH_SEL_BMSK                                                0xc000
#define HWIO_SDC1_SDCC_HC_REG_60_62_INIT_DRIVER_STRENGTH_SEL_SHFT                                                   0xe
#define HWIO_SDC1_SDCC_HC_REG_60_62_INIT_CLK_GEN_SEL_BMSK                                                         0x400
#define HWIO_SDC1_SDCC_HC_REG_60_62_INIT_CLK_GEN_SEL_SHFT                                                           0xa
#define HWIO_SDC1_SDCC_HC_REG_60_62_INIT_SDCLK_FREQ_SEL_BMSK                                                      0x3ff
#define HWIO_SDC1_SDCC_HC_REG_60_62_INIT_SDCLK_FREQ_SEL_SHFT                                                        0x0

#define HWIO_SDC1_SDCC_HC_REG_64_66_ADDR                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000064)
#define HWIO_SDC1_SDCC_HC_REG_64_66_OFFS                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000064)
#define HWIO_SDC1_SDCC_HC_REG_64_66_RMSK                                                                     0xc7ffc7ff
#define HWIO_SDC1_SDCC_HC_REG_64_66_POR                                                                      0x00040002
#define HWIO_SDC1_SDCC_HC_REG_64_66_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_64_66_ATTR                                                                            0x1
#define HWIO_SDC1_SDCC_HC_REG_64_66_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_64_66_ADDR, HWIO_SDC1_SDCC_HC_REG_64_66_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_64_66_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_64_66_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_64_66_SDR12_DRIVER_STRENGTH_SEL_BMSK                                           0xc0000000
#define HWIO_SDC1_SDCC_HC_REG_64_66_SDR12_DRIVER_STRENGTH_SEL_SHFT                                                 0x1e
#define HWIO_SDC1_SDCC_HC_REG_64_66_SDR12_CLK_GEN_SEL_BMSK                                                    0x4000000
#define HWIO_SDC1_SDCC_HC_REG_64_66_SDR12_CLK_GEN_SEL_SHFT                                                         0x1a
#define HWIO_SDC1_SDCC_HC_REG_64_66_SDR12_SDCLK_FREQ_SEL_BMSK                                                 0x3ff0000
#define HWIO_SDC1_SDCC_HC_REG_64_66_SDR12_SDCLK_FREQ_SEL_SHFT                                                      0x10
#define HWIO_SDC1_SDCC_HC_REG_64_66_HS_DRIVER_STRENGTH_SEL_BMSK                                                  0xc000
#define HWIO_SDC1_SDCC_HC_REG_64_66_HS_DRIVER_STRENGTH_SEL_SHFT                                                     0xe
#define HWIO_SDC1_SDCC_HC_REG_64_66_HS_CLK_GEN_SEL_BMSK                                                           0x400
#define HWIO_SDC1_SDCC_HC_REG_64_66_HS_CLK_GEN_SEL_SHFT                                                             0xa
#define HWIO_SDC1_SDCC_HC_REG_64_66_HS_SDCLK_FREQ_SEL_BMSK                                                        0x3ff
#define HWIO_SDC1_SDCC_HC_REG_64_66_HS_SDCLK_FREQ_SEL_SHFT                                                          0x0

#define HWIO_SDC1_SDCC_HC_REG_68_6A_ADDR                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000068)
#define HWIO_SDC1_SDCC_HC_REG_68_6A_OFFS                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000068)
#define HWIO_SDC1_SDCC_HC_REG_68_6A_RMSK                                                                     0xc7ffc7ff
#define HWIO_SDC1_SDCC_HC_REG_68_6A_POR                                                                      0x00010002
#define HWIO_SDC1_SDCC_HC_REG_68_6A_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_68_6A_ATTR                                                                            0x1
#define HWIO_SDC1_SDCC_HC_REG_68_6A_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_68_6A_ADDR, HWIO_SDC1_SDCC_HC_REG_68_6A_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_68_6A_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_68_6A_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_68_6A_SDR50_DRIVER_STRENGTH_SEL_BMSK                                           0xc0000000
#define HWIO_SDC1_SDCC_HC_REG_68_6A_SDR50_DRIVER_STRENGTH_SEL_SHFT                                                 0x1e
#define HWIO_SDC1_SDCC_HC_REG_68_6A_SDR50_CLK_GEN_SEL_BMSK                                                    0x4000000
#define HWIO_SDC1_SDCC_HC_REG_68_6A_SDR50_CLK_GEN_SEL_SHFT                                                         0x1a
#define HWIO_SDC1_SDCC_HC_REG_68_6A_SDR50_SDCLK_FREQ_SEL_BMSK                                                 0x3ff0000
#define HWIO_SDC1_SDCC_HC_REG_68_6A_SDR50_SDCLK_FREQ_SEL_SHFT                                                      0x10
#define HWIO_SDC1_SDCC_HC_REG_68_6A_SDR25_DRIVER_STRENGTH_SEL_BMSK                                               0xc000
#define HWIO_SDC1_SDCC_HC_REG_68_6A_SDR25_DRIVER_STRENGTH_SEL_SHFT                                                  0xe
#define HWIO_SDC1_SDCC_HC_REG_68_6A_SDR25_CLK_GEN_SEL_BMSK                                                        0x400
#define HWIO_SDC1_SDCC_HC_REG_68_6A_SDR25_CLK_GEN_SEL_SHFT                                                          0xa
#define HWIO_SDC1_SDCC_HC_REG_68_6A_SDR25_SDCLK_FREQ_SEL_BMSK                                                     0x3ff
#define HWIO_SDC1_SDCC_HC_REG_68_6A_SDR25_SDCLK_FREQ_SEL_SHFT                                                       0x0

#define HWIO_SDC1_SDCC_HC_REG_6C_6E_ADDR                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x0000006c)
#define HWIO_SDC1_SDCC_HC_REG_6C_6E_OFFS                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000006c)
#define HWIO_SDC1_SDCC_HC_REG_6C_6E_RMSK                                                                     0xc7ffc7ff
#define HWIO_SDC1_SDCC_HC_REG_6C_6E_POR                                                                      0x00020000
#define HWIO_SDC1_SDCC_HC_REG_6C_6E_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_6C_6E_ATTR                                                                            0x1
#define HWIO_SDC1_SDCC_HC_REG_6C_6E_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_6C_6E_ADDR, HWIO_SDC1_SDCC_HC_REG_6C_6E_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_6C_6E_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_6C_6E_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_6C_6E_DDR50_DRIVER_STRENGTH_SEL_BMSK                                           0xc0000000
#define HWIO_SDC1_SDCC_HC_REG_6C_6E_DDR50_DRIVER_STRENGTH_SEL_SHFT                                                 0x1e
#define HWIO_SDC1_SDCC_HC_REG_6C_6E_DDR50_CLK_GEN_SEL_BMSK                                                    0x4000000
#define HWIO_SDC1_SDCC_HC_REG_6C_6E_DDR50_CLK_GEN_SEL_SHFT                                                         0x1a
#define HWIO_SDC1_SDCC_HC_REG_6C_6E_DDR50_SDCLK_FREQ_SEL_BMSK                                                 0x3ff0000
#define HWIO_SDC1_SDCC_HC_REG_6C_6E_DDR50_SDCLK_FREQ_SEL_SHFT                                                      0x10
#define HWIO_SDC1_SDCC_HC_REG_6C_6E_SDR104_DRIVER_STRENGTH_SEL_BMSK                                              0xc000
#define HWIO_SDC1_SDCC_HC_REG_6C_6E_SDR104_DRIVER_STRENGTH_SEL_SHFT                                                 0xe
#define HWIO_SDC1_SDCC_HC_REG_6C_6E_SDR104_CLK_GEN_SEL_BMSK                                                       0x400
#define HWIO_SDC1_SDCC_HC_REG_6C_6E_SDR104_CLK_GEN_SEL_SHFT                                                         0xa
#define HWIO_SDC1_SDCC_HC_REG_6C_6E_SDR104_SDCLK_FREQ_SEL_BMSK                                                    0x3ff
#define HWIO_SDC1_SDCC_HC_REG_6C_6E_SDR104_SDCLK_FREQ_SEL_SHFT                                                      0x0

#define HWIO_SDC1_SDCC_HC_REG_E0_E2_ADDR                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x000000e0)
#define HWIO_SDC1_SDCC_HC_REG_E0_E2_OFFS                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x000000e0)
#define HWIO_SDC1_SDCC_HC_REG_E0_E2_RMSK                                                                     0x7f777f37
#define HWIO_SDC1_SDCC_HC_REG_E0_E2_POR                                                                      0x00000000
#define HWIO_SDC1_SDCC_HC_REG_E0_E2_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_E0_E2_ATTR                                                                            0x3
#define HWIO_SDC1_SDCC_HC_REG_E0_E2_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_E0_E2_ADDR, HWIO_SDC1_SDCC_HC_REG_E0_E2_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_E0_E2_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_E0_E2_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_E0_E2_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_REG_E0_E2_ADDR,v)
#define HWIO_SDC1_SDCC_HC_REG_E0_E2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_REG_E0_E2_ADDR,m,v,HWIO_SDC1_SDCC_HC_REG_E0_E2_IN)
#define HWIO_SDC1_SDCC_HC_REG_E0_E2_SHARED_BUS_CTL_BACK_END_PWR_BMSK                                         0x7f000000
#define HWIO_SDC1_SDCC_HC_REG_E0_E2_SHARED_BUS_CTL_BACK_END_PWR_SHFT                                               0x18
#define HWIO_SDC1_SDCC_HC_REG_E0_E2_SHARED_BUS_INT_PIN_SEL_BMSK                                                0x700000
#define HWIO_SDC1_SDCC_HC_REG_E0_E2_SHARED_BUS_INT_PIN_SEL_SHFT                                                    0x14
#define HWIO_SDC1_SDCC_HC_REG_E0_E2_SHARED_BUS_CTL_CLK_PIN_SEL_BMSK                                             0x70000
#define HWIO_SDC1_SDCC_HC_REG_E0_E2_SHARED_BUS_CTL_CLK_PIN_SEL_SHFT                                                0x10
#define HWIO_SDC1_SDCC_HC_REG_E0_E2_SHARED_BUS_CTL_BUS_WIDTH_PRESET_BMSK                                         0x7f00
#define HWIO_SDC1_SDCC_HC_REG_E0_E2_SHARED_BUS_CTL_BUS_WIDTH_PRESET_SHFT                                            0x8
#define HWIO_SDC1_SDCC_HC_REG_E0_E2_SHARED_BUS_CTL_NUM_INT_INPUT_PINS_BMSK                                         0x30
#define HWIO_SDC1_SDCC_HC_REG_E0_E2_SHARED_BUS_CTL_NUM_INT_INPUT_PINS_SHFT                                          0x4
#define HWIO_SDC1_SDCC_HC_REG_E0_E2_SHARED_BUS_CTL_NUM_CLK_PINS_BMSK                                                0x7
#define HWIO_SDC1_SDCC_HC_REG_E0_E2_SHARED_BUS_CTL_NUM_CLK_PINS_SHFT                                                0x0

#define HWIO_SDC1_SDCC_HC_REG_FC_FE_ADDR                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x000000fc)
#define HWIO_SDC1_SDCC_HC_REG_FC_FE_OFFS                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x000000fc)
#define HWIO_SDC1_SDCC_HC_REG_FC_FE_RMSK                                                                     0xffff00ff
#define HWIO_SDC1_SDCC_HC_REG_FC_FE_POR                                                                      0x00020000
#define HWIO_SDC1_SDCC_HC_REG_FC_FE_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_FC_FE_ATTR                                                                            0x3
#define HWIO_SDC1_SDCC_HC_REG_FC_FE_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_FC_FE_ADDR, HWIO_SDC1_SDCC_HC_REG_FC_FE_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_FC_FE_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_FC_FE_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_FC_FE_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_REG_FC_FE_ADDR,v)
#define HWIO_SDC1_SDCC_HC_REG_FC_FE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_REG_FC_FE_ADDR,m,v,HWIO_SDC1_SDCC_HC_REG_FC_FE_IN)
#define HWIO_SDC1_SDCC_HC_REG_FC_FE_HC_VENDOR_VERSION_NUM_BMSK                                               0xff000000
#define HWIO_SDC1_SDCC_HC_REG_FC_FE_HC_VENDOR_VERSION_NUM_SHFT                                                     0x18
#define HWIO_SDC1_SDCC_HC_REG_FC_FE_HC_SPEC_VERSION_NUM_BMSK                                                   0xff0000
#define HWIO_SDC1_SDCC_HC_REG_FC_FE_HC_SPEC_VERSION_NUM_SHFT                                                       0x10
#define HWIO_SDC1_SDCC_HC_REG_FC_FE_INT_SIGNAL_FOR_EACH_SLOT_BMSK                                                  0xff
#define HWIO_SDC1_SDCC_HC_REG_FC_FE_INT_SIGNAL_FOR_EACH_SLOT_SHFT                                                   0x0

#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_ADDR                                                                (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000200)
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_OFFS                                                                (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000200)
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_RMSK                                                                0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_POR                                                                 0x6000642c
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_POR_RMSK                                                            0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_ATTR                                                                       0x3
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_ADDR, HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_ADDR,v)
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_ADDR,m,v,HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_IN)
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_SDC4_DIS_DOUT_BMSK                                                  0x80000000
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_SDC4_DIS_DOUT_SHFT                                                        0x1f
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_DLL_RST_BMSK                                                        0x40000000
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_DLL_RST_SHFT                                                              0x1e
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_PDN_BMSK                                                            0x20000000
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_PDN_SHFT                                                                  0x1d
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_CK_INTP_SEL_BMSK                                                    0x10000000
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_CK_INTP_SEL_SHFT                                                          0x1c
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_CK_INTP_EN_BMSK                                                      0x8000000
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_CK_INTP_EN_SHFT                                                           0x1b
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_MCLK_FREQ_BMSK                                                       0x7000000
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_MCLK_FREQ_SHFT                                                            0x18
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_CDR_SELEXT_BMSK                                                       0xf00000
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_CDR_SELEXT_SHFT                                                           0x14
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_CDR_EXT_EN_BMSK                                                        0x80000
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_CDR_EXT_EN_SHFT                                                           0x13
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_CK_OUT_EN_BMSK                                                         0x40000
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_CK_OUT_EN_SHFT                                                            0x12
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_CDR_EN_BMSK                                                            0x20000
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_CDR_EN_SHFT                                                               0x11
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_DLL_EN_BMSK                                                            0x10000
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_DLL_EN_SHFT                                                               0x10
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_CDR_UPD_RATE_BMSK                                                       0xc000
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_CDR_UPD_RATE_SHFT                                                          0xe
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_DLL_UPD_RATE_BMSK                                                       0x3000
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_DLL_UPD_RATE_SHFT                                                          0xc
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_DLL_PHASE_DET_BMSK                                                       0xc00
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_DLL_PHASE_DET_SHFT                                                         0xa
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_CDR_ALGORITHM_SEL_BMSK                                                   0x200
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_CDR_ALGORITHM_SEL_SHFT                                                     0x9
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_DLY_LINE_SWITCH_CLK_BMSK                                                 0x100
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_DLY_LINE_SWITCH_CLK_SHFT                                                   0x8
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_CDR_PHASE_SEL_MODE_BMSK                                                   0xc0
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_CDR_PHASE_SEL_MODE_SHFT                                                    0x6
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_MCLK_GATING_ENABLE_BMSK                                                   0x20
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_MCLK_GATING_ENABLE_SHFT                                                    0x5
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_FINE_PHASE_ENABLE_BMSK                                                    0x10
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_FINE_PHASE_ENABLE_SHFT                                                     0x4
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_CDR_FINE_PHASE_BMSK                                                        0xc
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_CDR_FINE_PHASE_SHFT                                                        0x2
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_RCLK_IN_TEST_EN_BMSK                                                       0x2
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_RCLK_IN_TEST_EN_SHFT                                                       0x1
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_CMD_DAT_TRACK_SEL_BMSK                                                     0x1
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_CMD_DAT_TRACK_SEL_SHFT                                                     0x0

#define HWIO_SDC1_SDCC_HC_REG_DLL_TEST_CTL_ADDR                                                              (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000204)
#define HWIO_SDC1_SDCC_HC_REG_DLL_TEST_CTL_OFFS                                                              (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000204)
#define HWIO_SDC1_SDCC_HC_REG_DLL_TEST_CTL_RMSK                                                              0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_DLL_TEST_CTL_POR                                                               0x00000000
#define HWIO_SDC1_SDCC_HC_REG_DLL_TEST_CTL_POR_RMSK                                                          0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_DLL_TEST_CTL_ATTR                                                                     0x3
#define HWIO_SDC1_SDCC_HC_REG_DLL_TEST_CTL_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_DLL_TEST_CTL_ADDR, HWIO_SDC1_SDCC_HC_REG_DLL_TEST_CTL_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_DLL_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_DLL_TEST_CTL_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_DLL_TEST_CTL_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_REG_DLL_TEST_CTL_ADDR,v)
#define HWIO_SDC1_SDCC_HC_REG_DLL_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_REG_DLL_TEST_CTL_ADDR,m,v,HWIO_SDC1_SDCC_HC_REG_DLL_TEST_CTL_IN)
#define HWIO_SDC1_SDCC_HC_REG_DLL_TEST_CTL_SDC4_DLL_TEST_CTL_BMSK                                            0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_DLL_TEST_CTL_SDC4_DLL_TEST_CTL_SHFT                                                   0x0

#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_ADDR                                                                (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000208)
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_OFFS                                                                (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000208)
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_RMSK                                                                    0x1ffd
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_POR                                                                 0x00000000
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_POR_RMSK                                                            0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_ATTR                                                                       0x1
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_ADDR, HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_SDC4_DTEST_MUXSEL_BMSK                                                  0x1000
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_SDC4_DTEST_MUXSEL_SHFT                                                     0xc
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_DDR_DLL_LOCK_JDR_BMSK                                                    0x800
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_DDR_DLL_LOCK_JDR_SHFT                                                      0xb
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_SDC4_DLL_DTEST_OUT_ATPG_BMSK                                             0x600
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_SDC4_DLL_DTEST_OUT_ATPG_SHFT                                               0x9
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_SDC4_DLL_LOCK_ATPG_BMSK                                                  0x100
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_SDC4_DLL_LOCK_ATPG_SHFT                                                    0x8
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_DLL_LOCK_BMSK                                                             0x80
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_DLL_LOCK_SHFT                                                              0x7
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_CDR_PHASE_BMSK                                                            0x78
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_CDR_PHASE_SHFT                                                             0x3
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_DDLL_COARSE_CAL_BMSK                                                       0x4
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_DDLL_COARSE_CAL_SHFT                                                       0x2
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_DDR_DLL_LOCK_BMSK                                                          0x1
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_DDR_DLL_LOCK_SHFT                                                          0x0

#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_ADDR                                                          (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x0000020c)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_OFFS                                                          (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000020c)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_RMSK                                                          0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_POR                                                           0x00000a0c
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_POR_RMSK                                                      0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_ATTR                                                                 0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_ADDR, HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_RMSK)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_ADDR, m)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_ADDR,v)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_ADDR,m,v,HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_IN)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_AUTO_CMD_COUNTER_BMSK                                      0xffc00000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_AUTO_CMD_COUNTER_SHFT                                            0x16
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_SELECT_IN_BMSK                                               0x380000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_SELECT_IN_SHFT                                                   0x13
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_SELECT_IN_EN_BMSK                                             0x40000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_SELECT_IN_EN_SHFT                                                0x12
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_ADMA_DESC_PRIORITY_BMSK                                          0x20000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_ADMA_DESC_PRIORITY_SHFT                                             0x11
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_IO_PAD_PWR_SWITCH_BMSK                                        0x10000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_IO_PAD_PWR_SWITCH_SHFT                                           0x10
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_IO_PAD_PWR_SWITCH_EN_BMSK                                      0x8000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_IO_PAD_PWR_SWITCH_EN_SHFT                                         0xf
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_IGNORE_CTOUT_BMSK                                              0x4000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_IGNORE_CTOUT_SHFT                                                 0xe
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_SW_SDC4_CMD19_BMSK                                             0x2000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_SW_SDC4_CMD19_SHFT                                                0xd
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_INVERT_OUT_BMSK                                                0x1000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_INVERT_OUT_SHFT                                                   0xc
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_INVERT_OUT_EN_BMSK                                              0x800
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_INVERT_OUT_EN_SHFT                                                0xb
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_MODE_BMSK                                                          0x400
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_MODE_SHFT                                                            0xa
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_SDC4_MCLK_SEL_BMSK                                                 0x300
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_SDC4_MCLK_SEL_SHFT                                                   0x8
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_ADMA_ERR_SIZE_EN_BMSK                                               0x80
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_ADMA_ERR_SIZE_EN_SHFT                                                0x7
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_AUTO_CMD21_EN_BMSK                                               0x40
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_AUTO_CMD21_EN_SHFT                                                0x6
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_AUTO_CMD19_EN_BMSK                                               0x20
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_AUTO_CMD19_EN_SHFT                                                0x5
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_HCLKON_SW_EN_BMSK                                                0x10
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_HCLKON_SW_EN_SHFT                                                 0x4
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_ADMA_BUSREQ_EN_BMSK                                                  0x8
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_ADMA_BUSREQ_EN_SHFT                                                  0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_FLOW_EN_BMSK                                                         0x4
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_FLOW_EN_SHFT                                                         0x2
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_PWRSAVE_BMSK                                                         0x2
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_PWRSAVE_SHFT                                                         0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_CLK_INV_BMSK                                                         0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC_CLK_INV_SHFT                                                         0x0

#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_ADDR                                                         (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000210)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_OFFS                                                         (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000210)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_RMSK                                                         0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_POR                                                          0xf88218a8
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_POR_RMSK                                                     0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_ATTR                                                                0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_ADDR, HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_RMSK)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_ADDR, m)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_ADDR,v)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_ADDR,m,v,HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_IN)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_ABURST_BMSK                                          0x80000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_ABURST_SHFT                                                0x1f
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_NUM_OUTSTANDING_DATA_BMSK                                    0x70000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_NUM_OUTSTANDING_DATA_SHFT                                          0x1c
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_PROCEED_AXI_AFTER_ERR_BMSK                                    0x8000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_PROCEED_AXI_AFTER_ERR_SHFT                                         0x1b
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_AFULL_CALC_BMSK                                       0x4000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_AFULL_CALC_SHFT                                            0x1a
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_ONE_MID_SUPPORT_BMSK                                          0x2000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_ONE_MID_SUPPORT_SHFT                                               0x19
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_READ_MEMTYPE_BMSK                                     0x1c00000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_READ_MEMTYPE_SHFT                                          0x16
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_HC_SW_RST_REQ_BMSK                                             0x200000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_HC_SW_RST_REQ_SHFT                                                 0x15
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_HC_SW_RST_WAIT_IDLE_DIS_BMSK                                   0x100000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_HC_SW_RST_WAIT_IDLE_DIS_SHFT                                       0x14
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_SDCC5_HALT_REQ_BMSK                                             0x80000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_SDCC5_HALT_REQ_SHFT                                                0x13
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_SDCC5_HALT_ACK_BMSK                                             0x40000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_SDCC5_HALT_ACK_SHFT                                                0x12
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_SDCC5_M_IDLE_BMSK                                               0x20000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_SDCC5_M_IDLE_SHFT                                                  0x11
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_SDCC5_HALT_ACK_SW_EN_BMSK                                       0x10000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_SDCC5_HALT_ACK_SW_EN_SHFT                                          0x10
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_SDCC5_HALT_ACK_SW_BMSK                                           0x8000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_SDCC5_HALT_ACK_SW_SHFT                                              0xf
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_SDCC5_M_IDLE_DIS_BMSK                                            0x4000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_SDCC5_M_IDLE_DIS_SHFT                                               0xe
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_INTERLEAVING_EN_BMSK                                     0x2000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_INTERLEAVING_EN_SHFT                                        0xd
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_TRANSIENT_BMSK                                           0x1000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_TRANSIENT_SHFT                                              0xc
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_PROTNS_BMSK                                               0x800
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_PROTNS_SHFT                                                 0xb
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_REQPRIORITY_BMSK                                          0x600
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_REQPRIORITY_SHFT                                            0x9
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_MEMTYPE_BMSK                                              0x1c0
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_MEMTYPE_SHFT                                                0x6
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_NOALLOCATE_BMSK                                            0x20
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_NOALLOCATE_SHFT                                             0x5
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_INNERSHARED_BMSK                                           0x10
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_INNERSHARED_SHFT                                            0x4
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_SHARED_BMSK                                                 0x8
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_SHARED_SHFT                                                 0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_OOOWR_BMSK                                                  0x4
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_OOOWR_SHFT                                                  0x2
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_OOORD_BMSK                                                  0x2
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_OOORD_SHFT                                                  0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_AFULL_BMSK                                                  0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_AFULL_SHFT                                                  0x0

#define HWIO_SDC1_SDCC_HC_VS_ADMA_ERR_ADDR0_ADDR                                                             (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000214)
#define HWIO_SDC1_SDCC_HC_VS_ADMA_ERR_ADDR0_OFFS                                                             (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000214)
#define HWIO_SDC1_SDCC_HC_VS_ADMA_ERR_ADDR0_RMSK                                                             0xffffffff
#define HWIO_SDC1_SDCC_HC_VS_ADMA_ERR_ADDR0_POR                                                              0x00000000
#define HWIO_SDC1_SDCC_HC_VS_ADMA_ERR_ADDR0_POR_RMSK                                                         0xffffffff
#define HWIO_SDC1_SDCC_HC_VS_ADMA_ERR_ADDR0_ATTR                                                                    0x1
#define HWIO_SDC1_SDCC_HC_VS_ADMA_ERR_ADDR0_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VS_ADMA_ERR_ADDR0_ADDR, HWIO_SDC1_SDCC_HC_VS_ADMA_ERR_ADDR0_RMSK)
#define HWIO_SDC1_SDCC_HC_VS_ADMA_ERR_ADDR0_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VS_ADMA_ERR_ADDR0_ADDR, m)
#define HWIO_SDC1_SDCC_HC_VS_ADMA_ERR_ADDR0_CMDQ_TASK_DESC_ERROR_BMSK                                        0x80000000
#define HWIO_SDC1_SDCC_HC_VS_ADMA_ERR_ADDR0_CMDQ_TASK_DESC_ERROR_SHFT                                              0x1f
#define HWIO_SDC1_SDCC_HC_VS_ADMA_ERR_ADDR0_ADMA_VALID_ERROR_BMSK                                            0x40000000
#define HWIO_SDC1_SDCC_HC_VS_ADMA_ERR_ADDR0_ADMA_VALID_ERROR_SHFT                                                  0x1e
#define HWIO_SDC1_SDCC_HC_VS_ADMA_ERR_ADDR0_ADMA_RESP_ERROR_BMSK                                             0x20000000
#define HWIO_SDC1_SDCC_HC_VS_ADMA_ERR_ADDR0_ADMA_RESP_ERROR_SHFT                                                   0x1d
#define HWIO_SDC1_SDCC_HC_VS_ADMA_ERR_ADDR0_DATA_RESP_ERROR_BMSK                                             0x10000000
#define HWIO_SDC1_SDCC_HC_VS_ADMA_ERR_ADDR0_DATA_RESP_ERROR_SHFT                                                   0x1c
#define HWIO_SDC1_SDCC_HC_VS_ADMA_ERR_ADDR0_DATA_WR_RESP_ERROR_BMSK                                           0x8000000
#define HWIO_SDC1_SDCC_HC_VS_ADMA_ERR_ADDR0_DATA_WR_RESP_ERROR_SHFT                                                0x1b
#define HWIO_SDC1_SDCC_HC_VS_ADMA_ERR_ADDR0_ADMA_LEN_MISMATCH_BMSK                                            0x4000000
#define HWIO_SDC1_SDCC_HC_VS_ADMA_ERR_ADDR0_ADMA_LEN_MISMATCH_SHFT                                                 0x1a
#define HWIO_SDC1_SDCC_HC_VS_ADMA_ERR_ADDR0_RESP_ADDR_MSB_BMSK                                                0x3ffe000
#define HWIO_SDC1_SDCC_HC_VS_ADMA_ERR_ADDR0_RESP_ADDR_MSB_SHFT                                                      0xd
#define HWIO_SDC1_SDCC_HC_VS_ADMA_ERR_ADDR0_RESP_ADDR_LSB_BMSK                                                   0x1fff
#define HWIO_SDC1_SDCC_HC_VS_ADMA_ERR_ADDR0_RESP_ADDR_LSB_SHFT                                                      0x0

#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_ADDR                                                 (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x0000021c)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_OFFS                                                 (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000021c)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_RMSK                                                 0xf7efffbf
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_POR                                                  0x3029c8b2
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_POR_RMSK                                             0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_ATTR                                                        0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_ADDR, HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_RMSK)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_ADDR, m)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_ADDR,v)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_ADDR,m,v,HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_IN)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_SLOT_TYPE_BMSK                       0xc0000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_SLOT_TYPE_SHFT                             0x1e
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_ASYNC_INT_SUPPORT_BMSK               0x20000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_ASYNC_INT_SUPPORT_SHFT                     0x1d
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_SYS_BUS_SUPPORT_64_BIT_BMSK          0x10000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_SYS_BUS_SUPPORT_64_BIT_SHFT                0x1c
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_VOLTAGE_SUPPORT_1_8V_BMSK             0x4000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_VOLTAGE_SUPPORT_1_8V_SHFT                  0x1a
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_VOLTAGE_SUPPORT_3_0V_BMSK             0x2000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_VOLTAGE_SUPPORT_3_0V_SHFT                  0x19
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_VOLTAGE_SUPPORT_3_3V_BMSK             0x1000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_VOLTAGE_SUPPORT_3_3V_SHFT                  0x18
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_SUSPEND_RESUME_SUPPORT_BMSK            0x800000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_SUSPEND_RESUME_SUPPORT_SHFT                0x17
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_SDMA_SUPPORT_BMSK                      0x400000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_SDMA_SUPPORT_SHFT                          0x16
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_HS_SUPPORT_BMSK                        0x200000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_HS_SUPPORT_SHFT                            0x15
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_ADMA2_SUPPORT_BMSK                      0x80000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_ADMA2_SUPPORT_SHFT                         0x13
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_SUPPORT_8_BIT_BMSK                      0x40000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_SUPPORT_8_BIT_SHFT                         0x12
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_MAX_BLK_LENGTH_BMSK                     0x30000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_MAX_BLK_LENGTH_SHFT                        0x10
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_BASE_SDCLK_FREQ_BMSK                     0xff00
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_BASE_SDCLK_FREQ_SHFT                        0x8
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_TIMEOUT_CLK_UNIT_BMSK                      0x80
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_TIMEOUT_CLK_UNIT_SHFT                       0x7
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_TIMEOUT_CLK_FREQ_BMSK                      0x3f
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_TIMEOUT_CLK_FREQ_SHFT                       0x0

#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_ADDR                                                 (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000220)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_OFFS                                                 (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000220)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_RMSK                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_POR                                                  0x0200808f
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_POR_RMSK                                             0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_ATTR                                                        0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_ADDR, HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_RMSK)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_ADDR, m)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_ADDR,v)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_ADDR,m,v,HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_IN)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_SPEC_VERSION_BMSK                                    0xff000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_SPEC_VERSION_SHFT                                          0x18
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_CLK_MULTIPLIER_BMSK                    0xff0000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_CLK_MULTIPLIER_SHFT                        0x10
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_RETUNING_MODE_BMSK                       0xc000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_RETUNING_MODE_SHFT                          0xe
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_USE_TUNING_FOR_SDR50_BMSK                0x2000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_USE_TUNING_FOR_SDR50_SHFT                   0xd
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_FUSE_ICE_DISABLE_BMSK                    0x1000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_FUSE_ICE_DISABLE_SHFT                       0xc
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_TIMER_CNT_FOR_RETUNING_BMSK               0xf00
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_TIMER_CNT_FOR_RETUNING_SHFT                 0x8
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_CMDQ_SUPPORT_BMSK                          0x80
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_CMDQ_SUPPORT_SHFT                           0x7
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_DRIVER_TYPE_D_SUPPORT_BMSK                 0x40
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_DRIVER_TYPE_D_SUPPORT_SHFT                  0x6
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_DRIVER_TYPE_C_SUPPORT_BMSK                 0x20
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_DRIVER_TYPE_C_SUPPORT_SHFT                  0x5
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_DRIVER_TYPE_A_SUPPORT_BMSK                 0x10
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_DRIVER_TYPE_A_SUPPORT_SHFT                  0x4
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_HS400_SUPPORT_BMSK                          0x8
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_HS400_SUPPORT_SHFT                          0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_DDR_50_SUPPORT_BMSK                         0x4
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_DDR_50_SUPPORT_SHFT                         0x2
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_SDR_104_SUPPORT_BMSK                        0x2
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_SDR_104_SUPPORT_SHFT                        0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_SDR_50_SUPPORT_BMSK                         0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_SDR_50_SUPPORT_SHFT                         0x0

#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_ADDR                                                    (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000224)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_OFFS                                                    (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000224)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_RMSK                                                         0x7ff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_POR                                                     0x00000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_POR_RMSK                                                0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_ATTR                                                           0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_ADDR, HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_RMSK)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_ADDR, m)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_ADDR,v)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_ADDR,m,v,HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_IN)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_FF_CLK_DIS_BMSK                                              0x400
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_FF_CLK_DIS_SHFT                                                0xa
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_VOLTAGE_MUX_SEL_BMSK                                         0x200
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_VOLTAGE_MUX_SEL_SHFT                                           0x9
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_CDC_TRAFFIC_SEL_BMSK                                         0x180
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_CDC_TRAFFIC_SEL_SHFT                                           0x7
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_START_CDC_TRAFFIC_BMSK                                        0x40
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_START_CDC_TRAFFIC_SHFT                                         0x6
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_CRC_TOKEN_SAMPL_FALL_EDGE_BMSK                                0x20
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_CRC_TOKEN_SAMPL_FALL_EDGE_SHFT                                 0x5
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_DATIN_SAMPL_FALL_EDGE_BMSK                                    0x10
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_DATIN_SAMPL_FALL_EDGE_SHFT                                     0x4
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_CDC_T4_TEST_OUT_SEL_BMSK                                       0x8
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_CDC_T4_TEST_OUT_SEL_SHFT                                       0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_CMDIN_EDGE_SEL_BMSK                                            0x4
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_CMDIN_EDGE_SEL_SHFT                                            0x2
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_CMDIN_RCLK_EN_BMSK                                             0x2
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_CMDIN_RCLK_EN_SHFT                                             0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_CDC_T4_DLY_SEL_BMSK                                            0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_CDC_T4_DLY_SEL_SHFT                                            0x0

#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_ADDR                                               (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000228)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_OFFS                                               (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000228)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_RMSK                                                      0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_POR                                                0x00000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_POR_RMSK                                           0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_ATTR                                                      0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_ADDR, HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_RMSK)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_ADDR, m)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_ADDR,v)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_ADDR,m,v,HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_IN)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_CMD_PERIOD_BMSK                                           0x2
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_CMD_PERIOD_SHFT                                           0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_ENABLE_BMSK                                               0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_ENABLE_SHFT                                               0x0

#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FLOW_CTRL_TIME_COUNT_ADDR                                          (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x0000022c)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FLOW_CTRL_TIME_COUNT_OFFS                                          (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000022c)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FLOW_CTRL_TIME_COUNT_RMSK                                          0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FLOW_CTRL_TIME_COUNT_POR                                           0x00000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FLOW_CTRL_TIME_COUNT_POR_RMSK                                      0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FLOW_CTRL_TIME_COUNT_ATTR                                                 0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FLOW_CTRL_TIME_COUNT_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FLOW_CTRL_TIME_COUNT_ADDR, HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FLOW_CTRL_TIME_COUNT_RMSK)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FLOW_CTRL_TIME_COUNT_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FLOW_CTRL_TIME_COUNT_ADDR, m)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FLOW_CTRL_TIME_COUNT_FLOW_CTRL_TIME_BMSK                           0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FLOW_CTRL_TIME_COUNT_FLOW_CTRL_TIME_SHFT                                  0x0

#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_BUSY_TIME_COUNT_ADDR                                               (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000230)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_BUSY_TIME_COUNT_OFFS                                               (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000230)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_BUSY_TIME_COUNT_RMSK                                               0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_BUSY_TIME_COUNT_POR                                                0x00000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_BUSY_TIME_COUNT_POR_RMSK                                           0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_BUSY_TIME_COUNT_ATTR                                                      0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_BUSY_TIME_COUNT_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_BUSY_TIME_COUNT_ADDR, HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_BUSY_TIME_COUNT_RMSK)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_BUSY_TIME_COUNT_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_BUSY_TIME_COUNT_ADDR, m)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_BUSY_TIME_COUNT_BUSY_TIME_BMSK                                     0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_BUSY_TIME_COUNT_BUSY_TIME_SHFT                                            0x0

#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DATA_TIME_COUNT_ADDR                                               (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000234)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DATA_TIME_COUNT_OFFS                                               (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000234)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DATA_TIME_COUNT_RMSK                                               0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DATA_TIME_COUNT_POR                                                0x00000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DATA_TIME_COUNT_POR_RMSK                                           0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DATA_TIME_COUNT_ATTR                                                      0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DATA_TIME_COUNT_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DATA_TIME_COUNT_ADDR, HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DATA_TIME_COUNT_RMSK)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DATA_TIME_COUNT_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DATA_TIME_COUNT_ADDR, m)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DATA_TIME_COUNT_DATA_TIME_BMSK                                     0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DATA_TIME_COUNT_DATA_TIME_SHFT                                            0x0

#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CMD_TIME_COUNT_ADDR                                                (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000238)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CMD_TIME_COUNT_OFFS                                                (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000238)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CMD_TIME_COUNT_RMSK                                                0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CMD_TIME_COUNT_POR                                                 0x00000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CMD_TIME_COUNT_POR_RMSK                                            0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CMD_TIME_COUNT_ATTR                                                       0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CMD_TIME_COUNT_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CMD_TIME_COUNT_ADDR, HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CMD_TIME_COUNT_RMSK)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CMD_TIME_COUNT_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CMD_TIME_COUNT_ADDR, m)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CMD_TIME_COUNT_CMD_TIME_BMSK                                       0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_CMD_TIME_COUNT_CMD_TIME_SHFT                                              0x0

#define HWIO_SDC1_SDCC_SDCC_TEST_MODE_ADDR                                                                   (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x0000023c)
#define HWIO_SDC1_SDCC_SDCC_TEST_MODE_OFFS                                                                   (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000023c)
#define HWIO_SDC1_SDCC_SDCC_TEST_MODE_RMSK                                                                        0x7ff
#define HWIO_SDC1_SDCC_SDCC_TEST_MODE_POR                                                                    0x00000000
#define HWIO_SDC1_SDCC_SDCC_TEST_MODE_POR_RMSK                                                               0xffffffff
#define HWIO_SDC1_SDCC_SDCC_TEST_MODE_ATTR                                                                          0x3
#define HWIO_SDC1_SDCC_SDCC_TEST_MODE_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_SDCC_TEST_MODE_ADDR, HWIO_SDC1_SDCC_SDCC_TEST_MODE_RMSK)
#define HWIO_SDC1_SDCC_SDCC_TEST_MODE_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_SDCC_TEST_MODE_ADDR, m)
#define HWIO_SDC1_SDCC_SDCC_TEST_MODE_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_SDCC_TEST_MODE_ADDR,v)
#define HWIO_SDC1_SDCC_SDCC_TEST_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_SDCC_TEST_MODE_ADDR,m,v,HWIO_SDC1_SDCC_SDCC_TEST_MODE_IN)
#define HWIO_SDC1_SDCC_SDCC_TEST_MODE_TEST_MODE_ENABLE_BMSK                                                       0x400
#define HWIO_SDC1_SDCC_SDCC_TEST_MODE_TEST_MODE_ENABLE_SHFT                                                         0xa
#define HWIO_SDC1_SDCC_SDCC_TEST_MODE_TEST_MODE_CLK_BMSK                                                          0x200
#define HWIO_SDC1_SDCC_SDCC_TEST_MODE_TEST_MODE_CLK_SHFT                                                            0x9
#define HWIO_SDC1_SDCC_SDCC_TEST_MODE_TEST_MODE_CMD_BMSK                                                          0x100
#define HWIO_SDC1_SDCC_SDCC_TEST_MODE_TEST_MODE_CMD_SHFT                                                            0x8
#define HWIO_SDC1_SDCC_SDCC_TEST_MODE_TEST_MODE_DATA_BMSK                                                          0xff
#define HWIO_SDC1_SDCC_SDCC_TEST_MODE_TEST_MODE_DATA_SHFT                                                           0x0

#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_ADDR                                             (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000240)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_OFFS                                             (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000240)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_RMSK                                                    0xf
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_POR                                              0x00000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_POR_RMSK                                         0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_ATTR                                                    0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_ADDR, HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_RMSK)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_ADDR, m)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_IO_HIGH_V_BMSK                                          0x8
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_IO_HIGH_V_SHFT                                          0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_IO_LOW_V_BMSK                                           0x4
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_IO_LOW_V_SHFT                                           0x2
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_BUS_ON_BMSK                                             0x2
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_BUS_ON_SHFT                                             0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_BUS_OFF_BMSK                                            0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_BUS_OFF_SHFT                                            0x0

#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_ADDR                                               (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000244)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_OFFS                                               (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000244)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_RMSK                                                      0xf
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_POR                                                0x00000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_POR_RMSK                                           0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_ATTR                                                      0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_ADDR, HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_RMSK)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_ADDR, m)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_ADDR,v)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_ADDR,m,v,HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_IN)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_IO_HIGH_V_BMSK                                            0x8
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_IO_HIGH_V_SHFT                                            0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_IO_LOW_V_BMSK                                             0x4
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_IO_LOW_V_SHFT                                             0x2
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_BUS_ON_BMSK                                               0x2
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_BUS_ON_SHFT                                               0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_BUS_OFF_BMSK                                              0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_BUS_OFF_SHFT                                              0x0

#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_ADDR                                              (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000248)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_OFFS                                              (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000248)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_RMSK                                                     0xf
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_POR                                               0x00000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_POR_RMSK                                          0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_ATTR                                                     0x2
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_ADDR,v)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_IO_HIGH_V_BMSK                                           0x8
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_IO_HIGH_V_SHFT                                           0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_IO_LOW_V_BMSK                                            0x4
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_IO_LOW_V_SHFT                                            0x2
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_BUS_ON_BMSK                                              0x2
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_BUS_ON_SHFT                                              0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_BUS_OFF_BMSK                                             0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_BUS_OFF_SHFT                                             0x0

#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_ADDR                                                (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x0000024c)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_OFFS                                                (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000024c)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_RMSK                                                     0x3ff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_POR                                                 0x00000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_POR_RMSK                                            0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_ATTR                                                       0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_ADDR, HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_RMSK)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_ADDR, m)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_ADDR,v)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_ADDR,m,v,HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_IN)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_HC_CARD_STATE_STABLE_BMSK                                0x200
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_HC_CARD_STATE_STABLE_SHFT                                  0x9
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_RST_NOT_WAIT_PWRCTL_REG_BMSK                             0x100
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_RST_NOT_WAIT_PWRCTL_REG_SHFT                               0x8
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_HC_WRITE_PROTECT_PAD_BMSK                                 0xc0
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_HC_WRITE_PROTECT_PAD_SHFT                                  0x6
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_CARD_DETECT_PIN_LEVEL_BMSK                                0x20
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_CARD_DETECT_PIN_LEVEL_SHFT                                 0x5
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_HC_WRITE_PROTECT_BMSK                                     0x10
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_HC_WRITE_PROTECT_SHFT                                      0x4
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_IO_SIG_SWITCH_FAIL_BMSK                                    0x8
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_IO_SIG_SWITCH_FAIL_SHFT                                    0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_IO_SIG_SWITCH_SUCCESS_BMSK                                 0x4
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_IO_SIG_SWITCH_SUCCESS_SHFT                                 0x2
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_BUS_ON_OFF_FAIL_BMSK                                       0x2
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_BUS_ON_OFF_FAIL_SHFT                                       0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_BUS_ON_OFF_SUCCESS_BMSK                                    0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_BUS_ON_OFF_SUCCESS_SHFT                                    0x0

#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_ADDR                                                         (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000250)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_OFFS                                                         (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000250)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_RMSK                                                         0xffefffef
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_POR                                                          0x02226040
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_POR_RMSK                                                     0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_ATTR                                                                0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_ADDR, HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_RMSK)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_ADDR, m)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_ADDR,v)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_ADDR,m,v,HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_IN)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_BUSY_CHECK_VALID_PERIOD_BMSK                                 0xc0000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_BUSY_CHECK_VALID_PERIOD_SHFT                                       0x1e
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_BUSY_CHECK_VALID_ALWAYS_BMSK                                 0x20000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_BUSY_CHECK_VALID_ALWAYS_SHFT                                       0x1d
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_ADMA_INT_DATA_BMSK                                           0x10000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_ADMA_INT_DATA_SHFT                                                 0x1c
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_IRQ_PCLK_DIS_BMSK                                             0x8000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_IRQ_PCLK_DIS_SHFT                                                  0x1b
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_CLOCK_AFTER_CMDEND_DIS_BMSK                                   0x4000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_CLOCK_AFTER_CMDEND_DIS_SHFT                                        0x1a
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_SDCC_CLK_EXT_EN_BMSK                                          0x2000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_SDCC_CLK_EXT_EN_SHFT                                               0x19
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_SD_DEV_SEL_BMSK                                               0x1800000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_SD_DEV_SEL_SHFT                                                    0x17
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_SD_CLK_STABLE_BMSK                                             0x400000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_SD_CLK_STABLE_SHFT                                                 0x16
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_TCXO_SW_RST_EN_BMSK                                            0x200000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_TCXO_SW_RST_EN_SHFT                                                0x15
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_HS400_BLK_END_RST_DISABLE_BMSK                                  0x80000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_HS400_BLK_END_RST_DISABLE_SHFT                                     0x13
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_DATEN_HS400_INPUT_MASK_CNT_BMSK                                 0x70000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_DATEN_HS400_INPUT_MASK_CNT_SHFT                                    0x10
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_CMDEN_HS400_INPUT_MASK_CNT_BMSK                                  0xe000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_CMDEN_HS400_INPUT_MASK_CNT_SHFT                                     0xd
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_DATEN_HS400_INPUT_MASK_DIS_BMSK                                  0x1000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_DATEN_HS400_INPUT_MASK_DIS_SHFT                                     0xc
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_CMDEN_HS400_INPUT_MASK_DIS_BMSK                                   0x800
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_CMDEN_HS400_INPUT_MASK_DIS_SHFT                                     0xb
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_HC_FIFO_ALT_ENABLE_BMSK                                           0x400
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_HC_FIFO_ALT_ENABLE_SHFT                                             0xa
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_WIDEBUS_MASK_DISABLE_BMSK                                         0x200
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_WIDEBUS_MASK_DISABLE_SHFT                                           0x9
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_TXFLOWCONTROL_WITH_NO_TX_BMSK                                     0x100
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_TXFLOWCONTROL_WITH_NO_TX_SHFT                                       0x8
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_MCLK_DURING_SW_RST_REQ_DIS_BMSK                                    0x80
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_MCLK_DURING_SW_RST_REQ_DIS_SHFT                                     0x7
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_HS200_ASYNC_FIFO_WR_CLK_EN_BMSK                                    0x40
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_HS200_ASYNC_FIFO_WR_CLK_EN_SHFT                                     0x6
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_IGNORE_START_BIT_ERR_BMSK                                          0x20
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_IGNORE_START_BIT_ERR_SHFT                                           0x5
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_PWRSAVE_DLL_BMSK                                                    0x8
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_PWRSAVE_DLL_SHFT                                                    0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_SDIO_TRANS_BMSK                                                     0x4
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_SDIO_TRANS_SHFT                                                     0x2
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_HCLK_IDLE_GATING_BMSK                                               0x2
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_HCLK_IDLE_GATING_SHFT                                               0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_MCLK_IDLE_GATING_BMSK                                               0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC3_MCLK_IDLE_GATING_SHFT                                               0x0

#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_ADDR                                                              (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000254)
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_OFFS                                                              (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000254)
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_RMSK                                                                0x6ffc0f
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_POR                                                               0x0020a000
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_POR_RMSK                                                          0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_ATTR                                                                     0x3
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_ADDR, HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_ADDR,v)
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_ADDR,m,v,HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_IN)
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_DLL_OUTPUT_TO_PAD_BMSK                                              0x400000
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_DLL_OUTPUT_TO_PAD_SHFT                                                  0x16
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_DLL_CLOCK_DISABLE_BMSK                                              0x200000
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_DLL_CLOCK_DISABLE_SHFT                                                  0x15
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_LOW_FREQ_MODE_BMSK                                                   0x80000
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_LOW_FREQ_MODE_SHFT                                                      0x13
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_FLL_CYCLE_CNT_BMSK                                                   0x40000
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_FLL_CYCLE_CNT_SHFT                                                      0x12
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_MCLK_FREQ_CALC_BMSK                                                  0x3fc00
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_MCLK_FREQ_CALC_SHFT                                                      0xa
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_DDR_TRAFFIC_INIT_SEL_BMSK                                                0xc
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_DDR_TRAFFIC_INIT_SEL_SHFT                                                0x2
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_DDR_TRAFFIC_INIT_SW_BMSK                                                 0x2
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_DDR_TRAFFIC_INIT_SW_SHFT                                                 0x1
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_DDR_CAL_EN_BMSK                                                          0x1
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_2_DDR_CAL_EN_SHFT                                                          0x0

#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_3_ADDR                                                              (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000258)
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_3_OFFS                                                              (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000258)
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_3_RMSK                                                                    0xff
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_3_POR                                                               0x00000000
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_3_POR_RMSK                                                          0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_3_ATTR                                                                     0x3
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_3_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_3_ADDR, HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_3_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_3_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_3_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_3_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_3_ADDR,v)
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_3_ADDR,m,v,HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_3_IN)
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_3_SDC4_CONFIG_MSB_BMSK                                                    0xff
#define HWIO_SDC1_SDCC_HC_REG_DLL_CONFIG_3_SDC4_CONFIG_MSB_SHFT                                                     0x0

#define HWIO_SDC1_SDCC_HC_REG_DDR_CONFIG_ADDR                                                                (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x0000025c)
#define HWIO_SDC1_SDCC_HC_REG_DDR_CONFIG_OFFS                                                                (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000025c)
#define HWIO_SDC1_SDCC_HC_REG_DDR_CONFIG_RMSK                                                                0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_DDR_CONFIG_POR                                                                 0x80040873
#define HWIO_SDC1_SDCC_HC_REG_DDR_CONFIG_POR_RMSK                                                            0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_DDR_CONFIG_ATTR                                                                       0x3
#define HWIO_SDC1_SDCC_HC_REG_DDR_CONFIG_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_DDR_CONFIG_ADDR, HWIO_SDC1_SDCC_HC_REG_DDR_CONFIG_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_DDR_CONFIG_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_DDR_CONFIG_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_DDR_CONFIG_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_REG_DDR_CONFIG_ADDR,v)
#define HWIO_SDC1_SDCC_HC_REG_DDR_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_REG_DDR_CONFIG_ADDR,m,v,HWIO_SDC1_SDCC_HC_REG_DDR_CONFIG_IN)
#define HWIO_SDC1_SDCC_HC_REG_DDR_CONFIG_PRG_DLY_EN_BMSK                                                     0x80000000
#define HWIO_SDC1_SDCC_HC_REG_DDR_CONFIG_PRG_DLY_EN_SHFT                                                           0x1f
#define HWIO_SDC1_SDCC_HC_REG_DDR_CONFIG_EXT_PRG_RCLK_DLY_EN_BMSK                                            0x40000000
#define HWIO_SDC1_SDCC_HC_REG_DDR_CONFIG_EXT_PRG_RCLK_DLY_EN_SHFT                                                  0x1e
#define HWIO_SDC1_SDCC_HC_REG_DDR_CONFIG_EXT_PRG_RCLK_DLY_CODE_BMSK                                          0x38000000
#define HWIO_SDC1_SDCC_HC_REG_DDR_CONFIG_EXT_PRG_RCLK_DLY_CODE_SHFT                                                0x1b
#define HWIO_SDC1_SDCC_HC_REG_DDR_CONFIG_EXT_PRG_RCLK_DLY_BMSK                                                0x7e00000
#define HWIO_SDC1_SDCC_HC_REG_DDR_CONFIG_EXT_PRG_RCLK_DLY_SHFT                                                     0x15
#define HWIO_SDC1_SDCC_HC_REG_DDR_CONFIG_TCXO_CYCLES_DLY_LINE_BMSK                                             0x1ff000
#define HWIO_SDC1_SDCC_HC_REG_DDR_CONFIG_TCXO_CYCLES_DLY_LINE_SHFT                                                  0xc
#define HWIO_SDC1_SDCC_HC_REG_DDR_CONFIG_TCXO_CYCLES_CNT_BMSK                                                     0xe00
#define HWIO_SDC1_SDCC_HC_REG_DDR_CONFIG_TCXO_CYCLES_CNT_SHFT                                                       0x9
#define HWIO_SDC1_SDCC_HC_REG_DDR_CONFIG_PRG_RCLK_DLY_BMSK                                                        0x1ff
#define HWIO_SDC1_SDCC_HC_REG_DDR_CONFIG_PRG_RCLK_DLY_SHFT                                                          0x0

#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_ADDR                                                         (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000260)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_OFFS                                                         (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000260)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_RMSK                                                           0xffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_POR                                                          0x00030022
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_POR_RMSK                                                     0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_ATTR                                                                0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_ADDR, HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_RMSK)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_ADDR, m)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_ADDR,v)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_ADDR,m,v,HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_IN)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_DLL_CDR_EN_OVERRIDE_BMSK                                       0x800000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_DLL_CDR_EN_OVERRIDE_SHFT                                           0x17
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_UARM_PRIORITY_MODE_BMSK                                        0x700000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_UARM_PRIORITY_MODE_SHFT                                            0x14
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_SBFE_UAWM_DIS_BMSK                                              0x80000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_SBFE_UAWM_DIS_SHFT                                                 0x13
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_SBFE_UARM_DIS_BMSK                                              0x40000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_SBFE_UARM_DIS_SHFT                                                 0x12
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_AXI_MAX_BURST_LENGTH_BMSK                                       0x30000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_AXI_MAX_BURST_LENGTH_SHFT                                          0x10
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_DISABLE_CRYPTO_BMSK                                              0x8000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_DISABLE_CRYPTO_SHFT                                                 0xf
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_WRAP_ERROR_BMSK                                                  0x4000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_WRAP_ERROR_SHFT                                                     0xe
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_RX_FLOW_TIMING_BMSK                                              0x2000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_RX_FLOW_TIMING_SHFT                                                 0xd
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_PROGDONE_WO_CMD_RESP_BMSK                                        0x1000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_PROGDONE_WO_CMD_RESP_SHFT                                           0xc
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_HC_AXI_ARLOCK_BMSK                                                0xc00
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_HC_AXI_ARLOCK_SHFT                                                  0xa
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_HC_AXI_AWLOCK_BMSK                                                0x300
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_HC_AXI_AWLOCK_SHFT                                                  0x8
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_HC_AXI_ARCACHE_BMSK                                                0xf0
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_HC_AXI_ARCACHE_SHFT                                                 0x4
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_HC_AXI_AWCACHE_BMSK                                                 0xf
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_FUNC4_HC_AXI_AWCACHE_SHFT                                                 0x0

#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_ADDR(n)                                           (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000264 + 0x4 * (n))
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_OFFS(n)                                           (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000264 + 0x4 * (n))
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_RMSK                                              0x80000001
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_MAXn                                                      31
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_POR                                               0x00000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_POR_RMSK                                          0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_ATTR                                                     0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_INI(n)        \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_ADDR(n), HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_RMSK)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_INMI(n,mask)    \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_ADDR(n), mask)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_OUTI(n,val)    \
        out_dword(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_ADDR(n),val)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_ADDR(n),mask,val,HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_INI(n))
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_HW_EVENT_EN_BMSK                                  0x80000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_HW_EVENT_EN_SHFT                                        0x1f
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_HW_EVENT_SEL_BMSK                                        0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_HW_EVENT_SEL_SHFT                                        0x0

#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_ADDR                                                    (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x000002e4)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_OFFS                                                    (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x000002e4)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_RMSK                                                       0x300ff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_POR                                                     0x00000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_POR_RMSK                                                0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_ATTR                                                           0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_ADDR, HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_RMSK)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_ADDR, m)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_ADDR,v)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_ADDR,m,v,HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_IN)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_HC_AXI_AW_CNTR_EOT_RST_BMSK                                0x20000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_HC_AXI_AW_CNTR_EOT_RST_SHFT                                   0x11
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_HC_AXI_AW_CNTR_SOT_RST_BMSK                                0x10000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_HC_AXI_AW_CNTR_SOT_RST_SHFT                                   0x10
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_HC_AXI_AW_CNTR_BMSK                                           0xff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_HC_AXI_AW_CNTR_SHFT                                            0x0

#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_ADDR                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x000002e8)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_OFFS                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x000002e8)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_RMSK                                                        0x300ff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_POR                                                      0x00000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_POR_RMSK                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_ATTR                                                            0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_ADDR, HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_RMSK)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_ADDR, m)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_ADDR,v)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_ADDR,m,v,HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_IN)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_HC_AXI_W_CNTR_EOT_RST_BMSK                                  0x20000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_HC_AXI_W_CNTR_EOT_RST_SHFT                                     0x11
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_HC_AXI_W_CNTR_SOT_RST_BMSK                                  0x10000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_HC_AXI_W_CNTR_SOT_RST_SHFT                                     0x10
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_HC_AXI_W_CNTR_BMSK                                             0xff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_HC_AXI_W_CNTR_SHFT                                              0x0

#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_ADDR                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x000002ec)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_OFFS                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x000002ec)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_RMSK                                                        0x300ff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_POR                                                      0x00000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_POR_RMSK                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_ATTR                                                            0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_ADDR, HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_RMSK)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_ADDR, m)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_ADDR,v)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_ADDR,m,v,HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_IN)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_HC_AXI_B_CNTR_EOT_RST_BMSK                                  0x20000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_HC_AXI_B_CNTR_EOT_RST_SHFT                                     0x11
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_HC_AXI_B_CNTR_SOT_RST_BMSK                                  0x10000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_HC_AXI_B_CNTR_SOT_RST_SHFT                                     0x10
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_HC_AXI_B_CNTR_BMSK                                             0xff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_HC_AXI_B_CNTR_SHFT                                              0x0

#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_ADDR                                                    (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x000002f0)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_OFFS                                                    (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x000002f0)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_RMSK                                                       0x300ff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_POR                                                     0x00000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_POR_RMSK                                                0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_ATTR                                                           0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_ADDR, HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_RMSK)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_ADDR, m)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_ADDR,v)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_ADDR,m,v,HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_IN)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_HC_AXI_AR_CNTR_EOT_RST_BMSK                                0x20000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_HC_AXI_AR_CNTR_EOT_RST_SHFT                                   0x11
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_HC_AXI_AR_CNTR_SOT_RST_BMSK                                0x10000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_HC_AXI_AR_CNTR_SOT_RST_SHFT                                   0x10
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_HC_AXI_AR_CNTR_BMSK                                           0xff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_HC_AXI_AR_CNTR_SHFT                                            0x0

#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_ADDR                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x000002f4)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_OFFS                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x000002f4)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_RMSK                                                        0x300ff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_POR                                                      0x00000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_POR_RMSK                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_ATTR                                                            0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_ADDR, HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_RMSK)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_ADDR, m)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_ADDR,v)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_ADDR,m,v,HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_IN)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_HC_AXI_R_CNTR_EOT_RST_BMSK                                  0x20000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_HC_AXI_R_CNTR_EOT_RST_SHFT                                     0x11
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_HC_AXI_R_CNTR_SOT_RST_BMSK                                  0x10000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_HC_AXI_R_CNTR_SOT_RST_SHFT                                     0x10
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_HC_AXI_R_CNTR_BMSK                                             0xff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_HC_AXI_R_CNTR_SHFT                                              0x0

#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ADDR                                                       (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x000002f8)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_OFFS                                                       (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x000002f8)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_RMSK                                                         0x3f0001
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_POR                                                        0x00000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_POR_RMSK                                                   0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ATTR                                                              0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ADDR, HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_RMSK)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ADDR, m)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ADDR,v)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ADDR,m,v,HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_IN)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ERR_INT_STS_START_BIT_ERROR_BMSK                             0x200000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ERR_INT_STS_START_BIT_ERROR_SHFT                                 0x15
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ERR_INT_STS_FIFO_FULL_WR_BMSK                                0x100000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ERR_INT_STS_FIFO_FULL_WR_SHFT                                    0x14
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ERR_INT_STS_FIFO_EMPTY_RD_BMSK                                0x80000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ERR_INT_STS_FIFO_EMPTY_RD_SHFT                                   0x13
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ERR_INT_STS_AUTO_CMD19_TIMEOUT_BMSK                           0x40000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ERR_INT_STS_AUTO_CMD19_TIMEOUT_SHFT                              0x12
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ERR_INT_STS_BOOT_TIMEOUT_BMSK                                 0x20000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ERR_INT_STS_BOOT_TIMEOUT_SHFT                                    0x11
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ERR_INT_STS_BOOT_ACK_ERR_BMSK                                 0x10000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ERR_INT_STS_BOOT_ACK_ERR_SHFT                                    0x10
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_NORMAL_INT_STS_BOOT_ACK_REC_BMSK                                  0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_NORMAL_INT_STS_BOOT_ACK_REC_SHFT                                  0x0

#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ADDR                                                    (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x000002fc)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_OFFS                                                    (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x000002fc)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_RMSK                                                      0x3f0001
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_POR                                                     0x00000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_POR_RMSK                                                0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ATTR                                                           0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ADDR, HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_RMSK)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ADDR, m)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ADDR,v)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ADDR,m,v,HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_IN)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ERR_INT_STS_EN_START_BIT_ERROR_BMSK                       0x200000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ERR_INT_STS_EN_START_BIT_ERROR_SHFT                           0x15
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ERR_INT_STS_EN_FIFO_FULL_WR_BMSK                          0x100000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ERR_INT_STS_EN_FIFO_FULL_WR_SHFT                              0x14
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ERR_INT_STS_EN_FIFO_EMPTY_RD_BMSK                          0x80000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ERR_INT_STS_EN_FIFO_EMPTY_RD_SHFT                             0x13
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ERR_INT_STS_EN_AUTO_CMD19_TIMEOUT_BMSK                     0x40000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ERR_INT_STS_EN_AUTO_CMD19_TIMEOUT_SHFT                        0x12
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ERR_INT_STS_EN_BOOT_TIMEOUT_BMSK                           0x20000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ERR_INT_STS_EN_BOOT_TIMEOUT_SHFT                              0x11
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ERR_INT_STS_EN_BOOT_ACK_ERR_BMSK                           0x10000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ERR_INT_STS_EN_BOOT_ACK_ERR_SHFT                              0x10
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_NORMAL_INT_STS_EN_BOOT_ACK_REC_BMSK                            0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_NORMAL_INT_STS_EN_BOOT_ACK_REC_SHFT                            0x0

#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ADDR                                                    (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000300)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_OFFS                                                    (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000300)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_RMSK                                                      0x3f0001
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_POR                                                     0x00000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_POR_RMSK                                                0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ATTR                                                           0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ADDR, HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_RMSK)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ADDR, m)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ADDR,v)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ADDR,m,v,HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_IN)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ERR_INT_SIGNAL_EN_START_BIT_ERROR_BMSK                    0x200000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ERR_INT_SIGNAL_EN_START_BIT_ERROR_SHFT                        0x15
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ERR_INT_SIGNAL_EN_FIFO_FULL_WR_BMSK                       0x100000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ERR_INT_SIGNAL_EN_FIFO_FULL_WR_SHFT                           0x14
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ERR_INT_SIGNAL_EN_FIFO_EMPTY_RD_BMSK                       0x80000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ERR_INT_SIGNAL_EN_FIFO_EMPTY_RD_SHFT                          0x13
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ERR_INT_SIGNAL_EN_AUTO_CMD19_TIMEOUT_BMSK                  0x40000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ERR_INT_SIGNAL_EN_AUTO_CMD19_TIMEOUT_SHFT                     0x12
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ERR_INT_SIGNAL_EN_BOOT_TIMEOUT_BMSK                        0x20000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ERR_INT_SIGNAL_EN_BOOT_TIMEOUT_SHFT                           0x11
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ERR_INT_SIGNAL_EN_BOOT_ACK_ERR_BMSK                        0x10000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ERR_INT_SIGNAL_EN_BOOT_ACK_ERR_SHFT                           0x10
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_NORMAL_INT_SIGNAL_EN_BOOT_ACK_REC_BMSK                         0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_NORMAL_INT_SIGNAL_EN_BOOT_ACK_REC_SHFT                         0x0

#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DEBUG_ADDR                                                         (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000304)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DEBUG_OFFS                                                         (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000304)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DEBUG_RMSK                                                              0x1ff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DEBUG_POR                                                          0x00000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DEBUG_POR_RMSK                                                     0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DEBUG_ATTR                                                                0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DEBUG_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DEBUG_ADDR, HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DEBUG_RMSK)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DEBUG_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DEBUG_ADDR, m)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DEBUG_IO_MACRO_SW_RST_PERIOD_BMSK                                       0x100
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DEBUG_IO_MACRO_SW_RST_PERIOD_SHFT                                         0x8
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DEBUG_MCLK_REG_WR_ACTIVE_BMSK                                            0x80
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DEBUG_MCLK_REG_WR_ACTIVE_SHFT                                             0x7
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DEBUG_PROGDONE_BMSK                                                      0x40
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DEBUG_PROGDONE_SHFT                                                       0x6
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DEBUG_DATA_END_BMSK                                                      0x20
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DEBUG_DATA_END_SHFT                                                       0x5
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DEBUG_CMD_ACTIVE_BMSK                                                    0x10
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DEBUG_CMD_ACTIVE_SHFT                                                     0x4
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DEBUG_RXACTIVE_EXT_BMSK                                                   0x8
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DEBUG_RXACTIVE_EXT_SHFT                                                   0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DEBUG_RXACTIVE_BMSK                                                       0x4
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DEBUG_RXACTIVE_SHFT                                                       0x2
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DEBUG_TXACTIVE_EXT_BMSK                                                   0x2
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DEBUG_TXACTIVE_EXT_SHFT                                                   0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DEBUG_TXACTIVE_BMSK                                                       0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_DEBUG_TXACTIVE_SHFT                                                       0x0

#define HWIO_SDC1_SDCC_FIFO_COUNT_ADDR                                                                       (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000308)
#define HWIO_SDC1_SDCC_FIFO_COUNT_OFFS                                                                       (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000308)
#define HWIO_SDC1_SDCC_FIFO_COUNT_RMSK                                                                        0xfffffff
#define HWIO_SDC1_SDCC_FIFO_COUNT_POR                                                                        0x00000000
#define HWIO_SDC1_SDCC_FIFO_COUNT_POR_RMSK                                                                   0xffffffff
#define HWIO_SDC1_SDCC_FIFO_COUNT_ATTR                                                                              0x1
#define HWIO_SDC1_SDCC_FIFO_COUNT_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_FIFO_COUNT_ADDR, HWIO_SDC1_SDCC_FIFO_COUNT_RMSK)
#define HWIO_SDC1_SDCC_FIFO_COUNT_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_FIFO_COUNT_ADDR, m)
#define HWIO_SDC1_SDCC_FIFO_COUNT_DATA_COUNT_BMSK                                                             0xfffffff
#define HWIO_SDC1_SDCC_FIFO_COUNT_DATA_COUNT_SHFT                                                                   0x0

#define HWIO_SDC1_SDCC_SDCC_BOOT_ADDR                                                                        (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x0000030c)
#define HWIO_SDC1_SDCC_SDCC_BOOT_OFFS                                                                        (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000030c)
#define HWIO_SDC1_SDCC_SDCC_BOOT_RMSK                                                                               0xf
#define HWIO_SDC1_SDCC_SDCC_BOOT_POR                                                                         0x00000000
#define HWIO_SDC1_SDCC_SDCC_BOOT_POR_RMSK                                                                    0xffffffff
#define HWIO_SDC1_SDCC_SDCC_BOOT_ATTR                                                                               0x3
#define HWIO_SDC1_SDCC_SDCC_BOOT_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_SDCC_BOOT_ADDR, HWIO_SDC1_SDCC_SDCC_BOOT_RMSK)
#define HWIO_SDC1_SDCC_SDCC_BOOT_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_SDCC_BOOT_ADDR, m)
#define HWIO_SDC1_SDCC_SDCC_BOOT_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_SDCC_BOOT_ADDR,v)
#define HWIO_SDC1_SDCC_SDCC_BOOT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_SDCC_BOOT_ADDR,m,v,HWIO_SDC1_SDCC_SDCC_BOOT_IN)
#define HWIO_SDC1_SDCC_SDCC_BOOT_EARLY_ASSERT_CMD_LINE_BMSK                                                         0x8
#define HWIO_SDC1_SDCC_SDCC_BOOT_EARLY_ASSERT_CMD_LINE_SHFT                                                         0x3
#define HWIO_SDC1_SDCC_SDCC_BOOT_BOOT_ACK_EN_BMSK                                                                   0x4
#define HWIO_SDC1_SDCC_SDCC_BOOT_BOOT_ACK_EN_SHFT                                                                   0x2
#define HWIO_SDC1_SDCC_SDCC_BOOT_BOOT_EN_BMSK                                                                       0x2
#define HWIO_SDC1_SDCC_SDCC_BOOT_BOOT_EN_SHFT                                                                       0x1
#define HWIO_SDC1_SDCC_SDCC_BOOT_BOOT_MODE_BMSK                                                                     0x1
#define HWIO_SDC1_SDCC_SDCC_BOOT_BOOT_MODE_SHFT                                                                     0x0

#define HWIO_SDC1_SDCC_SDCC_BOOT_ACK_TIMER_ADDR                                                              (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000310)
#define HWIO_SDC1_SDCC_SDCC_BOOT_ACK_TIMER_OFFS                                                              (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000310)
#define HWIO_SDC1_SDCC_SDCC_BOOT_ACK_TIMER_RMSK                                                              0xffffffff
#define HWIO_SDC1_SDCC_SDCC_BOOT_ACK_TIMER_POR                                                               0x00000000
#define HWIO_SDC1_SDCC_SDCC_BOOT_ACK_TIMER_POR_RMSK                                                          0xffffffff
#define HWIO_SDC1_SDCC_SDCC_BOOT_ACK_TIMER_ATTR                                                                     0x3
#define HWIO_SDC1_SDCC_SDCC_BOOT_ACK_TIMER_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_SDCC_BOOT_ACK_TIMER_ADDR, HWIO_SDC1_SDCC_SDCC_BOOT_ACK_TIMER_RMSK)
#define HWIO_SDC1_SDCC_SDCC_BOOT_ACK_TIMER_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_SDCC_BOOT_ACK_TIMER_ADDR, m)
#define HWIO_SDC1_SDCC_SDCC_BOOT_ACK_TIMER_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_SDCC_BOOT_ACK_TIMER_ADDR,v)
#define HWIO_SDC1_SDCC_SDCC_BOOT_ACK_TIMER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_SDCC_BOOT_ACK_TIMER_ADDR,m,v,HWIO_SDC1_SDCC_SDCC_BOOT_ACK_TIMER_IN)
#define HWIO_SDC1_SDCC_SDCC_BOOT_ACK_TIMER_BOOT_ACK_TIMER_BMSK                                               0xffffffff
#define HWIO_SDC1_SDCC_SDCC_BOOT_ACK_TIMER_BOOT_ACK_TIMER_SHFT                                                      0x0

#define HWIO_SDC1_SDCC_SDCC_BOOT_DATA_TIMER_ADDR                                                             (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000314)
#define HWIO_SDC1_SDCC_SDCC_BOOT_DATA_TIMER_OFFS                                                             (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000314)
#define HWIO_SDC1_SDCC_SDCC_BOOT_DATA_TIMER_RMSK                                                             0xffffffff
#define HWIO_SDC1_SDCC_SDCC_BOOT_DATA_TIMER_POR                                                              0x00000000
#define HWIO_SDC1_SDCC_SDCC_BOOT_DATA_TIMER_POR_RMSK                                                         0xffffffff
#define HWIO_SDC1_SDCC_SDCC_BOOT_DATA_TIMER_ATTR                                                                    0x3
#define HWIO_SDC1_SDCC_SDCC_BOOT_DATA_TIMER_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_SDCC_BOOT_DATA_TIMER_ADDR, HWIO_SDC1_SDCC_SDCC_BOOT_DATA_TIMER_RMSK)
#define HWIO_SDC1_SDCC_SDCC_BOOT_DATA_TIMER_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_SDCC_BOOT_DATA_TIMER_ADDR, m)
#define HWIO_SDC1_SDCC_SDCC_BOOT_DATA_TIMER_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_SDCC_BOOT_DATA_TIMER_ADDR,v)
#define HWIO_SDC1_SDCC_SDCC_BOOT_DATA_TIMER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_SDCC_BOOT_DATA_TIMER_ADDR,m,v,HWIO_SDC1_SDCC_SDCC_BOOT_DATA_TIMER_IN)
#define HWIO_SDC1_SDCC_SDCC_BOOT_DATA_TIMER_BOOT_DATA_TIMER_BMSK                                             0xffffffff
#define HWIO_SDC1_SDCC_SDCC_BOOT_DATA_TIMER_BOOT_DATA_TIMER_SHFT                                                    0x0

#define HWIO_SDC1_SDCC_SDCC_VERSION_ADDR                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000318)
#define HWIO_SDC1_SDCC_SDCC_VERSION_OFFS                                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000318)
#define HWIO_SDC1_SDCC_SDCC_VERSION_RMSK                                                                     0xffffffff
#define HWIO_SDC1_SDCC_SDCC_VERSION_POR                                                                      0x10000072
#define HWIO_SDC1_SDCC_SDCC_VERSION_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC1_SDCC_SDCC_VERSION_ATTR                                                                            0x1
#define HWIO_SDC1_SDCC_SDCC_VERSION_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_SDCC_VERSION_ADDR, HWIO_SDC1_SDCC_SDCC_VERSION_RMSK)
#define HWIO_SDC1_SDCC_SDCC_VERSION_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_SDCC_VERSION_ADDR, m)
#define HWIO_SDC1_SDCC_SDCC_VERSION_SDCC_VERSION_BMSK                                                        0xffffffff
#define HWIO_SDC1_SDCC_SDCC_VERSION_SDCC_VERSION_SHFT                                                               0x0

#define HWIO_SDC1_SDCC_SDCC_GENERICS_ADDR                                                                    (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000320)
#define HWIO_SDC1_SDCC_SDCC_GENERICS_OFFS                                                                    (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000320)
#define HWIO_SDC1_SDCC_SDCC_GENERICS_RMSK                                                                     0x7ffffff
#define HWIO_SDC1_SDCC_SDCC_GENERICS_POR                                                                     0x00000000
#define HWIO_SDC1_SDCC_SDCC_GENERICS_POR_RMSK                                                                0xffffffff
#define HWIO_SDC1_SDCC_SDCC_GENERICS_ATTR                                                                           0x1
#define HWIO_SDC1_SDCC_SDCC_GENERICS_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_SDCC_GENERICS_ADDR, HWIO_SDC1_SDCC_SDCC_GENERICS_RMSK)
#define HWIO_SDC1_SDCC_SDCC_GENERICS_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_SDCC_GENERICS_ADDR, m)
#define HWIO_SDC1_SDCC_SDCC_GENERICS_USE_SCM_BMSK                                                             0x4000000
#define HWIO_SDC1_SDCC_SDCC_GENERICS_USE_SCM_SHFT                                                                  0x1a
#define HWIO_SDC1_SDCC_SDCC_GENERICS_USE_XPU_BMSK                                                             0x2000000
#define HWIO_SDC1_SDCC_SDCC_GENERICS_USE_XPU_SHFT                                                                  0x19
#define HWIO_SDC1_SDCC_SDCC_GENERICS_USE_VMIDMT_BMSK                                                          0x1000000
#define HWIO_SDC1_SDCC_SDCC_GENERICS_USE_VMIDMT_SHFT                                                               0x18
#define HWIO_SDC1_SDCC_SDCC_GENERICS_USE_CMD_QUE_BMSK                                                          0x800000
#define HWIO_SDC1_SDCC_SDCC_GENERICS_USE_CMD_QUE_SHFT                                                              0x17
#define HWIO_SDC1_SDCC_SDCC_GENERICS_HS400_SUPPORT_BMSK                                                        0x400000
#define HWIO_SDC1_SDCC_SDCC_GENERICS_HS400_SUPPORT_SHFT                                                            0x16
#define HWIO_SDC1_SDCC_SDCC_GENERICS_USE_ICE_BMSK                                                              0x200000
#define HWIO_SDC1_SDCC_SDCC_GENERICS_USE_ICE_SHFT                                                                  0x15
#define HWIO_SDC1_SDCC_SDCC_GENERICS_SD_DATA_WIDTH_BMSK                                                        0x1e0000
#define HWIO_SDC1_SDCC_SDCC_GENERICS_SD_DATA_WIDTH_SHFT                                                            0x11
#define HWIO_SDC1_SDCC_SDCC_GENERICS_RAM_SIZE_BMSK                                                              0x1fff0
#define HWIO_SDC1_SDCC_SDCC_GENERICS_RAM_SIZE_SHFT                                                                  0x4
#define HWIO_SDC1_SDCC_SDCC_GENERICS_NUM_OF_DEV_BMSK                                                                0xe
#define HWIO_SDC1_SDCC_SDCC_GENERICS_NUM_OF_DEV_SHFT                                                                0x1
#define HWIO_SDC1_SDCC_SDCC_GENERICS_USE_DLL_SDC4_BMSK                                                              0x1
#define HWIO_SDC1_SDCC_SDCC_GENERICS_USE_DLL_SDC4_SHFT                                                              0x0

#define HWIO_SDC1_SDCC_FIFO_STATUS_ADDR                                                                      (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000324)
#define HWIO_SDC1_SDCC_FIFO_STATUS_OFFS                                                                      (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000324)
#define HWIO_SDC1_SDCC_FIFO_STATUS_RMSK                                                                         0x7ffff
#define HWIO_SDC1_SDCC_FIFO_STATUS_POR                                                                       0x00007800
#define HWIO_SDC1_SDCC_FIFO_STATUS_POR_RMSK                                                                  0xffffffff
#define HWIO_SDC1_SDCC_FIFO_STATUS_ATTR                                                                             0x1
#define HWIO_SDC1_SDCC_FIFO_STATUS_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_FIFO_STATUS_ADDR, HWIO_SDC1_SDCC_FIFO_STATUS_RMSK)
#define HWIO_SDC1_SDCC_FIFO_STATUS_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_FIFO_STATUS_ADDR, m)
#define HWIO_SDC1_SDCC_FIFO_STATUS_RX_FIFO_512_BMSK                                                             0x40000
#define HWIO_SDC1_SDCC_FIFO_STATUS_RX_FIFO_512_SHFT                                                                0x12
#define HWIO_SDC1_SDCC_FIFO_STATUS_RX_FIFO_256_BMSK                                                             0x20000
#define HWIO_SDC1_SDCC_FIFO_STATUS_RX_FIFO_256_SHFT                                                                0x11
#define HWIO_SDC1_SDCC_FIFO_STATUS_RX_FIFO_128_BMSK                                                             0x10000
#define HWIO_SDC1_SDCC_FIFO_STATUS_RX_FIFO_128_SHFT                                                                0x10
#define HWIO_SDC1_SDCC_FIFO_STATUS_RX_FIFO_64_BMSK                                                               0x8000
#define HWIO_SDC1_SDCC_FIFO_STATUS_RX_FIFO_64_SHFT                                                                  0xf
#define HWIO_SDC1_SDCC_FIFO_STATUS_TX_FIFO_512_BMSK                                                              0x4000
#define HWIO_SDC1_SDCC_FIFO_STATUS_TX_FIFO_512_SHFT                                                                 0xe
#define HWIO_SDC1_SDCC_FIFO_STATUS_TX_FIFO_256_BMSK                                                              0x2000
#define HWIO_SDC1_SDCC_FIFO_STATUS_TX_FIFO_256_SHFT                                                                 0xd
#define HWIO_SDC1_SDCC_FIFO_STATUS_TX_FIFO_128_BMSK                                                              0x1000
#define HWIO_SDC1_SDCC_FIFO_STATUS_TX_FIFO_128_SHFT                                                                 0xc
#define HWIO_SDC1_SDCC_FIFO_STATUS_TX_FIFO_64_BMSK                                                                0x800
#define HWIO_SDC1_SDCC_FIFO_STATUS_TX_FIFO_64_SHFT                                                                  0xb
#define HWIO_SDC1_SDCC_FIFO_STATUS_FIFO_FILL_LEVEL_BMSK                                                           0x7ff
#define HWIO_SDC1_SDCC_FIFO_STATUS_FIFO_FILL_LEVEL_SHFT                                                             0x0

#define HWIO_SDC1_SDCC_IP_CATALOG_ADDR                                                                       (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000328)
#define HWIO_SDC1_SDCC_IP_CATALOG_OFFS                                                                       (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000328)
#define HWIO_SDC1_SDCC_IP_CATALOG_RMSK                                                                       0xffffffff
#define HWIO_SDC1_SDCC_IP_CATALOG_POR                                                                        0x50010000
#define HWIO_SDC1_SDCC_IP_CATALOG_POR_RMSK                                                                   0xffffffff
#define HWIO_SDC1_SDCC_IP_CATALOG_ATTR                                                                              0x1
#define HWIO_SDC1_SDCC_IP_CATALOG_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_IP_CATALOG_ADDR, HWIO_SDC1_SDCC_IP_CATALOG_RMSK)
#define HWIO_SDC1_SDCC_IP_CATALOG_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_IP_CATALOG_ADDR, m)
#define HWIO_SDC1_SDCC_IP_CATALOG_MAJOR_BMSK                                                                 0xf0000000
#define HWIO_SDC1_SDCC_IP_CATALOG_MAJOR_SHFT                                                                       0x1c
#define HWIO_SDC1_SDCC_IP_CATALOG_MINOR_BMSK                                                                  0xfff0000
#define HWIO_SDC1_SDCC_IP_CATALOG_MINOR_SHFT                                                                       0x10
#define HWIO_SDC1_SDCC_IP_CATALOG_STEP_BMSK                                                                      0xffff
#define HWIO_SDC1_SDCC_IP_CATALOG_STEP_SHFT                                                                         0x0

#define HWIO_SDC1_SDCC_TESTBUS_CONFIG_ADDR                                                                   (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x0000032c)
#define HWIO_SDC1_SDCC_TESTBUS_CONFIG_OFFS                                                                   (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000032c)
#define HWIO_SDC1_SDCC_TESTBUS_CONFIG_RMSK                                                                   0xc000ffff
#define HWIO_SDC1_SDCC_TESTBUS_CONFIG_POR                                                                    0x00000000
#define HWIO_SDC1_SDCC_TESTBUS_CONFIG_POR_RMSK                                                               0xffffffff
#define HWIO_SDC1_SDCC_TESTBUS_CONFIG_ATTR                                                                          0x3
#define HWIO_SDC1_SDCC_TESTBUS_CONFIG_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_TESTBUS_CONFIG_ADDR, HWIO_SDC1_SDCC_TESTBUS_CONFIG_RMSK)
#define HWIO_SDC1_SDCC_TESTBUS_CONFIG_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_TESTBUS_CONFIG_ADDR, m)
#define HWIO_SDC1_SDCC_TESTBUS_CONFIG_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_TESTBUS_CONFIG_ADDR,v)
#define HWIO_SDC1_SDCC_TESTBUS_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_TESTBUS_CONFIG_ADDR,m,v,HWIO_SDC1_SDCC_TESTBUS_CONFIG_IN)
#define HWIO_SDC1_SDCC_TESTBUS_CONFIG_TESTBUS_ENA_BMSK                                                       0x80000000
#define HWIO_SDC1_SDCC_TESTBUS_CONFIG_TESTBUS_ENA_SHFT                                                             0x1f
#define HWIO_SDC1_SDCC_TESTBUS_CONFIG_SW_EVENTS_EN_BMSK                                                      0x40000000
#define HWIO_SDC1_SDCC_TESTBUS_CONFIG_SW_EVENTS_EN_SHFT                                                            0x1e
#define HWIO_SDC1_SDCC_TESTBUS_CONFIG_TESTBUS_SEL_BMSK                                                           0xffff
#define HWIO_SDC1_SDCC_TESTBUS_CONFIG_TESTBUS_SEL_SHFT                                                              0x0

#define HWIO_SDC1_SDCC_CHAR_CFG_ADDR                                                                         (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000330)
#define HWIO_SDC1_SDCC_CHAR_CFG_OFFS                                                                         (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000330)
#define HWIO_SDC1_SDCC_CHAR_CFG_RMSK                                                                             0xff7f
#define HWIO_SDC1_SDCC_CHAR_CFG_POR                                                                          0x00000000
#define HWIO_SDC1_SDCC_CHAR_CFG_POR_RMSK                                                                     0xffffffff
#define HWIO_SDC1_SDCC_CHAR_CFG_ATTR                                                                                0x3
#define HWIO_SDC1_SDCC_CHAR_CFG_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_CHAR_CFG_ADDR, HWIO_SDC1_SDCC_CHAR_CFG_RMSK)
#define HWIO_SDC1_SDCC_CHAR_CFG_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_CHAR_CFG_ADDR, m)
#define HWIO_SDC1_SDCC_CHAR_CFG_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_CHAR_CFG_ADDR,v)
#define HWIO_SDC1_SDCC_CHAR_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_CHAR_CFG_ADDR,m,v,HWIO_SDC1_SDCC_CHAR_CFG_IN)
#define HWIO_SDC1_SDCC_CHAR_CFG_CHAR_MODE_BMSK                                                                   0xf000
#define HWIO_SDC1_SDCC_CHAR_CFG_CHAR_MODE_SHFT                                                                      0xc
#define HWIO_SDC1_SDCC_CHAR_CFG_CHAR_STATUS_BMSK                                                                  0xf00
#define HWIO_SDC1_SDCC_CHAR_CFG_CHAR_STATUS_SHFT                                                                    0x8
#define HWIO_SDC1_SDCC_CHAR_CFG_DLL_INV_CLK_BMSK                                                                   0x40
#define HWIO_SDC1_SDCC_CHAR_CFG_DLL_INV_CLK_SHFT                                                                    0x6
#define HWIO_SDC1_SDCC_CHAR_CFG_DIRECTION_BMSK                                                                     0x20
#define HWIO_SDC1_SDCC_CHAR_CFG_DIRECTION_SHFT                                                                      0x5
#define HWIO_SDC1_SDCC_CHAR_CFG_LOOPBACK_TEST_STATUS_BMSK                                                          0x10
#define HWIO_SDC1_SDCC_CHAR_CFG_LOOPBACK_TEST_STATUS_SHFT                                                           0x4
#define HWIO_SDC1_SDCC_CHAR_CFG_LOOPBACK_TEST_DONE_BMSK                                                             0x8
#define HWIO_SDC1_SDCC_CHAR_CFG_LOOPBACK_TEST_DONE_SHFT                                                             0x3
#define HWIO_SDC1_SDCC_CHAR_CFG_LOOPBACK_MODE_EN_BMSK                                                               0x4
#define HWIO_SDC1_SDCC_CHAR_CFG_LOOPBACK_MODE_EN_SHFT                                                               0x2
#define HWIO_SDC1_SDCC_CHAR_CFG_CHAR_TEST_START_BMSK                                                                0x2
#define HWIO_SDC1_SDCC_CHAR_CFG_CHAR_TEST_START_SHFT                                                                0x1
#define HWIO_SDC1_SDCC_CHAR_CFG_CHAR_TEST_MODE_EN_BMSK                                                              0x1
#define HWIO_SDC1_SDCC_CHAR_CFG_CHAR_TEST_MODE_EN_SHFT                                                              0x0

#define HWIO_SDC1_SDCC_CHAR_CMD_ADDR                                                                         (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000334)
#define HWIO_SDC1_SDCC_CHAR_CMD_OFFS                                                                         (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000334)
#define HWIO_SDC1_SDCC_CHAR_CMD_RMSK                                                                             0xffff
#define HWIO_SDC1_SDCC_CHAR_CMD_POR                                                                          0x00000000
#define HWIO_SDC1_SDCC_CHAR_CMD_POR_RMSK                                                                     0xffffffff
#define HWIO_SDC1_SDCC_CHAR_CMD_ATTR                                                                                0x3
#define HWIO_SDC1_SDCC_CHAR_CMD_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_CHAR_CMD_ADDR, HWIO_SDC1_SDCC_CHAR_CMD_RMSK)
#define HWIO_SDC1_SDCC_CHAR_CMD_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_CHAR_CMD_ADDR, m)
#define HWIO_SDC1_SDCC_CHAR_CMD_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_CHAR_CMD_ADDR,v)
#define HWIO_SDC1_SDCC_CHAR_CMD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_CHAR_CMD_ADDR,m,v,HWIO_SDC1_SDCC_CHAR_CMD_IN)
#define HWIO_SDC1_SDCC_CHAR_CMD_CMDIN_ACTUAL_BMSK                                                                0xff00
#define HWIO_SDC1_SDCC_CHAR_CMD_CMDIN_ACTUAL_SHFT                                                                   0x8
#define HWIO_SDC1_SDCC_CHAR_CMD_CMDOUT_DATA_DIN_EXP_BMSK                                                           0xff
#define HWIO_SDC1_SDCC_CHAR_CMD_CMDOUT_DATA_DIN_EXP_SHFT                                                            0x0

#define HWIO_SDC1_SDCC_CHAR_DATA_n_ADDR(n)                                                                   (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000338 + 0x4 * (n))
#define HWIO_SDC1_SDCC_CHAR_DATA_n_OFFS(n)                                                                   (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000338 + 0x4 * (n))
#define HWIO_SDC1_SDCC_CHAR_DATA_n_RMSK                                                                          0xffff
#define HWIO_SDC1_SDCC_CHAR_DATA_n_MAXn                                                                               7
#define HWIO_SDC1_SDCC_CHAR_DATA_n_POR                                                                       0x00000000
#define HWIO_SDC1_SDCC_CHAR_DATA_n_POR_RMSK                                                                  0xffffffff
#define HWIO_SDC1_SDCC_CHAR_DATA_n_ATTR                                                                             0x3
#define HWIO_SDC1_SDCC_CHAR_DATA_n_INI(n)        \
        in_dword_masked(HWIO_SDC1_SDCC_CHAR_DATA_n_ADDR(n), HWIO_SDC1_SDCC_CHAR_DATA_n_RMSK)
#define HWIO_SDC1_SDCC_CHAR_DATA_n_INMI(n,mask)    \
        in_dword_masked(HWIO_SDC1_SDCC_CHAR_DATA_n_ADDR(n), mask)
#define HWIO_SDC1_SDCC_CHAR_DATA_n_OUTI(n,val)    \
        out_dword(HWIO_SDC1_SDCC_CHAR_DATA_n_ADDR(n),val)
#define HWIO_SDC1_SDCC_CHAR_DATA_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_CHAR_DATA_n_ADDR(n),mask,val,HWIO_SDC1_SDCC_CHAR_DATA_n_INI(n))
#define HWIO_SDC1_SDCC_CHAR_DATA_n_DIN_ACTUAL_BMSK                                                               0xff00
#define HWIO_SDC1_SDCC_CHAR_DATA_n_DIN_ACTUAL_SHFT                                                                  0x8
#define HWIO_SDC1_SDCC_CHAR_DATA_n_DOUT_DATA_DIN_EXP_BMSK                                                          0xff
#define HWIO_SDC1_SDCC_CHAR_DATA_n_DOUT_DATA_DIN_EXP_SHFT                                                           0x0

#define HWIO_SDC1_SDCC_DEBUG_REG_ADDR                                                                        (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000358)
#define HWIO_SDC1_SDCC_DEBUG_REG_OFFS                                                                        (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000358)
#define HWIO_SDC1_SDCC_DEBUG_REG_RMSK                                                                        0xffffffff
#define HWIO_SDC1_SDCC_DEBUG_REG_POR                                                                         0x00000000
#define HWIO_SDC1_SDCC_DEBUG_REG_POR_RMSK                                                                    0xffffffff
#define HWIO_SDC1_SDCC_DEBUG_REG_ATTR                                                                               0x1
#define HWIO_SDC1_SDCC_DEBUG_REG_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_DEBUG_REG_ADDR, HWIO_SDC1_SDCC_DEBUG_REG_RMSK)
#define HWIO_SDC1_SDCC_DEBUG_REG_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_DEBUG_REG_ADDR, m)
#define HWIO_SDC1_SDCC_DEBUG_REG_TEST_BUS_BMSK                                                               0xffffffff
#define HWIO_SDC1_SDCC_DEBUG_REG_TEST_BUS_SHFT                                                                      0x0

#define HWIO_SDC1_SDCC_DATA_COUNT_ADDR                                                                       (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x0000035c)
#define HWIO_SDC1_SDCC_DATA_COUNT_OFFS                                                                       (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000035c)
#define HWIO_SDC1_SDCC_DATA_COUNT_RMSK                                                                        0xfffffff
#define HWIO_SDC1_SDCC_DATA_COUNT_POR                                                                        0x00000000
#define HWIO_SDC1_SDCC_DATA_COUNT_POR_RMSK                                                                   0xffffffff
#define HWIO_SDC1_SDCC_DATA_COUNT_ATTR                                                                              0x1
#define HWIO_SDC1_SDCC_DATA_COUNT_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_DATA_COUNT_ADDR, HWIO_SDC1_SDCC_DATA_COUNT_RMSK)
#define HWIO_SDC1_SDCC_DATA_COUNT_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_DATA_COUNT_ADDR, m)
#define HWIO_SDC1_SDCC_DATA_COUNT_DATACOUNT_BMSK                                                              0xfffffff
#define HWIO_SDC1_SDCC_DATA_COUNT_DATACOUNT_SHFT                                                                    0x0

#define HWIO_SDC1_SDCC_RAM_DATA_OUT_LSB_ADDR                                                                 (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000360)
#define HWIO_SDC1_SDCC_RAM_DATA_OUT_LSB_OFFS                                                                 (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000360)
#define HWIO_SDC1_SDCC_RAM_DATA_OUT_LSB_RMSK                                                                 0xffffffff
#define HWIO_SDC1_SDCC_RAM_DATA_OUT_LSB_POR                                                                  0x00000000
#define HWIO_SDC1_SDCC_RAM_DATA_OUT_LSB_POR_RMSK                                                             0xffffffff
#define HWIO_SDC1_SDCC_RAM_DATA_OUT_LSB_ATTR                                                                        0x1
#define HWIO_SDC1_SDCC_RAM_DATA_OUT_LSB_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_RAM_DATA_OUT_LSB_ADDR, HWIO_SDC1_SDCC_RAM_DATA_OUT_LSB_RMSK)
#define HWIO_SDC1_SDCC_RAM_DATA_OUT_LSB_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_RAM_DATA_OUT_LSB_ADDR, m)
#define HWIO_SDC1_SDCC_RAM_DATA_OUT_LSB_RAM_DATA_BMSK                                                        0xffffffff
#define HWIO_SDC1_SDCC_RAM_DATA_OUT_LSB_RAM_DATA_SHFT                                                               0x0

#define HWIO_SDC1_SDCC_RAM_DATA_OUT_MSB_ADDR                                                                 (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000364)
#define HWIO_SDC1_SDCC_RAM_DATA_OUT_MSB_OFFS                                                                 (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000364)
#define HWIO_SDC1_SDCC_RAM_DATA_OUT_MSB_RMSK                                                                 0xffffffff
#define HWIO_SDC1_SDCC_RAM_DATA_OUT_MSB_POR                                                                  0x00000000
#define HWIO_SDC1_SDCC_RAM_DATA_OUT_MSB_POR_RMSK                                                             0xffffffff
#define HWIO_SDC1_SDCC_RAM_DATA_OUT_MSB_ATTR                                                                        0x1
#define HWIO_SDC1_SDCC_RAM_DATA_OUT_MSB_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_RAM_DATA_OUT_MSB_ADDR, HWIO_SDC1_SDCC_RAM_DATA_OUT_MSB_RMSK)
#define HWIO_SDC1_SDCC_RAM_DATA_OUT_MSB_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_RAM_DATA_OUT_MSB_ADDR, m)
#define HWIO_SDC1_SDCC_RAM_DATA_OUT_MSB_RAM_DATA_BMSK                                                        0xffffffff
#define HWIO_SDC1_SDCC_RAM_DATA_OUT_MSB_RAM_DATA_SHFT                                                               0x0

#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_ADDR_ADDR                                                            (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000368)
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_ADDR_OFFS                                                            (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000368)
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_ADDR_RMSK                                                            0xffffffff
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_ADDR_POR                                                             0x00000000
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_ADDR_POR_RMSK                                                        0xffffffff
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_ADDR_ATTR                                                                   0x1
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_ADDR_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_ADDR_ADDR, HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_ADDR_RMSK)
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_ADDR_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_ADDR_ADDR, m)
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_ADDR_DESC_ADDRESS_LSB_BMSK                                           0xffffffff
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_ADDR_DESC_ADDRESS_LSB_SHFT                                                  0x0

#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_INFO_ADDR                                                            (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x0000036c)
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_INFO_OFFS                                                            (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000036c)
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_INFO_RMSK                                                            0xf33fffff
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_INFO_POR                                                             0x00000000
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_INFO_POR_RMSK                                                        0xffffffff
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_INFO_ATTR                                                                   0x1
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_INFO_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_INFO_ADDR, HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_INFO_RMSK)
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_INFO_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_INFO_ADDR, m)
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_ADDRESS_MSB_BMSK                                           0xf0000000
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_ADDRESS_MSB_SHFT                                                 0x1c
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_CNT_BMSK                                                    0x3000000
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_CNT_SHFT                                                         0x18
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_MAX_LENGTH_BMSK                                              0x200000
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_MAX_LENGTH_SHFT                                                  0x15
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_ACT_BMSK                                                     0x180000
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_ACT_SHFT                                                         0x13
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_INT_BMSK                                                      0x40000
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_INT_SHFT                                                         0x12
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_EOT_BMSK                                                      0x20000
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_EOT_SHFT                                                         0x11
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_VALID_BMSK                                                    0x10000
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_VALID_SHFT                                                       0x10
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_LENGTH_BMSK                                                    0xffff
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_LENGTH_SHFT                                                       0x0

#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_ADDR_ADDR                                                            (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000370)
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_ADDR_OFFS                                                            (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000370)
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_ADDR_RMSK                                                            0xffffffff
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_ADDR_POR                                                             0x00000000
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_ADDR_POR_RMSK                                                        0xffffffff
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_ADDR_ATTR                                                                   0x1
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_ADDR_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_ADDR_ADDR, HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_ADDR_RMSK)
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_ADDR_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_ADDR_ADDR, m)
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_ADDR_DESC_ADDRESS_LSB_BMSK                                           0xffffffff
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_ADDR_DESC_ADDRESS_LSB_SHFT                                                  0x0

#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_INFO_ADDR                                                            (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000374)
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_INFO_OFFS                                                            (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000374)
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_INFO_RMSK                                                            0xf33fffff
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_INFO_POR                                                             0x00000000
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_INFO_POR_RMSK                                                        0xffffffff
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_INFO_ATTR                                                                   0x1
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_INFO_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_INFO_ADDR, HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_INFO_RMSK)
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_INFO_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_INFO_ADDR, m)
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_ADDRESS_MSB_BMSK                                           0xf0000000
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_ADDRESS_MSB_SHFT                                                 0x1c
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_CNT_BMSK                                                    0x3000000
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_CNT_SHFT                                                         0x18
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_MAX_LENGTH_BMSK                                              0x200000
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_MAX_LENGTH_SHFT                                                  0x15
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_ACT_BMSK                                                     0x180000
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_ACT_SHFT                                                         0x13
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_INT_BMSK                                                      0x40000
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_INT_SHFT                                                         0x12
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_EOT_BMSK                                                      0x20000
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_EOT_SHFT                                                         0x11
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_VALID_BMSK                                                    0x10000
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_VALID_SHFT                                                       0x10
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_LENGTH_BMSK                                                    0xffff
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_LENGTH_SHFT                                                       0x0

#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_ADDR_ADDR                                                            (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000378)
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_ADDR_OFFS                                                            (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000378)
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_ADDR_RMSK                                                            0xffffffff
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_ADDR_POR                                                             0x00000000
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_ADDR_POR_RMSK                                                        0xffffffff
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_ADDR_ATTR                                                                   0x1
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_ADDR_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_ADDR_ADDR, HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_ADDR_RMSK)
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_ADDR_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_ADDR_ADDR, m)
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_ADDR_DESC_ADDRESS_LSB_BMSK                                           0xffffffff
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_ADDR_DESC_ADDRESS_LSB_SHFT                                                  0x0

#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_INFO_ADDR                                                            (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x0000037c)
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_INFO_OFFS                                                            (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000037c)
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_INFO_RMSK                                                            0xf33fffff
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_INFO_POR                                                             0x00000000
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_INFO_POR_RMSK                                                        0xffffffff
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_INFO_ATTR                                                                   0x1
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_INFO_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_INFO_ADDR, HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_INFO_RMSK)
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_INFO_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_INFO_ADDR, m)
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_ADDRESS_MSB_BMSK                                           0xf0000000
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_ADDRESS_MSB_SHFT                                                 0x1c
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_CNT_BMSK                                                    0x3000000
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_CNT_SHFT                                                         0x18
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_MAX_LENGTH_BMSK                                              0x200000
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_MAX_LENGTH_SHFT                                                  0x15
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_ACT_BMSK                                                     0x180000
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_ACT_SHFT                                                         0x13
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_INT_BMSK                                                      0x40000
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_INT_SHFT                                                         0x12
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_EOT_BMSK                                                      0x20000
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_EOT_SHFT                                                         0x11
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_VALID_BMSK                                                    0x10000
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_VALID_SHFT                                                       0x10
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_LENGTH_BMSK                                                    0xffff
#define HWIO_SDC1_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_LENGTH_SHFT                                                       0x0

#define HWIO_SDC1_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_ADDR                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000380)
#define HWIO_SDC1_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_OFFS                                                     (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000380)
#define HWIO_SDC1_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_RMSK                                                     0xbfffffff
#define HWIO_SDC1_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_POR                                                      0x00000000
#define HWIO_SDC1_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_POR_RMSK                                                 0xffffffff
#define HWIO_SDC1_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_ATTR                                                            0x3
#define HWIO_SDC1_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_ADDR, HWIO_SDC1_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_RMSK)
#define HWIO_SDC1_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_ADDR, m)
#define HWIO_SDC1_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_ADDR,v)
#define HWIO_SDC1_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_ADDR,m,v,HWIO_SDC1_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_IN)
#define HWIO_SDC1_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_TIMER_EN_BMSK                                            0x80000000
#define HWIO_SDC1_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_TIMER_EN_SHFT                                                  0x1f
#define HWIO_SDC1_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_DATA_PRESENT_BMSK                                        0x20000000
#define HWIO_SDC1_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_DATA_PRESENT_SHFT                                              0x1d
#define HWIO_SDC1_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_TIMER_RST_BMSK                                           0x10000000
#define HWIO_SDC1_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_TIMER_RST_SHFT                                                 0x1c
#define HWIO_SDC1_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_TIMER_STOP_BMSK                                           0x8000000
#define HWIO_SDC1_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_TIMER_STOP_SHFT                                                0x1b
#define HWIO_SDC1_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_TIMER_VALUE_BMSK                                          0x7ffffff
#define HWIO_SDC1_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_TIMER_VALUE_SHFT                                                0x0

#define HWIO_SDC1_SDCC_SDCC_RAM_ADDR_SW_CTL_ADDR                                                             (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000384)
#define HWIO_SDC1_SDCC_SDCC_RAM_ADDR_SW_CTL_OFFS                                                             (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000384)
#define HWIO_SDC1_SDCC_SDCC_RAM_ADDR_SW_CTL_RMSK                                                             0xc07fffff
#define HWIO_SDC1_SDCC_SDCC_RAM_ADDR_SW_CTL_POR                                                              0x00000000
#define HWIO_SDC1_SDCC_SDCC_RAM_ADDR_SW_CTL_POR_RMSK                                                         0xffffffff
#define HWIO_SDC1_SDCC_SDCC_RAM_ADDR_SW_CTL_ATTR                                                                    0x3
#define HWIO_SDC1_SDCC_SDCC_RAM_ADDR_SW_CTL_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_SDCC_RAM_ADDR_SW_CTL_ADDR, HWIO_SDC1_SDCC_SDCC_RAM_ADDR_SW_CTL_RMSK)
#define HWIO_SDC1_SDCC_SDCC_RAM_ADDR_SW_CTL_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_SDCC_RAM_ADDR_SW_CTL_ADDR, m)
#define HWIO_SDC1_SDCC_SDCC_RAM_ADDR_SW_CTL_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_SDCC_RAM_ADDR_SW_CTL_ADDR,v)
#define HWIO_SDC1_SDCC_SDCC_RAM_ADDR_SW_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_SDCC_RAM_ADDR_SW_CTL_ADDR,m,v,HWIO_SDC1_SDCC_SDCC_RAM_ADDR_SW_CTL_IN)
#define HWIO_SDC1_SDCC_SDCC_RAM_ADDR_SW_CTL_SW_RAM_RD_ADDRESS_ENABLE_BMSK                                    0x80000000
#define HWIO_SDC1_SDCC_SDCC_RAM_ADDR_SW_CTL_SW_RAM_RD_ADDRESS_ENABLE_SHFT                                          0x1f
#define HWIO_SDC1_SDCC_SDCC_RAM_ADDR_SW_CTL_SW_RAM_RD_ADDRESS_READ_ENABLE_BMSK                               0x40000000
#define HWIO_SDC1_SDCC_SDCC_RAM_ADDR_SW_CTL_SW_RAM_RD_ADDRESS_READ_ENABLE_SHFT                                     0x1e
#define HWIO_SDC1_SDCC_SDCC_RAM_ADDR_SW_CTL_SW_RAM_RD_ADDRESS_BMSK                                             0x7f0000
#define HWIO_SDC1_SDCC_SDCC_RAM_ADDR_SW_CTL_SW_RAM_RD_ADDRESS_SHFT                                                 0x10
#define HWIO_SDC1_SDCC_SDCC_RAM_ADDR_SW_CTL_FUNC_RAM_WR_ADDRESS_BMSK                                             0xff00
#define HWIO_SDC1_SDCC_SDCC_RAM_ADDR_SW_CTL_FUNC_RAM_WR_ADDRESS_SHFT                                                0x8
#define HWIO_SDC1_SDCC_SDCC_RAM_ADDR_SW_CTL_FUNC_RAM_RD_ADDRESS_BMSK                                               0xff
#define HWIO_SDC1_SDCC_SDCC_RAM_ADDR_SW_CTL_FUNC_RAM_RD_ADDRESS_SHFT                                                0x0

#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_ADDR                                                              (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000388)
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_OFFS                                                              (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000388)
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_RMSK                                                              0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_POR                                                               0x00010800
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_POR_RMSK                                                          0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_ATTR                                                                     0x3
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_ADDR, HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_ADDR,v)
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_ADDR,m,v,HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_IN)
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_COARSE_TUNING_CTRL_BMSK                                           0x80000000
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_COARSE_TUNING_CTRL_SHFT                                                 0x1f
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_DRR_BYPASS_EN_BMSK                                                0x40000000
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_DRR_BYPASS_EN_SHFT                                                      0x1e
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_BIAS_OK_SIGNAL_BMSK                                               0x20000000
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_BIAS_OK_SIGNAL_SHFT                                                     0x1d
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_FLL_MODE_EN_BMSK                                                  0x10000000
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_FLL_MODE_EN_SHFT                                                        0x1c
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_FINE_TUNE_MODE_EN_BMSK                                             0x8000000
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_FINE_TUNE_MODE_EN_SHFT                                                  0x1b
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_TEST_CONFIG_BMSK                                                   0x7000000
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_TEST_CONFIG_SHFT                                                        0x18
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_RDAC_CONFIG_BMSK                                                    0xc00000
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_RDAC_CONFIG_SHFT                                                        0x16
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_REF_VOLT_CONFIG_BMSK                                                0x300000
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_REF_VOLT_CONFIG_SHFT                                                    0x14
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_FREEZE_CDAC_OP_BMSK                                                  0x80000
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_FREEZE_CDAC_OP_SHFT                                                     0x13
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_FREEZE_RDAC_OP_BMSK                                                  0x40000
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_FREEZE_RDAC_OP_SHFT                                                     0x12
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_RESET_SDR_DAT_OUT_BMSK                                               0x20000
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_RESET_SDR_DAT_OUT_SHFT                                                  0x11
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_DELAY_LINE_OSC_MODE_BMSK                                             0x10000
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_DELAY_LINE_OSC_MODE_SHFT                                                0x10
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_EDGE_LEVEL_BMSK                                                       0xc000
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_EDGE_LEVEL_SHFT                                                          0xe
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_PROG_DLY_DLL_UP_DN_BMSK                                               0x3000
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_PROG_DLY_DLL_UP_DN_SHFT                                                  0xc
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_RDAC_INIT_CODE_BMSK                                                    0xff0
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_RDAC_INIT_CODE_SHFT                                                      0x4
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_BIST_ERR_INJECT_BMSK                                                     0xf
#define HWIO_SDC1_SDCC_HC_REG_DLL_USER_CTL_BIST_ERR_INJECT_SHFT                                                     0x0

#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_EXT_ADDR                                                            (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x0000038c)
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_EXT_OFFS                                                            (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000038c)
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_EXT_RMSK                                                             0x1ffffff
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_EXT_POR                                                             0x00000000
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_EXT_POR_RMSK                                                        0xffffffff
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_EXT_ATTR                                                                   0x1
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_EXT_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_EXT_ADDR, HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_EXT_RMSK)
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_EXT_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_EXT_ADDR, m)
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_EXT_SDC4_DLL_LOCK_BMSK                                               0x1000000
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_EXT_SDC4_DLL_LOCK_SHFT                                                    0x18
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_EXT_SDC4_DLL_STATUS_EXT_BMSK                                          0xffffff
#define HWIO_SDC1_SDCC_HC_REG_DLL_STATUS_EXT_SDC4_DLL_STATUS_EXT_SHFT                                               0x0

#define HWIO_SDC1_SDCC_HC_FIFO_ALTn_ADDR(n)                                                                  (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000400 + 0x4 * (n))
#define HWIO_SDC1_SDCC_HC_FIFO_ALTn_OFFS(n)                                                                  (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000400 + 0x4 * (n))
#define HWIO_SDC1_SDCC_HC_FIFO_ALTn_RMSK                                                                     0xffffffff
#define HWIO_SDC1_SDCC_HC_FIFO_ALTn_MAXn                                                                            255
#define HWIO_SDC1_SDCC_HC_FIFO_ALTn_POR                                                                      0x00000000
#define HWIO_SDC1_SDCC_HC_FIFO_ALTn_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC1_SDCC_HC_FIFO_ALTn_ATTR                                                                            0x3
#define HWIO_SDC1_SDCC_HC_FIFO_ALTn_INI(n)        \
        in_dword_masked(HWIO_SDC1_SDCC_HC_FIFO_ALTn_ADDR(n), HWIO_SDC1_SDCC_HC_FIFO_ALTn_RMSK)
#define HWIO_SDC1_SDCC_HC_FIFO_ALTn_INMI(n,mask)    \
        in_dword_masked(HWIO_SDC1_SDCC_HC_FIFO_ALTn_ADDR(n), mask)
#define HWIO_SDC1_SDCC_HC_FIFO_ALTn_OUTI(n,val)    \
        out_dword(HWIO_SDC1_SDCC_HC_FIFO_ALTn_ADDR(n),val)
#define HWIO_SDC1_SDCC_HC_FIFO_ALTn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_FIFO_ALTn_ADDR(n),mask,val,HWIO_SDC1_SDCC_HC_FIFO_ALTn_INI(n))
#define HWIO_SDC1_SDCC_HC_FIFO_ALTn_DATA_BMSK                                                                0xffffffff
#define HWIO_SDC1_SDCC_HC_FIFO_ALTn_DATA_SHFT                                                                       0x0

#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_ICE_CTRL_ADDR                                                      (SDC1_SDCC_SDCC5_HC_REG_BASE      + 0x00000800)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_ICE_CTRL_OFFS                                                      (SDC1_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000800)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_ICE_CTRL_RMSK                                                             0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_ICE_CTRL_POR                                                       0x00000000
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_ICE_CTRL_POR_RMSK                                                  0xffffffff
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_ICE_CTRL_ATTR                                                             0x3
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_ICE_CTRL_IN          \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_ICE_CTRL_ADDR, HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_ICE_CTRL_RMSK)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_ICE_CTRL_INM(m)      \
        in_dword_masked(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_ICE_CTRL_ADDR, m)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_ICE_CTRL_OUT(v)      \
        out_dword(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_ICE_CTRL_ADDR,v)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_ICE_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_ICE_CTRL_ADDR,m,v,HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_ICE_CTRL_IN)
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_ICE_CTRL_ICE_SW_RST_EN_BMSK                                               0x1
#define HWIO_SDC1_SDCC_HC_VENDOR_SPECIFIC_ICE_CTRL_ICE_SW_RST_EN_SHFT                                               0x0

/*----------------------------------------------------------------------------
 * MODULE: SDC2_SDCC_SDCC5_HC
 *--------------------------------------------------------------------------*/

#define SDC2_SDCC_SDCC5_HC_REG_BASE                                                                          (SDC2_SDCC5_TOP_BASE      + 0x00004000)
#define SDC2_SDCC_SDCC5_HC_REG_BASE_SIZE                                                                     0x1000
#define SDC2_SDCC_SDCC5_HC_REG_BASE_USED                                                                     0x7fc
#define SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS                                                                     0x00004000

#define HWIO_SDC2_SDCC_HC_REG_0_2_ADDR                                                                       (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000000)
#define HWIO_SDC2_SDCC_HC_REG_0_2_OFFS                                                                       (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000000)
#define HWIO_SDC2_SDCC_HC_REG_0_2_RMSK                                                                       0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_0_2_POR                                                                        0x00000000
#define HWIO_SDC2_SDCC_HC_REG_0_2_POR_RMSK                                                                   0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_0_2_ATTR                                                                              0x3
#define HWIO_SDC2_SDCC_HC_REG_0_2_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_0_2_ADDR, HWIO_SDC2_SDCC_HC_REG_0_2_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_0_2_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_0_2_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_0_2_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_REG_0_2_ADDR,v)
#define HWIO_SDC2_SDCC_HC_REG_0_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_REG_0_2_ADDR,m,v,HWIO_SDC2_SDCC_HC_REG_0_2_IN)
#define HWIO_SDC2_SDCC_HC_REG_0_2_ARG_2_BMSK                                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_0_2_ARG_2_SHFT                                                                        0x0

#define HWIO_SDC2_SDCC_HC_REG_4_6_ADDR                                                                       (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000004)
#define HWIO_SDC2_SDCC_HC_REG_4_6_OFFS                                                                       (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000004)
#define HWIO_SDC2_SDCC_HC_REG_4_6_RMSK                                                                       0xffff7fff
#define HWIO_SDC2_SDCC_HC_REG_4_6_POR                                                                        0x00000000
#define HWIO_SDC2_SDCC_HC_REG_4_6_POR_RMSK                                                                   0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_4_6_ATTR                                                                              0x3
#define HWIO_SDC2_SDCC_HC_REG_4_6_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_4_6_ADDR, HWIO_SDC2_SDCC_HC_REG_4_6_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_4_6_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_4_6_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_4_6_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_REG_4_6_ADDR,v)
#define HWIO_SDC2_SDCC_HC_REG_4_6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_REG_4_6_ADDR,m,v,HWIO_SDC2_SDCC_HC_REG_4_6_IN)
#define HWIO_SDC2_SDCC_HC_REG_4_6_BLK_CNT_FOR_CUR_TRANS_BMSK                                                 0xffff0000
#define HWIO_SDC2_SDCC_HC_REG_4_6_BLK_CNT_FOR_CUR_TRANS_SHFT                                                       0x10
#define HWIO_SDC2_SDCC_HC_REG_4_6_BLK_SIZE_HST_SDMA_BUF_BMSK                                                     0x7000
#define HWIO_SDC2_SDCC_HC_REG_4_6_BLK_SIZE_HST_SDMA_BUF_SHFT                                                        0xc
#define HWIO_SDC2_SDCC_HC_REG_4_6_BLK_SIZE_TRANS_BMSK                                                             0xfff
#define HWIO_SDC2_SDCC_HC_REG_4_6_BLK_SIZE_TRANS_SHFT                                                               0x0

#define HWIO_SDC2_SDCC_HC_REG_8_A_ADDR                                                                       (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000008)
#define HWIO_SDC2_SDCC_HC_REG_8_A_OFFS                                                                       (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000008)
#define HWIO_SDC2_SDCC_HC_REG_8_A_RMSK                                                                       0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_8_A_POR                                                                        0x00000000
#define HWIO_SDC2_SDCC_HC_REG_8_A_POR_RMSK                                                                   0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_8_A_ATTR                                                                              0x3
#define HWIO_SDC2_SDCC_HC_REG_8_A_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_8_A_ADDR, HWIO_SDC2_SDCC_HC_REG_8_A_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_8_A_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_8_A_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_8_A_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_REG_8_A_ADDR,v)
#define HWIO_SDC2_SDCC_HC_REG_8_A_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_REG_8_A_ADDR,m,v,HWIO_SDC2_SDCC_HC_REG_8_A_IN)
#define HWIO_SDC2_SDCC_HC_REG_8_A_CMD_ARG_1_BMSK                                                             0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_8_A_CMD_ARG_1_SHFT                                                                    0x0

#define HWIO_SDC2_SDCC_HC_REG_C_E_ADDR                                                                       (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x0000000c)
#define HWIO_SDC2_SDCC_HC_REG_C_E_OFFS                                                                       (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000000c)
#define HWIO_SDC2_SDCC_HC_REG_C_E_RMSK                                                                       0x3ffb003f
#define HWIO_SDC2_SDCC_HC_REG_C_E_POR                                                                        0x00000000
#define HWIO_SDC2_SDCC_HC_REG_C_E_POR_RMSK                                                                   0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_C_E_ATTR                                                                              0x3
#define HWIO_SDC2_SDCC_HC_REG_C_E_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_C_E_ADDR, HWIO_SDC2_SDCC_HC_REG_C_E_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_C_E_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_C_E_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_C_E_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_REG_C_E_ADDR,v)
#define HWIO_SDC2_SDCC_HC_REG_C_E_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_REG_C_E_ADDR,m,v,HWIO_SDC2_SDCC_HC_REG_C_E_IN)
#define HWIO_SDC2_SDCC_HC_REG_C_E_CMD_INDX_BMSK                                                              0x3f000000
#define HWIO_SDC2_SDCC_HC_REG_C_E_CMD_INDX_SHFT                                                                    0x18
#define HWIO_SDC2_SDCC_HC_REG_C_E_CMD_TYPE_BMSK                                                                0xc00000
#define HWIO_SDC2_SDCC_HC_REG_C_E_CMD_TYPE_SHFT                                                                    0x16
#define HWIO_SDC2_SDCC_HC_REG_C_E_CMD_DATA_PRESENT_SEL_BMSK                                                    0x200000
#define HWIO_SDC2_SDCC_HC_REG_C_E_CMD_DATA_PRESENT_SEL_SHFT                                                        0x15
#define HWIO_SDC2_SDCC_HC_REG_C_E_CMD_INDX_CHECK_EN_BMSK                                                       0x100000
#define HWIO_SDC2_SDCC_HC_REG_C_E_CMD_INDX_CHECK_EN_SHFT                                                           0x14
#define HWIO_SDC2_SDCC_HC_REG_C_E_CMD_CRC_CHECK_EN_BMSK                                                         0x80000
#define HWIO_SDC2_SDCC_HC_REG_C_E_CMD_CRC_CHECK_EN_SHFT                                                            0x13
#define HWIO_SDC2_SDCC_HC_REG_C_E_CMD_RESP_TYPE_SEL_BMSK                                                        0x30000
#define HWIO_SDC2_SDCC_HC_REG_C_E_CMD_RESP_TYPE_SEL_SHFT                                                           0x10
#define HWIO_SDC2_SDCC_HC_REG_C_E_TRANS_MODE_MULTI_SINGLE_BLK_SEL_BMSK                                             0x20
#define HWIO_SDC2_SDCC_HC_REG_C_E_TRANS_MODE_MULTI_SINGLE_BLK_SEL_SHFT                                              0x5
#define HWIO_SDC2_SDCC_HC_REG_C_E_TRANS_MODE_DATA_DIRECTION_SEL_BMSK                                               0x10
#define HWIO_SDC2_SDCC_HC_REG_C_E_TRANS_MODE_DATA_DIRECTION_SEL_SHFT                                                0x4
#define HWIO_SDC2_SDCC_HC_REG_C_E_TRANS_MODE_AUTO_CMD_EN_BMSK                                                       0xc
#define HWIO_SDC2_SDCC_HC_REG_C_E_TRANS_MODE_AUTO_CMD_EN_SHFT                                                       0x2
#define HWIO_SDC2_SDCC_HC_REG_C_E_TRANS_MODE_BLK_CNT_EN_BMSK                                                        0x2
#define HWIO_SDC2_SDCC_HC_REG_C_E_TRANS_MODE_BLK_CNT_EN_SHFT                                                        0x1
#define HWIO_SDC2_SDCC_HC_REG_C_E_TRANS_MODE_DMA_EN_BMSK                                                            0x1
#define HWIO_SDC2_SDCC_HC_REG_C_E_TRANS_MODE_DMA_EN_SHFT                                                            0x0

#define HWIO_SDC2_SDCC_HC_REG_10_12_ADDR                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000010)
#define HWIO_SDC2_SDCC_HC_REG_10_12_OFFS                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000010)
#define HWIO_SDC2_SDCC_HC_REG_10_12_RMSK                                                                     0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_10_12_POR                                                                      0x00000000
#define HWIO_SDC2_SDCC_HC_REG_10_12_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_10_12_ATTR                                                                            0x1
#define HWIO_SDC2_SDCC_HC_REG_10_12_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_10_12_ADDR, HWIO_SDC2_SDCC_HC_REG_10_12_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_10_12_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_10_12_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_10_12_CMD_RESP_BMSK                                                            0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_10_12_CMD_RESP_SHFT                                                                   0x0

#define HWIO_SDC2_SDCC_HC_REG_14_16_ADDR                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000014)
#define HWIO_SDC2_SDCC_HC_REG_14_16_OFFS                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000014)
#define HWIO_SDC2_SDCC_HC_REG_14_16_RMSK                                                                     0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_14_16_POR                                                                      0x00000000
#define HWIO_SDC2_SDCC_HC_REG_14_16_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_14_16_ATTR                                                                            0x1
#define HWIO_SDC2_SDCC_HC_REG_14_16_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_14_16_ADDR, HWIO_SDC2_SDCC_HC_REG_14_16_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_14_16_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_14_16_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_14_16_CMD_RESP_BMSK                                                            0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_14_16_CMD_RESP_SHFT                                                                   0x0

#define HWIO_SDC2_SDCC_HC_REG_18_1A_ADDR                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000018)
#define HWIO_SDC2_SDCC_HC_REG_18_1A_OFFS                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000018)
#define HWIO_SDC2_SDCC_HC_REG_18_1A_RMSK                                                                     0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_18_1A_POR                                                                      0x00000000
#define HWIO_SDC2_SDCC_HC_REG_18_1A_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_18_1A_ATTR                                                                            0x1
#define HWIO_SDC2_SDCC_HC_REG_18_1A_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_18_1A_ADDR, HWIO_SDC2_SDCC_HC_REG_18_1A_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_18_1A_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_18_1A_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_18_1A_CMD_RESP_BMSK                                                            0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_18_1A_CMD_RESP_SHFT                                                                   0x0

#define HWIO_SDC2_SDCC_HC_REG_1C_1E_ADDR                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x0000001c)
#define HWIO_SDC2_SDCC_HC_REG_1C_1E_OFFS                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000001c)
#define HWIO_SDC2_SDCC_HC_REG_1C_1E_RMSK                                                                     0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_1C_1E_POR                                                                      0x00000000
#define HWIO_SDC2_SDCC_HC_REG_1C_1E_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_1C_1E_ATTR                                                                            0x1
#define HWIO_SDC2_SDCC_HC_REG_1C_1E_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_1C_1E_ADDR, HWIO_SDC2_SDCC_HC_REG_1C_1E_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_1C_1E_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_1C_1E_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_1C_1E_CMD_RESP_BMSK                                                            0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_1C_1E_CMD_RESP_SHFT                                                                   0x0

#define HWIO_SDC2_SDCC_HC_REG_20_22_ADDR                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000020)
#define HWIO_SDC2_SDCC_HC_REG_20_22_OFFS                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000020)
#define HWIO_SDC2_SDCC_HC_REG_20_22_RMSK                                                                     0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_20_22_POR                                                                      0x00000000
#define HWIO_SDC2_SDCC_HC_REG_20_22_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_20_22_ATTR                                                                            0x3
#define HWIO_SDC2_SDCC_HC_REG_20_22_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_20_22_ADDR, HWIO_SDC2_SDCC_HC_REG_20_22_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_20_22_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_20_22_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_20_22_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_REG_20_22_ADDR,v)
#define HWIO_SDC2_SDCC_HC_REG_20_22_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_REG_20_22_ADDR,m,v,HWIO_SDC2_SDCC_HC_REG_20_22_IN)
#define HWIO_SDC2_SDCC_HC_REG_20_22_BUF_DATA_PORT_3_BMSK                                                     0xff000000
#define HWIO_SDC2_SDCC_HC_REG_20_22_BUF_DATA_PORT_3_SHFT                                                           0x18
#define HWIO_SDC2_SDCC_HC_REG_20_22_BUF_DATA_PORT_2_BMSK                                                       0xff0000
#define HWIO_SDC2_SDCC_HC_REG_20_22_BUF_DATA_PORT_2_SHFT                                                           0x10
#define HWIO_SDC2_SDCC_HC_REG_20_22_BUF_DATA_PORT_1_BMSK                                                         0xff00
#define HWIO_SDC2_SDCC_HC_REG_20_22_BUF_DATA_PORT_1_SHFT                                                            0x8
#define HWIO_SDC2_SDCC_HC_REG_20_22_BUF_DATA_PORT_0_BMSK                                                           0xff
#define HWIO_SDC2_SDCC_HC_REG_20_22_BUF_DATA_PORT_0_SHFT                                                            0x0

#define HWIO_SDC2_SDCC_HC_REG_24_26_ADDR                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000024)
#define HWIO_SDC2_SDCC_HC_REG_24_26_OFFS                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000024)
#define HWIO_SDC2_SDCC_HC_REG_24_26_RMSK                                                                      0x3ff0fff
#define HWIO_SDC2_SDCC_HC_REG_24_26_POR                                                                      0x01f800f0
#define HWIO_SDC2_SDCC_HC_REG_24_26_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_24_26_ATTR                                                                            0x1
#define HWIO_SDC2_SDCC_HC_REG_24_26_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_24_26_ADDR, HWIO_SDC2_SDCC_HC_REG_24_26_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_24_26_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_24_26_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_24_26_SIGANLING_18_SWITCHING_STS_BMSK                                           0x2000000
#define HWIO_SDC2_SDCC_HC_REG_24_26_SIGANLING_18_SWITCHING_STS_SHFT                                                0x19
#define HWIO_SDC2_SDCC_HC_REG_24_26_PRESENT_STATE_CMD_LINE_SIGNAL_LEVEL_BMSK                                  0x1000000
#define HWIO_SDC2_SDCC_HC_REG_24_26_PRESENT_STATE_CMD_LINE_SIGNAL_LEVEL_SHFT                                       0x18
#define HWIO_SDC2_SDCC_HC_REG_24_26_PRESENT_STATE_DAT_3_0_LINE_SIGNAL_LEVEL_BMSK                               0xf00000
#define HWIO_SDC2_SDCC_HC_REG_24_26_PRESENT_STATE_DAT_3_0_LINE_SIGNAL_LEVEL_SHFT                                   0x14
#define HWIO_SDC2_SDCC_HC_REG_24_26_PRESENT_STATE_WR_PROTECT_SWITCH_PIN_LEVEL_BMSK                              0x80000
#define HWIO_SDC2_SDCC_HC_REG_24_26_PRESENT_STATE_WR_PROTECT_SWITCH_PIN_LEVEL_SHFT                                 0x13
#define HWIO_SDC2_SDCC_HC_REG_24_26_PRESENT_STATE_CARD_DETECT_PIN_LEVEL_BMSK                                    0x40000
#define HWIO_SDC2_SDCC_HC_REG_24_26_PRESENT_STATE_CARD_DETECT_PIN_LEVEL_SHFT                                       0x12
#define HWIO_SDC2_SDCC_HC_REG_24_26_PRESENT_STATE_CARD_STATE_STABLE_BMSK                                        0x20000
#define HWIO_SDC2_SDCC_HC_REG_24_26_PRESENT_STATE_CARD_STATE_STABLE_SHFT                                           0x11
#define HWIO_SDC2_SDCC_HC_REG_24_26_PRESENT_STATE_CARD_INSERTED_BMSK                                            0x10000
#define HWIO_SDC2_SDCC_HC_REG_24_26_PRESENT_STATE_CARD_INSERTED_SHFT                                               0x10
#define HWIO_SDC2_SDCC_HC_REG_24_26_PRESENT_STATE_BUF_RD_EN_BMSK                                                  0x800
#define HWIO_SDC2_SDCC_HC_REG_24_26_PRESENT_STATE_BUF_RD_EN_SHFT                                                    0xb
#define HWIO_SDC2_SDCC_HC_REG_24_26_PRESENT_STATE_BUF_WR_EN_BMSK                                                  0x400
#define HWIO_SDC2_SDCC_HC_REG_24_26_PRESENT_STATE_BUF_WR_EN_SHFT                                                    0xa
#define HWIO_SDC2_SDCC_HC_REG_24_26_PRESENT_STATE_RD_TRANS_ACT_BMSK                                               0x200
#define HWIO_SDC2_SDCC_HC_REG_24_26_PRESENT_STATE_RD_TRANS_ACT_SHFT                                                 0x9
#define HWIO_SDC2_SDCC_HC_REG_24_26_PRESENT_STATE_WR_TRANS_ACT_BMSK                                               0x100
#define HWIO_SDC2_SDCC_HC_REG_24_26_PRESENT_STATE_WR_TRANS_ACT_SHFT                                                 0x8
#define HWIO_SDC2_SDCC_HC_REG_24_26_PRESENT_STATE_DAT_7_4_LINE_SIGNAL_LEVEL_BMSK                                   0xf0
#define HWIO_SDC2_SDCC_HC_REG_24_26_PRESENT_STATE_DAT_7_4_LINE_SIGNAL_LEVEL_SHFT                                    0x4
#define HWIO_SDC2_SDCC_HC_REG_24_26_PRESENT_STATE_RETUNING_REQ_BMSK                                                 0x8
#define HWIO_SDC2_SDCC_HC_REG_24_26_PRESENT_STATE_RETUNING_REQ_SHFT                                                 0x3
#define HWIO_SDC2_SDCC_HC_REG_24_26_PRESENT_STATE_DAT_LINE_ACT_BMSK                                                 0x4
#define HWIO_SDC2_SDCC_HC_REG_24_26_PRESENT_STATE_DAT_LINE_ACT_SHFT                                                 0x2
#define HWIO_SDC2_SDCC_HC_REG_24_26_PRESENT_STATE_CMD_INHIBIT_DAT_BMSK                                              0x2
#define HWIO_SDC2_SDCC_HC_REG_24_26_PRESENT_STATE_CMD_INHIBIT_DAT_SHFT                                              0x1
#define HWIO_SDC2_SDCC_HC_REG_24_26_PRESENT_STATE_CMD_INHIBIT_CMD_BMSK                                              0x1
#define HWIO_SDC2_SDCC_HC_REG_24_26_PRESENT_STATE_CMD_INHIBIT_CMD_SHFT                                              0x0

#define HWIO_SDC2_SDCC_HC_REG_28_2A_ADDR                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000028)
#define HWIO_SDC2_SDCC_HC_REG_28_2A_OFFS                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000028)
#define HWIO_SDC2_SDCC_HC_REG_28_2A_RMSK                                                                      0x70f0fff
#define HWIO_SDC2_SDCC_HC_REG_28_2A_POR                                                                      0x00000000
#define HWIO_SDC2_SDCC_HC_REG_28_2A_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_28_2A_ATTR                                                                            0x3
#define HWIO_SDC2_SDCC_HC_REG_28_2A_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_28_2A_ADDR, HWIO_SDC2_SDCC_HC_REG_28_2A_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_28_2A_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_28_2A_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_28_2A_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_REG_28_2A_ADDR,v)
#define HWIO_SDC2_SDCC_HC_REG_28_2A_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_REG_28_2A_ADDR,m,v,HWIO_SDC2_SDCC_HC_REG_28_2A_IN)
#define HWIO_SDC2_SDCC_HC_REG_28_2A_WKUP_EVENT_EN_ON_SD_CARD_REMOVAL_BMSK                                     0x4000000
#define HWIO_SDC2_SDCC_HC_REG_28_2A_WKUP_EVENT_EN_ON_SD_CARD_REMOVAL_SHFT                                          0x1a
#define HWIO_SDC2_SDCC_HC_REG_28_2A_WKUP_EVENT_EN_ON_SD_CARD_INSERTION_BMSK                                   0x2000000
#define HWIO_SDC2_SDCC_HC_REG_28_2A_WKUP_EVENT_EN_ON_SD_CARD_INSERTION_SHFT                                        0x19
#define HWIO_SDC2_SDCC_HC_REG_28_2A_WKUP_EVENT_EN_ON_SD_CARD_INT_BMSK                                         0x1000000
#define HWIO_SDC2_SDCC_HC_REG_28_2A_WKUP_EVENT_EN_ON_SD_CARD_INT_SHFT                                              0x18
#define HWIO_SDC2_SDCC_HC_REG_28_2A_BLK_GAP_CTL_INT_BMSK                                                        0x80000
#define HWIO_SDC2_SDCC_HC_REG_28_2A_BLK_GAP_CTL_INT_SHFT                                                           0x13
#define HWIO_SDC2_SDCC_HC_REG_28_2A_BLK_GAP_CTL_RD_WAIT_BMSK                                                    0x40000
#define HWIO_SDC2_SDCC_HC_REG_28_2A_BLK_GAP_CTL_RD_WAIT_SHFT                                                       0x12
#define HWIO_SDC2_SDCC_HC_REG_28_2A_BLK_GAP_CTL_CONTINUE_REQ_BMSK                                               0x20000
#define HWIO_SDC2_SDCC_HC_REG_28_2A_BLK_GAP_CTL_CONTINUE_REQ_SHFT                                                  0x11
#define HWIO_SDC2_SDCC_HC_REG_28_2A_BLK_GAP_CTL_STOP_GAP_REQ_BMSK                                               0x10000
#define HWIO_SDC2_SDCC_HC_REG_28_2A_BLK_GAP_CTL_STOP_GAP_REQ_SHFT                                                  0x10
#define HWIO_SDC2_SDCC_HC_REG_28_2A_PWR_CTL_SD_BUS_VOLTAGE_SEL_BMSK                                               0xe00
#define HWIO_SDC2_SDCC_HC_REG_28_2A_PWR_CTL_SD_BUS_VOLTAGE_SEL_SHFT                                                 0x9
#define HWIO_SDC2_SDCC_HC_REG_28_2A_PWR_CTL_SD_BUS_PWR_BMSK                                                       0x100
#define HWIO_SDC2_SDCC_HC_REG_28_2A_PWR_CTL_SD_BUS_PWR_SHFT                                                         0x8
#define HWIO_SDC2_SDCC_HC_REG_28_2A_HST_CTL1_CARD_DETECT_SIGNAL_SEL_BMSK                                           0x80
#define HWIO_SDC2_SDCC_HC_REG_28_2A_HST_CTL1_CARD_DETECT_SIGNAL_SEL_SHFT                                            0x7
#define HWIO_SDC2_SDCC_HC_REG_28_2A_HST_CTL1_CARD_DETECT_TEST_LEVEL_BMSK                                           0x40
#define HWIO_SDC2_SDCC_HC_REG_28_2A_HST_CTL1_CARD_DETECT_TEST_LEVEL_SHFT                                            0x6
#define HWIO_SDC2_SDCC_HC_REG_28_2A_HST_CTL1_EXTENDED_DATA_TRANS_WIDTH_BMSK                                        0x20
#define HWIO_SDC2_SDCC_HC_REG_28_2A_HST_CTL1_EXTENDED_DATA_TRANS_WIDTH_SHFT                                         0x5
#define HWIO_SDC2_SDCC_HC_REG_28_2A_HST_CTL1_DMA_SEL_BMSK                                                          0x18
#define HWIO_SDC2_SDCC_HC_REG_28_2A_HST_CTL1_DMA_SEL_SHFT                                                           0x3
#define HWIO_SDC2_SDCC_HC_REG_28_2A_HST_CTL1_HS_EN_BMSK                                                             0x4
#define HWIO_SDC2_SDCC_HC_REG_28_2A_HST_CTL1_HS_EN_SHFT                                                             0x2
#define HWIO_SDC2_SDCC_HC_REG_28_2A_HST_CTL1_DATA_TRANS_WIDTH_BMSK                                                  0x2
#define HWIO_SDC2_SDCC_HC_REG_28_2A_HST_CTL1_DATA_TRANS_WIDTH_SHFT                                                  0x1
#define HWIO_SDC2_SDCC_HC_REG_28_2A_HST_CTL1_LED_CTL_BMSK                                                           0x1
#define HWIO_SDC2_SDCC_HC_REG_28_2A_HST_CTL1_LED_CTL_SHFT                                                           0x0

#define HWIO_SDC2_SDCC_HC_REG_2C_2E_ADDR                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x0000002c)
#define HWIO_SDC2_SDCC_HC_REG_2C_2E_OFFS                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000002c)
#define HWIO_SDC2_SDCC_HC_REG_2C_2E_RMSK                                                                      0x70fffe7
#define HWIO_SDC2_SDCC_HC_REG_2C_2E_POR                                                                      0x00000001
#define HWIO_SDC2_SDCC_HC_REG_2C_2E_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_2C_2E_ATTR                                                                            0x3
#define HWIO_SDC2_SDCC_HC_REG_2C_2E_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_2C_2E_ADDR, HWIO_SDC2_SDCC_HC_REG_2C_2E_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_2C_2E_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_2C_2E_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_2C_2E_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_REG_2C_2E_ADDR,v)
#define HWIO_SDC2_SDCC_HC_REG_2C_2E_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_REG_2C_2E_ADDR,m,v,HWIO_SDC2_SDCC_HC_REG_2C_2E_IN)
#define HWIO_SDC2_SDCC_HC_REG_2C_2E_SW_RST_DAT_LINE_BMSK                                                      0x4000000
#define HWIO_SDC2_SDCC_HC_REG_2C_2E_SW_RST_DAT_LINE_SHFT                                                           0x1a
#define HWIO_SDC2_SDCC_HC_REG_2C_2E_SW_RST_CMD_LINE_BMSK                                                      0x2000000
#define HWIO_SDC2_SDCC_HC_REG_2C_2E_SW_RST_CMD_LINE_SHFT                                                           0x19
#define HWIO_SDC2_SDCC_HC_REG_2C_2E_SW_RST_FOR_ALL_BMSK                                                       0x1000000
#define HWIO_SDC2_SDCC_HC_REG_2C_2E_SW_RST_FOR_ALL_SHFT                                                            0x18
#define HWIO_SDC2_SDCC_HC_REG_2C_2E_DATA_TIMEOUT_COUNTER_BMSK                                                   0xf0000
#define HWIO_SDC2_SDCC_HC_REG_2C_2E_DATA_TIMEOUT_COUNTER_SHFT                                                      0x10
#define HWIO_SDC2_SDCC_HC_REG_2C_2E_CLK_CTL_SDCLK_FREQ_SEL_BMSK                                                  0xff00
#define HWIO_SDC2_SDCC_HC_REG_2C_2E_CLK_CTL_SDCLK_FREQ_SEL_SHFT                                                     0x8
#define HWIO_SDC2_SDCC_HC_REG_2C_2E_CLK_CTL_SDCLK_FREQ_SEL_MSB_BMSK                                                0xc0
#define HWIO_SDC2_SDCC_HC_REG_2C_2E_CLK_CTL_SDCLK_FREQ_SEL_MSB_SHFT                                                 0x6
#define HWIO_SDC2_SDCC_HC_REG_2C_2E_CLK_CTL_GEN_SEL_BMSK                                                           0x20
#define HWIO_SDC2_SDCC_HC_REG_2C_2E_CLK_CTL_GEN_SEL_SHFT                                                            0x5
#define HWIO_SDC2_SDCC_HC_REG_2C_2E_CLK_CTL_SDCLK_EN_BMSK                                                           0x4
#define HWIO_SDC2_SDCC_HC_REG_2C_2E_CLK_CTL_SDCLK_EN_SHFT                                                           0x2
#define HWIO_SDC2_SDCC_HC_REG_2C_2E_CLK_CTL_INTERNAL_CLK_STABLE_BMSK                                                0x2
#define HWIO_SDC2_SDCC_HC_REG_2C_2E_CLK_CTL_INTERNAL_CLK_STABLE_SHFT                                                0x1
#define HWIO_SDC2_SDCC_HC_REG_2C_2E_CLK_CTL_INTERNAL_CLK_EN_BMSK                                                    0x1
#define HWIO_SDC2_SDCC_HC_REG_2C_2E_CLK_CTL_INTERNAL_CLK_EN_SHFT                                                    0x0

#define HWIO_SDC2_SDCC_HC_REG_30_32_ADDR                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000030)
#define HWIO_SDC2_SDCC_HC_REG_30_32_OFFS                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000030)
#define HWIO_SDC2_SDCC_HC_REG_30_32_RMSK                                                                     0xc7ffdfff
#define HWIO_SDC2_SDCC_HC_REG_30_32_POR                                                                      0x00000000
#define HWIO_SDC2_SDCC_HC_REG_30_32_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_30_32_ATTR                                                                            0x3
#define HWIO_SDC2_SDCC_HC_REG_30_32_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_30_32_ADDR, HWIO_SDC2_SDCC_HC_REG_30_32_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_30_32_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_30_32_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_30_32_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_REG_30_32_ADDR,v)
#define HWIO_SDC2_SDCC_HC_REG_30_32_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_REG_30_32_ADDR,m,v,HWIO_SDC2_SDCC_HC_REG_30_32_IN)
#define HWIO_SDC2_SDCC_HC_REG_30_32_ERR_INT_STS_VENDOR_SPECIFIC_ERR_BMSK                                     0x80000000
#define HWIO_SDC2_SDCC_HC_REG_30_32_ERR_INT_STS_VENDOR_SPECIFIC_ERR_SHFT                                           0x1f
#define HWIO_SDC2_SDCC_HC_REG_30_32_ERR_INT_STS_VENDOR_SPECIFIC_STAT_BMSK                                    0x40000000
#define HWIO_SDC2_SDCC_HC_REG_30_32_ERR_INT_STS_VENDOR_SPECIFIC_STAT_SHFT                                          0x1e
#define HWIO_SDC2_SDCC_HC_REG_30_32_ERR_INT_STS_TUNING_ERR_BMSK                                               0x4000000
#define HWIO_SDC2_SDCC_HC_REG_30_32_ERR_INT_STS_TUNING_ERR_SHFT                                                    0x1a
#define HWIO_SDC2_SDCC_HC_REG_30_32_ERR_INT_STS_ADMA_ERR_BMSK                                                 0x2000000
#define HWIO_SDC2_SDCC_HC_REG_30_32_ERR_INT_STS_ADMA_ERR_SHFT                                                      0x19
#define HWIO_SDC2_SDCC_HC_REG_30_32_ERR_INT_STS_AUTO_CMD_ERR_BMSK                                             0x1000000
#define HWIO_SDC2_SDCC_HC_REG_30_32_ERR_INT_STS_AUTO_CMD_ERR_SHFT                                                  0x18
#define HWIO_SDC2_SDCC_HC_REG_30_32_ERR_INT_STS_CURRENT_LIMIT_ERR_BMSK                                         0x800000
#define HWIO_SDC2_SDCC_HC_REG_30_32_ERR_INT_STS_CURRENT_LIMIT_ERR_SHFT                                             0x17
#define HWIO_SDC2_SDCC_HC_REG_30_32_ERR_INT_STS_DATA_END_BIT_ERR_BMSK                                          0x400000
#define HWIO_SDC2_SDCC_HC_REG_30_32_ERR_INT_STS_DATA_END_BIT_ERR_SHFT                                              0x16
#define HWIO_SDC2_SDCC_HC_REG_30_32_ERR_INT_STS_DATA_CRC_ERR_BMSK                                              0x200000
#define HWIO_SDC2_SDCC_HC_REG_30_32_ERR_INT_STS_DATA_CRC_ERR_SHFT                                                  0x15
#define HWIO_SDC2_SDCC_HC_REG_30_32_ERR_INT_STS_DATA_TIMEOUT_ERR_BMSK                                          0x100000
#define HWIO_SDC2_SDCC_HC_REG_30_32_ERR_INT_STS_DATA_TIMEOUT_ERR_SHFT                                              0x14
#define HWIO_SDC2_SDCC_HC_REG_30_32_ERR_INT_STS_CMD_INDX_ERR_BMSK                                               0x80000
#define HWIO_SDC2_SDCC_HC_REG_30_32_ERR_INT_STS_CMD_INDX_ERR_SHFT                                                  0x13
#define HWIO_SDC2_SDCC_HC_REG_30_32_ERR_INT_STS_CMD_END_BIT_ERR_BMSK                                            0x40000
#define HWIO_SDC2_SDCC_HC_REG_30_32_ERR_INT_STS_CMD_END_BIT_ERR_SHFT                                               0x12
#define HWIO_SDC2_SDCC_HC_REG_30_32_ERR_INT_STS_CMD_CRC_ERR_BMSK                                                0x20000
#define HWIO_SDC2_SDCC_HC_REG_30_32_ERR_INT_STS_CMD_CRC_ERR_SHFT                                                   0x11
#define HWIO_SDC2_SDCC_HC_REG_30_32_ERR_INT_STS_CMD_TIMEOUT_ERR_BMSK                                            0x10000
#define HWIO_SDC2_SDCC_HC_REG_30_32_ERR_INT_STS_CMD_TIMEOUT_ERR_SHFT                                               0x10
#define HWIO_SDC2_SDCC_HC_REG_30_32_NORMAL_INT_STS_ERR_INT_BMSK                                                  0x8000
#define HWIO_SDC2_SDCC_HC_REG_30_32_NORMAL_INT_STS_ERR_INT_SHFT                                                     0xf
#define HWIO_SDC2_SDCC_HC_REG_30_32_NORMAL_INT_STS_CMD_QUEUE_BMSK                                                0x4000
#define HWIO_SDC2_SDCC_HC_REG_30_32_NORMAL_INT_STS_CMD_QUEUE_SHFT                                                   0xe
#define HWIO_SDC2_SDCC_HC_REG_30_32_NORMAL_INT_STS_RETUNING_EVENT_BMSK                                           0x1000
#define HWIO_SDC2_SDCC_HC_REG_30_32_NORMAL_INT_STS_RETUNING_EVENT_SHFT                                              0xc
#define HWIO_SDC2_SDCC_HC_REG_30_32_NORMAL_INT_STS_INT_C_BMSK                                                     0x800
#define HWIO_SDC2_SDCC_HC_REG_30_32_NORMAL_INT_STS_INT_C_SHFT                                                       0xb
#define HWIO_SDC2_SDCC_HC_REG_30_32_NORMAL_INT_STS_INT_B_BMSK                                                     0x400
#define HWIO_SDC2_SDCC_HC_REG_30_32_NORMAL_INT_STS_INT_B_SHFT                                                       0xa
#define HWIO_SDC2_SDCC_HC_REG_30_32_NORMAL_INT_STS_INT_A_BMSK                                                     0x200
#define HWIO_SDC2_SDCC_HC_REG_30_32_NORMAL_INT_STS_INT_A_SHFT                                                       0x9
#define HWIO_SDC2_SDCC_HC_REG_30_32_NORMAL_INT_STS_CARD_INT_BMSK                                                  0x100
#define HWIO_SDC2_SDCC_HC_REG_30_32_NORMAL_INT_STS_CARD_INT_SHFT                                                    0x8
#define HWIO_SDC2_SDCC_HC_REG_30_32_NORMAL_INT_STS_CARD_REMOVAL_BMSK                                               0x80
#define HWIO_SDC2_SDCC_HC_REG_30_32_NORMAL_INT_STS_CARD_REMOVAL_SHFT                                                0x7
#define HWIO_SDC2_SDCC_HC_REG_30_32_NORMAL_INT_STS_CARD_INSERTION_BMSK                                             0x40
#define HWIO_SDC2_SDCC_HC_REG_30_32_NORMAL_INT_STS_CARD_INSERTION_SHFT                                              0x6
#define HWIO_SDC2_SDCC_HC_REG_30_32_NORMAL_INT_STS_BUF_RD_READY_BMSK                                               0x20
#define HWIO_SDC2_SDCC_HC_REG_30_32_NORMAL_INT_STS_BUF_RD_READY_SHFT                                                0x5
#define HWIO_SDC2_SDCC_HC_REG_30_32_NORMAL_INT_STS_BUF_WR_READY_BMSK                                               0x10
#define HWIO_SDC2_SDCC_HC_REG_30_32_NORMAL_INT_STS_BUF_WR_READY_SHFT                                                0x4
#define HWIO_SDC2_SDCC_HC_REG_30_32_NORMAL_INT_STS_DMA_INT_BMSK                                                     0x8
#define HWIO_SDC2_SDCC_HC_REG_30_32_NORMAL_INT_STS_DMA_INT_SHFT                                                     0x3
#define HWIO_SDC2_SDCC_HC_REG_30_32_NORMAL_INT_STS_BLK_GAP_EVENT_BMSK                                               0x4
#define HWIO_SDC2_SDCC_HC_REG_30_32_NORMAL_INT_STS_BLK_GAP_EVENT_SHFT                                               0x2
#define HWIO_SDC2_SDCC_HC_REG_30_32_NORMAL_INT_STS_TRANS_COMPLETE_BMSK                                              0x2
#define HWIO_SDC2_SDCC_HC_REG_30_32_NORMAL_INT_STS_TRANS_COMPLETE_SHFT                                              0x1
#define HWIO_SDC2_SDCC_HC_REG_30_32_NORMAL_INT_STS_CMD_COMPLETE_BMSK                                                0x1
#define HWIO_SDC2_SDCC_HC_REG_30_32_NORMAL_INT_STS_CMD_COMPLETE_SHFT                                                0x0

#define HWIO_SDC2_SDCC_HC_REG_34_36_ADDR                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000034)
#define HWIO_SDC2_SDCC_HC_REG_34_36_OFFS                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000034)
#define HWIO_SDC2_SDCC_HC_REG_34_36_RMSK                                                                     0xc7ff5fff
#define HWIO_SDC2_SDCC_HC_REG_34_36_POR                                                                      0x00000000
#define HWIO_SDC2_SDCC_HC_REG_34_36_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_34_36_ATTR                                                                            0x3
#define HWIO_SDC2_SDCC_HC_REG_34_36_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_34_36_ADDR, HWIO_SDC2_SDCC_HC_REG_34_36_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_34_36_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_34_36_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_34_36_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_REG_34_36_ADDR,v)
#define HWIO_SDC2_SDCC_HC_REG_34_36_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_REG_34_36_ADDR,m,v,HWIO_SDC2_SDCC_HC_REG_34_36_IN)
#define HWIO_SDC2_SDCC_HC_REG_34_36_ERR_INT_STS_EN_VENDOR_SPECIFIC_ERR_BMSK                                  0x80000000
#define HWIO_SDC2_SDCC_HC_REG_34_36_ERR_INT_STS_EN_VENDOR_SPECIFIC_ERR_SHFT                                        0x1f
#define HWIO_SDC2_SDCC_HC_REG_34_36_ERR_INT_STS_EN_VENDOR_SPECIFIC_STAT_BMSK                                 0x40000000
#define HWIO_SDC2_SDCC_HC_REG_34_36_ERR_INT_STS_EN_VENDOR_SPECIFIC_STAT_SHFT                                       0x1e
#define HWIO_SDC2_SDCC_HC_REG_34_36_ERR_INT_STS_EN_TUNING_ERR_BMSK                                            0x4000000
#define HWIO_SDC2_SDCC_HC_REG_34_36_ERR_INT_STS_EN_TUNING_ERR_SHFT                                                 0x1a
#define HWIO_SDC2_SDCC_HC_REG_34_36_ERR_INT_STS_EN_ADMA_ERR_BMSK                                              0x2000000
#define HWIO_SDC2_SDCC_HC_REG_34_36_ERR_INT_STS_EN_ADMA_ERR_SHFT                                                   0x19
#define HWIO_SDC2_SDCC_HC_REG_34_36_ERR_INT_STS_EN_AUTO_CMD_ERR_BMSK                                          0x1000000
#define HWIO_SDC2_SDCC_HC_REG_34_36_ERR_INT_STS_EN_AUTO_CMD_ERR_SHFT                                               0x18
#define HWIO_SDC2_SDCC_HC_REG_34_36_ERR_INT_STS_EN_CURRENT_LIMIT_ERR_BMSK                                      0x800000
#define HWIO_SDC2_SDCC_HC_REG_34_36_ERR_INT_STS_EN_CURRENT_LIMIT_ERR_SHFT                                          0x17
#define HWIO_SDC2_SDCC_HC_REG_34_36_ERR_INT_STS_EN_DATA_END_BIT_ERR_BMSK                                       0x400000
#define HWIO_SDC2_SDCC_HC_REG_34_36_ERR_INT_STS_EN_DATA_END_BIT_ERR_SHFT                                           0x16
#define HWIO_SDC2_SDCC_HC_REG_34_36_ERR_INT_STS_EN_DATA_CRC_ERR_BMSK                                           0x200000
#define HWIO_SDC2_SDCC_HC_REG_34_36_ERR_INT_STS_EN_DATA_CRC_ERR_SHFT                                               0x15
#define HWIO_SDC2_SDCC_HC_REG_34_36_ERR_INT_STS_EN_DATA_TIMEOUT_BMSK                                           0x100000
#define HWIO_SDC2_SDCC_HC_REG_34_36_ERR_INT_STS_EN_DATA_TIMEOUT_SHFT                                               0x14
#define HWIO_SDC2_SDCC_HC_REG_34_36_ERR_INT_STS_EN_CMD_INDX_ERR_BMSK                                            0x80000
#define HWIO_SDC2_SDCC_HC_REG_34_36_ERR_INT_STS_EN_CMD_INDX_ERR_SHFT                                               0x13
#define HWIO_SDC2_SDCC_HC_REG_34_36_ERR_INT_STS_EN_CMD_END_BIT_ERR_BMSK                                         0x40000
#define HWIO_SDC2_SDCC_HC_REG_34_36_ERR_INT_STS_EN_CMD_END_BIT_ERR_SHFT                                            0x12
#define HWIO_SDC2_SDCC_HC_REG_34_36_ERR_INT_STS_EN_CMD_CRC_ERR_BMSK                                             0x20000
#define HWIO_SDC2_SDCC_HC_REG_34_36_ERR_INT_STS_EN_CMD_CRC_ERR_SHFT                                                0x11
#define HWIO_SDC2_SDCC_HC_REG_34_36_ERR_INT_STS_EN_CMD_TIMEOUT_BMSK                                             0x10000
#define HWIO_SDC2_SDCC_HC_REG_34_36_ERR_INT_STS_EN_CMD_TIMEOUT_SHFT                                                0x10
#define HWIO_SDC2_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_CMD_QUEUE_BMSK                                             0x4000
#define HWIO_SDC2_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_CMD_QUEUE_SHFT                                                0xe
#define HWIO_SDC2_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_RETUNING_EVENT_BMSK                                        0x1000
#define HWIO_SDC2_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_RETUNING_EVENT_SHFT                                           0xc
#define HWIO_SDC2_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_INT_C_BMSK                                                  0x800
#define HWIO_SDC2_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_INT_C_SHFT                                                    0xb
#define HWIO_SDC2_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_INT_B_BMSK                                                  0x400
#define HWIO_SDC2_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_INT_B_SHFT                                                    0xa
#define HWIO_SDC2_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_INT_A_BMSK                                                  0x200
#define HWIO_SDC2_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_INT_A_SHFT                                                    0x9
#define HWIO_SDC2_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_CARD_INT_BMSK                                               0x100
#define HWIO_SDC2_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_CARD_INT_SHFT                                                 0x8
#define HWIO_SDC2_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_CARD_REMOVAL_BMSK                                            0x80
#define HWIO_SDC2_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_CARD_REMOVAL_SHFT                                             0x7
#define HWIO_SDC2_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_CARD_INSERTION_BMSK                                          0x40
#define HWIO_SDC2_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_CARD_INSERTION_SHFT                                           0x6
#define HWIO_SDC2_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_BUF_RD_READY_BMSK                                            0x20
#define HWIO_SDC2_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_BUF_RD_READY_SHFT                                             0x5
#define HWIO_SDC2_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_BUF_WR_READY_BMSK                                            0x10
#define HWIO_SDC2_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_BUF_WR_READY_SHFT                                             0x4
#define HWIO_SDC2_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_DMA_INT_BMSK                                                  0x8
#define HWIO_SDC2_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_DMA_INT_SHFT                                                  0x3
#define HWIO_SDC2_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_BLK_GAP_EVENT_BMSK                                            0x4
#define HWIO_SDC2_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_BLK_GAP_EVENT_SHFT                                            0x2
#define HWIO_SDC2_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_TRANS_COMPLETE_BMSK                                           0x2
#define HWIO_SDC2_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_TRANS_COMPLETE_SHFT                                           0x1
#define HWIO_SDC2_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_CMD_COMPLETE_BMSK                                             0x1
#define HWIO_SDC2_SDCC_HC_REG_34_36_NORMAL_INT_STS_EN_CMD_COMPLETE_SHFT                                             0x0

#define HWIO_SDC2_SDCC_HC_REG_38_3A_ADDR                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000038)
#define HWIO_SDC2_SDCC_HC_REG_38_3A_OFFS                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000038)
#define HWIO_SDC2_SDCC_HC_REG_38_3A_RMSK                                                                     0xc7ffdfff
#define HWIO_SDC2_SDCC_HC_REG_38_3A_POR                                                                      0x00000000
#define HWIO_SDC2_SDCC_HC_REG_38_3A_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_38_3A_ATTR                                                                            0x3
#define HWIO_SDC2_SDCC_HC_REG_38_3A_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_38_3A_ADDR, HWIO_SDC2_SDCC_HC_REG_38_3A_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_38_3A_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_38_3A_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_38_3A_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_REG_38_3A_ADDR,v)
#define HWIO_SDC2_SDCC_HC_REG_38_3A_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_REG_38_3A_ADDR,m,v,HWIO_SDC2_SDCC_HC_REG_38_3A_IN)
#define HWIO_SDC2_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_VENDOR_SPECIFIC_ERR_BMSK                               0x80000000
#define HWIO_SDC2_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_VENDOR_SPECIFIC_ERR_SHFT                                     0x1f
#define HWIO_SDC2_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_VENDOR_SPECIFIC_STAT_BMSK                              0x40000000
#define HWIO_SDC2_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_VENDOR_SPECIFIC_STAT_SHFT                                    0x1e
#define HWIO_SDC2_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_TUNING_ERR_BMSK                                         0x4000000
#define HWIO_SDC2_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_TUNING_ERR_SHFT                                              0x1a
#define HWIO_SDC2_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_ADMA_ERR_BMSK                                           0x2000000
#define HWIO_SDC2_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_ADMA_ERR_SHFT                                                0x19
#define HWIO_SDC2_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_AUTO_CMD_ERR_BMSK                                       0x1000000
#define HWIO_SDC2_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_AUTO_CMD_ERR_SHFT                                            0x18
#define HWIO_SDC2_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_CURRENT_LIMIT_ERR_BMSK                                   0x800000
#define HWIO_SDC2_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_CURRENT_LIMIT_ERR_SHFT                                       0x17
#define HWIO_SDC2_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_DATA_END_BIT_ERR_BMSK                                    0x400000
#define HWIO_SDC2_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_DATA_END_BIT_ERR_SHFT                                        0x16
#define HWIO_SDC2_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_DATA_CRC_ERR_BMSK                                        0x200000
#define HWIO_SDC2_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_DATA_CRC_ERR_SHFT                                            0x15
#define HWIO_SDC2_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_DATA_TIMEOUT_BMSK                                        0x100000
#define HWIO_SDC2_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_DATA_TIMEOUT_SHFT                                            0x14
#define HWIO_SDC2_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_CMD_INDX_ERR_BMSK                                         0x80000
#define HWIO_SDC2_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_CMD_INDX_ERR_SHFT                                            0x13
#define HWIO_SDC2_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_CMD_END_BIT_ERR_BMSK                                      0x40000
#define HWIO_SDC2_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_CMD_END_BIT_ERR_SHFT                                         0x12
#define HWIO_SDC2_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_CMD_CRC_ERR_BMSK                                          0x20000
#define HWIO_SDC2_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_CMD_CRC_ERR_SHFT                                             0x11
#define HWIO_SDC2_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_CMD_TIMEOUT_BMSK                                          0x10000
#define HWIO_SDC2_SDCC_HC_REG_38_3A_ERR_INT_SIGNAL_EN_CMD_TIMEOUT_SHFT                                             0x10
#define HWIO_SDC2_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_ERR_INT_BMSK                                            0x8000
#define HWIO_SDC2_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_ERR_INT_SHFT                                               0xf
#define HWIO_SDC2_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_CMD_QUEUE_BMSK                                          0x4000
#define HWIO_SDC2_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_CMD_QUEUE_SHFT                                             0xe
#define HWIO_SDC2_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_RETUNING_EVENT_BMSK                                     0x1000
#define HWIO_SDC2_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_RETUNING_EVENT_SHFT                                        0xc
#define HWIO_SDC2_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_INT_C_BMSK                                               0x800
#define HWIO_SDC2_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_INT_C_SHFT                                                 0xb
#define HWIO_SDC2_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_INT_B_BMSK                                               0x400
#define HWIO_SDC2_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_INT_B_SHFT                                                 0xa
#define HWIO_SDC2_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_INT_A_BMSK                                               0x200
#define HWIO_SDC2_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_INT_A_SHFT                                                 0x9
#define HWIO_SDC2_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_CARD_INT_BMSK                                            0x100
#define HWIO_SDC2_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_CARD_INT_SHFT                                              0x8
#define HWIO_SDC2_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_CARD_REMOVAL_BMSK                                         0x80
#define HWIO_SDC2_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_CARD_REMOVAL_SHFT                                          0x7
#define HWIO_SDC2_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_CARD_INSERTION_BMSK                                       0x40
#define HWIO_SDC2_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_CARD_INSERTION_SHFT                                        0x6
#define HWIO_SDC2_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_BUF_RD_READY_BMSK                                         0x20
#define HWIO_SDC2_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_BUF_RD_READY_SHFT                                          0x5
#define HWIO_SDC2_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_BUF_WR_READY_BMSK                                         0x10
#define HWIO_SDC2_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_BUF_WR_READY_SHFT                                          0x4
#define HWIO_SDC2_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_DMA_INT_BMSK                                               0x8
#define HWIO_SDC2_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_DMA_INT_SHFT                                               0x3
#define HWIO_SDC2_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_BLK_GAP_EVENT_BMSK                                         0x4
#define HWIO_SDC2_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_BLK_GAP_EVENT_SHFT                                         0x2
#define HWIO_SDC2_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_TRANS_COMPLETE_BMSK                                        0x2
#define HWIO_SDC2_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_TRANS_COMPLETE_SHFT                                        0x1
#define HWIO_SDC2_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_CMD_COMPLETE_BMSK                                          0x1
#define HWIO_SDC2_SDCC_HC_REG_38_3A_NORMAL_INT_SIGNAL_EN_CMD_COMPLETE_SHFT                                          0x0

#define HWIO_SDC2_SDCC_HC_REG_3C_3E_ADDR                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x0000003c)
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_OFFS                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000003c)
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_RMSK                                                                     0xc0ff009f
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_POR                                                                      0x00000000
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_ATTR                                                                            0x3
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_3C_3E_ADDR, HWIO_SDC2_SDCC_HC_REG_3C_3E_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_3C_3E_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_REG_3C_3E_ADDR,v)
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_REG_3C_3E_ADDR,m,v,HWIO_SDC2_SDCC_HC_REG_3C_3E_IN)
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_HST_CTL2_PRESET_VALUE_EN_BMSK                                            0x80000000
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_HST_CTL2_PRESET_VALUE_EN_SHFT                                                  0x1f
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_HST_CTL2_ASYNC_INT_EN_BMSK                                               0x40000000
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_HST_CTL2_ASYNC_INT_EN_SHFT                                                     0x1e
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_HST_CTL2_SAMPL_CLK_SEL_BMSK                                                0x800000
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_HST_CTL2_SAMPL_CLK_SEL_SHFT                                                    0x17
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_HST_CTL2_EXEC_TUNING_BMSK                                                  0x400000
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_HST_CTL2_EXEC_TUNING_SHFT                                                      0x16
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_HST_CTL2_DRIVER_STRENGTH_SEL_BMSK                                          0x300000
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_HST_CTL2_DRIVER_STRENGTH_SEL_SHFT                                              0x14
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_HST_CTL2_SIGNALING_1_8_EN_BMSK                                              0x80000
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_HST_CTL2_SIGNALING_1_8_EN_SHFT                                                 0x13
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_HST_CTL2_UHS_MODE_SEL_BMSK                                                  0x70000
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_HST_CTL2_UHS_MODE_SEL_SHFT                                                     0x10
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_AUTO_CMD_NOT_ISSUED_BY_AUTO_CMD12_BMSK                                         0x80
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_AUTO_CMD_NOT_ISSUED_BY_AUTO_CMD12_SHFT                                          0x7
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_AUTO_CMD_INDX_ERR_BMSK                                                         0x10
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_AUTO_CMD_INDX_ERR_SHFT                                                          0x4
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_AUTO_CMD_END_BIT_ERR_BMSK                                                       0x8
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_AUTO_CMD_END_BIT_ERR_SHFT                                                       0x3
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_AUTO_CMD_CRC_ERR_BMSK                                                           0x4
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_AUTO_CMD_CRC_ERR_SHFT                                                           0x2
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_AUTO_CMD_TIMEOUT_BMSK                                                           0x2
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_AUTO_CMD_TIMEOUT_SHFT                                                           0x1
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_AUTO_CMD12_NOT_EXEC_BMSK                                                        0x1
#define HWIO_SDC2_SDCC_HC_REG_3C_3E_AUTO_CMD12_NOT_EXEC_SHFT                                                        0x0

#define HWIO_SDC2_SDCC_HC_REG_40_42_ADDR                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000040)
#define HWIO_SDC2_SDCC_HC_REG_40_42_OFFS                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000040)
#define HWIO_SDC2_SDCC_HC_REG_40_42_RMSK                                                                     0xf7efffbf
#define HWIO_SDC2_SDCC_HC_REG_40_42_POR                                                                      0x3029c8b2
#define HWIO_SDC2_SDCC_HC_REG_40_42_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_40_42_ATTR                                                                            0x1
#define HWIO_SDC2_SDCC_HC_REG_40_42_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_40_42_ADDR, HWIO_SDC2_SDCC_HC_REG_40_42_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_40_42_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_40_42_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_40_42_CAPABILITIES_SLOT_TYPE_BMSK                                              0xc0000000
#define HWIO_SDC2_SDCC_HC_REG_40_42_CAPABILITIES_SLOT_TYPE_SHFT                                                    0x1e
#define HWIO_SDC2_SDCC_HC_REG_40_42_CAPABILITIES_ASYNC_INT_SUPPORT_BMSK                                      0x20000000
#define HWIO_SDC2_SDCC_HC_REG_40_42_CAPABILITIES_ASYNC_INT_SUPPORT_SHFT                                            0x1d
#define HWIO_SDC2_SDCC_HC_REG_40_42_CAPABILITIES_SYS_BUS_SUPPORT_64_BIT_BMSK                                 0x10000000
#define HWIO_SDC2_SDCC_HC_REG_40_42_CAPABILITIES_SYS_BUS_SUPPORT_64_BIT_SHFT                                       0x1c
#define HWIO_SDC2_SDCC_HC_REG_40_42_CAPABILITIES_VOLTAGE_SUPPORT_1_8V_BMSK                                    0x4000000
#define HWIO_SDC2_SDCC_HC_REG_40_42_CAPABILITIES_VOLTAGE_SUPPORT_1_8V_SHFT                                         0x1a
#define HWIO_SDC2_SDCC_HC_REG_40_42_CAPABILITIES_VOLTAGE_SUPPORT_3_0V_BMSK                                    0x2000000
#define HWIO_SDC2_SDCC_HC_REG_40_42_CAPABILITIES_VOLTAGE_SUPPORT_3_0V_SHFT                                         0x19
#define HWIO_SDC2_SDCC_HC_REG_40_42_CAPABILITIES_VOLTAGE_SUPPORT_3_3V_BMSK                                    0x1000000
#define HWIO_SDC2_SDCC_HC_REG_40_42_CAPABILITIES_VOLTAGE_SUPPORT_3_3V_SHFT                                         0x18
#define HWIO_SDC2_SDCC_HC_REG_40_42_CAPABILITIES_SUSPEND_RESUME_SUPPORT_BMSK                                   0x800000
#define HWIO_SDC2_SDCC_HC_REG_40_42_CAPABILITIES_SUSPEND_RESUME_SUPPORT_SHFT                                       0x17
#define HWIO_SDC2_SDCC_HC_REG_40_42_CAPABILITIES_SDMA_SUPPORT_BMSK                                             0x400000
#define HWIO_SDC2_SDCC_HC_REG_40_42_CAPABILITIES_SDMA_SUPPORT_SHFT                                                 0x16
#define HWIO_SDC2_SDCC_HC_REG_40_42_CAPABILITIES_HS_SUPPORT_BMSK                                               0x200000
#define HWIO_SDC2_SDCC_HC_REG_40_42_CAPABILITIES_HS_SUPPORT_SHFT                                                   0x15
#define HWIO_SDC2_SDCC_HC_REG_40_42_CAPABILITIES_ADMA2_SUPPORT_BMSK                                             0x80000
#define HWIO_SDC2_SDCC_HC_REG_40_42_CAPABILITIES_ADMA2_SUPPORT_SHFT                                                0x13
#define HWIO_SDC2_SDCC_HC_REG_40_42_CAPABILITIES_SUPPORT_8_BIT_BMSK                                             0x40000
#define HWIO_SDC2_SDCC_HC_REG_40_42_CAPABILITIES_SUPPORT_8_BIT_SHFT                                                0x12
#define HWIO_SDC2_SDCC_HC_REG_40_42_CAPABILITIES_MAX_BLK_LENGTH_BMSK                                            0x30000
#define HWIO_SDC2_SDCC_HC_REG_40_42_CAPABILITIES_MAX_BLK_LENGTH_SHFT                                               0x10
#define HWIO_SDC2_SDCC_HC_REG_40_42_CAPABILITIES_BASE_SDCLK_FREQ_BMSK                                            0xff00
#define HWIO_SDC2_SDCC_HC_REG_40_42_CAPABILITIES_BASE_SDCLK_FREQ_SHFT                                               0x8
#define HWIO_SDC2_SDCC_HC_REG_40_42_CAPABILITIES_TIMEOUT_CLK_UNIT_BMSK                                             0x80
#define HWIO_SDC2_SDCC_HC_REG_40_42_CAPABILITIES_TIMEOUT_CLK_UNIT_SHFT                                              0x7
#define HWIO_SDC2_SDCC_HC_REG_40_42_CAPABILITIES_TIMEOUT_CLK_FREQ_BMSK                                             0x3f
#define HWIO_SDC2_SDCC_HC_REG_40_42_CAPABILITIES_TIMEOUT_CLK_FREQ_SHFT                                              0x0

#define HWIO_SDC2_SDCC_HC_REG_44_46_ADDR                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000044)
#define HWIO_SDC2_SDCC_HC_REG_44_46_OFFS                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000044)
#define HWIO_SDC2_SDCC_HC_REG_44_46_RMSK                                                                       0xffef77
#define HWIO_SDC2_SDCC_HC_REG_44_46_POR                                                                      0x00008007
#define HWIO_SDC2_SDCC_HC_REG_44_46_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_44_46_ATTR                                                                            0x1
#define HWIO_SDC2_SDCC_HC_REG_44_46_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_44_46_ADDR, HWIO_SDC2_SDCC_HC_REG_44_46_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_44_46_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_44_46_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_44_46_CAPABILITIES_CLK_MULTIPLIER_BMSK                                           0xff0000
#define HWIO_SDC2_SDCC_HC_REG_44_46_CAPABILITIES_CLK_MULTIPLIER_SHFT                                               0x10
#define HWIO_SDC2_SDCC_HC_REG_44_46_CAPABILITIES_RETUNING_MODE_BMSK                                              0xc000
#define HWIO_SDC2_SDCC_HC_REG_44_46_CAPABILITIES_RETUNING_MODE_SHFT                                                 0xe
#define HWIO_SDC2_SDCC_HC_REG_44_46_CAPABILITIES_USE_TUNING_FOR_SDR50_BMSK                                       0x2000
#define HWIO_SDC2_SDCC_HC_REG_44_46_CAPABILITIES_USE_TUNING_FOR_SDR50_SHFT                                          0xd
#define HWIO_SDC2_SDCC_HC_REG_44_46_CAPABILITIES_TIMER_CNT_FOR_RETUNING_BMSK                                      0xf00
#define HWIO_SDC2_SDCC_HC_REG_44_46_CAPABILITIES_TIMER_CNT_FOR_RETUNING_SHFT                                        0x8
#define HWIO_SDC2_SDCC_HC_REG_44_46_CAPABILITIES_DRIVER_TYPE_D_SUPPORT_BMSK                                        0x40
#define HWIO_SDC2_SDCC_HC_REG_44_46_CAPABILITIES_DRIVER_TYPE_D_SUPPORT_SHFT                                         0x6
#define HWIO_SDC2_SDCC_HC_REG_44_46_CAPABILITIES_DRIVER_TYPE_C_SUPPORT_BMSK                                        0x20
#define HWIO_SDC2_SDCC_HC_REG_44_46_CAPABILITIES_DRIVER_TYPE_C_SUPPORT_SHFT                                         0x5
#define HWIO_SDC2_SDCC_HC_REG_44_46_CAPABILITIES_DRIVER_TYPE_A_SUPPORT_BMSK                                        0x10
#define HWIO_SDC2_SDCC_HC_REG_44_46_CAPABILITIES_DRIVER_TYPE_A_SUPPORT_SHFT                                         0x4
#define HWIO_SDC2_SDCC_HC_REG_44_46_CAPABILITIES_DDR_50_SUPPORT_BMSK                                                0x4
#define HWIO_SDC2_SDCC_HC_REG_44_46_CAPABILITIES_DDR_50_SUPPORT_SHFT                                                0x2
#define HWIO_SDC2_SDCC_HC_REG_44_46_CAPABILITIES_SDR_104_SUPPORT_BMSK                                               0x2
#define HWIO_SDC2_SDCC_HC_REG_44_46_CAPABILITIES_SDR_104_SUPPORT_SHFT                                               0x1
#define HWIO_SDC2_SDCC_HC_REG_44_46_CAPABILITIES_SDR_50_SUPPORT_BMSK                                                0x1
#define HWIO_SDC2_SDCC_HC_REG_44_46_CAPABILITIES_SDR_50_SUPPORT_SHFT                                                0x0

#define HWIO_SDC2_SDCC_HC_REG_48_4A_ADDR                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000048)
#define HWIO_SDC2_SDCC_HC_REG_48_4A_OFFS                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000048)
#define HWIO_SDC2_SDCC_HC_REG_48_4A_RMSK                                                                       0xffffff
#define HWIO_SDC2_SDCC_HC_REG_48_4A_POR                                                                      0x00000000
#define HWIO_SDC2_SDCC_HC_REG_48_4A_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_48_4A_ATTR                                                                            0x3
#define HWIO_SDC2_SDCC_HC_REG_48_4A_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_48_4A_ADDR, HWIO_SDC2_SDCC_HC_REG_48_4A_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_48_4A_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_48_4A_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_48_4A_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_REG_48_4A_ADDR,v)
#define HWIO_SDC2_SDCC_HC_REG_48_4A_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_REG_48_4A_ADDR,m,v,HWIO_SDC2_SDCC_HC_REG_48_4A_IN)
#define HWIO_SDC2_SDCC_HC_REG_48_4A_MAX_CURRENT_1_8V_BMSK                                                      0xff0000
#define HWIO_SDC2_SDCC_HC_REG_48_4A_MAX_CURRENT_1_8V_SHFT                                                          0x10
#define HWIO_SDC2_SDCC_HC_REG_48_4A_MAX_CURRENT_3_0V_BMSK                                                        0xff00
#define HWIO_SDC2_SDCC_HC_REG_48_4A_MAX_CURRENT_3_0V_SHFT                                                           0x8
#define HWIO_SDC2_SDCC_HC_REG_48_4A_MAX_CURRENT_3_3V_BMSK                                                          0xff
#define HWIO_SDC2_SDCC_HC_REG_48_4A_MAX_CURRENT_3_3V_SHFT                                                           0x0

#define HWIO_SDC2_SDCC_HC_REG_50_52_ADDR                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000050)
#define HWIO_SDC2_SDCC_HC_REG_50_52_OFFS                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000050)
#define HWIO_SDC2_SDCC_HC_REG_50_52_RMSK                                                                     0xf3ff009f
#define HWIO_SDC2_SDCC_HC_REG_50_52_POR                                                                      0x00000000
#define HWIO_SDC2_SDCC_HC_REG_50_52_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_50_52_ATTR                                                                            0x2
#define HWIO_SDC2_SDCC_HC_REG_50_52_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_REG_50_52_ADDR,v)
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_VENDOR_SPECIFIC_ERR_STS_BMSK                         0xf0000000
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_VENDOR_SPECIFIC_ERR_STS_SHFT                               0x1c
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_ADMA_BMSK                                             0x2000000
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_ADMA_SHFT                                                  0x19
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_AUTO_CMD_BMSK                                         0x1000000
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_AUTO_CMD_SHFT                                              0x18
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_CURRENT_LIMIT_BMSK                                     0x800000
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_CURRENT_LIMIT_SHFT                                         0x17
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_DATA_END_BIT_BMSK                                      0x400000
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_DATA_END_BIT_SHFT                                          0x16
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_DATA_CRC_BMSK                                          0x200000
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_DATA_CRC_SHFT                                              0x15
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_DATA_TIMEOUT_BMSK                                      0x100000
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_DATA_TIMEOUT_SHFT                                          0x14
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_CMD_INDX_BMSK                                           0x80000
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_CMD_INDX_SHFT                                              0x13
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_CMD_END_BIT_BMSK                                        0x40000
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_CMD_END_BIT_SHFT                                           0x12
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_CMD_CRC_BMSK                                            0x20000
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_CMD_CRC_SHFT                                               0x11
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_CMD_TIMEOUT_BMSK                                        0x10000
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_ERR_CMD_TIMEOUT_SHFT                                           0x10
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_CMD_NOT_ISSUED_BY_AUTO_CMD12_BMSK                              0x80
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_CMD_NOT_ISSUED_BY_AUTO_CMD12_SHFT                               0x7
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_AUTO_CMD_INDX_ERR_BMSK                                         0x10
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_AUTO_CMD_INDX_ERR_SHFT                                          0x4
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_AUTO_CMD_END_BIT_ERR_BMSK                                       0x8
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_AUTO_CMD_END_BIT_ERR_SHFT                                       0x3
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_AUTO_CMD_CRC_ERR_BMSK                                           0x4
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_AUTO_CMD_CRC_ERR_SHFT                                           0x2
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_AUTO_CMD_TIMEOUT_ERR_BMSK                                       0x2
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_AUTO_CMD_TIMEOUT_ERR_SHFT                                       0x1
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_AUTO_CMD12_NOT_EXEC_BMSK                                        0x1
#define HWIO_SDC2_SDCC_HC_REG_50_52_FORCE_EVENT_FOR_AUTO_CMD12_NOT_EXEC_SHFT                                        0x0

#define HWIO_SDC2_SDCC_HC_REG_54_56_ADDR                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000054)
#define HWIO_SDC2_SDCC_HC_REG_54_56_OFFS                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000054)
#define HWIO_SDC2_SDCC_HC_REG_54_56_RMSK                                                                            0x7
#define HWIO_SDC2_SDCC_HC_REG_54_56_POR                                                                      0x00000000
#define HWIO_SDC2_SDCC_HC_REG_54_56_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_54_56_ATTR                                                                            0x1
#define HWIO_SDC2_SDCC_HC_REG_54_56_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_54_56_ADDR, HWIO_SDC2_SDCC_HC_REG_54_56_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_54_56_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_54_56_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_54_56_ADMA_LENGTH_MISMATCH_ERR_BMSK                                                   0x4
#define HWIO_SDC2_SDCC_HC_REG_54_56_ADMA_LENGTH_MISMATCH_ERR_SHFT                                                   0x2
#define HWIO_SDC2_SDCC_HC_REG_54_56_ADMA_ERR_STATE_BMSK                                                             0x3
#define HWIO_SDC2_SDCC_HC_REG_54_56_ADMA_ERR_STATE_SHFT                                                             0x0

#define HWIO_SDC2_SDCC_HC_REG_58_5A_ADDR                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000058)
#define HWIO_SDC2_SDCC_HC_REG_58_5A_OFFS                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000058)
#define HWIO_SDC2_SDCC_HC_REG_58_5A_RMSK                                                                     0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_58_5A_POR                                                                      0x00000000
#define HWIO_SDC2_SDCC_HC_REG_58_5A_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_58_5A_ATTR                                                                            0x3
#define HWIO_SDC2_SDCC_HC_REG_58_5A_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_58_5A_ADDR, HWIO_SDC2_SDCC_HC_REG_58_5A_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_58_5A_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_58_5A_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_58_5A_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_REG_58_5A_ADDR,v)
#define HWIO_SDC2_SDCC_HC_REG_58_5A_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_REG_58_5A_ADDR,m,v,HWIO_SDC2_SDCC_HC_REG_58_5A_IN)
#define HWIO_SDC2_SDCC_HC_REG_58_5A_ADMA_SYS_ADDRESS_BMSK                                                    0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_58_5A_ADMA_SYS_ADDRESS_SHFT                                                           0x0

#define HWIO_SDC2_SDCC_HC_REG_5C_5E_ADDR                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x0000005c)
#define HWIO_SDC2_SDCC_HC_REG_5C_5E_OFFS                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000005c)
#define HWIO_SDC2_SDCC_HC_REG_5C_5E_RMSK                                                                     0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_5C_5E_POR                                                                      0x00000000
#define HWIO_SDC2_SDCC_HC_REG_5C_5E_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_5C_5E_ATTR                                                                            0x3
#define HWIO_SDC2_SDCC_HC_REG_5C_5E_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_5C_5E_ADDR, HWIO_SDC2_SDCC_HC_REG_5C_5E_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_5C_5E_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_5C_5E_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_5C_5E_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_REG_5C_5E_ADDR,v)
#define HWIO_SDC2_SDCC_HC_REG_5C_5E_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_REG_5C_5E_ADDR,m,v,HWIO_SDC2_SDCC_HC_REG_5C_5E_IN)
#define HWIO_SDC2_SDCC_HC_REG_5C_5E_ADMA_SYS_ADDRESS_64_BMSK                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_5C_5E_ADMA_SYS_ADDRESS_64_SHFT                                                        0x0

#define HWIO_SDC2_SDCC_HC_REG_60_62_ADDR                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000060)
#define HWIO_SDC2_SDCC_HC_REG_60_62_OFFS                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000060)
#define HWIO_SDC2_SDCC_HC_REG_60_62_RMSK                                                                     0xc7ffc7ff
#define HWIO_SDC2_SDCC_HC_REG_60_62_POR                                                                      0x00040000
#define HWIO_SDC2_SDCC_HC_REG_60_62_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_60_62_ATTR                                                                            0x1
#define HWIO_SDC2_SDCC_HC_REG_60_62_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_60_62_ADDR, HWIO_SDC2_SDCC_HC_REG_60_62_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_60_62_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_60_62_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_60_62_DEFAULT_SPEED_DRIVER_STRENGTH_SEL_BMSK                                   0xc0000000
#define HWIO_SDC2_SDCC_HC_REG_60_62_DEFAULT_SPEED_DRIVER_STRENGTH_SEL_SHFT                                         0x1e
#define HWIO_SDC2_SDCC_HC_REG_60_62_DEFAULT_SPEED_CLK_GEN_SEL_BMSK                                            0x4000000
#define HWIO_SDC2_SDCC_HC_REG_60_62_DEFAULT_SPEED_CLK_GEN_SEL_SHFT                                                 0x1a
#define HWIO_SDC2_SDCC_HC_REG_60_62_DEFAULT_SPEED_SDCLK_FREQ_SEL_BMSK                                         0x3ff0000
#define HWIO_SDC2_SDCC_HC_REG_60_62_DEFAULT_SPEED_SDCLK_FREQ_SEL_SHFT                                              0x10
#define HWIO_SDC2_SDCC_HC_REG_60_62_INIT_DRIVER_STRENGTH_SEL_BMSK                                                0xc000
#define HWIO_SDC2_SDCC_HC_REG_60_62_INIT_DRIVER_STRENGTH_SEL_SHFT                                                   0xe
#define HWIO_SDC2_SDCC_HC_REG_60_62_INIT_CLK_GEN_SEL_BMSK                                                         0x400
#define HWIO_SDC2_SDCC_HC_REG_60_62_INIT_CLK_GEN_SEL_SHFT                                                           0xa
#define HWIO_SDC2_SDCC_HC_REG_60_62_INIT_SDCLK_FREQ_SEL_BMSK                                                      0x3ff
#define HWIO_SDC2_SDCC_HC_REG_60_62_INIT_SDCLK_FREQ_SEL_SHFT                                                        0x0

#define HWIO_SDC2_SDCC_HC_REG_64_66_ADDR                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000064)
#define HWIO_SDC2_SDCC_HC_REG_64_66_OFFS                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000064)
#define HWIO_SDC2_SDCC_HC_REG_64_66_RMSK                                                                     0xc7ffc7ff
#define HWIO_SDC2_SDCC_HC_REG_64_66_POR                                                                      0x00040002
#define HWIO_SDC2_SDCC_HC_REG_64_66_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_64_66_ATTR                                                                            0x1
#define HWIO_SDC2_SDCC_HC_REG_64_66_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_64_66_ADDR, HWIO_SDC2_SDCC_HC_REG_64_66_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_64_66_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_64_66_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_64_66_SDR12_DRIVER_STRENGTH_SEL_BMSK                                           0xc0000000
#define HWIO_SDC2_SDCC_HC_REG_64_66_SDR12_DRIVER_STRENGTH_SEL_SHFT                                                 0x1e
#define HWIO_SDC2_SDCC_HC_REG_64_66_SDR12_CLK_GEN_SEL_BMSK                                                    0x4000000
#define HWIO_SDC2_SDCC_HC_REG_64_66_SDR12_CLK_GEN_SEL_SHFT                                                         0x1a
#define HWIO_SDC2_SDCC_HC_REG_64_66_SDR12_SDCLK_FREQ_SEL_BMSK                                                 0x3ff0000
#define HWIO_SDC2_SDCC_HC_REG_64_66_SDR12_SDCLK_FREQ_SEL_SHFT                                                      0x10
#define HWIO_SDC2_SDCC_HC_REG_64_66_HS_DRIVER_STRENGTH_SEL_BMSK                                                  0xc000
#define HWIO_SDC2_SDCC_HC_REG_64_66_HS_DRIVER_STRENGTH_SEL_SHFT                                                     0xe
#define HWIO_SDC2_SDCC_HC_REG_64_66_HS_CLK_GEN_SEL_BMSK                                                           0x400
#define HWIO_SDC2_SDCC_HC_REG_64_66_HS_CLK_GEN_SEL_SHFT                                                             0xa
#define HWIO_SDC2_SDCC_HC_REG_64_66_HS_SDCLK_FREQ_SEL_BMSK                                                        0x3ff
#define HWIO_SDC2_SDCC_HC_REG_64_66_HS_SDCLK_FREQ_SEL_SHFT                                                          0x0

#define HWIO_SDC2_SDCC_HC_REG_68_6A_ADDR                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000068)
#define HWIO_SDC2_SDCC_HC_REG_68_6A_OFFS                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000068)
#define HWIO_SDC2_SDCC_HC_REG_68_6A_RMSK                                                                     0xc7ffc7ff
#define HWIO_SDC2_SDCC_HC_REG_68_6A_POR                                                                      0x00010002
#define HWIO_SDC2_SDCC_HC_REG_68_6A_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_68_6A_ATTR                                                                            0x1
#define HWIO_SDC2_SDCC_HC_REG_68_6A_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_68_6A_ADDR, HWIO_SDC2_SDCC_HC_REG_68_6A_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_68_6A_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_68_6A_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_68_6A_SDR50_DRIVER_STRENGTH_SEL_BMSK                                           0xc0000000
#define HWIO_SDC2_SDCC_HC_REG_68_6A_SDR50_DRIVER_STRENGTH_SEL_SHFT                                                 0x1e
#define HWIO_SDC2_SDCC_HC_REG_68_6A_SDR50_CLK_GEN_SEL_BMSK                                                    0x4000000
#define HWIO_SDC2_SDCC_HC_REG_68_6A_SDR50_CLK_GEN_SEL_SHFT                                                         0x1a
#define HWIO_SDC2_SDCC_HC_REG_68_6A_SDR50_SDCLK_FREQ_SEL_BMSK                                                 0x3ff0000
#define HWIO_SDC2_SDCC_HC_REG_68_6A_SDR50_SDCLK_FREQ_SEL_SHFT                                                      0x10
#define HWIO_SDC2_SDCC_HC_REG_68_6A_SDR25_DRIVER_STRENGTH_SEL_BMSK                                               0xc000
#define HWIO_SDC2_SDCC_HC_REG_68_6A_SDR25_DRIVER_STRENGTH_SEL_SHFT                                                  0xe
#define HWIO_SDC2_SDCC_HC_REG_68_6A_SDR25_CLK_GEN_SEL_BMSK                                                        0x400
#define HWIO_SDC2_SDCC_HC_REG_68_6A_SDR25_CLK_GEN_SEL_SHFT                                                          0xa
#define HWIO_SDC2_SDCC_HC_REG_68_6A_SDR25_SDCLK_FREQ_SEL_BMSK                                                     0x3ff
#define HWIO_SDC2_SDCC_HC_REG_68_6A_SDR25_SDCLK_FREQ_SEL_SHFT                                                       0x0

#define HWIO_SDC2_SDCC_HC_REG_6C_6E_ADDR                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x0000006c)
#define HWIO_SDC2_SDCC_HC_REG_6C_6E_OFFS                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000006c)
#define HWIO_SDC2_SDCC_HC_REG_6C_6E_RMSK                                                                     0xc7ffc7ff
#define HWIO_SDC2_SDCC_HC_REG_6C_6E_POR                                                                      0x00020000
#define HWIO_SDC2_SDCC_HC_REG_6C_6E_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_6C_6E_ATTR                                                                            0x1
#define HWIO_SDC2_SDCC_HC_REG_6C_6E_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_6C_6E_ADDR, HWIO_SDC2_SDCC_HC_REG_6C_6E_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_6C_6E_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_6C_6E_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_6C_6E_DDR50_DRIVER_STRENGTH_SEL_BMSK                                           0xc0000000
#define HWIO_SDC2_SDCC_HC_REG_6C_6E_DDR50_DRIVER_STRENGTH_SEL_SHFT                                                 0x1e
#define HWIO_SDC2_SDCC_HC_REG_6C_6E_DDR50_CLK_GEN_SEL_BMSK                                                    0x4000000
#define HWIO_SDC2_SDCC_HC_REG_6C_6E_DDR50_CLK_GEN_SEL_SHFT                                                         0x1a
#define HWIO_SDC2_SDCC_HC_REG_6C_6E_DDR50_SDCLK_FREQ_SEL_BMSK                                                 0x3ff0000
#define HWIO_SDC2_SDCC_HC_REG_6C_6E_DDR50_SDCLK_FREQ_SEL_SHFT                                                      0x10
#define HWIO_SDC2_SDCC_HC_REG_6C_6E_SDR104_DRIVER_STRENGTH_SEL_BMSK                                              0xc000
#define HWIO_SDC2_SDCC_HC_REG_6C_6E_SDR104_DRIVER_STRENGTH_SEL_SHFT                                                 0xe
#define HWIO_SDC2_SDCC_HC_REG_6C_6E_SDR104_CLK_GEN_SEL_BMSK                                                       0x400
#define HWIO_SDC2_SDCC_HC_REG_6C_6E_SDR104_CLK_GEN_SEL_SHFT                                                         0xa
#define HWIO_SDC2_SDCC_HC_REG_6C_6E_SDR104_SDCLK_FREQ_SEL_BMSK                                                    0x3ff
#define HWIO_SDC2_SDCC_HC_REG_6C_6E_SDR104_SDCLK_FREQ_SEL_SHFT                                                      0x0

#define HWIO_SDC2_SDCC_HC_REG_E0_E2_ADDR                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x000000e0)
#define HWIO_SDC2_SDCC_HC_REG_E0_E2_OFFS                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x000000e0)
#define HWIO_SDC2_SDCC_HC_REG_E0_E2_RMSK                                                                     0x7f777f37
#define HWIO_SDC2_SDCC_HC_REG_E0_E2_POR                                                                      0x00000000
#define HWIO_SDC2_SDCC_HC_REG_E0_E2_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_E0_E2_ATTR                                                                            0x3
#define HWIO_SDC2_SDCC_HC_REG_E0_E2_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_E0_E2_ADDR, HWIO_SDC2_SDCC_HC_REG_E0_E2_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_E0_E2_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_E0_E2_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_E0_E2_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_REG_E0_E2_ADDR,v)
#define HWIO_SDC2_SDCC_HC_REG_E0_E2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_REG_E0_E2_ADDR,m,v,HWIO_SDC2_SDCC_HC_REG_E0_E2_IN)
#define HWIO_SDC2_SDCC_HC_REG_E0_E2_SHARED_BUS_CTL_BACK_END_PWR_BMSK                                         0x7f000000
#define HWIO_SDC2_SDCC_HC_REG_E0_E2_SHARED_BUS_CTL_BACK_END_PWR_SHFT                                               0x18
#define HWIO_SDC2_SDCC_HC_REG_E0_E2_SHARED_BUS_INT_PIN_SEL_BMSK                                                0x700000
#define HWIO_SDC2_SDCC_HC_REG_E0_E2_SHARED_BUS_INT_PIN_SEL_SHFT                                                    0x14
#define HWIO_SDC2_SDCC_HC_REG_E0_E2_SHARED_BUS_CTL_CLK_PIN_SEL_BMSK                                             0x70000
#define HWIO_SDC2_SDCC_HC_REG_E0_E2_SHARED_BUS_CTL_CLK_PIN_SEL_SHFT                                                0x10
#define HWIO_SDC2_SDCC_HC_REG_E0_E2_SHARED_BUS_CTL_BUS_WIDTH_PRESET_BMSK                                         0x7f00
#define HWIO_SDC2_SDCC_HC_REG_E0_E2_SHARED_BUS_CTL_BUS_WIDTH_PRESET_SHFT                                            0x8
#define HWIO_SDC2_SDCC_HC_REG_E0_E2_SHARED_BUS_CTL_NUM_INT_INPUT_PINS_BMSK                                         0x30
#define HWIO_SDC2_SDCC_HC_REG_E0_E2_SHARED_BUS_CTL_NUM_INT_INPUT_PINS_SHFT                                          0x4
#define HWIO_SDC2_SDCC_HC_REG_E0_E2_SHARED_BUS_CTL_NUM_CLK_PINS_BMSK                                                0x7
#define HWIO_SDC2_SDCC_HC_REG_E0_E2_SHARED_BUS_CTL_NUM_CLK_PINS_SHFT                                                0x0

#define HWIO_SDC2_SDCC_HC_REG_FC_FE_ADDR                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x000000fc)
#define HWIO_SDC2_SDCC_HC_REG_FC_FE_OFFS                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x000000fc)
#define HWIO_SDC2_SDCC_HC_REG_FC_FE_RMSK                                                                     0xffff00ff
#define HWIO_SDC2_SDCC_HC_REG_FC_FE_POR                                                                      0x00020000
#define HWIO_SDC2_SDCC_HC_REG_FC_FE_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_FC_FE_ATTR                                                                            0x3
#define HWIO_SDC2_SDCC_HC_REG_FC_FE_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_FC_FE_ADDR, HWIO_SDC2_SDCC_HC_REG_FC_FE_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_FC_FE_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_FC_FE_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_FC_FE_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_REG_FC_FE_ADDR,v)
#define HWIO_SDC2_SDCC_HC_REG_FC_FE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_REG_FC_FE_ADDR,m,v,HWIO_SDC2_SDCC_HC_REG_FC_FE_IN)
#define HWIO_SDC2_SDCC_HC_REG_FC_FE_HC_VENDOR_VERSION_NUM_BMSK                                               0xff000000
#define HWIO_SDC2_SDCC_HC_REG_FC_FE_HC_VENDOR_VERSION_NUM_SHFT                                                     0x18
#define HWIO_SDC2_SDCC_HC_REG_FC_FE_HC_SPEC_VERSION_NUM_BMSK                                                   0xff0000
#define HWIO_SDC2_SDCC_HC_REG_FC_FE_HC_SPEC_VERSION_NUM_SHFT                                                       0x10
#define HWIO_SDC2_SDCC_HC_REG_FC_FE_INT_SIGNAL_FOR_EACH_SLOT_BMSK                                                  0xff
#define HWIO_SDC2_SDCC_HC_REG_FC_FE_INT_SIGNAL_FOR_EACH_SLOT_SHFT                                                   0x0

#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_ADDR                                                                (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000200)
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_OFFS                                                                (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000200)
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_RMSK                                                                0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_POR                                                                 0x6000642c
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_POR_RMSK                                                            0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_ATTR                                                                       0x3
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_ADDR, HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_ADDR,v)
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_ADDR,m,v,HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_IN)
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_SDC4_DIS_DOUT_BMSK                                                  0x80000000
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_SDC4_DIS_DOUT_SHFT                                                        0x1f
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_DLL_RST_BMSK                                                        0x40000000
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_DLL_RST_SHFT                                                              0x1e
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_PDN_BMSK                                                            0x20000000
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_PDN_SHFT                                                                  0x1d
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_CK_INTP_SEL_BMSK                                                    0x10000000
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_CK_INTP_SEL_SHFT                                                          0x1c
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_CK_INTP_EN_BMSK                                                      0x8000000
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_CK_INTP_EN_SHFT                                                           0x1b
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_MCLK_FREQ_BMSK                                                       0x7000000
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_MCLK_FREQ_SHFT                                                            0x18
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_CDR_SELEXT_BMSK                                                       0xf00000
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_CDR_SELEXT_SHFT                                                           0x14
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_CDR_EXT_EN_BMSK                                                        0x80000
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_CDR_EXT_EN_SHFT                                                           0x13
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_CK_OUT_EN_BMSK                                                         0x40000
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_CK_OUT_EN_SHFT                                                            0x12
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_CDR_EN_BMSK                                                            0x20000
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_CDR_EN_SHFT                                                               0x11
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_DLL_EN_BMSK                                                            0x10000
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_DLL_EN_SHFT                                                               0x10
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_CDR_UPD_RATE_BMSK                                                       0xc000
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_CDR_UPD_RATE_SHFT                                                          0xe
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_DLL_UPD_RATE_BMSK                                                       0x3000
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_DLL_UPD_RATE_SHFT                                                          0xc
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_DLL_PHASE_DET_BMSK                                                       0xc00
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_DLL_PHASE_DET_SHFT                                                         0xa
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_CDR_ALGORITHM_SEL_BMSK                                                   0x200
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_CDR_ALGORITHM_SEL_SHFT                                                     0x9
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_DLY_LINE_SWITCH_CLK_BMSK                                                 0x100
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_DLY_LINE_SWITCH_CLK_SHFT                                                   0x8
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_CDR_PHASE_SEL_MODE_BMSK                                                   0xc0
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_CDR_PHASE_SEL_MODE_SHFT                                                    0x6
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_MCLK_GATING_ENABLE_BMSK                                                   0x20
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_MCLK_GATING_ENABLE_SHFT                                                    0x5
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_FINE_PHASE_ENABLE_BMSK                                                    0x10
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_FINE_PHASE_ENABLE_SHFT                                                     0x4
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_CDR_FINE_PHASE_BMSK                                                        0xc
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_CDR_FINE_PHASE_SHFT                                                        0x2
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_RCLK_IN_TEST_EN_BMSK                                                       0x2
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_RCLK_IN_TEST_EN_SHFT                                                       0x1
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_CMD_DAT_TRACK_SEL_BMSK                                                     0x1
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_CMD_DAT_TRACK_SEL_SHFT                                                     0x0

#define HWIO_SDC2_SDCC_HC_REG_DLL_TEST_CTL_ADDR                                                              (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000204)
#define HWIO_SDC2_SDCC_HC_REG_DLL_TEST_CTL_OFFS                                                              (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000204)
#define HWIO_SDC2_SDCC_HC_REG_DLL_TEST_CTL_RMSK                                                              0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_DLL_TEST_CTL_POR                                                               0x00000000
#define HWIO_SDC2_SDCC_HC_REG_DLL_TEST_CTL_POR_RMSK                                                          0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_DLL_TEST_CTL_ATTR                                                                     0x3
#define HWIO_SDC2_SDCC_HC_REG_DLL_TEST_CTL_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_DLL_TEST_CTL_ADDR, HWIO_SDC2_SDCC_HC_REG_DLL_TEST_CTL_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_DLL_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_DLL_TEST_CTL_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_DLL_TEST_CTL_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_REG_DLL_TEST_CTL_ADDR,v)
#define HWIO_SDC2_SDCC_HC_REG_DLL_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_REG_DLL_TEST_CTL_ADDR,m,v,HWIO_SDC2_SDCC_HC_REG_DLL_TEST_CTL_IN)
#define HWIO_SDC2_SDCC_HC_REG_DLL_TEST_CTL_SDC4_DLL_TEST_CTL_BMSK                                            0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_DLL_TEST_CTL_SDC4_DLL_TEST_CTL_SHFT                                                   0x0

#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_ADDR                                                                (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000208)
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_OFFS                                                                (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000208)
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_RMSK                                                                    0x1ffd
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_POR                                                                 0x00000000
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_POR_RMSK                                                            0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_ATTR                                                                       0x1
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_ADDR, HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_SDC4_DTEST_MUXSEL_BMSK                                                  0x1000
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_SDC4_DTEST_MUXSEL_SHFT                                                     0xc
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_DDR_DLL_LOCK_JDR_BMSK                                                    0x800
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_DDR_DLL_LOCK_JDR_SHFT                                                      0xb
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_SDC4_DLL_DTEST_OUT_ATPG_BMSK                                             0x600
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_SDC4_DLL_DTEST_OUT_ATPG_SHFT                                               0x9
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_SDC4_DLL_LOCK_ATPG_BMSK                                                  0x100
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_SDC4_DLL_LOCK_ATPG_SHFT                                                    0x8
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_DLL_LOCK_BMSK                                                             0x80
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_DLL_LOCK_SHFT                                                              0x7
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_CDR_PHASE_BMSK                                                            0x78
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_CDR_PHASE_SHFT                                                             0x3
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_DDLL_COARSE_CAL_BMSK                                                       0x4
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_DDLL_COARSE_CAL_SHFT                                                       0x2
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_DDR_DLL_LOCK_BMSK                                                          0x1
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_DDR_DLL_LOCK_SHFT                                                          0x0

#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_ADDR                                                          (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x0000020c)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_OFFS                                                          (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000020c)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_RMSK                                                          0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_POR                                                           0x00000a0c
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_POR_RMSK                                                      0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_ATTR                                                                 0x3
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_ADDR, HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_RMSK)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_ADDR, m)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_ADDR,v)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_ADDR,m,v,HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_IN)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_AUTO_CMD_COUNTER_BMSK                                      0xffc00000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_AUTO_CMD_COUNTER_SHFT                                            0x16
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_SELECT_IN_BMSK                                               0x380000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_SELECT_IN_SHFT                                                   0x13
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_SELECT_IN_EN_BMSK                                             0x40000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_SELECT_IN_EN_SHFT                                                0x12
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_ADMA_DESC_PRIORITY_BMSK                                          0x20000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_ADMA_DESC_PRIORITY_SHFT                                             0x11
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_IO_PAD_PWR_SWITCH_BMSK                                        0x10000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_IO_PAD_PWR_SWITCH_SHFT                                           0x10
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_IO_PAD_PWR_SWITCH_EN_BMSK                                      0x8000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_IO_PAD_PWR_SWITCH_EN_SHFT                                         0xf
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_IGNORE_CTOUT_BMSK                                              0x4000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_IGNORE_CTOUT_SHFT                                                 0xe
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_SW_SDC4_CMD19_BMSK                                             0x2000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_SW_SDC4_CMD19_SHFT                                                0xd
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_INVERT_OUT_BMSK                                                0x1000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_INVERT_OUT_SHFT                                                   0xc
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_INVERT_OUT_EN_BMSK                                              0x800
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_INVERT_OUT_EN_SHFT                                                0xb
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_MODE_BMSK                                                          0x400
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_MODE_SHFT                                                            0xa
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_SDC4_MCLK_SEL_BMSK                                                 0x300
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_SDC4_MCLK_SEL_SHFT                                                   0x8
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_ADMA_ERR_SIZE_EN_BMSK                                               0x80
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_ADMA_ERR_SIZE_EN_SHFT                                                0x7
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_AUTO_CMD21_EN_BMSK                                               0x40
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_AUTO_CMD21_EN_SHFT                                                0x6
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_AUTO_CMD19_EN_BMSK                                               0x20
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_AUTO_CMD19_EN_SHFT                                                0x5
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_HCLKON_SW_EN_BMSK                                                0x10
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_HC_HCLKON_SW_EN_SHFT                                                 0x4
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_ADMA_BUSREQ_EN_BMSK                                                  0x8
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_ADMA_BUSREQ_EN_SHFT                                                  0x3
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_FLOW_EN_BMSK                                                         0x4
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_FLOW_EN_SHFT                                                         0x2
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_PWRSAVE_BMSK                                                         0x2
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_PWRSAVE_SHFT                                                         0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_CLK_INV_BMSK                                                         0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC_CLK_INV_SHFT                                                         0x0

#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_ADDR                                                         (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000210)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_OFFS                                                         (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000210)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_RMSK                                                         0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_POR                                                          0xf88218a8
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_POR_RMSK                                                     0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_ATTR                                                                0x3
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_ADDR, HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_RMSK)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_ADDR, m)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_ADDR,v)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_ADDR,m,v,HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_IN)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_ABURST_BMSK                                          0x80000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_ABURST_SHFT                                                0x1f
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_NUM_OUTSTANDING_DATA_BMSK                                    0x70000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_NUM_OUTSTANDING_DATA_SHFT                                          0x1c
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_PROCEED_AXI_AFTER_ERR_BMSK                                    0x8000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_PROCEED_AXI_AFTER_ERR_SHFT                                         0x1b
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_AFULL_CALC_BMSK                                       0x4000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_AFULL_CALC_SHFT                                            0x1a
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_ONE_MID_SUPPORT_BMSK                                          0x2000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_ONE_MID_SUPPORT_SHFT                                               0x19
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_READ_MEMTYPE_BMSK                                     0x1c00000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_READ_MEMTYPE_SHFT                                          0x16
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_HC_SW_RST_REQ_BMSK                                             0x200000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_HC_SW_RST_REQ_SHFT                                                 0x15
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_HC_SW_RST_WAIT_IDLE_DIS_BMSK                                   0x100000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_HC_SW_RST_WAIT_IDLE_DIS_SHFT                                       0x14
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_SDCC5_HALT_REQ_BMSK                                             0x80000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_SDCC5_HALT_REQ_SHFT                                                0x13
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_SDCC5_HALT_ACK_BMSK                                             0x40000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_SDCC5_HALT_ACK_SHFT                                                0x12
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_SDCC5_M_IDLE_BMSK                                               0x20000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_SDCC5_M_IDLE_SHFT                                                  0x11
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_SDCC5_HALT_ACK_SW_EN_BMSK                                       0x10000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_SDCC5_HALT_ACK_SW_EN_SHFT                                          0x10
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_SDCC5_HALT_ACK_SW_BMSK                                           0x8000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_SDCC5_HALT_ACK_SW_SHFT                                              0xf
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_SDCC5_M_IDLE_DIS_BMSK                                            0x4000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_SDCC5_M_IDLE_DIS_SHFT                                               0xe
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_INTERLEAVING_EN_BMSK                                     0x2000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_INTERLEAVING_EN_SHFT                                        0xd
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_TRANSIENT_BMSK                                           0x1000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_TRANSIENT_SHFT                                              0xc
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_PROTNS_BMSK                                               0x800
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_PROTNS_SHFT                                                 0xb
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_REQPRIORITY_BMSK                                          0x600
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_REQPRIORITY_SHFT                                            0x9
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_MEMTYPE_BMSK                                              0x1c0
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_MEMTYPE_SHFT                                                0x6
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_NOALLOCATE_BMSK                                            0x20
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_NOALLOCATE_SHFT                                             0x5
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_INNERSHARED_BMSK                                           0x10
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_INNERSHARED_SHFT                                            0x4
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_SHARED_BMSK                                                 0x8
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_SHARED_SHFT                                                 0x3
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_OOOWR_BMSK                                                  0x4
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_OOOWR_SHFT                                                  0x2
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_OOORD_BMSK                                                  0x2
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_OOORD_SHFT                                                  0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_AFULL_BMSK                                                  0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC2_QSB_AXI_AFULL_SHFT                                                  0x0

#define HWIO_SDC2_SDCC_HC_VS_ADMA_ERR_ADDR0_ADDR                                                             (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000214)
#define HWIO_SDC2_SDCC_HC_VS_ADMA_ERR_ADDR0_OFFS                                                             (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000214)
#define HWIO_SDC2_SDCC_HC_VS_ADMA_ERR_ADDR0_RMSK                                                             0xffffffff
#define HWIO_SDC2_SDCC_HC_VS_ADMA_ERR_ADDR0_POR                                                              0x00000000
#define HWIO_SDC2_SDCC_HC_VS_ADMA_ERR_ADDR0_POR_RMSK                                                         0xffffffff
#define HWIO_SDC2_SDCC_HC_VS_ADMA_ERR_ADDR0_ATTR                                                                    0x1
#define HWIO_SDC2_SDCC_HC_VS_ADMA_ERR_ADDR0_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VS_ADMA_ERR_ADDR0_ADDR, HWIO_SDC2_SDCC_HC_VS_ADMA_ERR_ADDR0_RMSK)
#define HWIO_SDC2_SDCC_HC_VS_ADMA_ERR_ADDR0_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VS_ADMA_ERR_ADDR0_ADDR, m)
#define HWIO_SDC2_SDCC_HC_VS_ADMA_ERR_ADDR0_CMDQ_TASK_DESC_ERROR_BMSK                                        0x80000000
#define HWIO_SDC2_SDCC_HC_VS_ADMA_ERR_ADDR0_CMDQ_TASK_DESC_ERROR_SHFT                                              0x1f
#define HWIO_SDC2_SDCC_HC_VS_ADMA_ERR_ADDR0_ADMA_VALID_ERROR_BMSK                                            0x40000000
#define HWIO_SDC2_SDCC_HC_VS_ADMA_ERR_ADDR0_ADMA_VALID_ERROR_SHFT                                                  0x1e
#define HWIO_SDC2_SDCC_HC_VS_ADMA_ERR_ADDR0_ADMA_RESP_ERROR_BMSK                                             0x20000000
#define HWIO_SDC2_SDCC_HC_VS_ADMA_ERR_ADDR0_ADMA_RESP_ERROR_SHFT                                                   0x1d
#define HWIO_SDC2_SDCC_HC_VS_ADMA_ERR_ADDR0_DATA_RESP_ERROR_BMSK                                             0x10000000
#define HWIO_SDC2_SDCC_HC_VS_ADMA_ERR_ADDR0_DATA_RESP_ERROR_SHFT                                                   0x1c
#define HWIO_SDC2_SDCC_HC_VS_ADMA_ERR_ADDR0_DATA_WR_RESP_ERROR_BMSK                                           0x8000000
#define HWIO_SDC2_SDCC_HC_VS_ADMA_ERR_ADDR0_DATA_WR_RESP_ERROR_SHFT                                                0x1b
#define HWIO_SDC2_SDCC_HC_VS_ADMA_ERR_ADDR0_ADMA_LEN_MISMATCH_BMSK                                            0x4000000
#define HWIO_SDC2_SDCC_HC_VS_ADMA_ERR_ADDR0_ADMA_LEN_MISMATCH_SHFT                                                 0x1a
#define HWIO_SDC2_SDCC_HC_VS_ADMA_ERR_ADDR0_RESP_ADDR_MSB_BMSK                                                0x3ffe000
#define HWIO_SDC2_SDCC_HC_VS_ADMA_ERR_ADDR0_RESP_ADDR_MSB_SHFT                                                      0xd
#define HWIO_SDC2_SDCC_HC_VS_ADMA_ERR_ADDR0_RESP_ADDR_LSB_BMSK                                                   0x1fff
#define HWIO_SDC2_SDCC_HC_VS_ADMA_ERR_ADDR0_RESP_ADDR_LSB_SHFT                                                      0x0

#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_ADDR                                                 (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x0000021c)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_OFFS                                                 (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000021c)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_RMSK                                                 0xf7efffbf
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_POR                                                  0x3029c8b2
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_POR_RMSK                                             0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_ATTR                                                        0x3
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_ADDR, HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_RMSK)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_ADDR, m)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_ADDR,v)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_ADDR,m,v,HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_IN)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_SLOT_TYPE_BMSK                       0xc0000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_SLOT_TYPE_SHFT                             0x1e
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_ASYNC_INT_SUPPORT_BMSK               0x20000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_ASYNC_INT_SUPPORT_SHFT                     0x1d
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_SYS_BUS_SUPPORT_64_BIT_BMSK          0x10000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_SYS_BUS_SUPPORT_64_BIT_SHFT                0x1c
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_VOLTAGE_SUPPORT_1_8V_BMSK             0x4000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_VOLTAGE_SUPPORT_1_8V_SHFT                  0x1a
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_VOLTAGE_SUPPORT_3_0V_BMSK             0x2000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_VOLTAGE_SUPPORT_3_0V_SHFT                  0x19
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_VOLTAGE_SUPPORT_3_3V_BMSK             0x1000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_VOLTAGE_SUPPORT_3_3V_SHFT                  0x18
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_SUSPEND_RESUME_SUPPORT_BMSK            0x800000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_SUSPEND_RESUME_SUPPORT_SHFT                0x17
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_SDMA_SUPPORT_BMSK                      0x400000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_SDMA_SUPPORT_SHFT                          0x16
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_HS_SUPPORT_BMSK                        0x200000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_HS_SUPPORT_SHFT                            0x15
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_ADMA2_SUPPORT_BMSK                      0x80000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_ADMA2_SUPPORT_SHFT                         0x13
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_SUPPORT_8_BIT_BMSK                      0x40000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_SUPPORT_8_BIT_SHFT                         0x12
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_MAX_BLK_LENGTH_BMSK                     0x30000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_MAX_BLK_LENGTH_SHFT                        0x10
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_BASE_SDCLK_FREQ_BMSK                     0xff00
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_BASE_SDCLK_FREQ_SHFT                        0x8
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_TIMEOUT_CLK_UNIT_BMSK                      0x80
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_TIMEOUT_CLK_UNIT_SHFT                       0x7
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_TIMEOUT_CLK_FREQ_BMSK                      0x3f
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES0_VS_CAPABILITIES_TIMEOUT_CLK_FREQ_SHFT                       0x0

#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_ADDR                                                 (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000220)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_OFFS                                                 (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000220)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_RMSK                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_POR                                                  0x0200808f
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_POR_RMSK                                             0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_ATTR                                                        0x3
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_ADDR, HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_RMSK)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_ADDR, m)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_ADDR,v)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_ADDR,m,v,HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_IN)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_SPEC_VERSION_BMSK                                    0xff000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_SPEC_VERSION_SHFT                                          0x18
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_CLK_MULTIPLIER_BMSK                    0xff0000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_CLK_MULTIPLIER_SHFT                        0x10
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_RETUNING_MODE_BMSK                       0xc000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_RETUNING_MODE_SHFT                          0xe
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_USE_TUNING_FOR_SDR50_BMSK                0x2000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_USE_TUNING_FOR_SDR50_SHFT                   0xd
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_FUSE_ICE_DISABLE_BMSK                    0x1000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_FUSE_ICE_DISABLE_SHFT                       0xc
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_TIMER_CNT_FOR_RETUNING_BMSK               0xf00
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_TIMER_CNT_FOR_RETUNING_SHFT                 0x8
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_CMDQ_SUPPORT_BMSK                          0x80
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_CMDQ_SUPPORT_SHFT                           0x7
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_DRIVER_TYPE_D_SUPPORT_BMSK                 0x40
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_DRIVER_TYPE_D_SUPPORT_SHFT                  0x6
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_DRIVER_TYPE_C_SUPPORT_BMSK                 0x20
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_DRIVER_TYPE_C_SUPPORT_SHFT                  0x5
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_DRIVER_TYPE_A_SUPPORT_BMSK                 0x10
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_DRIVER_TYPE_A_SUPPORT_SHFT                  0x4
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_HS400_SUPPORT_BMSK                          0x8
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_HS400_SUPPORT_SHFT                          0x3
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_DDR_50_SUPPORT_BMSK                         0x4
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_DDR_50_SUPPORT_SHFT                         0x2
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_SDR_104_SUPPORT_BMSK                        0x2
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_SDR_104_SUPPORT_SHFT                        0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_SDR_50_SUPPORT_BMSK                         0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CAPABILITIES1_VS_CAPABILITIES_SDR_50_SUPPORT_SHFT                         0x0

#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_ADDR                                                    (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000224)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_OFFS                                                    (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000224)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_RMSK                                                         0x7ff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_POR                                                     0x00000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_POR_RMSK                                                0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_ATTR                                                           0x3
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_ADDR, HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_RMSK)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_ADDR, m)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_ADDR,v)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_ADDR,m,v,HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_IN)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_FF_CLK_DIS_BMSK                                              0x400
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_FF_CLK_DIS_SHFT                                                0xa
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_VOLTAGE_MUX_SEL_BMSK                                         0x200
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_VOLTAGE_MUX_SEL_SHFT                                           0x9
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_CDC_TRAFFIC_SEL_BMSK                                         0x180
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_CDC_TRAFFIC_SEL_SHFT                                           0x7
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_START_CDC_TRAFFIC_BMSK                                        0x40
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_START_CDC_TRAFFIC_SHFT                                         0x6
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_CRC_TOKEN_SAMPL_FALL_EDGE_BMSK                                0x20
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_CRC_TOKEN_SAMPL_FALL_EDGE_SHFT                                 0x5
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_DATIN_SAMPL_FALL_EDGE_BMSK                                    0x10
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_DATIN_SAMPL_FALL_EDGE_SHFT                                     0x4
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_CDC_T4_TEST_OUT_SEL_BMSK                                       0x8
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_CDC_T4_TEST_OUT_SEL_SHFT                                       0x3
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_CMDIN_EDGE_SEL_BMSK                                            0x4
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_CMDIN_EDGE_SEL_SHFT                                            0x2
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_CMDIN_RCLK_EN_BMSK                                             0x2
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_CMDIN_RCLK_EN_SHFT                                             0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_CDC_T4_DLY_SEL_BMSK                                            0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DDR200_CFG_CDC_T4_DLY_SEL_SHFT                                            0x0

#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_ADDR                                               (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000228)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_OFFS                                               (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000228)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_RMSK                                                      0x3
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_POR                                                0x00000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_POR_RMSK                                           0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_ATTR                                                      0x3
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_ADDR, HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_RMSK)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_ADDR, m)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_ADDR,v)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_ADDR,m,v,HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_IN)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_CMD_PERIOD_BMSK                                           0x2
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_CMD_PERIOD_SHFT                                           0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_ENABLE_BMSK                                               0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_TIME_COUNT_CTRL_ENABLE_SHFT                                               0x0

#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FLOW_CTRL_TIME_COUNT_ADDR                                          (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x0000022c)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FLOW_CTRL_TIME_COUNT_OFFS                                          (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000022c)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FLOW_CTRL_TIME_COUNT_RMSK                                          0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FLOW_CTRL_TIME_COUNT_POR                                           0x00000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FLOW_CTRL_TIME_COUNT_POR_RMSK                                      0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FLOW_CTRL_TIME_COUNT_ATTR                                                 0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FLOW_CTRL_TIME_COUNT_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FLOW_CTRL_TIME_COUNT_ADDR, HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FLOW_CTRL_TIME_COUNT_RMSK)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FLOW_CTRL_TIME_COUNT_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FLOW_CTRL_TIME_COUNT_ADDR, m)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FLOW_CTRL_TIME_COUNT_FLOW_CTRL_TIME_BMSK                           0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FLOW_CTRL_TIME_COUNT_FLOW_CTRL_TIME_SHFT                                  0x0

#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_BUSY_TIME_COUNT_ADDR                                               (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000230)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_BUSY_TIME_COUNT_OFFS                                               (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000230)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_BUSY_TIME_COUNT_RMSK                                               0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_BUSY_TIME_COUNT_POR                                                0x00000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_BUSY_TIME_COUNT_POR_RMSK                                           0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_BUSY_TIME_COUNT_ATTR                                                      0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_BUSY_TIME_COUNT_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_BUSY_TIME_COUNT_ADDR, HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_BUSY_TIME_COUNT_RMSK)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_BUSY_TIME_COUNT_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_BUSY_TIME_COUNT_ADDR, m)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_BUSY_TIME_COUNT_BUSY_TIME_BMSK                                     0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_BUSY_TIME_COUNT_BUSY_TIME_SHFT                                            0x0

#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DATA_TIME_COUNT_ADDR                                               (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000234)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DATA_TIME_COUNT_OFFS                                               (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000234)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DATA_TIME_COUNT_RMSK                                               0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DATA_TIME_COUNT_POR                                                0x00000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DATA_TIME_COUNT_POR_RMSK                                           0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DATA_TIME_COUNT_ATTR                                                      0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DATA_TIME_COUNT_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DATA_TIME_COUNT_ADDR, HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DATA_TIME_COUNT_RMSK)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DATA_TIME_COUNT_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DATA_TIME_COUNT_ADDR, m)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DATA_TIME_COUNT_DATA_TIME_BMSK                                     0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DATA_TIME_COUNT_DATA_TIME_SHFT                                            0x0

#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CMD_TIME_COUNT_ADDR                                                (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000238)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CMD_TIME_COUNT_OFFS                                                (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000238)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CMD_TIME_COUNT_RMSK                                                0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CMD_TIME_COUNT_POR                                                 0x00000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CMD_TIME_COUNT_POR_RMSK                                            0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CMD_TIME_COUNT_ATTR                                                       0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CMD_TIME_COUNT_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CMD_TIME_COUNT_ADDR, HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CMD_TIME_COUNT_RMSK)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CMD_TIME_COUNT_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CMD_TIME_COUNT_ADDR, m)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CMD_TIME_COUNT_CMD_TIME_BMSK                                       0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_CMD_TIME_COUNT_CMD_TIME_SHFT                                              0x0

#define HWIO_SDC2_SDCC_SDCC_TEST_MODE_ADDR                                                                   (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x0000023c)
#define HWIO_SDC2_SDCC_SDCC_TEST_MODE_OFFS                                                                   (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000023c)
#define HWIO_SDC2_SDCC_SDCC_TEST_MODE_RMSK                                                                        0x7ff
#define HWIO_SDC2_SDCC_SDCC_TEST_MODE_POR                                                                    0x00000000
#define HWIO_SDC2_SDCC_SDCC_TEST_MODE_POR_RMSK                                                               0xffffffff
#define HWIO_SDC2_SDCC_SDCC_TEST_MODE_ATTR                                                                          0x3
#define HWIO_SDC2_SDCC_SDCC_TEST_MODE_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_SDCC_TEST_MODE_ADDR, HWIO_SDC2_SDCC_SDCC_TEST_MODE_RMSK)
#define HWIO_SDC2_SDCC_SDCC_TEST_MODE_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_SDCC_TEST_MODE_ADDR, m)
#define HWIO_SDC2_SDCC_SDCC_TEST_MODE_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_SDCC_TEST_MODE_ADDR,v)
#define HWIO_SDC2_SDCC_SDCC_TEST_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_SDCC_TEST_MODE_ADDR,m,v,HWIO_SDC2_SDCC_SDCC_TEST_MODE_IN)
#define HWIO_SDC2_SDCC_SDCC_TEST_MODE_TEST_MODE_ENABLE_BMSK                                                       0x400
#define HWIO_SDC2_SDCC_SDCC_TEST_MODE_TEST_MODE_ENABLE_SHFT                                                         0xa
#define HWIO_SDC2_SDCC_SDCC_TEST_MODE_TEST_MODE_CLK_BMSK                                                          0x200
#define HWIO_SDC2_SDCC_SDCC_TEST_MODE_TEST_MODE_CLK_SHFT                                                            0x9
#define HWIO_SDC2_SDCC_SDCC_TEST_MODE_TEST_MODE_CMD_BMSK                                                          0x100
#define HWIO_SDC2_SDCC_SDCC_TEST_MODE_TEST_MODE_CMD_SHFT                                                            0x8
#define HWIO_SDC2_SDCC_SDCC_TEST_MODE_TEST_MODE_DATA_BMSK                                                          0xff
#define HWIO_SDC2_SDCC_SDCC_TEST_MODE_TEST_MODE_DATA_SHFT                                                           0x0

#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_ADDR                                             (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000240)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_OFFS                                             (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000240)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_RMSK                                                    0xf
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_POR                                              0x00000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_POR_RMSK                                         0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_ATTR                                                    0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_ADDR, HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_RMSK)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_ADDR, m)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_IO_HIGH_V_BMSK                                          0x8
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_IO_HIGH_V_SHFT                                          0x3
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_IO_LOW_V_BMSK                                           0x4
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_IO_LOW_V_SHFT                                           0x2
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_BUS_ON_BMSK                                             0x2
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_BUS_ON_SHFT                                             0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_BUS_OFF_BMSK                                            0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_STATUS_REG_BUS_OFF_SHFT                                            0x0

#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_ADDR                                               (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000244)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_OFFS                                               (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000244)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_RMSK                                                      0xf
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_POR                                                0x00000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_POR_RMSK                                           0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_ATTR                                                      0x3
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_ADDR, HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_RMSK)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_ADDR, m)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_ADDR,v)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_ADDR,m,v,HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_IN)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_IO_HIGH_V_BMSK                                            0x8
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_IO_HIGH_V_SHFT                                            0x3
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_IO_LOW_V_BMSK                                             0x4
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_IO_LOW_V_SHFT                                             0x2
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_BUS_ON_BMSK                                               0x2
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_BUS_ON_SHFT                                               0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_BUS_OFF_BMSK                                              0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_MASK_REG_BUS_OFF_SHFT                                              0x0

#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_ADDR                                              (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000248)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_OFFS                                              (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000248)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_RMSK                                                     0xf
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_POR                                               0x00000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_POR_RMSK                                          0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_ATTR                                                     0x2
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_ADDR,v)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_IO_HIGH_V_BMSK                                           0x8
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_IO_HIGH_V_SHFT                                           0x3
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_IO_LOW_V_BMSK                                            0x4
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_IO_LOW_V_SHFT                                            0x2
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_BUS_ON_BMSK                                              0x2
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_BUS_ON_SHFT                                              0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_BUS_OFF_BMSK                                             0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CLEAR_REG_BUS_OFF_SHFT                                             0x0

#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_ADDR                                                (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x0000024c)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_OFFS                                                (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000024c)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_RMSK                                                     0x3ff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_POR                                                 0x00000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_POR_RMSK                                            0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_ATTR                                                       0x3
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_ADDR, HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_RMSK)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_ADDR, m)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_ADDR,v)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_ADDR,m,v,HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_IN)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_HC_CARD_STATE_STABLE_BMSK                                0x200
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_HC_CARD_STATE_STABLE_SHFT                                  0x9
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_RST_NOT_WAIT_PWRCTL_REG_BMSK                             0x100
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_RST_NOT_WAIT_PWRCTL_REG_SHFT                               0x8
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_HC_WRITE_PROTECT_PAD_BMSK                                 0xc0
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_HC_WRITE_PROTECT_PAD_SHFT                                  0x6
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_CARD_DETECT_PIN_LEVEL_BMSK                                0x20
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_CARD_DETECT_PIN_LEVEL_SHFT                                 0x5
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_HC_WRITE_PROTECT_BMSK                                     0x10
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_HC_WRITE_PROTECT_SHFT                                      0x4
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_IO_SIG_SWITCH_FAIL_BMSK                                    0x8
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_IO_SIG_SWITCH_FAIL_SHFT                                    0x3
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_IO_SIG_SWITCH_SUCCESS_BMSK                                 0x4
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_IO_SIG_SWITCH_SUCCESS_SHFT                                 0x2
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_BUS_ON_OFF_FAIL_BMSK                                       0x2
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_BUS_ON_OFF_FAIL_SHFT                                       0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_BUS_ON_OFF_SUCCESS_BMSK                                    0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_PWRCTL_CTL_REG_BUS_ON_OFF_SUCCESS_SHFT                                    0x0

#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_ADDR                                                         (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000250)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_OFFS                                                         (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000250)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_RMSK                                                         0xffefffef
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_POR                                                          0x02226040
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_POR_RMSK                                                     0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_ATTR                                                                0x3
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_ADDR, HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_RMSK)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_ADDR, m)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_ADDR,v)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_ADDR,m,v,HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_IN)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_BUSY_CHECK_VALID_PERIOD_BMSK                                 0xc0000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_BUSY_CHECK_VALID_PERIOD_SHFT                                       0x1e
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_BUSY_CHECK_VALID_ALWAYS_BMSK                                 0x20000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_BUSY_CHECK_VALID_ALWAYS_SHFT                                       0x1d
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_ADMA_INT_DATA_BMSK                                           0x10000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_ADMA_INT_DATA_SHFT                                                 0x1c
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_IRQ_PCLK_DIS_BMSK                                             0x8000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_IRQ_PCLK_DIS_SHFT                                                  0x1b
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_CLOCK_AFTER_CMDEND_DIS_BMSK                                   0x4000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_CLOCK_AFTER_CMDEND_DIS_SHFT                                        0x1a
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_SDCC_CLK_EXT_EN_BMSK                                          0x2000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_SDCC_CLK_EXT_EN_SHFT                                               0x19
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_SD_DEV_SEL_BMSK                                               0x1800000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_SD_DEV_SEL_SHFT                                                    0x17
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_SD_CLK_STABLE_BMSK                                             0x400000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_SD_CLK_STABLE_SHFT                                                 0x16
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_TCXO_SW_RST_EN_BMSK                                            0x200000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_TCXO_SW_RST_EN_SHFT                                                0x15
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_HS400_BLK_END_RST_DISABLE_BMSK                                  0x80000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_HS400_BLK_END_RST_DISABLE_SHFT                                     0x13
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_DATEN_HS400_INPUT_MASK_CNT_BMSK                                 0x70000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_DATEN_HS400_INPUT_MASK_CNT_SHFT                                    0x10
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_CMDEN_HS400_INPUT_MASK_CNT_BMSK                                  0xe000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_CMDEN_HS400_INPUT_MASK_CNT_SHFT                                     0xd
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_DATEN_HS400_INPUT_MASK_DIS_BMSK                                  0x1000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_DATEN_HS400_INPUT_MASK_DIS_SHFT                                     0xc
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_CMDEN_HS400_INPUT_MASK_DIS_BMSK                                   0x800
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_CMDEN_HS400_INPUT_MASK_DIS_SHFT                                     0xb
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_HC_FIFO_ALT_ENABLE_BMSK                                           0x400
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_HC_FIFO_ALT_ENABLE_SHFT                                             0xa
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_WIDEBUS_MASK_DISABLE_BMSK                                         0x200
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_WIDEBUS_MASK_DISABLE_SHFT                                           0x9
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_TXFLOWCONTROL_WITH_NO_TX_BMSK                                     0x100
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_TXFLOWCONTROL_WITH_NO_TX_SHFT                                       0x8
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_MCLK_DURING_SW_RST_REQ_DIS_BMSK                                    0x80
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_MCLK_DURING_SW_RST_REQ_DIS_SHFT                                     0x7
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_HS200_ASYNC_FIFO_WR_CLK_EN_BMSK                                    0x40
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_HS200_ASYNC_FIFO_WR_CLK_EN_SHFT                                     0x6
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_IGNORE_START_BIT_ERR_BMSK                                          0x20
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_IGNORE_START_BIT_ERR_SHFT                                           0x5
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_PWRSAVE_DLL_BMSK                                                    0x8
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_PWRSAVE_DLL_SHFT                                                    0x3
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_SDIO_TRANS_BMSK                                                     0x4
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_SDIO_TRANS_SHFT                                                     0x2
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_HCLK_IDLE_GATING_BMSK                                               0x2
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_HCLK_IDLE_GATING_SHFT                                               0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_MCLK_IDLE_GATING_BMSK                                               0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC3_MCLK_IDLE_GATING_SHFT                                               0x0

#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_ADDR                                                              (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000254)
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_OFFS                                                              (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000254)
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_RMSK                                                                0x6ffc0f
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_POR                                                               0x0020a000
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_POR_RMSK                                                          0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_ATTR                                                                     0x3
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_ADDR, HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_ADDR,v)
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_ADDR,m,v,HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_IN)
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_DLL_OUTPUT_TO_PAD_BMSK                                              0x400000
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_DLL_OUTPUT_TO_PAD_SHFT                                                  0x16
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_DLL_CLOCK_DISABLE_BMSK                                              0x200000
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_DLL_CLOCK_DISABLE_SHFT                                                  0x15
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_LOW_FREQ_MODE_BMSK                                                   0x80000
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_LOW_FREQ_MODE_SHFT                                                      0x13
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_FLL_CYCLE_CNT_BMSK                                                   0x40000
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_FLL_CYCLE_CNT_SHFT                                                      0x12
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_MCLK_FREQ_CALC_BMSK                                                  0x3fc00
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_MCLK_FREQ_CALC_SHFT                                                      0xa
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_DDR_TRAFFIC_INIT_SEL_BMSK                                                0xc
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_DDR_TRAFFIC_INIT_SEL_SHFT                                                0x2
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_DDR_TRAFFIC_INIT_SW_BMSK                                                 0x2
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_DDR_TRAFFIC_INIT_SW_SHFT                                                 0x1
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_DDR_CAL_EN_BMSK                                                          0x1
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_2_DDR_CAL_EN_SHFT                                                          0x0

#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_3_ADDR                                                              (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000258)
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_3_OFFS                                                              (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000258)
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_3_RMSK                                                                    0xff
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_3_POR                                                               0x00000000
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_3_POR_RMSK                                                          0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_3_ATTR                                                                     0x3
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_3_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_3_ADDR, HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_3_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_3_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_3_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_3_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_3_ADDR,v)
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_3_ADDR,m,v,HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_3_IN)
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_3_SDC4_CONFIG_MSB_BMSK                                                    0xff
#define HWIO_SDC2_SDCC_HC_REG_DLL_CONFIG_3_SDC4_CONFIG_MSB_SHFT                                                     0x0

#define HWIO_SDC2_SDCC_HC_REG_DDR_CONFIG_ADDR                                                                (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x0000025c)
#define HWIO_SDC2_SDCC_HC_REG_DDR_CONFIG_OFFS                                                                (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000025c)
#define HWIO_SDC2_SDCC_HC_REG_DDR_CONFIG_RMSK                                                                0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_DDR_CONFIG_POR                                                                 0x80040873
#define HWIO_SDC2_SDCC_HC_REG_DDR_CONFIG_POR_RMSK                                                            0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_DDR_CONFIG_ATTR                                                                       0x3
#define HWIO_SDC2_SDCC_HC_REG_DDR_CONFIG_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_DDR_CONFIG_ADDR, HWIO_SDC2_SDCC_HC_REG_DDR_CONFIG_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_DDR_CONFIG_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_DDR_CONFIG_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_DDR_CONFIG_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_REG_DDR_CONFIG_ADDR,v)
#define HWIO_SDC2_SDCC_HC_REG_DDR_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_REG_DDR_CONFIG_ADDR,m,v,HWIO_SDC2_SDCC_HC_REG_DDR_CONFIG_IN)
#define HWIO_SDC2_SDCC_HC_REG_DDR_CONFIG_PRG_DLY_EN_BMSK                                                     0x80000000
#define HWIO_SDC2_SDCC_HC_REG_DDR_CONFIG_PRG_DLY_EN_SHFT                                                           0x1f
#define HWIO_SDC2_SDCC_HC_REG_DDR_CONFIG_EXT_PRG_RCLK_DLY_EN_BMSK                                            0x40000000
#define HWIO_SDC2_SDCC_HC_REG_DDR_CONFIG_EXT_PRG_RCLK_DLY_EN_SHFT                                                  0x1e
#define HWIO_SDC2_SDCC_HC_REG_DDR_CONFIG_EXT_PRG_RCLK_DLY_CODE_BMSK                                          0x38000000
#define HWIO_SDC2_SDCC_HC_REG_DDR_CONFIG_EXT_PRG_RCLK_DLY_CODE_SHFT                                                0x1b
#define HWIO_SDC2_SDCC_HC_REG_DDR_CONFIG_EXT_PRG_RCLK_DLY_BMSK                                                0x7e00000
#define HWIO_SDC2_SDCC_HC_REG_DDR_CONFIG_EXT_PRG_RCLK_DLY_SHFT                                                     0x15
#define HWIO_SDC2_SDCC_HC_REG_DDR_CONFIG_TCXO_CYCLES_DLY_LINE_BMSK                                             0x1ff000
#define HWIO_SDC2_SDCC_HC_REG_DDR_CONFIG_TCXO_CYCLES_DLY_LINE_SHFT                                                  0xc
#define HWIO_SDC2_SDCC_HC_REG_DDR_CONFIG_TCXO_CYCLES_CNT_BMSK                                                     0xe00
#define HWIO_SDC2_SDCC_HC_REG_DDR_CONFIG_TCXO_CYCLES_CNT_SHFT                                                       0x9
#define HWIO_SDC2_SDCC_HC_REG_DDR_CONFIG_PRG_RCLK_DLY_BMSK                                                        0x1ff
#define HWIO_SDC2_SDCC_HC_REG_DDR_CONFIG_PRG_RCLK_DLY_SHFT                                                          0x0

#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_ADDR                                                         (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000260)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_OFFS                                                         (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000260)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_RMSK                                                           0xffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_POR                                                          0x00030022
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_POR_RMSK                                                     0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_ATTR                                                                0x3
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_ADDR, HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_RMSK)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_ADDR, m)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_ADDR,v)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_ADDR,m,v,HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_IN)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_DLL_CDR_EN_OVERRIDE_BMSK                                       0x800000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_DLL_CDR_EN_OVERRIDE_SHFT                                           0x17
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_UARM_PRIORITY_MODE_BMSK                                        0x700000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_UARM_PRIORITY_MODE_SHFT                                            0x14
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_SBFE_UAWM_DIS_BMSK                                              0x80000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_SBFE_UAWM_DIS_SHFT                                                 0x13
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_SBFE_UARM_DIS_BMSK                                              0x40000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_SBFE_UARM_DIS_SHFT                                                 0x12
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_AXI_MAX_BURST_LENGTH_BMSK                                       0x30000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_AXI_MAX_BURST_LENGTH_SHFT                                          0x10
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_DISABLE_CRYPTO_BMSK                                              0x8000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_DISABLE_CRYPTO_SHFT                                                 0xf
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_WRAP_ERROR_BMSK                                                  0x4000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_WRAP_ERROR_SHFT                                                     0xe
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_RX_FLOW_TIMING_BMSK                                              0x2000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_RX_FLOW_TIMING_SHFT                                                 0xd
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_PROGDONE_WO_CMD_RESP_BMSK                                        0x1000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_PROGDONE_WO_CMD_RESP_SHFT                                           0xc
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_HC_AXI_ARLOCK_BMSK                                                0xc00
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_HC_AXI_ARLOCK_SHFT                                                  0xa
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_HC_AXI_AWLOCK_BMSK                                                0x300
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_HC_AXI_AWLOCK_SHFT                                                  0x8
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_HC_AXI_ARCACHE_BMSK                                                0xf0
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_HC_AXI_ARCACHE_SHFT                                                 0x4
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_HC_AXI_AWCACHE_BMSK                                                 0xf
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_FUNC4_HC_AXI_AWCACHE_SHFT                                                 0x0

#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_ADDR(n)                                           (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000264 + 0x4 * (n))
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_OFFS(n)                                           (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000264 + 0x4 * (n))
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_RMSK                                              0x80000001
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_MAXn                                                      31
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_POR                                               0x00000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_POR_RMSK                                          0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_ATTR                                                     0x3
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_INI(n)        \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_ADDR(n), HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_RMSK)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_INMI(n,mask)    \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_ADDR(n), mask)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_OUTI(n,val)    \
        out_dword(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_ADDR(n),val)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_ADDR(n),mask,val,HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_INI(n))
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_HW_EVENT_EN_BMSK                                  0x80000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_HW_EVENT_EN_SHFT                                        0x1f
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_HW_EVENT_SEL_BMSK                                        0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_HW_EVENT_MUXSELn_HW_EVENT_SEL_SHFT                                        0x0

#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_ADDR                                                    (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x000002e4)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_OFFS                                                    (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x000002e4)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_RMSK                                                       0x300ff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_POR                                                     0x00000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_POR_RMSK                                                0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_ATTR                                                           0x3
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_ADDR, HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_RMSK)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_ADDR, m)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_ADDR,v)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_ADDR,m,v,HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_IN)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_HC_AXI_AW_CNTR_EOT_RST_BMSK                                0x20000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_HC_AXI_AW_CNTR_EOT_RST_SHFT                                   0x11
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_HC_AXI_AW_CNTR_SOT_RST_BMSK                                0x10000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_HC_AXI_AW_CNTR_SOT_RST_SHFT                                   0x10
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_HC_AXI_AW_CNTR_BMSK                                           0xff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AW_MON_HC_AXI_AW_CNTR_SHFT                                            0x0

#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_ADDR                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x000002e8)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_OFFS                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x000002e8)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_RMSK                                                        0x300ff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_POR                                                      0x00000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_POR_RMSK                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_ATTR                                                            0x3
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_ADDR, HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_RMSK)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_ADDR, m)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_ADDR,v)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_ADDR,m,v,HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_IN)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_HC_AXI_W_CNTR_EOT_RST_BMSK                                  0x20000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_HC_AXI_W_CNTR_EOT_RST_SHFT                                     0x11
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_HC_AXI_W_CNTR_SOT_RST_BMSK                                  0x10000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_HC_AXI_W_CNTR_SOT_RST_SHFT                                     0x10
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_HC_AXI_W_CNTR_BMSK                                             0xff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_W_MON_HC_AXI_W_CNTR_SHFT                                              0x0

#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_ADDR                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x000002ec)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_OFFS                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x000002ec)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_RMSK                                                        0x300ff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_POR                                                      0x00000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_POR_RMSK                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_ATTR                                                            0x3
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_ADDR, HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_RMSK)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_ADDR, m)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_ADDR,v)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_ADDR,m,v,HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_IN)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_HC_AXI_B_CNTR_EOT_RST_BMSK                                  0x20000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_HC_AXI_B_CNTR_EOT_RST_SHFT                                     0x11
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_HC_AXI_B_CNTR_SOT_RST_BMSK                                  0x10000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_HC_AXI_B_CNTR_SOT_RST_SHFT                                     0x10
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_HC_AXI_B_CNTR_BMSK                                             0xff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_B_MON_HC_AXI_B_CNTR_SHFT                                              0x0

#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_ADDR                                                    (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x000002f0)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_OFFS                                                    (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x000002f0)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_RMSK                                                       0x300ff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_POR                                                     0x00000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_POR_RMSK                                                0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_ATTR                                                           0x3
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_ADDR, HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_RMSK)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_ADDR, m)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_ADDR,v)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_ADDR,m,v,HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_IN)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_HC_AXI_AR_CNTR_EOT_RST_BMSK                                0x20000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_HC_AXI_AR_CNTR_EOT_RST_SHFT                                   0x11
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_HC_AXI_AR_CNTR_SOT_RST_BMSK                                0x10000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_HC_AXI_AR_CNTR_SOT_RST_SHFT                                   0x10
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_HC_AXI_AR_CNTR_BMSK                                           0xff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_AR_MON_HC_AXI_AR_CNTR_SHFT                                            0x0

#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_ADDR                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x000002f4)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_OFFS                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x000002f4)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_RMSK                                                        0x300ff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_POR                                                      0x00000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_POR_RMSK                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_ATTR                                                            0x3
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_ADDR, HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_RMSK)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_ADDR, m)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_ADDR,v)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_ADDR,m,v,HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_IN)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_HC_AXI_R_CNTR_EOT_RST_BMSK                                  0x20000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_HC_AXI_R_CNTR_EOT_RST_SHFT                                     0x11
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_HC_AXI_R_CNTR_SOT_RST_BMSK                                  0x10000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_HC_AXI_R_CNTR_SOT_RST_SHFT                                     0x10
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_HC_AXI_R_CNTR_BMSK                                             0xff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_AXI_R_MON_HC_AXI_R_CNTR_SHFT                                              0x0

#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ADDR                                                       (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x000002f8)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_OFFS                                                       (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x000002f8)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_RMSK                                                         0x3f0001
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_POR                                                        0x00000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_POR_RMSK                                                   0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ATTR                                                              0x3
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ADDR, HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_RMSK)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ADDR, m)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ADDR,v)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ADDR,m,v,HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_IN)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ERR_INT_STS_START_BIT_ERROR_BMSK                             0x200000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ERR_INT_STS_START_BIT_ERROR_SHFT                                 0x15
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ERR_INT_STS_FIFO_FULL_WR_BMSK                                0x100000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ERR_INT_STS_FIFO_FULL_WR_SHFT                                    0x14
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ERR_INT_STS_FIFO_EMPTY_RD_BMSK                                0x80000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ERR_INT_STS_FIFO_EMPTY_RD_SHFT                                   0x13
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ERR_INT_STS_AUTO_CMD19_TIMEOUT_BMSK                           0x40000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ERR_INT_STS_AUTO_CMD19_TIMEOUT_SHFT                              0x12
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ERR_INT_STS_BOOT_TIMEOUT_BMSK                                 0x20000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ERR_INT_STS_BOOT_TIMEOUT_SHFT                                    0x11
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ERR_INT_STS_BOOT_ACK_ERR_BMSK                                 0x10000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_ERR_INT_STS_BOOT_ACK_ERR_SHFT                                    0x10
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_NORMAL_INT_STS_BOOT_ACK_REC_BMSK                                  0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_NORMAL_INT_STS_BOOT_ACK_REC_SHFT                                  0x0

#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ADDR                                                    (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x000002fc)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_OFFS                                                    (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x000002fc)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_RMSK                                                      0x3f0001
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_POR                                                     0x00000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_POR_RMSK                                                0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ATTR                                                           0x3
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ADDR, HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_RMSK)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ADDR, m)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ADDR,v)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ADDR,m,v,HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_IN)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ERR_INT_STS_EN_START_BIT_ERROR_BMSK                       0x200000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ERR_INT_STS_EN_START_BIT_ERROR_SHFT                           0x15
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ERR_INT_STS_EN_FIFO_FULL_WR_BMSK                          0x100000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ERR_INT_STS_EN_FIFO_FULL_WR_SHFT                              0x14
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ERR_INT_STS_EN_FIFO_EMPTY_RD_BMSK                          0x80000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ERR_INT_STS_EN_FIFO_EMPTY_RD_SHFT                             0x13
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ERR_INT_STS_EN_AUTO_CMD19_TIMEOUT_BMSK                     0x40000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ERR_INT_STS_EN_AUTO_CMD19_TIMEOUT_SHFT                        0x12
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ERR_INT_STS_EN_BOOT_TIMEOUT_BMSK                           0x20000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ERR_INT_STS_EN_BOOT_TIMEOUT_SHFT                              0x11
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ERR_INT_STS_EN_BOOT_ACK_ERR_BMSK                           0x10000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_ERR_INT_STS_EN_BOOT_ACK_ERR_SHFT                              0x10
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_NORMAL_INT_STS_EN_BOOT_ACK_REC_BMSK                            0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_STS_EN_NORMAL_INT_STS_EN_BOOT_ACK_REC_SHFT                            0x0

#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ADDR                                                    (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000300)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_OFFS                                                    (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000300)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_RMSK                                                      0x3f0001
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_POR                                                     0x00000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_POR_RMSK                                                0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ATTR                                                           0x3
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ADDR, HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_RMSK)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ADDR, m)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ADDR,v)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ADDR,m,v,HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_IN)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ERR_INT_SIGNAL_EN_START_BIT_ERROR_BMSK                    0x200000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ERR_INT_SIGNAL_EN_START_BIT_ERROR_SHFT                        0x15
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ERR_INT_SIGNAL_EN_FIFO_FULL_WR_BMSK                       0x100000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ERR_INT_SIGNAL_EN_FIFO_FULL_WR_SHFT                           0x14
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ERR_INT_SIGNAL_EN_FIFO_EMPTY_RD_BMSK                       0x80000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ERR_INT_SIGNAL_EN_FIFO_EMPTY_RD_SHFT                          0x13
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ERR_INT_SIGNAL_EN_AUTO_CMD19_TIMEOUT_BMSK                  0x40000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ERR_INT_SIGNAL_EN_AUTO_CMD19_TIMEOUT_SHFT                     0x12
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ERR_INT_SIGNAL_EN_BOOT_TIMEOUT_BMSK                        0x20000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ERR_INT_SIGNAL_EN_BOOT_TIMEOUT_SHFT                           0x11
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ERR_INT_SIGNAL_EN_BOOT_ACK_ERR_BMSK                        0x10000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_ERR_INT_SIGNAL_EN_BOOT_ACK_ERR_SHFT                           0x10
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_NORMAL_INT_SIGNAL_EN_BOOT_ACK_REC_BMSK                         0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_INT_SIG_EN_NORMAL_INT_SIGNAL_EN_BOOT_ACK_REC_SHFT                         0x0

#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DEBUG_ADDR                                                         (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000304)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DEBUG_OFFS                                                         (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000304)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DEBUG_RMSK                                                              0x1ff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DEBUG_POR                                                          0x00000000
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DEBUG_POR_RMSK                                                     0xffffffff
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DEBUG_ATTR                                                                0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DEBUG_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DEBUG_ADDR, HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DEBUG_RMSK)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DEBUG_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DEBUG_ADDR, m)
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DEBUG_IO_MACRO_SW_RST_PERIOD_BMSK                                       0x100
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DEBUG_IO_MACRO_SW_RST_PERIOD_SHFT                                         0x8
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DEBUG_MCLK_REG_WR_ACTIVE_BMSK                                            0x80
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DEBUG_MCLK_REG_WR_ACTIVE_SHFT                                             0x7
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DEBUG_PROGDONE_BMSK                                                      0x40
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DEBUG_PROGDONE_SHFT                                                       0x6
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DEBUG_DATA_END_BMSK                                                      0x20
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DEBUG_DATA_END_SHFT                                                       0x5
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DEBUG_CMD_ACTIVE_BMSK                                                    0x10
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DEBUG_CMD_ACTIVE_SHFT                                                     0x4
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DEBUG_RXACTIVE_EXT_BMSK                                                   0x8
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DEBUG_RXACTIVE_EXT_SHFT                                                   0x3
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DEBUG_RXACTIVE_BMSK                                                       0x4
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DEBUG_RXACTIVE_SHFT                                                       0x2
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DEBUG_TXACTIVE_EXT_BMSK                                                   0x2
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DEBUG_TXACTIVE_EXT_SHFT                                                   0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DEBUG_TXACTIVE_BMSK                                                       0x1
#define HWIO_SDC2_SDCC_HC_VENDOR_SPECIFIC_DEBUG_TXACTIVE_SHFT                                                       0x0

#define HWIO_SDC2_SDCC_FIFO_COUNT_ADDR                                                                       (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000308)
#define HWIO_SDC2_SDCC_FIFO_COUNT_OFFS                                                                       (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000308)
#define HWIO_SDC2_SDCC_FIFO_COUNT_RMSK                                                                        0xfffffff
#define HWIO_SDC2_SDCC_FIFO_COUNT_POR                                                                        0x00000000
#define HWIO_SDC2_SDCC_FIFO_COUNT_POR_RMSK                                                                   0xffffffff
#define HWIO_SDC2_SDCC_FIFO_COUNT_ATTR                                                                              0x1
#define HWIO_SDC2_SDCC_FIFO_COUNT_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_FIFO_COUNT_ADDR, HWIO_SDC2_SDCC_FIFO_COUNT_RMSK)
#define HWIO_SDC2_SDCC_FIFO_COUNT_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_FIFO_COUNT_ADDR, m)
#define HWIO_SDC2_SDCC_FIFO_COUNT_DATA_COUNT_BMSK                                                             0xfffffff
#define HWIO_SDC2_SDCC_FIFO_COUNT_DATA_COUNT_SHFT                                                                   0x0

#define HWIO_SDC2_SDCC_SDCC_BOOT_ADDR                                                                        (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x0000030c)
#define HWIO_SDC2_SDCC_SDCC_BOOT_OFFS                                                                        (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000030c)
#define HWIO_SDC2_SDCC_SDCC_BOOT_RMSK                                                                               0xf
#define HWIO_SDC2_SDCC_SDCC_BOOT_POR                                                                         0x00000000
#define HWIO_SDC2_SDCC_SDCC_BOOT_POR_RMSK                                                                    0xffffffff
#define HWIO_SDC2_SDCC_SDCC_BOOT_ATTR                                                                               0x3
#define HWIO_SDC2_SDCC_SDCC_BOOT_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_SDCC_BOOT_ADDR, HWIO_SDC2_SDCC_SDCC_BOOT_RMSK)
#define HWIO_SDC2_SDCC_SDCC_BOOT_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_SDCC_BOOT_ADDR, m)
#define HWIO_SDC2_SDCC_SDCC_BOOT_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_SDCC_BOOT_ADDR,v)
#define HWIO_SDC2_SDCC_SDCC_BOOT_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_SDCC_BOOT_ADDR,m,v,HWIO_SDC2_SDCC_SDCC_BOOT_IN)
#define HWIO_SDC2_SDCC_SDCC_BOOT_EARLY_ASSERT_CMD_LINE_BMSK                                                         0x8
#define HWIO_SDC2_SDCC_SDCC_BOOT_EARLY_ASSERT_CMD_LINE_SHFT                                                         0x3
#define HWIO_SDC2_SDCC_SDCC_BOOT_BOOT_ACK_EN_BMSK                                                                   0x4
#define HWIO_SDC2_SDCC_SDCC_BOOT_BOOT_ACK_EN_SHFT                                                                   0x2
#define HWIO_SDC2_SDCC_SDCC_BOOT_BOOT_EN_BMSK                                                                       0x2
#define HWIO_SDC2_SDCC_SDCC_BOOT_BOOT_EN_SHFT                                                                       0x1
#define HWIO_SDC2_SDCC_SDCC_BOOT_BOOT_MODE_BMSK                                                                     0x1
#define HWIO_SDC2_SDCC_SDCC_BOOT_BOOT_MODE_SHFT                                                                     0x0

#define HWIO_SDC2_SDCC_SDCC_BOOT_ACK_TIMER_ADDR                                                              (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000310)
#define HWIO_SDC2_SDCC_SDCC_BOOT_ACK_TIMER_OFFS                                                              (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000310)
#define HWIO_SDC2_SDCC_SDCC_BOOT_ACK_TIMER_RMSK                                                              0xffffffff
#define HWIO_SDC2_SDCC_SDCC_BOOT_ACK_TIMER_POR                                                               0x00000000
#define HWIO_SDC2_SDCC_SDCC_BOOT_ACK_TIMER_POR_RMSK                                                          0xffffffff
#define HWIO_SDC2_SDCC_SDCC_BOOT_ACK_TIMER_ATTR                                                                     0x3
#define HWIO_SDC2_SDCC_SDCC_BOOT_ACK_TIMER_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_SDCC_BOOT_ACK_TIMER_ADDR, HWIO_SDC2_SDCC_SDCC_BOOT_ACK_TIMER_RMSK)
#define HWIO_SDC2_SDCC_SDCC_BOOT_ACK_TIMER_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_SDCC_BOOT_ACK_TIMER_ADDR, m)
#define HWIO_SDC2_SDCC_SDCC_BOOT_ACK_TIMER_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_SDCC_BOOT_ACK_TIMER_ADDR,v)
#define HWIO_SDC2_SDCC_SDCC_BOOT_ACK_TIMER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_SDCC_BOOT_ACK_TIMER_ADDR,m,v,HWIO_SDC2_SDCC_SDCC_BOOT_ACK_TIMER_IN)
#define HWIO_SDC2_SDCC_SDCC_BOOT_ACK_TIMER_BOOT_ACK_TIMER_BMSK                                               0xffffffff
#define HWIO_SDC2_SDCC_SDCC_BOOT_ACK_TIMER_BOOT_ACK_TIMER_SHFT                                                      0x0

#define HWIO_SDC2_SDCC_SDCC_BOOT_DATA_TIMER_ADDR                                                             (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000314)
#define HWIO_SDC2_SDCC_SDCC_BOOT_DATA_TIMER_OFFS                                                             (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000314)
#define HWIO_SDC2_SDCC_SDCC_BOOT_DATA_TIMER_RMSK                                                             0xffffffff
#define HWIO_SDC2_SDCC_SDCC_BOOT_DATA_TIMER_POR                                                              0x00000000
#define HWIO_SDC2_SDCC_SDCC_BOOT_DATA_TIMER_POR_RMSK                                                         0xffffffff
#define HWIO_SDC2_SDCC_SDCC_BOOT_DATA_TIMER_ATTR                                                                    0x3
#define HWIO_SDC2_SDCC_SDCC_BOOT_DATA_TIMER_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_SDCC_BOOT_DATA_TIMER_ADDR, HWIO_SDC2_SDCC_SDCC_BOOT_DATA_TIMER_RMSK)
#define HWIO_SDC2_SDCC_SDCC_BOOT_DATA_TIMER_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_SDCC_BOOT_DATA_TIMER_ADDR, m)
#define HWIO_SDC2_SDCC_SDCC_BOOT_DATA_TIMER_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_SDCC_BOOT_DATA_TIMER_ADDR,v)
#define HWIO_SDC2_SDCC_SDCC_BOOT_DATA_TIMER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_SDCC_BOOT_DATA_TIMER_ADDR,m,v,HWIO_SDC2_SDCC_SDCC_BOOT_DATA_TIMER_IN)
#define HWIO_SDC2_SDCC_SDCC_BOOT_DATA_TIMER_BOOT_DATA_TIMER_BMSK                                             0xffffffff
#define HWIO_SDC2_SDCC_SDCC_BOOT_DATA_TIMER_BOOT_DATA_TIMER_SHFT                                                    0x0

#define HWIO_SDC2_SDCC_SDCC_VERSION_ADDR                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000318)
#define HWIO_SDC2_SDCC_SDCC_VERSION_OFFS                                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000318)
#define HWIO_SDC2_SDCC_SDCC_VERSION_RMSK                                                                     0xffffffff
#define HWIO_SDC2_SDCC_SDCC_VERSION_POR                                                                      0x10000072
#define HWIO_SDC2_SDCC_SDCC_VERSION_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC2_SDCC_SDCC_VERSION_ATTR                                                                            0x1
#define HWIO_SDC2_SDCC_SDCC_VERSION_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_SDCC_VERSION_ADDR, HWIO_SDC2_SDCC_SDCC_VERSION_RMSK)
#define HWIO_SDC2_SDCC_SDCC_VERSION_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_SDCC_VERSION_ADDR, m)
#define HWIO_SDC2_SDCC_SDCC_VERSION_SDCC_VERSION_BMSK                                                        0xffffffff
#define HWIO_SDC2_SDCC_SDCC_VERSION_SDCC_VERSION_SHFT                                                               0x0

#define HWIO_SDC2_SDCC_SDCC_GENERICS_ADDR                                                                    (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000320)
#define HWIO_SDC2_SDCC_SDCC_GENERICS_OFFS                                                                    (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000320)
#define HWIO_SDC2_SDCC_SDCC_GENERICS_RMSK                                                                     0x7ffffff
#define HWIO_SDC2_SDCC_SDCC_GENERICS_POR                                                                     0x00000000
#define HWIO_SDC2_SDCC_SDCC_GENERICS_POR_RMSK                                                                0xffffffff
#define HWIO_SDC2_SDCC_SDCC_GENERICS_ATTR                                                                           0x1
#define HWIO_SDC2_SDCC_SDCC_GENERICS_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_SDCC_GENERICS_ADDR, HWIO_SDC2_SDCC_SDCC_GENERICS_RMSK)
#define HWIO_SDC2_SDCC_SDCC_GENERICS_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_SDCC_GENERICS_ADDR, m)
#define HWIO_SDC2_SDCC_SDCC_GENERICS_USE_SCM_BMSK                                                             0x4000000
#define HWIO_SDC2_SDCC_SDCC_GENERICS_USE_SCM_SHFT                                                                  0x1a
#define HWIO_SDC2_SDCC_SDCC_GENERICS_USE_XPU_BMSK                                                             0x2000000
#define HWIO_SDC2_SDCC_SDCC_GENERICS_USE_XPU_SHFT                                                                  0x19
#define HWIO_SDC2_SDCC_SDCC_GENERICS_USE_VMIDMT_BMSK                                                          0x1000000
#define HWIO_SDC2_SDCC_SDCC_GENERICS_USE_VMIDMT_SHFT                                                               0x18
#define HWIO_SDC2_SDCC_SDCC_GENERICS_USE_CMD_QUE_BMSK                                                          0x800000
#define HWIO_SDC2_SDCC_SDCC_GENERICS_USE_CMD_QUE_SHFT                                                              0x17
#define HWIO_SDC2_SDCC_SDCC_GENERICS_HS400_SUPPORT_BMSK                                                        0x400000
#define HWIO_SDC2_SDCC_SDCC_GENERICS_HS400_SUPPORT_SHFT                                                            0x16
#define HWIO_SDC2_SDCC_SDCC_GENERICS_USE_ICE_BMSK                                                              0x200000
#define HWIO_SDC2_SDCC_SDCC_GENERICS_USE_ICE_SHFT                                                                  0x15
#define HWIO_SDC2_SDCC_SDCC_GENERICS_SD_DATA_WIDTH_BMSK                                                        0x1e0000
#define HWIO_SDC2_SDCC_SDCC_GENERICS_SD_DATA_WIDTH_SHFT                                                            0x11
#define HWIO_SDC2_SDCC_SDCC_GENERICS_RAM_SIZE_BMSK                                                              0x1fff0
#define HWIO_SDC2_SDCC_SDCC_GENERICS_RAM_SIZE_SHFT                                                                  0x4
#define HWIO_SDC2_SDCC_SDCC_GENERICS_NUM_OF_DEV_BMSK                                                                0xe
#define HWIO_SDC2_SDCC_SDCC_GENERICS_NUM_OF_DEV_SHFT                                                                0x1
#define HWIO_SDC2_SDCC_SDCC_GENERICS_USE_DLL_SDC4_BMSK                                                              0x1
#define HWIO_SDC2_SDCC_SDCC_GENERICS_USE_DLL_SDC4_SHFT                                                              0x0

#define HWIO_SDC2_SDCC_FIFO_STATUS_ADDR                                                                      (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000324)
#define HWIO_SDC2_SDCC_FIFO_STATUS_OFFS                                                                      (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000324)
#define HWIO_SDC2_SDCC_FIFO_STATUS_RMSK                                                                         0x7ffff
#define HWIO_SDC2_SDCC_FIFO_STATUS_POR                                                                       0x00007800
#define HWIO_SDC2_SDCC_FIFO_STATUS_POR_RMSK                                                                  0xffffffff
#define HWIO_SDC2_SDCC_FIFO_STATUS_ATTR                                                                             0x1
#define HWIO_SDC2_SDCC_FIFO_STATUS_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_FIFO_STATUS_ADDR, HWIO_SDC2_SDCC_FIFO_STATUS_RMSK)
#define HWIO_SDC2_SDCC_FIFO_STATUS_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_FIFO_STATUS_ADDR, m)
#define HWIO_SDC2_SDCC_FIFO_STATUS_RX_FIFO_512_BMSK                                                             0x40000
#define HWIO_SDC2_SDCC_FIFO_STATUS_RX_FIFO_512_SHFT                                                                0x12
#define HWIO_SDC2_SDCC_FIFO_STATUS_RX_FIFO_256_BMSK                                                             0x20000
#define HWIO_SDC2_SDCC_FIFO_STATUS_RX_FIFO_256_SHFT                                                                0x11
#define HWIO_SDC2_SDCC_FIFO_STATUS_RX_FIFO_128_BMSK                                                             0x10000
#define HWIO_SDC2_SDCC_FIFO_STATUS_RX_FIFO_128_SHFT                                                                0x10
#define HWIO_SDC2_SDCC_FIFO_STATUS_RX_FIFO_64_BMSK                                                               0x8000
#define HWIO_SDC2_SDCC_FIFO_STATUS_RX_FIFO_64_SHFT                                                                  0xf
#define HWIO_SDC2_SDCC_FIFO_STATUS_TX_FIFO_512_BMSK                                                              0x4000
#define HWIO_SDC2_SDCC_FIFO_STATUS_TX_FIFO_512_SHFT                                                                 0xe
#define HWIO_SDC2_SDCC_FIFO_STATUS_TX_FIFO_256_BMSK                                                              0x2000
#define HWIO_SDC2_SDCC_FIFO_STATUS_TX_FIFO_256_SHFT                                                                 0xd
#define HWIO_SDC2_SDCC_FIFO_STATUS_TX_FIFO_128_BMSK                                                              0x1000
#define HWIO_SDC2_SDCC_FIFO_STATUS_TX_FIFO_128_SHFT                                                                 0xc
#define HWIO_SDC2_SDCC_FIFO_STATUS_TX_FIFO_64_BMSK                                                                0x800
#define HWIO_SDC2_SDCC_FIFO_STATUS_TX_FIFO_64_SHFT                                                                  0xb
#define HWIO_SDC2_SDCC_FIFO_STATUS_FIFO_FILL_LEVEL_BMSK                                                           0x7ff
#define HWIO_SDC2_SDCC_FIFO_STATUS_FIFO_FILL_LEVEL_SHFT                                                             0x0

#define HWIO_SDC2_SDCC_IP_CATALOG_ADDR                                                                       (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000328)
#define HWIO_SDC2_SDCC_IP_CATALOG_OFFS                                                                       (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000328)
#define HWIO_SDC2_SDCC_IP_CATALOG_RMSK                                                                       0xffffffff
#define HWIO_SDC2_SDCC_IP_CATALOG_POR                                                                        0x50010000
#define HWIO_SDC2_SDCC_IP_CATALOG_POR_RMSK                                                                   0xffffffff
#define HWIO_SDC2_SDCC_IP_CATALOG_ATTR                                                                              0x1
#define HWIO_SDC2_SDCC_IP_CATALOG_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_IP_CATALOG_ADDR, HWIO_SDC2_SDCC_IP_CATALOG_RMSK)
#define HWIO_SDC2_SDCC_IP_CATALOG_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_IP_CATALOG_ADDR, m)
#define HWIO_SDC2_SDCC_IP_CATALOG_MAJOR_BMSK                                                                 0xf0000000
#define HWIO_SDC2_SDCC_IP_CATALOG_MAJOR_SHFT                                                                       0x1c
#define HWIO_SDC2_SDCC_IP_CATALOG_MINOR_BMSK                                                                  0xfff0000
#define HWIO_SDC2_SDCC_IP_CATALOG_MINOR_SHFT                                                                       0x10
#define HWIO_SDC2_SDCC_IP_CATALOG_STEP_BMSK                                                                      0xffff
#define HWIO_SDC2_SDCC_IP_CATALOG_STEP_SHFT                                                                         0x0

#define HWIO_SDC2_SDCC_TESTBUS_CONFIG_ADDR                                                                   (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x0000032c)
#define HWIO_SDC2_SDCC_TESTBUS_CONFIG_OFFS                                                                   (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000032c)
#define HWIO_SDC2_SDCC_TESTBUS_CONFIG_RMSK                                                                   0xc000ffff
#define HWIO_SDC2_SDCC_TESTBUS_CONFIG_POR                                                                    0x00000000
#define HWIO_SDC2_SDCC_TESTBUS_CONFIG_POR_RMSK                                                               0xffffffff
#define HWIO_SDC2_SDCC_TESTBUS_CONFIG_ATTR                                                                          0x3
#define HWIO_SDC2_SDCC_TESTBUS_CONFIG_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_TESTBUS_CONFIG_ADDR, HWIO_SDC2_SDCC_TESTBUS_CONFIG_RMSK)
#define HWIO_SDC2_SDCC_TESTBUS_CONFIG_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_TESTBUS_CONFIG_ADDR, m)
#define HWIO_SDC2_SDCC_TESTBUS_CONFIG_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_TESTBUS_CONFIG_ADDR,v)
#define HWIO_SDC2_SDCC_TESTBUS_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_TESTBUS_CONFIG_ADDR,m,v,HWIO_SDC2_SDCC_TESTBUS_CONFIG_IN)
#define HWIO_SDC2_SDCC_TESTBUS_CONFIG_TESTBUS_ENA_BMSK                                                       0x80000000
#define HWIO_SDC2_SDCC_TESTBUS_CONFIG_TESTBUS_ENA_SHFT                                                             0x1f
#define HWIO_SDC2_SDCC_TESTBUS_CONFIG_SW_EVENTS_EN_BMSK                                                      0x40000000
#define HWIO_SDC2_SDCC_TESTBUS_CONFIG_SW_EVENTS_EN_SHFT                                                            0x1e
#define HWIO_SDC2_SDCC_TESTBUS_CONFIG_TESTBUS_SEL_BMSK                                                           0xffff
#define HWIO_SDC2_SDCC_TESTBUS_CONFIG_TESTBUS_SEL_SHFT                                                              0x0

#define HWIO_SDC2_SDCC_CHAR_CFG_ADDR                                                                         (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000330)
#define HWIO_SDC2_SDCC_CHAR_CFG_OFFS                                                                         (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000330)
#define HWIO_SDC2_SDCC_CHAR_CFG_RMSK                                                                             0xff7f
#define HWIO_SDC2_SDCC_CHAR_CFG_POR                                                                          0x00000000
#define HWIO_SDC2_SDCC_CHAR_CFG_POR_RMSK                                                                     0xffffffff
#define HWIO_SDC2_SDCC_CHAR_CFG_ATTR                                                                                0x3
#define HWIO_SDC2_SDCC_CHAR_CFG_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_CHAR_CFG_ADDR, HWIO_SDC2_SDCC_CHAR_CFG_RMSK)
#define HWIO_SDC2_SDCC_CHAR_CFG_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_CHAR_CFG_ADDR, m)
#define HWIO_SDC2_SDCC_CHAR_CFG_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_CHAR_CFG_ADDR,v)
#define HWIO_SDC2_SDCC_CHAR_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_CHAR_CFG_ADDR,m,v,HWIO_SDC2_SDCC_CHAR_CFG_IN)
#define HWIO_SDC2_SDCC_CHAR_CFG_CHAR_MODE_BMSK                                                                   0xf000
#define HWIO_SDC2_SDCC_CHAR_CFG_CHAR_MODE_SHFT                                                                      0xc
#define HWIO_SDC2_SDCC_CHAR_CFG_CHAR_STATUS_BMSK                                                                  0xf00
#define HWIO_SDC2_SDCC_CHAR_CFG_CHAR_STATUS_SHFT                                                                    0x8
#define HWIO_SDC2_SDCC_CHAR_CFG_DLL_INV_CLK_BMSK                                                                   0x40
#define HWIO_SDC2_SDCC_CHAR_CFG_DLL_INV_CLK_SHFT                                                                    0x6
#define HWIO_SDC2_SDCC_CHAR_CFG_DIRECTION_BMSK                                                                     0x20
#define HWIO_SDC2_SDCC_CHAR_CFG_DIRECTION_SHFT                                                                      0x5
#define HWIO_SDC2_SDCC_CHAR_CFG_LOOPBACK_TEST_STATUS_BMSK                                                          0x10
#define HWIO_SDC2_SDCC_CHAR_CFG_LOOPBACK_TEST_STATUS_SHFT                                                           0x4
#define HWIO_SDC2_SDCC_CHAR_CFG_LOOPBACK_TEST_DONE_BMSK                                                             0x8
#define HWIO_SDC2_SDCC_CHAR_CFG_LOOPBACK_TEST_DONE_SHFT                                                             0x3
#define HWIO_SDC2_SDCC_CHAR_CFG_LOOPBACK_MODE_EN_BMSK                                                               0x4
#define HWIO_SDC2_SDCC_CHAR_CFG_LOOPBACK_MODE_EN_SHFT                                                               0x2
#define HWIO_SDC2_SDCC_CHAR_CFG_CHAR_TEST_START_BMSK                                                                0x2
#define HWIO_SDC2_SDCC_CHAR_CFG_CHAR_TEST_START_SHFT                                                                0x1
#define HWIO_SDC2_SDCC_CHAR_CFG_CHAR_TEST_MODE_EN_BMSK                                                              0x1
#define HWIO_SDC2_SDCC_CHAR_CFG_CHAR_TEST_MODE_EN_SHFT                                                              0x0

#define HWIO_SDC2_SDCC_CHAR_CMD_ADDR                                                                         (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000334)
#define HWIO_SDC2_SDCC_CHAR_CMD_OFFS                                                                         (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000334)
#define HWIO_SDC2_SDCC_CHAR_CMD_RMSK                                                                             0xffff
#define HWIO_SDC2_SDCC_CHAR_CMD_POR                                                                          0x00000000
#define HWIO_SDC2_SDCC_CHAR_CMD_POR_RMSK                                                                     0xffffffff
#define HWIO_SDC2_SDCC_CHAR_CMD_ATTR                                                                                0x3
#define HWIO_SDC2_SDCC_CHAR_CMD_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_CHAR_CMD_ADDR, HWIO_SDC2_SDCC_CHAR_CMD_RMSK)
#define HWIO_SDC2_SDCC_CHAR_CMD_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_CHAR_CMD_ADDR, m)
#define HWIO_SDC2_SDCC_CHAR_CMD_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_CHAR_CMD_ADDR,v)
#define HWIO_SDC2_SDCC_CHAR_CMD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_CHAR_CMD_ADDR,m,v,HWIO_SDC2_SDCC_CHAR_CMD_IN)
#define HWIO_SDC2_SDCC_CHAR_CMD_CMDIN_ACTUAL_BMSK                                                                0xff00
#define HWIO_SDC2_SDCC_CHAR_CMD_CMDIN_ACTUAL_SHFT                                                                   0x8
#define HWIO_SDC2_SDCC_CHAR_CMD_CMDOUT_DATA_DIN_EXP_BMSK                                                           0xff
#define HWIO_SDC2_SDCC_CHAR_CMD_CMDOUT_DATA_DIN_EXP_SHFT                                                            0x0

#define HWIO_SDC2_SDCC_CHAR_DATA_n_ADDR(n)                                                                   (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000338 + 0x4 * (n))
#define HWIO_SDC2_SDCC_CHAR_DATA_n_OFFS(n)                                                                   (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000338 + 0x4 * (n))
#define HWIO_SDC2_SDCC_CHAR_DATA_n_RMSK                                                                          0xffff
#define HWIO_SDC2_SDCC_CHAR_DATA_n_MAXn                                                                               7
#define HWIO_SDC2_SDCC_CHAR_DATA_n_POR                                                                       0x00000000
#define HWIO_SDC2_SDCC_CHAR_DATA_n_POR_RMSK                                                                  0xffffffff
#define HWIO_SDC2_SDCC_CHAR_DATA_n_ATTR                                                                             0x3
#define HWIO_SDC2_SDCC_CHAR_DATA_n_INI(n)        \
        in_dword_masked(HWIO_SDC2_SDCC_CHAR_DATA_n_ADDR(n), HWIO_SDC2_SDCC_CHAR_DATA_n_RMSK)
#define HWIO_SDC2_SDCC_CHAR_DATA_n_INMI(n,mask)    \
        in_dword_masked(HWIO_SDC2_SDCC_CHAR_DATA_n_ADDR(n), mask)
#define HWIO_SDC2_SDCC_CHAR_DATA_n_OUTI(n,val)    \
        out_dword(HWIO_SDC2_SDCC_CHAR_DATA_n_ADDR(n),val)
#define HWIO_SDC2_SDCC_CHAR_DATA_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_CHAR_DATA_n_ADDR(n),mask,val,HWIO_SDC2_SDCC_CHAR_DATA_n_INI(n))
#define HWIO_SDC2_SDCC_CHAR_DATA_n_DIN_ACTUAL_BMSK                                                               0xff00
#define HWIO_SDC2_SDCC_CHAR_DATA_n_DIN_ACTUAL_SHFT                                                                  0x8
#define HWIO_SDC2_SDCC_CHAR_DATA_n_DOUT_DATA_DIN_EXP_BMSK                                                          0xff
#define HWIO_SDC2_SDCC_CHAR_DATA_n_DOUT_DATA_DIN_EXP_SHFT                                                           0x0

#define HWIO_SDC2_SDCC_DEBUG_REG_ADDR                                                                        (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000358)
#define HWIO_SDC2_SDCC_DEBUG_REG_OFFS                                                                        (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000358)
#define HWIO_SDC2_SDCC_DEBUG_REG_RMSK                                                                        0xffffffff
#define HWIO_SDC2_SDCC_DEBUG_REG_POR                                                                         0x00000000
#define HWIO_SDC2_SDCC_DEBUG_REG_POR_RMSK                                                                    0xffffffff
#define HWIO_SDC2_SDCC_DEBUG_REG_ATTR                                                                               0x1
#define HWIO_SDC2_SDCC_DEBUG_REG_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_DEBUG_REG_ADDR, HWIO_SDC2_SDCC_DEBUG_REG_RMSK)
#define HWIO_SDC2_SDCC_DEBUG_REG_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_DEBUG_REG_ADDR, m)
#define HWIO_SDC2_SDCC_DEBUG_REG_TEST_BUS_BMSK                                                               0xffffffff
#define HWIO_SDC2_SDCC_DEBUG_REG_TEST_BUS_SHFT                                                                      0x0

#define HWIO_SDC2_SDCC_DATA_COUNT_ADDR                                                                       (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x0000035c)
#define HWIO_SDC2_SDCC_DATA_COUNT_OFFS                                                                       (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000035c)
#define HWIO_SDC2_SDCC_DATA_COUNT_RMSK                                                                        0xfffffff
#define HWIO_SDC2_SDCC_DATA_COUNT_POR                                                                        0x00000000
#define HWIO_SDC2_SDCC_DATA_COUNT_POR_RMSK                                                                   0xffffffff
#define HWIO_SDC2_SDCC_DATA_COUNT_ATTR                                                                              0x1
#define HWIO_SDC2_SDCC_DATA_COUNT_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_DATA_COUNT_ADDR, HWIO_SDC2_SDCC_DATA_COUNT_RMSK)
#define HWIO_SDC2_SDCC_DATA_COUNT_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_DATA_COUNT_ADDR, m)
#define HWIO_SDC2_SDCC_DATA_COUNT_DATACOUNT_BMSK                                                              0xfffffff
#define HWIO_SDC2_SDCC_DATA_COUNT_DATACOUNT_SHFT                                                                    0x0

#define HWIO_SDC2_SDCC_RAM_DATA_OUT_LSB_ADDR                                                                 (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000360)
#define HWIO_SDC2_SDCC_RAM_DATA_OUT_LSB_OFFS                                                                 (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000360)
#define HWIO_SDC2_SDCC_RAM_DATA_OUT_LSB_RMSK                                                                 0xffffffff
#define HWIO_SDC2_SDCC_RAM_DATA_OUT_LSB_POR                                                                  0x00000000
#define HWIO_SDC2_SDCC_RAM_DATA_OUT_LSB_POR_RMSK                                                             0xffffffff
#define HWIO_SDC2_SDCC_RAM_DATA_OUT_LSB_ATTR                                                                        0x1
#define HWIO_SDC2_SDCC_RAM_DATA_OUT_LSB_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_RAM_DATA_OUT_LSB_ADDR, HWIO_SDC2_SDCC_RAM_DATA_OUT_LSB_RMSK)
#define HWIO_SDC2_SDCC_RAM_DATA_OUT_LSB_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_RAM_DATA_OUT_LSB_ADDR, m)
#define HWIO_SDC2_SDCC_RAM_DATA_OUT_LSB_RAM_DATA_BMSK                                                        0xffffffff
#define HWIO_SDC2_SDCC_RAM_DATA_OUT_LSB_RAM_DATA_SHFT                                                               0x0

#define HWIO_SDC2_SDCC_RAM_DATA_OUT_MSB_ADDR                                                                 (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000364)
#define HWIO_SDC2_SDCC_RAM_DATA_OUT_MSB_OFFS                                                                 (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000364)
#define HWIO_SDC2_SDCC_RAM_DATA_OUT_MSB_RMSK                                                                 0xffffffff
#define HWIO_SDC2_SDCC_RAM_DATA_OUT_MSB_POR                                                                  0x00000000
#define HWIO_SDC2_SDCC_RAM_DATA_OUT_MSB_POR_RMSK                                                             0xffffffff
#define HWIO_SDC2_SDCC_RAM_DATA_OUT_MSB_ATTR                                                                        0x1
#define HWIO_SDC2_SDCC_RAM_DATA_OUT_MSB_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_RAM_DATA_OUT_MSB_ADDR, HWIO_SDC2_SDCC_RAM_DATA_OUT_MSB_RMSK)
#define HWIO_SDC2_SDCC_RAM_DATA_OUT_MSB_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_RAM_DATA_OUT_MSB_ADDR, m)
#define HWIO_SDC2_SDCC_RAM_DATA_OUT_MSB_RAM_DATA_BMSK                                                        0xffffffff
#define HWIO_SDC2_SDCC_RAM_DATA_OUT_MSB_RAM_DATA_SHFT                                                               0x0

#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_ADDR_ADDR                                                            (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000368)
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_ADDR_OFFS                                                            (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000368)
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_ADDR_RMSK                                                            0xffffffff
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_ADDR_POR                                                             0x00000000
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_ADDR_POR_RMSK                                                        0xffffffff
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_ADDR_ATTR                                                                   0x1
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_ADDR_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_ADDR_ADDR, HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_ADDR_RMSK)
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_ADDR_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_ADDR_ADDR, m)
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_ADDR_DESC_ADDRESS_LSB_BMSK                                           0xffffffff
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_ADDR_DESC_ADDRESS_LSB_SHFT                                                  0x0

#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_INFO_ADDR                                                            (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x0000036c)
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_INFO_OFFS                                                            (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000036c)
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_INFO_RMSK                                                            0xf33fffff
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_INFO_POR                                                             0x00000000
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_INFO_POR_RMSK                                                        0xffffffff
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_INFO_ATTR                                                                   0x1
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_INFO_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_INFO_ADDR, HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_INFO_RMSK)
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_INFO_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_INFO_ADDR, m)
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_ADDRESS_MSB_BMSK                                           0xf0000000
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_ADDRESS_MSB_SHFT                                                 0x1c
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_CNT_BMSK                                                    0x3000000
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_CNT_SHFT                                                         0x18
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_MAX_LENGTH_BMSK                                              0x200000
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_MAX_LENGTH_SHFT                                                  0x15
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_ACT_BMSK                                                     0x180000
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_ACT_SHFT                                                         0x13
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_INT_BMSK                                                      0x40000
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_INT_SHFT                                                         0x12
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_EOT_BMSK                                                      0x20000
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_EOT_SHFT                                                         0x11
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_VALID_BMSK                                                    0x10000
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_VALID_SHFT                                                       0x10
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_LENGTH_BMSK                                                    0xffff
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR2_INFO_DESC_LENGTH_SHFT                                                       0x0

#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_ADDR_ADDR                                                            (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000370)
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_ADDR_OFFS                                                            (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000370)
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_ADDR_RMSK                                                            0xffffffff
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_ADDR_POR                                                             0x00000000
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_ADDR_POR_RMSK                                                        0xffffffff
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_ADDR_ATTR                                                                   0x1
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_ADDR_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_ADDR_ADDR, HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_ADDR_RMSK)
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_ADDR_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_ADDR_ADDR, m)
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_ADDR_DESC_ADDRESS_LSB_BMSK                                           0xffffffff
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_ADDR_DESC_ADDRESS_LSB_SHFT                                                  0x0

#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_INFO_ADDR                                                            (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000374)
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_INFO_OFFS                                                            (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000374)
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_INFO_RMSK                                                            0xf33fffff
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_INFO_POR                                                             0x00000000
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_INFO_POR_RMSK                                                        0xffffffff
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_INFO_ATTR                                                                   0x1
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_INFO_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_INFO_ADDR, HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_INFO_RMSK)
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_INFO_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_INFO_ADDR, m)
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_ADDRESS_MSB_BMSK                                           0xf0000000
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_ADDRESS_MSB_SHFT                                                 0x1c
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_CNT_BMSK                                                    0x3000000
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_CNT_SHFT                                                         0x18
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_MAX_LENGTH_BMSK                                              0x200000
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_MAX_LENGTH_SHFT                                                  0x15
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_ACT_BMSK                                                     0x180000
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_ACT_SHFT                                                         0x13
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_INT_BMSK                                                      0x40000
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_INT_SHFT                                                         0x12
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_EOT_BMSK                                                      0x20000
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_EOT_SHFT                                                         0x11
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_VALID_BMSK                                                    0x10000
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_VALID_SHFT                                                       0x10
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_LENGTH_BMSK                                                    0xffff
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR1_INFO_DESC_LENGTH_SHFT                                                       0x0

#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_ADDR_ADDR                                                            (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000378)
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_ADDR_OFFS                                                            (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000378)
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_ADDR_RMSK                                                            0xffffffff
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_ADDR_POR                                                             0x00000000
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_ADDR_POR_RMSK                                                        0xffffffff
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_ADDR_ATTR                                                                   0x1
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_ADDR_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_ADDR_ADDR, HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_ADDR_RMSK)
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_ADDR_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_ADDR_ADDR, m)
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_ADDR_DESC_ADDRESS_LSB_BMSK                                           0xffffffff
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_ADDR_DESC_ADDRESS_LSB_SHFT                                                  0x0

#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_INFO_ADDR                                                            (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x0000037c)
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_INFO_OFFS                                                            (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000037c)
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_INFO_RMSK                                                            0xf33fffff
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_INFO_POR                                                             0x00000000
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_INFO_POR_RMSK                                                        0xffffffff
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_INFO_ATTR                                                                   0x1
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_INFO_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_INFO_ADDR, HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_INFO_RMSK)
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_INFO_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_INFO_ADDR, m)
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_ADDRESS_MSB_BMSK                                           0xf0000000
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_ADDRESS_MSB_SHFT                                                 0x1c
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_CNT_BMSK                                                    0x3000000
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_CNT_SHFT                                                         0x18
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_MAX_LENGTH_BMSK                                              0x200000
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_MAX_LENGTH_SHFT                                                  0x15
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_ACT_BMSK                                                     0x180000
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_ACT_SHFT                                                         0x13
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_INT_BMSK                                                      0x40000
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_INT_SHFT                                                         0x12
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_EOT_BMSK                                                      0x20000
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_EOT_SHFT                                                         0x11
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_VALID_BMSK                                                    0x10000
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_VALID_SHFT                                                       0x10
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_LENGTH_BMSK                                                    0xffff
#define HWIO_SDC2_SDCC_SDCC_DESCRIPTOR0_INFO_DESC_LENGTH_SHFT                                                       0x0

#define HWIO_SDC2_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_ADDR                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000380)
#define HWIO_SDC2_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_OFFS                                                     (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000380)
#define HWIO_SDC2_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_RMSK                                                     0xbfffffff
#define HWIO_SDC2_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_POR                                                      0x00000000
#define HWIO_SDC2_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_POR_RMSK                                                 0xffffffff
#define HWIO_SDC2_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_ATTR                                                            0x3
#define HWIO_SDC2_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_ADDR, HWIO_SDC2_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_RMSK)
#define HWIO_SDC2_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_ADDR, m)
#define HWIO_SDC2_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_ADDR,v)
#define HWIO_SDC2_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_ADDR,m,v,HWIO_SDC2_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_IN)
#define HWIO_SDC2_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_TIMER_EN_BMSK                                            0x80000000
#define HWIO_SDC2_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_TIMER_EN_SHFT                                                  0x1f
#define HWIO_SDC2_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_DATA_PRESENT_BMSK                                        0x20000000
#define HWIO_SDC2_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_DATA_PRESENT_SHFT                                              0x1d
#define HWIO_SDC2_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_TIMER_RST_BMSK                                           0x10000000
#define HWIO_SDC2_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_TIMER_RST_SHFT                                                 0x1c
#define HWIO_SDC2_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_TIMER_STOP_BMSK                                           0x8000000
#define HWIO_SDC2_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_TIMER_STOP_SHFT                                                0x1b
#define HWIO_SDC2_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_TIMER_VALUE_BMSK                                          0x7ffffff
#define HWIO_SDC2_SDCC_SDCC_INTERRUPT_LATENCY_TIMER_TIMER_VALUE_SHFT                                                0x0

#define HWIO_SDC2_SDCC_SDCC_RAM_ADDR_SW_CTL_ADDR                                                             (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000384)
#define HWIO_SDC2_SDCC_SDCC_RAM_ADDR_SW_CTL_OFFS                                                             (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000384)
#define HWIO_SDC2_SDCC_SDCC_RAM_ADDR_SW_CTL_RMSK                                                             0xc07fffff
#define HWIO_SDC2_SDCC_SDCC_RAM_ADDR_SW_CTL_POR                                                              0x00000000
#define HWIO_SDC2_SDCC_SDCC_RAM_ADDR_SW_CTL_POR_RMSK                                                         0xffffffff
#define HWIO_SDC2_SDCC_SDCC_RAM_ADDR_SW_CTL_ATTR                                                                    0x3
#define HWIO_SDC2_SDCC_SDCC_RAM_ADDR_SW_CTL_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_SDCC_RAM_ADDR_SW_CTL_ADDR, HWIO_SDC2_SDCC_SDCC_RAM_ADDR_SW_CTL_RMSK)
#define HWIO_SDC2_SDCC_SDCC_RAM_ADDR_SW_CTL_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_SDCC_RAM_ADDR_SW_CTL_ADDR, m)
#define HWIO_SDC2_SDCC_SDCC_RAM_ADDR_SW_CTL_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_SDCC_RAM_ADDR_SW_CTL_ADDR,v)
#define HWIO_SDC2_SDCC_SDCC_RAM_ADDR_SW_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_SDCC_RAM_ADDR_SW_CTL_ADDR,m,v,HWIO_SDC2_SDCC_SDCC_RAM_ADDR_SW_CTL_IN)
#define HWIO_SDC2_SDCC_SDCC_RAM_ADDR_SW_CTL_SW_RAM_RD_ADDRESS_ENABLE_BMSK                                    0x80000000
#define HWIO_SDC2_SDCC_SDCC_RAM_ADDR_SW_CTL_SW_RAM_RD_ADDRESS_ENABLE_SHFT                                          0x1f
#define HWIO_SDC2_SDCC_SDCC_RAM_ADDR_SW_CTL_SW_RAM_RD_ADDRESS_READ_ENABLE_BMSK                               0x40000000
#define HWIO_SDC2_SDCC_SDCC_RAM_ADDR_SW_CTL_SW_RAM_RD_ADDRESS_READ_ENABLE_SHFT                                     0x1e
#define HWIO_SDC2_SDCC_SDCC_RAM_ADDR_SW_CTL_SW_RAM_RD_ADDRESS_BMSK                                             0x7f0000
#define HWIO_SDC2_SDCC_SDCC_RAM_ADDR_SW_CTL_SW_RAM_RD_ADDRESS_SHFT                                                 0x10
#define HWIO_SDC2_SDCC_SDCC_RAM_ADDR_SW_CTL_FUNC_RAM_WR_ADDRESS_BMSK                                             0xff00
#define HWIO_SDC2_SDCC_SDCC_RAM_ADDR_SW_CTL_FUNC_RAM_WR_ADDRESS_SHFT                                                0x8
#define HWIO_SDC2_SDCC_SDCC_RAM_ADDR_SW_CTL_FUNC_RAM_RD_ADDRESS_BMSK                                               0xff
#define HWIO_SDC2_SDCC_SDCC_RAM_ADDR_SW_CTL_FUNC_RAM_RD_ADDRESS_SHFT                                                0x0

#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_ADDR                                                              (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000388)
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_OFFS                                                              (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000388)
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_RMSK                                                              0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_POR                                                               0x00010800
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_POR_RMSK                                                          0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_ATTR                                                                     0x3
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_ADDR, HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_OUT(v)      \
        out_dword(HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_ADDR,v)
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_ADDR,m,v,HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_IN)
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_COARSE_TUNING_CTRL_BMSK                                           0x80000000
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_COARSE_TUNING_CTRL_SHFT                                                 0x1f
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_DRR_BYPASS_EN_BMSK                                                0x40000000
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_DRR_BYPASS_EN_SHFT                                                      0x1e
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_BIAS_OK_SIGNAL_BMSK                                               0x20000000
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_BIAS_OK_SIGNAL_SHFT                                                     0x1d
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_FLL_MODE_EN_BMSK                                                  0x10000000
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_FLL_MODE_EN_SHFT                                                        0x1c
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_FINE_TUNE_MODE_EN_BMSK                                             0x8000000
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_FINE_TUNE_MODE_EN_SHFT                                                  0x1b
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_TEST_CONFIG_BMSK                                                   0x7000000
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_TEST_CONFIG_SHFT                                                        0x18
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_RDAC_CONFIG_BMSK                                                    0xc00000
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_RDAC_CONFIG_SHFT                                                        0x16
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_REF_VOLT_CONFIG_BMSK                                                0x300000
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_REF_VOLT_CONFIG_SHFT                                                    0x14
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_FREEZE_CDAC_OP_BMSK                                                  0x80000
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_FREEZE_CDAC_OP_SHFT                                                     0x13
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_FREEZE_RDAC_OP_BMSK                                                  0x40000
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_FREEZE_RDAC_OP_SHFT                                                     0x12
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_RESET_SDR_DAT_OUT_BMSK                                               0x20000
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_RESET_SDR_DAT_OUT_SHFT                                                  0x11
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_DELAY_LINE_OSC_MODE_BMSK                                             0x10000
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_DELAY_LINE_OSC_MODE_SHFT                                                0x10
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_EDGE_LEVEL_BMSK                                                       0xc000
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_EDGE_LEVEL_SHFT                                                          0xe
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_PROG_DLY_DLL_UP_DN_BMSK                                               0x3000
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_PROG_DLY_DLL_UP_DN_SHFT                                                  0xc
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_RDAC_INIT_CODE_BMSK                                                    0xff0
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_RDAC_INIT_CODE_SHFT                                                      0x4
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_BIST_ERR_INJECT_BMSK                                                     0xf
#define HWIO_SDC2_SDCC_HC_REG_DLL_USER_CTL_BIST_ERR_INJECT_SHFT                                                     0x0

#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_EXT_ADDR                                                            (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x0000038c)
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_EXT_OFFS                                                            (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x0000038c)
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_EXT_RMSK                                                             0x1ffffff
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_EXT_POR                                                             0x00000000
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_EXT_POR_RMSK                                                        0xffffffff
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_EXT_ATTR                                                                   0x1
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_EXT_IN          \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_EXT_ADDR, HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_EXT_RMSK)
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_EXT_INM(m)      \
        in_dword_masked(HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_EXT_ADDR, m)
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_EXT_SDC4_DLL_LOCK_BMSK                                               0x1000000
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_EXT_SDC4_DLL_LOCK_SHFT                                                    0x18
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_EXT_SDC4_DLL_STATUS_EXT_BMSK                                          0xffffff
#define HWIO_SDC2_SDCC_HC_REG_DLL_STATUS_EXT_SDC4_DLL_STATUS_EXT_SHFT                                               0x0

#define HWIO_SDC2_SDCC_HC_FIFO_ALTn_ADDR(n)                                                                  (SDC2_SDCC_SDCC5_HC_REG_BASE      + 0x00000400 + 0x4 * (n))
#define HWIO_SDC2_SDCC_HC_FIFO_ALTn_OFFS(n)                                                                  (SDC2_SDCC_SDCC5_HC_REG_BASE_OFFS + 0x00000400 + 0x4 * (n))
#define HWIO_SDC2_SDCC_HC_FIFO_ALTn_RMSK                                                                     0xffffffff
#define HWIO_SDC2_SDCC_HC_FIFO_ALTn_MAXn                                                                            255
#define HWIO_SDC2_SDCC_HC_FIFO_ALTn_POR                                                                      0x00000000
#define HWIO_SDC2_SDCC_HC_FIFO_ALTn_POR_RMSK                                                                 0xffffffff
#define HWIO_SDC2_SDCC_HC_FIFO_ALTn_ATTR                                                                            0x3
#define HWIO_SDC2_SDCC_HC_FIFO_ALTn_INI(n)        \
        in_dword_masked(HWIO_SDC2_SDCC_HC_FIFO_ALTn_ADDR(n), HWIO_SDC2_SDCC_HC_FIFO_ALTn_RMSK)
#define HWIO_SDC2_SDCC_HC_FIFO_ALTn_INMI(n,mask)    \
        in_dword_masked(HWIO_SDC2_SDCC_HC_FIFO_ALTn_ADDR(n), mask)
#define HWIO_SDC2_SDCC_HC_FIFO_ALTn_OUTI(n,val)    \
        out_dword(HWIO_SDC2_SDCC_HC_FIFO_ALTn_ADDR(n),val)
#define HWIO_SDC2_SDCC_HC_FIFO_ALTn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_SDC2_SDCC_HC_FIFO_ALTn_ADDR(n),mask,val,HWIO_SDC2_SDCC_HC_FIFO_ALTn_INI(n))
#define HWIO_SDC2_SDCC_HC_FIFO_ALTn_DATA_BMSK                                                                0xffffffff
#define HWIO_SDC2_SDCC_HC_FIFO_ALTn_DATA_SHFT                                                                       0x0


#endif /* __HWIO_SDCC_BASE_H__ */
