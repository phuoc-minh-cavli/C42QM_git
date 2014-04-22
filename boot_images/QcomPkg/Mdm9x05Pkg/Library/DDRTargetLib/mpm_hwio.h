#ifndef __MPM_HWIO_H__
#define __MPM_HWIO_H__
/*
===========================================================================
*/
/**
  @file mpm_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    MDM9205 (Twizy) [twizy_v1.0_p3q3r29.1]
 
  This file contains HWIO register definitions for the following modules:
    MPM2_G_CTRL_CNTR
    MPM2_PSHOLD
    MPM2_QTIMR_AC
    MPM2_QTIMR_V1


  Generation parameters: 
  { u'filename': u'mpm_hwio.h',
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [ u'MPM2_G_CTRL_CNTR',
                  u'MPM2_PSHOLD',
                  u'MPM2_QTIMR_AC',
                  u'MPM2_QTIMR_V1']}
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

  $Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Library/DDRTargetLib/mpm_hwio.h#2 $
  $DateTime: 2018/12/19 23:46:57 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: MPM2_G_CTRL_CNTR
 *--------------------------------------------------------------------------*/

#define MPM2_G_CTRL_CNTR_REG_BASE                             (MPM2_MPM_BASE      + 0x00001000)
#define MPM2_G_CTRL_CNTR_REG_BASE_SIZE                        0x1000
#define MPM2_G_CTRL_CNTR_REG_BASE_USED                        0xfd0

#define HWIO_MPM2_MPM_CONTROL_CNTCR_ADDR                      (MPM2_G_CTRL_CNTR_REG_BASE      + 0x00000000)
#define HWIO_MPM2_MPM_CONTROL_CNTCR_RMSK                           0x103
#define HWIO_MPM2_MPM_CONTROL_CNTCR_IN          \
        in_dword(HWIO_MPM2_MPM_CONTROL_CNTCR_ADDR)
#define HWIO_MPM2_MPM_CONTROL_CNTCR_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_CONTROL_CNTCR_ADDR, m)
#define HWIO_MPM2_MPM_CONTROL_CNTCR_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_CONTROL_CNTCR_ADDR,v)
#define HWIO_MPM2_MPM_CONTROL_CNTCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_CONTROL_CNTCR_ADDR,m,v,HWIO_MPM2_MPM_CONTROL_CNTCR_IN)
#define HWIO_MPM2_MPM_CONTROL_CNTCR_FCREQ_BMSK                     0x100
#define HWIO_MPM2_MPM_CONTROL_CNTCR_FCREQ_SHFT                       0x8
#define HWIO_MPM2_MPM_CONTROL_CNTCR_HDBG_BMSK                        0x2
#define HWIO_MPM2_MPM_CONTROL_CNTCR_HDBG_SHFT                        0x1
#define HWIO_MPM2_MPM_CONTROL_CNTCR_EN_BMSK                          0x1
#define HWIO_MPM2_MPM_CONTROL_CNTCR_EN_SHFT                          0x0

#define HWIO_MPM2_MPM_CONTROL_CNTSR_ADDR                      (MPM2_G_CTRL_CNTR_REG_BASE      + 0x00000004)
#define HWIO_MPM2_MPM_CONTROL_CNTSR_RMSK                           0x102
#define HWIO_MPM2_MPM_CONTROL_CNTSR_IN          \
        in_dword(HWIO_MPM2_MPM_CONTROL_CNTSR_ADDR)
#define HWIO_MPM2_MPM_CONTROL_CNTSR_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_CONTROL_CNTSR_ADDR, m)
#define HWIO_MPM2_MPM_CONTROL_CNTSR_FCACK_BMSK                     0x100
#define HWIO_MPM2_MPM_CONTROL_CNTSR_FCACK_SHFT                       0x8
#define HWIO_MPM2_MPM_CONTROL_CNTSR_HDBG_BMSK                        0x2
#define HWIO_MPM2_MPM_CONTROL_CNTSR_HDBG_SHFT                        0x1

#define HWIO_MPM2_MPM_CONTROL_CNTCV_LO_ADDR                   (MPM2_G_CTRL_CNTR_REG_BASE      + 0x00000008)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_LO_RMSK                   0xffffffff
#define HWIO_MPM2_MPM_CONTROL_CNTCV_LO_IN          \
        in_dword(HWIO_MPM2_MPM_CONTROL_CNTCV_LO_ADDR)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_LO_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_CONTROL_CNTCV_LO_ADDR, m)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_LO_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_CONTROL_CNTCV_LO_ADDR,v)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_CONTROL_CNTCV_LO_ADDR,m,v,HWIO_MPM2_MPM_CONTROL_CNTCV_LO_IN)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_LO_LOAD_VAL_BMSK          0xffffffff
#define HWIO_MPM2_MPM_CONTROL_CNTCV_LO_LOAD_VAL_SHFT                 0x0

#define HWIO_MPM2_MPM_CONTROL_CNTCV_HI_ADDR                   (MPM2_G_CTRL_CNTR_REG_BASE      + 0x0000000c)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_HI_RMSK                     0xffffff
#define HWIO_MPM2_MPM_CONTROL_CNTCV_HI_IN          \
        in_dword(HWIO_MPM2_MPM_CONTROL_CNTCV_HI_ADDR)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_HI_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_CONTROL_CNTCV_HI_ADDR, m)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_HI_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_CONTROL_CNTCV_HI_ADDR,v)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_CONTROL_CNTCV_HI_ADDR,m,v,HWIO_MPM2_MPM_CONTROL_CNTCV_HI_IN)
#define HWIO_MPM2_MPM_CONTROL_CNTCV_HI_LOAD_VAL_BMSK            0xffffff
#define HWIO_MPM2_MPM_CONTROL_CNTCV_HI_LOAD_VAL_SHFT                 0x0

#define HWIO_MPM2_MPM_CONTROL_CNTFID0_ADDR                    (MPM2_G_CTRL_CNTR_REG_BASE      + 0x00000020)
#define HWIO_MPM2_MPM_CONTROL_CNTFID0_RMSK                    0xffffffff
#define HWIO_MPM2_MPM_CONTROL_CNTFID0_IN          \
        in_dword(HWIO_MPM2_MPM_CONTROL_CNTFID0_ADDR)
#define HWIO_MPM2_MPM_CONTROL_CNTFID0_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_CONTROL_CNTFID0_ADDR, m)
#define HWIO_MPM2_MPM_CONTROL_CNTFID0_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_CONTROL_CNTFID0_ADDR,v)
#define HWIO_MPM2_MPM_CONTROL_CNTFID0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_CONTROL_CNTFID0_ADDR,m,v,HWIO_MPM2_MPM_CONTROL_CNTFID0_IN)
#define HWIO_MPM2_MPM_CONTROL_CNTFID0_FREQ_BMSK               0xffffffff
#define HWIO_MPM2_MPM_CONTROL_CNTFID0_FREQ_SHFT                      0x0

#define HWIO_MPM2_MPM_CONTROL_ID_ADDR                         (MPM2_G_CTRL_CNTR_REG_BASE      + 0x00000fd0)
#define HWIO_MPM2_MPM_CONTROL_ID_RMSK                         0xffffffff
#define HWIO_MPM2_MPM_CONTROL_ID_IN          \
        in_dword(HWIO_MPM2_MPM_CONTROL_ID_ADDR)
#define HWIO_MPM2_MPM_CONTROL_ID_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_CONTROL_ID_ADDR, m)
#define HWIO_MPM2_MPM_CONTROL_ID_MAJOR_BMSK                   0xf0000000
#define HWIO_MPM2_MPM_CONTROL_ID_MAJOR_SHFT                         0x1c
#define HWIO_MPM2_MPM_CONTROL_ID_MINOR_BMSK                    0xfff0000
#define HWIO_MPM2_MPM_CONTROL_ID_MINOR_SHFT                         0x10
#define HWIO_MPM2_MPM_CONTROL_ID_STEP_BMSK                        0xffff
#define HWIO_MPM2_MPM_CONTROL_ID_STEP_SHFT                           0x0

/*----------------------------------------------------------------------------
 * MODULE: MPM2_PSHOLD
 *--------------------------------------------------------------------------*/

#define MPM2_PSHOLD_REG_BASE                                                    (MPM2_MPM_BASE      + 0x0000b000)
#define MPM2_PSHOLD_REG_BASE_SIZE                                               0x1000
#define MPM2_PSHOLD_REG_BASE_USED                                               0x24

#define HWIO_MPM2_MPM_PS_HOLD_ADDR                                              (MPM2_PSHOLD_REG_BASE      + 0x00000000)
#define HWIO_MPM2_MPM_PS_HOLD_RMSK                                                     0x1
#define HWIO_MPM2_MPM_PS_HOLD_IN          \
        in_dword(HWIO_MPM2_MPM_PS_HOLD_ADDR)
#define HWIO_MPM2_MPM_PS_HOLD_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_PS_HOLD_ADDR, m)
#define HWIO_MPM2_MPM_PS_HOLD_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_PS_HOLD_ADDR,v)
#define HWIO_MPM2_MPM_PS_HOLD_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_PS_HOLD_ADDR,m,v,HWIO_MPM2_MPM_PS_HOLD_IN)
#define HWIO_MPM2_MPM_PS_HOLD_PSHOLD_BMSK                                              0x1
#define HWIO_MPM2_MPM_PS_HOLD_PSHOLD_SHFT                                              0x0

#define HWIO_MPM2_MPM_DDR_PHY_FREEZEIO_EBI1_ADDR                                (MPM2_PSHOLD_REG_BASE      + 0x00000004)
#define HWIO_MPM2_MPM_DDR_PHY_FREEZEIO_EBI1_RMSK                                       0x1
#define HWIO_MPM2_MPM_DDR_PHY_FREEZEIO_EBI1_IN          \
        in_dword(HWIO_MPM2_MPM_DDR_PHY_FREEZEIO_EBI1_ADDR)
#define HWIO_MPM2_MPM_DDR_PHY_FREEZEIO_EBI1_INM(m)      \
        in_dword_masked(HWIO_MPM2_MPM_DDR_PHY_FREEZEIO_EBI1_ADDR, m)
#define HWIO_MPM2_MPM_DDR_PHY_FREEZEIO_EBI1_OUT(v)      \
        out_dword(HWIO_MPM2_MPM_DDR_PHY_FREEZEIO_EBI1_ADDR,v)
#define HWIO_MPM2_MPM_DDR_PHY_FREEZEIO_EBI1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_MPM_DDR_PHY_FREEZEIO_EBI1_ADDR,m,v,HWIO_MPM2_MPM_DDR_PHY_FREEZEIO_EBI1_IN)
#define HWIO_MPM2_MPM_DDR_PHY_FREEZEIO_EBI1_DDR_PHY_FREEZEIO_EBI1_BMSK                 0x1
#define HWIO_MPM2_MPM_DDR_PHY_FREEZEIO_EBI1_DDR_PHY_FREEZEIO_EBI1_SHFT                 0x0

#define HWIO_MPM2_PONOFF_CLAMP_MEM_STATUS_ADDR_ADDR                             (MPM2_PSHOLD_REG_BASE      + 0x00000020)
#define HWIO_MPM2_PONOFF_CLAMP_MEM_STATUS_ADDR_RMSK                                    0x1
#define HWIO_MPM2_PONOFF_CLAMP_MEM_STATUS_ADDR_IN          \
        in_dword(HWIO_MPM2_PONOFF_CLAMP_MEM_STATUS_ADDR_ADDR)
#define HWIO_MPM2_PONOFF_CLAMP_MEM_STATUS_ADDR_INM(m)      \
        in_dword_masked(HWIO_MPM2_PONOFF_CLAMP_MEM_STATUS_ADDR_ADDR, m)
#define HWIO_MPM2_PONOFF_CLAMP_MEM_STATUS_ADDR_STATUS_BMSK                             0x1
#define HWIO_MPM2_PONOFF_CLAMP_MEM_STATUS_ADDR_STATUS_SHFT                             0x0

#define HWIO_MPM2_CLAMP_CTRL_PONOFF_PHY_ADDR                                    (MPM2_PSHOLD_REG_BASE      + 0x00000024)
#define HWIO_MPM2_CLAMP_CTRL_PONOFF_PHY_RMSK                                           0x1
#define HWIO_MPM2_CLAMP_CTRL_PONOFF_PHY_IN          \
        in_dword(HWIO_MPM2_CLAMP_CTRL_PONOFF_PHY_ADDR)
#define HWIO_MPM2_CLAMP_CTRL_PONOFF_PHY_INM(m)      \
        in_dword_masked(HWIO_MPM2_CLAMP_CTRL_PONOFF_PHY_ADDR, m)
#define HWIO_MPM2_CLAMP_CTRL_PONOFF_PHY_OUT(v)      \
        out_dword(HWIO_MPM2_CLAMP_CTRL_PONOFF_PHY_ADDR,v)
#define HWIO_MPM2_CLAMP_CTRL_PONOFF_PHY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_CLAMP_CTRL_PONOFF_PHY_ADDR,m,v,HWIO_MPM2_CLAMP_CTRL_PONOFF_PHY_IN)
#define HWIO_MPM2_CLAMP_CTRL_PONOFF_PHY_RESET_BMSK                                     0x1
#define HWIO_MPM2_CLAMP_CTRL_PONOFF_PHY_RESET_SHFT                                     0x0

/*----------------------------------------------------------------------------
 * MODULE: MPM2_QTIMR_AC
 *--------------------------------------------------------------------------*/

#define MPM2_QTIMR_AC_REG_BASE                                  (MPM2_MPM_BASE      + 0x00004000)
#define MPM2_QTIMR_AC_REG_BASE_SIZE                             0x1000
#define MPM2_QTIMR_AC_REG_BASE_USED                             0xfd0

#define HWIO_MPM2_QTMR_AC_CNTFRQ_ADDR                           (MPM2_QTIMR_AC_REG_BASE      + 0x00000000)
#define HWIO_MPM2_QTMR_AC_CNTFRQ_RMSK                           0xffffffff
#define HWIO_MPM2_QTMR_AC_CNTFRQ_IN          \
        in_dword(HWIO_MPM2_QTMR_AC_CNTFRQ_ADDR)
#define HWIO_MPM2_QTMR_AC_CNTFRQ_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_AC_CNTFRQ_ADDR, m)
#define HWIO_MPM2_QTMR_AC_CNTFRQ_OUT(v)      \
        out_dword(HWIO_MPM2_QTMR_AC_CNTFRQ_ADDR,v)
#define HWIO_MPM2_QTMR_AC_CNTFRQ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_QTMR_AC_CNTFRQ_ADDR,m,v,HWIO_MPM2_QTMR_AC_CNTFRQ_IN)
#define HWIO_MPM2_QTMR_AC_CNTFRQ_CNTFRQ_BMSK                    0xffffffff
#define HWIO_MPM2_QTMR_AC_CNTFRQ_CNTFRQ_SHFT                           0x0

#define HWIO_MPM2_QTMR_AC_CNTSR_ADDR                            (MPM2_QTIMR_AC_REG_BASE      + 0x00000004)
#define HWIO_MPM2_QTMR_AC_CNTSR_RMSK                                   0x1
#define HWIO_MPM2_QTMR_AC_CNTSR_IN          \
        in_dword(HWIO_MPM2_QTMR_AC_CNTSR_ADDR)
#define HWIO_MPM2_QTMR_AC_CNTSR_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_AC_CNTSR_ADDR, m)
#define HWIO_MPM2_QTMR_AC_CNTSR_OUT(v)      \
        out_dword(HWIO_MPM2_QTMR_AC_CNTSR_ADDR,v)
#define HWIO_MPM2_QTMR_AC_CNTSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_QTMR_AC_CNTSR_ADDR,m,v,HWIO_MPM2_QTMR_AC_CNTSR_IN)
#define HWIO_MPM2_QTMR_AC_CNTSR_NSN_BMSK                               0x1
#define HWIO_MPM2_QTMR_AC_CNTSR_NSN_SHFT                               0x0

#define HWIO_MPM2_QTMR_AC_CNTTID_ADDR                           (MPM2_QTIMR_AC_REG_BASE      + 0x00000008)
#define HWIO_MPM2_QTMR_AC_CNTTID_RMSK                           0xffffffff
#define HWIO_MPM2_QTMR_AC_CNTTID_IN          \
        in_dword(HWIO_MPM2_QTMR_AC_CNTTID_ADDR)
#define HWIO_MPM2_QTMR_AC_CNTTID_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_AC_CNTTID_ADDR, m)
#define HWIO_MPM2_QTMR_AC_CNTTID_F7_CFG_BMSK                    0xf0000000
#define HWIO_MPM2_QTMR_AC_CNTTID_F7_CFG_SHFT                          0x1c
#define HWIO_MPM2_QTMR_AC_CNTTID_F6_CFG_BMSK                     0xf000000
#define HWIO_MPM2_QTMR_AC_CNTTID_F6_CFG_SHFT                          0x18
#define HWIO_MPM2_QTMR_AC_CNTTID_F5_CFG_BMSK                      0xf00000
#define HWIO_MPM2_QTMR_AC_CNTTID_F5_CFG_SHFT                          0x14
#define HWIO_MPM2_QTMR_AC_CNTTID_F4_CFG_BMSK                       0xf0000
#define HWIO_MPM2_QTMR_AC_CNTTID_F4_CFG_SHFT                          0x10
#define HWIO_MPM2_QTMR_AC_CNTTID_F3_CFG_BMSK                        0xf000
#define HWIO_MPM2_QTMR_AC_CNTTID_F3_CFG_SHFT                           0xc
#define HWIO_MPM2_QTMR_AC_CNTTID_F2_CFG_BMSK                         0xf00
#define HWIO_MPM2_QTMR_AC_CNTTID_F2_CFG_SHFT                           0x8
#define HWIO_MPM2_QTMR_AC_CNTTID_F1_CFG_BMSK                          0xf0
#define HWIO_MPM2_QTMR_AC_CNTTID_F1_CFG_SHFT                           0x4
#define HWIO_MPM2_QTMR_AC_CNTTID_F0_CFG_BMSK                           0xf
#define HWIO_MPM2_QTMR_AC_CNTTID_F0_CFG_SHFT                           0x0

#define HWIO_MPM2_QTMR_AC_CNTACR_n_ADDR(n)                      (MPM2_QTIMR_AC_REG_BASE      + 0x00000040 + 0x4 * (n))
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RMSK                               0x3f
#define HWIO_MPM2_QTMR_AC_CNTACR_n_MAXn                                  0
#define HWIO_MPM2_QTMR_AC_CNTACR_n_INI(n)        \
        in_dword_masked(HWIO_MPM2_QTMR_AC_CNTACR_n_ADDR(n), HWIO_MPM2_QTMR_AC_CNTACR_n_RMSK)
#define HWIO_MPM2_QTMR_AC_CNTACR_n_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_QTMR_AC_CNTACR_n_ADDR(n), mask)
#define HWIO_MPM2_QTMR_AC_CNTACR_n_OUTI(n,val)    \
        out_dword(HWIO_MPM2_QTMR_AC_CNTACR_n_ADDR(n),val)
#define HWIO_MPM2_QTMR_AC_CNTACR_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MPM2_QTMR_AC_CNTACR_n_ADDR(n),mask,val,HWIO_MPM2_QTMR_AC_CNTACR_n_INI(n))
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RWPT_BMSK                          0x20
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RWPT_SHFT                           0x5
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RWVT_BMSK                          0x10
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RWVT_SHFT                           0x4
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RVOFF_BMSK                          0x8
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RVOFF_SHFT                          0x3
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RFRQ_BMSK                           0x4
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RFRQ_SHFT                           0x2
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RPVCT_BMSK                          0x2
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RPVCT_SHFT                          0x1
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RPCT_BMSK                           0x1
#define HWIO_MPM2_QTMR_AC_CNTACR_n_RPCT_SHFT                           0x0

#define HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_ADDR(n)                  (MPM2_QTIMR_AC_REG_BASE      + 0x00000080 + 0x8 * (n))
#define HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_RMSK                     0xffffffff
#define HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_MAXn                              0
#define HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_INI(n)        \
        in_dword_masked(HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_ADDR(n), HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_RMSK)
#define HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_ADDR(n), mask)
#define HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_OUTI(n,val)    \
        out_dword(HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_ADDR(n),val)
#define HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_ADDR(n),mask,val,HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_INI(n))
#define HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_CNTVOFF_LO_BMSK          0xffffffff
#define HWIO_MPM2_QTMR_AC_CNTVOFF_LO_n_CNTVOFF_LO_SHFT                 0x0

#define HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_ADDR(n)                  (MPM2_QTIMR_AC_REG_BASE      + 0x00000084 + 0x8 * (n))
#define HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_RMSK                       0xffffff
#define HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_MAXn                              0
#define HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_INI(n)        \
        in_dword_masked(HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_ADDR(n), HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_RMSK)
#define HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_INMI(n,mask)    \
        in_dword_masked(HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_ADDR(n), mask)
#define HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_OUTI(n,val)    \
        out_dword(HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_ADDR(n),val)
#define HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_ADDR(n),mask,val,HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_INI(n))
#define HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_CNTVOFF_HI_BMSK            0xffffff
#define HWIO_MPM2_QTMR_AC_CNTVOFF_HI_n_CNTVOFF_HI_SHFT                 0x0

#define HWIO_MPM2_QTMR_AC_CFG_ADDR                              (MPM2_QTIMR_AC_REG_BASE      + 0x00000fc0)
#define HWIO_MPM2_QTMR_AC_CFG_RMSK                                     0x1
#define HWIO_MPM2_QTMR_AC_CFG_IN          \
        in_dword(HWIO_MPM2_QTMR_AC_CFG_ADDR)
#define HWIO_MPM2_QTMR_AC_CFG_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_AC_CFG_ADDR, m)
#define HWIO_MPM2_QTMR_AC_CFG_OUT(v)      \
        out_dword(HWIO_MPM2_QTMR_AC_CFG_ADDR,v)
#define HWIO_MPM2_QTMR_AC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_QTMR_AC_CFG_ADDR,m,v,HWIO_MPM2_QTMR_AC_CFG_IN)
#define HWIO_MPM2_QTMR_AC_CFG_TEST_BUS_EN_BMSK                         0x1
#define HWIO_MPM2_QTMR_AC_CFG_TEST_BUS_EN_SHFT                         0x0

#define HWIO_MPM2_QTMR_AC_VERSION_ADDR                          (MPM2_QTIMR_AC_REG_BASE      + 0x00000fd0)
#define HWIO_MPM2_QTMR_AC_VERSION_RMSK                          0xffffffff
#define HWIO_MPM2_QTMR_AC_VERSION_IN          \
        in_dword(HWIO_MPM2_QTMR_AC_VERSION_ADDR)
#define HWIO_MPM2_QTMR_AC_VERSION_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_AC_VERSION_ADDR, m)
#define HWIO_MPM2_QTMR_AC_VERSION_MAJOR_BMSK                    0xf0000000
#define HWIO_MPM2_QTMR_AC_VERSION_MAJOR_SHFT                          0x1c
#define HWIO_MPM2_QTMR_AC_VERSION_MINOR_BMSK                     0xfff0000
#define HWIO_MPM2_QTMR_AC_VERSION_MINOR_SHFT                          0x10
#define HWIO_MPM2_QTMR_AC_VERSION_STEP_BMSK                         0xffff
#define HWIO_MPM2_QTMR_AC_VERSION_STEP_SHFT                            0x0

/*----------------------------------------------------------------------------
 * MODULE: MPM2_QTIMR_V1
 *--------------------------------------------------------------------------*/

#define MPM2_QTIMR_V1_REG_BASE                                    (MPM2_MPM_BASE      + 0x00005000)
#define MPM2_QTIMR_V1_REG_BASE_SIZE                               0x1000
#define MPM2_QTIMR_V1_REG_BASE_USED                               0xfd0

#define HWIO_MPM2_QTMR_V1_CNTPCT_LO_ADDR                          (MPM2_QTIMR_V1_REG_BASE      + 0x00000000)
#define HWIO_MPM2_QTMR_V1_CNTPCT_LO_RMSK                          0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTPCT_LO_IN          \
        in_dword(HWIO_MPM2_QTMR_V1_CNTPCT_LO_ADDR)
#define HWIO_MPM2_QTMR_V1_CNTPCT_LO_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTPCT_LO_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTPCT_LO_CNTPCT_LO_BMSK                0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTPCT_LO_CNTPCT_LO_SHFT                       0x0

#define HWIO_MPM2_QTMR_V1_CNTPCT_HI_ADDR                          (MPM2_QTIMR_V1_REG_BASE      + 0x00000004)
#define HWIO_MPM2_QTMR_V1_CNTPCT_HI_RMSK                            0xffffff
#define HWIO_MPM2_QTMR_V1_CNTPCT_HI_IN          \
        in_dword(HWIO_MPM2_QTMR_V1_CNTPCT_HI_ADDR)
#define HWIO_MPM2_QTMR_V1_CNTPCT_HI_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTPCT_HI_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTPCT_HI_CNTPCT_HI_BMSK                  0xffffff
#define HWIO_MPM2_QTMR_V1_CNTPCT_HI_CNTPCT_HI_SHFT                       0x0

#define HWIO_MPM2_QTMR_V1_CNTVCT_LO_ADDR                          (MPM2_QTIMR_V1_REG_BASE      + 0x00000008)
#define HWIO_MPM2_QTMR_V1_CNTVCT_LO_RMSK                          0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTVCT_LO_IN          \
        in_dword(HWIO_MPM2_QTMR_V1_CNTVCT_LO_ADDR)
#define HWIO_MPM2_QTMR_V1_CNTVCT_LO_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTVCT_LO_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTVCT_LO_CNTVCT_LO_BMSK                0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTVCT_LO_CNTVCT_LO_SHFT                       0x0

#define HWIO_MPM2_QTMR_V1_CNTVCT_HI_ADDR                          (MPM2_QTIMR_V1_REG_BASE      + 0x0000000c)
#define HWIO_MPM2_QTMR_V1_CNTVCT_HI_RMSK                            0xffffff
#define HWIO_MPM2_QTMR_V1_CNTVCT_HI_IN          \
        in_dword(HWIO_MPM2_QTMR_V1_CNTVCT_HI_ADDR)
#define HWIO_MPM2_QTMR_V1_CNTVCT_HI_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTVCT_HI_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTVCT_HI_CNTVCT_HI_BMSK                  0xffffff
#define HWIO_MPM2_QTMR_V1_CNTVCT_HI_CNTVCT_HI_SHFT                       0x0

#define HWIO_MPM2_QTMR_V1_CNTFRQ_ADDR                             (MPM2_QTIMR_V1_REG_BASE      + 0x00000010)
#define HWIO_MPM2_QTMR_V1_CNTFRQ_RMSK                             0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTFRQ_IN          \
        in_dword(HWIO_MPM2_QTMR_V1_CNTFRQ_ADDR)
#define HWIO_MPM2_QTMR_V1_CNTFRQ_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTFRQ_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTFRQ_CNTFRQ_BMSK                      0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTFRQ_CNTFRQ_SHFT                             0x0

#define HWIO_MPM2_QTMR_V1_CNTPL0ACR_ADDR                          (MPM2_QTIMR_V1_REG_BASE      + 0x00000014)
#define HWIO_MPM2_QTMR_V1_CNTPL0ACR_RMSK                               0x303
#define HWIO_MPM2_QTMR_V1_CNTPL0ACR_IN          \
        in_dword(HWIO_MPM2_QTMR_V1_CNTPL0ACR_ADDR)
#define HWIO_MPM2_QTMR_V1_CNTPL0ACR_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTPL0ACR_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTPL0ACR_OUT(v)      \
        out_dword(HWIO_MPM2_QTMR_V1_CNTPL0ACR_ADDR,v)
#define HWIO_MPM2_QTMR_V1_CNTPL0ACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_QTMR_V1_CNTPL0ACR_ADDR,m,v,HWIO_MPM2_QTMR_V1_CNTPL0ACR_IN)
#define HWIO_MPM2_QTMR_V1_CNTPL0ACR_PL0CTEN_BMSK                       0x200
#define HWIO_MPM2_QTMR_V1_CNTPL0ACR_PL0CTEN_SHFT                         0x9
#define HWIO_MPM2_QTMR_V1_CNTPL0ACR_PL0VTEN_BMSK                       0x100
#define HWIO_MPM2_QTMR_V1_CNTPL0ACR_PL0VTEN_SHFT                         0x8
#define HWIO_MPM2_QTMR_V1_CNTPL0ACR_PL0VCTEN_BMSK                        0x2
#define HWIO_MPM2_QTMR_V1_CNTPL0ACR_PL0VCTEN_SHFT                        0x1
#define HWIO_MPM2_QTMR_V1_CNTPL0ACR_PL0PCTEN_BMSK                        0x1
#define HWIO_MPM2_QTMR_V1_CNTPL0ACR_PL0PCTEN_SHFT                        0x0

#define HWIO_MPM2_QTMR_V1_CNTVOFF_LO_ADDR                         (MPM2_QTIMR_V1_REG_BASE      + 0x00000018)
#define HWIO_MPM2_QTMR_V1_CNTVOFF_LO_RMSK                         0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTVOFF_LO_IN          \
        in_dword(HWIO_MPM2_QTMR_V1_CNTVOFF_LO_ADDR)
#define HWIO_MPM2_QTMR_V1_CNTVOFF_LO_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTVOFF_LO_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTVOFF_LO_CNTVOFF_L0_BMSK              0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTVOFF_LO_CNTVOFF_L0_SHFT                     0x0

#define HWIO_MPM2_QTMR_V1_CNTVOFF_HI_ADDR                         (MPM2_QTIMR_V1_REG_BASE      + 0x0000001c)
#define HWIO_MPM2_QTMR_V1_CNTVOFF_HI_RMSK                           0xffffff
#define HWIO_MPM2_QTMR_V1_CNTVOFF_HI_IN          \
        in_dword(HWIO_MPM2_QTMR_V1_CNTVOFF_HI_ADDR)
#define HWIO_MPM2_QTMR_V1_CNTVOFF_HI_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTVOFF_HI_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTVOFF_HI_CNTVOFF_HI_BMSK                0xffffff
#define HWIO_MPM2_QTMR_V1_CNTVOFF_HI_CNTVOFF_HI_SHFT                     0x0

#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_LO_ADDR                       (MPM2_QTIMR_V1_REG_BASE      + 0x00000020)
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_LO_RMSK                       0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_LO_IN          \
        in_dword(HWIO_MPM2_QTMR_V1_CNTP_CVAL_LO_ADDR)
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_LO_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTP_CVAL_LO_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_LO_OUT(v)      \
        out_dword(HWIO_MPM2_QTMR_V1_CNTP_CVAL_LO_ADDR,v)
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_QTMR_V1_CNTP_CVAL_LO_ADDR,m,v,HWIO_MPM2_QTMR_V1_CNTP_CVAL_LO_IN)
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_LO_CNTP_CVAL_L0_BMSK          0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_LO_CNTP_CVAL_L0_SHFT                 0x0

#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_HI_ADDR                       (MPM2_QTIMR_V1_REG_BASE      + 0x00000024)
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_HI_RMSK                         0xffffff
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_HI_IN          \
        in_dword(HWIO_MPM2_QTMR_V1_CNTP_CVAL_HI_ADDR)
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_HI_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTP_CVAL_HI_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_HI_OUT(v)      \
        out_dword(HWIO_MPM2_QTMR_V1_CNTP_CVAL_HI_ADDR,v)
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_QTMR_V1_CNTP_CVAL_HI_ADDR,m,v,HWIO_MPM2_QTMR_V1_CNTP_CVAL_HI_IN)
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_HI_CNTP_CVAL_HI_BMSK            0xffffff
#define HWIO_MPM2_QTMR_V1_CNTP_CVAL_HI_CNTP_CVAL_HI_SHFT                 0x0

#define HWIO_MPM2_QTMR_V1_CNTP_TVAL_ADDR                          (MPM2_QTIMR_V1_REG_BASE      + 0x00000028)
#define HWIO_MPM2_QTMR_V1_CNTP_TVAL_RMSK                          0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTP_TVAL_IN          \
        in_dword(HWIO_MPM2_QTMR_V1_CNTP_TVAL_ADDR)
#define HWIO_MPM2_QTMR_V1_CNTP_TVAL_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTP_TVAL_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTP_TVAL_OUT(v)      \
        out_dword(HWIO_MPM2_QTMR_V1_CNTP_TVAL_ADDR,v)
#define HWIO_MPM2_QTMR_V1_CNTP_TVAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_QTMR_V1_CNTP_TVAL_ADDR,m,v,HWIO_MPM2_QTMR_V1_CNTP_TVAL_IN)
#define HWIO_MPM2_QTMR_V1_CNTP_TVAL_CNTP_TVAL_BMSK                0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTP_TVAL_CNTP_TVAL_SHFT                       0x0

#define HWIO_MPM2_QTMR_V1_CNTP_CTL_ADDR                           (MPM2_QTIMR_V1_REG_BASE      + 0x0000002c)
#define HWIO_MPM2_QTMR_V1_CNTP_CTL_RMSK                                  0x7
#define HWIO_MPM2_QTMR_V1_CNTP_CTL_IN          \
        in_dword(HWIO_MPM2_QTMR_V1_CNTP_CTL_ADDR)
#define HWIO_MPM2_QTMR_V1_CNTP_CTL_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTP_CTL_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTP_CTL_OUT(v)      \
        out_dword(HWIO_MPM2_QTMR_V1_CNTP_CTL_ADDR,v)
#define HWIO_MPM2_QTMR_V1_CNTP_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_QTMR_V1_CNTP_CTL_ADDR,m,v,HWIO_MPM2_QTMR_V1_CNTP_CTL_IN)
#define HWIO_MPM2_QTMR_V1_CNTP_CTL_ISTAT_BMSK                            0x4
#define HWIO_MPM2_QTMR_V1_CNTP_CTL_ISTAT_SHFT                            0x2
#define HWIO_MPM2_QTMR_V1_CNTP_CTL_IMSK_BMSK                             0x2
#define HWIO_MPM2_QTMR_V1_CNTP_CTL_IMSK_SHFT                             0x1
#define HWIO_MPM2_QTMR_V1_CNTP_CTL_EN_BMSK                               0x1
#define HWIO_MPM2_QTMR_V1_CNTP_CTL_EN_SHFT                               0x0

#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_LO_ADDR                       (MPM2_QTIMR_V1_REG_BASE      + 0x00000030)
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_LO_RMSK                       0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_LO_IN          \
        in_dword(HWIO_MPM2_QTMR_V1_CNTV_CVAL_LO_ADDR)
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_LO_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTV_CVAL_LO_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_LO_OUT(v)      \
        out_dword(HWIO_MPM2_QTMR_V1_CNTV_CVAL_LO_ADDR,v)
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_QTMR_V1_CNTV_CVAL_LO_ADDR,m,v,HWIO_MPM2_QTMR_V1_CNTV_CVAL_LO_IN)
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_LO_CNTV_CVAL_L0_BMSK          0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_LO_CNTV_CVAL_L0_SHFT                 0x0

#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_HI_ADDR                       (MPM2_QTIMR_V1_REG_BASE      + 0x00000034)
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_HI_RMSK                         0xffffff
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_HI_IN          \
        in_dword(HWIO_MPM2_QTMR_V1_CNTV_CVAL_HI_ADDR)
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_HI_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTV_CVAL_HI_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_HI_OUT(v)      \
        out_dword(HWIO_MPM2_QTMR_V1_CNTV_CVAL_HI_ADDR,v)
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_QTMR_V1_CNTV_CVAL_HI_ADDR,m,v,HWIO_MPM2_QTMR_V1_CNTV_CVAL_HI_IN)
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_HI_CNTV_CVAL_HI_BMSK            0xffffff
#define HWIO_MPM2_QTMR_V1_CNTV_CVAL_HI_CNTV_CVAL_HI_SHFT                 0x0

#define HWIO_MPM2_QTMR_V1_CNTV_TVAL_ADDR                          (MPM2_QTIMR_V1_REG_BASE      + 0x00000038)
#define HWIO_MPM2_QTMR_V1_CNTV_TVAL_RMSK                          0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTV_TVAL_IN          \
        in_dword(HWIO_MPM2_QTMR_V1_CNTV_TVAL_ADDR)
#define HWIO_MPM2_QTMR_V1_CNTV_TVAL_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTV_TVAL_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTV_TVAL_OUT(v)      \
        out_dword(HWIO_MPM2_QTMR_V1_CNTV_TVAL_ADDR,v)
#define HWIO_MPM2_QTMR_V1_CNTV_TVAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_QTMR_V1_CNTV_TVAL_ADDR,m,v,HWIO_MPM2_QTMR_V1_CNTV_TVAL_IN)
#define HWIO_MPM2_QTMR_V1_CNTV_TVAL_CNTV_TVAL_BMSK                0xffffffff
#define HWIO_MPM2_QTMR_V1_CNTV_TVAL_CNTV_TVAL_SHFT                       0x0

#define HWIO_MPM2_QTMR_V1_CNTV_CTL_ADDR                           (MPM2_QTIMR_V1_REG_BASE      + 0x0000003c)
#define HWIO_MPM2_QTMR_V1_CNTV_CTL_RMSK                                  0x7
#define HWIO_MPM2_QTMR_V1_CNTV_CTL_IN          \
        in_dword(HWIO_MPM2_QTMR_V1_CNTV_CTL_ADDR)
#define HWIO_MPM2_QTMR_V1_CNTV_CTL_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_CNTV_CTL_ADDR, m)
#define HWIO_MPM2_QTMR_V1_CNTV_CTL_OUT(v)      \
        out_dword(HWIO_MPM2_QTMR_V1_CNTV_CTL_ADDR,v)
#define HWIO_MPM2_QTMR_V1_CNTV_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_MPM2_QTMR_V1_CNTV_CTL_ADDR,m,v,HWIO_MPM2_QTMR_V1_CNTV_CTL_IN)
#define HWIO_MPM2_QTMR_V1_CNTV_CTL_ISTAT_BMSK                            0x4
#define HWIO_MPM2_QTMR_V1_CNTV_CTL_ISTAT_SHFT                            0x2
#define HWIO_MPM2_QTMR_V1_CNTV_CTL_IMSK_BMSK                             0x2
#define HWIO_MPM2_QTMR_V1_CNTV_CTL_IMSK_SHFT                             0x1
#define HWIO_MPM2_QTMR_V1_CNTV_CTL_EN_BMSK                               0x1
#define HWIO_MPM2_QTMR_V1_CNTV_CTL_EN_SHFT                               0x0

#define HWIO_MPM2_QTMR_V1_VERSION_ADDR                            (MPM2_QTIMR_V1_REG_BASE      + 0x00000fd0)
#define HWIO_MPM2_QTMR_V1_VERSION_RMSK                            0xffffffff
#define HWIO_MPM2_QTMR_V1_VERSION_IN          \
        in_dword(HWIO_MPM2_QTMR_V1_VERSION_ADDR)
#define HWIO_MPM2_QTMR_V1_VERSION_INM(m)      \
        in_dword_masked(HWIO_MPM2_QTMR_V1_VERSION_ADDR, m)
#define HWIO_MPM2_QTMR_V1_VERSION_MAJOR_BMSK                      0xf0000000
#define HWIO_MPM2_QTMR_V1_VERSION_MAJOR_SHFT                            0x1c
#define HWIO_MPM2_QTMR_V1_VERSION_MINOR_BMSK                       0xfff0000
#define HWIO_MPM2_QTMR_V1_VERSION_MINOR_SHFT                            0x10
#define HWIO_MPM2_QTMR_V1_VERSION_STEP_BMSK                           0xffff
#define HWIO_MPM2_QTMR_V1_VERSION_STEP_SHFT                              0x0


#endif /* __MPM_HWIO_H__ */
