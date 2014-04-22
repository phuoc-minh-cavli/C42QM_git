#ifndef __HALTLMMTILEHWIO_H__
#define __HALTLMMTILEHWIO_H__
/*
===========================================================================
*/
/**
  @file HALtlmmtileHWIO.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    TLMM_CSR

  'Exclude' filters applied: .*_END$ .*_END_ADDRESS$ .*_SIZE$ EBI1_MEM_RPM_REGION. 

  Generation parameters: 
  { 'check-for-overlaps': True,
    'check-sizes': True,
    'filename': 'HALtlmmtileHWIO.h',
    'filter-exclude': [ '.*_END$',
                        '.*_END_ADDRESS$',
                        '.*_SIZE$',
                        'EBI1_MEM_RPM_REGION.'],
    'header': '\n#include "msmhwiobase.h"\n',
    'map-type': 'physical',
    'modules': ['TLMM_CSR'],
    'unroll-array': [ 'TLMM_GPIO_CFGn',
                      'TLMM_GPIO_INTR_CFGn',
                      'TLMM_DIR_CONN_INTRn_CFG_LPA_DSP',
                      'TLMM_GPIO_IN_OUTn',
                      'TLMM_GPIO_INTR_STATUSn']}
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

  $Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Library/TLMMTargetLib/HALtlmmtileHWIO.h#2 $
  $DateTime: 2018/08/31 03:22:47 $
  $Author: pwbldsvc $

  ===========================================================================
*/


#include "msmhwiobase.h"


/*----------------------------------------------------------------------------
 * MODULE: TLMM_CSR
 *--------------------------------------------------------------------------*/

#define TLMM_CSR_REG_BASE                                                   (TLMM_BASE      + 0x00000000)
#define TLMM_CSR_REG_BASE_SIZE                                              0x300000
#define TLMM_CSR_REG_BASE_USED                                              0x2000c4

#define HWIO_TLMM_GPIO_CFGn_ADDR(n)                                         (TLMM_CSR_REG_BASE      + 0x00000000 + 0x1000 * (n))
#define HWIO_TLMM_GPIO_CFGn_RMSK                                                 0x7ff
#define HWIO_TLMM_GPIO_CFGn_MAXn                                                    42
#define HWIO_TLMM_GPIO_CFGn_INI(n)        \
        in_dword_masked(HWIO_TLMM_GPIO_CFGn_ADDR(n), HWIO_TLMM_GPIO_CFGn_RMSK)
#define HWIO_TLMM_GPIO_CFGn_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_CFGn_ADDR(n), mask)
#define HWIO_TLMM_GPIO_CFGn_OUTI(n,val)    \
        out_dword(HWIO_TLMM_GPIO_CFGn_ADDR(n),val)
#define HWIO_TLMM_GPIO_CFGn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFGn_ADDR(n),mask,val,HWIO_TLMM_GPIO_CFGn_INI(n))
#define HWIO_TLMM_GPIO_CFGn_GPIO_HIHYS_EN_BMSK                                   0x400
#define HWIO_TLMM_GPIO_CFGn_GPIO_HIHYS_EN_SHFT                                     0xa
#define HWIO_TLMM_GPIO_CFGn_GPIO_OE_BMSK                                         0x200
#define HWIO_TLMM_GPIO_CFGn_GPIO_OE_SHFT                                           0x9
#define HWIO_TLMM_GPIO_CFGn_DRV_STRENGTH_BMSK                                    0x1c0
#define HWIO_TLMM_GPIO_CFGn_DRV_STRENGTH_SHFT                                      0x6
#define HWIO_TLMM_GPIO_CFGn_FUNC_SEL_BMSK                                         0x3c
#define HWIO_TLMM_GPIO_CFGn_FUNC_SEL_SHFT                                          0x2
#define HWIO_TLMM_GPIO_CFGn_GPIO_PULL_BMSK                                         0x3
#define HWIO_TLMM_GPIO_CFGn_GPIO_PULL_SHFT                                         0x0

#define HWIO_TLMM_GPIO_CFG0_ADDR                                            (TLMM_CSR_REG_BASE      + 0x00000000)
#define HWIO_TLMM_GPIO_CFG0_RMSK                                                 0x7ff
#define HWIO_TLMM_GPIO_CFG0_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG0_ADDR)
#define HWIO_TLMM_GPIO_CFG0_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG0_ADDR, m)
#define HWIO_TLMM_GPIO_CFG0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG0_ADDR,v)
#define HWIO_TLMM_GPIO_CFG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG0_ADDR,m,v,HWIO_TLMM_GPIO_CFG0_IN)
#define HWIO_TLMM_GPIO_CFG0_GPIO_HIHYS_EN_BMSK                                   0x400
#define HWIO_TLMM_GPIO_CFG0_GPIO_HIHYS_EN_SHFT                                     0xa
#define HWIO_TLMM_GPIO_CFG0_GPIO_OE_BMSK                                         0x200
#define HWIO_TLMM_GPIO_CFG0_GPIO_OE_SHFT                                           0x9
#define HWIO_TLMM_GPIO_CFG0_DRV_STRENGTH_BMSK                                    0x1c0
#define HWIO_TLMM_GPIO_CFG0_DRV_STRENGTH_SHFT                                      0x6
#define HWIO_TLMM_GPIO_CFG0_FUNC_SEL_BMSK                                         0x3c
#define HWIO_TLMM_GPIO_CFG0_FUNC_SEL_SHFT                                          0x2
#define HWIO_TLMM_GPIO_CFG0_GPIO_PULL_BMSK                                         0x3
#define HWIO_TLMM_GPIO_CFG0_GPIO_PULL_SHFT                                         0x0

#define HWIO_TLMM_GPIO_CFG1_ADDR                                            (TLMM_CSR_REG_BASE      + 0x00001000)
#define HWIO_TLMM_GPIO_CFG1_RMSK                                                 0x7ff
#define HWIO_TLMM_GPIO_CFG1_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG1_ADDR)
#define HWIO_TLMM_GPIO_CFG1_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG1_ADDR, m)
#define HWIO_TLMM_GPIO_CFG1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG1_ADDR,v)
#define HWIO_TLMM_GPIO_CFG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG1_ADDR,m,v,HWIO_TLMM_GPIO_CFG1_IN)
#define HWIO_TLMM_GPIO_CFG1_GPIO_HIHYS_EN_BMSK                                   0x400
#define HWIO_TLMM_GPIO_CFG1_GPIO_HIHYS_EN_SHFT                                     0xa
#define HWIO_TLMM_GPIO_CFG1_GPIO_OE_BMSK                                         0x200
#define HWIO_TLMM_GPIO_CFG1_GPIO_OE_SHFT                                           0x9
#define HWIO_TLMM_GPIO_CFG1_DRV_STRENGTH_BMSK                                    0x1c0
#define HWIO_TLMM_GPIO_CFG1_DRV_STRENGTH_SHFT                                      0x6
#define HWIO_TLMM_GPIO_CFG1_FUNC_SEL_BMSK                                         0x3c
#define HWIO_TLMM_GPIO_CFG1_FUNC_SEL_SHFT                                          0x2
#define HWIO_TLMM_GPIO_CFG1_GPIO_PULL_BMSK                                         0x3
#define HWIO_TLMM_GPIO_CFG1_GPIO_PULL_SHFT                                         0x0

#define HWIO_TLMM_GPIO_CFG2_ADDR                                            (TLMM_CSR_REG_BASE      + 0x00002000)
#define HWIO_TLMM_GPIO_CFG2_RMSK                                                 0x7ff
#define HWIO_TLMM_GPIO_CFG2_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG2_ADDR)
#define HWIO_TLMM_GPIO_CFG2_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG2_ADDR, m)
#define HWIO_TLMM_GPIO_CFG2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG2_ADDR,v)
#define HWIO_TLMM_GPIO_CFG2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG2_ADDR,m,v,HWIO_TLMM_GPIO_CFG2_IN)
#define HWIO_TLMM_GPIO_CFG2_GPIO_HIHYS_EN_BMSK                                   0x400
#define HWIO_TLMM_GPIO_CFG2_GPIO_HIHYS_EN_SHFT                                     0xa
#define HWIO_TLMM_GPIO_CFG2_GPIO_OE_BMSK                                         0x200
#define HWIO_TLMM_GPIO_CFG2_GPIO_OE_SHFT                                           0x9
#define HWIO_TLMM_GPIO_CFG2_DRV_STRENGTH_BMSK                                    0x1c0
#define HWIO_TLMM_GPIO_CFG2_DRV_STRENGTH_SHFT                                      0x6
#define HWIO_TLMM_GPIO_CFG2_FUNC_SEL_BMSK                                         0x3c
#define HWIO_TLMM_GPIO_CFG2_FUNC_SEL_SHFT                                          0x2
#define HWIO_TLMM_GPIO_CFG2_GPIO_PULL_BMSK                                         0x3
#define HWIO_TLMM_GPIO_CFG2_GPIO_PULL_SHFT                                         0x0

#define HWIO_TLMM_GPIO_CFG3_ADDR                                            (TLMM_CSR_REG_BASE      + 0x00003000)
#define HWIO_TLMM_GPIO_CFG3_RMSK                                                 0x7ff
#define HWIO_TLMM_GPIO_CFG3_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG3_ADDR)
#define HWIO_TLMM_GPIO_CFG3_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG3_ADDR, m)
#define HWIO_TLMM_GPIO_CFG3_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG3_ADDR,v)
#define HWIO_TLMM_GPIO_CFG3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG3_ADDR,m,v,HWIO_TLMM_GPIO_CFG3_IN)
#define HWIO_TLMM_GPIO_CFG3_GPIO_HIHYS_EN_BMSK                                   0x400
#define HWIO_TLMM_GPIO_CFG3_GPIO_HIHYS_EN_SHFT                                     0xa
#define HWIO_TLMM_GPIO_CFG3_GPIO_OE_BMSK                                         0x200
#define HWIO_TLMM_GPIO_CFG3_GPIO_OE_SHFT                                           0x9
#define HWIO_TLMM_GPIO_CFG3_DRV_STRENGTH_BMSK                                    0x1c0
#define HWIO_TLMM_GPIO_CFG3_DRV_STRENGTH_SHFT                                      0x6
#define HWIO_TLMM_GPIO_CFG3_FUNC_SEL_BMSK                                         0x3c
#define HWIO_TLMM_GPIO_CFG3_FUNC_SEL_SHFT                                          0x2
#define HWIO_TLMM_GPIO_CFG3_GPIO_PULL_BMSK                                         0x3
#define HWIO_TLMM_GPIO_CFG3_GPIO_PULL_SHFT                                         0x0

#define HWIO_TLMM_GPIO_CFG4_ADDR                                            (TLMM_CSR_REG_BASE      + 0x00004000)
#define HWIO_TLMM_GPIO_CFG4_RMSK                                                 0x7ff
#define HWIO_TLMM_GPIO_CFG4_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG4_ADDR)
#define HWIO_TLMM_GPIO_CFG4_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG4_ADDR, m)
#define HWIO_TLMM_GPIO_CFG4_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG4_ADDR,v)
#define HWIO_TLMM_GPIO_CFG4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG4_ADDR,m,v,HWIO_TLMM_GPIO_CFG4_IN)
#define HWIO_TLMM_GPIO_CFG4_GPIO_HIHYS_EN_BMSK                                   0x400
#define HWIO_TLMM_GPIO_CFG4_GPIO_HIHYS_EN_SHFT                                     0xa
#define HWIO_TLMM_GPIO_CFG4_GPIO_OE_BMSK                                         0x200
#define HWIO_TLMM_GPIO_CFG4_GPIO_OE_SHFT                                           0x9
#define HWIO_TLMM_GPIO_CFG4_DRV_STRENGTH_BMSK                                    0x1c0
#define HWIO_TLMM_GPIO_CFG4_DRV_STRENGTH_SHFT                                      0x6
#define HWIO_TLMM_GPIO_CFG4_FUNC_SEL_BMSK                                         0x3c
#define HWIO_TLMM_GPIO_CFG4_FUNC_SEL_SHFT                                          0x2
#define HWIO_TLMM_GPIO_CFG4_GPIO_PULL_BMSK                                         0x3
#define HWIO_TLMM_GPIO_CFG4_GPIO_PULL_SHFT                                         0x0

#define HWIO_TLMM_GPIO_CFG5_ADDR                                            (TLMM_CSR_REG_BASE      + 0x00005000)
#define HWIO_TLMM_GPIO_CFG5_RMSK                                                 0x7ff
#define HWIO_TLMM_GPIO_CFG5_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG5_ADDR)
#define HWIO_TLMM_GPIO_CFG5_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG5_ADDR, m)
#define HWIO_TLMM_GPIO_CFG5_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG5_ADDR,v)
#define HWIO_TLMM_GPIO_CFG5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG5_ADDR,m,v,HWIO_TLMM_GPIO_CFG5_IN)
#define HWIO_TLMM_GPIO_CFG5_GPIO_HIHYS_EN_BMSK                                   0x400
#define HWIO_TLMM_GPIO_CFG5_GPIO_HIHYS_EN_SHFT                                     0xa
#define HWIO_TLMM_GPIO_CFG5_GPIO_OE_BMSK                                         0x200
#define HWIO_TLMM_GPIO_CFG5_GPIO_OE_SHFT                                           0x9
#define HWIO_TLMM_GPIO_CFG5_DRV_STRENGTH_BMSK                                    0x1c0
#define HWIO_TLMM_GPIO_CFG5_DRV_STRENGTH_SHFT                                      0x6
#define HWIO_TLMM_GPIO_CFG5_FUNC_SEL_BMSK                                         0x3c
#define HWIO_TLMM_GPIO_CFG5_FUNC_SEL_SHFT                                          0x2
#define HWIO_TLMM_GPIO_CFG5_GPIO_PULL_BMSK                                         0x3
#define HWIO_TLMM_GPIO_CFG5_GPIO_PULL_SHFT                                         0x0

#define HWIO_TLMM_GPIO_CFG6_ADDR                                            (TLMM_CSR_REG_BASE      + 0x00006000)
#define HWIO_TLMM_GPIO_CFG6_RMSK                                                 0x7ff
#define HWIO_TLMM_GPIO_CFG6_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG6_ADDR)
#define HWIO_TLMM_GPIO_CFG6_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG6_ADDR, m)
#define HWIO_TLMM_GPIO_CFG6_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG6_ADDR,v)
#define HWIO_TLMM_GPIO_CFG6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG6_ADDR,m,v,HWIO_TLMM_GPIO_CFG6_IN)
#define HWIO_TLMM_GPIO_CFG6_GPIO_HIHYS_EN_BMSK                                   0x400
#define HWIO_TLMM_GPIO_CFG6_GPIO_HIHYS_EN_SHFT                                     0xa
#define HWIO_TLMM_GPIO_CFG6_GPIO_OE_BMSK                                         0x200
#define HWIO_TLMM_GPIO_CFG6_GPIO_OE_SHFT                                           0x9
#define HWIO_TLMM_GPIO_CFG6_DRV_STRENGTH_BMSK                                    0x1c0
#define HWIO_TLMM_GPIO_CFG6_DRV_STRENGTH_SHFT                                      0x6
#define HWIO_TLMM_GPIO_CFG6_FUNC_SEL_BMSK                                         0x3c
#define HWIO_TLMM_GPIO_CFG6_FUNC_SEL_SHFT                                          0x2
#define HWIO_TLMM_GPIO_CFG6_GPIO_PULL_BMSK                                         0x3
#define HWIO_TLMM_GPIO_CFG6_GPIO_PULL_SHFT                                         0x0

#define HWIO_TLMM_GPIO_CFG7_ADDR                                            (TLMM_CSR_REG_BASE      + 0x00007000)
#define HWIO_TLMM_GPIO_CFG7_RMSK                                                 0x7ff
#define HWIO_TLMM_GPIO_CFG7_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG7_ADDR)
#define HWIO_TLMM_GPIO_CFG7_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG7_ADDR, m)
#define HWIO_TLMM_GPIO_CFG7_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG7_ADDR,v)
#define HWIO_TLMM_GPIO_CFG7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG7_ADDR,m,v,HWIO_TLMM_GPIO_CFG7_IN)
#define HWIO_TLMM_GPIO_CFG7_GPIO_HIHYS_EN_BMSK                                   0x400
#define HWIO_TLMM_GPIO_CFG7_GPIO_HIHYS_EN_SHFT                                     0xa
#define HWIO_TLMM_GPIO_CFG7_GPIO_OE_BMSK                                         0x200
#define HWIO_TLMM_GPIO_CFG7_GPIO_OE_SHFT                                           0x9
#define HWIO_TLMM_GPIO_CFG7_DRV_STRENGTH_BMSK                                    0x1c0
#define HWIO_TLMM_GPIO_CFG7_DRV_STRENGTH_SHFT                                      0x6
#define HWIO_TLMM_GPIO_CFG7_FUNC_SEL_BMSK                                         0x3c
#define HWIO_TLMM_GPIO_CFG7_FUNC_SEL_SHFT                                          0x2
#define HWIO_TLMM_GPIO_CFG7_GPIO_PULL_BMSK                                         0x3
#define HWIO_TLMM_GPIO_CFG7_GPIO_PULL_SHFT                                         0x0

#define HWIO_TLMM_GPIO_CFG8_ADDR                                            (TLMM_CSR_REG_BASE      + 0x00008000)
#define HWIO_TLMM_GPIO_CFG8_RMSK                                                 0x7ff
#define HWIO_TLMM_GPIO_CFG8_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG8_ADDR)
#define HWIO_TLMM_GPIO_CFG8_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG8_ADDR, m)
#define HWIO_TLMM_GPIO_CFG8_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG8_ADDR,v)
#define HWIO_TLMM_GPIO_CFG8_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG8_ADDR,m,v,HWIO_TLMM_GPIO_CFG8_IN)
#define HWIO_TLMM_GPIO_CFG8_GPIO_HIHYS_EN_BMSK                                   0x400
#define HWIO_TLMM_GPIO_CFG8_GPIO_HIHYS_EN_SHFT                                     0xa
#define HWIO_TLMM_GPIO_CFG8_GPIO_OE_BMSK                                         0x200
#define HWIO_TLMM_GPIO_CFG8_GPIO_OE_SHFT                                           0x9
#define HWIO_TLMM_GPIO_CFG8_DRV_STRENGTH_BMSK                                    0x1c0
#define HWIO_TLMM_GPIO_CFG8_DRV_STRENGTH_SHFT                                      0x6
#define HWIO_TLMM_GPIO_CFG8_FUNC_SEL_BMSK                                         0x3c
#define HWIO_TLMM_GPIO_CFG8_FUNC_SEL_SHFT                                          0x2
#define HWIO_TLMM_GPIO_CFG8_GPIO_PULL_BMSK                                         0x3
#define HWIO_TLMM_GPIO_CFG8_GPIO_PULL_SHFT                                         0x0

#define HWIO_TLMM_GPIO_CFG9_ADDR                                            (TLMM_CSR_REG_BASE      + 0x00009000)
#define HWIO_TLMM_GPIO_CFG9_RMSK                                                 0x7ff
#define HWIO_TLMM_GPIO_CFG9_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG9_ADDR)
#define HWIO_TLMM_GPIO_CFG9_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG9_ADDR, m)
#define HWIO_TLMM_GPIO_CFG9_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG9_ADDR,v)
#define HWIO_TLMM_GPIO_CFG9_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG9_ADDR,m,v,HWIO_TLMM_GPIO_CFG9_IN)
#define HWIO_TLMM_GPIO_CFG9_GPIO_HIHYS_EN_BMSK                                   0x400
#define HWIO_TLMM_GPIO_CFG9_GPIO_HIHYS_EN_SHFT                                     0xa
#define HWIO_TLMM_GPIO_CFG9_GPIO_OE_BMSK                                         0x200
#define HWIO_TLMM_GPIO_CFG9_GPIO_OE_SHFT                                           0x9
#define HWIO_TLMM_GPIO_CFG9_DRV_STRENGTH_BMSK                                    0x1c0
#define HWIO_TLMM_GPIO_CFG9_DRV_STRENGTH_SHFT                                      0x6
#define HWIO_TLMM_GPIO_CFG9_FUNC_SEL_BMSK                                         0x3c
#define HWIO_TLMM_GPIO_CFG9_FUNC_SEL_SHFT                                          0x2
#define HWIO_TLMM_GPIO_CFG9_GPIO_PULL_BMSK                                         0x3
#define HWIO_TLMM_GPIO_CFG9_GPIO_PULL_SHFT                                         0x0

#define HWIO_TLMM_GPIO_CFG10_ADDR                                           (TLMM_CSR_REG_BASE      + 0x0000a000)
#define HWIO_TLMM_GPIO_CFG10_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG10_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG10_ADDR)
#define HWIO_TLMM_GPIO_CFG10_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG10_ADDR, m)
#define HWIO_TLMM_GPIO_CFG10_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG10_ADDR,v)
#define HWIO_TLMM_GPIO_CFG10_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG10_ADDR,m,v,HWIO_TLMM_GPIO_CFG10_IN)
#define HWIO_TLMM_GPIO_CFG10_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG10_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG10_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG10_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG10_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG10_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG10_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG10_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG10_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG10_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG11_ADDR                                           (TLMM_CSR_REG_BASE      + 0x0000b000)
#define HWIO_TLMM_GPIO_CFG11_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG11_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG11_ADDR)
#define HWIO_TLMM_GPIO_CFG11_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG11_ADDR, m)
#define HWIO_TLMM_GPIO_CFG11_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG11_ADDR,v)
#define HWIO_TLMM_GPIO_CFG11_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG11_ADDR,m,v,HWIO_TLMM_GPIO_CFG11_IN)
#define HWIO_TLMM_GPIO_CFG11_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG11_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG11_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG11_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG11_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG11_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG11_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG11_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG11_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG11_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG12_ADDR                                           (TLMM_CSR_REG_BASE      + 0x0000c000)
#define HWIO_TLMM_GPIO_CFG12_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG12_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG12_ADDR)
#define HWIO_TLMM_GPIO_CFG12_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG12_ADDR, m)
#define HWIO_TLMM_GPIO_CFG12_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG12_ADDR,v)
#define HWIO_TLMM_GPIO_CFG12_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG12_ADDR,m,v,HWIO_TLMM_GPIO_CFG12_IN)
#define HWIO_TLMM_GPIO_CFG12_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG12_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG12_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG12_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG12_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG12_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG12_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG12_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG12_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG12_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG13_ADDR                                           (TLMM_CSR_REG_BASE      + 0x0000d000)
#define HWIO_TLMM_GPIO_CFG13_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG13_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG13_ADDR)
#define HWIO_TLMM_GPIO_CFG13_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG13_ADDR, m)
#define HWIO_TLMM_GPIO_CFG13_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG13_ADDR,v)
#define HWIO_TLMM_GPIO_CFG13_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG13_ADDR,m,v,HWIO_TLMM_GPIO_CFG13_IN)
#define HWIO_TLMM_GPIO_CFG13_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG13_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG13_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG13_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG13_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG13_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG13_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG13_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG13_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG13_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG14_ADDR                                           (TLMM_CSR_REG_BASE      + 0x0000e000)
#define HWIO_TLMM_GPIO_CFG14_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG14_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG14_ADDR)
#define HWIO_TLMM_GPIO_CFG14_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG14_ADDR, m)
#define HWIO_TLMM_GPIO_CFG14_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG14_ADDR,v)
#define HWIO_TLMM_GPIO_CFG14_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG14_ADDR,m,v,HWIO_TLMM_GPIO_CFG14_IN)
#define HWIO_TLMM_GPIO_CFG14_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG14_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG14_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG14_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG14_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG14_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG14_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG14_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG14_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG14_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG15_ADDR                                           (TLMM_CSR_REG_BASE      + 0x0000f000)
#define HWIO_TLMM_GPIO_CFG15_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG15_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG15_ADDR)
#define HWIO_TLMM_GPIO_CFG15_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG15_ADDR, m)
#define HWIO_TLMM_GPIO_CFG15_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG15_ADDR,v)
#define HWIO_TLMM_GPIO_CFG15_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG15_ADDR,m,v,HWIO_TLMM_GPIO_CFG15_IN)
#define HWIO_TLMM_GPIO_CFG15_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG15_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG15_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG15_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG15_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG15_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG15_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG15_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG15_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG15_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG16_ADDR                                           (TLMM_CSR_REG_BASE      + 0x00010000)
#define HWIO_TLMM_GPIO_CFG16_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG16_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG16_ADDR)
#define HWIO_TLMM_GPIO_CFG16_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG16_ADDR, m)
#define HWIO_TLMM_GPIO_CFG16_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG16_ADDR,v)
#define HWIO_TLMM_GPIO_CFG16_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG16_ADDR,m,v,HWIO_TLMM_GPIO_CFG16_IN)
#define HWIO_TLMM_GPIO_CFG16_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG16_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG16_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG16_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG16_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG16_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG16_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG16_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG16_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG16_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG17_ADDR                                           (TLMM_CSR_REG_BASE      + 0x00011000)
#define HWIO_TLMM_GPIO_CFG17_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG17_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG17_ADDR)
#define HWIO_TLMM_GPIO_CFG17_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG17_ADDR, m)
#define HWIO_TLMM_GPIO_CFG17_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG17_ADDR,v)
#define HWIO_TLMM_GPIO_CFG17_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG17_ADDR,m,v,HWIO_TLMM_GPIO_CFG17_IN)
#define HWIO_TLMM_GPIO_CFG17_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG17_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG17_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG17_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG17_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG17_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG17_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG17_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG17_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG17_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG18_ADDR                                           (TLMM_CSR_REG_BASE      + 0x00012000)
#define HWIO_TLMM_GPIO_CFG18_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG18_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG18_ADDR)
#define HWIO_TLMM_GPIO_CFG18_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG18_ADDR, m)
#define HWIO_TLMM_GPIO_CFG18_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG18_ADDR,v)
#define HWIO_TLMM_GPIO_CFG18_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG18_ADDR,m,v,HWIO_TLMM_GPIO_CFG18_IN)
#define HWIO_TLMM_GPIO_CFG18_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG18_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG18_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG18_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG18_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG18_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG18_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG18_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG18_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG18_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG19_ADDR                                           (TLMM_CSR_REG_BASE      + 0x00013000)
#define HWIO_TLMM_GPIO_CFG19_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG19_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG19_ADDR)
#define HWIO_TLMM_GPIO_CFG19_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG19_ADDR, m)
#define HWIO_TLMM_GPIO_CFG19_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG19_ADDR,v)
#define HWIO_TLMM_GPIO_CFG19_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG19_ADDR,m,v,HWIO_TLMM_GPIO_CFG19_IN)
#define HWIO_TLMM_GPIO_CFG19_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG19_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG19_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG19_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG19_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG19_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG19_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG19_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG19_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG19_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG20_ADDR                                           (TLMM_CSR_REG_BASE      + 0x00014000)
#define HWIO_TLMM_GPIO_CFG20_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG20_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG20_ADDR)
#define HWIO_TLMM_GPIO_CFG20_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG20_ADDR, m)
#define HWIO_TLMM_GPIO_CFG20_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG20_ADDR,v)
#define HWIO_TLMM_GPIO_CFG20_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG20_ADDR,m,v,HWIO_TLMM_GPIO_CFG20_IN)
#define HWIO_TLMM_GPIO_CFG20_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG20_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG20_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG20_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG20_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG20_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG20_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG20_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG20_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG20_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG21_ADDR                                           (TLMM_CSR_REG_BASE      + 0x00015000)
#define HWIO_TLMM_GPIO_CFG21_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG21_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG21_ADDR)
#define HWIO_TLMM_GPIO_CFG21_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG21_ADDR, m)
#define HWIO_TLMM_GPIO_CFG21_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG21_ADDR,v)
#define HWIO_TLMM_GPIO_CFG21_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG21_ADDR,m,v,HWIO_TLMM_GPIO_CFG21_IN)
#define HWIO_TLMM_GPIO_CFG21_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG21_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG21_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG21_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG21_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG21_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG21_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG21_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG21_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG21_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG22_ADDR                                           (TLMM_CSR_REG_BASE      + 0x00016000)
#define HWIO_TLMM_GPIO_CFG22_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG22_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG22_ADDR)
#define HWIO_TLMM_GPIO_CFG22_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG22_ADDR, m)
#define HWIO_TLMM_GPIO_CFG22_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG22_ADDR,v)
#define HWIO_TLMM_GPIO_CFG22_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG22_ADDR,m,v,HWIO_TLMM_GPIO_CFG22_IN)
#define HWIO_TLMM_GPIO_CFG22_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG22_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG22_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG22_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG22_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG22_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG22_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG22_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG22_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG22_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG23_ADDR                                           (TLMM_CSR_REG_BASE      + 0x00017000)
#define HWIO_TLMM_GPIO_CFG23_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG23_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG23_ADDR)
#define HWIO_TLMM_GPIO_CFG23_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG23_ADDR, m)
#define HWIO_TLMM_GPIO_CFG23_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG23_ADDR,v)
#define HWIO_TLMM_GPIO_CFG23_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG23_ADDR,m,v,HWIO_TLMM_GPIO_CFG23_IN)
#define HWIO_TLMM_GPIO_CFG23_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG23_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG23_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG23_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG23_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG23_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG23_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG23_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG23_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG23_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG24_ADDR                                           (TLMM_CSR_REG_BASE      + 0x00018000)
#define HWIO_TLMM_GPIO_CFG24_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG24_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG24_ADDR)
#define HWIO_TLMM_GPIO_CFG24_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG24_ADDR, m)
#define HWIO_TLMM_GPIO_CFG24_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG24_ADDR,v)
#define HWIO_TLMM_GPIO_CFG24_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG24_ADDR,m,v,HWIO_TLMM_GPIO_CFG24_IN)
#define HWIO_TLMM_GPIO_CFG24_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG24_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG24_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG24_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG24_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG24_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG24_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG24_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG24_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG24_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG25_ADDR                                           (TLMM_CSR_REG_BASE      + 0x00019000)
#define HWIO_TLMM_GPIO_CFG25_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG25_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG25_ADDR)
#define HWIO_TLMM_GPIO_CFG25_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG25_ADDR, m)
#define HWIO_TLMM_GPIO_CFG25_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG25_ADDR,v)
#define HWIO_TLMM_GPIO_CFG25_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG25_ADDR,m,v,HWIO_TLMM_GPIO_CFG25_IN)
#define HWIO_TLMM_GPIO_CFG25_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG25_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG25_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG25_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG25_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG25_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG25_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG25_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG25_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG25_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG26_ADDR                                           (TLMM_CSR_REG_BASE      + 0x0001a000)
#define HWIO_TLMM_GPIO_CFG26_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG26_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG26_ADDR)
#define HWIO_TLMM_GPIO_CFG26_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG26_ADDR, m)
#define HWIO_TLMM_GPIO_CFG26_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG26_ADDR,v)
#define HWIO_TLMM_GPIO_CFG26_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG26_ADDR,m,v,HWIO_TLMM_GPIO_CFG26_IN)
#define HWIO_TLMM_GPIO_CFG26_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG26_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG26_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG26_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG26_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG26_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG26_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG26_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG26_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG26_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG27_ADDR                                           (TLMM_CSR_REG_BASE      + 0x0001b000)
#define HWIO_TLMM_GPIO_CFG27_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG27_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG27_ADDR)
#define HWIO_TLMM_GPIO_CFG27_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG27_ADDR, m)
#define HWIO_TLMM_GPIO_CFG27_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG27_ADDR,v)
#define HWIO_TLMM_GPIO_CFG27_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG27_ADDR,m,v,HWIO_TLMM_GPIO_CFG27_IN)
#define HWIO_TLMM_GPIO_CFG27_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG27_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG27_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG27_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG27_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG27_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG27_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG27_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG27_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG27_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG28_ADDR                                           (TLMM_CSR_REG_BASE      + 0x0001c000)
#define HWIO_TLMM_GPIO_CFG28_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG28_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG28_ADDR)
#define HWIO_TLMM_GPIO_CFG28_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG28_ADDR, m)
#define HWIO_TLMM_GPIO_CFG28_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG28_ADDR,v)
#define HWIO_TLMM_GPIO_CFG28_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG28_ADDR,m,v,HWIO_TLMM_GPIO_CFG28_IN)
#define HWIO_TLMM_GPIO_CFG28_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG28_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG28_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG28_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG28_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG28_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG28_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG28_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG28_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG28_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG29_ADDR                                           (TLMM_CSR_REG_BASE      + 0x0001d000)
#define HWIO_TLMM_GPIO_CFG29_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG29_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG29_ADDR)
#define HWIO_TLMM_GPIO_CFG29_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG29_ADDR, m)
#define HWIO_TLMM_GPIO_CFG29_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG29_ADDR,v)
#define HWIO_TLMM_GPIO_CFG29_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG29_ADDR,m,v,HWIO_TLMM_GPIO_CFG29_IN)
#define HWIO_TLMM_GPIO_CFG29_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG29_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG29_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG29_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG29_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG29_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG29_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG29_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG29_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG29_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG30_ADDR                                           (TLMM_CSR_REG_BASE      + 0x0001e000)
#define HWIO_TLMM_GPIO_CFG30_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG30_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG30_ADDR)
#define HWIO_TLMM_GPIO_CFG30_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG30_ADDR, m)
#define HWIO_TLMM_GPIO_CFG30_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG30_ADDR,v)
#define HWIO_TLMM_GPIO_CFG30_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG30_ADDR,m,v,HWIO_TLMM_GPIO_CFG30_IN)
#define HWIO_TLMM_GPIO_CFG30_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG30_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG30_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG30_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG30_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG30_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG30_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG30_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG30_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG30_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG31_ADDR                                           (TLMM_CSR_REG_BASE      + 0x0001f000)
#define HWIO_TLMM_GPIO_CFG31_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG31_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG31_ADDR)
#define HWIO_TLMM_GPIO_CFG31_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG31_ADDR, m)
#define HWIO_TLMM_GPIO_CFG31_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG31_ADDR,v)
#define HWIO_TLMM_GPIO_CFG31_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG31_ADDR,m,v,HWIO_TLMM_GPIO_CFG31_IN)
#define HWIO_TLMM_GPIO_CFG31_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG31_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG31_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG31_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG31_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG31_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG31_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG31_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG31_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG31_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG32_ADDR                                           (TLMM_CSR_REG_BASE      + 0x00020000)
#define HWIO_TLMM_GPIO_CFG32_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG32_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG32_ADDR)
#define HWIO_TLMM_GPIO_CFG32_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG32_ADDR, m)
#define HWIO_TLMM_GPIO_CFG32_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG32_ADDR,v)
#define HWIO_TLMM_GPIO_CFG32_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG32_ADDR,m,v,HWIO_TLMM_GPIO_CFG32_IN)
#define HWIO_TLMM_GPIO_CFG32_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG32_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG32_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG32_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG32_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG32_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG32_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG32_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG32_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG32_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG33_ADDR                                           (TLMM_CSR_REG_BASE      + 0x00021000)
#define HWIO_TLMM_GPIO_CFG33_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG33_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG33_ADDR)
#define HWIO_TLMM_GPIO_CFG33_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG33_ADDR, m)
#define HWIO_TLMM_GPIO_CFG33_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG33_ADDR,v)
#define HWIO_TLMM_GPIO_CFG33_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG33_ADDR,m,v,HWIO_TLMM_GPIO_CFG33_IN)
#define HWIO_TLMM_GPIO_CFG33_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG33_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG33_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG33_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG33_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG33_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG33_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG33_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG33_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG33_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG34_ADDR                                           (TLMM_CSR_REG_BASE      + 0x00022000)
#define HWIO_TLMM_GPIO_CFG34_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG34_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG34_ADDR)
#define HWIO_TLMM_GPIO_CFG34_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG34_ADDR, m)
#define HWIO_TLMM_GPIO_CFG34_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG34_ADDR,v)
#define HWIO_TLMM_GPIO_CFG34_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG34_ADDR,m,v,HWIO_TLMM_GPIO_CFG34_IN)
#define HWIO_TLMM_GPIO_CFG34_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG34_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG34_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG34_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG34_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG34_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG34_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG34_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG34_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG34_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG35_ADDR                                           (TLMM_CSR_REG_BASE      + 0x00023000)
#define HWIO_TLMM_GPIO_CFG35_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG35_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG35_ADDR)
#define HWIO_TLMM_GPIO_CFG35_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG35_ADDR, m)
#define HWIO_TLMM_GPIO_CFG35_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG35_ADDR,v)
#define HWIO_TLMM_GPIO_CFG35_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG35_ADDR,m,v,HWIO_TLMM_GPIO_CFG35_IN)
#define HWIO_TLMM_GPIO_CFG35_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG35_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG35_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG35_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG35_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG35_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG35_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG35_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG35_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG35_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG36_ADDR                                           (TLMM_CSR_REG_BASE      + 0x00024000)
#define HWIO_TLMM_GPIO_CFG36_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG36_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG36_ADDR)
#define HWIO_TLMM_GPIO_CFG36_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG36_ADDR, m)
#define HWIO_TLMM_GPIO_CFG36_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG36_ADDR,v)
#define HWIO_TLMM_GPIO_CFG36_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG36_ADDR,m,v,HWIO_TLMM_GPIO_CFG36_IN)
#define HWIO_TLMM_GPIO_CFG36_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG36_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG36_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG36_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG36_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG36_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG36_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG36_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG36_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG36_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG37_ADDR                                           (TLMM_CSR_REG_BASE      + 0x00025000)
#define HWIO_TLMM_GPIO_CFG37_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG37_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG37_ADDR)
#define HWIO_TLMM_GPIO_CFG37_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG37_ADDR, m)
#define HWIO_TLMM_GPIO_CFG37_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG37_ADDR,v)
#define HWIO_TLMM_GPIO_CFG37_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG37_ADDR,m,v,HWIO_TLMM_GPIO_CFG37_IN)
#define HWIO_TLMM_GPIO_CFG37_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG37_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG37_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG37_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG37_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG37_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG37_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG37_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG37_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG37_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG38_ADDR                                           (TLMM_CSR_REG_BASE      + 0x00026000)
#define HWIO_TLMM_GPIO_CFG38_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG38_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG38_ADDR)
#define HWIO_TLMM_GPIO_CFG38_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG38_ADDR, m)
#define HWIO_TLMM_GPIO_CFG38_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG38_ADDR,v)
#define HWIO_TLMM_GPIO_CFG38_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG38_ADDR,m,v,HWIO_TLMM_GPIO_CFG38_IN)
#define HWIO_TLMM_GPIO_CFG38_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG38_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG38_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG38_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG38_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG38_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG38_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG38_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG38_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG38_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG39_ADDR                                           (TLMM_CSR_REG_BASE      + 0x00027000)
#define HWIO_TLMM_GPIO_CFG39_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG39_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG39_ADDR)
#define HWIO_TLMM_GPIO_CFG39_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG39_ADDR, m)
#define HWIO_TLMM_GPIO_CFG39_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG39_ADDR,v)
#define HWIO_TLMM_GPIO_CFG39_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG39_ADDR,m,v,HWIO_TLMM_GPIO_CFG39_IN)
#define HWIO_TLMM_GPIO_CFG39_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG39_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG39_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG39_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG39_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG39_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG39_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG39_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG39_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG39_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG40_ADDR                                           (TLMM_CSR_REG_BASE      + 0x00028000)
#define HWIO_TLMM_GPIO_CFG40_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG40_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG40_ADDR)
#define HWIO_TLMM_GPIO_CFG40_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG40_ADDR, m)
#define HWIO_TLMM_GPIO_CFG40_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG40_ADDR,v)
#define HWIO_TLMM_GPIO_CFG40_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG40_ADDR,m,v,HWIO_TLMM_GPIO_CFG40_IN)
#define HWIO_TLMM_GPIO_CFG40_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG40_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG40_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG40_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG40_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG40_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG40_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG40_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG40_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG40_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG41_ADDR                                           (TLMM_CSR_REG_BASE      + 0x00029000)
#define HWIO_TLMM_GPIO_CFG41_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG41_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG41_ADDR)
#define HWIO_TLMM_GPIO_CFG41_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG41_ADDR, m)
#define HWIO_TLMM_GPIO_CFG41_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG41_ADDR,v)
#define HWIO_TLMM_GPIO_CFG41_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG41_ADDR,m,v,HWIO_TLMM_GPIO_CFG41_IN)
#define HWIO_TLMM_GPIO_CFG41_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG41_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG41_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG41_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG41_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG41_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG41_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG41_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG41_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG41_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFG42_ADDR                                           (TLMM_CSR_REG_BASE      + 0x0002a000)
#define HWIO_TLMM_GPIO_CFG42_RMSK                                                0x7ff
#define HWIO_TLMM_GPIO_CFG42_IN          \
        in_dword(HWIO_TLMM_GPIO_CFG42_ADDR)
#define HWIO_TLMM_GPIO_CFG42_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_CFG42_ADDR, m)
#define HWIO_TLMM_GPIO_CFG42_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_CFG42_ADDR,v)
#define HWIO_TLMM_GPIO_CFG42_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFG42_ADDR,m,v,HWIO_TLMM_GPIO_CFG42_IN)
#define HWIO_TLMM_GPIO_CFG42_GPIO_HIHYS_EN_BMSK                                  0x400
#define HWIO_TLMM_GPIO_CFG42_GPIO_HIHYS_EN_SHFT                                    0xa
#define HWIO_TLMM_GPIO_CFG42_GPIO_OE_BMSK                                        0x200
#define HWIO_TLMM_GPIO_CFG42_GPIO_OE_SHFT                                          0x9
#define HWIO_TLMM_GPIO_CFG42_DRV_STRENGTH_BMSK                                   0x1c0
#define HWIO_TLMM_GPIO_CFG42_DRV_STRENGTH_SHFT                                     0x6
#define HWIO_TLMM_GPIO_CFG42_FUNC_SEL_BMSK                                        0x3c
#define HWIO_TLMM_GPIO_CFG42_FUNC_SEL_SHFT                                         0x2
#define HWIO_TLMM_GPIO_CFG42_GPIO_PULL_BMSK                                        0x3
#define HWIO_TLMM_GPIO_CFG42_GPIO_PULL_SHFT                                        0x0

#define HWIO_TLMM_GPIO_CFGk_ADDR(k)                                         (TLMM_CSR_REG_BASE      + 0x00000000 + 0x1000 * (k))
#define HWIO_TLMM_GPIO_CFGk_RMSK                                                 0x7ff
#define HWIO_TLMM_GPIO_CFGk_MAXk                                                    45
#define HWIO_TLMM_GPIO_CFGk_INI(k)        \
        in_dword_masked(HWIO_TLMM_GPIO_CFGk_ADDR(k), HWIO_TLMM_GPIO_CFGk_RMSK)
#define HWIO_TLMM_GPIO_CFGk_INMI(k,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_CFGk_ADDR(k), mask)
#define HWIO_TLMM_GPIO_CFGk_OUTI(k,val)    \
        out_dword(HWIO_TLMM_GPIO_CFGk_ADDR(k),val)
#define HWIO_TLMM_GPIO_CFGk_OUTMI(k,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFGk_ADDR(k),mask,val,HWIO_TLMM_GPIO_CFGk_INI(k))
#define HWIO_TLMM_GPIO_CFGk_GPIO_HIHYS_EN_BMSK                                   0x400
#define HWIO_TLMM_GPIO_CFGk_GPIO_HIHYS_EN_SHFT                                     0xa
#define HWIO_TLMM_GPIO_CFGk_GPIO_OE_BMSK                                         0x200
#define HWIO_TLMM_GPIO_CFGk_GPIO_OE_SHFT                                           0x9
#define HWIO_TLMM_GPIO_CFGk_DRV_STRENGTH_BMSK                                    0x1c0
#define HWIO_TLMM_GPIO_CFGk_DRV_STRENGTH_SHFT                                      0x6
#define HWIO_TLMM_GPIO_CFGk_FUNC_SEL_BMSK                                         0x3c
#define HWIO_TLMM_GPIO_CFGk_FUNC_SEL_SHFT                                          0x2
#define HWIO_TLMM_GPIO_CFGk_GPIO_PULL_BMSK                                         0x3
#define HWIO_TLMM_GPIO_CFGk_GPIO_PULL_SHFT                                         0x0

#define HWIO_TLMM_GPIO_CFGp_ADDR(p)                                         (TLMM_CSR_REG_BASE      + 0x00000000 + 0x1000 * (p))
#define HWIO_TLMM_GPIO_CFGp_RMSK                                                 0x7ff
#define HWIO_TLMM_GPIO_CFGp_MAXp                                                    62
#define HWIO_TLMM_GPIO_CFGp_INI(p)        \
        in_dword_masked(HWIO_TLMM_GPIO_CFGp_ADDR(p), HWIO_TLMM_GPIO_CFGp_RMSK)
#define HWIO_TLMM_GPIO_CFGp_INMI(p,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_CFGp_ADDR(p), mask)
#define HWIO_TLMM_GPIO_CFGp_OUTI(p,val)    \
        out_dword(HWIO_TLMM_GPIO_CFGp_ADDR(p),val)
#define HWIO_TLMM_GPIO_CFGp_OUTMI(p,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_CFGp_ADDR(p),mask,val,HWIO_TLMM_GPIO_CFGp_INI(p))
#define HWIO_TLMM_GPIO_CFGp_GPIO_HIHYS_EN_BMSK                                   0x400
#define HWIO_TLMM_GPIO_CFGp_GPIO_HIHYS_EN_SHFT                                     0xa
#define HWIO_TLMM_GPIO_CFGp_GPIO_OE_BMSK                                         0x200
#define HWIO_TLMM_GPIO_CFGp_GPIO_OE_SHFT                                           0x9
#define HWIO_TLMM_GPIO_CFGp_DRV_STRENGTH_BMSK                                    0x1c0
#define HWIO_TLMM_GPIO_CFGp_DRV_STRENGTH_SHFT                                      0x6
#define HWIO_TLMM_GPIO_CFGp_FUNC_SEL_BMSK                                         0x3c
#define HWIO_TLMM_GPIO_CFGp_FUNC_SEL_SHFT                                          0x2
#define HWIO_TLMM_GPIO_CFGp_GPIO_PULL_BMSK                                         0x3
#define HWIO_TLMM_GPIO_CFGp_GPIO_PULL_SHFT                                         0x0

#define HWIO_TLMM_GPIO_IN_OUTn_ADDR(n)                                      (TLMM_CSR_REG_BASE      + 0x00000004 + 0x1000 * (n))
#define HWIO_TLMM_GPIO_IN_OUTn_RMSK                                                0x3
#define HWIO_TLMM_GPIO_IN_OUTn_MAXn                                                 62
#define HWIO_TLMM_GPIO_IN_OUTn_INI(n)        \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUTn_ADDR(n), HWIO_TLMM_GPIO_IN_OUTn_RMSK)
#define HWIO_TLMM_GPIO_IN_OUTn_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUTn_ADDR(n), mask)
#define HWIO_TLMM_GPIO_IN_OUTn_OUTI(n,val)    \
        out_dword(HWIO_TLMM_GPIO_IN_OUTn_ADDR(n),val)
#define HWIO_TLMM_GPIO_IN_OUTn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUTn_ADDR(n),mask,val,HWIO_TLMM_GPIO_IN_OUTn_INI(n))
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_OUT_BMSK                                       0x2
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_OUT_SHFT                                       0x1
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_IN_BMSK                                        0x1
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_IN_SHFT                                        0x0

#define HWIO_TLMM_GPIO_IN_OUT0_ADDR                                         (TLMM_CSR_REG_BASE      + 0x00000004)
#define HWIO_TLMM_GPIO_IN_OUT0_RMSK                                                0x3
#define HWIO_TLMM_GPIO_IN_OUT0_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT0_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT0_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT0_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT0_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT0_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT0_IN)
#define HWIO_TLMM_GPIO_IN_OUT0_GPIO_OUT_BMSK                                       0x2
#define HWIO_TLMM_GPIO_IN_OUT0_GPIO_OUT_SHFT                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT0_GPIO_IN_BMSK                                        0x1
#define HWIO_TLMM_GPIO_IN_OUT0_GPIO_IN_SHFT                                        0x0

#define HWIO_TLMM_GPIO_IN_OUT1_ADDR                                         (TLMM_CSR_REG_BASE      + 0x00001004)
#define HWIO_TLMM_GPIO_IN_OUT1_RMSK                                                0x3
#define HWIO_TLMM_GPIO_IN_OUT1_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT1_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT1_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT1_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT1_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT1_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT1_IN)
#define HWIO_TLMM_GPIO_IN_OUT1_GPIO_OUT_BMSK                                       0x2
#define HWIO_TLMM_GPIO_IN_OUT1_GPIO_OUT_SHFT                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT1_GPIO_IN_BMSK                                        0x1
#define HWIO_TLMM_GPIO_IN_OUT1_GPIO_IN_SHFT                                        0x0

#define HWIO_TLMM_GPIO_IN_OUT2_ADDR                                         (TLMM_CSR_REG_BASE      + 0x00002004)
#define HWIO_TLMM_GPIO_IN_OUT2_RMSK                                                0x3
#define HWIO_TLMM_GPIO_IN_OUT2_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT2_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT2_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT2_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT2_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT2_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT2_IN)
#define HWIO_TLMM_GPIO_IN_OUT2_GPIO_OUT_BMSK                                       0x2
#define HWIO_TLMM_GPIO_IN_OUT2_GPIO_OUT_SHFT                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT2_GPIO_IN_BMSK                                        0x1
#define HWIO_TLMM_GPIO_IN_OUT2_GPIO_IN_SHFT                                        0x0

#define HWIO_TLMM_GPIO_IN_OUT3_ADDR                                         (TLMM_CSR_REG_BASE      + 0x00003004)
#define HWIO_TLMM_GPIO_IN_OUT3_RMSK                                                0x3
#define HWIO_TLMM_GPIO_IN_OUT3_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT3_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT3_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT3_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT3_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT3_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT3_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT3_IN)
#define HWIO_TLMM_GPIO_IN_OUT3_GPIO_OUT_BMSK                                       0x2
#define HWIO_TLMM_GPIO_IN_OUT3_GPIO_OUT_SHFT                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT3_GPIO_IN_BMSK                                        0x1
#define HWIO_TLMM_GPIO_IN_OUT3_GPIO_IN_SHFT                                        0x0

#define HWIO_TLMM_GPIO_IN_OUT4_ADDR                                         (TLMM_CSR_REG_BASE      + 0x00004004)
#define HWIO_TLMM_GPIO_IN_OUT4_RMSK                                                0x3
#define HWIO_TLMM_GPIO_IN_OUT4_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT4_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT4_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT4_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT4_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT4_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT4_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT4_IN)
#define HWIO_TLMM_GPIO_IN_OUT4_GPIO_OUT_BMSK                                       0x2
#define HWIO_TLMM_GPIO_IN_OUT4_GPIO_OUT_SHFT                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT4_GPIO_IN_BMSK                                        0x1
#define HWIO_TLMM_GPIO_IN_OUT4_GPIO_IN_SHFT                                        0x0

#define HWIO_TLMM_GPIO_IN_OUT5_ADDR                                         (TLMM_CSR_REG_BASE      + 0x00005004)
#define HWIO_TLMM_GPIO_IN_OUT5_RMSK                                                0x3
#define HWIO_TLMM_GPIO_IN_OUT5_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT5_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT5_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT5_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT5_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT5_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT5_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT5_IN)
#define HWIO_TLMM_GPIO_IN_OUT5_GPIO_OUT_BMSK                                       0x2
#define HWIO_TLMM_GPIO_IN_OUT5_GPIO_OUT_SHFT                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT5_GPIO_IN_BMSK                                        0x1
#define HWIO_TLMM_GPIO_IN_OUT5_GPIO_IN_SHFT                                        0x0

#define HWIO_TLMM_GPIO_IN_OUT6_ADDR                                         (TLMM_CSR_REG_BASE      + 0x00006004)
#define HWIO_TLMM_GPIO_IN_OUT6_RMSK                                                0x3
#define HWIO_TLMM_GPIO_IN_OUT6_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT6_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT6_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT6_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT6_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT6_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT6_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT6_IN)
#define HWIO_TLMM_GPIO_IN_OUT6_GPIO_OUT_BMSK                                       0x2
#define HWIO_TLMM_GPIO_IN_OUT6_GPIO_OUT_SHFT                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT6_GPIO_IN_BMSK                                        0x1
#define HWIO_TLMM_GPIO_IN_OUT6_GPIO_IN_SHFT                                        0x0

#define HWIO_TLMM_GPIO_IN_OUT7_ADDR                                         (TLMM_CSR_REG_BASE      + 0x00007004)
#define HWIO_TLMM_GPIO_IN_OUT7_RMSK                                                0x3
#define HWIO_TLMM_GPIO_IN_OUT7_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT7_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT7_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT7_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT7_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT7_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT7_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT7_IN)
#define HWIO_TLMM_GPIO_IN_OUT7_GPIO_OUT_BMSK                                       0x2
#define HWIO_TLMM_GPIO_IN_OUT7_GPIO_OUT_SHFT                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT7_GPIO_IN_BMSK                                        0x1
#define HWIO_TLMM_GPIO_IN_OUT7_GPIO_IN_SHFT                                        0x0

#define HWIO_TLMM_GPIO_IN_OUT8_ADDR                                         (TLMM_CSR_REG_BASE      + 0x00008004)
#define HWIO_TLMM_GPIO_IN_OUT8_RMSK                                                0x3
#define HWIO_TLMM_GPIO_IN_OUT8_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT8_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT8_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT8_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT8_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT8_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT8_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT8_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT8_IN)
#define HWIO_TLMM_GPIO_IN_OUT8_GPIO_OUT_BMSK                                       0x2
#define HWIO_TLMM_GPIO_IN_OUT8_GPIO_OUT_SHFT                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT8_GPIO_IN_BMSK                                        0x1
#define HWIO_TLMM_GPIO_IN_OUT8_GPIO_IN_SHFT                                        0x0

#define HWIO_TLMM_GPIO_IN_OUT9_ADDR                                         (TLMM_CSR_REG_BASE      + 0x00009004)
#define HWIO_TLMM_GPIO_IN_OUT9_RMSK                                                0x3
#define HWIO_TLMM_GPIO_IN_OUT9_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT9_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT9_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT9_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT9_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT9_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT9_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT9_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT9_IN)
#define HWIO_TLMM_GPIO_IN_OUT9_GPIO_OUT_BMSK                                       0x2
#define HWIO_TLMM_GPIO_IN_OUT9_GPIO_OUT_SHFT                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT9_GPIO_IN_BMSK                                        0x1
#define HWIO_TLMM_GPIO_IN_OUT9_GPIO_IN_SHFT                                        0x0

#define HWIO_TLMM_GPIO_IN_OUT10_ADDR                                        (TLMM_CSR_REG_BASE      + 0x0000a004)
#define HWIO_TLMM_GPIO_IN_OUT10_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT10_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT10_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT10_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT10_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT10_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT10_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT10_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT10_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT10_IN)
#define HWIO_TLMM_GPIO_IN_OUT10_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT10_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT10_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT10_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT11_ADDR                                        (TLMM_CSR_REG_BASE      + 0x0000b004)
#define HWIO_TLMM_GPIO_IN_OUT11_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT11_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT11_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT11_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT11_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT11_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT11_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT11_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT11_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT11_IN)
#define HWIO_TLMM_GPIO_IN_OUT11_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT11_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT11_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT11_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT12_ADDR                                        (TLMM_CSR_REG_BASE      + 0x0000c004)
#define HWIO_TLMM_GPIO_IN_OUT12_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT12_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT12_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT12_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT12_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT12_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT12_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT12_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT12_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT12_IN)
#define HWIO_TLMM_GPIO_IN_OUT12_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT12_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT12_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT12_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT13_ADDR                                        (TLMM_CSR_REG_BASE      + 0x0000d004)
#define HWIO_TLMM_GPIO_IN_OUT13_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT13_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT13_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT13_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT13_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT13_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT13_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT13_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT13_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT13_IN)
#define HWIO_TLMM_GPIO_IN_OUT13_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT13_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT13_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT13_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT14_ADDR                                        (TLMM_CSR_REG_BASE      + 0x0000e004)
#define HWIO_TLMM_GPIO_IN_OUT14_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT14_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT14_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT14_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT14_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT14_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT14_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT14_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT14_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT14_IN)
#define HWIO_TLMM_GPIO_IN_OUT14_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT14_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT14_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT14_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT15_ADDR                                        (TLMM_CSR_REG_BASE      + 0x0000f004)
#define HWIO_TLMM_GPIO_IN_OUT15_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT15_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT15_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT15_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT15_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT15_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT15_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT15_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT15_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT15_IN)
#define HWIO_TLMM_GPIO_IN_OUT15_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT15_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT15_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT15_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT16_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00010004)
#define HWIO_TLMM_GPIO_IN_OUT16_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT16_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT16_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT16_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT16_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT16_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT16_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT16_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT16_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT16_IN)
#define HWIO_TLMM_GPIO_IN_OUT16_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT16_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT16_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT16_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT17_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00011004)
#define HWIO_TLMM_GPIO_IN_OUT17_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT17_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT17_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT17_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT17_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT17_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT17_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT17_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT17_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT17_IN)
#define HWIO_TLMM_GPIO_IN_OUT17_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT17_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT17_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT17_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT18_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00012004)
#define HWIO_TLMM_GPIO_IN_OUT18_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT18_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT18_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT18_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT18_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT18_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT18_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT18_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT18_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT18_IN)
#define HWIO_TLMM_GPIO_IN_OUT18_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT18_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT18_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT18_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT19_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00013004)
#define HWIO_TLMM_GPIO_IN_OUT19_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT19_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT19_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT19_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT19_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT19_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT19_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT19_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT19_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT19_IN)
#define HWIO_TLMM_GPIO_IN_OUT19_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT19_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT19_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT19_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT20_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00014004)
#define HWIO_TLMM_GPIO_IN_OUT20_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT20_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT20_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT20_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT20_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT20_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT20_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT20_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT20_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT20_IN)
#define HWIO_TLMM_GPIO_IN_OUT20_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT20_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT20_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT20_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT21_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00015004)
#define HWIO_TLMM_GPIO_IN_OUT21_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT21_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT21_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT21_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT21_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT21_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT21_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT21_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT21_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT21_IN)
#define HWIO_TLMM_GPIO_IN_OUT21_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT21_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT21_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT21_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT22_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00016004)
#define HWIO_TLMM_GPIO_IN_OUT22_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT22_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT22_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT22_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT22_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT22_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT22_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT22_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT22_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT22_IN)
#define HWIO_TLMM_GPIO_IN_OUT22_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT22_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT22_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT22_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT23_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00017004)
#define HWIO_TLMM_GPIO_IN_OUT23_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT23_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT23_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT23_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT23_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT23_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT23_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT23_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT23_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT23_IN)
#define HWIO_TLMM_GPIO_IN_OUT23_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT23_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT23_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT23_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT24_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00018004)
#define HWIO_TLMM_GPIO_IN_OUT24_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT24_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT24_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT24_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT24_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT24_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT24_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT24_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT24_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT24_IN)
#define HWIO_TLMM_GPIO_IN_OUT24_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT24_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT24_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT24_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT25_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00019004)
#define HWIO_TLMM_GPIO_IN_OUT25_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT25_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT25_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT25_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT25_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT25_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT25_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT25_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT25_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT25_IN)
#define HWIO_TLMM_GPIO_IN_OUT25_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT25_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT25_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT25_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT26_ADDR                                        (TLMM_CSR_REG_BASE      + 0x0001a004)
#define HWIO_TLMM_GPIO_IN_OUT26_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT26_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT26_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT26_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT26_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT26_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT26_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT26_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT26_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT26_IN)
#define HWIO_TLMM_GPIO_IN_OUT26_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT26_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT26_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT26_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT27_ADDR                                        (TLMM_CSR_REG_BASE      + 0x0001b004)
#define HWIO_TLMM_GPIO_IN_OUT27_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT27_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT27_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT27_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT27_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT27_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT27_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT27_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT27_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT27_IN)
#define HWIO_TLMM_GPIO_IN_OUT27_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT27_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT27_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT27_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT28_ADDR                                        (TLMM_CSR_REG_BASE      + 0x0001c004)
#define HWIO_TLMM_GPIO_IN_OUT28_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT28_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT28_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT28_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT28_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT28_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT28_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT28_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT28_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT28_IN)
#define HWIO_TLMM_GPIO_IN_OUT28_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT28_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT28_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT28_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT29_ADDR                                        (TLMM_CSR_REG_BASE      + 0x0001d004)
#define HWIO_TLMM_GPIO_IN_OUT29_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT29_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT29_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT29_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT29_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT29_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT29_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT29_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT29_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT29_IN)
#define HWIO_TLMM_GPIO_IN_OUT29_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT29_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT29_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT29_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT30_ADDR                                        (TLMM_CSR_REG_BASE      + 0x0001e004)
#define HWIO_TLMM_GPIO_IN_OUT30_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT30_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT30_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT30_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT30_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT30_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT30_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT30_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT30_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT30_IN)
#define HWIO_TLMM_GPIO_IN_OUT30_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT30_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT30_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT30_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT31_ADDR                                        (TLMM_CSR_REG_BASE      + 0x0001f004)
#define HWIO_TLMM_GPIO_IN_OUT31_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT31_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT31_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT31_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT31_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT31_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT31_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT31_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT31_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT31_IN)
#define HWIO_TLMM_GPIO_IN_OUT31_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT31_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT31_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT31_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT32_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00020004)
#define HWIO_TLMM_GPIO_IN_OUT32_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT32_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT32_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT32_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT32_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT32_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT32_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT32_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT32_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT32_IN)
#define HWIO_TLMM_GPIO_IN_OUT32_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT32_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT32_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT32_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT33_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00021004)
#define HWIO_TLMM_GPIO_IN_OUT33_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT33_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT33_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT33_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT33_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT33_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT33_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT33_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT33_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT33_IN)
#define HWIO_TLMM_GPIO_IN_OUT33_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT33_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT33_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT33_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT34_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00022004)
#define HWIO_TLMM_GPIO_IN_OUT34_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT34_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT34_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT34_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT34_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT34_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT34_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT34_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT34_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT34_IN)
#define HWIO_TLMM_GPIO_IN_OUT34_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT34_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT34_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT34_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT35_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00023004)
#define HWIO_TLMM_GPIO_IN_OUT35_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT35_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT35_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT35_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT35_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT35_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT35_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT35_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT35_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT35_IN)
#define HWIO_TLMM_GPIO_IN_OUT35_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT35_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT35_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT35_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT36_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00024004)
#define HWIO_TLMM_GPIO_IN_OUT36_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT36_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT36_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT36_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT36_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT36_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT36_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT36_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT36_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT36_IN)
#define HWIO_TLMM_GPIO_IN_OUT36_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT36_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT36_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT36_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT37_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00025004)
#define HWIO_TLMM_GPIO_IN_OUT37_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT37_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT37_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT37_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT37_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT37_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT37_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT37_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT37_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT37_IN)
#define HWIO_TLMM_GPIO_IN_OUT37_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT37_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT37_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT37_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT38_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00026004)
#define HWIO_TLMM_GPIO_IN_OUT38_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT38_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT38_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT38_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT38_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT38_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT38_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT38_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT38_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT38_IN)
#define HWIO_TLMM_GPIO_IN_OUT38_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT38_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT38_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT38_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT39_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00027004)
#define HWIO_TLMM_GPIO_IN_OUT39_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT39_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT39_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT39_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT39_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT39_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT39_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT39_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT39_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT39_IN)
#define HWIO_TLMM_GPIO_IN_OUT39_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT39_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT39_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT39_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT40_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00028004)
#define HWIO_TLMM_GPIO_IN_OUT40_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT40_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT40_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT40_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT40_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT40_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT40_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT40_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT40_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT40_IN)
#define HWIO_TLMM_GPIO_IN_OUT40_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT40_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT40_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT40_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT41_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00029004)
#define HWIO_TLMM_GPIO_IN_OUT41_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT41_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT41_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT41_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT41_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT41_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT41_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT41_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT41_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT41_IN)
#define HWIO_TLMM_GPIO_IN_OUT41_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT41_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT41_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT41_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT42_ADDR                                        (TLMM_CSR_REG_BASE      + 0x0002a004)
#define HWIO_TLMM_GPIO_IN_OUT42_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT42_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT42_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT42_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT42_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT42_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT42_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT42_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT42_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT42_IN)
#define HWIO_TLMM_GPIO_IN_OUT42_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT42_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT42_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT42_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT43_ADDR                                        (TLMM_CSR_REG_BASE      + 0x0002b004)
#define HWIO_TLMM_GPIO_IN_OUT43_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT43_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT43_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT43_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT43_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT43_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT43_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT43_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT43_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT43_IN)
#define HWIO_TLMM_GPIO_IN_OUT43_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT43_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT43_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT43_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT44_ADDR                                        (TLMM_CSR_REG_BASE      + 0x0002c004)
#define HWIO_TLMM_GPIO_IN_OUT44_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT44_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT44_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT44_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT44_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT44_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT44_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT44_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT44_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT44_IN)
#define HWIO_TLMM_GPIO_IN_OUT44_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT44_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT44_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT44_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT45_ADDR                                        (TLMM_CSR_REG_BASE      + 0x0002d004)
#define HWIO_TLMM_GPIO_IN_OUT45_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT45_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT45_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT45_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT45_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT45_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT45_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT45_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT45_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT45_IN)
#define HWIO_TLMM_GPIO_IN_OUT45_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT45_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT45_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT45_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT46_ADDR                                        (TLMM_CSR_REG_BASE      + 0x0002e004)
#define HWIO_TLMM_GPIO_IN_OUT46_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT46_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT46_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT46_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT46_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT46_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT46_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT46_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT46_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT46_IN)
#define HWIO_TLMM_GPIO_IN_OUT46_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT46_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT46_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT46_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT47_ADDR                                        (TLMM_CSR_REG_BASE      + 0x0002f004)
#define HWIO_TLMM_GPIO_IN_OUT47_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT47_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT47_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT47_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT47_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT47_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT47_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT47_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT47_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT47_IN)
#define HWIO_TLMM_GPIO_IN_OUT47_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT47_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT47_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT47_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT48_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00030004)
#define HWIO_TLMM_GPIO_IN_OUT48_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT48_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT48_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT48_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT48_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT48_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT48_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT48_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT48_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT48_IN)
#define HWIO_TLMM_GPIO_IN_OUT48_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT48_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT48_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT48_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT49_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00031004)
#define HWIO_TLMM_GPIO_IN_OUT49_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT49_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT49_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT49_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT49_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT49_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT49_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT49_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT49_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT49_IN)
#define HWIO_TLMM_GPIO_IN_OUT49_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT49_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT49_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT49_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT50_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00032004)
#define HWIO_TLMM_GPIO_IN_OUT50_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT50_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT50_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT50_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT50_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT50_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT50_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT50_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT50_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT50_IN)
#define HWIO_TLMM_GPIO_IN_OUT50_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT50_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT50_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT50_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT51_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00033004)
#define HWIO_TLMM_GPIO_IN_OUT51_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT51_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT51_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT51_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT51_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT51_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT51_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT51_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT51_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT51_IN)
#define HWIO_TLMM_GPIO_IN_OUT51_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT51_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT51_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT51_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT52_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00034004)
#define HWIO_TLMM_GPIO_IN_OUT52_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT52_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT52_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT52_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT52_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT52_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT52_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT52_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT52_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT52_IN)
#define HWIO_TLMM_GPIO_IN_OUT52_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT52_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT52_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT52_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT53_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00035004)
#define HWIO_TLMM_GPIO_IN_OUT53_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT53_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT53_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT53_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT53_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT53_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT53_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT53_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT53_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT53_IN)
#define HWIO_TLMM_GPIO_IN_OUT53_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT53_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT53_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT53_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT54_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00036004)
#define HWIO_TLMM_GPIO_IN_OUT54_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT54_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT54_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT54_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT54_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT54_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT54_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT54_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT54_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT54_IN)
#define HWIO_TLMM_GPIO_IN_OUT54_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT54_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT54_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT54_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT55_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00037004)
#define HWIO_TLMM_GPIO_IN_OUT55_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT55_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT55_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT55_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT55_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT55_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT55_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT55_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT55_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT55_IN)
#define HWIO_TLMM_GPIO_IN_OUT55_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT55_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT55_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT55_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT56_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00038004)
#define HWIO_TLMM_GPIO_IN_OUT56_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT56_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT56_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT56_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT56_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT56_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT56_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT56_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT56_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT56_IN)
#define HWIO_TLMM_GPIO_IN_OUT56_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT56_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT56_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT56_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT57_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00039004)
#define HWIO_TLMM_GPIO_IN_OUT57_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT57_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT57_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT57_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT57_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT57_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT57_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT57_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT57_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT57_IN)
#define HWIO_TLMM_GPIO_IN_OUT57_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT57_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT57_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT57_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT58_ADDR                                        (TLMM_CSR_REG_BASE      + 0x0003a004)
#define HWIO_TLMM_GPIO_IN_OUT58_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT58_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT58_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT58_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT58_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT58_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT58_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT58_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT58_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT58_IN)
#define HWIO_TLMM_GPIO_IN_OUT58_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT58_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT58_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT58_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT59_ADDR                                        (TLMM_CSR_REG_BASE      + 0x0003b004)
#define HWIO_TLMM_GPIO_IN_OUT59_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT59_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT59_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT59_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT59_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT59_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT59_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT59_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT59_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT59_IN)
#define HWIO_TLMM_GPIO_IN_OUT59_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT59_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT59_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT59_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT60_ADDR                                        (TLMM_CSR_REG_BASE      + 0x0003c004)
#define HWIO_TLMM_GPIO_IN_OUT60_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT60_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT60_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT60_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT60_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT60_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT60_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT60_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT60_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT60_IN)
#define HWIO_TLMM_GPIO_IN_OUT60_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT60_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT60_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT60_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT61_ADDR                                        (TLMM_CSR_REG_BASE      + 0x0003d004)
#define HWIO_TLMM_GPIO_IN_OUT61_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT61_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT61_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT61_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT61_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT61_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT61_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT61_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT61_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT61_IN)
#define HWIO_TLMM_GPIO_IN_OUT61_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT61_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT61_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT61_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_IN_OUT62_ADDR                                        (TLMM_CSR_REG_BASE      + 0x0003e004)
#define HWIO_TLMM_GPIO_IN_OUT62_RMSK                                               0x3
#define HWIO_TLMM_GPIO_IN_OUT62_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_OUT62_ADDR)
#define HWIO_TLMM_GPIO_IN_OUT62_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUT62_ADDR, m)
#define HWIO_TLMM_GPIO_IN_OUT62_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_IN_OUT62_ADDR,v)
#define HWIO_TLMM_GPIO_IN_OUT62_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUT62_ADDR,m,v,HWIO_TLMM_GPIO_IN_OUT62_IN)
#define HWIO_TLMM_GPIO_IN_OUT62_GPIO_OUT_BMSK                                      0x2
#define HWIO_TLMM_GPIO_IN_OUT62_GPIO_OUT_SHFT                                      0x1
#define HWIO_TLMM_GPIO_IN_OUT62_GPIO_IN_BMSK                                       0x1
#define HWIO_TLMM_GPIO_IN_OUT62_GPIO_IN_SHFT                                       0x0

#define HWIO_TLMM_GPIO_INTR_CFGn_ADDR(n)                                    (TLMM_CSR_REG_BASE      + 0x00000008 + 0x1000 * (n))
#define HWIO_TLMM_GPIO_INTR_CFGn_RMSK                                            0x1ff
#define HWIO_TLMM_GPIO_INTR_CFGn_MAXn                                               62
#define HWIO_TLMM_GPIO_INTR_CFGn_INI(n)        \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFGn_ADDR(n), HWIO_TLMM_GPIO_INTR_CFGn_RMSK)
#define HWIO_TLMM_GPIO_INTR_CFGn_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFGn_ADDR(n), mask)
#define HWIO_TLMM_GPIO_INTR_CFGn_OUTI(n,val)    \
        out_dword(HWIO_TLMM_GPIO_INTR_CFGn_ADDR(n),val)
#define HWIO_TLMM_GPIO_INTR_CFGn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFGn_ADDR(n),mask,val,HWIO_TLMM_GPIO_INTR_CFGn_INI(n))
#define HWIO_TLMM_GPIO_INTR_CFGn_DIR_CONN_EN_BMSK                                0x100
#define HWIO_TLMM_GPIO_INTR_CFGn_DIR_CONN_EN_SHFT                                  0x8
#define HWIO_TLMM_GPIO_INTR_CFGn_TARGET_PROC_BMSK                                 0xe0
#define HWIO_TLMM_GPIO_INTR_CFGn_TARGET_PROC_SHFT                                  0x5
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_RAW_STATUS_EN_BMSK                          0x10
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_RAW_STATUS_EN_SHFT                           0x4
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_DECT_CTL_BMSK                                0xc
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_DECT_CTL_SHFT                                0x2
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_POL_CTL_BMSK                                 0x2
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_POL_CTL_SHFT                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_ENABLE_BMSK                                  0x1
#define HWIO_TLMM_GPIO_INTR_CFGn_INTR_ENABLE_SHFT                                  0x0

#define HWIO_TLMM_GPIO_INTR_CFG0_ADDR                                       (TLMM_CSR_REG_BASE      + 0x00000008)
#define HWIO_TLMM_GPIO_INTR_CFG0_RMSK                                            0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG0_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG0_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG0_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG0_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG0_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG0_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG0_IN)
#define HWIO_TLMM_GPIO_INTR_CFG0_DIR_CONN_EN_BMSK                                0x100
#define HWIO_TLMM_GPIO_INTR_CFG0_DIR_CONN_EN_SHFT                                  0x8
#define HWIO_TLMM_GPIO_INTR_CFG0_TARGET_PROC_BMSK                                 0xe0
#define HWIO_TLMM_GPIO_INTR_CFG0_TARGET_PROC_SHFT                                  0x5
#define HWIO_TLMM_GPIO_INTR_CFG0_INTR_RAW_STATUS_EN_BMSK                          0x10
#define HWIO_TLMM_GPIO_INTR_CFG0_INTR_RAW_STATUS_EN_SHFT                           0x4
#define HWIO_TLMM_GPIO_INTR_CFG0_INTR_DECT_CTL_BMSK                                0xc
#define HWIO_TLMM_GPIO_INTR_CFG0_INTR_DECT_CTL_SHFT                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG0_INTR_POL_CTL_BMSK                                 0x2
#define HWIO_TLMM_GPIO_INTR_CFG0_INTR_POL_CTL_SHFT                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG0_INTR_ENABLE_BMSK                                  0x1
#define HWIO_TLMM_GPIO_INTR_CFG0_INTR_ENABLE_SHFT                                  0x0

#define HWIO_TLMM_GPIO_INTR_CFG1_ADDR                                       (TLMM_CSR_REG_BASE      + 0x00001008)
#define HWIO_TLMM_GPIO_INTR_CFG1_RMSK                                            0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG1_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG1_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG1_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG1_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG1_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG1_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG1_IN)
#define HWIO_TLMM_GPIO_INTR_CFG1_DIR_CONN_EN_BMSK                                0x100
#define HWIO_TLMM_GPIO_INTR_CFG1_DIR_CONN_EN_SHFT                                  0x8
#define HWIO_TLMM_GPIO_INTR_CFG1_TARGET_PROC_BMSK                                 0xe0
#define HWIO_TLMM_GPIO_INTR_CFG1_TARGET_PROC_SHFT                                  0x5
#define HWIO_TLMM_GPIO_INTR_CFG1_INTR_RAW_STATUS_EN_BMSK                          0x10
#define HWIO_TLMM_GPIO_INTR_CFG1_INTR_RAW_STATUS_EN_SHFT                           0x4
#define HWIO_TLMM_GPIO_INTR_CFG1_INTR_DECT_CTL_BMSK                                0xc
#define HWIO_TLMM_GPIO_INTR_CFG1_INTR_DECT_CTL_SHFT                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG1_INTR_POL_CTL_BMSK                                 0x2
#define HWIO_TLMM_GPIO_INTR_CFG1_INTR_POL_CTL_SHFT                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG1_INTR_ENABLE_BMSK                                  0x1
#define HWIO_TLMM_GPIO_INTR_CFG1_INTR_ENABLE_SHFT                                  0x0

#define HWIO_TLMM_GPIO_INTR_CFG2_ADDR                                       (TLMM_CSR_REG_BASE      + 0x00002008)
#define HWIO_TLMM_GPIO_INTR_CFG2_RMSK                                            0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG2_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG2_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG2_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG2_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG2_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG2_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG2_IN)
#define HWIO_TLMM_GPIO_INTR_CFG2_DIR_CONN_EN_BMSK                                0x100
#define HWIO_TLMM_GPIO_INTR_CFG2_DIR_CONN_EN_SHFT                                  0x8
#define HWIO_TLMM_GPIO_INTR_CFG2_TARGET_PROC_BMSK                                 0xe0
#define HWIO_TLMM_GPIO_INTR_CFG2_TARGET_PROC_SHFT                                  0x5
#define HWIO_TLMM_GPIO_INTR_CFG2_INTR_RAW_STATUS_EN_BMSK                          0x10
#define HWIO_TLMM_GPIO_INTR_CFG2_INTR_RAW_STATUS_EN_SHFT                           0x4
#define HWIO_TLMM_GPIO_INTR_CFG2_INTR_DECT_CTL_BMSK                                0xc
#define HWIO_TLMM_GPIO_INTR_CFG2_INTR_DECT_CTL_SHFT                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG2_INTR_POL_CTL_BMSK                                 0x2
#define HWIO_TLMM_GPIO_INTR_CFG2_INTR_POL_CTL_SHFT                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG2_INTR_ENABLE_BMSK                                  0x1
#define HWIO_TLMM_GPIO_INTR_CFG2_INTR_ENABLE_SHFT                                  0x0

#define HWIO_TLMM_GPIO_INTR_CFG3_ADDR                                       (TLMM_CSR_REG_BASE      + 0x00003008)
#define HWIO_TLMM_GPIO_INTR_CFG3_RMSK                                            0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG3_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG3_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG3_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG3_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG3_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG3_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG3_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG3_IN)
#define HWIO_TLMM_GPIO_INTR_CFG3_DIR_CONN_EN_BMSK                                0x100
#define HWIO_TLMM_GPIO_INTR_CFG3_DIR_CONN_EN_SHFT                                  0x8
#define HWIO_TLMM_GPIO_INTR_CFG3_TARGET_PROC_BMSK                                 0xe0
#define HWIO_TLMM_GPIO_INTR_CFG3_TARGET_PROC_SHFT                                  0x5
#define HWIO_TLMM_GPIO_INTR_CFG3_INTR_RAW_STATUS_EN_BMSK                          0x10
#define HWIO_TLMM_GPIO_INTR_CFG3_INTR_RAW_STATUS_EN_SHFT                           0x4
#define HWIO_TLMM_GPIO_INTR_CFG3_INTR_DECT_CTL_BMSK                                0xc
#define HWIO_TLMM_GPIO_INTR_CFG3_INTR_DECT_CTL_SHFT                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG3_INTR_POL_CTL_BMSK                                 0x2
#define HWIO_TLMM_GPIO_INTR_CFG3_INTR_POL_CTL_SHFT                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG3_INTR_ENABLE_BMSK                                  0x1
#define HWIO_TLMM_GPIO_INTR_CFG3_INTR_ENABLE_SHFT                                  0x0

#define HWIO_TLMM_GPIO_INTR_CFG4_ADDR                                       (TLMM_CSR_REG_BASE      + 0x00004008)
#define HWIO_TLMM_GPIO_INTR_CFG4_RMSK                                            0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG4_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG4_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG4_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG4_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG4_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG4_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG4_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG4_IN)
#define HWIO_TLMM_GPIO_INTR_CFG4_DIR_CONN_EN_BMSK                                0x100
#define HWIO_TLMM_GPIO_INTR_CFG4_DIR_CONN_EN_SHFT                                  0x8
#define HWIO_TLMM_GPIO_INTR_CFG4_TARGET_PROC_BMSK                                 0xe0
#define HWIO_TLMM_GPIO_INTR_CFG4_TARGET_PROC_SHFT                                  0x5
#define HWIO_TLMM_GPIO_INTR_CFG4_INTR_RAW_STATUS_EN_BMSK                          0x10
#define HWIO_TLMM_GPIO_INTR_CFG4_INTR_RAW_STATUS_EN_SHFT                           0x4
#define HWIO_TLMM_GPIO_INTR_CFG4_INTR_DECT_CTL_BMSK                                0xc
#define HWIO_TLMM_GPIO_INTR_CFG4_INTR_DECT_CTL_SHFT                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG4_INTR_POL_CTL_BMSK                                 0x2
#define HWIO_TLMM_GPIO_INTR_CFG4_INTR_POL_CTL_SHFT                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG4_INTR_ENABLE_BMSK                                  0x1
#define HWIO_TLMM_GPIO_INTR_CFG4_INTR_ENABLE_SHFT                                  0x0

#define HWIO_TLMM_GPIO_INTR_CFG5_ADDR                                       (TLMM_CSR_REG_BASE      + 0x00005008)
#define HWIO_TLMM_GPIO_INTR_CFG5_RMSK                                            0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG5_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG5_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG5_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG5_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG5_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG5_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG5_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG5_IN)
#define HWIO_TLMM_GPIO_INTR_CFG5_DIR_CONN_EN_BMSK                                0x100
#define HWIO_TLMM_GPIO_INTR_CFG5_DIR_CONN_EN_SHFT                                  0x8
#define HWIO_TLMM_GPIO_INTR_CFG5_TARGET_PROC_BMSK                                 0xe0
#define HWIO_TLMM_GPIO_INTR_CFG5_TARGET_PROC_SHFT                                  0x5
#define HWIO_TLMM_GPIO_INTR_CFG5_INTR_RAW_STATUS_EN_BMSK                          0x10
#define HWIO_TLMM_GPIO_INTR_CFG5_INTR_RAW_STATUS_EN_SHFT                           0x4
#define HWIO_TLMM_GPIO_INTR_CFG5_INTR_DECT_CTL_BMSK                                0xc
#define HWIO_TLMM_GPIO_INTR_CFG5_INTR_DECT_CTL_SHFT                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG5_INTR_POL_CTL_BMSK                                 0x2
#define HWIO_TLMM_GPIO_INTR_CFG5_INTR_POL_CTL_SHFT                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG5_INTR_ENABLE_BMSK                                  0x1
#define HWIO_TLMM_GPIO_INTR_CFG5_INTR_ENABLE_SHFT                                  0x0

#define HWIO_TLMM_GPIO_INTR_CFG6_ADDR                                       (TLMM_CSR_REG_BASE      + 0x00006008)
#define HWIO_TLMM_GPIO_INTR_CFG6_RMSK                                            0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG6_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG6_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG6_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG6_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG6_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG6_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG6_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG6_IN)
#define HWIO_TLMM_GPIO_INTR_CFG6_DIR_CONN_EN_BMSK                                0x100
#define HWIO_TLMM_GPIO_INTR_CFG6_DIR_CONN_EN_SHFT                                  0x8
#define HWIO_TLMM_GPIO_INTR_CFG6_TARGET_PROC_BMSK                                 0xe0
#define HWIO_TLMM_GPIO_INTR_CFG6_TARGET_PROC_SHFT                                  0x5
#define HWIO_TLMM_GPIO_INTR_CFG6_INTR_RAW_STATUS_EN_BMSK                          0x10
#define HWIO_TLMM_GPIO_INTR_CFG6_INTR_RAW_STATUS_EN_SHFT                           0x4
#define HWIO_TLMM_GPIO_INTR_CFG6_INTR_DECT_CTL_BMSK                                0xc
#define HWIO_TLMM_GPIO_INTR_CFG6_INTR_DECT_CTL_SHFT                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG6_INTR_POL_CTL_BMSK                                 0x2
#define HWIO_TLMM_GPIO_INTR_CFG6_INTR_POL_CTL_SHFT                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG6_INTR_ENABLE_BMSK                                  0x1
#define HWIO_TLMM_GPIO_INTR_CFG6_INTR_ENABLE_SHFT                                  0x0

#define HWIO_TLMM_GPIO_INTR_CFG7_ADDR                                       (TLMM_CSR_REG_BASE      + 0x00007008)
#define HWIO_TLMM_GPIO_INTR_CFG7_RMSK                                            0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG7_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG7_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG7_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG7_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG7_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG7_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG7_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG7_IN)
#define HWIO_TLMM_GPIO_INTR_CFG7_DIR_CONN_EN_BMSK                                0x100
#define HWIO_TLMM_GPIO_INTR_CFG7_DIR_CONN_EN_SHFT                                  0x8
#define HWIO_TLMM_GPIO_INTR_CFG7_TARGET_PROC_BMSK                                 0xe0
#define HWIO_TLMM_GPIO_INTR_CFG7_TARGET_PROC_SHFT                                  0x5
#define HWIO_TLMM_GPIO_INTR_CFG7_INTR_RAW_STATUS_EN_BMSK                          0x10
#define HWIO_TLMM_GPIO_INTR_CFG7_INTR_RAW_STATUS_EN_SHFT                           0x4
#define HWIO_TLMM_GPIO_INTR_CFG7_INTR_DECT_CTL_BMSK                                0xc
#define HWIO_TLMM_GPIO_INTR_CFG7_INTR_DECT_CTL_SHFT                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG7_INTR_POL_CTL_BMSK                                 0x2
#define HWIO_TLMM_GPIO_INTR_CFG7_INTR_POL_CTL_SHFT                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG7_INTR_ENABLE_BMSK                                  0x1
#define HWIO_TLMM_GPIO_INTR_CFG7_INTR_ENABLE_SHFT                                  0x0

#define HWIO_TLMM_GPIO_INTR_CFG8_ADDR                                       (TLMM_CSR_REG_BASE      + 0x00008008)
#define HWIO_TLMM_GPIO_INTR_CFG8_RMSK                                            0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG8_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG8_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG8_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG8_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG8_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG8_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG8_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG8_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG8_IN)
#define HWIO_TLMM_GPIO_INTR_CFG8_DIR_CONN_EN_BMSK                                0x100
#define HWIO_TLMM_GPIO_INTR_CFG8_DIR_CONN_EN_SHFT                                  0x8
#define HWIO_TLMM_GPIO_INTR_CFG8_TARGET_PROC_BMSK                                 0xe0
#define HWIO_TLMM_GPIO_INTR_CFG8_TARGET_PROC_SHFT                                  0x5
#define HWIO_TLMM_GPIO_INTR_CFG8_INTR_RAW_STATUS_EN_BMSK                          0x10
#define HWIO_TLMM_GPIO_INTR_CFG8_INTR_RAW_STATUS_EN_SHFT                           0x4
#define HWIO_TLMM_GPIO_INTR_CFG8_INTR_DECT_CTL_BMSK                                0xc
#define HWIO_TLMM_GPIO_INTR_CFG8_INTR_DECT_CTL_SHFT                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG8_INTR_POL_CTL_BMSK                                 0x2
#define HWIO_TLMM_GPIO_INTR_CFG8_INTR_POL_CTL_SHFT                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG8_INTR_ENABLE_BMSK                                  0x1
#define HWIO_TLMM_GPIO_INTR_CFG8_INTR_ENABLE_SHFT                                  0x0

#define HWIO_TLMM_GPIO_INTR_CFG9_ADDR                                       (TLMM_CSR_REG_BASE      + 0x00009008)
#define HWIO_TLMM_GPIO_INTR_CFG9_RMSK                                            0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG9_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG9_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG9_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG9_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG9_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG9_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG9_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG9_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG9_IN)
#define HWIO_TLMM_GPIO_INTR_CFG9_DIR_CONN_EN_BMSK                                0x100
#define HWIO_TLMM_GPIO_INTR_CFG9_DIR_CONN_EN_SHFT                                  0x8
#define HWIO_TLMM_GPIO_INTR_CFG9_TARGET_PROC_BMSK                                 0xe0
#define HWIO_TLMM_GPIO_INTR_CFG9_TARGET_PROC_SHFT                                  0x5
#define HWIO_TLMM_GPIO_INTR_CFG9_INTR_RAW_STATUS_EN_BMSK                          0x10
#define HWIO_TLMM_GPIO_INTR_CFG9_INTR_RAW_STATUS_EN_SHFT                           0x4
#define HWIO_TLMM_GPIO_INTR_CFG9_INTR_DECT_CTL_BMSK                                0xc
#define HWIO_TLMM_GPIO_INTR_CFG9_INTR_DECT_CTL_SHFT                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG9_INTR_POL_CTL_BMSK                                 0x2
#define HWIO_TLMM_GPIO_INTR_CFG9_INTR_POL_CTL_SHFT                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG9_INTR_ENABLE_BMSK                                  0x1
#define HWIO_TLMM_GPIO_INTR_CFG9_INTR_ENABLE_SHFT                                  0x0

#define HWIO_TLMM_GPIO_INTR_CFG10_ADDR                                      (TLMM_CSR_REG_BASE      + 0x0000a008)
#define HWIO_TLMM_GPIO_INTR_CFG10_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG10_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG10_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG10_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG10_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG10_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG10_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG10_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG10_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG10_IN)
#define HWIO_TLMM_GPIO_INTR_CFG10_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG10_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG10_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG10_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG10_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG10_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG10_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG10_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG10_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG10_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG10_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG10_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG11_ADDR                                      (TLMM_CSR_REG_BASE      + 0x0000b008)
#define HWIO_TLMM_GPIO_INTR_CFG11_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG11_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG11_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG11_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG11_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG11_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG11_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG11_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG11_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG11_IN)
#define HWIO_TLMM_GPIO_INTR_CFG11_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG11_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG11_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG11_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG11_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG11_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG11_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG11_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG11_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG11_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG11_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG11_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG12_ADDR                                      (TLMM_CSR_REG_BASE      + 0x0000c008)
#define HWIO_TLMM_GPIO_INTR_CFG12_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG12_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG12_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG12_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG12_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG12_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG12_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG12_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG12_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG12_IN)
#define HWIO_TLMM_GPIO_INTR_CFG12_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG12_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG12_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG12_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG12_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG12_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG12_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG12_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG12_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG12_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG12_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG12_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG13_ADDR                                      (TLMM_CSR_REG_BASE      + 0x0000d008)
#define HWIO_TLMM_GPIO_INTR_CFG13_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG13_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG13_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG13_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG13_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG13_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG13_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG13_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG13_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG13_IN)
#define HWIO_TLMM_GPIO_INTR_CFG13_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG13_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG13_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG13_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG13_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG13_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG13_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG13_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG13_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG13_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG13_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG13_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG14_ADDR                                      (TLMM_CSR_REG_BASE      + 0x0000e008)
#define HWIO_TLMM_GPIO_INTR_CFG14_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG14_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG14_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG14_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG14_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG14_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG14_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG14_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG14_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG14_IN)
#define HWIO_TLMM_GPIO_INTR_CFG14_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG14_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG14_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG14_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG14_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG14_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG14_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG14_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG14_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG14_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG14_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG14_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG15_ADDR                                      (TLMM_CSR_REG_BASE      + 0x0000f008)
#define HWIO_TLMM_GPIO_INTR_CFG15_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG15_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG15_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG15_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG15_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG15_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG15_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG15_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG15_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG15_IN)
#define HWIO_TLMM_GPIO_INTR_CFG15_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG15_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG15_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG15_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG15_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG15_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG15_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG15_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG15_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG15_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG15_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG15_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG16_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00010008)
#define HWIO_TLMM_GPIO_INTR_CFG16_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG16_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG16_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG16_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG16_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG16_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG16_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG16_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG16_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG16_IN)
#define HWIO_TLMM_GPIO_INTR_CFG16_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG16_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG16_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG16_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG16_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG16_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG16_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG16_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG16_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG16_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG16_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG16_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG17_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00011008)
#define HWIO_TLMM_GPIO_INTR_CFG17_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG17_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG17_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG17_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG17_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG17_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG17_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG17_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG17_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG17_IN)
#define HWIO_TLMM_GPIO_INTR_CFG17_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG17_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG17_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG17_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG17_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG17_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG17_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG17_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG17_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG17_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG17_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG17_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG18_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00012008)
#define HWIO_TLMM_GPIO_INTR_CFG18_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG18_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG18_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG18_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG18_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG18_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG18_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG18_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG18_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG18_IN)
#define HWIO_TLMM_GPIO_INTR_CFG18_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG18_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG18_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG18_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG18_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG18_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG18_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG18_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG18_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG18_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG18_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG18_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG19_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00013008)
#define HWIO_TLMM_GPIO_INTR_CFG19_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG19_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG19_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG19_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG19_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG19_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG19_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG19_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG19_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG19_IN)
#define HWIO_TLMM_GPIO_INTR_CFG19_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG19_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG19_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG19_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG19_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG19_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG19_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG19_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG19_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG19_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG19_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG19_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG20_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00014008)
#define HWIO_TLMM_GPIO_INTR_CFG20_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG20_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG20_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG20_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG20_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG20_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG20_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG20_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG20_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG20_IN)
#define HWIO_TLMM_GPIO_INTR_CFG20_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG20_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG20_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG20_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG20_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG20_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG20_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG20_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG20_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG20_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG20_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG20_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG21_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00015008)
#define HWIO_TLMM_GPIO_INTR_CFG21_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG21_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG21_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG21_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG21_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG21_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG21_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG21_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG21_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG21_IN)
#define HWIO_TLMM_GPIO_INTR_CFG21_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG21_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG21_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG21_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG21_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG21_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG21_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG21_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG21_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG21_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG21_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG21_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG22_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00016008)
#define HWIO_TLMM_GPIO_INTR_CFG22_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG22_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG22_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG22_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG22_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG22_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG22_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG22_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG22_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG22_IN)
#define HWIO_TLMM_GPIO_INTR_CFG22_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG22_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG22_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG22_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG22_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG22_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG22_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG22_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG22_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG22_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG22_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG22_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG23_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00017008)
#define HWIO_TLMM_GPIO_INTR_CFG23_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG23_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG23_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG23_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG23_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG23_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG23_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG23_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG23_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG23_IN)
#define HWIO_TLMM_GPIO_INTR_CFG23_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG23_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG23_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG23_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG23_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG23_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG23_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG23_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG23_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG23_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG23_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG23_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG24_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00018008)
#define HWIO_TLMM_GPIO_INTR_CFG24_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG24_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG24_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG24_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG24_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG24_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG24_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG24_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG24_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG24_IN)
#define HWIO_TLMM_GPIO_INTR_CFG24_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG24_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG24_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG24_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG24_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG24_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG24_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG24_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG24_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG24_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG24_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG24_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG25_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00019008)
#define HWIO_TLMM_GPIO_INTR_CFG25_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG25_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG25_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG25_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG25_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG25_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG25_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG25_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG25_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG25_IN)
#define HWIO_TLMM_GPIO_INTR_CFG25_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG25_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG25_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG25_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG25_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG25_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG25_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG25_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG25_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG25_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG25_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG25_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG26_ADDR                                      (TLMM_CSR_REG_BASE      + 0x0001a008)
#define HWIO_TLMM_GPIO_INTR_CFG26_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG26_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG26_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG26_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG26_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG26_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG26_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG26_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG26_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG26_IN)
#define HWIO_TLMM_GPIO_INTR_CFG26_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG26_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG26_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG26_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG26_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG26_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG26_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG26_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG26_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG26_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG26_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG26_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG27_ADDR                                      (TLMM_CSR_REG_BASE      + 0x0001b008)
#define HWIO_TLMM_GPIO_INTR_CFG27_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG27_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG27_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG27_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG27_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG27_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG27_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG27_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG27_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG27_IN)
#define HWIO_TLMM_GPIO_INTR_CFG27_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG27_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG27_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG27_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG27_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG27_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG27_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG27_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG27_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG27_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG27_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG27_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG28_ADDR                                      (TLMM_CSR_REG_BASE      + 0x0001c008)
#define HWIO_TLMM_GPIO_INTR_CFG28_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG28_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG28_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG28_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG28_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG28_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG28_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG28_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG28_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG28_IN)
#define HWIO_TLMM_GPIO_INTR_CFG28_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG28_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG28_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG28_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG28_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG28_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG28_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG28_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG28_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG28_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG28_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG28_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG29_ADDR                                      (TLMM_CSR_REG_BASE      + 0x0001d008)
#define HWIO_TLMM_GPIO_INTR_CFG29_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG29_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG29_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG29_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG29_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG29_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG29_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG29_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG29_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG29_IN)
#define HWIO_TLMM_GPIO_INTR_CFG29_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG29_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG29_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG29_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG29_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG29_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG29_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG29_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG29_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG29_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG29_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG29_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG30_ADDR                                      (TLMM_CSR_REG_BASE      + 0x0001e008)
#define HWIO_TLMM_GPIO_INTR_CFG30_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG30_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG30_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG30_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG30_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG30_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG30_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG30_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG30_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG30_IN)
#define HWIO_TLMM_GPIO_INTR_CFG30_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG30_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG30_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG30_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG30_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG30_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG30_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG30_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG30_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG30_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG30_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG30_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG31_ADDR                                      (TLMM_CSR_REG_BASE      + 0x0001f008)
#define HWIO_TLMM_GPIO_INTR_CFG31_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG31_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG31_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG31_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG31_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG31_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG31_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG31_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG31_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG31_IN)
#define HWIO_TLMM_GPIO_INTR_CFG31_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG31_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG31_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG31_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG31_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG31_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG31_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG31_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG31_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG31_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG31_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG31_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG32_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00020008)
#define HWIO_TLMM_GPIO_INTR_CFG32_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG32_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG32_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG32_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG32_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG32_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG32_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG32_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG32_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG32_IN)
#define HWIO_TLMM_GPIO_INTR_CFG32_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG32_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG32_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG32_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG32_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG32_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG32_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG32_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG32_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG32_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG32_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG32_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG33_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00021008)
#define HWIO_TLMM_GPIO_INTR_CFG33_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG33_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG33_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG33_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG33_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG33_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG33_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG33_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG33_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG33_IN)
#define HWIO_TLMM_GPIO_INTR_CFG33_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG33_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG33_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG33_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG33_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG33_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG33_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG33_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG33_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG33_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG33_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG33_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG34_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00022008)
#define HWIO_TLMM_GPIO_INTR_CFG34_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG34_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG34_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG34_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG34_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG34_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG34_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG34_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG34_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG34_IN)
#define HWIO_TLMM_GPIO_INTR_CFG34_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG34_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG34_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG34_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG34_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG34_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG34_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG34_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG34_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG34_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG34_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG34_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG35_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00023008)
#define HWIO_TLMM_GPIO_INTR_CFG35_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG35_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG35_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG35_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG35_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG35_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG35_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG35_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG35_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG35_IN)
#define HWIO_TLMM_GPIO_INTR_CFG35_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG35_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG35_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG35_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG35_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG35_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG35_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG35_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG35_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG35_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG35_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG35_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG36_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00024008)
#define HWIO_TLMM_GPIO_INTR_CFG36_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG36_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG36_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG36_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG36_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG36_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG36_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG36_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG36_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG36_IN)
#define HWIO_TLMM_GPIO_INTR_CFG36_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG36_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG36_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG36_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG36_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG36_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG36_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG36_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG36_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG36_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG36_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG36_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG37_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00025008)
#define HWIO_TLMM_GPIO_INTR_CFG37_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG37_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG37_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG37_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG37_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG37_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG37_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG37_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG37_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG37_IN)
#define HWIO_TLMM_GPIO_INTR_CFG37_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG37_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG37_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG37_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG37_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG37_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG37_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG37_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG37_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG37_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG37_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG37_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG38_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00026008)
#define HWIO_TLMM_GPIO_INTR_CFG38_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG38_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG38_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG38_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG38_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG38_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG38_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG38_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG38_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG38_IN)
#define HWIO_TLMM_GPIO_INTR_CFG38_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG38_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG38_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG38_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG38_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG38_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG38_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG38_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG38_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG38_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG38_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG38_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG39_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00027008)
#define HWIO_TLMM_GPIO_INTR_CFG39_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG39_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG39_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG39_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG39_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG39_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG39_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG39_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG39_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG39_IN)
#define HWIO_TLMM_GPIO_INTR_CFG39_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG39_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG39_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG39_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG39_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG39_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG39_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG39_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG39_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG39_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG39_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG39_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG40_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00028008)
#define HWIO_TLMM_GPIO_INTR_CFG40_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG40_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG40_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG40_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG40_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG40_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG40_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG40_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG40_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG40_IN)
#define HWIO_TLMM_GPIO_INTR_CFG40_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG40_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG40_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG40_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG40_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG40_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG40_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG40_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG40_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG40_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG40_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG40_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG41_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00029008)
#define HWIO_TLMM_GPIO_INTR_CFG41_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG41_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG41_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG41_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG41_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG41_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG41_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG41_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG41_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG41_IN)
#define HWIO_TLMM_GPIO_INTR_CFG41_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG41_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG41_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG41_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG41_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG41_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG41_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG41_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG41_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG41_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG41_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG41_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG42_ADDR                                      (TLMM_CSR_REG_BASE      + 0x0002a008)
#define HWIO_TLMM_GPIO_INTR_CFG42_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG42_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG42_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG42_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG42_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG42_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG42_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG42_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG42_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG42_IN)
#define HWIO_TLMM_GPIO_INTR_CFG42_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG42_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG42_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG42_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG42_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG42_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG42_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG42_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG42_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG42_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG42_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG42_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG43_ADDR                                      (TLMM_CSR_REG_BASE      + 0x0002b008)
#define HWIO_TLMM_GPIO_INTR_CFG43_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG43_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG43_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG43_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG43_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG43_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG43_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG43_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG43_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG43_IN)
#define HWIO_TLMM_GPIO_INTR_CFG43_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG43_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG43_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG43_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG43_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG43_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG43_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG43_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG43_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG43_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG43_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG43_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG44_ADDR                                      (TLMM_CSR_REG_BASE      + 0x0002c008)
#define HWIO_TLMM_GPIO_INTR_CFG44_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG44_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG44_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG44_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG44_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG44_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG44_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG44_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG44_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG44_IN)
#define HWIO_TLMM_GPIO_INTR_CFG44_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG44_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG44_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG44_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG44_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG44_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG44_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG44_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG44_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG44_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG44_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG44_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG45_ADDR                                      (TLMM_CSR_REG_BASE      + 0x0002d008)
#define HWIO_TLMM_GPIO_INTR_CFG45_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG45_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG45_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG45_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG45_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG45_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG45_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG45_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG45_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG45_IN)
#define HWIO_TLMM_GPIO_INTR_CFG45_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG45_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG45_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG45_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG45_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG45_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG45_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG45_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG45_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG45_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG45_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG45_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG46_ADDR                                      (TLMM_CSR_REG_BASE      + 0x0002e008)
#define HWIO_TLMM_GPIO_INTR_CFG46_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG46_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG46_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG46_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG46_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG46_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG46_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG46_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG46_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG46_IN)
#define HWIO_TLMM_GPIO_INTR_CFG46_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG46_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG46_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG46_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG46_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG46_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG46_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG46_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG46_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG46_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG46_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG46_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG47_ADDR                                      (TLMM_CSR_REG_BASE      + 0x0002f008)
#define HWIO_TLMM_GPIO_INTR_CFG47_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG47_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG47_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG47_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG47_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG47_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG47_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG47_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG47_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG47_IN)
#define HWIO_TLMM_GPIO_INTR_CFG47_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG47_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG47_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG47_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG47_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG47_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG47_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG47_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG47_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG47_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG47_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG47_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG48_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00030008)
#define HWIO_TLMM_GPIO_INTR_CFG48_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG48_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG48_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG48_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG48_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG48_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG48_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG48_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG48_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG48_IN)
#define HWIO_TLMM_GPIO_INTR_CFG48_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG48_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG48_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG48_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG48_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG48_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG48_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG48_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG48_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG48_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG48_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG48_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG49_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00031008)
#define HWIO_TLMM_GPIO_INTR_CFG49_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG49_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG49_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG49_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG49_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG49_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG49_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG49_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG49_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG49_IN)
#define HWIO_TLMM_GPIO_INTR_CFG49_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG49_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG49_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG49_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG49_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG49_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG49_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG49_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG49_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG49_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG49_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG49_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG50_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00032008)
#define HWIO_TLMM_GPIO_INTR_CFG50_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG50_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG50_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG50_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG50_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG50_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG50_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG50_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG50_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG50_IN)
#define HWIO_TLMM_GPIO_INTR_CFG50_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG50_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG50_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG50_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG50_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG50_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG50_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG50_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG50_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG50_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG50_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG50_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG51_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00033008)
#define HWIO_TLMM_GPIO_INTR_CFG51_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG51_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG51_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG51_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG51_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG51_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG51_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG51_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG51_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG51_IN)
#define HWIO_TLMM_GPIO_INTR_CFG51_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG51_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG51_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG51_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG51_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG51_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG51_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG51_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG51_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG51_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG51_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG51_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG52_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00034008)
#define HWIO_TLMM_GPIO_INTR_CFG52_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG52_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG52_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG52_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG52_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG52_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG52_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG52_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG52_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG52_IN)
#define HWIO_TLMM_GPIO_INTR_CFG52_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG52_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG52_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG52_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG52_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG52_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG52_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG52_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG52_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG52_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG52_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG52_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG53_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00035008)
#define HWIO_TLMM_GPIO_INTR_CFG53_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG53_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG53_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG53_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG53_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG53_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG53_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG53_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG53_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG53_IN)
#define HWIO_TLMM_GPIO_INTR_CFG53_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG53_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG53_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG53_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG53_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG53_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG53_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG53_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG53_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG53_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG53_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG53_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG54_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00036008)
#define HWIO_TLMM_GPIO_INTR_CFG54_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG54_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG54_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG54_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG54_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG54_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG54_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG54_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG54_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG54_IN)
#define HWIO_TLMM_GPIO_INTR_CFG54_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG54_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG54_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG54_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG54_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG54_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG54_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG54_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG54_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG54_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG54_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG54_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG55_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00037008)
#define HWIO_TLMM_GPIO_INTR_CFG55_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG55_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG55_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG55_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG55_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG55_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG55_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG55_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG55_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG55_IN)
#define HWIO_TLMM_GPIO_INTR_CFG55_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG55_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG55_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG55_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG55_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG55_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG55_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG55_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG55_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG55_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG55_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG55_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG56_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00038008)
#define HWIO_TLMM_GPIO_INTR_CFG56_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG56_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG56_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG56_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG56_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG56_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG56_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG56_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG56_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG56_IN)
#define HWIO_TLMM_GPIO_INTR_CFG56_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG56_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG56_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG56_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG56_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG56_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG56_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG56_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG56_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG56_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG56_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG56_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG57_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00039008)
#define HWIO_TLMM_GPIO_INTR_CFG57_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG57_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG57_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG57_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG57_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG57_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG57_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG57_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG57_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG57_IN)
#define HWIO_TLMM_GPIO_INTR_CFG57_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG57_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG57_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG57_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG57_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG57_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG57_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG57_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG57_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG57_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG57_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG57_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG58_ADDR                                      (TLMM_CSR_REG_BASE      + 0x0003a008)
#define HWIO_TLMM_GPIO_INTR_CFG58_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG58_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG58_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG58_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG58_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG58_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG58_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG58_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG58_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG58_IN)
#define HWIO_TLMM_GPIO_INTR_CFG58_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG58_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG58_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG58_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG58_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG58_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG58_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG58_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG58_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG58_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG58_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG58_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG59_ADDR                                      (TLMM_CSR_REG_BASE      + 0x0003b008)
#define HWIO_TLMM_GPIO_INTR_CFG59_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG59_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG59_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG59_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG59_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG59_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG59_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG59_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG59_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG59_IN)
#define HWIO_TLMM_GPIO_INTR_CFG59_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG59_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG59_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG59_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG59_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG59_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG59_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG59_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG59_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG59_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG59_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG59_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG60_ADDR                                      (TLMM_CSR_REG_BASE      + 0x0003c008)
#define HWIO_TLMM_GPIO_INTR_CFG60_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG60_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG60_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG60_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG60_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG60_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG60_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG60_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG60_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG60_IN)
#define HWIO_TLMM_GPIO_INTR_CFG60_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG60_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG60_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG60_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG60_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG60_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG60_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG60_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG60_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG60_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG60_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG60_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG61_ADDR                                      (TLMM_CSR_REG_BASE      + 0x0003d008)
#define HWIO_TLMM_GPIO_INTR_CFG61_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG61_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG61_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG61_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG61_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG61_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG61_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG61_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG61_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG61_IN)
#define HWIO_TLMM_GPIO_INTR_CFG61_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG61_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG61_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG61_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG61_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG61_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG61_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG61_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG61_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG61_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG61_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG61_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_CFG62_ADDR                                      (TLMM_CSR_REG_BASE      + 0x0003e008)
#define HWIO_TLMM_GPIO_INTR_CFG62_RMSK                                           0x1ff
#define HWIO_TLMM_GPIO_INTR_CFG62_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_CFG62_ADDR)
#define HWIO_TLMM_GPIO_INTR_CFG62_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_CFG62_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_CFG62_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_CFG62_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_CFG62_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_CFG62_ADDR,m,v,HWIO_TLMM_GPIO_INTR_CFG62_IN)
#define HWIO_TLMM_GPIO_INTR_CFG62_DIR_CONN_EN_BMSK                               0x100
#define HWIO_TLMM_GPIO_INTR_CFG62_DIR_CONN_EN_SHFT                                 0x8
#define HWIO_TLMM_GPIO_INTR_CFG62_TARGET_PROC_BMSK                                0xe0
#define HWIO_TLMM_GPIO_INTR_CFG62_TARGET_PROC_SHFT                                 0x5
#define HWIO_TLMM_GPIO_INTR_CFG62_INTR_RAW_STATUS_EN_BMSK                         0x10
#define HWIO_TLMM_GPIO_INTR_CFG62_INTR_RAW_STATUS_EN_SHFT                          0x4
#define HWIO_TLMM_GPIO_INTR_CFG62_INTR_DECT_CTL_BMSK                               0xc
#define HWIO_TLMM_GPIO_INTR_CFG62_INTR_DECT_CTL_SHFT                               0x2
#define HWIO_TLMM_GPIO_INTR_CFG62_INTR_POL_CTL_BMSK                                0x2
#define HWIO_TLMM_GPIO_INTR_CFG62_INTR_POL_CTL_SHFT                                0x1
#define HWIO_TLMM_GPIO_INTR_CFG62_INTR_ENABLE_BMSK                                 0x1
#define HWIO_TLMM_GPIO_INTR_CFG62_INTR_ENABLE_SHFT                                 0x0

#define HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(n)                                 (TLMM_CSR_REG_BASE      + 0x0000000c + 0x1000 * (n))
#define HWIO_TLMM_GPIO_INTR_STATUSn_RMSK                                           0x1
#define HWIO_TLMM_GPIO_INTR_STATUSn_MAXn                                            62
#define HWIO_TLMM_GPIO_INTR_STATUSn_INI(n)        \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(n), HWIO_TLMM_GPIO_INTR_STATUSn_RMSK)
#define HWIO_TLMM_GPIO_INTR_STATUSn_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(n), mask)
#define HWIO_TLMM_GPIO_INTR_STATUSn_OUTI(n,val)    \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(n),val)
#define HWIO_TLMM_GPIO_INTR_STATUSn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUSn_ADDR(n),mask,val,HWIO_TLMM_GPIO_INTR_STATUSn_INI(n))
#define HWIO_TLMM_GPIO_INTR_STATUSn_INTR_STATUS_BMSK                               0x1
#define HWIO_TLMM_GPIO_INTR_STATUSn_INTR_STATUS_SHFT                               0x0

#define HWIO_TLMM_GPIO_INTR_STATUS0_ADDR                                    (TLMM_CSR_REG_BASE      + 0x0000000c)
#define HWIO_TLMM_GPIO_INTR_STATUS0_RMSK                                           0x1
#define HWIO_TLMM_GPIO_INTR_STATUS0_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS0_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS0_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS0_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS0_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS0_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS0_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS0_INTR_STATUS_BMSK                               0x1
#define HWIO_TLMM_GPIO_INTR_STATUS0_INTR_STATUS_SHFT                               0x0

#define HWIO_TLMM_GPIO_INTR_STATUS1_ADDR                                    (TLMM_CSR_REG_BASE      + 0x0000100c)
#define HWIO_TLMM_GPIO_INTR_STATUS1_RMSK                                           0x1
#define HWIO_TLMM_GPIO_INTR_STATUS1_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS1_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS1_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS1_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS1_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS1_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS1_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS1_INTR_STATUS_BMSK                               0x1
#define HWIO_TLMM_GPIO_INTR_STATUS1_INTR_STATUS_SHFT                               0x0

#define HWIO_TLMM_GPIO_INTR_STATUS2_ADDR                                    (TLMM_CSR_REG_BASE      + 0x0000200c)
#define HWIO_TLMM_GPIO_INTR_STATUS2_RMSK                                           0x1
#define HWIO_TLMM_GPIO_INTR_STATUS2_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS2_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS2_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS2_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS2_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS2_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS2_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS2_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS2_INTR_STATUS_BMSK                               0x1
#define HWIO_TLMM_GPIO_INTR_STATUS2_INTR_STATUS_SHFT                               0x0

#define HWIO_TLMM_GPIO_INTR_STATUS3_ADDR                                    (TLMM_CSR_REG_BASE      + 0x0000300c)
#define HWIO_TLMM_GPIO_INTR_STATUS3_RMSK                                           0x1
#define HWIO_TLMM_GPIO_INTR_STATUS3_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS3_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS3_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS3_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS3_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS3_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS3_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS3_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS3_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS3_INTR_STATUS_BMSK                               0x1
#define HWIO_TLMM_GPIO_INTR_STATUS3_INTR_STATUS_SHFT                               0x0

#define HWIO_TLMM_GPIO_INTR_STATUS4_ADDR                                    (TLMM_CSR_REG_BASE      + 0x0000400c)
#define HWIO_TLMM_GPIO_INTR_STATUS4_RMSK                                           0x1
#define HWIO_TLMM_GPIO_INTR_STATUS4_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS4_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS4_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS4_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS4_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS4_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS4_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS4_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS4_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS4_INTR_STATUS_BMSK                               0x1
#define HWIO_TLMM_GPIO_INTR_STATUS4_INTR_STATUS_SHFT                               0x0

#define HWIO_TLMM_GPIO_INTR_STATUS5_ADDR                                    (TLMM_CSR_REG_BASE      + 0x0000500c)
#define HWIO_TLMM_GPIO_INTR_STATUS5_RMSK                                           0x1
#define HWIO_TLMM_GPIO_INTR_STATUS5_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS5_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS5_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS5_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS5_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS5_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS5_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS5_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS5_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS5_INTR_STATUS_BMSK                               0x1
#define HWIO_TLMM_GPIO_INTR_STATUS5_INTR_STATUS_SHFT                               0x0

#define HWIO_TLMM_GPIO_INTR_STATUS6_ADDR                                    (TLMM_CSR_REG_BASE      + 0x0000600c)
#define HWIO_TLMM_GPIO_INTR_STATUS6_RMSK                                           0x1
#define HWIO_TLMM_GPIO_INTR_STATUS6_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS6_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS6_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS6_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS6_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS6_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS6_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS6_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS6_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS6_INTR_STATUS_BMSK                               0x1
#define HWIO_TLMM_GPIO_INTR_STATUS6_INTR_STATUS_SHFT                               0x0

#define HWIO_TLMM_GPIO_INTR_STATUS7_ADDR                                    (TLMM_CSR_REG_BASE      + 0x0000700c)
#define HWIO_TLMM_GPIO_INTR_STATUS7_RMSK                                           0x1
#define HWIO_TLMM_GPIO_INTR_STATUS7_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS7_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS7_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS7_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS7_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS7_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS7_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS7_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS7_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS7_INTR_STATUS_BMSK                               0x1
#define HWIO_TLMM_GPIO_INTR_STATUS7_INTR_STATUS_SHFT                               0x0

#define HWIO_TLMM_GPIO_INTR_STATUS8_ADDR                                    (TLMM_CSR_REG_BASE      + 0x0000800c)
#define HWIO_TLMM_GPIO_INTR_STATUS8_RMSK                                           0x1
#define HWIO_TLMM_GPIO_INTR_STATUS8_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS8_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS8_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS8_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS8_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS8_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS8_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS8_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS8_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS8_INTR_STATUS_BMSK                               0x1
#define HWIO_TLMM_GPIO_INTR_STATUS8_INTR_STATUS_SHFT                               0x0

#define HWIO_TLMM_GPIO_INTR_STATUS9_ADDR                                    (TLMM_CSR_REG_BASE      + 0x0000900c)
#define HWIO_TLMM_GPIO_INTR_STATUS9_RMSK                                           0x1
#define HWIO_TLMM_GPIO_INTR_STATUS9_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS9_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS9_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS9_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS9_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS9_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS9_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS9_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS9_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS9_INTR_STATUS_BMSK                               0x1
#define HWIO_TLMM_GPIO_INTR_STATUS9_INTR_STATUS_SHFT                               0x0

#define HWIO_TLMM_GPIO_INTR_STATUS10_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0000a00c)
#define HWIO_TLMM_GPIO_INTR_STATUS10_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS10_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS10_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS10_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS10_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS10_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS10_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS10_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS10_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS10_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS10_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS10_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS11_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0000b00c)
#define HWIO_TLMM_GPIO_INTR_STATUS11_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS11_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS11_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS11_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS11_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS11_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS11_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS11_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS11_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS11_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS11_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS11_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS12_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0000c00c)
#define HWIO_TLMM_GPIO_INTR_STATUS12_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS12_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS12_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS12_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS12_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS12_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS12_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS12_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS12_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS12_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS12_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS12_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS13_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0000d00c)
#define HWIO_TLMM_GPIO_INTR_STATUS13_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS13_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS13_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS13_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS13_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS13_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS13_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS13_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS13_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS13_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS13_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS13_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS14_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0000e00c)
#define HWIO_TLMM_GPIO_INTR_STATUS14_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS14_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS14_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS14_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS14_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS14_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS14_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS14_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS14_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS14_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS14_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS14_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS15_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0000f00c)
#define HWIO_TLMM_GPIO_INTR_STATUS15_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS15_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS15_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS15_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS15_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS15_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS15_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS15_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS15_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS15_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS15_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS15_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS16_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0001000c)
#define HWIO_TLMM_GPIO_INTR_STATUS16_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS16_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS16_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS16_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS16_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS16_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS16_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS16_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS16_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS16_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS16_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS16_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS17_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0001100c)
#define HWIO_TLMM_GPIO_INTR_STATUS17_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS17_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS17_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS17_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS17_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS17_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS17_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS17_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS17_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS17_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS17_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS17_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS18_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0001200c)
#define HWIO_TLMM_GPIO_INTR_STATUS18_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS18_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS18_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS18_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS18_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS18_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS18_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS18_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS18_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS18_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS18_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS18_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS19_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0001300c)
#define HWIO_TLMM_GPIO_INTR_STATUS19_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS19_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS19_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS19_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS19_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS19_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS19_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS19_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS19_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS19_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS19_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS19_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS20_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0001400c)
#define HWIO_TLMM_GPIO_INTR_STATUS20_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS20_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS20_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS20_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS20_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS20_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS20_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS20_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS20_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS20_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS20_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS20_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS21_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0001500c)
#define HWIO_TLMM_GPIO_INTR_STATUS21_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS21_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS21_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS21_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS21_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS21_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS21_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS21_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS21_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS21_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS21_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS21_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS22_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0001600c)
#define HWIO_TLMM_GPIO_INTR_STATUS22_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS22_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS22_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS22_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS22_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS22_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS22_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS22_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS22_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS22_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS22_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS22_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS23_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0001700c)
#define HWIO_TLMM_GPIO_INTR_STATUS23_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS23_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS23_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS23_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS23_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS23_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS23_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS23_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS23_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS23_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS23_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS23_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS24_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0001800c)
#define HWIO_TLMM_GPIO_INTR_STATUS24_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS24_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS24_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS24_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS24_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS24_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS24_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS24_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS24_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS24_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS24_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS24_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS25_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0001900c)
#define HWIO_TLMM_GPIO_INTR_STATUS25_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS25_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS25_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS25_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS25_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS25_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS25_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS25_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS25_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS25_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS25_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS25_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS26_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0001a00c)
#define HWIO_TLMM_GPIO_INTR_STATUS26_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS26_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS26_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS26_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS26_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS26_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS26_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS26_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS26_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS26_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS26_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS26_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS27_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0001b00c)
#define HWIO_TLMM_GPIO_INTR_STATUS27_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS27_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS27_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS27_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS27_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS27_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS27_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS27_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS27_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS27_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS27_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS27_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS28_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0001c00c)
#define HWIO_TLMM_GPIO_INTR_STATUS28_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS28_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS28_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS28_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS28_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS28_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS28_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS28_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS28_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS28_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS28_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS28_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS29_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0001d00c)
#define HWIO_TLMM_GPIO_INTR_STATUS29_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS29_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS29_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS29_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS29_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS29_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS29_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS29_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS29_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS29_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS29_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS29_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS30_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0001e00c)
#define HWIO_TLMM_GPIO_INTR_STATUS30_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS30_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS30_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS30_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS30_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS30_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS30_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS30_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS30_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS30_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS30_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS30_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS31_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0001f00c)
#define HWIO_TLMM_GPIO_INTR_STATUS31_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS31_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS31_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS31_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS31_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS31_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS31_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS31_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS31_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS31_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS31_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS31_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS32_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0002000c)
#define HWIO_TLMM_GPIO_INTR_STATUS32_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS32_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS32_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS32_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS32_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS32_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS32_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS32_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS32_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS32_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS32_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS32_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS33_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0002100c)
#define HWIO_TLMM_GPIO_INTR_STATUS33_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS33_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS33_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS33_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS33_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS33_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS33_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS33_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS33_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS33_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS33_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS33_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS34_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0002200c)
#define HWIO_TLMM_GPIO_INTR_STATUS34_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS34_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS34_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS34_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS34_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS34_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS34_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS34_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS34_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS34_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS34_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS34_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS35_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0002300c)
#define HWIO_TLMM_GPIO_INTR_STATUS35_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS35_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS35_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS35_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS35_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS35_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS35_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS35_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS35_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS35_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS35_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS35_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS36_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0002400c)
#define HWIO_TLMM_GPIO_INTR_STATUS36_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS36_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS36_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS36_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS36_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS36_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS36_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS36_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS36_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS36_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS36_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS36_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS37_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0002500c)
#define HWIO_TLMM_GPIO_INTR_STATUS37_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS37_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS37_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS37_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS37_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS37_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS37_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS37_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS37_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS37_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS37_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS37_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS38_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0002600c)
#define HWIO_TLMM_GPIO_INTR_STATUS38_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS38_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS38_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS38_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS38_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS38_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS38_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS38_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS38_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS38_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS38_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS38_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS39_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0002700c)
#define HWIO_TLMM_GPIO_INTR_STATUS39_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS39_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS39_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS39_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS39_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS39_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS39_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS39_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS39_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS39_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS39_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS39_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS40_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0002800c)
#define HWIO_TLMM_GPIO_INTR_STATUS40_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS40_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS40_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS40_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS40_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS40_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS40_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS40_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS40_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS40_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS40_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS40_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS41_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0002900c)
#define HWIO_TLMM_GPIO_INTR_STATUS41_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS41_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS41_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS41_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS41_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS41_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS41_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS41_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS41_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS41_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS41_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS41_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS42_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0002a00c)
#define HWIO_TLMM_GPIO_INTR_STATUS42_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS42_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS42_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS42_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS42_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS42_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS42_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS42_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS42_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS42_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS42_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS42_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS43_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0002b00c)
#define HWIO_TLMM_GPIO_INTR_STATUS43_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS43_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS43_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS43_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS43_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS43_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS43_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS43_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS43_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS43_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS43_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS43_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS44_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0002c00c)
#define HWIO_TLMM_GPIO_INTR_STATUS44_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS44_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS44_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS44_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS44_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS44_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS44_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS44_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS44_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS44_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS44_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS44_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS45_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0002d00c)
#define HWIO_TLMM_GPIO_INTR_STATUS45_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS45_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS45_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS45_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS45_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS45_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS45_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS45_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS45_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS45_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS45_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS45_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS46_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0002e00c)
#define HWIO_TLMM_GPIO_INTR_STATUS46_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS46_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS46_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS46_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS46_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS46_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS46_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS46_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS46_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS46_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS46_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS46_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS47_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0002f00c)
#define HWIO_TLMM_GPIO_INTR_STATUS47_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS47_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS47_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS47_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS47_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS47_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS47_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS47_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS47_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS47_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS47_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS47_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS48_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0003000c)
#define HWIO_TLMM_GPIO_INTR_STATUS48_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS48_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS48_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS48_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS48_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS48_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS48_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS48_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS48_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS48_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS48_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS48_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS49_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0003100c)
#define HWIO_TLMM_GPIO_INTR_STATUS49_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS49_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS49_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS49_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS49_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS49_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS49_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS49_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS49_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS49_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS49_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS49_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS50_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0003200c)
#define HWIO_TLMM_GPIO_INTR_STATUS50_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS50_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS50_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS50_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS50_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS50_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS50_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS50_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS50_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS50_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS50_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS50_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS51_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0003300c)
#define HWIO_TLMM_GPIO_INTR_STATUS51_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS51_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS51_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS51_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS51_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS51_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS51_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS51_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS51_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS51_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS51_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS51_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS52_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0003400c)
#define HWIO_TLMM_GPIO_INTR_STATUS52_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS52_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS52_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS52_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS52_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS52_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS52_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS52_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS52_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS52_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS52_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS52_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS53_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0003500c)
#define HWIO_TLMM_GPIO_INTR_STATUS53_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS53_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS53_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS53_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS53_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS53_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS53_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS53_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS53_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS53_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS53_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS53_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS54_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0003600c)
#define HWIO_TLMM_GPIO_INTR_STATUS54_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS54_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS54_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS54_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS54_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS54_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS54_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS54_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS54_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS54_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS54_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS54_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS55_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0003700c)
#define HWIO_TLMM_GPIO_INTR_STATUS55_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS55_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS55_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS55_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS55_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS55_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS55_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS55_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS55_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS55_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS55_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS55_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS56_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0003800c)
#define HWIO_TLMM_GPIO_INTR_STATUS56_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS56_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS56_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS56_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS56_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS56_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS56_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS56_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS56_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS56_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS56_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS56_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS57_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0003900c)
#define HWIO_TLMM_GPIO_INTR_STATUS57_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS57_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS57_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS57_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS57_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS57_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS57_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS57_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS57_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS57_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS57_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS57_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS58_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0003a00c)
#define HWIO_TLMM_GPIO_INTR_STATUS58_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS58_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS58_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS58_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS58_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS58_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS58_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS58_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS58_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS58_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS58_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS58_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS59_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0003b00c)
#define HWIO_TLMM_GPIO_INTR_STATUS59_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS59_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS59_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS59_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS59_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS59_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS59_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS59_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS59_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS59_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS59_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS59_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS60_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0003c00c)
#define HWIO_TLMM_GPIO_INTR_STATUS60_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS60_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS60_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS60_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS60_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS60_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS60_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS60_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS60_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS60_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS60_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS60_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS61_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0003d00c)
#define HWIO_TLMM_GPIO_INTR_STATUS61_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS61_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS61_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS61_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS61_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS61_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS61_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS61_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS61_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS61_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS61_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS61_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_GPIO_INTR_STATUS62_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0003e00c)
#define HWIO_TLMM_GPIO_INTR_STATUS62_RMSK                                          0x1
#define HWIO_TLMM_GPIO_INTR_STATUS62_IN          \
        in_dword(HWIO_TLMM_GPIO_INTR_STATUS62_ADDR)
#define HWIO_TLMM_GPIO_INTR_STATUS62_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_INTR_STATUS62_ADDR, m)
#define HWIO_TLMM_GPIO_INTR_STATUS62_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_INTR_STATUS62_ADDR,v)
#define HWIO_TLMM_GPIO_INTR_STATUS62_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_INTR_STATUS62_ADDR,m,v,HWIO_TLMM_GPIO_INTR_STATUS62_IN)
#define HWIO_TLMM_GPIO_INTR_STATUS62_INTR_STATUS_BMSK                              0x1
#define HWIO_TLMM_GPIO_INTR_STATUS62_INTR_STATUS_SHFT                              0x0

#define HWIO_TLMM_CLK_GATE_EN_ADDR                                          (TLMM_CSR_REG_BASE      + 0x00100000)
#define HWIO_TLMM_CLK_GATE_EN_RMSK                                                 0x7
#define HWIO_TLMM_CLK_GATE_EN_IN          \
        in_dword(HWIO_TLMM_CLK_GATE_EN_ADDR)
#define HWIO_TLMM_CLK_GATE_EN_INM(m)      \
        in_dword_masked(HWIO_TLMM_CLK_GATE_EN_ADDR, m)
#define HWIO_TLMM_CLK_GATE_EN_OUT(v)      \
        out_dword(HWIO_TLMM_CLK_GATE_EN_ADDR,v)
#define HWIO_TLMM_CLK_GATE_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_CLK_GATE_EN_ADDR,m,v,HWIO_TLMM_CLK_GATE_EN_IN)
#define HWIO_TLMM_CLK_GATE_EN_AHB_HCLK_EN_BMSK                                     0x4
#define HWIO_TLMM_CLK_GATE_EN_AHB_HCLK_EN_SHFT                                     0x2
#define HWIO_TLMM_CLK_GATE_EN_SUMMARY_INTR_EN_BMSK                                 0x2
#define HWIO_TLMM_CLK_GATE_EN_SUMMARY_INTR_EN_SHFT                                 0x1
#define HWIO_TLMM_CLK_GATE_EN_CRIF_READ_EN_BMSK                                    0x1
#define HWIO_TLMM_CLK_GATE_EN_CRIF_READ_EN_SHFT                                    0x0

#define HWIO_TLMM_IE_CTRL_DISABLE_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00100004)
#define HWIO_TLMM_IE_CTRL_DISABLE_RMSK                                             0x1
#define HWIO_TLMM_IE_CTRL_DISABLE_IN          \
        in_dword(HWIO_TLMM_IE_CTRL_DISABLE_ADDR)
#define HWIO_TLMM_IE_CTRL_DISABLE_INM(m)      \
        in_dword_masked(HWIO_TLMM_IE_CTRL_DISABLE_ADDR, m)
#define HWIO_TLMM_IE_CTRL_DISABLE_OUT(v)      \
        out_dword(HWIO_TLMM_IE_CTRL_DISABLE_ADDR,v)
#define HWIO_TLMM_IE_CTRL_DISABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_IE_CTRL_DISABLE_ADDR,m,v,HWIO_TLMM_IE_CTRL_DISABLE_IN)
#define HWIO_TLMM_IE_CTRL_DISABLE_IE_CTRL_DISABLE_BMSK                             0x1
#define HWIO_TLMM_IE_CTRL_DISABLE_IE_CTRL_DISABLE_SHFT                             0x0

#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR                                  (TLMM_CSR_REG_BASE      + 0x00100008)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_RMSK                                    0x7fffff
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_IN          \
        in_dword(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_INM(m)      \
        in_dword_masked(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR, m)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_OUT(v)      \
        out_dword(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR,v)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_MPM_WAKEUP_INT_EN_0_ADDR,m,v,HWIO_TLMM_MPM_WAKEUP_INT_EN_0_IN)
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SRST_N_BMSK                             0x400000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_SRST_N_SHFT                                 0x16
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_50_BMSK                            0x200000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_50_SHFT                                0x15
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_49_BMSK                            0x100000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_49_SHFT                                0x14
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_47_BMSK                             0x80000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_47_SHFT                                0x13
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_46_BMSK                             0x40000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_46_SHFT                                0x12
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_36_BMSK                             0x20000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_36_SHFT                                0x11
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_30_BMSK                             0x10000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_30_SHFT                                0x10
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_28_BMSK                              0x8000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_28_SHFT                                 0xf
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_27_BMSK                              0x4000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_27_SHFT                                 0xe
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_26_BMSK                              0x2000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_26_SHFT                                 0xd
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_22_BMSK                              0x1000
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_22_SHFT                                 0xc
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_21_BMSK                               0x800
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_21_SHFT                                 0xb
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_20_BMSK                               0x400
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_20_SHFT                                 0xa
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_19_BMSK                               0x200
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_19_SHFT                                 0x9
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_14_BMSK                               0x100
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_14_SHFT                                 0x8
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_13_BMSK                                0x80
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_13_SHFT                                 0x7
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_9_BMSK                                 0x40
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_9_SHFT                                  0x6
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_8_BMSK                                 0x20
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_8_SHFT                                  0x5
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_6_BMSK                                 0x10
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_6_SHFT                                  0x4
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_5_BMSK                                  0x8
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_5_SHFT                                  0x3
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_2_BMSK                                  0x4
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_2_SHFT                                  0x2
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_1_BMSK                                  0x2
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_1_SHFT                                  0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_0_BMSK                                  0x1
#define HWIO_TLMM_MPM_WAKEUP_INT_EN_0_GPIO_0_SHFT                                  0x0

#define HWIO_TLMM_INT_JTAG_CTL_ADDR                                         (TLMM_CSR_REG_BASE      + 0x00110000)
#define HWIO_TLMM_INT_JTAG_CTL_RMSK                                                0x7
#define HWIO_TLMM_INT_JTAG_CTL_IN          \
        in_dword(HWIO_TLMM_INT_JTAG_CTL_ADDR)
#define HWIO_TLMM_INT_JTAG_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_INT_JTAG_CTL_ADDR, m)
#define HWIO_TLMM_INT_JTAG_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_INT_JTAG_CTL_ADDR,v)
#define HWIO_TLMM_INT_JTAG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_INT_JTAG_CTL_ADDR,m,v,HWIO_TLMM_INT_JTAG_CTL_IN)
#define HWIO_TLMM_INT_JTAG_CTL_APSS_TAP_ENA_BMSK                                   0x4
#define HWIO_TLMM_INT_JTAG_CTL_APSS_TAP_ENA_SHFT                                   0x2
#define HWIO_TLMM_INT_JTAG_CTL_QDSS_TAP_ENA_BMSK                                   0x2
#define HWIO_TLMM_INT_JTAG_CTL_QDSS_TAP_ENA_SHFT                                   0x1
#define HWIO_TLMM_INT_JTAG_CTL_MSM_TAP_ENA_BMSK                                    0x1
#define HWIO_TLMM_INT_JTAG_CTL_MSM_TAP_ENA_SHFT                                    0x0

#define HWIO_TLMM_ETM_MODE_ADDR                                             (TLMM_CSR_REG_BASE      + 0x00110004)
#define HWIO_TLMM_ETM_MODE_RMSK                                                    0x3
#define HWIO_TLMM_ETM_MODE_IN          \
        in_dword(HWIO_TLMM_ETM_MODE_ADDR)
#define HWIO_TLMM_ETM_MODE_INM(m)      \
        in_dword_masked(HWIO_TLMM_ETM_MODE_ADDR, m)
#define HWIO_TLMM_ETM_MODE_OUT(v)      \
        out_dword(HWIO_TLMM_ETM_MODE_ADDR,v)
#define HWIO_TLMM_ETM_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_ETM_MODE_ADDR,m,v,HWIO_TLMM_ETM_MODE_IN)
#define HWIO_TLMM_ETM_MODE_TRACE_OVER_SDC2_BMSK                                    0x3
#define HWIO_TLMM_ETM_MODE_TRACE_OVER_SDC2_SHFT                                    0x0

#define HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR                                      (TLMM_CSR_REG_BASE      + 0x00110008)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_RMSK                                             0x7
#define HWIO_TLMM_DBG_BUS_OUT_SEL_IN          \
        in_dword(HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_INM(m)      \
        in_dword_masked(HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR, m)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_OUT(v)      \
        out_dword(HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR,v)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_DBG_BUS_OUT_SEL_ADDR,m,v,HWIO_TLMM_DBG_BUS_OUT_SEL_IN)
#define HWIO_TLMM_DBG_BUS_OUT_SEL_QDSS_ETM_BYTE_SHIFT_BMSK                         0x4
#define HWIO_TLMM_DBG_BUS_OUT_SEL_QDSS_ETM_BYTE_SHIFT_SHFT                         0x2
#define HWIO_TLMM_DBG_BUS_OUT_SEL_COPY_SEL_BMSK                                    0x3
#define HWIO_TLMM_DBG_BUS_OUT_SEL_COPY_SEL_SHFT                                    0x0

#define HWIO_TLMM_CHIP_MODE_ADDR                                            (TLMM_CSR_REG_BASE      + 0x0011000c)
#define HWIO_TLMM_CHIP_MODE_RMSK                                                   0x3
#define HWIO_TLMM_CHIP_MODE_IN          \
        in_dword(HWIO_TLMM_CHIP_MODE_ADDR)
#define HWIO_TLMM_CHIP_MODE_INM(m)      \
        in_dword_masked(HWIO_TLMM_CHIP_MODE_ADDR, m)
#define HWIO_TLMM_CHIP_MODE_MODE1_PIN_BMSK                                         0x2
#define HWIO_TLMM_CHIP_MODE_MODE1_PIN_SHFT                                         0x1
#define HWIO_TLMM_CHIP_MODE_MODE0_PIN_BMSK                                         0x1
#define HWIO_TLMM_CHIP_MODE_MODE0_PIN_SHFT                                         0x0

#define HWIO_TLMM_SPARE_ADDR                                                (TLMM_CSR_REG_BASE      + 0x0010e000)
#define HWIO_TLMM_SPARE_RMSK                                                0xffffffff
#define HWIO_TLMM_SPARE_IN          \
        in_dword(HWIO_TLMM_SPARE_ADDR)
#define HWIO_TLMM_SPARE_INM(m)      \
        in_dword_masked(HWIO_TLMM_SPARE_ADDR, m)
#define HWIO_TLMM_SPARE_OUT(v)      \
        out_dword(HWIO_TLMM_SPARE_ADDR,v)
#define HWIO_TLMM_SPARE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_SPARE_ADDR,m,v,HWIO_TLMM_SPARE_IN)
#define HWIO_TLMM_SPARE_SPARE_BMSK                                          0xffffff00
#define HWIO_TLMM_SPARE_SPARE_SHFT                                                 0x8
#define HWIO_TLMM_SPARE_MISC_BMSK                                                 0xff
#define HWIO_TLMM_SPARE_MISC_SHFT                                                  0x0

#define HWIO_SPARE1_ADDR                                                    (TLMM_CSR_REG_BASE      + 0x0011001c)
#define HWIO_SPARE1_RMSK                                                    0xffffffff
#define HWIO_SPARE1_IN          \
        in_dword(HWIO_SPARE1_ADDR)
#define HWIO_SPARE1_INM(m)      \
        in_dword_masked(HWIO_SPARE1_ADDR, m)
#define HWIO_SPARE1_OUT(v)      \
        out_dword(HWIO_SPARE1_ADDR,v)
#define HWIO_SPARE1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPARE1_ADDR,m,v,HWIO_SPARE1_IN)
#define HWIO_SPARE1_MISC1_BMSK                                              0xffffffff
#define HWIO_SPARE1_MISC1_SHFT                                                     0x0

#define HWIO_SPARE2_ADDR                                                    (TLMM_CSR_REG_BASE      + 0x00110020)
#define HWIO_SPARE2_RMSK                                                    0xffffffff
#define HWIO_SPARE2_IN          \
        in_dword(HWIO_SPARE2_ADDR)
#define HWIO_SPARE2_INM(m)      \
        in_dword_masked(HWIO_SPARE2_ADDR, m)
#define HWIO_SPARE2_OUT(v)      \
        out_dword(HWIO_SPARE2_ADDR,v)
#define HWIO_SPARE2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPARE2_ADDR,m,v,HWIO_SPARE2_IN)
#define HWIO_SPARE2_MISC2_BMSK                                              0xffffffff
#define HWIO_SPARE2_MISC2_SHFT                                                     0x0

#define HWIO_TLMM_HW_REVISION_NUMBER_ADDR                                   (TLMM_CSR_REG_BASE      + 0x00110010)
#define HWIO_TLMM_HW_REVISION_NUMBER_RMSK                                   0xffffffff
#define HWIO_TLMM_HW_REVISION_NUMBER_IN          \
        in_dword(HWIO_TLMM_HW_REVISION_NUMBER_ADDR)
#define HWIO_TLMM_HW_REVISION_NUMBER_INM(m)      \
        in_dword_masked(HWIO_TLMM_HW_REVISION_NUMBER_ADDR, m)
#define HWIO_TLMM_HW_REVISION_NUMBER_VERSION_ID_BMSK                        0xf0000000
#define HWIO_TLMM_HW_REVISION_NUMBER_VERSION_ID_SHFT                              0x1c
#define HWIO_TLMM_HW_REVISION_NUMBER_PARTNUM_BMSK                            0xffff000
#define HWIO_TLMM_HW_REVISION_NUMBER_PARTNUM_SHFT                                  0xc
#define HWIO_TLMM_HW_REVISION_NUMBER_QUALCOMM_MFG_ID_BMSK                        0xffe
#define HWIO_TLMM_HW_REVISION_NUMBER_QUALCOMM_MFG_ID_SHFT                          0x1
#define HWIO_TLMM_HW_REVISION_NUMBER_START_BIT_BMSK                                0x1
#define HWIO_TLMM_HW_REVISION_NUMBER_START_BIT_SHFT                                0x0

#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_ADDR                                (TLMM_CSR_REG_BASE      + 0x00110014)
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_RMSK                                       0x1
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_IN          \
        in_dword(HWIO_TLMM_PERIPH_CHAR_TEST_MODE_ADDR)
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_INM(m)      \
        in_dword_masked(HWIO_TLMM_PERIPH_CHAR_TEST_MODE_ADDR, m)
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_OUT(v)      \
        out_dword(HWIO_TLMM_PERIPH_CHAR_TEST_MODE_ADDR,v)
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_PERIPH_CHAR_TEST_MODE_ADDR,m,v,HWIO_TLMM_PERIPH_CHAR_TEST_MODE_IN)
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_CHAR_TEST_MODE_BMSK                        0x1
#define HWIO_TLMM_PERIPH_CHAR_TEST_MODE_CHAR_TEST_MODE_SHFT                        0x0

#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_ADDR                                   (TLMM_CSR_REG_BASE      + 0x00111000)
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_RMSK                                   0xffffffff
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_IN          \
        in_dword(HWIO_TLMM_EBI2_EMMC_GPIO_CFG_ADDR)
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_INM(m)      \
        in_dword_masked(HWIO_TLMM_EBI2_EMMC_GPIO_CFG_ADDR, m)
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_OUT(v)      \
        out_dword(HWIO_TLMM_EBI2_EMMC_GPIO_CFG_ADDR,v)
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_EBI2_EMMC_GPIO_CFG_ADDR,m,v,HWIO_TLMM_EBI2_EMMC_GPIO_CFG_IN)
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_BUSY_HDRV_BMSK                    0xe0000000
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_BUSY_HDRV_SHFT                          0x1d
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_BUSY_PULL_BMSK                    0x18000000
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_BUSY_PULL_SHFT                          0x1b
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_WE_HDRV_BMSK                       0x7000000
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_WE_HDRV_SHFT                            0x18
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_WE_PULL_BMSK                        0xc00000
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_WE_PULL_SHFT                            0x16
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_CLE_HDRV_BMSK                       0x380000
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_CLE_HDRV_SHFT                           0x13
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_CLE_PULL_BMSK                        0x60000
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_CLE_PULL_SHFT                           0x11
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_ALE_HDRV_BMSK                        0x1c000
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_ALE_HDRV_SHFT                            0xe
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_ALE_PULL_BMSK                         0x3000
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_ALE_PULL_SHFT                            0xc
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_OE_HDRV_BMSK                           0xe00
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_OE_HDRV_SHFT                             0x9
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_OE_PULL_BMSK                           0x180
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_OE_PULL_SHFT                             0x7
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_CS_HDRV_BMSK                            0x70
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_CS_HDRV_SHFT                             0x4
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_CS_PULL_BMSK                             0xc
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_CS_PULL_SHFT                             0x2
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_SPARE4_BMSK                                   0x2
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_SPARE4_SHFT                                   0x1
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_BOOT_SELECT_BMSK                         0x1
#define HWIO_TLMM_EBI2_EMMC_GPIO_CFG_EBI2_BOOT_SELECT_SHFT                         0x0

#define HWIO_TLMM_PBL_DEBUG_ADDR                                            (TLMM_CSR_REG_BASE      + 0x00110018)
#define HWIO_TLMM_PBL_DEBUG_RMSK                                                   0x1
#define HWIO_TLMM_PBL_DEBUG_IN          \
        in_dword(HWIO_TLMM_PBL_DEBUG_ADDR)
#define HWIO_TLMM_PBL_DEBUG_INM(m)      \
        in_dword_masked(HWIO_TLMM_PBL_DEBUG_ADDR, m)
#define HWIO_TLMM_PBL_DEBUG_OUT(v)      \
        out_dword(HWIO_TLMM_PBL_DEBUG_ADDR,v)
#define HWIO_TLMM_PBL_DEBUG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_PBL_DEBUG_ADDR,m,v,HWIO_TLMM_PBL_DEBUG_IN)
#define HWIO_TLMM_PBL_DEBUG_PBL_DEBUG_BMSK                                         0x1
#define HWIO_TLMM_PBL_DEBUG_PBL_DEBUG_SHFT                                         0x0

#define HWIO_TLMM_RFFE_CTL_ADDR                                             (TLMM_CSR_REG_BASE      + 0x00108000)
#define HWIO_TLMM_RFFE_CTL_RMSK                                                0xfffff
#define HWIO_TLMM_RFFE_CTL_IN          \
        in_dword(HWIO_TLMM_RFFE_CTL_ADDR)
#define HWIO_TLMM_RFFE_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_RFFE_CTL_ADDR, m)
#define HWIO_TLMM_RFFE_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_RFFE_CTL_ADDR,v)
#define HWIO_TLMM_RFFE_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_RFFE_CTL_ADDR,m,v,HWIO_TLMM_RFFE_CTL_IN)
#define HWIO_TLMM_RFFE_CTL_RFFE5_DATA_SR_CTL_EN_BMSK                           0xc0000
#define HWIO_TLMM_RFFE_CTL_RFFE5_DATA_SR_CTL_EN_SHFT                              0x12
#define HWIO_TLMM_RFFE_CTL_RFFE5_CLK_SR_CTL_EN_BMSK                            0x30000
#define HWIO_TLMM_RFFE_CTL_RFFE5_CLK_SR_CTL_EN_SHFT                               0x10
#define HWIO_TLMM_RFFE_CTL_RFFE4_DATA_SR_CTL_EN_BMSK                            0xc000
#define HWIO_TLMM_RFFE_CTL_RFFE4_DATA_SR_CTL_EN_SHFT                               0xe
#define HWIO_TLMM_RFFE_CTL_RFFE4_CLK_SR_CTL_EN_BMSK                             0x3000
#define HWIO_TLMM_RFFE_CTL_RFFE4_CLK_SR_CTL_EN_SHFT                                0xc
#define HWIO_TLMM_RFFE_CTL_RFFE3_DATA_SR_CTL_EN_BMSK                             0xc00
#define HWIO_TLMM_RFFE_CTL_RFFE3_DATA_SR_CTL_EN_SHFT                               0xa
#define HWIO_TLMM_RFFE_CTL_RFFE3_CLK_SR_CTL_EN_BMSK                              0x300
#define HWIO_TLMM_RFFE_CTL_RFFE3_CLK_SR_CTL_EN_SHFT                                0x8
#define HWIO_TLMM_RFFE_CTL_RFFE2_DATA_SR_CTL_EN_BMSK                              0xc0
#define HWIO_TLMM_RFFE_CTL_RFFE2_DATA_SR_CTL_EN_SHFT                               0x6
#define HWIO_TLMM_RFFE_CTL_RFFE2_CLK_SR_CTL_EN_BMSK                               0x30
#define HWIO_TLMM_RFFE_CTL_RFFE2_CLK_SR_CTL_EN_SHFT                                0x4
#define HWIO_TLMM_RFFE_CTL_RFFE1_DATA_SR_CTL_EN_BMSK                               0xc
#define HWIO_TLMM_RFFE_CTL_RFFE1_DATA_SR_CTL_EN_SHFT                               0x2
#define HWIO_TLMM_RFFE_CTL_RFFE1_CLK_SR_CTL_EN_BMSK                                0x3
#define HWIO_TLMM_RFFE_CTL_RFFE1_CLK_SR_CTL_EN_SHFT                                0x0

#define HWIO_TLMM_RESOUT_HDRV_CTL_ADDR                                      (TLMM_CSR_REG_BASE      + 0x0010d000)
#define HWIO_TLMM_RESOUT_HDRV_CTL_RMSK                                             0x7
#define HWIO_TLMM_RESOUT_HDRV_CTL_IN          \
        in_dword(HWIO_TLMM_RESOUT_HDRV_CTL_ADDR)
#define HWIO_TLMM_RESOUT_HDRV_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_RESOUT_HDRV_CTL_ADDR, m)
#define HWIO_TLMM_RESOUT_HDRV_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_RESOUT_HDRV_CTL_ADDR,v)
#define HWIO_TLMM_RESOUT_HDRV_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_RESOUT_HDRV_CTL_ADDR,m,v,HWIO_TLMM_RESOUT_HDRV_CTL_IN)
#define HWIO_TLMM_RESOUT_HDRV_CTL_RESOUT_N_HDRV_BMSK                               0x7
#define HWIO_TLMM_RESOUT_HDRV_CTL_RESOUT_N_HDRV_SHFT                               0x0

#define HWIO_TLMM_JTAG_HDRV_CTL_ADDR                                        (TLMM_CSR_REG_BASE      + 0x0010c000)
#define HWIO_TLMM_JTAG_HDRV_CTL_RMSK                                             0x3ff
#define HWIO_TLMM_JTAG_HDRV_CTL_IN          \
        in_dword(HWIO_TLMM_JTAG_HDRV_CTL_ADDR)
#define HWIO_TLMM_JTAG_HDRV_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_JTAG_HDRV_CTL_ADDR, m)
#define HWIO_TLMM_JTAG_HDRV_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_JTAG_HDRV_CTL_ADDR,v)
#define HWIO_TLMM_JTAG_HDRV_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_JTAG_HDRV_CTL_ADDR,m,v,HWIO_TLMM_JTAG_HDRV_CTL_IN)
#define HWIO_TLMM_JTAG_HDRV_CTL_TDI_PULL_BMSK                                    0x300
#define HWIO_TLMM_JTAG_HDRV_CTL_TDI_PULL_SHFT                                      0x8
#define HWIO_TLMM_JTAG_HDRV_CTL_TMS_PULL_BMSK                                     0xc0
#define HWIO_TLMM_JTAG_HDRV_CTL_TMS_PULL_SHFT                                      0x6
#define HWIO_TLMM_JTAG_HDRV_CTL_TMS_HDRV_BMSK                                     0x38
#define HWIO_TLMM_JTAG_HDRV_CTL_TMS_HDRV_SHFT                                      0x3
#define HWIO_TLMM_JTAG_HDRV_CTL_TDO_HDRV_BMSK                                      0x7
#define HWIO_TLMM_JTAG_HDRV_CTL_TDO_HDRV_SHFT                                      0x0

#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0010b000)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_RMSK                                      0xfffff
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_IN          \
        in_dword(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR, m)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR,v)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_PMIC_HDRV_PULL_CTL_ADDR,m,v,HWIO_TLMM_PMIC_HDRV_PULL_CTL_IN)
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_SR_CTL_EN_BMSK             0xc0000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_SR_CTL_EN_SHFT                0x12
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_SR_CTL_EN_BMSK              0x30000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_SR_CTL_EN_SHFT                 0x10
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_PULL_BMSK                   0xc000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_PULL_SHFT                      0xe
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_PULL_BMSK                    0x3000
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_PULL_SHFT                       0xc
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PSHOLD_HDRV_BMSK                            0xe00
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PSHOLD_HDRV_SHFT                              0x9
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_CXO_EN_HDRV_BMSK                            0x1c0
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_CXO_EN_HDRV_SHFT                              0x6
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_HDRV_BMSK                     0x38
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_DATA_HDRV_SHFT                      0x3
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_HDRV_BMSK                       0x7
#define HWIO_TLMM_PMIC_HDRV_PULL_CTL_PMIC_SPMI_CLK_HDRV_SHFT                       0x0

#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_ADDR                                   (TLMM_CSR_REG_BASE      + 0x0010a000)
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_RMSK                                        0x7ff
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_IN          \
        in_dword(HWIO_TLMM_EBI2_HDRV_PULL_CTL_ADDR)
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_EBI2_HDRV_PULL_CTL_ADDR, m)
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_EBI2_HDRV_PULL_CTL_ADDR,v)
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_EBI2_HDRV_PULL_CTL_ADDR,m,v,HWIO_TLMM_EBI2_HDRV_PULL_CTL_IN)
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_TE_SEL_BMSK                            0x400
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_TE_SEL_SHFT                              0xa
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_DATA_HDRV_BMSK                         0x380
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_DATA_HDRV_SHFT                           0x7
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_DATA_PULL_BMSK                          0x60
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_DATA_PULL_SHFT                           0x5
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_QSPI_DATA_PULL_BMSK                     0x18
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_QSPI_DATA_PULL_SHFT                      0x3
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_QSPI_DATA_HDRV_BMSK                      0x7
#define HWIO_TLMM_EBI2_HDRV_PULL_CTL_EBI2_QSPI_DATA_HDRV_SHFT                      0x0

#define HWIO_TLMM_MODE_PULL_CTL_ADDR                                        (TLMM_CSR_REG_BASE      + 0x00107000)
#define HWIO_TLMM_MODE_PULL_CTL_RMSK                                               0xf
#define HWIO_TLMM_MODE_PULL_CTL_IN          \
        in_dword(HWIO_TLMM_MODE_PULL_CTL_ADDR)
#define HWIO_TLMM_MODE_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_MODE_PULL_CTL_ADDR, m)
#define HWIO_TLMM_MODE_PULL_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_MODE_PULL_CTL_ADDR,v)
#define HWIO_TLMM_MODE_PULL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_MODE_PULL_CTL_ADDR,m,v,HWIO_TLMM_MODE_PULL_CTL_IN)
#define HWIO_TLMM_MODE_PULL_CTL_MODE_1_PULL_BMSK                                   0xc
#define HWIO_TLMM_MODE_PULL_CTL_MODE_1_PULL_SHFT                                   0x2
#define HWIO_TLMM_MODE_PULL_CTL_MODE_0_PULL_BMSK                                   0x3
#define HWIO_TLMM_MODE_PULL_CTL_MODE_0_PULL_SHFT                                   0x0

#define HWIO_TLMM_GPIO_HS_I2C_CTL_ADDR                                      (TLMM_CSR_REG_BASE      + 0x0010f000)
#define HWIO_TLMM_GPIO_HS_I2C_CTL_RMSK                                             0x3
#define HWIO_TLMM_GPIO_HS_I2C_CTL_IN          \
        in_dword(HWIO_TLMM_GPIO_HS_I2C_CTL_ADDR)
#define HWIO_TLMM_GPIO_HS_I2C_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_HS_I2C_CTL_ADDR, m)
#define HWIO_TLMM_GPIO_HS_I2C_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_HS_I2C_CTL_ADDR,v)
#define HWIO_TLMM_GPIO_HS_I2C_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_HS_I2C_CTL_ADDR,m,v,HWIO_TLMM_GPIO_HS_I2C_CTL_IN)
#define HWIO_TLMM_GPIO_HS_I2C_CTL_MODE_BMSK                                        0x3
#define HWIO_TLMM_GPIO_HS_I2C_CTL_MODE_SHFT                                        0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_ADDR(n)                           (TLMM_CSR_REG_BASE      + 0x00106000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_RMSK                                   0x13f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_MAXn                                       1
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_POLARITY_BMSK                          0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_POLARITY_SHFT                            0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_GPIO_SEL_BMSK                           0x3f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_WCSS_GPIO_SEL_SHFT                            0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(n)                        (TLMM_CSR_REG_BASE      + 0x00105000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_RMSK                                0x13f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_MAXn                                    9
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_POLARITY_BMSK                       0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_POLARITY_SHFT                         0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_GPIO_SEL_BMSK                        0x3f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_SENSORS_GPIO_SEL_SHFT                         0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(n)                        (TLMM_CSR_REG_BASE      + 0x00104000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_RMSK                                0x13f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_MAXn                                    5
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_POLARITY_BMSK                       0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_POLARITY_SHFT                         0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_GPIO_SEL_BMSK                        0x3f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_LPA_DSP_GPIO_SEL_SHFT                         0x0

#define HWIO_TLMM_DIR_CONN_INTR0_CFG_LPA_DSP_ADDR                           (TLMM_CSR_REG_BASE      + 0x00104000)
#define HWIO_TLMM_DIR_CONN_INTR0_CFG_LPA_DSP_RMSK                                0x13f
#define HWIO_TLMM_DIR_CONN_INTR0_CFG_LPA_DSP_IN          \
        in_dword(HWIO_TLMM_DIR_CONN_INTR0_CFG_LPA_DSP_ADDR)
#define HWIO_TLMM_DIR_CONN_INTR0_CFG_LPA_DSP_INM(m)      \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTR0_CFG_LPA_DSP_ADDR, m)
#define HWIO_TLMM_DIR_CONN_INTR0_CFG_LPA_DSP_OUT(v)      \
        out_dword(HWIO_TLMM_DIR_CONN_INTR0_CFG_LPA_DSP_ADDR,v)
#define HWIO_TLMM_DIR_CONN_INTR0_CFG_LPA_DSP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTR0_CFG_LPA_DSP_ADDR,m,v,HWIO_TLMM_DIR_CONN_INTR0_CFG_LPA_DSP_IN)
#define HWIO_TLMM_DIR_CONN_INTR0_CFG_LPA_DSP_POLARITY_BMSK                       0x100
#define HWIO_TLMM_DIR_CONN_INTR0_CFG_LPA_DSP_POLARITY_SHFT                         0x8
#define HWIO_TLMM_DIR_CONN_INTR0_CFG_LPA_DSP_GPIO_SEL_BMSK                        0x3f
#define HWIO_TLMM_DIR_CONN_INTR0_CFG_LPA_DSP_GPIO_SEL_SHFT                         0x0

#define HWIO_TLMM_DIR_CONN_INTR1_CFG_LPA_DSP_ADDR                           (TLMM_CSR_REG_BASE      + 0x00104004)
#define HWIO_TLMM_DIR_CONN_INTR1_CFG_LPA_DSP_RMSK                                0x13f
#define HWIO_TLMM_DIR_CONN_INTR1_CFG_LPA_DSP_IN          \
        in_dword(HWIO_TLMM_DIR_CONN_INTR1_CFG_LPA_DSP_ADDR)
#define HWIO_TLMM_DIR_CONN_INTR1_CFG_LPA_DSP_INM(m)      \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTR1_CFG_LPA_DSP_ADDR, m)
#define HWIO_TLMM_DIR_CONN_INTR1_CFG_LPA_DSP_OUT(v)      \
        out_dword(HWIO_TLMM_DIR_CONN_INTR1_CFG_LPA_DSP_ADDR,v)
#define HWIO_TLMM_DIR_CONN_INTR1_CFG_LPA_DSP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTR1_CFG_LPA_DSP_ADDR,m,v,HWIO_TLMM_DIR_CONN_INTR1_CFG_LPA_DSP_IN)
#define HWIO_TLMM_DIR_CONN_INTR1_CFG_LPA_DSP_POLARITY_BMSK                       0x100
#define HWIO_TLMM_DIR_CONN_INTR1_CFG_LPA_DSP_POLARITY_SHFT                         0x8
#define HWIO_TLMM_DIR_CONN_INTR1_CFG_LPA_DSP_GPIO_SEL_BMSK                        0x3f
#define HWIO_TLMM_DIR_CONN_INTR1_CFG_LPA_DSP_GPIO_SEL_SHFT                         0x0

#define HWIO_TLMM_DIR_CONN_INTR2_CFG_LPA_DSP_ADDR                           (TLMM_CSR_REG_BASE      + 0x00104008)
#define HWIO_TLMM_DIR_CONN_INTR2_CFG_LPA_DSP_RMSK                                0x13f
#define HWIO_TLMM_DIR_CONN_INTR2_CFG_LPA_DSP_IN          \
        in_dword(HWIO_TLMM_DIR_CONN_INTR2_CFG_LPA_DSP_ADDR)
#define HWIO_TLMM_DIR_CONN_INTR2_CFG_LPA_DSP_INM(m)      \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTR2_CFG_LPA_DSP_ADDR, m)
#define HWIO_TLMM_DIR_CONN_INTR2_CFG_LPA_DSP_OUT(v)      \
        out_dword(HWIO_TLMM_DIR_CONN_INTR2_CFG_LPA_DSP_ADDR,v)
#define HWIO_TLMM_DIR_CONN_INTR2_CFG_LPA_DSP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTR2_CFG_LPA_DSP_ADDR,m,v,HWIO_TLMM_DIR_CONN_INTR2_CFG_LPA_DSP_IN)
#define HWIO_TLMM_DIR_CONN_INTR2_CFG_LPA_DSP_POLARITY_BMSK                       0x100
#define HWIO_TLMM_DIR_CONN_INTR2_CFG_LPA_DSP_POLARITY_SHFT                         0x8
#define HWIO_TLMM_DIR_CONN_INTR2_CFG_LPA_DSP_GPIO_SEL_BMSK                        0x3f
#define HWIO_TLMM_DIR_CONN_INTR2_CFG_LPA_DSP_GPIO_SEL_SHFT                         0x0

#define HWIO_TLMM_DIR_CONN_INTR3_CFG_LPA_DSP_ADDR                           (TLMM_CSR_REG_BASE      + 0x0010400c)
#define HWIO_TLMM_DIR_CONN_INTR3_CFG_LPA_DSP_RMSK                                0x13f
#define HWIO_TLMM_DIR_CONN_INTR3_CFG_LPA_DSP_IN          \
        in_dword(HWIO_TLMM_DIR_CONN_INTR3_CFG_LPA_DSP_ADDR)
#define HWIO_TLMM_DIR_CONN_INTR3_CFG_LPA_DSP_INM(m)      \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTR3_CFG_LPA_DSP_ADDR, m)
#define HWIO_TLMM_DIR_CONN_INTR3_CFG_LPA_DSP_OUT(v)      \
        out_dword(HWIO_TLMM_DIR_CONN_INTR3_CFG_LPA_DSP_ADDR,v)
#define HWIO_TLMM_DIR_CONN_INTR3_CFG_LPA_DSP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTR3_CFG_LPA_DSP_ADDR,m,v,HWIO_TLMM_DIR_CONN_INTR3_CFG_LPA_DSP_IN)
#define HWIO_TLMM_DIR_CONN_INTR3_CFG_LPA_DSP_POLARITY_BMSK                       0x100
#define HWIO_TLMM_DIR_CONN_INTR3_CFG_LPA_DSP_POLARITY_SHFT                         0x8
#define HWIO_TLMM_DIR_CONN_INTR3_CFG_LPA_DSP_GPIO_SEL_BMSK                        0x3f
#define HWIO_TLMM_DIR_CONN_INTR3_CFG_LPA_DSP_GPIO_SEL_SHFT                         0x0

#define HWIO_TLMM_DIR_CONN_INTR4_CFG_LPA_DSP_ADDR                           (TLMM_CSR_REG_BASE      + 0x00104010)
#define HWIO_TLMM_DIR_CONN_INTR4_CFG_LPA_DSP_RMSK                                0x13f
#define HWIO_TLMM_DIR_CONN_INTR4_CFG_LPA_DSP_IN          \
        in_dword(HWIO_TLMM_DIR_CONN_INTR4_CFG_LPA_DSP_ADDR)
#define HWIO_TLMM_DIR_CONN_INTR4_CFG_LPA_DSP_INM(m)      \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTR4_CFG_LPA_DSP_ADDR, m)
#define HWIO_TLMM_DIR_CONN_INTR4_CFG_LPA_DSP_OUT(v)      \
        out_dword(HWIO_TLMM_DIR_CONN_INTR4_CFG_LPA_DSP_ADDR,v)
#define HWIO_TLMM_DIR_CONN_INTR4_CFG_LPA_DSP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTR4_CFG_LPA_DSP_ADDR,m,v,HWIO_TLMM_DIR_CONN_INTR4_CFG_LPA_DSP_IN)
#define HWIO_TLMM_DIR_CONN_INTR4_CFG_LPA_DSP_POLARITY_BMSK                       0x100
#define HWIO_TLMM_DIR_CONN_INTR4_CFG_LPA_DSP_POLARITY_SHFT                         0x8
#define HWIO_TLMM_DIR_CONN_INTR4_CFG_LPA_DSP_GPIO_SEL_BMSK                        0x3f
#define HWIO_TLMM_DIR_CONN_INTR4_CFG_LPA_DSP_GPIO_SEL_SHFT                         0x0

#define HWIO_TLMM_DIR_CONN_INTR5_CFG_LPA_DSP_ADDR                           (TLMM_CSR_REG_BASE      + 0x00104014)
#define HWIO_TLMM_DIR_CONN_INTR5_CFG_LPA_DSP_RMSK                                0x13f
#define HWIO_TLMM_DIR_CONN_INTR5_CFG_LPA_DSP_IN          \
        in_dword(HWIO_TLMM_DIR_CONN_INTR5_CFG_LPA_DSP_ADDR)
#define HWIO_TLMM_DIR_CONN_INTR5_CFG_LPA_DSP_INM(m)      \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTR5_CFG_LPA_DSP_ADDR, m)
#define HWIO_TLMM_DIR_CONN_INTR5_CFG_LPA_DSP_OUT(v)      \
        out_dword(HWIO_TLMM_DIR_CONN_INTR5_CFG_LPA_DSP_ADDR,v)
#define HWIO_TLMM_DIR_CONN_INTR5_CFG_LPA_DSP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTR5_CFG_LPA_DSP_ADDR,m,v,HWIO_TLMM_DIR_CONN_INTR5_CFG_LPA_DSP_IN)
#define HWIO_TLMM_DIR_CONN_INTR5_CFG_LPA_DSP_POLARITY_BMSK                       0x100
#define HWIO_TLMM_DIR_CONN_INTR5_CFG_LPA_DSP_POLARITY_SHFT                         0x8
#define HWIO_TLMM_DIR_CONN_INTR5_CFG_LPA_DSP_GPIO_SEL_BMSK                        0x3f
#define HWIO_TLMM_DIR_CONN_INTR5_CFG_LPA_DSP_GPIO_SEL_SHFT                         0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(n)                            (TLMM_CSR_REG_BASE      + 0x00103000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_RMSK                                    0x13f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_MAXn                                        0
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_POLARITY_BMSK                           0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_POLARITY_SHFT                             0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_GPIO_SEL_BMSK                            0x3f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_RPM_GPIO_SEL_SHFT                             0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_ADDR(n)                           (TLMM_CSR_REG_BASE      + 0x00102000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_RMSK                                   0x13f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_MAXn                                       7
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_POLARITY_BMSK                          0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_POLARITY_SHFT                            0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_GPIO_SEL_BMSK                           0x3f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_APSS_GPIO_SEL_SHFT                            0x0

#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(n)                            (TLMM_CSR_REG_BASE      + 0x00101000 + 0x4 * (n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_RMSK                                    0x13f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_MAXn                                        1
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_INI(n)        \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(n), HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_RMSK)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(n), mask)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_OUTI(n,val)    \
        out_dword(HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(n),val)
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_ADDR(n),mask,val,HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_INI(n))
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_POLARITY_BMSK                           0x100
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_POLARITY_SHFT                             0x8
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_GPIO_SEL_BMSK                            0x3f
#define HWIO_TLMM_DIR_CONN_INTRn_CFG_MSS_GPIO_SEL_SHFT                             0x0

#define HWIO_TLMM_GPIO_OUT_0_ADDR                                           (TLMM_CSR_REG_BASE      + 0x00200000)
#define HWIO_TLMM_GPIO_OUT_0_RMSK                                           0xffffffff
#define HWIO_TLMM_GPIO_OUT_0_IN          \
        in_dword(HWIO_TLMM_GPIO_OUT_0_ADDR)
#define HWIO_TLMM_GPIO_OUT_0_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_0_ADDR, m)
#define HWIO_TLMM_GPIO_OUT_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_0_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OUT_0_ADDR,m,v,HWIO_TLMM_GPIO_OUT_0_IN)
#define HWIO_TLMM_GPIO_OUT_0_GPIO_OUT_BMSK                                  0xffffffff
#define HWIO_TLMM_GPIO_OUT_0_GPIO_OUT_SHFT                                         0x0

#define HWIO_TLMM_GPIO_OUT_1_ADDR                                           (TLMM_CSR_REG_BASE      + 0x00200004)
#define HWIO_TLMM_GPIO_OUT_1_RMSK                                           0x7fffffff
#define HWIO_TLMM_GPIO_OUT_1_IN          \
        in_dword(HWIO_TLMM_GPIO_OUT_1_ADDR)
#define HWIO_TLMM_GPIO_OUT_1_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OUT_1_ADDR, m)
#define HWIO_TLMM_GPIO_OUT_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_1_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OUT_1_ADDR,m,v,HWIO_TLMM_GPIO_OUT_1_IN)
#define HWIO_TLMM_GPIO_OUT_1_GPIO_OUT_BMSK                                  0x7fffffff
#define HWIO_TLMM_GPIO_OUT_1_GPIO_OUT_SHFT                                         0x0

#define HWIO_TLMM_GPIO_OUT_CLR_0_ADDR                                       (TLMM_CSR_REG_BASE      + 0x00200020)
#define HWIO_TLMM_GPIO_OUT_CLR_0_RMSK                                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_CLR_0_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_CLR_0_GPIO_OUT_CLR_BMSK                          0xffffffff
#define HWIO_TLMM_GPIO_OUT_CLR_0_GPIO_OUT_CLR_SHFT                                 0x0

#define HWIO_TLMM_GPIO_OUT_CLR_1_ADDR                                       (TLMM_CSR_REG_BASE      + 0x00200024)
#define HWIO_TLMM_GPIO_OUT_CLR_1_RMSK                                       0x7fffffff
#define HWIO_TLMM_GPIO_OUT_CLR_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_CLR_1_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_CLR_1_GPIO_OUT_CLR_BMSK                          0x7fffffff
#define HWIO_TLMM_GPIO_OUT_CLR_1_GPIO_OUT_CLR_SHFT                                 0x0

#define HWIO_TLMM_GPIO_OUT_SET_0_ADDR                                       (TLMM_CSR_REG_BASE      + 0x00200040)
#define HWIO_TLMM_GPIO_OUT_SET_0_RMSK                                       0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_SET_0_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_SET_0_GPIO_OUT_SET_BMSK                          0xffffffff
#define HWIO_TLMM_GPIO_OUT_SET_0_GPIO_OUT_SET_SHFT                                 0x0

#define HWIO_TLMM_GPIO_OUT_SET_1_ADDR                                       (TLMM_CSR_REG_BASE      + 0x00200044)
#define HWIO_TLMM_GPIO_OUT_SET_1_RMSK                                       0x7fffffff
#define HWIO_TLMM_GPIO_OUT_SET_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OUT_SET_1_ADDR,v)
#define HWIO_TLMM_GPIO_OUT_SET_1_GPIO_OUT_SET_BMSK                          0x7fffffff
#define HWIO_TLMM_GPIO_OUT_SET_1_GPIO_OUT_SET_SHFT                                 0x0

#define HWIO_TLMM_GPIO_IN_0_ADDR                                            (TLMM_CSR_REG_BASE      + 0x00200060)
#define HWIO_TLMM_GPIO_IN_0_RMSK                                            0xffffffff
#define HWIO_TLMM_GPIO_IN_0_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_0_ADDR)
#define HWIO_TLMM_GPIO_IN_0_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_0_ADDR, m)
#define HWIO_TLMM_GPIO_IN_0_GPIO_IN_BMSK                                    0xffffffff
#define HWIO_TLMM_GPIO_IN_0_GPIO_IN_SHFT                                           0x0

#define HWIO_TLMM_GPIO_IN_1_ADDR                                            (TLMM_CSR_REG_BASE      + 0x00200064)
#define HWIO_TLMM_GPIO_IN_1_RMSK                                            0x7fffffff
#define HWIO_TLMM_GPIO_IN_1_IN          \
        in_dword(HWIO_TLMM_GPIO_IN_1_ADDR)
#define HWIO_TLMM_GPIO_IN_1_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_IN_1_ADDR, m)
#define HWIO_TLMM_GPIO_IN_1_GPIO_IN_BMSK                                    0x7fffffff
#define HWIO_TLMM_GPIO_IN_1_GPIO_IN_SHFT                                           0x0

#define HWIO_TLMM_GPIO_OE_0_ADDR                                            (TLMM_CSR_REG_BASE      + 0x00200080)
#define HWIO_TLMM_GPIO_OE_0_RMSK                                            0xffffffff
#define HWIO_TLMM_GPIO_OE_0_IN          \
        in_dword(HWIO_TLMM_GPIO_OE_0_ADDR)
#define HWIO_TLMM_GPIO_OE_0_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_0_ADDR, m)
#define HWIO_TLMM_GPIO_OE_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_0_ADDR,v)
#define HWIO_TLMM_GPIO_OE_0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OE_0_ADDR,m,v,HWIO_TLMM_GPIO_OE_0_IN)
#define HWIO_TLMM_GPIO_OE_0_GPIO_OE_BMSK                                    0xffffffff
#define HWIO_TLMM_GPIO_OE_0_GPIO_OE_SHFT                                           0x0

#define HWIO_TLMM_GPIO_OE_1_ADDR                                            (TLMM_CSR_REG_BASE      + 0x00200084)
#define HWIO_TLMM_GPIO_OE_1_RMSK                                            0x7fffffff
#define HWIO_TLMM_GPIO_OE_1_IN          \
        in_dword(HWIO_TLMM_GPIO_OE_1_ADDR)
#define HWIO_TLMM_GPIO_OE_1_INM(m)      \
        in_dword_masked(HWIO_TLMM_GPIO_OE_1_ADDR, m)
#define HWIO_TLMM_GPIO_OE_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_1_ADDR,v)
#define HWIO_TLMM_GPIO_OE_1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_OE_1_ADDR,m,v,HWIO_TLMM_GPIO_OE_1_IN)
#define HWIO_TLMM_GPIO_OE_1_GPIO_OE_BMSK                                    0x7fffffff
#define HWIO_TLMM_GPIO_OE_1_GPIO_OE_SHFT                                           0x0

#define HWIO_TLMM_GPIO_OE_CLR_0_ADDR                                        (TLMM_CSR_REG_BASE      + 0x002000a0)
#define HWIO_TLMM_GPIO_OE_CLR_0_RMSK                                        0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_CLR_0_ADDR,v)
#define HWIO_TLMM_GPIO_OE_CLR_0_GPIO_OE_CLR_BMSK                            0xffffffff
#define HWIO_TLMM_GPIO_OE_CLR_0_GPIO_OE_CLR_SHFT                                   0x0

#define HWIO_TLMM_GPIO_OE_CLR_1_ADDR                                        (TLMM_CSR_REG_BASE      + 0x002000a4)
#define HWIO_TLMM_GPIO_OE_CLR_1_RMSK                                        0x7fffffff
#define HWIO_TLMM_GPIO_OE_CLR_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_CLR_1_ADDR,v)
#define HWIO_TLMM_GPIO_OE_CLR_1_GPIO_OE_CLR_BMSK                            0x7fffffff
#define HWIO_TLMM_GPIO_OE_CLR_1_GPIO_OE_CLR_SHFT                                   0x0

#define HWIO_TLMM_GPIO_OE_SET_0_ADDR                                        (TLMM_CSR_REG_BASE      + 0x002000c0)
#define HWIO_TLMM_GPIO_OE_SET_0_RMSK                                        0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_0_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_SET_0_ADDR,v)
#define HWIO_TLMM_GPIO_OE_SET_0_GPIO_OE_SET_BMSK                            0xffffffff
#define HWIO_TLMM_GPIO_OE_SET_0_GPIO_OE_SET_SHFT                                   0x0

#define HWIO_TLMM_GPIO_OE_SET_1_ADDR                                        (TLMM_CSR_REG_BASE      + 0x002000c4)
#define HWIO_TLMM_GPIO_OE_SET_1_RMSK                                        0x7fffffff
#define HWIO_TLMM_GPIO_OE_SET_1_OUT(v)      \
        out_dword(HWIO_TLMM_GPIO_OE_SET_1_ADDR,v)
#define HWIO_TLMM_GPIO_OE_SET_1_GPIO_OE_SET_BMSK                            0x7fffffff
#define HWIO_TLMM_GPIO_OE_SET_1_GPIO_OE_SET_SHFT                                   0x0


#endif /* __HALTLMMTILEHWIO_H__ */
