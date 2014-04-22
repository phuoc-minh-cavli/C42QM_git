#ifndef __USLEEP_HALHWIO_H__
#define __USLEEP_HALHWIO_H__
/*
===========================================================================
*/
/**
  @file uSleep_HALhwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    QCN7605 (Genoa) [genoa_prj_1.0_p1qxr31@/vobs/qca_genoa_pvob]
 
  This file contains HWIO register definitions for the following modules:
    QDSP6V67SS_CSR
    QDSP6V67SS_PUB


  Generation parameters: 
  { u'filename': u'uSleep_HALhwio.h',
    u'header': u'#include "msmhwiobase.h"',
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [u'QDSP6V67SS_CSR', u'QDSP6V67SS_PUB']}
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

  $Header: //components/rel/core.mpss/3.10/power/uSleep/src/hwio/default/wlan/uSleep_HALhwio.h#1 $
  $DateTime: 2019/02/20 03:28:54 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: QDSP6V67SS_CSR
 *--------------------------------------------------------------------------*/

#define QDSP6V67SS_CSR_REG_BASE                                                      (QDSP6V67SS_BASE      + 0x00080000)
#define QDSP6V67SS_CSR_REG_BASE_SIZE                                                 0xd000
#define QDSP6V67SS_CSR_REG_BASE_USED                                                 0xc24c

#define HWIO_QDSP6SS_INSTANCE_ID_ADDR                                                (QDSP6V67SS_CSR_REG_BASE      + 0x00000000)
#define HWIO_QDSP6SS_INSTANCE_ID_RMSK                                                       0x7
#define HWIO_QDSP6SS_INSTANCE_ID_IN          \
        in_dword_masked(HWIO_QDSP6SS_INSTANCE_ID_ADDR, HWIO_QDSP6SS_INSTANCE_ID_RMSK)
#define HWIO_QDSP6SS_INSTANCE_ID_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_INSTANCE_ID_ADDR, m)
#define HWIO_QDSP6SS_INSTANCE_ID_ID_BMSK                                                    0x7
#define HWIO_QDSP6SS_INSTANCE_ID_ID_SHFT                                                    0x0

#define HWIO_QDSP6SS_CP_EFUSE_STATUS_ADDR                                            (QDSP6V67SS_CSR_REG_BASE      + 0x00000100)
#define HWIO_QDSP6SS_CP_EFUSE_STATUS_RMSK                                                   0x1
#define HWIO_QDSP6SS_CP_EFUSE_STATUS_IN          \
        in_dword_masked(HWIO_QDSP6SS_CP_EFUSE_STATUS_ADDR, HWIO_QDSP6SS_CP_EFUSE_STATUS_RMSK)
#define HWIO_QDSP6SS_CP_EFUSE_STATUS_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_CP_EFUSE_STATUS_ADDR, m)
#define HWIO_QDSP6SS_CP_EFUSE_STATUS_HVX_EN_BMSK                                            0x1
#define HWIO_QDSP6SS_CP_EFUSE_STATUS_HVX_EN_SHFT                                            0x0

#define HWIO_QDSP6SS_WDOG_RESET_ADDR                                                 (QDSP6V67SS_CSR_REG_BASE      + 0x00004000)
#define HWIO_QDSP6SS_WDOG_RESET_RMSK                                                        0x1
#define HWIO_QDSP6SS_WDOG_RESET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_WDOG_RESET_ADDR,v)
#define HWIO_QDSP6SS_WDOG_RESET_WDOG_RESET_BMSK                                             0x1
#define HWIO_QDSP6SS_WDOG_RESET_WDOG_RESET_SHFT                                             0x0

#define HWIO_QDSP6SS_WDOG_CTL_ADDR                                                   (QDSP6V67SS_CSR_REG_BASE      + 0x00004004)
#define HWIO_QDSP6SS_WDOG_CTL_RMSK                                                          0x7
#define HWIO_QDSP6SS_WDOG_CTL_IN          \
        in_dword_masked(HWIO_QDSP6SS_WDOG_CTL_ADDR, HWIO_QDSP6SS_WDOG_CTL_RMSK)
#define HWIO_QDSP6SS_WDOG_CTL_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_WDOG_CTL_ADDR, m)
#define HWIO_QDSP6SS_WDOG_CTL_OUT(v)      \
        out_dword(HWIO_QDSP6SS_WDOG_CTL_ADDR,v)
#define HWIO_QDSP6SS_WDOG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_WDOG_CTL_ADDR,m,v,HWIO_QDSP6SS_WDOG_CTL_IN)
#define HWIO_QDSP6SS_WDOG_CTL_WDOG_TO_NMI_EN_BMSK                                           0x4
#define HWIO_QDSP6SS_WDOG_CTL_WDOG_TO_NMI_EN_SHFT                                           0x2
#define HWIO_QDSP6SS_WDOG_CTL_UNMASKED_INT_EN_BMSK                                          0x2
#define HWIO_QDSP6SS_WDOG_CTL_UNMASKED_INT_EN_SHFT                                          0x1
#define HWIO_QDSP6SS_WDOG_CTL_ENABLE_BMSK                                                   0x1
#define HWIO_QDSP6SS_WDOG_CTL_ENABLE_SHFT                                                   0x0

#define HWIO_QDSP6SS_WDOG_STATUS_ADDR                                                (QDSP6V67SS_CSR_REG_BASE      + 0x00004008)
#define HWIO_QDSP6SS_WDOG_STATUS_RMSK                                                0xf81fffff
#define HWIO_QDSP6SS_WDOG_STATUS_IN          \
        in_dword_masked(HWIO_QDSP6SS_WDOG_STATUS_ADDR, HWIO_QDSP6SS_WDOG_STATUS_RMSK)
#define HWIO_QDSP6SS_WDOG_STATUS_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_WDOG_STATUS_ADDR, m)
#define HWIO_QDSP6SS_WDOG_STATUS_RESET_SYNC_STATUS_BMSK                              0x80000000
#define HWIO_QDSP6SS_WDOG_STATUS_RESET_SYNC_STATUS_SHFT                                    0x1f
#define HWIO_QDSP6SS_WDOG_STATUS_BITE_TIME_SYNC_STATUS_BMSK                          0x40000000
#define HWIO_QDSP6SS_WDOG_STATUS_BITE_TIME_SYNC_STATUS_SHFT                                0x1e
#define HWIO_QDSP6SS_WDOG_STATUS_BARK_TIME_SYNC_STATUS_BMSK                          0x20000000
#define HWIO_QDSP6SS_WDOG_STATUS_BARK_TIME_SYNC_STATUS_SHFT                                0x1d
#define HWIO_QDSP6SS_WDOG_STATUS_NMI_TIME_SYNC_STATUS_BMSK                           0x10000000
#define HWIO_QDSP6SS_WDOG_STATUS_NMI_TIME_SYNC_STATUS_SHFT                                 0x1c
#define HWIO_QDSP6SS_WDOG_STATUS_TEST_LOAD_SYNC_STATUS_BMSK                           0x8000000
#define HWIO_QDSP6SS_WDOG_STATUS_TEST_LOAD_SYNC_STATUS_SHFT                                0x1b
#define HWIO_QDSP6SS_WDOG_STATUS_WDOG_COUNT_BMSK                                       0x1ffffe
#define HWIO_QDSP6SS_WDOG_STATUS_WDOG_COUNT_SHFT                                            0x1
#define HWIO_QDSP6SS_WDOG_STATUS_WDOG_EXPIRED_STATUS_BMSK                                   0x1
#define HWIO_QDSP6SS_WDOG_STATUS_WDOG_EXPIRED_STATUS_SHFT                                   0x0

#define HWIO_QDSP6SS_WDOG_BARK_TIME_ADDR                                             (QDSP6V67SS_CSR_REG_BASE      + 0x0000400c)
#define HWIO_QDSP6SS_WDOG_BARK_TIME_RMSK                                                0xfffff
#define HWIO_QDSP6SS_WDOG_BARK_TIME_IN          \
        in_dword_masked(HWIO_QDSP6SS_WDOG_BARK_TIME_ADDR, HWIO_QDSP6SS_WDOG_BARK_TIME_RMSK)
#define HWIO_QDSP6SS_WDOG_BARK_TIME_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_WDOG_BARK_TIME_ADDR, m)
#define HWIO_QDSP6SS_WDOG_BARK_TIME_OUT(v)      \
        out_dword(HWIO_QDSP6SS_WDOG_BARK_TIME_ADDR,v)
#define HWIO_QDSP6SS_WDOG_BARK_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_WDOG_BARK_TIME_ADDR,m,v,HWIO_QDSP6SS_WDOG_BARK_TIME_IN)
#define HWIO_QDSP6SS_WDOG_BARK_TIME_DATA_BMSK                                           0xfffff
#define HWIO_QDSP6SS_WDOG_BARK_TIME_DATA_SHFT                                               0x0

#define HWIO_QDSP6SS_WDOG_BITE_TIME_ADDR                                             (QDSP6V67SS_CSR_REG_BASE      + 0x00004010)
#define HWIO_QDSP6SS_WDOG_BITE_TIME_RMSK                                                0xfffff
#define HWIO_QDSP6SS_WDOG_BITE_TIME_IN          \
        in_dword_masked(HWIO_QDSP6SS_WDOG_BITE_TIME_ADDR, HWIO_QDSP6SS_WDOG_BITE_TIME_RMSK)
#define HWIO_QDSP6SS_WDOG_BITE_TIME_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_WDOG_BITE_TIME_ADDR, m)
#define HWIO_QDSP6SS_WDOG_BITE_TIME_OUT(v)      \
        out_dword(HWIO_QDSP6SS_WDOG_BITE_TIME_ADDR,v)
#define HWIO_QDSP6SS_WDOG_BITE_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_WDOG_BITE_TIME_ADDR,m,v,HWIO_QDSP6SS_WDOG_BITE_TIME_IN)
#define HWIO_QDSP6SS_WDOG_BITE_TIME_DATA_BMSK                                           0xfffff
#define HWIO_QDSP6SS_WDOG_BITE_TIME_DATA_SHFT                                               0x0

#define HWIO_QDSP6SS_WDOG_NMI_TIME_ADDR                                              (QDSP6V67SS_CSR_REG_BASE      + 0x00004014)
#define HWIO_QDSP6SS_WDOG_NMI_TIME_RMSK                                                 0xfffff
#define HWIO_QDSP6SS_WDOG_NMI_TIME_IN          \
        in_dword_masked(HWIO_QDSP6SS_WDOG_NMI_TIME_ADDR, HWIO_QDSP6SS_WDOG_NMI_TIME_RMSK)
#define HWIO_QDSP6SS_WDOG_NMI_TIME_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_WDOG_NMI_TIME_ADDR, m)
#define HWIO_QDSP6SS_WDOG_NMI_TIME_OUT(v)      \
        out_dword(HWIO_QDSP6SS_WDOG_NMI_TIME_ADDR,v)
#define HWIO_QDSP6SS_WDOG_NMI_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_WDOG_NMI_TIME_ADDR,m,v,HWIO_QDSP6SS_WDOG_NMI_TIME_IN)
#define HWIO_QDSP6SS_WDOG_NMI_TIME_DATA_BMSK                                            0xfffff
#define HWIO_QDSP6SS_WDOG_NMI_TIME_DATA_SHFT                                                0x0

#define HWIO_QDSP6SS_WDOG_TEST_LOAD_ADDR                                             (QDSP6V67SS_CSR_REG_BASE      + 0x00004018)
#define HWIO_QDSP6SS_WDOG_TEST_LOAD_RMSK                                                    0x1
#define HWIO_QDSP6SS_WDOG_TEST_LOAD_OUT(v)      \
        out_dword(HWIO_QDSP6SS_WDOG_TEST_LOAD_ADDR,v)
#define HWIO_QDSP6SS_WDOG_TEST_LOAD_LOAD_BMSK                                               0x1
#define HWIO_QDSP6SS_WDOG_TEST_LOAD_LOAD_SHFT                                               0x0

#define HWIO_QDSP6SS_WDOG_TEST_ADDR                                                  (QDSP6V67SS_CSR_REG_BASE      + 0x0000401c)
#define HWIO_QDSP6SS_WDOG_TEST_RMSK                                                    0x1fffff
#define HWIO_QDSP6SS_WDOG_TEST_IN          \
        in_dword_masked(HWIO_QDSP6SS_WDOG_TEST_ADDR, HWIO_QDSP6SS_WDOG_TEST_RMSK)
#define HWIO_QDSP6SS_WDOG_TEST_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_WDOG_TEST_ADDR, m)
#define HWIO_QDSP6SS_WDOG_TEST_OUT(v)      \
        out_dword(HWIO_QDSP6SS_WDOG_TEST_ADDR,v)
#define HWIO_QDSP6SS_WDOG_TEST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_WDOG_TEST_ADDR,m,v,HWIO_QDSP6SS_WDOG_TEST_IN)
#define HWIO_QDSP6SS_WDOG_TEST_SYNC_STATUS_BMSK                                        0x100000
#define HWIO_QDSP6SS_WDOG_TEST_SYNC_STATUS_SHFT                                            0x14
#define HWIO_QDSP6SS_WDOG_TEST_LOAD_VALUE_BMSK                                          0xfffff
#define HWIO_QDSP6SS_WDOG_TEST_LOAD_VALUE_SHFT                                              0x0

#define HWIO_QDSP6SS_GLBCNT_TRIG_CFG_ADDR                                            (QDSP6V67SS_CSR_REG_BASE      + 0x00004100)
#define HWIO_QDSP6SS_GLBCNT_TRIG_CFG_RMSK                                                   0xf
#define HWIO_QDSP6SS_GLBCNT_TRIG_CFG_IN          \
        in_dword_masked(HWIO_QDSP6SS_GLBCNT_TRIG_CFG_ADDR, HWIO_QDSP6SS_GLBCNT_TRIG_CFG_RMSK)
#define HWIO_QDSP6SS_GLBCNT_TRIG_CFG_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_GLBCNT_TRIG_CFG_ADDR, m)
#define HWIO_QDSP6SS_GLBCNT_TRIG_CFG_OUT(v)      \
        out_dword(HWIO_QDSP6SS_GLBCNT_TRIG_CFG_ADDR,v)
#define HWIO_QDSP6SS_GLBCNT_TRIG_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_GLBCNT_TRIG_CFG_ADDR,m,v,HWIO_QDSP6SS_GLBCNT_TRIG_CFG_IN)
#define HWIO_QDSP6SS_GLBCNT_TRIG_CFG_MAX_COUNT_BMSK                                         0xe
#define HWIO_QDSP6SS_GLBCNT_TRIG_CFG_MAX_COUNT_SHFT                                         0x1
#define HWIO_QDSP6SS_GLBCNT_TRIG_CFG_DISABLE_BMSK                                           0x1
#define HWIO_QDSP6SS_GLBCNT_TRIG_CFG_DISABLE_SHFT                                           0x0

#define HWIO_QDSP6SS_TS_m_EN_ADDR(m)                                                 (QDSP6V67SS_CSR_REG_BASE      + 0x00005000 + 0x10 * (m))
#define HWIO_QDSP6SS_TS_m_EN_RMSK                                                           0x7
#define HWIO_QDSP6SS_TS_m_EN_MAXm                                                             3
#define HWIO_QDSP6SS_TS_m_EN_INI(m)        \
        in_dword_masked(HWIO_QDSP6SS_TS_m_EN_ADDR(m), HWIO_QDSP6SS_TS_m_EN_RMSK)
#define HWIO_QDSP6SS_TS_m_EN_INMI(m,mask)    \
        in_dword_masked(HWIO_QDSP6SS_TS_m_EN_ADDR(m), mask)
#define HWIO_QDSP6SS_TS_m_EN_OUTI(m,val)    \
        out_dword(HWIO_QDSP6SS_TS_m_EN_ADDR(m),val)
#define HWIO_QDSP6SS_TS_m_EN_OUTMI(m,mask,val) \
        out_dword_masked_ns(HWIO_QDSP6SS_TS_m_EN_ADDR(m),mask,val,HWIO_QDSP6SS_TS_m_EN_INI(m))
#define HWIO_QDSP6SS_TS_m_EN_MODE_BMSK                                                      0x4
#define HWIO_QDSP6SS_TS_m_EN_MODE_SHFT                                                      0x2
#define HWIO_QDSP6SS_TS_m_EN_CLEAR_BMSK                                                     0x2
#define HWIO_QDSP6SS_TS_m_EN_CLEAR_SHFT                                                     0x1
#define HWIO_QDSP6SS_TS_m_EN_EN_BMSK                                                        0x1
#define HWIO_QDSP6SS_TS_m_EN_EN_SHFT                                                        0x0

#define HWIO_QDSP6SS_TS_m_STATUS_ADDR(m)                                             (QDSP6V67SS_CSR_REG_BASE      + 0x00005004 + 0x10 * (m))
#define HWIO_QDSP6SS_TS_m_STATUS_RMSK                                                     0x101
#define HWIO_QDSP6SS_TS_m_STATUS_MAXm                                                         3
#define HWIO_QDSP6SS_TS_m_STATUS_INI(m)        \
        in_dword_masked(HWIO_QDSP6SS_TS_m_STATUS_ADDR(m), HWIO_QDSP6SS_TS_m_STATUS_RMSK)
#define HWIO_QDSP6SS_TS_m_STATUS_INMI(m,mask)    \
        in_dword_masked(HWIO_QDSP6SS_TS_m_STATUS_ADDR(m), mask)
#define HWIO_QDSP6SS_TS_m_STATUS_OVERFLOW_BMSK                                            0x100
#define HWIO_QDSP6SS_TS_m_STATUS_OVERFLOW_SHFT                                              0x8
#define HWIO_QDSP6SS_TS_m_STATUS_VALID_BMSK                                                 0x1
#define HWIO_QDSP6SS_TS_m_STATUS_VALID_SHFT                                                 0x0

#define HWIO_QDSP6SS_TS_m_TIMESTAMP_L_ADDR(m)                                        (QDSP6V67SS_CSR_REG_BASE      + 0x00005008 + 0x10 * (m))
#define HWIO_QDSP6SS_TS_m_TIMESTAMP_L_RMSK                                           0xffffffff
#define HWIO_QDSP6SS_TS_m_TIMESTAMP_L_MAXm                                                    3
#define HWIO_QDSP6SS_TS_m_TIMESTAMP_L_INI(m)        \
        in_dword_masked(HWIO_QDSP6SS_TS_m_TIMESTAMP_L_ADDR(m), HWIO_QDSP6SS_TS_m_TIMESTAMP_L_RMSK)
#define HWIO_QDSP6SS_TS_m_TIMESTAMP_L_INMI(m,mask)    \
        in_dword_masked(HWIO_QDSP6SS_TS_m_TIMESTAMP_L_ADDR(m), mask)
#define HWIO_QDSP6SS_TS_m_TIMESTAMP_L_DATA_L_BMSK                                    0xffffffff
#define HWIO_QDSP6SS_TS_m_TIMESTAMP_L_DATA_L_SHFT                                           0x0

#define HWIO_QDSP6SS_TS_m_TIMESTAMP_H_ADDR(m)                                        (QDSP6V67SS_CSR_REG_BASE      + 0x0000500c + 0x10 * (m))
#define HWIO_QDSP6SS_TS_m_TIMESTAMP_H_RMSK                                             0xffffff
#define HWIO_QDSP6SS_TS_m_TIMESTAMP_H_MAXm                                                    3
#define HWIO_QDSP6SS_TS_m_TIMESTAMP_H_INI(m)        \
        in_dword_masked(HWIO_QDSP6SS_TS_m_TIMESTAMP_H_ADDR(m), HWIO_QDSP6SS_TS_m_TIMESTAMP_H_RMSK)
#define HWIO_QDSP6SS_TS_m_TIMESTAMP_H_INMI(m,mask)    \
        in_dword_masked(HWIO_QDSP6SS_TS_m_TIMESTAMP_H_ADDR(m), mask)
#define HWIO_QDSP6SS_TS_m_TIMESTAMP_H_DATA_H_BMSK                                      0xffffff
#define HWIO_QDSP6SS_TS_m_TIMESTAMP_H_DATA_H_SHFT                                           0x0

#define HWIO_QDSP6SS_TS_HWE_SEL_ADDR                                                 (QDSP6V67SS_CSR_REG_BASE      + 0x00005100)
#define HWIO_QDSP6SS_TS_HWE_SEL_RMSK                                                       0x3f
#define HWIO_QDSP6SS_TS_HWE_SEL_IN          \
        in_dword_masked(HWIO_QDSP6SS_TS_HWE_SEL_ADDR, HWIO_QDSP6SS_TS_HWE_SEL_RMSK)
#define HWIO_QDSP6SS_TS_HWE_SEL_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_TS_HWE_SEL_ADDR, m)
#define HWIO_QDSP6SS_TS_HWE_SEL_OUT(v)      \
        out_dword(HWIO_QDSP6SS_TS_HWE_SEL_ADDR,v)
#define HWIO_QDSP6SS_TS_HWE_SEL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_TS_HWE_SEL_ADDR,m,v,HWIO_QDSP6SS_TS_HWE_SEL_IN)
#define HWIO_QDSP6SS_TS_HWE_SEL_MUX_SEL_BMSK                                               0x3f
#define HWIO_QDSP6SS_TS_HWE_SEL_MUX_SEL_SHFT                                                0x0

#define HWIO_QDSP6SS_IPC_ADDR                                                        (QDSP6V67SS_CSR_REG_BASE      + 0x00008000)
#define HWIO_QDSP6SS_IPC_RMSK                                                        0xffffffff
#define HWIO_QDSP6SS_IPC_OUT(v)      \
        out_dword(HWIO_QDSP6SS_IPC_ADDR,v)
#define HWIO_QDSP6SS_IPC_INTR_BMSK                                                   0xffffffff
#define HWIO_QDSP6SS_IPC_INTR_SHFT                                                          0x0

#define HWIO_QDSP6SS_IPC1_ADDR                                                       (QDSP6V67SS_CSR_REG_BASE      + 0x00008004)
#define HWIO_QDSP6SS_IPC1_RMSK                                                       0xffffffff
#define HWIO_QDSP6SS_IPC1_OUT(v)      \
        out_dword(HWIO_QDSP6SS_IPC1_ADDR,v)
#define HWIO_QDSP6SS_IPC1_INTR_BMSK                                                  0xffffffff
#define HWIO_QDSP6SS_IPC1_INTR_SHFT                                                         0x0

#define HWIO_QDSP6SS_BLOCK_INTR_ADDR                                                 (QDSP6V67SS_CSR_REG_BASE      + 0x0000c008)
#define HWIO_QDSP6SS_BLOCK_INTR_RMSK                                                        0x7
#define HWIO_QDSP6SS_BLOCK_INTR_IN          \
        in_dword_masked(HWIO_QDSP6SS_BLOCK_INTR_ADDR, HWIO_QDSP6SS_BLOCK_INTR_RMSK)
#define HWIO_QDSP6SS_BLOCK_INTR_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_BLOCK_INTR_ADDR, m)
#define HWIO_QDSP6SS_BLOCK_INTR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_BLOCK_INTR_ADDR,v)
#define HWIO_QDSP6SS_BLOCK_INTR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_BLOCK_INTR_ADDR,m,v,HWIO_QDSP6SS_BLOCK_INTR_IN)
#define HWIO_QDSP6SS_BLOCK_INTR_CLEAR_BLOCK_BMSK                                            0x4
#define HWIO_QDSP6SS_BLOCK_INTR_CLEAR_BLOCK_SHFT                                            0x2
#define HWIO_QDSP6SS_BLOCK_INTR_BLOCK_STATUS_BMSK                                           0x2
#define HWIO_QDSP6SS_BLOCK_INTR_BLOCK_STATUS_SHFT                                           0x1
#define HWIO_QDSP6SS_BLOCK_INTR_BLOCK_INTR_BMSK                                             0x1
#define HWIO_QDSP6SS_BLOCK_INTR_BLOCK_INTR_SHFT                                             0x0

#define HWIO_QDSP6SS_SLPC_CFG_ADDR                                                   (QDSP6V67SS_CSR_REG_BASE      + 0x0000c01c)
#define HWIO_QDSP6SS_SLPC_CFG_RMSK                                                       0x7fa0
#define HWIO_QDSP6SS_SLPC_CFG_IN          \
        in_dword_masked(HWIO_QDSP6SS_SLPC_CFG_ADDR, HWIO_QDSP6SS_SLPC_CFG_RMSK)
#define HWIO_QDSP6SS_SLPC_CFG_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_SLPC_CFG_ADDR, m)
#define HWIO_QDSP6SS_SLPC_CFG_OUT(v)      \
        out_dword(HWIO_QDSP6SS_SLPC_CFG_ADDR,v)
#define HWIO_QDSP6SS_SLPC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_SLPC_CFG_ADDR,m,v,HWIO_QDSP6SS_SLPC_CFG_IN)
#define HWIO_QDSP6SS_SLPC_CFG_MEM_FAST_STAGGER_BMSK                                      0x4000
#define HWIO_QDSP6SS_SLPC_CFG_MEM_FAST_STAGGER_SHFT                                         0xe
#define HWIO_QDSP6SS_SLPC_CFG_IDLE_CORE_CLK_EN_BMSK                                      0x2000
#define HWIO_QDSP6SS_SLPC_CFG_IDLE_CORE_CLK_EN_SHFT                                         0xd
#define HWIO_QDSP6SS_SLPC_CFG_EXTHW_WAKE_EN_BMSK                                         0x1000
#define HWIO_QDSP6SS_SLPC_CFG_EXTHW_WAKE_EN_SHFT                                            0xc
#define HWIO_QDSP6SS_SLPC_CFG_MEM_PU_PERI_STAGGER_BMSK                                    0x800
#define HWIO_QDSP6SS_SLPC_CFG_MEM_PU_PERI_STAGGER_SHFT                                      0xb
#define HWIO_QDSP6SS_SLPC_CFG_MEM_PD_PERI_STAGGER_BMSK                                    0x400
#define HWIO_QDSP6SS_SLPC_CFG_MEM_PD_PERI_STAGGER_SHFT                                      0xa
#define HWIO_QDSP6SS_SLPC_CFG_MEM_PU_ARRY_STAGGER_BMSK                                    0x200
#define HWIO_QDSP6SS_SLPC_CFG_MEM_PU_ARRY_STAGGER_SHFT                                      0x9
#define HWIO_QDSP6SS_SLPC_CFG_MEM_PD_ARRY_STAGGER_BMSK                                    0x100
#define HWIO_QDSP6SS_SLPC_CFG_MEM_PD_ARRY_STAGGER_SHFT                                      0x8
#define HWIO_QDSP6SS_SLPC_CFG_WAKEUP_IN_EN_BMSK                                            0x80
#define HWIO_QDSP6SS_SLPC_CFG_WAKEUP_IN_EN_SHFT                                             0x7
#define HWIO_QDSP6SS_SLPC_CFG_CLK_GATING_MODE_BMSK                                         0x20
#define HWIO_QDSP6SS_SLPC_CFG_CLK_GATING_MODE_SHFT                                          0x5

#define HWIO_QDSP6SS_RSCCTL_BR_ADDR                                                  (QDSP6V67SS_CSR_REG_BASE      + 0x0000c020)
#define HWIO_QDSP6SS_RSCCTL_BR_RMSK                                                         0xe
#define HWIO_QDSP6SS_RSCCTL_BR_IN          \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_BR_ADDR, HWIO_QDSP6SS_RSCCTL_BR_RMSK)
#define HWIO_QDSP6SS_RSCCTL_BR_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_BR_ADDR, m)
#define HWIO_QDSP6SS_RSCCTL_BR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_BR_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_BR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_RSCCTL_BR_ADDR,m,v,HWIO_QDSP6SS_RSCCTL_BR_IN)
#define HWIO_QDSP6SS_RSCCTL_BR_EVENT_BMSK                                                   0xe
#define HWIO_QDSP6SS_RSCCTL_BR_EVENT_SHFT                                                   0x1

#define HWIO_QDSP6SS_WAKEUP_CTL_ADDR                                                 (QDSP6V67SS_CSR_REG_BASE      + 0x0000c024)
#define HWIO_QDSP6SS_WAKEUP_CTL_RMSK                                                        0x3
#define HWIO_QDSP6SS_WAKEUP_CTL_IN          \
        in_dword_masked(HWIO_QDSP6SS_WAKEUP_CTL_ADDR, HWIO_QDSP6SS_WAKEUP_CTL_RMSK)
#define HWIO_QDSP6SS_WAKEUP_CTL_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_WAKEUP_CTL_ADDR, m)
#define HWIO_QDSP6SS_WAKEUP_CTL_OUT(v)      \
        out_dword(HWIO_QDSP6SS_WAKEUP_CTL_ADDR,v)
#define HWIO_QDSP6SS_WAKEUP_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_WAKEUP_CTL_ADDR,m,v,HWIO_QDSP6SS_WAKEUP_CTL_IN)
#define HWIO_QDSP6SS_WAKEUP_CTL_NMI_WAKE_EN_BMSK                                            0x2
#define HWIO_QDSP6SS_WAKEUP_CTL_NMI_WAKE_EN_SHFT                                            0x1
#define HWIO_QDSP6SS_WAKEUP_CTL_L2VIC_WAKE_EN_BMSK                                          0x1
#define HWIO_QDSP6SS_WAKEUP_CTL_L2VIC_WAKE_EN_SHFT                                          0x0

#define HWIO_QDSP6SS_L2VIC_WAKEUP_EN_ADDR                                            (QDSP6V67SS_CSR_REG_BASE      + 0x0000c028)
#define HWIO_QDSP6SS_L2VIC_WAKEUP_EN_RMSK                                                  0x1c
#define HWIO_QDSP6SS_L2VIC_WAKEUP_EN_IN          \
        in_dword_masked(HWIO_QDSP6SS_L2VIC_WAKEUP_EN_ADDR, HWIO_QDSP6SS_L2VIC_WAKEUP_EN_RMSK)
#define HWIO_QDSP6SS_L2VIC_WAKEUP_EN_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_L2VIC_WAKEUP_EN_ADDR, m)
#define HWIO_QDSP6SS_L2VIC_WAKEUP_EN_OUT(v)      \
        out_dword(HWIO_QDSP6SS_L2VIC_WAKEUP_EN_ADDR,v)
#define HWIO_QDSP6SS_L2VIC_WAKEUP_EN_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_L2VIC_WAKEUP_EN_ADDR,m,v,HWIO_QDSP6SS_L2VIC_WAKEUP_EN_IN)
#define HWIO_QDSP6SS_L2VIC_WAKEUP_EN_WAKEUP_EN_BMSK                                        0x1c
#define HWIO_QDSP6SS_L2VIC_WAKEUP_EN_WAKEUP_EN_SHFT                                         0x2

#define HWIO_QDSP6SS_WAKEUP_STATUS_ADDR                                              (QDSP6V67SS_CSR_REG_BASE      + 0x0000c02c)
#define HWIO_QDSP6SS_WAKEUP_STATUS_RMSK                                                    0x1f
#define HWIO_QDSP6SS_WAKEUP_STATUS_IN          \
        in_dword_masked(HWIO_QDSP6SS_WAKEUP_STATUS_ADDR, HWIO_QDSP6SS_WAKEUP_STATUS_RMSK)
#define HWIO_QDSP6SS_WAKEUP_STATUS_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_WAKEUP_STATUS_ADDR, m)
#define HWIO_QDSP6SS_WAKEUP_STATUS_OUT(v)      \
        out_dword(HWIO_QDSP6SS_WAKEUP_STATUS_ADDR,v)
#define HWIO_QDSP6SS_WAKEUP_STATUS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_WAKEUP_STATUS_ADDR,m,v,HWIO_QDSP6SS_WAKEUP_STATUS_IN)
#define HWIO_QDSP6SS_WAKEUP_STATUS_AXIS_WAKEUP_STICKY_BMSK                                 0x10
#define HWIO_QDSP6SS_WAKEUP_STATUS_AXIS_WAKEUP_STICKY_SHFT                                  0x4
#define HWIO_QDSP6SS_WAKEUP_STATUS_AXIS_WAKEUP_BMSK                                         0x8
#define HWIO_QDSP6SS_WAKEUP_STATUS_AXIS_WAKEUP_SHFT                                         0x3
#define HWIO_QDSP6SS_WAKEUP_STATUS_DBG_NMI_BMSK                                             0x4
#define HWIO_QDSP6SS_WAKEUP_STATUS_DBG_NMI_SHFT                                             0x2
#define HWIO_QDSP6SS_WAKEUP_STATUS_EXT_TRIGGER_BMSK                                         0x2
#define HWIO_QDSP6SS_WAKEUP_STATUS_EXT_TRIGGER_SHFT                                         0x1
#define HWIO_QDSP6SS_WAKEUP_STATUS_L2VIC_WAKEUP_BMSK                                        0x1
#define HWIO_QDSP6SS_WAKEUP_STATUS_L2VIC_WAKEUP_SHFT                                        0x0

#define HWIO_QDSP6SS_RSC_EVENT_ISOINTF_OVR_ADDR                                      (QDSP6V67SS_CSR_REG_BASE      + 0x0000c054)
#define HWIO_QDSP6SS_RSC_EVENT_ISOINTF_OVR_RMSK                                             0x7
#define HWIO_QDSP6SS_RSC_EVENT_ISOINTF_OVR_IN          \
        in_dword_masked(HWIO_QDSP6SS_RSC_EVENT_ISOINTF_OVR_ADDR, HWIO_QDSP6SS_RSC_EVENT_ISOINTF_OVR_RMSK)
#define HWIO_QDSP6SS_RSC_EVENT_ISOINTF_OVR_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_RSC_EVENT_ISOINTF_OVR_ADDR, m)
#define HWIO_QDSP6SS_RSC_EVENT_ISOINTF_OVR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSC_EVENT_ISOINTF_OVR_ADDR,v)
#define HWIO_QDSP6SS_RSC_EVENT_ISOINTF_OVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_RSC_EVENT_ISOINTF_OVR_ADDR,m,v,HWIO_QDSP6SS_RSC_EVENT_ISOINTF_OVR_IN)
#define HWIO_QDSP6SS_RSC_EVENT_ISOINTF_OVR_AXIM2_HALTACK_OVRRD_BMSK                         0x4
#define HWIO_QDSP6SS_RSC_EVENT_ISOINTF_OVR_AXIM2_HALTACK_OVRRD_SHFT                         0x2
#define HWIO_QDSP6SS_RSC_EVENT_ISOINTF_OVR_AXIM_HALTACK_OVRRD_BMSK                          0x2
#define HWIO_QDSP6SS_RSC_EVENT_ISOINTF_OVR_AXIM_HALTACK_OVRRD_SHFT                          0x1
#define HWIO_QDSP6SS_RSC_EVENT_ISOINTF_OVR_ALL_HALTACK_OVRRD_BMSK                           0x1
#define HWIO_QDSP6SS_RSC_EVENT_ISOINTF_OVR_ALL_HALTACK_OVRRD_SHFT                           0x0

#define HWIO_QDSP6SS_RSC_EVENT_PRSC_OVR_ADDR                                         (QDSP6V67SS_CSR_REG_BASE      + 0x0000c058)
#define HWIO_QDSP6SS_RSC_EVENT_PRSC_OVR_RMSK                                             0x3fff
#define HWIO_QDSP6SS_RSC_EVENT_PRSC_OVR_IN          \
        in_dword_masked(HWIO_QDSP6SS_RSC_EVENT_PRSC_OVR_ADDR, HWIO_QDSP6SS_RSC_EVENT_PRSC_OVR_RMSK)
#define HWIO_QDSP6SS_RSC_EVENT_PRSC_OVR_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_RSC_EVENT_PRSC_OVR_ADDR, m)
#define HWIO_QDSP6SS_RSC_EVENT_PRSC_OVR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSC_EVENT_PRSC_OVR_ADDR,v)
#define HWIO_QDSP6SS_RSC_EVENT_PRSC_OVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_RSC_EVENT_PRSC_OVR_ADDR,m,v,HWIO_QDSP6SS_RSC_EVENT_PRSC_OVR_IN)
#define HWIO_QDSP6SS_RSC_EVENT_PRSC_OVR_SPARE_OVRRD_BMSK                                 0x3c00
#define HWIO_QDSP6SS_RSC_EVENT_PRSC_OVR_SPARE_OVRRD_SHFT                                    0xa
#define HWIO_QDSP6SS_RSC_EVENT_PRSC_OVR_TCS_HW_ACK_OVRRD_BMSK                             0x3fc
#define HWIO_QDSP6SS_RSC_EVENT_PRSC_OVR_TCS_HW_ACK_OVRRD_SHFT                               0x2
#define HWIO_QDSP6SS_RSC_EVENT_PRSC_OVR_BRINGUP_ACK_OVRRD_BMSK                              0x2
#define HWIO_QDSP6SS_RSC_EVENT_PRSC_OVR_BRINGUP_ACK_OVRRD_SHFT                              0x1
#define HWIO_QDSP6SS_RSC_EVENT_PRSC_OVR_SHUTDOWN_ACK_OVRRD_BMSK                             0x1
#define HWIO_QDSP6SS_RSC_EVENT_PRSC_OVR_SHUTDOWN_ACK_OVRRD_SHFT                             0x0

#define HWIO_QDSP6SS_RSC_EVENT_PLL_OVR_ADDR                                          (QDSP6V67SS_CSR_REG_BASE      + 0x0000c05c)
#define HWIO_QDSP6SS_RSC_EVENT_PLL_OVR_RMSK                                                 0x7
#define HWIO_QDSP6SS_RSC_EVENT_PLL_OVR_IN          \
        in_dword_masked(HWIO_QDSP6SS_RSC_EVENT_PLL_OVR_ADDR, HWIO_QDSP6SS_RSC_EVENT_PLL_OVR_RMSK)
#define HWIO_QDSP6SS_RSC_EVENT_PLL_OVR_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_RSC_EVENT_PLL_OVR_ADDR, m)
#define HWIO_QDSP6SS_RSC_EVENT_PLL_OVR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSC_EVENT_PLL_OVR_ADDR,v)
#define HWIO_QDSP6SS_RSC_EVENT_PLL_OVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_RSC_EVENT_PLL_OVR_ADDR,m,v,HWIO_QDSP6SS_RSC_EVENT_PLL_OVR_IN)
#define HWIO_QDSP6SS_RSC_EVENT_PLL_OVR_PLL_DYN_OVRRD_BMSK                                   0x4
#define HWIO_QDSP6SS_RSC_EVENT_PLL_OVR_PLL_DYN_OVRRD_SHFT                                   0x2
#define HWIO_QDSP6SS_RSC_EVENT_PLL_OVR_PLL_FINE_OVRRD_BMSK                                  0x2
#define HWIO_QDSP6SS_RSC_EVENT_PLL_OVR_PLL_FINE_OVRRD_SHFT                                  0x1
#define HWIO_QDSP6SS_RSC_EVENT_PLL_OVR_PLL_OVRRD_BMSK                                       0x1
#define HWIO_QDSP6SS_RSC_EVENT_PLL_OVR_PLL_OVRRD_SHFT                                       0x0

#define HWIO_QDSP6SS_RSC_EVENT_MEM_OVR_ADDR                                          (QDSP6V67SS_CSR_REG_BASE      + 0x0000c060)
#define HWIO_QDSP6SS_RSC_EVENT_MEM_OVR_RMSK                                                0x3f
#define HWIO_QDSP6SS_RSC_EVENT_MEM_OVR_IN          \
        in_dword_masked(HWIO_QDSP6SS_RSC_EVENT_MEM_OVR_ADDR, HWIO_QDSP6SS_RSC_EVENT_MEM_OVR_RMSK)
#define HWIO_QDSP6SS_RSC_EVENT_MEM_OVR_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_RSC_EVENT_MEM_OVR_ADDR, m)
#define HWIO_QDSP6SS_RSC_EVENT_MEM_OVR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSC_EVENT_MEM_OVR_ADDR,v)
#define HWIO_QDSP6SS_RSC_EVENT_MEM_OVR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_RSC_EVENT_MEM_OVR_ADDR,m,v,HWIO_QDSP6SS_RSC_EVENT_MEM_OVR_IN)
#define HWIO_QDSP6SS_RSC_EVENT_MEM_OVR_STAGGER_RESTORE_BMSK                                0x20
#define HWIO_QDSP6SS_RSC_EVENT_MEM_OVR_STAGGER_RESTORE_SHFT                                 0x5
#define HWIO_QDSP6SS_RSC_EVENT_MEM_OVR_REFRESH_BMSK                                        0x10
#define HWIO_QDSP6SS_RSC_EVENT_MEM_OVR_REFRESH_SHFT                                         0x4
#define HWIO_QDSP6SS_RSC_EVENT_MEM_OVR_PERIPH_PD_BMSK                                       0x8
#define HWIO_QDSP6SS_RSC_EVENT_MEM_OVR_PERIPH_PD_SHFT                                       0x3
#define HWIO_QDSP6SS_RSC_EVENT_MEM_OVR_PERIPH_READY_BMSK                                    0x4
#define HWIO_QDSP6SS_RSC_EVENT_MEM_OVR_PERIPH_READY_SHFT                                    0x2
#define HWIO_QDSP6SS_RSC_EVENT_MEM_OVR_CORE_PD_BMSK                                         0x2
#define HWIO_QDSP6SS_RSC_EVENT_MEM_OVR_CORE_PD_SHFT                                         0x1
#define HWIO_QDSP6SS_RSC_EVENT_MEM_OVR_CORE_READY_BMSK                                      0x1
#define HWIO_QDSP6SS_RSC_EVENT_MEM_OVR_CORE_READY_SHFT                                      0x0

#define HWIO_QDSP6SS_RSCCTL_EN_ARES_ADDR                                             (QDSP6V67SS_CSR_REG_BASE      + 0x0000c100)
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_RMSK                                                 0xffff
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_IN          \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_ARES_ADDR, HWIO_QDSP6SS_RSCCTL_EN_ARES_RMSK)
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_ARES_ADDR, m)
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_ARES_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_RSCCTL_EN_ARES_ADDR,m,v,HWIO_QDSP6SS_RSCCTL_EN_ARES_IN)
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_VPE_VU_BMSK                                          0x8000
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_VPE_VU_SHFT                                             0xf
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_NRET_BMSK                                            0x4000
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_NRET_SHFT                                               0xe
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_MNOC_BMSK                                            0x2000
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_MNOC_SHFT                                               0xd
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_APB_BMSK                                             0x1000
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_APB_SHFT                                                0xc
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_AXIS2_BMSK                                            0x800
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_AXIS2_SHFT                                              0xb
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CSI1_BMSK                                             0x400
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CSI1_SHFT                                               0xa
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CSI0_BMSK                                             0x200
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CSI0_SHFT                                               0x9
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_ARCH_BMSK                                             0x100
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_ARCH_SHFT                                               0x8
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CORE_BMSK                                              0x80
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CORE_SHFT                                               0x7
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_AXIM2_BMSK                                             0x40
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_AXIM2_SHFT                                              0x6
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_AXIS_BMSK                                              0x20
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_AXIS_SHFT                                               0x5
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_ATBM_BMSK                                              0x10
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_ATBM_SHFT                                               0x4
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_AHBM_BMSK                                               0x8
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_AHBM_SHFT                                               0x3
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_AXIM_BMSK                                               0x4
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_AXIM_SHFT                                               0x2
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_ISDB_ETM_BMSK                                           0x2
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_ISDB_ETM_SHFT                                           0x1
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_RET_BMSK                                                0x1
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_RET_SHFT                                                0x0

#define HWIO_QDSP6SS_RSCCTL_EN_CLAMP_ADDR                                            (QDSP6V67SS_CSR_REG_BASE      + 0x0000c104)
#define HWIO_QDSP6SS_RSCCTL_EN_CLAMP_RMSK                                                   0x3
#define HWIO_QDSP6SS_RSCCTL_EN_CLAMP_IN          \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_CLAMP_ADDR, HWIO_QDSP6SS_RSCCTL_EN_CLAMP_RMSK)
#define HWIO_QDSP6SS_RSCCTL_EN_CLAMP_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_CLAMP_ADDR, m)
#define HWIO_QDSP6SS_RSCCTL_EN_CLAMP_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_CLAMP_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_CLAMP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_RSCCTL_EN_CLAMP_ADDR,m,v,HWIO_QDSP6SS_RSCCTL_EN_CLAMP_IN)
#define HWIO_QDSP6SS_RSCCTL_EN_CLAMP_CLAMP_MSS2CX_BMSK                                      0x2
#define HWIO_QDSP6SS_RSCCTL_EN_CLAMP_CLAMP_MSS2CX_SHFT                                      0x1
#define HWIO_QDSP6SS_RSCCTL_EN_CLAMP_CLAMP_IO_BMSK                                          0x1
#define HWIO_QDSP6SS_RSCCTL_EN_CLAMP_CLAMP_IO_SHFT                                          0x0

#define HWIO_QDSP6SS_RSCCTL_EN_CLK_ADDR                                              (QDSP6V67SS_CSR_REG_BASE      + 0x0000c108)
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_RMSK                                                   0x7ff
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_IN          \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_CLK_ADDR, HWIO_QDSP6SS_RSCCTL_EN_CLK_RMSK)
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_CLK_ADDR, m)
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_CLK_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_RSCCTL_EN_CLK_ADDR,m,v,HWIO_QDSP6SS_RSCCTL_EN_CLK_IN)
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_VPE_VU_BMSK                                            0x400
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_VPE_VU_SHFT                                              0xa
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_AXIS2_BMSK                                             0x200
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_AXIS2_SHFT                                               0x9
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CSI1_BMSK                                              0x100
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CSI1_SHFT                                                0x8
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CSI0_BMSK                                               0x80
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CSI0_SHFT                                                0x7
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_AXIM2_BMSK                                              0x40
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_AXIM2_SHFT                                               0x6
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_AXIS_BMSK                                               0x20
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_AXIS_SHFT                                                0x5
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_ATBM_BMSK                                               0x10
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_ATBM_SHFT                                                0x4
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_AHBM_BMSK                                                0x8
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_AHBM_SHFT                                                0x3
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_AXIM_BMSK                                                0x4
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_AXIM_SHFT                                                0x2
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_APB_BMSK                                                 0x2
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_APB_SHFT                                                 0x1
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CORE_BMSK                                                0x1
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CORE_SHFT                                                0x0

#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_ADDR                                          (QDSP6V67SS_CSR_REG_BASE      + 0x0000c10c)
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_RMSK                                              0xffff
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_IN          \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_ADDR, HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_RMSK)
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_ADDR, m)
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_ADDR,m,v,HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_IN)
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SPARE_BMSK                                        0xc000
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SPARE_SHFT                                           0xe
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_L2CLADE_BMSK                                      0x2000
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_L2CLADE_SHFT                                         0xd
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_L2PLRU_BMSK                                       0x1000
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_L2PLRU_SHFT                                          0xc
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_L2TAG_BMSK                                         0x800
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_L2TAG_SHFT                                           0xb
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_L2STBUF_BMSK                                       0x400
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_L2STBUF_SHFT                                         0xa
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_L1DU_TAG_BMSK                                      0x200
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_L1DU_TAG_SHFT                                        0x9
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_L1DU_BMSK                                          0x100
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_L1DU_SHFT                                            0x8
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_JU_TLB64_BMSK                                       0x80
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_JU_TLB64_SHFT                                        0x7
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_JU_TLB128_BMSK                                      0x40
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_JU_TLB128_SHFT                                       0x6
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_JU_CAM_BMSK                                         0x20
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_JU_CAM_SHFT                                          0x5
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_ETB_BMSK                                            0x10
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_ETB_SHFT                                             0x4
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_L1IU_TAG_BMSK                                        0x8
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_L1IU_TAG_SHFT                                        0x3
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_L1IU_PDEC_BMSK                                       0x4
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_L1IU_PDEC_SHFT                                       0x2
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_L1IU_BMSK                                            0x2
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_L1IU_SHFT                                            0x1
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_L1IU_BTB_BMSK                                        0x1
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_L1IU_BTB_SHFT                                        0x0

#define HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_ADDR                                          (QDSP6V67SS_CSR_REG_BASE      + 0x0000c110)
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_RMSK                                                 0xa
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_IN          \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_ADDR, HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_RMSK)
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_ADDR, m)
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_ADDR,m,v,HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_IN)
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_CLAMP_QMC_MEM_BMSK                                   0x8
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_CLAMP_QMC_MEM_SHFT                                   0x3
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_MEM_PERIPH_BMSK                                      0x2
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_MEM_PERIPH_SHFT                                      0x1

#define HWIO_QDSP6SS_RSCCTL_EN_BHS_ADDR                                              (QDSP6V67SS_CSR_REG_BASE      + 0x0000c114)
#define HWIO_QDSP6SS_RSCCTL_EN_BHS_RMSK                                                     0x1
#define HWIO_QDSP6SS_RSCCTL_EN_BHS_IN          \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_BHS_ADDR, HWIO_QDSP6SS_RSCCTL_EN_BHS_RMSK)
#define HWIO_QDSP6SS_RSCCTL_EN_BHS_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_BHS_ADDR, m)
#define HWIO_QDSP6SS_RSCCTL_EN_BHS_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_BHS_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_BHS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_RSCCTL_EN_BHS_ADDR,m,v,HWIO_QDSP6SS_RSCCTL_EN_BHS_IN)
#define HWIO_QDSP6SS_RSCCTL_EN_BHS_BHS_BMSK                                                 0x1
#define HWIO_QDSP6SS_RSCCTL_EN_BHS_BHS_SHFT                                                 0x0

#define HWIO_QDSP6SS_RSCCTL_EN_LDO_ADDR                                              (QDSP6V67SS_CSR_REG_BASE      + 0x0000c118)
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_RMSK                                                    0x7f
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_IN          \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_LDO_ADDR, HWIO_QDSP6SS_RSCCTL_EN_LDO_RMSK)
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_LDO_ADDR, m)
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_LDO_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_RSCCTL_EN_LDO_ADDR,m,v,HWIO_QDSP6SS_RSCCTL_EN_LDO_IN)
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_BG_OFF_BMSK                                             0x40
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_BG_OFF_SHFT                                              0x6
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_FORCEOFF_BMSK                                           0x20
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_FORCEOFF_SHFT                                            0x5
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_SPARE_BITS4_2_BMSK                                      0x1c
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_SPARE_BITS4_2_SHFT                                       0x2
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_VREF_SEL_UPDATE_BMSK                                     0x2
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_VREF_SEL_UPDATE_SHFT                                     0x1
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_BYPASS_OPEN_BMSK                                         0x1
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_BYPASS_OPEN_SHFT                                         0x0

#define HWIO_QDSP6SS_RSCCTL_EN_PLL_ADDR                                              (QDSP6V67SS_CSR_REG_BASE      + 0x0000c120)
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_RMSK                                                    0x17
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_IN          \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_PLL_ADDR, HWIO_QDSP6SS_RSCCTL_EN_PLL_RMSK)
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_PLL_ADDR, m)
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_PLL_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_RSCCTL_EN_PLL_ADDR,m,v,HWIO_QDSP6SS_RSCCTL_EN_PLL_IN)
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_PLL_STBY_BMSK                                           0x10
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_PLL_STBY_SHFT                                            0x4
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_PLL_OUTDIS_BMSK                                          0x4
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_PLL_OUTDIS_SHFT                                          0x2
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_PLL_OPMODE1_BMSK                                         0x2
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_PLL_OPMODE1_SHFT                                         0x1
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_PLL_RESET_BMSK                                           0x1
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_PLL_RESET_SHFT                                           0x0

#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_ADDR                                         (QDSP6V67SS_CSR_REG_BASE      + 0x0000c124)
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_RMSK                                                0xf
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_IN          \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_STATERET_ADDR, HWIO_QDSP6SS_RSCCTL_EN_STATERET_RMSK)
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_STATERET_ADDR, m)
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_STATERET_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_RSCCTL_EN_STATERET_ADDR,m,v,HWIO_QDSP6SS_RSCCTL_EN_STATERET_IN)
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_PROF_IRQ_BMSK                                       0x8
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_PROF_IRQ_SHFT                                       0x3
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_WAKE_IRQ_BMSK                                       0x4
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_WAKE_IRQ_SHFT                                       0x2
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_RESTORE_BMSK                                        0x2
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_RESTORE_SHFT                                        0x1
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_SAVE_BMSK                                           0x1
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_SAVE_SHFT                                           0x0

#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_ADDR                                      (QDSP6V67SS_CSR_REG_BASE      + 0x0000c128)
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_RMSK                                            0x3d
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_IN          \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_ADDR, HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_RMSK)
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_ADDR, m)
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_ADDR,m,v,HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_IN)
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_AXIM2_FIFO_ARESET_BMSK                          0x20
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_AXIM2_FIFO_ARESET_SHFT                           0x5
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_AXIM2_HALTREQ_BMSK                              0x10
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_AXIM2_HALTREQ_SHFT                               0x4
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_AXIM_FIFO_ARESET_BMSK                            0x8
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_AXIM_FIFO_ARESET_SHFT                            0x3
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_AXIM_HALTREQ_BMSK                                0x4
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_AXIM_HALTREQ_SHFT                                0x2
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_BUSM_HALTREQ_BMSK                                0x1
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_BUSM_HALTREQ_SHFT                                0x0

#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_ADDR                                    (QDSP6V67SS_CSR_REG_BASE      + 0x0000c12c)
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_RMSK                                          0xff
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_IN          \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_ADDR, HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_RMSK)
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_ADDR, m)
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_ADDR,m,v,HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_IN)
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM2_CLAMP_E_CLEAR_BMSK                      0x80
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM2_CLAMP_E_CLEAR_SHFT                       0x7
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM2_CLAMP_E_SET_BMSK                        0x40
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM2_CLAMP_E_SET_SHFT                         0x6
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM2_CLAMP_L_CLEAR_BMSK                      0x20
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM2_CLAMP_L_CLEAR_SHFT                       0x5
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM2_CLAMP_L_SET_BMSK                        0x10
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM2_CLAMP_L_SET_SHFT                         0x4
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM_CLAMP_E_CLEAR_BMSK                        0x8
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM_CLAMP_E_CLEAR_SHFT                        0x3
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM_CLAMP_E_SET_BMSK                          0x4
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM_CLAMP_E_SET_SHFT                          0x2
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM_CLAMP_L_CLEAR_BMSK                        0x2
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM_CLAMP_L_CLEAR_SHFT                        0x1
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM_CLAMP_L_SET_BMSK                          0x1
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_AXIM_CLAMP_L_SET_SHFT                          0x0

#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_ADDR                                      (QDSP6V67SS_CSR_REG_BASE      + 0x0000c130)
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_RMSK                                             0x7
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_IN          \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_ADDR, HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_RMSK)
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_ADDR, m)
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_ADDR,m,v,HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_IN)
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_AXIM2_BMSK                                       0x4
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_AXIM2_SHFT                                       0x2
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_AXIM_BMSK                                        0x2
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_AXIM_SHFT                                        0x1
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CORE_BMSK                                        0x1
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CORE_SHFT                                        0x0

#define HWIO_QDSP6SS_RSCCTL_EN_L2MEM_RET_ADDR                                        (QDSP6V67SS_CSR_REG_BASE      + 0x0000c134)
#define HWIO_QDSP6SS_RSCCTL_EN_L2MEM_RET_RMSK                                               0xf
#define HWIO_QDSP6SS_RSCCTL_EN_L2MEM_RET_IN          \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_L2MEM_RET_ADDR, HWIO_QDSP6SS_RSCCTL_EN_L2MEM_RET_RMSK)
#define HWIO_QDSP6SS_RSCCTL_EN_L2MEM_RET_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_L2MEM_RET_ADDR, m)
#define HWIO_QDSP6SS_RSCCTL_EN_L2MEM_RET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_L2MEM_RET_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_L2MEM_RET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_RSCCTL_EN_L2MEM_RET_ADDR,m,v,HWIO_QDSP6SS_RSCCTL_EN_L2MEM_RET_IN)
#define HWIO_QDSP6SS_RSCCTL_EN_L2MEM_RET_L2DATA_BMSK                                        0xf
#define HWIO_QDSP6SS_RSCCTL_EN_L2MEM_RET_L2DATA_SHFT                                        0x0

#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_ADDR                                             (QDSP6V67SS_CSR_REG_BASE      + 0x0000c138)
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_RMSK                                                 0x7fff
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_IN          \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_PRSC_ADDR, HWIO_QDSP6SS_RSCCTL_EN_PRSC_RMSK)
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_PRSC_ADDR, m)
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_PRSC_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_RSCCTL_EN_PRSC_ADDR,m,v,HWIO_QDSP6SS_RSCCTL_EN_PRSC_IN)
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_FAL10_VETO_BMSK                                      0x4000
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_FAL10_VETO_SHFT                                         0xe
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_SPARE_BMSK                                           0x3c00
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_SPARE_SHFT                                              0xa
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_TCS_HW_TRIGGER_BMSK                                   0x3fc
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_TCS_HW_TRIGGER_SHFT                                     0x2
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_BRINGUP_REQ_BMSK                                        0x2
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_BRINGUP_REQ_SHFT                                        0x1
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_SHUTDOWN_REQ_BMSK                                       0x1
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_SHUTDOWN_REQ_SHFT                                       0x0

#define HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_ADDR                                       (QDSP6V67SS_CSR_REG_BASE      + 0x0000c140)
#define HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_RMSK                                           0x3f3f
#define HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_IN          \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_ADDR, HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_RMSK)
#define HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_ADDR, m)
#define HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_ADDR,m,v,HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_IN)
#define HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_L1S2DATA_BMSK                                  0x3f00
#define HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_L1S2DATA_SHFT                                     0x8
#define HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_L1SDATA_BMSK                                     0x3f
#define HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_L1SDATA_SHFT                                      0x0

#define HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_ADDR                                      (QDSP6V67SS_CSR_REG_BASE      + 0x0000c144)
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_RMSK                                             0x7
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_IN          \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_ADDR, HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_RMSK)
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_ADDR, m)
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_ADDR,m,v,HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_IN)
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_STAGGER_RESTORE_BMSK                             0x4
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_STAGGER_RESTORE_SHFT                             0x2
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_STAGGER_ALT_ARES_BMSK                            0x2
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_STAGGER_ALT_ARES_SHFT                            0x1
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_STAGGER_FUNC_ARES_BMSK                           0x1
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_STAGGER_FUNC_ARES_SHFT                           0x0

#define HWIO_QDSP6SS_RSCCTL_EN_VTCMEM_RET_ADDR                                       (QDSP6V67SS_CSR_REG_BASE      + 0x0000c148)
#define HWIO_QDSP6SS_RSCCTL_EN_VTCMEM_RET_RMSK                                              0x1
#define HWIO_QDSP6SS_RSCCTL_EN_VTCMEM_RET_IN          \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_VTCMEM_RET_ADDR, HWIO_QDSP6SS_RSCCTL_EN_VTCMEM_RET_RMSK)
#define HWIO_QDSP6SS_RSCCTL_EN_VTCMEM_RET_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_VTCMEM_RET_ADDR, m)
#define HWIO_QDSP6SS_RSCCTL_EN_VTCMEM_RET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_VTCMEM_RET_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_VTCMEM_RET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_RSCCTL_EN_VTCMEM_RET_ADDR,m,v,HWIO_QDSP6SS_RSCCTL_EN_VTCMEM_RET_IN)
#define HWIO_QDSP6SS_RSCCTL_EN_VTCMEM_RET_VTCM_BMSK                                         0x1
#define HWIO_QDSP6SS_RSCCTL_EN_VTCMEM_RET_VTCM_SHFT                                         0x0

#define HWIO_QDSP6SS_RSCCTL_EN_L2ITCM_RET_ADDR                                       (QDSP6V67SS_CSR_REG_BASE      + 0x0000c14c)
#define HWIO_QDSP6SS_RSCCTL_EN_L2ITCM_RET_RMSK                                              0x3
#define HWIO_QDSP6SS_RSCCTL_EN_L2ITCM_RET_IN          \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_L2ITCM_RET_ADDR, HWIO_QDSP6SS_RSCCTL_EN_L2ITCM_RET_RMSK)
#define HWIO_QDSP6SS_RSCCTL_EN_L2ITCM_RET_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_RSCCTL_EN_L2ITCM_RET_ADDR, m)
#define HWIO_QDSP6SS_RSCCTL_EN_L2ITCM_RET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_L2ITCM_RET_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_L2ITCM_RET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_RSCCTL_EN_L2ITCM_RET_ADDR,m,v,HWIO_QDSP6SS_RSCCTL_EN_L2ITCM_RET_IN)
#define HWIO_QDSP6SS_RSCCTL_EN_L2ITCM_RET_L2ITCM_BMSK                                       0x3
#define HWIO_QDSP6SS_RSCCTL_EN_L2ITCM_RET_L2ITCM_SHFT                                       0x0

#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_ADDR                                         (QDSP6V67SS_CSR_REG_BASE      + 0x0000c180)
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_RMSK                                             0xffff
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_VPE_VU_BMSK                                      0x8000
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_VPE_VU_SHFT                                         0xf
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_NRET_BMSK                                        0x4000
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_NRET_SHFT                                           0xe
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_MNOC_BMSK                                        0x2000
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_MNOC_SHFT                                           0xd
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_APB_BMSK                                         0x1000
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_APB_SHFT                                            0xc
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_AXIS2_BMSK                                        0x800
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_AXIS2_SHFT                                          0xb
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_CSI1_BMSK                                         0x400
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_CSI1_SHFT                                           0xa
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_CSI0_BMSK                                         0x200
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_CSI0_SHFT                                           0x9
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_ARCH_BMSK                                         0x100
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_ARCH_SHFT                                           0x8
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_CORE_BMSK                                          0x80
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_CORE_SHFT                                           0x7
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_AXIM2_BMSK                                         0x40
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_AXIM2_SHFT                                          0x6
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_AXIS_BMSK                                          0x20
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_AXIS_SHFT                                           0x5
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_ATBM_BMSK                                          0x10
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_ATBM_SHFT                                           0x4
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_AHBM_BMSK                                           0x8
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_AHBM_SHFT                                           0x3
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_AXIM_BMSK                                           0x4
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_AXIM_SHFT                                           0x2
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_ISDB_ETM_BMSK                                       0x2
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_ISDB_ETM_SHFT                                       0x1
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_RET_BMSK                                            0x1
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_SET_RET_SHFT                                            0x0

#define HWIO_QDSP6SS_RSCCTL_EN_CLAMP_SET_ADDR                                        (QDSP6V67SS_CSR_REG_BASE      + 0x0000c184)
#define HWIO_QDSP6SS_RSCCTL_EN_CLAMP_SET_RMSK                                               0x3
#define HWIO_QDSP6SS_RSCCTL_EN_CLAMP_SET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_CLAMP_SET_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_CLAMP_SET_CLAMP_MSS2CX_BMSK                                  0x2
#define HWIO_QDSP6SS_RSCCTL_EN_CLAMP_SET_CLAMP_MSS2CX_SHFT                                  0x1
#define HWIO_QDSP6SS_RSCCTL_EN_CLAMP_SET_CLAMP_IO_BMSK                                      0x1
#define HWIO_QDSP6SS_RSCCTL_EN_CLAMP_SET_CLAMP_IO_SHFT                                      0x0

#define HWIO_QDSP6SS_RSCCTL_EN_CLK_SET_ADDR                                          (QDSP6V67SS_CSR_REG_BASE      + 0x0000c188)
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_SET_RMSK                                               0x7ff
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_SET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_CLK_SET_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_SET_VPE_VU_BMSK                                        0x400
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_SET_VPE_VU_SHFT                                          0xa
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_SET_AXIS2_BMSK                                         0x200
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_SET_AXIS2_SHFT                                           0x9
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_SET_CSI1_BMSK                                          0x100
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_SET_CSI1_SHFT                                            0x8
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_SET_CSI0_BMSK                                           0x80
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_SET_CSI0_SHFT                                            0x7
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_SET_AXIM2_BMSK                                          0x40
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_SET_AXIM2_SHFT                                           0x6
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_SET_AXIS_BMSK                                           0x20
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_SET_AXIS_SHFT                                            0x5
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_SET_ATBM_BMSK                                           0x10
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_SET_ATBM_SHFT                                            0x4
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_SET_AHBM_BMSK                                            0x8
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_SET_AHBM_SHFT                                            0x3
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_SET_AXIM_BMSK                                            0x4
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_SET_AXIM_SHFT                                            0x2
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_SET_APB_BMSK                                             0x2
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_SET_APB_SHFT                                             0x1
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_SET_CORE_BMSK                                            0x1
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_SET_CORE_SHFT                                            0x0

#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_ADDR                                      (QDSP6V67SS_CSR_REG_BASE      + 0x0000c18c)
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_RMSK                                          0xffff
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_SPARE_BMSK                                    0xc000
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_SPARE_SHFT                                       0xe
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L2CLADE_BMSK                                  0x2000
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L2CLADE_SHFT                                     0xd
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L2PLRU_BMSK                                   0x1000
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L2PLRU_SHFT                                      0xc
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L2TAG_BMSK                                     0x800
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L2TAG_SHFT                                       0xb
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L2STBUF_BMSK                                   0x400
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L2STBUF_SHFT                                     0xa
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1DU_TAG_BMSK                                  0x200
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1DU_TAG_SHFT                                    0x9
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1DU_BMSK                                      0x100
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1DU_SHFT                                        0x8
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_JU_TLB64_BMSK                                   0x80
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_JU_TLB64_SHFT                                    0x7
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_JU_TLB128_BMSK                                  0x40
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_JU_TLB128_SHFT                                   0x6
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_JU_CAM_BMSK                                     0x20
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_JU_CAM_SHFT                                      0x5
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_ETB_BMSK                                        0x10
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_ETB_SHFT                                         0x4
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1IU_TAG_BMSK                                    0x8
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1IU_TAG_SHFT                                    0x3
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1IU_PDEC_BMSK                                   0x4
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1IU_PDEC_SHFT                                   0x2
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1IU_BMSK                                        0x2
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1IU_SHFT                                        0x1
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1IU_BTB_BMSK                                    0x1
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_SET_L1IU_BTB_SHFT                                    0x0

#define HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_SET_ADDR                                      (QDSP6V67SS_CSR_REG_BASE      + 0x0000c190)
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_SET_RMSK                                             0xa
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_SET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_SET_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_SET_CLAMP_QMC_MEM_BMSK                               0x8
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_SET_CLAMP_QMC_MEM_SHFT                               0x3
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_SET_MEM_PERIPH_BMSK                                  0x2
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_SET_MEM_PERIPH_SHFT                                  0x1

#define HWIO_QDSP6SS_RSCCTL_EN_BHS_SET_ADDR                                          (QDSP6V67SS_CSR_REG_BASE      + 0x0000c194)
#define HWIO_QDSP6SS_RSCCTL_EN_BHS_SET_RMSK                                                 0x1
#define HWIO_QDSP6SS_RSCCTL_EN_BHS_SET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_BHS_SET_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_BHS_SET_BHS_BMSK                                             0x1
#define HWIO_QDSP6SS_RSCCTL_EN_BHS_SET_BHS_SHFT                                             0x0

#define HWIO_QDSP6SS_RSCCTL_EN_LDO_SET_ADDR                                          (QDSP6V67SS_CSR_REG_BASE      + 0x0000c198)
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_SET_RMSK                                                0x7f
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_SET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_LDO_SET_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_SET_BG_OFF_BMSK                                         0x40
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_SET_BG_OFF_SHFT                                          0x6
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_SET_FORCEOFF_BMSK                                       0x20
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_SET_FORCEOFF_SHFT                                        0x5
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_SET_SPARE_BITS4_2_BMSK                                  0x1c
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_SET_SPARE_BITS4_2_SHFT                                   0x2
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_SET_UPDATE_BMSK                                          0x2
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_SET_UPDATE_SHFT                                          0x1
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_SET_BYPASS_OPEN_BMSK                                     0x1
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_SET_BYPASS_OPEN_SHFT                                     0x0

#define HWIO_QDSP6SS_RSCCTL_EN_PLL_SET_ADDR                                          (QDSP6V67SS_CSR_REG_BASE      + 0x0000c1a0)
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_SET_RMSK                                                0x17
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_SET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_PLL_SET_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_SET_PLL_STBY_BMSK                                       0x10
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_SET_PLL_STBY_SHFT                                        0x4
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_SET_PLL_OUTDIS_BMSK                                      0x4
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_SET_PLL_OUTDIS_SHFT                                      0x2
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_SET_PLL_OPMODE1_BMSK                                     0x2
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_SET_PLL_OPMODE1_SHFT                                     0x1
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_SET_PLL_RESET_BMSK                                       0x1
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_SET_PLL_RESET_SHFT                                       0x0

#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_SET_ADDR                                     (QDSP6V67SS_CSR_REG_BASE      + 0x0000c1a4)
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_SET_RMSK                                            0xf
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_SET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_STATERET_SET_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_SET_PROF_IRQ_BMSK                                   0x8
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_SET_PROF_IRQ_SHFT                                   0x3
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_SET_WAKE_IRQ_BMSK                                   0x4
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_SET_WAKE_IRQ_SHFT                                   0x2
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_SET_RESTORE_BMSK                                    0x2
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_SET_RESTORE_SHFT                                    0x1
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_SET_SAVE_BMSK                                       0x1
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_SET_SAVE_SHFT                                       0x0

#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_ADDR                                  (QDSP6V67SS_CSR_REG_BASE      + 0x0000c1a8)
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_RMSK                                        0x3d
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_AXIM2_FIFO_ARESET_BMSK                      0x20
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_AXIM2_FIFO_ARESET_SHFT                       0x5
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_AXIM2_HALTREQ_BMSK                          0x10
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_AXIM2_HALTREQ_SHFT                           0x4
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_AXIM_FIFO_ARESET_BMSK                        0x8
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_AXIM_FIFO_ARESET_SHFT                        0x3
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_AXIM_HALTREQ_BMSK                            0x4
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_AXIM_HALTREQ_SHFT                            0x2
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_BUSM_HALTREQ_BMSK                            0x1
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_SET_BUSM_HALTREQ_SHFT                            0x0

#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_ADDR                                (QDSP6V67SS_CSR_REG_BASE      + 0x0000c1ac)
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_RMSK                                      0xff
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM2_CLAMP_E_CLEAR_BMSK                  0x80
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM2_CLAMP_E_CLEAR_SHFT                   0x7
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM2_CLAMP_E_SET_BMSK                    0x40
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM2_CLAMP_E_SET_SHFT                     0x6
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM2_CLAMP_L_CLEAR_BMSK                  0x20
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM2_CLAMP_L_CLEAR_SHFT                   0x5
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM2_CLAMP_L_SET_BMSK                    0x10
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM2_CLAMP_L_SET_SHFT                     0x4
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM_CLAMP_E_CLEAR_BMSK                    0x8
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM_CLAMP_E_CLEAR_SHFT                    0x3
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM_CLAMP_E_SET_BMSK                      0x4
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM_CLAMP_E_SET_SHFT                      0x2
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM_CLAMP_L_CLEAR_BMSK                    0x2
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM_CLAMP_L_CLEAR_SHFT                    0x1
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM_CLAMP_L_SET_BMSK                      0x1
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_SET_AXIM_CLAMP_L_SET_SHFT                      0x0

#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_SET_ADDR                                  (QDSP6V67SS_CSR_REG_BASE      + 0x0000c1b0)
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_SET_RMSK                                         0x7
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_SET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_SET_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_SET_AXIM2_BMSK                                   0x4
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_SET_AXIM2_SHFT                                   0x2
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_SET_AXIM_BMSK                                    0x2
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_SET_AXIM_SHFT                                    0x1
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_SET_CORE_BMSK                                    0x1
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_SET_CORE_SHFT                                    0x0

#define HWIO_QDSP6SS_RSCCTL_EN_L2MEM_RET_SET_ADDR                                    (QDSP6V67SS_CSR_REG_BASE      + 0x0000c1b4)
#define HWIO_QDSP6SS_RSCCTL_EN_L2MEM_RET_SET_RMSK                                           0xf
#define HWIO_QDSP6SS_RSCCTL_EN_L2MEM_RET_SET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_L2MEM_RET_SET_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_L2MEM_RET_SET_L2DATA_BMSK                                    0xf
#define HWIO_QDSP6SS_RSCCTL_EN_L2MEM_RET_SET_L2DATA_SHFT                                    0x0

#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_SET_ADDR                                         (QDSP6V67SS_CSR_REG_BASE      + 0x0000c1b8)
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_SET_RMSK                                             0x7fff
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_SET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_PRSC_SET_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_SET_FAL10_VETO_BMSK                                  0x4000
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_SET_FAL10_VETO_SHFT                                     0xe
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_SET_SPARE_BMSK                                       0x3c00
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_SET_SPARE_SHFT                                          0xa
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_SET_TCS_HW_TRIGGER_BMSK                               0x3fc
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_SET_TCS_HW_TRIGGER_SHFT                                 0x2
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_SET_BRINGUP_REQ_BMSK                                    0x2
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_SET_BRINGUP_REQ_SHFT                                    0x1
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_SET_SHUTDOWN_REQ_BMSK                                   0x1
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_SET_SHUTDOWN_REQ_SHFT                                   0x0

#define HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_SET_ADDR                                   (QDSP6V67SS_CSR_REG_BASE      + 0x0000c1c0)
#define HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_SET_RMSK                                       0x3f3f
#define HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_SET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_SET_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_SET_L1S2DATA_BMSK                              0x3f00
#define HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_SET_L1S2DATA_SHFT                                 0x8
#define HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_SET_L1SDATA_BMSK                                 0x3f
#define HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_SET_L1SDATA_SHFT                                  0x0

#define HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_SET_ADDR                                  (QDSP6V67SS_CSR_REG_BASE      + 0x0000c1c4)
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_SET_RMSK                                         0x7
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_SET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_SET_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_SET_STAGGER_RESTORE_BMSK                         0x4
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_SET_STAGGER_RESTORE_SHFT                         0x2
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_SET_STAGGER_ALT_ARES_BMSK                        0x2
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_SET_STAGGER_ALT_ARES_SHFT                        0x1
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_SET_STAGGER_FUNC_ARES_BMSK                       0x1
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_SET_STAGGER_FUNC_ARES_SHFT                       0x0

#define HWIO_QDSP6SS_RSCCTL_EN_VTCMEM_RET_SET_ADDR                                   (QDSP6V67SS_CSR_REG_BASE      + 0x0000c1c8)
#define HWIO_QDSP6SS_RSCCTL_EN_VTCMEM_RET_SET_RMSK                                          0x1
#define HWIO_QDSP6SS_RSCCTL_EN_VTCMEM_RET_SET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_VTCMEM_RET_SET_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_VTCMEM_RET_SET_VTCM_BMSK                                     0x1
#define HWIO_QDSP6SS_RSCCTL_EN_VTCMEM_RET_SET_VTCM_SHFT                                     0x0

#define HWIO_QDSP6SS_RSCCTL_EN_L2ITCM_RET_SET_ADDR                                   (QDSP6V67SS_CSR_REG_BASE      + 0x0000c1cc)
#define HWIO_QDSP6SS_RSCCTL_EN_L2ITCM_RET_SET_RMSK                                          0x3
#define HWIO_QDSP6SS_RSCCTL_EN_L2ITCM_RET_SET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_L2ITCM_RET_SET_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_L2ITCM_RET_SET_L2ITCM_BMSK                                   0x3
#define HWIO_QDSP6SS_RSCCTL_EN_L2ITCM_RET_SET_L2ITCM_SHFT                                   0x0

#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_ADDR                                       (QDSP6V67SS_CSR_REG_BASE      + 0x0000c200)
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_RMSK                                           0xffff
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_VPE_VU_BMSK                                    0x8000
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_VPE_VU_SHFT                                       0xf
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_NRET_BMSK                                      0x4000
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_NRET_SHFT                                         0xe
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_MNOC_BMSK                                      0x2000
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_MNOC_SHFT                                         0xd
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_APB_BMSK                                       0x1000
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_APB_SHFT                                          0xc
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_AXIS2_BMSK                                      0x800
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_AXIS2_SHFT                                        0xb
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_CSI1_BMSK                                       0x400
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_CSI1_SHFT                                         0xa
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_CSI0_BMSK                                       0x200
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_CSI0_SHFT                                         0x9
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_ARCH_BMSK                                       0x100
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_ARCH_SHFT                                         0x8
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_CORE_BMSK                                        0x80
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_CORE_SHFT                                         0x7
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_AXIM2_BMSK                                       0x40
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_AXIM2_SHFT                                        0x6
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_AXIS_BMSK                                        0x20
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_AXIS_SHFT                                         0x5
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_ATBM_BMSK                                        0x10
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_ATBM_SHFT                                         0x4
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_AHBM_BMSK                                         0x8
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_AHBM_SHFT                                         0x3
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_AXIM_BMSK                                         0x4
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_AXIM_SHFT                                         0x2
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_ISDB_ETM_BMSK                                     0x2
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_ISDB_ETM_SHFT                                     0x1
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_RET_BMSK                                          0x1
#define HWIO_QDSP6SS_RSCCTL_EN_ARES_CLEAR_RET_SHFT                                          0x0

#define HWIO_QDSP6SS_RSCCTL_EN_CLAMP_CLEAR_ADDR                                      (QDSP6V67SS_CSR_REG_BASE      + 0x0000c204)
#define HWIO_QDSP6SS_RSCCTL_EN_CLAMP_CLEAR_RMSK                                             0x3
#define HWIO_QDSP6SS_RSCCTL_EN_CLAMP_CLEAR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_CLAMP_CLEAR_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_CLAMP_CLEAR_CLAMP_MSS2CX_BMSK                                0x2
#define HWIO_QDSP6SS_RSCCTL_EN_CLAMP_CLEAR_CLAMP_MSS2CX_SHFT                                0x1
#define HWIO_QDSP6SS_RSCCTL_EN_CLAMP_CLEAR_CLAMP_IO_BMSK                                    0x1
#define HWIO_QDSP6SS_RSCCTL_EN_CLAMP_CLEAR_CLAMP_IO_SHFT                                    0x0

#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CLEAR_ADDR                                        (QDSP6V67SS_CSR_REG_BASE      + 0x0000c208)
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CLEAR_RMSK                                             0x7ff
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CLEAR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_CLK_CLEAR_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CLEAR_VPE_VU_BMSK                                      0x400
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CLEAR_VPE_VU_SHFT                                        0xa
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CLEAR_AXIS2_BMSK                                       0x200
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CLEAR_AXIS2_SHFT                                         0x9
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CLEAR_CSI1_BMSK                                        0x100
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CLEAR_CSI1_SHFT                                          0x8
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CLEAR_CSI0_BMSK                                         0x80
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CLEAR_CSI0_SHFT                                          0x7
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CLEAR_AXIM2_BMSK                                        0x40
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CLEAR_AXIM2_SHFT                                         0x6
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CLEAR_AXIS_BMSK                                         0x20
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CLEAR_AXIS_SHFT                                          0x5
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CLEAR_ATBM_BMSK                                         0x10
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CLEAR_ATBM_SHFT                                          0x4
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CLEAR_AHBM_BMSK                                          0x8
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CLEAR_AHBM_SHFT                                          0x3
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CLEAR_AXIM_BMSK                                          0x4
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CLEAR_AXIM_SHFT                                          0x2
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CLEAR_APB_BMSK                                           0x2
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CLEAR_APB_SHFT                                           0x1
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CLEAR_CORE_BMSK                                          0x1
#define HWIO_QDSP6SS_RSCCTL_EN_CLK_CLEAR_CORE_SHFT                                          0x0

#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_ADDR                                    (QDSP6V67SS_CSR_REG_BASE      + 0x0000c20c)
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_RMSK                                        0xffff
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_SPARE_BMSK                                  0xc000
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_SPARE_SHFT                                     0xe
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L2CLADE_BMSK                                0x2000
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L2CLADE_SHFT                                   0xd
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L2PLRU_BMSK                                 0x1000
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L2PLRU_SHFT                                    0xc
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L2TAG_BMSK                                   0x800
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L2TAG_SHFT                                     0xb
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L2STBUF_BMSK                                 0x400
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L2STBUF_SHFT                                   0xa
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1DU_TAG_BMSK                                0x200
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1DU_TAG_SHFT                                  0x9
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1DU_BMSK                                    0x100
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1DU_SHFT                                      0x8
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_JU_TLB64_BMSK                                 0x80
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_JU_TLB64_SHFT                                  0x7
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_JU_TLB128_BMSK                                0x40
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_JU_TLB128_SHFT                                 0x6
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_JU_CAM_BMSK                                   0x20
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_JU_CAM_SHFT                                    0x5
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_ETB_BMSK                                      0x10
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_ETB_SHFT                                       0x4
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1IU_TAG_BMSK                                  0x8
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1IU_TAG_SHFT                                  0x3
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1IU_PDEC_BMSK                                 0x4
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1IU_PDEC_SHFT                                 0x2
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1IU_BMSK                                      0x2
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1IU_SHFT                                      0x1
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1IU_BTB_BMSK                                  0x1
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_RET_CLEAR_L1IU_BTB_SHFT                                  0x0

#define HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_CLEAR_ADDR                                    (QDSP6V67SS_CSR_REG_BASE      + 0x0000c210)
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_CLEAR_RMSK                                           0xa
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_CLEAR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_CLEAR_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_CLEAR_CLAMP_QMC_MEM_BMSK                             0x8
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_CLEAR_CLAMP_QMC_MEM_SHFT                             0x3
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_CLEAR_MEM_PERIPH_BMSK                                0x2
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_CTL_CLEAR_MEM_PERIPH_SHFT                                0x1

#define HWIO_QDSP6SS_RSCCTL_EN_BHS_CLEAR_ADDR                                        (QDSP6V67SS_CSR_REG_BASE      + 0x0000c214)
#define HWIO_QDSP6SS_RSCCTL_EN_BHS_CLEAR_RMSK                                               0x1
#define HWIO_QDSP6SS_RSCCTL_EN_BHS_CLEAR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_BHS_CLEAR_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_BHS_CLEAR_BHS_BMSK                                           0x1
#define HWIO_QDSP6SS_RSCCTL_EN_BHS_CLEAR_BHS_SHFT                                           0x0

#define HWIO_QDSP6SS_RSCCTL_EN_LDO_CLEAR_ADDR                                        (QDSP6V67SS_CSR_REG_BASE      + 0x0000c218)
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_CLEAR_RMSK                                              0x7f
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_CLEAR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_LDO_CLEAR_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_CLEAR_BG_OFF_BMSK                                       0x40
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_CLEAR_BG_OFF_SHFT                                        0x6
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_CLEAR_FORCEOFF_BMSK                                     0x20
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_CLEAR_FORCEOFF_SHFT                                      0x5
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_CLEAR_SPARE_BITS4_2_BMSK                                0x1c
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_CLEAR_SPARE_BITS4_2_SHFT                                 0x2
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_CLEAR_UPDATE_BMSK                                        0x2
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_CLEAR_UPDATE_SHFT                                        0x1
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_CLEAR_BYPASS_OPEN_BMSK                                   0x1
#define HWIO_QDSP6SS_RSCCTL_EN_LDO_CLEAR_BYPASS_OPEN_SHFT                                   0x0

#define HWIO_QDSP6SS_RSCCTL_EN_PLL_CLEAR_ADDR                                        (QDSP6V67SS_CSR_REG_BASE      + 0x0000c220)
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_CLEAR_RMSK                                              0x17
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_CLEAR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_PLL_CLEAR_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_CLEAR_PLL_STBY_BMSK                                     0x10
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_CLEAR_PLL_STBY_SHFT                                      0x4
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_CLEAR_PLL_OUTDIS_BMSK                                    0x4
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_CLEAR_PLL_OUTDIS_SHFT                                    0x2
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_CLEAR_PLL_OPMODE1_BMSK                                   0x2
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_CLEAR_PLL_OPMODE1_SHFT                                   0x1
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_CLEAR_PLL_RESET_BMSK                                     0x1
#define HWIO_QDSP6SS_RSCCTL_EN_PLL_CLEAR_PLL_RESET_SHFT                                     0x0

#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_ADDR                                   (QDSP6V67SS_CSR_REG_BASE      + 0x0000c224)
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_RMSK                                          0xf
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_PROF_IRQ_BMSK                                 0x8
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_PROF_IRQ_SHFT                                 0x3
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_WAKE_IRQ_BMSK                                 0x4
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_WAKE_IRQ_SHFT                                 0x2
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_RESTORE_BMSK                                  0x2
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_RESTORE_SHFT                                  0x1
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_SAVE_BMSK                                     0x1
#define HWIO_QDSP6SS_RSCCTL_EN_STATERET_CLEAR_SAVE_SHFT                                     0x0

#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_ADDR                                (QDSP6V67SS_CSR_REG_BASE      + 0x0000c228)
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_RMSK                                      0x3d
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_AXIM2_FIFO_ARESET_BMSK                    0x20
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_AXIM2_FIFO_ARESET_SHFT                     0x5
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_AXIM2_HALTREQ_BMSK                        0x10
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_AXIM2_HALTREQ_SHFT                         0x4
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_AXIM_FIFO_ARESET_BMSK                      0x8
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_AXIM_FIFO_ARESET_SHFT                      0x3
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_AXIM_HALTREQ_BMSK                          0x4
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_AXIM_HALTREQ_SHFT                          0x2
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_BUSM_HALTREQ_BMSK                          0x1
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CTL_CLEAR_BUSM_HALTREQ_SHFT                          0x0

#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_ADDR                              (QDSP6V67SS_CSR_REG_BASE      + 0x0000c22c)
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_RMSK                                    0xff
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM2_CLAMP_E_CLEAR_BMSK                0x80
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM2_CLAMP_E_CLEAR_SHFT                 0x7
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM2_CLAMP_E_SET_BMSK                  0x40
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM2_CLAMP_E_SET_SHFT                   0x6
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM2_CLAMP_L_CLEAR_BMSK                0x20
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM2_CLAMP_L_CLEAR_SHFT                 0x5
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM2_CLAMP_L_SET_BMSK                  0x10
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM2_CLAMP_L_SET_SHFT                   0x4
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM_CLAMP_E_CLEAR_BMSK                  0x8
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM_CLAMP_E_CLEAR_SHFT                  0x3
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM_CLAMP_E_SET_BMSK                    0x4
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM_CLAMP_E_SET_SHFT                    0x2
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM_CLAMP_L_CLEAR_BMSK                  0x2
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM_CLAMP_L_CLEAR_SHFT                  0x1
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM_CLAMP_L_SET_BMSK                    0x1
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLAMP_CLEAR_AXIM_CLAMP_L_SET_SHFT                    0x0

#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CLEAR_ADDR                                (QDSP6V67SS_CSR_REG_BASE      + 0x0000c230)
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CLEAR_RMSK                                       0x7
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CLEAR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CLEAR_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CLEAR_AXIM2_BMSK                                 0x4
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CLEAR_AXIM2_SHFT                                 0x2
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CLEAR_AXIM_BMSK                                  0x2
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CLEAR_AXIM_SHFT                                  0x1
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CLEAR_CORE_BMSK                                  0x1
#define HWIO_QDSP6SS_RSCCTL_EN_ISOINTF_CLK_CLEAR_CORE_SHFT                                  0x0

#define HWIO_QDSP6SS_RSCCTL_EN_L2MEM_RET_CLEAR_ADDR                                  (QDSP6V67SS_CSR_REG_BASE      + 0x0000c234)
#define HWIO_QDSP6SS_RSCCTL_EN_L2MEM_RET_CLEAR_RMSK                                         0xf
#define HWIO_QDSP6SS_RSCCTL_EN_L2MEM_RET_CLEAR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_L2MEM_RET_CLEAR_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_L2MEM_RET_CLEAR_L2DATA_BMSK                                  0xf
#define HWIO_QDSP6SS_RSCCTL_EN_L2MEM_RET_CLEAR_L2DATA_SHFT                                  0x0

#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_ADDR                                       (QDSP6V67SS_CSR_REG_BASE      + 0x0000c238)
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_RMSK                                           0x7fff
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_FAL10_VETO_BMSK                                0x4000
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_FAL10_VETO_SHFT                                   0xe
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_SPARE_BMSK                                     0x3c00
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_SPARE_SHFT                                        0xa
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_TCS_HW_TRIGGER_BMSK                             0x3fc
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_TCS_HW_TRIGGER_SHFT                               0x2
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_BRINGUP_REQ_BMSK                                  0x2
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_BRINGUP_REQ_SHFT                                  0x1
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_SHUTDOWN_REQ_BMSK                                 0x1
#define HWIO_QDSP6SS_RSCCTL_EN_PRSC_CLEAR_SHUTDOWN_REQ_SHFT                                 0x0

#define HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_CLEAR_ADDR                                 (QDSP6V67SS_CSR_REG_BASE      + 0x0000c240)
#define HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_CLEAR_RMSK                                     0x3f3f
#define HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_CLEAR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_CLEAR_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_CLEAR_L1S2DATA_BMSK                            0x3f00
#define HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_CLEAR_L1S2DATA_SHFT                               0x8
#define HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_CLEAR_L1SDATA_BMSK                               0x3f
#define HWIO_QDSP6SS_RSCCTL_EN_L1SMEM_RET_CLEAR_L1SDATA_SHFT                                0x0

#define HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_CLEAR_ADDR                                (QDSP6V67SS_CSR_REG_BASE      + 0x0000c244)
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_CLEAR_RMSK                                       0x7
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_CLEAR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_CLEAR_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_CLEAR_STAGGER_RESTORE_BMSK                       0x4
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_CLEAR_STAGGER_RESTORE_SHFT                       0x2
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_CLEAR_STAGGER_ALT_ARES_BMSK                      0x2
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_CLEAR_STAGGER_ALT_ARES_SHFT                      0x1
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_CLEAR_STAGGER_FUNC_ARES_BMSK                     0x1
#define HWIO_QDSP6SS_RSCCTL_EN_MEM_STAGGER_CLEAR_STAGGER_FUNC_ARES_SHFT                     0x0

#define HWIO_QDSP6SS_RSCCTL_EN_VTCMEM_RET_CLEAR_ADDR                                 (QDSP6V67SS_CSR_REG_BASE      + 0x0000c248)
#define HWIO_QDSP6SS_RSCCTL_EN_VTCMEM_RET_CLEAR_RMSK                                        0x1
#define HWIO_QDSP6SS_RSCCTL_EN_VTCMEM_RET_CLEAR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_VTCMEM_RET_CLEAR_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_VTCMEM_RET_CLEAR_VTCM_BMSK                                   0x1
#define HWIO_QDSP6SS_RSCCTL_EN_VTCMEM_RET_CLEAR_VTCM_SHFT                                   0x0

#define HWIO_QDSP6SS_RSCCTL_EN_L2ITCM_RET_CLEAR_ADDR                                 (QDSP6V67SS_CSR_REG_BASE      + 0x0000c24c)
#define HWIO_QDSP6SS_RSCCTL_EN_L2ITCM_RET_CLEAR_RMSK                                        0x3
#define HWIO_QDSP6SS_RSCCTL_EN_L2ITCM_RET_CLEAR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RSCCTL_EN_L2ITCM_RET_CLEAR_ADDR,v)
#define HWIO_QDSP6SS_RSCCTL_EN_L2ITCM_RET_CLEAR_L2ITCM_BMSK                                 0x3
#define HWIO_QDSP6SS_RSCCTL_EN_L2ITCM_RET_CLEAR_L2ITCM_SHFT                                 0x0

/*----------------------------------------------------------------------------
 * MODULE: QDSP6V67SS_PUB
 *--------------------------------------------------------------------------*/

#define QDSP6V67SS_PUB_REG_BASE                                               (QDSP6V67SS_BASE      + 0x00000000)
#define QDSP6V67SS_PUB_REG_BASE_SIZE                                          0x10000
#define QDSP6V67SS_PUB_REG_BASE_USED                                          0x2030

#define HWIO_QDSP6SS_VERSION_ADDR                                             (QDSP6V67SS_PUB_REG_BASE      + 0x00000000)
#define HWIO_QDSP6SS_VERSION_RMSK                                             0xffffffff
#define HWIO_QDSP6SS_VERSION_IN          \
        in_dword_masked(HWIO_QDSP6SS_VERSION_ADDR, HWIO_QDSP6SS_VERSION_RMSK)
#define HWIO_QDSP6SS_VERSION_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_VERSION_ADDR, m)
#define HWIO_QDSP6SS_VERSION_MAJOR_BMSK                                       0xf0000000
#define HWIO_QDSP6SS_VERSION_MAJOR_SHFT                                             0x1c
#define HWIO_QDSP6SS_VERSION_MINOR_BMSK                                        0xfff0000
#define HWIO_QDSP6SS_VERSION_MINOR_SHFT                                             0x10
#define HWIO_QDSP6SS_VERSION_STEP_BMSK                                            0xffff
#define HWIO_QDSP6SS_VERSION_STEP_SHFT                                               0x0

#define HWIO_QDSP6SS_RST_EVB_ADDR                                             (QDSP6V67SS_PUB_REG_BASE      + 0x00000010)
#define HWIO_QDSP6SS_RST_EVB_RMSK                                              0xffffff0
#define HWIO_QDSP6SS_RST_EVB_IN          \
        in_dword_masked(HWIO_QDSP6SS_RST_EVB_ADDR, HWIO_QDSP6SS_RST_EVB_RMSK)
#define HWIO_QDSP6SS_RST_EVB_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_RST_EVB_ADDR, m)
#define HWIO_QDSP6SS_RST_EVB_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RST_EVB_ADDR,v)
#define HWIO_QDSP6SS_RST_EVB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_RST_EVB_ADDR,m,v,HWIO_QDSP6SS_RST_EVB_IN)
#define HWIO_QDSP6SS_RST_EVB_EVB_BMSK                                          0xffffff0
#define HWIO_QDSP6SS_RST_EVB_EVB_SHFT                                                0x4

#define HWIO_QDSP6SS_DBG_CFG_ADDR                                             (QDSP6V67SS_PUB_REG_BASE      + 0x00000018)
#define HWIO_QDSP6SS_DBG_CFG_RMSK                                             0xffff0002
#define HWIO_QDSP6SS_DBG_CFG_IN          \
        in_dword_masked(HWIO_QDSP6SS_DBG_CFG_ADDR, HWIO_QDSP6SS_DBG_CFG_RMSK)
#define HWIO_QDSP6SS_DBG_CFG_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_DBG_CFG_ADDR, m)
#define HWIO_QDSP6SS_DBG_CFG_OUT(v)      \
        out_dword(HWIO_QDSP6SS_DBG_CFG_ADDR,v)
#define HWIO_QDSP6SS_DBG_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_DBG_CFG_ADDR,m,v,HWIO_QDSP6SS_DBG_CFG_IN)
#define HWIO_QDSP6SS_DBG_CFG_DBG_SW_REG_BMSK                                  0xff000000
#define HWIO_QDSP6SS_DBG_CFG_DBG_SW_REG_SHFT                                        0x18
#define HWIO_QDSP6SS_DBG_CFG_DBG_SPARE_BMSK                                     0xff0000
#define HWIO_QDSP6SS_DBG_CFG_DBG_SPARE_SHFT                                         0x10
#define HWIO_QDSP6SS_DBG_CFG_CTI_TIHS_ENA_BMSK                                       0x2
#define HWIO_QDSP6SS_DBG_CFG_CTI_TIHS_ENA_SHFT                                       0x1

#define HWIO_QDSP6SS_RET_CFG_ADDR                                             (QDSP6V67SS_PUB_REG_BASE      + 0x0000001c)
#define HWIO_QDSP6SS_RET_CFG_RMSK                                                    0x3
#define HWIO_QDSP6SS_RET_CFG_IN          \
        in_dword_masked(HWIO_QDSP6SS_RET_CFG_ADDR, HWIO_QDSP6SS_RET_CFG_RMSK)
#define HWIO_QDSP6SS_RET_CFG_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_RET_CFG_ADDR, m)
#define HWIO_QDSP6SS_RET_CFG_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RET_CFG_ADDR,v)
#define HWIO_QDSP6SS_RET_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_RET_CFG_ADDR,m,v,HWIO_QDSP6SS_RET_CFG_IN)
#define HWIO_QDSP6SS_RET_CFG_NRET_ARES_ENA_BMSK                                      0x2
#define HWIO_QDSP6SS_RET_CFG_NRET_ARES_ENA_SHFT                                      0x1
#define HWIO_QDSP6SS_RET_CFG_RET_ARES_ENA_BMSK                                       0x1
#define HWIO_QDSP6SS_RET_CFG_RET_ARES_ENA_SHFT                                       0x0

#define HWIO_QDSP6SS_CORE_CBCR_ADDR                                           (QDSP6V67SS_PUB_REG_BASE      + 0x00000020)
#define HWIO_QDSP6SS_CORE_CBCR_RMSK                                           0x80000001
#define HWIO_QDSP6SS_CORE_CBCR_IN          \
        in_dword_masked(HWIO_QDSP6SS_CORE_CBCR_ADDR, HWIO_QDSP6SS_CORE_CBCR_RMSK)
#define HWIO_QDSP6SS_CORE_CBCR_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_CORE_CBCR_ADDR, m)
#define HWIO_QDSP6SS_CORE_CBCR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_CORE_CBCR_ADDR,v)
#define HWIO_QDSP6SS_CORE_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_CORE_CBCR_ADDR,m,v,HWIO_QDSP6SS_CORE_CBCR_IN)
#define HWIO_QDSP6SS_CORE_CBCR_CLKOFF_BMSK                                    0x80000000
#define HWIO_QDSP6SS_CORE_CBCR_CLKOFF_SHFT                                          0x1f
#define HWIO_QDSP6SS_CORE_CBCR_CLKEN_BMSK                                            0x1
#define HWIO_QDSP6SS_CORE_CBCR_CLKEN_SHFT                                            0x0

#define HWIO_QDSP6SS_CORE_CMD_RCGR_ADDR                                       (QDSP6V67SS_PUB_REG_BASE      + 0x00000028)
#define HWIO_QDSP6SS_CORE_CMD_RCGR_RMSK                                       0x80000013
#define HWIO_QDSP6SS_CORE_CMD_RCGR_IN          \
        in_dword_masked(HWIO_QDSP6SS_CORE_CMD_RCGR_ADDR, HWIO_QDSP6SS_CORE_CMD_RCGR_RMSK)
#define HWIO_QDSP6SS_CORE_CMD_RCGR_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_CORE_CMD_RCGR_ADDR, m)
#define HWIO_QDSP6SS_CORE_CMD_RCGR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_CORE_CMD_RCGR_ADDR,v)
#define HWIO_QDSP6SS_CORE_CMD_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_CORE_CMD_RCGR_ADDR,m,v,HWIO_QDSP6SS_CORE_CMD_RCGR_IN)
#define HWIO_QDSP6SS_CORE_CMD_RCGR_ROOT_OFF_BMSK                              0x80000000
#define HWIO_QDSP6SS_CORE_CMD_RCGR_ROOT_OFF_SHFT                                    0x1f
#define HWIO_QDSP6SS_CORE_CMD_RCGR_DIRTY_CFG_RCGR_BMSK                              0x10
#define HWIO_QDSP6SS_CORE_CMD_RCGR_DIRTY_CFG_RCGR_SHFT                               0x4
#define HWIO_QDSP6SS_CORE_CMD_RCGR_ROOT_EN_BMSK                                      0x2
#define HWIO_QDSP6SS_CORE_CMD_RCGR_ROOT_EN_SHFT                                      0x1
#define HWIO_QDSP6SS_CORE_CMD_RCGR_UPDATE_BMSK                                       0x1
#define HWIO_QDSP6SS_CORE_CMD_RCGR_UPDATE_SHFT                                       0x0

#define HWIO_QDSP6SS_CORE_CFG_RCGR_ADDR                                       (QDSP6V67SS_PUB_REG_BASE      + 0x0000002c)
#define HWIO_QDSP6SS_CORE_CFG_RCGR_RMSK                                         0x11071f
#define HWIO_QDSP6SS_CORE_CFG_RCGR_IN          \
        in_dword_masked(HWIO_QDSP6SS_CORE_CFG_RCGR_ADDR, HWIO_QDSP6SS_CORE_CFG_RCGR_RMSK)
#define HWIO_QDSP6SS_CORE_CFG_RCGR_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_CORE_CFG_RCGR_ADDR, m)
#define HWIO_QDSP6SS_CORE_CFG_RCGR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_CORE_CFG_RCGR_ADDR,v)
#define HWIO_QDSP6SS_CORE_CFG_RCGR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_CORE_CFG_RCGR_ADDR,m,v,HWIO_QDSP6SS_CORE_CFG_RCGR_IN)
#define HWIO_QDSP6SS_CORE_CFG_RCGR_HW_CLK_CONTROL_BMSK                          0x100000
#define HWIO_QDSP6SS_CORE_CFG_RCGR_HW_CLK_CONTROL_SHFT                              0x14
#define HWIO_QDSP6SS_CORE_CFG_RCGR_RCGLITE_DISABLE_BMSK                          0x10000
#define HWIO_QDSP6SS_CORE_CFG_RCGR_RCGLITE_DISABLE_SHFT                             0x10
#define HWIO_QDSP6SS_CORE_CFG_RCGR_SRC_SEL_BMSK                                    0x700
#define HWIO_QDSP6SS_CORE_CFG_RCGR_SRC_SEL_SHFT                                      0x8
#define HWIO_QDSP6SS_CORE_CFG_RCGR_SRC_DIV_BMSK                                     0x1f
#define HWIO_QDSP6SS_CORE_CFG_RCGR_SRC_DIV_SHFT                                      0x0

#define HWIO_QDSP6SS_SPDM_MON_CBCR_ADDR                                       (QDSP6V67SS_PUB_REG_BASE      + 0x00000048)
#define HWIO_QDSP6SS_SPDM_MON_CBCR_RMSK                                       0x80000001
#define HWIO_QDSP6SS_SPDM_MON_CBCR_IN          \
        in_dword_masked(HWIO_QDSP6SS_SPDM_MON_CBCR_ADDR, HWIO_QDSP6SS_SPDM_MON_CBCR_RMSK)
#define HWIO_QDSP6SS_SPDM_MON_CBCR_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_SPDM_MON_CBCR_ADDR, m)
#define HWIO_QDSP6SS_SPDM_MON_CBCR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_SPDM_MON_CBCR_ADDR,v)
#define HWIO_QDSP6SS_SPDM_MON_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_SPDM_MON_CBCR_ADDR,m,v,HWIO_QDSP6SS_SPDM_MON_CBCR_IN)
#define HWIO_QDSP6SS_SPDM_MON_CBCR_CLKOFF_BMSK                                0x80000000
#define HWIO_QDSP6SS_SPDM_MON_CBCR_CLKOFF_SHFT                                      0x1f
#define HWIO_QDSP6SS_SPDM_MON_CBCR_CLKEN_BMSK                                        0x1
#define HWIO_QDSP6SS_SPDM_MON_CBCR_CLKEN_SHFT                                        0x0

#define HWIO_QDSP6SS_XO_CBCR_ADDR                                             (QDSP6V67SS_PUB_REG_BASE      + 0x00000038)
#define HWIO_QDSP6SS_XO_CBCR_RMSK                                             0x80000001
#define HWIO_QDSP6SS_XO_CBCR_IN          \
        in_dword_masked(HWIO_QDSP6SS_XO_CBCR_ADDR, HWIO_QDSP6SS_XO_CBCR_RMSK)
#define HWIO_QDSP6SS_XO_CBCR_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_XO_CBCR_ADDR, m)
#define HWIO_QDSP6SS_XO_CBCR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_XO_CBCR_ADDR,v)
#define HWIO_QDSP6SS_XO_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_XO_CBCR_ADDR,m,v,HWIO_QDSP6SS_XO_CBCR_IN)
#define HWIO_QDSP6SS_XO_CBCR_CLKOFF_BMSK                                      0x80000000
#define HWIO_QDSP6SS_XO_CBCR_CLKOFF_SHFT                                            0x1f
#define HWIO_QDSP6SS_XO_CBCR_CLKEN_BMSK                                              0x1
#define HWIO_QDSP6SS_XO_CBCR_CLKEN_SHFT                                              0x0

#define HWIO_QDSP6SS_SLEEP_CBCR_ADDR                                          (QDSP6V67SS_PUB_REG_BASE      + 0x0000003c)
#define HWIO_QDSP6SS_SLEEP_CBCR_RMSK                                          0x80000001
#define HWIO_QDSP6SS_SLEEP_CBCR_IN          \
        in_dword_masked(HWIO_QDSP6SS_SLEEP_CBCR_ADDR, HWIO_QDSP6SS_SLEEP_CBCR_RMSK)
#define HWIO_QDSP6SS_SLEEP_CBCR_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_SLEEP_CBCR_ADDR, m)
#define HWIO_QDSP6SS_SLEEP_CBCR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_SLEEP_CBCR_ADDR,v)
#define HWIO_QDSP6SS_SLEEP_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_SLEEP_CBCR_ADDR,m,v,HWIO_QDSP6SS_SLEEP_CBCR_IN)
#define HWIO_QDSP6SS_SLEEP_CBCR_CLKOFF_BMSK                                   0x80000000
#define HWIO_QDSP6SS_SLEEP_CBCR_CLKOFF_SHFT                                         0x1f
#define HWIO_QDSP6SS_SLEEP_CBCR_CLKEN_BMSK                                           0x1
#define HWIO_QDSP6SS_SLEEP_CBCR_CLKEN_SHFT                                           0x0

#define HWIO_QDSP6SS_NMI_ADDR                                                 (QDSP6V67SS_PUB_REG_BASE      + 0x00000040)
#define HWIO_QDSP6SS_NMI_RMSK                                                        0x3
#define HWIO_QDSP6SS_NMI_OUT(v)      \
        out_dword(HWIO_QDSP6SS_NMI_ADDR,v)
#define HWIO_QDSP6SS_NMI_CLEAR_STATUS_BMSK                                           0x2
#define HWIO_QDSP6SS_NMI_CLEAR_STATUS_SHFT                                           0x1
#define HWIO_QDSP6SS_NMI_SET_NMI_BMSK                                                0x1
#define HWIO_QDSP6SS_NMI_SET_NMI_SHFT                                                0x0

#define HWIO_QDSP6SS_NMI_STATUS_ADDR                                          (QDSP6V67SS_PUB_REG_BASE      + 0x00000044)
#define HWIO_QDSP6SS_NMI_STATUS_RMSK                                                 0x7
#define HWIO_QDSP6SS_NMI_STATUS_IN          \
        in_dword_masked(HWIO_QDSP6SS_NMI_STATUS_ADDR, HWIO_QDSP6SS_NMI_STATUS_RMSK)
#define HWIO_QDSP6SS_NMI_STATUS_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_NMI_STATUS_ADDR, m)
#define HWIO_QDSP6SS_NMI_STATUS_EXT_DBG_TRIG_BMSK                                    0x4
#define HWIO_QDSP6SS_NMI_STATUS_EXT_DBG_TRIG_SHFT                                    0x2
#define HWIO_QDSP6SS_NMI_STATUS_WDOG_TRIG_BMSK                                       0x2
#define HWIO_QDSP6SS_NMI_STATUS_WDOG_TRIG_SHFT                                       0x1
#define HWIO_QDSP6SS_NMI_STATUS_PUBCSR_TRIG_BMSK                                     0x1
#define HWIO_QDSP6SS_NMI_STATUS_PUBCSR_TRIG_SHFT                                     0x0

#define HWIO_QDSP6SS_INTF_HALTREQ_ADDR                                        (QDSP6V67SS_PUB_REG_BASE      + 0x00000088)
#define HWIO_QDSP6SS_INTF_HALTREQ_RMSK                                               0x7
#define HWIO_QDSP6SS_INTF_HALTREQ_IN          \
        in_dword_masked(HWIO_QDSP6SS_INTF_HALTREQ_ADDR, HWIO_QDSP6SS_INTF_HALTREQ_RMSK)
#define HWIO_QDSP6SS_INTF_HALTREQ_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_INTF_HALTREQ_ADDR, m)
#define HWIO_QDSP6SS_INTF_HALTREQ_OUT(v)      \
        out_dword(HWIO_QDSP6SS_INTF_HALTREQ_ADDR,v)
#define HWIO_QDSP6SS_INTF_HALTREQ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_INTF_HALTREQ_ADDR,m,v,HWIO_QDSP6SS_INTF_HALTREQ_IN)
#define HWIO_QDSP6SS_INTF_HALTREQ_AXIM2_BMSK                                         0x4
#define HWIO_QDSP6SS_INTF_HALTREQ_AXIM2_SHFT                                         0x2
#define HWIO_QDSP6SS_INTF_HALTREQ_AXIM_BMSK                                          0x2
#define HWIO_QDSP6SS_INTF_HALTREQ_AXIM_SHFT                                          0x1
#define HWIO_QDSP6SS_INTF_HALTREQ_ALL_BMSK                                           0x1
#define HWIO_QDSP6SS_INTF_HALTREQ_ALL_SHFT                                           0x0

#define HWIO_QDSP6SS_INTF_HALTACK_ADDR                                        (QDSP6V67SS_PUB_REG_BASE      + 0x0000008c)
#define HWIO_QDSP6SS_INTF_HALTACK_RMSK                                               0x7
#define HWIO_QDSP6SS_INTF_HALTACK_IN          \
        in_dword_masked(HWIO_QDSP6SS_INTF_HALTACK_ADDR, HWIO_QDSP6SS_INTF_HALTACK_RMSK)
#define HWIO_QDSP6SS_INTF_HALTACK_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_INTF_HALTACK_ADDR, m)
#define HWIO_QDSP6SS_INTF_HALTACK_AXIM2_BMSK                                         0x4
#define HWIO_QDSP6SS_INTF_HALTACK_AXIM2_SHFT                                         0x2
#define HWIO_QDSP6SS_INTF_HALTACK_AXIM_BMSK                                          0x2
#define HWIO_QDSP6SS_INTF_HALTACK_AXIM_SHFT                                          0x1
#define HWIO_QDSP6SS_INTF_HALTACK_ALL_BMSK                                           0x1
#define HWIO_QDSP6SS_INTF_HALTACK_ALL_SHFT                                           0x0

#define HWIO_QDSP6SS_INTFCLAMP_SET_ADDR                                       (QDSP6V67SS_PUB_REG_BASE      + 0x00000090)
#define HWIO_QDSP6SS_INTFCLAMP_SET_RMSK                                              0xf
#define HWIO_QDSP6SS_INTFCLAMP_SET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_INTFCLAMP_SET_ADDR,v)
#define HWIO_QDSP6SS_INTFCLAMP_SET_AXIM2_CLAMP_E_BMSK                                0x8
#define HWIO_QDSP6SS_INTFCLAMP_SET_AXIM2_CLAMP_E_SHFT                                0x3
#define HWIO_QDSP6SS_INTFCLAMP_SET_AXIM2_CLAMP_L_BMSK                                0x4
#define HWIO_QDSP6SS_INTFCLAMP_SET_AXIM2_CLAMP_L_SHFT                                0x2
#define HWIO_QDSP6SS_INTFCLAMP_SET_AXIM_CLAMP_E_BMSK                                 0x2
#define HWIO_QDSP6SS_INTFCLAMP_SET_AXIM_CLAMP_E_SHFT                                 0x1
#define HWIO_QDSP6SS_INTFCLAMP_SET_AXIM_CLAMP_L_BMSK                                 0x1
#define HWIO_QDSP6SS_INTFCLAMP_SET_AXIM_CLAMP_L_SHFT                                 0x0

#define HWIO_QDSP6SS_INTFCLAMP_CLEAR_ADDR                                     (QDSP6V67SS_PUB_REG_BASE      + 0x00000094)
#define HWIO_QDSP6SS_INTFCLAMP_CLEAR_RMSK                                            0xf
#define HWIO_QDSP6SS_INTFCLAMP_CLEAR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_INTFCLAMP_CLEAR_ADDR,v)
#define HWIO_QDSP6SS_INTFCLAMP_CLEAR_AXIM2_CLAMP_E_BMSK                              0x8
#define HWIO_QDSP6SS_INTFCLAMP_CLEAR_AXIM2_CLAMP_E_SHFT                              0x3
#define HWIO_QDSP6SS_INTFCLAMP_CLEAR_AXIM2_CLAMP_L_BMSK                              0x4
#define HWIO_QDSP6SS_INTFCLAMP_CLEAR_AXIM2_CLAMP_L_SHFT                              0x2
#define HWIO_QDSP6SS_INTFCLAMP_CLEAR_AXIM_CLAMP_E_BMSK                               0x2
#define HWIO_QDSP6SS_INTFCLAMP_CLEAR_AXIM_CLAMP_E_SHFT                               0x1
#define HWIO_QDSP6SS_INTFCLAMP_CLEAR_AXIM_CLAMP_L_BMSK                               0x1
#define HWIO_QDSP6SS_INTFCLAMP_CLEAR_AXIM_CLAMP_L_SHFT                               0x0

#define HWIO_QDSP6SS_INTFCLAMP_STATUS_ADDR                                    (QDSP6V67SS_PUB_REG_BASE      + 0x00000098)
#define HWIO_QDSP6SS_INTFCLAMP_STATUS_RMSK                                           0xf
#define HWIO_QDSP6SS_INTFCLAMP_STATUS_IN          \
        in_dword_masked(HWIO_QDSP6SS_INTFCLAMP_STATUS_ADDR, HWIO_QDSP6SS_INTFCLAMP_STATUS_RMSK)
#define HWIO_QDSP6SS_INTFCLAMP_STATUS_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_INTFCLAMP_STATUS_ADDR, m)
#define HWIO_QDSP6SS_INTFCLAMP_STATUS_AXIM2_CLAMP_E_BMSK                             0x8
#define HWIO_QDSP6SS_INTFCLAMP_STATUS_AXIM2_CLAMP_E_SHFT                             0x3
#define HWIO_QDSP6SS_INTFCLAMP_STATUS_AXIM2_CLAMP_L_BMSK                             0x4
#define HWIO_QDSP6SS_INTFCLAMP_STATUS_AXIM2_CLAMP_L_SHFT                             0x2
#define HWIO_QDSP6SS_INTFCLAMP_STATUS_AXIM_CLAMP_E_BMSK                              0x2
#define HWIO_QDSP6SS_INTFCLAMP_STATUS_AXIM_CLAMP_E_SHFT                              0x1
#define HWIO_QDSP6SS_INTFCLAMP_STATUS_AXIM_CLAMP_L_BMSK                              0x1
#define HWIO_QDSP6SS_INTFCLAMP_STATUS_AXIM_CLAMP_L_SHFT                              0x0

#define HWIO_QDSP6SS_INTF_FIFO_RESET_ADDR                                     (QDSP6V67SS_PUB_REG_BASE      + 0x0000009c)
#define HWIO_QDSP6SS_INTF_FIFO_RESET_RMSK                                            0x3
#define HWIO_QDSP6SS_INTF_FIFO_RESET_IN          \
        in_dword_masked(HWIO_QDSP6SS_INTF_FIFO_RESET_ADDR, HWIO_QDSP6SS_INTF_FIFO_RESET_RMSK)
#define HWIO_QDSP6SS_INTF_FIFO_RESET_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_INTF_FIFO_RESET_ADDR, m)
#define HWIO_QDSP6SS_INTF_FIFO_RESET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_INTF_FIFO_RESET_ADDR,v)
#define HWIO_QDSP6SS_INTF_FIFO_RESET_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_INTF_FIFO_RESET_ADDR,m,v,HWIO_QDSP6SS_INTF_FIFO_RESET_IN)
#define HWIO_QDSP6SS_INTF_FIFO_RESET_AXIM2_BMSK                                      0x2
#define HWIO_QDSP6SS_INTF_FIFO_RESET_AXIM2_SHFT                                      0x1
#define HWIO_QDSP6SS_INTF_FIFO_RESET_AXIM_BMSK                                       0x1
#define HWIO_QDSP6SS_INTF_FIFO_RESET_AXIM_SHFT                                       0x0

#define HWIO_QDSP6SS_STATERET_CTL_ADDR                                        (QDSP6V67SS_PUB_REG_BASE      + 0x000000a0)
#define HWIO_QDSP6SS_STATERET_CTL_RMSK                                               0xf
#define HWIO_QDSP6SS_STATERET_CTL_IN          \
        in_dword_masked(HWIO_QDSP6SS_STATERET_CTL_ADDR, HWIO_QDSP6SS_STATERET_CTL_RMSK)
#define HWIO_QDSP6SS_STATERET_CTL_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_STATERET_CTL_ADDR, m)
#define HWIO_QDSP6SS_STATERET_CTL_OUT(v)      \
        out_dword(HWIO_QDSP6SS_STATERET_CTL_ADDR,v)
#define HWIO_QDSP6SS_STATERET_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_STATERET_CTL_ADDR,m,v,HWIO_QDSP6SS_STATERET_CTL_IN)
#define HWIO_QDSP6SS_STATERET_CTL_WAKEUP_IN_BMSK                                     0x8
#define HWIO_QDSP6SS_STATERET_CTL_WAKEUP_IN_SHFT                                     0x3
#define HWIO_QDSP6SS_STATERET_CTL_WAKE_IRQ_BMSK                                      0x4
#define HWIO_QDSP6SS_STATERET_CTL_WAKE_IRQ_SHFT                                      0x2
#define HWIO_QDSP6SS_STATERET_CTL_RESTORE_BMSK                                       0x2
#define HWIO_QDSP6SS_STATERET_CTL_RESTORE_SHFT                                       0x1
#define HWIO_QDSP6SS_STATERET_CTL_SAVE_BMSK                                          0x1
#define HWIO_QDSP6SS_STATERET_CTL_SAVE_SHFT                                          0x0

#define HWIO_QDSP6SS_CGC_OVERRIDE_ADDR                                        (QDSP6V67SS_PUB_REG_BASE      + 0x000000b0)
#define HWIO_QDSP6SS_CGC_OVERRIDE_RMSK                                            0xffff
#define HWIO_QDSP6SS_CGC_OVERRIDE_IN          \
        in_dword_masked(HWIO_QDSP6SS_CGC_OVERRIDE_ADDR, HWIO_QDSP6SS_CGC_OVERRIDE_RMSK)
#define HWIO_QDSP6SS_CGC_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_CGC_OVERRIDE_ADDR, m)
#define HWIO_QDSP6SS_CGC_OVERRIDE_OUT(v)      \
        out_dword(HWIO_QDSP6SS_CGC_OVERRIDE_ADDR,v)
#define HWIO_QDSP6SS_CGC_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_CGC_OVERRIDE_ADDR,m,v,HWIO_QDSP6SS_CGC_OVERRIDE_IN)
#define HWIO_QDSP6SS_CGC_OVERRIDE_CP0_CLK_EN_BMSK                                 0x8000
#define HWIO_QDSP6SS_CGC_OVERRIDE_CP0_CLK_EN_SHFT                                    0xf
#define HWIO_QDSP6SS_CGC_OVERRIDE_SPARE_BIT14_BMSK                                0x4000
#define HWIO_QDSP6SS_CGC_OVERRIDE_SPARE_BIT14_SHFT                                   0xe
#define HWIO_QDSP6SS_CGC_OVERRIDE_AXIS_CLK_EN_BMSK                                0x2000
#define HWIO_QDSP6SS_CGC_OVERRIDE_AXIS_CLK_EN_SHFT                                   0xd
#define HWIO_QDSP6SS_CGC_OVERRIDE_SPARE_BIT12_BMSK                                0x1000
#define HWIO_QDSP6SS_CGC_OVERRIDE_SPARE_BIT12_SHFT                                   0xc
#define HWIO_QDSP6SS_CGC_OVERRIDE_AHBM_CLK_EN_BMSK                                 0x800
#define HWIO_QDSP6SS_CGC_OVERRIDE_AHBM_CLK_EN_SHFT                                   0xb
#define HWIO_QDSP6SS_CGC_OVERRIDE_AHBS_CLK_EN_BMSK                                 0x400
#define HWIO_QDSP6SS_CGC_OVERRIDE_AHBS_CLK_EN_SHFT                                   0xa
#define HWIO_QDSP6SS_CGC_OVERRIDE_PRIV_AHBS_CLK_EN_BMSK                            0x300
#define HWIO_QDSP6SS_CGC_OVERRIDE_PRIV_AHBS_CLK_EN_SHFT                              0x8
#define HWIO_QDSP6SS_CGC_OVERRIDE_SPARE_BIT7_BMSK                                   0x80
#define HWIO_QDSP6SS_CGC_OVERRIDE_SPARE_BIT7_SHFT                                    0x7
#define HWIO_QDSP6SS_CGC_OVERRIDE_L2VIC_AHBS_CLK_EN_BMSK                            0x60
#define HWIO_QDSP6SS_CGC_OVERRIDE_L2VIC_AHBS_CLK_EN_SHFT                             0x5
#define HWIO_QDSP6SS_CGC_OVERRIDE_TEST_AHBS_CLK_EN_BMSK                             0x10
#define HWIO_QDSP6SS_CGC_OVERRIDE_TEST_AHBS_CLK_EN_SHFT                              0x4
#define HWIO_QDSP6SS_CGC_OVERRIDE_PUB_AHBS_CLK_EN_BMSK                               0x8
#define HWIO_QDSP6SS_CGC_OVERRIDE_PUB_AHBS_CLK_EN_SHFT                               0x3
#define HWIO_QDSP6SS_CGC_OVERRIDE_CORE_BUS_EN_BMSK                                   0x4
#define HWIO_QDSP6SS_CGC_OVERRIDE_CORE_BUS_EN_SHFT                                   0x2
#define HWIO_QDSP6SS_CGC_OVERRIDE_CORE_RCLK_EN_BMSK                                  0x2
#define HWIO_QDSP6SS_CGC_OVERRIDE_CORE_RCLK_EN_SHFT                                  0x1
#define HWIO_QDSP6SS_CGC_OVERRIDE_CORE_CLK_EN_BMSK                                   0x1
#define HWIO_QDSP6SS_CGC_OVERRIDE_CORE_CLK_EN_SHFT                                   0x0

#define HWIO_QDSP6SS_CORE_BHS_CTL_ADDR                                        (QDSP6V67SS_PUB_REG_BASE      + 0x000000c0)
#define HWIO_QDSP6SS_CORE_BHS_CTL_RMSK                                           0x3f3ff
#define HWIO_QDSP6SS_CORE_BHS_CTL_IN          \
        in_dword_masked(HWIO_QDSP6SS_CORE_BHS_CTL_ADDR, HWIO_QDSP6SS_CORE_BHS_CTL_RMSK)
#define HWIO_QDSP6SS_CORE_BHS_CTL_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_CORE_BHS_CTL_ADDR, m)
#define HWIO_QDSP6SS_CORE_BHS_CTL_OUT(v)      \
        out_dword(HWIO_QDSP6SS_CORE_BHS_CTL_ADDR,v)
#define HWIO_QDSP6SS_CORE_BHS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_CORE_BHS_CTL_ADDR,m,v,HWIO_QDSP6SS_CORE_BHS_CTL_IN)
#define HWIO_QDSP6SS_CORE_BHS_CTL_DELAY_PROG_BMSK                                0x3f000
#define HWIO_QDSP6SS_CORE_BHS_CTL_DELAY_PROG_SHFT                                    0xc
#define HWIO_QDSP6SS_CORE_BHS_CTL_ENF_PROG_BMSK                                    0x3f0
#define HWIO_QDSP6SS_CORE_BHS_CTL_ENF_PROG_SHFT                                      0x4
#define HWIO_QDSP6SS_CORE_BHS_CTL_DRIVE_SEL_BMSK                                     0xf
#define HWIO_QDSP6SS_CORE_BHS_CTL_DRIVE_SEL_SHFT                                     0x0

#define HWIO_QDSP6SS_CPR_OVERRIDE_ADDR                                        (QDSP6V67SS_PUB_REG_BASE      + 0x000000d0)
#define HWIO_QDSP6SS_CPR_OVERRIDE_RMSK                                               0x3
#define HWIO_QDSP6SS_CPR_OVERRIDE_IN          \
        in_dword_masked(HWIO_QDSP6SS_CPR_OVERRIDE_ADDR, HWIO_QDSP6SS_CPR_OVERRIDE_RMSK)
#define HWIO_QDSP6SS_CPR_OVERRIDE_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_CPR_OVERRIDE_ADDR, m)
#define HWIO_QDSP6SS_CPR_OVERRIDE_OUT(v)      \
        out_dword(HWIO_QDSP6SS_CPR_OVERRIDE_ADDR,v)
#define HWIO_QDSP6SS_CPR_OVERRIDE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_CPR_OVERRIDE_ADDR,m,v,HWIO_QDSP6SS_CPR_OVERRIDE_IN)
#define HWIO_QDSP6SS_CPR_OVERRIDE_EXT_ABYP_DIS_BMSK                                  0x2
#define HWIO_QDSP6SS_CPR_OVERRIDE_EXT_ABYP_DIS_SHFT                                  0x1
#define HWIO_QDSP6SS_CPR_OVERRIDE_LOCAL_BYP_DIS_BMSK                                 0x1
#define HWIO_QDSP6SS_CPR_OVERRIDE_LOCAL_BYP_DIS_SHFT                                 0x0

#define HWIO_QDSP6SS_STRAP_TCM_ADDR                                           (QDSP6V67SS_PUB_REG_BASE      + 0x00000100)
#define HWIO_QDSP6SS_STRAP_TCM_RMSK                                           0xfffc0000
#define HWIO_QDSP6SS_STRAP_TCM_IN          \
        in_dword_masked(HWIO_QDSP6SS_STRAP_TCM_ADDR, HWIO_QDSP6SS_STRAP_TCM_RMSK)
#define HWIO_QDSP6SS_STRAP_TCM_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_STRAP_TCM_ADDR, m)
#define HWIO_QDSP6SS_STRAP_TCM_OUT(v)      \
        out_dword(HWIO_QDSP6SS_STRAP_TCM_ADDR,v)
#define HWIO_QDSP6SS_STRAP_TCM_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_STRAP_TCM_ADDR,m,v,HWIO_QDSP6SS_STRAP_TCM_IN)
#define HWIO_QDSP6SS_STRAP_TCM_BASE_ADDR_BMSK                                 0xfffc0000
#define HWIO_QDSP6SS_STRAP_TCM_BASE_ADDR_SHFT                                       0x12

#define HWIO_QDSP6SS_STRAP_AHBUPPER_ADDR                                      (QDSP6V67SS_PUB_REG_BASE      + 0x00000104)
#define HWIO_QDSP6SS_STRAP_AHBUPPER_RMSK                                      0xffffc000
#define HWIO_QDSP6SS_STRAP_AHBUPPER_IN          \
        in_dword_masked(HWIO_QDSP6SS_STRAP_AHBUPPER_ADDR, HWIO_QDSP6SS_STRAP_AHBUPPER_RMSK)
#define HWIO_QDSP6SS_STRAP_AHBUPPER_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_STRAP_AHBUPPER_ADDR, m)
#define HWIO_QDSP6SS_STRAP_AHBUPPER_OUT(v)      \
        out_dword(HWIO_QDSP6SS_STRAP_AHBUPPER_ADDR,v)
#define HWIO_QDSP6SS_STRAP_AHBUPPER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_STRAP_AHBUPPER_ADDR,m,v,HWIO_QDSP6SS_STRAP_AHBUPPER_IN)
#define HWIO_QDSP6SS_STRAP_AHBUPPER_UPPER_ADDR_BMSK                           0xffffc000
#define HWIO_QDSP6SS_STRAP_AHBUPPER_UPPER_ADDR_SHFT                                  0xe

#define HWIO_QDSP6SS_STRAP_AHBLOWER_ADDR                                      (QDSP6V67SS_PUB_REG_BASE      + 0x00000108)
#define HWIO_QDSP6SS_STRAP_AHBLOWER_RMSK                                      0xffffc000
#define HWIO_QDSP6SS_STRAP_AHBLOWER_IN          \
        in_dword_masked(HWIO_QDSP6SS_STRAP_AHBLOWER_ADDR, HWIO_QDSP6SS_STRAP_AHBLOWER_RMSK)
#define HWIO_QDSP6SS_STRAP_AHBLOWER_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_STRAP_AHBLOWER_ADDR, m)
#define HWIO_QDSP6SS_STRAP_AHBLOWER_OUT(v)      \
        out_dword(HWIO_QDSP6SS_STRAP_AHBLOWER_ADDR,v)
#define HWIO_QDSP6SS_STRAP_AHBLOWER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_STRAP_AHBLOWER_ADDR,m,v,HWIO_QDSP6SS_STRAP_AHBLOWER_IN)
#define HWIO_QDSP6SS_STRAP_AHBLOWER_LOWER_ADDR_BMSK                           0xffffc000
#define HWIO_QDSP6SS_STRAP_AHBLOWER_LOWER_ADDR_SHFT                                  0xe

#define HWIO_QDSP6SS_DCC_CTRL_ADDR                                            (QDSP6V67SS_PUB_REG_BASE      + 0x00000118)
#define HWIO_QDSP6SS_DCC_CTRL_RMSK                                            0xffffffff
#define HWIO_QDSP6SS_DCC_CTRL_IN          \
        in_dword_masked(HWIO_QDSP6SS_DCC_CTRL_ADDR, HWIO_QDSP6SS_DCC_CTRL_RMSK)
#define HWIO_QDSP6SS_DCC_CTRL_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_DCC_CTRL_ADDR, m)
#define HWIO_QDSP6SS_DCC_CTRL_OUT(v)      \
        out_dword(HWIO_QDSP6SS_DCC_CTRL_ADDR,v)
#define HWIO_QDSP6SS_DCC_CTRL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_DCC_CTRL_ADDR,m,v,HWIO_QDSP6SS_DCC_CTRL_IN)
#define HWIO_QDSP6SS_DCC_CTRL_CTRL_BMSK                                       0xffffffff
#define HWIO_QDSP6SS_DCC_CTRL_CTRL_SHFT                                              0x0

#define HWIO_QDSP6SS_STRAP_AXIM2UPPER_ADDR                                    (QDSP6V67SS_PUB_REG_BASE      + 0x0000011c)
#define HWIO_QDSP6SS_STRAP_AXIM2UPPER_RMSK                                    0xffffc000
#define HWIO_QDSP6SS_STRAP_AXIM2UPPER_IN          \
        in_dword_masked(HWIO_QDSP6SS_STRAP_AXIM2UPPER_ADDR, HWIO_QDSP6SS_STRAP_AXIM2UPPER_RMSK)
#define HWIO_QDSP6SS_STRAP_AXIM2UPPER_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_STRAP_AXIM2UPPER_ADDR, m)
#define HWIO_QDSP6SS_STRAP_AXIM2UPPER_OUT(v)      \
        out_dword(HWIO_QDSP6SS_STRAP_AXIM2UPPER_ADDR,v)
#define HWIO_QDSP6SS_STRAP_AXIM2UPPER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_STRAP_AXIM2UPPER_ADDR,m,v,HWIO_QDSP6SS_STRAP_AXIM2UPPER_IN)
#define HWIO_QDSP6SS_STRAP_AXIM2UPPER_UPPER_ADDR_BMSK                         0xffffc000
#define HWIO_QDSP6SS_STRAP_AXIM2UPPER_UPPER_ADDR_SHFT                                0xe

#define HWIO_QDSP6SS_STRAP_AXIM2LOWER_ADDR                                    (QDSP6V67SS_PUB_REG_BASE      + 0x00000120)
#define HWIO_QDSP6SS_STRAP_AXIM2LOWER_RMSK                                    0xffffc000
#define HWIO_QDSP6SS_STRAP_AXIM2LOWER_IN          \
        in_dword_masked(HWIO_QDSP6SS_STRAP_AXIM2LOWER_ADDR, HWIO_QDSP6SS_STRAP_AXIM2LOWER_RMSK)
#define HWIO_QDSP6SS_STRAP_AXIM2LOWER_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_STRAP_AXIM2LOWER_ADDR, m)
#define HWIO_QDSP6SS_STRAP_AXIM2LOWER_OUT(v)      \
        out_dword(HWIO_QDSP6SS_STRAP_AXIM2LOWER_ADDR,v)
#define HWIO_QDSP6SS_STRAP_AXIM2LOWER_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_STRAP_AXIM2LOWER_ADDR,m,v,HWIO_QDSP6SS_STRAP_AXIM2LOWER_IN)
#define HWIO_QDSP6SS_STRAP_AXIM2LOWER_LOWER_ADDR_BMSK                         0xffffc000
#define HWIO_QDSP6SS_STRAP_AXIM2LOWER_LOWER_ADDR_SHFT                                0xe

#define HWIO_QDSP6SS_QMC_SVS_CTL_ADDR                                         (QDSP6V67SS_PUB_REG_BASE      + 0x00000130)
#define HWIO_QDSP6SS_QMC_SVS_CTL_RMSK                                                0x3
#define HWIO_QDSP6SS_QMC_SVS_CTL_IN          \
        in_dword_masked(HWIO_QDSP6SS_QMC_SVS_CTL_ADDR, HWIO_QDSP6SS_QMC_SVS_CTL_RMSK)
#define HWIO_QDSP6SS_QMC_SVS_CTL_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_QMC_SVS_CTL_ADDR, m)
#define HWIO_QDSP6SS_QMC_SVS_CTL_OUT(v)      \
        out_dword(HWIO_QDSP6SS_QMC_SVS_CTL_ADDR,v)
#define HWIO_QDSP6SS_QMC_SVS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_QMC_SVS_CTL_ADDR,m,v,HWIO_QDSP6SS_QMC_SVS_CTL_IN)
#define HWIO_QDSP6SS_QMC_SVS_CTL_QMC_MEM_SVS_SEL_BMSK                                0x2
#define HWIO_QDSP6SS_QMC_SVS_CTL_QMC_MEM_SVS_SEL_SHFT                                0x1
#define HWIO_QDSP6SS_QMC_SVS_CTL_QMC_MEM_SVS_BMSK                                    0x1
#define HWIO_QDSP6SS_QMC_SVS_CTL_QMC_MEM_SVS_SHFT                                    0x0

#define HWIO_QDSP6SS_CORE_MEM_STAGGER_CTL_ADDR                                (QDSP6V67SS_PUB_REG_BASE      + 0x00000140)
#define HWIO_QDSP6SS_CORE_MEM_STAGGER_CTL_RMSK                                       0xf
#define HWIO_QDSP6SS_CORE_MEM_STAGGER_CTL_IN          \
        in_dword_masked(HWIO_QDSP6SS_CORE_MEM_STAGGER_CTL_ADDR, HWIO_QDSP6SS_CORE_MEM_STAGGER_CTL_RMSK)
#define HWIO_QDSP6SS_CORE_MEM_STAGGER_CTL_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_CORE_MEM_STAGGER_CTL_ADDR, m)
#define HWIO_QDSP6SS_CORE_MEM_STAGGER_CTL_OUT(v)      \
        out_dword(HWIO_QDSP6SS_CORE_MEM_STAGGER_CTL_ADDR,v)
#define HWIO_QDSP6SS_CORE_MEM_STAGGER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_CORE_MEM_STAGGER_CTL_ADDR,m,v,HWIO_QDSP6SS_CORE_MEM_STAGGER_CTL_IN)
#define HWIO_QDSP6SS_CORE_MEM_STAGGER_CTL_DIV_BMSK                                   0xf
#define HWIO_QDSP6SS_CORE_MEM_STAGGER_CTL_DIV_SHFT                                   0x0

#define HWIO_QDSP6SS_PLL_MODE_ADDR                                            (QDSP6V67SS_PUB_REG_BASE      + 0x00000200)
#define HWIO_QDSP6SS_PLL_MODE_RMSK                                            0xffffffff
#define HWIO_QDSP6SS_PLL_MODE_IN          \
        in_dword_masked(HWIO_QDSP6SS_PLL_MODE_ADDR, HWIO_QDSP6SS_PLL_MODE_RMSK)
#define HWIO_QDSP6SS_PLL_MODE_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_PLL_MODE_ADDR, m)
#define HWIO_QDSP6SS_PLL_MODE_OUT(v)      \
        out_dword(HWIO_QDSP6SS_PLL_MODE_ADDR,v)
#define HWIO_QDSP6SS_PLL_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_PLL_MODE_ADDR,m,v,HWIO_QDSP6SS_PLL_MODE_IN)
#define HWIO_QDSP6SS_PLL_MODE_PLL_LOCK_DET_BMSK                               0x80000000
#define HWIO_QDSP6SS_PLL_MODE_PLL_LOCK_DET_SHFT                                     0x1f
#define HWIO_QDSP6SS_PLL_MODE_PLL_ACTIVE_FLAG_BMSK                            0x40000000
#define HWIO_QDSP6SS_PLL_MODE_PLL_ACTIVE_FLAG_SHFT                                  0x1e
#define HWIO_QDSP6SS_PLL_MODE_PLL_ACK_LATCH_BMSK                              0x20000000
#define HWIO_QDSP6SS_PLL_MODE_PLL_ACK_LATCH_SHFT                                    0x1d
#define HWIO_QDSP6SS_PLL_MODE_RESERVE_BITS28_24_BMSK                          0x1f000000
#define HWIO_QDSP6SS_PLL_MODE_RESERVE_BITS28_24_SHFT                                0x18
#define HWIO_QDSP6SS_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_BMSK                   0x800000
#define HWIO_QDSP6SS_PLL_MODE_PLL_HW_UPDATE_LOGIC_BYPASS_SHFT                       0x17
#define HWIO_QDSP6SS_PLL_MODE_PLL_UPDATE_BMSK                                   0x400000
#define HWIO_QDSP6SS_PLL_MODE_PLL_UPDATE_SHFT                                       0x16
#define HWIO_QDSP6SS_PLL_MODE_PLL_VOTE_FSM_RESET_BMSK                           0x200000
#define HWIO_QDSP6SS_PLL_MODE_PLL_VOTE_FSM_RESET_SHFT                               0x15
#define HWIO_QDSP6SS_PLL_MODE_PLL_VOTE_FSM_ENA_BMSK                             0x100000
#define HWIO_QDSP6SS_PLL_MODE_PLL_VOTE_FSM_ENA_SHFT                                 0x14
#define HWIO_QDSP6SS_PLL_MODE_PLL_BIAS_COUNT_BMSK                                0xfc000
#define HWIO_QDSP6SS_PLL_MODE_PLL_BIAS_COUNT_SHFT                                    0xe
#define HWIO_QDSP6SS_PLL_MODE_PLL_LOCK_COUNT_BMSK                                 0x3f00
#define HWIO_QDSP6SS_PLL_MODE_PLL_LOCK_COUNT_SHFT                                    0x8
#define HWIO_QDSP6SS_PLL_MODE_RESERVE_BITS7_4_BMSK                                  0xf0
#define HWIO_QDSP6SS_PLL_MODE_RESERVE_BITS7_4_SHFT                                   0x4
#define HWIO_QDSP6SS_PLL_MODE_PLL_PLLTEST_BMSK                                       0x8
#define HWIO_QDSP6SS_PLL_MODE_PLL_PLLTEST_SHFT                                       0x3
#define HWIO_QDSP6SS_PLL_MODE_PLL_RESET_N_BMSK                                       0x4
#define HWIO_QDSP6SS_PLL_MODE_PLL_RESET_N_SHFT                                       0x2
#define HWIO_QDSP6SS_PLL_MODE_PLL_BYPASSNL_BMSK                                      0x2
#define HWIO_QDSP6SS_PLL_MODE_PLL_BYPASSNL_SHFT                                      0x1
#define HWIO_QDSP6SS_PLL_MODE_PLL_OUTCTRL_BMSK                                       0x1
#define HWIO_QDSP6SS_PLL_MODE_PLL_OUTCTRL_SHFT                                       0x0

#define HWIO_QDSP6SS_PLL_L_VAL_ADDR                                           (QDSP6V67SS_PUB_REG_BASE      + 0x00000204)
#define HWIO_QDSP6SS_PLL_L_VAL_RMSK                                               0xffff
#define HWIO_QDSP6SS_PLL_L_VAL_IN          \
        in_dword_masked(HWIO_QDSP6SS_PLL_L_VAL_ADDR, HWIO_QDSP6SS_PLL_L_VAL_RMSK)
#define HWIO_QDSP6SS_PLL_L_VAL_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_PLL_L_VAL_ADDR, m)
#define HWIO_QDSP6SS_PLL_L_VAL_OUT(v)      \
        out_dword(HWIO_QDSP6SS_PLL_L_VAL_ADDR,v)
#define HWIO_QDSP6SS_PLL_L_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_PLL_L_VAL_ADDR,m,v,HWIO_QDSP6SS_PLL_L_VAL_IN)
#define HWIO_QDSP6SS_PLL_L_VAL_PLL_L_BMSK                                         0xffff
#define HWIO_QDSP6SS_PLL_L_VAL_PLL_L_SHFT                                            0x0

#define HWIO_QDSP6SS_PLL_ALPHA_VAL_ADDR                                       (QDSP6V67SS_PUB_REG_BASE      + 0x00000208)
#define HWIO_QDSP6SS_PLL_ALPHA_VAL_RMSK                                       0xffffffff
#define HWIO_QDSP6SS_PLL_ALPHA_VAL_IN          \
        in_dword_masked(HWIO_QDSP6SS_PLL_ALPHA_VAL_ADDR, HWIO_QDSP6SS_PLL_ALPHA_VAL_RMSK)
#define HWIO_QDSP6SS_PLL_ALPHA_VAL_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_PLL_ALPHA_VAL_ADDR, m)
#define HWIO_QDSP6SS_PLL_ALPHA_VAL_OUT(v)      \
        out_dword(HWIO_QDSP6SS_PLL_ALPHA_VAL_ADDR,v)
#define HWIO_QDSP6SS_PLL_ALPHA_VAL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_PLL_ALPHA_VAL_ADDR,m,v,HWIO_QDSP6SS_PLL_ALPHA_VAL_IN)
#define HWIO_QDSP6SS_PLL_ALPHA_VAL_PLL_ALPHA_31_0_BMSK                        0xffffffff
#define HWIO_QDSP6SS_PLL_ALPHA_VAL_PLL_ALPHA_31_0_SHFT                               0x0

#define HWIO_QDSP6SS_PLL_ALPHA_VAL_U_ADDR                                     (QDSP6V67SS_PUB_REG_BASE      + 0x0000020c)
#define HWIO_QDSP6SS_PLL_ALPHA_VAL_U_RMSK                                           0xff
#define HWIO_QDSP6SS_PLL_ALPHA_VAL_U_IN          \
        in_dword_masked(HWIO_QDSP6SS_PLL_ALPHA_VAL_U_ADDR, HWIO_QDSP6SS_PLL_ALPHA_VAL_U_RMSK)
#define HWIO_QDSP6SS_PLL_ALPHA_VAL_U_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_PLL_ALPHA_VAL_U_ADDR, m)
#define HWIO_QDSP6SS_PLL_ALPHA_VAL_U_OUT(v)      \
        out_dword(HWIO_QDSP6SS_PLL_ALPHA_VAL_U_ADDR,v)
#define HWIO_QDSP6SS_PLL_ALPHA_VAL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_PLL_ALPHA_VAL_U_ADDR,m,v,HWIO_QDSP6SS_PLL_ALPHA_VAL_U_IN)
#define HWIO_QDSP6SS_PLL_ALPHA_VAL_U_PLL_ALPHA_39_32_BMSK                           0xff
#define HWIO_QDSP6SS_PLL_ALPHA_VAL_U_PLL_ALPHA_39_32_SHFT                            0x0

#define HWIO_QDSP6SS_PLL_USER_CTL_ADDR                                        (QDSP6V67SS_PUB_REG_BASE      + 0x00000210)
#define HWIO_QDSP6SS_PLL_USER_CTL_RMSK                                        0xffffffff
#define HWIO_QDSP6SS_PLL_USER_CTL_IN          \
        in_dword_masked(HWIO_QDSP6SS_PLL_USER_CTL_ADDR, HWIO_QDSP6SS_PLL_USER_CTL_RMSK)
#define HWIO_QDSP6SS_PLL_USER_CTL_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_PLL_USER_CTL_ADDR, m)
#define HWIO_QDSP6SS_PLL_USER_CTL_OUT(v)      \
        out_dword(HWIO_QDSP6SS_PLL_USER_CTL_ADDR,v)
#define HWIO_QDSP6SS_PLL_USER_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_PLL_USER_CTL_ADDR,m,v,HWIO_QDSP6SS_PLL_USER_CTL_IN)
#define HWIO_QDSP6SS_PLL_USER_CTL_RESERVE_BITS31_28_BMSK                      0xf0000000
#define HWIO_QDSP6SS_PLL_USER_CTL_RESERVE_BITS31_28_SHFT                            0x1c
#define HWIO_QDSP6SS_PLL_USER_CTL_SSC_MODE_CONTROL_BMSK                        0x8000000
#define HWIO_QDSP6SS_PLL_USER_CTL_SSC_MODE_CONTROL_SHFT                             0x1b
#define HWIO_QDSP6SS_PLL_USER_CTL_RESERVE_BITS26_25_BMSK                       0x6000000
#define HWIO_QDSP6SS_PLL_USER_CTL_RESERVE_BITS26_25_SHFT                            0x19
#define HWIO_QDSP6SS_PLL_USER_CTL_ALPHA_EN_BMSK                                0x1000000
#define HWIO_QDSP6SS_PLL_USER_CTL_ALPHA_EN_SHFT                                     0x18
#define HWIO_QDSP6SS_PLL_USER_CTL_RESERVE_BITS23_22_BMSK                        0xc00000
#define HWIO_QDSP6SS_PLL_USER_CTL_RESERVE_BITS23_22_SHFT                            0x16
#define HWIO_QDSP6SS_PLL_USER_CTL_VCO_SEL_BMSK                                  0x300000
#define HWIO_QDSP6SS_PLL_USER_CTL_VCO_SEL_SHFT                                      0x14
#define HWIO_QDSP6SS_PLL_USER_CTL_RESERVE_BITS19_15_BMSK                         0xf8000
#define HWIO_QDSP6SS_PLL_USER_CTL_RESERVE_BITS19_15_SHFT                             0xf
#define HWIO_QDSP6SS_PLL_USER_CTL_PRE_DIV_RATIO_BMSK                              0x7000
#define HWIO_QDSP6SS_PLL_USER_CTL_PRE_DIV_RATIO_SHFT                                 0xc
#define HWIO_QDSP6SS_PLL_USER_CTL_POST_DIV_RATIO_BMSK                              0xf00
#define HWIO_QDSP6SS_PLL_USER_CTL_POST_DIV_RATIO_SHFT                                0x8
#define HWIO_QDSP6SS_PLL_USER_CTL_OUTPUT_INV_BMSK                                   0x80
#define HWIO_QDSP6SS_PLL_USER_CTL_OUTPUT_INV_SHFT                                    0x7
#define HWIO_QDSP6SS_PLL_USER_CTL_RESERVE_BITS6_5_BMSK                              0x60
#define HWIO_QDSP6SS_PLL_USER_CTL_RESERVE_BITS6_5_SHFT                               0x5
#define HWIO_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_TEST_BMSK                               0x10
#define HWIO_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_TEST_SHFT                                0x4
#define HWIO_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_EARLY_BMSK                               0x8
#define HWIO_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_EARLY_SHFT                               0x3
#define HWIO_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_AUX2_BMSK                                0x4
#define HWIO_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_AUX2_SHFT                                0x2
#define HWIO_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_AUX_BMSK                                 0x2
#define HWIO_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_AUX_SHFT                                 0x1
#define HWIO_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_MAIN_BMSK                                0x1
#define HWIO_QDSP6SS_PLL_USER_CTL_PLLOUT_LV_MAIN_SHFT                                0x0

#define HWIO_QDSP6SS_PLL_USER_CTL_U_ADDR                                      (QDSP6V67SS_PUB_REG_BASE      + 0x00000214)
#define HWIO_QDSP6SS_PLL_USER_CTL_U_RMSK                                      0xffffffff
#define HWIO_QDSP6SS_PLL_USER_CTL_U_IN          \
        in_dword_masked(HWIO_QDSP6SS_PLL_USER_CTL_U_ADDR, HWIO_QDSP6SS_PLL_USER_CTL_U_RMSK)
#define HWIO_QDSP6SS_PLL_USER_CTL_U_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_PLL_USER_CTL_U_ADDR, m)
#define HWIO_QDSP6SS_PLL_USER_CTL_U_OUT(v)      \
        out_dword(HWIO_QDSP6SS_PLL_USER_CTL_U_ADDR,v)
#define HWIO_QDSP6SS_PLL_USER_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_PLL_USER_CTL_U_ADDR,m,v,HWIO_QDSP6SS_PLL_USER_CTL_U_IN)
#define HWIO_QDSP6SS_PLL_USER_CTL_U_CALIBRATION_L_BMSK                        0xffff0000
#define HWIO_QDSP6SS_PLL_USER_CTL_U_CALIBRATION_L_SHFT                              0x10
#define HWIO_QDSP6SS_PLL_USER_CTL_U_RESERVE_BITS15_12_BMSK                        0xf000
#define HWIO_QDSP6SS_PLL_USER_CTL_U_RESERVE_BITS15_12_SHFT                           0xc
#define HWIO_QDSP6SS_PLL_USER_CTL_U_LATCH_INTERFACE_BYPASS_BMSK                    0x800
#define HWIO_QDSP6SS_PLL_USER_CTL_U_LATCH_INTERFACE_BYPASS_SHFT                      0xb
#define HWIO_QDSP6SS_PLL_USER_CTL_U_STATUS_REGISTER_BMSK                           0x700
#define HWIO_QDSP6SS_PLL_USER_CTL_U_STATUS_REGISTER_SHFT                             0x8
#define HWIO_QDSP6SS_PLL_USER_CTL_U_DSM_BMSK                                        0x80
#define HWIO_QDSP6SS_PLL_USER_CTL_U_DSM_SHFT                                         0x7
#define HWIO_QDSP6SS_PLL_USER_CTL_U_WRITE_STATE_BMSK                                0x40
#define HWIO_QDSP6SS_PLL_USER_CTL_U_WRITE_STATE_SHFT                                 0x6
#define HWIO_QDSP6SS_PLL_USER_CTL_U_TARGET_CTL_BMSK                                 0x38
#define HWIO_QDSP6SS_PLL_USER_CTL_U_TARGET_CTL_SHFT                                  0x3
#define HWIO_QDSP6SS_PLL_USER_CTL_U_LOCK_DET_BMSK                                    0x4
#define HWIO_QDSP6SS_PLL_USER_CTL_U_LOCK_DET_SHFT                                    0x2
#define HWIO_QDSP6SS_PLL_USER_CTL_U_FREEZE_PLL_BMSK                                  0x2
#define HWIO_QDSP6SS_PLL_USER_CTL_U_FREEZE_PLL_SHFT                                  0x1
#define HWIO_QDSP6SS_PLL_USER_CTL_U_TOGGLE_DET_BMSK                                  0x1
#define HWIO_QDSP6SS_PLL_USER_CTL_U_TOGGLE_DET_SHFT                                  0x0

#define HWIO_QDSP6SS_PLL_CONFIG_CTL_ADDR                                      (QDSP6V67SS_PUB_REG_BASE      + 0x00000218)
#define HWIO_QDSP6SS_PLL_CONFIG_CTL_RMSK                                      0xffffffff
#define HWIO_QDSP6SS_PLL_CONFIG_CTL_IN          \
        in_dword_masked(HWIO_QDSP6SS_PLL_CONFIG_CTL_ADDR, HWIO_QDSP6SS_PLL_CONFIG_CTL_RMSK)
#define HWIO_QDSP6SS_PLL_CONFIG_CTL_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_PLL_CONFIG_CTL_ADDR, m)
#define HWIO_QDSP6SS_PLL_CONFIG_CTL_OUT(v)      \
        out_dword(HWIO_QDSP6SS_PLL_CONFIG_CTL_ADDR,v)
#define HWIO_QDSP6SS_PLL_CONFIG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_PLL_CONFIG_CTL_ADDR,m,v,HWIO_QDSP6SS_PLL_CONFIG_CTL_IN)
#define HWIO_QDSP6SS_PLL_CONFIG_CTL_SINGLE_DMET_MODE_ENABLE_BMSK              0x80000000
#define HWIO_QDSP6SS_PLL_CONFIG_CTL_SINGLE_DMET_MODE_ENABLE_SHFT                    0x1f
#define HWIO_QDSP6SS_PLL_CONFIG_CTL_DMET_WINDOW_ENABLE_BMSK                   0x40000000
#define HWIO_QDSP6SS_PLL_CONFIG_CTL_DMET_WINDOW_ENABLE_SHFT                         0x1e
#define HWIO_QDSP6SS_PLL_CONFIG_CTL_TOGGLE_DET_SAMPLE_INTER_BMSK              0x3c000000
#define HWIO_QDSP6SS_PLL_CONFIG_CTL_TOGGLE_DET_SAMPLE_INTER_SHFT                    0x1a
#define HWIO_QDSP6SS_PLL_CONFIG_CTL_TOGGLE_DET_THRESHOLD_BMSK                  0x3800000
#define HWIO_QDSP6SS_PLL_CONFIG_CTL_TOGGLE_DET_THRESHOLD_SHFT                       0x17
#define HWIO_QDSP6SS_PLL_CONFIG_CTL_TOGGLE_DET_SAMPLE_BMSK                      0x700000
#define HWIO_QDSP6SS_PLL_CONFIG_CTL_TOGGLE_DET_SAMPLE_SHFT                          0x14
#define HWIO_QDSP6SS_PLL_CONFIG_CTL_LOCK_DET_THRESHOLD_BMSK                      0xff000
#define HWIO_QDSP6SS_PLL_CONFIG_CTL_LOCK_DET_THRESHOLD_SHFT                          0xc
#define HWIO_QDSP6SS_PLL_CONFIG_CTL_LOCK_DET_SAMPLE_SIZE_BMSK                      0xf00
#define HWIO_QDSP6SS_PLL_CONFIG_CTL_LOCK_DET_SAMPLE_SIZE_SHFT                        0x8
#define HWIO_QDSP6SS_PLL_CONFIG_CTL_GLITCH_THRESHOLD_BMSK                           0xc0
#define HWIO_QDSP6SS_PLL_CONFIG_CTL_GLITCH_THRESHOLD_SHFT                            0x6
#define HWIO_QDSP6SS_PLL_CONFIG_CTL_REF_CYCLE_BMSK                                  0x30
#define HWIO_QDSP6SS_PLL_CONFIG_CTL_REF_CYCLE_SHFT                                   0x4
#define HWIO_QDSP6SS_PLL_CONFIG_CTL_KFN_BMSK                                         0xf
#define HWIO_QDSP6SS_PLL_CONFIG_CTL_KFN_SHFT                                         0x0

#define HWIO_QDSP6SS_PLL_TEST_CTL_ADDR                                        (QDSP6V67SS_PUB_REG_BASE      + 0x0000021c)
#define HWIO_QDSP6SS_PLL_TEST_CTL_RMSK                                        0xffffffff
#define HWIO_QDSP6SS_PLL_TEST_CTL_IN          \
        in_dword_masked(HWIO_QDSP6SS_PLL_TEST_CTL_ADDR, HWIO_QDSP6SS_PLL_TEST_CTL_RMSK)
#define HWIO_QDSP6SS_PLL_TEST_CTL_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_PLL_TEST_CTL_ADDR, m)
#define HWIO_QDSP6SS_PLL_TEST_CTL_OUT(v)      \
        out_dword(HWIO_QDSP6SS_PLL_TEST_CTL_ADDR,v)
#define HWIO_QDSP6SS_PLL_TEST_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_PLL_TEST_CTL_ADDR,m,v,HWIO_QDSP6SS_PLL_TEST_CTL_IN)
#define HWIO_QDSP6SS_PLL_TEST_CTL_BIAS_GEN_TRIM_BMSK                          0xe0000000
#define HWIO_QDSP6SS_PLL_TEST_CTL_BIAS_GEN_TRIM_SHFT                                0x1d
#define HWIO_QDSP6SS_PLL_TEST_CTL_DCO_BMSK                                    0x10000000
#define HWIO_QDSP6SS_PLL_TEST_CTL_DCO_SHFT                                          0x1c
#define HWIO_QDSP6SS_PLL_TEST_CTL_PROCESS_CALB_BMSK                            0xc000000
#define HWIO_QDSP6SS_PLL_TEST_CTL_PROCESS_CALB_SHFT                                 0x1a
#define HWIO_QDSP6SS_PLL_TEST_CTL_OVERRIDE_PROCESS_CALB_BMSK                   0x2000000
#define HWIO_QDSP6SS_PLL_TEST_CTL_OVERRIDE_PROCESS_CALB_SHFT                        0x19
#define HWIO_QDSP6SS_PLL_TEST_CTL_FINE_FCW_BMSK                                0x1e00000
#define HWIO_QDSP6SS_PLL_TEST_CTL_FINE_FCW_SHFT                                     0x15
#define HWIO_QDSP6SS_PLL_TEST_CTL_OVERRIDE_FINE_FCW_BMSK                        0x100000
#define HWIO_QDSP6SS_PLL_TEST_CTL_OVERRIDE_FINE_FCW_SHFT                            0x14
#define HWIO_QDSP6SS_PLL_TEST_CTL_COARSE_FCW_BMSK                                0xfe000
#define HWIO_QDSP6SS_PLL_TEST_CTL_COARSE_FCW_SHFT                                    0xd
#define HWIO_QDSP6SS_PLL_TEST_CTL_OVERRIDE_COARSE_BMSK                            0x1000
#define HWIO_QDSP6SS_PLL_TEST_CTL_OVERRIDE_COARSE_SHFT                               0xc
#define HWIO_QDSP6SS_PLL_TEST_CTL_DISABLE_LFSR_BMSK                                0x800
#define HWIO_QDSP6SS_PLL_TEST_CTL_DISABLE_LFSR_SHFT                                  0xb
#define HWIO_QDSP6SS_PLL_TEST_CTL_DTEST_SEL_BMSK                                   0x700
#define HWIO_QDSP6SS_PLL_TEST_CTL_DTEST_SEL_SHFT                                     0x8
#define HWIO_QDSP6SS_PLL_TEST_CTL_DTEST_EN_BMSK                                     0x80
#define HWIO_QDSP6SS_PLL_TEST_CTL_DTEST_EN_SHFT                                      0x7
#define HWIO_QDSP6SS_PLL_TEST_CTL_BYP_TESTAMP_BMSK                                  0x40
#define HWIO_QDSP6SS_PLL_TEST_CTL_BYP_TESTAMP_SHFT                                   0x6
#define HWIO_QDSP6SS_PLL_TEST_CTL_ATEST1_SEL_BMSK                                   0x30
#define HWIO_QDSP6SS_PLL_TEST_CTL_ATEST1_SEL_SHFT                                    0x4
#define HWIO_QDSP6SS_PLL_TEST_CTL_ATEST0_SEL_BMSK                                    0xc
#define HWIO_QDSP6SS_PLL_TEST_CTL_ATEST0_SEL_SHFT                                    0x2
#define HWIO_QDSP6SS_PLL_TEST_CTL_ATEST1_EN_BMSK                                     0x2
#define HWIO_QDSP6SS_PLL_TEST_CTL_ATEST1_EN_SHFT                                     0x1
#define HWIO_QDSP6SS_PLL_TEST_CTL_ATEST0_EN_BMSK                                     0x1
#define HWIO_QDSP6SS_PLL_TEST_CTL_ATEST0_EN_SHFT                                     0x0

#define HWIO_QDSP6SS_PLL_TEST_CTL_U_ADDR                                      (QDSP6V67SS_PUB_REG_BASE      + 0x00000220)
#define HWIO_QDSP6SS_PLL_TEST_CTL_U_RMSK                                      0xffffffff
#define HWIO_QDSP6SS_PLL_TEST_CTL_U_IN          \
        in_dword_masked(HWIO_QDSP6SS_PLL_TEST_CTL_U_ADDR, HWIO_QDSP6SS_PLL_TEST_CTL_U_RMSK)
#define HWIO_QDSP6SS_PLL_TEST_CTL_U_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_PLL_TEST_CTL_U_ADDR, m)
#define HWIO_QDSP6SS_PLL_TEST_CTL_U_OUT(v)      \
        out_dword(HWIO_QDSP6SS_PLL_TEST_CTL_U_ADDR,v)
#define HWIO_QDSP6SS_PLL_TEST_CTL_U_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_PLL_TEST_CTL_U_ADDR,m,v,HWIO_QDSP6SS_PLL_TEST_CTL_U_IN)
#define HWIO_QDSP6SS_PLL_TEST_CTL_U_RESERVE_BITS31_14_BMSK                    0xffffc000
#define HWIO_QDSP6SS_PLL_TEST_CTL_U_RESERVE_BITS31_14_SHFT                           0xe
#define HWIO_QDSP6SS_PLL_TEST_CTL_U_OVERRIDE_FINE_FCW_MSB_BMSK                    0x2000
#define HWIO_QDSP6SS_PLL_TEST_CTL_U_OVERRIDE_FINE_FCW_MSB_SHFT                       0xd
#define HWIO_QDSP6SS_PLL_TEST_CTL_U_DTEST_MODE_SEL_BMSK                           0x1800
#define HWIO_QDSP6SS_PLL_TEST_CTL_U_DTEST_MODE_SEL_SHFT                              0xb
#define HWIO_QDSP6SS_PLL_TEST_CTL_U_NMO_OSC_SEL_BMSK                               0x600
#define HWIO_QDSP6SS_PLL_TEST_CTL_U_NMO_OSC_SEL_SHFT                                 0x9
#define HWIO_QDSP6SS_PLL_TEST_CTL_U_NMO_EN_BMSK                                    0x100
#define HWIO_QDSP6SS_PLL_TEST_CTL_U_NMO_EN_SHFT                                      0x8
#define HWIO_QDSP6SS_PLL_TEST_CTL_U_NOISE_MAG_BMSK                                  0xe0
#define HWIO_QDSP6SS_PLL_TEST_CTL_U_NOISE_MAG_SHFT                                   0x5
#define HWIO_QDSP6SS_PLL_TEST_CTL_U_NOISE_GEN_BMSK                                  0x10
#define HWIO_QDSP6SS_PLL_TEST_CTL_U_NOISE_GEN_SHFT                                   0x4
#define HWIO_QDSP6SS_PLL_TEST_CTL_U_OSC_BIAS_GND_BMSK                                0x8
#define HWIO_QDSP6SS_PLL_TEST_CTL_U_OSC_BIAS_GND_SHFT                                0x3
#define HWIO_QDSP6SS_PLL_TEST_CTL_U_PLL_TEST_OUT_SEL_BMSK                            0x6
#define HWIO_QDSP6SS_PLL_TEST_CTL_U_PLL_TEST_OUT_SEL_SHFT                            0x1
#define HWIO_QDSP6SS_PLL_TEST_CTL_U_CAL_CODE_UPDATE_BMSK                             0x1
#define HWIO_QDSP6SS_PLL_TEST_CTL_U_CAL_CODE_UPDATE_SHFT                             0x0

#define HWIO_QDSP6SS_PLL_STATUS_ADDR                                          (QDSP6V67SS_PUB_REG_BASE      + 0x00000224)
#define HWIO_QDSP6SS_PLL_STATUS_RMSK                                          0xffffffff
#define HWIO_QDSP6SS_PLL_STATUS_IN          \
        in_dword_masked(HWIO_QDSP6SS_PLL_STATUS_ADDR, HWIO_QDSP6SS_PLL_STATUS_RMSK)
#define HWIO_QDSP6SS_PLL_STATUS_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_PLL_STATUS_ADDR, m)
#define HWIO_QDSP6SS_PLL_STATUS_STATUS_31_0_BMSK                              0xffffffff
#define HWIO_QDSP6SS_PLL_STATUS_STATUS_31_0_SHFT                                     0x0

#define HWIO_QDSP6SS_PLL_FREQ_CTL_ADDR                                        (QDSP6V67SS_PUB_REG_BASE      + 0x00000228)
#define HWIO_QDSP6SS_PLL_FREQ_CTL_RMSK                                        0xffffffff
#define HWIO_QDSP6SS_PLL_FREQ_CTL_IN          \
        in_dword_masked(HWIO_QDSP6SS_PLL_FREQ_CTL_ADDR, HWIO_QDSP6SS_PLL_FREQ_CTL_RMSK)
#define HWIO_QDSP6SS_PLL_FREQ_CTL_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_PLL_FREQ_CTL_ADDR, m)
#define HWIO_QDSP6SS_PLL_FREQ_CTL_OUT(v)      \
        out_dword(HWIO_QDSP6SS_PLL_FREQ_CTL_ADDR,v)
#define HWIO_QDSP6SS_PLL_FREQ_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_PLL_FREQ_CTL_ADDR,m,v,HWIO_QDSP6SS_PLL_FREQ_CTL_IN)
#define HWIO_QDSP6SS_PLL_FREQ_CTL_FREQUENCY_CTL_WORD_BMSK                     0xffffffff
#define HWIO_QDSP6SS_PLL_FREQ_CTL_FREQUENCY_CTL_WORD_SHFT                            0x0

#define HWIO_QDSP6SS_PLL_RCG_UPDATE_STATUS_ADDR                               (QDSP6V67SS_PUB_REG_BASE      + 0x00000260)
#define HWIO_QDSP6SS_PLL_RCG_UPDATE_STATUS_RMSK                                      0x3
#define HWIO_QDSP6SS_PLL_RCG_UPDATE_STATUS_IN          \
        in_dword_masked(HWIO_QDSP6SS_PLL_RCG_UPDATE_STATUS_ADDR, HWIO_QDSP6SS_PLL_RCG_UPDATE_STATUS_RMSK)
#define HWIO_QDSP6SS_PLL_RCG_UPDATE_STATUS_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_PLL_RCG_UPDATE_STATUS_ADDR, m)
#define HWIO_QDSP6SS_PLL_RCG_UPDATE_STATUS_RCG2PLL_UPD_STATUS_BMSK                   0x2
#define HWIO_QDSP6SS_PLL_RCG_UPDATE_STATUS_RCG2PLL_UPD_STATUS_SHFT                   0x1
#define HWIO_QDSP6SS_PLL_RCG_UPDATE_STATUS_PLL2RCG_UPD_STATUS_BMSK                   0x1
#define HWIO_QDSP6SS_PLL_RCG_UPDATE_STATUS_PLL2RCG_UPD_STATUS_SHFT                   0x0

#define HWIO_QDSP6SS_PLL_RCG_UPDATE_CFG_ADDR                                  (QDSP6V67SS_PUB_REG_BASE      + 0x00000264)
#define HWIO_QDSP6SS_PLL_RCG_UPDATE_CFG_RMSK                                         0x3
#define HWIO_QDSP6SS_PLL_RCG_UPDATE_CFG_IN          \
        in_dword_masked(HWIO_QDSP6SS_PLL_RCG_UPDATE_CFG_ADDR, HWIO_QDSP6SS_PLL_RCG_UPDATE_CFG_RMSK)
#define HWIO_QDSP6SS_PLL_RCG_UPDATE_CFG_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_PLL_RCG_UPDATE_CFG_ADDR, m)
#define HWIO_QDSP6SS_PLL_RCG_UPDATE_CFG_OUT(v)      \
        out_dword(HWIO_QDSP6SS_PLL_RCG_UPDATE_CFG_ADDR,v)
#define HWIO_QDSP6SS_PLL_RCG_UPDATE_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_PLL_RCG_UPDATE_CFG_ADDR,m,v,HWIO_QDSP6SS_PLL_RCG_UPDATE_CFG_IN)
#define HWIO_QDSP6SS_PLL_RCG_UPDATE_CFG_RCG2PLL_UPD_EN_BMSK                          0x2
#define HWIO_QDSP6SS_PLL_RCG_UPDATE_CFG_RCG2PLL_UPD_EN_SHFT                          0x1
#define HWIO_QDSP6SS_PLL_RCG_UPDATE_CFG_PLL2RCG_UPD_EN_BMSK                          0x1
#define HWIO_QDSP6SS_PLL_RCG_UPDATE_CFG_PLL2RCG_UPD_EN_SHFT                          0x0

#define HWIO_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_ADDR                               (QDSP6V67SS_PUB_REG_BASE      + 0x00000268)
#define HWIO_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_RMSK                                     0x1f
#define HWIO_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_IN          \
        in_dword_masked(HWIO_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_ADDR, HWIO_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_RMSK)
#define HWIO_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_ADDR, m)
#define HWIO_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_OUT(v)      \
        out_dword(HWIO_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_ADDR,v)
#define HWIO_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_ADDR,m,v,HWIO_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_IN)
#define HWIO_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_MATCH_VALUE_BMSK                         0x1f
#define HWIO_QDSP6SS_PLL_RCG_UPDATE_DLYCTL_MATCH_VALUE_SHFT                          0x0

#define HWIO_QDSP6SS_PLL_RCG_UPDATE_CMD_ADDR                                  (QDSP6V67SS_PUB_REG_BASE      + 0x0000026c)
#define HWIO_QDSP6SS_PLL_RCG_UPDATE_CMD_RMSK                                         0x1
#define HWIO_QDSP6SS_PLL_RCG_UPDATE_CMD_OUT(v)      \
        out_dword(HWIO_QDSP6SS_PLL_RCG_UPDATE_CMD_ADDR,v)
#define HWIO_QDSP6SS_PLL_RCG_UPDATE_CMD_CMD_BMSK                                     0x1
#define HWIO_QDSP6SS_PLL_RCG_UPDATE_CMD_CMD_SHFT                                     0x0

#define HWIO_QDSP6SS_DBG_NMI_CFG_ADDR                                         (QDSP6V67SS_PUB_REG_BASE      + 0x00000300)
#define HWIO_QDSP6SS_DBG_NMI_CFG_RMSK                                                0xf
#define HWIO_QDSP6SS_DBG_NMI_CFG_IN          \
        in_dword_masked(HWIO_QDSP6SS_DBG_NMI_CFG_ADDR, HWIO_QDSP6SS_DBG_NMI_CFG_RMSK)
#define HWIO_QDSP6SS_DBG_NMI_CFG_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_DBG_NMI_CFG_ADDR, m)
#define HWIO_QDSP6SS_DBG_NMI_CFG_OUT(v)      \
        out_dword(HWIO_QDSP6SS_DBG_NMI_CFG_ADDR,v)
#define HWIO_QDSP6SS_DBG_NMI_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_DBG_NMI_CFG_ADDR,m,v,HWIO_QDSP6SS_DBG_NMI_CFG_IN)
#define HWIO_QDSP6SS_DBG_NMI_CFG_DBG_NMI_EN_IN_APCR_BMSK                             0x8
#define HWIO_QDSP6SS_DBG_NMI_CFG_DBG_NMI_EN_IN_APCR_SHFT                             0x3
#define HWIO_QDSP6SS_DBG_NMI_CFG_DBG_NMI_EN_IN_PWR_COLLAPSE_BMSK                     0x4
#define HWIO_QDSP6SS_DBG_NMI_CFG_DBG_NMI_EN_IN_PWR_COLLAPSE_SHFT                     0x2
#define HWIO_QDSP6SS_DBG_NMI_CFG_DBG_NMI_DISABLE_BMSK                                0x2
#define HWIO_QDSP6SS_DBG_NMI_CFG_DBG_NMI_DISABLE_SHFT                                0x1
#define HWIO_QDSP6SS_DBG_NMI_CFG_DBG_NMI_WAKEUP_DISABLE_BMSK                         0x1
#define HWIO_QDSP6SS_DBG_NMI_CFG_DBG_NMI_WAKEUP_DISABLE_SHFT                         0x0

#define HWIO_QDSP6SS_DBG_NMI_PWR_STATUS_ADDR                                  (QDSP6V67SS_PUB_REG_BASE      + 0x00000304)
#define HWIO_QDSP6SS_DBG_NMI_PWR_STATUS_RMSK                                     0x7000f
#define HWIO_QDSP6SS_DBG_NMI_PWR_STATUS_IN          \
        in_dword_masked(HWIO_QDSP6SS_DBG_NMI_PWR_STATUS_ADDR, HWIO_QDSP6SS_DBG_NMI_PWR_STATUS_RMSK)
#define HWIO_QDSP6SS_DBG_NMI_PWR_STATUS_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_DBG_NMI_PWR_STATUS_ADDR, m)
#define HWIO_QDSP6SS_DBG_NMI_PWR_STATUS_RSC_IN_SLEEP_BMSK                        0x40000
#define HWIO_QDSP6SS_DBG_NMI_PWR_STATUS_RSC_IN_SLEEP_SHFT                           0x12
#define HWIO_QDSP6SS_DBG_NMI_PWR_STATUS_QDSP6_CLK_GATING_BMSK                    0x20000
#define HWIO_QDSP6SS_DBG_NMI_PWR_STATUS_QDSP6_CLK_GATING_SHFT                       0x11
#define HWIO_QDSP6SS_DBG_NMI_PWR_STATUS_QDSP6_CORE_STATE_BMSK                    0x10000
#define HWIO_QDSP6SS_DBG_NMI_PWR_STATUS_QDSP6_CORE_STATE_SHFT                       0x10
#define HWIO_QDSP6SS_DBG_NMI_PWR_STATUS_NMI_CAUSE_BMSK                               0xc
#define HWIO_QDSP6SS_DBG_NMI_PWR_STATUS_NMI_CAUSE_SHFT                               0x2
#define HWIO_QDSP6SS_DBG_NMI_PWR_STATUS_QDSP6_PWR_COLLAPSE_BMSK                      0x2
#define HWIO_QDSP6SS_DBG_NMI_PWR_STATUS_QDSP6_PWR_COLLAPSE_SHFT                      0x1
#define HWIO_QDSP6SS_DBG_NMI_PWR_STATUS_QDSP6_IN_SLEEP_BMSK                          0x1
#define HWIO_QDSP6SS_DBG_NMI_PWR_STATUS_QDSP6_IN_SLEEP_SHFT                          0x0

#define HWIO_QDSP6SS_BOOT_CORE_START_ADDR                                     (QDSP6V67SS_PUB_REG_BASE      + 0x00000400)
#define HWIO_QDSP6SS_BOOT_CORE_START_RMSK                                            0x1
#define HWIO_QDSP6SS_BOOT_CORE_START_IN          \
        in_dword_masked(HWIO_QDSP6SS_BOOT_CORE_START_ADDR, HWIO_QDSP6SS_BOOT_CORE_START_RMSK)
#define HWIO_QDSP6SS_BOOT_CORE_START_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_BOOT_CORE_START_ADDR, m)
#define HWIO_QDSP6SS_BOOT_CORE_START_OUT(v)      \
        out_dword(HWIO_QDSP6SS_BOOT_CORE_START_ADDR,v)
#define HWIO_QDSP6SS_BOOT_CORE_START_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_BOOT_CORE_START_ADDR,m,v,HWIO_QDSP6SS_BOOT_CORE_START_IN)
#define HWIO_QDSP6SS_BOOT_CORE_START_START_BMSK                                      0x1
#define HWIO_QDSP6SS_BOOT_CORE_START_START_SHFT                                      0x0

#define HWIO_QDSP6SS_BOOT_CMD_ADDR                                            (QDSP6V67SS_PUB_REG_BASE      + 0x00000404)
#define HWIO_QDSP6SS_BOOT_CMD_RMSK                                                   0x1
#define HWIO_QDSP6SS_BOOT_CMD_OUT(v)      \
        out_dword(HWIO_QDSP6SS_BOOT_CMD_ADDR,v)
#define HWIO_QDSP6SS_BOOT_CMD_CMD_BMSK                                               0x1
#define HWIO_QDSP6SS_BOOT_CMD_CMD_SHFT                                               0x0

#define HWIO_QDSP6SS_BOOT_STATUS_ADDR                                         (QDSP6V67SS_PUB_REG_BASE      + 0x00000408)
#define HWIO_QDSP6SS_BOOT_STATUS_RMSK                                                0x1
#define HWIO_QDSP6SS_BOOT_STATUS_IN          \
        in_dword_masked(HWIO_QDSP6SS_BOOT_STATUS_ADDR, HWIO_QDSP6SS_BOOT_STATUS_RMSK)
#define HWIO_QDSP6SS_BOOT_STATUS_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_BOOT_STATUS_ADDR, m)
#define HWIO_QDSP6SS_BOOT_STATUS_STATUS_BMSK                                         0x1
#define HWIO_QDSP6SS_BOOT_STATUS_STATUS_SHFT                                         0x0

#define HWIO_QDSP6SS_CORE_BHS_CFG_ADDR                                        (QDSP6V67SS_PUB_REG_BASE      + 0x00000410)
#define HWIO_QDSP6SS_CORE_BHS_CFG_RMSK                                               0x1
#define HWIO_QDSP6SS_CORE_BHS_CFG_IN          \
        in_dword_masked(HWIO_QDSP6SS_CORE_BHS_CFG_ADDR, HWIO_QDSP6SS_CORE_BHS_CFG_RMSK)
#define HWIO_QDSP6SS_CORE_BHS_CFG_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_CORE_BHS_CFG_ADDR, m)
#define HWIO_QDSP6SS_CORE_BHS_CFG_OUT(v)      \
        out_dword(HWIO_QDSP6SS_CORE_BHS_CFG_ADDR,v)
#define HWIO_QDSP6SS_CORE_BHS_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_CORE_BHS_CFG_ADDR,m,v,HWIO_QDSP6SS_CORE_BHS_CFG_IN)
#define HWIO_QDSP6SS_CORE_BHS_CFG_BHS_ON_BMSK                                        0x1
#define HWIO_QDSP6SS_CORE_BHS_CFG_BHS_ON_SHFT                                        0x0

#define HWIO_QDSP6SS_CORE_BHS_CMD_ADDR                                        (QDSP6V67SS_PUB_REG_BASE      + 0x00000414)
#define HWIO_QDSP6SS_CORE_BHS_CMD_RMSK                                               0x1
#define HWIO_QDSP6SS_CORE_BHS_CMD_OUT(v)      \
        out_dword(HWIO_QDSP6SS_CORE_BHS_CMD_ADDR,v)
#define HWIO_QDSP6SS_CORE_BHS_CMD_UPDATE_BMSK                                        0x1
#define HWIO_QDSP6SS_CORE_BHS_CMD_UPDATE_SHFT                                        0x0

#define HWIO_QDSP6SS_CORE_BHS_STATUS_ADDR                                     (QDSP6V67SS_PUB_REG_BASE      + 0x00000418)
#define HWIO_QDSP6SS_CORE_BHS_STATUS_RMSK                                            0x1
#define HWIO_QDSP6SS_CORE_BHS_STATUS_IN          \
        in_dword_masked(HWIO_QDSP6SS_CORE_BHS_STATUS_ADDR, HWIO_QDSP6SS_CORE_BHS_STATUS_RMSK)
#define HWIO_QDSP6SS_CORE_BHS_STATUS_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_CORE_BHS_STATUS_ADDR, m)
#define HWIO_QDSP6SS_CORE_BHS_STATUS_BHS_ON_BMSK                                     0x1
#define HWIO_QDSP6SS_CORE_BHS_STATUS_BHS_ON_SHFT                                     0x0

#define HWIO_QDSP6SS_MEM_CFG_ADDR                                             (QDSP6V67SS_PUB_REG_BASE      + 0x00000430)
#define HWIO_QDSP6SS_MEM_CFG_RMSK                                               0x1f001f
#define HWIO_QDSP6SS_MEM_CFG_IN          \
        in_dword_masked(HWIO_QDSP6SS_MEM_CFG_ADDR, HWIO_QDSP6SS_MEM_CFG_RMSK)
#define HWIO_QDSP6SS_MEM_CFG_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_MEM_CFG_ADDR, m)
#define HWIO_QDSP6SS_MEM_CFG_OUT(v)      \
        out_dword(HWIO_QDSP6SS_MEM_CFG_ADDR,v)
#define HWIO_QDSP6SS_MEM_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_MEM_CFG_ADDR,m,v,HWIO_QDSP6SS_MEM_CFG_IN)
#define HWIO_QDSP6SS_MEM_CFG_L2MEM_SLP_RET_N_BMSK                               0x1e0000
#define HWIO_QDSP6SS_MEM_CFG_L2MEM_SLP_RET_N_SHFT                                   0x11
#define HWIO_QDSP6SS_MEM_CFG_MEM_SLP_RET_N_BMSK                                  0x10000
#define HWIO_QDSP6SS_MEM_CFG_MEM_SLP_RET_N_SHFT                                     0x10
#define HWIO_QDSP6SS_MEM_CFG_L2MEM_SLP_NRET_N_BMSK                                  0x1e
#define HWIO_QDSP6SS_MEM_CFG_L2MEM_SLP_NRET_N_SHFT                                   0x1
#define HWIO_QDSP6SS_MEM_CFG_MEM_SLP_NRET_N_BMSK                                     0x1
#define HWIO_QDSP6SS_MEM_CFG_MEM_SLP_NRET_N_SHFT                                     0x0

#define HWIO_QDSP6SS_MEM_CMD_ADDR                                             (QDSP6V67SS_PUB_REG_BASE      + 0x00000434)
#define HWIO_QDSP6SS_MEM_CMD_RMSK                                                    0x3
#define HWIO_QDSP6SS_MEM_CMD_OUT(v)      \
        out_dword(HWIO_QDSP6SS_MEM_CMD_ADDR,v)
#define HWIO_QDSP6SS_MEM_CMD_UPDATE_SLP_RET_N_BMSK                                   0x2
#define HWIO_QDSP6SS_MEM_CMD_UPDATE_SLP_RET_N_SHFT                                   0x1
#define HWIO_QDSP6SS_MEM_CMD_UPDATE_SLP_NRET_N_BMSK                                  0x1
#define HWIO_QDSP6SS_MEM_CMD_UPDATE_SLP_NRET_N_SHFT                                  0x0

#define HWIO_QDSP6SS_MEM_STATUS_ADDR                                          (QDSP6V67SS_PUB_REG_BASE      + 0x00000438)
#define HWIO_QDSP6SS_MEM_STATUS_RMSK                                            0x1f001f
#define HWIO_QDSP6SS_MEM_STATUS_IN          \
        in_dword_masked(HWIO_QDSP6SS_MEM_STATUS_ADDR, HWIO_QDSP6SS_MEM_STATUS_RMSK)
#define HWIO_QDSP6SS_MEM_STATUS_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_MEM_STATUS_ADDR, m)
#define HWIO_QDSP6SS_MEM_STATUS_L2MEM_SLP_RET_N_BMSK                            0x1e0000
#define HWIO_QDSP6SS_MEM_STATUS_L2MEM_SLP_RET_N_SHFT                                0x11
#define HWIO_QDSP6SS_MEM_STATUS_MEM_SLP_RET_N_BMSK                               0x10000
#define HWIO_QDSP6SS_MEM_STATUS_MEM_SLP_RET_N_SHFT                                  0x10
#define HWIO_QDSP6SS_MEM_STATUS_L2MEM_SLP_NRET_N_BMSK                               0x1e
#define HWIO_QDSP6SS_MEM_STATUS_L2MEM_SLP_NRET_N_SHFT                                0x1
#define HWIO_QDSP6SS_MEM_STATUS_MEM_SLP_NRET_N_BMSK                                  0x1
#define HWIO_QDSP6SS_MEM_STATUS_MEM_SLP_NRET_N_SHFT                                  0x0

#define HWIO_QDSP6SS_RESET_CFG_ADDR                                           (QDSP6V67SS_PUB_REG_BASE      + 0x00000440)
#define HWIO_QDSP6SS_RESET_CFG_RMSK                                               0xffff
#define HWIO_QDSP6SS_RESET_CFG_IN          \
        in_dword_masked(HWIO_QDSP6SS_RESET_CFG_ADDR, HWIO_QDSP6SS_RESET_CFG_RMSK)
#define HWIO_QDSP6SS_RESET_CFG_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_RESET_CFG_ADDR, m)
#define HWIO_QDSP6SS_RESET_CFG_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RESET_CFG_ADDR,v)
#define HWIO_QDSP6SS_RESET_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_RESET_CFG_ADDR,m,v,HWIO_QDSP6SS_RESET_CFG_IN)
#define HWIO_QDSP6SS_RESET_CFG_VPEVU_BMSK                                         0x8000
#define HWIO_QDSP6SS_RESET_CFG_VPEVU_SHFT                                            0xf
#define HWIO_QDSP6SS_RESET_CFG_NRET_BMSK                                          0x4000
#define HWIO_QDSP6SS_RESET_CFG_NRET_SHFT                                             0xe
#define HWIO_QDSP6SS_RESET_CFG_MNOC_BMSK                                          0x2000
#define HWIO_QDSP6SS_RESET_CFG_MNOC_SHFT                                             0xd
#define HWIO_QDSP6SS_RESET_CFG_APB_BMSK                                           0x1000
#define HWIO_QDSP6SS_RESET_CFG_APB_SHFT                                              0xc
#define HWIO_QDSP6SS_RESET_CFG_AXIS2_BMSK                                          0x800
#define HWIO_QDSP6SS_RESET_CFG_AXIS2_SHFT                                            0xb
#define HWIO_QDSP6SS_RESET_CFG_CSI1_BMSK                                           0x400
#define HWIO_QDSP6SS_RESET_CFG_CSI1_SHFT                                             0xa
#define HWIO_QDSP6SS_RESET_CFG_CSI0_BMSK                                           0x200
#define HWIO_QDSP6SS_RESET_CFG_CSI0_SHFT                                             0x9
#define HWIO_QDSP6SS_RESET_CFG_ARCH_BMSK                                           0x100
#define HWIO_QDSP6SS_RESET_CFG_ARCH_SHFT                                             0x8
#define HWIO_QDSP6SS_RESET_CFG_CORE_BMSK                                            0x80
#define HWIO_QDSP6SS_RESET_CFG_CORE_SHFT                                             0x7
#define HWIO_QDSP6SS_RESET_CFG_AXIM2_BMSK                                           0x40
#define HWIO_QDSP6SS_RESET_CFG_AXIM2_SHFT                                            0x6
#define HWIO_QDSP6SS_RESET_CFG_AXIS_BMSK                                            0x20
#define HWIO_QDSP6SS_RESET_CFG_AXIS_SHFT                                             0x5
#define HWIO_QDSP6SS_RESET_CFG_ATBM_BMSK                                            0x10
#define HWIO_QDSP6SS_RESET_CFG_ATBM_SHFT                                             0x4
#define HWIO_QDSP6SS_RESET_CFG_AHBM_BMSK                                             0x8
#define HWIO_QDSP6SS_RESET_CFG_AHBM_SHFT                                             0x3
#define HWIO_QDSP6SS_RESET_CFG_AXIM_BMSK                                             0x4
#define HWIO_QDSP6SS_RESET_CFG_AXIM_SHFT                                             0x2
#define HWIO_QDSP6SS_RESET_CFG_ISDB_ETM_BMSK                                         0x2
#define HWIO_QDSP6SS_RESET_CFG_ISDB_ETM_SHFT                                         0x1
#define HWIO_QDSP6SS_RESET_CFG_RET_BMSK                                              0x1
#define HWIO_QDSP6SS_RESET_CFG_RET_SHFT                                              0x0

#define HWIO_QDSP6SS_RESET_CMD_ADDR                                           (QDSP6V67SS_PUB_REG_BASE      + 0x00000444)
#define HWIO_QDSP6SS_RESET_CMD_RMSK                                                  0x1
#define HWIO_QDSP6SS_RESET_CMD_OUT(v)      \
        out_dword(HWIO_QDSP6SS_RESET_CMD_ADDR,v)
#define HWIO_QDSP6SS_RESET_CMD_UPDATE_BMSK                                           0x1
#define HWIO_QDSP6SS_RESET_CMD_UPDATE_SHFT                                           0x0

#define HWIO_QDSP6SS_RESET_STATUS_ADDR                                        (QDSP6V67SS_PUB_REG_BASE      + 0x00000448)
#define HWIO_QDSP6SS_RESET_STATUS_RMSK                                            0xffff
#define HWIO_QDSP6SS_RESET_STATUS_IN          \
        in_dword_masked(HWIO_QDSP6SS_RESET_STATUS_ADDR, HWIO_QDSP6SS_RESET_STATUS_RMSK)
#define HWIO_QDSP6SS_RESET_STATUS_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_RESET_STATUS_ADDR, m)
#define HWIO_QDSP6SS_RESET_STATUS_VPEVU_BMSK                                      0x8000
#define HWIO_QDSP6SS_RESET_STATUS_VPEVU_SHFT                                         0xf
#define HWIO_QDSP6SS_RESET_STATUS_NRET_BMSK                                       0x4000
#define HWIO_QDSP6SS_RESET_STATUS_NRET_SHFT                                          0xe
#define HWIO_QDSP6SS_RESET_STATUS_MNOC_BMSK                                       0x2000
#define HWIO_QDSP6SS_RESET_STATUS_MNOC_SHFT                                          0xd
#define HWIO_QDSP6SS_RESET_STATUS_APB_BMSK                                        0x1000
#define HWIO_QDSP6SS_RESET_STATUS_APB_SHFT                                           0xc
#define HWIO_QDSP6SS_RESET_STATUS_AXIS2_BMSK                                       0x800
#define HWIO_QDSP6SS_RESET_STATUS_AXIS2_SHFT                                         0xb
#define HWIO_QDSP6SS_RESET_STATUS_CSI1_BMSK                                        0x400
#define HWIO_QDSP6SS_RESET_STATUS_CSI1_SHFT                                          0xa
#define HWIO_QDSP6SS_RESET_STATUS_CSI0_BMSK                                        0x200
#define HWIO_QDSP6SS_RESET_STATUS_CSI0_SHFT                                          0x9
#define HWIO_QDSP6SS_RESET_STATUS_ARCH_BMSK                                        0x100
#define HWIO_QDSP6SS_RESET_STATUS_ARCH_SHFT                                          0x8
#define HWIO_QDSP6SS_RESET_STATUS_CORE_BMSK                                         0x80
#define HWIO_QDSP6SS_RESET_STATUS_CORE_SHFT                                          0x7
#define HWIO_QDSP6SS_RESET_STATUS_AXIM2_BMSK                                        0x40
#define HWIO_QDSP6SS_RESET_STATUS_AXIM2_SHFT                                         0x6
#define HWIO_QDSP6SS_RESET_STATUS_AXIS_BMSK                                         0x20
#define HWIO_QDSP6SS_RESET_STATUS_AXIS_SHFT                                          0x5
#define HWIO_QDSP6SS_RESET_STATUS_ATBM_BMSK                                         0x10
#define HWIO_QDSP6SS_RESET_STATUS_ATBM_SHFT                                          0x4
#define HWIO_QDSP6SS_RESET_STATUS_AHBM_BMSK                                          0x8
#define HWIO_QDSP6SS_RESET_STATUS_AHBM_SHFT                                          0x3
#define HWIO_QDSP6SS_RESET_STATUS_AXIM_BMSK                                          0x4
#define HWIO_QDSP6SS_RESET_STATUS_AXIM_SHFT                                          0x2
#define HWIO_QDSP6SS_RESET_STATUS_ISDB_ETM_BMSK                                      0x2
#define HWIO_QDSP6SS_RESET_STATUS_ISDB_ETM_SHFT                                      0x1
#define HWIO_QDSP6SS_RESET_STATUS_RET_BMSK                                           0x1
#define HWIO_QDSP6SS_RESET_STATUS_RET_SHFT                                           0x0

#define HWIO_QDSP6SS_CLAMP_CFG_ADDR                                           (QDSP6V67SS_PUB_REG_BASE      + 0x00000450)
#define HWIO_QDSP6SS_CLAMP_CFG_RMSK                                                  0xd
#define HWIO_QDSP6SS_CLAMP_CFG_IN          \
        in_dword_masked(HWIO_QDSP6SS_CLAMP_CFG_ADDR, HWIO_QDSP6SS_CLAMP_CFG_RMSK)
#define HWIO_QDSP6SS_CLAMP_CFG_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_CLAMP_CFG_ADDR, m)
#define HWIO_QDSP6SS_CLAMP_CFG_OUT(v)      \
        out_dword(HWIO_QDSP6SS_CLAMP_CFG_ADDR,v)
#define HWIO_QDSP6SS_CLAMP_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_CLAMP_CFG_ADDR,m,v,HWIO_QDSP6SS_CLAMP_CFG_IN)
#define HWIO_QDSP6SS_CLAMP_CFG_CLAMP_MSS2CX_BMSK                                     0x8
#define HWIO_QDSP6SS_CLAMP_CFG_CLAMP_MSS2CX_SHFT                                     0x3
#define HWIO_QDSP6SS_CLAMP_CFG_CLAMP_QMC_MEM_BMSK                                    0x4
#define HWIO_QDSP6SS_CLAMP_CFG_CLAMP_QMC_MEM_SHFT                                    0x2
#define HWIO_QDSP6SS_CLAMP_CFG_CLAMP_IO_BMSK                                         0x1
#define HWIO_QDSP6SS_CLAMP_CFG_CLAMP_IO_SHFT                                         0x0

#define HWIO_QDSP6SS_CLAMP_CMD_ADDR                                           (QDSP6V67SS_PUB_REG_BASE      + 0x00000454)
#define HWIO_QDSP6SS_CLAMP_CMD_RMSK                                                  0xd
#define HWIO_QDSP6SS_CLAMP_CMD_OUT(v)      \
        out_dword(HWIO_QDSP6SS_CLAMP_CMD_ADDR,v)
#define HWIO_QDSP6SS_CLAMP_CMD_UPDATE_CLAMP_MSS2CX_BMSK                              0x8
#define HWIO_QDSP6SS_CLAMP_CMD_UPDATE_CLAMP_MSS2CX_SHFT                              0x3
#define HWIO_QDSP6SS_CLAMP_CMD_UPDATE_CLAMP_QMC_MEM_BMSK                             0x4
#define HWIO_QDSP6SS_CLAMP_CMD_UPDATE_CLAMP_QMC_MEM_SHFT                             0x2
#define HWIO_QDSP6SS_CLAMP_CMD_UPDATE_CLAMP_IO_BMSK                                  0x1
#define HWIO_QDSP6SS_CLAMP_CMD_UPDATE_CLAMP_IO_SHFT                                  0x0

#define HWIO_QDSP6SS_CLAMP_STATUS_ADDR                                        (QDSP6V67SS_PUB_REG_BASE      + 0x00000458)
#define HWIO_QDSP6SS_CLAMP_STATUS_RMSK                                               0xd
#define HWIO_QDSP6SS_CLAMP_STATUS_IN          \
        in_dword_masked(HWIO_QDSP6SS_CLAMP_STATUS_ADDR, HWIO_QDSP6SS_CLAMP_STATUS_RMSK)
#define HWIO_QDSP6SS_CLAMP_STATUS_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_CLAMP_STATUS_ADDR, m)
#define HWIO_QDSP6SS_CLAMP_STATUS_CLAMP_MSS2CX_BMSK                                  0x8
#define HWIO_QDSP6SS_CLAMP_STATUS_CLAMP_MSS2CX_SHFT                                  0x3
#define HWIO_QDSP6SS_CLAMP_STATUS_CLAMP_QMC_MEM_BMSK                                 0x4
#define HWIO_QDSP6SS_CLAMP_STATUS_CLAMP_QMC_MEM_SHFT                                 0x2
#define HWIO_QDSP6SS_CLAMP_STATUS_CLAMP_IO_BMSK                                      0x1
#define HWIO_QDSP6SS_CLAMP_STATUS_CLAMP_IO_SHFT                                      0x0

#define HWIO_QDSP6SS_CLK_CFG_ADDR                                             (QDSP6V67SS_PUB_REG_BASE      + 0x00000460)
#define HWIO_QDSP6SS_CLK_CFG_RMSK                                                  0x7ff
#define HWIO_QDSP6SS_CLK_CFG_IN          \
        in_dword_masked(HWIO_QDSP6SS_CLK_CFG_ADDR, HWIO_QDSP6SS_CLK_CFG_RMSK)
#define HWIO_QDSP6SS_CLK_CFG_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_CLK_CFG_ADDR, m)
#define HWIO_QDSP6SS_CLK_CFG_OUT(v)      \
        out_dword(HWIO_QDSP6SS_CLK_CFG_ADDR,v)
#define HWIO_QDSP6SS_CLK_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_CLK_CFG_ADDR,m,v,HWIO_QDSP6SS_CLK_CFG_IN)
#define HWIO_QDSP6SS_CLK_CFG_VPEVU_BMSK                                            0x400
#define HWIO_QDSP6SS_CLK_CFG_VPEVU_SHFT                                              0xa
#define HWIO_QDSP6SS_CLK_CFG_AXIS2_BMSK                                            0x200
#define HWIO_QDSP6SS_CLK_CFG_AXIS2_SHFT                                              0x9
#define HWIO_QDSP6SS_CLK_CFG_CSI1_BMSK                                             0x100
#define HWIO_QDSP6SS_CLK_CFG_CSI1_SHFT                                               0x8
#define HWIO_QDSP6SS_CLK_CFG_CSI0_BMSK                                              0x80
#define HWIO_QDSP6SS_CLK_CFG_CSI0_SHFT                                               0x7
#define HWIO_QDSP6SS_CLK_CFG_AXIM2_BMSK                                             0x40
#define HWIO_QDSP6SS_CLK_CFG_AXIM2_SHFT                                              0x6
#define HWIO_QDSP6SS_CLK_CFG_AXIS_BMSK                                              0x20
#define HWIO_QDSP6SS_CLK_CFG_AXIS_SHFT                                               0x5
#define HWIO_QDSP6SS_CLK_CFG_ATBM_BMSK                                              0x10
#define HWIO_QDSP6SS_CLK_CFG_ATBM_SHFT                                               0x4
#define HWIO_QDSP6SS_CLK_CFG_AHBM_BMSK                                               0x8
#define HWIO_QDSP6SS_CLK_CFG_AHBM_SHFT                                               0x3
#define HWIO_QDSP6SS_CLK_CFG_AXIM_BMSK                                               0x4
#define HWIO_QDSP6SS_CLK_CFG_AXIM_SHFT                                               0x2
#define HWIO_QDSP6SS_CLK_CFG_APB_BMSK                                                0x2
#define HWIO_QDSP6SS_CLK_CFG_APB_SHFT                                                0x1
#define HWIO_QDSP6SS_CLK_CFG_CORE_BMSK                                               0x1
#define HWIO_QDSP6SS_CLK_CFG_CORE_SHFT                                               0x0

#define HWIO_QDSP6SS_CLK_CMD_ADDR                                             (QDSP6V67SS_PUB_REG_BASE      + 0x00000464)
#define HWIO_QDSP6SS_CLK_CMD_RMSK                                                    0x1
#define HWIO_QDSP6SS_CLK_CMD_OUT(v)      \
        out_dword(HWIO_QDSP6SS_CLK_CMD_ADDR,v)
#define HWIO_QDSP6SS_CLK_CMD_UPDATE_CLK_BMSK                                         0x1
#define HWIO_QDSP6SS_CLK_CMD_UPDATE_CLK_SHFT                                         0x0

#define HWIO_QDSP6SS_CLK_STATUS_ADDR                                          (QDSP6V67SS_PUB_REG_BASE      + 0x00000468)
#define HWIO_QDSP6SS_CLK_STATUS_RMSK                                               0x7ff
#define HWIO_QDSP6SS_CLK_STATUS_IN          \
        in_dword_masked(HWIO_QDSP6SS_CLK_STATUS_ADDR, HWIO_QDSP6SS_CLK_STATUS_RMSK)
#define HWIO_QDSP6SS_CLK_STATUS_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_CLK_STATUS_ADDR, m)
#define HWIO_QDSP6SS_CLK_STATUS_VPEVU_BMSK                                         0x400
#define HWIO_QDSP6SS_CLK_STATUS_VPEVU_SHFT                                           0xa
#define HWIO_QDSP6SS_CLK_STATUS_AXIS2_BMSK                                         0x200
#define HWIO_QDSP6SS_CLK_STATUS_AXIS2_SHFT                                           0x9
#define HWIO_QDSP6SS_CLK_STATUS_CSI1_BMSK                                          0x100
#define HWIO_QDSP6SS_CLK_STATUS_CSI1_SHFT                                            0x8
#define HWIO_QDSP6SS_CLK_STATUS_CSI0_BMSK                                           0x80
#define HWIO_QDSP6SS_CLK_STATUS_CSI0_SHFT                                            0x7
#define HWIO_QDSP6SS_CLK_STATUS_AXIM2_BMSK                                          0x40
#define HWIO_QDSP6SS_CLK_STATUS_AXIM2_SHFT                                           0x6
#define HWIO_QDSP6SS_CLK_STATUS_AXIS_BMSK                                           0x20
#define HWIO_QDSP6SS_CLK_STATUS_AXIS_SHFT                                            0x5
#define HWIO_QDSP6SS_CLK_STATUS_ATBM_BMSK                                           0x10
#define HWIO_QDSP6SS_CLK_STATUS_ATBM_SHFT                                            0x4
#define HWIO_QDSP6SS_CLK_STATUS_AHBM_BMSK                                            0x8
#define HWIO_QDSP6SS_CLK_STATUS_AHBM_SHFT                                            0x3
#define HWIO_QDSP6SS_CLK_STATUS_AXIM_BMSK                                            0x4
#define HWIO_QDSP6SS_CLK_STATUS_AXIM_SHFT                                            0x2
#define HWIO_QDSP6SS_CLK_STATUS_APB_BMSK                                             0x2
#define HWIO_QDSP6SS_CLK_STATUS_APB_SHFT                                             0x1
#define HWIO_QDSP6SS_CLK_STATUS_CORE_BMSK                                            0x1
#define HWIO_QDSP6SS_CLK_STATUS_CORE_SHFT                                            0x0

#define HWIO_QDSP6SS_MEM_STAGGER_RESET_CFG_ADDR                               (QDSP6V67SS_PUB_REG_BASE      + 0x00000470)
#define HWIO_QDSP6SS_MEM_STAGGER_RESET_CFG_RMSK                                      0x3
#define HWIO_QDSP6SS_MEM_STAGGER_RESET_CFG_IN          \
        in_dword_masked(HWIO_QDSP6SS_MEM_STAGGER_RESET_CFG_ADDR, HWIO_QDSP6SS_MEM_STAGGER_RESET_CFG_RMSK)
#define HWIO_QDSP6SS_MEM_STAGGER_RESET_CFG_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_MEM_STAGGER_RESET_CFG_ADDR, m)
#define HWIO_QDSP6SS_MEM_STAGGER_RESET_CFG_OUT(v)      \
        out_dword(HWIO_QDSP6SS_MEM_STAGGER_RESET_CFG_ADDR,v)
#define HWIO_QDSP6SS_MEM_STAGGER_RESET_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_MEM_STAGGER_RESET_CFG_ADDR,m,v,HWIO_QDSP6SS_MEM_STAGGER_RESET_CFG_IN)
#define HWIO_QDSP6SS_MEM_STAGGER_RESET_CFG_STAGGER_ALT_ARES_BMSK                     0x2
#define HWIO_QDSP6SS_MEM_STAGGER_RESET_CFG_STAGGER_ALT_ARES_SHFT                     0x1
#define HWIO_QDSP6SS_MEM_STAGGER_RESET_CFG_STAGGER_FUNC_ARES_BMSK                    0x1
#define HWIO_QDSP6SS_MEM_STAGGER_RESET_CFG_STAGGER_FUNC_ARES_SHFT                    0x0

#define HWIO_QDSP6SS_MEM_STAGGER_RESET_CMD_ADDR                               (QDSP6V67SS_PUB_REG_BASE      + 0x00000474)
#define HWIO_QDSP6SS_MEM_STAGGER_RESET_CMD_RMSK                                      0x1
#define HWIO_QDSP6SS_MEM_STAGGER_RESET_CMD_OUT(v)      \
        out_dword(HWIO_QDSP6SS_MEM_STAGGER_RESET_CMD_ADDR,v)
#define HWIO_QDSP6SS_MEM_STAGGER_RESET_CMD_UPDATE_BMSK                               0x1
#define HWIO_QDSP6SS_MEM_STAGGER_RESET_CMD_UPDATE_SHFT                               0x0

#define HWIO_QDSP6SS_MEM_STAGGER_RESET_STATUS_ADDR                            (QDSP6V67SS_PUB_REG_BASE      + 0x00000478)
#define HWIO_QDSP6SS_MEM_STAGGER_RESET_STATUS_RMSK                                   0x3
#define HWIO_QDSP6SS_MEM_STAGGER_RESET_STATUS_IN          \
        in_dword_masked(HWIO_QDSP6SS_MEM_STAGGER_RESET_STATUS_ADDR, HWIO_QDSP6SS_MEM_STAGGER_RESET_STATUS_RMSK)
#define HWIO_QDSP6SS_MEM_STAGGER_RESET_STATUS_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_MEM_STAGGER_RESET_STATUS_ADDR, m)
#define HWIO_QDSP6SS_MEM_STAGGER_RESET_STATUS_STAGGER_ALT_ARES_BMSK                  0x2
#define HWIO_QDSP6SS_MEM_STAGGER_RESET_STATUS_STAGGER_ALT_ARES_SHFT                  0x1
#define HWIO_QDSP6SS_MEM_STAGGER_RESET_STATUS_STAGGER_FUNC_ARES_BMSK                 0x1
#define HWIO_QDSP6SS_MEM_STAGGER_RESET_STATUS_STAGGER_FUNC_ARES_SHFT                 0x0

#define HWIO_QDSP6SS_MEM_STAGGER_RESTORE_CMD_ADDR                             (QDSP6V67SS_PUB_REG_BASE      + 0x00000484)
#define HWIO_QDSP6SS_MEM_STAGGER_RESTORE_CMD_RMSK                                    0x1
#define HWIO_QDSP6SS_MEM_STAGGER_RESTORE_CMD_OUT(v)      \
        out_dword(HWIO_QDSP6SS_MEM_STAGGER_RESTORE_CMD_ADDR,v)
#define HWIO_QDSP6SS_MEM_STAGGER_RESTORE_CMD_RESTORE_CMD_BMSK                        0x1
#define HWIO_QDSP6SS_MEM_STAGGER_RESTORE_CMD_RESTORE_CMD_SHFT                        0x0

#define HWIO_QDSP6SS_MEM_STAGGER_RESTORE_STATUS_ADDR                          (QDSP6V67SS_PUB_REG_BASE      + 0x00000488)
#define HWIO_QDSP6SS_MEM_STAGGER_RESTORE_STATUS_RMSK                                 0x1
#define HWIO_QDSP6SS_MEM_STAGGER_RESTORE_STATUS_OUT(v)      \
        out_dword(HWIO_QDSP6SS_MEM_STAGGER_RESTORE_STATUS_ADDR,v)
#define HWIO_QDSP6SS_MEM_STAGGER_RESTORE_STATUS_RESTORE_BUSY_BMSK                    0x1
#define HWIO_QDSP6SS_MEM_STAGGER_RESTORE_STATUS_RESTORE_BUSY_SHFT                    0x0

#define HWIO_QDSP6SS_CPMEM_CFG_ADDR                                           (QDSP6V67SS_PUB_REG_BASE      + 0x00000520)
#define HWIO_QDSP6SS_CPMEM_CFG_RMSK                                                  0x3
#define HWIO_QDSP6SS_CPMEM_CFG_IN          \
        in_dword_masked(HWIO_QDSP6SS_CPMEM_CFG_ADDR, HWIO_QDSP6SS_CPMEM_CFG_RMSK)
#define HWIO_QDSP6SS_CPMEM_CFG_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_CPMEM_CFG_ADDR, m)
#define HWIO_QDSP6SS_CPMEM_CFG_OUT(v)      \
        out_dword(HWIO_QDSP6SS_CPMEM_CFG_ADDR,v)
#define HWIO_QDSP6SS_CPMEM_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_CPMEM_CFG_ADDR,m,v,HWIO_QDSP6SS_CPMEM_CFG_IN)
#define HWIO_QDSP6SS_CPMEM_CFG_VTCM_SLP_RET_N_BMSK                                   0x2
#define HWIO_QDSP6SS_CPMEM_CFG_VTCM_SLP_RET_N_SHFT                                   0x1
#define HWIO_QDSP6SS_CPMEM_CFG_VTCM_SLP_NRET_N_BMSK                                  0x1
#define HWIO_QDSP6SS_CPMEM_CFG_VTCM_SLP_NRET_N_SHFT                                  0x0

#define HWIO_QDSP6SS_CPMEM_CMD_ADDR                                           (QDSP6V67SS_PUB_REG_BASE      + 0x00000524)
#define HWIO_QDSP6SS_CPMEM_CMD_RMSK                                                  0x3
#define HWIO_QDSP6SS_CPMEM_CMD_OUT(v)      \
        out_dword(HWIO_QDSP6SS_CPMEM_CMD_ADDR,v)
#define HWIO_QDSP6SS_CPMEM_CMD_UPDATE_VTCM_SLP_RET_N_BMSK                            0x2
#define HWIO_QDSP6SS_CPMEM_CMD_UPDATE_VTCM_SLP_RET_N_SHFT                            0x1
#define HWIO_QDSP6SS_CPMEM_CMD_UPDATE_VTCM_SLP_NRET_N_BMSK                           0x1
#define HWIO_QDSP6SS_CPMEM_CMD_UPDATE_VTCM_SLP_NRET_N_SHFT                           0x0

#define HWIO_QDSP6SS_CPMEM_STATUS_ADDR                                        (QDSP6V67SS_PUB_REG_BASE      + 0x00000528)
#define HWIO_QDSP6SS_CPMEM_STATUS_RMSK                                               0x3
#define HWIO_QDSP6SS_CPMEM_STATUS_IN          \
        in_dword_masked(HWIO_QDSP6SS_CPMEM_STATUS_ADDR, HWIO_QDSP6SS_CPMEM_STATUS_RMSK)
#define HWIO_QDSP6SS_CPMEM_STATUS_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_CPMEM_STATUS_ADDR, m)
#define HWIO_QDSP6SS_CPMEM_STATUS_VTCM_SLP_RET_N_BMSK                                0x2
#define HWIO_QDSP6SS_CPMEM_STATUS_VTCM_SLP_RET_N_SHFT                                0x1
#define HWIO_QDSP6SS_CPMEM_STATUS_VTCM_SLP_NRET_N_BMSK                               0x1
#define HWIO_QDSP6SS_CPMEM_STATUS_VTCM_SLP_NRET_N_SHFT                               0x0

#define HWIO_QDSP6SS_L2ITCM_CFG_ADDR                                          (QDSP6V67SS_PUB_REG_BASE      + 0x00000550)
#define HWIO_QDSP6SS_L2ITCM_CFG_RMSK                                             0x30003
#define HWIO_QDSP6SS_L2ITCM_CFG_IN          \
        in_dword_masked(HWIO_QDSP6SS_L2ITCM_CFG_ADDR, HWIO_QDSP6SS_L2ITCM_CFG_RMSK)
#define HWIO_QDSP6SS_L2ITCM_CFG_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_L2ITCM_CFG_ADDR, m)
#define HWIO_QDSP6SS_L2ITCM_CFG_OUT(v)      \
        out_dword(HWIO_QDSP6SS_L2ITCM_CFG_ADDR,v)
#define HWIO_QDSP6SS_L2ITCM_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_L2ITCM_CFG_ADDR,m,v,HWIO_QDSP6SS_L2ITCM_CFG_IN)
#define HWIO_QDSP6SS_L2ITCM_CFG_L2ITCM_SLP_RET_N_BMSK                            0x30000
#define HWIO_QDSP6SS_L2ITCM_CFG_L2ITCM_SLP_RET_N_SHFT                               0x10
#define HWIO_QDSP6SS_L2ITCM_CFG_L2ITCM_SLP_NRET_N_BMSK                               0x3
#define HWIO_QDSP6SS_L2ITCM_CFG_L2ITCM_SLP_NRET_N_SHFT                               0x0

#define HWIO_QDSP6SS_L2ITCM_CMD_ADDR                                          (QDSP6V67SS_PUB_REG_BASE      + 0x00000554)
#define HWIO_QDSP6SS_L2ITCM_CMD_RMSK                                                 0x3
#define HWIO_QDSP6SS_L2ITCM_CMD_OUT(v)      \
        out_dword(HWIO_QDSP6SS_L2ITCM_CMD_ADDR,v)
#define HWIO_QDSP6SS_L2ITCM_CMD_UPDATE_L2ITCM_SLP_RET_N_BMSK                         0x2
#define HWIO_QDSP6SS_L2ITCM_CMD_UPDATE_L2ITCM_SLP_RET_N_SHFT                         0x1
#define HWIO_QDSP6SS_L2ITCM_CMD_UPDATE_L2ITCM_SLP_NRET_N_BMSK                        0x1
#define HWIO_QDSP6SS_L2ITCM_CMD_UPDATE_L2ITCM_SLP_NRET_N_SHFT                        0x0

#define HWIO_QDSP6SS_L2ITCM_STATUS_ADDR                                       (QDSP6V67SS_PUB_REG_BASE      + 0x00000558)
#define HWIO_QDSP6SS_L2ITCM_STATUS_RMSK                                          0x30003
#define HWIO_QDSP6SS_L2ITCM_STATUS_IN          \
        in_dword_masked(HWIO_QDSP6SS_L2ITCM_STATUS_ADDR, HWIO_QDSP6SS_L2ITCM_STATUS_RMSK)
#define HWIO_QDSP6SS_L2ITCM_STATUS_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_L2ITCM_STATUS_ADDR, m)
#define HWIO_QDSP6SS_L2ITCM_STATUS_L2ITCM_SLP_RET_N_BMSK                         0x30000
#define HWIO_QDSP6SS_L2ITCM_STATUS_L2ITCM_SLP_RET_N_SHFT                            0x10
#define HWIO_QDSP6SS_L2ITCM_STATUS_L2ITCM_SLP_NRET_N_BMSK                            0x3
#define HWIO_QDSP6SS_L2ITCM_STATUS_L2ITCM_SLP_NRET_N_SHFT                            0x0

#define HWIO_QDSP6SS_L1SMEM_CFG_ADDR                                          (QDSP6V67SS_PUB_REG_BASE      + 0x00000540)
#define HWIO_QDSP6SS_L1SMEM_CFG_RMSK                                          0x3f3f3f3f
#define HWIO_QDSP6SS_L1SMEM_CFG_IN          \
        in_dword_masked(HWIO_QDSP6SS_L1SMEM_CFG_ADDR, HWIO_QDSP6SS_L1SMEM_CFG_RMSK)
#define HWIO_QDSP6SS_L1SMEM_CFG_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_L1SMEM_CFG_ADDR, m)
#define HWIO_QDSP6SS_L1SMEM_CFG_OUT(v)      \
        out_dword(HWIO_QDSP6SS_L1SMEM_CFG_ADDR,v)
#define HWIO_QDSP6SS_L1SMEM_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_L1SMEM_CFG_ADDR,m,v,HWIO_QDSP6SS_L1SMEM_CFG_IN)
#define HWIO_QDSP6SS_L1SMEM_CFG_L1S2_SLP_RET_N_BMSK                           0x3f000000
#define HWIO_QDSP6SS_L1SMEM_CFG_L1S2_SLP_RET_N_SHFT                                 0x18
#define HWIO_QDSP6SS_L1SMEM_CFG_L1S_SLP_RET_N_BMSK                              0x3f0000
#define HWIO_QDSP6SS_L1SMEM_CFG_L1S_SLP_RET_N_SHFT                                  0x10
#define HWIO_QDSP6SS_L1SMEM_CFG_L1S2_SLP_NRET_N_BMSK                              0x3f00
#define HWIO_QDSP6SS_L1SMEM_CFG_L1S2_SLP_NRET_N_SHFT                                 0x8
#define HWIO_QDSP6SS_L1SMEM_CFG_L1S_SLP_NRET_N_BMSK                                 0x3f
#define HWIO_QDSP6SS_L1SMEM_CFG_L1S_SLP_NRET_N_SHFT                                  0x0

#define HWIO_QDSP6SS_L1SMEM_CMD_ADDR                                          (QDSP6V67SS_PUB_REG_BASE      + 0x00000544)
#define HWIO_QDSP6SS_L1SMEM_CMD_RMSK                                                 0xf
#define HWIO_QDSP6SS_L1SMEM_CMD_OUT(v)      \
        out_dword(HWIO_QDSP6SS_L1SMEM_CMD_ADDR,v)
#define HWIO_QDSP6SS_L1SMEM_CMD_UPDATE_L1S2_SLP_RET_N_BMSK                           0x8
#define HWIO_QDSP6SS_L1SMEM_CMD_UPDATE_L1S2_SLP_RET_N_SHFT                           0x3
#define HWIO_QDSP6SS_L1SMEM_CMD_UPDATE_L1S2_SLP_NRET_N_BMSK                          0x4
#define HWIO_QDSP6SS_L1SMEM_CMD_UPDATE_L1S2_SLP_NRET_N_SHFT                          0x2
#define HWIO_QDSP6SS_L1SMEM_CMD_UPDATE_L1S_SLP_RET_N_BMSK                            0x2
#define HWIO_QDSP6SS_L1SMEM_CMD_UPDATE_L1S_SLP_RET_N_SHFT                            0x1
#define HWIO_QDSP6SS_L1SMEM_CMD_UPDATE_L1S_SLP_NRET_N_BMSK                           0x1
#define HWIO_QDSP6SS_L1SMEM_CMD_UPDATE_L1S_SLP_NRET_N_SHFT                           0x0

#define HWIO_QDSP6SS_L1SMEM_STATUS_ADDR                                       (QDSP6V67SS_PUB_REG_BASE      + 0x00000548)
#define HWIO_QDSP6SS_L1SMEM_STATUS_RMSK                                       0x3f3f3f3f
#define HWIO_QDSP6SS_L1SMEM_STATUS_IN          \
        in_dword_masked(HWIO_QDSP6SS_L1SMEM_STATUS_ADDR, HWIO_QDSP6SS_L1SMEM_STATUS_RMSK)
#define HWIO_QDSP6SS_L1SMEM_STATUS_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_L1SMEM_STATUS_ADDR, m)
#define HWIO_QDSP6SS_L1SMEM_STATUS_L1S2_SLP_RET_N_BMSK                        0x3f000000
#define HWIO_QDSP6SS_L1SMEM_STATUS_L1S2_SLP_RET_N_SHFT                              0x18
#define HWIO_QDSP6SS_L1SMEM_STATUS_L1S_SLP_RET_N_BMSK                           0x3f0000
#define HWIO_QDSP6SS_L1SMEM_STATUS_L1S_SLP_RET_N_SHFT                               0x10
#define HWIO_QDSP6SS_L1SMEM_STATUS_L1S2_SLP_NRET_N_BMSK                           0x3f00
#define HWIO_QDSP6SS_L1SMEM_STATUS_L1S2_SLP_NRET_N_SHFT                              0x8
#define HWIO_QDSP6SS_L1SMEM_STATUS_L1S_SLP_NRET_N_BMSK                              0x3f
#define HWIO_QDSP6SS_L1SMEM_STATUS_L1S_SLP_NRET_N_SHFT                               0x0

#define HWIO_QDSP6SS_VU0_PWR_VOTE_SET_ADDR                                    (QDSP6V67SS_PUB_REG_BASE      + 0x00000600)
#define HWIO_QDSP6SS_VU0_PWR_VOTE_SET_RMSK                                          0xff
#define HWIO_QDSP6SS_VU0_PWR_VOTE_SET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_VU0_PWR_VOTE_SET_ADDR,v)
#define HWIO_QDSP6SS_VU0_PWR_VOTE_SET_SET_BMSK                                      0xff
#define HWIO_QDSP6SS_VU0_PWR_VOTE_SET_SET_SHFT                                       0x0

#define HWIO_QDSP6SS_VU0_PWR_VOTE_CLR_ADDR                                    (QDSP6V67SS_PUB_REG_BASE      + 0x00000604)
#define HWIO_QDSP6SS_VU0_PWR_VOTE_CLR_RMSK                                         0xfff
#define HWIO_QDSP6SS_VU0_PWR_VOTE_CLR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_VU0_PWR_VOTE_CLR_ADDR,v)
#define HWIO_QDSP6SS_VU0_PWR_VOTE_CLR_CLR_BMSK                                     0xfff
#define HWIO_QDSP6SS_VU0_PWR_VOTE_CLR_CLR_SHFT                                       0x0

#define HWIO_QDSP6SS_VU0_PWR_VOTE_MASK_ADDR                                   (QDSP6V67SS_PUB_REG_BASE      + 0x00000608)
#define HWIO_QDSP6SS_VU0_PWR_VOTE_MASK_RMSK                                        0xfff
#define HWIO_QDSP6SS_VU0_PWR_VOTE_MASK_IN          \
        in_dword_masked(HWIO_QDSP6SS_VU0_PWR_VOTE_MASK_ADDR, HWIO_QDSP6SS_VU0_PWR_VOTE_MASK_RMSK)
#define HWIO_QDSP6SS_VU0_PWR_VOTE_MASK_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_VU0_PWR_VOTE_MASK_ADDR, m)
#define HWIO_QDSP6SS_VU0_PWR_VOTE_MASK_OUT(v)      \
        out_dword(HWIO_QDSP6SS_VU0_PWR_VOTE_MASK_ADDR,v)
#define HWIO_QDSP6SS_VU0_PWR_VOTE_MASK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_VU0_PWR_VOTE_MASK_ADDR,m,v,HWIO_QDSP6SS_VU0_PWR_VOTE_MASK_IN)
#define HWIO_QDSP6SS_VU0_PWR_VOTE_MASK_HW_VOTE_EN_BMSK                             0xf00
#define HWIO_QDSP6SS_VU0_PWR_VOTE_MASK_HW_VOTE_EN_SHFT                               0x8
#define HWIO_QDSP6SS_VU0_PWR_VOTE_MASK_VOTE_EN_BMSK                                 0xff
#define HWIO_QDSP6SS_VU0_PWR_VOTE_MASK_VOTE_EN_SHFT                                  0x0

#define HWIO_QDSP6SS_VU0_PWR_STATUS_ADDR                                      (QDSP6V67SS_PUB_REG_BASE      + 0x0000060c)
#define HWIO_QDSP6SS_VU0_PWR_STATUS_RMSK                                         0xfff1f
#define HWIO_QDSP6SS_VU0_PWR_STATUS_IN          \
        in_dword_masked(HWIO_QDSP6SS_VU0_PWR_STATUS_ADDR, HWIO_QDSP6SS_VU0_PWR_STATUS_RMSK)
#define HWIO_QDSP6SS_VU0_PWR_STATUS_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_VU0_PWR_STATUS_ADDR, m)
#define HWIO_QDSP6SS_VU0_PWR_STATUS_VOTE_STAT_BMSK                               0xfff00
#define HWIO_QDSP6SS_VU0_PWR_STATUS_VOTE_STAT_SHFT                                   0x8
#define HWIO_QDSP6SS_VU0_PWR_STATUS_WAIT_NEXT_BMSK                                  0x10
#define HWIO_QDSP6SS_VU0_PWR_STATUS_WAIT_NEXT_SHFT                                   0x4
#define HWIO_QDSP6SS_VU0_PWR_STATUS_WAIT_PREV_BMSK                                   0x8
#define HWIO_QDSP6SS_VU0_PWR_STATUS_WAIT_PREV_SHFT                                   0x3
#define HWIO_QDSP6SS_VU0_PWR_STATUS_PWR_DOWN_BMSK                                    0x4
#define HWIO_QDSP6SS_VU0_PWR_STATUS_PWR_DOWN_SHFT                                    0x2
#define HWIO_QDSP6SS_VU0_PWR_STATUS_PWR_REQ_BMSK                                     0x2
#define HWIO_QDSP6SS_VU0_PWR_STATUS_PWR_REQ_SHFT                                     0x1
#define HWIO_QDSP6SS_VU0_PWR_STATUS_PWR_ON_BMSK                                      0x1
#define HWIO_QDSP6SS_VU0_PWR_STATUS_PWR_ON_SHFT                                      0x0

#define HWIO_QDSP6SS_VU0_PWR_DEBUG_ADDR                                       (QDSP6V67SS_PUB_REG_BASE      + 0x00000610)
#define HWIO_QDSP6SS_VU0_PWR_DEBUG_RMSK                                             0xf7
#define HWIO_QDSP6SS_VU0_PWR_DEBUG_IN          \
        in_dword_masked(HWIO_QDSP6SS_VU0_PWR_DEBUG_ADDR, HWIO_QDSP6SS_VU0_PWR_DEBUG_RMSK)
#define HWIO_QDSP6SS_VU0_PWR_DEBUG_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_VU0_PWR_DEBUG_ADDR, m)
#define HWIO_QDSP6SS_VU0_PWR_DEBUG_OUT(v)      \
        out_dword(HWIO_QDSP6SS_VU0_PWR_DEBUG_ADDR,v)
#define HWIO_QDSP6SS_VU0_PWR_DEBUG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_VU0_PWR_DEBUG_ADDR,m,v,HWIO_QDSP6SS_VU0_PWR_DEBUG_IN)
#define HWIO_QDSP6SS_VU0_PWR_DEBUG_SEQ_DELAY_BMSK                                   0xf0
#define HWIO_QDSP6SS_VU0_PWR_DEBUG_SEQ_DELAY_SHFT                                    0x4
#define HWIO_QDSP6SS_VU0_PWR_DEBUG_CLAMP_IO_BMSK                                     0x4
#define HWIO_QDSP6SS_VU0_PWR_DEBUG_CLAMP_IO_SHFT                                     0x2
#define HWIO_QDSP6SS_VU0_PWR_DEBUG_CP_ARES_BMSK                                      0x2
#define HWIO_QDSP6SS_VU0_PWR_DEBUG_CP_ARES_SHFT                                      0x1
#define HWIO_QDSP6SS_VU0_PWR_DEBUG_CP_CLK_EN_BMSK                                    0x1
#define HWIO_QDSP6SS_VU0_PWR_DEBUG_CP_CLK_EN_SHFT                                    0x0

#define HWIO_QDSP6SS_VU1_PWR_VOTE_SET_ADDR                                    (QDSP6V67SS_PUB_REG_BASE      + 0x00000620)
#define HWIO_QDSP6SS_VU1_PWR_VOTE_SET_RMSK                                          0xff
#define HWIO_QDSP6SS_VU1_PWR_VOTE_SET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_VU1_PWR_VOTE_SET_ADDR,v)
#define HWIO_QDSP6SS_VU1_PWR_VOTE_SET_SET_BMSK                                      0xff
#define HWIO_QDSP6SS_VU1_PWR_VOTE_SET_SET_SHFT                                       0x0

#define HWIO_QDSP6SS_VU1_PWR_VOTE_CLR_ADDR                                    (QDSP6V67SS_PUB_REG_BASE      + 0x00000624)
#define HWIO_QDSP6SS_VU1_PWR_VOTE_CLR_RMSK                                         0xfff
#define HWIO_QDSP6SS_VU1_PWR_VOTE_CLR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_VU1_PWR_VOTE_CLR_ADDR,v)
#define HWIO_QDSP6SS_VU1_PWR_VOTE_CLR_CLR_BMSK                                     0xfff
#define HWIO_QDSP6SS_VU1_PWR_VOTE_CLR_CLR_SHFT                                       0x0

#define HWIO_QDSP6SS_VU1_PWR_VOTE_MASK_ADDR                                   (QDSP6V67SS_PUB_REG_BASE      + 0x00000628)
#define HWIO_QDSP6SS_VU1_PWR_VOTE_MASK_RMSK                                        0xfff
#define HWIO_QDSP6SS_VU1_PWR_VOTE_MASK_IN          \
        in_dword_masked(HWIO_QDSP6SS_VU1_PWR_VOTE_MASK_ADDR, HWIO_QDSP6SS_VU1_PWR_VOTE_MASK_RMSK)
#define HWIO_QDSP6SS_VU1_PWR_VOTE_MASK_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_VU1_PWR_VOTE_MASK_ADDR, m)
#define HWIO_QDSP6SS_VU1_PWR_VOTE_MASK_OUT(v)      \
        out_dword(HWIO_QDSP6SS_VU1_PWR_VOTE_MASK_ADDR,v)
#define HWIO_QDSP6SS_VU1_PWR_VOTE_MASK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_VU1_PWR_VOTE_MASK_ADDR,m,v,HWIO_QDSP6SS_VU1_PWR_VOTE_MASK_IN)
#define HWIO_QDSP6SS_VU1_PWR_VOTE_MASK_HW_VOTE_EN_BMSK                             0xf00
#define HWIO_QDSP6SS_VU1_PWR_VOTE_MASK_HW_VOTE_EN_SHFT                               0x8
#define HWIO_QDSP6SS_VU1_PWR_VOTE_MASK_VOTE_EN_BMSK                                 0xff
#define HWIO_QDSP6SS_VU1_PWR_VOTE_MASK_VOTE_EN_SHFT                                  0x0

#define HWIO_QDSP6SS_VU1_PWR_STATUS_ADDR                                      (QDSP6V67SS_PUB_REG_BASE      + 0x0000062c)
#define HWIO_QDSP6SS_VU1_PWR_STATUS_RMSK                                         0xfff1f
#define HWIO_QDSP6SS_VU1_PWR_STATUS_IN          \
        in_dword_masked(HWIO_QDSP6SS_VU1_PWR_STATUS_ADDR, HWIO_QDSP6SS_VU1_PWR_STATUS_RMSK)
#define HWIO_QDSP6SS_VU1_PWR_STATUS_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_VU1_PWR_STATUS_ADDR, m)
#define HWIO_QDSP6SS_VU1_PWR_STATUS_VOTE_STAT_BMSK                               0xfff00
#define HWIO_QDSP6SS_VU1_PWR_STATUS_VOTE_STAT_SHFT                                   0x8
#define HWIO_QDSP6SS_VU1_PWR_STATUS_WAIT_NEXT_BMSK                                  0x10
#define HWIO_QDSP6SS_VU1_PWR_STATUS_WAIT_NEXT_SHFT                                   0x4
#define HWIO_QDSP6SS_VU1_PWR_STATUS_WAIT_PREV_BMSK                                   0x8
#define HWIO_QDSP6SS_VU1_PWR_STATUS_WAIT_PREV_SHFT                                   0x3
#define HWIO_QDSP6SS_VU1_PWR_STATUS_PWR_DOWN_BMSK                                    0x4
#define HWIO_QDSP6SS_VU1_PWR_STATUS_PWR_DOWN_SHFT                                    0x2
#define HWIO_QDSP6SS_VU1_PWR_STATUS_PWR_REQ_BMSK                                     0x2
#define HWIO_QDSP6SS_VU1_PWR_STATUS_PWR_REQ_SHFT                                     0x1
#define HWIO_QDSP6SS_VU1_PWR_STATUS_PWR_ON_BMSK                                      0x1
#define HWIO_QDSP6SS_VU1_PWR_STATUS_PWR_ON_SHFT                                      0x0

#define HWIO_QDSP6SS_VU1_PWR_DEBUG_ADDR                                       (QDSP6V67SS_PUB_REG_BASE      + 0x00000630)
#define HWIO_QDSP6SS_VU1_PWR_DEBUG_RMSK                                             0xf7
#define HWIO_QDSP6SS_VU1_PWR_DEBUG_IN          \
        in_dword_masked(HWIO_QDSP6SS_VU1_PWR_DEBUG_ADDR, HWIO_QDSP6SS_VU1_PWR_DEBUG_RMSK)
#define HWIO_QDSP6SS_VU1_PWR_DEBUG_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_VU1_PWR_DEBUG_ADDR, m)
#define HWIO_QDSP6SS_VU1_PWR_DEBUG_OUT(v)      \
        out_dword(HWIO_QDSP6SS_VU1_PWR_DEBUG_ADDR,v)
#define HWIO_QDSP6SS_VU1_PWR_DEBUG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_VU1_PWR_DEBUG_ADDR,m,v,HWIO_QDSP6SS_VU1_PWR_DEBUG_IN)
#define HWIO_QDSP6SS_VU1_PWR_DEBUG_SEQ_DELAY_BMSK                                   0xf0
#define HWIO_QDSP6SS_VU1_PWR_DEBUG_SEQ_DELAY_SHFT                                    0x4
#define HWIO_QDSP6SS_VU1_PWR_DEBUG_CLAMP_IO_BMSK                                     0x4
#define HWIO_QDSP6SS_VU1_PWR_DEBUG_CLAMP_IO_SHFT                                     0x2
#define HWIO_QDSP6SS_VU1_PWR_DEBUG_CP_ARES_BMSK                                      0x2
#define HWIO_QDSP6SS_VU1_PWR_DEBUG_CP_ARES_SHFT                                      0x1
#define HWIO_QDSP6SS_VU1_PWR_DEBUG_CP_CLK_EN_BMSK                                    0x1
#define HWIO_QDSP6SS_VU1_PWR_DEBUG_CP_CLK_EN_SHFT                                    0x0

#define HWIO_QDSP6SS_VU2_PWR_VOTE_SET_ADDR                                    (QDSP6V67SS_PUB_REG_BASE      + 0x00000640)
#define HWIO_QDSP6SS_VU2_PWR_VOTE_SET_RMSK                                          0xff
#define HWIO_QDSP6SS_VU2_PWR_VOTE_SET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_VU2_PWR_VOTE_SET_ADDR,v)
#define HWIO_QDSP6SS_VU2_PWR_VOTE_SET_SET_BMSK                                      0xff
#define HWIO_QDSP6SS_VU2_PWR_VOTE_SET_SET_SHFT                                       0x0

#define HWIO_QDSP6SS_VU2_PWR_VOTE_CLR_ADDR                                    (QDSP6V67SS_PUB_REG_BASE      + 0x00000644)
#define HWIO_QDSP6SS_VU2_PWR_VOTE_CLR_RMSK                                         0xfff
#define HWIO_QDSP6SS_VU2_PWR_VOTE_CLR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_VU2_PWR_VOTE_CLR_ADDR,v)
#define HWIO_QDSP6SS_VU2_PWR_VOTE_CLR_CLR_BMSK                                     0xfff
#define HWIO_QDSP6SS_VU2_PWR_VOTE_CLR_CLR_SHFT                                       0x0

#define HWIO_QDSP6SS_VU2_PWR_VOTE_MASK_ADDR                                   (QDSP6V67SS_PUB_REG_BASE      + 0x00000648)
#define HWIO_QDSP6SS_VU2_PWR_VOTE_MASK_RMSK                                        0xfff
#define HWIO_QDSP6SS_VU2_PWR_VOTE_MASK_IN          \
        in_dword_masked(HWIO_QDSP6SS_VU2_PWR_VOTE_MASK_ADDR, HWIO_QDSP6SS_VU2_PWR_VOTE_MASK_RMSK)
#define HWIO_QDSP6SS_VU2_PWR_VOTE_MASK_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_VU2_PWR_VOTE_MASK_ADDR, m)
#define HWIO_QDSP6SS_VU2_PWR_VOTE_MASK_OUT(v)      \
        out_dword(HWIO_QDSP6SS_VU2_PWR_VOTE_MASK_ADDR,v)
#define HWIO_QDSP6SS_VU2_PWR_VOTE_MASK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_VU2_PWR_VOTE_MASK_ADDR,m,v,HWIO_QDSP6SS_VU2_PWR_VOTE_MASK_IN)
#define HWIO_QDSP6SS_VU2_PWR_VOTE_MASK_HW_VOTE_EN_BMSK                             0xf00
#define HWIO_QDSP6SS_VU2_PWR_VOTE_MASK_HW_VOTE_EN_SHFT                               0x8
#define HWIO_QDSP6SS_VU2_PWR_VOTE_MASK_VOTE_EN_BMSK                                 0xff
#define HWIO_QDSP6SS_VU2_PWR_VOTE_MASK_VOTE_EN_SHFT                                  0x0

#define HWIO_QDSP6SS_VU2_PWR_STATUS_ADDR                                      (QDSP6V67SS_PUB_REG_BASE      + 0x0000064c)
#define HWIO_QDSP6SS_VU2_PWR_STATUS_RMSK                                         0xfff1f
#define HWIO_QDSP6SS_VU2_PWR_STATUS_IN          \
        in_dword_masked(HWIO_QDSP6SS_VU2_PWR_STATUS_ADDR, HWIO_QDSP6SS_VU2_PWR_STATUS_RMSK)
#define HWIO_QDSP6SS_VU2_PWR_STATUS_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_VU2_PWR_STATUS_ADDR, m)
#define HWIO_QDSP6SS_VU2_PWR_STATUS_VOTE_STAT_BMSK                               0xfff00
#define HWIO_QDSP6SS_VU2_PWR_STATUS_VOTE_STAT_SHFT                                   0x8
#define HWIO_QDSP6SS_VU2_PWR_STATUS_WAIT_NEXT_BMSK                                  0x10
#define HWIO_QDSP6SS_VU2_PWR_STATUS_WAIT_NEXT_SHFT                                   0x4
#define HWIO_QDSP6SS_VU2_PWR_STATUS_WAIT_PREV_BMSK                                   0x8
#define HWIO_QDSP6SS_VU2_PWR_STATUS_WAIT_PREV_SHFT                                   0x3
#define HWIO_QDSP6SS_VU2_PWR_STATUS_PWR_DOWN_BMSK                                    0x4
#define HWIO_QDSP6SS_VU2_PWR_STATUS_PWR_DOWN_SHFT                                    0x2
#define HWIO_QDSP6SS_VU2_PWR_STATUS_PWR_REQ_BMSK                                     0x2
#define HWIO_QDSP6SS_VU2_PWR_STATUS_PWR_REQ_SHFT                                     0x1
#define HWIO_QDSP6SS_VU2_PWR_STATUS_PWR_ON_BMSK                                      0x1
#define HWIO_QDSP6SS_VU2_PWR_STATUS_PWR_ON_SHFT                                      0x0

#define HWIO_QDSP6SS_VU2_PWR_DEBUG_ADDR                                       (QDSP6V67SS_PUB_REG_BASE      + 0x00000650)
#define HWIO_QDSP6SS_VU2_PWR_DEBUG_RMSK                                             0xf7
#define HWIO_QDSP6SS_VU2_PWR_DEBUG_IN          \
        in_dword_masked(HWIO_QDSP6SS_VU2_PWR_DEBUG_ADDR, HWIO_QDSP6SS_VU2_PWR_DEBUG_RMSK)
#define HWIO_QDSP6SS_VU2_PWR_DEBUG_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_VU2_PWR_DEBUG_ADDR, m)
#define HWIO_QDSP6SS_VU2_PWR_DEBUG_OUT(v)      \
        out_dword(HWIO_QDSP6SS_VU2_PWR_DEBUG_ADDR,v)
#define HWIO_QDSP6SS_VU2_PWR_DEBUG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_VU2_PWR_DEBUG_ADDR,m,v,HWIO_QDSP6SS_VU2_PWR_DEBUG_IN)
#define HWIO_QDSP6SS_VU2_PWR_DEBUG_SEQ_DELAY_BMSK                                   0xf0
#define HWIO_QDSP6SS_VU2_PWR_DEBUG_SEQ_DELAY_SHFT                                    0x4
#define HWIO_QDSP6SS_VU2_PWR_DEBUG_CLAMP_IO_BMSK                                     0x4
#define HWIO_QDSP6SS_VU2_PWR_DEBUG_CLAMP_IO_SHFT                                     0x2
#define HWIO_QDSP6SS_VU2_PWR_DEBUG_CP_ARES_BMSK                                      0x2
#define HWIO_QDSP6SS_VU2_PWR_DEBUG_CP_ARES_SHFT                                      0x1
#define HWIO_QDSP6SS_VU2_PWR_DEBUG_CP_CLK_EN_BMSK                                    0x1
#define HWIO_QDSP6SS_VU2_PWR_DEBUG_CP_CLK_EN_SHFT                                    0x0

#define HWIO_QDSP6SS_VU3_PWR_VOTE_SET_ADDR                                    (QDSP6V67SS_PUB_REG_BASE      + 0x00000660)
#define HWIO_QDSP6SS_VU3_PWR_VOTE_SET_RMSK                                          0xff
#define HWIO_QDSP6SS_VU3_PWR_VOTE_SET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_VU3_PWR_VOTE_SET_ADDR,v)
#define HWIO_QDSP6SS_VU3_PWR_VOTE_SET_SET_BMSK                                      0xff
#define HWIO_QDSP6SS_VU3_PWR_VOTE_SET_SET_SHFT                                       0x0

#define HWIO_QDSP6SS_VU3_PWR_VOTE_CLR_ADDR                                    (QDSP6V67SS_PUB_REG_BASE      + 0x00000664)
#define HWIO_QDSP6SS_VU3_PWR_VOTE_CLR_RMSK                                         0xfff
#define HWIO_QDSP6SS_VU3_PWR_VOTE_CLR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_VU3_PWR_VOTE_CLR_ADDR,v)
#define HWIO_QDSP6SS_VU3_PWR_VOTE_CLR_CLR_BMSK                                     0xfff
#define HWIO_QDSP6SS_VU3_PWR_VOTE_CLR_CLR_SHFT                                       0x0

#define HWIO_QDSP6SS_VU3_PWR_VOTE_MASK_ADDR                                   (QDSP6V67SS_PUB_REG_BASE      + 0x00000668)
#define HWIO_QDSP6SS_VU3_PWR_VOTE_MASK_RMSK                                        0xfff
#define HWIO_QDSP6SS_VU3_PWR_VOTE_MASK_IN          \
        in_dword_masked(HWIO_QDSP6SS_VU3_PWR_VOTE_MASK_ADDR, HWIO_QDSP6SS_VU3_PWR_VOTE_MASK_RMSK)
#define HWIO_QDSP6SS_VU3_PWR_VOTE_MASK_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_VU3_PWR_VOTE_MASK_ADDR, m)
#define HWIO_QDSP6SS_VU3_PWR_VOTE_MASK_OUT(v)      \
        out_dword(HWIO_QDSP6SS_VU3_PWR_VOTE_MASK_ADDR,v)
#define HWIO_QDSP6SS_VU3_PWR_VOTE_MASK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_VU3_PWR_VOTE_MASK_ADDR,m,v,HWIO_QDSP6SS_VU3_PWR_VOTE_MASK_IN)
#define HWIO_QDSP6SS_VU3_PWR_VOTE_MASK_HW_VOTE_EN_BMSK                             0xf00
#define HWIO_QDSP6SS_VU3_PWR_VOTE_MASK_HW_VOTE_EN_SHFT                               0x8
#define HWIO_QDSP6SS_VU3_PWR_VOTE_MASK_VOTE_EN_BMSK                                 0xff
#define HWIO_QDSP6SS_VU3_PWR_VOTE_MASK_VOTE_EN_SHFT                                  0x0

#define HWIO_QDSP6SS_VU3_PWR_STATUS_ADDR                                      (QDSP6V67SS_PUB_REG_BASE      + 0x0000066c)
#define HWIO_QDSP6SS_VU3_PWR_STATUS_RMSK                                         0xfff1f
#define HWIO_QDSP6SS_VU3_PWR_STATUS_IN          \
        in_dword_masked(HWIO_QDSP6SS_VU3_PWR_STATUS_ADDR, HWIO_QDSP6SS_VU3_PWR_STATUS_RMSK)
#define HWIO_QDSP6SS_VU3_PWR_STATUS_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_VU3_PWR_STATUS_ADDR, m)
#define HWIO_QDSP6SS_VU3_PWR_STATUS_VOTE_STAT_BMSK                               0xfff00
#define HWIO_QDSP6SS_VU3_PWR_STATUS_VOTE_STAT_SHFT                                   0x8
#define HWIO_QDSP6SS_VU3_PWR_STATUS_WAIT_NEXT_BMSK                                  0x10
#define HWIO_QDSP6SS_VU3_PWR_STATUS_WAIT_NEXT_SHFT                                   0x4
#define HWIO_QDSP6SS_VU3_PWR_STATUS_WAIT_PREV_BMSK                                   0x8
#define HWIO_QDSP6SS_VU3_PWR_STATUS_WAIT_PREV_SHFT                                   0x3
#define HWIO_QDSP6SS_VU3_PWR_STATUS_PWR_DOWN_BMSK                                    0x4
#define HWIO_QDSP6SS_VU3_PWR_STATUS_PWR_DOWN_SHFT                                    0x2
#define HWIO_QDSP6SS_VU3_PWR_STATUS_PWR_REQ_BMSK                                     0x2
#define HWIO_QDSP6SS_VU3_PWR_STATUS_PWR_REQ_SHFT                                     0x1
#define HWIO_QDSP6SS_VU3_PWR_STATUS_PWR_ON_BMSK                                      0x1
#define HWIO_QDSP6SS_VU3_PWR_STATUS_PWR_ON_SHFT                                      0x0

#define HWIO_QDSP6SS_VU3_PWR_DEBUG_ADDR                                       (QDSP6V67SS_PUB_REG_BASE      + 0x00000670)
#define HWIO_QDSP6SS_VU3_PWR_DEBUG_RMSK                                             0xf7
#define HWIO_QDSP6SS_VU3_PWR_DEBUG_IN          \
        in_dword_masked(HWIO_QDSP6SS_VU3_PWR_DEBUG_ADDR, HWIO_QDSP6SS_VU3_PWR_DEBUG_RMSK)
#define HWIO_QDSP6SS_VU3_PWR_DEBUG_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_VU3_PWR_DEBUG_ADDR, m)
#define HWIO_QDSP6SS_VU3_PWR_DEBUG_OUT(v)      \
        out_dword(HWIO_QDSP6SS_VU3_PWR_DEBUG_ADDR,v)
#define HWIO_QDSP6SS_VU3_PWR_DEBUG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_VU3_PWR_DEBUG_ADDR,m,v,HWIO_QDSP6SS_VU3_PWR_DEBUG_IN)
#define HWIO_QDSP6SS_VU3_PWR_DEBUG_SEQ_DELAY_BMSK                                   0xf0
#define HWIO_QDSP6SS_VU3_PWR_DEBUG_SEQ_DELAY_SHFT                                    0x4
#define HWIO_QDSP6SS_VU3_PWR_DEBUG_CLAMP_IO_BMSK                                     0x4
#define HWIO_QDSP6SS_VU3_PWR_DEBUG_CLAMP_IO_SHFT                                     0x2
#define HWIO_QDSP6SS_VU3_PWR_DEBUG_CP_ARES_BMSK                                      0x2
#define HWIO_QDSP6SS_VU3_PWR_DEBUG_CP_ARES_SHFT                                      0x1
#define HWIO_QDSP6SS_VU3_PWR_DEBUG_CP_CLK_EN_BMSK                                    0x1
#define HWIO_QDSP6SS_VU3_PWR_DEBUG_CP_CLK_EN_SHFT                                    0x0

#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_CFG_ADDR                                  (QDSP6V67SS_PUB_REG_BASE      + 0x00000680)
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_CFG_RMSK                                         0x1
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_CFG_IN          \
        in_dword_masked(HWIO_QDSP6SS_VUSRCH_CSF_PWR_CFG_ADDR, HWIO_QDSP6SS_VUSRCH_CSF_PWR_CFG_RMSK)
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_CFG_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_VUSRCH_CSF_PWR_CFG_ADDR, m)
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_CFG_OUT(v)      \
        out_dword(HWIO_QDSP6SS_VUSRCH_CSF_PWR_CFG_ADDR,v)
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_VUSRCH_CSF_PWR_CFG_ADDR,m,v,HWIO_QDSP6SS_VUSRCH_CSF_PWR_CFG_IN)
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_CFG_MEM_OFF_BMSK                                 0x1
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_CFG_MEM_OFF_SHFT                                 0x0

#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_VOTE_SET_ADDR                             (QDSP6V67SS_PUB_REG_BASE      + 0x00000684)
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_VOTE_SET_RMSK                                   0xff
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_VOTE_SET_OUT(v)      \
        out_dword(HWIO_QDSP6SS_VUSRCH_CSF_PWR_VOTE_SET_ADDR,v)
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_VOTE_SET_SET_BMSK                               0xff
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_VOTE_SET_SET_SHFT                                0x0

#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_VOTE_CLR_ADDR                             (QDSP6V67SS_PUB_REG_BASE      + 0x00000688)
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_VOTE_CLR_RMSK                                   0xff
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_VOTE_CLR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_VUSRCH_CSF_PWR_VOTE_CLR_ADDR,v)
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_VOTE_CLR_CLR_BMSK                               0xff
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_VOTE_CLR_CLR_SHFT                                0x0

#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_VOTE_MASK_ADDR                            (QDSP6V67SS_PUB_REG_BASE      + 0x0000068c)
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_VOTE_MASK_RMSK                                  0xff
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_VOTE_MASK_IN          \
        in_dword_masked(HWIO_QDSP6SS_VUSRCH_CSF_PWR_VOTE_MASK_ADDR, HWIO_QDSP6SS_VUSRCH_CSF_PWR_VOTE_MASK_RMSK)
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_VOTE_MASK_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_VUSRCH_CSF_PWR_VOTE_MASK_ADDR, m)
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_VOTE_MASK_OUT(v)      \
        out_dword(HWIO_QDSP6SS_VUSRCH_CSF_PWR_VOTE_MASK_ADDR,v)
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_VOTE_MASK_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_VUSRCH_CSF_PWR_VOTE_MASK_ADDR,m,v,HWIO_QDSP6SS_VUSRCH_CSF_PWR_VOTE_MASK_IN)
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_VOTE_MASK_VOTE_EN_BMSK                          0xff
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_VOTE_MASK_VOTE_EN_SHFT                           0x0

#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_STATUS_ADDR                               (QDSP6V67SS_PUB_REG_BASE      + 0x00000690)
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_STATUS_RMSK                                  0xfffdf
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_STATUS_IN          \
        in_dword_masked(HWIO_QDSP6SS_VUSRCH_CSF_PWR_STATUS_ADDR, HWIO_QDSP6SS_VUSRCH_CSF_PWR_STATUS_RMSK)
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_STATUS_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_VUSRCH_CSF_PWR_STATUS_ADDR, m)
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_STATUS_VOTE_STAT_BMSK                        0xfff00
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_STATUS_VOTE_STAT_SHFT                            0x8
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_STATUS_SLP_RET_N_BMSK                           0x80
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_STATUS_SLP_RET_N_SHFT                            0x7
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_STATUS_SLP_NRET_N_BMSK                          0x40
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_STATUS_SLP_NRET_N_SHFT                           0x6
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_STATUS_WAIT_NEXT_BMSK                           0x10
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_STATUS_WAIT_NEXT_SHFT                            0x4
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_STATUS_WAIT_PREV_BMSK                            0x8
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_STATUS_WAIT_PREV_SHFT                            0x3
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_STATUS_PWR_DOWN_BMSK                             0x4
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_STATUS_PWR_DOWN_SHFT                             0x2
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_STATUS_PWR_REQ_BMSK                              0x2
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_STATUS_PWR_REQ_SHFT                              0x1
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_STATUS_PWR_ON_BMSK                               0x1
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_STATUS_PWR_ON_SHFT                               0x0

#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_DEBUG_ADDR                                (QDSP6V67SS_PUB_REG_BASE      + 0x00000694)
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_DEBUG_RMSK                                      0xf7
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_DEBUG_IN          \
        in_dword_masked(HWIO_QDSP6SS_VUSRCH_CSF_PWR_DEBUG_ADDR, HWIO_QDSP6SS_VUSRCH_CSF_PWR_DEBUG_RMSK)
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_DEBUG_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_VUSRCH_CSF_PWR_DEBUG_ADDR, m)
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_DEBUG_OUT(v)      \
        out_dword(HWIO_QDSP6SS_VUSRCH_CSF_PWR_DEBUG_ADDR,v)
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_DEBUG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_VUSRCH_CSF_PWR_DEBUG_ADDR,m,v,HWIO_QDSP6SS_VUSRCH_CSF_PWR_DEBUG_IN)
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_DEBUG_SEQ_DELAY_BMSK                            0xf0
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_DEBUG_SEQ_DELAY_SHFT                             0x4
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_DEBUG_CLAMP_IO_BMSK                              0x4
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_DEBUG_CLAMP_IO_SHFT                              0x2
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_DEBUG_CP_ARES_BMSK                               0x2
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_DEBUG_CP_ARES_SHFT                               0x1
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_DEBUG_CP_CLK_EN_BMSK                             0x1
#define HWIO_QDSP6SS_VUSRCH_CSF_PWR_DEBUG_CP_CLK_EN_SHFT                             0x0

#define HWIO_QDSP6SS_VU_QOS_ADDR                                              (QDSP6V67SS_PUB_REG_BASE      + 0x000006a0)
#define HWIO_QDSP6SS_VU_QOS_RMSK                                                    0x77
#define HWIO_QDSP6SS_VU_QOS_IN          \
        in_dword_masked(HWIO_QDSP6SS_VU_QOS_ADDR, HWIO_QDSP6SS_VU_QOS_RMSK)
#define HWIO_QDSP6SS_VU_QOS_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_VU_QOS_ADDR, m)
#define HWIO_QDSP6SS_VU_QOS_OUT(v)      \
        out_dword(HWIO_QDSP6SS_VU_QOS_ADDR,v)
#define HWIO_QDSP6SS_VU_QOS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_VU_QOS_ADDR,m,v,HWIO_QDSP6SS_VU_QOS_IN)
#define HWIO_QDSP6SS_VU_QOS_QOS_AXIS_BMSK                                           0x70
#define HWIO_QDSP6SS_VU_QOS_QOS_AXIS_SHFT                                            0x4
#define HWIO_QDSP6SS_VU_QOS_QOS_MNOC_BMSK                                            0x7
#define HWIO_QDSP6SS_VU_QOS_QOS_MNOC_SHFT                                            0x0

#define HWIO_QDSP6SS_VPE_VU_CBCR_ADDR                                         (QDSP6V67SS_PUB_REG_BASE      + 0x000006b0)
#define HWIO_QDSP6SS_VPE_VU_CBCR_RMSK                                         0x80000001
#define HWIO_QDSP6SS_VPE_VU_CBCR_IN          \
        in_dword_masked(HWIO_QDSP6SS_VPE_VU_CBCR_ADDR, HWIO_QDSP6SS_VPE_VU_CBCR_RMSK)
#define HWIO_QDSP6SS_VPE_VU_CBCR_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_VPE_VU_CBCR_ADDR, m)
#define HWIO_QDSP6SS_VPE_VU_CBCR_OUT(v)      \
        out_dword(HWIO_QDSP6SS_VPE_VU_CBCR_ADDR,v)
#define HWIO_QDSP6SS_VPE_VU_CBCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_VPE_VU_CBCR_ADDR,m,v,HWIO_QDSP6SS_VPE_VU_CBCR_IN)
#define HWIO_QDSP6SS_VPE_VU_CBCR_CLKOFF_BMSK                                  0x80000000
#define HWIO_QDSP6SS_VPE_VU_CBCR_CLKOFF_SHFT                                        0x1f
#define HWIO_QDSP6SS_VPE_VU_CBCR_CLKEN_BMSK                                          0x1
#define HWIO_QDSP6SS_VPE_VU_CBCR_CLKEN_SHFT                                          0x0

#define HWIO_QDSP6SS_TEST_BUS_CTL_ADDR                                        (QDSP6V67SS_PUB_REG_BASE      + 0x00002000)
#define HWIO_QDSP6SS_TEST_BUS_CTL_RMSK                                        0xffffffff
#define HWIO_QDSP6SS_TEST_BUS_CTL_IN          \
        in_dword_masked(HWIO_QDSP6SS_TEST_BUS_CTL_ADDR, HWIO_QDSP6SS_TEST_BUS_CTL_RMSK)
#define HWIO_QDSP6SS_TEST_BUS_CTL_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_TEST_BUS_CTL_ADDR, m)
#define HWIO_QDSP6SS_TEST_BUS_CTL_OUT(v)      \
        out_dword(HWIO_QDSP6SS_TEST_BUS_CTL_ADDR,v)
#define HWIO_QDSP6SS_TEST_BUS_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_TEST_BUS_CTL_ADDR,m,v,HWIO_QDSP6SS_TEST_BUS_CTL_IN)
#define HWIO_QDSP6SS_TEST_BUS_CTL_SPARE_CFG_BMSK                              0xff000000
#define HWIO_QDSP6SS_TEST_BUS_CTL_SPARE_CFG_SHFT                                    0x18
#define HWIO_QDSP6SS_TEST_BUS_CTL_HWE_INV_BMSK                                  0x800000
#define HWIO_QDSP6SS_TEST_BUS_CTL_HWE_INV_SHFT                                      0x17
#define HWIO_QDSP6SS_TEST_BUS_CTL_HWE_SEL_BMSK                                  0x7c0000
#define HWIO_QDSP6SS_TEST_BUS_CTL_HWE_SEL_SHFT                                      0x12
#define HWIO_QDSP6SS_TEST_BUS_CTL_TESTBUS_EN_BMSK                                0x20000
#define HWIO_QDSP6SS_TEST_BUS_CTL_TESTBUS_EN_SHFT                                   0x11
#define HWIO_QDSP6SS_TEST_BUS_CTL_Q6_SEL_BMSK                                    0x10000
#define HWIO_QDSP6SS_TEST_BUS_CTL_Q6_SEL_SHFT                                       0x10
#define HWIO_QDSP6SS_TEST_BUS_CTL_SEL_BMSK                                        0xffff
#define HWIO_QDSP6SS_TEST_BUS_CTL_SEL_SHFT                                           0x0

#define HWIO_QDSP6SS_TEST_BUS_VALUE_ADDR                                      (QDSP6V67SS_PUB_REG_BASE      + 0x00002004)
#define HWIO_QDSP6SS_TEST_BUS_VALUE_RMSK                                      0xffffffff
#define HWIO_QDSP6SS_TEST_BUS_VALUE_IN          \
        in_dword_masked(HWIO_QDSP6SS_TEST_BUS_VALUE_ADDR, HWIO_QDSP6SS_TEST_BUS_VALUE_RMSK)
#define HWIO_QDSP6SS_TEST_BUS_VALUE_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_TEST_BUS_VALUE_ADDR, m)
#define HWIO_QDSP6SS_TEST_BUS_VALUE_VALUE_BMSK                                0xffffffff
#define HWIO_QDSP6SS_TEST_BUS_VALUE_VALUE_SHFT                                       0x0

#define HWIO_QDSP6SS_TEST_BUS_WDATA_ADDR                                      (QDSP6V67SS_PUB_REG_BASE      + 0x00002008)
#define HWIO_QDSP6SS_TEST_BUS_WDATA_RMSK                                      0xffffffff
#define HWIO_QDSP6SS_TEST_BUS_WDATA_IN          \
        in_dword_masked(HWIO_QDSP6SS_TEST_BUS_WDATA_ADDR, HWIO_QDSP6SS_TEST_BUS_WDATA_RMSK)
#define HWIO_QDSP6SS_TEST_BUS_WDATA_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_TEST_BUS_WDATA_ADDR, m)
#define HWIO_QDSP6SS_TEST_BUS_WDATA_OUT(v)      \
        out_dword(HWIO_QDSP6SS_TEST_BUS_WDATA_ADDR,v)
#define HWIO_QDSP6SS_TEST_BUS_WDATA_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_TEST_BUS_WDATA_ADDR,m,v,HWIO_QDSP6SS_TEST_BUS_WDATA_IN)
#define HWIO_QDSP6SS_TEST_BUS_WDATA_VALUE_BMSK                                0xffffffff
#define HWIO_QDSP6SS_TEST_BUS_WDATA_VALUE_SHFT                                       0x0

#define HWIO_QDSP6SS_PWRDBG_CTL_ADDR                                          (QDSP6V67SS_PUB_REG_BASE      + 0x0000200c)
#define HWIO_QDSP6SS_PWRDBG_CTL_RMSK                                                 0x1
#define HWIO_QDSP6SS_PWRDBG_CTL_IN          \
        in_dword_masked(HWIO_QDSP6SS_PWRDBG_CTL_ADDR, HWIO_QDSP6SS_PWRDBG_CTL_RMSK)
#define HWIO_QDSP6SS_PWRDBG_CTL_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_PWRDBG_CTL_ADDR, m)
#define HWIO_QDSP6SS_PWRDBG_CTL_OUT(v)      \
        out_dword(HWIO_QDSP6SS_PWRDBG_CTL_ADDR,v)
#define HWIO_QDSP6SS_PWRDBG_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_PWRDBG_CTL_ADDR,m,v,HWIO_QDSP6SS_PWRDBG_CTL_IN)
#define HWIO_QDSP6SS_PWRDBG_CTL_ENA_BMSK                                             0x1
#define HWIO_QDSP6SS_PWRDBG_CTL_ENA_SHFT                                             0x0

#define HWIO_QDSP6SS_TEST_CLK_CTL_ADDR                                        (QDSP6V67SS_PUB_REG_BASE      + 0x00002010)
#define HWIO_QDSP6SS_TEST_CLK_CTL_RMSK                                              0x7f
#define HWIO_QDSP6SS_TEST_CLK_CTL_IN          \
        in_dword_masked(HWIO_QDSP6SS_TEST_CLK_CTL_ADDR, HWIO_QDSP6SS_TEST_CLK_CTL_RMSK)
#define HWIO_QDSP6SS_TEST_CLK_CTL_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_TEST_CLK_CTL_ADDR, m)
#define HWIO_QDSP6SS_TEST_CLK_CTL_OUT(v)      \
        out_dword(HWIO_QDSP6SS_TEST_CLK_CTL_ADDR,v)
#define HWIO_QDSP6SS_TEST_CLK_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_TEST_CLK_CTL_ADDR,m,v,HWIO_QDSP6SS_TEST_CLK_CTL_IN)
#define HWIO_QDSP6SS_TEST_CLK_CTL_Q6_DBG_CLK_EN_BMSK                                0x40
#define HWIO_QDSP6SS_TEST_CLK_CTL_Q6_DBG_CLK_EN_SHFT                                 0x6
#define HWIO_QDSP6SS_TEST_CLK_CTL_Q6_DBG_CLK_INV_BMSK                               0x20
#define HWIO_QDSP6SS_TEST_CLK_CTL_Q6_DBG_CLK_INV_SHFT                                0x5
#define HWIO_QDSP6SS_TEST_CLK_CTL_DBG_CLK_SEL_BMSK                                  0x1f
#define HWIO_QDSP6SS_TEST_CLK_CTL_DBG_CLK_SEL_SHFT                                   0x0

#define HWIO_QDSP6SS_BHS_TEST_ADDR                                            (QDSP6V67SS_PUB_REG_BASE      + 0x00002014)
#define HWIO_QDSP6SS_BHS_TEST_RMSK                                                   0x1
#define HWIO_QDSP6SS_BHS_TEST_IN          \
        in_dword_masked(HWIO_QDSP6SS_BHS_TEST_ADDR, HWIO_QDSP6SS_BHS_TEST_RMSK)
#define HWIO_QDSP6SS_BHS_TEST_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_BHS_TEST_ADDR, m)
#define HWIO_QDSP6SS_BHS_TEST_OUT(v)      \
        out_dword(HWIO_QDSP6SS_BHS_TEST_ADDR,v)
#define HWIO_QDSP6SS_BHS_TEST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_BHS_TEST_ADDR,m,v,HWIO_QDSP6SS_BHS_TEST_IN)
#define HWIO_QDSP6SS_BHS_TEST_INSTANT_ON_BMSK                                        0x1
#define HWIO_QDSP6SS_BHS_TEST_INSTANT_ON_SHFT                                        0x0

#define HWIO_QDSP6SS_ZMEAS_CONFIG_ADDR                                        (QDSP6V67SS_PUB_REG_BASE      + 0x00002018)
#define HWIO_QDSP6SS_ZMEAS_CONFIG_RMSK                                          0xffffff
#define HWIO_QDSP6SS_ZMEAS_CONFIG_IN          \
        in_dword_masked(HWIO_QDSP6SS_ZMEAS_CONFIG_ADDR, HWIO_QDSP6SS_ZMEAS_CONFIG_RMSK)
#define HWIO_QDSP6SS_ZMEAS_CONFIG_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_ZMEAS_CONFIG_ADDR, m)
#define HWIO_QDSP6SS_ZMEAS_CONFIG_OUT(v)      \
        out_dword(HWIO_QDSP6SS_ZMEAS_CONFIG_ADDR,v)
#define HWIO_QDSP6SS_ZMEAS_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_ZMEAS_CONFIG_ADDR,m,v,HWIO_QDSP6SS_ZMEAS_CONFIG_IN)
#define HWIO_QDSP6SS_ZMEAS_CONFIG_ZMEAS_COUNT_BMSK                              0xffffe0
#define HWIO_QDSP6SS_ZMEAS_CONFIG_ZMEAS_COUNT_SHFT                                   0x5
#define HWIO_QDSP6SS_ZMEAS_CONFIG_MUX_SEL_BMSK                                      0x10
#define HWIO_QDSP6SS_ZMEAS_CONFIG_MUX_SEL_SHFT                                       0x4
#define HWIO_QDSP6SS_ZMEAS_CONFIG_ZMEAS_CLK_R_ENA_BMSK                               0x8
#define HWIO_QDSP6SS_ZMEAS_CONFIG_ZMEAS_CLK_R_ENA_SHFT                               0x3
#define HWIO_QDSP6SS_ZMEAS_CONFIG_ZMEAS_CGC_EN_BMSK                                  0x4
#define HWIO_QDSP6SS_ZMEAS_CONFIG_ZMEAS_CGC_EN_SHFT                                  0x2
#define HWIO_QDSP6SS_ZMEAS_CONFIG_ZMEAS_BYPASS_BMSK                                  0x2
#define HWIO_QDSP6SS_ZMEAS_CONFIG_ZMEAS_BYPASS_SHFT                                  0x1
#define HWIO_QDSP6SS_ZMEAS_CONFIG_ZMEAS_SW_RESET_BMSK                                0x1
#define HWIO_QDSP6SS_ZMEAS_CONFIG_ZMEAS_SW_RESET_SHFT                                0x0

#define HWIO_QDSP6SS_CORE_STATUS_ADDR                                         (QDSP6V67SS_PUB_REG_BASE      + 0x00002028)
#define HWIO_QDSP6SS_CORE_STATUS_RMSK                                               0x3f
#define HWIO_QDSP6SS_CORE_STATUS_IN          \
        in_dword_masked(HWIO_QDSP6SS_CORE_STATUS_ADDR, HWIO_QDSP6SS_CORE_STATUS_RMSK)
#define HWIO_QDSP6SS_CORE_STATUS_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_CORE_STATUS_ADDR, m)
#define HWIO_QDSP6SS_CORE_STATUS_VU1_VX_IDLE_BMSK                                   0x20
#define HWIO_QDSP6SS_CORE_STATUS_VU1_VX_IDLE_SHFT                                    0x5
#define HWIO_QDSP6SS_CORE_STATUS_VU0_VX_IDLE_BMSK                                   0x10
#define HWIO_QDSP6SS_CORE_STATUS_VU0_VX_IDLE_SHFT                                    0x4
#define HWIO_QDSP6SS_CORE_STATUS_VU_OVERALL_IDLE_BMSK                                0x8
#define HWIO_QDSP6SS_CORE_STATUS_VU_OVERALL_IDLE_SHFT                                0x3
#define HWIO_QDSP6SS_CORE_STATUS_CP0_IDLE_BMSK                                       0x4
#define HWIO_QDSP6SS_CORE_STATUS_CP0_IDLE_SHFT                                       0x2
#define HWIO_QDSP6SS_CORE_STATUS_CORE_IN_SLEEP_BMSK                                  0x2
#define HWIO_QDSP6SS_CORE_STATUS_CORE_IN_SLEEP_SHFT                                  0x1
#define HWIO_QDSP6SS_CORE_STATUS_CORE_STATE_BMSK                                     0x1
#define HWIO_QDSP6SS_CORE_STATUS_CORE_STATE_SHFT                                     0x0

#define HWIO_QDSP6SS_MEM_DEBUG_ADDR                                           (QDSP6V67SS_PUB_REG_BASE      + 0x00002030)
#define HWIO_QDSP6SS_MEM_DEBUG_RMSK                                               0xffff
#define HWIO_QDSP6SS_MEM_DEBUG_IN          \
        in_dword_masked(HWIO_QDSP6SS_MEM_DEBUG_ADDR, HWIO_QDSP6SS_MEM_DEBUG_RMSK)
#define HWIO_QDSP6SS_MEM_DEBUG_INM(m)      \
        in_dword_masked(HWIO_QDSP6SS_MEM_DEBUG_ADDR, m)
#define HWIO_QDSP6SS_MEM_DEBUG_OUT(v)      \
        out_dword(HWIO_QDSP6SS_MEM_DEBUG_ADDR,v)
#define HWIO_QDSP6SS_MEM_DEBUG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QDSP6SS_MEM_DEBUG_ADDR,m,v,HWIO_QDSP6SS_MEM_DEBUG_IN)
#define HWIO_QDSP6SS_MEM_DEBUG_OFF_BMSK                                           0xffff
#define HWIO_QDSP6SS_MEM_DEBUG_OFF_SHFT                                              0x0


#endif /* __USLEEP_HALHWIO_H__ */
