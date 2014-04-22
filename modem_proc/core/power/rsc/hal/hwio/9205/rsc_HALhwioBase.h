#ifndef __RSC_HALHWIOBASE_H__
#define __RSC_HALHWIOBASE_H__
/*
===========================================================================
*/
/**
  @file rsc_HALhwioBase.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    MDM9205 (Twizy) [twizy_v1.0_p3q1r22.1]
 
  This file contains HWIO register definitions for the following modules:
    MSS_QDSP6SS_RSCC_RSCC_RSC


  Generation parameters: 
  { u'explicit-addressing': True,
    u'filename': u'rsc_HALhwioBase.h',
    u'header': u'msmhwiobase.h',
    u'ignore-prefixes': True,
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [u'MSS_QDSP6SS_RSCC_RSCC_RSC'],
    u'rmsk-input': True}
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

  $Header: //components/rel/core.mpss/3.10/power/rsc/hal/hwio/9205/rsc_HALhwioBase.h#1 $
  $DateTime: 2018/12/28 00:36:13 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: RSCC_RSC
 *--------------------------------------------------------------------------*/

#define RSCC_RSC_REG_BASE                                                                                               (MSS_TOP_BASE      + 0x00130000)
#define RSCC_RSC_REG_BASE_SIZE                                                                                          0x10000
#define RSCC_RSC_REG_BASE_USED                                                                                          0x6bc

#define HWIO_RSC_ID_DRVd_ADDR(base,d)                                                                                   ((base) + 0x00000000 + 0x10000 * (d))
#define HWIO_RSC_ID_DRVd_RMSK                                                                                             0xffffff
#define HWIO_RSC_ID_DRVd_MAXd                                                                                                    0
#define HWIO_RSC_ID_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_RSC_ID_DRVd_ADDR(base,d), HWIO_RSC_ID_DRVd_RMSK)
#define HWIO_RSC_ID_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_RSC_ID_DRVd_ADDR(base,d), mask)
#define HWIO_RSC_ID_DRVd_MAJOR_VER_BMSK                                                                                   0xff0000
#define HWIO_RSC_ID_DRVd_MAJOR_VER_SHFT                                                                                       0x10
#define HWIO_RSC_ID_DRVd_MINOR_VER_BMSK                                                                                     0xff00
#define HWIO_RSC_ID_DRVd_MINOR_VER_SHFT                                                                                        0x8
#define HWIO_RSC_ID_DRVd_STEP_VER_BMSK                                                                                        0xff
#define HWIO_RSC_ID_DRVd_STEP_VER_SHFT                                                                                         0x0

#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_ADDR(base,d)                                                                  ((base) + 0x00000004 + 0x10000 * (d))
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_RMSK                                                                           0x71f1f1f
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_MAXd                                                                                   0
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_ADDR(base,d), HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_RMSK)
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_ADDR(base,d), mask)
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_SEQ_EXTENDED_BMSK                                                              0x4000000
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_SEQ_EXTENDED_SHFT                                                                   0x1a
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_IS_CHILD_RSC_BMSK                                                              0x2000000
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_IS_CHILD_RSC_SHFT                                                                   0x19
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_HW_SOLVER_BMSK                                                                 0x1000000
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_HW_SOLVER_SHFT                                                                      0x18
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_SOLVER_SLOTS_BMSK                                                               0x1f0000
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_SOLVER_SLOTS_SHFT                                                                   0x10
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_SOLVER_MODES_BMSK                                                                 0x1f00
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_SOLVER_MODES_SHFT                                                                    0x8
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_NUM_TIMERS_BMSK                                                                     0x1f
#define HWIO_RSC_PARAM_SOLVER_CONFIG_DRVd_NUM_TIMERS_SHFT                                                                      0x0

#define HWIO_RSC_PARAM_RSC_CONFIG_DRVd_ADDR(base,d)                                                                     ((base) + 0x00000008 + 0x10000 * (d))
#define HWIO_RSC_PARAM_RSC_CONFIG_DRVd_RMSK                                                                              0x7ff0f1f
#define HWIO_RSC_PARAM_RSC_CONFIG_DRVd_MAXd                                                                                      0
#define HWIO_RSC_PARAM_RSC_CONFIG_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_RSC_PARAM_RSC_CONFIG_DRVd_ADDR(base,d), HWIO_RSC_PARAM_RSC_CONFIG_DRVd_RMSK)
#define HWIO_RSC_PARAM_RSC_CONFIG_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_RSC_PARAM_RSC_CONFIG_DRVd_ADDR(base,d), mask)
#define HWIO_RSC_PARAM_RSC_CONFIG_DRVd_NUM_DRV_BMSK                                                                      0x7000000
#define HWIO_RSC_PARAM_RSC_CONFIG_DRVd_NUM_DRV_SHFT                                                                           0x18
#define HWIO_RSC_PARAM_RSC_CONFIG_DRVd_NUM_SEQ_CMD_WORDS_BMSK                                                             0xff0000
#define HWIO_RSC_PARAM_RSC_CONFIG_DRVd_NUM_SEQ_CMD_WORDS_SHFT                                                                 0x10
#define HWIO_RSC_PARAM_RSC_CONFIG_DRVd_NUM_TS_EVENTS_BMSK                                                                    0xf00
#define HWIO_RSC_PARAM_RSC_CONFIG_DRVd_NUM_TS_EVENTS_SHFT                                                                      0x8
#define HWIO_RSC_PARAM_RSC_CONFIG_DRVd_DELAY_CNTR_BITWIDTH_BMSK                                                               0x1f
#define HWIO_RSC_PARAM_RSC_CONFIG_DRVd_DELAY_CNTR_BITWIDTH_SHFT                                                                0x0

#define HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_ADDR(base,d)                                                         ((base) + 0x0000000c + 0x10000 * (d))
#define HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_RMSK                                                                 0xf8ffffff
#define HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_MAXd                                                                          0
#define HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_ADDR(base,d), HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_RMSK)
#define HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_ADDR(base,d), mask)
#define HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_CMDS_PER_TCS_BMSK                                                0xf8000000
#define HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_CMDS_PER_TCS_SHFT                                                      0x1b
#define HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV3_BMSK                                                      0xfc0000
#define HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV3_SHFT                                                          0x12
#define HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV2_BMSK                                                       0x3f000
#define HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV2_SHFT                                                           0xc
#define HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV1_BMSK                                                         0xfc0
#define HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV1_SHFT                                                           0x6
#define HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV0_BMSK                                                          0x3f
#define HWIO_RSC_PARAM_RSC_PARENTCHILD_CONFIG_DRVd_NUM_TCS_DRV0_SHFT                                                           0x0

#define HWIO_RSC_STATUS0_DRVd_ADDR(base,d)                                                                              ((base) + 0x00000010 + 0x10000 * (d))
#define HWIO_RSC_STATUS0_DRVd_RMSK                                                                                       0x101ffff
#define HWIO_RSC_STATUS0_DRVd_MAXd                                                                                               0
#define HWIO_RSC_STATUS0_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_RSC_STATUS0_DRVd_ADDR(base,d), HWIO_RSC_STATUS0_DRVd_RMSK)
#define HWIO_RSC_STATUS0_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_RSC_STATUS0_DRVd_ADDR(base,d), mask)
#define HWIO_RSC_STATUS0_DRVd_OUTI(base,d,val)    \
        out_dword(HWIO_RSC_STATUS0_DRVd_ADDR(base,d),val)
#define HWIO_RSC_STATUS0_DRVd_OUTMI(base,d,mask,val) \
        out_dword_masked_ns(HWIO_RSC_STATUS0_DRVd_ADDR(base,d),mask,val,HWIO_RSC_STATUS0_DRVd_INI(base,d))
#define HWIO_RSC_STATUS0_DRVd_CLK_GATE_DISABLE_BMSK                                                                      0x1000000
#define HWIO_RSC_STATUS0_DRVd_CLK_GATE_DISABLE_SHFT                                                                           0x18
#define HWIO_RSC_STATUS0_DRVd_STATUS_SLEEP_ENTRY_DONE_BMSK                                                                 0x10000
#define HWIO_RSC_STATUS0_DRVd_STATUS_SLEEP_ENTRY_DONE_SHFT                                                                    0x10
#define HWIO_RSC_STATUS0_DRVd_STATUS_SLEEP_CTL_FSM_BMSK                                                                     0xffff
#define HWIO_RSC_STATUS0_DRVd_STATUS_SLEEP_CTL_FSM_SHFT                                                                        0x0

#define HWIO_RSC_STATUS1_DRVd_ADDR(base,d)                                                                              ((base) + 0x00000014 + 0x10000 * (d))
#define HWIO_RSC_STATUS1_DRVd_RMSK                                                                                      0xffffffff
#define HWIO_RSC_STATUS1_DRVd_MAXd                                                                                               0
#define HWIO_RSC_STATUS1_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_RSC_STATUS1_DRVd_ADDR(base,d), HWIO_RSC_STATUS1_DRVd_RMSK)
#define HWIO_RSC_STATUS1_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_RSC_STATUS1_DRVd_ADDR(base,d), mask)
#define HWIO_RSC_STATUS1_DRVd_PDC_MATCH_VALUE_LO_BMSK                                                                   0xffffffff
#define HWIO_RSC_STATUS1_DRVd_PDC_MATCH_VALUE_LO_SHFT                                                                          0x0

#define HWIO_RSC_STATUS2_DRVd_ADDR(base,d)                                                                              ((base) + 0x00000018 + 0x10000 * (d))
#define HWIO_RSC_STATUS2_DRVd_RMSK                                                                                        0xffffff
#define HWIO_RSC_STATUS2_DRVd_MAXd                                                                                               0
#define HWIO_RSC_STATUS2_DRVd_INI(base,d)        \
        in_dword_masked(HWIO_RSC_STATUS2_DRVd_ADDR(base,d), HWIO_RSC_STATUS2_DRVd_RMSK)
#define HWIO_RSC_STATUS2_DRVd_INMI(base,d,mask)    \
        in_dword_masked(HWIO_RSC_STATUS2_DRVd_ADDR(base,d), mask)
#define HWIO_RSC_STATUS2_DRVd_PDC_MATCH_VALUE_HI_BMSK                                                                     0xffffff
#define HWIO_RSC_STATUS2_DRVd_PDC_MATCH_VALUE_HI_SHFT                                                                          0x0

#define HWIO_HW_EVENT_OWNER_DRV0_ADDR(x)                                                                                ((x) + 0x0000004c)
#define HWIO_HW_EVENT_OWNER_DRV0_RMSK                                                                                          0x3
#define HWIO_HW_EVENT_OWNER_DRV0_IN(x)      \
        in_dword_masked(HWIO_HW_EVENT_OWNER_DRV0_ADDR(x), HWIO_HW_EVENT_OWNER_DRV0_RMSK)
#define HWIO_HW_EVENT_OWNER_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_HW_EVENT_OWNER_DRV0_ADDR(x), m)
#define HWIO_HW_EVENT_OWNER_DRV0_OUT(x, v)      \
        out_dword(HWIO_HW_EVENT_OWNER_DRV0_ADDR(x),v)
#define HWIO_HW_EVENT_OWNER_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_HW_EVENT_OWNER_DRV0_ADDR(x),m,v,HWIO_HW_EVENT_OWNER_DRV0_IN(x))
#define HWIO_HW_EVENT_OWNER_DRV0_OWNER_BMSK                                                                                    0x3
#define HWIO_HW_EVENT_OWNER_DRV0_OWNER_SHFT                                                                                    0x0

#define HWIO_HW_EVENT_MUXm_SELECT_DRVd_ADDR(base,d,m)                                                                   ((base) + 0x00000050 + 0x4 * (m) + 0x10000 * (d))
#define HWIO_HW_EVENT_MUXm_SELECT_DRVd_RMSK                                                                             0x80000007
#define HWIO_HW_EVENT_MUXm_SELECT_DRVd_MAXd                                                                                      0
#define HWIO_HW_EVENT_MUXm_SELECT_DRVd_MAXm                                                                                     31
#define HWIO_HW_EVENT_MUXm_SELECT_DRVd_INI2(base,d,m)        \
        in_dword_masked(HWIO_HW_EVENT_MUXm_SELECT_DRVd_ADDR(base,d,m), HWIO_HW_EVENT_MUXm_SELECT_DRVd_RMSK)
#define HWIO_HW_EVENT_MUXm_SELECT_DRVd_INMI2(base,d,m,mask)    \
        in_dword_masked(HWIO_HW_EVENT_MUXm_SELECT_DRVd_ADDR(base,d,m), mask)
#define HWIO_HW_EVENT_MUXm_SELECT_DRVd_OUTI2(base,d,m,val)    \
        out_dword(HWIO_HW_EVENT_MUXm_SELECT_DRVd_ADDR(base,d,m),val)
#define HWIO_HW_EVENT_MUXm_SELECT_DRVd_OUTMI2(base,d,m,mask,val) \
        out_dword_masked_ns(HWIO_HW_EVENT_MUXm_SELECT_DRVd_ADDR(base,d,m),mask,val,HWIO_HW_EVENT_MUXm_SELECT_DRVd_INI2(base,d,m))
#define HWIO_HW_EVENT_MUXm_SELECT_DRVd_ENABLE_BMSK                                                                      0x80000000
#define HWIO_HW_EVENT_MUXm_SELECT_DRVd_ENABLE_SHFT                                                                            0x1f
#define HWIO_HW_EVENT_MUXm_SELECT_DRVd_EVENT_SELECT_BMSK                                                                       0x7
#define HWIO_HW_EVENT_MUXm_SELECT_DRVd_EVENT_SELECT_SHFT                                                                       0x0

#define HWIO_RSC_ERROR_IRQ_STATUS_DRV0_ADDR(x)                                                                          ((x) + 0x000000d0)
#define HWIO_RSC_ERROR_IRQ_STATUS_DRV0_RMSK                                                                                    0x1
#define HWIO_RSC_ERROR_IRQ_STATUS_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_ERROR_IRQ_STATUS_DRV0_ADDR(x), HWIO_RSC_ERROR_IRQ_STATUS_DRV0_RMSK)
#define HWIO_RSC_ERROR_IRQ_STATUS_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_ERROR_IRQ_STATUS_DRV0_ADDR(x), m)
#define HWIO_RSC_ERROR_IRQ_STATUS_DRV0_EPCB_TIMEOUT_IRQ_BMSK                                                                   0x1
#define HWIO_RSC_ERROR_IRQ_STATUS_DRV0_EPCB_TIMEOUT_IRQ_SHFT                                                                   0x0

#define HWIO_RSC_ERROR_IRQ_CLEAR_DRV0_ADDR(x)                                                                           ((x) + 0x000000d4)
#define HWIO_RSC_ERROR_IRQ_CLEAR_DRV0_RMSK                                                                                     0x1
#define HWIO_RSC_ERROR_IRQ_CLEAR_DRV0_OUT(x, v)      \
        out_dword(HWIO_RSC_ERROR_IRQ_CLEAR_DRV0_ADDR(x),v)
#define HWIO_RSC_ERROR_IRQ_CLEAR_DRV0_CLR_EPCB_TIMEOUT_IRQ_BMSK                                                                0x1
#define HWIO_RSC_ERROR_IRQ_CLEAR_DRV0_CLR_EPCB_TIMEOUT_IRQ_SHFT                                                                0x0

#define HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_ADDR(x)                                                                          ((x) + 0x000000d8)
#define HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_RMSK                                                                               0x10ffff
#define HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_ADDR(x), HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_RMSK)
#define HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_ADDR(x), m)
#define HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_OUT(x, v)      \
        out_dword(HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_ADDR(x),v)
#define HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_ADDR(x),m,v,HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_IN(x))
#define HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_EPCB_TIMEOUT_IRQ_EN_BMSK                                                           0x100000
#define HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_EPCB_TIMEOUT_IRQ_EN_SHFT                                                               0x14
#define HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_EPCB_TIMEOUT_THRESHOLD_BMSK                                                          0xffff
#define HWIO_RSC_ERROR_IRQ_ENABLE_DRV0_EPCB_TIMEOUT_THRESHOLD_SHFT                                                             0x0

#define HWIO_RSC_ERROR_RESP_CTRL_DRV0_ADDR(x)                                                                           ((x) + 0x00000100)
#define HWIO_RSC_ERROR_RESP_CTRL_DRV0_RMSK                                                                                     0x1
#define HWIO_RSC_ERROR_RESP_CTRL_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_ERROR_RESP_CTRL_DRV0_ADDR(x), HWIO_RSC_ERROR_RESP_CTRL_DRV0_RMSK)
#define HWIO_RSC_ERROR_RESP_CTRL_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_ERROR_RESP_CTRL_DRV0_ADDR(x), m)
#define HWIO_RSC_ERROR_RESP_CTRL_DRV0_OUT(x, v)      \
        out_dword(HWIO_RSC_ERROR_RESP_CTRL_DRV0_ADDR(x),v)
#define HWIO_RSC_ERROR_RESP_CTRL_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RSC_ERROR_RESP_CTRL_DRV0_ADDR(x),m,v,HWIO_RSC_ERROR_RESP_CTRL_DRV0_IN(x))
#define HWIO_RSC_ERROR_RESP_CTRL_DRV0_ERROR_RESP_FOR_INVALID_ADDR_BMSK                                                         0x1
#define HWIO_RSC_ERROR_RESP_CTRL_DRV0_ERROR_RESP_FOR_INVALID_ADDR_SHFT                                                         0x0

#define HWIO_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ADDR(x)                                                                      ((x) + 0x00000200)
#define HWIO_RSC_TIMESTAMP_UNIT_OWNER_DRV0_RMSK                                                                                0x3
#define HWIO_RSC_TIMESTAMP_UNIT_OWNER_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ADDR(x), HWIO_RSC_TIMESTAMP_UNIT_OWNER_DRV0_RMSK)
#define HWIO_RSC_TIMESTAMP_UNIT_OWNER_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ADDR(x), m)
#define HWIO_RSC_TIMESTAMP_UNIT_OWNER_DRV0_OUT(x, v)      \
        out_dword(HWIO_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ADDR(x),v)
#define HWIO_RSC_TIMESTAMP_UNIT_OWNER_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RSC_TIMESTAMP_UNIT_OWNER_DRV0_ADDR(x),m,v,HWIO_RSC_TIMESTAMP_UNIT_OWNER_DRV0_IN(x))
#define HWIO_RSC_TIMESTAMP_UNIT_OWNER_DRV0_OWNER_BMSK                                                                          0x3
#define HWIO_RSC_TIMESTAMP_UNIT_OWNER_DRV0_OWNER_SHFT                                                                          0x0

#define HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_ADDR(base,d,m)                                                                 ((base) + 0x00000204 + 0x20 * (m) + 0x10000 * (d))
#define HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_RMSK                                                                                  0x1
#define HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_MAXd                                                                                    0
#define HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_MAXm                                                                                    5
#define HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_INI2(base,d,m)        \
        in_dword_masked(HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_ADDR(base,d,m), HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_RMSK)
#define HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_INMI2(base,d,m,mask)    \
        in_dword_masked(HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_ADDR(base,d,m), mask)
#define HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_OUTI2(base,d,m,val)    \
        out_dword(HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_ADDR(base,d,m),val)
#define HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_OUTMI2(base,d,m,mask,val) \
        out_dword_masked_ns(HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_ADDR(base,d,m),mask,val,HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_INI2(base,d,m))
#define HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_ENABLE_BMSK                                                                           0x1
#define HWIO_RSC_TIMESTAMP_UNITm_EN_DRVd_ENABLE_SHFT                                                                           0x0

#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_ADDR(base,d,m)                                                        ((base) + 0x00000208 + 0x20 * (m) + 0x10000 * (d))
#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_RMSK                                                                  0xffffffff
#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_MAXd                                                                           0
#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_MAXm                                                                           5
#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_INI2(base,d,m)        \
        in_dword_masked(HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_ADDR(base,d,m), HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_RMSK)
#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_INMI2(base,d,m,mask)    \
        in_dword_masked(HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_ADDR(base,d,m), mask)
#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_TS_DATA_L_BMSK                                                        0xffffffff
#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_L_DRVd_TS_DATA_L_SHFT                                                               0x0

#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_ADDR(base,d,m)                                                        ((base) + 0x0000020c + 0x20 * (m) + 0x10000 * (d))
#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_RMSK                                                                    0xffffff
#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_MAXd                                                                           0
#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_MAXm                                                                           5
#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_INI2(base,d,m)        \
        in_dword_masked(HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_ADDR(base,d,m), HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_RMSK)
#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_INMI2(base,d,m,mask)    \
        in_dword_masked(HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_ADDR(base,d,m), mask)
#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_TS_DATA_H_BMSK                                                          0xffffff
#define HWIO_RSC_TIMESTAMP_UNITm_TIMESTAMP_H_DRVd_TS_DATA_H_SHFT                                                               0x0

#define HWIO_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_ADDR(base,d,m)                                                             ((base) + 0x00000210 + 0x20 * (m) + 0x10000 * (d))
#define HWIO_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_RMSK                                                                            0x101
#define HWIO_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_MAXd                                                                                0
#define HWIO_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_MAXm                                                                                5
#define HWIO_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_INI2(base,d,m)        \
        in_dword_masked(HWIO_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_ADDR(base,d,m), HWIO_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_RMSK)
#define HWIO_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_INMI2(base,d,m,mask)    \
        in_dword_masked(HWIO_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_ADDR(base,d,m), mask)
#define HWIO_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_TS_OVERFLOW_BMSK                                                                0x100
#define HWIO_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_TS_OVERFLOW_SHFT                                                                  0x8
#define HWIO_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_TS_VALID_BMSK                                                                     0x1
#define HWIO_RSC_TIMESTAMP_UNITm_OUTPUT_DRVd_TS_VALID_SHFT                                                                     0x0

#define HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR(x)                                                                   ((x) + 0x00000400)
#define HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_RMSK                                                                      0x800003ff
#define HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR(x), HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_RMSK)
#define HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR(x), m)
#define HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_OUT(x, v)      \
        out_dword(HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR(x),v)
#define HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR(x),m,v,HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_IN(x))
#define HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_VALID_BMSK                                                                0x80000000
#define HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_VALID_SHFT                                                                      0x1f
#define HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR_BMSK                                                                      0x3ff
#define HWIO_RSC_SEQ_OVERRIDE_START_ADDR_DRV0_ADDR_SHFT                                                                        0x0

#define HWIO_RSC_SEQ_BUSY_DRV0_ADDR(x)                                                                                  ((x) + 0x00000404)
#define HWIO_RSC_SEQ_BUSY_DRV0_RMSK                                                                                            0x1
#define HWIO_RSC_SEQ_BUSY_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_SEQ_BUSY_DRV0_ADDR(x), HWIO_RSC_SEQ_BUSY_DRV0_RMSK)
#define HWIO_RSC_SEQ_BUSY_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_SEQ_BUSY_DRV0_ADDR(x), m)
#define HWIO_RSC_SEQ_BUSY_DRV0_BUSY_STAT_BMSK                                                                                  0x1
#define HWIO_RSC_SEQ_BUSY_DRV0_BUSY_STAT_SHFT                                                                                  0x0

#define HWIO_RSC_SEQ_PROGRAM_COUNTER_DRV0_ADDR(x)                                                                       ((x) + 0x00000408)
#define HWIO_RSC_SEQ_PROGRAM_COUNTER_DRV0_RMSK                                                                               0x3ff
#define HWIO_RSC_SEQ_PROGRAM_COUNTER_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_SEQ_PROGRAM_COUNTER_DRV0_ADDR(x), HWIO_RSC_SEQ_PROGRAM_COUNTER_DRV0_RMSK)
#define HWIO_RSC_SEQ_PROGRAM_COUNTER_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_SEQ_PROGRAM_COUNTER_DRV0_ADDR(x), m)
#define HWIO_RSC_SEQ_PROGRAM_COUNTER_DRV0_CURRENT_INSTR_ADDR_BMSK                                                            0x3ff
#define HWIO_RSC_SEQ_PROGRAM_COUNTER_DRV0_CURRENT_INSTR_ADDR_SHFT                                                              0x0

#define HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR(base,i)                                                                    ((base) + 0x00000410 + 0x4 * (i))
#define HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRV0_RMSK                                                                                 0x3ff
#define HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRV0_MAXi                                                                                     3
#define HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRV0_INI(base,i)        \
        in_dword_masked(HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR(base,i), HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRV0_RMSK)
#define HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRV0_INMI(base,i,mask)    \
        in_dword_masked(HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR(base,i), mask)
#define HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRV0_OUTI(base,i,val)    \
        out_dword(HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR(base,i),val)
#define HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRV0_OUTMI(base,i,mask,val) \
        out_dword_masked_ns(HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR(base,i),mask,val,HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRV0_INI(base,i))
#define HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR_BMSK                                                                            0x3ff
#define HWIO_RSC_SEQ_CFG_BR_ADDR_i_DRV0_ADDR_SHFT                                                                              0x0

#define HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ADDR(base,i)                                                                  ((base) + 0x00000450 + 0x4 * (i))
#define HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_RMSK                                                                             0xfffff
#define HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_MAXi                                                                                   3
#define HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_INI(base,i)        \
        in_dword_masked(HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ADDR(base,i), HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_RMSK)
#define HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_INMI(base,i,mask)    \
        in_dword_masked(HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ADDR(base,i), mask)
#define HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_OUTI(base,i,val)    \
        out_dword(HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ADDR(base,i),val)
#define HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_OUTMI(base,i,mask,val) \
        out_dword_masked_ns(HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_ADDR(base,i),mask,val,HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_INI(base,i))
#define HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_DELAY_VAL_BMSK                                                                   0xfffff
#define HWIO_RSC_SEQ_CFG_DELAY_VAL_i_DRV0_DELAY_VAL_SHFT                                                                       0x0

#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ADDR(x)                                                                      ((x) + 0x00000460)
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_RMSK                                                                                0x1
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ADDR(x), HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_RMSK)
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ADDR(x), m)
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_OUT(x, v)      \
        out_dword(HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ADDR(x),v)
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_ADDR(x),m,v,HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_IN(x))
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_SEQ_OVERRIDE_TRIGGER_BMSK                                                           0x1
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_DRV0_SEQ_OVERRIDE_TRIGGER_SHFT                                                           0x0

#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ADDR(x)                                                        ((x) + 0x00000464)
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_RMSK                                                                0x3ff
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ADDR(x), HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_RMSK)
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ADDR(x), m)
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_OUT(x, v)      \
        out_dword(HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ADDR(x),v)
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_ADDR(x),m,v,HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_IN(x))
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_SEQ_OVERRIDE_TRIGGER_START_ADDR_BMSK                                0x3ff
#define HWIO_RSC_SEQ_OVERRIDE_TRIGGER_START_ADDRESS_DRV0_SEQ_OVERRIDE_TRIGGER_START_ADDR_SHFT                                  0x0

#define HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ADDR(x)                                                                   ((x) + 0x00000490)
#define HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_RMSK                                                                      0x800003ff
#define HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ADDR(x), HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_RMSK)
#define HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ADDR(x), m)
#define HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_OUT(x, v)      \
        out_dword(HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ADDR(x),v)
#define HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_ADDR(x),m,v,HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_IN(x))
#define HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_DBG_BREAKPOINT_VALID_BMSK                                                 0x80000000
#define HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_DBG_BREAKPOINT_VALID_SHFT                                                       0x1f
#define HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_DBG_BREAKPOINT_ADDR_BMSK                                                       0x3ff
#define HWIO_RSC_SEQ_DBG_BREAKPOINT_ADDR_DRV0_DBG_BREAKPOINT_ADDR_SHFT                                                         0x0

#define HWIO_RSC_SEQ_DBG_STEP_DRV0_ADDR(x)                                                                              ((x) + 0x00000494)
#define HWIO_RSC_SEQ_DBG_STEP_DRV0_RMSK                                                                                        0x1
#define HWIO_RSC_SEQ_DBG_STEP_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_SEQ_DBG_STEP_DRV0_ADDR(x), HWIO_RSC_SEQ_DBG_STEP_DRV0_RMSK)
#define HWIO_RSC_SEQ_DBG_STEP_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_SEQ_DBG_STEP_DRV0_ADDR(x), m)
#define HWIO_RSC_SEQ_DBG_STEP_DRV0_OUT(x, v)      \
        out_dword(HWIO_RSC_SEQ_DBG_STEP_DRV0_ADDR(x),v)
#define HWIO_RSC_SEQ_DBG_STEP_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RSC_SEQ_DBG_STEP_DRV0_ADDR(x),m,v,HWIO_RSC_SEQ_DBG_STEP_DRV0_IN(x))
#define HWIO_RSC_SEQ_DBG_STEP_DRV0_DBG_STEP_TRIGGER_BMSK                                                                       0x1
#define HWIO_RSC_SEQ_DBG_STEP_DRV0_DBG_STEP_TRIGGER_SHFT                                                                       0x0

#define HWIO_RSC_SEQ_DBG_CONTINUE_DRV0_ADDR(x)                                                                          ((x) + 0x00000498)
#define HWIO_RSC_SEQ_DBG_CONTINUE_DRV0_RMSK                                                                                    0x1
#define HWIO_RSC_SEQ_DBG_CONTINUE_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_SEQ_DBG_CONTINUE_DRV0_ADDR(x), HWIO_RSC_SEQ_DBG_CONTINUE_DRV0_RMSK)
#define HWIO_RSC_SEQ_DBG_CONTINUE_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_SEQ_DBG_CONTINUE_DRV0_ADDR(x), m)
#define HWIO_RSC_SEQ_DBG_CONTINUE_DRV0_OUT(x, v)      \
        out_dword(HWIO_RSC_SEQ_DBG_CONTINUE_DRV0_ADDR(x),v)
#define HWIO_RSC_SEQ_DBG_CONTINUE_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RSC_SEQ_DBG_CONTINUE_DRV0_ADDR(x),m,v,HWIO_RSC_SEQ_DBG_CONTINUE_DRV0_IN(x))
#define HWIO_RSC_SEQ_DBG_CONTINUE_DRV0_DBG_STEP_CONTINUE_BMSK                                                                  0x1
#define HWIO_RSC_SEQ_DBG_CONTINUE_DRV0_DBG_STEP_CONTINUE_SHFT                                                                  0x0

#define HWIO_RSC_SEQ_DBG_STAT_DRV0_ADDR(x)                                                                              ((x) + 0x0000049c)
#define HWIO_RSC_SEQ_DBG_STAT_DRV0_RMSK                                                                                        0x1
#define HWIO_RSC_SEQ_DBG_STAT_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_SEQ_DBG_STAT_DRV0_ADDR(x), HWIO_RSC_SEQ_DBG_STAT_DRV0_RMSK)
#define HWIO_RSC_SEQ_DBG_STAT_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_SEQ_DBG_STAT_DRV0_ADDR(x), m)
#define HWIO_RSC_SEQ_DBG_STAT_DRV0_DBG_STEPPING_STATUS_BMSK                                                                    0x1
#define HWIO_RSC_SEQ_DBG_STAT_DRV0_DBG_STEPPING_STATUS_SHFT                                                                    0x0

#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ADDR(x)                                                               ((x) + 0x000004a0)
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_RMSK                                                                        0x3f
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ADDR(x), HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_RMSK)
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ADDR(x), m)
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_OUT(x, v)      \
        out_dword(HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ADDR(x),v)
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_ADDR(x),m,v,HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_IN(x))
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_SEQ_OVERRIDE_PWR_CNTRL_MASK_BMSK                                            0x3f
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_MASK_DRV0_SEQ_OVERRIDE_PWR_CNTRL_MASK_SHFT                                             0x0

#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ADDR(x)                                                                ((x) + 0x000004a4)
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_RMSK                                                                         0x3f
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ADDR(x), HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_RMSK)
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ADDR(x), m)
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_OUT(x, v)      \
        out_dword(HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ADDR(x),v)
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_ADDR(x),m,v,HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_IN(x))
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_SEQ_OVERRIDE_PWR_CNTRL_VAL_BMSK                                              0x3f
#define HWIO_RSC_SEQ_OVERRIDE_PWR_CNTRL_VAL_DRV0_SEQ_OVERRIDE_PWR_CNTRL_VAL_SHFT                                               0x0

#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ADDR(x)                                                              ((x) + 0x000004a8)
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_RMSK                                                                       0x3f
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ADDR(x), HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_RMSK)
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ADDR(x), m)
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_OUT(x, v)      \
        out_dword(HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ADDR(x),v)
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_ADDR(x),m,v,HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_IN(x))
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_SEQ_OVERRIDE_WAIT_EVENT_MASK_BMSK                                          0x3f
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_MASK_DRV0_SEQ_OVERRIDE_WAIT_EVENT_MASK_SHFT                                           0x0

#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ADDR(x)                                                               ((x) + 0x000004ac)
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_RMSK                                                                        0x3f
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_IN(x)      \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ADDR(x), HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_RMSK)
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_INM(x, m)      \
        in_dword_masked(HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ADDR(x), m)
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_OUT(x, v)      \
        out_dword(HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ADDR(x),v)
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_OUTM(x,m,v) \
        out_dword_masked_ns(HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_ADDR(x),m,v,HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_IN(x))
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_SEQ_OVERRIDE_WAIT_EVENT_VAL_BMSK                                            0x3f
#define HWIO_RSC_SEQ_OVERRIDE_WAIT_EVENT_VAL_DRV0_SEQ_OVERRIDE_WAIT_EVENT_VAL_SHFT                                             0x0

#define HWIO_SEQ_MEM_m_DRV0_ADDR(base,m)                                                                                ((base) + 0x00000600 + 0x4 * (m))
#define HWIO_SEQ_MEM_m_DRV0_RMSK                                                                                        0xffffffff
#define HWIO_SEQ_MEM_m_DRV0_MAXm                                                                                                47
#define HWIO_SEQ_MEM_m_DRV0_INI(base,m)        \
        in_dword_masked(HWIO_SEQ_MEM_m_DRV0_ADDR(base,m), HWIO_SEQ_MEM_m_DRV0_RMSK)
#define HWIO_SEQ_MEM_m_DRV0_INMI(base,m,mask)    \
        in_dword_masked(HWIO_SEQ_MEM_m_DRV0_ADDR(base,m), mask)
#define HWIO_SEQ_MEM_m_DRV0_OUTI(base,m,val)    \
        out_dword(HWIO_SEQ_MEM_m_DRV0_ADDR(base,m),val)
#define HWIO_SEQ_MEM_m_DRV0_OUTMI(base,m,mask,val) \
        out_dword_masked_ns(HWIO_SEQ_MEM_m_DRV0_ADDR(base,m),mask,val,HWIO_SEQ_MEM_m_DRV0_INI(base,m))
#define HWIO_SEQ_MEM_m_DRV0_CMD_WORD_BMSK                                                                               0xffffffff
#define HWIO_SEQ_MEM_m_DRV0_CMD_WORD_SHFT                                                                                      0x0


#endif /* __RSC_HALHWIOBASE_H__ */
