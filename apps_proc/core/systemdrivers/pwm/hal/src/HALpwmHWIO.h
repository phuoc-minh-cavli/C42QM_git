#ifndef __HALPWMHWIO_H__
#define __HALPWMHWIO_H__
/*
===========================================================================
*/
/**
  @file HALpwmHWIO.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    MDM9205 (Twizy) [twizy_v1.0_p3q3r35_MTO]
 
  This file contains HWIO register definitions for the following modules:
    PDM_WEB_PWM


  Generation parameters: 
  { u'explicit-addressing': True,
    u'filename': u'HALpwmHWIO.h',
    u'header': u'#include "msmhwiobase.h"',
    u'ignore-prefixes': True,
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [u'PDM_WEB_PWM'],
    u'output-offsets': True,
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

  $Header: //components/rel/core.tx/6.0/systemdrivers/pwm/hal/src/HALpwmHWIO.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: WEB_PWM
 *--------------------------------------------------------------------------*/

#define WEB_PWM_REG_BASE                                      (PDM_PERPH_WEB_BASE      + 0x00000000)
#define WEB_PWM_REG_BASE_SIZE                                 0xc000
#define WEB_PWM_REG_BASE_USED                                 0xa014
#define WEB_PWM_REG_BASE_OFFS                                 0x00000000

#define HWIO_WEB_PWM_TOPCTL0_ADDR(x)                          ((x) + 0x00000000)
#define HWIO_WEB_PWM_TOPCTL0_OFFS                             (0x00000000)
#define HWIO_WEB_PWM_TOPCTL0_RMSK                                 0xffff
#define HWIO_WEB_PWM_TOPCTL0_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM_TOPCTL0_ADDR(x), HWIO_WEB_PWM_TOPCTL0_RMSK)
#define HWIO_WEB_PWM_TOPCTL0_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM_TOPCTL0_ADDR(x), m)
#define HWIO_WEB_PWM_TOPCTL0_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM_TOPCTL0_ADDR(x),v)
#define HWIO_WEB_PWM_TOPCTL0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM_TOPCTL0_ADDR(x),m,v,HWIO_WEB_PWM_TOPCTL0_IN(x))
#define HWIO_WEB_PWM_TOPCTL0_DEBUG_BUS_SEL_BMSK                   0xf000
#define HWIO_WEB_PWM_TOPCTL0_DEBUG_BUS_SEL_SHFT                      0xc
#define HWIO_WEB_PWM_TOPCTL0_DEBUG_BUS_EN_BMSK                     0x800
#define HWIO_WEB_PWM_TOPCTL0_DEBUG_BUS_EN_SHFT                       0xb
#define HWIO_WEB_PWM_TOPCTL0_SW_EVENT_EN_BMSK                      0x400
#define HWIO_WEB_PWM_TOPCTL0_SW_EVENT_EN_SHFT                        0xa
#define HWIO_WEB_PWM_TOPCTL0_PWM_CLK_EN_BMSK                       0x3ff
#define HWIO_WEB_PWM_TOPCTL0_PWM_CLK_EN_SHFT                         0x0

#define HWIO_WEB_PWM_TOPCTL1_ADDR(x)                          ((x) + 0x00000004)
#define HWIO_WEB_PWM_TOPCTL1_OFFS                             (0x00000004)
#define HWIO_WEB_PWM_TOPCTL1_RMSK                                  0x3ff
#define HWIO_WEB_PWM_TOPCTL1_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM_TOPCTL1_ADDR(x), HWIO_WEB_PWM_TOPCTL1_RMSK)
#define HWIO_WEB_PWM_TOPCTL1_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM_TOPCTL1_ADDR(x), m)
#define HWIO_WEB_PWM_TOPCTL1_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM_TOPCTL1_ADDR(x),v)
#define HWIO_WEB_PWM_TOPCTL1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM_TOPCTL1_ADDR(x),m,v,HWIO_WEB_PWM_TOPCTL1_IN(x))
#define HWIO_WEB_PWM_TOPCTL1_PWM_OEN_BMSK                          0x3ff
#define HWIO_WEB_PWM_TOPCTL1_PWM_OEN_SHFT                            0x0

#define HWIO_WEB_PWM_VERSION_ADDR(x)                          ((x) + 0x00000008)
#define HWIO_WEB_PWM_VERSION_OFFS                             (0x00000008)
#define HWIO_WEB_PWM_VERSION_RMSK                             0xffffffff
#define HWIO_WEB_PWM_VERSION_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM_VERSION_ADDR(x), HWIO_WEB_PWM_VERSION_RMSK)
#define HWIO_WEB_PWM_VERSION_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM_VERSION_ADDR(x), m)
#define HWIO_WEB_PWM_VERSION_PWM_VERSION_BMSK                 0xffffffff
#define HWIO_WEB_PWM_VERSION_PWM_VERSION_SHFT                        0x0

#define HWIO_WEB_PWM_EN_STATUS_ADDR(x)                        ((x) + 0x0000000c)
#define HWIO_WEB_PWM_EN_STATUS_OFFS                           (0x0000000c)
#define HWIO_WEB_PWM_EN_STATUS_RMSK                               0xffff
#define HWIO_WEB_PWM_EN_STATUS_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM_EN_STATUS_ADDR(x), HWIO_WEB_PWM_EN_STATUS_RMSK)
#define HWIO_WEB_PWM_EN_STATUS_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM_EN_STATUS_ADDR(x), m)
#define HWIO_WEB_PWM_EN_STATUS_PWM_OEN_BMSK                       0xf800
#define HWIO_WEB_PWM_EN_STATUS_PWM_OEN_SHFT                          0xb
#define HWIO_WEB_PWM_EN_STATUS_PWM_CLK_EN_BMSK                     0x7fe
#define HWIO_WEB_PWM_EN_STATUS_PWM_CLK_EN_SHFT                       0x1
#define HWIO_WEB_PWM_EN_STATUS_PWM_EN_BMSK                           0x1
#define HWIO_WEB_PWM_EN_STATUS_PWM_EN_SHFT                           0x0

#define HWIO_WEB_PWM0_CTL0_ADDR(x)                            ((x) + 0x00001000)
#define HWIO_WEB_PWM0_CTL0_OFFS                               (0x00001000)
#define HWIO_WEB_PWM0_CTL0_RMSK                                      0xf
#define HWIO_WEB_PWM0_CTL0_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM0_CTL0_ADDR(x), HWIO_WEB_PWM0_CTL0_RMSK)
#define HWIO_WEB_PWM0_CTL0_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM0_CTL0_ADDR(x), m)
#define HWIO_WEB_PWM0_CTL0_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM0_CTL0_ADDR(x),v)
#define HWIO_WEB_PWM0_CTL0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM0_CTL0_ADDR(x),m,v,HWIO_WEB_PWM0_CTL0_IN(x))
#define HWIO_WEB_PWM0_CTL0_LOCKED_MODE_BMSK                          0x8
#define HWIO_WEB_PWM0_CTL0_LOCKED_MODE_SHFT                          0x3
#define HWIO_WEB_PWM0_CTL0_DIV_FACT_BMSK                             0x6
#define HWIO_WEB_PWM0_CTL0_DIV_FACT_SHFT                             0x1
#define HWIO_WEB_PWM0_CTL0_POL_BMSK                                  0x1
#define HWIO_WEB_PWM0_CTL0_POL_SHFT                                  0x0

#define HWIO_WEB_PWM0_CTL1_ADDR(x)                            ((x) + 0x00001004)
#define HWIO_WEB_PWM0_CTL1_OFFS                               (0x00001004)
#define HWIO_WEB_PWM0_CTL1_RMSK                                   0xffff
#define HWIO_WEB_PWM0_CTL1_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM0_CTL1_ADDR(x), HWIO_WEB_PWM0_CTL1_RMSK)
#define HWIO_WEB_PWM0_CTL1_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM0_CTL1_ADDR(x), m)
#define HWIO_WEB_PWM0_CTL1_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM0_CTL1_ADDR(x),v)
#define HWIO_WEB_PWM0_CTL1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM0_CTL1_ADDR(x),m,v,HWIO_WEB_PWM0_CTL1_IN(x))
#define HWIO_WEB_PWM0_CTL1_CYC_OFFSET1_BMSK                       0xffff
#define HWIO_WEB_PWM0_CTL1_CYC_OFFSET1_SHFT                          0x0

#define HWIO_WEB_PWM0_CTL2_ADDR(x)                            ((x) + 0x00001008)
#define HWIO_WEB_PWM0_CTL2_OFFS                               (0x00001008)
#define HWIO_WEB_PWM0_CTL2_RMSK                                   0xffff
#define HWIO_WEB_PWM0_CTL2_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM0_CTL2_ADDR(x), HWIO_WEB_PWM0_CTL2_RMSK)
#define HWIO_WEB_PWM0_CTL2_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM0_CTL2_ADDR(x), m)
#define HWIO_WEB_PWM0_CTL2_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM0_CTL2_ADDR(x),v)
#define HWIO_WEB_PWM0_CTL2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM0_CTL2_ADDR(x),m,v,HWIO_WEB_PWM0_CTL2_IN(x))
#define HWIO_WEB_PWM0_CTL2_CYC_OFFSET2_BMSK                       0xffff
#define HWIO_WEB_PWM0_CTL2_CYC_OFFSET2_SHFT                          0x0

#define HWIO_WEB_PWM0_CYC_CFG_ADDR(x)                         ((x) + 0x0000100c)
#define HWIO_WEB_PWM0_CYC_CFG_OFFS                            (0x0000100c)
#define HWIO_WEB_PWM0_CYC_CFG_RMSK                                0xffff
#define HWIO_WEB_PWM0_CYC_CFG_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM0_CYC_CFG_ADDR(x), HWIO_WEB_PWM0_CYC_CFG_RMSK)
#define HWIO_WEB_PWM0_CYC_CFG_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM0_CYC_CFG_ADDR(x), m)
#define HWIO_WEB_PWM0_CYC_CFG_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM0_CYC_CFG_ADDR(x),v)
#define HWIO_WEB_PWM0_CYC_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM0_CYC_CFG_ADDR(x),m,v,HWIO_WEB_PWM0_CYC_CFG_IN(x))
#define HWIO_WEB_PWM0_CYC_CFG_CYCTIME_BMSK                        0xffff
#define HWIO_WEB_PWM0_CYC_CFG_CYCTIME_SHFT                           0x0

#define HWIO_WEB_PWM0_UPDATE_ADDR(x)                          ((x) + 0x00001010)
#define HWIO_WEB_PWM0_UPDATE_OFFS                             (0x00001010)
#define HWIO_WEB_PWM0_UPDATE_RMSK                                    0x1
#define HWIO_WEB_PWM0_UPDATE_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM0_UPDATE_ADDR(x),v)
#define HWIO_WEB_PWM0_UPDATE_UPD_EN_BMSK                             0x1
#define HWIO_WEB_PWM0_UPDATE_UPD_EN_SHFT                             0x0

#define HWIO_WEB_PWM0_PERIOD_CNT_ADDR(x)                      ((x) + 0x00001014)
#define HWIO_WEB_PWM0_PERIOD_CNT_OFFS                         (0x00001014)
#define HWIO_WEB_PWM0_PERIOD_CNT_RMSK                             0xffff
#define HWIO_WEB_PWM0_PERIOD_CNT_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM0_PERIOD_CNT_ADDR(x), HWIO_WEB_PWM0_PERIOD_CNT_RMSK)
#define HWIO_WEB_PWM0_PERIOD_CNT_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM0_PERIOD_CNT_ADDR(x), m)
#define HWIO_WEB_PWM0_PERIOD_CNT_PERIOD_CNT_BMSK                  0xffff
#define HWIO_WEB_PWM0_PERIOD_CNT_PERIOD_CNT_SHFT                     0x0

#define HWIO_WEB_PWM1_CTL0_ADDR(x)                            ((x) + 0x00002000)
#define HWIO_WEB_PWM1_CTL0_OFFS                               (0x00002000)
#define HWIO_WEB_PWM1_CTL0_RMSK                                      0xf
#define HWIO_WEB_PWM1_CTL0_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM1_CTL0_ADDR(x), HWIO_WEB_PWM1_CTL0_RMSK)
#define HWIO_WEB_PWM1_CTL0_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM1_CTL0_ADDR(x), m)
#define HWIO_WEB_PWM1_CTL0_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM1_CTL0_ADDR(x),v)
#define HWIO_WEB_PWM1_CTL0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM1_CTL0_ADDR(x),m,v,HWIO_WEB_PWM1_CTL0_IN(x))
#define HWIO_WEB_PWM1_CTL0_LOCKED_MODE_BMSK                          0x8
#define HWIO_WEB_PWM1_CTL0_LOCKED_MODE_SHFT                          0x3
#define HWIO_WEB_PWM1_CTL0_DIV_FACT_BMSK                             0x6
#define HWIO_WEB_PWM1_CTL0_DIV_FACT_SHFT                             0x1
#define HWIO_WEB_PWM1_CTL0_POL_BMSK                                  0x1
#define HWIO_WEB_PWM1_CTL0_POL_SHFT                                  0x0

#define HWIO_WEB_PWM1_CTL1_ADDR(x)                            ((x) + 0x00002004)
#define HWIO_WEB_PWM1_CTL1_OFFS                               (0x00002004)
#define HWIO_WEB_PWM1_CTL1_RMSK                                   0xffff
#define HWIO_WEB_PWM1_CTL1_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM1_CTL1_ADDR(x), HWIO_WEB_PWM1_CTL1_RMSK)
#define HWIO_WEB_PWM1_CTL1_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM1_CTL1_ADDR(x), m)
#define HWIO_WEB_PWM1_CTL1_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM1_CTL1_ADDR(x),v)
#define HWIO_WEB_PWM1_CTL1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM1_CTL1_ADDR(x),m,v,HWIO_WEB_PWM1_CTL1_IN(x))
#define HWIO_WEB_PWM1_CTL1_CYC_OFFSET1_BMSK                       0xffff
#define HWIO_WEB_PWM1_CTL1_CYC_OFFSET1_SHFT                          0x0

#define HWIO_WEB_PWM1_CTL2_ADDR(x)                            ((x) + 0x00002008)
#define HWIO_WEB_PWM1_CTL2_OFFS                               (0x00002008)
#define HWIO_WEB_PWM1_CTL2_RMSK                                   0xffff
#define HWIO_WEB_PWM1_CTL2_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM1_CTL2_ADDR(x), HWIO_WEB_PWM1_CTL2_RMSK)
#define HWIO_WEB_PWM1_CTL2_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM1_CTL2_ADDR(x), m)
#define HWIO_WEB_PWM1_CTL2_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM1_CTL2_ADDR(x),v)
#define HWIO_WEB_PWM1_CTL2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM1_CTL2_ADDR(x),m,v,HWIO_WEB_PWM1_CTL2_IN(x))
#define HWIO_WEB_PWM1_CTL2_CYC_OFFSET2_BMSK                       0xffff
#define HWIO_WEB_PWM1_CTL2_CYC_OFFSET2_SHFT                          0x0

#define HWIO_WEB_PWM1_CYC_CFG_ADDR(x)                         ((x) + 0x0000200c)
#define HWIO_WEB_PWM1_CYC_CFG_OFFS                            (0x0000200c)
#define HWIO_WEB_PWM1_CYC_CFG_RMSK                                0xffff
#define HWIO_WEB_PWM1_CYC_CFG_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM1_CYC_CFG_ADDR(x), HWIO_WEB_PWM1_CYC_CFG_RMSK)
#define HWIO_WEB_PWM1_CYC_CFG_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM1_CYC_CFG_ADDR(x), m)
#define HWIO_WEB_PWM1_CYC_CFG_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM1_CYC_CFG_ADDR(x),v)
#define HWIO_WEB_PWM1_CYC_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM1_CYC_CFG_ADDR(x),m,v,HWIO_WEB_PWM1_CYC_CFG_IN(x))
#define HWIO_WEB_PWM1_CYC_CFG_CYCTIME_BMSK                        0xffff
#define HWIO_WEB_PWM1_CYC_CFG_CYCTIME_SHFT                           0x0

#define HWIO_WEB_PWM1_UPDATE_ADDR(x)                          ((x) + 0x00002010)
#define HWIO_WEB_PWM1_UPDATE_OFFS                             (0x00002010)
#define HWIO_WEB_PWM1_UPDATE_RMSK                                    0x1
#define HWIO_WEB_PWM1_UPDATE_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM1_UPDATE_ADDR(x),v)
#define HWIO_WEB_PWM1_UPDATE_UPD_EN_BMSK                             0x1
#define HWIO_WEB_PWM1_UPDATE_UPD_EN_SHFT                             0x0

#define HWIO_WEB_PWM1_PERIOD_CNT_ADDR(x)                      ((x) + 0x00002014)
#define HWIO_WEB_PWM1_PERIOD_CNT_OFFS                         (0x00002014)
#define HWIO_WEB_PWM1_PERIOD_CNT_RMSK                             0xffff
#define HWIO_WEB_PWM1_PERIOD_CNT_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM1_PERIOD_CNT_ADDR(x), HWIO_WEB_PWM1_PERIOD_CNT_RMSK)
#define HWIO_WEB_PWM1_PERIOD_CNT_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM1_PERIOD_CNT_ADDR(x), m)
#define HWIO_WEB_PWM1_PERIOD_CNT_PERIOD_CNT_BMSK                  0xffff
#define HWIO_WEB_PWM1_PERIOD_CNT_PERIOD_CNT_SHFT                     0x0

#define HWIO_WEB_PWM2_CTL0_ADDR(x)                            ((x) + 0x00003000)
#define HWIO_WEB_PWM2_CTL0_OFFS                               (0x00003000)
#define HWIO_WEB_PWM2_CTL0_RMSK                                      0xf
#define HWIO_WEB_PWM2_CTL0_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM2_CTL0_ADDR(x), HWIO_WEB_PWM2_CTL0_RMSK)
#define HWIO_WEB_PWM2_CTL0_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM2_CTL0_ADDR(x), m)
#define HWIO_WEB_PWM2_CTL0_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM2_CTL0_ADDR(x),v)
#define HWIO_WEB_PWM2_CTL0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM2_CTL0_ADDR(x),m,v,HWIO_WEB_PWM2_CTL0_IN(x))
#define HWIO_WEB_PWM2_CTL0_LOCKED_MODE_BMSK                          0x8
#define HWIO_WEB_PWM2_CTL0_LOCKED_MODE_SHFT                          0x3
#define HWIO_WEB_PWM2_CTL0_DIV_FACT_BMSK                             0x6
#define HWIO_WEB_PWM2_CTL0_DIV_FACT_SHFT                             0x1
#define HWIO_WEB_PWM2_CTL0_POL_BMSK                                  0x1
#define HWIO_WEB_PWM2_CTL0_POL_SHFT                                  0x0

#define HWIO_WEB_PWM2_CTL1_ADDR(x)                            ((x) + 0x00003004)
#define HWIO_WEB_PWM2_CTL1_OFFS                               (0x00003004)
#define HWIO_WEB_PWM2_CTL1_RMSK                                   0xffff
#define HWIO_WEB_PWM2_CTL1_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM2_CTL1_ADDR(x), HWIO_WEB_PWM2_CTL1_RMSK)
#define HWIO_WEB_PWM2_CTL1_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM2_CTL1_ADDR(x), m)
#define HWIO_WEB_PWM2_CTL1_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM2_CTL1_ADDR(x),v)
#define HWIO_WEB_PWM2_CTL1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM2_CTL1_ADDR(x),m,v,HWIO_WEB_PWM2_CTL1_IN(x))
#define HWIO_WEB_PWM2_CTL1_CYC_OFFSET1_BMSK                       0xffff
#define HWIO_WEB_PWM2_CTL1_CYC_OFFSET1_SHFT                          0x0

#define HWIO_WEB_PWM2_CTL2_ADDR(x)                            ((x) + 0x00003008)
#define HWIO_WEB_PWM2_CTL2_OFFS                               (0x00003008)
#define HWIO_WEB_PWM2_CTL2_RMSK                                   0xffff
#define HWIO_WEB_PWM2_CTL2_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM2_CTL2_ADDR(x), HWIO_WEB_PWM2_CTL2_RMSK)
#define HWIO_WEB_PWM2_CTL2_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM2_CTL2_ADDR(x), m)
#define HWIO_WEB_PWM2_CTL2_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM2_CTL2_ADDR(x),v)
#define HWIO_WEB_PWM2_CTL2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM2_CTL2_ADDR(x),m,v,HWIO_WEB_PWM2_CTL2_IN(x))
#define HWIO_WEB_PWM2_CTL2_CYC_OFFSET2_BMSK                       0xffff
#define HWIO_WEB_PWM2_CTL2_CYC_OFFSET2_SHFT                          0x0

#define HWIO_WEB_PWM2_CYC_CFG_ADDR(x)                         ((x) + 0x0000300c)
#define HWIO_WEB_PWM2_CYC_CFG_OFFS                            (0x0000300c)
#define HWIO_WEB_PWM2_CYC_CFG_RMSK                                0xffff
#define HWIO_WEB_PWM2_CYC_CFG_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM2_CYC_CFG_ADDR(x), HWIO_WEB_PWM2_CYC_CFG_RMSK)
#define HWIO_WEB_PWM2_CYC_CFG_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM2_CYC_CFG_ADDR(x), m)
#define HWIO_WEB_PWM2_CYC_CFG_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM2_CYC_CFG_ADDR(x),v)
#define HWIO_WEB_PWM2_CYC_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM2_CYC_CFG_ADDR(x),m,v,HWIO_WEB_PWM2_CYC_CFG_IN(x))
#define HWIO_WEB_PWM2_CYC_CFG_CYCTIME_BMSK                        0xffff
#define HWIO_WEB_PWM2_CYC_CFG_CYCTIME_SHFT                           0x0

#define HWIO_WEB_PWM2_UPDATE_ADDR(x)                          ((x) + 0x00003010)
#define HWIO_WEB_PWM2_UPDATE_OFFS                             (0x00003010)
#define HWIO_WEB_PWM2_UPDATE_RMSK                                    0x1
#define HWIO_WEB_PWM2_UPDATE_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM2_UPDATE_ADDR(x),v)
#define HWIO_WEB_PWM2_UPDATE_UPD_EN_BMSK                             0x1
#define HWIO_WEB_PWM2_UPDATE_UPD_EN_SHFT                             0x0

#define HWIO_WEB_PWM2_PERIOD_CNT_ADDR(x)                      ((x) + 0x00003014)
#define HWIO_WEB_PWM2_PERIOD_CNT_OFFS                         (0x00003014)
#define HWIO_WEB_PWM2_PERIOD_CNT_RMSK                             0xffff
#define HWIO_WEB_PWM2_PERIOD_CNT_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM2_PERIOD_CNT_ADDR(x), HWIO_WEB_PWM2_PERIOD_CNT_RMSK)
#define HWIO_WEB_PWM2_PERIOD_CNT_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM2_PERIOD_CNT_ADDR(x), m)
#define HWIO_WEB_PWM2_PERIOD_CNT_PERIOD_CNT_BMSK                  0xffff
#define HWIO_WEB_PWM2_PERIOD_CNT_PERIOD_CNT_SHFT                     0x0

#define HWIO_WEB_PWM3_CTL0_ADDR(x)                            ((x) + 0x00004000)
#define HWIO_WEB_PWM3_CTL0_OFFS                               (0x00004000)
#define HWIO_WEB_PWM3_CTL0_RMSK                                      0xf
#define HWIO_WEB_PWM3_CTL0_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM3_CTL0_ADDR(x), HWIO_WEB_PWM3_CTL0_RMSK)
#define HWIO_WEB_PWM3_CTL0_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM3_CTL0_ADDR(x), m)
#define HWIO_WEB_PWM3_CTL0_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM3_CTL0_ADDR(x),v)
#define HWIO_WEB_PWM3_CTL0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM3_CTL0_ADDR(x),m,v,HWIO_WEB_PWM3_CTL0_IN(x))
#define HWIO_WEB_PWM3_CTL0_LOCKED_MODE_BMSK                          0x8
#define HWIO_WEB_PWM3_CTL0_LOCKED_MODE_SHFT                          0x3
#define HWIO_WEB_PWM3_CTL0_DIV_FACT_BMSK                             0x6
#define HWIO_WEB_PWM3_CTL0_DIV_FACT_SHFT                             0x1
#define HWIO_WEB_PWM3_CTL0_POL_BMSK                                  0x1
#define HWIO_WEB_PWM3_CTL0_POL_SHFT                                  0x0

#define HWIO_WEB_PWM3_CTL1_ADDR(x)                            ((x) + 0x00004004)
#define HWIO_WEB_PWM3_CTL1_OFFS                               (0x00004004)
#define HWIO_WEB_PWM3_CTL1_RMSK                                   0xffff
#define HWIO_WEB_PWM3_CTL1_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM3_CTL1_ADDR(x), HWIO_WEB_PWM3_CTL1_RMSK)
#define HWIO_WEB_PWM3_CTL1_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM3_CTL1_ADDR(x), m)
#define HWIO_WEB_PWM3_CTL1_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM3_CTL1_ADDR(x),v)
#define HWIO_WEB_PWM3_CTL1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM3_CTL1_ADDR(x),m,v,HWIO_WEB_PWM3_CTL1_IN(x))
#define HWIO_WEB_PWM3_CTL1_CYC_OFFSET1_BMSK                       0xffff
#define HWIO_WEB_PWM3_CTL1_CYC_OFFSET1_SHFT                          0x0

#define HWIO_WEB_PWM3_CTL2_ADDR(x)                            ((x) + 0x00004008)
#define HWIO_WEB_PWM3_CTL2_OFFS                               (0x00004008)
#define HWIO_WEB_PWM3_CTL2_RMSK                                   0xffff
#define HWIO_WEB_PWM3_CTL2_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM3_CTL2_ADDR(x), HWIO_WEB_PWM3_CTL2_RMSK)
#define HWIO_WEB_PWM3_CTL2_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM3_CTL2_ADDR(x), m)
#define HWIO_WEB_PWM3_CTL2_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM3_CTL2_ADDR(x),v)
#define HWIO_WEB_PWM3_CTL2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM3_CTL2_ADDR(x),m,v,HWIO_WEB_PWM3_CTL2_IN(x))
#define HWIO_WEB_PWM3_CTL2_CYC_OFFSET2_BMSK                       0xffff
#define HWIO_WEB_PWM3_CTL2_CYC_OFFSET2_SHFT                          0x0

#define HWIO_WEB_PWM3_CYC_CFG_ADDR(x)                         ((x) + 0x0000400c)
#define HWIO_WEB_PWM3_CYC_CFG_OFFS                            (0x0000400c)
#define HWIO_WEB_PWM3_CYC_CFG_RMSK                                0xffff
#define HWIO_WEB_PWM3_CYC_CFG_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM3_CYC_CFG_ADDR(x), HWIO_WEB_PWM3_CYC_CFG_RMSK)
#define HWIO_WEB_PWM3_CYC_CFG_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM3_CYC_CFG_ADDR(x), m)
#define HWIO_WEB_PWM3_CYC_CFG_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM3_CYC_CFG_ADDR(x),v)
#define HWIO_WEB_PWM3_CYC_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM3_CYC_CFG_ADDR(x),m,v,HWIO_WEB_PWM3_CYC_CFG_IN(x))
#define HWIO_WEB_PWM3_CYC_CFG_CYCTIME_BMSK                        0xffff
#define HWIO_WEB_PWM3_CYC_CFG_CYCTIME_SHFT                           0x0

#define HWIO_WEB_PWM3_UPDATE_ADDR(x)                          ((x) + 0x00004010)
#define HWIO_WEB_PWM3_UPDATE_OFFS                             (0x00004010)
#define HWIO_WEB_PWM3_UPDATE_RMSK                                    0x1
#define HWIO_WEB_PWM3_UPDATE_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM3_UPDATE_ADDR(x),v)
#define HWIO_WEB_PWM3_UPDATE_UPD_EN_BMSK                             0x1
#define HWIO_WEB_PWM3_UPDATE_UPD_EN_SHFT                             0x0

#define HWIO_WEB_PWM3_PERIOD_CNT_ADDR(x)                      ((x) + 0x00004014)
#define HWIO_WEB_PWM3_PERIOD_CNT_OFFS                         (0x00004014)
#define HWIO_WEB_PWM3_PERIOD_CNT_RMSK                             0xffff
#define HWIO_WEB_PWM3_PERIOD_CNT_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM3_PERIOD_CNT_ADDR(x), HWIO_WEB_PWM3_PERIOD_CNT_RMSK)
#define HWIO_WEB_PWM3_PERIOD_CNT_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM3_PERIOD_CNT_ADDR(x), m)
#define HWIO_WEB_PWM3_PERIOD_CNT_PERIOD_CNT_BMSK                  0xffff
#define HWIO_WEB_PWM3_PERIOD_CNT_PERIOD_CNT_SHFT                     0x0

#define HWIO_WEB_PWM4_CTL0_ADDR(x)                            ((x) + 0x00005000)
#define HWIO_WEB_PWM4_CTL0_OFFS                               (0x00005000)
#define HWIO_WEB_PWM4_CTL0_RMSK                                      0xf
#define HWIO_WEB_PWM4_CTL0_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM4_CTL0_ADDR(x), HWIO_WEB_PWM4_CTL0_RMSK)
#define HWIO_WEB_PWM4_CTL0_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM4_CTL0_ADDR(x), m)
#define HWIO_WEB_PWM4_CTL0_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM4_CTL0_ADDR(x),v)
#define HWIO_WEB_PWM4_CTL0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM4_CTL0_ADDR(x),m,v,HWIO_WEB_PWM4_CTL0_IN(x))
#define HWIO_WEB_PWM4_CTL0_LOCKED_MODE_BMSK                          0x8
#define HWIO_WEB_PWM4_CTL0_LOCKED_MODE_SHFT                          0x3
#define HWIO_WEB_PWM4_CTL0_DIV_FACT_BMSK                             0x6
#define HWIO_WEB_PWM4_CTL0_DIV_FACT_SHFT                             0x1
#define HWIO_WEB_PWM4_CTL0_POL_BMSK                                  0x1
#define HWIO_WEB_PWM4_CTL0_POL_SHFT                                  0x0

#define HWIO_WEB_PWM4_CTL1_ADDR(x)                            ((x) + 0x00005004)
#define HWIO_WEB_PWM4_CTL1_OFFS                               (0x00005004)
#define HWIO_WEB_PWM4_CTL1_RMSK                                   0xffff
#define HWIO_WEB_PWM4_CTL1_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM4_CTL1_ADDR(x), HWIO_WEB_PWM4_CTL1_RMSK)
#define HWIO_WEB_PWM4_CTL1_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM4_CTL1_ADDR(x), m)
#define HWIO_WEB_PWM4_CTL1_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM4_CTL1_ADDR(x),v)
#define HWIO_WEB_PWM4_CTL1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM4_CTL1_ADDR(x),m,v,HWIO_WEB_PWM4_CTL1_IN(x))
#define HWIO_WEB_PWM4_CTL1_CYC_OFFSET1_BMSK                       0xffff
#define HWIO_WEB_PWM4_CTL1_CYC_OFFSET1_SHFT                          0x0

#define HWIO_WEB_PWM4_CTL2_ADDR(x)                            ((x) + 0x00005008)
#define HWIO_WEB_PWM4_CTL2_OFFS                               (0x00005008)
#define HWIO_WEB_PWM4_CTL2_RMSK                                   0xffff
#define HWIO_WEB_PWM4_CTL2_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM4_CTL2_ADDR(x), HWIO_WEB_PWM4_CTL2_RMSK)
#define HWIO_WEB_PWM4_CTL2_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM4_CTL2_ADDR(x), m)
#define HWIO_WEB_PWM4_CTL2_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM4_CTL2_ADDR(x),v)
#define HWIO_WEB_PWM4_CTL2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM4_CTL2_ADDR(x),m,v,HWIO_WEB_PWM4_CTL2_IN(x))
#define HWIO_WEB_PWM4_CTL2_CYC_OFFSET2_BMSK                       0xffff
#define HWIO_WEB_PWM4_CTL2_CYC_OFFSET2_SHFT                          0x0

#define HWIO_WEB_PWM4_CYC_CFG_ADDR(x)                         ((x) + 0x0000500c)
#define HWIO_WEB_PWM4_CYC_CFG_OFFS                            (0x0000500c)
#define HWIO_WEB_PWM4_CYC_CFG_RMSK                                0xffff
#define HWIO_WEB_PWM4_CYC_CFG_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM4_CYC_CFG_ADDR(x), HWIO_WEB_PWM4_CYC_CFG_RMSK)
#define HWIO_WEB_PWM4_CYC_CFG_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM4_CYC_CFG_ADDR(x), m)
#define HWIO_WEB_PWM4_CYC_CFG_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM4_CYC_CFG_ADDR(x),v)
#define HWIO_WEB_PWM4_CYC_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM4_CYC_CFG_ADDR(x),m,v,HWIO_WEB_PWM4_CYC_CFG_IN(x))
#define HWIO_WEB_PWM4_CYC_CFG_CYCTIME_BMSK                        0xffff
#define HWIO_WEB_PWM4_CYC_CFG_CYCTIME_SHFT                           0x0

#define HWIO_WEB_PWM4_UPDATE_ADDR(x)                          ((x) + 0x00005010)
#define HWIO_WEB_PWM4_UPDATE_OFFS                             (0x00005010)
#define HWIO_WEB_PWM4_UPDATE_RMSK                                    0x1
#define HWIO_WEB_PWM4_UPDATE_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM4_UPDATE_ADDR(x),v)
#define HWIO_WEB_PWM4_UPDATE_UPD_EN_BMSK                             0x1
#define HWIO_WEB_PWM4_UPDATE_UPD_EN_SHFT                             0x0

#define HWIO_WEB_PWM4_PERIOD_CNT_ADDR(x)                      ((x) + 0x00005014)
#define HWIO_WEB_PWM4_PERIOD_CNT_OFFS                         (0x00005014)
#define HWIO_WEB_PWM4_PERIOD_CNT_RMSK                             0xffff
#define HWIO_WEB_PWM4_PERIOD_CNT_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM4_PERIOD_CNT_ADDR(x), HWIO_WEB_PWM4_PERIOD_CNT_RMSK)
#define HWIO_WEB_PWM4_PERIOD_CNT_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM4_PERIOD_CNT_ADDR(x), m)
#define HWIO_WEB_PWM4_PERIOD_CNT_PERIOD_CNT_BMSK                  0xffff
#define HWIO_WEB_PWM4_PERIOD_CNT_PERIOD_CNT_SHFT                     0x0

#define HWIO_WEB_PWM5_CTL0_ADDR(x)                            ((x) + 0x00006000)
#define HWIO_WEB_PWM5_CTL0_OFFS                               (0x00006000)
#define HWIO_WEB_PWM5_CTL0_RMSK                                      0xf
#define HWIO_WEB_PWM5_CTL0_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM5_CTL0_ADDR(x), HWIO_WEB_PWM5_CTL0_RMSK)
#define HWIO_WEB_PWM5_CTL0_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM5_CTL0_ADDR(x), m)
#define HWIO_WEB_PWM5_CTL0_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM5_CTL0_ADDR(x),v)
#define HWIO_WEB_PWM5_CTL0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM5_CTL0_ADDR(x),m,v,HWIO_WEB_PWM5_CTL0_IN(x))
#define HWIO_WEB_PWM5_CTL0_LOCKED_MODE_BMSK                          0x8
#define HWIO_WEB_PWM5_CTL0_LOCKED_MODE_SHFT                          0x3
#define HWIO_WEB_PWM5_CTL0_DIV_FACT_BMSK                             0x6
#define HWIO_WEB_PWM5_CTL0_DIV_FACT_SHFT                             0x1
#define HWIO_WEB_PWM5_CTL0_POL_BMSK                                  0x1
#define HWIO_WEB_PWM5_CTL0_POL_SHFT                                  0x0

#define HWIO_WEB_PWM5_CTL1_ADDR(x)                            ((x) + 0x00006004)
#define HWIO_WEB_PWM5_CTL1_OFFS                               (0x00006004)
#define HWIO_WEB_PWM5_CTL1_RMSK                                   0xffff
#define HWIO_WEB_PWM5_CTL1_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM5_CTL1_ADDR(x), HWIO_WEB_PWM5_CTL1_RMSK)
#define HWIO_WEB_PWM5_CTL1_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM5_CTL1_ADDR(x), m)
#define HWIO_WEB_PWM5_CTL1_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM5_CTL1_ADDR(x),v)
#define HWIO_WEB_PWM5_CTL1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM5_CTL1_ADDR(x),m,v,HWIO_WEB_PWM5_CTL1_IN(x))
#define HWIO_WEB_PWM5_CTL1_CYC_OFFSET1_BMSK                       0xffff
#define HWIO_WEB_PWM5_CTL1_CYC_OFFSET1_SHFT                          0x0

#define HWIO_WEB_PWM5_CTL2_ADDR(x)                            ((x) + 0x00006008)
#define HWIO_WEB_PWM5_CTL2_OFFS                               (0x00006008)
#define HWIO_WEB_PWM5_CTL2_RMSK                                   0xffff
#define HWIO_WEB_PWM5_CTL2_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM5_CTL2_ADDR(x), HWIO_WEB_PWM5_CTL2_RMSK)
#define HWIO_WEB_PWM5_CTL2_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM5_CTL2_ADDR(x), m)
#define HWIO_WEB_PWM5_CTL2_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM5_CTL2_ADDR(x),v)
#define HWIO_WEB_PWM5_CTL2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM5_CTL2_ADDR(x),m,v,HWIO_WEB_PWM5_CTL2_IN(x))
#define HWIO_WEB_PWM5_CTL2_CYC_OFFSET2_BMSK                       0xffff
#define HWIO_WEB_PWM5_CTL2_CYC_OFFSET2_SHFT                          0x0

#define HWIO_WEB_PWM5_CYC_CFG_ADDR(x)                         ((x) + 0x0000600c)
#define HWIO_WEB_PWM5_CYC_CFG_OFFS                            (0x0000600c)
#define HWIO_WEB_PWM5_CYC_CFG_RMSK                                0xffff
#define HWIO_WEB_PWM5_CYC_CFG_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM5_CYC_CFG_ADDR(x), HWIO_WEB_PWM5_CYC_CFG_RMSK)
#define HWIO_WEB_PWM5_CYC_CFG_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM5_CYC_CFG_ADDR(x), m)
#define HWIO_WEB_PWM5_CYC_CFG_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM5_CYC_CFG_ADDR(x),v)
#define HWIO_WEB_PWM5_CYC_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM5_CYC_CFG_ADDR(x),m,v,HWIO_WEB_PWM5_CYC_CFG_IN(x))
#define HWIO_WEB_PWM5_CYC_CFG_CYCTIME_BMSK                        0xffff
#define HWIO_WEB_PWM5_CYC_CFG_CYCTIME_SHFT                           0x0

#define HWIO_WEB_PWM5_UPDATE_ADDR(x)                          ((x) + 0x00006010)
#define HWIO_WEB_PWM5_UPDATE_OFFS                             (0x00006010)
#define HWIO_WEB_PWM5_UPDATE_RMSK                                    0x1
#define HWIO_WEB_PWM5_UPDATE_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM5_UPDATE_ADDR(x),v)
#define HWIO_WEB_PWM5_UPDATE_UPD_EN_BMSK                             0x1
#define HWIO_WEB_PWM5_UPDATE_UPD_EN_SHFT                             0x0

#define HWIO_WEB_PWM5_PERIOD_CNT_ADDR(x)                      ((x) + 0x00006014)
#define HWIO_WEB_PWM5_PERIOD_CNT_OFFS                         (0x00006014)
#define HWIO_WEB_PWM5_PERIOD_CNT_RMSK                             0xffff
#define HWIO_WEB_PWM5_PERIOD_CNT_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM5_PERIOD_CNT_ADDR(x), HWIO_WEB_PWM5_PERIOD_CNT_RMSK)
#define HWIO_WEB_PWM5_PERIOD_CNT_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM5_PERIOD_CNT_ADDR(x), m)
#define HWIO_WEB_PWM5_PERIOD_CNT_PERIOD_CNT_BMSK                  0xffff
#define HWIO_WEB_PWM5_PERIOD_CNT_PERIOD_CNT_SHFT                     0x0

#define HWIO_WEB_PWM6_CTL0_ADDR(x)                            ((x) + 0x00007000)
#define HWIO_WEB_PWM6_CTL0_OFFS                               (0x00007000)
#define HWIO_WEB_PWM6_CTL0_RMSK                                      0xf
#define HWIO_WEB_PWM6_CTL0_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM6_CTL0_ADDR(x), HWIO_WEB_PWM6_CTL0_RMSK)
#define HWIO_WEB_PWM6_CTL0_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM6_CTL0_ADDR(x), m)
#define HWIO_WEB_PWM6_CTL0_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM6_CTL0_ADDR(x),v)
#define HWIO_WEB_PWM6_CTL0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM6_CTL0_ADDR(x),m,v,HWIO_WEB_PWM6_CTL0_IN(x))
#define HWIO_WEB_PWM6_CTL0_LOCKED_MODE_BMSK                          0x8
#define HWIO_WEB_PWM6_CTL0_LOCKED_MODE_SHFT                          0x3
#define HWIO_WEB_PWM6_CTL0_DIV_FACT_BMSK                             0x6
#define HWIO_WEB_PWM6_CTL0_DIV_FACT_SHFT                             0x1
#define HWIO_WEB_PWM6_CTL0_POL_BMSK                                  0x1
#define HWIO_WEB_PWM6_CTL0_POL_SHFT                                  0x0

#define HWIO_WEB_PWM6_CTL1_ADDR(x)                            ((x) + 0x00007004)
#define HWIO_WEB_PWM6_CTL1_OFFS                               (0x00007004)
#define HWIO_WEB_PWM6_CTL1_RMSK                                   0xffff
#define HWIO_WEB_PWM6_CTL1_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM6_CTL1_ADDR(x), HWIO_WEB_PWM6_CTL1_RMSK)
#define HWIO_WEB_PWM6_CTL1_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM6_CTL1_ADDR(x), m)
#define HWIO_WEB_PWM6_CTL1_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM6_CTL1_ADDR(x),v)
#define HWIO_WEB_PWM6_CTL1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM6_CTL1_ADDR(x),m,v,HWIO_WEB_PWM6_CTL1_IN(x))
#define HWIO_WEB_PWM6_CTL1_CYC_OFFSET1_BMSK                       0xffff
#define HWIO_WEB_PWM6_CTL1_CYC_OFFSET1_SHFT                          0x0

#define HWIO_WEB_PWM6_CTL2_ADDR(x)                            ((x) + 0x00007008)
#define HWIO_WEB_PWM6_CTL2_OFFS                               (0x00007008)
#define HWIO_WEB_PWM6_CTL2_RMSK                                   0xffff
#define HWIO_WEB_PWM6_CTL2_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM6_CTL2_ADDR(x), HWIO_WEB_PWM6_CTL2_RMSK)
#define HWIO_WEB_PWM6_CTL2_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM6_CTL2_ADDR(x), m)
#define HWIO_WEB_PWM6_CTL2_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM6_CTL2_ADDR(x),v)
#define HWIO_WEB_PWM6_CTL2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM6_CTL2_ADDR(x),m,v,HWIO_WEB_PWM6_CTL2_IN(x))
#define HWIO_WEB_PWM6_CTL2_CYC_OFFSET2_BMSK                       0xffff
#define HWIO_WEB_PWM6_CTL2_CYC_OFFSET2_SHFT                          0x0

#define HWIO_WEB_PWM6_CYC_CFG_ADDR(x)                         ((x) + 0x0000700c)
#define HWIO_WEB_PWM6_CYC_CFG_OFFS                            (0x0000700c)
#define HWIO_WEB_PWM6_CYC_CFG_RMSK                                0xffff
#define HWIO_WEB_PWM6_CYC_CFG_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM6_CYC_CFG_ADDR(x), HWIO_WEB_PWM6_CYC_CFG_RMSK)
#define HWIO_WEB_PWM6_CYC_CFG_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM6_CYC_CFG_ADDR(x), m)
#define HWIO_WEB_PWM6_CYC_CFG_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM6_CYC_CFG_ADDR(x),v)
#define HWIO_WEB_PWM6_CYC_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM6_CYC_CFG_ADDR(x),m,v,HWIO_WEB_PWM6_CYC_CFG_IN(x))
#define HWIO_WEB_PWM6_CYC_CFG_CYCTIME_BMSK                        0xffff
#define HWIO_WEB_PWM6_CYC_CFG_CYCTIME_SHFT                           0x0

#define HWIO_WEB_PWM6_UPDATE_ADDR(x)                          ((x) + 0x00007010)
#define HWIO_WEB_PWM6_UPDATE_OFFS                             (0x00007010)
#define HWIO_WEB_PWM6_UPDATE_RMSK                                    0x1
#define HWIO_WEB_PWM6_UPDATE_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM6_UPDATE_ADDR(x),v)
#define HWIO_WEB_PWM6_UPDATE_UPD_EN_BMSK                             0x1
#define HWIO_WEB_PWM6_UPDATE_UPD_EN_SHFT                             0x0

#define HWIO_WEB_PWM6_PERIOD_CNT_ADDR(x)                      ((x) + 0x00007014)
#define HWIO_WEB_PWM6_PERIOD_CNT_OFFS                         (0x00007014)
#define HWIO_WEB_PWM6_PERIOD_CNT_RMSK                             0xffff
#define HWIO_WEB_PWM6_PERIOD_CNT_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM6_PERIOD_CNT_ADDR(x), HWIO_WEB_PWM6_PERIOD_CNT_RMSK)
#define HWIO_WEB_PWM6_PERIOD_CNT_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM6_PERIOD_CNT_ADDR(x), m)
#define HWIO_WEB_PWM6_PERIOD_CNT_PERIOD_CNT_BMSK                  0xffff
#define HWIO_WEB_PWM6_PERIOD_CNT_PERIOD_CNT_SHFT                     0x0

#define HWIO_WEB_PWM7_CTL0_ADDR(x)                            ((x) + 0x00008000)
#define HWIO_WEB_PWM7_CTL0_OFFS                               (0x00008000)
#define HWIO_WEB_PWM7_CTL0_RMSK                                      0xf
#define HWIO_WEB_PWM7_CTL0_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM7_CTL0_ADDR(x), HWIO_WEB_PWM7_CTL0_RMSK)
#define HWIO_WEB_PWM7_CTL0_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM7_CTL0_ADDR(x), m)
#define HWIO_WEB_PWM7_CTL0_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM7_CTL0_ADDR(x),v)
#define HWIO_WEB_PWM7_CTL0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM7_CTL0_ADDR(x),m,v,HWIO_WEB_PWM7_CTL0_IN(x))
#define HWIO_WEB_PWM7_CTL0_LOCKED_MODE_BMSK                          0x8
#define HWIO_WEB_PWM7_CTL0_LOCKED_MODE_SHFT                          0x3
#define HWIO_WEB_PWM7_CTL0_DIV_FACT_BMSK                             0x6
#define HWIO_WEB_PWM7_CTL0_DIV_FACT_SHFT                             0x1
#define HWIO_WEB_PWM7_CTL0_POL_BMSK                                  0x1
#define HWIO_WEB_PWM7_CTL0_POL_SHFT                                  0x0

#define HWIO_WEB_PWM7_CTL1_ADDR(x)                            ((x) + 0x00008004)
#define HWIO_WEB_PWM7_CTL1_OFFS                               (0x00008004)
#define HWIO_WEB_PWM7_CTL1_RMSK                                   0xffff
#define HWIO_WEB_PWM7_CTL1_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM7_CTL1_ADDR(x), HWIO_WEB_PWM7_CTL1_RMSK)
#define HWIO_WEB_PWM7_CTL1_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM7_CTL1_ADDR(x), m)
#define HWIO_WEB_PWM7_CTL1_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM7_CTL1_ADDR(x),v)
#define HWIO_WEB_PWM7_CTL1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM7_CTL1_ADDR(x),m,v,HWIO_WEB_PWM7_CTL1_IN(x))
#define HWIO_WEB_PWM7_CTL1_CYC_OFFSET1_BMSK                       0xffff
#define HWIO_WEB_PWM7_CTL1_CYC_OFFSET1_SHFT                          0x0

#define HWIO_WEB_PWM7_CTL2_ADDR(x)                            ((x) + 0x00008008)
#define HWIO_WEB_PWM7_CTL2_OFFS                               (0x00008008)
#define HWIO_WEB_PWM7_CTL2_RMSK                                   0xffff
#define HWIO_WEB_PWM7_CTL2_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM7_CTL2_ADDR(x), HWIO_WEB_PWM7_CTL2_RMSK)
#define HWIO_WEB_PWM7_CTL2_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM7_CTL2_ADDR(x), m)
#define HWIO_WEB_PWM7_CTL2_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM7_CTL2_ADDR(x),v)
#define HWIO_WEB_PWM7_CTL2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM7_CTL2_ADDR(x),m,v,HWIO_WEB_PWM7_CTL2_IN(x))
#define HWIO_WEB_PWM7_CTL2_CYC_OFFSET2_BMSK                       0xffff
#define HWIO_WEB_PWM7_CTL2_CYC_OFFSET2_SHFT                          0x0

#define HWIO_WEB_PWM7_CYC_CFG_ADDR(x)                         ((x) + 0x0000800c)
#define HWIO_WEB_PWM7_CYC_CFG_OFFS                            (0x0000800c)
#define HWIO_WEB_PWM7_CYC_CFG_RMSK                                0xffff
#define HWIO_WEB_PWM7_CYC_CFG_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM7_CYC_CFG_ADDR(x), HWIO_WEB_PWM7_CYC_CFG_RMSK)
#define HWIO_WEB_PWM7_CYC_CFG_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM7_CYC_CFG_ADDR(x), m)
#define HWIO_WEB_PWM7_CYC_CFG_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM7_CYC_CFG_ADDR(x),v)
#define HWIO_WEB_PWM7_CYC_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM7_CYC_CFG_ADDR(x),m,v,HWIO_WEB_PWM7_CYC_CFG_IN(x))
#define HWIO_WEB_PWM7_CYC_CFG_CYCTIME_BMSK                        0xffff
#define HWIO_WEB_PWM7_CYC_CFG_CYCTIME_SHFT                           0x0

#define HWIO_WEB_PWM7_UPDATE_ADDR(x)                          ((x) + 0x00008010)
#define HWIO_WEB_PWM7_UPDATE_OFFS                             (0x00008010)
#define HWIO_WEB_PWM7_UPDATE_RMSK                                    0x1
#define HWIO_WEB_PWM7_UPDATE_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM7_UPDATE_ADDR(x),v)
#define HWIO_WEB_PWM7_UPDATE_UPD_EN_BMSK                             0x1
#define HWIO_WEB_PWM7_UPDATE_UPD_EN_SHFT                             0x0

#define HWIO_WEB_PWM7_PERIOD_CNT_ADDR(x)                      ((x) + 0x00008014)
#define HWIO_WEB_PWM7_PERIOD_CNT_OFFS                         (0x00008014)
#define HWIO_WEB_PWM7_PERIOD_CNT_RMSK                             0xffff
#define HWIO_WEB_PWM7_PERIOD_CNT_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM7_PERIOD_CNT_ADDR(x), HWIO_WEB_PWM7_PERIOD_CNT_RMSK)
#define HWIO_WEB_PWM7_PERIOD_CNT_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM7_PERIOD_CNT_ADDR(x), m)
#define HWIO_WEB_PWM7_PERIOD_CNT_PERIOD_CNT_BMSK                  0xffff
#define HWIO_WEB_PWM7_PERIOD_CNT_PERIOD_CNT_SHFT                     0x0

#define HWIO_WEB_PWM8_CTL0_ADDR(x)                            ((x) + 0x00009000)
#define HWIO_WEB_PWM8_CTL0_OFFS                               (0x00009000)
#define HWIO_WEB_PWM8_CTL0_RMSK                                      0xf
#define HWIO_WEB_PWM8_CTL0_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM8_CTL0_ADDR(x), HWIO_WEB_PWM8_CTL0_RMSK)
#define HWIO_WEB_PWM8_CTL0_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM8_CTL0_ADDR(x), m)
#define HWIO_WEB_PWM8_CTL0_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM8_CTL0_ADDR(x),v)
#define HWIO_WEB_PWM8_CTL0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM8_CTL0_ADDR(x),m,v,HWIO_WEB_PWM8_CTL0_IN(x))
#define HWIO_WEB_PWM8_CTL0_LOCKED_MODE_BMSK                          0x8
#define HWIO_WEB_PWM8_CTL0_LOCKED_MODE_SHFT                          0x3
#define HWIO_WEB_PWM8_CTL0_DIV_FACT_BMSK                             0x6
#define HWIO_WEB_PWM8_CTL0_DIV_FACT_SHFT                             0x1
#define HWIO_WEB_PWM8_CTL0_POL_BMSK                                  0x1
#define HWIO_WEB_PWM8_CTL0_POL_SHFT                                  0x0

#define HWIO_WEB_PWM8_CTL1_ADDR(x)                            ((x) + 0x00009004)
#define HWIO_WEB_PWM8_CTL1_OFFS                               (0x00009004)
#define HWIO_WEB_PWM8_CTL1_RMSK                                   0xffff
#define HWIO_WEB_PWM8_CTL1_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM8_CTL1_ADDR(x), HWIO_WEB_PWM8_CTL1_RMSK)
#define HWIO_WEB_PWM8_CTL1_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM8_CTL1_ADDR(x), m)
#define HWIO_WEB_PWM8_CTL1_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM8_CTL1_ADDR(x),v)
#define HWIO_WEB_PWM8_CTL1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM8_CTL1_ADDR(x),m,v,HWIO_WEB_PWM8_CTL1_IN(x))
#define HWIO_WEB_PWM8_CTL1_CYC_OFFSET1_BMSK                       0xffff
#define HWIO_WEB_PWM8_CTL1_CYC_OFFSET1_SHFT                          0x0

#define HWIO_WEB_PWM8_CTL2_ADDR(x)                            ((x) + 0x00009008)
#define HWIO_WEB_PWM8_CTL2_OFFS                               (0x00009008)
#define HWIO_WEB_PWM8_CTL2_RMSK                                   0xffff
#define HWIO_WEB_PWM8_CTL2_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM8_CTL2_ADDR(x), HWIO_WEB_PWM8_CTL2_RMSK)
#define HWIO_WEB_PWM8_CTL2_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM8_CTL2_ADDR(x), m)
#define HWIO_WEB_PWM8_CTL2_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM8_CTL2_ADDR(x),v)
#define HWIO_WEB_PWM8_CTL2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM8_CTL2_ADDR(x),m,v,HWIO_WEB_PWM8_CTL2_IN(x))
#define HWIO_WEB_PWM8_CTL2_CYC_OFFSET2_BMSK                       0xffff
#define HWIO_WEB_PWM8_CTL2_CYC_OFFSET2_SHFT                          0x0

#define HWIO_WEB_PWM8_CYC_CFG_ADDR(x)                         ((x) + 0x0000900c)
#define HWIO_WEB_PWM8_CYC_CFG_OFFS                            (0x0000900c)
#define HWIO_WEB_PWM8_CYC_CFG_RMSK                                0xffff
#define HWIO_WEB_PWM8_CYC_CFG_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM8_CYC_CFG_ADDR(x), HWIO_WEB_PWM8_CYC_CFG_RMSK)
#define HWIO_WEB_PWM8_CYC_CFG_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM8_CYC_CFG_ADDR(x), m)
#define HWIO_WEB_PWM8_CYC_CFG_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM8_CYC_CFG_ADDR(x),v)
#define HWIO_WEB_PWM8_CYC_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM8_CYC_CFG_ADDR(x),m,v,HWIO_WEB_PWM8_CYC_CFG_IN(x))
#define HWIO_WEB_PWM8_CYC_CFG_CYCTIME_BMSK                        0xffff
#define HWIO_WEB_PWM8_CYC_CFG_CYCTIME_SHFT                           0x0

#define HWIO_WEB_PWM8_UPDATE_ADDR(x)                          ((x) + 0x00009010)
#define HWIO_WEB_PWM8_UPDATE_OFFS                             (0x00009010)
#define HWIO_WEB_PWM8_UPDATE_RMSK                                    0x1
#define HWIO_WEB_PWM8_UPDATE_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM8_UPDATE_ADDR(x),v)
#define HWIO_WEB_PWM8_UPDATE_UPD_EN_BMSK                             0x1
#define HWIO_WEB_PWM8_UPDATE_UPD_EN_SHFT                             0x0

#define HWIO_WEB_PWM8_PERIOD_CNT_ADDR(x)                      ((x) + 0x00009014)
#define HWIO_WEB_PWM8_PERIOD_CNT_OFFS                         (0x00009014)
#define HWIO_WEB_PWM8_PERIOD_CNT_RMSK                             0xffff
#define HWIO_WEB_PWM8_PERIOD_CNT_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM8_PERIOD_CNT_ADDR(x), HWIO_WEB_PWM8_PERIOD_CNT_RMSK)
#define HWIO_WEB_PWM8_PERIOD_CNT_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM8_PERIOD_CNT_ADDR(x), m)
#define HWIO_WEB_PWM8_PERIOD_CNT_PERIOD_CNT_BMSK                  0xffff
#define HWIO_WEB_PWM8_PERIOD_CNT_PERIOD_CNT_SHFT                     0x0

#define HWIO_WEB_PWM9_CTL0_ADDR(x)                            ((x) + 0x0000a000)
#define HWIO_WEB_PWM9_CTL0_OFFS                               (0x0000a000)
#define HWIO_WEB_PWM9_CTL0_RMSK                                      0xf
#define HWIO_WEB_PWM9_CTL0_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM9_CTL0_ADDR(x), HWIO_WEB_PWM9_CTL0_RMSK)
#define HWIO_WEB_PWM9_CTL0_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM9_CTL0_ADDR(x), m)
#define HWIO_WEB_PWM9_CTL0_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM9_CTL0_ADDR(x),v)
#define HWIO_WEB_PWM9_CTL0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM9_CTL0_ADDR(x),m,v,HWIO_WEB_PWM9_CTL0_IN(x))
#define HWIO_WEB_PWM9_CTL0_LOCKED_MODE_BMSK                          0x8
#define HWIO_WEB_PWM9_CTL0_LOCKED_MODE_SHFT                          0x3
#define HWIO_WEB_PWM9_CTL0_DIV_FACT_BMSK                             0x6
#define HWIO_WEB_PWM9_CTL0_DIV_FACT_SHFT                             0x1
#define HWIO_WEB_PWM9_CTL0_POL_BMSK                                  0x1
#define HWIO_WEB_PWM9_CTL0_POL_SHFT                                  0x0

#define HWIO_WEB_PWM9_CTL1_ADDR(x)                            ((x) + 0x0000a004)
#define HWIO_WEB_PWM9_CTL1_OFFS                               (0x0000a004)
#define HWIO_WEB_PWM9_CTL1_RMSK                                   0xffff
#define HWIO_WEB_PWM9_CTL1_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM9_CTL1_ADDR(x), HWIO_WEB_PWM9_CTL1_RMSK)
#define HWIO_WEB_PWM9_CTL1_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM9_CTL1_ADDR(x), m)
#define HWIO_WEB_PWM9_CTL1_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM9_CTL1_ADDR(x),v)
#define HWIO_WEB_PWM9_CTL1_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM9_CTL1_ADDR(x),m,v,HWIO_WEB_PWM9_CTL1_IN(x))
#define HWIO_WEB_PWM9_CTL1_CYC_OFFSET1_BMSK                       0xffff
#define HWIO_WEB_PWM9_CTL1_CYC_OFFSET1_SHFT                          0x0

#define HWIO_WEB_PWM9_CTL2_ADDR(x)                            ((x) + 0x0000a008)
#define HWIO_WEB_PWM9_CTL2_OFFS                               (0x0000a008)
#define HWIO_WEB_PWM9_CTL2_RMSK                                   0xffff
#define HWIO_WEB_PWM9_CTL2_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM9_CTL2_ADDR(x), HWIO_WEB_PWM9_CTL2_RMSK)
#define HWIO_WEB_PWM9_CTL2_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM9_CTL2_ADDR(x), m)
#define HWIO_WEB_PWM9_CTL2_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM9_CTL2_ADDR(x),v)
#define HWIO_WEB_PWM9_CTL2_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM9_CTL2_ADDR(x),m,v,HWIO_WEB_PWM9_CTL2_IN(x))
#define HWIO_WEB_PWM9_CTL2_CYC_OFFSET2_BMSK                       0xffff
#define HWIO_WEB_PWM9_CTL2_CYC_OFFSET2_SHFT                          0x0

#define HWIO_WEB_PWM9_CYC_CFG_ADDR(x)                         ((x) + 0x0000a00c)
#define HWIO_WEB_PWM9_CYC_CFG_OFFS                            (0x0000a00c)
#define HWIO_WEB_PWM9_CYC_CFG_RMSK                                0xffff
#define HWIO_WEB_PWM9_CYC_CFG_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM9_CYC_CFG_ADDR(x), HWIO_WEB_PWM9_CYC_CFG_RMSK)
#define HWIO_WEB_PWM9_CYC_CFG_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM9_CYC_CFG_ADDR(x), m)
#define HWIO_WEB_PWM9_CYC_CFG_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM9_CYC_CFG_ADDR(x),v)
#define HWIO_WEB_PWM9_CYC_CFG_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_WEB_PWM9_CYC_CFG_ADDR(x),m,v,HWIO_WEB_PWM9_CYC_CFG_IN(x))
#define HWIO_WEB_PWM9_CYC_CFG_CYCTIME_BMSK                        0xffff
#define HWIO_WEB_PWM9_CYC_CFG_CYCTIME_SHFT                           0x0

#define HWIO_WEB_PWM9_UPDATE_ADDR(x)                          ((x) + 0x0000a010)
#define HWIO_WEB_PWM9_UPDATE_OFFS                             (0x0000a010)
#define HWIO_WEB_PWM9_UPDATE_RMSK                                    0x1
#define HWIO_WEB_PWM9_UPDATE_OUT(x, v)      \
        out_dword(HWIO_WEB_PWM9_UPDATE_ADDR(x),v)
#define HWIO_WEB_PWM9_UPDATE_UPD_EN_BMSK                             0x1
#define HWIO_WEB_PWM9_UPDATE_UPD_EN_SHFT                             0x0

#define HWIO_WEB_PWM9_PERIOD_CNT_ADDR(x)                      ((x) + 0x0000a014)
#define HWIO_WEB_PWM9_PERIOD_CNT_OFFS                         (0x0000a014)
#define HWIO_WEB_PWM9_PERIOD_CNT_RMSK                             0xffff
#define HWIO_WEB_PWM9_PERIOD_CNT_IN(x)      \
        in_dword_masked(HWIO_WEB_PWM9_PERIOD_CNT_ADDR(x), HWIO_WEB_PWM9_PERIOD_CNT_RMSK)
#define HWIO_WEB_PWM9_PERIOD_CNT_INM(x, m)      \
        in_dword_masked(HWIO_WEB_PWM9_PERIOD_CNT_ADDR(x), m)
#define HWIO_WEB_PWM9_PERIOD_CNT_PERIOD_CNT_BMSK                  0xffff
#define HWIO_WEB_PWM9_PERIOD_CNT_PERIOD_CNT_SHFT                     0x0


#endif /* __HALPWMHWIO_H__ */
