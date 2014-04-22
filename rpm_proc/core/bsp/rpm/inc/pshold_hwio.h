#ifndef __PSHOLD_HWIO_H__
#define __PSHOLD_HWIO_H__
/*
===========================================================================
*/
/**
  @file pshold_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    MDM9205 (Twizy) [twizy_v1.0_p3q3r35_MTO]
 
  This file contains HWIO register definitions for the following modules:
    MPM2_PSHOLD


  Generation parameters: 
  { u'filename': u'pshold_hwio.h',
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [u'MPM2_PSHOLD'],
    u'output-fvals': True,
    u'output-offsets': True}
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

  $Header: //components/rel/rpm.bf/2.1.3/core/bsp/rpm/inc/pshold_hwio.h#1 $
  $DateTime: 2019/02/13 05:01:05 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: MPM2_PSHOLD
 *--------------------------------------------------------------------------*/

#define MPM2_PSHOLD_REG_BASE                                                    (MPM2_MPM_BASE      + 0x0000b000)
#define MPM2_PSHOLD_REG_BASE_SIZE                                               0x1000
#define MPM2_PSHOLD_REG_BASE_USED                                               0x24
#define MPM2_PSHOLD_REG_BASE_OFFS                                               0x0000b000

#define HWIO_MPM2_MPM_PS_HOLD_ADDR                                              (MPM2_PSHOLD_REG_BASE      + 0x00000000)
#define HWIO_MPM2_MPM_PS_HOLD_OFFS                                              (MPM2_PSHOLD_REG_BASE_OFFS + 0x00000000)
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
#define HWIO_MPM2_MPM_DDR_PHY_FREEZEIO_EBI1_OFFS                                (MPM2_PSHOLD_REG_BASE_OFFS + 0x00000004)
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
#define HWIO_MPM2_PONOFF_CLAMP_MEM_STATUS_ADDR_OFFS                             (MPM2_PSHOLD_REG_BASE_OFFS + 0x00000020)
#define HWIO_MPM2_PONOFF_CLAMP_MEM_STATUS_ADDR_RMSK                                    0x1
#define HWIO_MPM2_PONOFF_CLAMP_MEM_STATUS_ADDR_IN          \
        in_dword(HWIO_MPM2_PONOFF_CLAMP_MEM_STATUS_ADDR_ADDR)
#define HWIO_MPM2_PONOFF_CLAMP_MEM_STATUS_ADDR_INM(m)      \
        in_dword_masked(HWIO_MPM2_PONOFF_CLAMP_MEM_STATUS_ADDR_ADDR, m)
#define HWIO_MPM2_PONOFF_CLAMP_MEM_STATUS_ADDR_STATUS_BMSK                             0x1
#define HWIO_MPM2_PONOFF_CLAMP_MEM_STATUS_ADDR_STATUS_SHFT                             0x0

#define HWIO_MPM2_CLAMP_CTRL_PONOFF_PHY_ADDR                                    (MPM2_PSHOLD_REG_BASE      + 0x00000024)
#define HWIO_MPM2_CLAMP_CTRL_PONOFF_PHY_OFFS                                    (MPM2_PSHOLD_REG_BASE_OFFS + 0x00000024)
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


#endif /* __PSHOLD_HWIO_H__ */
