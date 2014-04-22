#ifndef __CLOCKLITEHWIO_H__
#define __CLOCKLITEHWIO_H__
/*
===========================================================================
*/
/**
  @file ClockLiteHWIO.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    MDM9205 (Twizy) [twizy_v1.0_p3q3r35_MTO]
 
  This file contains HWIO register definitions for the following modules:
    GCC_CLK_CTL_REG
    APCS_F0_QTMR_V1

  'Include' filters applied: CNTPCT_LO[APCS_F0_QTMR_V1] GCC_APCS_CLOCK_BRANCH_ENA_VOTE[GCC_CLK_CTL_REG] GCC_PRNG_AHB_CBCR[GCC_CLK_CTL_REG] 

  Generation parameters: 
  { u'filename': u'ClockLiteHWIO.h',
    u'header': u'#include <HALhwio.h>\n#include <msmhwiobase.h>\n\n#define PROC_CLK_BRANCH_ENA_VOTE   GCC_APCS_CLOCK_BRANCH_ENA_VOTE',
    u'module-filter-exclude': { },
    u'module-filter-include': { u'APCS_F0_QTMR_V1': [u'CNTPCT_LO'],
                                u'GCC_CLK_CTL_REG': [ u'GCC_APCS_CLOCK_BRANCH_ENA_VOTE',
                                                      u'GCC_PRNG_AHB_CBCR']},
    u'modules': [u'GCC_CLK_CTL_REG', u'APCS_F0_QTMR_V1'],
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

  $Header: //components/rel/core.tx/6.0/systemdrivers/clock/hw/mdm9205/apps/inc/ClockLiteHWIO.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include <HALhwio.h>
#include <msmhwiobase.h>

#define PROC_CLK_BRANCH_ENA_VOTE   GCC_APCS_CLOCK_BRANCH_ENA_VOTE

/*----------------------------------------------------------------------------
 * MODULE: GCC_CLK_CTL_REG
 *--------------------------------------------------------------------------*/

#define GCC_CLK_CTL_REG_REG_BASE                                                              (CLK_CTL_BASE      + 0x00000000)
#define GCC_CLK_CTL_REG_REG_BASE_SIZE                                                         0x80000
#define GCC_CLK_CTL_REG_REG_BASE_USED                                                         0x7f000

#define HWIO_GCC_PRNG_AHB_CBCR_ADDR                                                           (GCC_CLK_CTL_REG_REG_BASE      + 0x00013004)
#define HWIO_GCC_PRNG_AHB_CBCR_RMSK                                                           0x80000000
#define HWIO_GCC_PRNG_AHB_CBCR_IN          \
        in_dword_masked(HWIO_GCC_PRNG_AHB_CBCR_ADDR, HWIO_GCC_PRNG_AHB_CBCR_RMSK)
#define HWIO_GCC_PRNG_AHB_CBCR_INM(m)      \
        in_dword_masked(HWIO_GCC_PRNG_AHB_CBCR_ADDR, m)
#define HWIO_GCC_PRNG_AHB_CBCR_CLK_OFF_BMSK                                                   0x80000000
#define HWIO_GCC_PRNG_AHB_CBCR_CLK_OFF_SHFT                                                         0x1f

#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_ADDR                                              (GCC_CLK_CTL_REG_REG_BASE      + 0x00045004)
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_RMSK                                                0x1bf7ff
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_IN          \
        in_dword_masked(HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_ADDR, HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_RMSK)
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_INM(m)      \
        in_dword_masked(HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_ADDR, m)
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_OUT(v)      \
        out_dword(HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_ADDR,v)
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_ADDR,m,v,HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_IN)
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_QDSS_CFG_AHB_CLK_ENA_BMSK                           0x100000
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_QDSS_CFG_AHB_CLK_ENA_SHFT                               0x14
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_QDSS_DAP_CLK_ENA_BMSK                                0x80000
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_QDSS_DAP_CLK_ENA_SHFT                                   0x13
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_IMEM_AXI_CLK_ENA_BMSK                                0x20000
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_IMEM_AXI_CLK_ENA_SHFT                                   0x11
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_PCNOC_APSS_AHB_CLK_ENA_BMSK                          0x10000
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_PCNOC_APSS_AHB_CLK_ENA_SHFT                             0x10
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_BIMC_APSS_AXI_CLK_ENA_BMSK                            0x8000
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_BIMC_APSS_AXI_CLK_ENA_SHFT                               0xf
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_APSS_AHB_CLK_ENA_BMSK                                 0x4000
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_APSS_AHB_CLK_ENA_SHFT                                    0xe
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_APSS_AXI_CLK_ENA_BMSK                                 0x2000
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_APSS_AXI_CLK_ENA_SHFT                                    0xd
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_MPM_AHB_CLK_ENA_BMSK                                  0x1000
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_MPM_AHB_CLK_ENA_SHFT                                     0xc
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_BLSP1_AHB_CLK_ENA_BMSK                                 0x400
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_BLSP1_AHB_CLK_ENA_SHFT                                   0xa
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_BLSP1_SLEEP_CLK_ENA_BMSK                               0x200
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_BLSP1_SLEEP_CLK_ENA_SHFT                                 0x9
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_PRNG_AHB_CLK_ENA_BMSK                                  0x100
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_PRNG_AHB_CLK_ENA_SHFT                                    0x8
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_BOOT_ROM_AHB_CLK_ENA_BMSK                               0x80
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_BOOT_ROM_AHB_CLK_ENA_SHFT                                0x7
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_MSG_RAM_AHB_CLK_ENA_BMSK                                0x40
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_MSG_RAM_AHB_CLK_ENA_SHFT                                 0x6
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_TLMM_AHB_CLK_ENA_BMSK                                   0x20
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_TLMM_AHB_CLK_ENA_SHFT                                    0x5
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_TLMM_CLK_ENA_BMSK                                       0x10
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_TLMM_CLK_ENA_SHFT                                        0x4
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_SPMI_PCNOC_AHB_CLK_ENA_BMSK                              0x8
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_SPMI_PCNOC_AHB_CLK_ENA_SHFT                              0x3
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_CRYPTO_CLK_ENA_BMSK                                      0x4
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_CRYPTO_CLK_ENA_SHFT                                      0x2
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AXI_CLK_ENA_BMSK                                  0x2
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AXI_CLK_ENA_SHFT                                  0x1
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AHB_CLK_ENA_BMSK                                  0x1
#define HWIO_GCC_APCS_CLOCK_BRANCH_ENA_VOTE_CRYPTO_AHB_CLK_ENA_SHFT                                  0x0

/*----------------------------------------------------------------------------
 * MODULE: APCS_F0_QTMR_V1
 *--------------------------------------------------------------------------*/

#define APCS_F0_QTMR_V1_REG_BASE                                     (A7SS_BASE      + 0x00021000)
#define APCS_F0_QTMR_V1_REG_BASE_SIZE                                0x1000
#define APCS_F0_QTMR_V1_REG_BASE_USED                                0xfd0

#define HWIO_APCS_F0_QTMR_V1_CNTPCT_LO_ADDR                          (APCS_F0_QTMR_V1_REG_BASE      + 0x00000000)
#define HWIO_APCS_F0_QTMR_V1_CNTPCT_LO_RMSK                          0xffffffff
#define HWIO_APCS_F0_QTMR_V1_CNTPCT_LO_IN          \
        in_dword_masked(HWIO_APCS_F0_QTMR_V1_CNTPCT_LO_ADDR, HWIO_APCS_F0_QTMR_V1_CNTPCT_LO_RMSK)
#define HWIO_APCS_F0_QTMR_V1_CNTPCT_LO_INM(m)      \
        in_dword_masked(HWIO_APCS_F0_QTMR_V1_CNTPCT_LO_ADDR, m)
#define HWIO_APCS_F0_QTMR_V1_CNTPCT_LO_CNTPCT_LO_BMSK                0xffffffff
#define HWIO_APCS_F0_QTMR_V1_CNTPCT_LO_CNTPCT_LO_SHFT                       0x0


#endif /* __CLOCKLITEHWIO_H__ */
