#ifndef __HWIO_TLMM_SDCC_H__
#define __HWIO_TLMM_SDCC_H__
/*
===========================================================================
*/
/**
  @file hwio_tlmm_sdcc.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    QCS405 (Vipertooth) [vipertooth_prj_1.0_p3q3r103]
 
  This file contains HWIO register definitions for the following modules:
    TLMM_SOUTH


  Generation parameters: 
  { u'filename': u'hwio_tlmm_sdcc.h',
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [u'TLMM_SOUTH']}
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

  $Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Settings/SDCC/inc/hwio_tlmm_sdcc.h#1 $
  $DateTime: 2018/08/08 05:31:55 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: TLMM_SOUTH
 *--------------------------------------------------------------------------*/

#define TLMM_SOUTH_REG_BASE                                                                 (TLMM_SOUTH_BASE      + 0x00000000)
#define TLMM_SOUTH_REG_BASE_SIZE                                                            0x200000
#define TLMM_SOUTH_REG_BASE_USED                                                            0x1c0004

#define HWIO_TLMM_NAND_HDRV_PULL_CTL_ADDR                                                   (TLMM_SOUTH_REG_BASE      + 0x000c0000)
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_RMSK                                                   0xffffffff
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_IN          \
        in_dword_masked(HWIO_TLMM_NAND_HDRV_PULL_CTL_ADDR, HWIO_TLMM_NAND_HDRV_PULL_CTL_RMSK)
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_NAND_HDRV_PULL_CTL_ADDR, m)
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_NAND_HDRV_PULL_CTL_ADDR,v)
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_NAND_HDRV_PULL_CTL_ADDR,m,v,HWIO_TLMM_NAND_HDRV_PULL_CTL_IN)
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_NAND_RDY_PULL_BMSK                                     0xc0000000
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_NAND_RDY_PULL_SHFT                                           0x1e
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_NAND_CLE_HDRV_BMSK                                     0x38000000
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_NAND_CLE_HDRV_SHFT                                           0x1b
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_NAND_CLE_PULL_BMSK                                      0x6000000
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_NAND_CLE_PULL_SHFT                                           0x19
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_NAND_ALE_HDRV_BMSK                                      0x1c00000
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_NAND_ALE_HDRV_SHFT                                           0x16
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_NAND_ALE_PULL_BMSK                                       0x300000
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_NAND_ALE_PULL_SHFT                                           0x14
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_NAND_OE_N_HDRV_BMSK                                       0xe0000
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_NAND_OE_N_HDRV_SHFT                                          0x11
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_NAND_OE_N_PULL_BMSK                                       0x18000
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_NAND_OE_N_PULL_SHFT                                           0xf
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_NAND_WE_N_PULL_BMSK                                        0x6000
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_NAND_WE_N_PULL_SHFT                                           0xd
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_NAND_CS_N_PULL_BMSK                                        0x1800
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_NAND_CS_N_PULL_SHFT                                           0xb
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_NAND_IO_PULL_BMSK                                           0x600
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_NAND_IO_PULL_SHFT                                             0x9
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_NAND_WE_N_HDRV_BMSK                                         0x1c0
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_NAND_WE_N_HDRV_SHFT                                           0x6
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_NAND_CS_N_HDRV_BMSK                                          0x38
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_NAND_CS_N_HDRV_SHFT                                           0x3
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_NAND_IO_HDRV_BMSK                                             0x7
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_NAND_IO_HDRV_SHFT                                             0x0

#define HWIO_TLMM_NAND_HDRV_PULL_CTL_BOOT_SEL_ADDR                                          (TLMM_SOUTH_REG_BASE      + 0x000c1000)
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_BOOT_SEL_RMSK                                          0xffffffff
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_BOOT_SEL_IN          \
        in_dword_masked(HWIO_TLMM_NAND_HDRV_PULL_CTL_BOOT_SEL_ADDR, HWIO_TLMM_NAND_HDRV_PULL_CTL_BOOT_SEL_RMSK)
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_BOOT_SEL_INM(m)      \
        in_dword_masked(HWIO_TLMM_NAND_HDRV_PULL_CTL_BOOT_SEL_ADDR, m)
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_BOOT_SEL_OUT(v)      \
        out_dword(HWIO_TLMM_NAND_HDRV_PULL_CTL_BOOT_SEL_ADDR,v)
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_BOOT_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_NAND_HDRV_PULL_CTL_BOOT_SEL_ADDR,m,v,HWIO_TLMM_NAND_HDRV_PULL_CTL_BOOT_SEL_IN)
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_BOOT_SEL_NAND_RDY_HDRV_BMSK                            0xe0000000
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_BOOT_SEL_NAND_RDY_HDRV_SHFT                                  0x1d
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_BOOT_SEL_EBI2_BOOT_BMSK                                0x10000000
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_BOOT_SEL_EBI2_BOOT_SHFT                                      0x1c
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_BOOT_SEL_EMMC_BOOT_BMSK                                 0x8000000
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_BOOT_SEL_EMMC_BOOT_SHFT                                      0x1b
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_BOOT_SEL_MISC2_BMSK                                     0x7ffffff
#define HWIO_TLMM_NAND_HDRV_PULL_CTL_BOOT_SEL_MISC2_SHFT                                           0x0

#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR                                                   (TLMM_SOUTH_REG_BASE      + 0x000c2000)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_RMSK                                                      0x1ffff
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_IN          \
        in_dword_masked(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR, HWIO_TLMM_SDC1_HDRV_PULL_CTL_RMSK)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR, m)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR,v)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_SDC1_HDRV_PULL_CTL_ADDR,m,v,HWIO_TLMM_SDC1_HDRV_PULL_CTL_IN)
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_RCLK_PULL_BMSK                                       0x18000
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_RCLK_PULL_SHFT                                           0xf
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CLK_PULL_BMSK                                         0x6000
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CLK_PULL_SHFT                                            0xd
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CMD_PULL_BMSK                                         0x1800
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CMD_PULL_SHFT                                            0xb
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_DATA_PULL_BMSK                                         0x600
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_DATA_PULL_SHFT                                           0x9
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CLK_HDRV_BMSK                                          0x1c0
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CLK_HDRV_SHFT                                            0x6
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CMD_HDRV_BMSK                                           0x38
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_CMD_HDRV_SHFT                                            0x3
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_DATA_HDRV_BMSK                                           0x7
#define HWIO_TLMM_SDC1_HDRV_PULL_CTL_SDC1_DATA_HDRV_SHFT                                           0x0

#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR                                                   (TLMM_SOUTH_REG_BASE      + 0x000c3000)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_RMSK                                                       0xffff
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_IN          \
        in_dword_masked(HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR, HWIO_TLMM_SDC2_HDRV_PULL_CTL_RMSK)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR, m)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR,v)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR,m,v,HWIO_TLMM_SDC2_HDRV_PULL_CTL_IN)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CLK_PULL_BMSK                                         0xc000
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CLK_PULL_SHFT                                            0xe
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_HYS_CTL_BMSK                                          0x2000
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_HYS_CTL_SHFT                                             0xd
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CMD_PULL_BMSK                                         0x1800
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CMD_PULL_SHFT                                            0xb
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_DATA_PULL_BMSK                                         0x600
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_DATA_PULL_SHFT                                           0x9
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CLK_HDRV_BMSK                                          0x1c0
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CLK_HDRV_SHFT                                            0x6
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CMD_HDRV_BMSK                                           0x38
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CMD_HDRV_SHFT                                            0x3
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_DATA_HDRV_BMSK                                           0x7
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_DATA_HDRV_SHFT                                           0x0

#endif /* __HWIO_TLMM_SDCC_H__ */
