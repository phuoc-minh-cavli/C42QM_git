#ifndef __PRNG_MSMHWIOREG_H__
#define __PRNG_MSMHWIOREG_H__
/*
===========================================================================
*/
/**
  @file prng_msmhwioreg.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    MDM9205 (Twizy) [twizy_v1.0_p2q1r17]
 
  This file contains HWIO register definitions for the following modules:
    PRNG_TZ_TZ_PRNG_TZ
    PRNG_MSA_MSA_PRNG_SUB


  Generation parameters: 
  { u'filename': u'prng_msmhwioreg.h',
    u'header': u'#include "msmhwiobase.h"',
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [u'PRNG_TZ_TZ_PRNG_TZ', u'PRNG_MSA_MSA_PRNG_SUB']}
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

  $Header: //components/rel/core.mpss/3.10/securemsm/cryptodrivers/prng/chipset/mdm9205/inc/prng_msmhwioreg.h#1 $
  $DateTime: 2018/09/18 22:13:35 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: PRNG_TZ_TZ_PRNG_TZ
 *--------------------------------------------------------------------------*/

#define PRNG_TZ_TZ_PRNG_TZ_REG_BASE                                                       (PRNG_PRNG_TOP_BASE      + 0x00001000)
#define PRNG_TZ_TZ_PRNG_TZ_REG_BASE_SIZE                                                  0x1000
#define PRNG_TZ_TZ_PRNG_TZ_REG_BASE_USED                                                  0x144

#define HWIO_PRNG_TZ_PRNG_DATA_OUT_ADDR                                                   (PRNG_TZ_TZ_PRNG_TZ_REG_BASE      + 0x00000000)
#define HWIO_PRNG_TZ_PRNG_DATA_OUT_RMSK                                                   0xffffffff
#define HWIO_PRNG_TZ_PRNG_DATA_OUT_IN          \
        in_dword(HWIO_PRNG_TZ_PRNG_DATA_OUT_ADDR)
#define HWIO_PRNG_TZ_PRNG_DATA_OUT_INM(m)      \
        in_dword_masked(HWIO_PRNG_TZ_PRNG_DATA_OUT_ADDR, m)
#define HWIO_PRNG_TZ_PRNG_DATA_OUT_PRNG_DATA_OUT_BMSK                                     0xffffffff
#define HWIO_PRNG_TZ_PRNG_DATA_OUT_PRNG_DATA_OUT_SHFT                                            0x0

#define HWIO_PRNG_TZ_PRNG_STATUS_ADDR                                                     (PRNG_TZ_TZ_PRNG_TZ_REG_BASE      + 0x00000004)
#define HWIO_PRNG_TZ_PRNG_STATUS_RMSK                                                         0x7fff
#define HWIO_PRNG_TZ_PRNG_STATUS_IN          \
        in_dword(HWIO_PRNG_TZ_PRNG_STATUS_ADDR)
#define HWIO_PRNG_TZ_PRNG_STATUS_INM(m)      \
        in_dword_masked(HWIO_PRNG_TZ_PRNG_STATUS_ADDR, m)
#define HWIO_PRNG_TZ_PRNG_STATUS_CRC32_CONTINUOUS_GRACE_COUNTER_BMSK                          0x7000
#define HWIO_PRNG_TZ_PRNG_STATUS_CRC32_CONTINUOUS_GRACE_COUNTER_SHFT                             0xc
#define HWIO_PRNG_TZ_PRNG_STATUS_DRBG_CONTINUOUS_GRACE_COUNTER_BMSK                            0xc00
#define HWIO_PRNG_TZ_PRNG_STATUS_DRBG_CONTINUOUS_GRACE_COUNTER_SHFT                              0xa
#define HWIO_PRNG_TZ_PRNG_STATUS_CURRENT_OPERATION_BMSK                                        0x300
#define HWIO_PRNG_TZ_PRNG_STATUS_CURRENT_OPERATION_SHFT                                          0x8
#define HWIO_PRNG_TZ_PRNG_STATUS_GENERATE_REQUIRED_BMSK                                         0x80
#define HWIO_PRNG_TZ_PRNG_STATUS_GENERATE_REQUIRED_SHFT                                          0x7
#define HWIO_PRNG_TZ_PRNG_STATUS_RESEED_REQUIRED_BMSK                                           0x40
#define HWIO_PRNG_TZ_PRNG_STATUS_RESEED_REQUIRED_SHFT                                            0x6
#define HWIO_PRNG_TZ_PRNG_STATUS_INSTANTIATE_REQUIRED_BMSK                                      0x20
#define HWIO_PRNG_TZ_PRNG_STATUS_INSTANTIATE_REQUIRED_SHFT                                       0x5
#define HWIO_PRNG_TZ_PRNG_STATUS_RING_OSC3_HEALTHY_BMSK                                         0x10
#define HWIO_PRNG_TZ_PRNG_STATUS_RING_OSC3_HEALTHY_SHFT                                          0x4
#define HWIO_PRNG_TZ_PRNG_STATUS_RING_OSC2_HEALTHY_BMSK                                          0x8
#define HWIO_PRNG_TZ_PRNG_STATUS_RING_OSC2_HEALTHY_SHFT                                          0x3
#define HWIO_PRNG_TZ_PRNG_STATUS_RING_OSC1_HEALTHY_BMSK                                          0x4
#define HWIO_PRNG_TZ_PRNG_STATUS_RING_OSC1_HEALTHY_SHFT                                          0x2
#define HWIO_PRNG_TZ_PRNG_STATUS_RING_OSC0_HEALTHY_BMSK                                          0x2
#define HWIO_PRNG_TZ_PRNG_STATUS_RING_OSC0_HEALTHY_SHFT                                          0x1
#define HWIO_PRNG_TZ_PRNG_STATUS_DATA_AVAIL_BMSK                                                 0x1
#define HWIO_PRNG_TZ_PRNG_STATUS_DATA_AVAIL_SHFT                                                 0x0

#define HWIO_PRNG_TZ_PRNG_ENTROPY_CNTR_ADDR                                               (PRNG_TZ_TZ_PRNG_TZ_REG_BASE      + 0x00000010)
#define HWIO_PRNG_TZ_PRNG_ENTROPY_CNTR_RMSK                                               0xffffffff
#define HWIO_PRNG_TZ_PRNG_ENTROPY_CNTR_IN          \
        in_dword(HWIO_PRNG_TZ_PRNG_ENTROPY_CNTR_ADDR)
#define HWIO_PRNG_TZ_PRNG_ENTROPY_CNTR_INM(m)      \
        in_dword_masked(HWIO_PRNG_TZ_PRNG_ENTROPY_CNTR_ADDR, m)
#define HWIO_PRNG_TZ_PRNG_ENTROPY_CNTR_ENTROPY_CNTR_BMSK                                  0xffffffff
#define HWIO_PRNG_TZ_PRNG_ENTROPY_CNTR_ENTROPY_CNTR_SHFT                                         0x0

#define HWIO_PRNG_TZ_PRNG_SAMPLE_CNTR_ADDR                                                (PRNG_TZ_TZ_PRNG_TZ_REG_BASE      + 0x00000014)
#define HWIO_PRNG_TZ_PRNG_SAMPLE_CNTR_RMSK                                                0xffffffff
#define HWIO_PRNG_TZ_PRNG_SAMPLE_CNTR_IN          \
        in_dword(HWIO_PRNG_TZ_PRNG_SAMPLE_CNTR_ADDR)
#define HWIO_PRNG_TZ_PRNG_SAMPLE_CNTR_INM(m)      \
        in_dword_masked(HWIO_PRNG_TZ_PRNG_SAMPLE_CNTR_ADDR, m)
#define HWIO_PRNG_TZ_PRNG_SAMPLE_CNTR_SAMPLE_CNTR_BMSK                                    0xffffffff
#define HWIO_PRNG_TZ_PRNG_SAMPLE_CNTR_SAMPLE_CNTR_SHFT                                           0x0

#define HWIO_PRNG_TZ_PRNG_GEN_CNTR_ADDR                                                   (PRNG_TZ_TZ_PRNG_TZ_REG_BASE      + 0x00000018)
#define HWIO_PRNG_TZ_PRNG_GEN_CNTR_RMSK                                                   0xffffffff
#define HWIO_PRNG_TZ_PRNG_GEN_CNTR_IN          \
        in_dword(HWIO_PRNG_TZ_PRNG_GEN_CNTR_ADDR)
#define HWIO_PRNG_TZ_PRNG_GEN_CNTR_INM(m)      \
        in_dword_masked(HWIO_PRNG_TZ_PRNG_GEN_CNTR_ADDR, m)
#define HWIO_PRNG_TZ_PRNG_GEN_CNTR_PRNG_GEN_CNTR_BMSK                                     0xffffffff
#define HWIO_PRNG_TZ_PRNG_GEN_CNTR_PRNG_GEN_CNTR_SHFT                                            0x0

#define HWIO_PRNG_TZ_PRNG_RESEED_CNTR_ADDR                                                (PRNG_TZ_TZ_PRNG_TZ_REG_BASE      + 0x0000001c)
#define HWIO_PRNG_TZ_PRNG_RESEED_CNTR_RMSK                                                0xffffffff
#define HWIO_PRNG_TZ_PRNG_RESEED_CNTR_IN          \
        in_dword(HWIO_PRNG_TZ_PRNG_RESEED_CNTR_ADDR)
#define HWIO_PRNG_TZ_PRNG_RESEED_CNTR_INM(m)      \
        in_dword_masked(HWIO_PRNG_TZ_PRNG_RESEED_CNTR_ADDR, m)
#define HWIO_PRNG_TZ_PRNG_RESEED_CNTR_RESEED_CNTR_BMSK                                    0xffffffff
#define HWIO_PRNG_TZ_PRNG_RESEED_CNTR_RESEED_CNTR_SHFT                                           0x0

#define HWIO_PRNG_TZ_PRNG_INSTANTIATE_CNTR_ADDR                                           (PRNG_TZ_TZ_PRNG_TZ_REG_BASE      + 0x00000020)
#define HWIO_PRNG_TZ_PRNG_INSTANTIATE_CNTR_RMSK                                           0xffffffff
#define HWIO_PRNG_TZ_PRNG_INSTANTIATE_CNTR_IN          \
        in_dword(HWIO_PRNG_TZ_PRNG_INSTANTIATE_CNTR_ADDR)
#define HWIO_PRNG_TZ_PRNG_INSTANTIATE_CNTR_INM(m)      \
        in_dword_masked(HWIO_PRNG_TZ_PRNG_INSTANTIATE_CNTR_ADDR, m)
#define HWIO_PRNG_TZ_PRNG_INSTANTIATE_CNTR_INSTANTIATE_CNTR_BMSK                          0xffffffff
#define HWIO_PRNG_TZ_PRNG_INSTANTIATE_CNTR_INSTANTIATE_CNTR_SHFT                                 0x0

#define HWIO_PRNG_TZ_PRNG_DEBUG_ADDR                                                      (PRNG_TZ_TZ_PRNG_TZ_REG_BASE      + 0x00000024)
#define HWIO_PRNG_TZ_PRNG_DEBUG_RMSK                                                       0x1ffffff
#define HWIO_PRNG_TZ_PRNG_DEBUG_IN          \
        in_dword(HWIO_PRNG_TZ_PRNG_DEBUG_ADDR)
#define HWIO_PRNG_TZ_PRNG_DEBUG_INM(m)      \
        in_dword_masked(HWIO_PRNG_TZ_PRNG_DEBUG_ADDR, m)
#define HWIO_PRNG_TZ_PRNG_DEBUG_OUTPUT_DATA_AVAIL_BMSK                                     0x1000000
#define HWIO_PRNG_TZ_PRNG_DEBUG_OUTPUT_DATA_AVAIL_SHFT                                          0x18
#define HWIO_PRNG_TZ_PRNG_DEBUG_DRBG_GRACE_COUNTER_STATE_BMSK                               0xc00000
#define HWIO_PRNG_TZ_PRNG_DEBUG_DRBG_GRACE_COUNTER_STATE_SHFT                                   0x16
#define HWIO_PRNG_TZ_PRNG_DEBUG_CRC32_GRACE_COUNTER_STATE_BMSK                              0x380000
#define HWIO_PRNG_TZ_PRNG_DEBUG_CRC32_GRACE_COUNTER_STATE_SHFT                                  0x13
#define HWIO_PRNG_TZ_PRNG_DEBUG_KAT_STATE_BMSK                                               0x7c000
#define HWIO_PRNG_TZ_PRNG_DEBUG_KAT_STATE_SHFT                                                   0xe
#define HWIO_PRNG_TZ_PRNG_DEBUG_RNG_STATE_BMSK                                                0x3c00
#define HWIO_PRNG_TZ_PRNG_DEBUG_RNG_STATE_SHFT                                                   0xa
#define HWIO_PRNG_TZ_PRNG_DEBUG_HASHGEN_STATE_BMSK                                             0x380
#define HWIO_PRNG_TZ_PRNG_DEBUG_HASHGEN_STATE_SHFT                                               0x7
#define HWIO_PRNG_TZ_PRNG_DEBUG_HASH_DF_STATE_BMSK                                              0x70
#define HWIO_PRNG_TZ_PRNG_DEBUG_HASH_DF_STATE_SHFT                                               0x4
#define HWIO_PRNG_TZ_PRNG_DEBUG_PAD_STATE_BMSK                                                   0xf
#define HWIO_PRNG_TZ_PRNG_DEBUG_PAD_STATE_SHFT                                                   0x0

#define HWIO_PRNG_TZ_PRNG_ARB_DEBUG_ADDR                                                  (PRNG_TZ_TZ_PRNG_TZ_REG_BASE      + 0x00000028)
#define HWIO_PRNG_TZ_PRNG_ARB_DEBUG_RMSK                                                  0xffffffff
#define HWIO_PRNG_TZ_PRNG_ARB_DEBUG_IN          \
        in_dword(HWIO_PRNG_TZ_PRNG_ARB_DEBUG_ADDR)
#define HWIO_PRNG_TZ_PRNG_ARB_DEBUG_INM(m)      \
        in_dword_masked(HWIO_PRNG_TZ_PRNG_ARB_DEBUG_ADDR, m)
#define HWIO_PRNG_TZ_PRNG_ARB_DEBUG_ARB_MASK_ARRAY_BMSK                                   0xfffe0000
#define HWIO_PRNG_TZ_PRNG_ARB_DEBUG_ARB_MASK_ARRAY_SHFT                                         0x11
#define HWIO_PRNG_TZ_PRNG_ARB_DEBUG_ARB_GRANT_ARRAY_BMSK                                     0x1fffc
#define HWIO_PRNG_TZ_PRNG_ARB_DEBUG_ARB_GRANT_ARRAY_SHFT                                         0x2
#define HWIO_PRNG_TZ_PRNG_ARB_DEBUG_ARB_STATE_BMSK                                               0x3
#define HWIO_PRNG_TZ_PRNG_ARB_DEBUG_ARB_STATE_SHFT                                               0x0

#define HWIO_PRNG_TZ_PRNG_LFSR_CFG_ADDR                                                   (PRNG_TZ_TZ_PRNG_TZ_REG_BASE      + 0x00000100)
#define HWIO_PRNG_TZ_PRNG_LFSR_CFG_RMSK                                                      0x1ffff
#define HWIO_PRNG_TZ_PRNG_LFSR_CFG_IN          \
        in_dword(HWIO_PRNG_TZ_PRNG_LFSR_CFG_ADDR)
#define HWIO_PRNG_TZ_PRNG_LFSR_CFG_INM(m)      \
        in_dword_masked(HWIO_PRNG_TZ_PRNG_LFSR_CFG_ADDR, m)
#define HWIO_PRNG_TZ_PRNG_LFSR_CFG_OUT(v)      \
        out_dword(HWIO_PRNG_TZ_PRNG_LFSR_CFG_ADDR,v)
#define HWIO_PRNG_TZ_PRNG_LFSR_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PRNG_TZ_PRNG_LFSR_CFG_ADDR,m,v,HWIO_PRNG_TZ_PRNG_LFSR_CFG_IN)
#define HWIO_PRNG_TZ_PRNG_LFSR_CFG_LFSR_OVRID_ON_BMSK                                        0x10000
#define HWIO_PRNG_TZ_PRNG_LFSR_CFG_LFSR_OVRID_ON_SHFT                                           0x10
#define HWIO_PRNG_TZ_PRNG_LFSR_CFG_LFSR3_EN_BMSK                                              0x8000
#define HWIO_PRNG_TZ_PRNG_LFSR_CFG_LFSR3_EN_SHFT                                                 0xf
#define HWIO_PRNG_TZ_PRNG_LFSR_CFG_RING_OSC3_CFG_BMSK                                         0x7000
#define HWIO_PRNG_TZ_PRNG_LFSR_CFG_RING_OSC3_CFG_SHFT                                            0xc
#define HWIO_PRNG_TZ_PRNG_LFSR_CFG_LFSR2_EN_BMSK                                               0x800
#define HWIO_PRNG_TZ_PRNG_LFSR_CFG_LFSR2_EN_SHFT                                                 0xb
#define HWIO_PRNG_TZ_PRNG_LFSR_CFG_RING_OSC2_CFG_BMSK                                          0x700
#define HWIO_PRNG_TZ_PRNG_LFSR_CFG_RING_OSC2_CFG_SHFT                                            0x8
#define HWIO_PRNG_TZ_PRNG_LFSR_CFG_LFSR1_EN_BMSK                                                0x80
#define HWIO_PRNG_TZ_PRNG_LFSR_CFG_LFSR1_EN_SHFT                                                 0x7
#define HWIO_PRNG_TZ_PRNG_LFSR_CFG_RING_OSC1_CFG_BMSK                                           0x70
#define HWIO_PRNG_TZ_PRNG_LFSR_CFG_RING_OSC1_CFG_SHFT                                            0x4
#define HWIO_PRNG_TZ_PRNG_LFSR_CFG_LFSR0_EN_BMSK                                                 0x8
#define HWIO_PRNG_TZ_PRNG_LFSR_CFG_LFSR0_EN_SHFT                                                 0x3
#define HWIO_PRNG_TZ_PRNG_LFSR_CFG_RING_OSC0_CFG_BMSK                                            0x7
#define HWIO_PRNG_TZ_PRNG_LFSR_CFG_RING_OSC0_CFG_SHFT                                            0x0

#define HWIO_PRNG_TZ_PRNG_CONFIG_ADDR                                                     (PRNG_TZ_TZ_PRNG_TZ_REG_BASE      + 0x00000104)
#define HWIO_PRNG_TZ_PRNG_CONFIG_RMSK                                                           0x3f
#define HWIO_PRNG_TZ_PRNG_CONFIG_IN          \
        in_dword(HWIO_PRNG_TZ_PRNG_CONFIG_ADDR)
#define HWIO_PRNG_TZ_PRNG_CONFIG_INM(m)      \
        in_dword_masked(HWIO_PRNG_TZ_PRNG_CONFIG_ADDR, m)
#define HWIO_PRNG_TZ_PRNG_CONFIG_OUT(v)      \
        out_dword(HWIO_PRNG_TZ_PRNG_CONFIG_ADDR,v)
#define HWIO_PRNG_TZ_PRNG_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PRNG_TZ_PRNG_CONFIG_ADDR,m,v,HWIO_PRNG_TZ_PRNG_CONFIG_IN)
#define HWIO_PRNG_TZ_PRNG_CONFIG_TEST_OUT_SEL_BMSK                                              0x3c
#define HWIO_PRNG_TZ_PRNG_CONFIG_TEST_OUT_SEL_SHFT                                               0x2
#define HWIO_PRNG_TZ_PRNG_CONFIG_PRNG_EN_BMSK                                                    0x2
#define HWIO_PRNG_TZ_PRNG_CONFIG_PRNG_EN_SHFT                                                    0x1
#define HWIO_PRNG_TZ_PRNG_CONFIG_SW_RESET_BMSK                                                   0x1
#define HWIO_PRNG_TZ_PRNG_CONFIG_SW_RESET_SHFT                                                   0x0

#define HWIO_PRNG_TZ_PRNG_PERSONALIZATION_STRING_ADDR                                     (PRNG_TZ_TZ_PRNG_TZ_REG_BASE      + 0x00000108)
#define HWIO_PRNG_TZ_PRNG_PERSONALIZATION_STRING_RMSK                                     0xffffffff
#define HWIO_PRNG_TZ_PRNG_PERSONALIZATION_STRING_IN          \
        in_dword(HWIO_PRNG_TZ_PRNG_PERSONALIZATION_STRING_ADDR)
#define HWIO_PRNG_TZ_PRNG_PERSONALIZATION_STRING_INM(m)      \
        in_dword_masked(HWIO_PRNG_TZ_PRNG_PERSONALIZATION_STRING_ADDR, m)
#define HWIO_PRNG_TZ_PRNG_PERSONALIZATION_STRING_OUT(v)      \
        out_dword(HWIO_PRNG_TZ_PRNG_PERSONALIZATION_STRING_ADDR,v)
#define HWIO_PRNG_TZ_PRNG_PERSONALIZATION_STRING_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PRNG_TZ_PRNG_PERSONALIZATION_STRING_ADDR,m,v,HWIO_PRNG_TZ_PRNG_PERSONALIZATION_STRING_IN)
#define HWIO_PRNG_TZ_PRNG_PERSONALIZATION_STRING_PERSONALIZATION_STRING_BMSK              0xffffffff
#define HWIO_PRNG_TZ_PRNG_PERSONALIZATION_STRING_PERSONALIZATION_STRING_SHFT                     0x0

#define HWIO_PRNG_TZ_PRNG_NUM_ENTROPY_ADDR                                                (PRNG_TZ_TZ_PRNG_TZ_REG_BASE      + 0x00000110)
#define HWIO_PRNG_TZ_PRNG_NUM_ENTROPY_RMSK                                                0xffffffff
#define HWIO_PRNG_TZ_PRNG_NUM_ENTROPY_IN          \
        in_dword(HWIO_PRNG_TZ_PRNG_NUM_ENTROPY_ADDR)
#define HWIO_PRNG_TZ_PRNG_NUM_ENTROPY_INM(m)      \
        in_dword_masked(HWIO_PRNG_TZ_PRNG_NUM_ENTROPY_ADDR, m)
#define HWIO_PRNG_TZ_PRNG_NUM_ENTROPY_OUT(v)      \
        out_dword(HWIO_PRNG_TZ_PRNG_NUM_ENTROPY_ADDR,v)
#define HWIO_PRNG_TZ_PRNG_NUM_ENTROPY_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PRNG_TZ_PRNG_NUM_ENTROPY_ADDR,m,v,HWIO_PRNG_TZ_PRNG_NUM_ENTROPY_IN)
#define HWIO_PRNG_TZ_PRNG_NUM_ENTROPY_NUM_ENTROPY_BYTES_BMSK                              0xffffffff
#define HWIO_PRNG_TZ_PRNG_NUM_ENTROPY_NUM_ENTROPY_BYTES_SHFT                                     0x0

#define HWIO_PRNG_TZ_PRNG_SAMPLE_FREQ_ADDR                                                (PRNG_TZ_TZ_PRNG_TZ_REG_BASE      + 0x00000114)
#define HWIO_PRNG_TZ_PRNG_SAMPLE_FREQ_RMSK                                                0xffffffff
#define HWIO_PRNG_TZ_PRNG_SAMPLE_FREQ_IN          \
        in_dword(HWIO_PRNG_TZ_PRNG_SAMPLE_FREQ_ADDR)
#define HWIO_PRNG_TZ_PRNG_SAMPLE_FREQ_INM(m)      \
        in_dword_masked(HWIO_PRNG_TZ_PRNG_SAMPLE_FREQ_ADDR, m)
#define HWIO_PRNG_TZ_PRNG_SAMPLE_FREQ_OUT(v)      \
        out_dword(HWIO_PRNG_TZ_PRNG_SAMPLE_FREQ_ADDR,v)
#define HWIO_PRNG_TZ_PRNG_SAMPLE_FREQ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PRNG_TZ_PRNG_SAMPLE_FREQ_ADDR,m,v,HWIO_PRNG_TZ_PRNG_SAMPLE_FREQ_IN)
#define HWIO_PRNG_TZ_PRNG_SAMPLE_FREQ_SAMPLE_FREQ_BMSK                                    0xffffffff
#define HWIO_PRNG_TZ_PRNG_SAMPLE_FREQ_SAMPLE_FREQ_SHFT                                           0x0

#define HWIO_PRNG_TZ_PRNG_GEN_WORDS_ADDR                                                  (PRNG_TZ_TZ_PRNG_TZ_REG_BASE      + 0x00000118)
#define HWIO_PRNG_TZ_PRNG_GEN_WORDS_RMSK                                                  0xffffffff
#define HWIO_PRNG_TZ_PRNG_GEN_WORDS_IN          \
        in_dword(HWIO_PRNG_TZ_PRNG_GEN_WORDS_ADDR)
#define HWIO_PRNG_TZ_PRNG_GEN_WORDS_INM(m)      \
        in_dword_masked(HWIO_PRNG_TZ_PRNG_GEN_WORDS_ADDR, m)
#define HWIO_PRNG_TZ_PRNG_GEN_WORDS_OUT(v)      \
        out_dword(HWIO_PRNG_TZ_PRNG_GEN_WORDS_ADDR,v)
#define HWIO_PRNG_TZ_PRNG_GEN_WORDS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PRNG_TZ_PRNG_GEN_WORDS_ADDR,m,v,HWIO_PRNG_TZ_PRNG_GEN_WORDS_IN)
#define HWIO_PRNG_TZ_PRNG_GEN_WORDS_REQUESTED_WORDS_BMSK                                  0xffffffff
#define HWIO_PRNG_TZ_PRNG_GEN_WORDS_REQUESTED_WORDS_SHFT                                         0x0

#define HWIO_PRNG_TZ_PRNG_RESEED_FREQ_ADDR                                                (PRNG_TZ_TZ_PRNG_TZ_REG_BASE      + 0x0000011c)
#define HWIO_PRNG_TZ_PRNG_RESEED_FREQ_RMSK                                                0xffffffff
#define HWIO_PRNG_TZ_PRNG_RESEED_FREQ_IN          \
        in_dword(HWIO_PRNG_TZ_PRNG_RESEED_FREQ_ADDR)
#define HWIO_PRNG_TZ_PRNG_RESEED_FREQ_INM(m)      \
        in_dword_masked(HWIO_PRNG_TZ_PRNG_RESEED_FREQ_ADDR, m)
#define HWIO_PRNG_TZ_PRNG_RESEED_FREQ_OUT(v)      \
        out_dword(HWIO_PRNG_TZ_PRNG_RESEED_FREQ_ADDR,v)
#define HWIO_PRNG_TZ_PRNG_RESEED_FREQ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PRNG_TZ_PRNG_RESEED_FREQ_ADDR,m,v,HWIO_PRNG_TZ_PRNG_RESEED_FREQ_IN)
#define HWIO_PRNG_TZ_PRNG_RESEED_FREQ_RESEED_FREQ_BMSK                                    0xffffffff
#define HWIO_PRNG_TZ_PRNG_RESEED_FREQ_RESEED_FREQ_SHFT                                           0x0

#define HWIO_PRNG_TZ_PRNG_INSTANTIATE_FREQ_ADDR                                           (PRNG_TZ_TZ_PRNG_TZ_REG_BASE      + 0x00000120)
#define HWIO_PRNG_TZ_PRNG_INSTANTIATE_FREQ_RMSK                                           0xffffffff
#define HWIO_PRNG_TZ_PRNG_INSTANTIATE_FREQ_IN          \
        in_dword(HWIO_PRNG_TZ_PRNG_INSTANTIATE_FREQ_ADDR)
#define HWIO_PRNG_TZ_PRNG_INSTANTIATE_FREQ_INM(m)      \
        in_dword_masked(HWIO_PRNG_TZ_PRNG_INSTANTIATE_FREQ_ADDR, m)
#define HWIO_PRNG_TZ_PRNG_INSTANTIATE_FREQ_OUT(v)      \
        out_dword(HWIO_PRNG_TZ_PRNG_INSTANTIATE_FREQ_ADDR,v)
#define HWIO_PRNG_TZ_PRNG_INSTANTIATE_FREQ_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PRNG_TZ_PRNG_INSTANTIATE_FREQ_ADDR,m,v,HWIO_PRNG_TZ_PRNG_INSTANTIATE_FREQ_IN)
#define HWIO_PRNG_TZ_PRNG_INSTANTIATE_FREQ_INSTANTIATE_FREQ_BMSK                          0xffffffff
#define HWIO_PRNG_TZ_PRNG_INSTANTIATE_FREQ_INSTANTIATE_FREQ_SHFT                                 0x0

#define HWIO_PRNG_TZ_PRNG_SWKAT_ENTROPY_WINDOW_IN_ADDR                                    (PRNG_TZ_TZ_PRNG_TZ_REG_BASE      + 0x00000124)
#define HWIO_PRNG_TZ_PRNG_SWKAT_ENTROPY_WINDOW_IN_RMSK                                    0xffffffff
#define HWIO_PRNG_TZ_PRNG_SWKAT_ENTROPY_WINDOW_IN_OUT(v)      \
        out_dword(HWIO_PRNG_TZ_PRNG_SWKAT_ENTROPY_WINDOW_IN_ADDR,v)
#define HWIO_PRNG_TZ_PRNG_SWKAT_ENTROPY_WINDOW_IN_SWKAT_ENTROPY_WINDOW_IN_BMSK            0xffffffff
#define HWIO_PRNG_TZ_PRNG_SWKAT_ENTROPY_WINDOW_IN_SWKAT_ENTROPY_WINDOW_IN_SHFT                   0x0

#define HWIO_PRNG_TZ_PRNG_SWKAT_VC_WINDOW_IN_ADDR                                         (PRNG_TZ_TZ_PRNG_TZ_REG_BASE      + 0x00000128)
#define HWIO_PRNG_TZ_PRNG_SWKAT_VC_WINDOW_IN_RMSK                                         0xffffffff
#define HWIO_PRNG_TZ_PRNG_SWKAT_VC_WINDOW_IN_OUT(v)      \
        out_dword(HWIO_PRNG_TZ_PRNG_SWKAT_VC_WINDOW_IN_ADDR,v)
#define HWIO_PRNG_TZ_PRNG_SWKAT_VC_WINDOW_IN_SWKAT_VC_WINDOW_IN_BMSK                      0xffffffff
#define HWIO_PRNG_TZ_PRNG_SWKAT_VC_WINDOW_IN_SWKAT_VC_WINDOW_IN_SHFT                             0x0

#define HWIO_PRNG_TZ_PRNG_SWKAT_ENTROPY_WINDOW_OUT_ADDR                                   (PRNG_TZ_TZ_PRNG_TZ_REG_BASE      + 0x00000130)
#define HWIO_PRNG_TZ_PRNG_SWKAT_ENTROPY_WINDOW_OUT_RMSK                                   0xffffffff
#define HWIO_PRNG_TZ_PRNG_SWKAT_ENTROPY_WINDOW_OUT_IN          \
        in_dword(HWIO_PRNG_TZ_PRNG_SWKAT_ENTROPY_WINDOW_OUT_ADDR)
#define HWIO_PRNG_TZ_PRNG_SWKAT_ENTROPY_WINDOW_OUT_INM(m)      \
        in_dword_masked(HWIO_PRNG_TZ_PRNG_SWKAT_ENTROPY_WINDOW_OUT_ADDR, m)
#define HWIO_PRNG_TZ_PRNG_SWKAT_ENTROPY_WINDOW_OUT_SWKAT_ENTROPY_WINDOW_OUT_BMSK          0xffffffff
#define HWIO_PRNG_TZ_PRNG_SWKAT_ENTROPY_WINDOW_OUT_SWKAT_ENTROPY_WINDOW_OUT_SHFT                 0x0

#define HWIO_PRNG_TZ_PRNG_SWKAT_VC_WINDOW_OUT_ADDR                                        (PRNG_TZ_TZ_PRNG_TZ_REG_BASE      + 0x00000134)
#define HWIO_PRNG_TZ_PRNG_SWKAT_VC_WINDOW_OUT_RMSK                                        0xffffffff
#define HWIO_PRNG_TZ_PRNG_SWKAT_VC_WINDOW_OUT_IN          \
        in_dword(HWIO_PRNG_TZ_PRNG_SWKAT_VC_WINDOW_OUT_ADDR)
#define HWIO_PRNG_TZ_PRNG_SWKAT_VC_WINDOW_OUT_INM(m)      \
        in_dword_masked(HWIO_PRNG_TZ_PRNG_SWKAT_VC_WINDOW_OUT_ADDR, m)
#define HWIO_PRNG_TZ_PRNG_SWKAT_VC_WINDOW_OUT_SWKAT_VC_WINDOW_OUT_BMSK                    0xffffffff
#define HWIO_PRNG_TZ_PRNG_SWKAT_VC_WINDOW_OUT_SWKAT_VC_WINDOW_OUT_SHFT                           0x0

#define HWIO_PRNG_TZ_PRNG_SWKAT_CONFIG_ADDR                                               (PRNG_TZ_TZ_PRNG_TZ_REG_BASE      + 0x0000013c)
#define HWIO_PRNG_TZ_PRNG_SWKAT_CONFIG_RMSK                                                      0xf
#define HWIO_PRNG_TZ_PRNG_SWKAT_CONFIG_IN          \
        in_dword(HWIO_PRNG_TZ_PRNG_SWKAT_CONFIG_ADDR)
#define HWIO_PRNG_TZ_PRNG_SWKAT_CONFIG_INM(m)      \
        in_dword_masked(HWIO_PRNG_TZ_PRNG_SWKAT_CONFIG_ADDR, m)
#define HWIO_PRNG_TZ_PRNG_SWKAT_CONFIG_OUT(v)      \
        out_dword(HWIO_PRNG_TZ_PRNG_SWKAT_CONFIG_ADDR,v)
#define HWIO_PRNG_TZ_PRNG_SWKAT_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PRNG_TZ_PRNG_SWKAT_CONFIG_ADDR,m,v,HWIO_PRNG_TZ_PRNG_SWKAT_CONFIG_IN)
#define HWIO_PRNG_TZ_PRNG_SWKAT_CONFIG_SWKAT_TESTMODE_SEL_BMSK                                   0xe
#define HWIO_PRNG_TZ_PRNG_SWKAT_CONFIG_SWKAT_TESTMODE_SEL_SHFT                                   0x1
#define HWIO_PRNG_TZ_PRNG_SWKAT_CONFIG_SWKAT_EN_BMSK                                             0x1
#define HWIO_PRNG_TZ_PRNG_SWKAT_CONFIG_SWKAT_EN_SHFT                                             0x0

#define HWIO_PRNG_TZ_PRNG_KAT_STATUS_ADDR                                                 (PRNG_TZ_TZ_PRNG_TZ_REG_BASE      + 0x00000140)
#define HWIO_PRNG_TZ_PRNG_KAT_STATUS_RMSK                                                 0x1fffffff
#define HWIO_PRNG_TZ_PRNG_KAT_STATUS_IN          \
        in_dword(HWIO_PRNG_TZ_PRNG_KAT_STATUS_ADDR)
#define HWIO_PRNG_TZ_PRNG_KAT_STATUS_INM(m)      \
        in_dword_masked(HWIO_PRNG_TZ_PRNG_KAT_STATUS_ADDR, m)
#define HWIO_PRNG_TZ_PRNG_KAT_STATUS_CRC32_CONTINUOUS_TEST_FAILURE_BMSK                   0x10000000
#define HWIO_PRNG_TZ_PRNG_KAT_STATUS_CRC32_CONTINUOUS_TEST_FAILURE_SHFT                         0x1c
#define HWIO_PRNG_TZ_PRNG_KAT_STATUS_DRBG_CONTINUOUS_TEST_FAILURE_BMSK                     0x8000000
#define HWIO_PRNG_TZ_PRNG_KAT_STATUS_DRBG_CONTINUOUS_TEST_FAILURE_SHFT                          0x1b
#define HWIO_PRNG_TZ_PRNG_KAT_STATUS_KAT_FAILURE_BMSK                                      0x4000000
#define HWIO_PRNG_TZ_PRNG_KAT_STATUS_KAT_FAILURE_SHFT                                           0x1a
#define HWIO_PRNG_TZ_PRNG_KAT_STATUS_KAT_DONE_BMSK                                         0x2000000
#define HWIO_PRNG_TZ_PRNG_KAT_STATUS_KAT_DONE_SHFT                                              0x19
#define HWIO_PRNG_TZ_PRNG_KAT_STATUS_SWKAT_VC_RD_READY_BMSK                                0x1000000
#define HWIO_PRNG_TZ_PRNG_KAT_STATUS_SWKAT_VC_RD_READY_SHFT                                     0x18
#define HWIO_PRNG_TZ_PRNG_KAT_STATUS_SWKAT_ENTROPY_RD_READY_BMSK                            0x800000
#define HWIO_PRNG_TZ_PRNG_KAT_STATUS_SWKAT_ENTROPY_RD_READY_SHFT                                0x17
#define HWIO_PRNG_TZ_PRNG_KAT_STATUS_SWKAT_VC_WR_READY_BMSK                                 0x400000
#define HWIO_PRNG_TZ_PRNG_KAT_STATUS_SWKAT_VC_WR_READY_SHFT                                     0x16
#define HWIO_PRNG_TZ_PRNG_KAT_STATUS_SWKAT_ENTROPY_WR_READY_BMSK                            0x200000
#define HWIO_PRNG_TZ_PRNG_KAT_STATUS_SWKAT_ENTROPY_WR_READY_SHFT                                0x15
#define HWIO_PRNG_TZ_PRNG_KAT_STATUS_SWKAT_VC_CNT_BMSK                                      0x1e0000
#define HWIO_PRNG_TZ_PRNG_KAT_STATUS_SWKAT_VC_CNT_SHFT                                          0x11
#define HWIO_PRNG_TZ_PRNG_KAT_STATUS_SWKAT_ENTROPY_CNT_BMSK                                  0x1fffe
#define HWIO_PRNG_TZ_PRNG_KAT_STATUS_SWKAT_ENTROPY_CNT_SHFT                                      0x1
#define HWIO_PRNG_TZ_PRNG_KAT_STATUS_SWKAT_DONE_BMSK                                             0x1
#define HWIO_PRNG_TZ_PRNG_KAT_STATUS_SWKAT_DONE_SHFT                                             0x0

#define HWIO_PRNG_TZ_PRNG_AC_CONFIG_ADDR                                                  (PRNG_TZ_TZ_PRNG_TZ_REG_BASE      + 0x00000144)
#define HWIO_PRNG_TZ_PRNG_AC_CONFIG_RMSK                                                         0x1
#define HWIO_PRNG_TZ_PRNG_AC_CONFIG_IN          \
        in_dword(HWIO_PRNG_TZ_PRNG_AC_CONFIG_ADDR)
#define HWIO_PRNG_TZ_PRNG_AC_CONFIG_INM(m)      \
        in_dword_masked(HWIO_PRNG_TZ_PRNG_AC_CONFIG_ADDR, m)
#define HWIO_PRNG_TZ_PRNG_AC_CONFIG_OUT(v)      \
        out_dword(HWIO_PRNG_TZ_PRNG_AC_CONFIG_ADDR,v)
#define HWIO_PRNG_TZ_PRNG_AC_CONFIG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_PRNG_TZ_PRNG_AC_CONFIG_ADDR,m,v,HWIO_PRNG_TZ_PRNG_AC_CONFIG_IN)
#define HWIO_PRNG_TZ_PRNG_AC_CONFIG_ACCESS_ENABLE_BMSK                                           0x1
#define HWIO_PRNG_TZ_PRNG_AC_CONFIG_ACCESS_ENABLE_SHFT                                           0x0

/*----------------------------------------------------------------------------
 * MODULE: PRNG_MSA_MSA_PRNG_SUB
 *--------------------------------------------------------------------------*/

#define PRNG_MSA_MSA_PRNG_SUB_REG_BASE                               (PRNG_PRNG_TOP_BASE      + 0x00002000)
#define PRNG_MSA_MSA_PRNG_SUB_REG_BASE_SIZE                          0x1000
#define PRNG_MSA_MSA_PRNG_SUB_REG_BASE_USED                          0x4

#define HWIO_PRNG_MSA_PRNG_DATA_OUT_ADDR                             (PRNG_MSA_MSA_PRNG_SUB_REG_BASE      + 0x00000000)
#define HWIO_PRNG_MSA_PRNG_DATA_OUT_RMSK                             0xffffffff
#define HWIO_PRNG_MSA_PRNG_DATA_OUT_IN          \
        in_dword(HWIO_PRNG_MSA_PRNG_DATA_OUT_ADDR)
#define HWIO_PRNG_MSA_PRNG_DATA_OUT_INM(m)      \
        in_dword_masked(HWIO_PRNG_MSA_PRNG_DATA_OUT_ADDR, m)
#define HWIO_PRNG_MSA_PRNG_DATA_OUT_PRNG_DATA_OUT_BMSK               0xffffffff
#define HWIO_PRNG_MSA_PRNG_DATA_OUT_PRNG_DATA_OUT_SHFT                      0x0

#define HWIO_PRNG_MSA_PRNG_STATUS_ADDR                               (PRNG_MSA_MSA_PRNG_SUB_REG_BASE      + 0x00000004)
#define HWIO_PRNG_MSA_PRNG_STATUS_RMSK                                    0x3ff
#define HWIO_PRNG_MSA_PRNG_STATUS_IN          \
        in_dword(HWIO_PRNG_MSA_PRNG_STATUS_ADDR)
#define HWIO_PRNG_MSA_PRNG_STATUS_INM(m)      \
        in_dword_masked(HWIO_PRNG_MSA_PRNG_STATUS_ADDR, m)
#define HWIO_PRNG_MSA_PRNG_STATUS_CURRENT_OPERATION_BMSK                  0x300
#define HWIO_PRNG_MSA_PRNG_STATUS_CURRENT_OPERATION_SHFT                    0x8
#define HWIO_PRNG_MSA_PRNG_STATUS_GENERATE_REQUIRED_BMSK                   0x80
#define HWIO_PRNG_MSA_PRNG_STATUS_GENERATE_REQUIRED_SHFT                    0x7
#define HWIO_PRNG_MSA_PRNG_STATUS_RESEED_REQUIRED_BMSK                     0x40
#define HWIO_PRNG_MSA_PRNG_STATUS_RESEED_REQUIRED_SHFT                      0x6
#define HWIO_PRNG_MSA_PRNG_STATUS_INSTANTIATE_REQUIRED_BMSK                0x20
#define HWIO_PRNG_MSA_PRNG_STATUS_INSTANTIATE_REQUIRED_SHFT                 0x5
#define HWIO_PRNG_MSA_PRNG_STATUS_RING_OSC3_HEALTHY_BMSK                   0x10
#define HWIO_PRNG_MSA_PRNG_STATUS_RING_OSC3_HEALTHY_SHFT                    0x4
#define HWIO_PRNG_MSA_PRNG_STATUS_RING_OSC2_HEALTHY_BMSK                    0x8
#define HWIO_PRNG_MSA_PRNG_STATUS_RING_OSC2_HEALTHY_SHFT                    0x3
#define HWIO_PRNG_MSA_PRNG_STATUS_RING_OSC1_HEALTHY_BMSK                    0x4
#define HWIO_PRNG_MSA_PRNG_STATUS_RING_OSC1_HEALTHY_SHFT                    0x2
#define HWIO_PRNG_MSA_PRNG_STATUS_RING_OSC0_HEALTHY_BMSK                    0x2
#define HWIO_PRNG_MSA_PRNG_STATUS_RING_OSC0_HEALTHY_SHFT                    0x1
#define HWIO_PRNG_MSA_PRNG_STATUS_DATA_AVAIL_BMSK                           0x1
#define HWIO_PRNG_MSA_PRNG_STATUS_DATA_AVAIL_SHFT                           0x0


#endif /* __PRNG_MSMHWIOREG_H__ */
