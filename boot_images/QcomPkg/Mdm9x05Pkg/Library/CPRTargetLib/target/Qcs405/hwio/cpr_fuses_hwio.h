#ifndef __CPR_FUSES_HWIO_H__
#define __CPR_FUSES_HWIO_H__
/*
===========================================================================
*/
/**
  @file cpr_fuses_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    QCS405 (Vipertooth) [vipertooth_prj_1.0_p3q3r103]
 
  This file contains HWIO register definitions for the following modules:
    SECURITY_CONTROL_CORE


  Generation parameters: 
  { u'filename': u'cpr_fuses_hwio.h',
    u'header': u'#include "msmhwiobase.h"',
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [u'SECURITY_CONTROL_CORE'],
    u'output-fvals': True}
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

  $Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Library/CPRTargetLib/target/Qcs405/hwio/cpr_fuses_hwio.h#1 $
  $DateTime: 2018/08/08 05:31:55 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: SECURITY_CONTROL_CORE
 *--------------------------------------------------------------------------*/

#define SECURITY_CONTROL_CORE_REG_BASE                                                           (SECURITY_CONTROL_BASE      + 0x00000000)
#define SECURITY_CONTROL_CORE_REG_BASE_SIZE                                                      0x7000
#define SECURITY_CONTROL_CORE_REG_BASE_USED                                                      0x6400

#define HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_ADDR(n)                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000000 + 0x4 * (n))
#define HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_RMSK                                                     0xffffffff
#define HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_MAXn                                                             71
#define HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_ADDR(n), HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_RMSK)
#define HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_ADDR(n), mask)
#define HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_CRI_CM_PRIVATE_BMSK                                      0xffffffff
#define HWIO_QFPROM_RAW_CRI_CM_PRIVATEn_CRI_CM_PRIVATE_SHFT                                             0x0

#define HWIO_QFPROM_RAW_MRC_2_0_ROW0_LSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000120)
#define HWIO_QFPROM_RAW_MRC_2_0_ROW0_LSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_RAW_MRC_2_0_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MRC_2_0_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_MRC_2_0_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_MRC_2_0_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MRC_2_0_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_MRC_2_0_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MRC_2_0_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_MRC_2_0_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MRC_2_0_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_MRC_2_0_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_MRC_2_0_ROW0_LSB_RSVD0_BMSK                                              0xfffffff0
#define HWIO_QFPROM_RAW_MRC_2_0_ROW0_LSB_RSVD0_SHFT                                                     0x4
#define HWIO_QFPROM_RAW_MRC_2_0_ROW0_LSB_ROOT_CERT_ACTIVATION_LIST_BMSK                                 0xf
#define HWIO_QFPROM_RAW_MRC_2_0_ROW0_LSB_ROOT_CERT_ACTIVATION_LIST_SHFT                                 0x0

#define HWIO_QFPROM_RAW_MRC_2_0_ROW0_MSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000124)
#define HWIO_QFPROM_RAW_MRC_2_0_ROW0_MSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_RAW_MRC_2_0_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MRC_2_0_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_MRC_2_0_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_MRC_2_0_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MRC_2_0_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_MRC_2_0_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MRC_2_0_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_MRC_2_0_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MRC_2_0_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_MRC_2_0_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_MRC_2_0_ROW0_MSB_RSVD1_BMSK                                              0xfffffffe
#define HWIO_QFPROM_RAW_MRC_2_0_ROW0_MSB_RSVD1_SHFT                                                     0x1
#define HWIO_QFPROM_RAW_MRC_2_0_ROW0_MSB_CURRENT_UIE_KEY_SEL_BMSK                                       0x1
#define HWIO_QFPROM_RAW_MRC_2_0_ROW0_MSB_CURRENT_UIE_KEY_SEL_SHFT                                       0x0

#define HWIO_QFPROM_RAW_MRC_2_0_ROW1_LSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000128)
#define HWIO_QFPROM_RAW_MRC_2_0_ROW1_LSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_RAW_MRC_2_0_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MRC_2_0_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_MRC_2_0_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_MRC_2_0_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MRC_2_0_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_MRC_2_0_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MRC_2_0_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_MRC_2_0_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MRC_2_0_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_MRC_2_0_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_MRC_2_0_ROW1_LSB_RSVD0_BMSK                                              0xfffffff0
#define HWIO_QFPROM_RAW_MRC_2_0_ROW1_LSB_RSVD0_SHFT                                                     0x4
#define HWIO_QFPROM_RAW_MRC_2_0_ROW1_LSB_ROOT_CERT_REVOCATION_LIST_BMSK                                 0xf
#define HWIO_QFPROM_RAW_MRC_2_0_ROW1_LSB_ROOT_CERT_REVOCATION_LIST_SHFT                                 0x0

#define HWIO_QFPROM_RAW_MRC_2_0_ROW1_MSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000012c)
#define HWIO_QFPROM_RAW_MRC_2_0_ROW1_MSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_RAW_MRC_2_0_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_MRC_2_0_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_MRC_2_0_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_MRC_2_0_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_MRC_2_0_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_MRC_2_0_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_MRC_2_0_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_MRC_2_0_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MRC_2_0_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_MRC_2_0_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_MRC_2_0_ROW1_MSB_RSVD0_BMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_MRC_2_0_ROW1_MSB_RSVD0_SHFT                                                     0x0

#define HWIO_QFPROM_RAW_PTE_ROW0_LSB_ADDR                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000130)
#define HWIO_QFPROM_RAW_PTE_ROW0_LSB_RMSK                                                        0xffffffff
#define HWIO_QFPROM_RAW_PTE_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_PTE_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_PTE_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_PTE_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PTE_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_PTE_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PTE_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_PTE_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_PTE_ROW0_LSB_RSVD0_BMSK                                                  0xc0000000
#define HWIO_QFPROM_RAW_PTE_ROW0_LSB_RSVD0_SHFT                                                        0x1e
#define HWIO_QFPROM_RAW_PTE_ROW0_LSB_LDO_SUB_FIX_DET_BMSK                                        0x20000000
#define HWIO_QFPROM_RAW_PTE_ROW0_LSB_LDO_SUB_FIX_DET_SHFT                                              0x1d
#define HWIO_QFPROM_RAW_PTE_ROW0_LSB_MACCHIATO_EN_BMSK                                           0x10000000
#define HWIO_QFPROM_RAW_PTE_ROW0_LSB_MACCHIATO_EN_SHFT                                                 0x1c
#define HWIO_QFPROM_RAW_PTE_ROW0_LSB_FEATURE_ID_BMSK                                              0xff00000
#define HWIO_QFPROM_RAW_PTE_ROW0_LSB_FEATURE_ID_SHFT                                                   0x14
#define HWIO_QFPROM_RAW_PTE_ROW0_LSB_JTAG_ID_BMSK                                                   0xfffff
#define HWIO_QFPROM_RAW_PTE_ROW0_LSB_JTAG_ID_SHFT                                                       0x0

#define HWIO_QFPROM_RAW_PTE_ROW0_MSB_ADDR                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000134)
#define HWIO_QFPROM_RAW_PTE_ROW0_MSB_RMSK                                                        0xffffffff
#define HWIO_QFPROM_RAW_PTE_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_PTE_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_PTE_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_PTE_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PTE_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_PTE_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PTE_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_PTE_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_PTE_ROW0_MSB_PTE_DATA1_BMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_PTE_ROW0_MSB_PTE_DATA1_SHFT                                                     0x0

#define HWIO_QFPROM_RAW_PTE_ROW1_LSB_ADDR                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000138)
#define HWIO_QFPROM_RAW_PTE_ROW1_LSB_RMSK                                                        0xffffffff
#define HWIO_QFPROM_RAW_PTE_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_PTE_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_PTE_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_PTE_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PTE_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_PTE_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PTE_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_PTE_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_PTE_ROW1_LSB_SERIAL_NUM_BMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_PTE_ROW1_LSB_SERIAL_NUM_SHFT                                                    0x0

#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_ADDR                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000013c)
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_RMSK                                                        0xffffffff
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_PTE_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PTE_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PTE_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_PTE_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_WAFER_ID_BMSK                                               0xf8000000
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_WAFER_ID_SHFT                                                     0x1b
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_DIE_X_BMSK                                                   0x7f80000
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_DIE_X_SHFT                                                        0x13
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_DIE_Y_BMSK                                                     0x7f800
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_DIE_Y_SHFT                                                         0xb
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_RSVD_R39_B42_BMSK                                                0x400
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_RSVD_R39_B42_SHFT                                                  0xa
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_RSVD_R39_B41_BMSK                                                0x200
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_RSVD_R39_B41_SHFT                                                  0x9
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_RSVD_R39_B40_BMSK                                                0x100
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_RSVD_R39_B40_SHFT                                                  0x8
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_LOGIC_RETENTION_BMSK                                              0xe0
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_LOGIC_RETENTION_SHFT                                               0x5
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_SPEED_BIN_BMSK                                                    0x1c
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_SPEED_BIN_SHFT                                                     0x2
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_MX_RET_BIN_BMSK                                                    0x3
#define HWIO_QFPROM_RAW_PTE_ROW1_MSB_MX_RET_BIN_SHFT                                                    0x0

#define HWIO_QFPROM_RAW_PTE_ROW2_LSB_ADDR                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000140)
#define HWIO_QFPROM_RAW_PTE_ROW2_LSB_RMSK                                                        0xffffffff
#define HWIO_QFPROM_RAW_PTE_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW2_LSB_ADDR, HWIO_QFPROM_RAW_PTE_ROW2_LSB_RMSK)
#define HWIO_QFPROM_RAW_PTE_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_PTE_ROW2_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PTE_ROW2_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_PTE_ROW2_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PTE_ROW2_LSB_ADDR,m,v,HWIO_QFPROM_RAW_PTE_ROW2_LSB_IN)
#define HWIO_QFPROM_RAW_PTE_ROW2_LSB_PTE_DATA0_BMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_PTE_ROW2_LSB_PTE_DATA0_SHFT                                                     0x0

#define HWIO_QFPROM_RAW_PTE_ROW2_MSB_ADDR                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000144)
#define HWIO_QFPROM_RAW_PTE_ROW2_MSB_RMSK                                                        0xffffffff
#define HWIO_QFPROM_RAW_PTE_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW2_MSB_ADDR, HWIO_QFPROM_RAW_PTE_ROW2_MSB_RMSK)
#define HWIO_QFPROM_RAW_PTE_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_PTE_ROW2_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PTE_ROW2_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_PTE_ROW2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PTE_ROW2_MSB_ADDR,m,v,HWIO_QFPROM_RAW_PTE_ROW2_MSB_IN)
#define HWIO_QFPROM_RAW_PTE_ROW2_MSB_PTE_DATA1_BMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_PTE_ROW2_MSB_PTE_DATA1_SHFT                                                     0x0

#define HWIO_QFPROM_RAW_PTE_ROW3_LSB_ADDR                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000148)
#define HWIO_QFPROM_RAW_PTE_ROW3_LSB_RMSK                                                        0xffffffff
#define HWIO_QFPROM_RAW_PTE_ROW3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW3_LSB_ADDR, HWIO_QFPROM_RAW_PTE_ROW3_LSB_RMSK)
#define HWIO_QFPROM_RAW_PTE_ROW3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW3_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_PTE_ROW3_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PTE_ROW3_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_PTE_ROW3_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PTE_ROW3_LSB_ADDR,m,v,HWIO_QFPROM_RAW_PTE_ROW3_LSB_IN)
#define HWIO_QFPROM_RAW_PTE_ROW3_LSB_PTE_DATA0_BMSK                                              0xffff0000
#define HWIO_QFPROM_RAW_PTE_ROW3_LSB_PTE_DATA0_SHFT                                                    0x10
#define HWIO_QFPROM_RAW_PTE_ROW3_LSB_CHIP_ID_BMSK                                                    0xffff
#define HWIO_QFPROM_RAW_PTE_ROW3_LSB_CHIP_ID_SHFT                                                       0x0

#define HWIO_QFPROM_RAW_PTE_ROW3_MSB_ADDR                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000014c)
#define HWIO_QFPROM_RAW_PTE_ROW3_MSB_RMSK                                                        0xffffffff
#define HWIO_QFPROM_RAW_PTE_ROW3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW3_MSB_ADDR, HWIO_QFPROM_RAW_PTE_ROW3_MSB_RMSK)
#define HWIO_QFPROM_RAW_PTE_ROW3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PTE_ROW3_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_PTE_ROW3_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PTE_ROW3_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_PTE_ROW3_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PTE_ROW3_MSB_ADDR,m,v,HWIO_QFPROM_RAW_PTE_ROW3_MSB_IN)
#define HWIO_QFPROM_RAW_PTE_ROW3_MSB_PTE_DATA1_BMSK                                              0xffffffff
#define HWIO_QFPROM_RAW_PTE_ROW3_MSB_PTE_DATA1_SHFT                                                     0x0

#define HWIO_QFPROM_RAW_RD_PERM_LSB_ADDR                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000150)
#define HWIO_QFPROM_RAW_RD_PERM_LSB_RMSK                                                         0xffffffff
#define HWIO_QFPROM_RAW_RD_PERM_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_RD_PERM_LSB_ADDR, HWIO_QFPROM_RAW_RD_PERM_LSB_RMSK)
#define HWIO_QFPROM_RAW_RD_PERM_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_RD_PERM_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_RD_PERM_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_RD_PERM_LSB_ADDR,m,v,HWIO_QFPROM_RAW_RD_PERM_LSB_IN)
#define HWIO_QFPROM_RAW_RD_PERM_LSB_RSVD0_BMSK                                                   0xf0000000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_RSVD0_SHFT                                                         0x1c
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SPARE_REG27_BMSK                                          0x8000000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SPARE_REG27_SHFT                                               0x1b
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SPARE_REG27_ALLOW_READ_FVAL                                     0x0
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SPARE_REG27_DISABLE_READ_FVAL                                   0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SPARE_REG26_BMSK                                          0x4000000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SPARE_REG26_SHFT                                               0x1a
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SPARE_REG26_ALLOW_READ_FVAL                                     0x0
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SPARE_REG26_DISABLE_READ_FVAL                                   0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SPARE_REG25_BMSK                                          0x2000000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SPARE_REG25_SHFT                                               0x19
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SPARE_REG25_ALLOW_READ_FVAL                                     0x0
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SPARE_REG25_DISABLE_READ_FVAL                                   0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SPARE_REG24_BMSK                                          0x1000000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SPARE_REG24_SHFT                                               0x18
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SPARE_REG24_ALLOW_READ_FVAL                                     0x0
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SPARE_REG24_DISABLE_READ_FVAL                                   0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SPARE_REG23_BMSK                                           0x800000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SPARE_REG23_SHFT                                               0x17
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SPARE_REG23_ALLOW_READ_FVAL                                     0x0
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SPARE_REG23_DISABLE_READ_FVAL                                   0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_BOOT_ROM_PATCH_BMSK                                            0x400000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_BOOT_ROM_PATCH_SHFT                                                0x16
#define HWIO_QFPROM_RAW_RD_PERM_LSB_BOOT_ROM_PATCH_ALLOW_READ_FVAL                                      0x0
#define HWIO_QFPROM_RAW_RD_PERM_LSB_BOOT_ROM_PATCH_DISABLE_READ_FVAL                                    0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_SEC_KEY_DERIVATION_KEY_BMSK                                    0x200000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_SEC_KEY_DERIVATION_KEY_SHFT                                        0x15
#define HWIO_QFPROM_RAW_RD_PERM_LSB_SEC_KEY_DERIVATION_KEY_ALLOW_READ_FVAL                              0x0
#define HWIO_QFPROM_RAW_RD_PERM_LSB_SEC_KEY_DERIVATION_KEY_DISABLE_READ_FVAL                            0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_PRI_KEY_DERIVATION_KEY_BMSK                                    0x100000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_PRI_KEY_DERIVATION_KEY_SHFT                                        0x14
#define HWIO_QFPROM_RAW_RD_PERM_LSB_PRI_KEY_DERIVATION_KEY_ALLOW_READ_FVAL                              0x0
#define HWIO_QFPROM_RAW_RD_PERM_LSB_PRI_KEY_DERIVATION_KEY_DISABLE_READ_FVAL                            0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SEC_BOOT_BMSK                                               0x80000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SEC_BOOT_SHFT                                                  0x13
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SEC_BOOT_ALLOW_READ_FVAL                                        0x0
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_SEC_BOOT_DISABLE_READ_FVAL                                      0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_IMAGE_ENCR_KEY_BMSK                                         0x40000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_IMAGE_ENCR_KEY_SHFT                                            0x12
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_IMAGE_ENCR_KEY_ALLOW_READ_FVAL                                  0x0
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_IMAGE_ENCR_KEY_DISABLE_READ_FVAL                                0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_QC_SPARE_REG17_BMSK                                             0x20000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_QC_SPARE_REG17_SHFT                                                0x11
#define HWIO_QFPROM_RAW_RD_PERM_LSB_QC_SPARE_REG17_ALLOW_READ_FVAL                                      0x0
#define HWIO_QFPROM_RAW_RD_PERM_LSB_QC_SPARE_REG17_DISABLE_READ_FVAL                                    0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_QC_SPARE_REG16_BMSK                                             0x10000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_QC_SPARE_REG16_SHFT                                                0x10
#define HWIO_QFPROM_RAW_RD_PERM_LSB_QC_SPARE_REG16_ALLOW_READ_FVAL                                      0x0
#define HWIO_QFPROM_RAW_RD_PERM_LSB_QC_SPARE_REG16_DISABLE_READ_FVAL                                    0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_CM_FEAT_CONFIG_BMSK                                              0x8000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_CM_FEAT_CONFIG_SHFT                                                 0xf
#define HWIO_QFPROM_RAW_RD_PERM_LSB_CM_FEAT_CONFIG_ALLOW_READ_FVAL                                      0x0
#define HWIO_QFPROM_RAW_RD_PERM_LSB_CM_FEAT_CONFIG_DISABLE_READ_FVAL                                    0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_MEM_CONFIG_BMSK                                                  0x4000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_MEM_CONFIG_SHFT                                                     0xe
#define HWIO_QFPROM_RAW_RD_PERM_LSB_MEM_CONFIG_ALLOW_READ_FVAL                                          0x0
#define HWIO_QFPROM_RAW_RD_PERM_LSB_MEM_CONFIG_DISABLE_READ_FVAL                                        0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_CALIB_BMSK                                                       0x2000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_CALIB_SHFT                                                          0xd
#define HWIO_QFPROM_RAW_RD_PERM_LSB_CALIB_ALLOW_READ_FVAL                                               0x0
#define HWIO_QFPROM_RAW_RD_PERM_LSB_CALIB_DISABLE_READ_FVAL                                             0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_PK_HASH0_BMSK                                                    0x1000
#define HWIO_QFPROM_RAW_RD_PERM_LSB_PK_HASH0_SHFT                                                       0xc
#define HWIO_QFPROM_RAW_RD_PERM_LSB_PK_HASH0_ALLOW_READ_FVAL                                            0x0
#define HWIO_QFPROM_RAW_RD_PERM_LSB_PK_HASH0_DISABLE_READ_FVAL                                          0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_FEAT_CONFIG_BMSK                                                  0x800
#define HWIO_QFPROM_RAW_RD_PERM_LSB_FEAT_CONFIG_SHFT                                                    0xb
#define HWIO_QFPROM_RAW_RD_PERM_LSB_FEAT_CONFIG_ALLOW_READ_FVAL                                         0x0
#define HWIO_QFPROM_RAW_RD_PERM_LSB_FEAT_CONFIG_DISABLE_READ_FVAL                                       0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_CONFIG_BMSK                                                   0x400
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_CONFIG_SHFT                                                     0xa
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_CONFIG_ALLOW_READ_FVAL                                          0x0
#define HWIO_QFPROM_RAW_RD_PERM_LSB_OEM_CONFIG_DISABLE_READ_FVAL                                        0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_ANTI_ROLLBACK_4_BMSK                                              0x200
#define HWIO_QFPROM_RAW_RD_PERM_LSB_ANTI_ROLLBACK_4_SHFT                                                0x9
#define HWIO_QFPROM_RAW_RD_PERM_LSB_ANTI_ROLLBACK_4_ALLOW_READ_FVAL                                     0x0
#define HWIO_QFPROM_RAW_RD_PERM_LSB_ANTI_ROLLBACK_4_DISABLE_READ_FVAL                                   0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_ANTI_ROLLBACK_3_BMSK                                              0x100
#define HWIO_QFPROM_RAW_RD_PERM_LSB_ANTI_ROLLBACK_3_SHFT                                                0x8
#define HWIO_QFPROM_RAW_RD_PERM_LSB_ANTI_ROLLBACK_3_ALLOW_READ_FVAL                                     0x0
#define HWIO_QFPROM_RAW_RD_PERM_LSB_ANTI_ROLLBACK_3_DISABLE_READ_FVAL                                   0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_ANTI_ROLLBACK_2_BMSK                                               0x80
#define HWIO_QFPROM_RAW_RD_PERM_LSB_ANTI_ROLLBACK_2_SHFT                                                0x7
#define HWIO_QFPROM_RAW_RD_PERM_LSB_ANTI_ROLLBACK_2_ALLOW_READ_FVAL                                     0x0
#define HWIO_QFPROM_RAW_RD_PERM_LSB_ANTI_ROLLBACK_2_DISABLE_READ_FVAL                                   0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_ANTI_ROLLBACK_1_BMSK                                               0x40
#define HWIO_QFPROM_RAW_RD_PERM_LSB_ANTI_ROLLBACK_1_SHFT                                                0x6
#define HWIO_QFPROM_RAW_RD_PERM_LSB_ANTI_ROLLBACK_1_ALLOW_READ_FVAL                                     0x0
#define HWIO_QFPROM_RAW_RD_PERM_LSB_ANTI_ROLLBACK_1_DISABLE_READ_FVAL                                   0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_FEC_EN_BMSK                                                        0x20
#define HWIO_QFPROM_RAW_RD_PERM_LSB_FEC_EN_SHFT                                                         0x5
#define HWIO_QFPROM_RAW_RD_PERM_LSB_FEC_EN_ALLOW_READ_FVAL                                              0x0
#define HWIO_QFPROM_RAW_RD_PERM_LSB_FEC_EN_DISABLE_READ_FVAL                                            0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_WR_PERM_BMSK                                                       0x10
#define HWIO_QFPROM_RAW_RD_PERM_LSB_WR_PERM_SHFT                                                        0x4
#define HWIO_QFPROM_RAW_RD_PERM_LSB_WR_PERM_ALLOW_READ_FVAL                                             0x0
#define HWIO_QFPROM_RAW_RD_PERM_LSB_WR_PERM_DISABLE_READ_FVAL                                           0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_RD_PERM_BMSK                                                        0x8
#define HWIO_QFPROM_RAW_RD_PERM_LSB_RD_PERM_SHFT                                                        0x3
#define HWIO_QFPROM_RAW_RD_PERM_LSB_RD_PERM_ALLOW_READ_FVAL                                             0x0
#define HWIO_QFPROM_RAW_RD_PERM_LSB_RD_PERM_DISABLE_READ_FVAL                                           0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_PTE_BMSK                                                            0x4
#define HWIO_QFPROM_RAW_RD_PERM_LSB_PTE_SHFT                                                            0x2
#define HWIO_QFPROM_RAW_RD_PERM_LSB_PTE_ALLOW_READ_FVAL                                                 0x0
#define HWIO_QFPROM_RAW_RD_PERM_LSB_PTE_DISABLE_READ_FVAL                                               0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_MRC_2_0_BMSK                                                        0x2
#define HWIO_QFPROM_RAW_RD_PERM_LSB_MRC_2_0_SHFT                                                        0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_MRC_2_0_ALLOW_READ_FVAL                                             0x0
#define HWIO_QFPROM_RAW_RD_PERM_LSB_MRC_2_0_DISABLE_READ_FVAL                                           0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_CRI_CM_PRIVATE_BMSK                                                 0x1
#define HWIO_QFPROM_RAW_RD_PERM_LSB_CRI_CM_PRIVATE_SHFT                                                 0x0
#define HWIO_QFPROM_RAW_RD_PERM_LSB_CRI_CM_PRIVATE_ALLOW_READ_FVAL                                      0x0
#define HWIO_QFPROM_RAW_RD_PERM_LSB_CRI_CM_PRIVATE_DISABLE_READ_FVAL                                    0x1

#define HWIO_QFPROM_RAW_RD_PERM_MSB_ADDR                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000154)
#define HWIO_QFPROM_RAW_RD_PERM_MSB_RMSK                                                         0xffffffff
#define HWIO_QFPROM_RAW_RD_PERM_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_RD_PERM_MSB_ADDR, HWIO_QFPROM_RAW_RD_PERM_MSB_RMSK)
#define HWIO_QFPROM_RAW_RD_PERM_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_RD_PERM_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_RD_PERM_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_RD_PERM_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_RD_PERM_MSB_ADDR,m,v,HWIO_QFPROM_RAW_RD_PERM_MSB_IN)
#define HWIO_QFPROM_RAW_RD_PERM_MSB_RSVD0_BMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_RD_PERM_MSB_RSVD0_SHFT                                                          0x0

#define HWIO_QFPROM_RAW_WR_PERM_LSB_ADDR                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000158)
#define HWIO_QFPROM_RAW_WR_PERM_LSB_RMSK                                                         0xffffffff
#define HWIO_QFPROM_RAW_WR_PERM_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_WR_PERM_LSB_ADDR, HWIO_QFPROM_RAW_WR_PERM_LSB_RMSK)
#define HWIO_QFPROM_RAW_WR_PERM_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_WR_PERM_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_WR_PERM_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_WR_PERM_LSB_ADDR,m,v,HWIO_QFPROM_RAW_WR_PERM_LSB_IN)
#define HWIO_QFPROM_RAW_WR_PERM_LSB_RSVD0_BMSK                                                   0xf0000000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_RSVD0_SHFT                                                         0x1c
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SPARE_REG27_BMSK                                          0x8000000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SPARE_REG27_SHFT                                               0x1b
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SPARE_REG27_ALLOW_WRITE_FVAL                                    0x0
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SPARE_REG27_DISABLE_WRITE_FVAL                                  0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SPARE_REG26_BMSK                                          0x4000000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SPARE_REG26_SHFT                                               0x1a
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SPARE_REG26_ALLOW_WRITE_FVAL                                    0x0
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SPARE_REG26_DISABLE_WRITE_FVAL                                  0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SPARE_REG25_BMSK                                          0x2000000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SPARE_REG25_SHFT                                               0x19
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SPARE_REG25_ALLOW_WRITE_FVAL                                    0x0
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SPARE_REG25_DISABLE_WRITE_FVAL                                  0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SPARE_REG24_BMSK                                          0x1000000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SPARE_REG24_SHFT                                               0x18
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SPARE_REG24_ALLOW_WRITE_FVAL                                    0x0
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SPARE_REG24_DISABLE_WRITE_FVAL                                  0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SPARE_REG23_BMSK                                           0x800000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SPARE_REG23_SHFT                                               0x17
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SPARE_REG23_ALLOW_WRITE_FVAL                                    0x0
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SPARE_REG23_DISABLE_WRITE_FVAL                                  0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_BOOT_ROM_PATCH_BMSK                                            0x400000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_BOOT_ROM_PATCH_SHFT                                                0x16
#define HWIO_QFPROM_RAW_WR_PERM_LSB_BOOT_ROM_PATCH_ALLOW_WRITE_FVAL                                     0x0
#define HWIO_QFPROM_RAW_WR_PERM_LSB_BOOT_ROM_PATCH_DISABLE_WRITE_FVAL                                   0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_BMSK                                    0x200000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_SHFT                                        0x15
#define HWIO_QFPROM_RAW_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_ALLOW_WRITE_FVAL                             0x0
#define HWIO_QFPROM_RAW_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_DISABLE_WRITE_FVAL                           0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_BMSK                                    0x100000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_SHFT                                        0x14
#define HWIO_QFPROM_RAW_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_ALLOW_WRITE_FVAL                             0x0
#define HWIO_QFPROM_RAW_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_DISABLE_WRITE_FVAL                           0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SEC_BOOT_BMSK                                               0x80000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SEC_BOOT_SHFT                                                  0x13
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SEC_BOOT_ALLOW_WRITE_FVAL                                       0x0
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_SEC_BOOT_DISABLE_WRITE_FVAL                                     0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_IMAGE_ENCR_KEY_BMSK                                         0x40000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_IMAGE_ENCR_KEY_SHFT                                            0x12
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_IMAGE_ENCR_KEY_ALLOW_WRITE_FVAL                                 0x0
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_IMAGE_ENCR_KEY_DISABLE_WRITE_FVAL                               0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_QC_SPARE_REG17_BMSK                                             0x20000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_QC_SPARE_REG17_SHFT                                                0x11
#define HWIO_QFPROM_RAW_WR_PERM_LSB_QC_SPARE_REG17_ALLOW_WRITE_FVAL                                     0x0
#define HWIO_QFPROM_RAW_WR_PERM_LSB_QC_SPARE_REG17_DISABLE_WRITE_FVAL                                   0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_QC_SPARE_REG16_BMSK                                             0x10000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_QC_SPARE_REG16_SHFT                                                0x10
#define HWIO_QFPROM_RAW_WR_PERM_LSB_QC_SPARE_REG16_ALLOW_WRITE_FVAL                                     0x0
#define HWIO_QFPROM_RAW_WR_PERM_LSB_QC_SPARE_REG16_DISABLE_WRITE_FVAL                                   0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_CM_FEAT_CONFIG_BMSK                                              0x8000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_CM_FEAT_CONFIG_SHFT                                                 0xf
#define HWIO_QFPROM_RAW_WR_PERM_LSB_CM_FEAT_CONFIG_ALLOW_WRITE_FVAL                                     0x0
#define HWIO_QFPROM_RAW_WR_PERM_LSB_CM_FEAT_CONFIG_DISABLE_WRITE_FVAL                                   0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_MEM_CONFIG_BMSK                                                  0x4000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_MEM_CONFIG_SHFT                                                     0xe
#define HWIO_QFPROM_RAW_WR_PERM_LSB_MEM_CONFIG_ALLOW_WRITE_FVAL                                         0x0
#define HWIO_QFPROM_RAW_WR_PERM_LSB_MEM_CONFIG_DISABLE_WRITE_FVAL                                       0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_CALIB_BMSK                                                       0x2000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_CALIB_SHFT                                                          0xd
#define HWIO_QFPROM_RAW_WR_PERM_LSB_CALIB_ALLOW_WRITE_FVAL                                              0x0
#define HWIO_QFPROM_RAW_WR_PERM_LSB_CALIB_DISABLE_WRITE_FVAL                                            0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_PK_HASH0_BMSK                                                    0x1000
#define HWIO_QFPROM_RAW_WR_PERM_LSB_PK_HASH0_SHFT                                                       0xc
#define HWIO_QFPROM_RAW_WR_PERM_LSB_PK_HASH0_ALLOW_WRITE_FVAL                                           0x0
#define HWIO_QFPROM_RAW_WR_PERM_LSB_PK_HASH0_DISABLE_WRITE_FVAL                                         0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_FEAT_CONFIG_BMSK                                                  0x800
#define HWIO_QFPROM_RAW_WR_PERM_LSB_FEAT_CONFIG_SHFT                                                    0xb
#define HWIO_QFPROM_RAW_WR_PERM_LSB_FEAT_CONFIG_ALLOW_WRITE_FVAL                                        0x0
#define HWIO_QFPROM_RAW_WR_PERM_LSB_FEAT_CONFIG_DISABLE_WRITE_FVAL                                      0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_CONFIG_BMSK                                                   0x400
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_CONFIG_SHFT                                                     0xa
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_CONFIG_ALLOW_WRITE_FVAL                                         0x0
#define HWIO_QFPROM_RAW_WR_PERM_LSB_OEM_CONFIG_DISABLE_WRITE_FVAL                                       0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_ANTI_ROLLBACK_4_BMSK                                              0x200
#define HWIO_QFPROM_RAW_WR_PERM_LSB_ANTI_ROLLBACK_4_SHFT                                                0x9
#define HWIO_QFPROM_RAW_WR_PERM_LSB_ANTI_ROLLBACK_4_ALLOW_WRITE_FVAL                                    0x0
#define HWIO_QFPROM_RAW_WR_PERM_LSB_ANTI_ROLLBACK_4_DISABLE_WRITE_FVAL                                  0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_ANTI_ROLLBACK_3_BMSK                                              0x100
#define HWIO_QFPROM_RAW_WR_PERM_LSB_ANTI_ROLLBACK_3_SHFT                                                0x8
#define HWIO_QFPROM_RAW_WR_PERM_LSB_ANTI_ROLLBACK_3_ALLOW_WRITE_FVAL                                    0x0
#define HWIO_QFPROM_RAW_WR_PERM_LSB_ANTI_ROLLBACK_3_DISABLE_WRITE_FVAL                                  0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_ANTI_ROLLBACK_2_BMSK                                               0x80
#define HWIO_QFPROM_RAW_WR_PERM_LSB_ANTI_ROLLBACK_2_SHFT                                                0x7
#define HWIO_QFPROM_RAW_WR_PERM_LSB_ANTI_ROLLBACK_2_ALLOW_WRITE_FVAL                                    0x0
#define HWIO_QFPROM_RAW_WR_PERM_LSB_ANTI_ROLLBACK_2_DISABLE_WRITE_FVAL                                  0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_ANTI_ROLLBACK_1_BMSK                                               0x40
#define HWIO_QFPROM_RAW_WR_PERM_LSB_ANTI_ROLLBACK_1_SHFT                                                0x6
#define HWIO_QFPROM_RAW_WR_PERM_LSB_ANTI_ROLLBACK_1_ALLOW_WRITE_FVAL                                    0x0
#define HWIO_QFPROM_RAW_WR_PERM_LSB_ANTI_ROLLBACK_1_DISABLE_WRITE_FVAL                                  0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_FEC_EN_BMSK                                                        0x20
#define HWIO_QFPROM_RAW_WR_PERM_LSB_FEC_EN_SHFT                                                         0x5
#define HWIO_QFPROM_RAW_WR_PERM_LSB_FEC_EN_ALLOW_WRITE_FVAL                                             0x0
#define HWIO_QFPROM_RAW_WR_PERM_LSB_FEC_EN_DISABLE_WRITE_FVAL                                           0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_WR_PERM_BMSK                                                       0x10
#define HWIO_QFPROM_RAW_WR_PERM_LSB_WR_PERM_SHFT                                                        0x4
#define HWIO_QFPROM_RAW_WR_PERM_LSB_WR_PERM_ALLOW_WRITE_FVAL                                            0x0
#define HWIO_QFPROM_RAW_WR_PERM_LSB_WR_PERM_DISABLE_WRITE_FVAL                                          0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_RD_PERM_BMSK                                                        0x8
#define HWIO_QFPROM_RAW_WR_PERM_LSB_RD_PERM_SHFT                                                        0x3
#define HWIO_QFPROM_RAW_WR_PERM_LSB_RD_PERM_ALLOW_WRITE_FVAL                                            0x0
#define HWIO_QFPROM_RAW_WR_PERM_LSB_RD_PERM_DISABLE_WRITE_FVAL                                          0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_PTE_BMSK                                                            0x4
#define HWIO_QFPROM_RAW_WR_PERM_LSB_PTE_SHFT                                                            0x2
#define HWIO_QFPROM_RAW_WR_PERM_LSB_PTE_ALLOW_WRITE_FVAL                                                0x0
#define HWIO_QFPROM_RAW_WR_PERM_LSB_PTE_DISABLE_WRITE_FVAL                                              0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_MRC_2_0_BMSK                                                        0x2
#define HWIO_QFPROM_RAW_WR_PERM_LSB_MRC_2_0_SHFT                                                        0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_MRC_2_0_ALLOW_WRITE_FVAL                                            0x0
#define HWIO_QFPROM_RAW_WR_PERM_LSB_MRC_2_0_DISABLE_WRITE_FVAL                                          0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_CRI_CM_PRIVATE_BMSK                                                 0x1
#define HWIO_QFPROM_RAW_WR_PERM_LSB_CRI_CM_PRIVATE_SHFT                                                 0x0
#define HWIO_QFPROM_RAW_WR_PERM_LSB_CRI_CM_PRIVATE_ALLOW_WRITE_FVAL                                     0x0
#define HWIO_QFPROM_RAW_WR_PERM_LSB_CRI_CM_PRIVATE_DISABLE_WRITE_FVAL                                   0x1

#define HWIO_QFPROM_RAW_WR_PERM_MSB_ADDR                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000015c)
#define HWIO_QFPROM_RAW_WR_PERM_MSB_RMSK                                                         0xffffffff
#define HWIO_QFPROM_RAW_WR_PERM_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_WR_PERM_MSB_ADDR, HWIO_QFPROM_RAW_WR_PERM_MSB_RMSK)
#define HWIO_QFPROM_RAW_WR_PERM_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_WR_PERM_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_WR_PERM_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_WR_PERM_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_WR_PERM_MSB_ADDR,m,v,HWIO_QFPROM_RAW_WR_PERM_MSB_IN)
#define HWIO_QFPROM_RAW_WR_PERM_MSB_RSVD0_BMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_WR_PERM_MSB_RSVD0_SHFT                                                          0x0

#define HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR                                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000160)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_RMSK                                                          0xffffffff
#define HWIO_QFPROM_RAW_FEC_EN_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR, HWIO_QFPROM_RAW_FEC_EN_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEC_EN_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEC_EN_LSB_IN)
#define HWIO_QFPROM_RAW_FEC_EN_LSB_RSVD0_BMSK                                                    0xf0000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_RSVD0_SHFT                                                          0x1c
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION27_FEC_EN_BMSK                                           0x8000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION27_FEC_EN_SHFT                                                0x1b
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION27_FEC_EN_DISABLE_FVAL                                         0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION27_FEC_EN_ENABLE_FVAL                                          0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION26_FEC_EN_BMSK                                           0x4000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION26_FEC_EN_SHFT                                                0x1a
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION26_FEC_EN_DISABLE_FVAL                                         0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION26_FEC_EN_ENABLE_FVAL                                          0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION25_FEC_EN_BMSK                                           0x2000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION25_FEC_EN_SHFT                                                0x19
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION25_FEC_EN_DISABLE_FVAL                                         0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION25_FEC_EN_ENABLE_FVAL                                          0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION24_FEC_EN_BMSK                                           0x1000000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION24_FEC_EN_SHFT                                                0x18
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION24_FEC_EN_DISABLE_FVAL                                         0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION24_FEC_EN_ENABLE_FVAL                                          0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION23_FEC_EN_BMSK                                            0x800000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION23_FEC_EN_SHFT                                                0x17
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION23_FEC_EN_DISABLE_FVAL                                         0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION23_FEC_EN_ENABLE_FVAL                                          0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION22_FEC_EN_BMSK                                            0x400000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION22_FEC_EN_SHFT                                                0x16
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION22_FEC_EN_DISABLE_FVAL                                         0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION22_FEC_EN_ENABLE_FVAL                                          0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION21_FEC_EN_BMSK                                            0x200000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION21_FEC_EN_SHFT                                                0x15
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION21_FEC_EN_DISABLE_FVAL                                         0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION21_FEC_EN_ENABLE_FVAL                                          0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION20_FEC_EN_BMSK                                            0x100000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION20_FEC_EN_SHFT                                                0x14
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION20_FEC_EN_DISABLE_FVAL                                         0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION20_FEC_EN_ENABLE_FVAL                                          0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION19_FEC_EN_BMSK                                             0x80000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION19_FEC_EN_SHFT                                                0x13
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION19_FEC_EN_DISABLE_FVAL                                         0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION19_FEC_EN_ENABLE_FVAL                                          0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION18_FEC_EN_BMSK                                             0x40000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION18_FEC_EN_SHFT                                                0x12
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION18_FEC_EN_DISABLE_FVAL                                         0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION18_FEC_EN_ENABLE_FVAL                                          0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION17_FEC_EN_BMSK                                             0x20000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION17_FEC_EN_SHFT                                                0x11
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION17_FEC_EN_DISABLE_FVAL                                         0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION17_FEC_EN_ENABLE_FVAL                                          0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION16_FEC_EN_BMSK                                             0x10000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION16_FEC_EN_SHFT                                                0x10
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION16_FEC_EN_DISABLE_FVAL                                         0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION16_FEC_EN_ENABLE_FVAL                                          0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION15_FEC_EN_BMSK                                              0x8000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION15_FEC_EN_SHFT                                                 0xf
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION15_FEC_EN_DISABLE_FVAL                                         0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION15_FEC_EN_ENABLE_FVAL                                          0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION14_FEC_EN_BMSK                                              0x4000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION14_FEC_EN_SHFT                                                 0xe
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION14_FEC_EN_DISABLE_FVAL                                         0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION14_FEC_EN_ENABLE_FVAL                                          0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION13_FEC_EN_BMSK                                              0x2000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION13_FEC_EN_SHFT                                                 0xd
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION13_FEC_EN_DISABLE_FVAL                                         0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION13_FEC_EN_ENABLE_FVAL                                          0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION12_FEC_EN_BMSK                                              0x1000
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION12_FEC_EN_SHFT                                                 0xc
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION12_FEC_EN_DISABLE_FVAL                                         0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION12_FEC_EN_ENABLE_FVAL                                          0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION11_FEC_EN_BMSK                                               0x800
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION11_FEC_EN_SHFT                                                 0xb
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION11_FEC_EN_DISABLE_FVAL                                         0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION11_FEC_EN_ENABLE_FVAL                                          0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION10_FEC_EN_BMSK                                               0x400
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION10_FEC_EN_SHFT                                                 0xa
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION10_FEC_EN_DISABLE_FVAL                                         0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION10_FEC_EN_ENABLE_FVAL                                          0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION9_FEC_EN_BMSK                                                0x200
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION9_FEC_EN_SHFT                                                  0x9
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION9_FEC_EN_DISABLE_FVAL                                          0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION9_FEC_EN_ENABLE_FVAL                                           0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION8_FEC_EN_BMSK                                                0x100
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION8_FEC_EN_SHFT                                                  0x8
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION8_FEC_EN_DISABLE_FVAL                                          0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION8_FEC_EN_ENABLE_FVAL                                           0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION7_FEC_EN_BMSK                                                 0x80
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION7_FEC_EN_SHFT                                                  0x7
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION7_FEC_EN_DISABLE_FVAL                                          0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION7_FEC_EN_ENABLE_FVAL                                           0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION6_FEC_EN_BMSK                                                 0x40
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION6_FEC_EN_SHFT                                                  0x6
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION6_FEC_EN_DISABLE_FVAL                                          0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION6_FEC_EN_ENABLE_FVAL                                           0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION5_FEC_EN_BMSK                                                 0x20
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION5_FEC_EN_SHFT                                                  0x5
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION5_FEC_EN_DISABLE_FVAL                                          0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION5_FEC_EN_ENABLE_FVAL                                           0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION4_FEC_EN_BMSK                                                 0x10
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION4_FEC_EN_SHFT                                                  0x4
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION4_FEC_EN_DISABLE_FVAL                                          0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION4_FEC_EN_ENABLE_FVAL                                           0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION3_FEC_EN_BMSK                                                  0x8
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION3_FEC_EN_SHFT                                                  0x3
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION3_FEC_EN_DISABLE_FVAL                                          0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION3_FEC_EN_ENABLE_FVAL                                           0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION2_FEC_EN_BMSK                                                  0x4
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION2_FEC_EN_SHFT                                                  0x2
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION2_FEC_EN_DISABLE_FVAL                                          0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION2_FEC_EN_ENABLE_FVAL                                           0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION1_FEC_EN_BMSK                                                  0x2
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION1_FEC_EN_SHFT                                                  0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION1_FEC_EN_DISABLE_FVAL                                          0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION1_FEC_EN_ENABLE_FVAL                                           0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION0_FEC_EN_BMSK                                                  0x1
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION0_FEC_EN_SHFT                                                  0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION0_FEC_EN_DISABLE_FVAL                                          0x0
#define HWIO_QFPROM_RAW_FEC_EN_LSB_REGION0_FEC_EN_ENABLE_FVAL                                           0x1

#define HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR                                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000164)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_RMSK                                                          0xffffffff
#define HWIO_QFPROM_RAW_FEC_EN_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR, HWIO_QFPROM_RAW_FEC_EN_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEC_EN_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEC_EN_MSB_IN)
#define HWIO_QFPROM_RAW_FEC_EN_MSB_RSVD0_BMSK                                                    0xffffffff
#define HWIO_QFPROM_RAW_FEC_EN_MSB_RSVD0_SHFT                                                           0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000168)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_XBL0_BMSK                                            0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_LSB_XBL0_SHFT                                                   0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000016c)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_XBL1_BMSK                                            0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_1_MSB_XBL1_SHFT                                                   0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000170)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_PIL_SUBSYSTEM_31_0_BMSK                              0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_LSB_PIL_SUBSYSTEM_31_0_SHFT                                     0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000174)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_XBL_SEC_BMSK                                         0xfe000000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_XBL_SEC_SHFT                                               0x19
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_RPM_BMSK                                              0x1fe0000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_RPM_SHFT                                                   0x11
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_TZ_BMSK                                                 0x1ffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_2_MSB_TZ_SHFT                                                     0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000178)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_XBL_CONFIG_1_0_BMSK                                  0xc0000000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_XBL_CONFIG_1_0_SHFT                                        0x1e
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_TQS_HASH_ACTIVE_BMSK                                 0x3e000000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_TQS_HASH_ACTIVE_SHFT                                       0x19
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_RPMB_KEY_PROVISIONED_BMSK                             0x1000000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_RPMB_KEY_PROVISIONED_SHFT                                  0x18
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_RPMB_KEY_PROVISIONED_RPMB_KEY_NOT_PROVISIONED_FVAL          0x0
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_RPMB_KEY_PROVISIONED_RPMB_KEY_PROVISIONED_FVAL              0x1
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_PIL_SUBSYSTEM_47_32_BMSK                               0xffff00
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_PIL_SUBSYSTEM_47_32_SHFT                                    0x8
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_SAFESWITCH_BMSK                                            0xff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_LSB_SAFESWITCH_SHFT                                             0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000017c)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_XBL_CONFIG_5_2_BMSK                                  0xf0000000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_XBL_CONFIG_5_2_SHFT                                        0x1c
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_DEVICE_CFG_BMSK                                       0xffe0000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_DEVICE_CFG_SHFT                                            0x11
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_DEBUG_POLICY_BMSK                                       0x1f000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_DEBUG_POLICY_SHFT                                           0xc
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_HYPERVISOR_BMSK                                           0xfff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_3_MSB_HYPERVISOR_SHFT                                             0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000180)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_MSS_BMSK                                             0xffff0000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_MSS_SHFT                                                   0x10
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_MISC_BMSK                                                0xffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_LSB_MISC_SHFT                                                   0x0

#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000184)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_ADDR, HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_RMSK)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_ADDR,m,v,HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_IN)
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_SIMLOCK_BMSK                                         0x80000000
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_SIMLOCK_SHFT                                               0x1f
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_RSVD0_BMSK                                           0x7fffffff
#define HWIO_QFPROM_RAW_ANTI_ROLLBACK_4_MSB_RSVD0_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000188)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SHARED_QSEE_SPNIDEN_DISABLE_BMSK                     0x80000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SHARED_QSEE_SPNIDEN_DISABLE_SHFT                           0x1f
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SHARED_QSEE_SPNIDEN_DISABLE_ENABLE_FVAL                     0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SHARED_QSEE_SPNIDEN_DISABLE_DISABLE_FVAL                    0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SHARED_QSEE_SPIDEN_DISABLE_BMSK                      0x40000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SHARED_QSEE_SPIDEN_DISABLE_SHFT                            0x1e
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SHARED_QSEE_SPIDEN_DISABLE_ENABLE_FVAL                      0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SHARED_QSEE_SPIDEN_DISABLE_DISABLE_FVAL                     0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ALL_DEBUG_DISABLE_BMSK                               0x20000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ALL_DEBUG_DISABLE_SHFT                                     0x1d
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ALL_DEBUG_DISABLE_ENABLE_FVAL                               0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ALL_DEBUG_DISABLE_DISABLE_FVAL                              0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_DEBUG_POLICY_DISABLE_BMSK                            0x10000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_DEBUG_POLICY_DISABLE_SHFT                                  0x1c
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_DEBUG_POLICY_DISABLE_ENABLE_FVAL                            0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_DEBUG_POLICY_DISABLE_DISABLE_FVAL                           0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RSVD3_BMSK                                            0xc000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RSVD3_SHFT                                                 0x1a
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_DEBUG_DISABLE_IN_ROM_BMSK                             0x2000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_DEBUG_DISABLE_IN_ROM_SHFT                                  0x19
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RSVD2_BMSK                                            0x1000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RSVD2_SHFT                                                 0x18
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_APPS_HASH_INTEGRITY_CHECK_DISABLE_BMSK                 0x800000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_APPS_HASH_INTEGRITY_CHECK_DISABLE_SHFT                     0x17
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_APPS_HASH_INTEGRITY_CHECK_DISABLE_ENABLE_FVAL               0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_APPS_HASH_INTEGRITY_CHECK_DISABLE_DISABLE_FVAL              0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RSVD1B_BMSK                                            0x400000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RSVD1B_SHFT                                                0x16
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SW_ROT_USE_SERIAL_NUM_BMSK                             0x200000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SW_ROT_USE_SERIAL_NUM_SHFT                                 0x15
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SW_ROT_USE_SERIAL_NUM_USE_OEM_ID_FVAL                       0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SW_ROT_USE_SERIAL_NUM_USE_SERIAL_NUM_FVAL                   0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_DISABLE_ROT_TRANSFER_BMSK                              0x100000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_DISABLE_ROT_TRANSFER_SHFT                                  0x14
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_DISABLE_ROT_TRANSFER_ENABLE_FVAL                            0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_DISABLE_ROT_TRANSFER_DISABLE_FVAL                           0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_IMAGE_ENCRYPTION_ENABLE_BMSK                            0x80000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_IMAGE_ENCRYPTION_ENABLE_SHFT                               0x13
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ROOT_CERT_TOTAL_NUM_BMSK                                0x60000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ROOT_CERT_TOTAL_NUM_SHFT                                   0x11
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RSVD1_BMSK                                              0x10000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RSVD1_SHFT                                                 0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_PBL_LOG_DISABLE_BMSK                                     0x8000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_PBL_LOG_DISABLE_SHFT                                        0xf
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_WDOG_EN_BMSK                                             0x4000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_WDOG_EN_SHFT                                                0xe
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_WDOG_EN_USE_GPIO_FVAL                                       0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_WDOG_EN_IGNORE_GPIO_ENABLE_WDOG_FVAL                        0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_PBL_FDL_TIMEOUT_RESET_FEATURE_ENABLE_BMSK                0x2000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_PBL_FDL_TIMEOUT_RESET_FEATURE_ENABLE_SHFT                   0xd
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_PBL_FDL_TIMEOUT_RESET_FEATURE_ENABLE_DISABLE_EDL_RESET_FVAL        0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_PBL_FDL_TIMEOUT_RESET_FEATURE_ENABLE_ENABLE_EDL_RESET_FVAL        0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SW_FUSE_PROG_DISABLE_BMSK                                0x1000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SW_FUSE_PROG_DISABLE_SHFT                                   0xc
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RSVD0_BMSK                                                0xf00
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_RSVD0_SHFT                                                  0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FAST_BOOT_BMSK                                             0xe0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FAST_BOOT_SHFT                                              0x5
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FAST_BOOT_DEFAULT_FVAL                                      0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_BMSK                                   0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_SHFT                                    0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_ENUM_19_2_MHZ_FVAL                      0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_ENUM_25_MHZ_FVAL                        0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FORCE_USB_BOOT_DISABLE_BMSK                                 0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FORCE_USB_BOOT_DISABLE_SHFT                                 0x3
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FORCE_USB_BOOT_DISABLE_USE_FORCE_USB_BOOT_GPIO_TO_FORCE_BOOT_FROM_USB_FVAL        0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FORCE_USB_BOOT_DISABLE_NOT_USE_FORCE_USB_BOOT_PIN_FVAL        0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FORCE_DLOAD_DISABLE_BMSK                                    0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FORCE_DLOAD_DISABLE_SHFT                                    0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FORCE_DLOAD_DISABLE_USE_FORCE_USB_BOOT_GPIO_TO_FORCE_BOOT_FROM_USB_FVAL        0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_FORCE_DLOAD_DISABLE_NOT_USE_FORCE_USB_BOOT_PIN_FVAL         0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ENUM_TIMEOUT_BMSK                                           0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ENUM_TIMEOUT_SHFT                                           0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ENUM_TIMEOUT_TIMEOUT_DISABLED_FVAL                          0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_ENUM_TIMEOUT_TIMEOUT_ENABLED_90S_FVAL                       0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_BMSK                                        0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_SHFT                                        0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_DOWNLOADER_ENABLED_FVAL                     0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_DOWNLOADER_DISABLED_FVAL                    0x1

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000018c)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RSVD0_BMSK                                           0xfffff000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_RSVD0_SHFT                                                  0xc
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_NAND_XFER_PARAM_BMSK                                      0x800
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_NAND_XFER_PARAM_SHFT                                        0xb
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_MISC2_DEBUG_DISABLE_BMSK                           0x400
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_MISC2_DEBUG_DISABLE_SHFT                             0xa
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_MISC2_DEBUG_DISABLE_ENABLE_FVAL                      0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_MISC2_DEBUG_DISABLE_DISABLE_FVAL                     0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_MISC1_DEBUG_DISABLE_BMSK                           0x200
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_MISC1_DEBUG_DISABLE_SHFT                             0x9
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_MISC1_DEBUG_DISABLE_ENABLE_FVAL                      0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_MISC1_DEBUG_DISABLE_DISABLE_FVAL                     0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_MISC_DEBUG_DISABLE_BMSK                            0x100
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_MISC_DEBUG_DISABLE_SHFT                              0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_MISC_DEBUG_DISABLE_ENABLE_FVAL                       0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_MISC_DEBUG_DISABLE_DISABLE_FVAL                      0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_NIDEN_DISABLE_BMSK                                    0x80
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_NIDEN_DISABLE_SHFT                                     0x7
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_NIDEN_DISABLE_ENABLE_FVAL                              0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_NIDEN_DISABLE_DISABLE_FVAL                             0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_DBGEN_DISABLE_BMSK                                    0x40
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_DBGEN_DISABLE_SHFT                                     0x6
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_DBGEN_DISABLE_ENABLE_FVAL                              0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_APPS_DBGEN_DISABLE_DISABLE_FVAL                             0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_NS_NIDEN_DISABLE_BMSK                               0x20
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_NS_NIDEN_DISABLE_SHFT                                0x5
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_NS_NIDEN_DISABLE_ENABLE_FVAL                         0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_NS_NIDEN_DISABLE_DISABLE_FVAL                        0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_NS_DBGEN_DISABLE_BMSK                               0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_NS_DBGEN_DISABLE_SHFT                                0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_NS_DBGEN_DISABLE_ENABLE_FVAL                         0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_NS_DBGEN_DISABLE_DISABLE_FVAL                        0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_CP_NIDEN_DISABLE_BMSK                                0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_CP_NIDEN_DISABLE_SHFT                                0x3
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_CP_NIDEN_DISABLE_ENABLE_FVAL                         0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_CP_NIDEN_DISABLE_DISABLE_FVAL                        0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_CP_DBGEN_DISABLE_BMSK                                0x4
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_CP_DBGEN_DISABLE_SHFT                                0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_CP_DBGEN_DISABLE_ENABLE_FVAL                         0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_CP_DBGEN_DISABLE_DISABLE_FVAL                        0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_MSS_NIDEN_DISABLE_BMSK                               0x2
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_MSS_NIDEN_DISABLE_SHFT                               0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_MSS_NIDEN_DISABLE_ENABLE_FVAL                        0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_MSS_NIDEN_DISABLE_DISABLE_FVAL                       0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_MSS_DBGEN_DISABLE_BMSK                               0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_MSS_DBGEN_DISABLE_SHFT                               0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_MSS_DBGEN_DISABLE_ENABLE_FVAL                        0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW0_MSB_SHARED_MSS_DBGEN_DISABLE_DISABLE_FVAL                       0x1

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000190)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_DISABLE_RSA_BMSK                                     0x80000000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_DISABLE_RSA_SHFT                                           0x1f
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_RSVD2_BMSK                                           0x7fff8000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_RSVD2_SHFT                                                  0xf
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG27_SECURE_BMSK                              0x4000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG27_SECURE_SHFT                                 0xe
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG27_SECURE_NOT_SECURE_FVAL                      0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG27_SECURE_SECURE_FVAL                          0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG26_SECURE_BMSK                              0x2000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG26_SECURE_SHFT                                 0xd
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG26_SECURE_NOT_SECURE_FVAL                      0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG26_SECURE_SECURE_FVAL                          0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG25_SECURE_BMSK                              0x1000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG25_SECURE_SHFT                                 0xc
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG25_SECURE_NOT_SECURE_FVAL                      0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG25_SECURE_SECURE_FVAL                          0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG24_SECURE_BMSK                               0x800
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG24_SECURE_SHFT                                 0xb
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG24_SECURE_NOT_SECURE_FVAL                      0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG24_SECURE_SECURE_FVAL                          0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG23_SECURE_BMSK                               0x400
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG23_SECURE_SHFT                                 0xa
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG23_SECURE_NOT_SECURE_FVAL                      0x0
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG23_SECURE_SECURE_FVAL                          0x1
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_RSVD1_BMSK                                                0x3f8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_RSVD1_SHFT                                                  0x3
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_RSVD0_BMSK                                                  0x7
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_LSB_RSVD0_SHFT                                                  0x0

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000194)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_OEM_PRODUCT_ID_BMSK                                  0xffff0000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_OEM_PRODUCT_ID_SHFT                                        0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_OEM_HW_ID_BMSK                                           0xffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW1_MSB_OEM_HW_ID_SHFT                                              0x0

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000198)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_PERIPH_VID_BMSK                                      0xffff0000
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_PERIPH_VID_SHFT                                            0x10
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_PERIPH_PID_BMSK                                          0xffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_LSB_PERIPH_PID_SHFT                                             0x0

#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_ADDR                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000019c)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_ADDR, HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_IN)
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_RSVD0_BMSK                                           0xffffff00
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_RSVD0_SHFT                                                  0x8
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_ANTI_ROLLBACK_FEATURE_EN_BMSK                              0xff
#define HWIO_QFPROM_RAW_OEM_CONFIG_ROW2_MSB_ANTI_ROLLBACK_FEATURE_EN_SHFT                               0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001a0)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_DISPLAY_RESOLUTION0_LIMITER_1_BMSK                  0x80000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_DISPLAY_RESOLUTION0_LIMITER_1_SHFT                        0x1f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RSVD2_BMSK                                          0x40000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RSVD2_SHFT                                                0x1e
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_DSI_0_DISABLE_BMSK                                  0x20000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_DSI_0_DISABLE_SHFT                                        0x1d
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_DISPLAY_RESOLUTION0_LIMITER_0_BMSK                  0x10000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_DISPLAY_RESOLUTION0_LIMITER_0_SHFT                        0x1c
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_NIDNT_DISABLE_BMSK                                   0x8000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_NIDNT_DISABLE_SHFT                                        0x1b
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RSVD1_BMSK                                           0x7e00000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RSVD1_SHFT                                                0x15
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SPDIF_0_1_SW_DISABLE_BMSK                             0x100000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SPDIF_0_1_SW_DISABLE_SHFT                                 0x14
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SPDIF_0_1_SW_DISABLE_ENABLE_FVAL                           0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SPDIF_0_1_SW_DISABLE_DISABLE_FVAL                          0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_I2S3_DISABLE_BMSK                                      0x80000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_I2S3_DISABLE_SHFT                                         0x13
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_I2S3_DISABLE_ENABLE_FVAL                                   0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_I2S3_DISABLE_DISABLE_FVAL                                  0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_I2S2_DISABLE_BMSK                                      0x40000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_I2S2_DISABLE_SHFT                                         0x12
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_I2S2_DISABLE_ENABLE_FVAL                                   0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_I2S2_DISABLE_DISABLE_FVAL                                  0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_I2S1_DISABLE_BMSK                                      0x20000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_I2S1_DISABLE_SHFT                                         0x11
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_I2S1_DISABLE_ENABLE_FVAL                                   0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_I2S1_DISABLE_DISABLE_FVAL                                  0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_I2S0_DISABLE_BMSK                                      0x10000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_I2S0_DISABLE_SHFT                                         0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_I2S0_DISABLE_ENABLE_FVAL                                   0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_I2S0_DISABLE_DISABLE_FVAL                                  0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_USB3_DISABLE_BMSK                                       0x8000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_USB3_DISABLE_SHFT                                          0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_USB3_DISABLE_ENABLE_FVAL                                   0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_USB3_DISABLE_DISABLE_FVAL                                  0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_PCIE_DISABLE_BMSK                                       0x4000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_PCIE_DISABLE_SHFT                                          0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_PCIE_DISABLE_ENABLE_FVAL                                   0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_PCIE_DISABLE_DISABLE_FVAL                                  0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_HDMI_DISABLE_BMSK                                       0x2000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_HDMI_DISABLE_SHFT                                          0xd
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_HDMI_DISABLE_ENABLE_FVAL                                   0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_HDMI_DISABLE_DISABLE_FVAL                                  0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_TURING_Q6_DISABLE_BMSK                                  0x1000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_TURING_Q6_DISABLE_SHFT                                     0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_TURING_Q6_DISABLE_ENABLE_FVAL                              0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_TURING_Q6_DISABLE_DISABLE_FVAL                             0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_TURING_Q6SS_HVX_DISABLE_BMSK                             0x800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_TURING_Q6SS_HVX_DISABLE_SHFT                               0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_TURING_Q6SS_HVX_DISABLE_ENABLE_FVAL                        0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_TURING_Q6SS_HVX_DISABLE_DISABLE_FVAL                       0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_EMAC_DISABLE_BMSK                                        0x400
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_EMAC_DISABLE_SHFT                                          0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_EMAC_DISABLE_ENABLE_FVAL                                   0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_EMAC_DISABLE_DISABLE_FVAL                                  0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RSVD0_BMSK                                               0x380
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RSVD0_SHFT                                                 0x7
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_DOLBY_BIT_BMSK                                            0x40
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_DOLBY_BIT_SHFT                                             0x6
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_DOLBY_BIT_DISABLE_FVAL                                     0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_DOLBY_BIT_ENABLE_FVAL                                      0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_LPASS_SVA_DISABLE_BMSK                                    0x20
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_LPASS_SVA_DISABLE_SHFT                                     0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_LPASS_SVA_DISABLE_ENABLE_FVAL                              0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_LPASS_SVA_DISABLE_DISABLE_FVAL                             0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_LPASS_Q6_TCM_BOOT_DISABLE_BMSK                            0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_LPASS_Q6_TCM_BOOT_DISABLE_SHFT                             0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_LPASS_Q6_TCM_BOOT_DISABLE_ENABLE_FVAL                      0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_LPASS_Q6_TCM_BOOT_DISABLE_DISABLE_FVAL                     0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ENABLE_DEVICE_IN_TEST_MODE_BMSK                            0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ENABLE_DEVICE_IN_TEST_MODE_SHFT                            0x3
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ARM_CE_DISABLE_USAGE_BMSK                                  0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ARM_CE_DISABLE_USAGE_SHFT                                  0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_CE_BAM_DISABLE_BMSK                                        0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_CE_BAM_DISABLE_SHFT                                        0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_CE_BAM_DISABLE_ENABLE_FVAL                                 0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_CE_BAM_DISABLE_DISABLE_FVAL                                0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_XBL_SEC_AUTH_DISABLE_BMSK                                  0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_XBL_SEC_AUTH_DISABLE_SHFT                                  0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_XBL_SEC_AUTH_DISABLE_ENABLE_FVAL                           0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_XBL_SEC_AUTH_DISABLE_DISABLE_FVAL                          0x1

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001a4)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_RSVD5_BMSK                                          0x80000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_RSVD5_SHFT                                                0x1f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_CM_FEAT_CONFIG_DISABLE_BMSK                         0x40000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_CM_FEAT_CONFIG_DISABLE_SHFT                               0x1e
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_RSVD4_BMSK                                          0x3f000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_RSVD4_SHFT                                                0x18
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY1_BMSK                          0x800000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY1_SHFT                              0x17
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY1_ICE_ENGINE_USES_SW_KEY_FVAL        0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY1_ICE_ENGINE_USES_HW_KEY_FVAL        0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY0_BMSK                          0x400000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY0_SHFT                              0x16
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY0_ICE_ENGINE_USES_SW_KEY_FVAL        0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY0_ICE_ENGINE_USES_HW_KEY_FVAL        0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_DISABLE_BMSK                                0x200000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_DISABLE_SHFT                                    0x15
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_DISABLE_ENABLE_FVAL                              0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_DISABLE_DISABLE_FVAL                             0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_RSVD3_BMSK                                            0x1e0000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_RSVD3_SHFT                                                0x11
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_CLOCKCFG_BMSK                                     0x18000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_CLOCKCFG_SHFT                                         0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_RSVD2_BMSK                                              0x7800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_RSVD2_SHFT                                                 0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_GFX_FRQ_LIM_FUSE_BMSK                                    0x600
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_GFX_FRQ_LIM_FUSE_SHFT                                      0x9
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_DDR_FRQ_LIM_FUSE_BMSK                                    0x180
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_DDR_FRQ_LIM_FUSE_SHFT                                      0x7
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_RSVD1_BMSK                                                0x40
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_RSVD1_SHFT                                                 0x6
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_CFG_BMSK                                    0x20
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_CFG_SHFT                                     0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_CFG_DEFAULT_POWER_ON_BRINGUP_DISABLE_BOOT_FSM_FVAL        0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_CFG_BOOT_FSM_BASED_BRING_UP_FVAL             0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_DELAY_BMSK                                  0x18
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_DELAY_SHFT                                   0x3
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_DELAY_GDHS_CNT_128_CLAMP_RESET_128_L2_RESET_256_FVAL        0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_DELAY_GDHS_CNT_32_CLAMP_RESET_48_L2_RESET_128_FVAL        0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_DELAY_GDHS_CNT_16_CLAMP_RESET_23_L2_RESET_103_FVAL        0x3
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_DISPLAY_RESOLUTION1_LIMITER_BMSK                           0x6
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_DISPLAY_RESOLUTION1_LIMITER_SHFT                           0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_RSVD0_BMSK                                                 0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_MSB_RSVD0_SHFT                                                 0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001a8)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_RSVD0_BMSK                                          0xffe00000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_RSVD0_SHFT                                                0x15
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MISC_DEBUG_DISABLE_BMSK                     0x100000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MISC_DEBUG_DISABLE_SHFT                         0x14
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MISC_DEBUG_DISABLE_ENABLE_FVAL                   0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MISC_DEBUG_DISABLE_DISABLE_FVAL                  0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_BMSK                             0x80000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_SHFT                                0x13
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_ENABLE_FVAL                          0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_DISABLE_FVAL                         0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_BMSK                             0x40000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_SHFT                                0x12
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_ENABLE_FVAL                          0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_DISABLE_FVAL                         0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_NIDEN_DISABLE_BMSK                        0x20000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_NIDEN_DISABLE_SHFT                           0x11
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_NIDEN_DISABLE_ENABLE_FVAL                     0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_NIDEN_DISABLE_DISABLE_FVAL                    0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_DBGEN_DISABLE_BMSK                        0x10000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_DBGEN_DISABLE_SHFT                           0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_DBGEN_DISABLE_ENABLE_FVAL                     0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_DBGEN_DISABLE_DISABLE_FVAL                    0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_NIDEN_DISABLE_BMSK                         0x8000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_NIDEN_DISABLE_SHFT                            0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_NIDEN_DISABLE_ENABLE_FVAL                     0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_NIDEN_DISABLE_DISABLE_FVAL                    0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_DBGEN_DISABLE_BMSK                         0x4000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_DBGEN_DISABLE_SHFT                            0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_DBGEN_DISABLE_ENABLE_FVAL                     0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_DBGEN_DISABLE_DISABLE_FVAL                    0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_NIDEN_DISABLE_BMSK                        0x2000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_NIDEN_DISABLE_SHFT                           0xd
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_NIDEN_DISABLE_ENABLE_FVAL                    0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_NIDEN_DISABLE_DISABLE_FVAL                   0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_DBGEN_DISABLE_BMSK                        0x1000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_DBGEN_DISABLE_SHFT                           0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_DBGEN_DISABLE_ENABLE_FVAL                    0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_DBGEN_DISABLE_DISABLE_FVAL                   0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPNIDEN_DISABLE_BMSK                      0x800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPNIDEN_DISABLE_SHFT                        0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPNIDEN_DISABLE_ENABLE_FVAL                 0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPNIDEN_DISABLE_DISABLE_FVAL                0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPIDEN_DISABLE_BMSK                       0x400
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPIDEN_DISABLE_SHFT                         0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPIDEN_DISABLE_ENABLE_FVAL                  0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPIDEN_DISABLE_DISABLE_FVAL                 0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_NIDEN_DISABLE_BMSK                            0x200
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_NIDEN_DISABLE_SHFT                              0x9
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_NIDEN_DISABLE_ENABLE_FVAL                       0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_NIDEN_DISABLE_DISABLE_FVAL                      0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_DBGEN_DISABLE_BMSK                            0x100
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_DBGEN_DISABLE_SHFT                              0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_DBGEN_DISABLE_ENABLE_FVAL                       0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_DBGEN_DISABLE_DISABLE_FVAL                      0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_NIDEN_DISABLE_BMSK                             0x80
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_NIDEN_DISABLE_SHFT                              0x7
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_NIDEN_DISABLE_ENABLE_FVAL                       0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_NIDEN_DISABLE_DISABLE_FVAL                      0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_DBGEN_DISABLE_BMSK                             0x40
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_DBGEN_DISABLE_SHFT                              0x6
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_DBGEN_DISABLE_ENABLE_FVAL                       0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_DBGEN_DISABLE_DISABLE_FVAL                      0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_NIDEN_DISABLE_BMSK                            0x20
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_NIDEN_DISABLE_SHFT                             0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_NIDEN_DISABLE_ENABLE_FVAL                      0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_NIDEN_DISABLE_DISABLE_FVAL                     0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_DBGEN_DISABLE_BMSK                            0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_DBGEN_DISABLE_SHFT                             0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_DBGEN_DISABLE_ENABLE_FVAL                      0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_DBGEN_DISABLE_DISABLE_FVAL                     0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPNIDEN_DISABLE_BMSK                          0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPNIDEN_DISABLE_SHFT                          0x3
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPNIDEN_DISABLE_ENABLE_FVAL                   0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPNIDEN_DISABLE_DISABLE_FVAL                  0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPIDEN_DISABLE_BMSK                           0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPIDEN_DISABLE_SHFT                           0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPIDEN_DISABLE_ENABLE_FVAL                    0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPIDEN_DISABLE_DISABLE_FVAL                   0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_BMSK                                       0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_SHFT                                       0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_ENABLE_FVAL                                0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_DISABLE_FVAL                               0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_BMSK                                           0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_SHFT                                           0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_ENABLE_FVAL                                    0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_DISABLE_FVAL                                   0x1

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001ac)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_BMSK                         0xfe000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_SHFT                               0x19
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_BMSK                               0x1ffff80
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_SHFT                                     0x7
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_RSVD0_BMSK                                                0x40
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_RSVD0_SHFT                                                 0x6
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_APPS_PBL_PATCH_VERSION_BMSK                               0x3f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW1_MSB_APPS_PBL_PATCH_VERSION_SHFT                                0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001b0)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_TAP_GEN_SPARE_INSTR_DISABLE_31_0_BMSK               0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_LSB_TAP_GEN_SPARE_INSTR_DISABLE_31_0_SHFT                      0x0

#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001b4)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SPARE0_BMSK                                         0x80000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_SPARE0_SHFT                                               0x1f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_A53SS_DEFEATURE_L2CACHE_1_BMSK                      0x40000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_A53SS_DEFEATURE_L2CACHE_1_SHFT                            0x1e
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_PBL_QSEE_BOOT_FLOW_DISABLE_BMSK                     0x20000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_PBL_QSEE_BOOT_FLOW_DISABLE_SHFT                           0x1d
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_4_BMSK                            0x10000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_4_SHFT                                  0x1c
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_TRIGGER_DISABLE_BMSK                       0x8000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_TRIGGER_DISABLE_SHFT                            0x1b
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_FOUNDRY_ID_BMSK                                      0x7800000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_FOUNDRY_ID_SHFT                                           0x17
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_BMSK                                0x780000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_SHFT                                    0x13
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_7_BMSK                            0x40000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_7_SHFT                               0x12
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_6_BMSK                            0x20000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_6_SHFT                               0x11
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_5_BMSK                            0x10000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_5_SHFT                               0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_RSVD1_BMSK                                              0x8000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_RSVD1_SHFT                                                 0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_AARCH64_ENABLE_BMSK                                0x4000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_AARCH64_ENABLE_SHFT                                   0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_BMSK                                0x3000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_SHFT                                   0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_ENUM_800_MHZ_FVAL                      0x0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_ENUM_400_MHZ_FVAL                      0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_ENUM_200_MHZ_FVAL                      0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_XO_FVAL                                0x3
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FROM_ROM_BMSK                                  0x800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FROM_ROM_SHFT                                    0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_RSVD0_BMSK                                               0x700
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_RSVD0_SHFT                                                 0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_BOOT_ROM_PATCH_DISABLE_BMSK                               0xe0
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_BOOT_ROM_PATCH_DISABLE_SHFT                                0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_TAP_GEN_SPARE_INSTR_DISABLE_36_32_BMSK                    0x1f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW2_MSB_TAP_GEN_SPARE_INSTR_DISABLE_36_32_SHFT                     0x0

#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_LSB_ADDR(n)                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001b8 + 0x8 * (n))
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_LSB_MAXn                                                            6
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH0_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_PK_HASH0_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH0_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_PK_HASH0_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PK_HASH0_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_PK_HASH0_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_LSB_HASH_DATA0_BMSK                                        0xffffffff
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_LSB_HASH_DATA0_SHFT                                               0x0

#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_ADDR(n)                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001bc + 0x8 * (n))
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_MAXn                                                            6
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_RSVD0_BMSK                                             0x80000000
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_RSVD0_SHFT                                                   0x1f
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_FEC_VALUE_BMSK                                         0x7f000000
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_FEC_VALUE_SHFT                                               0x18
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_HASH_DATA1_BMSK                                          0xffffff
#define HWIO_QFPROM_RAW_PK_HASH0_ROWn_MSB_HASH_DATA1_SHFT                                               0x0

#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001f0)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B31_BMSK                                         0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B31_SHFT                                               0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B30_BMSK                                         0x40000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B30_SHFT                                               0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B29_BMSK                                         0x20000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B29_SHFT                                               0x1d
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B28_BMSK                                         0x10000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B28_SHFT                                               0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B27_BMSK                                          0x8000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B27_SHFT                                               0x1b
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B26_BMSK                                          0x4000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B26_SHFT                                               0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B25_BMSK                                          0x2000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B25_SHFT                                               0x19
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B24_BMSK                                          0x1000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B24_SHFT                                               0x18
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B23_BMSK                                           0x800000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B23_SHFT                                               0x17
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B22_BMSK                                           0x400000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B22_SHFT                                               0x16
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B21_BMSK                                           0x200000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B21_SHFT                                               0x15
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B20_BMSK                                           0x100000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B20_SHFT                                               0x14
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B19_BMSK                                            0x80000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B19_SHFT                                               0x13
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B18_BMSK                                            0x40000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B18_SHFT                                               0x12
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B17_BMSK                                            0x20000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B17_SHFT                                               0x11
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B16_BMSK                                            0x10000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B16_SHFT                                               0x10
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B15_BMSK                                             0x8000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B15_SHFT                                                0xf
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B14_BMSK                                             0x4000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B14_SHFT                                                0xe
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B13_BMSK                                             0x2000
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_RSVD_R62_B13_SHFT                                                0xd
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ACC_BMSK                                                      0x1c00
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_ACC_SHFT                                                         0xa
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_IDDQ_REV_BMSK                                                  0x300
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_IDDQ_REV_SHFT                                                    0x8
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_DSI_PHY_FUSE_BMSK                                               0xf0
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_DSI_PHY_FUSE_SHFT                                                0x4
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_DSI_PHY_PLL_EFUSE_BMSK                                           0xf
#define HWIO_QFPROM_RAW_CALIB_ROW0_LSB_DSI_PHY_PLL_EFUSE_SHFT                                           0x0

#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001f4)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW0_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW0_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B63_BMSK                                         0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B63_SHFT                                               0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B62_BMSK                                         0x40000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B62_SHFT                                               0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B61_BMSK                                         0x20000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B61_SHFT                                               0x1d
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B60_BMSK                                         0x10000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B60_SHFT                                               0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B59_BMSK                                          0x8000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B59_SHFT                                               0x1b
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B58_BMSK                                          0x4000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B58_SHFT                                               0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B57_BMSK                                          0x2000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B57_SHFT                                               0x19
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B56_BMSK                                          0x1000000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B56_SHFT                                               0x18
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B55_BMSK                                           0x800000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B55_SHFT                                               0x17
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B54_BMSK                                           0x400000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B54_SHFT                                               0x16
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B53_BMSK                                           0x200000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B53_SHFT                                               0x15
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B52_BMSK                                           0x100000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B52_SHFT                                               0x14
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B51_BMSK                                            0x80000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B51_SHFT                                               0x13
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B50_BMSK                                            0x40000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B50_SHFT                                               0x12
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B49_BMSK                                            0x20000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B49_SHFT                                               0x11
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B48_BMSK                                            0x10000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B48_SHFT                                               0x10
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B47_BMSK                                             0x8000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B47_SHFT                                                0xf
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B46_BMSK                                             0x4000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B46_SHFT                                                0xe
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B45_BMSK                                             0x2000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B45_SHFT                                                0xd
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B44_BMSK                                             0x1000
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B44_SHFT                                                0xc
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B43_BMSK                                              0x800
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B43_SHFT                                                0xb
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B42_BMSK                                              0x400
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B42_SHFT                                                0xa
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B41_BMSK                                              0x200
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B41_SHFT                                                0x9
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B40_BMSK                                              0x100
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B40_SHFT                                                0x8
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B39_BMSK                                               0x80
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B39_SHFT                                                0x7
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B38_BMSK                                               0x40
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B38_SHFT                                                0x6
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B37_BMSK                                               0x20
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B37_SHFT                                                0x5
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B36_BMSK                                               0x10
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B36_SHFT                                                0x4
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B35_BMSK                                                0x8
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B35_SHFT                                                0x3
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B34_BMSK                                                0x4
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B34_SHFT                                                0x2
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B33_BMSK                                                0x2
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B33_SHFT                                                0x1
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B32_BMSK                                                0x1
#define HWIO_QFPROM_RAW_CALIB_ROW0_MSB_RSVD_R62_B32_SHFT                                                0x0

#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001f8)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW1_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW1_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_TSENS2_POINT2_1_0_BMSK                                    0xc0000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_TSENS2_POINT2_1_0_SHFT                                          0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_TSENS2_POINT1_BMSK                                        0x3f000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_TSENS2_POINT1_SHFT                                              0x18
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_TSENS1_POINT2_BMSK                                          0xfc0000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_TSENS1_POINT2_SHFT                                              0x12
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_TSENS1_POINT1_BMSK                                           0x3f000
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_TSENS1_POINT1_SHFT                                               0xc
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_TSENS0_POINT2_BMSK                                             0xfc0
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_TSENS0_POINT2_SHFT                                               0x6
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_TSENS0_POINT1_BMSK                                              0x3f
#define HWIO_QFPROM_RAW_CALIB_ROW1_LSB_TSENS0_POINT1_SHFT                                               0x0

#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x000001fc)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW1_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW1_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_RSVD_R63_B63_BMSK                                         0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_RSVD_R63_B63_SHFT                                               0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_RSVD_R63_B62_BMSK                                         0x40000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_RSVD_R63_B62_SHFT                                               0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_RSVD_R63_B61_BMSK                                         0x20000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_RSVD_R63_B61_SHFT                                               0x1d
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_RSVD_R63_B60_BMSK                                         0x10000000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_RSVD_R63_B60_SHFT                                               0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_TSENS4_POINT2_BMSK                                         0xfc00000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_TSENS4_POINT2_SHFT                                              0x16
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_TSENS4_POINT1_BMSK                                          0x3f0000
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_TSENS4_POINT1_SHFT                                              0x10
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_TSENS3_POINT2_BMSK                                            0xfc00
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_TSENS3_POINT2_SHFT                                               0xa
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_TSENS3_POINT1_BMSK                                             0x3f0
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_TSENS3_POINT1_SHFT                                               0x4
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_TSENS2_POINT2_5_2_BMSK                                           0xf
#define HWIO_QFPROM_RAW_CALIB_ROW1_MSB_TSENS2_POINT2_5_2_SHFT                                           0x0

#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000200)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW2_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW2_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW2_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW2_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW2_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_TSENS7_POINT2_1_0_BMSK                                    0xc0000000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_TSENS7_POINT2_1_0_SHFT                                          0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_TSENS7_POINT1_BMSK                                        0x3f000000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_TSENS7_POINT1_SHFT                                              0x18
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_TSENS6_POINT2_BMSK                                          0xfc0000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_TSENS6_POINT2_SHFT                                              0x12
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_TSENS6_POINT1_BMSK                                           0x3f000
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_TSENS6_POINT1_SHFT                                               0xc
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_TSENS5_POINT2_BMSK                                             0xfc0
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_TSENS5_POINT2_SHFT                                               0x6
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_TSENS5_POINT1_BMSK                                              0x3f
#define HWIO_QFPROM_RAW_CALIB_ROW2_LSB_TSENS5_POINT1_SHFT                                               0x0

#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000204)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW2_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW2_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW2_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW2_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW2_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_RSVD_R64_B63_BMSK                                         0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_RSVD_R64_B63_SHFT                                               0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_RSVD_R64_B62_BMSK                                         0x40000000
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_RSVD_R64_B62_SHFT                                               0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_RSVD_R64_B61_BMSK                                         0x20000000
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_RSVD_R64_B61_SHFT                                               0x1d
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_RSVD_R64_B60_BMSK                                         0x10000000
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_RSVD_R64_B60_SHFT                                               0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_TSENS9_POINT2_BMSK                                         0xfc00000
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_TSENS9_POINT2_SHFT                                              0x16
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_TSENS9_POINT1_BMSK                                          0x3f0000
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_TSENS9_POINT1_SHFT                                              0x10
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_TSENS8_POINT2_BMSK                                            0xfc00
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_TSENS8_POINT2_SHFT                                               0xa
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_TSENS8_POINT1_BMSK                                             0x3f0
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_TSENS8_POINT1_SHFT                                               0x4
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_TSENS7_POINT2_5_2_BMSK                                           0xf
#define HWIO_QFPROM_RAW_CALIB_ROW2_MSB_TSENS7_POINT2_5_2_SHFT                                           0x0

#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000208)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW3_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW3_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RSVD_R65_B31_BMSK                                         0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RSVD_R65_B31_SHFT                                               0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RSVD_R65_B30_BMSK                                         0x40000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RSVD_R65_B30_SHFT                                               0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RSVD_R65_B29_BMSK                                         0x20000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RSVD_R65_B29_SHFT                                               0x1d
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RSVD_R65_B28_BMSK                                         0x10000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RSVD_R65_B28_SHFT                                               0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RSVD_R65_B27_BMSK                                          0x8000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RSVD_R65_B27_SHFT                                               0x1b
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RSVD_R65_B26_BMSK                                          0x4000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RSVD_R65_B26_SHFT                                               0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RSVD_R65_B25_BMSK                                          0x2000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RSVD_R65_B25_SHFT                                               0x19
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RSVD_R65_B24_BMSK                                          0x1000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RSVD_R65_B24_SHFT                                               0x18
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RSVD_R65_B23_BMSK                                           0x800000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RSVD_R65_B23_SHFT                                               0x17
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RSVD_R65_B22_BMSK                                           0x400000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RSVD_R65_B22_SHFT                                               0x16
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RSVD_R65_B21_BMSK                                           0x200000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RSVD_R65_B21_SHFT                                               0x15
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RSVD_R65_B20_BMSK                                           0x100000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RSVD_R65_B20_SHFT                                               0x14
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RSVD_R65_B19_BMSK                                            0x80000
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_RSVD_R65_B19_SHFT                                               0x13
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_TSENS_BASE1_BMSK                                             0x7f800
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_TSENS_BASE1_SHFT                                                 0xb
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_TSENS_BASE0_BMSK                                               0x7f8
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_TSENS_BASE0_SHFT                                                 0x3
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_TSENS_CALIB_BMSK                                                 0x7
#define HWIO_QFPROM_RAW_CALIB_ROW3_LSB_TSENS_CALIB_SHFT                                                 0x0

#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000020c)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW3_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW3_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B63_BMSK                                         0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B63_SHFT                                               0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B62_BMSK                                         0x40000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B62_SHFT                                               0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B61_BMSK                                         0x20000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B61_SHFT                                               0x1d
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B60_BMSK                                         0x10000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B60_SHFT                                               0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B59_BMSK                                          0x8000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B59_SHFT                                               0x1b
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B58_BMSK                                          0x4000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B58_SHFT                                               0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B57_BMSK                                          0x2000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B57_SHFT                                               0x19
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B56_BMSK                                          0x1000000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B56_SHFT                                               0x18
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B55_BMSK                                           0x800000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B55_SHFT                                               0x17
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B54_BMSK                                           0x400000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B54_SHFT                                               0x16
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B53_BMSK                                           0x200000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B53_SHFT                                               0x15
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B52_BMSK                                           0x100000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B52_SHFT                                               0x14
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B51_BMSK                                            0x80000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B51_SHFT                                               0x13
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B50_BMSK                                            0x40000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B50_SHFT                                               0x12
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B49_BMSK                                            0x20000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B49_SHFT                                               0x11
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B48_BMSK                                            0x10000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B48_SHFT                                               0x10
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B47_BMSK                                             0x8000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B47_SHFT                                                0xf
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B46_BMSK                                             0x4000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B46_SHFT                                                0xe
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B45_BMSK                                             0x2000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B45_SHFT                                                0xd
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B44_BMSK                                             0x1000
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B44_SHFT                                                0xc
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B43_BMSK                                              0x800
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B43_SHFT                                                0xb
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B42_BMSK                                              0x400
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B42_SHFT                                                0xa
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B41_BMSK                                              0x200
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B41_SHFT                                                0x9
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B40_BMSK                                              0x100
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B40_SHFT                                                0x8
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B39_BMSK                                               0x80
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B39_SHFT                                                0x7
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B38_BMSK                                               0x40
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B38_SHFT                                                0x6
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B37_BMSK                                               0x20
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B37_SHFT                                                0x5
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B36_BMSK                                               0x10
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B36_SHFT                                                0x4
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B35_BMSK                                                0x8
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B35_SHFT                                                0x3
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B34_BMSK                                                0x4
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B34_SHFT                                                0x2
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B33_BMSK                                                0x2
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B33_SHFT                                                0x1
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B32_BMSK                                                0x1
#define HWIO_QFPROM_RAW_CALIB_ROW3_MSB_RSVD_R65_B32_SHFT                                                0x0

#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000210)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW4_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW4_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW4_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW4_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW4_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_BIASPLL_VCO_CALIB_CODE_MODE1_LOW_1_0_BMSK                 0xc0000000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_BIASPLL_VCO_CALIB_CODE_MODE1_LOW_1_0_SHFT                       0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_BIASPLL_VCO_CALIB_CODE_MODE1_UP_BMSK                      0x3f000000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_BIASPLL_VCO_CALIB_CODE_MODE1_UP_SHFT                            0x18
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_BIASPLL_VCO_CALIB_CODE_MODE1_NORM_BMSK                      0xfc0000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_BIASPLL_VCO_CALIB_CODE_MODE1_NORM_SHFT                          0x12
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_BIASPLL_VCO_CALIB_CODE_MODE0_LOW_BMSK                        0x3f000
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_BIASPLL_VCO_CALIB_CODE_MODE0_LOW_SHFT                            0xc
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_BIASPLL_VCO_CALIB_CODE_MODE0_UP_BMSK                           0xfc0
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_BIASPLL_VCO_CALIB_CODE_MODE0_UP_SHFT                             0x6
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_BIASPLL_VCO_CALIB_CODE_MODE0_NORM_BMSK                          0x3f
#define HWIO_QFPROM_RAW_CALIB_ROW4_LSB_BIASPLL_VCO_CALIB_CODE_MODE0_NORM_SHFT                           0x0

#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000214)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW4_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW4_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW4_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW4_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW4_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_BIASPLL_UPHY_LDO_RSV_BMSK                                 0xffff8000
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_BIASPLL_UPHY_LDO_RSV_SHFT                                        0xf
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_BIASPLL_LDO_LVL_BMSK                                          0x7800
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_BIASPLL_LDO_LVL_SHFT                                             0xb
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_BIASPLL_LPF_RES_BMSK                                           0x780
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_BIASPLL_LPF_RES_SHFT                                             0x7
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_BIASPLL_CP_SEL_BMSK                                             0x70
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_BIASPLL_CP_SEL_SHFT                                              0x4
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_BIASPLL_VCO_CALIB_CODE_MODE1_LOW_5_2_BMSK                        0xf
#define HWIO_QFPROM_RAW_CALIB_ROW4_MSB_BIASPLL_VCO_CALIB_CODE_MODE1_LOW_5_2_SHFT                        0x0

#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000218)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW5_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW5_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW5_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW5_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW5_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW5_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CPR1_TARGETVOL_TURBO_1_0_BMSK                             0xc0000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CPR1_TARGETVOL_TURBO_1_0_SHFT                                   0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CPR1_TARGETVOL_NOM_BMSK                                   0x3e000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CPR1_TARGETVOL_NOM_SHFT                                         0x19
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CPR0_TARGETVOL_SVS_BMSK                                    0x1f00000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CPR0_TARGETVOL_SVS_SHFT                                         0x14
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CPR0_TARGETVOL_TURBO_BMSK                                    0xf8000
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CPR0_TARGETVOL_TURBO_SHFT                                        0xf
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CPR0_TARGETVOL_NOM_BMSK                                       0x7c00
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CPR0_TARGETVOL_NOM_SHFT                                          0xa
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_RSVD_R67_B9_BMSK                                               0x200
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_RSVD_R67_B9_SHFT                                                 0x9
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_RSVD_R67_B8_BMSK                                               0x100
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_RSVD_R67_B8_SHFT                                                 0x8
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_RSVD_R67_B7_BMSK                                                0x80
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_RSVD_R67_B7_SHFT                                                 0x7
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_RSVD_R67_B6_BMSK                                                0x40
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_RSVD_R67_B6_SHFT                                                 0x6
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CPR_LOCAL_RC_BMSK                                               0x38
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CPR_LOCAL_RC_SHFT                                                0x3
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CPR_GLOBAL_RC_BMSK                                               0x7
#define HWIO_QFPROM_RAW_CALIB_ROW5_LSB_CPR_GLOBAL_RC_SHFT                                               0x0

#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000021c)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW5_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW5_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW5_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_RSVD_R67_B63_BMSK                                         0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_RSVD_R67_B63_SHFT                                               0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_RSVD_R67_B62_BMSK                                         0x40000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_RSVD_R67_B62_SHFT                                               0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_RSVD_R67_B61_BMSK                                         0x20000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_RSVD_R67_B61_SHFT                                               0x1d
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_RSVD_R67_B60_BMSK                                         0x10000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_RSVD_R67_B60_SHFT                                               0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_RSVD_R67_B59_BMSK                                          0x8000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_RSVD_R67_B59_SHFT                                               0x1b
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_RSVD_R67_B58_BMSK                                          0x4000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_RSVD_R67_B58_SHFT                                               0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_RSVD_R67_B57_BMSK                                          0x2000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_RSVD_R67_B57_SHFT                                               0x19
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_RSVD_R67_B56_BMSK                                          0x1000000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_RSVD_R67_B56_SHFT                                               0x18
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_RSVD_R67_B55_BMSK                                           0x800000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_RSVD_R67_B55_SHFT                                               0x17
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR2_TARGETVOL_SVS_BMSK                                     0x7c0000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR2_TARGETVOL_SVS_SHFT                                         0x12
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR2_TARGETVOL_TURBO_BMSK                                    0x3e000
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR2_TARGETVOL_TURBO_SHFT                                        0xd
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR2_TARGETVOL_NOM_BMSK                                       0x1f00
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR2_TARGETVOL_NOM_SHFT                                          0x8
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARGETVOL_SVS_BMSK                                         0xf8
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARGETVOL_SVS_SHFT                                          0x3
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARGETVOL_TURBO_4_2_BMSK                                    0x7
#define HWIO_QFPROM_RAW_CALIB_ROW5_MSB_CPR1_TARGETVOL_TURBO_4_2_SHFT                                    0x0

#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000220)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW6_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW6_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RSVD_R68_B31_BMSK                                         0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RSVD_R68_B31_SHFT                                               0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RSVD_R68_B30_BMSK                                         0x40000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RSVD_R68_B30_SHFT                                               0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RSVD_R68_B29_BMSK                                         0x20000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RSVD_R68_B29_SHFT                                               0x1d
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RSVD_R68_B28_BMSK                                         0x10000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RSVD_R68_B28_SHFT                                               0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RSVD_R68_B27_BMSK                                          0x8000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RSVD_R68_B27_SHFT                                               0x1b
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RSVD_R68_B26_BMSK                                          0x4000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RSVD_R68_B26_SHFT                                               0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RSVD_R68_B25_BMSK                                          0x2000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RSVD_R68_B25_SHFT                                               0x19
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RSVD_R68_B24_BMSK                                          0x1000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RSVD_R68_B24_SHFT                                               0x18
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RSVD_R68_B23_BMSK                                           0x800000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RSVD_R68_B23_SHFT                                               0x17
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RSVD_R68_B22_BMSK                                           0x400000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RSVD_R68_B22_SHFT                                               0x16
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RSVD_R68_B21_BMSK                                           0x200000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RSVD_R68_B21_SHFT                                               0x15
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RSVD_R68_B20_BMSK                                           0x100000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_RSVD_R68_B20_SHFT                                               0x14
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CPR4_TARGETVOL_NOM_BMSK                                      0xf8000
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CPR4_TARGETVOL_NOM_SHFT                                          0xf
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CPR3_TARGETVOL_SVS_BMSK                                       0x7c00
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CPR3_TARGETVOL_SVS_SHFT                                          0xa
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CPR3_TARGETVOL_TURBO_BMSK                                      0x3e0
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CPR3_TARGETVOL_TURBO_SHFT                                        0x5
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CPR3_TARGETVOL_NOM_BMSK                                         0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW6_LSB_CPR3_TARGETVOL_NOM_SHFT                                          0x0

#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000224)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW6_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW6_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW6_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW6_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW6_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B63_BMSK                                         0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B63_SHFT                                               0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B62_BMSK                                         0x40000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B62_SHFT                                               0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B61_BMSK                                         0x20000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B61_SHFT                                               0x1d
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B60_BMSK                                         0x10000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B60_SHFT                                               0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B59_BMSK                                          0x8000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B59_SHFT                                               0x1b
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B58_BMSK                                          0x4000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B58_SHFT                                               0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B57_BMSK                                          0x2000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B57_SHFT                                               0x19
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B56_BMSK                                          0x1000000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B56_SHFT                                               0x18
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B55_BMSK                                           0x800000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B55_SHFT                                               0x17
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B54_BMSK                                           0x400000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B54_SHFT                                               0x16
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B53_BMSK                                           0x200000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B53_SHFT                                               0x15
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B52_BMSK                                           0x100000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B52_SHFT                                               0x14
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B51_BMSK                                            0x80000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B51_SHFT                                               0x13
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B50_BMSK                                            0x40000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B50_SHFT                                               0x12
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B49_BMSK                                            0x20000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B49_SHFT                                               0x11
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B48_BMSK                                            0x10000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B48_SHFT                                               0x10
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B47_BMSK                                             0x8000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B47_SHFT                                                0xf
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B46_BMSK                                             0x4000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B46_SHFT                                                0xe
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B45_BMSK                                             0x2000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B45_SHFT                                                0xd
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B44_BMSK                                             0x1000
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B44_SHFT                                                0xc
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B43_BMSK                                              0x800
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B43_SHFT                                                0xb
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B42_BMSK                                              0x400
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B42_SHFT                                                0xa
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B41_BMSK                                              0x200
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B41_SHFT                                                0x9
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B40_BMSK                                              0x100
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B40_SHFT                                                0x8
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B39_BMSK                                               0x80
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B39_SHFT                                                0x7
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B38_BMSK                                               0x40
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B38_SHFT                                                0x6
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B37_BMSK                                               0x20
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B37_SHFT                                                0x5
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B36_BMSK                                               0x10
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B36_SHFT                                                0x4
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B35_BMSK                                                0x8
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B35_SHFT                                                0x3
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B34_BMSK                                                0x4
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B34_SHFT                                                0x2
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B33_BMSK                                                0x2
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B33_SHFT                                                0x1
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B32_BMSK                                                0x1
#define HWIO_QFPROM_RAW_CALIB_ROW6_MSB_RSVD_R68_B32_SHFT                                                0x0

#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000228)
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW7_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW7_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW7_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW7_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW7_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW7_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CPR5_LOCALQUOT_NOM_1_0_BMSK                               0xc0000000
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CPR5_LOCALQUOT_NOM_1_0_SHFT                                     0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CPR5_TARGETVOL_TURBO_BMSK                                 0x3f000000
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CPR5_TARGETVOL_TURBO_SHFT                                       0x18
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CPR5_TARGETVOL_NOM_L1_BMSK                                  0xfc0000
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CPR5_TARGETVOL_NOM_L1_SHFT                                      0x12
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CPR5_TARGETVOL_NOM_BMSK                                      0x3f000
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CPR5_TARGETVOL_NOM_SHFT                                          0xc
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_RSVD2_BMSK                                                     0x800
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_RSVD2_SHFT                                                       0xb
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CPR5_ROSEL_TURBO_BMSK                                          0x700
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CPR5_ROSEL_TURBO_SHFT                                            0x8
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_RSVD1_BMSK                                                      0x80
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_RSVD1_SHFT                                                       0x7
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CPR5_ROSEL_NOM_L1_BMSK                                          0x70
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CPR5_ROSEL_NOM_L1_SHFT                                           0x4
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_RSVD0_BMSK                                                       0x8
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_RSVD0_SHFT                                                       0x3
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CPR5_ROSEL_NOM_BMSK                                              0x7
#define HWIO_QFPROM_RAW_CALIB_ROW7_LSB_CPR5_ROSEL_NOM_SHFT                                              0x0

#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000022c)
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW7_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW7_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW7_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW7_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW7_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW7_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_RSVD_R69_B63_BMSK                                         0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_RSVD_R69_B63_SHFT                                               0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_RSVD_R69_B62_BMSK                                         0x40000000
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_RSVD_R69_B62_SHFT                                               0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_RSVD_R69_B61_BMSK                                         0x20000000
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_RSVD_R69_B61_SHFT                                               0x1d
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_RSVD_R69_B60_BMSK                                         0x10000000
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_RSVD_R69_B60_SHFT                                               0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_RSVD_R69_B59_BMSK                                          0x8000000
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_RSVD_R69_B59_SHFT                                               0x1b
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_RSVD_R69_B58_BMSK                                          0x4000000
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_RSVD_R69_B58_SHFT                                               0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_RSVD_R69_B57_BMSK                                          0x2000000
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_RSVD_R69_B57_SHFT                                               0x19
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_RSVD_R69_B56_BMSK                                          0x1000000
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_RSVD_R69_B56_SHFT                                               0x18
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_RSVD_R69_B55_BMSK                                           0x800000
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_RSVD_R69_B55_SHFT                                               0x17
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_RSVD_R69_B54_BMSK                                           0x400000
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_RSVD_R69_B54_SHFT                                               0x16
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_CPR5_LOCALQUOT_NOM_L1_BMSK                                  0x3ffc00
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_CPR5_LOCALQUOT_NOM_L1_SHFT                                       0xa
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_CPR5_LOCALQUOT_NOM_11_2_BMSK                                   0x3ff
#define HWIO_QFPROM_RAW_CALIB_ROW7_MSB_CPR5_LOCALQUOT_NOM_11_2_SHFT                                     0x0

#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000230)
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW8_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW8_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW8_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW8_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW8_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW8_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_CPR5_QUOT_OFFSET_TURBO_5_0_BMSK                           0xfc000000
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_CPR5_QUOT_OFFSET_TURBO_5_0_SHFT                                 0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_CPR5_QUOT_OFFSET_NOM_L1_BMSK                               0x3f80000
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_CPR5_QUOT_OFFSET_NOM_L1_SHFT                                    0x13
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_CPR5_QUOT_OFFSET_NOM_BMSK                                    0x7f000
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_CPR5_QUOT_OFFSET_NOM_SHFT                                        0xc
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_CPR5_LOCALQUOT_TURBO_BMSK                                      0xfff
#define HWIO_QFPROM_RAW_CALIB_ROW8_LSB_CPR5_LOCALQUOT_TURBO_SHFT                                        0x0

#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000234)
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW8_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW8_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW8_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW8_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW8_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW8_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_RSVD_R70_B63_BMSK                                         0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_RSVD_R70_B63_SHFT                                               0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_RSVD_R70_B62_BMSK                                         0x40000000
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_RSVD_R70_B62_SHFT                                               0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_RSVD_R70_B61_BMSK                                         0x20000000
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_RSVD_R70_B61_SHFT                                               0x1d
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_RSVD_R70_B60_BMSK                                         0x10000000
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_RSVD_R70_B60_SHFT                                               0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_RSVD_R70_B59_BMSK                                          0x8000000
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_RSVD_R70_B59_SHFT                                               0x1b
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_RSVD_R70_B58_BMSK                                          0x4000000
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_RSVD_R70_B58_SHFT                                               0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_RSVD_R70_B57_BMSK                                          0x2000000
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_RSVD_R70_B57_SHFT                                               0x19
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_RSVD_R70_B56_BMSK                                          0x1000000
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_RSVD_R70_B56_SHFT                                               0x18
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_RSVD_R70_B55_BMSK                                           0x800000
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_RSVD_R70_B55_SHFT                                               0x17
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_RSVD_R70_B54_BMSK                                           0x400000
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_RSVD_R70_B54_SHFT                                               0x16
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_RSVD_R70_B53_BMSK                                           0x200000
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_RSVD_R70_B53_SHFT                                               0x15
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_CPR_RSVD_BMSK                                               0x1ffffe
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_CPR_RSVD_SHFT                                                    0x1
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_CPR5_QUOT_OFFSET_TURBO_6_BMSK                                    0x1
#define HWIO_QFPROM_RAW_CALIB_ROW8_MSB_CPR5_QUOT_OFFSET_TURBO_6_SHFT                                    0x0

#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000238)
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW9_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW9_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW9_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW9_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW9_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B31_BMSK                                         0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B31_SHFT                                               0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B30_BMSK                                         0x40000000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B30_SHFT                                               0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B29_BMSK                                         0x20000000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B29_SHFT                                               0x1d
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B28_BMSK                                         0x10000000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B28_SHFT                                               0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B27_BMSK                                          0x8000000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B27_SHFT                                               0x1b
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B26_BMSK                                          0x4000000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B26_SHFT                                               0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B25_BMSK                                          0x2000000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B25_SHFT                                               0x19
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B24_BMSK                                          0x1000000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B24_SHFT                                               0x18
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B23_BMSK                                           0x800000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B23_SHFT                                               0x17
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B22_BMSK                                           0x400000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B22_SHFT                                               0x16
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B21_BMSK                                           0x200000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B21_SHFT                                               0x15
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B20_BMSK                                           0x100000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B20_SHFT                                               0x14
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B19_BMSK                                            0x80000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B19_SHFT                                               0x13
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B18_BMSK                                            0x40000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B18_SHFT                                               0x12
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B17_BMSK                                            0x20000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B17_SHFT                                               0x11
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B16_BMSK                                            0x10000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B16_SHFT                                               0x10
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B15_BMSK                                             0x8000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B15_SHFT                                                0xf
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B14_BMSK                                             0x4000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B14_SHFT                                                0xe
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B13_BMSK                                             0x2000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B13_SHFT                                                0xd
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B12_BMSK                                             0x1000
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B12_SHFT                                                0xc
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B11_BMSK                                              0x800
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B11_SHFT                                                0xb
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B10_BMSK                                              0x400
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B10_SHFT                                                0xa
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B9_BMSK                                               0x200
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B9_SHFT                                                 0x9
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B8_BMSK                                               0x100
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B8_SHFT                                                 0x8
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B7_BMSK                                                0x80
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B7_SHFT                                                 0x7
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B6_BMSK                                                0x40
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B6_SHFT                                                 0x6
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B5_BMSK                                                0x20
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B5_SHFT                                                 0x5
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B4_BMSK                                                0x10
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B4_SHFT                                                 0x4
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B3_BMSK                                                 0x8
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B3_SHFT                                                 0x3
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B2_BMSK                                                 0x4
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B2_SHFT                                                 0x2
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B1_BMSK                                                 0x2
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B1_SHFT                                                 0x1
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B0_BMSK                                                 0x1
#define HWIO_QFPROM_RAW_CALIB_ROW9_LSB_RSVD_R71_B0_SHFT                                                 0x0

#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_ADDR                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000023c)
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RMSK                                                      0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW9_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW9_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW9_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW9_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW9_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B63_BMSK                                         0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B63_SHFT                                               0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B62_BMSK                                         0x40000000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B62_SHFT                                               0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B61_BMSK                                         0x20000000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B61_SHFT                                               0x1d
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B60_BMSK                                         0x10000000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B60_SHFT                                               0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B59_BMSK                                          0x8000000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B59_SHFT                                               0x1b
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B58_BMSK                                          0x4000000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B58_SHFT                                               0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B57_BMSK                                          0x2000000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B57_SHFT                                               0x19
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B56_BMSK                                          0x1000000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B56_SHFT                                               0x18
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B55_BMSK                                           0x800000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B55_SHFT                                               0x17
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B54_BMSK                                           0x400000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B54_SHFT                                               0x16
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B53_BMSK                                           0x200000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B53_SHFT                                               0x15
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B52_BMSK                                           0x100000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B52_SHFT                                               0x14
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B51_BMSK                                            0x80000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B51_SHFT                                               0x13
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B50_BMSK                                            0x40000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B50_SHFT                                               0x12
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B49_BMSK                                            0x20000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B49_SHFT                                               0x11
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B48_BMSK                                            0x10000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B48_SHFT                                               0x10
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B47_BMSK                                             0x8000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B47_SHFT                                                0xf
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B46_BMSK                                             0x4000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B46_SHFT                                                0xe
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B45_BMSK                                             0x2000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B45_SHFT                                                0xd
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B44_BMSK                                             0x1000
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B44_SHFT                                                0xc
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B43_BMSK                                              0x800
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B43_SHFT                                                0xb
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B42_BMSK                                              0x400
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B42_SHFT                                                0xa
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B41_BMSK                                              0x200
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B41_SHFT                                                0x9
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B40_BMSK                                              0x100
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B40_SHFT                                                0x8
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B39_BMSK                                               0x80
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B39_SHFT                                                0x7
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B38_BMSK                                               0x40
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B38_SHFT                                                0x6
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B37_BMSK                                               0x20
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B37_SHFT                                                0x5
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B36_BMSK                                               0x10
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B36_SHFT                                                0x4
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B35_BMSK                                                0x8
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B35_SHFT                                                0x3
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B34_BMSK                                                0x4
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B34_SHFT                                                0x2
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B33_BMSK                                                0x2
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B33_SHFT                                                0x1
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B32_BMSK                                                0x1
#define HWIO_QFPROM_RAW_CALIB_ROW9_MSB_RSVD_R71_B32_SHFT                                                0x0

#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000240)
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW10_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW10_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW10_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW10_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW10_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B31_BMSK                                        0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B31_SHFT                                              0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B30_BMSK                                        0x40000000
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B30_SHFT                                              0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B29_BMSK                                        0x20000000
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B29_SHFT                                              0x1d
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B28_BMSK                                        0x10000000
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B28_SHFT                                              0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B27_BMSK                                         0x8000000
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B27_SHFT                                              0x1b
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B26_BMSK                                         0x4000000
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B26_SHFT                                              0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B25_BMSK                                         0x2000000
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B25_SHFT                                              0x19
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B24_BMSK                                         0x1000000
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B24_SHFT                                              0x18
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B23_BMSK                                          0x800000
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B23_SHFT                                              0x17
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B22_BMSK                                          0x400000
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B22_SHFT                                              0x16
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B21_BMSK                                          0x200000
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B21_SHFT                                              0x15
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B20_BMSK                                          0x100000
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B20_SHFT                                              0x14
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B19_BMSK                                           0x80000
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B19_SHFT                                              0x13
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B18_BMSK                                           0x40000
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B18_SHFT                                              0x12
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B17_BMSK                                           0x20000
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B17_SHFT                                              0x11
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B16_BMSK                                           0x10000
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B16_SHFT                                              0x10
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B15_BMSK                                            0x8000
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B15_SHFT                                               0xf
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B14_BMSK                                            0x4000
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B14_SHFT                                               0xe
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B13_BMSK                                            0x2000
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B13_SHFT                                               0xd
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B12_BMSK                                            0x1000
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B12_SHFT                                               0xc
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B11_BMSK                                             0x800
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B11_SHFT                                               0xb
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B10_BMSK                                             0x400
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B10_SHFT                                               0xa
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B9_BMSK                                              0x200
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B9_SHFT                                                0x9
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B8_BMSK                                              0x100
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B8_SHFT                                                0x8
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B7_BMSK                                               0x80
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B7_SHFT                                                0x7
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B6_BMSK                                               0x40
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B6_SHFT                                                0x6
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B5_BMSK                                               0x20
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B5_SHFT                                                0x5
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B4_BMSK                                               0x10
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B4_SHFT                                                0x4
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B3_BMSK                                                0x8
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B3_SHFT                                                0x3
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B2_BMSK                                                0x4
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B2_SHFT                                                0x2
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B1_BMSK                                                0x2
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B1_SHFT                                                0x1
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B0_BMSK                                                0x1
#define HWIO_QFPROM_RAW_CALIB_ROW10_LSB_RSVD_R72_B0_SHFT                                                0x0

#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000244)
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW10_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW10_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW10_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW10_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW10_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B63_BMSK                                        0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B63_SHFT                                              0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B62_BMSK                                        0x40000000
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B62_SHFT                                              0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B61_BMSK                                        0x20000000
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B61_SHFT                                              0x1d
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B60_BMSK                                        0x10000000
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B60_SHFT                                              0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B59_BMSK                                         0x8000000
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B59_SHFT                                              0x1b
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B58_BMSK                                         0x4000000
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B58_SHFT                                              0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B57_BMSK                                         0x2000000
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B57_SHFT                                              0x19
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B56_BMSK                                         0x1000000
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B56_SHFT                                              0x18
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B55_BMSK                                          0x800000
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B55_SHFT                                              0x17
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B54_BMSK                                          0x400000
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B54_SHFT                                              0x16
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B53_BMSK                                          0x200000
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B53_SHFT                                              0x15
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B52_BMSK                                          0x100000
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B52_SHFT                                              0x14
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B51_BMSK                                           0x80000
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B51_SHFT                                              0x13
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B50_BMSK                                           0x40000
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B50_SHFT                                              0x12
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B49_BMSK                                           0x20000
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B49_SHFT                                              0x11
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B48_BMSK                                           0x10000
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B48_SHFT                                              0x10
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B47_BMSK                                            0x8000
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B47_SHFT                                               0xf
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B46_BMSK                                            0x4000
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B46_SHFT                                               0xe
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B45_BMSK                                            0x2000
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B45_SHFT                                               0xd
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B44_BMSK                                            0x1000
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B44_SHFT                                               0xc
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B43_BMSK                                             0x800
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B43_SHFT                                               0xb
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B42_BMSK                                             0x400
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B42_SHFT                                               0xa
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B41_BMSK                                             0x200
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B41_SHFT                                               0x9
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B40_BMSK                                             0x100
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B40_SHFT                                               0x8
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B39_BMSK                                              0x80
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B39_SHFT                                               0x7
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B38_BMSK                                              0x40
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B38_SHFT                                               0x6
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B37_BMSK                                              0x20
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B37_SHFT                                               0x5
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B36_BMSK                                              0x10
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B36_SHFT                                               0x4
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B35_BMSK                                               0x8
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B35_SHFT                                               0x3
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B34_BMSK                                               0x4
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B34_SHFT                                               0x2
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B33_BMSK                                               0x2
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B33_SHFT                                               0x1
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B32_BMSK                                               0x1
#define HWIO_QFPROM_RAW_CALIB_ROW10_MSB_RSVD_R72_B32_SHFT                                               0x0

#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000248)
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW11_LSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW11_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW11_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW11_LSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW11_LSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B31_BMSK                                        0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B31_SHFT                                              0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B30_BMSK                                        0x40000000
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B30_SHFT                                              0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B29_BMSK                                        0x20000000
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B29_SHFT                                              0x1d
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B28_BMSK                                        0x10000000
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B28_SHFT                                              0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B27_BMSK                                         0x8000000
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B27_SHFT                                              0x1b
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B26_BMSK                                         0x4000000
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B26_SHFT                                              0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B25_BMSK                                         0x2000000
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B25_SHFT                                              0x19
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B24_BMSK                                         0x1000000
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B24_SHFT                                              0x18
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B23_BMSK                                          0x800000
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B23_SHFT                                              0x17
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B22_BMSK                                          0x400000
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B22_SHFT                                              0x16
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B21_BMSK                                          0x200000
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B21_SHFT                                              0x15
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B20_BMSK                                          0x100000
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B20_SHFT                                              0x14
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B19_BMSK                                           0x80000
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B19_SHFT                                              0x13
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B18_BMSK                                           0x40000
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B18_SHFT                                              0x12
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B17_BMSK                                           0x20000
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B17_SHFT                                              0x11
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B16_BMSK                                           0x10000
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B16_SHFT                                              0x10
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B15_BMSK                                            0x8000
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B15_SHFT                                               0xf
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B14_BMSK                                            0x4000
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B14_SHFT                                               0xe
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B13_BMSK                                            0x2000
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B13_SHFT                                               0xd
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B12_BMSK                                            0x1000
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B12_SHFT                                               0xc
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B11_BMSK                                             0x800
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B11_SHFT                                               0xb
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B10_BMSK                                             0x400
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B10_SHFT                                               0xa
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B9_BMSK                                              0x200
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B9_SHFT                                                0x9
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B8_BMSK                                              0x100
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B8_SHFT                                                0x8
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B7_BMSK                                               0x80
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B7_SHFT                                                0x7
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B6_BMSK                                               0x40
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B6_SHFT                                                0x6
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B5_BMSK                                               0x20
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B5_SHFT                                                0x5
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B4_BMSK                                               0x10
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B4_SHFT                                                0x4
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B3_BMSK                                                0x8
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B3_SHFT                                                0x3
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B2_BMSK                                                0x4
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B2_SHFT                                                0x2
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B1_BMSK                                                0x2
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B1_SHFT                                                0x1
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B0_BMSK                                                0x1
#define HWIO_QFPROM_RAW_CALIB_ROW11_LSB_RSVD_R73_B0_SHFT                                                0x0

#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000024c)
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW11_MSB_ADDR, HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RMSK)
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CALIB_ROW11_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_CALIB_ROW11_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_CALIB_ROW11_MSB_ADDR,m,v,HWIO_QFPROM_RAW_CALIB_ROW11_MSB_IN)
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B63_BMSK                                        0x80000000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B63_SHFT                                              0x1f
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B62_BMSK                                        0x40000000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B62_SHFT                                              0x1e
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B61_BMSK                                        0x20000000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B61_SHFT                                              0x1d
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B60_BMSK                                        0x10000000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B60_SHFT                                              0x1c
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B59_BMSK                                         0x8000000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B59_SHFT                                              0x1b
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B58_BMSK                                         0x4000000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B58_SHFT                                              0x1a
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B57_BMSK                                         0x2000000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B57_SHFT                                              0x19
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B56_BMSK                                         0x1000000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B56_SHFT                                              0x18
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B55_BMSK                                          0x800000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B55_SHFT                                              0x17
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B54_BMSK                                          0x400000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B54_SHFT                                              0x16
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B53_BMSK                                          0x200000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B53_SHFT                                              0x15
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B52_BMSK                                          0x100000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B52_SHFT                                              0x14
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B51_BMSK                                           0x80000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B51_SHFT                                              0x13
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B50_BMSK                                           0x40000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B50_SHFT                                              0x12
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B49_BMSK                                           0x20000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B49_SHFT                                              0x11
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B48_BMSK                                           0x10000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B48_SHFT                                              0x10
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B47_BMSK                                            0x8000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B47_SHFT                                               0xf
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B46_BMSK                                            0x4000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B46_SHFT                                               0xe
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B45_BMSK                                            0x2000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B45_SHFT                                               0xd
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B44_BMSK                                            0x1000
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B44_SHFT                                               0xc
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B43_BMSK                                             0x800
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B43_SHFT                                               0xb
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B42_BMSK                                             0x400
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B42_SHFT                                               0xa
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B41_BMSK                                             0x200
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B41_SHFT                                               0x9
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B40_BMSK                                             0x100
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B40_SHFT                                               0x8
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B39_BMSK                                              0x80
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B39_SHFT                                               0x7
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B38_BMSK                                              0x40
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B38_SHFT                                               0x6
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B37_BMSK                                              0x20
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B37_SHFT                                               0x5
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B36_BMSK                                              0x10
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B36_SHFT                                               0x4
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B35_BMSK                                               0x8
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B35_SHFT                                               0x3
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B34_BMSK                                               0x4
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B34_SHFT                                               0x2
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B33_BMSK                                               0x2
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B33_SHFT                                               0x1
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B32_BMSK                                               0x1
#define HWIO_QFPROM_RAW_CALIB_ROW11_MSB_RSVD_R73_B32_SHFT                                               0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_ADDR(n)                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000250 + 0x8 * (n))
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_MAXn                                                         15
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_REDUN_DATA_BMSK                                      0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_LSB_REDUN_DATA_SHFT                                             0x0

#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_ADDR(n)                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000254 + 0x8 * (n))
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_MAXn                                                         15
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_REDUN_DATA_BMSK                                      0xffffffff
#define HWIO_QFPROM_RAW_MEM_CONFIG_ROWn_MSB_REDUN_DATA_SHFT                                             0x0

#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002d0)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_DISPLAY_RESOLUTION0_LIMITER_1_BMSK               0x80000000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_DISPLAY_RESOLUTION0_LIMITER_1_SHFT                     0x1f
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_RSVD2_BMSK                                       0x40000000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_RSVD2_SHFT                                             0x1e
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_DSI_0_DISABLE_BMSK                               0x20000000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_DSI_0_DISABLE_SHFT                                     0x1d
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_DISPLAY_RESOLUTION0_LIMITER_0_BMSK               0x10000000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_DISPLAY_RESOLUTION0_LIMITER_0_SHFT                     0x1c
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_NIDNT_DISABLE_BMSK                                0x8000000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_NIDNT_DISABLE_SHFT                                     0x1b
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_RSVD1_BMSK                                        0x7e00000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_RSVD1_SHFT                                             0x15
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_SPDIF_0_1_SW_DISABLE_BMSK                          0x100000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_SPDIF_0_1_SW_DISABLE_SHFT                              0x14
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_SPDIF_0_1_SW_DISABLE_ENABLE_FVAL                        0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_SPDIF_0_1_SW_DISABLE_DISABLE_FVAL                       0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_I2S3_DISABLE_BMSK                                   0x80000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_I2S3_DISABLE_SHFT                                      0x13
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_I2S3_DISABLE_ENABLE_FVAL                                0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_I2S3_DISABLE_DISABLE_FVAL                               0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_I2S2_DISABLE_BMSK                                   0x40000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_I2S2_DISABLE_SHFT                                      0x12
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_I2S2_DISABLE_ENABLE_FVAL                                0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_I2S2_DISABLE_DISABLE_FVAL                               0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_I2S1_DISABLE_BMSK                                   0x20000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_I2S1_DISABLE_SHFT                                      0x11
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_I2S1_DISABLE_ENABLE_FVAL                                0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_I2S1_DISABLE_DISABLE_FVAL                               0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_I2S0_DISABLE_BMSK                                   0x10000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_I2S0_DISABLE_SHFT                                      0x10
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_I2S0_DISABLE_ENABLE_FVAL                                0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_I2S0_DISABLE_DISABLE_FVAL                               0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_USB3_DISABLE_BMSK                                    0x8000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_USB3_DISABLE_SHFT                                       0xf
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_USB3_DISABLE_ENABLE_FVAL                                0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_USB3_DISABLE_DISABLE_FVAL                               0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_PCIE_DISABLE_BMSK                                    0x4000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_PCIE_DISABLE_SHFT                                       0xe
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_PCIE_DISABLE_ENABLE_FVAL                                0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_PCIE_DISABLE_DISABLE_FVAL                               0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_HDMI_DISABLE_BMSK                                    0x2000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_HDMI_DISABLE_SHFT                                       0xd
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_HDMI_DISABLE_ENABLE_FVAL                                0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_HDMI_DISABLE_DISABLE_FVAL                               0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_TURING_Q6_DISABLE_BMSK                               0x1000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_TURING_Q6_DISABLE_SHFT                                  0xc
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_TURING_Q6_DISABLE_ENABLE_FVAL                           0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_TURING_Q6_DISABLE_DISABLE_FVAL                          0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_TURING_Q6SS_HVX_DISABLE_BMSK                          0x800
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_TURING_Q6SS_HVX_DISABLE_SHFT                            0xb
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_TURING_Q6SS_HVX_DISABLE_ENABLE_FVAL                     0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_TURING_Q6SS_HVX_DISABLE_DISABLE_FVAL                    0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_EMAC_DISABLE_BMSK                                     0x400
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_EMAC_DISABLE_SHFT                                       0xa
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_EMAC_DISABLE_ENABLE_FVAL                                0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_EMAC_DISABLE_DISABLE_FVAL                               0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_RSVD0_BMSK                                            0x380
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_RSVD0_SHFT                                              0x7
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_DOLBY_BIT_BMSK                                         0x40
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_DOLBY_BIT_SHFT                                          0x6
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_DOLBY_BIT_DISABLE_FVAL                                  0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_DOLBY_BIT_ENABLE_FVAL                                   0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_LPASS_SVA_DISABLE_BMSK                                 0x20
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_LPASS_SVA_DISABLE_SHFT                                  0x5
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_LPASS_SVA_DISABLE_ENABLE_FVAL                           0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_LPASS_SVA_DISABLE_DISABLE_FVAL                          0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_LPASS_Q6_TCM_BOOT_DISABLE_BMSK                         0x10
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_LPASS_Q6_TCM_BOOT_DISABLE_SHFT                          0x4
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_LPASS_Q6_TCM_BOOT_DISABLE_ENABLE_FVAL                   0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_LPASS_Q6_TCM_BOOT_DISABLE_DISABLE_FVAL                  0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_ENABLE_DEVICE_IN_TEST_MODE_BMSK                         0x8
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_ENABLE_DEVICE_IN_TEST_MODE_SHFT                         0x3
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_ARM_CE_DISABLE_USAGE_BMSK                               0x4
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_ARM_CE_DISABLE_USAGE_SHFT                               0x2
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_CE_BAM_DISABLE_BMSK                                     0x2
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_CE_BAM_DISABLE_SHFT                                     0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_CE_BAM_DISABLE_ENABLE_FVAL                              0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_CE_BAM_DISABLE_DISABLE_FVAL                             0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_XBL_SEC_AUTH_DISABLE_BMSK                               0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_XBL_SEC_AUTH_DISABLE_SHFT                               0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_XBL_SEC_AUTH_DISABLE_ENABLE_FVAL                        0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_LSB_XBL_SEC_AUTH_DISABLE_DISABLE_FVAL                       0x1

#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002d4)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_RSVD5_BMSK                                       0x80000000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_RSVD5_SHFT                                             0x1f
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_CM_FEAT_CONFIG_DISABLE_BMSK                      0x40000000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_CM_FEAT_CONFIG_DISABLE_SHFT                            0x1e
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_RSVD4_BMSK                                       0x3f000000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_RSVD4_SHFT                                             0x18
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY1_BMSK                       0x800000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY1_SHFT                           0x17
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY1_ICE_ENGINE_USES_SW_KEY_FVAL        0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY1_ICE_ENGINE_USES_HW_KEY_FVAL        0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY0_BMSK                       0x400000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY0_SHFT                           0x16
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY0_ICE_ENGINE_USES_SW_KEY_FVAL        0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY0_ICE_ENGINE_USES_HW_KEY_FVAL        0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_DISABLE_BMSK                             0x200000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_DISABLE_SHFT                                 0x15
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_DISABLE_ENABLE_FVAL                           0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_DISABLE_DISABLE_FVAL                          0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_RSVD3_BMSK                                         0x1e0000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_RSVD3_SHFT                                             0x11
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_APPS_CLOCKCFG_BMSK                                  0x18000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_APPS_CLOCKCFG_SHFT                                      0xf
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_RSVD2_BMSK                                           0x7800
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_RSVD2_SHFT                                              0xb
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_GFX_FRQ_LIM_FUSE_BMSK                                 0x600
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_GFX_FRQ_LIM_FUSE_SHFT                                   0x9
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_DDR_FRQ_LIM_FUSE_BMSK                                 0x180
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_DDR_FRQ_LIM_FUSE_SHFT                                   0x7
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_RSVD1_BMSK                                             0x40
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_RSVD1_SHFT                                              0x6
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_CFG_BMSK                                 0x20
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_CFG_SHFT                                  0x5
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_CFG_DEFAULT_POWER_ON_BRINGUP_DISABLE_BOOT_FSM_FVAL        0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_CFG_BOOT_FSM_BASED_BRING_UP_FVAL          0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_DELAY_BMSK                               0x18
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_DELAY_SHFT                                0x3
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_DELAY_GDHS_CNT_128_CLAMP_RESET_128_L2_RESET_256_FVAL        0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_DELAY_GDHS_CNT_32_CLAMP_RESET_48_L2_RESET_128_FVAL        0x2
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_DELAY_GDHS_CNT_16_CLAMP_RESET_23_L2_RESET_103_FVAL        0x3
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_DISPLAY_RESOLUTION1_LIMITER_BMSK                        0x6
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_DISPLAY_RESOLUTION1_LIMITER_SHFT                        0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_RSVD0_BMSK                                              0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW0_MSB_RSVD0_SHFT                                              0x0

#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002d8)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_RSVD0_BMSK                                       0xffe00000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_RSVD0_SHFT                                             0x15
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MISC_DEBUG_DISABLE_BMSK                  0x100000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MISC_DEBUG_DISABLE_SHFT                      0x14
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MISC_DEBUG_DISABLE_ENABLE_FVAL                0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MISC_DEBUG_DISABLE_DISABLE_FVAL               0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_BMSK                          0x80000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_SHFT                             0x13
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_ENABLE_FVAL                       0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_DISABLE_FVAL                      0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_BMSK                          0x40000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_SHFT                             0x12
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_ENABLE_FVAL                       0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_DISABLE_FVAL                      0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_NIDEN_DISABLE_BMSK                     0x20000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_NIDEN_DISABLE_SHFT                        0x11
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_NIDEN_DISABLE_ENABLE_FVAL                  0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_NIDEN_DISABLE_DISABLE_FVAL                 0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_DBGEN_DISABLE_BMSK                     0x10000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_DBGEN_DISABLE_SHFT                        0x10
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_DBGEN_DISABLE_ENABLE_FVAL                  0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_DBGEN_DISABLE_DISABLE_FVAL                 0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_NIDEN_DISABLE_BMSK                      0x8000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_NIDEN_DISABLE_SHFT                         0xf
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_NIDEN_DISABLE_ENABLE_FVAL                  0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_NIDEN_DISABLE_DISABLE_FVAL                 0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_DBGEN_DISABLE_BMSK                      0x4000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_DBGEN_DISABLE_SHFT                         0xe
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_DBGEN_DISABLE_ENABLE_FVAL                  0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_DBGEN_DISABLE_DISABLE_FVAL                 0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_NIDEN_DISABLE_BMSK                     0x2000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_NIDEN_DISABLE_SHFT                        0xd
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_NIDEN_DISABLE_ENABLE_FVAL                 0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_NIDEN_DISABLE_DISABLE_FVAL                0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_DBGEN_DISABLE_BMSK                     0x1000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_DBGEN_DISABLE_SHFT                        0xc
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_DBGEN_DISABLE_ENABLE_FVAL                 0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_DBGEN_DISABLE_DISABLE_FVAL                0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPNIDEN_DISABLE_BMSK                   0x800
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPNIDEN_DISABLE_SHFT                     0xb
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPNIDEN_DISABLE_ENABLE_FVAL              0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPNIDEN_DISABLE_DISABLE_FVAL             0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPIDEN_DISABLE_BMSK                    0x400
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPIDEN_DISABLE_SHFT                      0xa
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPIDEN_DISABLE_ENABLE_FVAL               0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPIDEN_DISABLE_DISABLE_FVAL              0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_NIDEN_DISABLE_BMSK                         0x200
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_NIDEN_DISABLE_SHFT                           0x9
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_NIDEN_DISABLE_ENABLE_FVAL                    0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_NIDEN_DISABLE_DISABLE_FVAL                   0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_DBGEN_DISABLE_BMSK                         0x100
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_DBGEN_DISABLE_SHFT                           0x8
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_DBGEN_DISABLE_ENABLE_FVAL                    0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_DBGEN_DISABLE_DISABLE_FVAL                   0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_NIDEN_DISABLE_BMSK                          0x80
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_NIDEN_DISABLE_SHFT                           0x7
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_NIDEN_DISABLE_ENABLE_FVAL                    0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_NIDEN_DISABLE_DISABLE_FVAL                   0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_DBGEN_DISABLE_BMSK                          0x40
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_DBGEN_DISABLE_SHFT                           0x6
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_DBGEN_DISABLE_ENABLE_FVAL                    0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_DBGEN_DISABLE_DISABLE_FVAL                   0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_NIDEN_DISABLE_BMSK                         0x20
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_NIDEN_DISABLE_SHFT                          0x5
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_NIDEN_DISABLE_ENABLE_FVAL                   0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_NIDEN_DISABLE_DISABLE_FVAL                  0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_DBGEN_DISABLE_BMSK                         0x10
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_DBGEN_DISABLE_SHFT                          0x4
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_DBGEN_DISABLE_ENABLE_FVAL                   0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_DBGEN_DISABLE_DISABLE_FVAL                  0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPNIDEN_DISABLE_BMSK                       0x8
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPNIDEN_DISABLE_SHFT                       0x3
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPNIDEN_DISABLE_ENABLE_FVAL                0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPNIDEN_DISABLE_DISABLE_FVAL               0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPIDEN_DISABLE_BMSK                        0x4
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPIDEN_DISABLE_SHFT                        0x2
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPIDEN_DISABLE_ENABLE_FVAL                 0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPIDEN_DISABLE_DISABLE_FVAL                0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_BMSK                                    0x2
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_SHFT                                    0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_ENABLE_FVAL                             0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_DISABLE_FVAL                            0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_BMSK                                        0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_SHFT                                        0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_ENABLE_FVAL                                 0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_DISABLE_FVAL                                0x1

#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002dc)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_BMSK                      0xfe000000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_SHFT                            0x19
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_BMSK                            0x1ffff80
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_SHFT                                  0x7
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_RSVD0_BMSK                                             0x40
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_RSVD0_SHFT                                              0x6
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_APPS_PBL_PATCH_VERSION_BMSK                            0x3f
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW1_MSB_APPS_PBL_PATCH_VERSION_SHFT                             0x0

#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002e0)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_ADDR, HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_RMSK)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_TAP_GEN_SPARE_INSTR_DISABLE_31_0_BMSK            0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_LSB_TAP_GEN_SPARE_INSTR_DISABLE_31_0_SHFT                   0x0

#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002e4)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_ADDR, HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_RMSK)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_SPARE0_BMSK                                      0x80000000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_SPARE0_SHFT                                            0x1f
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_A53SS_DEFEATURE_L2CACHE_1_BMSK                   0x40000000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_A53SS_DEFEATURE_L2CACHE_1_SHFT                         0x1e
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_PBL_QSEE_BOOT_FLOW_DISABLE_BMSK                  0x20000000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_PBL_QSEE_BOOT_FLOW_DISABLE_SHFT                        0x1d
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_4_BMSK                         0x10000000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_4_SHFT                               0x1c
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_TRIGGER_DISABLE_BMSK                    0x8000000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_TRIGGER_DISABLE_SHFT                         0x1b
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_FOUNDRY_ID_BMSK                                   0x7800000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_FOUNDRY_ID_SHFT                                        0x17
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_BMSK                             0x780000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_SHFT                                 0x13
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_7_BMSK                         0x40000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_7_SHFT                            0x12
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_6_BMSK                         0x20000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_6_SHFT                            0x11
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_5_BMSK                         0x10000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_5_SHFT                            0x10
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_RSVD1_BMSK                                           0x8000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_RSVD1_SHFT                                              0xf
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_APPS_AARCH64_ENABLE_BMSK                             0x4000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_APPS_AARCH64_ENABLE_SHFT                                0xe
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_BMSK                             0x3000
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_SHFT                                0xc
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_ENUM_800_MHZ_FVAL                   0x0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_ENUM_400_MHZ_FVAL                   0x1
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_ENUM_200_MHZ_FVAL                   0x2
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_XO_FVAL                             0x3
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FROM_ROM_BMSK                               0x800
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FROM_ROM_SHFT                                 0xb
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_RSVD0_BMSK                                            0x700
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_RSVD0_SHFT                                              0x8
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_BOOT_ROM_PATCH_DISABLE_BMSK                            0xe0
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_BOOT_ROM_PATCH_DISABLE_SHFT                             0x5
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_TAP_GEN_SPARE_INSTR_DISABLE_36_32_BMSK                 0x1f
#define HWIO_QFPROM_RAW_CM_FEAT_CONFIG_ROW2_MSB_TAP_GEN_SPARE_INSTR_DISABLE_36_32_SHFT                  0x0

#define HWIO_QFPROM_RAW_QC_SPARE_REGn_LSB_ADDR(n)                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000268 + 0x8 * (n))
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_LSB_MAXn                                                           17
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SPARE_REGn_LSB_ADDR(n), HWIO_QFPROM_RAW_QC_SPARE_REGn_LSB_RMSK)
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SPARE_REGn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_QC_SPARE_REGn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_QC_SPARE_REGn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_QC_SPARE_REGn_LSB_INI(n))
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_LSB_QC_SPARE_BMSK                                          0xffffffff
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_LSB_QC_SPARE_SHFT                                                 0x0

#define HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_ADDR(n)                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000026c + 0x8 * (n))
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_MAXn                                                           17
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_ADDR(n), HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_RMSK)
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_INI(n))
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_RSVD0_BMSK                                             0x80000000
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_RSVD0_SHFT                                                   0x1f
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_FEC_VALUE_BMSK                                         0x7f000000
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_FEC_VALUE_SHFT                                               0x18
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_QC_SPARE_BMSK                                            0xffffff
#define HWIO_QFPROM_RAW_QC_SPARE_REGn_MSB_QC_SPARE_SHFT                                                 0x0

#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_LSB_ADDR(n)                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002f8 + 0x8 * (n))
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_LSB_RMSK                                         0xffffffff
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_LSB_MAXn                                                  1
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_LSB_KEY_DATA0_BMSK                               0xffffffff
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_LSB_KEY_DATA0_SHFT                                      0x0

#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_ADDR(n)                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x000002fc + 0x8 * (n))
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_RMSK                                         0xffffffff
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_MAXn                                                  1
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_RSVD1_BMSK                                   0x80000000
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_RSVD1_SHFT                                         0x1f
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_FEC_VALUE_BMSK                               0x7f000000
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_FEC_VALUE_SHFT                                     0x18
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_KEY_DATA1_BMSK                                 0xffffff
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROWn_MSB_KEY_DATA1_SHFT                                      0x0

#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_ADDR                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000308)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_RMSK                                         0xffffffff
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_ADDR, HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_IN)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_RSVD0_BMSK                                   0xffff0000
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_RSVD0_SHFT                                         0x10
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_KEY_DATA0_BMSK                                   0xffff
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_LSB_KEY_DATA0_SHFT                                      0x0

#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_ADDR                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000030c)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_RMSK                                         0xffffffff
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_ADDR, HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_ADDR,m,v,HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_IN)
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_RSVD1_BMSK                                   0x80000000
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_RSVD1_SHFT                                         0x1f
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_FEC_VALUE_BMSK                               0x7f000000
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_FEC_VALUE_SHFT                                     0x18
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_RSVD0_BMSK                                     0xffffff
#define HWIO_QFPROM_RAW_OEM_IMAGE_ENCR_KEY_ROW2_MSB_RSVD0_SHFT                                          0x0

#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_ADDR(n)                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000310 + 0x8 * (n))
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_MAXn                                                        1
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT4_BMSK                                     0xff000000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT4_SHFT                                           0x18
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT3_BMSK                                       0xff0000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT3_SHFT                                           0x10
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT2_BMSK                                         0xff00
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT2_SHFT                                            0x8
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT1_BMSK                                           0xff
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT1_SHFT                                            0x0

#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_ADDR(n)                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000314 + 0x8 * (n))
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_MAXn                                                        1
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_RSVD1_BMSK                                         0x80000000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_RSVD1_SHFT                                               0x1f
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_FEC_VALUE_BMSK                                     0x7f000000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_FEC_VALUE_SHFT                                           0x18
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT7_BMSK                                       0xff0000
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT7_SHFT                                           0x10
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT6_BMSK                                         0xff00
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT6_SHFT                                            0x8
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT5_BMSK                                           0xff
#define HWIO_QFPROM_RAW_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT5_SHFT                                            0x0

#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000320 + 0x8 * (n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_RMSK                                     0xffffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_MAXn                                              3
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_BMSK                           0xffffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_SHFT                                  0x0

#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000324 + 0x8 * (n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RMSK                                     0xffffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_MAXn                                              3
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RSVD1_BMSK                               0x80000000
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RSVD1_SHFT                                     0x1f
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_FEC_VALUE_BMSK                           0x7f000000
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_FEC_VALUE_SHFT                                 0x18
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK                             0xffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_SHFT                                  0x0

#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000340)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_RMSK                                     0xffffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_RMSK)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR,m,v,HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_IN)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_BMSK                           0xffffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_SHFT                                  0x0

#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000344)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RMSK                                     0xffffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RMSK)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR,m,v,HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_IN)
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RSVD1_BMSK                               0x80000000
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RSVD1_SHFT                                     0x1f
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_FEC_VALUE_BMSK                           0x7f000000
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_FEC_VALUE_SHFT                                 0x18
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RSVD0_BMSK                                 0xffffff
#define HWIO_QFPROM_RAW_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RSVD0_SHFT                                      0x0

#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000348 + 0x8 * (n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_RMSK                                     0xffffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_MAXn                                              3
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_BMSK                           0xffffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_SHFT                                  0x0

#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000034c + 0x8 * (n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RMSK                                     0xffffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_MAXn                                              3
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RSVD1_BMSK                               0x80000000
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RSVD1_SHFT                                     0x1f
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_FEC_VALUE_BMSK                           0x7f000000
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_FEC_VALUE_SHFT                                 0x18
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK                             0xffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_SHFT                                  0x0

#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000368)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_RMSK                                     0xffffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_RMSK)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR,m,v,HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_IN)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_BMSK                           0xffffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_SHFT                                  0x0

#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000036c)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RMSK                                     0xffffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RMSK)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR,v)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR,m,v,HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_IN)
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RSVD1_BMSK                               0x80000000
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RSVD1_SHFT                                     0x1f
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_FEC_VALUE_BMSK                           0x7f000000
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_FEC_VALUE_SHFT                                 0x18
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RSVD0_BMSK                                 0xffffff
#define HWIO_QFPROM_RAW_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RSVD0_SHFT                                      0x0

#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_ADDR(n)                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000370 + 0x8 * (n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_MAXn                                                          39
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_ADDR(n), HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_RMSK)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_INI(n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_PATCH_DATA_BMSK                                       0xffffffff
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_LSB_PATCH_DATA_SHFT                                              0x0

#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n)                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000374 + 0x8 * (n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_MAXn                                                          39
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n), HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_RMSK)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_INI(n))
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_RSVD1_BMSK                                            0x80000000
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_RSVD1_SHFT                                                  0x1f
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_FEC_VALUE_BMSK                                        0x7f000000
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_FEC_VALUE_SHFT                                              0x18
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_RSVD0_BMSK                                              0xfe0000
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_RSVD0_SHFT                                                  0x11
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_PATCH_ADDR_BMSK                                          0x1fffe
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_PATCH_ADDR_SHFT                                              0x1
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_PATCH_EN_BMSK                                                0x1
#define HWIO_QFPROM_RAW_ROM_PATCH_ROWn_MSB_PATCH_EN_SHFT                                                0x0

#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_ADDR(n)                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000340 + 0x10 * (n))
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_MAXn                                                     27
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_ADDR(n), HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_INI(n))
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_OEM_SPARE_BMSK                                   0xffffffff
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_LSB_OEM_SPARE_SHFT                                          0x0

#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_MSB_ADDR(n)                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000344 + 0x10 * (n))
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_MSB_MAXn                                                     27
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_MSB_ADDR(n), HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_MSB_INI(n))
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_MSB_OEM_SPARE_BMSK                                   0xffffffff
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW0_MSB_OEM_SPARE_SHFT                                          0x0

#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_LSB_ADDR(n)                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000348 + 0x10 * (n))
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_LSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_LSB_MAXn                                                     27
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_LSB_ADDR(n), HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_LSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_LSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_LSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_LSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_LSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_LSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_LSB_INI(n))
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_LSB_OEM_SPARE_BMSK                                   0xffffffff
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_LSB_OEM_SPARE_SHFT                                          0x0

#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_MSB_ADDR(n)                                          (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000034c + 0x10 * (n))
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_MSB_RMSK                                             0xffffffff
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_MSB_MAXn                                                     27
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_MSB_ADDR(n), HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_MSB_RMSK)
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_MSB_ADDR(n), mask)
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_MSB_OUTI(n,val)    \
        out_dword(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_MSB_ADDR(n),val)
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_MSB_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_MSB_ADDR(n),mask,val,HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_MSB_INI(n))
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_MSB_OEM_SPARE_BMSK                                   0xffffffff
#define HWIO_QFPROM_RAW_OEM_SPARE_REGn_ROW1_MSB_OEM_SPARE_SHFT                                          0x0

#define HWIO_QFPROM_BLOW_TIMER_ADDR                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000203c)
#define HWIO_QFPROM_BLOW_TIMER_RMSK                                                                   0xfff
#define HWIO_QFPROM_BLOW_TIMER_IN          \
        in_dword_masked(HWIO_QFPROM_BLOW_TIMER_ADDR, HWIO_QFPROM_BLOW_TIMER_RMSK)
#define HWIO_QFPROM_BLOW_TIMER_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BLOW_TIMER_ADDR, m)
#define HWIO_QFPROM_BLOW_TIMER_OUT(v)      \
        out_dword(HWIO_QFPROM_BLOW_TIMER_ADDR,v)
#define HWIO_QFPROM_BLOW_TIMER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_BLOW_TIMER_ADDR,m,v,HWIO_QFPROM_BLOW_TIMER_IN)
#define HWIO_QFPROM_BLOW_TIMER_BLOW_TIMER_BMSK                                                        0xfff
#define HWIO_QFPROM_BLOW_TIMER_BLOW_TIMER_SHFT                                                          0x0

#define HWIO_QFPROM_TEST_CTRL_ADDR                                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002040)
#define HWIO_QFPROM_TEST_CTRL_RMSK                                                                      0xf
#define HWIO_QFPROM_TEST_CTRL_IN          \
        in_dword_masked(HWIO_QFPROM_TEST_CTRL_ADDR, HWIO_QFPROM_TEST_CTRL_RMSK)
#define HWIO_QFPROM_TEST_CTRL_INM(m)      \
        in_dword_masked(HWIO_QFPROM_TEST_CTRL_ADDR, m)
#define HWIO_QFPROM_TEST_CTRL_OUT(v)      \
        out_dword(HWIO_QFPROM_TEST_CTRL_ADDR,v)
#define HWIO_QFPROM_TEST_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_TEST_CTRL_ADDR,m,v,HWIO_QFPROM_TEST_CTRL_IN)
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_ROM_BMSK                                                          0x8
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_ROM_SHFT                                                          0x3
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_WL_BMSK                                                           0x4
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_WL_SHFT                                                           0x2
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_BL_BMSK                                                           0x2
#define HWIO_QFPROM_TEST_CTRL_SEL_TST_BL_SHFT                                                           0x1
#define HWIO_QFPROM_TEST_CTRL_EN_FUSE_RES_MEAS_BMSK                                                     0x1
#define HWIO_QFPROM_TEST_CTRL_EN_FUSE_RES_MEAS_SHFT                                                     0x0

#define HWIO_QFPROM_ACCEL_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002044)
#define HWIO_QFPROM_ACCEL_RMSK                                                                        0xfff
#define HWIO_QFPROM_ACCEL_IN          \
        in_dword_masked(HWIO_QFPROM_ACCEL_ADDR, HWIO_QFPROM_ACCEL_RMSK)
#define HWIO_QFPROM_ACCEL_INM(m)      \
        in_dword_masked(HWIO_QFPROM_ACCEL_ADDR, m)
#define HWIO_QFPROM_ACCEL_OUT(v)      \
        out_dword(HWIO_QFPROM_ACCEL_ADDR,v)
#define HWIO_QFPROM_ACCEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_ACCEL_ADDR,m,v,HWIO_QFPROM_ACCEL_IN)
#define HWIO_QFPROM_ACCEL_QFPROM_GATELAST_BMSK                                                        0x800
#define HWIO_QFPROM_ACCEL_QFPROM_GATELAST_SHFT                                                          0xb
#define HWIO_QFPROM_ACCEL_QFPROM_TRIPPT_SEL_BMSK                                                      0x700
#define HWIO_QFPROM_ACCEL_QFPROM_TRIPPT_SEL_SHFT                                                        0x8
#define HWIO_QFPROM_ACCEL_QFPROM_ACCEL_BMSK                                                            0xff
#define HWIO_QFPROM_ACCEL_QFPROM_ACCEL_SHFT                                                             0x0

#define HWIO_QFPROM_BLOW_STATUS_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002048)
#define HWIO_QFPROM_BLOW_STATUS_RMSK                                                                    0x3
#define HWIO_QFPROM_BLOW_STATUS_IN          \
        in_dword_masked(HWIO_QFPROM_BLOW_STATUS_ADDR, HWIO_QFPROM_BLOW_STATUS_RMSK)
#define HWIO_QFPROM_BLOW_STATUS_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BLOW_STATUS_ADDR, m)
#define HWIO_QFPROM_BLOW_STATUS_QFPROM_WR_ERR_BMSK                                                      0x2
#define HWIO_QFPROM_BLOW_STATUS_QFPROM_WR_ERR_SHFT                                                      0x1
#define HWIO_QFPROM_BLOW_STATUS_QFPROM_WR_ERR_NO_ERROR_FVAL                                             0x0
#define HWIO_QFPROM_BLOW_STATUS_QFPROM_WR_ERR_ERROR_FVAL                                                0x1
#define HWIO_QFPROM_BLOW_STATUS_QFPROM_BUSY_BMSK                                                        0x1
#define HWIO_QFPROM_BLOW_STATUS_QFPROM_BUSY_SHFT                                                        0x0

#define HWIO_QFPROM_ROM_ERROR_ADDR                                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000204c)
#define HWIO_QFPROM_ROM_ERROR_RMSK                                                                      0x1
#define HWIO_QFPROM_ROM_ERROR_IN          \
        in_dword_masked(HWIO_QFPROM_ROM_ERROR_ADDR, HWIO_QFPROM_ROM_ERROR_RMSK)
#define HWIO_QFPROM_ROM_ERROR_INM(m)      \
        in_dword_masked(HWIO_QFPROM_ROM_ERROR_ADDR, m)
#define HWIO_QFPROM_ROM_ERROR_ERROR_BMSK                                                                0x1
#define HWIO_QFPROM_ROM_ERROR_ERROR_SHFT                                                                0x0
#define HWIO_QFPROM_ROM_ERROR_ERROR_NO_ERROR_FVAL                                                       0x0
#define HWIO_QFPROM_ROM_ERROR_ERROR_ERROR_FVAL                                                          0x1

#define HWIO_QFPROM0_MATCH_STATUS_ADDR                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002050)
#define HWIO_QFPROM0_MATCH_STATUS_RMSK                                                           0xffffffff
#define HWIO_QFPROM0_MATCH_STATUS_IN          \
        in_dword_masked(HWIO_QFPROM0_MATCH_STATUS_ADDR, HWIO_QFPROM0_MATCH_STATUS_RMSK)
#define HWIO_QFPROM0_MATCH_STATUS_INM(m)      \
        in_dword_masked(HWIO_QFPROM0_MATCH_STATUS_ADDR, m)
#define HWIO_QFPROM0_MATCH_STATUS_FLAG_BMSK                                                      0xffffffff
#define HWIO_QFPROM0_MATCH_STATUS_FLAG_SHFT                                                             0x0

#define HWIO_QFPROM1_MATCH_STATUS_ADDR                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002054)
#define HWIO_QFPROM1_MATCH_STATUS_RMSK                                                           0xffffffff
#define HWIO_QFPROM1_MATCH_STATUS_IN          \
        in_dword_masked(HWIO_QFPROM1_MATCH_STATUS_ADDR, HWIO_QFPROM1_MATCH_STATUS_RMSK)
#define HWIO_QFPROM1_MATCH_STATUS_INM(m)      \
        in_dword_masked(HWIO_QFPROM1_MATCH_STATUS_ADDR, m)
#define HWIO_QFPROM1_MATCH_STATUS_FLAG_BMSK                                                      0xffffffff
#define HWIO_QFPROM1_MATCH_STATUS_FLAG_SHFT                                                             0x0

#define HWIO_FEC_ESR_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002058)
#define HWIO_FEC_ESR_RMSK                                                                            0x3fff
#define HWIO_FEC_ESR_IN          \
        in_dword_masked(HWIO_FEC_ESR_ADDR, HWIO_FEC_ESR_RMSK)
#define HWIO_FEC_ESR_INM(m)      \
        in_dword_masked(HWIO_FEC_ESR_ADDR, m)
#define HWIO_FEC_ESR_OUT(v)      \
        out_dword(HWIO_FEC_ESR_ADDR,v)
#define HWIO_FEC_ESR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_FEC_ESR_ADDR,m,v,HWIO_FEC_ESR_IN)
#define HWIO_FEC_ESR_CORR_SW_ACC_BMSK                                                                0x2000
#define HWIO_FEC_ESR_CORR_SW_ACC_SHFT                                                                   0xd
#define HWIO_FEC_ESR_RSVD1_BMSK                                                                      0x1000
#define HWIO_FEC_ESR_RSVD1_SHFT                                                                         0xc
#define HWIO_FEC_ESR_CORR_SECURE_CHANNEL_BMSK                                                         0x800
#define HWIO_FEC_ESR_CORR_SECURE_CHANNEL_SHFT                                                           0xb
#define HWIO_FEC_ESR_CORR_BOOT_ROM_BMSK                                                               0x400
#define HWIO_FEC_ESR_CORR_BOOT_ROM_SHFT                                                                 0xa
#define HWIO_FEC_ESR_CORR_FUSE_SENSE_BMSK                                                             0x200
#define HWIO_FEC_ESR_CORR_FUSE_SENSE_SHFT                                                               0x9
#define HWIO_FEC_ESR_CORR_MULT_BMSK                                                                   0x100
#define HWIO_FEC_ESR_CORR_MULT_SHFT                                                                     0x8
#define HWIO_FEC_ESR_CORR_SEEN_BMSK                                                                    0x80
#define HWIO_FEC_ESR_CORR_SEEN_SHFT                                                                     0x7
#define HWIO_FEC_ESR_CORR_SEEN_NO_CORRECTION_FVAL                                                       0x0
#define HWIO_FEC_ESR_CORR_SEEN_CORRECTION_FVAL                                                          0x1
#define HWIO_FEC_ESR_ERR_SW_ACC_BMSK                                                                   0x40
#define HWIO_FEC_ESR_ERR_SW_ACC_SHFT                                                                    0x6
#define HWIO_FEC_ESR_RSVD0_BMSK                                                                        0x20
#define HWIO_FEC_ESR_RSVD0_SHFT                                                                         0x5
#define HWIO_FEC_ESR_ERR_SECURE_CHANNEL_BMSK                                                           0x10
#define HWIO_FEC_ESR_ERR_SECURE_CHANNEL_SHFT                                                            0x4
#define HWIO_FEC_ESR_ERR_BOOT_ROM_BMSK                                                                  0x8
#define HWIO_FEC_ESR_ERR_BOOT_ROM_SHFT                                                                  0x3
#define HWIO_FEC_ESR_ERR_FUSE_SENSE_BMSK                                                                0x4
#define HWIO_FEC_ESR_ERR_FUSE_SENSE_SHFT                                                                0x2
#define HWIO_FEC_ESR_ERR_MULT_BMSK                                                                      0x2
#define HWIO_FEC_ESR_ERR_MULT_SHFT                                                                      0x1
#define HWIO_FEC_ESR_ERR_SEEN_BMSK                                                                      0x1
#define HWIO_FEC_ESR_ERR_SEEN_SHFT                                                                      0x0
#define HWIO_FEC_ESR_ERR_SEEN_NO_ERROR_FVAL                                                             0x0
#define HWIO_FEC_ESR_ERR_SEEN_ERROR_FVAL                                                                0x1

#define HWIO_FEC_EAR_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000205c)
#define HWIO_FEC_EAR_RMSK                                                                        0xffffffff
#define HWIO_FEC_EAR_IN          \
        in_dword_masked(HWIO_FEC_EAR_ADDR, HWIO_FEC_EAR_RMSK)
#define HWIO_FEC_EAR_INM(m)      \
        in_dword_masked(HWIO_FEC_EAR_ADDR, m)
#define HWIO_FEC_EAR_CORR_ADDR_BMSK                                                              0xffff0000
#define HWIO_FEC_EAR_CORR_ADDR_SHFT                                                                    0x10
#define HWIO_FEC_EAR_ERR_ADDR_BMSK                                                                   0xffff
#define HWIO_FEC_EAR_ERR_ADDR_SHFT                                                                      0x0

#define HWIO_QFPROM_BIST_CTRL_ADDR                                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002060)
#define HWIO_QFPROM_BIST_CTRL_RMSK                                                                     0xff
#define HWIO_QFPROM_BIST_CTRL_IN          \
        in_dword_masked(HWIO_QFPROM_BIST_CTRL_ADDR, HWIO_QFPROM_BIST_CTRL_RMSK)
#define HWIO_QFPROM_BIST_CTRL_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BIST_CTRL_ADDR, m)
#define HWIO_QFPROM_BIST_CTRL_OUT(v)      \
        out_dword(HWIO_QFPROM_BIST_CTRL_ADDR,v)
#define HWIO_QFPROM_BIST_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_BIST_CTRL_ADDR,m,v,HWIO_QFPROM_BIST_CTRL_IN)
#define HWIO_QFPROM_BIST_CTRL_AUTH_REGION_BMSK                                                         0xfc
#define HWIO_QFPROM_BIST_CTRL_AUTH_REGION_SHFT                                                          0x2
#define HWIO_QFPROM_BIST_CTRL_SHA_ENABLE_BMSK                                                           0x2
#define HWIO_QFPROM_BIST_CTRL_SHA_ENABLE_SHFT                                                           0x1
#define HWIO_QFPROM_BIST_CTRL_START_BMSK                                                                0x1
#define HWIO_QFPROM_BIST_CTRL_START_SHFT                                                                0x0

#define HWIO_QFPROM_BIST_ERROR0_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002064)
#define HWIO_QFPROM_BIST_ERROR0_RMSK                                                             0xffffffff
#define HWIO_QFPROM_BIST_ERROR0_IN          \
        in_dword_masked(HWIO_QFPROM_BIST_ERROR0_ADDR, HWIO_QFPROM_BIST_ERROR0_RMSK)
#define HWIO_QFPROM_BIST_ERROR0_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BIST_ERROR0_ADDR, m)
#define HWIO_QFPROM_BIST_ERROR0_ERROR_BMSK                                                       0xffffffff
#define HWIO_QFPROM_BIST_ERROR0_ERROR_SHFT                                                              0x0
#define HWIO_QFPROM_BIST_ERROR0_ERROR_NO_ERROR_FVAL                                                     0x0
#define HWIO_QFPROM_BIST_ERROR0_ERROR_ERROR_FVAL                                                        0x1

#define HWIO_QFPROM_BIST_ERROR1_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002068)
#define HWIO_QFPROM_BIST_ERROR1_RMSK                                                             0xffffffff
#define HWIO_QFPROM_BIST_ERROR1_IN          \
        in_dword_masked(HWIO_QFPROM_BIST_ERROR1_ADDR, HWIO_QFPROM_BIST_ERROR1_RMSK)
#define HWIO_QFPROM_BIST_ERROR1_INM(m)      \
        in_dword_masked(HWIO_QFPROM_BIST_ERROR1_ADDR, m)
#define HWIO_QFPROM_BIST_ERROR1_ERROR_BMSK                                                       0xffffffff
#define HWIO_QFPROM_BIST_ERROR1_ERROR_SHFT                                                              0x0
#define HWIO_QFPROM_BIST_ERROR1_ERROR_NO_ERROR_FVAL                                                     0x0
#define HWIO_QFPROM_BIST_ERROR1_ERROR_ERROR_FVAL                                                        0x1

#define HWIO_QFPROM_HASH_SIGNATUREn_ADDR(n)                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000206c + 0x4 * (n))
#define HWIO_QFPROM_HASH_SIGNATUREn_RMSK                                                         0xffffffff
#define HWIO_QFPROM_HASH_SIGNATUREn_MAXn                                                                  7
#define HWIO_QFPROM_HASH_SIGNATUREn_INI(n)        \
        in_dword_masked(HWIO_QFPROM_HASH_SIGNATUREn_ADDR(n), HWIO_QFPROM_HASH_SIGNATUREn_RMSK)
#define HWIO_QFPROM_HASH_SIGNATUREn_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_HASH_SIGNATUREn_ADDR(n), mask)
#define HWIO_QFPROM_HASH_SIGNATUREn_HASH_VALUE_BMSK                                              0xffffffff
#define HWIO_QFPROM_HASH_SIGNATUREn_HASH_VALUE_SHFT                                                     0x0

#define HWIO_HW_KEY_STATUS_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000208c)
#define HWIO_HW_KEY_STATUS_RMSK                                                                       0x7ff
#define HWIO_HW_KEY_STATUS_IN          \
        in_dword_masked(HWIO_HW_KEY_STATUS_ADDR, HWIO_HW_KEY_STATUS_RMSK)
#define HWIO_HW_KEY_STATUS_INM(m)      \
        in_dword_masked(HWIO_HW_KEY_STATUS_ADDR, m)
#define HWIO_HW_KEY_STATUS_MSA_SECURE_BMSK                                                            0x400
#define HWIO_HW_KEY_STATUS_MSA_SECURE_SHFT                                                              0xa
#define HWIO_HW_KEY_STATUS_MSA_SECURE_MSA_NOT_SECURE_FVAL                                               0x0
#define HWIO_HW_KEY_STATUS_MSA_SECURE_MSA_SECURE_FVAL                                                   0x1
#define HWIO_HW_KEY_STATUS_APPS_SECURE_BMSK                                                           0x200
#define HWIO_HW_KEY_STATUS_APPS_SECURE_SHFT                                                             0x9
#define HWIO_HW_KEY_STATUS_APPS_SECURE_APPS_NOT_SECURE_FVAL                                             0x0
#define HWIO_HW_KEY_STATUS_APPS_SECURE_APPS_SECURE_FVAL                                                 0x1
#define HWIO_HW_KEY_STATUS_KDF_AND_HW_KEY_SHIFT_DONE_BMSK                                             0x100
#define HWIO_HW_KEY_STATUS_KDF_AND_HW_KEY_SHIFT_DONE_SHFT                                               0x8
#define HWIO_HW_KEY_STATUS_KDF_AND_HW_KEY_SHIFT_DONE_KDF_AND_HW_KEY_SHIFT_IN_PROGRESS_FVAL              0x0
#define HWIO_HW_KEY_STATUS_KDF_AND_HW_KEY_SHIFT_DONE_KDF_AND_HW_KEY_SHIFT_HAS_COMPLETED_FVAL            0x1
#define HWIO_HW_KEY_STATUS_HW_KEY_SHIFT_DONE_BMSK                                                      0x80
#define HWIO_HW_KEY_STATUS_HW_KEY_SHIFT_DONE_SHFT                                                       0x7
#define HWIO_HW_KEY_STATUS_HW_KEY_SHIFT_DONE_HW_KEY_SHIFT_IN_PROGRESS_FVAL                              0x0
#define HWIO_HW_KEY_STATUS_HW_KEY_SHIFT_DONE_HW_KEY_SHIFT_HAS_COMPLETED_FVAL                            0x1
#define HWIO_HW_KEY_STATUS_FUSE_SENSE_DONE_BMSK                                                        0x40
#define HWIO_HW_KEY_STATUS_FUSE_SENSE_DONE_SHFT                                                         0x6
#define HWIO_HW_KEY_STATUS_FUSE_SENSE_DONE_FUSE_SENSE_IN_PROGRESS_FVAL                                  0x0
#define HWIO_HW_KEY_STATUS_FUSE_SENSE_DONE_FUSE_SENSE_HAS_COMPLETED_FVAL                                0x1
#define HWIO_HW_KEY_STATUS_CRI_CM_BOOT_DONE_BMSK                                                       0x20
#define HWIO_HW_KEY_STATUS_CRI_CM_BOOT_DONE_SHFT                                                        0x5
#define HWIO_HW_KEY_STATUS_CRI_CM_BOOT_DONE_CRI_CM_BOOT_IN_PROGRESS_FVAL                                0x0
#define HWIO_HW_KEY_STATUS_CRI_CM_BOOT_DONE_CRI_CM_BOOT_HAS_COMPLETED_FVAL                              0x1
#define HWIO_HW_KEY_STATUS_KDF_DONE_BMSK                                                               0x10
#define HWIO_HW_KEY_STATUS_KDF_DONE_SHFT                                                                0x4
#define HWIO_HW_KEY_STATUS_KDF_DONE_KDF_OPERATION_IN_PROGRESS_FVAL                                      0x0
#define HWIO_HW_KEY_STATUS_KDF_DONE_KDF_OPERATION_HAS_COMPLETED_FVAL                                    0x1
#define HWIO_HW_KEY_STATUS_MSA_KEYS_BLOCKED_BMSK                                                        0x8
#define HWIO_HW_KEY_STATUS_MSA_KEYS_BLOCKED_SHFT                                                        0x3
#define HWIO_HW_KEY_STATUS_MSA_KEYS_BLOCKED_NOT_BLOCKED_FVAL                                            0x0
#define HWIO_HW_KEY_STATUS_MSA_KEYS_BLOCKED_BLOCKED_FVAL                                                0x1
#define HWIO_HW_KEY_STATUS_APPS_KEYS_BLOCKED_BMSK                                                       0x4
#define HWIO_HW_KEY_STATUS_APPS_KEYS_BLOCKED_SHFT                                                       0x2
#define HWIO_HW_KEY_STATUS_APPS_KEYS_BLOCKED_NOT_BLOCKED_FVAL                                           0x0
#define HWIO_HW_KEY_STATUS_APPS_KEYS_BLOCKED_BLOCKED_FVAL                                               0x1
#define HWIO_HW_KEY_STATUS_SEC_KEY_DERIVATION_KEY_BLOWN_BMSK                                            0x2
#define HWIO_HW_KEY_STATUS_SEC_KEY_DERIVATION_KEY_BLOWN_SHFT                                            0x1
#define HWIO_HW_KEY_STATUS_SEC_KEY_DERIVATION_KEY_BLOWN_NOT_BLOWN_FVAL                                  0x0
#define HWIO_HW_KEY_STATUS_SEC_KEY_DERIVATION_KEY_BLOWN_BLOWN_FVAL                                      0x1
#define HWIO_HW_KEY_STATUS_PRI_KEY_DERIVATION_KEY_BLOWN_BMSK                                            0x1
#define HWIO_HW_KEY_STATUS_PRI_KEY_DERIVATION_KEY_BLOWN_SHFT                                            0x0
#define HWIO_HW_KEY_STATUS_PRI_KEY_DERIVATION_KEY_BLOWN_NOT_BLOWN_FVAL                                  0x0
#define HWIO_HW_KEY_STATUS_PRI_KEY_DERIVATION_KEY_BLOWN_BLOWN_FVAL                                      0x1

#define HWIO_RESET_JDR_STATUS_ADDR                                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002090)
#define HWIO_RESET_JDR_STATUS_RMSK                                                                      0x3
#define HWIO_RESET_JDR_STATUS_IN          \
        in_dword_masked(HWIO_RESET_JDR_STATUS_ADDR, HWIO_RESET_JDR_STATUS_RMSK)
#define HWIO_RESET_JDR_STATUS_INM(m)      \
        in_dword_masked(HWIO_RESET_JDR_STATUS_ADDR, m)
#define HWIO_RESET_JDR_STATUS_FORCE_RESET_BMSK                                                          0x2
#define HWIO_RESET_JDR_STATUS_FORCE_RESET_SHFT                                                          0x1
#define HWIO_RESET_JDR_STATUS_DISABLE_SYSTEM_RESET_BMSK                                                 0x1
#define HWIO_RESET_JDR_STATUS_DISABLE_SYSTEM_RESET_SHFT                                                 0x0

#define HWIO_ATPG_JDR_STATUS_ADDR                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002094)
#define HWIO_ATPG_JDR_STATUS_RMSK                                                                       0x1
#define HWIO_ATPG_JDR_STATUS_IN          \
        in_dword_masked(HWIO_ATPG_JDR_STATUS_ADDR, HWIO_ATPG_JDR_STATUS_RMSK)
#define HWIO_ATPG_JDR_STATUS_INM(m)      \
        in_dword_masked(HWIO_ATPG_JDR_STATUS_ADDR, m)
#define HWIO_ATPG_JDR_STATUS_FUSE_SENSE_ATPG_CTL_BMSK                                                   0x1
#define HWIO_ATPG_JDR_STATUS_FUSE_SENSE_ATPG_CTL_SHFT                                                   0x0

#define HWIO_FEAT_PROV_OUTn_ADDR(n)                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00002098 + 0x4 * (n))
#define HWIO_FEAT_PROV_OUTn_RMSK                                                                 0xffffffff
#define HWIO_FEAT_PROV_OUTn_MAXn                                                                          3
#define HWIO_FEAT_PROV_OUTn_INI(n)        \
        in_dword_masked(HWIO_FEAT_PROV_OUTn_ADDR(n), HWIO_FEAT_PROV_OUTn_RMSK)
#define HWIO_FEAT_PROV_OUTn_INMI(n,mask)    \
        in_dword_masked(HWIO_FEAT_PROV_OUTn_ADDR(n), mask)
#define HWIO_FEAT_PROV_OUTn_FEAT_PROV_OUT_VALUE_BMSK                                             0xffffffff
#define HWIO_FEAT_PROV_OUTn_FEAT_PROV_OUT_VALUE_SHFT                                                    0x0

#define HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_ADDR(n)                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x000020a8 + 0x4 * (n))
#define HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_RMSK                                                   0xffffffff
#define HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_MAXn                                                            3
#define HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_INI(n)        \
        in_dword_masked(HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_ADDR(n), HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_RMSK)
#define HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_INMI(n,mask)    \
        in_dword_masked(HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_ADDR(n), mask)
#define HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_SEC_CTRL_MISC_CONFIG_STATUS_VALUE_BMSK                 0xffffffff
#define HWIO_SEC_CTRL_MISC_CONFIG_STATUSn_SEC_CTRL_MISC_CONFIG_STATUS_VALUE_SHFT                        0x0

#define HWIO_DEBUG_FUSE_STATUS_ADDR                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000020b8)
#define HWIO_DEBUG_FUSE_STATUS_RMSK                                                              0xffffffff
#define HWIO_DEBUG_FUSE_STATUS_IN          \
        in_dword_masked(HWIO_DEBUG_FUSE_STATUS_ADDR, HWIO_DEBUG_FUSE_STATUS_RMSK)
#define HWIO_DEBUG_FUSE_STATUS_INM(m)      \
        in_dword_masked(HWIO_DEBUG_FUSE_STATUS_ADDR, m)
#define HWIO_DEBUG_FUSE_STATUS_RSVD0_BMSK                                                        0xffe00000
#define HWIO_DEBUG_FUSE_STATUS_RSVD0_SHFT                                                              0x15
#define HWIO_DEBUG_FUSE_STATUS_SHARED_MISC2_DEBUG_BMSK                                             0x100000
#define HWIO_DEBUG_FUSE_STATUS_SHARED_MISC2_DEBUG_SHFT                                                 0x14
#define HWIO_DEBUG_FUSE_STATUS_SHARED_MISC1_DEBUG_BMSK                                              0x80000
#define HWIO_DEBUG_FUSE_STATUS_SHARED_MISC1_DEBUG_SHFT                                                 0x13
#define HWIO_DEBUG_FUSE_STATUS_SHARED_MISC_DEBUG_BMSK                                               0x40000
#define HWIO_DEBUG_FUSE_STATUS_SHARED_MISC_DEBUG_SHFT                                                  0x12
#define HWIO_DEBUG_FUSE_STATUS_APPS_NIDEN_BMSK                                                      0x20000
#define HWIO_DEBUG_FUSE_STATUS_APPS_NIDEN_SHFT                                                         0x11
#define HWIO_DEBUG_FUSE_STATUS_APPS_DBGEN_BMSK                                                      0x10000
#define HWIO_DEBUG_FUSE_STATUS_APPS_DBGEN_SHFT                                                         0x10
#define HWIO_DEBUG_FUSE_STATUS_PRIVATE_QSEE_SPNIDEN_BMSK                                             0x8000
#define HWIO_DEBUG_FUSE_STATUS_PRIVATE_QSEE_SPNIDEN_SHFT                                                0xf
#define HWIO_DEBUG_FUSE_STATUS_SHARED_QSEE_SPNIDEN_BMSK                                              0x4000
#define HWIO_DEBUG_FUSE_STATUS_SHARED_QSEE_SPNIDEN_SHFT                                                 0xe
#define HWIO_DEBUG_FUSE_STATUS_PRIVATE_MSS_NIDEN_BMSK                                                0x2000
#define HWIO_DEBUG_FUSE_STATUS_PRIVATE_MSS_NIDEN_SHFT                                                   0xd
#define HWIO_DEBUG_FUSE_STATUS_SHARED_MSS_NIDEN_BMSK                                                 0x1000
#define HWIO_DEBUG_FUSE_STATUS_SHARED_MSS_NIDEN_SHFT                                                    0xc
#define HWIO_DEBUG_FUSE_STATUS_PRIVATE_CP_NIDEN_BMSK                                                  0x800
#define HWIO_DEBUG_FUSE_STATUS_PRIVATE_CP_NIDEN_SHFT                                                    0xb
#define HWIO_DEBUG_FUSE_STATUS_SHARED_CP_NIDEN_BMSK                                                   0x400
#define HWIO_DEBUG_FUSE_STATUS_SHARED_CP_NIDEN_SHFT                                                     0xa
#define HWIO_DEBUG_FUSE_STATUS_PRIVATE_NS_NIDEN_BMSK                                                  0x200
#define HWIO_DEBUG_FUSE_STATUS_PRIVATE_NS_NIDEN_SHFT                                                    0x9
#define HWIO_DEBUG_FUSE_STATUS_SHARED_NS_NIDEN_BMSK                                                   0x100
#define HWIO_DEBUG_FUSE_STATUS_SHARED_NS_NIDEN_SHFT                                                     0x8
#define HWIO_DEBUG_FUSE_STATUS_PRIVATE_QSEE_SPIDEN_BMSK                                                0x80
#define HWIO_DEBUG_FUSE_STATUS_PRIVATE_QSEE_SPIDEN_SHFT                                                 0x7
#define HWIO_DEBUG_FUSE_STATUS_SHARED_QSEE_SPIDEN_BMSK                                                 0x40
#define HWIO_DEBUG_FUSE_STATUS_SHARED_QSEE_SPIDEN_SHFT                                                  0x6
#define HWIO_DEBUG_FUSE_STATUS_PRIVATE_MSS_DBGEN_BMSK                                                  0x20
#define HWIO_DEBUG_FUSE_STATUS_PRIVATE_MSS_DBGEN_SHFT                                                   0x5
#define HWIO_DEBUG_FUSE_STATUS_SHARED_MSS_DBGEN_BMSK                                                   0x10
#define HWIO_DEBUG_FUSE_STATUS_SHARED_MSS_DBGEN_SHFT                                                    0x4
#define HWIO_DEBUG_FUSE_STATUS_PRIVATE_CP_DBGEN_BMSK                                                    0x8
#define HWIO_DEBUG_FUSE_STATUS_PRIVATE_CP_DBGEN_SHFT                                                    0x3
#define HWIO_DEBUG_FUSE_STATUS_SHARED_CP_DBGEN_BMSK                                                     0x4
#define HWIO_DEBUG_FUSE_STATUS_SHARED_CP_DBGEN_SHFT                                                     0x2
#define HWIO_DEBUG_FUSE_STATUS_PRIVATE_NS_DBGEN_BMSK                                                    0x2
#define HWIO_DEBUG_FUSE_STATUS_PRIVATE_NS_DBGEN_SHFT                                                    0x1
#define HWIO_DEBUG_FUSE_STATUS_SHARED_NS_DBGEN_BMSK                                                     0x1
#define HWIO_DEBUG_FUSE_STATUS_SHARED_NS_DBGEN_SHFT                                                     0x0

#define HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_ADDR(n)                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004000 + 0x4 * (n))
#define HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_MAXn                                                            71
#define HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_ADDR(n), HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_RMSK)
#define HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_ADDR(n), mask)
#define HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_CRI_CM_PRIVATE_BMSK                                     0xffffffff
#define HWIO_QFPROM_CORR_CRI_CM_PRIVATEn_CRI_CM_PRIVATE_SHFT                                            0x0

#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004120)
#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MRC_2_0_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_MRC_2_0_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MRC_2_0_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_LSB_RSVD0_BMSK                                             0xfffffff0
#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_LSB_RSVD0_SHFT                                                    0x4
#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_LSB_ROOT_CERT_ACTIVATION_LIST_BMSK                                0xf
#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_LSB_ROOT_CERT_ACTIVATION_LIST_SHFT                                0x0

#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004124)
#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MRC_2_0_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_MRC_2_0_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MRC_2_0_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_MSB_RSVD1_BMSK                                             0xfffffffe
#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_MSB_RSVD1_SHFT                                                    0x1
#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_MSB_CURRENT_UIE_KEY_SEL_BMSK                                      0x1
#define HWIO_QFPROM_CORR_MRC_2_0_ROW0_MSB_CURRENT_UIE_KEY_SEL_SHFT                                      0x0

#define HWIO_QFPROM_CORR_MRC_2_0_ROW1_LSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004128)
#define HWIO_QFPROM_CORR_MRC_2_0_ROW1_LSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_MRC_2_0_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MRC_2_0_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_MRC_2_0_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_MRC_2_0_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MRC_2_0_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_MRC_2_0_ROW1_LSB_RSVD0_BMSK                                             0xfffffff0
#define HWIO_QFPROM_CORR_MRC_2_0_ROW1_LSB_RSVD0_SHFT                                                    0x4
#define HWIO_QFPROM_CORR_MRC_2_0_ROW1_LSB_ROOT_CERT_REVOCATION_LIST_BMSK                                0xf
#define HWIO_QFPROM_CORR_MRC_2_0_ROW1_LSB_ROOT_CERT_REVOCATION_LIST_SHFT                                0x0

#define HWIO_QFPROM_CORR_MRC_2_0_ROW1_MSB_ADDR                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000412c)
#define HWIO_QFPROM_CORR_MRC_2_0_ROW1_MSB_RMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_MRC_2_0_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_MRC_2_0_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_MRC_2_0_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_MRC_2_0_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_MRC_2_0_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_MRC_2_0_ROW1_MSB_RSVD0_BMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_MRC_2_0_ROW1_MSB_RSVD0_SHFT                                                    0x0

#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004130)
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_RMSK                                                       0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_RSVD0_BMSK                                                 0xc0000000
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_RSVD0_SHFT                                                       0x1e
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_LDO_SUB_FIX_DET_BMSK                                       0x20000000
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_LDO_SUB_FIX_DET_SHFT                                             0x1d
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_MACCHIATO_EN_BMSK                                          0x10000000
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_MACCHIATO_EN_SHFT                                                0x1c
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_FEATURE_ID_BMSK                                             0xff00000
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_FEATURE_ID_SHFT                                                  0x14
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_JTAG_ID_BMSK                                                  0xfffff
#define HWIO_QFPROM_CORR_PTE_ROW0_LSB_JTAG_ID_SHFT                                                      0x0

#define HWIO_QFPROM_CORR_PTE_ROW0_MSB_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004134)
#define HWIO_QFPROM_CORR_PTE_ROW0_MSB_RMSK                                                       0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW0_MSB_PTE_DATA1_BMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW0_MSB_PTE_DATA1_SHFT                                                    0x0

#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004138)
#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_RMSK                                                       0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_SERIAL_NUM_BMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW1_LSB_SERIAL_NUM_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000413c)
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_RMSK                                                       0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_WAFER_ID_BMSK                                              0xf8000000
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_WAFER_ID_SHFT                                                    0x1b
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_DIE_X_BMSK                                                  0x7f80000
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_DIE_X_SHFT                                                       0x13
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_DIE_Y_BMSK                                                    0x7f800
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_DIE_Y_SHFT                                                        0xb
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_RSVD_R39_B42_BMSK                                               0x400
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_RSVD_R39_B42_SHFT                                                 0xa
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_RSVD_R39_B41_BMSK                                               0x200
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_RSVD_R39_B41_SHFT                                                 0x9
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_RSVD_R39_B40_BMSK                                               0x100
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_RSVD_R39_B40_SHFT                                                 0x8
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_LOGIC_RETENTION_BMSK                                             0xe0
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_LOGIC_RETENTION_SHFT                                              0x5
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_SPEED_BIN_BMSK                                                   0x1c
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_SPEED_BIN_SHFT                                                    0x2
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_MX_RET_BIN_BMSK                                                   0x3
#define HWIO_QFPROM_CORR_PTE_ROW1_MSB_MX_RET_BIN_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_PTE_ROW2_LSB_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004140)
#define HWIO_QFPROM_CORR_PTE_ROW2_LSB_RMSK                                                       0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW2_LSB_PTE_DATA0_BMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW2_LSB_PTE_DATA0_SHFT                                                    0x0

#define HWIO_QFPROM_CORR_PTE_ROW2_MSB_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004144)
#define HWIO_QFPROM_CORR_PTE_ROW2_MSB_RMSK                                                       0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW2_MSB_PTE_DATA1_BMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW2_MSB_PTE_DATA1_SHFT                                                    0x0

#define HWIO_QFPROM_CORR_PTE_ROW3_LSB_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004148)
#define HWIO_QFPROM_CORR_PTE_ROW3_LSB_RMSK                                                       0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW3_LSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW3_LSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW3_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW3_LSB_PTE_DATA0_BMSK                                             0xffff0000
#define HWIO_QFPROM_CORR_PTE_ROW3_LSB_PTE_DATA0_SHFT                                                   0x10
#define HWIO_QFPROM_CORR_PTE_ROW3_LSB_CHIP_ID_BMSK                                                   0xffff
#define HWIO_QFPROM_CORR_PTE_ROW3_LSB_CHIP_ID_SHFT                                                      0x0

#define HWIO_QFPROM_CORR_PTE_ROW3_MSB_ADDR                                                       (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000414c)
#define HWIO_QFPROM_CORR_PTE_ROW3_MSB_RMSK                                                       0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW3_MSB_ADDR, HWIO_QFPROM_CORR_PTE_ROW3_MSB_RMSK)
#define HWIO_QFPROM_CORR_PTE_ROW3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PTE_ROW3_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_PTE_ROW3_MSB_PTE_DATA1_BMSK                                             0xffffffff
#define HWIO_QFPROM_CORR_PTE_ROW3_MSB_PTE_DATA1_SHFT                                                    0x0

#define HWIO_QFPROM_CORR_RD_PERM_LSB_ADDR                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004150)
#define HWIO_QFPROM_CORR_RD_PERM_LSB_RMSK                                                        0xffffffff
#define HWIO_QFPROM_CORR_RD_PERM_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_RD_PERM_LSB_ADDR, HWIO_QFPROM_CORR_RD_PERM_LSB_RMSK)
#define HWIO_QFPROM_CORR_RD_PERM_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_RD_PERM_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_RD_PERM_LSB_RSVD0_BMSK                                                  0xf0000000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_RSVD0_SHFT                                                        0x1c
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG27_BMSK                                         0x8000000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG27_SHFT                                              0x1b
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG27_ALLOW_READ_FVAL                                    0x0
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG27_DISABLE_READ_FVAL                                  0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG26_BMSK                                         0x4000000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG26_SHFT                                              0x1a
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG26_ALLOW_READ_FVAL                                    0x0
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG26_DISABLE_READ_FVAL                                  0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG25_BMSK                                         0x2000000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG25_SHFT                                              0x19
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG25_ALLOW_READ_FVAL                                    0x0
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG25_DISABLE_READ_FVAL                                  0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG24_BMSK                                         0x1000000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG24_SHFT                                              0x18
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG24_ALLOW_READ_FVAL                                    0x0
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG24_DISABLE_READ_FVAL                                  0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG23_BMSK                                          0x800000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG23_SHFT                                              0x17
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG23_ALLOW_READ_FVAL                                    0x0
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SPARE_REG23_DISABLE_READ_FVAL                                  0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_BOOT_ROM_PATCH_BMSK                                           0x400000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_BOOT_ROM_PATCH_SHFT                                               0x16
#define HWIO_QFPROM_CORR_RD_PERM_LSB_BOOT_ROM_PATCH_ALLOW_READ_FVAL                                     0x0
#define HWIO_QFPROM_CORR_RD_PERM_LSB_BOOT_ROM_PATCH_DISABLE_READ_FVAL                                   0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_SEC_KEY_DERIVATION_KEY_BMSK                                   0x200000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_SEC_KEY_DERIVATION_KEY_SHFT                                       0x15
#define HWIO_QFPROM_CORR_RD_PERM_LSB_SEC_KEY_DERIVATION_KEY_ALLOW_READ_FVAL                             0x0
#define HWIO_QFPROM_CORR_RD_PERM_LSB_SEC_KEY_DERIVATION_KEY_DISABLE_READ_FVAL                           0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_PRI_KEY_DERIVATION_KEY_BMSK                                   0x100000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_PRI_KEY_DERIVATION_KEY_SHFT                                       0x14
#define HWIO_QFPROM_CORR_RD_PERM_LSB_PRI_KEY_DERIVATION_KEY_ALLOW_READ_FVAL                             0x0
#define HWIO_QFPROM_CORR_RD_PERM_LSB_PRI_KEY_DERIVATION_KEY_DISABLE_READ_FVAL                           0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SEC_BOOT_BMSK                                              0x80000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SEC_BOOT_SHFT                                                 0x13
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SEC_BOOT_ALLOW_READ_FVAL                                       0x0
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_SEC_BOOT_DISABLE_READ_FVAL                                     0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_IMAGE_ENCR_KEY_BMSK                                        0x40000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_IMAGE_ENCR_KEY_SHFT                                           0x12
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_IMAGE_ENCR_KEY_ALLOW_READ_FVAL                                 0x0
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_IMAGE_ENCR_KEY_DISABLE_READ_FVAL                               0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_QC_SPARE_REG17_BMSK                                            0x20000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_QC_SPARE_REG17_SHFT                                               0x11
#define HWIO_QFPROM_CORR_RD_PERM_LSB_QC_SPARE_REG17_ALLOW_READ_FVAL                                     0x0
#define HWIO_QFPROM_CORR_RD_PERM_LSB_QC_SPARE_REG17_DISABLE_READ_FVAL                                   0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_QC_SPARE_REG16_BMSK                                            0x10000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_QC_SPARE_REG16_SHFT                                               0x10
#define HWIO_QFPROM_CORR_RD_PERM_LSB_QC_SPARE_REG16_ALLOW_READ_FVAL                                     0x0
#define HWIO_QFPROM_CORR_RD_PERM_LSB_QC_SPARE_REG16_DISABLE_READ_FVAL                                   0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_CM_FEAT_CONFIG_BMSK                                             0x8000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_CM_FEAT_CONFIG_SHFT                                                0xf
#define HWIO_QFPROM_CORR_RD_PERM_LSB_CM_FEAT_CONFIG_ALLOW_READ_FVAL                                     0x0
#define HWIO_QFPROM_CORR_RD_PERM_LSB_CM_FEAT_CONFIG_DISABLE_READ_FVAL                                   0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_MEM_CONFIG_BMSK                                                 0x4000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_MEM_CONFIG_SHFT                                                    0xe
#define HWIO_QFPROM_CORR_RD_PERM_LSB_MEM_CONFIG_ALLOW_READ_FVAL                                         0x0
#define HWIO_QFPROM_CORR_RD_PERM_LSB_MEM_CONFIG_DISABLE_READ_FVAL                                       0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_CALIB_BMSK                                                      0x2000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_CALIB_SHFT                                                         0xd
#define HWIO_QFPROM_CORR_RD_PERM_LSB_CALIB_ALLOW_READ_FVAL                                              0x0
#define HWIO_QFPROM_CORR_RD_PERM_LSB_CALIB_DISABLE_READ_FVAL                                            0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_PK_HASH0_BMSK                                                   0x1000
#define HWIO_QFPROM_CORR_RD_PERM_LSB_PK_HASH0_SHFT                                                      0xc
#define HWIO_QFPROM_CORR_RD_PERM_LSB_PK_HASH0_ALLOW_READ_FVAL                                           0x0
#define HWIO_QFPROM_CORR_RD_PERM_LSB_PK_HASH0_DISABLE_READ_FVAL                                         0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_FEAT_CONFIG_BMSK                                                 0x800
#define HWIO_QFPROM_CORR_RD_PERM_LSB_FEAT_CONFIG_SHFT                                                   0xb
#define HWIO_QFPROM_CORR_RD_PERM_LSB_FEAT_CONFIG_ALLOW_READ_FVAL                                        0x0
#define HWIO_QFPROM_CORR_RD_PERM_LSB_FEAT_CONFIG_DISABLE_READ_FVAL                                      0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_CONFIG_BMSK                                                  0x400
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_CONFIG_SHFT                                                    0xa
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_CONFIG_ALLOW_READ_FVAL                                         0x0
#define HWIO_QFPROM_CORR_RD_PERM_LSB_OEM_CONFIG_DISABLE_READ_FVAL                                       0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_4_BMSK                                             0x200
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_4_SHFT                                               0x9
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_4_ALLOW_READ_FVAL                                    0x0
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_4_DISABLE_READ_FVAL                                  0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_3_BMSK                                             0x100
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_3_SHFT                                               0x8
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_3_ALLOW_READ_FVAL                                    0x0
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_3_DISABLE_READ_FVAL                                  0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_2_BMSK                                              0x80
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_2_SHFT                                               0x7
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_2_ALLOW_READ_FVAL                                    0x0
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_2_DISABLE_READ_FVAL                                  0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_1_BMSK                                              0x40
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_1_SHFT                                               0x6
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_1_ALLOW_READ_FVAL                                    0x0
#define HWIO_QFPROM_CORR_RD_PERM_LSB_ANTI_ROLLBACK_1_DISABLE_READ_FVAL                                  0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_FEC_EN_BMSK                                                       0x20
#define HWIO_QFPROM_CORR_RD_PERM_LSB_FEC_EN_SHFT                                                        0x5
#define HWIO_QFPROM_CORR_RD_PERM_LSB_FEC_EN_ALLOW_READ_FVAL                                             0x0
#define HWIO_QFPROM_CORR_RD_PERM_LSB_FEC_EN_DISABLE_READ_FVAL                                           0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_WR_PERM_BMSK                                                      0x10
#define HWIO_QFPROM_CORR_RD_PERM_LSB_WR_PERM_SHFT                                                       0x4
#define HWIO_QFPROM_CORR_RD_PERM_LSB_WR_PERM_ALLOW_READ_FVAL                                            0x0
#define HWIO_QFPROM_CORR_RD_PERM_LSB_WR_PERM_DISABLE_READ_FVAL                                          0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_RD_PERM_BMSK                                                       0x8
#define HWIO_QFPROM_CORR_RD_PERM_LSB_RD_PERM_SHFT                                                       0x3
#define HWIO_QFPROM_CORR_RD_PERM_LSB_RD_PERM_ALLOW_READ_FVAL                                            0x0
#define HWIO_QFPROM_CORR_RD_PERM_LSB_RD_PERM_DISABLE_READ_FVAL                                          0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_PTE_BMSK                                                           0x4
#define HWIO_QFPROM_CORR_RD_PERM_LSB_PTE_SHFT                                                           0x2
#define HWIO_QFPROM_CORR_RD_PERM_LSB_PTE_ALLOW_READ_FVAL                                                0x0
#define HWIO_QFPROM_CORR_RD_PERM_LSB_PTE_DISABLE_READ_FVAL                                              0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_MRC_2_0_BMSK                                                       0x2
#define HWIO_QFPROM_CORR_RD_PERM_LSB_MRC_2_0_SHFT                                                       0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_MRC_2_0_ALLOW_READ_FVAL                                            0x0
#define HWIO_QFPROM_CORR_RD_PERM_LSB_MRC_2_0_DISABLE_READ_FVAL                                          0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_CRI_CM_PRIVATE_BMSK                                                0x1
#define HWIO_QFPROM_CORR_RD_PERM_LSB_CRI_CM_PRIVATE_SHFT                                                0x0
#define HWIO_QFPROM_CORR_RD_PERM_LSB_CRI_CM_PRIVATE_ALLOW_READ_FVAL                                     0x0
#define HWIO_QFPROM_CORR_RD_PERM_LSB_CRI_CM_PRIVATE_DISABLE_READ_FVAL                                   0x1

#define HWIO_QFPROM_CORR_RD_PERM_MSB_ADDR                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004154)
#define HWIO_QFPROM_CORR_RD_PERM_MSB_RMSK                                                        0xffffffff
#define HWIO_QFPROM_CORR_RD_PERM_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_RD_PERM_MSB_ADDR, HWIO_QFPROM_CORR_RD_PERM_MSB_RMSK)
#define HWIO_QFPROM_CORR_RD_PERM_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_RD_PERM_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_RD_PERM_MSB_RSVD0_BMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_RD_PERM_MSB_RSVD0_SHFT                                                         0x0

#define HWIO_QFPROM_CORR_WR_PERM_LSB_ADDR                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004158)
#define HWIO_QFPROM_CORR_WR_PERM_LSB_RMSK                                                        0xffffffff
#define HWIO_QFPROM_CORR_WR_PERM_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_WR_PERM_LSB_ADDR, HWIO_QFPROM_CORR_WR_PERM_LSB_RMSK)
#define HWIO_QFPROM_CORR_WR_PERM_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_WR_PERM_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_WR_PERM_LSB_RSVD0_BMSK                                                  0xf0000000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_RSVD0_SHFT                                                        0x1c
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG27_BMSK                                         0x8000000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG27_SHFT                                              0x1b
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG27_ALLOW_WRITE_FVAL                                   0x0
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG27_DISABLE_WRITE_FVAL                                 0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG26_BMSK                                         0x4000000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG26_SHFT                                              0x1a
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG26_ALLOW_WRITE_FVAL                                   0x0
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG26_DISABLE_WRITE_FVAL                                 0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG25_BMSK                                         0x2000000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG25_SHFT                                              0x19
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG25_ALLOW_WRITE_FVAL                                   0x0
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG25_DISABLE_WRITE_FVAL                                 0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG24_BMSK                                         0x1000000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG24_SHFT                                              0x18
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG24_ALLOW_WRITE_FVAL                                   0x0
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG24_DISABLE_WRITE_FVAL                                 0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG23_BMSK                                          0x800000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG23_SHFT                                              0x17
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG23_ALLOW_WRITE_FVAL                                   0x0
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SPARE_REG23_DISABLE_WRITE_FVAL                                 0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_BOOT_ROM_PATCH_BMSK                                           0x400000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_BOOT_ROM_PATCH_SHFT                                               0x16
#define HWIO_QFPROM_CORR_WR_PERM_LSB_BOOT_ROM_PATCH_ALLOW_WRITE_FVAL                                    0x0
#define HWIO_QFPROM_CORR_WR_PERM_LSB_BOOT_ROM_PATCH_DISABLE_WRITE_FVAL                                  0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_BMSK                                   0x200000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_SHFT                                       0x15
#define HWIO_QFPROM_CORR_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_ALLOW_WRITE_FVAL                            0x0
#define HWIO_QFPROM_CORR_WR_PERM_LSB_SEC_KEY_DERIVATION_KEY_DISABLE_WRITE_FVAL                          0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_BMSK                                   0x100000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_SHFT                                       0x14
#define HWIO_QFPROM_CORR_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_ALLOW_WRITE_FVAL                            0x0
#define HWIO_QFPROM_CORR_WR_PERM_LSB_PRI_KEY_DERIVATION_KEY_DISABLE_WRITE_FVAL                          0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SEC_BOOT_BMSK                                              0x80000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SEC_BOOT_SHFT                                                 0x13
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SEC_BOOT_ALLOW_WRITE_FVAL                                      0x0
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_SEC_BOOT_DISABLE_WRITE_FVAL                                    0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_IMAGE_ENCR_KEY_BMSK                                        0x40000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_IMAGE_ENCR_KEY_SHFT                                           0x12
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_IMAGE_ENCR_KEY_ALLOW_WRITE_FVAL                                0x0
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_IMAGE_ENCR_KEY_DISABLE_WRITE_FVAL                              0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_QC_SPARE_REG17_BMSK                                            0x20000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_QC_SPARE_REG17_SHFT                                               0x11
#define HWIO_QFPROM_CORR_WR_PERM_LSB_QC_SPARE_REG17_ALLOW_WRITE_FVAL                                    0x0
#define HWIO_QFPROM_CORR_WR_PERM_LSB_QC_SPARE_REG17_DISABLE_WRITE_FVAL                                  0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_QC_SPARE_REG16_BMSK                                            0x10000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_QC_SPARE_REG16_SHFT                                               0x10
#define HWIO_QFPROM_CORR_WR_PERM_LSB_QC_SPARE_REG16_ALLOW_WRITE_FVAL                                    0x0
#define HWIO_QFPROM_CORR_WR_PERM_LSB_QC_SPARE_REG16_DISABLE_WRITE_FVAL                                  0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_CM_FEAT_CONFIG_BMSK                                             0x8000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_CM_FEAT_CONFIG_SHFT                                                0xf
#define HWIO_QFPROM_CORR_WR_PERM_LSB_CM_FEAT_CONFIG_ALLOW_WRITE_FVAL                                    0x0
#define HWIO_QFPROM_CORR_WR_PERM_LSB_CM_FEAT_CONFIG_DISABLE_WRITE_FVAL                                  0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_MEM_CONFIG_BMSK                                                 0x4000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_MEM_CONFIG_SHFT                                                    0xe
#define HWIO_QFPROM_CORR_WR_PERM_LSB_MEM_CONFIG_ALLOW_WRITE_FVAL                                        0x0
#define HWIO_QFPROM_CORR_WR_PERM_LSB_MEM_CONFIG_DISABLE_WRITE_FVAL                                      0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_CALIB_BMSK                                                      0x2000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_CALIB_SHFT                                                         0xd
#define HWIO_QFPROM_CORR_WR_PERM_LSB_CALIB_ALLOW_WRITE_FVAL                                             0x0
#define HWIO_QFPROM_CORR_WR_PERM_LSB_CALIB_DISABLE_WRITE_FVAL                                           0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_PK_HASH0_BMSK                                                   0x1000
#define HWIO_QFPROM_CORR_WR_PERM_LSB_PK_HASH0_SHFT                                                      0xc
#define HWIO_QFPROM_CORR_WR_PERM_LSB_PK_HASH0_ALLOW_WRITE_FVAL                                          0x0
#define HWIO_QFPROM_CORR_WR_PERM_LSB_PK_HASH0_DISABLE_WRITE_FVAL                                        0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_FEAT_CONFIG_BMSK                                                 0x800
#define HWIO_QFPROM_CORR_WR_PERM_LSB_FEAT_CONFIG_SHFT                                                   0xb
#define HWIO_QFPROM_CORR_WR_PERM_LSB_FEAT_CONFIG_ALLOW_WRITE_FVAL                                       0x0
#define HWIO_QFPROM_CORR_WR_PERM_LSB_FEAT_CONFIG_DISABLE_WRITE_FVAL                                     0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_CONFIG_BMSK                                                  0x400
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_CONFIG_SHFT                                                    0xa
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_CONFIG_ALLOW_WRITE_FVAL                                        0x0
#define HWIO_QFPROM_CORR_WR_PERM_LSB_OEM_CONFIG_DISABLE_WRITE_FVAL                                      0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_4_BMSK                                             0x200
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_4_SHFT                                               0x9
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_4_ALLOW_WRITE_FVAL                                   0x0
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_4_DISABLE_WRITE_FVAL                                 0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_3_BMSK                                             0x100
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_3_SHFT                                               0x8
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_3_ALLOW_WRITE_FVAL                                   0x0
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_3_DISABLE_WRITE_FVAL                                 0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_2_BMSK                                              0x80
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_2_SHFT                                               0x7
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_2_ALLOW_WRITE_FVAL                                   0x0
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_2_DISABLE_WRITE_FVAL                                 0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_1_BMSK                                              0x40
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_1_SHFT                                               0x6
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_1_ALLOW_WRITE_FVAL                                   0x0
#define HWIO_QFPROM_CORR_WR_PERM_LSB_ANTI_ROLLBACK_1_DISABLE_WRITE_FVAL                                 0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_FEC_EN_BMSK                                                       0x20
#define HWIO_QFPROM_CORR_WR_PERM_LSB_FEC_EN_SHFT                                                        0x5
#define HWIO_QFPROM_CORR_WR_PERM_LSB_FEC_EN_ALLOW_WRITE_FVAL                                            0x0
#define HWIO_QFPROM_CORR_WR_PERM_LSB_FEC_EN_DISABLE_WRITE_FVAL                                          0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_WR_PERM_BMSK                                                      0x10
#define HWIO_QFPROM_CORR_WR_PERM_LSB_WR_PERM_SHFT                                                       0x4
#define HWIO_QFPROM_CORR_WR_PERM_LSB_WR_PERM_ALLOW_WRITE_FVAL                                           0x0
#define HWIO_QFPROM_CORR_WR_PERM_LSB_WR_PERM_DISABLE_WRITE_FVAL                                         0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_RD_PERM_BMSK                                                       0x8
#define HWIO_QFPROM_CORR_WR_PERM_LSB_RD_PERM_SHFT                                                       0x3
#define HWIO_QFPROM_CORR_WR_PERM_LSB_RD_PERM_ALLOW_WRITE_FVAL                                           0x0
#define HWIO_QFPROM_CORR_WR_PERM_LSB_RD_PERM_DISABLE_WRITE_FVAL                                         0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_PTE_BMSK                                                           0x4
#define HWIO_QFPROM_CORR_WR_PERM_LSB_PTE_SHFT                                                           0x2
#define HWIO_QFPROM_CORR_WR_PERM_LSB_PTE_ALLOW_WRITE_FVAL                                               0x0
#define HWIO_QFPROM_CORR_WR_PERM_LSB_PTE_DISABLE_WRITE_FVAL                                             0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_MRC_2_0_BMSK                                                       0x2
#define HWIO_QFPROM_CORR_WR_PERM_LSB_MRC_2_0_SHFT                                                       0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_MRC_2_0_ALLOW_WRITE_FVAL                                           0x0
#define HWIO_QFPROM_CORR_WR_PERM_LSB_MRC_2_0_DISABLE_WRITE_FVAL                                         0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_CRI_CM_PRIVATE_BMSK                                                0x1
#define HWIO_QFPROM_CORR_WR_PERM_LSB_CRI_CM_PRIVATE_SHFT                                                0x0
#define HWIO_QFPROM_CORR_WR_PERM_LSB_CRI_CM_PRIVATE_ALLOW_WRITE_FVAL                                    0x0
#define HWIO_QFPROM_CORR_WR_PERM_LSB_CRI_CM_PRIVATE_DISABLE_WRITE_FVAL                                  0x1

#define HWIO_QFPROM_CORR_WR_PERM_MSB_ADDR                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000415c)
#define HWIO_QFPROM_CORR_WR_PERM_MSB_RMSK                                                        0xffffffff
#define HWIO_QFPROM_CORR_WR_PERM_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_WR_PERM_MSB_ADDR, HWIO_QFPROM_CORR_WR_PERM_MSB_RMSK)
#define HWIO_QFPROM_CORR_WR_PERM_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_WR_PERM_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_WR_PERM_MSB_RSVD0_BMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_WR_PERM_MSB_RSVD0_SHFT                                                         0x0

#define HWIO_QFPROM_CORR_FEC_EN_LSB_ADDR                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004160)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_RMSK                                                         0xffffffff
#define HWIO_QFPROM_CORR_FEC_EN_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_LSB_ADDR, HWIO_QFPROM_CORR_FEC_EN_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEC_EN_LSB_RSVD0_BMSK                                                   0xf0000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_RSVD0_SHFT                                                         0x1c
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION27_FEC_EN_BMSK                                          0x8000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION27_FEC_EN_SHFT                                               0x1b
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION27_FEC_EN_DISABLE_FVAL                                        0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION27_FEC_EN_ENABLE_FVAL                                         0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION26_FEC_EN_BMSK                                          0x4000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION26_FEC_EN_SHFT                                               0x1a
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION26_FEC_EN_DISABLE_FVAL                                        0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION26_FEC_EN_ENABLE_FVAL                                         0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION25_FEC_EN_BMSK                                          0x2000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION25_FEC_EN_SHFT                                               0x19
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION25_FEC_EN_DISABLE_FVAL                                        0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION25_FEC_EN_ENABLE_FVAL                                         0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION24_FEC_EN_BMSK                                          0x1000000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION24_FEC_EN_SHFT                                               0x18
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION24_FEC_EN_DISABLE_FVAL                                        0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION24_FEC_EN_ENABLE_FVAL                                         0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION23_FEC_EN_BMSK                                           0x800000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION23_FEC_EN_SHFT                                               0x17
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION23_FEC_EN_DISABLE_FVAL                                        0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION23_FEC_EN_ENABLE_FVAL                                         0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION22_FEC_EN_BMSK                                           0x400000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION22_FEC_EN_SHFT                                               0x16
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION22_FEC_EN_DISABLE_FVAL                                        0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION22_FEC_EN_ENABLE_FVAL                                         0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION21_FEC_EN_BMSK                                           0x200000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION21_FEC_EN_SHFT                                               0x15
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION21_FEC_EN_DISABLE_FVAL                                        0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION21_FEC_EN_ENABLE_FVAL                                         0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION20_FEC_EN_BMSK                                           0x100000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION20_FEC_EN_SHFT                                               0x14
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION20_FEC_EN_DISABLE_FVAL                                        0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION20_FEC_EN_ENABLE_FVAL                                         0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION19_FEC_EN_BMSK                                            0x80000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION19_FEC_EN_SHFT                                               0x13
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION19_FEC_EN_DISABLE_FVAL                                        0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION19_FEC_EN_ENABLE_FVAL                                         0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION18_FEC_EN_BMSK                                            0x40000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION18_FEC_EN_SHFT                                               0x12
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION18_FEC_EN_DISABLE_FVAL                                        0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION18_FEC_EN_ENABLE_FVAL                                         0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION17_FEC_EN_BMSK                                            0x20000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION17_FEC_EN_SHFT                                               0x11
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION17_FEC_EN_DISABLE_FVAL                                        0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION17_FEC_EN_ENABLE_FVAL                                         0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION16_FEC_EN_BMSK                                            0x10000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION16_FEC_EN_SHFT                                               0x10
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION16_FEC_EN_DISABLE_FVAL                                        0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION16_FEC_EN_ENABLE_FVAL                                         0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION15_FEC_EN_BMSK                                             0x8000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION15_FEC_EN_SHFT                                                0xf
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION15_FEC_EN_DISABLE_FVAL                                        0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION15_FEC_EN_ENABLE_FVAL                                         0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION14_FEC_EN_BMSK                                             0x4000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION14_FEC_EN_SHFT                                                0xe
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION14_FEC_EN_DISABLE_FVAL                                        0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION14_FEC_EN_ENABLE_FVAL                                         0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION13_FEC_EN_BMSK                                             0x2000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION13_FEC_EN_SHFT                                                0xd
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION13_FEC_EN_DISABLE_FVAL                                        0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION13_FEC_EN_ENABLE_FVAL                                         0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION12_FEC_EN_BMSK                                             0x1000
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION12_FEC_EN_SHFT                                                0xc
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION12_FEC_EN_DISABLE_FVAL                                        0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION12_FEC_EN_ENABLE_FVAL                                         0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION11_FEC_EN_BMSK                                              0x800
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION11_FEC_EN_SHFT                                                0xb
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION11_FEC_EN_DISABLE_FVAL                                        0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION11_FEC_EN_ENABLE_FVAL                                         0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION10_FEC_EN_BMSK                                              0x400
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION10_FEC_EN_SHFT                                                0xa
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION10_FEC_EN_DISABLE_FVAL                                        0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION10_FEC_EN_ENABLE_FVAL                                         0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION9_FEC_EN_BMSK                                               0x200
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION9_FEC_EN_SHFT                                                 0x9
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION9_FEC_EN_DISABLE_FVAL                                         0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION9_FEC_EN_ENABLE_FVAL                                          0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION8_FEC_EN_BMSK                                               0x100
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION8_FEC_EN_SHFT                                                 0x8
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION8_FEC_EN_DISABLE_FVAL                                         0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION8_FEC_EN_ENABLE_FVAL                                          0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION7_FEC_EN_BMSK                                                0x80
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION7_FEC_EN_SHFT                                                 0x7
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION7_FEC_EN_DISABLE_FVAL                                         0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION7_FEC_EN_ENABLE_FVAL                                          0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION6_FEC_EN_BMSK                                                0x40
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION6_FEC_EN_SHFT                                                 0x6
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION6_FEC_EN_DISABLE_FVAL                                         0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION6_FEC_EN_ENABLE_FVAL                                          0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION5_FEC_EN_BMSK                                                0x20
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION5_FEC_EN_SHFT                                                 0x5
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION5_FEC_EN_DISABLE_FVAL                                         0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION5_FEC_EN_ENABLE_FVAL                                          0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION4_FEC_EN_BMSK                                                0x10
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION4_FEC_EN_SHFT                                                 0x4
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION4_FEC_EN_DISABLE_FVAL                                         0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION4_FEC_EN_ENABLE_FVAL                                          0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION3_FEC_EN_BMSK                                                 0x8
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION3_FEC_EN_SHFT                                                 0x3
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION3_FEC_EN_DISABLE_FVAL                                         0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION3_FEC_EN_ENABLE_FVAL                                          0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION2_FEC_EN_BMSK                                                 0x4
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION2_FEC_EN_SHFT                                                 0x2
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION2_FEC_EN_DISABLE_FVAL                                         0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION2_FEC_EN_ENABLE_FVAL                                          0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION1_FEC_EN_BMSK                                                 0x2
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION1_FEC_EN_SHFT                                                 0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION1_FEC_EN_DISABLE_FVAL                                         0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION1_FEC_EN_ENABLE_FVAL                                          0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION0_FEC_EN_BMSK                                                 0x1
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION0_FEC_EN_SHFT                                                 0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION0_FEC_EN_DISABLE_FVAL                                         0x0
#define HWIO_QFPROM_CORR_FEC_EN_LSB_REGION0_FEC_EN_ENABLE_FVAL                                          0x1

#define HWIO_QFPROM_CORR_FEC_EN_MSB_ADDR                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004164)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_RMSK                                                         0xffffffff
#define HWIO_QFPROM_CORR_FEC_EN_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_MSB_ADDR, HWIO_QFPROM_CORR_FEC_EN_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEC_EN_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEC_EN_MSB_RSVD0_BMSK                                                   0xffffffff
#define HWIO_QFPROM_CORR_FEC_EN_MSB_RSVD0_SHFT                                                          0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_ADDR                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004168)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_XBL0_BMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_LSB_XBL0_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_ADDR                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000416c)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_XBL1_BMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_1_MSB_XBL1_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_ADDR                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004170)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_PIL_SUBSYSTEM_31_0_BMSK                             0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_LSB_PIL_SUBSYSTEM_31_0_SHFT                                    0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ADDR                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004174)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_XBL_SEC_BMSK                                        0xfe000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_XBL_SEC_SHFT                                              0x19
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_RPM_BMSK                                             0x1fe0000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_RPM_SHFT                                                  0x11
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_TZ_BMSK                                                0x1ffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_2_MSB_TZ_SHFT                                                    0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_ADDR                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004178)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_XBL_CONFIG_1_0_BMSK                                 0xc0000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_XBL_CONFIG_1_0_SHFT                                       0x1e
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_TQS_HASH_ACTIVE_BMSK                                0x3e000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_TQS_HASH_ACTIVE_SHFT                                      0x19
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_RPMB_KEY_PROVISIONED_BMSK                            0x1000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_RPMB_KEY_PROVISIONED_SHFT                                 0x18
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_RPMB_KEY_PROVISIONED_RPMB_KEY_NOT_PROVISIONED_FVAL         0x0
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_RPMB_KEY_PROVISIONED_RPMB_KEY_PROVISIONED_FVAL             0x1
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_PIL_SUBSYSTEM_47_32_BMSK                              0xffff00
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_PIL_SUBSYSTEM_47_32_SHFT                                   0x8
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_SAFESWITCH_BMSK                                           0xff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_LSB_SAFESWITCH_SHFT                                            0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_ADDR                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000417c)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_XBL_CONFIG_5_2_BMSK                                 0xf0000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_XBL_CONFIG_5_2_SHFT                                       0x1c
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_DEVICE_CFG_BMSK                                      0xffe0000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_DEVICE_CFG_SHFT                                           0x11
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_DEBUG_POLICY_BMSK                                      0x1f000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_DEBUG_POLICY_SHFT                                          0xc
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_HYPERVISOR_BMSK                                          0xfff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_3_MSB_HYPERVISOR_SHFT                                            0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_ADDR                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004180)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_MSS_BMSK                                            0xffff0000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_MSS_SHFT                                                  0x10
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_MISC_BMSK                                               0xffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_LSB_MISC_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_ADDR                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004184)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_ADDR, HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_RMSK)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_SIMLOCK_BMSK                                        0x80000000
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_SIMLOCK_SHFT                                              0x1f
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_RSVD0_BMSK                                          0x7fffffff
#define HWIO_QFPROM_CORR_ANTI_ROLLBACK_4_MSB_RSVD0_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004188)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SHARED_QSEE_SPNIDEN_DISABLE_BMSK                    0x80000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SHARED_QSEE_SPNIDEN_DISABLE_SHFT                          0x1f
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SHARED_QSEE_SPNIDEN_DISABLE_ENABLE_FVAL                    0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SHARED_QSEE_SPNIDEN_DISABLE_DISABLE_FVAL                   0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SHARED_QSEE_SPIDEN_DISABLE_BMSK                     0x40000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SHARED_QSEE_SPIDEN_DISABLE_SHFT                           0x1e
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SHARED_QSEE_SPIDEN_DISABLE_ENABLE_FVAL                     0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SHARED_QSEE_SPIDEN_DISABLE_DISABLE_FVAL                    0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ALL_DEBUG_DISABLE_BMSK                              0x20000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ALL_DEBUG_DISABLE_SHFT                                    0x1d
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ALL_DEBUG_DISABLE_ENABLE_FVAL                              0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ALL_DEBUG_DISABLE_DISABLE_FVAL                             0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_DEBUG_POLICY_DISABLE_BMSK                           0x10000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_DEBUG_POLICY_DISABLE_SHFT                                 0x1c
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_DEBUG_POLICY_DISABLE_ENABLE_FVAL                           0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_DEBUG_POLICY_DISABLE_DISABLE_FVAL                          0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RSVD3_BMSK                                           0xc000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RSVD3_SHFT                                                0x1a
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_DEBUG_DISABLE_IN_ROM_BMSK                            0x2000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_DEBUG_DISABLE_IN_ROM_SHFT                                 0x19
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RSVD2_BMSK                                           0x1000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RSVD2_SHFT                                                0x18
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_APPS_HASH_INTEGRITY_CHECK_DISABLE_BMSK                0x800000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_APPS_HASH_INTEGRITY_CHECK_DISABLE_SHFT                    0x17
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_APPS_HASH_INTEGRITY_CHECK_DISABLE_ENABLE_FVAL              0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_APPS_HASH_INTEGRITY_CHECK_DISABLE_DISABLE_FVAL             0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RSVD1B_BMSK                                           0x400000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RSVD1B_SHFT                                               0x16
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SW_ROT_USE_SERIAL_NUM_BMSK                            0x200000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SW_ROT_USE_SERIAL_NUM_SHFT                                0x15
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SW_ROT_USE_SERIAL_NUM_USE_OEM_ID_FVAL                      0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SW_ROT_USE_SERIAL_NUM_USE_SERIAL_NUM_FVAL                  0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_DISABLE_ROT_TRANSFER_BMSK                             0x100000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_DISABLE_ROT_TRANSFER_SHFT                                 0x14
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_DISABLE_ROT_TRANSFER_ENABLE_FVAL                           0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_DISABLE_ROT_TRANSFER_DISABLE_FVAL                          0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_IMAGE_ENCRYPTION_ENABLE_BMSK                           0x80000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_IMAGE_ENCRYPTION_ENABLE_SHFT                              0x13
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ROOT_CERT_TOTAL_NUM_BMSK                               0x60000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ROOT_CERT_TOTAL_NUM_SHFT                                  0x11
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RSVD1_BMSK                                             0x10000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RSVD1_SHFT                                                0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_PBL_LOG_DISABLE_BMSK                                    0x8000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_PBL_LOG_DISABLE_SHFT                                       0xf
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_WDOG_EN_BMSK                                            0x4000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_WDOG_EN_SHFT                                               0xe
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_WDOG_EN_USE_GPIO_FVAL                                      0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_WDOG_EN_IGNORE_GPIO_ENABLE_WDOG_FVAL                       0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_PBL_FDL_TIMEOUT_RESET_FEATURE_ENABLE_BMSK               0x2000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_PBL_FDL_TIMEOUT_RESET_FEATURE_ENABLE_SHFT                  0xd
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_PBL_FDL_TIMEOUT_RESET_FEATURE_ENABLE_DISABLE_EDL_RESET_FVAL        0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_PBL_FDL_TIMEOUT_RESET_FEATURE_ENABLE_ENABLE_EDL_RESET_FVAL        0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SW_FUSE_PROG_DISABLE_BMSK                               0x1000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SW_FUSE_PROG_DISABLE_SHFT                                  0xc
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RSVD0_BMSK                                               0xf00
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_RSVD0_SHFT                                                 0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FAST_BOOT_BMSK                                            0xe0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FAST_BOOT_SHFT                                             0x5
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FAST_BOOT_DEFAULT_FVAL                                     0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_BMSK                                  0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_SHFT                                   0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_ENUM_19_2_MHZ_FVAL                     0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_SDCC_MCLK_BOOT_FREQ_ENUM_25_MHZ_FVAL                       0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FORCE_USB_BOOT_DISABLE_BMSK                                0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FORCE_USB_BOOT_DISABLE_SHFT                                0x3
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FORCE_USB_BOOT_DISABLE_USE_FORCE_USB_BOOT_GPIO_TO_FORCE_BOOT_FROM_USB_FVAL        0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FORCE_USB_BOOT_DISABLE_NOT_USE_FORCE_USB_BOOT_PIN_FVAL        0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FORCE_DLOAD_DISABLE_BMSK                                   0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FORCE_DLOAD_DISABLE_SHFT                                   0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FORCE_DLOAD_DISABLE_USE_FORCE_USB_BOOT_GPIO_TO_FORCE_BOOT_FROM_USB_FVAL        0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_FORCE_DLOAD_DISABLE_NOT_USE_FORCE_USB_BOOT_PIN_FVAL        0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ENUM_TIMEOUT_BMSK                                          0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ENUM_TIMEOUT_SHFT                                          0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ENUM_TIMEOUT_TIMEOUT_DISABLED_FVAL                         0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_ENUM_TIMEOUT_TIMEOUT_ENABLED_90S_FVAL                      0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_BMSK                                       0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_SHFT                                       0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_DOWNLOADER_ENABLED_FVAL                    0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_LSB_E_DLOAD_DISABLE_DOWNLOADER_DISABLED_FVAL                   0x1

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000418c)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RSVD0_BMSK                                          0xfffff000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_RSVD0_SHFT                                                 0xc
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_NAND_XFER_PARAM_BMSK                                     0x800
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_NAND_XFER_PARAM_SHFT                                       0xb
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_MISC2_DEBUG_DISABLE_BMSK                          0x400
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_MISC2_DEBUG_DISABLE_SHFT                            0xa
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_MISC2_DEBUG_DISABLE_ENABLE_FVAL                     0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_MISC2_DEBUG_DISABLE_DISABLE_FVAL                    0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_MISC1_DEBUG_DISABLE_BMSK                          0x200
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_MISC1_DEBUG_DISABLE_SHFT                            0x9
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_MISC1_DEBUG_DISABLE_ENABLE_FVAL                     0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_MISC1_DEBUG_DISABLE_DISABLE_FVAL                    0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_MISC_DEBUG_DISABLE_BMSK                           0x100
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_MISC_DEBUG_DISABLE_SHFT                             0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_MISC_DEBUG_DISABLE_ENABLE_FVAL                      0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_MISC_DEBUG_DISABLE_DISABLE_FVAL                     0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_NIDEN_DISABLE_BMSK                                   0x80
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_NIDEN_DISABLE_SHFT                                    0x7
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_NIDEN_DISABLE_ENABLE_FVAL                             0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_NIDEN_DISABLE_DISABLE_FVAL                            0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_DBGEN_DISABLE_BMSK                                   0x40
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_DBGEN_DISABLE_SHFT                                    0x6
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_DBGEN_DISABLE_ENABLE_FVAL                             0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_APPS_DBGEN_DISABLE_DISABLE_FVAL                            0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_NS_NIDEN_DISABLE_BMSK                              0x20
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_NS_NIDEN_DISABLE_SHFT                               0x5
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_NS_NIDEN_DISABLE_ENABLE_FVAL                        0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_NS_NIDEN_DISABLE_DISABLE_FVAL                       0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_NS_DBGEN_DISABLE_BMSK                              0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_NS_DBGEN_DISABLE_SHFT                               0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_NS_DBGEN_DISABLE_ENABLE_FVAL                        0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_NS_DBGEN_DISABLE_DISABLE_FVAL                       0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_CP_NIDEN_DISABLE_BMSK                               0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_CP_NIDEN_DISABLE_SHFT                               0x3
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_CP_NIDEN_DISABLE_ENABLE_FVAL                        0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_CP_NIDEN_DISABLE_DISABLE_FVAL                       0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_CP_DBGEN_DISABLE_BMSK                               0x4
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_CP_DBGEN_DISABLE_SHFT                               0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_CP_DBGEN_DISABLE_ENABLE_FVAL                        0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_CP_DBGEN_DISABLE_DISABLE_FVAL                       0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_MSS_NIDEN_DISABLE_BMSK                              0x2
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_MSS_NIDEN_DISABLE_SHFT                              0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_MSS_NIDEN_DISABLE_ENABLE_FVAL                       0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_MSS_NIDEN_DISABLE_DISABLE_FVAL                      0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_MSS_DBGEN_DISABLE_BMSK                              0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_MSS_DBGEN_DISABLE_SHFT                              0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_MSS_DBGEN_DISABLE_ENABLE_FVAL                       0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW0_MSB_SHARED_MSS_DBGEN_DISABLE_DISABLE_FVAL                      0x1

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ADDR                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004190)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_DISABLE_RSA_BMSK                                    0x80000000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_DISABLE_RSA_SHFT                                          0x1f
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_RSVD2_BMSK                                          0x7fff8000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_RSVD2_SHFT                                                 0xf
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG27_SECURE_BMSK                             0x4000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG27_SECURE_SHFT                                0xe
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG27_SECURE_NOT_SECURE_FVAL                     0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG27_SECURE_SECURE_FVAL                         0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG26_SECURE_BMSK                             0x2000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG26_SECURE_SHFT                                0xd
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG26_SECURE_NOT_SECURE_FVAL                     0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG26_SECURE_SECURE_FVAL                         0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG25_SECURE_BMSK                             0x1000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG25_SECURE_SHFT                                0xc
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG25_SECURE_NOT_SECURE_FVAL                     0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG25_SECURE_SECURE_FVAL                         0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG24_SECURE_BMSK                              0x800
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG24_SECURE_SHFT                                0xb
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG24_SECURE_NOT_SECURE_FVAL                     0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG24_SECURE_SECURE_FVAL                         0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG23_SECURE_BMSK                              0x400
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG23_SECURE_SHFT                                0xa
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG23_SECURE_NOT_SECURE_FVAL                     0x0
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_OEM_SPARE_REG23_SECURE_SECURE_FVAL                         0x1
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_RSVD1_BMSK                                               0x3f8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_RSVD1_SHFT                                                 0x3
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_RSVD0_BMSK                                                 0x7
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_LSB_RSVD0_SHFT                                                 0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_ADDR                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004194)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_OEM_PRODUCT_ID_BMSK                                 0xffff0000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_OEM_PRODUCT_ID_SHFT                                       0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_OEM_HW_ID_BMSK                                          0xffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW1_MSB_OEM_HW_ID_SHFT                                             0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_ADDR                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004198)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_PERIPH_VID_BMSK                                     0xffff0000
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_PERIPH_VID_SHFT                                           0x10
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_PERIPH_PID_BMSK                                         0xffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_LSB_PERIPH_PID_SHFT                                            0x0

#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_ADDR                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000419c)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_RSVD0_BMSK                                          0xffffff00
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_RSVD0_SHFT                                                 0x8
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_ANTI_ROLLBACK_FEATURE_EN_BMSK                             0xff
#define HWIO_QFPROM_CORR_OEM_CONFIG_ROW2_MSB_ANTI_ROLLBACK_FEATURE_EN_SHFT                              0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041a0)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_DISPLAY_RESOLUTION0_LIMITER_1_BMSK                 0x80000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_DISPLAY_RESOLUTION0_LIMITER_1_SHFT                       0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_RSVD2_BMSK                                         0x40000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_RSVD2_SHFT                                               0x1e
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_DSI_0_DISABLE_BMSK                                 0x20000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_DSI_0_DISABLE_SHFT                                       0x1d
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_DISPLAY_RESOLUTION0_LIMITER_0_BMSK                 0x10000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_DISPLAY_RESOLUTION0_LIMITER_0_SHFT                       0x1c
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_NIDNT_DISABLE_BMSK                                  0x8000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_NIDNT_DISABLE_SHFT                                       0x1b
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_RSVD1_BMSK                                          0x7e00000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_RSVD1_SHFT                                               0x15
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SPDIF_0_1_SW_DISABLE_BMSK                            0x100000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SPDIF_0_1_SW_DISABLE_SHFT                                0x14
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SPDIF_0_1_SW_DISABLE_ENABLE_FVAL                          0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_SPDIF_0_1_SW_DISABLE_DISABLE_FVAL                         0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_I2S3_DISABLE_BMSK                                     0x80000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_I2S3_DISABLE_SHFT                                        0x13
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_I2S3_DISABLE_ENABLE_FVAL                                  0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_I2S3_DISABLE_DISABLE_FVAL                                 0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_I2S2_DISABLE_BMSK                                     0x40000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_I2S2_DISABLE_SHFT                                        0x12
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_I2S2_DISABLE_ENABLE_FVAL                                  0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_I2S2_DISABLE_DISABLE_FVAL                                 0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_I2S1_DISABLE_BMSK                                     0x20000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_I2S1_DISABLE_SHFT                                        0x11
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_I2S1_DISABLE_ENABLE_FVAL                                  0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_I2S1_DISABLE_DISABLE_FVAL                                 0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_I2S0_DISABLE_BMSK                                     0x10000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_I2S0_DISABLE_SHFT                                        0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_I2S0_DISABLE_ENABLE_FVAL                                  0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_I2S0_DISABLE_DISABLE_FVAL                                 0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_USB3_DISABLE_BMSK                                      0x8000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_USB3_DISABLE_SHFT                                         0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_USB3_DISABLE_ENABLE_FVAL                                  0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_USB3_DISABLE_DISABLE_FVAL                                 0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_PCIE_DISABLE_BMSK                                      0x4000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_PCIE_DISABLE_SHFT                                         0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_PCIE_DISABLE_ENABLE_FVAL                                  0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_PCIE_DISABLE_DISABLE_FVAL                                 0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_HDMI_DISABLE_BMSK                                      0x2000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_HDMI_DISABLE_SHFT                                         0xd
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_HDMI_DISABLE_ENABLE_FVAL                                  0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_HDMI_DISABLE_DISABLE_FVAL                                 0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_TURING_Q6_DISABLE_BMSK                                 0x1000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_TURING_Q6_DISABLE_SHFT                                    0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_TURING_Q6_DISABLE_ENABLE_FVAL                             0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_TURING_Q6_DISABLE_DISABLE_FVAL                            0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_TURING_Q6SS_HVX_DISABLE_BMSK                            0x800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_TURING_Q6SS_HVX_DISABLE_SHFT                              0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_TURING_Q6SS_HVX_DISABLE_ENABLE_FVAL                       0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_TURING_Q6SS_HVX_DISABLE_DISABLE_FVAL                      0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_EMAC_DISABLE_BMSK                                       0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_EMAC_DISABLE_SHFT                                         0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_EMAC_DISABLE_ENABLE_FVAL                                  0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_EMAC_DISABLE_DISABLE_FVAL                                 0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_RSVD0_BMSK                                              0x380
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_RSVD0_SHFT                                                0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_DOLBY_BIT_BMSK                                           0x40
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_DOLBY_BIT_SHFT                                            0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_DOLBY_BIT_DISABLE_FVAL                                    0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_DOLBY_BIT_ENABLE_FVAL                                     0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_LPASS_SVA_DISABLE_BMSK                                   0x20
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_LPASS_SVA_DISABLE_SHFT                                    0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_LPASS_SVA_DISABLE_ENABLE_FVAL                             0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_LPASS_SVA_DISABLE_DISABLE_FVAL                            0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_LPASS_Q6_TCM_BOOT_DISABLE_BMSK                           0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_LPASS_Q6_TCM_BOOT_DISABLE_SHFT                            0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_LPASS_Q6_TCM_BOOT_DISABLE_ENABLE_FVAL                     0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_LPASS_Q6_TCM_BOOT_DISABLE_DISABLE_FVAL                    0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ENABLE_DEVICE_IN_TEST_MODE_BMSK                           0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ENABLE_DEVICE_IN_TEST_MODE_SHFT                           0x3
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ARM_CE_DISABLE_USAGE_BMSK                                 0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_ARM_CE_DISABLE_USAGE_SHFT                                 0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_CE_BAM_DISABLE_BMSK                                       0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_CE_BAM_DISABLE_SHFT                                       0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_CE_BAM_DISABLE_ENABLE_FVAL                                0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_CE_BAM_DISABLE_DISABLE_FVAL                               0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_XBL_SEC_AUTH_DISABLE_BMSK                                 0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_XBL_SEC_AUTH_DISABLE_SHFT                                 0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_XBL_SEC_AUTH_DISABLE_ENABLE_FVAL                          0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_LSB_XBL_SEC_AUTH_DISABLE_DISABLE_FVAL                         0x1

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041a4)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RSVD5_BMSK                                         0x80000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RSVD5_SHFT                                               0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_CM_FEAT_CONFIG_DISABLE_BMSK                        0x40000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_CM_FEAT_CONFIG_DISABLE_SHFT                              0x1e
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RSVD4_BMSK                                         0x3f000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RSVD4_SHFT                                               0x18
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY1_BMSK                         0x800000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY1_SHFT                             0x17
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY1_ICE_ENGINE_USES_SW_KEY_FVAL        0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY1_ICE_ENGINE_USES_HW_KEY_FVAL        0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY0_BMSK                         0x400000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY0_SHFT                             0x16
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY0_ICE_ENGINE_USES_SW_KEY_FVAL        0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY0_ICE_ENGINE_USES_HW_KEY_FVAL        0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_DISABLE_BMSK                               0x200000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_DISABLE_SHFT                                   0x15
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_DISABLE_ENABLE_FVAL                             0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_DISABLE_DISABLE_FVAL                            0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RSVD3_BMSK                                           0x1e0000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RSVD3_SHFT                                               0x11
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_CLOCKCFG_BMSK                                    0x18000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_CLOCKCFG_SHFT                                        0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RSVD2_BMSK                                             0x7800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RSVD2_SHFT                                                0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_GFX_FRQ_LIM_FUSE_BMSK                                   0x600
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_GFX_FRQ_LIM_FUSE_SHFT                                     0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_DDR_FRQ_LIM_FUSE_BMSK                                   0x180
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_DDR_FRQ_LIM_FUSE_SHFT                                     0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RSVD1_BMSK                                               0x40
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RSVD1_SHFT                                                0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_CFG_BMSK                                   0x20
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_CFG_SHFT                                    0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_CFG_DEFAULT_POWER_ON_BRINGUP_DISABLE_BOOT_FSM_FVAL        0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_CFG_BOOT_FSM_BASED_BRING_UP_FVAL            0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_DELAY_BMSK                                 0x18
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_DELAY_SHFT                                  0x3
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_DELAY_GDHS_CNT_128_CLAMP_RESET_128_L2_RESET_256_FVAL        0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_DELAY_GDHS_CNT_32_CLAMP_RESET_48_L2_RESET_128_FVAL        0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_DELAY_GDHS_CNT_16_CLAMP_RESET_23_L2_RESET_103_FVAL        0x3
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_DISPLAY_RESOLUTION1_LIMITER_BMSK                          0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_DISPLAY_RESOLUTION1_LIMITER_SHFT                          0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RSVD0_BMSK                                                0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW0_MSB_RSVD0_SHFT                                                0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041a8)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_RSVD0_BMSK                                         0xffe00000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_RSVD0_SHFT                                               0x15
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MISC_DEBUG_DISABLE_BMSK                    0x100000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MISC_DEBUG_DISABLE_SHFT                        0x14
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MISC_DEBUG_DISABLE_ENABLE_FVAL                  0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MISC_DEBUG_DISABLE_DISABLE_FVAL                 0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_BMSK                            0x80000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_SHFT                               0x13
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_ENABLE_FVAL                         0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_DISABLE_FVAL                        0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_BMSK                            0x40000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_SHFT                               0x12
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_ENABLE_FVAL                         0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_DISABLE_FVAL                        0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_NIDEN_DISABLE_BMSK                       0x20000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_NIDEN_DISABLE_SHFT                          0x11
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_NIDEN_DISABLE_ENABLE_FVAL                    0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_NIDEN_DISABLE_DISABLE_FVAL                   0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_DBGEN_DISABLE_BMSK                       0x10000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_DBGEN_DISABLE_SHFT                          0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_DBGEN_DISABLE_ENABLE_FVAL                    0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_DBGEN_DISABLE_DISABLE_FVAL                   0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_NIDEN_DISABLE_BMSK                        0x8000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_NIDEN_DISABLE_SHFT                           0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_NIDEN_DISABLE_ENABLE_FVAL                    0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_NIDEN_DISABLE_DISABLE_FVAL                   0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_DBGEN_DISABLE_BMSK                        0x4000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_DBGEN_DISABLE_SHFT                           0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_DBGEN_DISABLE_ENABLE_FVAL                    0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_DBGEN_DISABLE_DISABLE_FVAL                   0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_NIDEN_DISABLE_BMSK                       0x2000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_NIDEN_DISABLE_SHFT                          0xd
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_NIDEN_DISABLE_ENABLE_FVAL                   0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_NIDEN_DISABLE_DISABLE_FVAL                  0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_DBGEN_DISABLE_BMSK                       0x1000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_DBGEN_DISABLE_SHFT                          0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_DBGEN_DISABLE_ENABLE_FVAL                   0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_DBGEN_DISABLE_DISABLE_FVAL                  0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPNIDEN_DISABLE_BMSK                     0x800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPNIDEN_DISABLE_SHFT                       0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPNIDEN_DISABLE_ENABLE_FVAL                0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPNIDEN_DISABLE_DISABLE_FVAL               0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPIDEN_DISABLE_BMSK                      0x400
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPIDEN_DISABLE_SHFT                        0xa
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPIDEN_DISABLE_ENABLE_FVAL                 0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPIDEN_DISABLE_DISABLE_FVAL                0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_NIDEN_DISABLE_BMSK                           0x200
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_NIDEN_DISABLE_SHFT                             0x9
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_NIDEN_DISABLE_ENABLE_FVAL                      0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_NIDEN_DISABLE_DISABLE_FVAL                     0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_DBGEN_DISABLE_BMSK                           0x100
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_DBGEN_DISABLE_SHFT                             0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_DBGEN_DISABLE_ENABLE_FVAL                      0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_DBGEN_DISABLE_DISABLE_FVAL                     0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_NIDEN_DISABLE_BMSK                            0x80
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_NIDEN_DISABLE_SHFT                             0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_NIDEN_DISABLE_ENABLE_FVAL                      0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_NIDEN_DISABLE_DISABLE_FVAL                     0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_DBGEN_DISABLE_BMSK                            0x40
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_DBGEN_DISABLE_SHFT                             0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_DBGEN_DISABLE_ENABLE_FVAL                      0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_DBGEN_DISABLE_DISABLE_FVAL                     0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_NIDEN_DISABLE_BMSK                           0x20
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_NIDEN_DISABLE_SHFT                            0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_NIDEN_DISABLE_ENABLE_FVAL                     0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_NIDEN_DISABLE_DISABLE_FVAL                    0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_DBGEN_DISABLE_BMSK                           0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_DBGEN_DISABLE_SHFT                            0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_DBGEN_DISABLE_ENABLE_FVAL                     0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_DBGEN_DISABLE_DISABLE_FVAL                    0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPNIDEN_DISABLE_BMSK                         0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPNIDEN_DISABLE_SHFT                         0x3
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPNIDEN_DISABLE_ENABLE_FVAL                  0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPNIDEN_DISABLE_DISABLE_FVAL                 0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPIDEN_DISABLE_BMSK                          0x4
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPIDEN_DISABLE_SHFT                          0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPIDEN_DISABLE_ENABLE_FVAL                   0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPIDEN_DISABLE_DISABLE_FVAL                  0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_BMSK                                      0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_SHFT                                      0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_ENABLE_FVAL                               0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_DISABLE_FVAL                              0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_BMSK                                          0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_SHFT                                          0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_ENABLE_FVAL                                   0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_DISABLE_FVAL                                  0x1

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041ac)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_BMSK                        0xfe000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_SHFT                              0x19
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_BMSK                              0x1ffff80
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_SHFT                                    0x7
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_RSVD0_BMSK                                               0x40
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_RSVD0_SHFT                                                0x6
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_APPS_PBL_PATCH_VERSION_BMSK                              0x3f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW1_MSB_APPS_PBL_PATCH_VERSION_SHFT                               0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041b0)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_TAP_GEN_SPARE_INSTR_DISABLE_31_0_BMSK              0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_LSB_TAP_GEN_SPARE_INSTR_DISABLE_31_0_SHFT                     0x0

#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041b4)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_RMSK                                               0xffffffff
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SPARE0_BMSK                                        0x80000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_SPARE0_SHFT                                              0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_A53SS_DEFEATURE_L2CACHE_1_BMSK                     0x40000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_A53SS_DEFEATURE_L2CACHE_1_SHFT                           0x1e
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_PBL_QSEE_BOOT_FLOW_DISABLE_BMSK                    0x20000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_PBL_QSEE_BOOT_FLOW_DISABLE_SHFT                          0x1d
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_4_BMSK                           0x10000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_4_SHFT                                 0x1c
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_TRIGGER_DISABLE_BMSK                      0x8000000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_TRIGGER_DISABLE_SHFT                           0x1b
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_FOUNDRY_ID_BMSK                                     0x7800000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_FOUNDRY_ID_SHFT                                          0x17
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_BMSK                               0x780000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_SHFT                                   0x13
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_7_BMSK                           0x40000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_7_SHFT                              0x12
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_6_BMSK                           0x20000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_6_SHFT                              0x11
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_5_BMSK                           0x10000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_5_SHFT                              0x10
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_RSVD1_BMSK                                             0x8000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_RSVD1_SHFT                                                0xf
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_AARCH64_ENABLE_BMSK                               0x4000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_AARCH64_ENABLE_SHFT                                  0xe
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_BMSK                               0x3000
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_SHFT                                  0xc
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_ENUM_800_MHZ_FVAL                     0x0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_ENUM_400_MHZ_FVAL                     0x1
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_ENUM_200_MHZ_FVAL                     0x2
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_XO_FVAL                               0x3
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FROM_ROM_BMSK                                 0x800
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FROM_ROM_SHFT                                   0xb
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_RSVD0_BMSK                                              0x700
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_RSVD0_SHFT                                                0x8
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_BOOT_ROM_PATCH_DISABLE_BMSK                              0xe0
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_BOOT_ROM_PATCH_DISABLE_SHFT                               0x5
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_TAP_GEN_SPARE_INSTR_DISABLE_36_32_BMSK                   0x1f
#define HWIO_QFPROM_CORR_FEAT_CONFIG_ROW2_MSB_TAP_GEN_SPARE_INSTR_DISABLE_36_32_SHFT                    0x0

#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_LSB_ADDR(n)                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041b8 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_LSB_MAXn                                                           6
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH0_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_PK_HASH0_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH0_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_LSB_HASH_DATA0_BMSK                                       0xffffffff
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_LSB_HASH_DATA0_SHFT                                              0x0

#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_MSB_ADDR(n)                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041bc + 0x8 * (n))
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_MSB_RMSK                                                    0xffffff
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_MSB_MAXn                                                           6
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH0_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_PK_HASH0_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PK_HASH0_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_MSB_HASH_DATA1_BMSK                                         0xffffff
#define HWIO_QFPROM_CORR_PK_HASH0_ROWn_MSB_HASH_DATA1_SHFT                                              0x0

#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041f0)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B31_BMSK                                        0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B31_SHFT                                              0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B30_BMSK                                        0x40000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B30_SHFT                                              0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B29_BMSK                                        0x20000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B29_SHFT                                              0x1d
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B28_BMSK                                        0x10000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B28_SHFT                                              0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B27_BMSK                                         0x8000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B27_SHFT                                              0x1b
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B26_BMSK                                         0x4000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B26_SHFT                                              0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B25_BMSK                                         0x2000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B25_SHFT                                              0x19
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B24_BMSK                                         0x1000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B24_SHFT                                              0x18
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B23_BMSK                                          0x800000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B23_SHFT                                              0x17
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B22_BMSK                                          0x400000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B22_SHFT                                              0x16
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B21_BMSK                                          0x200000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B21_SHFT                                              0x15
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B20_BMSK                                          0x100000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B20_SHFT                                              0x14
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B19_BMSK                                           0x80000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B19_SHFT                                              0x13
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B18_BMSK                                           0x40000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B18_SHFT                                              0x12
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B17_BMSK                                           0x20000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B17_SHFT                                              0x11
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B16_BMSK                                           0x10000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B16_SHFT                                              0x10
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B15_BMSK                                            0x8000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B15_SHFT                                               0xf
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B14_BMSK                                            0x4000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B14_SHFT                                               0xe
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B13_BMSK                                            0x2000
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_RSVD_R62_B13_SHFT                                               0xd
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ACC_BMSK                                                     0x1c00
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_ACC_SHFT                                                        0xa
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_IDDQ_REV_BMSK                                                 0x300
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_IDDQ_REV_SHFT                                                   0x8
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_DSI_PHY_FUSE_BMSK                                              0xf0
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_DSI_PHY_FUSE_SHFT                                               0x4
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_DSI_PHY_PLL_EFUSE_BMSK                                          0xf
#define HWIO_QFPROM_CORR_CALIB_ROW0_LSB_DSI_PHY_PLL_EFUSE_SHFT                                          0x0

#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041f4)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B63_BMSK                                        0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B63_SHFT                                              0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B62_BMSK                                        0x40000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B62_SHFT                                              0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B61_BMSK                                        0x20000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B61_SHFT                                              0x1d
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B60_BMSK                                        0x10000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B60_SHFT                                              0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B59_BMSK                                         0x8000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B59_SHFT                                              0x1b
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B58_BMSK                                         0x4000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B58_SHFT                                              0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B57_BMSK                                         0x2000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B57_SHFT                                              0x19
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B56_BMSK                                         0x1000000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B56_SHFT                                              0x18
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B55_BMSK                                          0x800000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B55_SHFT                                              0x17
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B54_BMSK                                          0x400000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B54_SHFT                                              0x16
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B53_BMSK                                          0x200000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B53_SHFT                                              0x15
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B52_BMSK                                          0x100000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B52_SHFT                                              0x14
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B51_BMSK                                           0x80000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B51_SHFT                                              0x13
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B50_BMSK                                           0x40000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B50_SHFT                                              0x12
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B49_BMSK                                           0x20000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B49_SHFT                                              0x11
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B48_BMSK                                           0x10000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B48_SHFT                                              0x10
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B47_BMSK                                            0x8000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B47_SHFT                                               0xf
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B46_BMSK                                            0x4000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B46_SHFT                                               0xe
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B45_BMSK                                            0x2000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B45_SHFT                                               0xd
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B44_BMSK                                            0x1000
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B44_SHFT                                               0xc
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B43_BMSK                                             0x800
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B43_SHFT                                               0xb
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B42_BMSK                                             0x400
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B42_SHFT                                               0xa
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B41_BMSK                                             0x200
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B41_SHFT                                               0x9
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B40_BMSK                                             0x100
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B40_SHFT                                               0x8
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B39_BMSK                                              0x80
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B39_SHFT                                               0x7
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B38_BMSK                                              0x40
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B38_SHFT                                               0x6
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B37_BMSK                                              0x20
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B37_SHFT                                               0x5
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B36_BMSK                                              0x10
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B36_SHFT                                               0x4
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B35_BMSK                                               0x8
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B35_SHFT                                               0x3
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B34_BMSK                                               0x4
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B34_SHFT                                               0x2
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B33_BMSK                                               0x2
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B33_SHFT                                               0x1
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B32_BMSK                                               0x1
#define HWIO_QFPROM_CORR_CALIB_ROW0_MSB_RSVD_R62_B32_SHFT                                               0x0

#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041f8)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_TSENS2_POINT2_1_0_BMSK                                   0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_TSENS2_POINT2_1_0_SHFT                                         0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_TSENS2_POINT1_BMSK                                       0x3f000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_TSENS2_POINT1_SHFT                                             0x18
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_TSENS1_POINT2_BMSK                                         0xfc0000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_TSENS1_POINT2_SHFT                                             0x12
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_TSENS1_POINT1_BMSK                                          0x3f000
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_TSENS1_POINT1_SHFT                                              0xc
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_TSENS0_POINT2_BMSK                                            0xfc0
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_TSENS0_POINT2_SHFT                                              0x6
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_TSENS0_POINT1_BMSK                                             0x3f
#define HWIO_QFPROM_CORR_CALIB_ROW1_LSB_TSENS0_POINT1_SHFT                                              0x0

#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x000041fc)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_RSVD_R63_B63_BMSK                                        0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_RSVD_R63_B63_SHFT                                              0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_RSVD_R63_B62_BMSK                                        0x40000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_RSVD_R63_B62_SHFT                                              0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_RSVD_R63_B61_BMSK                                        0x20000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_RSVD_R63_B61_SHFT                                              0x1d
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_RSVD_R63_B60_BMSK                                        0x10000000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_RSVD_R63_B60_SHFT                                              0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_TSENS4_POINT2_BMSK                                        0xfc00000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_TSENS4_POINT2_SHFT                                             0x16
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_TSENS4_POINT1_BMSK                                         0x3f0000
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_TSENS4_POINT1_SHFT                                             0x10
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_TSENS3_POINT2_BMSK                                           0xfc00
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_TSENS3_POINT2_SHFT                                              0xa
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_TSENS3_POINT1_BMSK                                            0x3f0
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_TSENS3_POINT1_SHFT                                              0x4
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_TSENS2_POINT2_5_2_BMSK                                          0xf
#define HWIO_QFPROM_CORR_CALIB_ROW1_MSB_TSENS2_POINT2_5_2_SHFT                                          0x0

#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004200)
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_TSENS7_POINT2_1_0_BMSK                                   0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_TSENS7_POINT2_1_0_SHFT                                         0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_TSENS7_POINT1_BMSK                                       0x3f000000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_TSENS7_POINT1_SHFT                                             0x18
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_TSENS6_POINT2_BMSK                                         0xfc0000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_TSENS6_POINT2_SHFT                                             0x12
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_TSENS6_POINT1_BMSK                                          0x3f000
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_TSENS6_POINT1_SHFT                                              0xc
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_TSENS5_POINT2_BMSK                                            0xfc0
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_TSENS5_POINT2_SHFT                                              0x6
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_TSENS5_POINT1_BMSK                                             0x3f
#define HWIO_QFPROM_CORR_CALIB_ROW2_LSB_TSENS5_POINT1_SHFT                                              0x0

#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004204)
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_RSVD_R64_B63_BMSK                                        0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_RSVD_R64_B63_SHFT                                              0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_RSVD_R64_B62_BMSK                                        0x40000000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_RSVD_R64_B62_SHFT                                              0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_RSVD_R64_B61_BMSK                                        0x20000000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_RSVD_R64_B61_SHFT                                              0x1d
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_RSVD_R64_B60_BMSK                                        0x10000000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_RSVD_R64_B60_SHFT                                              0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_TSENS9_POINT2_BMSK                                        0xfc00000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_TSENS9_POINT2_SHFT                                             0x16
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_TSENS9_POINT1_BMSK                                         0x3f0000
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_TSENS9_POINT1_SHFT                                             0x10
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_TSENS8_POINT2_BMSK                                           0xfc00
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_TSENS8_POINT2_SHFT                                              0xa
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_TSENS8_POINT1_BMSK                                            0x3f0
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_TSENS8_POINT1_SHFT                                              0x4
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_TSENS7_POINT2_5_2_BMSK                                          0xf
#define HWIO_QFPROM_CORR_CALIB_ROW2_MSB_TSENS7_POINT2_5_2_SHFT                                          0x0

#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004208)
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RSVD_R65_B31_BMSK                                        0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RSVD_R65_B31_SHFT                                              0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RSVD_R65_B30_BMSK                                        0x40000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RSVD_R65_B30_SHFT                                              0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RSVD_R65_B29_BMSK                                        0x20000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RSVD_R65_B29_SHFT                                              0x1d
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RSVD_R65_B28_BMSK                                        0x10000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RSVD_R65_B28_SHFT                                              0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RSVD_R65_B27_BMSK                                         0x8000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RSVD_R65_B27_SHFT                                              0x1b
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RSVD_R65_B26_BMSK                                         0x4000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RSVD_R65_B26_SHFT                                              0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RSVD_R65_B25_BMSK                                         0x2000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RSVD_R65_B25_SHFT                                              0x19
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RSVD_R65_B24_BMSK                                         0x1000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RSVD_R65_B24_SHFT                                              0x18
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RSVD_R65_B23_BMSK                                          0x800000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RSVD_R65_B23_SHFT                                              0x17
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RSVD_R65_B22_BMSK                                          0x400000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RSVD_R65_B22_SHFT                                              0x16
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RSVD_R65_B21_BMSK                                          0x200000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RSVD_R65_B21_SHFT                                              0x15
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RSVD_R65_B20_BMSK                                          0x100000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RSVD_R65_B20_SHFT                                              0x14
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RSVD_R65_B19_BMSK                                           0x80000
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_RSVD_R65_B19_SHFT                                              0x13
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_TSENS_BASE1_BMSK                                            0x7f800
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_TSENS_BASE1_SHFT                                                0xb
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_TSENS_BASE0_BMSK                                              0x7f8
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_TSENS_BASE0_SHFT                                                0x3
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_TSENS_CALIB_BMSK                                                0x7
#define HWIO_QFPROM_CORR_CALIB_ROW3_LSB_TSENS_CALIB_SHFT                                                0x0

#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000420c)
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW3_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B63_BMSK                                        0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B63_SHFT                                              0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B62_BMSK                                        0x40000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B62_SHFT                                              0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B61_BMSK                                        0x20000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B61_SHFT                                              0x1d
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B60_BMSK                                        0x10000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B60_SHFT                                              0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B59_BMSK                                         0x8000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B59_SHFT                                              0x1b
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B58_BMSK                                         0x4000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B58_SHFT                                              0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B57_BMSK                                         0x2000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B57_SHFT                                              0x19
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B56_BMSK                                         0x1000000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B56_SHFT                                              0x18
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B55_BMSK                                          0x800000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B55_SHFT                                              0x17
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B54_BMSK                                          0x400000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B54_SHFT                                              0x16
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B53_BMSK                                          0x200000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B53_SHFT                                              0x15
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B52_BMSK                                          0x100000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B52_SHFT                                              0x14
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B51_BMSK                                           0x80000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B51_SHFT                                              0x13
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B50_BMSK                                           0x40000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B50_SHFT                                              0x12
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B49_BMSK                                           0x20000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B49_SHFT                                              0x11
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B48_BMSK                                           0x10000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B48_SHFT                                              0x10
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B47_BMSK                                            0x8000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B47_SHFT                                               0xf
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B46_BMSK                                            0x4000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B46_SHFT                                               0xe
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B45_BMSK                                            0x2000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B45_SHFT                                               0xd
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B44_BMSK                                            0x1000
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B44_SHFT                                               0xc
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B43_BMSK                                             0x800
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B43_SHFT                                               0xb
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B42_BMSK                                             0x400
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B42_SHFT                                               0xa
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B41_BMSK                                             0x200
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B41_SHFT                                               0x9
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B40_BMSK                                             0x100
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B40_SHFT                                               0x8
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B39_BMSK                                              0x80
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B39_SHFT                                               0x7
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B38_BMSK                                              0x40
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B38_SHFT                                               0x6
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B37_BMSK                                              0x20
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B37_SHFT                                               0x5
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B36_BMSK                                              0x10
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B36_SHFT                                               0x4
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B35_BMSK                                               0x8
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B35_SHFT                                               0x3
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B34_BMSK                                               0x4
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B34_SHFT                                               0x2
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B33_BMSK                                               0x2
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B33_SHFT                                               0x1
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B32_BMSK                                               0x1
#define HWIO_QFPROM_CORR_CALIB_ROW3_MSB_RSVD_R65_B32_SHFT                                               0x0

#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004210)
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW4_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW4_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_BIASPLL_VCO_CALIB_CODE_MODE1_LOW_1_0_BMSK                0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_BIASPLL_VCO_CALIB_CODE_MODE1_LOW_1_0_SHFT                      0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_BIASPLL_VCO_CALIB_CODE_MODE1_UP_BMSK                     0x3f000000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_BIASPLL_VCO_CALIB_CODE_MODE1_UP_SHFT                           0x18
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_BIASPLL_VCO_CALIB_CODE_MODE1_NORM_BMSK                     0xfc0000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_BIASPLL_VCO_CALIB_CODE_MODE1_NORM_SHFT                         0x12
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_BIASPLL_VCO_CALIB_CODE_MODE0_LOW_BMSK                       0x3f000
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_BIASPLL_VCO_CALIB_CODE_MODE0_LOW_SHFT                           0xc
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_BIASPLL_VCO_CALIB_CODE_MODE0_UP_BMSK                          0xfc0
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_BIASPLL_VCO_CALIB_CODE_MODE0_UP_SHFT                            0x6
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_BIASPLL_VCO_CALIB_CODE_MODE0_NORM_BMSK                         0x3f
#define HWIO_QFPROM_CORR_CALIB_ROW4_LSB_BIASPLL_VCO_CALIB_CODE_MODE0_NORM_SHFT                          0x0

#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004214)
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW4_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW4_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_BIASPLL_UPHY_LDO_RSV_BMSK                                0xffff8000
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_BIASPLL_UPHY_LDO_RSV_SHFT                                       0xf
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_BIASPLL_LDO_LVL_BMSK                                         0x7800
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_BIASPLL_LDO_LVL_SHFT                                            0xb
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_BIASPLL_LPF_RES_BMSK                                          0x780
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_BIASPLL_LPF_RES_SHFT                                            0x7
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_BIASPLL_CP_SEL_BMSK                                            0x70
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_BIASPLL_CP_SEL_SHFT                                             0x4
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_BIASPLL_VCO_CALIB_CODE_MODE1_LOW_5_2_BMSK                       0xf
#define HWIO_QFPROM_CORR_CALIB_ROW4_MSB_BIASPLL_VCO_CALIB_CODE_MODE1_LOW_5_2_SHFT                       0x0

#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004218)
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW5_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW5_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW5_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR1_TARGETVOL_TURBO_1_0_BMSK                            0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR1_TARGETVOL_TURBO_1_0_SHFT                                  0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR1_TARGETVOL_NOM_BMSK                                  0x3e000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR1_TARGETVOL_NOM_SHFT                                        0x19
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR0_TARGETVOL_SVS_BMSK                                   0x1f00000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR0_TARGETVOL_SVS_SHFT                                        0x14
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR0_TARGETVOL_TURBO_BMSK                                   0xf8000
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR0_TARGETVOL_TURBO_SHFT                                       0xf
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR0_TARGETVOL_NOM_BMSK                                      0x7c00
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR0_TARGETVOL_NOM_SHFT                                         0xa
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_RSVD_R67_B9_BMSK                                              0x200
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_RSVD_R67_B9_SHFT                                                0x9
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_RSVD_R67_B8_BMSK                                              0x100
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_RSVD_R67_B8_SHFT                                                0x8
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_RSVD_R67_B7_BMSK                                               0x80
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_RSVD_R67_B7_SHFT                                                0x7
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_RSVD_R67_B6_BMSK                                               0x40
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_RSVD_R67_B6_SHFT                                                0x6
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR_LOCAL_RC_BMSK                                              0x38
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR_LOCAL_RC_SHFT                                               0x3
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR_GLOBAL_RC_BMSK                                              0x7
#define HWIO_QFPROM_CORR_CALIB_ROW5_LSB_CPR_GLOBAL_RC_SHFT                                              0x0

#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000421c)
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW5_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW5_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW5_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_RSVD_R67_B63_BMSK                                        0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_RSVD_R67_B63_SHFT                                              0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_RSVD_R67_B62_BMSK                                        0x40000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_RSVD_R67_B62_SHFT                                              0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_RSVD_R67_B61_BMSK                                        0x20000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_RSVD_R67_B61_SHFT                                              0x1d
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_RSVD_R67_B60_BMSK                                        0x10000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_RSVD_R67_B60_SHFT                                              0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_RSVD_R67_B59_BMSK                                         0x8000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_RSVD_R67_B59_SHFT                                              0x1b
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_RSVD_R67_B58_BMSK                                         0x4000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_RSVD_R67_B58_SHFT                                              0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_RSVD_R67_B57_BMSK                                         0x2000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_RSVD_R67_B57_SHFT                                              0x19
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_RSVD_R67_B56_BMSK                                         0x1000000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_RSVD_R67_B56_SHFT                                              0x18
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_RSVD_R67_B55_BMSK                                          0x800000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_RSVD_R67_B55_SHFT                                              0x17
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR2_TARGETVOL_SVS_BMSK                                    0x7c0000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR2_TARGETVOL_SVS_SHFT                                        0x12
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR2_TARGETVOL_TURBO_BMSK                                   0x3e000
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR2_TARGETVOL_TURBO_SHFT                                       0xd
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR2_TARGETVOL_NOM_BMSK                                      0x1f00
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR2_TARGETVOL_NOM_SHFT                                         0x8
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR1_TARGETVOL_SVS_BMSK                                        0xf8
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR1_TARGETVOL_SVS_SHFT                                         0x3
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR1_TARGETVOL_TURBO_4_2_BMSK                                   0x7
#define HWIO_QFPROM_CORR_CALIB_ROW5_MSB_CPR1_TARGETVOL_TURBO_4_2_SHFT                                   0x0

#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004220)
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW6_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW6_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RSVD_R68_B31_BMSK                                        0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RSVD_R68_B31_SHFT                                              0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RSVD_R68_B30_BMSK                                        0x40000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RSVD_R68_B30_SHFT                                              0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RSVD_R68_B29_BMSK                                        0x20000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RSVD_R68_B29_SHFT                                              0x1d
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RSVD_R68_B28_BMSK                                        0x10000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RSVD_R68_B28_SHFT                                              0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RSVD_R68_B27_BMSK                                         0x8000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RSVD_R68_B27_SHFT                                              0x1b
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RSVD_R68_B26_BMSK                                         0x4000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RSVD_R68_B26_SHFT                                              0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RSVD_R68_B25_BMSK                                         0x2000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RSVD_R68_B25_SHFT                                              0x19
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RSVD_R68_B24_BMSK                                         0x1000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RSVD_R68_B24_SHFT                                              0x18
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RSVD_R68_B23_BMSK                                          0x800000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RSVD_R68_B23_SHFT                                              0x17
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RSVD_R68_B22_BMSK                                          0x400000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RSVD_R68_B22_SHFT                                              0x16
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RSVD_R68_B21_BMSK                                          0x200000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RSVD_R68_B21_SHFT                                              0x15
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RSVD_R68_B20_BMSK                                          0x100000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_RSVD_R68_B20_SHFT                                              0x14
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CPR4_TARGETVOL_NOM_BMSK                                     0xf8000
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CPR4_TARGETVOL_NOM_SHFT                                         0xf
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CPR3_TARGETVOL_SVS_BMSK                                      0x7c00
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CPR3_TARGETVOL_SVS_SHFT                                         0xa
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CPR3_TARGETVOL_TURBO_BMSK                                     0x3e0
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CPR3_TARGETVOL_TURBO_SHFT                                       0x5
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CPR3_TARGETVOL_NOM_BMSK                                        0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW6_LSB_CPR3_TARGETVOL_NOM_SHFT                                         0x0

#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004224)
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW6_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW6_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B63_BMSK                                        0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B63_SHFT                                              0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B62_BMSK                                        0x40000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B62_SHFT                                              0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B61_BMSK                                        0x20000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B61_SHFT                                              0x1d
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B60_BMSK                                        0x10000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B60_SHFT                                              0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B59_BMSK                                         0x8000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B59_SHFT                                              0x1b
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B58_BMSK                                         0x4000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B58_SHFT                                              0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B57_BMSK                                         0x2000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B57_SHFT                                              0x19
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B56_BMSK                                         0x1000000
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B56_SHFT                                              0x18
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B55_BMSK                                          0x800000
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B55_SHFT                                              0x17
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B54_BMSK                                          0x400000
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B54_SHFT                                              0x16
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B53_BMSK                                          0x200000
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B53_SHFT                                              0x15
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B52_BMSK                                          0x100000
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B52_SHFT                                              0x14
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B51_BMSK                                           0x80000
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B51_SHFT                                              0x13
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B50_BMSK                                           0x40000
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B50_SHFT                                              0x12
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B49_BMSK                                           0x20000
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B49_SHFT                                              0x11
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B48_BMSK                                           0x10000
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B48_SHFT                                              0x10
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B47_BMSK                                            0x8000
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B47_SHFT                                               0xf
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B46_BMSK                                            0x4000
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B46_SHFT                                               0xe
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B45_BMSK                                            0x2000
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B45_SHFT                                               0xd
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B44_BMSK                                            0x1000
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B44_SHFT                                               0xc
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B43_BMSK                                             0x800
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B43_SHFT                                               0xb
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B42_BMSK                                             0x400
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B42_SHFT                                               0xa
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B41_BMSK                                             0x200
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B41_SHFT                                               0x9
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B40_BMSK                                             0x100
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B40_SHFT                                               0x8
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B39_BMSK                                              0x80
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B39_SHFT                                               0x7
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B38_BMSK                                              0x40
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B38_SHFT                                               0x6
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B37_BMSK                                              0x20
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B37_SHFT                                               0x5
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B36_BMSK                                              0x10
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B36_SHFT                                               0x4
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B35_BMSK                                               0x8
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B35_SHFT                                               0x3
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B34_BMSK                                               0x4
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B34_SHFT                                               0x2
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B33_BMSK                                               0x2
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B33_SHFT                                               0x1
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B32_BMSK                                               0x1
#define HWIO_QFPROM_CORR_CALIB_ROW6_MSB_RSVD_R68_B32_SHFT                                               0x0

#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004228)
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW7_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW7_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW7_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR5_LOCALQUOT_NOM_1_0_BMSK                              0xc0000000
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR5_LOCALQUOT_NOM_1_0_SHFT                                    0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR5_TARGETVOL_TURBO_BMSK                                0x3f000000
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR5_TARGETVOL_TURBO_SHFT                                      0x18
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR5_TARGETVOL_NOM_L1_BMSK                                 0xfc0000
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR5_TARGETVOL_NOM_L1_SHFT                                     0x12
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR5_TARGETVOL_NOM_BMSK                                     0x3f000
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR5_TARGETVOL_NOM_SHFT                                         0xc
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_RSVD2_BMSK                                                    0x800
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_RSVD2_SHFT                                                      0xb
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR5_ROSEL_TURBO_BMSK                                         0x700
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR5_ROSEL_TURBO_SHFT                                           0x8
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_RSVD1_BMSK                                                     0x80
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_RSVD1_SHFT                                                      0x7
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR5_ROSEL_NOM_L1_BMSK                                         0x70
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR5_ROSEL_NOM_L1_SHFT                                          0x4
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_RSVD0_BMSK                                                      0x8
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_RSVD0_SHFT                                                      0x3
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR5_ROSEL_NOM_BMSK                                             0x7
#define HWIO_QFPROM_CORR_CALIB_ROW7_LSB_CPR5_ROSEL_NOM_SHFT                                             0x0

#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000422c)
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW7_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW7_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW7_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_RSVD_R69_B63_BMSK                                        0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_RSVD_R69_B63_SHFT                                              0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_RSVD_R69_B62_BMSK                                        0x40000000
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_RSVD_R69_B62_SHFT                                              0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_RSVD_R69_B61_BMSK                                        0x20000000
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_RSVD_R69_B61_SHFT                                              0x1d
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_RSVD_R69_B60_BMSK                                        0x10000000
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_RSVD_R69_B60_SHFT                                              0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_RSVD_R69_B59_BMSK                                         0x8000000
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_RSVD_R69_B59_SHFT                                              0x1b
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_RSVD_R69_B58_BMSK                                         0x4000000
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_RSVD_R69_B58_SHFT                                              0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_RSVD_R69_B57_BMSK                                         0x2000000
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_RSVD_R69_B57_SHFT                                              0x19
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_RSVD_R69_B56_BMSK                                         0x1000000
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_RSVD_R69_B56_SHFT                                              0x18
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_RSVD_R69_B55_BMSK                                          0x800000
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_RSVD_R69_B55_SHFT                                              0x17
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_RSVD_R69_B54_BMSK                                          0x400000
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_RSVD_R69_B54_SHFT                                              0x16
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_CPR5_LOCALQUOT_NOM_L1_BMSK                                 0x3ffc00
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_CPR5_LOCALQUOT_NOM_L1_SHFT                                      0xa
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_CPR5_LOCALQUOT_NOM_11_2_BMSK                                  0x3ff
#define HWIO_QFPROM_CORR_CALIB_ROW7_MSB_CPR5_LOCALQUOT_NOM_11_2_SHFT                                    0x0

#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004230)
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW8_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW8_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW8_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CPR5_QUOT_OFFSET_TURBO_5_0_BMSK                          0xfc000000
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CPR5_QUOT_OFFSET_TURBO_5_0_SHFT                                0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CPR5_QUOT_OFFSET_NOM_L1_BMSK                              0x3f80000
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CPR5_QUOT_OFFSET_NOM_L1_SHFT                                   0x13
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CPR5_QUOT_OFFSET_NOM_BMSK                                   0x7f000
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CPR5_QUOT_OFFSET_NOM_SHFT                                       0xc
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CPR5_LOCALQUOT_TURBO_BMSK                                     0xfff
#define HWIO_QFPROM_CORR_CALIB_ROW8_LSB_CPR5_LOCALQUOT_TURBO_SHFT                                       0x0

#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004234)
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW8_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW8_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RSVD_R70_B63_BMSK                                        0x80000000
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RSVD_R70_B63_SHFT                                              0x1f
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RSVD_R70_B62_BMSK                                        0x40000000
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RSVD_R70_B62_SHFT                                              0x1e
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RSVD_R70_B61_BMSK                                        0x20000000
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RSVD_R70_B61_SHFT                                              0x1d
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RSVD_R70_B60_BMSK                                        0x10000000
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RSVD_R70_B60_SHFT                                              0x1c
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RSVD_R70_B59_BMSK                                         0x8000000
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RSVD_R70_B59_SHFT                                              0x1b
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RSVD_R70_B58_BMSK                                         0x4000000
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RSVD_R70_B58_SHFT                                              0x1a
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RSVD_R70_B57_BMSK                                         0x2000000
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RSVD_R70_B57_SHFT                                              0x19
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RSVD_R70_B56_BMSK                                         0x1000000
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RSVD_R70_B56_SHFT                                              0x18
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RSVD_R70_B55_BMSK                                          0x800000
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RSVD_R70_B55_SHFT                                              0x17
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RSVD_R70_B54_BMSK                                          0x400000
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RSVD_R70_B54_SHFT                                              0x16
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RSVD_R70_B53_BMSK                                          0x200000
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_RSVD_R70_B53_SHFT                                              0x15
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_CPR_RSVD_BMSK                                              0x1ffffe
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_CPR_RSVD_SHFT                                                   0x1
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_CPR5_QUOT_OFFSET_TURBO_6_BMSK                                   0x1
#define HWIO_QFPROM_CORR_CALIB_ROW8_MSB_CPR5_QUOT_OFFSET_TURBO_6_SHFT                                   0x0

#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004238)
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW9_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW9_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW9_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_CORR_CALIB_ROW9_LSB_ADDR,v)
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CORR_CALIB_ROW9_LSB_ADDR,m,v,HWIO_QFPROM_CORR_CALIB_ROW9_LSB_IN)
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_PTE_RSVD_BMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW9_LSB_PTE_RSVD_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000423c)
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_RMSK                                                     0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW9_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW9_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW9_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_CORR_CALIB_ROW9_MSB_ADDR,v)
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CORR_CALIB_ROW9_MSB_ADDR,m,v,HWIO_QFPROM_CORR_CALIB_ROW9_MSB_IN)
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_PTE_RSVD_BMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW9_MSB_PTE_RSVD_SHFT                                                   0x0

#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004240)
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW10_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW10_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW10_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_CORR_CALIB_ROW10_LSB_ADDR,v)
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CORR_CALIB_ROW10_LSB_ADDR,m,v,HWIO_QFPROM_CORR_CALIB_ROW10_LSB_IN)
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_PTE_RSVD_BMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW10_LSB_PTE_RSVD_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004244)
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW10_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW10_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW10_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_CORR_CALIB_ROW10_MSB_ADDR,v)
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CORR_CALIB_ROW10_MSB_ADDR,m,v,HWIO_QFPROM_CORR_CALIB_ROW10_MSB_IN)
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_PTE_RSVD_BMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW10_MSB_PTE_RSVD_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004248)
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW11_LSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW11_LSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW11_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_CORR_CALIB_ROW11_LSB_ADDR,v)
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CORR_CALIB_ROW11_LSB_ADDR,m,v,HWIO_QFPROM_CORR_CALIB_ROW11_LSB_IN)
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_PTE_RSVD_BMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW11_LSB_PTE_RSVD_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_ADDR                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000424c)
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_RMSK                                                    0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW11_MSB_ADDR, HWIO_QFPROM_CORR_CALIB_ROW11_MSB_RMSK)
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CALIB_ROW11_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_OUT(v)      \
        out_dword(HWIO_QFPROM_CORR_CALIB_ROW11_MSB_ADDR,v)
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CORR_CALIB_ROW11_MSB_ADDR,m,v,HWIO_QFPROM_CORR_CALIB_ROW11_MSB_IN)
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_PTE_RSVD_BMSK                                           0xffffffff
#define HWIO_QFPROM_CORR_CALIB_ROW11_MSB_PTE_RSVD_SHFT                                                  0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_ADDR(n)                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004250 + 0x8 * (n))
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_MAXn                                                        15
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_REDUN_DATA_BMSK                                     0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_LSB_REDUN_DATA_SHFT                                            0x0

#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_ADDR(n)                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004254 + 0x8 * (n))
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_RMSK                                                0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_MAXn                                                        15
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_REDUN_DATA_BMSK                                     0xffffffff
#define HWIO_QFPROM_CORR_MEM_CONFIG_ROWn_MSB_REDUN_DATA_SHFT                                            0x0

#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042d0)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_DISPLAY_RESOLUTION0_LIMITER_1_BMSK              0x80000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_DISPLAY_RESOLUTION0_LIMITER_1_SHFT                    0x1f
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_RSVD2_BMSK                                      0x40000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_RSVD2_SHFT                                            0x1e
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_DSI_0_DISABLE_BMSK                              0x20000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_DSI_0_DISABLE_SHFT                                    0x1d
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_DISPLAY_RESOLUTION0_LIMITER_0_BMSK              0x10000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_DISPLAY_RESOLUTION0_LIMITER_0_SHFT                    0x1c
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_NIDNT_DISABLE_BMSK                               0x8000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_NIDNT_DISABLE_SHFT                                    0x1b
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_RSVD1_BMSK                                       0x7e00000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_RSVD1_SHFT                                            0x15
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_SPDIF_0_1_SW_DISABLE_BMSK                         0x100000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_SPDIF_0_1_SW_DISABLE_SHFT                             0x14
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_SPDIF_0_1_SW_DISABLE_ENABLE_FVAL                       0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_SPDIF_0_1_SW_DISABLE_DISABLE_FVAL                      0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_I2S3_DISABLE_BMSK                                  0x80000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_I2S3_DISABLE_SHFT                                     0x13
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_I2S3_DISABLE_ENABLE_FVAL                               0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_I2S3_DISABLE_DISABLE_FVAL                              0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_I2S2_DISABLE_BMSK                                  0x40000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_I2S2_DISABLE_SHFT                                     0x12
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_I2S2_DISABLE_ENABLE_FVAL                               0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_I2S2_DISABLE_DISABLE_FVAL                              0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_I2S1_DISABLE_BMSK                                  0x20000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_I2S1_DISABLE_SHFT                                     0x11
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_I2S1_DISABLE_ENABLE_FVAL                               0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_I2S1_DISABLE_DISABLE_FVAL                              0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_I2S0_DISABLE_BMSK                                  0x10000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_I2S0_DISABLE_SHFT                                     0x10
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_I2S0_DISABLE_ENABLE_FVAL                               0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_I2S0_DISABLE_DISABLE_FVAL                              0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_USB3_DISABLE_BMSK                                   0x8000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_USB3_DISABLE_SHFT                                      0xf
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_USB3_DISABLE_ENABLE_FVAL                               0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_USB3_DISABLE_DISABLE_FVAL                              0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_PCIE_DISABLE_BMSK                                   0x4000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_PCIE_DISABLE_SHFT                                      0xe
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_PCIE_DISABLE_ENABLE_FVAL                               0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_PCIE_DISABLE_DISABLE_FVAL                              0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_HDMI_DISABLE_BMSK                                   0x2000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_HDMI_DISABLE_SHFT                                      0xd
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_HDMI_DISABLE_ENABLE_FVAL                               0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_HDMI_DISABLE_DISABLE_FVAL                              0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_TURING_Q6_DISABLE_BMSK                              0x1000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_TURING_Q6_DISABLE_SHFT                                 0xc
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_TURING_Q6_DISABLE_ENABLE_FVAL                          0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_TURING_Q6_DISABLE_DISABLE_FVAL                         0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_TURING_Q6SS_HVX_DISABLE_BMSK                         0x800
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_TURING_Q6SS_HVX_DISABLE_SHFT                           0xb
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_TURING_Q6SS_HVX_DISABLE_ENABLE_FVAL                    0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_TURING_Q6SS_HVX_DISABLE_DISABLE_FVAL                   0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_EMAC_DISABLE_BMSK                                    0x400
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_EMAC_DISABLE_SHFT                                      0xa
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_EMAC_DISABLE_ENABLE_FVAL                               0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_EMAC_DISABLE_DISABLE_FVAL                              0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_RSVD0_BMSK                                           0x380
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_RSVD0_SHFT                                             0x7
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_DOLBY_BIT_BMSK                                        0x40
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_DOLBY_BIT_SHFT                                         0x6
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_DOLBY_BIT_DISABLE_FVAL                                 0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_DOLBY_BIT_ENABLE_FVAL                                  0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_LPASS_SVA_DISABLE_BMSK                                0x20
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_LPASS_SVA_DISABLE_SHFT                                 0x5
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_LPASS_SVA_DISABLE_ENABLE_FVAL                          0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_LPASS_SVA_DISABLE_DISABLE_FVAL                         0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_LPASS_Q6_TCM_BOOT_DISABLE_BMSK                        0x10
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_LPASS_Q6_TCM_BOOT_DISABLE_SHFT                         0x4
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_LPASS_Q6_TCM_BOOT_DISABLE_ENABLE_FVAL                  0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_LPASS_Q6_TCM_BOOT_DISABLE_DISABLE_FVAL                 0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_ENABLE_DEVICE_IN_TEST_MODE_BMSK                        0x8
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_ENABLE_DEVICE_IN_TEST_MODE_SHFT                        0x3
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_ARM_CE_DISABLE_USAGE_BMSK                              0x4
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_ARM_CE_DISABLE_USAGE_SHFT                              0x2
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_CE_BAM_DISABLE_BMSK                                    0x2
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_CE_BAM_DISABLE_SHFT                                    0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_CE_BAM_DISABLE_ENABLE_FVAL                             0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_CE_BAM_DISABLE_DISABLE_FVAL                            0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_XBL_SEC_AUTH_DISABLE_BMSK                              0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_XBL_SEC_AUTH_DISABLE_SHFT                              0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_XBL_SEC_AUTH_DISABLE_ENABLE_FVAL                       0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_LSB_XBL_SEC_AUTH_DISABLE_DISABLE_FVAL                      0x1

#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042d4)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_ADDR, HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_RSVD5_BMSK                                      0x80000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_RSVD5_SHFT                                            0x1f
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_CM_FEAT_CONFIG_DISABLE_BMSK                     0x40000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_CM_FEAT_CONFIG_DISABLE_SHFT                           0x1e
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_RSVD4_BMSK                                      0x3f000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_RSVD4_SHFT                                            0x18
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY1_BMSK                      0x800000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY1_SHFT                          0x17
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY1_ICE_ENGINE_USES_SW_KEY_FVAL        0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY1_ICE_ENGINE_USES_HW_KEY_FVAL        0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY0_BMSK                      0x400000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY0_SHFT                          0x16
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY0_ICE_ENGINE_USES_SW_KEY_FVAL        0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_FORCE_HW_KEY0_ICE_ENGINE_USES_HW_KEY_FVAL        0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_DISABLE_BMSK                            0x200000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_DISABLE_SHFT                                0x15
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_DISABLE_ENABLE_FVAL                          0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_SDCC5_ICE_DISABLE_DISABLE_FVAL                         0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_RSVD3_BMSK                                        0x1e0000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_RSVD3_SHFT                                            0x11
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_APPS_CLOCKCFG_BMSK                                 0x18000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_APPS_CLOCKCFG_SHFT                                     0xf
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_RSVD2_BMSK                                          0x7800
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_RSVD2_SHFT                                             0xb
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_GFX_FRQ_LIM_FUSE_BMSK                                0x600
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_GFX_FRQ_LIM_FUSE_SHFT                                  0x9
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_DDR_FRQ_LIM_FUSE_BMSK                                0x180
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_DDR_FRQ_LIM_FUSE_SHFT                                  0x7
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_RSVD1_BMSK                                            0x40
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_RSVD1_SHFT                                             0x6
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_CFG_BMSK                                0x20
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_CFG_SHFT                                 0x5
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_CFG_DEFAULT_POWER_ON_BRINGUP_DISABLE_BOOT_FSM_FVAL        0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_CFG_BOOT_FSM_BASED_BRING_UP_FVAL         0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_DELAY_BMSK                              0x18
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_DELAY_SHFT                               0x3
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_DELAY_GDHS_CNT_128_CLAMP_RESET_128_L2_RESET_256_FVAL        0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_DELAY_GDHS_CNT_32_CLAMP_RESET_48_L2_RESET_128_FVAL        0x2
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_APPS_BOOT_FSM_DELAY_GDHS_CNT_16_CLAMP_RESET_23_L2_RESET_103_FVAL        0x3
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_DISPLAY_RESOLUTION1_LIMITER_BMSK                       0x6
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_DISPLAY_RESOLUTION1_LIMITER_SHFT                       0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_RSVD0_BMSK                                             0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW0_MSB_RSVD0_SHFT                                             0x0

#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042d8)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_ADDR, HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_RSVD0_BMSK                                      0xffe00000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_RSVD0_SHFT                                            0x15
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MISC_DEBUG_DISABLE_BMSK                 0x100000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MISC_DEBUG_DISABLE_SHFT                     0x14
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MISC_DEBUG_DISABLE_ENABLE_FVAL               0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MISC_DEBUG_DISABLE_DISABLE_FVAL              0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_BMSK                         0x80000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_SHFT                            0x13
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_ENABLE_FVAL                      0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_APPS_NIDEN_DISABLE_DISABLE_FVAL                     0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_BMSK                         0x40000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_SHFT                            0x12
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_ENABLE_FVAL                      0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_APPS_DBGEN_DISABLE_DISABLE_FVAL                     0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_NIDEN_DISABLE_BMSK                    0x20000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_NIDEN_DISABLE_SHFT                       0x11
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_NIDEN_DISABLE_ENABLE_FVAL                 0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_NIDEN_DISABLE_DISABLE_FVAL                0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_DBGEN_DISABLE_BMSK                    0x10000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_DBGEN_DISABLE_SHFT                       0x10
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_DBGEN_DISABLE_ENABLE_FVAL                 0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_NS_DBGEN_DISABLE_DISABLE_FVAL                0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_NIDEN_DISABLE_BMSK                     0x8000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_NIDEN_DISABLE_SHFT                        0xf
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_NIDEN_DISABLE_ENABLE_FVAL                 0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_NIDEN_DISABLE_DISABLE_FVAL                0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_DBGEN_DISABLE_BMSK                     0x4000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_DBGEN_DISABLE_SHFT                        0xe
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_DBGEN_DISABLE_ENABLE_FVAL                 0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_CP_DBGEN_DISABLE_DISABLE_FVAL                0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_NIDEN_DISABLE_BMSK                    0x2000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_NIDEN_DISABLE_SHFT                       0xd
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_NIDEN_DISABLE_ENABLE_FVAL                0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_NIDEN_DISABLE_DISABLE_FVAL               0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_DBGEN_DISABLE_BMSK                    0x1000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_DBGEN_DISABLE_SHFT                       0xc
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_DBGEN_DISABLE_ENABLE_FVAL                0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_MSS_DBGEN_DISABLE_DISABLE_FVAL               0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPNIDEN_DISABLE_BMSK                  0x800
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPNIDEN_DISABLE_SHFT                    0xb
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPNIDEN_DISABLE_ENABLE_FVAL             0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPNIDEN_DISABLE_DISABLE_FVAL            0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPIDEN_DISABLE_BMSK                   0x400
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPIDEN_DISABLE_SHFT                     0xa
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPIDEN_DISABLE_ENABLE_FVAL              0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_QC_SHARED_QSEE_SPIDEN_DISABLE_DISABLE_FVAL             0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_NIDEN_DISABLE_BMSK                        0x200
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_NIDEN_DISABLE_SHFT                          0x9
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_NIDEN_DISABLE_ENABLE_FVAL                   0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_NIDEN_DISABLE_DISABLE_FVAL                  0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_DBGEN_DISABLE_BMSK                        0x100
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_DBGEN_DISABLE_SHFT                          0x8
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_DBGEN_DISABLE_ENABLE_FVAL                   0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_NS_DBGEN_DISABLE_DISABLE_FVAL                  0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_NIDEN_DISABLE_BMSK                         0x80
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_NIDEN_DISABLE_SHFT                          0x7
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_NIDEN_DISABLE_ENABLE_FVAL                   0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_NIDEN_DISABLE_DISABLE_FVAL                  0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_DBGEN_DISABLE_BMSK                         0x40
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_DBGEN_DISABLE_SHFT                          0x6
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_DBGEN_DISABLE_ENABLE_FVAL                   0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_CP_DBGEN_DISABLE_DISABLE_FVAL                  0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_NIDEN_DISABLE_BMSK                        0x20
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_NIDEN_DISABLE_SHFT                         0x5
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_NIDEN_DISABLE_ENABLE_FVAL                  0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_NIDEN_DISABLE_DISABLE_FVAL                 0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_DBGEN_DISABLE_BMSK                        0x10
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_DBGEN_DISABLE_SHFT                         0x4
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_DBGEN_DISABLE_ENABLE_FVAL                  0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_MSS_DBGEN_DISABLE_DISABLE_FVAL                 0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPNIDEN_DISABLE_BMSK                      0x8
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPNIDEN_DISABLE_SHFT                      0x3
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPNIDEN_DISABLE_ENABLE_FVAL               0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPNIDEN_DISABLE_DISABLE_FVAL              0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPIDEN_DISABLE_BMSK                       0x4
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPIDEN_DISABLE_SHFT                       0x2
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPIDEN_DISABLE_ENABLE_FVAL                0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_PRIVATE_QSEE_SPIDEN_DISABLE_DISABLE_FVAL               0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_BMSK                                   0x2
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_SHFT                                   0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_ENABLE_FVAL                            0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_SM_BIST_DISABLE_DISABLE_FVAL                           0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_BMSK                                       0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_SHFT                                       0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_ENABLE_FVAL                                0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_LSB_TIC_DISABLE_DISABLE_FVAL                               0x1

#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042dc)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_ADDR, HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_BMSK                     0xfe000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_SEC_TAP_ACCESS_DISABLE_SHFT                           0x19
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_BMSK                           0x1ffff80
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_TAP_INSTR_DISABLE_SHFT                                 0x7
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_RSVD0_BMSK                                            0x40
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_RSVD0_SHFT                                             0x6
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_APPS_PBL_PATCH_VERSION_BMSK                           0x3f
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW1_MSB_APPS_PBL_PATCH_VERSION_SHFT                            0x0

#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042e0)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_TAP_GEN_SPARE_INSTR_DISABLE_31_0_BMSK           0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_LSB_TAP_GEN_SPARE_INSTR_DISABLE_31_0_SHFT                  0x0

#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_ADDR                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042e4)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_SPARE0_BMSK                                     0x80000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_SPARE0_SHFT                                           0x1f
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_A53SS_DEFEATURE_L2CACHE_1_BMSK                  0x40000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_A53SS_DEFEATURE_L2CACHE_1_SHFT                        0x1e
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_PBL_QSEE_BOOT_FLOW_DISABLE_BMSK                 0x20000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_PBL_QSEE_BOOT_FLOW_DISABLE_SHFT                       0x1d
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_4_BMSK                        0x10000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_4_SHFT                              0x1c
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_TRIGGER_DISABLE_BMSK                   0x8000000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_TRIGGER_DISABLE_SHFT                        0x1b
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_FOUNDRY_ID_BMSK                                  0x7800000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_FOUNDRY_ID_SHFT                                       0x17
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_BMSK                            0x780000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_APPS_PBL_PLL_CTRL_SHFT                                0x13
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_7_BMSK                        0x40000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_7_SHFT                           0x12
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_6_BMSK                        0x20000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_6_SHFT                           0x11
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_5_BMSK                        0x10000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_APPS_CFGCPUPRESENT_N_5_SHFT                           0x10
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_RSVD1_BMSK                                          0x8000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_RSVD1_SHFT                                             0xf
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_APPS_AARCH64_ENABLE_BMSK                            0x4000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_APPS_AARCH64_ENABLE_SHFT                               0xe
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_BMSK                            0x3000
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_SHFT                               0xc
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_ENUM_800_MHZ_FVAL                  0x0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_ENUM_400_MHZ_FVAL                  0x1
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_ENUM_200_MHZ_FVAL                  0x2
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_APPS_PBL_BOOT_SPEED_XO_FVAL                            0x3
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FROM_ROM_BMSK                              0x800
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_APPS_BOOT_FROM_ROM_SHFT                                0xb
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_RSVD0_BMSK                                           0x700
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_RSVD0_SHFT                                             0x8
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_BOOT_ROM_PATCH_DISABLE_BMSK                           0xe0
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_BOOT_ROM_PATCH_DISABLE_SHFT                            0x5
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_TAP_GEN_SPARE_INSTR_DISABLE_36_32_BMSK                0x1f
#define HWIO_QFPROM_CORR_CM_FEAT_CONFIG_ROW2_MSB_TAP_GEN_SPARE_INSTR_DISABLE_36_32_SHFT                 0x0

#define HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_ADDR(n)                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004268 + 0x8 * (n))
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_RMSK                                                  0xffffffff
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_MAXn                                                          17
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_ADDR(n), HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_RMSK)
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_QC_SPARE_BMSK                                         0xffffffff
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_LSB_QC_SPARE_SHFT                                                0x0

#define HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_ADDR(n)                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000426c + 0x8 * (n))
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_RMSK                                                    0xffffff
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_MAXn                                                          17
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_ADDR(n), HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_RMSK)
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_QC_SPARE_BMSK                                           0xffffff
#define HWIO_QFPROM_CORR_QC_SPARE_REGn_MSB_QC_SPARE_SHFT                                                0x0

#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_LSB_ADDR(n)                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042f8 + 0x8 * (n))
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_LSB_RMSK                                        0xffffffff
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_LSB_MAXn                                                 1
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_LSB_KEY_DATA0_BMSK                              0xffffffff
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_LSB_KEY_DATA0_SHFT                                     0x0

#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_MSB_ADDR(n)                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x000042fc + 0x8 * (n))
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_MSB_RMSK                                          0xffffff
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_MSB_MAXn                                                 1
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_MSB_KEY_DATA1_BMSK                                0xffffff
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROWn_MSB_KEY_DATA1_SHFT                                     0x0

#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_ADDR                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004308)
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_RMSK                                        0xffffffff
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_ADDR, HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_RSVD0_BMSK                                  0xffff0000
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_RSVD0_SHFT                                        0x10
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_KEY_DATA0_BMSK                                  0xffff
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_LSB_KEY_DATA0_SHFT                                     0x0

#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_MSB_ADDR                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000430c)
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_MSB_RMSK                                          0xffffff
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_MSB_ADDR, HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_MSB_RSVD0_BMSK                                    0xffffff
#define HWIO_QFPROM_CORR_OEM_IMAGE_ENCR_KEY_ROW2_MSB_RSVD0_SHFT                                         0x0

#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_ADDR(n)                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004310 + 0x8 * (n))
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_RMSK                                              0xffffffff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_MAXn                                                       1
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT4_BMSK                                    0xff000000
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT4_SHFT                                          0x18
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT3_BMSK                                      0xff0000
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT3_SHFT                                          0x10
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT2_BMSK                                        0xff00
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT2_SHFT                                           0x8
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT1_BMSK                                          0xff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_LSB_SEC_BOOT1_SHFT                                           0x0

#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_ADDR(n)                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004314 + 0x8 * (n))
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_RMSK                                                0xffffff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_MAXn                                                       1
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT7_BMSK                                      0xff0000
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT7_SHFT                                          0x10
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT6_BMSK                                        0xff00
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT6_SHFT                                           0x8
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT5_BMSK                                          0xff
#define HWIO_QFPROM_CORR_OEM_SEC_BOOT_ROWn_MSB_SEC_BOOT5_SHFT                                           0x0

#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004320 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_RMSK                                    0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_MAXn                                             3
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_BMSK                          0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_SHFT                                 0x0

#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004324 + 0x8 * (n))
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RMSK                                      0xffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_MAXn                                             3
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK                            0xffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_SHFT                                 0x0

#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004340)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_RMSK                                    0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_RMSK)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_BMSK                          0xffffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_SHFT                                 0x0

#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004344)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RMSK                                      0xffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RMSK)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RSVD0_BMSK                                0xffffff
#define HWIO_QFPROM_CORR_PRI_KEY_DERIVATION_KEY_ROW4_MSB_RSVD0_SHFT                                     0x0

#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n)                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004348 + 0x8 * (n))
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_RMSK                                    0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_MAXn                                             3
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_BMSK                          0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_LSB_KEY_DATA0_SHFT                                 0x0

#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n)                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000434c + 0x8 * (n))
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RMSK                                      0xffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_MAXn                                             3
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_BMSK                            0xffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROWn_MSB_KEY_DATA1_SHFT                                 0x0

#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004368)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_RMSK                                    0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_RMSK)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_ADDR, m)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_BMSK                          0xffffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_LSB_KEY_DATA0_SHFT                                 0x0

#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000436c)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RMSK                                      0xffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_IN          \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RMSK)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_ADDR, m)
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RSVD0_BMSK                                0xffffff
#define HWIO_QFPROM_CORR_SEC_KEY_DERIVATION_KEY_ROW4_MSB_RSVD0_SHFT                                     0x0

#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_ADDR(n)                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004370 + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_RMSK                                                 0xffffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_MAXn                                                         39
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_ADDR(n), HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_RMSK)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_PATCH_DATA_BMSK                                      0xffffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_LSB_PATCH_DATA_SHFT                                             0x0

#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_ADDR(n)                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004374 + 0x8 * (n))
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_RMSK                                                   0xffffff
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_MAXn                                                         39
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_ADDR(n), HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_RMSK)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_RSVD0_BMSK                                             0xfe0000
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_RSVD0_SHFT                                                 0x11
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_PATCH_ADDR_BMSK                                         0x1fffe
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_PATCH_ADDR_SHFT                                             0x1
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_PATCH_EN_BMSK                                               0x1
#define HWIO_QFPROM_CORR_ROM_PATCH_ROWn_MSB_PATCH_EN_SHFT                                               0x0

#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_ADDR(n)                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004340 + 0x10 * (n))
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_MAXn                                                    27
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_ADDR(n), HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_OEM_SPARE_BMSK                                  0xffffffff
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_LSB_OEM_SPARE_SHFT                                         0x0

#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_MSB_ADDR(n)                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004344 + 0x10 * (n))
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_MSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_MSB_MAXn                                                    27
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_MSB_ADDR(n), HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_MSB_OEM_SPARE_BMSK                                  0xffffffff
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW0_MSB_OEM_SPARE_SHFT                                         0x0

#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_LSB_ADDR(n)                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x00004348 + 0x10 * (n))
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_LSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_LSB_MAXn                                                    27
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_LSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_LSB_ADDR(n), HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_LSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_LSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_LSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_LSB_OEM_SPARE_BMSK                                  0xffffffff
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_LSB_OEM_SPARE_SHFT                                         0x0

#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_MSB_ADDR(n)                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000434c + 0x10 * (n))
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_MSB_RMSK                                            0xffffffff
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_MSB_MAXn                                                    27
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_MSB_INI(n)        \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_MSB_ADDR(n), HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_MSB_RMSK)
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_MSB_INMI(n,mask)    \
        in_dword_masked(HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_MSB_ADDR(n), mask)
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_MSB_OEM_SPARE_BMSK                                  0xffffffff
#define HWIO_QFPROM_CORR_OEM_SPARE_REGn_ROW1_MSB_OEM_SPARE_SHFT                                         0x0

#define HWIO_SEC_CTRL_HW_VERSION_ADDR                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006000)
#define HWIO_SEC_CTRL_HW_VERSION_RMSK                                                            0xffffffff
#define HWIO_SEC_CTRL_HW_VERSION_IN          \
        in_dword_masked(HWIO_SEC_CTRL_HW_VERSION_ADDR, HWIO_SEC_CTRL_HW_VERSION_RMSK)
#define HWIO_SEC_CTRL_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_SEC_CTRL_HW_VERSION_ADDR, m)
#define HWIO_SEC_CTRL_HW_VERSION_MAJOR_BMSK                                                      0xf0000000
#define HWIO_SEC_CTRL_HW_VERSION_MAJOR_SHFT                                                            0x1c
#define HWIO_SEC_CTRL_HW_VERSION_MINOR_BMSK                                                       0xfff0000
#define HWIO_SEC_CTRL_HW_VERSION_MINOR_SHFT                                                            0x10
#define HWIO_SEC_CTRL_HW_VERSION_STEP_BMSK                                                           0xffff
#define HWIO_SEC_CTRL_HW_VERSION_STEP_SHFT                                                              0x0

#define HWIO_FEATURE_CONFIG0_ADDR                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006004)
#define HWIO_FEATURE_CONFIG0_RMSK                                                                0xffffffff
#define HWIO_FEATURE_CONFIG0_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG0_ADDR, HWIO_FEATURE_CONFIG0_RMSK)
#define HWIO_FEATURE_CONFIG0_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG0_ADDR, m)
#define HWIO_FEATURE_CONFIG0_DISPLAY_RESOLUTION0_LIMITER_1_BMSK                                  0x80000000
#define HWIO_FEATURE_CONFIG0_DISPLAY_RESOLUTION0_LIMITER_1_SHFT                                        0x1f
#define HWIO_FEATURE_CONFIG0_RSVD2_BMSK                                                          0x40000000
#define HWIO_FEATURE_CONFIG0_RSVD2_SHFT                                                                0x1e
#define HWIO_FEATURE_CONFIG0_DSI_0_DISABLE_BMSK                                                  0x20000000
#define HWIO_FEATURE_CONFIG0_DSI_0_DISABLE_SHFT                                                        0x1d
#define HWIO_FEATURE_CONFIG0_DISPLAY_RESOLUTION0_LIMITER_0_BMSK                                  0x10000000
#define HWIO_FEATURE_CONFIG0_DISPLAY_RESOLUTION0_LIMITER_0_SHFT                                        0x1c
#define HWIO_FEATURE_CONFIG0_NIDNT_DISABLE_BMSK                                                   0x8000000
#define HWIO_FEATURE_CONFIG0_NIDNT_DISABLE_SHFT                                                        0x1b
#define HWIO_FEATURE_CONFIG0_RSVD1_BMSK                                                           0x7e00000
#define HWIO_FEATURE_CONFIG0_RSVD1_SHFT                                                                0x15
#define HWIO_FEATURE_CONFIG0_SPDIF_0_1_SW_DISABLE_BMSK                                             0x100000
#define HWIO_FEATURE_CONFIG0_SPDIF_0_1_SW_DISABLE_SHFT                                                 0x14
#define HWIO_FEATURE_CONFIG0_SPDIF_0_1_SW_DISABLE_ENABLE_FVAL                                           0x0
#define HWIO_FEATURE_CONFIG0_SPDIF_0_1_SW_DISABLE_DISABLE_FVAL                                          0x1
#define HWIO_FEATURE_CONFIG0_I2S3_DISABLE_BMSK                                                      0x80000
#define HWIO_FEATURE_CONFIG0_I2S3_DISABLE_SHFT                                                         0x13
#define HWIO_FEATURE_CONFIG0_I2S3_DISABLE_ENABLE_FVAL                                                   0x0
#define HWIO_FEATURE_CONFIG0_I2S3_DISABLE_DISABLE_FVAL                                                  0x1
#define HWIO_FEATURE_CONFIG0_I2S2_DISABLE_BMSK                                                      0x40000
#define HWIO_FEATURE_CONFIG0_I2S2_DISABLE_SHFT                                                         0x12
#define HWIO_FEATURE_CONFIG0_I2S2_DISABLE_ENABLE_FVAL                                                   0x0
#define HWIO_FEATURE_CONFIG0_I2S2_DISABLE_DISABLE_FVAL                                                  0x1
#define HWIO_FEATURE_CONFIG0_I2S1_DISABLE_BMSK                                                      0x20000
#define HWIO_FEATURE_CONFIG0_I2S1_DISABLE_SHFT                                                         0x11
#define HWIO_FEATURE_CONFIG0_I2S1_DISABLE_ENABLE_FVAL                                                   0x0
#define HWIO_FEATURE_CONFIG0_I2S1_DISABLE_DISABLE_FVAL                                                  0x1
#define HWIO_FEATURE_CONFIG0_I2S0_DISABLE_BMSK                                                      0x10000
#define HWIO_FEATURE_CONFIG0_I2S0_DISABLE_SHFT                                                         0x10
#define HWIO_FEATURE_CONFIG0_I2S0_DISABLE_ENABLE_FVAL                                                   0x0
#define HWIO_FEATURE_CONFIG0_I2S0_DISABLE_DISABLE_FVAL                                                  0x1
#define HWIO_FEATURE_CONFIG0_USB3_DISABLE_BMSK                                                       0x8000
#define HWIO_FEATURE_CONFIG0_USB3_DISABLE_SHFT                                                          0xf
#define HWIO_FEATURE_CONFIG0_USB3_DISABLE_ENABLE_FVAL                                                   0x0
#define HWIO_FEATURE_CONFIG0_USB3_DISABLE_DISABLE_FVAL                                                  0x1
#define HWIO_FEATURE_CONFIG0_PCIE_DISABLE_BMSK                                                       0x4000
#define HWIO_FEATURE_CONFIG0_PCIE_DISABLE_SHFT                                                          0xe
#define HWIO_FEATURE_CONFIG0_PCIE_DISABLE_ENABLE_FVAL                                                   0x0
#define HWIO_FEATURE_CONFIG0_PCIE_DISABLE_DISABLE_FVAL                                                  0x1
#define HWIO_FEATURE_CONFIG0_HDMI_DISABLE_BMSK                                                       0x2000
#define HWIO_FEATURE_CONFIG0_HDMI_DISABLE_SHFT                                                          0xd
#define HWIO_FEATURE_CONFIG0_HDMI_DISABLE_ENABLE_FVAL                                                   0x0
#define HWIO_FEATURE_CONFIG0_HDMI_DISABLE_DISABLE_FVAL                                                  0x1
#define HWIO_FEATURE_CONFIG0_TURING_Q6_DISABLE_BMSK                                                  0x1000
#define HWIO_FEATURE_CONFIG0_TURING_Q6_DISABLE_SHFT                                                     0xc
#define HWIO_FEATURE_CONFIG0_TURING_Q6_DISABLE_ENABLE_FVAL                                              0x0
#define HWIO_FEATURE_CONFIG0_TURING_Q6_DISABLE_DISABLE_FVAL                                             0x1
#define HWIO_FEATURE_CONFIG0_TURING_Q6SS_HVX_DISABLE_BMSK                                             0x800
#define HWIO_FEATURE_CONFIG0_TURING_Q6SS_HVX_DISABLE_SHFT                                               0xb
#define HWIO_FEATURE_CONFIG0_TURING_Q6SS_HVX_DISABLE_ENABLE_FVAL                                        0x0
#define HWIO_FEATURE_CONFIG0_TURING_Q6SS_HVX_DISABLE_DISABLE_FVAL                                       0x1
#define HWIO_FEATURE_CONFIG0_EMAC_DISABLE_BMSK                                                        0x400
#define HWIO_FEATURE_CONFIG0_EMAC_DISABLE_SHFT                                                          0xa
#define HWIO_FEATURE_CONFIG0_EMAC_DISABLE_ENABLE_FVAL                                                   0x0
#define HWIO_FEATURE_CONFIG0_EMAC_DISABLE_DISABLE_FVAL                                                  0x1
#define HWIO_FEATURE_CONFIG0_RSVD0_BMSK                                                               0x380
#define HWIO_FEATURE_CONFIG0_RSVD0_SHFT                                                                 0x7
#define HWIO_FEATURE_CONFIG0_DOLBY_BIT_BMSK                                                            0x40
#define HWIO_FEATURE_CONFIG0_DOLBY_BIT_SHFT                                                             0x6
#define HWIO_FEATURE_CONFIG0_DOLBY_BIT_DISABLE_FVAL                                                     0x0
#define HWIO_FEATURE_CONFIG0_DOLBY_BIT_ENABLE_FVAL                                                      0x1
#define HWIO_FEATURE_CONFIG0_LPASS_SVA_DISABLE_BMSK                                                    0x20
#define HWIO_FEATURE_CONFIG0_LPASS_SVA_DISABLE_SHFT                                                     0x5
#define HWIO_FEATURE_CONFIG0_LPASS_SVA_DISABLE_ENABLE_FVAL                                              0x0
#define HWIO_FEATURE_CONFIG0_LPASS_SVA_DISABLE_DISABLE_FVAL                                             0x1
#define HWIO_FEATURE_CONFIG0_LPASS_Q6_TCM_BOOT_DISABLE_BMSK                                            0x10
#define HWIO_FEATURE_CONFIG0_LPASS_Q6_TCM_BOOT_DISABLE_SHFT                                             0x4
#define HWIO_FEATURE_CONFIG0_LPASS_Q6_TCM_BOOT_DISABLE_ENABLE_FVAL                                      0x0
#define HWIO_FEATURE_CONFIG0_LPASS_Q6_TCM_BOOT_DISABLE_DISABLE_FVAL                                     0x1
#define HWIO_FEATURE_CONFIG0_ENABLE_DEVICE_IN_TEST_MODE_BMSK                                            0x8
#define HWIO_FEATURE_CONFIG0_ENABLE_DEVICE_IN_TEST_MODE_SHFT                                            0x3
#define HWIO_FEATURE_CONFIG0_ARM_CE_DISABLE_USAGE_BMSK                                                  0x4
#define HWIO_FEATURE_CONFIG0_ARM_CE_DISABLE_USAGE_SHFT                                                  0x2
#define HWIO_FEATURE_CONFIG0_CE_BAM_DISABLE_BMSK                                                        0x2
#define HWIO_FEATURE_CONFIG0_CE_BAM_DISABLE_SHFT                                                        0x1
#define HWIO_FEATURE_CONFIG0_CE_BAM_DISABLE_ENABLE_FVAL                                                 0x0
#define HWIO_FEATURE_CONFIG0_CE_BAM_DISABLE_DISABLE_FVAL                                                0x1
#define HWIO_FEATURE_CONFIG0_XBL_SEC_AUTH_DISABLE_BMSK                                                  0x1
#define HWIO_FEATURE_CONFIG0_XBL_SEC_AUTH_DISABLE_SHFT                                                  0x0
#define HWIO_FEATURE_CONFIG0_XBL_SEC_AUTH_DISABLE_ENABLE_FVAL                                           0x0
#define HWIO_FEATURE_CONFIG0_XBL_SEC_AUTH_DISABLE_DISABLE_FVAL                                          0x1

#define HWIO_FEATURE_CONFIG1_ADDR                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006008)
#define HWIO_FEATURE_CONFIG1_RMSK                                                                0xffffffff
#define HWIO_FEATURE_CONFIG1_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG1_ADDR, HWIO_FEATURE_CONFIG1_RMSK)
#define HWIO_FEATURE_CONFIG1_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG1_ADDR, m)
#define HWIO_FEATURE_CONFIG1_RSVD5_BMSK                                                          0x80000000
#define HWIO_FEATURE_CONFIG1_RSVD5_SHFT                                                                0x1f
#define HWIO_FEATURE_CONFIG1_CM_FEAT_CONFIG_DISABLE_BMSK                                         0x40000000
#define HWIO_FEATURE_CONFIG1_CM_FEAT_CONFIG_DISABLE_SHFT                                               0x1e
#define HWIO_FEATURE_CONFIG1_RSVD4_BMSK                                                          0x3f000000
#define HWIO_FEATURE_CONFIG1_RSVD4_SHFT                                                                0x18
#define HWIO_FEATURE_CONFIG1_SDCC5_ICE_FORCE_HW_KEY1_BMSK                                          0x800000
#define HWIO_FEATURE_CONFIG1_SDCC5_ICE_FORCE_HW_KEY1_SHFT                                              0x17
#define HWIO_FEATURE_CONFIG1_SDCC5_ICE_FORCE_HW_KEY1_ICE_ENGINE_USES_SW_KEY_FVAL                        0x0
#define HWIO_FEATURE_CONFIG1_SDCC5_ICE_FORCE_HW_KEY1_ICE_ENGINE_USES_HW_KEY_FVAL                        0x1
#define HWIO_FEATURE_CONFIG1_SDCC5_ICE_FORCE_HW_KEY0_BMSK                                          0x400000
#define HWIO_FEATURE_CONFIG1_SDCC5_ICE_FORCE_HW_KEY0_SHFT                                              0x16
#define HWIO_FEATURE_CONFIG1_SDCC5_ICE_FORCE_HW_KEY0_ICE_ENGINE_USES_SW_KEY_FVAL                        0x0
#define HWIO_FEATURE_CONFIG1_SDCC5_ICE_FORCE_HW_KEY0_ICE_ENGINE_USES_HW_KEY_FVAL                        0x1
#define HWIO_FEATURE_CONFIG1_SDCC5_ICE_DISABLE_BMSK                                                0x200000
#define HWIO_FEATURE_CONFIG1_SDCC5_ICE_DISABLE_SHFT                                                    0x15
#define HWIO_FEATURE_CONFIG1_SDCC5_ICE_DISABLE_ENABLE_FVAL                                              0x0
#define HWIO_FEATURE_CONFIG1_SDCC5_ICE_DISABLE_DISABLE_FVAL                                             0x1
#define HWIO_FEATURE_CONFIG1_RSVD3_BMSK                                                            0x1e0000
#define HWIO_FEATURE_CONFIG1_RSVD3_SHFT                                                                0x11
#define HWIO_FEATURE_CONFIG1_APPS_CLOCKCFG_BMSK                                                     0x18000
#define HWIO_FEATURE_CONFIG1_APPS_CLOCKCFG_SHFT                                                         0xf
#define HWIO_FEATURE_CONFIG1_RSVD2_BMSK                                                              0x7800
#define HWIO_FEATURE_CONFIG1_RSVD2_SHFT                                                                 0xb
#define HWIO_FEATURE_CONFIG1_GFX_FRQ_LIM_FUSE_BMSK                                                    0x600
#define HWIO_FEATURE_CONFIG1_GFX_FRQ_LIM_FUSE_SHFT                                                      0x9
#define HWIO_FEATURE_CONFIG1_DDR_FRQ_LIM_FUSE_BMSK                                                    0x180
#define HWIO_FEATURE_CONFIG1_DDR_FRQ_LIM_FUSE_SHFT                                                      0x7
#define HWIO_FEATURE_CONFIG1_RSVD1_BMSK                                                                0x40
#define HWIO_FEATURE_CONFIG1_RSVD1_SHFT                                                                 0x6
#define HWIO_FEATURE_CONFIG1_APPS_BOOT_FSM_CFG_BMSK                                                    0x20
#define HWIO_FEATURE_CONFIG1_APPS_BOOT_FSM_CFG_SHFT                                                     0x5
#define HWIO_FEATURE_CONFIG1_APPS_BOOT_FSM_CFG_DEFAULT_POWER_ON_BRINGUP_DISABLE_BOOT_FSM_FVAL           0x0
#define HWIO_FEATURE_CONFIG1_APPS_BOOT_FSM_CFG_BOOT_FSM_BASED_BRING_UP_FVAL                             0x1
#define HWIO_FEATURE_CONFIG1_APPS_BOOT_FSM_DELAY_BMSK                                                  0x18
#define HWIO_FEATURE_CONFIG1_APPS_BOOT_FSM_DELAY_SHFT                                                   0x3
#define HWIO_FEATURE_CONFIG1_APPS_BOOT_FSM_DELAY_GDHS_CNT_128_CLAMP_RESET_128_L2_RESET_256_FVAL         0x0
#define HWIO_FEATURE_CONFIG1_APPS_BOOT_FSM_DELAY_GDHS_CNT_32_CLAMP_RESET_48_L2_RESET_128_FVAL           0x2
#define HWIO_FEATURE_CONFIG1_APPS_BOOT_FSM_DELAY_GDHS_CNT_16_CLAMP_RESET_23_L2_RESET_103_FVAL           0x3
#define HWIO_FEATURE_CONFIG1_DISPLAY_RESOLUTION1_LIMITER_BMSK                                           0x6
#define HWIO_FEATURE_CONFIG1_DISPLAY_RESOLUTION1_LIMITER_SHFT                                           0x1
#define HWIO_FEATURE_CONFIG1_RSVD0_BMSK                                                                 0x1
#define HWIO_FEATURE_CONFIG1_RSVD0_SHFT                                                                 0x0

#define HWIO_FEATURE_CONFIG2_ADDR                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000600c)
#define HWIO_FEATURE_CONFIG2_RMSK                                                                0xffffffff
#define HWIO_FEATURE_CONFIG2_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG2_ADDR, HWIO_FEATURE_CONFIG2_RMSK)
#define HWIO_FEATURE_CONFIG2_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG2_ADDR, m)
#define HWIO_FEATURE_CONFIG2_RSVD0_BMSK                                                          0xffe00000
#define HWIO_FEATURE_CONFIG2_RSVD0_SHFT                                                                0x15
#define HWIO_FEATURE_CONFIG2_QC_SHARED_MISC_DEBUG_DISABLE_BMSK                                     0x100000
#define HWIO_FEATURE_CONFIG2_QC_SHARED_MISC_DEBUG_DISABLE_SHFT                                         0x14
#define HWIO_FEATURE_CONFIG2_QC_SHARED_MISC_DEBUG_DISABLE_ENABLE_FVAL                                   0x0
#define HWIO_FEATURE_CONFIG2_QC_SHARED_MISC_DEBUG_DISABLE_DISABLE_FVAL                                  0x1
#define HWIO_FEATURE_CONFIG2_QC_APPS_NIDEN_DISABLE_BMSK                                             0x80000
#define HWIO_FEATURE_CONFIG2_QC_APPS_NIDEN_DISABLE_SHFT                                                0x13
#define HWIO_FEATURE_CONFIG2_QC_APPS_NIDEN_DISABLE_ENABLE_FVAL                                          0x0
#define HWIO_FEATURE_CONFIG2_QC_APPS_NIDEN_DISABLE_DISABLE_FVAL                                         0x1
#define HWIO_FEATURE_CONFIG2_QC_APPS_DBGEN_DISABLE_BMSK                                             0x40000
#define HWIO_FEATURE_CONFIG2_QC_APPS_DBGEN_DISABLE_SHFT                                                0x12
#define HWIO_FEATURE_CONFIG2_QC_APPS_DBGEN_DISABLE_ENABLE_FVAL                                          0x0
#define HWIO_FEATURE_CONFIG2_QC_APPS_DBGEN_DISABLE_DISABLE_FVAL                                         0x1
#define HWIO_FEATURE_CONFIG2_QC_SHARED_NS_NIDEN_DISABLE_BMSK                                        0x20000
#define HWIO_FEATURE_CONFIG2_QC_SHARED_NS_NIDEN_DISABLE_SHFT                                           0x11
#define HWIO_FEATURE_CONFIG2_QC_SHARED_NS_NIDEN_DISABLE_ENABLE_FVAL                                     0x0
#define HWIO_FEATURE_CONFIG2_QC_SHARED_NS_NIDEN_DISABLE_DISABLE_FVAL                                    0x1
#define HWIO_FEATURE_CONFIG2_QC_SHARED_NS_DBGEN_DISABLE_BMSK                                        0x10000
#define HWIO_FEATURE_CONFIG2_QC_SHARED_NS_DBGEN_DISABLE_SHFT                                           0x10
#define HWIO_FEATURE_CONFIG2_QC_SHARED_NS_DBGEN_DISABLE_ENABLE_FVAL                                     0x0
#define HWIO_FEATURE_CONFIG2_QC_SHARED_NS_DBGEN_DISABLE_DISABLE_FVAL                                    0x1
#define HWIO_FEATURE_CONFIG2_QC_SHARED_CP_NIDEN_DISABLE_BMSK                                         0x8000
#define HWIO_FEATURE_CONFIG2_QC_SHARED_CP_NIDEN_DISABLE_SHFT                                            0xf
#define HWIO_FEATURE_CONFIG2_QC_SHARED_CP_NIDEN_DISABLE_ENABLE_FVAL                                     0x0
#define HWIO_FEATURE_CONFIG2_QC_SHARED_CP_NIDEN_DISABLE_DISABLE_FVAL                                    0x1
#define HWIO_FEATURE_CONFIG2_QC_SHARED_CP_DBGEN_DISABLE_BMSK                                         0x4000
#define HWIO_FEATURE_CONFIG2_QC_SHARED_CP_DBGEN_DISABLE_SHFT                                            0xe
#define HWIO_FEATURE_CONFIG2_QC_SHARED_CP_DBGEN_DISABLE_ENABLE_FVAL                                     0x0
#define HWIO_FEATURE_CONFIG2_QC_SHARED_CP_DBGEN_DISABLE_DISABLE_FVAL                                    0x1
#define HWIO_FEATURE_CONFIG2_QC_SHARED_MSS_NIDEN_DISABLE_BMSK                                        0x2000
#define HWIO_FEATURE_CONFIG2_QC_SHARED_MSS_NIDEN_DISABLE_SHFT                                           0xd
#define HWIO_FEATURE_CONFIG2_QC_SHARED_MSS_NIDEN_DISABLE_ENABLE_FVAL                                    0x0
#define HWIO_FEATURE_CONFIG2_QC_SHARED_MSS_NIDEN_DISABLE_DISABLE_FVAL                                   0x1
#define HWIO_FEATURE_CONFIG2_QC_SHARED_MSS_DBGEN_DISABLE_BMSK                                        0x1000
#define HWIO_FEATURE_CONFIG2_QC_SHARED_MSS_DBGEN_DISABLE_SHFT                                           0xc
#define HWIO_FEATURE_CONFIG2_QC_SHARED_MSS_DBGEN_DISABLE_ENABLE_FVAL                                    0x0
#define HWIO_FEATURE_CONFIG2_QC_SHARED_MSS_DBGEN_DISABLE_DISABLE_FVAL                                   0x1
#define HWIO_FEATURE_CONFIG2_QC_SHARED_QSEE_SPNIDEN_DISABLE_BMSK                                      0x800
#define HWIO_FEATURE_CONFIG2_QC_SHARED_QSEE_SPNIDEN_DISABLE_SHFT                                        0xb
#define HWIO_FEATURE_CONFIG2_QC_SHARED_QSEE_SPNIDEN_DISABLE_ENABLE_FVAL                                 0x0
#define HWIO_FEATURE_CONFIG2_QC_SHARED_QSEE_SPNIDEN_DISABLE_DISABLE_FVAL                                0x1
#define HWIO_FEATURE_CONFIG2_QC_SHARED_QSEE_SPIDEN_DISABLE_BMSK                                       0x400
#define HWIO_FEATURE_CONFIG2_QC_SHARED_QSEE_SPIDEN_DISABLE_SHFT                                         0xa
#define HWIO_FEATURE_CONFIG2_QC_SHARED_QSEE_SPIDEN_DISABLE_ENABLE_FVAL                                  0x0
#define HWIO_FEATURE_CONFIG2_QC_SHARED_QSEE_SPIDEN_DISABLE_DISABLE_FVAL                                 0x1
#define HWIO_FEATURE_CONFIG2_PRIVATE_NS_NIDEN_DISABLE_BMSK                                            0x200
#define HWIO_FEATURE_CONFIG2_PRIVATE_NS_NIDEN_DISABLE_SHFT                                              0x9
#define HWIO_FEATURE_CONFIG2_PRIVATE_NS_NIDEN_DISABLE_ENABLE_FVAL                                       0x0
#define HWIO_FEATURE_CONFIG2_PRIVATE_NS_NIDEN_DISABLE_DISABLE_FVAL                                      0x1
#define HWIO_FEATURE_CONFIG2_PRIVATE_NS_DBGEN_DISABLE_BMSK                                            0x100
#define HWIO_FEATURE_CONFIG2_PRIVATE_NS_DBGEN_DISABLE_SHFT                                              0x8
#define HWIO_FEATURE_CONFIG2_PRIVATE_NS_DBGEN_DISABLE_ENABLE_FVAL                                       0x0
#define HWIO_FEATURE_CONFIG2_PRIVATE_NS_DBGEN_DISABLE_DISABLE_FVAL                                      0x1
#define HWIO_FEATURE_CONFIG2_PRIVATE_CP_NIDEN_DISABLE_BMSK                                             0x80
#define HWIO_FEATURE_CONFIG2_PRIVATE_CP_NIDEN_DISABLE_SHFT                                              0x7
#define HWIO_FEATURE_CONFIG2_PRIVATE_CP_NIDEN_DISABLE_ENABLE_FVAL                                       0x0
#define HWIO_FEATURE_CONFIG2_PRIVATE_CP_NIDEN_DISABLE_DISABLE_FVAL                                      0x1
#define HWIO_FEATURE_CONFIG2_PRIVATE_CP_DBGEN_DISABLE_BMSK                                             0x40
#define HWIO_FEATURE_CONFIG2_PRIVATE_CP_DBGEN_DISABLE_SHFT                                              0x6
#define HWIO_FEATURE_CONFIG2_PRIVATE_CP_DBGEN_DISABLE_ENABLE_FVAL                                       0x0
#define HWIO_FEATURE_CONFIG2_PRIVATE_CP_DBGEN_DISABLE_DISABLE_FVAL                                      0x1
#define HWIO_FEATURE_CONFIG2_PRIVATE_MSS_NIDEN_DISABLE_BMSK                                            0x20
#define HWIO_FEATURE_CONFIG2_PRIVATE_MSS_NIDEN_DISABLE_SHFT                                             0x5
#define HWIO_FEATURE_CONFIG2_PRIVATE_MSS_NIDEN_DISABLE_ENABLE_FVAL                                      0x0
#define HWIO_FEATURE_CONFIG2_PRIVATE_MSS_NIDEN_DISABLE_DISABLE_FVAL                                     0x1
#define HWIO_FEATURE_CONFIG2_PRIVATE_MSS_DBGEN_DISABLE_BMSK                                            0x10
#define HWIO_FEATURE_CONFIG2_PRIVATE_MSS_DBGEN_DISABLE_SHFT                                             0x4
#define HWIO_FEATURE_CONFIG2_PRIVATE_MSS_DBGEN_DISABLE_ENABLE_FVAL                                      0x0
#define HWIO_FEATURE_CONFIG2_PRIVATE_MSS_DBGEN_DISABLE_DISABLE_FVAL                                     0x1
#define HWIO_FEATURE_CONFIG2_PRIVATE_QSEE_SPNIDEN_DISABLE_BMSK                                          0x8
#define HWIO_FEATURE_CONFIG2_PRIVATE_QSEE_SPNIDEN_DISABLE_SHFT                                          0x3
#define HWIO_FEATURE_CONFIG2_PRIVATE_QSEE_SPNIDEN_DISABLE_ENABLE_FVAL                                   0x0
#define HWIO_FEATURE_CONFIG2_PRIVATE_QSEE_SPNIDEN_DISABLE_DISABLE_FVAL                                  0x1
#define HWIO_FEATURE_CONFIG2_PRIVATE_QSEE_SPIDEN_DISABLE_BMSK                                           0x4
#define HWIO_FEATURE_CONFIG2_PRIVATE_QSEE_SPIDEN_DISABLE_SHFT                                           0x2
#define HWIO_FEATURE_CONFIG2_PRIVATE_QSEE_SPIDEN_DISABLE_ENABLE_FVAL                                    0x0
#define HWIO_FEATURE_CONFIG2_PRIVATE_QSEE_SPIDEN_DISABLE_DISABLE_FVAL                                   0x1
#define HWIO_FEATURE_CONFIG2_SM_BIST_DISABLE_BMSK                                                       0x2
#define HWIO_FEATURE_CONFIG2_SM_BIST_DISABLE_SHFT                                                       0x1
#define HWIO_FEATURE_CONFIG2_SM_BIST_DISABLE_ENABLE_FVAL                                                0x0
#define HWIO_FEATURE_CONFIG2_SM_BIST_DISABLE_DISABLE_FVAL                                               0x1
#define HWIO_FEATURE_CONFIG2_TIC_DISABLE_BMSK                                                           0x1
#define HWIO_FEATURE_CONFIG2_TIC_DISABLE_SHFT                                                           0x0
#define HWIO_FEATURE_CONFIG2_TIC_DISABLE_ENABLE_FVAL                                                    0x0
#define HWIO_FEATURE_CONFIG2_TIC_DISABLE_DISABLE_FVAL                                                   0x1

#define HWIO_FEATURE_CONFIG3_ADDR                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006010)
#define HWIO_FEATURE_CONFIG3_RMSK                                                                0xffffffff
#define HWIO_FEATURE_CONFIG3_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG3_ADDR, HWIO_FEATURE_CONFIG3_RMSK)
#define HWIO_FEATURE_CONFIG3_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG3_ADDR, m)
#define HWIO_FEATURE_CONFIG3_SEC_TAP_ACCESS_DISABLE_BMSK                                         0xfe000000
#define HWIO_FEATURE_CONFIG3_SEC_TAP_ACCESS_DISABLE_SHFT                                               0x19
#define HWIO_FEATURE_CONFIG3_TAP_INSTR_DISABLE_BMSK                                               0x1ffff80
#define HWIO_FEATURE_CONFIG3_TAP_INSTR_DISABLE_SHFT                                                     0x7
#define HWIO_FEATURE_CONFIG3_RSVD0_BMSK                                                                0x40
#define HWIO_FEATURE_CONFIG3_RSVD0_SHFT                                                                 0x6
#define HWIO_FEATURE_CONFIG3_APPS_PBL_PATCH_VERSION_BMSK                                               0x3f
#define HWIO_FEATURE_CONFIG3_APPS_PBL_PATCH_VERSION_SHFT                                                0x0

#define HWIO_FEATURE_CONFIG4_ADDR                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006014)
#define HWIO_FEATURE_CONFIG4_RMSK                                                                0xffffffff
#define HWIO_FEATURE_CONFIG4_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG4_ADDR, HWIO_FEATURE_CONFIG4_RMSK)
#define HWIO_FEATURE_CONFIG4_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG4_ADDR, m)
#define HWIO_FEATURE_CONFIG4_TAP_GEN_SPARE_INSTR_DISABLE_31_0_BMSK                               0xffffffff
#define HWIO_FEATURE_CONFIG4_TAP_GEN_SPARE_INSTR_DISABLE_31_0_SHFT                                      0x0

#define HWIO_FEATURE_CONFIG5_ADDR                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006018)
#define HWIO_FEATURE_CONFIG5_RMSK                                                                0xffffffff
#define HWIO_FEATURE_CONFIG5_IN          \
        in_dword_masked(HWIO_FEATURE_CONFIG5_ADDR, HWIO_FEATURE_CONFIG5_RMSK)
#define HWIO_FEATURE_CONFIG5_INM(m)      \
        in_dword_masked(HWIO_FEATURE_CONFIG5_ADDR, m)
#define HWIO_FEATURE_CONFIG5_SPARE0_BMSK                                                         0x80000000
#define HWIO_FEATURE_CONFIG5_SPARE0_SHFT                                                               0x1f
#define HWIO_FEATURE_CONFIG5_A53SS_DEFEATURE_L2CACHE_1_BMSK                                      0x40000000
#define HWIO_FEATURE_CONFIG5_A53SS_DEFEATURE_L2CACHE_1_SHFT                                            0x1e
#define HWIO_FEATURE_CONFIG5_PBL_QSEE_BOOT_FLOW_DISABLE_BMSK                                     0x20000000
#define HWIO_FEATURE_CONFIG5_PBL_QSEE_BOOT_FLOW_DISABLE_SHFT                                           0x1d
#define HWIO_FEATURE_CONFIG5_APPS_PBL_PLL_CTRL_4_BMSK                                            0x10000000
#define HWIO_FEATURE_CONFIG5_APPS_PBL_PLL_CTRL_4_SHFT                                                  0x1c
#define HWIO_FEATURE_CONFIG5_APPS_BOOT_TRIGGER_DISABLE_BMSK                                       0x8000000
#define HWIO_FEATURE_CONFIG5_APPS_BOOT_TRIGGER_DISABLE_SHFT                                            0x1b
#define HWIO_FEATURE_CONFIG5_FOUNDRY_ID_BMSK                                                      0x7800000
#define HWIO_FEATURE_CONFIG5_FOUNDRY_ID_SHFT                                                           0x17
#define HWIO_FEATURE_CONFIG5_APPS_PBL_PLL_CTRL_BMSK                                                0x780000
#define HWIO_FEATURE_CONFIG5_APPS_PBL_PLL_CTRL_SHFT                                                    0x13
#define HWIO_FEATURE_CONFIG5_APPS_CFGCPUPRESENT_N_7_BMSK                                            0x40000
#define HWIO_FEATURE_CONFIG5_APPS_CFGCPUPRESENT_N_7_SHFT                                               0x12
#define HWIO_FEATURE_CONFIG5_APPS_CFGCPUPRESENT_N_6_BMSK                                            0x20000
#define HWIO_FEATURE_CONFIG5_APPS_CFGCPUPRESENT_N_6_SHFT                                               0x11
#define HWIO_FEATURE_CONFIG5_APPS_CFGCPUPRESENT_N_5_BMSK                                            0x10000
#define HWIO_FEATURE_CONFIG5_APPS_CFGCPUPRESENT_N_5_SHFT                                               0x10
#define HWIO_FEATURE_CONFIG5_RSVD1_BMSK                                                              0x8000
#define HWIO_FEATURE_CONFIG5_RSVD1_SHFT                                                                 0xf
#define HWIO_FEATURE_CONFIG5_APPS_AARCH64_ENABLE_BMSK                                                0x4000
#define HWIO_FEATURE_CONFIG5_APPS_AARCH64_ENABLE_SHFT                                                   0xe
#define HWIO_FEATURE_CONFIG5_APPS_PBL_BOOT_SPEED_BMSK                                                0x3000
#define HWIO_FEATURE_CONFIG5_APPS_PBL_BOOT_SPEED_SHFT                                                   0xc
#define HWIO_FEATURE_CONFIG5_APPS_PBL_BOOT_SPEED_ENUM_800_MHZ_FVAL                                      0x0
#define HWIO_FEATURE_CONFIG5_APPS_PBL_BOOT_SPEED_ENUM_400_MHZ_FVAL                                      0x1
#define HWIO_FEATURE_CONFIG5_APPS_PBL_BOOT_SPEED_ENUM_200_MHZ_FVAL                                      0x2
#define HWIO_FEATURE_CONFIG5_APPS_PBL_BOOT_SPEED_XO_FVAL                                                0x3
#define HWIO_FEATURE_CONFIG5_APPS_BOOT_FROM_ROM_BMSK                                                  0x800
#define HWIO_FEATURE_CONFIG5_APPS_BOOT_FROM_ROM_SHFT                                                    0xb
#define HWIO_FEATURE_CONFIG5_RSVD0_BMSK                                                               0x700
#define HWIO_FEATURE_CONFIG5_RSVD0_SHFT                                                                 0x8
#define HWIO_FEATURE_CONFIG5_BOOT_ROM_PATCH_DISABLE_BMSK                                               0xe0
#define HWIO_FEATURE_CONFIG5_BOOT_ROM_PATCH_DISABLE_SHFT                                                0x5
#define HWIO_FEATURE_CONFIG5_TAP_GEN_SPARE_INSTR_DISABLE_36_32_BMSK                                    0x1f
#define HWIO_FEATURE_CONFIG5_TAP_GEN_SPARE_INSTR_DISABLE_36_32_SHFT                                     0x0

#define HWIO_OEM_CONFIG0_ADDR                                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006040)
#define HWIO_OEM_CONFIG0_RMSK                                                                    0xffffffff
#define HWIO_OEM_CONFIG0_IN          \
        in_dword_masked(HWIO_OEM_CONFIG0_ADDR, HWIO_OEM_CONFIG0_RMSK)
#define HWIO_OEM_CONFIG0_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG0_ADDR, m)
#define HWIO_OEM_CONFIG0_SHARED_QSEE_SPNIDEN_DISABLE_BMSK                                        0x80000000
#define HWIO_OEM_CONFIG0_SHARED_QSEE_SPNIDEN_DISABLE_SHFT                                              0x1f
#define HWIO_OEM_CONFIG0_SHARED_QSEE_SPNIDEN_DISABLE_ENABLE_FVAL                                        0x0
#define HWIO_OEM_CONFIG0_SHARED_QSEE_SPNIDEN_DISABLE_DISABLE_FVAL                                       0x1
#define HWIO_OEM_CONFIG0_SHARED_QSEE_SPIDEN_DISABLE_BMSK                                         0x40000000
#define HWIO_OEM_CONFIG0_SHARED_QSEE_SPIDEN_DISABLE_SHFT                                               0x1e
#define HWIO_OEM_CONFIG0_SHARED_QSEE_SPIDEN_DISABLE_ENABLE_FVAL                                         0x0
#define HWIO_OEM_CONFIG0_SHARED_QSEE_SPIDEN_DISABLE_DISABLE_FVAL                                        0x1
#define HWIO_OEM_CONFIG0_ALL_DEBUG_DISABLE_BMSK                                                  0x20000000
#define HWIO_OEM_CONFIG0_ALL_DEBUG_DISABLE_SHFT                                                        0x1d
#define HWIO_OEM_CONFIG0_ALL_DEBUG_DISABLE_ENABLE_FVAL                                                  0x0
#define HWIO_OEM_CONFIG0_ALL_DEBUG_DISABLE_DISABLE_FVAL                                                 0x1
#define HWIO_OEM_CONFIG0_DEBUG_POLICY_DISABLE_BMSK                                               0x10000000
#define HWIO_OEM_CONFIG0_DEBUG_POLICY_DISABLE_SHFT                                                     0x1c
#define HWIO_OEM_CONFIG0_DEBUG_POLICY_DISABLE_ENABLE_FVAL                                               0x0
#define HWIO_OEM_CONFIG0_DEBUG_POLICY_DISABLE_DISABLE_FVAL                                              0x1
#define HWIO_OEM_CONFIG0_RSVD3_BMSK                                                               0xc000000
#define HWIO_OEM_CONFIG0_RSVD3_SHFT                                                                    0x1a
#define HWIO_OEM_CONFIG0_DEBUG_DISABLE_IN_ROM_BMSK                                                0x2000000
#define HWIO_OEM_CONFIG0_DEBUG_DISABLE_IN_ROM_SHFT                                                     0x19
#define HWIO_OEM_CONFIG0_RSVD2_BMSK                                                               0x1000000
#define HWIO_OEM_CONFIG0_RSVD2_SHFT                                                                    0x18
#define HWIO_OEM_CONFIG0_APPS_HASH_INTEGRITY_CHECK_DISABLE_BMSK                                    0x800000
#define HWIO_OEM_CONFIG0_APPS_HASH_INTEGRITY_CHECK_DISABLE_SHFT                                        0x17
#define HWIO_OEM_CONFIG0_APPS_HASH_INTEGRITY_CHECK_DISABLE_ENABLE_FVAL                                  0x0
#define HWIO_OEM_CONFIG0_APPS_HASH_INTEGRITY_CHECK_DISABLE_DISABLE_FVAL                                 0x1
#define HWIO_OEM_CONFIG0_RSVD1B_BMSK                                                               0x400000
#define HWIO_OEM_CONFIG0_RSVD1B_SHFT                                                                   0x16
#define HWIO_OEM_CONFIG0_SW_ROT_USE_SERIAL_NUM_BMSK                                                0x200000
#define HWIO_OEM_CONFIG0_SW_ROT_USE_SERIAL_NUM_SHFT                                                    0x15
#define HWIO_OEM_CONFIG0_SW_ROT_USE_SERIAL_NUM_USE_OEM_ID_FVAL                                          0x0
#define HWIO_OEM_CONFIG0_SW_ROT_USE_SERIAL_NUM_USE_SERIAL_NUM_FVAL                                      0x1
#define HWIO_OEM_CONFIG0_DISABLE_ROT_TRANSFER_BMSK                                                 0x100000
#define HWIO_OEM_CONFIG0_DISABLE_ROT_TRANSFER_SHFT                                                     0x14
#define HWIO_OEM_CONFIG0_DISABLE_ROT_TRANSFER_ENABLE_FVAL                                               0x0
#define HWIO_OEM_CONFIG0_DISABLE_ROT_TRANSFER_DISABLE_FVAL                                              0x1
#define HWIO_OEM_CONFIG0_IMAGE_ENCRYPTION_ENABLE_BMSK                                               0x80000
#define HWIO_OEM_CONFIG0_IMAGE_ENCRYPTION_ENABLE_SHFT                                                  0x13
#define HWIO_OEM_CONFIG0_ROOT_CERT_TOTAL_NUM_BMSK                                                   0x60000
#define HWIO_OEM_CONFIG0_ROOT_CERT_TOTAL_NUM_SHFT                                                      0x11
#define HWIO_OEM_CONFIG0_RSVD1_BMSK                                                                 0x10000
#define HWIO_OEM_CONFIG0_RSVD1_SHFT                                                                    0x10
#define HWIO_OEM_CONFIG0_PBL_LOG_DISABLE_BMSK                                                        0x8000
#define HWIO_OEM_CONFIG0_PBL_LOG_DISABLE_SHFT                                                           0xf
#define HWIO_OEM_CONFIG0_WDOG_EN_BMSK                                                                0x4000
#define HWIO_OEM_CONFIG0_WDOG_EN_SHFT                                                                   0xe
#define HWIO_OEM_CONFIG0_WDOG_EN_USE_GPIO_FVAL                                                          0x0
#define HWIO_OEM_CONFIG0_WDOG_EN_IGNORE_GPIO_ENABLE_WDOG_FVAL                                           0x1
#define HWIO_OEM_CONFIG0_PBL_FDL_TIMEOUT_RESET_FEATURE_ENABLE_BMSK                                   0x2000
#define HWIO_OEM_CONFIG0_PBL_FDL_TIMEOUT_RESET_FEATURE_ENABLE_SHFT                                      0xd
#define HWIO_OEM_CONFIG0_PBL_FDL_TIMEOUT_RESET_FEATURE_ENABLE_DISABLE_EDL_RESET_FVAL                    0x0
#define HWIO_OEM_CONFIG0_PBL_FDL_TIMEOUT_RESET_FEATURE_ENABLE_ENABLE_EDL_RESET_FVAL                     0x1
#define HWIO_OEM_CONFIG0_SW_FUSE_PROG_DISABLE_BMSK                                                   0x1000
#define HWIO_OEM_CONFIG0_SW_FUSE_PROG_DISABLE_SHFT                                                      0xc
#define HWIO_OEM_CONFIG0_RSVD0_BMSK                                                                   0xf00
#define HWIO_OEM_CONFIG0_RSVD0_SHFT                                                                     0x8
#define HWIO_OEM_CONFIG0_FAST_BOOT_BMSK                                                                0xe0
#define HWIO_OEM_CONFIG0_FAST_BOOT_SHFT                                                                 0x5
#define HWIO_OEM_CONFIG0_FAST_BOOT_DEFAULT_FVAL                                                         0x0
#define HWIO_OEM_CONFIG0_SDCC_MCLK_BOOT_FREQ_BMSK                                                      0x10
#define HWIO_OEM_CONFIG0_SDCC_MCLK_BOOT_FREQ_SHFT                                                       0x4
#define HWIO_OEM_CONFIG0_SDCC_MCLK_BOOT_FREQ_ENUM_19_2_MHZ_FVAL                                         0x0
#define HWIO_OEM_CONFIG0_SDCC_MCLK_BOOT_FREQ_ENUM_25_MHZ_FVAL                                           0x1
#define HWIO_OEM_CONFIG0_FORCE_USB_BOOT_DISABLE_BMSK                                                    0x8
#define HWIO_OEM_CONFIG0_FORCE_USB_BOOT_DISABLE_SHFT                                                    0x3
#define HWIO_OEM_CONFIG0_FORCE_USB_BOOT_DISABLE_USE_FORCE_USB_BOOT_GPIO_TO_FORCE_BOOT_FROM_USB_FVAL        0x0
#define HWIO_OEM_CONFIG0_FORCE_USB_BOOT_DISABLE_NOT_USE_FORCE_USB_BOOT_PIN_FVAL                         0x1
#define HWIO_OEM_CONFIG0_FORCE_DLOAD_DISABLE_BMSK                                                       0x4
#define HWIO_OEM_CONFIG0_FORCE_DLOAD_DISABLE_SHFT                                                       0x2
#define HWIO_OEM_CONFIG0_FORCE_DLOAD_DISABLE_USE_FORCE_USB_BOOT_GPIO_TO_FORCE_BOOT_FROM_USB_FVAL        0x0
#define HWIO_OEM_CONFIG0_FORCE_DLOAD_DISABLE_NOT_USE_FORCE_USB_BOOT_PIN_FVAL                            0x1
#define HWIO_OEM_CONFIG0_ENUM_TIMEOUT_BMSK                                                              0x2
#define HWIO_OEM_CONFIG0_ENUM_TIMEOUT_SHFT                                                              0x1
#define HWIO_OEM_CONFIG0_ENUM_TIMEOUT_TIMEOUT_DISABLED_FVAL                                             0x0
#define HWIO_OEM_CONFIG0_ENUM_TIMEOUT_TIMEOUT_ENABLED_90S_FVAL                                          0x1
#define HWIO_OEM_CONFIG0_E_DLOAD_DISABLE_BMSK                                                           0x1
#define HWIO_OEM_CONFIG0_E_DLOAD_DISABLE_SHFT                                                           0x0
#define HWIO_OEM_CONFIG0_E_DLOAD_DISABLE_DOWNLOADER_ENABLED_FVAL                                        0x0
#define HWIO_OEM_CONFIG0_E_DLOAD_DISABLE_DOWNLOADER_DISABLED_FVAL                                       0x1

#define HWIO_OEM_CONFIG1_ADDR                                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006044)
#define HWIO_OEM_CONFIG1_RMSK                                                                    0xffffffff
#define HWIO_OEM_CONFIG1_IN          \
        in_dword_masked(HWIO_OEM_CONFIG1_ADDR, HWIO_OEM_CONFIG1_RMSK)
#define HWIO_OEM_CONFIG1_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG1_ADDR, m)
#define HWIO_OEM_CONFIG1_RSVD0_BMSK                                                              0xfffff000
#define HWIO_OEM_CONFIG1_RSVD0_SHFT                                                                     0xc
#define HWIO_OEM_CONFIG1_NAND_XFER_PARAM_BMSK                                                         0x800
#define HWIO_OEM_CONFIG1_NAND_XFER_PARAM_SHFT                                                           0xb
#define HWIO_OEM_CONFIG1_SHARED_MISC2_DEBUG_DISABLE_BMSK                                              0x400
#define HWIO_OEM_CONFIG1_SHARED_MISC2_DEBUG_DISABLE_SHFT                                                0xa
#define HWIO_OEM_CONFIG1_SHARED_MISC2_DEBUG_DISABLE_ENABLE_FVAL                                         0x0
#define HWIO_OEM_CONFIG1_SHARED_MISC2_DEBUG_DISABLE_DISABLE_FVAL                                        0x1
#define HWIO_OEM_CONFIG1_SHARED_MISC1_DEBUG_DISABLE_BMSK                                              0x200
#define HWIO_OEM_CONFIG1_SHARED_MISC1_DEBUG_DISABLE_SHFT                                                0x9
#define HWIO_OEM_CONFIG1_SHARED_MISC1_DEBUG_DISABLE_ENABLE_FVAL                                         0x0
#define HWIO_OEM_CONFIG1_SHARED_MISC1_DEBUG_DISABLE_DISABLE_FVAL                                        0x1
#define HWIO_OEM_CONFIG1_SHARED_MISC_DEBUG_DISABLE_BMSK                                               0x100
#define HWIO_OEM_CONFIG1_SHARED_MISC_DEBUG_DISABLE_SHFT                                                 0x8
#define HWIO_OEM_CONFIG1_SHARED_MISC_DEBUG_DISABLE_ENABLE_FVAL                                          0x0
#define HWIO_OEM_CONFIG1_SHARED_MISC_DEBUG_DISABLE_DISABLE_FVAL                                         0x1
#define HWIO_OEM_CONFIG1_APPS_NIDEN_DISABLE_BMSK                                                       0x80
#define HWIO_OEM_CONFIG1_APPS_NIDEN_DISABLE_SHFT                                                        0x7
#define HWIO_OEM_CONFIG1_APPS_NIDEN_DISABLE_ENABLE_FVAL                                                 0x0
#define HWIO_OEM_CONFIG1_APPS_NIDEN_DISABLE_DISABLE_FVAL                                                0x1
#define HWIO_OEM_CONFIG1_APPS_DBGEN_DISABLE_BMSK                                                       0x40
#define HWIO_OEM_CONFIG1_APPS_DBGEN_DISABLE_SHFT                                                        0x6
#define HWIO_OEM_CONFIG1_APPS_DBGEN_DISABLE_ENABLE_FVAL                                                 0x0
#define HWIO_OEM_CONFIG1_APPS_DBGEN_DISABLE_DISABLE_FVAL                                                0x1
#define HWIO_OEM_CONFIG1_SHARED_NS_NIDEN_DISABLE_BMSK                                                  0x20
#define HWIO_OEM_CONFIG1_SHARED_NS_NIDEN_DISABLE_SHFT                                                   0x5
#define HWIO_OEM_CONFIG1_SHARED_NS_NIDEN_DISABLE_ENABLE_FVAL                                            0x0
#define HWIO_OEM_CONFIG1_SHARED_NS_NIDEN_DISABLE_DISABLE_FVAL                                           0x1
#define HWIO_OEM_CONFIG1_SHARED_NS_DBGEN_DISABLE_BMSK                                                  0x10
#define HWIO_OEM_CONFIG1_SHARED_NS_DBGEN_DISABLE_SHFT                                                   0x4
#define HWIO_OEM_CONFIG1_SHARED_NS_DBGEN_DISABLE_ENABLE_FVAL                                            0x0
#define HWIO_OEM_CONFIG1_SHARED_NS_DBGEN_DISABLE_DISABLE_FVAL                                           0x1
#define HWIO_OEM_CONFIG1_SHARED_CP_NIDEN_DISABLE_BMSK                                                   0x8
#define HWIO_OEM_CONFIG1_SHARED_CP_NIDEN_DISABLE_SHFT                                                   0x3
#define HWIO_OEM_CONFIG1_SHARED_CP_NIDEN_DISABLE_ENABLE_FVAL                                            0x0
#define HWIO_OEM_CONFIG1_SHARED_CP_NIDEN_DISABLE_DISABLE_FVAL                                           0x1
#define HWIO_OEM_CONFIG1_SHARED_CP_DBGEN_DISABLE_BMSK                                                   0x4
#define HWIO_OEM_CONFIG1_SHARED_CP_DBGEN_DISABLE_SHFT                                                   0x2
#define HWIO_OEM_CONFIG1_SHARED_CP_DBGEN_DISABLE_ENABLE_FVAL                                            0x0
#define HWIO_OEM_CONFIG1_SHARED_CP_DBGEN_DISABLE_DISABLE_FVAL                                           0x1
#define HWIO_OEM_CONFIG1_SHARED_MSS_NIDEN_DISABLE_BMSK                                                  0x2
#define HWIO_OEM_CONFIG1_SHARED_MSS_NIDEN_DISABLE_SHFT                                                  0x1
#define HWIO_OEM_CONFIG1_SHARED_MSS_NIDEN_DISABLE_ENABLE_FVAL                                           0x0
#define HWIO_OEM_CONFIG1_SHARED_MSS_NIDEN_DISABLE_DISABLE_FVAL                                          0x1
#define HWIO_OEM_CONFIG1_SHARED_MSS_DBGEN_DISABLE_BMSK                                                  0x1
#define HWIO_OEM_CONFIG1_SHARED_MSS_DBGEN_DISABLE_SHFT                                                  0x0
#define HWIO_OEM_CONFIG1_SHARED_MSS_DBGEN_DISABLE_ENABLE_FVAL                                           0x0
#define HWIO_OEM_CONFIG1_SHARED_MSS_DBGEN_DISABLE_DISABLE_FVAL                                          0x1

#define HWIO_OEM_CONFIG2_ADDR                                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006048)
#define HWIO_OEM_CONFIG2_RMSK                                                                    0xffffffff
#define HWIO_OEM_CONFIG2_IN          \
        in_dword_masked(HWIO_OEM_CONFIG2_ADDR, HWIO_OEM_CONFIG2_RMSK)
#define HWIO_OEM_CONFIG2_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG2_ADDR, m)
#define HWIO_OEM_CONFIG2_DISABLE_RSA_BMSK                                                        0x80000000
#define HWIO_OEM_CONFIG2_DISABLE_RSA_SHFT                                                              0x1f
#define HWIO_OEM_CONFIG2_RSVD2_BMSK                                                              0x7fff8000
#define HWIO_OEM_CONFIG2_RSVD2_SHFT                                                                     0xf
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG27_SECURE_BMSK                                                 0x4000
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG27_SECURE_SHFT                                                    0xe
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG27_SECURE_NOT_SECURE_FVAL                                         0x0
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG27_SECURE_SECURE_FVAL                                             0x1
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG26_SECURE_BMSK                                                 0x2000
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG26_SECURE_SHFT                                                    0xd
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG26_SECURE_NOT_SECURE_FVAL                                         0x0
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG26_SECURE_SECURE_FVAL                                             0x1
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG25_SECURE_BMSK                                                 0x1000
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG25_SECURE_SHFT                                                    0xc
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG25_SECURE_NOT_SECURE_FVAL                                         0x0
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG25_SECURE_SECURE_FVAL                                             0x1
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG24_SECURE_BMSK                                                  0x800
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG24_SECURE_SHFT                                                    0xb
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG24_SECURE_NOT_SECURE_FVAL                                         0x0
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG24_SECURE_SECURE_FVAL                                             0x1
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG23_SECURE_BMSK                                                  0x400
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG23_SECURE_SHFT                                                    0xa
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG23_SECURE_NOT_SECURE_FVAL                                         0x0
#define HWIO_OEM_CONFIG2_OEM_SPARE_REG23_SECURE_SECURE_FVAL                                             0x1
#define HWIO_OEM_CONFIG2_RSVD1_BMSK                                                                   0x3f8
#define HWIO_OEM_CONFIG2_RSVD1_SHFT                                                                     0x3
#define HWIO_OEM_CONFIG2_RSVD0_BMSK                                                                     0x7
#define HWIO_OEM_CONFIG2_RSVD0_SHFT                                                                     0x0

#define HWIO_OEM_CONFIG3_ADDR                                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000604c)
#define HWIO_OEM_CONFIG3_RMSK                                                                    0xffffffff
#define HWIO_OEM_CONFIG3_IN          \
        in_dword_masked(HWIO_OEM_CONFIG3_ADDR, HWIO_OEM_CONFIG3_RMSK)
#define HWIO_OEM_CONFIG3_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG3_ADDR, m)
#define HWIO_OEM_CONFIG3_OEM_PRODUCT_ID_BMSK                                                     0xffff0000
#define HWIO_OEM_CONFIG3_OEM_PRODUCT_ID_SHFT                                                           0x10
#define HWIO_OEM_CONFIG3_OEM_HW_ID_BMSK                                                              0xffff
#define HWIO_OEM_CONFIG3_OEM_HW_ID_SHFT                                                                 0x0

#define HWIO_OEM_CONFIG4_ADDR                                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006050)
#define HWIO_OEM_CONFIG4_RMSK                                                                    0xffffffff
#define HWIO_OEM_CONFIG4_IN          \
        in_dword_masked(HWIO_OEM_CONFIG4_ADDR, HWIO_OEM_CONFIG4_RMSK)
#define HWIO_OEM_CONFIG4_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG4_ADDR, m)
#define HWIO_OEM_CONFIG4_PERIPH_VID_BMSK                                                         0xffff0000
#define HWIO_OEM_CONFIG4_PERIPH_VID_SHFT                                                               0x10
#define HWIO_OEM_CONFIG4_PERIPH_PID_BMSK                                                             0xffff
#define HWIO_OEM_CONFIG4_PERIPH_PID_SHFT                                                                0x0

#define HWIO_OEM_CONFIG5_ADDR                                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006054)
#define HWIO_OEM_CONFIG5_RMSK                                                                    0xffffffff
#define HWIO_OEM_CONFIG5_IN          \
        in_dword_masked(HWIO_OEM_CONFIG5_ADDR, HWIO_OEM_CONFIG5_RMSK)
#define HWIO_OEM_CONFIG5_INM(m)      \
        in_dword_masked(HWIO_OEM_CONFIG5_ADDR, m)
#define HWIO_OEM_CONFIG5_RSVD0_BMSK                                                              0xffffff00
#define HWIO_OEM_CONFIG5_RSVD0_SHFT                                                                     0x8
#define HWIO_OEM_CONFIG5_ANTI_ROLLBACK_FEATURE_EN_BMSK                                                 0xff
#define HWIO_OEM_CONFIG5_ANTI_ROLLBACK_FEATURE_EN_SHFT                                                  0x0

#define HWIO_BOOT_CONFIG_ADDR                                                                    (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006070)
#define HWIO_BOOT_CONFIG_RMSK                                                                          0xff
#define HWIO_BOOT_CONFIG_IN          \
        in_dword_masked(HWIO_BOOT_CONFIG_ADDR, HWIO_BOOT_CONFIG_RMSK)
#define HWIO_BOOT_CONFIG_INM(m)      \
        in_dword_masked(HWIO_BOOT_CONFIG_ADDR, m)
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_BMSK                                                      0xc0
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_SHFT                                                       0x6
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_XO_FVAL                                                    0x0
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_ENUM_200_MHZ_FVAL                                          0x1
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_ENUM_400_MHZ_FVAL                                          0x2
#define HWIO_BOOT_CONFIG_APPS_PBL_BOOT_SPEED_ENUM_800_MHZ_FVAL                                          0x3
#define HWIO_BOOT_CONFIG_APPS_BOOT_FROM_ROM_BMSK                                                       0x20
#define HWIO_BOOT_CONFIG_APPS_BOOT_FROM_ROM_SHFT                                                        0x5
#define HWIO_BOOT_CONFIG_RSVD0_BMSK                                                                    0x10
#define HWIO_BOOT_CONFIG_RSVD0_SHFT                                                                     0x4
#define HWIO_BOOT_CONFIG_FAST_BOOT_BMSK                                                                 0xe
#define HWIO_BOOT_CONFIG_FAST_BOOT_SHFT                                                                 0x1
#define HWIO_BOOT_CONFIG_FAST_BOOT_DEFAULT_FVAL                                                         0x0
#define HWIO_BOOT_CONFIG_WDOG_EN_BMSK                                                                   0x1
#define HWIO_BOOT_CONFIG_WDOG_EN_SHFT                                                                   0x0
#define HWIO_BOOT_CONFIG_WDOG_EN_DISABLE_WDOG_FVAL                                                      0x0
#define HWIO_BOOT_CONFIG_WDOG_EN_ENABLE_WDOG_FVAL                                                       0x1

#define HWIO_SECURE_BOOTn_ADDR(n)                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006078 + 0x4 * (n))
#define HWIO_SECURE_BOOTn_RMSK                                                                        0x1ff
#define HWIO_SECURE_BOOTn_MAXn                                                                           14
#define HWIO_SECURE_BOOTn_INI(n)        \
        in_dword_masked(HWIO_SECURE_BOOTn_ADDR(n), HWIO_SECURE_BOOTn_RMSK)
#define HWIO_SECURE_BOOTn_INMI(n,mask)    \
        in_dword_masked(HWIO_SECURE_BOOTn_ADDR(n), mask)
#define HWIO_SECURE_BOOTn_FUSE_SRC_BMSK                                                               0x100
#define HWIO_SECURE_BOOTn_FUSE_SRC_SHFT                                                                 0x8
#define HWIO_SECURE_BOOTn_FUSE_SRC_QUALCOMM_FVAL                                                        0x0
#define HWIO_SECURE_BOOTn_FUSE_SRC_OEM_FVAL                                                             0x1
#define HWIO_SECURE_BOOTn_RSVD_7_BMSK                                                                  0x80
#define HWIO_SECURE_BOOTn_RSVD_7_SHFT                                                                   0x7
#define HWIO_SECURE_BOOTn_USE_SERIAL_NUM_BMSK                                                          0x40
#define HWIO_SECURE_BOOTn_USE_SERIAL_NUM_SHFT                                                           0x6
#define HWIO_SECURE_BOOTn_USE_SERIAL_NUM_USE_OEM_ID_FVAL                                                0x0
#define HWIO_SECURE_BOOTn_USE_SERIAL_NUM_USE_SERIAL_NUM_FVAL                                            0x1
#define HWIO_SECURE_BOOTn_AUTH_EN_BMSK                                                                 0x20
#define HWIO_SECURE_BOOTn_AUTH_EN_SHFT                                                                  0x5
#define HWIO_SECURE_BOOTn_PK_HASH_IN_FUSE_BMSK                                                         0x10
#define HWIO_SECURE_BOOTn_PK_HASH_IN_FUSE_SHFT                                                          0x4
#define HWIO_SECURE_BOOTn_PK_HASH_IN_FUSE_SHA_384_HASH_OF_ROOT_CERTIFICATE_IS_IN_ROM_FVAL               0x0
#define HWIO_SECURE_BOOTn_PK_HASH_IN_FUSE_SHA_384_HASH_OF_ROOT_CERTIFICATE_IS_IN_PK_HASH_FVAL           0x1
#define HWIO_SECURE_BOOTn_ROM_PK_HASH_INDEX_BMSK                                                        0xf
#define HWIO_SECURE_BOOTn_ROM_PK_HASH_INDEX_SHFT                                                        0x0

#define HWIO_QSEE_INV_OVERRIDE_ADDR                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060c0)
#define HWIO_QSEE_INV_OVERRIDE_RMSK                                                              0xffffffff
#define HWIO_QSEE_INV_OVERRIDE_IN          \
        in_dword_masked(HWIO_QSEE_INV_OVERRIDE_ADDR, HWIO_QSEE_INV_OVERRIDE_RMSK)
#define HWIO_QSEE_INV_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_QSEE_INV_OVERRIDE_ADDR, m)
#define HWIO_QSEE_INV_OVERRIDE_OUT(v)      \
        out_dword(HWIO_QSEE_INV_OVERRIDE_ADDR,v)
#define HWIO_QSEE_INV_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QSEE_INV_OVERRIDE_ADDR,m,v,HWIO_QSEE_INV_OVERRIDE_IN)
#define HWIO_QSEE_INV_OVERRIDE_RSVD_31_1_BMSK                                                    0xfffffffe
#define HWIO_QSEE_INV_OVERRIDE_RSVD_31_1_SHFT                                                           0x1
#define HWIO_QSEE_INV_OVERRIDE_SHARED_QSEE_SPIDEN_DISABLE_BMSK                                          0x1
#define HWIO_QSEE_INV_OVERRIDE_SHARED_QSEE_SPIDEN_DISABLE_SHFT                                          0x0
#define HWIO_QSEE_INV_OVERRIDE_SHARED_QSEE_SPIDEN_DISABLE_FUSE_VALUE_FVAL                               0x0
#define HWIO_QSEE_INV_OVERRIDE_SHARED_QSEE_SPIDEN_DISABLE_QC_FUSE_VALUE_FVAL                            0x1

#define HWIO_QSEE_NI_OVERRIDE_ADDR                                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060c4)
#define HWIO_QSEE_NI_OVERRIDE_RMSK                                                               0xffffffff
#define HWIO_QSEE_NI_OVERRIDE_IN          \
        in_dword_masked(HWIO_QSEE_NI_OVERRIDE_ADDR, HWIO_QSEE_NI_OVERRIDE_RMSK)
#define HWIO_QSEE_NI_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_QSEE_NI_OVERRIDE_ADDR, m)
#define HWIO_QSEE_NI_OVERRIDE_OUT(v)      \
        out_dword(HWIO_QSEE_NI_OVERRIDE_ADDR,v)
#define HWIO_QSEE_NI_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QSEE_NI_OVERRIDE_ADDR,m,v,HWIO_QSEE_NI_OVERRIDE_IN)
#define HWIO_QSEE_NI_OVERRIDE_RSVD_31_1_BMSK                                                     0xfffffffe
#define HWIO_QSEE_NI_OVERRIDE_RSVD_31_1_SHFT                                                            0x1
#define HWIO_QSEE_NI_OVERRIDE_SHARED_QSEE_SPNIDEN_DISABLE_BMSK                                          0x1
#define HWIO_QSEE_NI_OVERRIDE_SHARED_QSEE_SPNIDEN_DISABLE_SHFT                                          0x0
#define HWIO_QSEE_NI_OVERRIDE_SHARED_QSEE_SPNIDEN_DISABLE_FUSE_VALUE_FVAL                               0x0
#define HWIO_QSEE_NI_OVERRIDE_SHARED_QSEE_SPNIDEN_DISABLE_QC_FUSE_VALUE_FVAL                            0x1

#define HWIO_MSS_INV_OVERRIDE_ADDR                                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060c8)
#define HWIO_MSS_INV_OVERRIDE_RMSK                                                               0xffffffff
#define HWIO_MSS_INV_OVERRIDE_IN          \
        in_dword_masked(HWIO_MSS_INV_OVERRIDE_ADDR, HWIO_MSS_INV_OVERRIDE_RMSK)
#define HWIO_MSS_INV_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_MSS_INV_OVERRIDE_ADDR, m)
#define HWIO_MSS_INV_OVERRIDE_OUT(v)      \
        out_dword(HWIO_MSS_INV_OVERRIDE_ADDR,v)
#define HWIO_MSS_INV_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_INV_OVERRIDE_ADDR,m,v,HWIO_MSS_INV_OVERRIDE_IN)
#define HWIO_MSS_INV_OVERRIDE_RSVD_31_1_BMSK                                                     0xfffffffe
#define HWIO_MSS_INV_OVERRIDE_RSVD_31_1_SHFT                                                            0x1
#define HWIO_MSS_INV_OVERRIDE_SHARED_MSS_DBGEN_DISABLE_BMSK                                             0x1
#define HWIO_MSS_INV_OVERRIDE_SHARED_MSS_DBGEN_DISABLE_SHFT                                             0x0
#define HWIO_MSS_INV_OVERRIDE_SHARED_MSS_DBGEN_DISABLE_FUSE_VALUE_FVAL                                  0x0
#define HWIO_MSS_INV_OVERRIDE_SHARED_MSS_DBGEN_DISABLE_QC_FUSE_VALUE_FVAL                               0x1

#define HWIO_MSS_NI_OVERRIDE_ADDR                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060cc)
#define HWIO_MSS_NI_OVERRIDE_RMSK                                                                0xffffffff
#define HWIO_MSS_NI_OVERRIDE_IN          \
        in_dword_masked(HWIO_MSS_NI_OVERRIDE_ADDR, HWIO_MSS_NI_OVERRIDE_RMSK)
#define HWIO_MSS_NI_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_MSS_NI_OVERRIDE_ADDR, m)
#define HWIO_MSS_NI_OVERRIDE_OUT(v)      \
        out_dword(HWIO_MSS_NI_OVERRIDE_ADDR,v)
#define HWIO_MSS_NI_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_NI_OVERRIDE_ADDR,m,v,HWIO_MSS_NI_OVERRIDE_IN)
#define HWIO_MSS_NI_OVERRIDE_RSVD_31_1_BMSK                                                      0xfffffffe
#define HWIO_MSS_NI_OVERRIDE_RSVD_31_1_SHFT                                                             0x1
#define HWIO_MSS_NI_OVERRIDE_SHARED_MSS_NIDEN_DISABLE_BMSK                                              0x1
#define HWIO_MSS_NI_OVERRIDE_SHARED_MSS_NIDEN_DISABLE_SHFT                                              0x0
#define HWIO_MSS_NI_OVERRIDE_SHARED_MSS_NIDEN_DISABLE_FUSE_VALUE_FVAL                                   0x0
#define HWIO_MSS_NI_OVERRIDE_SHARED_MSS_NIDEN_DISABLE_QC_FUSE_VALUE_FVAL                                0x1

#define HWIO_CP_INV_OVERRIDE_ADDR                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060d0)
#define HWIO_CP_INV_OVERRIDE_RMSK                                                                0xffffffff
#define HWIO_CP_INV_OVERRIDE_IN          \
        in_dword_masked(HWIO_CP_INV_OVERRIDE_ADDR, HWIO_CP_INV_OVERRIDE_RMSK)
#define HWIO_CP_INV_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_CP_INV_OVERRIDE_ADDR, m)
#define HWIO_CP_INV_OVERRIDE_OUT(v)      \
        out_dword(HWIO_CP_INV_OVERRIDE_ADDR,v)
#define HWIO_CP_INV_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CP_INV_OVERRIDE_ADDR,m,v,HWIO_CP_INV_OVERRIDE_IN)
#define HWIO_CP_INV_OVERRIDE_RSVD_31_1_BMSK                                                      0xfffffffe
#define HWIO_CP_INV_OVERRIDE_RSVD_31_1_SHFT                                                             0x1
#define HWIO_CP_INV_OVERRIDE_SHARED_CP_DBGEN_DISABLE_BMSK                                               0x1
#define HWIO_CP_INV_OVERRIDE_SHARED_CP_DBGEN_DISABLE_SHFT                                               0x0
#define HWIO_CP_INV_OVERRIDE_SHARED_CP_DBGEN_DISABLE_FUSE_VALUE_FVAL                                    0x0
#define HWIO_CP_INV_OVERRIDE_SHARED_CP_DBGEN_DISABLE_QC_FUSE_VALUE_FVAL                                 0x1

#define HWIO_CP_NI_OVERRIDE_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060d4)
#define HWIO_CP_NI_OVERRIDE_RMSK                                                                 0xffffffff
#define HWIO_CP_NI_OVERRIDE_IN          \
        in_dword_masked(HWIO_CP_NI_OVERRIDE_ADDR, HWIO_CP_NI_OVERRIDE_RMSK)
#define HWIO_CP_NI_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_CP_NI_OVERRIDE_ADDR, m)
#define HWIO_CP_NI_OVERRIDE_OUT(v)      \
        out_dword(HWIO_CP_NI_OVERRIDE_ADDR,v)
#define HWIO_CP_NI_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CP_NI_OVERRIDE_ADDR,m,v,HWIO_CP_NI_OVERRIDE_IN)
#define HWIO_CP_NI_OVERRIDE_RSVD_31_1_BMSK                                                       0xfffffffe
#define HWIO_CP_NI_OVERRIDE_RSVD_31_1_SHFT                                                              0x1
#define HWIO_CP_NI_OVERRIDE_SHARED_CP_NIDEN_DISABLE_BMSK                                                0x1
#define HWIO_CP_NI_OVERRIDE_SHARED_CP_NIDEN_DISABLE_SHFT                                                0x0
#define HWIO_CP_NI_OVERRIDE_SHARED_CP_NIDEN_DISABLE_FUSE_VALUE_FVAL                                     0x0
#define HWIO_CP_NI_OVERRIDE_SHARED_CP_NIDEN_DISABLE_QC_FUSE_VALUE_FVAL                                  0x1

#define HWIO_NS_INV_OVERRIDE_ADDR                                                                (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060d8)
#define HWIO_NS_INV_OVERRIDE_RMSK                                                                0xffffffff
#define HWIO_NS_INV_OVERRIDE_IN          \
        in_dword_masked(HWIO_NS_INV_OVERRIDE_ADDR, HWIO_NS_INV_OVERRIDE_RMSK)
#define HWIO_NS_INV_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_NS_INV_OVERRIDE_ADDR, m)
#define HWIO_NS_INV_OVERRIDE_OUT(v)      \
        out_dword(HWIO_NS_INV_OVERRIDE_ADDR,v)
#define HWIO_NS_INV_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NS_INV_OVERRIDE_ADDR,m,v,HWIO_NS_INV_OVERRIDE_IN)
#define HWIO_NS_INV_OVERRIDE_RSVD_31_5_BMSK                                                      0xffffffe0
#define HWIO_NS_INV_OVERRIDE_RSVD_31_5_SHFT                                                             0x5
#define HWIO_NS_INV_OVERRIDE_APPS_DBGEN_DISABLE_BMSK                                                   0x10
#define HWIO_NS_INV_OVERRIDE_APPS_DBGEN_DISABLE_SHFT                                                    0x4
#define HWIO_NS_INV_OVERRIDE_APPS_DBGEN_DISABLE_FUSE_VALUE_FVAL                                         0x0
#define HWIO_NS_INV_OVERRIDE_APPS_DBGEN_DISABLE_QC_FUSE_VALUE_FVAL                                      0x1
#define HWIO_NS_INV_OVERRIDE_RSVD_3_1_BMSK                                                              0xe
#define HWIO_NS_INV_OVERRIDE_RSVD_3_1_SHFT                                                              0x1
#define HWIO_NS_INV_OVERRIDE_SHARED_NS_DBGEN_DISABLE_BMSK                                               0x1
#define HWIO_NS_INV_OVERRIDE_SHARED_NS_DBGEN_DISABLE_SHFT                                               0x0
#define HWIO_NS_INV_OVERRIDE_SHARED_NS_DBGEN_DISABLE_FUSE_VALUE_FVAL                                    0x0
#define HWIO_NS_INV_OVERRIDE_SHARED_NS_DBGEN_DISABLE_QC_FUSE_VALUE_FVAL                                 0x1

#define HWIO_NS_NI_OVERRIDE_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060dc)
#define HWIO_NS_NI_OVERRIDE_RMSK                                                                 0xffffffff
#define HWIO_NS_NI_OVERRIDE_IN          \
        in_dword_masked(HWIO_NS_NI_OVERRIDE_ADDR, HWIO_NS_NI_OVERRIDE_RMSK)
#define HWIO_NS_NI_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_NS_NI_OVERRIDE_ADDR, m)
#define HWIO_NS_NI_OVERRIDE_OUT(v)      \
        out_dword(HWIO_NS_NI_OVERRIDE_ADDR,v)
#define HWIO_NS_NI_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_NS_NI_OVERRIDE_ADDR,m,v,HWIO_NS_NI_OVERRIDE_IN)
#define HWIO_NS_NI_OVERRIDE_RSVD_31_5_BMSK                                                       0xffffffe0
#define HWIO_NS_NI_OVERRIDE_RSVD_31_5_SHFT                                                              0x5
#define HWIO_NS_NI_OVERRIDE_APPS_NIDEN_DISABLE_BMSK                                                    0x10
#define HWIO_NS_NI_OVERRIDE_APPS_NIDEN_DISABLE_SHFT                                                     0x4
#define HWIO_NS_NI_OVERRIDE_APPS_NIDEN_DISABLE_FUSE_VALUE_FVAL                                          0x0
#define HWIO_NS_NI_OVERRIDE_APPS_NIDEN_DISABLE_QC_FUSE_VALUE_FVAL                                       0x1
#define HWIO_NS_NI_OVERRIDE_RSVD_3_1_BMSK                                                               0xe
#define HWIO_NS_NI_OVERRIDE_RSVD_3_1_SHFT                                                               0x1
#define HWIO_NS_NI_OVERRIDE_SHARED_NS_NIDEN_DISABLE_BMSK                                                0x1
#define HWIO_NS_NI_OVERRIDE_SHARED_NS_NIDEN_DISABLE_SHFT                                                0x0
#define HWIO_NS_NI_OVERRIDE_SHARED_NS_NIDEN_DISABLE_FUSE_VALUE_FVAL                                     0x0
#define HWIO_NS_NI_OVERRIDE_SHARED_NS_NIDEN_DISABLE_QC_FUSE_VALUE_FVAL                                  0x1

#define HWIO_MISC_DEBUG_OVERRIDE_ADDR                                                            (SECURITY_CONTROL_CORE_REG_BASE      + 0x000060e0)
#define HWIO_MISC_DEBUG_OVERRIDE_RMSK                                                            0xffffffff
#define HWIO_MISC_DEBUG_OVERRIDE_IN          \
        in_dword_masked(HWIO_MISC_DEBUG_OVERRIDE_ADDR, HWIO_MISC_DEBUG_OVERRIDE_RMSK)
#define HWIO_MISC_DEBUG_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_MISC_DEBUG_OVERRIDE_ADDR, m)
#define HWIO_MISC_DEBUG_OVERRIDE_OUT(v)      \
        out_dword(HWIO_MISC_DEBUG_OVERRIDE_ADDR,v)
#define HWIO_MISC_DEBUG_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MISC_DEBUG_OVERRIDE_ADDR,m,v,HWIO_MISC_DEBUG_OVERRIDE_IN)
#define HWIO_MISC_DEBUG_OVERRIDE_RSVD_31_3_BMSK                                                  0xfffffff8
#define HWIO_MISC_DEBUG_OVERRIDE_RSVD_31_3_SHFT                                                         0x3
#define HWIO_MISC_DEBUG_OVERRIDE_SHARED_MISC2_DEBUG_DISABLE_BMSK                                        0x4
#define HWIO_MISC_DEBUG_OVERRIDE_SHARED_MISC2_DEBUG_DISABLE_SHFT                                        0x2
#define HWIO_MISC_DEBUG_OVERRIDE_SHARED_MISC2_DEBUG_DISABLE_FUSE_VALUE_FVAL                             0x0
#define HWIO_MISC_DEBUG_OVERRIDE_SHARED_MISC2_DEBUG_DISABLE_ZERO_FVAL                                   0x1
#define HWIO_MISC_DEBUG_OVERRIDE_SHARED_MISC1_DEBUG_DISABLE_BMSK                                        0x2
#define HWIO_MISC_DEBUG_OVERRIDE_SHARED_MISC1_DEBUG_DISABLE_SHFT                                        0x1
#define HWIO_MISC_DEBUG_OVERRIDE_SHARED_MISC1_DEBUG_DISABLE_FUSE_VALUE_FVAL                             0x0
#define HWIO_MISC_DEBUG_OVERRIDE_SHARED_MISC1_DEBUG_DISABLE_ZERO_FVAL                                   0x1
#define HWIO_MISC_DEBUG_OVERRIDE_SHARED_MISC_DEBUG_DISABLE_BMSK                                         0x1
#define HWIO_MISC_DEBUG_OVERRIDE_SHARED_MISC_DEBUG_DISABLE_SHFT                                         0x0
#define HWIO_MISC_DEBUG_OVERRIDE_SHARED_MISC_DEBUG_DISABLE_FUSE_VALUE_FVAL                              0x0
#define HWIO_MISC_DEBUG_OVERRIDE_SHARED_MISC_DEBUG_DISABLE_QC_FUSE_VALUE_FVAL                           0x1

#define HWIO_CAPT_SEC_GPIO_ADDR                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006100)
#define HWIO_CAPT_SEC_GPIO_RMSK                                                                      0xffff
#define HWIO_CAPT_SEC_GPIO_IN          \
        in_dword_masked(HWIO_CAPT_SEC_GPIO_ADDR, HWIO_CAPT_SEC_GPIO_RMSK)
#define HWIO_CAPT_SEC_GPIO_INM(m)      \
        in_dword_masked(HWIO_CAPT_SEC_GPIO_ADDR, m)
#define HWIO_CAPT_SEC_GPIO_OUT(v)      \
        out_dword(HWIO_CAPT_SEC_GPIO_ADDR,v)
#define HWIO_CAPT_SEC_GPIO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_CAPT_SEC_GPIO_ADDR,m,v,HWIO_CAPT_SEC_GPIO_IN)
#define HWIO_CAPT_SEC_GPIO_FORCE_USB_BOOT_GPIO_BMSK                                                  0x8000
#define HWIO_CAPT_SEC_GPIO_FORCE_USB_BOOT_GPIO_SHFT                                                     0xf
#define HWIO_CAPT_SEC_GPIO_RSVD2_BMSK                                                                0x4000
#define HWIO_CAPT_SEC_GPIO_RSVD2_SHFT                                                                   0xe
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_AP_AUTH_EN_BMSK                                          0x2000
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_AP_AUTH_EN_SHFT                                             0xd
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_AP_PK_HASH_IN_FUSE_BMSK                                  0x1000
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_AP_PK_HASH_IN_FUSE_SHFT                                     0xc
#define HWIO_CAPT_SEC_GPIO_RSVD1_BMSK                                                                 0xc00
#define HWIO_CAPT_SEC_GPIO_RSVD1_SHFT                                                                   0xa
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_ALL_USE_SERIAL_NUM_BMSK                                   0x200
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_ALL_USE_SERIAL_NUM_SHFT                                     0x9
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_PK_HASH_INDEX_SRC_BMSK                                    0x100
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_PK_HASH_INDEX_SRC_SHFT                                      0x8
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_APPS_PBL_BOOT_SPEED_BMSK                                   0xc0
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_APPS_PBL_BOOT_SPEED_SHFT                                    0x6
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_APPS_BOOT_FROM_ROM_BMSK                                    0x20
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_APPS_BOOT_FROM_ROM_SHFT                                     0x5
#define HWIO_CAPT_SEC_GPIO_RSVD0_BMSK                                                                  0x10
#define HWIO_CAPT_SEC_GPIO_RSVD0_SHFT                                                                   0x4
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_FAST_BOOT_BMSK                                              0xe
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_FAST_BOOT_SHFT                                              0x1
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_WDOG_DISABLE_BMSK                                           0x1
#define HWIO_CAPT_SEC_GPIO_BOOT_CONFIG_GPIO_WDOG_DISABLE_SHFT                                           0x0

#define HWIO_APP_PROC_CFG_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006110)
#define HWIO_APP_PROC_CFG_RMSK                                                                         0x1f
#define HWIO_APP_PROC_CFG_IN          \
        in_dword_masked(HWIO_APP_PROC_CFG_ADDR, HWIO_APP_PROC_CFG_RMSK)
#define HWIO_APP_PROC_CFG_INM(m)      \
        in_dword_masked(HWIO_APP_PROC_CFG_ADDR, m)
#define HWIO_APP_PROC_CFG_OUT(v)      \
        out_dword(HWIO_APP_PROC_CFG_ADDR,v)
#define HWIO_APP_PROC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APP_PROC_CFG_ADDR,m,v,HWIO_APP_PROC_CFG_IN)
#define HWIO_APP_PROC_CFG_APPS_CP15_DISABLE_BMSK                                                       0x10
#define HWIO_APP_PROC_CFG_APPS_CP15_DISABLE_SHFT                                                        0x4
#define HWIO_APP_PROC_CFG_SHARED_QSEE_SPNIDEN_BMSK                                                      0x8
#define HWIO_APP_PROC_CFG_SHARED_QSEE_SPNIDEN_SHFT                                                      0x3
#define HWIO_APP_PROC_CFG_SHARED_CP_NIDEN_BMSK                                                          0x4
#define HWIO_APP_PROC_CFG_SHARED_CP_NIDEN_SHFT                                                          0x2
#define HWIO_APP_PROC_CFG_SHARED_NS_NIDEN_BMSK                                                          0x2
#define HWIO_APP_PROC_CFG_SHARED_NS_NIDEN_SHFT                                                          0x1
#define HWIO_APP_PROC_CFG_APPS_NIDEN_BMSK                                                               0x1
#define HWIO_APP_PROC_CFG_APPS_NIDEN_SHFT                                                               0x0

#define HWIO_MSS_PROC_CFG_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006114)
#define HWIO_MSS_PROC_CFG_RMSK                                                                          0x1
#define HWIO_MSS_PROC_CFG_IN          \
        in_dword_masked(HWIO_MSS_PROC_CFG_ADDR, HWIO_MSS_PROC_CFG_RMSK)
#define HWIO_MSS_PROC_CFG_INM(m)      \
        in_dword_masked(HWIO_MSS_PROC_CFG_ADDR, m)
#define HWIO_MSS_PROC_CFG_OUT(v)      \
        out_dword(HWIO_MSS_PROC_CFG_ADDR,v)
#define HWIO_MSS_PROC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MSS_PROC_CFG_ADDR,m,v,HWIO_MSS_PROC_CFG_IN)
#define HWIO_MSS_PROC_CFG_SHARED_MSS_NIDEN_BMSK                                                         0x1
#define HWIO_MSS_PROC_CFG_SHARED_MSS_NIDEN_SHFT                                                         0x0

#define HWIO_QFPROM_CLK_CTL_ADDR                                                                 (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006118)
#define HWIO_QFPROM_CLK_CTL_RMSK                                                                        0x1
#define HWIO_QFPROM_CLK_CTL_IN          \
        in_dword_masked(HWIO_QFPROM_CLK_CTL_ADDR, HWIO_QFPROM_CLK_CTL_RMSK)
#define HWIO_QFPROM_CLK_CTL_INM(m)      \
        in_dword_masked(HWIO_QFPROM_CLK_CTL_ADDR, m)
#define HWIO_QFPROM_CLK_CTL_OUT(v)      \
        out_dword(HWIO_QFPROM_CLK_CTL_ADDR,v)
#define HWIO_QFPROM_CLK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_CLK_CTL_ADDR,m,v,HWIO_QFPROM_CLK_CTL_IN)
#define HWIO_QFPROM_CLK_CTL_CLK_HALT_BMSK                                                               0x1
#define HWIO_QFPROM_CLK_CTL_CLK_HALT_SHFT                                                               0x0
#define HWIO_QFPROM_CLK_CTL_CLK_HALT_CLK_ENABLED_FVAL                                                   0x0
#define HWIO_QFPROM_CLK_CTL_CLK_HALT_CLK_DISABLED_FVAL                                                  0x1

#define HWIO_JTAG_ID_ADDR                                                                        (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006130)
#define HWIO_JTAG_ID_RMSK                                                                        0xffffffff
#define HWIO_JTAG_ID_IN          \
        in_dword_masked(HWIO_JTAG_ID_ADDR, HWIO_JTAG_ID_RMSK)
#define HWIO_JTAG_ID_INM(m)      \
        in_dword_masked(HWIO_JTAG_ID_ADDR, m)
#define HWIO_JTAG_ID_JTAG_ID_BMSK                                                                0xffffffff
#define HWIO_JTAG_ID_JTAG_ID_SHFT                                                                       0x0

#define HWIO_SERIAL_NUM_ADDR                                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006134)
#define HWIO_SERIAL_NUM_RMSK                                                                     0xffffffff
#define HWIO_SERIAL_NUM_IN          \
        in_dword_masked(HWIO_SERIAL_NUM_ADDR, HWIO_SERIAL_NUM_RMSK)
#define HWIO_SERIAL_NUM_INM(m)      \
        in_dword_masked(HWIO_SERIAL_NUM_ADDR, m)
#define HWIO_SERIAL_NUM_SERIAL_NUM_BMSK                                                          0xffffffff
#define HWIO_SERIAL_NUM_SERIAL_NUM_SHFT                                                                 0x0

#define HWIO_OEM_ID_ADDR                                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006138)
#define HWIO_OEM_ID_RMSK                                                                         0xffffffff
#define HWIO_OEM_ID_IN          \
        in_dword_masked(HWIO_OEM_ID_ADDR, HWIO_OEM_ID_RMSK)
#define HWIO_OEM_ID_INM(m)      \
        in_dword_masked(HWIO_OEM_ID_ADDR, m)
#define HWIO_OEM_ID_OEM_ID_BMSK                                                                  0xffff0000
#define HWIO_OEM_ID_OEM_ID_SHFT                                                                        0x10
#define HWIO_OEM_ID_OEM_PRODUCT_ID_BMSK                                                              0xffff
#define HWIO_OEM_ID_OEM_PRODUCT_ID_SHFT                                                                 0x0

#define HWIO_TEST_BUS_SEL_ADDR                                                                   (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000613c)
#define HWIO_TEST_BUS_SEL_RMSK                                                                          0x7
#define HWIO_TEST_BUS_SEL_IN          \
        in_dword_masked(HWIO_TEST_BUS_SEL_ADDR, HWIO_TEST_BUS_SEL_RMSK)
#define HWIO_TEST_BUS_SEL_INM(m)      \
        in_dword_masked(HWIO_TEST_BUS_SEL_ADDR, m)
#define HWIO_TEST_BUS_SEL_OUT(v)      \
        out_dword(HWIO_TEST_BUS_SEL_ADDR,v)
#define HWIO_TEST_BUS_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TEST_BUS_SEL_ADDR,m,v,HWIO_TEST_BUS_SEL_IN)
#define HWIO_TEST_BUS_SEL_TEST_EN_BMSK                                                                  0x4
#define HWIO_TEST_BUS_SEL_TEST_EN_SHFT                                                                  0x2
#define HWIO_TEST_BUS_SEL_TEST_EN_DEBUG_DISABLED_FVAL                                                   0x0
#define HWIO_TEST_BUS_SEL_TEST_EN_DEBUG_ENABLED_FVAL                                                    0x1
#define HWIO_TEST_BUS_SEL_TEST_SELECT_BMSK                                                              0x3
#define HWIO_TEST_BUS_SEL_TEST_SELECT_SHFT                                                              0x0
#define HWIO_TEST_BUS_SEL_TEST_SELECT_FUSE_SENSE_FVAL                                                   0x0
#define HWIO_TEST_BUS_SEL_TEST_SELECT_QFPROM_ARBITER_FVAL                                               0x1
#define HWIO_TEST_BUS_SEL_TEST_SELECT_SW_FVAL                                                           0x2
#define HWIO_TEST_BUS_SEL_TEST_SELECT_ACC_FVAL                                                          0x3

#define HWIO_SPDM_DYN_SECURE_MODE_ADDR                                                           (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006140)
#define HWIO_SPDM_DYN_SECURE_MODE_RMSK                                                                  0x1
#define HWIO_SPDM_DYN_SECURE_MODE_IN          \
        in_dword_masked(HWIO_SPDM_DYN_SECURE_MODE_ADDR, HWIO_SPDM_DYN_SECURE_MODE_RMSK)
#define HWIO_SPDM_DYN_SECURE_MODE_INM(m)      \
        in_dword_masked(HWIO_SPDM_DYN_SECURE_MODE_ADDR, m)
#define HWIO_SPDM_DYN_SECURE_MODE_OUT(v)      \
        out_dword(HWIO_SPDM_DYN_SECURE_MODE_ADDR,v)
#define HWIO_SPDM_DYN_SECURE_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_SPDM_DYN_SECURE_MODE_ADDR,m,v,HWIO_SPDM_DYN_SECURE_MODE_IN)
#define HWIO_SPDM_DYN_SECURE_MODE_SECURE_MODE_BMSK                                                      0x1
#define HWIO_SPDM_DYN_SECURE_MODE_SECURE_MODE_SHFT                                                      0x0
#define HWIO_SPDM_DYN_SECURE_MODE_SECURE_MODE_DISABLE_FVAL                                              0x0
#define HWIO_SPDM_DYN_SECURE_MODE_SECURE_MODE_ENABLE_FVAL                                               0x1

#define HWIO_OEM_IMAGE_ENCR_KEYn_ADDR(n)                                                         (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006150 + 0x4 * (n))
#define HWIO_OEM_IMAGE_ENCR_KEYn_RMSK                                                            0xffffffff
#define HWIO_OEM_IMAGE_ENCR_KEYn_MAXn                                                                     3
#define HWIO_OEM_IMAGE_ENCR_KEYn_INI(n)        \
        in_dword_masked(HWIO_OEM_IMAGE_ENCR_KEYn_ADDR(n), HWIO_OEM_IMAGE_ENCR_KEYn_RMSK)
#define HWIO_OEM_IMAGE_ENCR_KEYn_INMI(n,mask)    \
        in_dword_masked(HWIO_OEM_IMAGE_ENCR_KEYn_ADDR(n), mask)
#define HWIO_OEM_IMAGE_ENCR_KEYn_KEY_DATA0_BMSK                                                  0xffffffff
#define HWIO_OEM_IMAGE_ENCR_KEYn_KEY_DATA0_SHFT                                                         0x0

#define HWIO_PK_HASH0_n_ADDR(n)                                                                  (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006190 + 0x4 * (n))
#define HWIO_PK_HASH0_n_RMSK                                                                     0xffffffff
#define HWIO_PK_HASH0_n_MAXn                                                                             11
#define HWIO_PK_HASH0_n_INI(n)        \
        in_dword_masked(HWIO_PK_HASH0_n_ADDR(n), HWIO_PK_HASH0_n_RMSK)
#define HWIO_PK_HASH0_n_INMI(n,mask)    \
        in_dword_masked(HWIO_PK_HASH0_n_ADDR(n), mask)
#define HWIO_PK_HASH0_n_HASH_DATA0_BMSK                                                          0xffffffff
#define HWIO_PK_HASH0_n_HASH_DATA0_SHFT                                                                 0x0

#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x000061f0)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_RMSK                                               0xffffffff
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_IN          \
        in_dword_masked(HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_ADDR, HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_RMSK)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_INM(m)      \
        in_dword_masked(HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_ADDR, m)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_OUT(v)      \
        out_dword(HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_ADDR,v)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_ADDR,m,v,HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_IN)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_RSVD0_BMSK                                         0xf0000000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_RSVD0_SHFT                                               0x1c
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION27_STICKY_BIT_BMSK                            0x8000000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION27_STICKY_BIT_SHFT                                 0x1b
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION27_STICKY_BIT_ALLOW_WRITE_FVAL                      0x0
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION27_STICKY_BIT_DISABLE_WRITE_FVAL                    0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION26_STICKY_BIT_BMSK                            0x4000000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION26_STICKY_BIT_SHFT                                 0x1a
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION26_STICKY_BIT_ALLOW_WRITE_FVAL                      0x0
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION26_STICKY_BIT_DISABLE_WRITE_FVAL                    0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION25_STICKY_BIT_BMSK                            0x2000000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION25_STICKY_BIT_SHFT                                 0x19
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION25_STICKY_BIT_ALLOW_WRITE_FVAL                      0x0
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION25_STICKY_BIT_DISABLE_WRITE_FVAL                    0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION24_STICKY_BIT_BMSK                            0x1000000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION24_STICKY_BIT_SHFT                                 0x18
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION24_STICKY_BIT_ALLOW_WRITE_FVAL                      0x0
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION24_STICKY_BIT_DISABLE_WRITE_FVAL                    0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION23_STICKY_BIT_BMSK                             0x800000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION23_STICKY_BIT_SHFT                                 0x17
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION23_STICKY_BIT_ALLOW_WRITE_FVAL                      0x0
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION23_STICKY_BIT_DISABLE_WRITE_FVAL                    0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION22_STICKY_BIT_BMSK                             0x400000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION22_STICKY_BIT_SHFT                                 0x16
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION22_STICKY_BIT_ALLOW_WRITE_FVAL                      0x0
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION22_STICKY_BIT_DISABLE_WRITE_FVAL                    0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION21_STICKY_BIT_BMSK                             0x200000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION21_STICKY_BIT_SHFT                                 0x15
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION21_STICKY_BIT_ALLOW_WRITE_FVAL                      0x0
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION21_STICKY_BIT_DISABLE_WRITE_FVAL                    0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION20_STICKY_BIT_BMSK                             0x100000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION20_STICKY_BIT_SHFT                                 0x14
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION20_STICKY_BIT_ALLOW_WRITE_FVAL                      0x0
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION20_STICKY_BIT_DISABLE_WRITE_FVAL                    0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION19_STICKY_BIT_BMSK                              0x80000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION19_STICKY_BIT_SHFT                                 0x13
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION19_STICKY_BIT_ALLOW_WRITE_FVAL                      0x0
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION19_STICKY_BIT_DISABLE_WRITE_FVAL                    0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION18_STICKY_BIT_BMSK                              0x40000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION18_STICKY_BIT_SHFT                                 0x12
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION18_STICKY_BIT_ALLOW_WRITE_FVAL                      0x0
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION18_STICKY_BIT_DISABLE_WRITE_FVAL                    0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION17_STICKY_BIT_BMSK                              0x20000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION17_STICKY_BIT_SHFT                                 0x11
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION17_STICKY_BIT_ALLOW_WRITE_FVAL                      0x0
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION17_STICKY_BIT_DISABLE_WRITE_FVAL                    0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION16_STICKY_BIT_BMSK                              0x10000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION16_STICKY_BIT_SHFT                                 0x10
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION16_STICKY_BIT_ALLOW_WRITE_FVAL                      0x0
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION16_STICKY_BIT_DISABLE_WRITE_FVAL                    0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION15_STICKY_BIT_BMSK                               0x8000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION15_STICKY_BIT_SHFT                                  0xf
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION15_STICKY_BIT_ALLOW_WRITE_FVAL                      0x0
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION15_STICKY_BIT_DISABLE_WRITE_FVAL                    0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION14_STICKY_BIT_BMSK                               0x4000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION14_STICKY_BIT_SHFT                                  0xe
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION14_STICKY_BIT_ALLOW_WRITE_FVAL                      0x0
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION14_STICKY_BIT_DISABLE_WRITE_FVAL                    0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION13_STICKY_BIT_BMSK                               0x2000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION13_STICKY_BIT_SHFT                                  0xd
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION13_STICKY_BIT_ALLOW_WRITE_FVAL                      0x0
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION13_STICKY_BIT_DISABLE_WRITE_FVAL                    0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION12_STICKY_BIT_BMSK                               0x1000
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION12_STICKY_BIT_SHFT                                  0xc
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION12_STICKY_BIT_ALLOW_WRITE_FVAL                      0x0
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION12_STICKY_BIT_DISABLE_WRITE_FVAL                    0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION11_STICKY_BIT_BMSK                                0x800
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION11_STICKY_BIT_SHFT                                  0xb
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION11_STICKY_BIT_ALLOW_WRITE_FVAL                      0x0
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION11_STICKY_BIT_DISABLE_WRITE_FVAL                    0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION10_STICKY_BIT_BMSK                                0x400
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION10_STICKY_BIT_SHFT                                  0xa
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION10_STICKY_BIT_ALLOW_WRITE_FVAL                      0x0
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION10_STICKY_BIT_DISABLE_WRITE_FVAL                    0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION9_STICKY_BIT_BMSK                                 0x200
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION9_STICKY_BIT_SHFT                                   0x9
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION9_STICKY_BIT_ALLOW_WRITE_FVAL                       0x0
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION9_STICKY_BIT_DISABLE_WRITE_FVAL                     0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION8_STICKY_BIT_BMSK                                 0x100
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION8_STICKY_BIT_SHFT                                   0x8
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION8_STICKY_BIT_ALLOW_WRITE_FVAL                       0x0
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION8_STICKY_BIT_DISABLE_WRITE_FVAL                     0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION7_STICKY_BIT_BMSK                                  0x80
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION7_STICKY_BIT_SHFT                                   0x7
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION7_STICKY_BIT_ALLOW_WRITE_FVAL                       0x0
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION7_STICKY_BIT_DISABLE_WRITE_FVAL                     0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION6_STICKY_BIT_BMSK                                  0x40
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION6_STICKY_BIT_SHFT                                   0x6
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION6_STICKY_BIT_ALLOW_WRITE_FVAL                       0x0
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION6_STICKY_BIT_DISABLE_WRITE_FVAL                     0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION5_STICKY_BIT_BMSK                                  0x20
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION5_STICKY_BIT_SHFT                                   0x5
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION5_STICKY_BIT_ALLOW_WRITE_FVAL                       0x0
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION5_STICKY_BIT_DISABLE_WRITE_FVAL                     0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION4_STICKY_BIT_BMSK                                  0x10
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION4_STICKY_BIT_SHFT                                   0x4
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION4_STICKY_BIT_ALLOW_WRITE_FVAL                       0x0
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION4_STICKY_BIT_DISABLE_WRITE_FVAL                     0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION3_STICKY_BIT_BMSK                                   0x8
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION3_STICKY_BIT_SHFT                                   0x3
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION3_STICKY_BIT_ALLOW_WRITE_FVAL                       0x0
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION3_STICKY_BIT_DISABLE_WRITE_FVAL                     0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION2_STICKY_BIT_BMSK                                   0x4
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION2_STICKY_BIT_SHFT                                   0x2
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION2_STICKY_BIT_ALLOW_WRITE_FVAL                       0x0
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION2_STICKY_BIT_DISABLE_WRITE_FVAL                     0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION1_STICKY_BIT_BMSK                                   0x2
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION1_STICKY_BIT_SHFT                                   0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION1_STICKY_BIT_ALLOW_WRITE_FVAL                       0x0
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION1_STICKY_BIT_DISABLE_WRITE_FVAL                     0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION0_STICKY_BIT_BMSK                                   0x1
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION0_STICKY_BIT_SHFT                                   0x0
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION0_STICKY_BIT_ALLOW_WRITE_FVAL                       0x0
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT0_REGION0_STICKY_BIT_DISABLE_WRITE_FVAL                     0x1

#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_ADDR                                               (SECURITY_CONTROL_CORE_REG_BASE      + 0x000061f4)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_RMSK                                               0xffffffff
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_IN          \
        in_dword_masked(HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_ADDR, HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_RMSK)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_INM(m)      \
        in_dword_masked(HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_ADDR, m)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_OUT(v)      \
        out_dword(HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_ADDR,v)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_ADDR,m,v,HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_IN)
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_RSVD0_BMSK                                         0xffffffff
#define HWIO_QFPROM_WRITE_DISABLE_STICKY_BIT1_RSVD0_SHFT                                                0x0

#define HWIO_ANTI_ROLLBACK_1_0_ADDR                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006200)
#define HWIO_ANTI_ROLLBACK_1_0_RMSK                                                              0xffffffff
#define HWIO_ANTI_ROLLBACK_1_0_IN          \
        in_dword_masked(HWIO_ANTI_ROLLBACK_1_0_ADDR, HWIO_ANTI_ROLLBACK_1_0_RMSK)
#define HWIO_ANTI_ROLLBACK_1_0_INM(m)      \
        in_dword_masked(HWIO_ANTI_ROLLBACK_1_0_ADDR, m)
#define HWIO_ANTI_ROLLBACK_1_0_XBL0_BMSK                                                         0xffffffff
#define HWIO_ANTI_ROLLBACK_1_0_XBL0_SHFT                                                                0x0

#define HWIO_ANTI_ROLLBACK_1_1_ADDR                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006204)
#define HWIO_ANTI_ROLLBACK_1_1_RMSK                                                              0xffffffff
#define HWIO_ANTI_ROLLBACK_1_1_IN          \
        in_dword_masked(HWIO_ANTI_ROLLBACK_1_1_ADDR, HWIO_ANTI_ROLLBACK_1_1_RMSK)
#define HWIO_ANTI_ROLLBACK_1_1_INM(m)      \
        in_dword_masked(HWIO_ANTI_ROLLBACK_1_1_ADDR, m)
#define HWIO_ANTI_ROLLBACK_1_1_XBL1_BMSK                                                         0xffffffff
#define HWIO_ANTI_ROLLBACK_1_1_XBL1_SHFT                                                                0x0

#define HWIO_ANTI_ROLLBACK_2_0_ADDR                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006208)
#define HWIO_ANTI_ROLLBACK_2_0_RMSK                                                              0xffffffff
#define HWIO_ANTI_ROLLBACK_2_0_IN          \
        in_dword_masked(HWIO_ANTI_ROLLBACK_2_0_ADDR, HWIO_ANTI_ROLLBACK_2_0_RMSK)
#define HWIO_ANTI_ROLLBACK_2_0_INM(m)      \
        in_dword_masked(HWIO_ANTI_ROLLBACK_2_0_ADDR, m)
#define HWIO_ANTI_ROLLBACK_2_0_PIL_SUBSYSTEM_31_0_BMSK                                           0xffffffff
#define HWIO_ANTI_ROLLBACK_2_0_PIL_SUBSYSTEM_31_0_SHFT                                                  0x0

#define HWIO_ANTI_ROLLBACK_2_1_ADDR                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000620c)
#define HWIO_ANTI_ROLLBACK_2_1_RMSK                                                              0xffffffff
#define HWIO_ANTI_ROLLBACK_2_1_IN          \
        in_dword_masked(HWIO_ANTI_ROLLBACK_2_1_ADDR, HWIO_ANTI_ROLLBACK_2_1_RMSK)
#define HWIO_ANTI_ROLLBACK_2_1_INM(m)      \
        in_dword_masked(HWIO_ANTI_ROLLBACK_2_1_ADDR, m)
#define HWIO_ANTI_ROLLBACK_2_1_XBL_SEC_BMSK                                                      0xfe000000
#define HWIO_ANTI_ROLLBACK_2_1_XBL_SEC_SHFT                                                            0x19
#define HWIO_ANTI_ROLLBACK_2_1_RPM_BMSK                                                           0x1fe0000
#define HWIO_ANTI_ROLLBACK_2_1_RPM_SHFT                                                                0x11
#define HWIO_ANTI_ROLLBACK_2_1_TZ_BMSK                                                              0x1ffff
#define HWIO_ANTI_ROLLBACK_2_1_TZ_SHFT                                                                  0x0

#define HWIO_ANTI_ROLLBACK_3_0_ADDR                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006210)
#define HWIO_ANTI_ROLLBACK_3_0_RMSK                                                              0xffffffff
#define HWIO_ANTI_ROLLBACK_3_0_IN          \
        in_dword_masked(HWIO_ANTI_ROLLBACK_3_0_ADDR, HWIO_ANTI_ROLLBACK_3_0_RMSK)
#define HWIO_ANTI_ROLLBACK_3_0_INM(m)      \
        in_dword_masked(HWIO_ANTI_ROLLBACK_3_0_ADDR, m)
#define HWIO_ANTI_ROLLBACK_3_0_XBL_CONFIG_1_0_BMSK                                               0xc0000000
#define HWIO_ANTI_ROLLBACK_3_0_XBL_CONFIG_1_0_SHFT                                                     0x1e
#define HWIO_ANTI_ROLLBACK_3_0_TQS_HASH_ACTIVE_BMSK                                              0x3e000000
#define HWIO_ANTI_ROLLBACK_3_0_TQS_HASH_ACTIVE_SHFT                                                    0x19
#define HWIO_ANTI_ROLLBACK_3_0_RPMB_KEY_PROVISIONED_BMSK                                          0x1000000
#define HWIO_ANTI_ROLLBACK_3_0_RPMB_KEY_PROVISIONED_SHFT                                               0x18
#define HWIO_ANTI_ROLLBACK_3_0_RPMB_KEY_PROVISIONED_RPMB_KEY_NOT_PROVISIONED_FVAL                       0x0
#define HWIO_ANTI_ROLLBACK_3_0_RPMB_KEY_PROVISIONED_RPMB_KEY_PROVISIONED_FVAL                           0x1
#define HWIO_ANTI_ROLLBACK_3_0_PIL_SUBSYSTEM_47_32_BMSK                                            0xffff00
#define HWIO_ANTI_ROLLBACK_3_0_PIL_SUBSYSTEM_47_32_SHFT                                                 0x8
#define HWIO_ANTI_ROLLBACK_3_0_SAFESWITCH_BMSK                                                         0xff
#define HWIO_ANTI_ROLLBACK_3_0_SAFESWITCH_SHFT                                                          0x0

#define HWIO_ANTI_ROLLBACK_3_1_ADDR                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006214)
#define HWIO_ANTI_ROLLBACK_3_1_RMSK                                                              0xffffffff
#define HWIO_ANTI_ROLLBACK_3_1_IN          \
        in_dword_masked(HWIO_ANTI_ROLLBACK_3_1_ADDR, HWIO_ANTI_ROLLBACK_3_1_RMSK)
#define HWIO_ANTI_ROLLBACK_3_1_INM(m)      \
        in_dword_masked(HWIO_ANTI_ROLLBACK_3_1_ADDR, m)
#define HWIO_ANTI_ROLLBACK_3_1_XBL_CONFIG_5_2_BMSK                                               0xf0000000
#define HWIO_ANTI_ROLLBACK_3_1_XBL_CONFIG_5_2_SHFT                                                     0x1c
#define HWIO_ANTI_ROLLBACK_3_1_DEVICE_CFG_BMSK                                                    0xffe0000
#define HWIO_ANTI_ROLLBACK_3_1_DEVICE_CFG_SHFT                                                         0x11
#define HWIO_ANTI_ROLLBACK_3_1_DEBUG_POLICY_BMSK                                                    0x1f000
#define HWIO_ANTI_ROLLBACK_3_1_DEBUG_POLICY_SHFT                                                        0xc
#define HWIO_ANTI_ROLLBACK_3_1_HYPERVISOR_BMSK                                                        0xfff
#define HWIO_ANTI_ROLLBACK_3_1_HYPERVISOR_SHFT                                                          0x0

#define HWIO_ANTI_ROLLBACK_4_0_ADDR                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006218)
#define HWIO_ANTI_ROLLBACK_4_0_RMSK                                                              0xffffffff
#define HWIO_ANTI_ROLLBACK_4_0_IN          \
        in_dword_masked(HWIO_ANTI_ROLLBACK_4_0_ADDR, HWIO_ANTI_ROLLBACK_4_0_RMSK)
#define HWIO_ANTI_ROLLBACK_4_0_INM(m)      \
        in_dword_masked(HWIO_ANTI_ROLLBACK_4_0_ADDR, m)
#define HWIO_ANTI_ROLLBACK_4_0_MSS_BMSK                                                          0xffff0000
#define HWIO_ANTI_ROLLBACK_4_0_MSS_SHFT                                                                0x10
#define HWIO_ANTI_ROLLBACK_4_0_MISC_BMSK                                                             0xffff
#define HWIO_ANTI_ROLLBACK_4_0_MISC_SHFT                                                                0x0

#define HWIO_ANTI_ROLLBACK_4_1_ADDR                                                              (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000621c)
#define HWIO_ANTI_ROLLBACK_4_1_RMSK                                                              0xffffffff
#define HWIO_ANTI_ROLLBACK_4_1_IN          \
        in_dword_masked(HWIO_ANTI_ROLLBACK_4_1_ADDR, HWIO_ANTI_ROLLBACK_4_1_RMSK)
#define HWIO_ANTI_ROLLBACK_4_1_INM(m)      \
        in_dword_masked(HWIO_ANTI_ROLLBACK_4_1_ADDR, m)
#define HWIO_ANTI_ROLLBACK_4_1_SIMLOCK_BMSK                                                      0x80000000
#define HWIO_ANTI_ROLLBACK_4_1_SIMLOCK_SHFT                                                            0x1f
#define HWIO_ANTI_ROLLBACK_4_1_RSVD0_BMSK                                                        0x7fffffff
#define HWIO_ANTI_ROLLBACK_4_1_RSVD0_SHFT                                                               0x0

#define HWIO_MRC_2_0_0_ADDR                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006250)
#define HWIO_MRC_2_0_0_RMSK                                                                      0xffffffff
#define HWIO_MRC_2_0_0_IN          \
        in_dword_masked(HWIO_MRC_2_0_0_ADDR, HWIO_MRC_2_0_0_RMSK)
#define HWIO_MRC_2_0_0_INM(m)      \
        in_dword_masked(HWIO_MRC_2_0_0_ADDR, m)
#define HWIO_MRC_2_0_0_RSVD0_BMSK                                                                0xfffffff0
#define HWIO_MRC_2_0_0_RSVD0_SHFT                                                                       0x4
#define HWIO_MRC_2_0_0_ROOT_CERT_ACTIVATION_LIST_BMSK                                                   0xf
#define HWIO_MRC_2_0_0_ROOT_CERT_ACTIVATION_LIST_SHFT                                                   0x0

#define HWIO_MRC_2_0_1_ADDR                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006254)
#define HWIO_MRC_2_0_1_RMSK                                                                      0xffffffff
#define HWIO_MRC_2_0_1_IN          \
        in_dword_masked(HWIO_MRC_2_0_1_ADDR, HWIO_MRC_2_0_1_RMSK)
#define HWIO_MRC_2_0_1_INM(m)      \
        in_dword_masked(HWIO_MRC_2_0_1_ADDR, m)
#define HWIO_MRC_2_0_1_RSVD1_BMSK                                                                0xfffffffe
#define HWIO_MRC_2_0_1_RSVD1_SHFT                                                                       0x1
#define HWIO_MRC_2_0_1_CURRENT_UIE_KEY_SEL_BMSK                                                         0x1
#define HWIO_MRC_2_0_1_CURRENT_UIE_KEY_SEL_SHFT                                                         0x0

#define HWIO_MRC_2_0_2_ADDR                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006258)
#define HWIO_MRC_2_0_2_RMSK                                                                      0xffffffff
#define HWIO_MRC_2_0_2_IN          \
        in_dword_masked(HWIO_MRC_2_0_2_ADDR, HWIO_MRC_2_0_2_RMSK)
#define HWIO_MRC_2_0_2_INM(m)      \
        in_dword_masked(HWIO_MRC_2_0_2_ADDR, m)
#define HWIO_MRC_2_0_2_RSVD0_BMSK                                                                0xfffffff0
#define HWIO_MRC_2_0_2_RSVD0_SHFT                                                                       0x4
#define HWIO_MRC_2_0_2_ROOT_CERT_REVOCATION_LIST_BMSK                                                   0xf
#define HWIO_MRC_2_0_2_ROOT_CERT_REVOCATION_LIST_SHFT                                                   0x0

#define HWIO_MRC_2_0_3_ADDR                                                                      (SECURITY_CONTROL_CORE_REG_BASE      + 0x0000625c)
#define HWIO_MRC_2_0_3_RMSK                                                                      0xffffffff
#define HWIO_MRC_2_0_3_IN          \
        in_dword_masked(HWIO_MRC_2_0_3_ADDR, HWIO_MRC_2_0_3_RMSK)
#define HWIO_MRC_2_0_3_INM(m)      \
        in_dword_masked(HWIO_MRC_2_0_3_ADDR, m)
#define HWIO_MRC_2_0_3_RSVD0_BMSK                                                                0xffffffff
#define HWIO_MRC_2_0_3_RSVD0_SHFT                                                                       0x0

#define HWIO_CRYPTO_LIB_VERSION_ADDR                                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006300)
#define HWIO_CRYPTO_LIB_VERSION_RMSK                                                             0xffffffff
#define HWIO_CRYPTO_LIB_VERSION_IN          \
        in_dword_masked(HWIO_CRYPTO_LIB_VERSION_ADDR, HWIO_CRYPTO_LIB_VERSION_RMSK)
#define HWIO_CRYPTO_LIB_VERSION_INM(m)      \
        in_dword_masked(HWIO_CRYPTO_LIB_VERSION_ADDR, m)
#define HWIO_CRYPTO_LIB_VERSION_VERSION_BMSK                                                     0xffffffff
#define HWIO_CRYPTO_LIB_VERSION_VERSION_SHFT                                                            0x0

#define HWIO_VIRTUAL_SEC_CTRL_BASE_ADDR_ADDR                                                     (SECURITY_CONTROL_CORE_REG_BASE      + 0x00006400)
#define HWIO_VIRTUAL_SEC_CTRL_BASE_ADDR_RMSK                                                     0xffffffff
#define HWIO_VIRTUAL_SEC_CTRL_BASE_ADDR_IN          \
        in_dword_masked(HWIO_VIRTUAL_SEC_CTRL_BASE_ADDR_ADDR, HWIO_VIRTUAL_SEC_CTRL_BASE_ADDR_RMSK)
#define HWIO_VIRTUAL_SEC_CTRL_BASE_ADDR_INM(m)      \
        in_dword_masked(HWIO_VIRTUAL_SEC_CTRL_BASE_ADDR_ADDR, m)
#define HWIO_VIRTUAL_SEC_CTRL_BASE_ADDR_OUT(v)      \
        out_dword(HWIO_VIRTUAL_SEC_CTRL_BASE_ADDR_ADDR,v)
#define HWIO_VIRTUAL_SEC_CTRL_BASE_ADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_VIRTUAL_SEC_CTRL_BASE_ADDR_ADDR,m,v,HWIO_VIRTUAL_SEC_CTRL_BASE_ADDR_IN)
#define HWIO_VIRTUAL_SEC_CTRL_BASE_ADDR_ADDRESS_BMSK                                             0xffffffff
#define HWIO_VIRTUAL_SEC_CTRL_BASE_ADDR_ADDRESS_SHFT                                                    0x0


#endif /* __CPR_FUSES_HWIO_H__ */