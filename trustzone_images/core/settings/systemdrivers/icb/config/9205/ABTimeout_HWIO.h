#ifndef __ABTIMEOUT_HWIO_H__
#define __ABTIMEOUT_HWIO_H__
/*
===========================================================================
*/
/**
  @file ABTimeout_HWIO.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    MDM9205 (Twizy) [twizy_v1.0_p2q0r16]
 
  This file contains HWIO register definitions for the following modules:
    PCNOC_0_BUS_TIMEOUT
    PCNOC_1_BUS_TIMEOUT
    PCNOC_2_BUS_TIMEOUT
    PCNOC_3_BUS_TIMEOUT
    PCNOC_4_BUS_TIMEOUT
    PCNOC_5_BUS_TIMEOUT
    PCNOC_6_BUS_TIMEOUT
    PCNOC_7_BUS_TIMEOUT
    TCSR_TCSR_REGS

  'Include' filters applied: BASE[PCNOC_4_BUS_TIMEOUT] BASE[PCNOC_5_BUS_TIMEOUT] BASE[PCNOC_6_BUS_TIMEOUT] BASE[PCNOC_1_BUS_TIMEOUT] BASE[PCNOC_2_BUS_TIMEOUT] BASE[PCNOC_7_BUS_TIMEOUT] BASE[PCNOC_0_BUS_TIMEOUT] TIMEOUT[TCSR_TCSR_REGS] BASE[PCNOC_3_BUS_TIMEOUT] 

  Generation parameters: 
  { u'filename': u'ABTimeout_HWIO.h',
    u'header': u'#include "msmhwiobase.h"',
    u'module-filter-exclude': { },
    u'module-filter-include': { u'PCNOC_0_BUS_TIMEOUT': [u'BASE'],
                                u'PCNOC_1_BUS_TIMEOUT': [u'BASE'],
                                u'PCNOC_2_BUS_TIMEOUT': [u'BASE'],
                                u'PCNOC_3_BUS_TIMEOUT': [u'BASE'],
                                u'PCNOC_4_BUS_TIMEOUT': [u'BASE'],
                                u'PCNOC_5_BUS_TIMEOUT': [u'BASE'],
                                u'PCNOC_6_BUS_TIMEOUT': [u'BASE'],
                                u'PCNOC_7_BUS_TIMEOUT': [u'BASE'],
                                u'TCSR_TCSR_REGS': [u'TIMEOUT']},
    u'modules': [ u'PCNOC_0_BUS_TIMEOUT',
                  u'PCNOC_1_BUS_TIMEOUT',
                  u'PCNOC_2_BUS_TIMEOUT',
                  u'PCNOC_3_BUS_TIMEOUT',
                  u'PCNOC_4_BUS_TIMEOUT',
                  u'PCNOC_5_BUS_TIMEOUT',
                  u'PCNOC_6_BUS_TIMEOUT',
                  u'PCNOC_7_BUS_TIMEOUT',
                  u'TCSR_TCSR_REGS']}
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

  $Header: //components/rel/core.tz/2.1.3/settings/systemdrivers/icb/config/9205/ABTimeout_HWIO.h#1 $
  $DateTime: 2019/05/23 05:43:45 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: PCNOC_0_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_0_BUS_TIMEOUT_REG_BASE                          (PCNOC_0_BUS_TIMEOUT_BASE      + 0x00000000)
#define PCNOC_0_BUS_TIMEOUT_REG_BASE_SIZE                     0x1000
#define PCNOC_0_BUS_TIMEOUT_REG_BASE_USED                     0x34

/*----------------------------------------------------------------------------
 * MODULE: PCNOC_1_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_1_BUS_TIMEOUT_REG_BASE                          (PCNOC_1_BUS_TIMEOUT_BASE      + 0x00000000)
#define PCNOC_1_BUS_TIMEOUT_REG_BASE_SIZE                     0x1000
#define PCNOC_1_BUS_TIMEOUT_REG_BASE_USED                     0x34

/*----------------------------------------------------------------------------
 * MODULE: PCNOC_2_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_2_BUS_TIMEOUT_REG_BASE                          (PCNOC_2_BUS_TIMEOUT_BASE      + 0x00000000)
#define PCNOC_2_BUS_TIMEOUT_REG_BASE_SIZE                     0x1000
#define PCNOC_2_BUS_TIMEOUT_REG_BASE_USED                     0x34

/*----------------------------------------------------------------------------
 * MODULE: PCNOC_3_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_3_BUS_TIMEOUT_REG_BASE                          (PCNOC_3_BUS_TIMEOUT_BASE      + 0x00000000)
#define PCNOC_3_BUS_TIMEOUT_REG_BASE_SIZE                     0x1000
#define PCNOC_3_BUS_TIMEOUT_REG_BASE_USED                     0x34

/*----------------------------------------------------------------------------
 * MODULE: PCNOC_4_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_4_BUS_TIMEOUT_REG_BASE                          (PCNOC_4_BUS_TIMEOUT_BASE      + 0x00000000)
#define PCNOC_4_BUS_TIMEOUT_REG_BASE_SIZE                     0x1000
#define PCNOC_4_BUS_TIMEOUT_REG_BASE_USED                     0x34

/*----------------------------------------------------------------------------
 * MODULE: PCNOC_5_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_5_BUS_TIMEOUT_REG_BASE                          (PCNOC_5_BUS_TIMEOUT_BASE      + 0x00000000)
#define PCNOC_5_BUS_TIMEOUT_REG_BASE_SIZE                     0x1000
#define PCNOC_5_BUS_TIMEOUT_REG_BASE_USED                     0x34

/*----------------------------------------------------------------------------
 * MODULE: PCNOC_6_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_6_BUS_TIMEOUT_REG_BASE                          (PCNOC_6_BUS_TIMEOUT_BASE      + 0x00000000)
#define PCNOC_6_BUS_TIMEOUT_REG_BASE_SIZE                     0x1000
#define PCNOC_6_BUS_TIMEOUT_REG_BASE_USED                     0x34

/*----------------------------------------------------------------------------
 * MODULE: PCNOC_7_BUS_TIMEOUT
 *--------------------------------------------------------------------------*/

#define PCNOC_7_BUS_TIMEOUT_REG_BASE                          (PCNOC_7_BUS_TIMEOUT_BASE      + 0x00000000)
#define PCNOC_7_BUS_TIMEOUT_REG_BASE_SIZE                     0x1000
#define PCNOC_7_BUS_TIMEOUT_REG_BASE_USED                     0x34

/*----------------------------------------------------------------------------
 * MODULE: TCSR_TCSR_REGS
 *--------------------------------------------------------------------------*/

#define TCSR_TCSR_REGS_REG_BASE                                                                                            (CORE_TOP_CSR_BASE      + 0x00037000)
#define TCSR_TCSR_REGS_REG_BASE_SIZE                                                                                       0x21000
#define TCSR_TCSR_REGS_REG_BASE_USED                                                                                       0x20008

#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_ADDR                                                                                (TCSR_TCSR_REGS_REG_BASE      + 0x00005000)
#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_RMSK                                                                                       0x1
#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_IN          \
        in_dword(HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_ADDR)
#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_INM(m)      \
        in_dword_masked(HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_ADDR, m)
#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_OUT(v)      \
        out_dword(HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_ADDR,v)
#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_ADDR,m,v,HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_IN)
#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_TIMEOUT_SLAVE_GLB_EN_BMSK                                                                  0x1
#define HWIO_TCSR_TIMEOUT_SLAVE_GLB_EN_TIMEOUT_SLAVE_GLB_EN_SHFT                                                                  0x0

#define HWIO_TCSR_MDP3_TIMEOUT_VAL_0_ADDR_ADDR                                                                             (TCSR_TCSR_REGS_REG_BASE      + 0x00005014)
#define HWIO_TCSR_MDP3_TIMEOUT_VAL_0_ADDR_RMSK                                                                                   0xff
#define HWIO_TCSR_MDP3_TIMEOUT_VAL_0_ADDR_IN          \
        in_dword(HWIO_TCSR_MDP3_TIMEOUT_VAL_0_ADDR_ADDR)
#define HWIO_TCSR_MDP3_TIMEOUT_VAL_0_ADDR_INM(m)      \
        in_dword_masked(HWIO_TCSR_MDP3_TIMEOUT_VAL_0_ADDR_ADDR, m)
#define HWIO_TCSR_MDP3_TIMEOUT_VAL_0_ADDR_OUT(v)      \
        out_dword(HWIO_TCSR_MDP3_TIMEOUT_VAL_0_ADDR_ADDR,v)
#define HWIO_TCSR_MDP3_TIMEOUT_VAL_0_ADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_MDP3_TIMEOUT_VAL_0_ADDR_ADDR,m,v,HWIO_TCSR_MDP3_TIMEOUT_VAL_0_ADDR_IN)
#define HWIO_TCSR_MDP3_TIMEOUT_VAL_0_ADDR_MDP3_TIMEOUT_VAL_L_BMSK                                                                0xff
#define HWIO_TCSR_MDP3_TIMEOUT_VAL_0_ADDR_MDP3_TIMEOUT_VAL_L_SHFT                                                                 0x0

#define HWIO_TCSR_MDP3_TIMEOUT_VAL_1_ADDR_ADDR                                                                             (TCSR_TCSR_REGS_REG_BASE      + 0x00005018)
#define HWIO_TCSR_MDP3_TIMEOUT_VAL_1_ADDR_RMSK                                                                                   0xff
#define HWIO_TCSR_MDP3_TIMEOUT_VAL_1_ADDR_IN          \
        in_dword(HWIO_TCSR_MDP3_TIMEOUT_VAL_1_ADDR_ADDR)
#define HWIO_TCSR_MDP3_TIMEOUT_VAL_1_ADDR_INM(m)      \
        in_dword_masked(HWIO_TCSR_MDP3_TIMEOUT_VAL_1_ADDR_ADDR, m)
#define HWIO_TCSR_MDP3_TIMEOUT_VAL_1_ADDR_OUT(v)      \
        out_dword(HWIO_TCSR_MDP3_TIMEOUT_VAL_1_ADDR_ADDR,v)
#define HWIO_TCSR_MDP3_TIMEOUT_VAL_1_ADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_MDP3_TIMEOUT_VAL_1_ADDR_ADDR,m,v,HWIO_TCSR_MDP3_TIMEOUT_VAL_1_ADDR_IN)
#define HWIO_TCSR_MDP3_TIMEOUT_VAL_1_ADDR_MDP3_TIMEOUT_VAL_H_BMSK                                                                0xff
#define HWIO_TCSR_MDP3_TIMEOUT_VAL_1_ADDR_MDP3_TIMEOUT_VAL_H_SHFT                                                                 0x0

#define HWIO_TCSR_MDP3_TIMEOUT_EN_ADDR                                                                                     (TCSR_TCSR_REGS_REG_BASE      + 0x0000501c)
#define HWIO_TCSR_MDP3_TIMEOUT_EN_RMSK                                                                                            0x1
#define HWIO_TCSR_MDP3_TIMEOUT_EN_IN          \
        in_dword(HWIO_TCSR_MDP3_TIMEOUT_EN_ADDR)
#define HWIO_TCSR_MDP3_TIMEOUT_EN_INM(m)      \
        in_dword_masked(HWIO_TCSR_MDP3_TIMEOUT_EN_ADDR, m)
#define HWIO_TCSR_MDP3_TIMEOUT_EN_OUT(v)      \
        out_dword(HWIO_TCSR_MDP3_TIMEOUT_EN_ADDR,v)
#define HWIO_TCSR_MDP3_TIMEOUT_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_MDP3_TIMEOUT_EN_ADDR,m,v,HWIO_TCSR_MDP3_TIMEOUT_EN_IN)
#define HWIO_TCSR_MDP3_TIMEOUT_EN_MDP3_TIMEOUT_EN_BMSK                                                                            0x1
#define HWIO_TCSR_MDP3_TIMEOUT_EN_MDP3_TIMEOUT_EN_SHFT                                                                            0x0

#define HWIO_TCSR_TIMEOUT_INTR_STATUS_ADDR                                                                                 (TCSR_TCSR_REGS_REG_BASE      + 0x00016020)
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_RMSK                                                                                   0x5061ff
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_IN          \
        in_dword(HWIO_TCSR_TIMEOUT_INTR_STATUS_ADDR)
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_INM(m)      \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_STATUS_ADDR, m)
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_ULTAUDIO_AHBI_TIMEOUT_IRQ_BMSK                                                         0x400000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_ULTAUDIO_AHBI_TIMEOUT_IRQ_SHFT                                                             0x16
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_MSS_CONFIG_TIMEOUT_IRQ_BMSK                                                            0x100000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_MSS_CONFIG_TIMEOUT_IRQ_SHFT                                                                0x14
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_SNOC_S1_TIMEOUT_IRQ_BMSK                                                                 0x4000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_SNOC_S1_TIMEOUT_IRQ_SHFT                                                                    0xe
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_SNOC_S0_TIMEOUT_IRQ_BMSK                                                                 0x2000
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_SNOC_S0_TIMEOUT_IRQ_SHFT                                                                    0xd
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_USB_TIMEOUT_IRQ_BMSK                                                                0x100
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_USB_TIMEOUT_IRQ_SHFT                                                                  0x8
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S7_TIMEOUT_IRQ_BMSK                                                                  0x80
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S7_TIMEOUT_IRQ_SHFT                                                                   0x7
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S6_TIMEOUT_IRQ_BMSK                                                                  0x40
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S6_TIMEOUT_IRQ_SHFT                                                                   0x6
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S5_TIMEOUT_IRQ_BMSK                                                                  0x20
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S5_TIMEOUT_IRQ_SHFT                                                                   0x5
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S4_TIMEOUT_IRQ_BMSK                                                                  0x10
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S4_TIMEOUT_IRQ_SHFT                                                                   0x4
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S3_TIMEOUT_IRQ_BMSK                                                                   0x8
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S3_TIMEOUT_IRQ_SHFT                                                                   0x3
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S2_TIMEOUT_IRQ_BMSK                                                                   0x4
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S2_TIMEOUT_IRQ_SHFT                                                                   0x2
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S1_TIMEOUT_IRQ_BMSK                                                                   0x2
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S1_TIMEOUT_IRQ_SHFT                                                                   0x1
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S0_TIMEOUT_IRQ_BMSK                                                                   0x1
#define HWIO_TCSR_TIMEOUT_INTR_STATUS_PCNOC_S0_TIMEOUT_IRQ_SHFT                                                                   0x0

#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_ADDR                                                                             (TCSR_TCSR_REGS_REG_BASE      + 0x00016030)
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_RMSK                                                                               0x5060ff
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_IN          \
        in_dword(HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_ADDR)
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_INM(m)      \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_ADDR, m)
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_OUT(v)      \
        out_dword(HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_ADDR,v)
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_ADDR,m,v,HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_IN)
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_ULTAUDIO_AHBI_TIMEOUT_IRQ_RPM_ENABLE_BMSK                                          0x400000
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_ULTAUDIO_AHBI_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                              0x16
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_MSS_CONFIG_TIMEOUT_IRQ_RPM_ENABLE_BMSK                                             0x100000
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_MSS_CONFIG_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                                 0x14
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_SNOC_S1_TIMEOUT_IRQ_RPM_ENABLE_BMSK                                                  0x4000
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_SNOC_S1_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                                     0xe
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_SNOC_S0_TIMEOUT_IRQ_RPM_ENABLE_BMSK                                                  0x2000
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_SNOC_S0_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                                     0xd
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S7_TIMEOUT_IRQ_RPM_ENABLE_BMSK                                                   0x80
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S7_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                                    0x7
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S6_TIMEOUT_IRQ_RPM_ENABLE_BMSK                                                   0x40
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S6_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                                    0x6
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S5_TIMEOUT_IRQ_RPM_ENABLE_BMSK                                                   0x20
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S5_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                                    0x5
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S4_TIMEOUT_IRQ_RPM_ENABLE_BMSK                                                   0x10
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S4_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                                    0x4
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S3_TIMEOUT_IRQ_RPM_ENABLE_BMSK                                                    0x8
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S3_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                                    0x3
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S2_TIMEOUT_IRQ_RPM_ENABLE_BMSK                                                    0x4
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S2_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                                    0x2
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S1_TIMEOUT_IRQ_RPM_ENABLE_BMSK                                                    0x2
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S1_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                                    0x1
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S0_TIMEOUT_IRQ_RPM_ENABLE_BMSK                                                    0x1
#define HWIO_TCSR_TIMEOUT_INTR_RPM_ENABLE_PCNOC_S0_TIMEOUT_IRQ_RPM_ENABLE_SHFT                                                    0x0

#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_ADDR                                                                            (TCSR_TCSR_REGS_REG_BASE      + 0x00016040)
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_RMSK                                                                              0x5060ff
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_IN          \
        in_dword(HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_ADDR)
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_INM(m)      \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_ADDR, m)
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_OUT(v)      \
        out_dword(HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_ADDR,v)
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_ADDR,m,v,HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_IN)
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_ULTAUDIO_AHBI_TIMEOUT_IRQ_APSS_ENABLE_BMSK                                        0x400000
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_ULTAUDIO_AHBI_TIMEOUT_IRQ_APSS_ENABLE_SHFT                                            0x16
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_MSS_CONFIG_TIMEOUT_IRQ_APSS_ENABLE_BMSK                                           0x100000
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_MSS_CONFIG_TIMEOUT_IRQ_APSS_ENABLE_SHFT                                               0x14
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_SNOC_S1_TIMEOUT_IRQ_APSS_ENABLE_BMSK                                                0x4000
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_SNOC_S1_TIMEOUT_IRQ_APSS_ENABLE_SHFT                                                   0xe
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_SNOC_S0_TIMEOUT_IRQ_APSS_ENABLE_BMSK                                                0x2000
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_SNOC_S0_TIMEOUT_IRQ_APSS_ENABLE_SHFT                                                   0xd
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S7_TIMEOUT_IRQ_APSS_ENABLE_BMSK                                                 0x80
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S7_TIMEOUT_IRQ_APSS_ENABLE_SHFT                                                  0x7
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S6_TIMEOUT_IRQ_APSS_ENABLE_BMSK                                                 0x40
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S6_TIMEOUT_IRQ_APSS_ENABLE_SHFT                                                  0x6
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S5_TIMEOUT_IRQ_APSS_ENABLE_BMSK                                                 0x20
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S5_TIMEOUT_IRQ_APSS_ENABLE_SHFT                                                  0x5
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S4_TIMEOUT_IRQ_APSS_ENABLE_BMSK                                                 0x10
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S4_TIMEOUT_IRQ_APSS_ENABLE_SHFT                                                  0x4
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S3_TIMEOUT_IRQ_APSS_ENABLE_BMSK                                                  0x8
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S3_TIMEOUT_IRQ_APSS_ENABLE_SHFT                                                  0x3
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S2_TIMEOUT_IRQ_APSS_ENABLE_BMSK                                                  0x4
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S2_TIMEOUT_IRQ_APSS_ENABLE_SHFT                                                  0x2
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S1_TIMEOUT_IRQ_APSS_ENABLE_BMSK                                                  0x2
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S1_TIMEOUT_IRQ_APSS_ENABLE_SHFT                                                  0x1
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S0_TIMEOUT_IRQ_APSS_ENABLE_BMSK                                                  0x1
#define HWIO_TCSR_TIMEOUT_INTR_APSS_ENABLE_PCNOC_S0_TIMEOUT_IRQ_APSS_ENABLE_SHFT                                                  0x0

#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_ADDR                                                                             (TCSR_TCSR_REGS_REG_BASE      + 0x00016060)
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_RMSK                                                                               0x5060ff
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_IN          \
        in_dword(HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_ADDR)
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_INM(m)      \
        in_dword_masked(HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_ADDR, m)
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_OUT(v)      \
        out_dword(HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_ADDR,v)
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_ADDR,m,v,HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_IN)
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_ULTAUDIO_AHBI_TIMEOUT_IRQ_MSS_ENABLE_BMSK                                          0x400000
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_ULTAUDIO_AHBI_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                              0x16
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_MSS_CONFIG_TIMEOUT_IRQ_MSS_ENABLE_BMSK                                             0x100000
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_MSS_CONFIG_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                                 0x14
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_SNOC_S1_TIMEOUT_IRQ_MSS_ENABLE_BMSK                                                  0x4000
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_SNOC_S1_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                                     0xe
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_SNOC_S0_TIMEOUT_IRQ_MSS_ENABLE_BMSK                                                  0x2000
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_SNOC_S0_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                                     0xd
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S7_TIMEOUT_IRQ_MSS_ENABLE_BMSK                                                   0x80
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S7_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                                    0x7
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S6_TIMEOUT_IRQ_MSS_ENABLE_BMSK                                                   0x40
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S6_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                                    0x6
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S5_TIMEOUT_IRQ_MSS_ENABLE_BMSK                                                   0x20
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S5_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                                    0x5
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S4_TIMEOUT_IRQ_MSS_ENABLE_BMSK                                                   0x10
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S4_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                                    0x4
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S3_TIMEOUT_IRQ_MSS_ENABLE_BMSK                                                    0x8
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S3_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                                    0x3
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S2_TIMEOUT_IRQ_MSS_ENABLE_BMSK                                                    0x4
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S2_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                                    0x2
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S1_TIMEOUT_IRQ_MSS_ENABLE_BMSK                                                    0x2
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S1_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                                    0x1
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S0_TIMEOUT_IRQ_MSS_ENABLE_BMSK                                                    0x1
#define HWIO_TCSR_TIMEOUT_INTR_MSS_ENABLE_PCNOC_S0_TIMEOUT_IRQ_MSS_ENABLE_SHFT                                                    0x0

#define HWIO_TCSR_PRONTO_BUS_TIMEOUT_RESET_ADDR                                                                            (TCSR_TCSR_REGS_REG_BASE      + 0x00016070)
#define HWIO_TCSR_PRONTO_BUS_TIMEOUT_RESET_RMSK                                                                                   0x1
#define HWIO_TCSR_PRONTO_BUS_TIMEOUT_RESET_IN          \
        in_dword(HWIO_TCSR_PRONTO_BUS_TIMEOUT_RESET_ADDR)
#define HWIO_TCSR_PRONTO_BUS_TIMEOUT_RESET_INM(m)      \
        in_dword_masked(HWIO_TCSR_PRONTO_BUS_TIMEOUT_RESET_ADDR, m)
#define HWIO_TCSR_PRONTO_BUS_TIMEOUT_RESET_OUT(v)      \
        out_dword(HWIO_TCSR_PRONTO_BUS_TIMEOUT_RESET_ADDR,v)
#define HWIO_TCSR_PRONTO_BUS_TIMEOUT_RESET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TCSR_PRONTO_BUS_TIMEOUT_RESET_ADDR,m,v,HWIO_TCSR_PRONTO_BUS_TIMEOUT_RESET_IN)
#define HWIO_TCSR_PRONTO_BUS_TIMEOUT_RESET_WCSS_BUS_TIMEOUT_NOC_SOFT_RESET_BMSK                                                   0x1
#define HWIO_TCSR_PRONTO_BUS_TIMEOUT_RESET_WCSS_BUS_TIMEOUT_NOC_SOFT_RESET_SHFT                                                   0x0


#endif /* __ABTIMEOUT_HWIO_H__ */
