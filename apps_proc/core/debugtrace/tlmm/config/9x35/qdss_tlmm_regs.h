#ifndef __QDSS_TLMM_REGS_H__
#define __QDSS_TLMM_REGS_H__
/*
===========================================================================
*/
/**
  @file qdss_tlmm_regs.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    TLMM_CSR

  'Include' filters applied: TLMM_ETM_MODE*[TLMM_CSR] TLMM_SDC2_HDRV*[TLMM_CSR] 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2013 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/core.tx/6.0/debugtrace/tlmm/config/9x35/qdss_tlmm_regs.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: TLMM_CSR
 *--------------------------------------------------------------------------*/

#define TLMM_CSR_REG_BASE                                                 (TLMM_BASE      + 0x00010000)

#define HWIO_TLMM_ETM_MODE_ADDR                                           (TLMM_CSR_REG_BASE      + 0x00002014)
#define HWIO_TLMM_ETM_MODE_RMSK                                                  0x1
#define HWIO_TLMM_ETM_MODE_POR                                            0x00000000
#define HWIO_TLMM_ETM_MODE_IN          \
        in_dword_masked(HWIO_TLMM_ETM_MODE_ADDR, HWIO_TLMM_ETM_MODE_RMSK)
#define HWIO_TLMM_ETM_MODE_INM(m)      \
        in_dword_masked(HWIO_TLMM_ETM_MODE_ADDR, m)
#define HWIO_TLMM_ETM_MODE_OUT(v)      \
        out_dword(HWIO_TLMM_ETM_MODE_ADDR,v)
#define HWIO_TLMM_ETM_MODE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_ETM_MODE_ADDR,m,v,HWIO_TLMM_ETM_MODE_IN)
#define HWIO_TLMM_ETM_MODE_TRACE_OVER_SDC2_BMSK                                  0x1
#define HWIO_TLMM_ETM_MODE_TRACE_OVER_SDC2_SHFT                                  0x0

#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR                                 (TLMM_CSR_REG_BASE      + 0x00002048)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_RMSK                                  0xfffffff
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_POR                                  0x00000bdb
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_IN          \
        in_dword_masked(HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR, HWIO_TLMM_SDC2_HDRV_PULL_CTL_RMSK)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_INM(m)      \
        in_dword_masked(HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR, m)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_OUT(v)      \
        out_dword(HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR,v)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_TLMM_SDC2_HDRV_PULL_CTL_ADDR,m,v,HWIO_TLMM_SDC2_HDRV_PULL_CTL_IN)
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SMT_DATA3_HDRV_BMSK                   0xe000000
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SMT_DATA3_HDRV_SHFT                        0x19
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SMT_DATA2_HDRV_BMSK                   0x1c00000
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SMT_DATA2_HDRV_SHFT                        0x16
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SMT_DATA1_HDRV_BMSK                    0x380000
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SMT_DATA1_HDRV_SHFT                        0x13
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SMT_DATA0_HDRV_BMSK                     0x70000
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SMT_DATA0_HDRV_SHFT                        0x10
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CLK_PULL_BMSK                       0xc000
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CLK_PULL_SHFT                          0xe
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_HYS_CTL_BMSK                        0x2000
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_HYS_CTL_SHFT                           0xd
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CMD_PULL_BMSK                       0x1800
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CMD_PULL_SHFT                          0xb
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_DATA_PULL_BMSK                       0x600
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_DATA_PULL_SHFT                         0x9
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CLK_HDRV_BMSK                        0x1c0
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CLK_HDRV_SHFT                          0x6
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CMD_HDRV_BMSK                         0x38
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_CMD_HDRV_SHFT                          0x3
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_DATA_HDRV_BMSK                         0x7
#define HWIO_TLMM_SDC2_HDRV_PULL_CTL_SDC2_DATA_HDRV_SHFT                         0x0


#endif /* __QDSS_TLMM_REGS_H__ */
