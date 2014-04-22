#ifndef __QTMR_FRAM3_HWIO_H__
#define __QTMR_FRAM3_HWIO_H__
/*
===========================================================================
*/
/**
  @file QTMR_fram3_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    MDM9x07 (Lykan) [lykan_v1.0_p3q3r163]
 
  This file contains HWIO register definitions for the following modules:
    APCS_F3_QTMR_V1

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
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

  $Header: //components/rel/core.tx/6.0/api/kernel/qurt/qurt_qtmr.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: APCS_F3_QTMR_V1
 *--------------------------------------------------------------------------*/

#define APCS_F3_QTMR_V1_REG_BASE                                     (A7SS_BASE      + 0x00025000)
#define APCS_F3_QTMR_V1_REG_BASE_PHYS                                (A7SS_BASE_PHYS + 0x00025000)
#define APCS_F3_QTMR_V1_REG_BASE_OFFS                                0x00025000

#define HWIO_APCS_F3_QTMR_V1_CNTPCT_LO_ADDR                          (APCS_F3_QTMR_V1_REG_BASE      + 0x00000000)
#define HWIO_APCS_F3_QTMR_V1_CNTPCT_LO_PHYS                          (APCS_F3_QTMR_V1_REG_BASE_PHYS + 0x00000000)
#define HWIO_APCS_F3_QTMR_V1_CNTPCT_LO_OFFS                          (APCS_F3_QTMR_V1_REG_BASE_OFFS + 0x00000000)
#define HWIO_APCS_F3_QTMR_V1_CNTPCT_LO_RMSK                          0xffffffff
#define HWIO_APCS_F3_QTMR_V1_CNTPCT_LO_IN          \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTPCT_LO_ADDR, HWIO_APCS_F3_QTMR_V1_CNTPCT_LO_RMSK)
#define HWIO_APCS_F3_QTMR_V1_CNTPCT_LO_INM(m)      \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTPCT_LO_ADDR, m)
#define HWIO_APCS_F3_QTMR_V1_CNTPCT_LO_CNTPCT_LO_BMSK                0xffffffff
#define HWIO_APCS_F3_QTMR_V1_CNTPCT_LO_CNTPCT_LO_SHFT                       0x0

#define HWIO_APCS_F3_QTMR_V1_CNTPCT_HI_ADDR                          (APCS_F3_QTMR_V1_REG_BASE      + 0x00000004)
#define HWIO_APCS_F3_QTMR_V1_CNTPCT_HI_PHYS                          (APCS_F3_QTMR_V1_REG_BASE_PHYS + 0x00000004)
#define HWIO_APCS_F3_QTMR_V1_CNTPCT_HI_OFFS                          (APCS_F3_QTMR_V1_REG_BASE_OFFS + 0x00000004)
#define HWIO_APCS_F3_QTMR_V1_CNTPCT_HI_RMSK                            0xffffff
#define HWIO_APCS_F3_QTMR_V1_CNTPCT_HI_IN          \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTPCT_HI_ADDR, HWIO_APCS_F3_QTMR_V1_CNTPCT_HI_RMSK)
#define HWIO_APCS_F3_QTMR_V1_CNTPCT_HI_INM(m)      \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTPCT_HI_ADDR, m)
#define HWIO_APCS_F3_QTMR_V1_CNTPCT_HI_CNTPCT_HI_BMSK                  0xffffff
#define HWIO_APCS_F3_QTMR_V1_CNTPCT_HI_CNTPCT_HI_SHFT                       0x0

#define HWIO_APCS_F3_QTMR_V1_CNTVCT_LO_ADDR                          (APCS_F3_QTMR_V1_REG_BASE      + 0x00000008)
#define HWIO_APCS_F3_QTMR_V1_CNTVCT_LO_PHYS                          (APCS_F3_QTMR_V1_REG_BASE_PHYS + 0x00000008)
#define HWIO_APCS_F3_QTMR_V1_CNTVCT_LO_OFFS                          (APCS_F3_QTMR_V1_REG_BASE_OFFS + 0x00000008)
#define HWIO_APCS_F3_QTMR_V1_CNTVCT_LO_RMSK                          0xffffffff
#define HWIO_APCS_F3_QTMR_V1_CNTVCT_LO_IN          \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTVCT_LO_ADDR, HWIO_APCS_F3_QTMR_V1_CNTVCT_LO_RMSK)
#define HWIO_APCS_F3_QTMR_V1_CNTVCT_LO_INM(m)      \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTVCT_LO_ADDR, m)
#define HWIO_APCS_F3_QTMR_V1_CNTVCT_LO_CNTVCT_LO_BMSK                0xffffffff
#define HWIO_APCS_F3_QTMR_V1_CNTVCT_LO_CNTVCT_LO_SHFT                       0x0

#define HWIO_APCS_F3_QTMR_V1_CNTVCT_HI_ADDR                          (APCS_F3_QTMR_V1_REG_BASE      + 0x0000000c)
#define HWIO_APCS_F3_QTMR_V1_CNTVCT_HI_PHYS                          (APCS_F3_QTMR_V1_REG_BASE_PHYS + 0x0000000c)
#define HWIO_APCS_F3_QTMR_V1_CNTVCT_HI_OFFS                          (APCS_F3_QTMR_V1_REG_BASE_OFFS + 0x0000000c)
#define HWIO_APCS_F3_QTMR_V1_CNTVCT_HI_RMSK                            0xffffff
#define HWIO_APCS_F3_QTMR_V1_CNTVCT_HI_IN          \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTVCT_HI_ADDR, HWIO_APCS_F3_QTMR_V1_CNTVCT_HI_RMSK)
#define HWIO_APCS_F3_QTMR_V1_CNTVCT_HI_INM(m)      \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTVCT_HI_ADDR, m)
#define HWIO_APCS_F3_QTMR_V1_CNTVCT_HI_CNTVCT_HI_BMSK                  0xffffff
#define HWIO_APCS_F3_QTMR_V1_CNTVCT_HI_CNTVCT_HI_SHFT                       0x0

#define HWIO_APCS_F3_QTMR_V1_CNTFRQ_ADDR                             (APCS_F3_QTMR_V1_REG_BASE      + 0x00000010)
#define HWIO_APCS_F3_QTMR_V1_CNTFRQ_PHYS                             (APCS_F3_QTMR_V1_REG_BASE_PHYS + 0x00000010)
#define HWIO_APCS_F3_QTMR_V1_CNTFRQ_OFFS                             (APCS_F3_QTMR_V1_REG_BASE_OFFS + 0x00000010)
#define HWIO_APCS_F3_QTMR_V1_CNTFRQ_RMSK                             0xffffffff
#define HWIO_APCS_F3_QTMR_V1_CNTFRQ_IN          \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTFRQ_ADDR, HWIO_APCS_F3_QTMR_V1_CNTFRQ_RMSK)
#define HWIO_APCS_F3_QTMR_V1_CNTFRQ_INM(m)      \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTFRQ_ADDR, m)
#define HWIO_APCS_F3_QTMR_V1_CNTFRQ_CNTFRQ_BMSK                      0xffffffff
#define HWIO_APCS_F3_QTMR_V1_CNTFRQ_CNTFRQ_SHFT                             0x0

#define HWIO_APCS_F3_QTMR_V1_CNTPL0ACR_ADDR                          (APCS_F3_QTMR_V1_REG_BASE      + 0x00000014)
#define HWIO_APCS_F3_QTMR_V1_CNTPL0ACR_PHYS                          (APCS_F3_QTMR_V1_REG_BASE_PHYS + 0x00000014)
#define HWIO_APCS_F3_QTMR_V1_CNTPL0ACR_OFFS                          (APCS_F3_QTMR_V1_REG_BASE_OFFS + 0x00000014)
#define HWIO_APCS_F3_QTMR_V1_CNTPL0ACR_RMSK                               0x303
#define HWIO_APCS_F3_QTMR_V1_CNTPL0ACR_IN          \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTPL0ACR_ADDR, HWIO_APCS_F3_QTMR_V1_CNTPL0ACR_RMSK)
#define HWIO_APCS_F3_QTMR_V1_CNTPL0ACR_INM(m)      \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTPL0ACR_ADDR, m)
#define HWIO_APCS_F3_QTMR_V1_CNTPL0ACR_OUT(v)      \
        out_dword(HWIO_APCS_F3_QTMR_V1_CNTPL0ACR_ADDR,v)
#define HWIO_APCS_F3_QTMR_V1_CNTPL0ACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APCS_F3_QTMR_V1_CNTPL0ACR_ADDR,m,v,HWIO_APCS_F3_QTMR_V1_CNTPL0ACR_IN)
#define HWIO_APCS_F3_QTMR_V1_CNTPL0ACR_PL0CTEN_BMSK                       0x200
#define HWIO_APCS_F3_QTMR_V1_CNTPL0ACR_PL0CTEN_SHFT                         0x9
#define HWIO_APCS_F3_QTMR_V1_CNTPL0ACR_PL0VTEN_BMSK                       0x100
#define HWIO_APCS_F3_QTMR_V1_CNTPL0ACR_PL0VTEN_SHFT                         0x8
#define HWIO_APCS_F3_QTMR_V1_CNTPL0ACR_PL0VCTEN_BMSK                        0x2
#define HWIO_APCS_F3_QTMR_V1_CNTPL0ACR_PL0VCTEN_SHFT                        0x1
#define HWIO_APCS_F3_QTMR_V1_CNTPL0ACR_PL0PCTEN_BMSK                        0x1
#define HWIO_APCS_F3_QTMR_V1_CNTPL0ACR_PL0PCTEN_SHFT                        0x0

#define HWIO_APCS_F3_QTMR_V1_CNTVOFF_LO_ADDR                         (APCS_F3_QTMR_V1_REG_BASE      + 0x00000018)
#define HWIO_APCS_F3_QTMR_V1_CNTVOFF_LO_PHYS                         (APCS_F3_QTMR_V1_REG_BASE_PHYS + 0x00000018)
#define HWIO_APCS_F3_QTMR_V1_CNTVOFF_LO_OFFS                         (APCS_F3_QTMR_V1_REG_BASE_OFFS + 0x00000018)
#define HWIO_APCS_F3_QTMR_V1_CNTVOFF_LO_RMSK                         0xffffffff
#define HWIO_APCS_F3_QTMR_V1_CNTVOFF_LO_IN          \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTVOFF_LO_ADDR, HWIO_APCS_F3_QTMR_V1_CNTVOFF_LO_RMSK)
#define HWIO_APCS_F3_QTMR_V1_CNTVOFF_LO_INM(m)      \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTVOFF_LO_ADDR, m)
#define HWIO_APCS_F3_QTMR_V1_CNTVOFF_LO_CNTVOFF_L0_BMSK              0xffffffff
#define HWIO_APCS_F3_QTMR_V1_CNTVOFF_LO_CNTVOFF_L0_SHFT                     0x0

#define HWIO_APCS_F3_QTMR_V1_CNTVOFF_HI_ADDR                         (APCS_F3_QTMR_V1_REG_BASE      + 0x0000001c)
#define HWIO_APCS_F3_QTMR_V1_CNTVOFF_HI_PHYS                         (APCS_F3_QTMR_V1_REG_BASE_PHYS + 0x0000001c)
#define HWIO_APCS_F3_QTMR_V1_CNTVOFF_HI_OFFS                         (APCS_F3_QTMR_V1_REG_BASE_OFFS + 0x0000001c)
#define HWIO_APCS_F3_QTMR_V1_CNTVOFF_HI_RMSK                           0xffffff
#define HWIO_APCS_F3_QTMR_V1_CNTVOFF_HI_IN          \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTVOFF_HI_ADDR, HWIO_APCS_F3_QTMR_V1_CNTVOFF_HI_RMSK)
#define HWIO_APCS_F3_QTMR_V1_CNTVOFF_HI_INM(m)      \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTVOFF_HI_ADDR, m)
#define HWIO_APCS_F3_QTMR_V1_CNTVOFF_HI_CNTVOFF_HI_BMSK                0xffffff
#define HWIO_APCS_F3_QTMR_V1_CNTVOFF_HI_CNTVOFF_HI_SHFT                     0x0

#define HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_LO_ADDR                       (APCS_F3_QTMR_V1_REG_BASE      + 0x00000020)
#define HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_LO_PHYS                       (APCS_F3_QTMR_V1_REG_BASE_PHYS + 0x00000020)
#define HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_LO_OFFS                       (APCS_F3_QTMR_V1_REG_BASE_OFFS + 0x00000020)
#define HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_LO_RMSK                       0xffffffff
#define HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_LO_IN          \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_LO_ADDR, HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_LO_RMSK)
#define HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_LO_INM(m)      \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_LO_ADDR, m)
#define HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_LO_OUT(v)      \
        out_dword(HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_LO_ADDR,v)
#define HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_LO_ADDR,m,v,HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_LO_IN)
#define HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_LO_CNTP_CVAL_L0_BMSK          0xffffffff
#define HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_LO_CNTP_CVAL_L0_SHFT                 0x0

#define HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_HI_ADDR                       (APCS_F3_QTMR_V1_REG_BASE      + 0x00000024)
#define HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_HI_PHYS                       (APCS_F3_QTMR_V1_REG_BASE_PHYS + 0x00000024)
#define HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_HI_OFFS                       (APCS_F3_QTMR_V1_REG_BASE_OFFS + 0x00000024)
#define HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_HI_RMSK                         0xffffff
#define HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_HI_IN          \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_HI_ADDR, HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_HI_RMSK)
#define HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_HI_INM(m)      \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_HI_ADDR, m)
#define HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_HI_OUT(v)      \
        out_dword(HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_HI_ADDR,v)
#define HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_HI_ADDR,m,v,HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_HI_IN)
#define HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_HI_CNTP_CVAL_HI_BMSK            0xffffff
#define HWIO_APCS_F3_QTMR_V1_CNTP_CVAL_HI_CNTP_CVAL_HI_SHFT                 0x0

#define HWIO_APCS_F3_QTMR_V1_CNTP_TVAL_ADDR                          (APCS_F3_QTMR_V1_REG_BASE      + 0x00000028)
#define HWIO_APCS_F3_QTMR_V1_CNTP_TVAL_PHYS                          (APCS_F3_QTMR_V1_REG_BASE_PHYS + 0x00000028)
#define HWIO_APCS_F3_QTMR_V1_CNTP_TVAL_OFFS                          (APCS_F3_QTMR_V1_REG_BASE_OFFS + 0x00000028)
#define HWIO_APCS_F3_QTMR_V1_CNTP_TVAL_RMSK                          0xffffffff
#define HWIO_APCS_F3_QTMR_V1_CNTP_TVAL_IN          \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTP_TVAL_ADDR, HWIO_APCS_F3_QTMR_V1_CNTP_TVAL_RMSK)
#define HWIO_APCS_F3_QTMR_V1_CNTP_TVAL_INM(m)      \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTP_TVAL_ADDR, m)
#define HWIO_APCS_F3_QTMR_V1_CNTP_TVAL_OUT(v)      \
        out_dword(HWIO_APCS_F3_QTMR_V1_CNTP_TVAL_ADDR,v)
#define HWIO_APCS_F3_QTMR_V1_CNTP_TVAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APCS_F3_QTMR_V1_CNTP_TVAL_ADDR,m,v,HWIO_APCS_F3_QTMR_V1_CNTP_TVAL_IN)
#define HWIO_APCS_F3_QTMR_V1_CNTP_TVAL_CNTP_TVAL_BMSK                0xffffffff
#define HWIO_APCS_F3_QTMR_V1_CNTP_TVAL_CNTP_TVAL_SHFT                       0x0

#define HWIO_APCS_F3_QTMR_V1_CNTP_CTL_ADDR                           (APCS_F3_QTMR_V1_REG_BASE      + 0x0000002c)
#define HWIO_APCS_F3_QTMR_V1_CNTP_CTL_PHYS                           (APCS_F3_QTMR_V1_REG_BASE_PHYS + 0x0000002c)
#define HWIO_APCS_F3_QTMR_V1_CNTP_CTL_OFFS                           (APCS_F3_QTMR_V1_REG_BASE_OFFS + 0x0000002c)
#define HWIO_APCS_F3_QTMR_V1_CNTP_CTL_RMSK                                  0x7
#define HWIO_APCS_F3_QTMR_V1_CNTP_CTL_IN          \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTP_CTL_ADDR, HWIO_APCS_F3_QTMR_V1_CNTP_CTL_RMSK)
#define HWIO_APCS_F3_QTMR_V1_CNTP_CTL_INM(m)      \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTP_CTL_ADDR, m)
#define HWIO_APCS_F3_QTMR_V1_CNTP_CTL_OUT(v)      \
        out_dword(HWIO_APCS_F3_QTMR_V1_CNTP_CTL_ADDR,v)
#define HWIO_APCS_F3_QTMR_V1_CNTP_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APCS_F3_QTMR_V1_CNTP_CTL_ADDR,m,v,HWIO_APCS_F3_QTMR_V1_CNTP_CTL_IN)
#define HWIO_APCS_F3_QTMR_V1_CNTP_CTL_ISTAT_BMSK                            0x4
#define HWIO_APCS_F3_QTMR_V1_CNTP_CTL_ISTAT_SHFT                            0x2
#define HWIO_APCS_F3_QTMR_V1_CNTP_CTL_IMSK_BMSK                             0x2
#define HWIO_APCS_F3_QTMR_V1_CNTP_CTL_IMSK_SHFT                             0x1
#define HWIO_APCS_F3_QTMR_V1_CNTP_CTL_EN_BMSK                               0x1
#define HWIO_APCS_F3_QTMR_V1_CNTP_CTL_EN_SHFT                               0x0

#define HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_LO_ADDR                       (APCS_F3_QTMR_V1_REG_BASE      + 0x00000030)
#define HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_LO_PHYS                       (APCS_F3_QTMR_V1_REG_BASE_PHYS + 0x00000030)
#define HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_LO_OFFS                       (APCS_F3_QTMR_V1_REG_BASE_OFFS + 0x00000030)
#define HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_LO_RMSK                       0xffffffff
#define HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_LO_IN          \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_LO_ADDR, HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_LO_RMSK)
#define HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_LO_INM(m)      \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_LO_ADDR, m)
#define HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_LO_OUT(v)      \
        out_dword(HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_LO_ADDR,v)
#define HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_LO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_LO_ADDR,m,v,HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_LO_IN)
#define HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_LO_CNTV_CVAL_L0_BMSK          0xffffffff
#define HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_LO_CNTV_CVAL_L0_SHFT                 0x0

#define HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_HI_ADDR                       (APCS_F3_QTMR_V1_REG_BASE      + 0x00000034)
#define HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_HI_PHYS                       (APCS_F3_QTMR_V1_REG_BASE_PHYS + 0x00000034)
#define HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_HI_OFFS                       (APCS_F3_QTMR_V1_REG_BASE_OFFS + 0x00000034)
#define HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_HI_RMSK                         0xffffff
#define HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_HI_IN          \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_HI_ADDR, HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_HI_RMSK)
#define HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_HI_INM(m)      \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_HI_ADDR, m)
#define HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_HI_OUT(v)      \
        out_dword(HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_HI_ADDR,v)
#define HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_HI_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_HI_ADDR,m,v,HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_HI_IN)
#define HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_HI_CNTV_CVAL_HI_BMSK            0xffffff
#define HWIO_APCS_F3_QTMR_V1_CNTV_CVAL_HI_CNTV_CVAL_HI_SHFT                 0x0

#define HWIO_APCS_F3_QTMR_V1_CNTV_TVAL_ADDR                          (APCS_F3_QTMR_V1_REG_BASE      + 0x00000038)
#define HWIO_APCS_F3_QTMR_V1_CNTV_TVAL_PHYS                          (APCS_F3_QTMR_V1_REG_BASE_PHYS + 0x00000038)
#define HWIO_APCS_F3_QTMR_V1_CNTV_TVAL_OFFS                          (APCS_F3_QTMR_V1_REG_BASE_OFFS + 0x00000038)
#define HWIO_APCS_F3_QTMR_V1_CNTV_TVAL_RMSK                          0xffffffff
#define HWIO_APCS_F3_QTMR_V1_CNTV_TVAL_IN          \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTV_TVAL_ADDR, HWIO_APCS_F3_QTMR_V1_CNTV_TVAL_RMSK)
#define HWIO_APCS_F3_QTMR_V1_CNTV_TVAL_INM(m)      \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTV_TVAL_ADDR, m)
#define HWIO_APCS_F3_QTMR_V1_CNTV_TVAL_OUT(v)      \
        out_dword(HWIO_APCS_F3_QTMR_V1_CNTV_TVAL_ADDR,v)
#define HWIO_APCS_F3_QTMR_V1_CNTV_TVAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APCS_F3_QTMR_V1_CNTV_TVAL_ADDR,m,v,HWIO_APCS_F3_QTMR_V1_CNTV_TVAL_IN)
#define HWIO_APCS_F3_QTMR_V1_CNTV_TVAL_CNTV_TVAL_BMSK                0xffffffff
#define HWIO_APCS_F3_QTMR_V1_CNTV_TVAL_CNTV_TVAL_SHFT                       0x0

#define HWIO_APCS_F3_QTMR_V1_CNTV_CTL_ADDR                           (APCS_F3_QTMR_V1_REG_BASE      + 0x0000003c)
#define HWIO_APCS_F3_QTMR_V1_CNTV_CTL_PHYS                           (APCS_F3_QTMR_V1_REG_BASE_PHYS + 0x0000003c)
#define HWIO_APCS_F3_QTMR_V1_CNTV_CTL_OFFS                           (APCS_F3_QTMR_V1_REG_BASE_OFFS + 0x0000003c)
#define HWIO_APCS_F3_QTMR_V1_CNTV_CTL_RMSK                                  0x7
#define HWIO_APCS_F3_QTMR_V1_CNTV_CTL_IN          \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTV_CTL_ADDR, HWIO_APCS_F3_QTMR_V1_CNTV_CTL_RMSK)
#define HWIO_APCS_F3_QTMR_V1_CNTV_CTL_INM(m)      \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_CNTV_CTL_ADDR, m)
#define HWIO_APCS_F3_QTMR_V1_CNTV_CTL_OUT(v)      \
        out_dword(HWIO_APCS_F3_QTMR_V1_CNTV_CTL_ADDR,v)
#define HWIO_APCS_F3_QTMR_V1_CNTV_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_APCS_F3_QTMR_V1_CNTV_CTL_ADDR,m,v,HWIO_APCS_F3_QTMR_V1_CNTV_CTL_IN)
#define HWIO_APCS_F3_QTMR_V1_CNTV_CTL_ISTAT_BMSK                            0x4
#define HWIO_APCS_F3_QTMR_V1_CNTV_CTL_ISTAT_SHFT                            0x2
#define HWIO_APCS_F3_QTMR_V1_CNTV_CTL_IMSK_BMSK                             0x2
#define HWIO_APCS_F3_QTMR_V1_CNTV_CTL_IMSK_SHFT                             0x1
#define HWIO_APCS_F3_QTMR_V1_CNTV_CTL_EN_BMSK                               0x1
#define HWIO_APCS_F3_QTMR_V1_CNTV_CTL_EN_SHFT                               0x0

#define HWIO_APCS_F3_QTMR_V1_VERSION_ADDR                            (APCS_F3_QTMR_V1_REG_BASE      + 0x00000fd0)
#define HWIO_APCS_F3_QTMR_V1_VERSION_PHYS                            (APCS_F3_QTMR_V1_REG_BASE_PHYS + 0x00000fd0)
#define HWIO_APCS_F3_QTMR_V1_VERSION_OFFS                            (APCS_F3_QTMR_V1_REG_BASE_OFFS + 0x00000fd0)
#define HWIO_APCS_F3_QTMR_V1_VERSION_RMSK                            0xffffffff
#define HWIO_APCS_F3_QTMR_V1_VERSION_IN          \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_VERSION_ADDR, HWIO_APCS_F3_QTMR_V1_VERSION_RMSK)
#define HWIO_APCS_F3_QTMR_V1_VERSION_INM(m)      \
        in_dword_masked(HWIO_APCS_F3_QTMR_V1_VERSION_ADDR, m)
#define HWIO_APCS_F3_QTMR_V1_VERSION_MAJOR_BMSK                      0xf0000000
#define HWIO_APCS_F3_QTMR_V1_VERSION_MAJOR_SHFT                            0x1c
#define HWIO_APCS_F3_QTMR_V1_VERSION_MINOR_BMSK                       0xfff0000
#define HWIO_APCS_F3_QTMR_V1_VERSION_MINOR_SHFT                            0x10
#define HWIO_APCS_F3_QTMR_V1_VERSION_STEP_BMSK                           0xffff
#define HWIO_APCS_F3_QTMR_V1_VERSION_STEP_SHFT                              0x0


#endif /* __QTMR_FRAM3_HWIO_H__ */
