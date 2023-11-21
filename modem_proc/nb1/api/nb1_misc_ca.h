#ifndef __NB1_MISC_CA_H__
#define __NB1_MISC_CA_H__

/*
===========================================================================
*/
/**
  @file nb1_misc_ca.h
  @brief Auto-generated HWIO interface include file.

  This file contains HWIO register definitions for the following modules:
    SECURITY_CONTROL_CORE

  'Include' filters applied: QFPROM_RAW_FEAT_CONFIG_ROW0_LSB[SECURITY_CONTROL_CORE]
  'Exclude' filters applied: RESERVED DUMMY
*/
/*
  ===========================================================================

  Copyright (c) 2016 QUALCOMM Technologies Incorporated. All Rights Reserved
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================

  $Header: //components/rel/nb1.mpss/2.2/api/nb1_misc_ca.h#1 $
  $DateTime: 2023/04/18 05:28:12 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: SECURITY_CONTROL_CORE
 *--------------------------------------------------------------------------*/

#define SECURITY_CONTROL_CORE_REG_BASE                                                                             (SECURITY_CONTROL_BASE      + 0x00000000)


#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR                                             (SECURITY_CONTROL_CORE_REG_BASE      + 0x00000160)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_PHYS                                             (SECURITY_CONTROL_CORE_REG_BASE_PHYS + 0x00000160)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RMSK                                                                  0xffffffff
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_IN          \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR, HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RMSK)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_INM(m)      \
        in_dword_masked(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR, m)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_OUT(v)      \
        out_dword(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR,v)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_OUTM(m,v) \
        out_dword_masked_ns(HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_ADDR,m,v,HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_IN)
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RESOLUTION_LIMITER_BMSK                          0x80000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_RESOLUTION_LIMITER_SHFT                                0x1f
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_DSI_1_DISABLE_BMSK                               0x40000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_DSI_1_DISABLE_SHFT                                     0x1e
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_DSI_0_DISABLE_BMSK                               0x20000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_DSI_0_DISABLE_SHFT                                     0x1d
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MDP_EFUSE_LTC0_DISABLE_BMSK                      0x10000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MDP_EFUSE_LTC0_DISABLE_SHFT                            0x1c
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_NIDNT_DISABLE_BMSK                                0x8000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_NIDNT_DISABLE_SHFT                                     0x1b
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SPARE_BMSK                                        0x4000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SPARE_SHFT                                             0x1a
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SECURE_CHANNEL_DISABLE_BMSK                       0x2000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SECURE_CHANNEL_DISABLE_SHFT                            0x19
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VP8_DECODER_DISABLE_BMSK                          0x1000000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VP8_DECODER_DISABLE_SHFT                               0x18
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VP8_ENCODER_DISABLE_BMSK                           0x800000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_VP8_ENCODER_DISABLE_SHFT                               0x17
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SPARE_22_BMSK                                      0x400000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_SPARE_22_SHFT                                          0x16
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_NAV_DISABLE_BMSK                                   0x200000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_NAV_DISABLE_SHFT                                       0x15
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_GSM_DISABLE_BMSK                                   0x100000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_GSM_DISABLE_SHFT                                       0x14
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_SPARE_DISABLE_BMSK                            0xe0000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_SPARE_DISABLE_SHFT                               0x11
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_UIM2_DISABLE_BMSK                             0x10000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_UIM2_DISABLE_SHFT                                0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_UIM1_DISABLE_BMSK                              0x8000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_UIM1_DISABLE_SHFT                                 0xf
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_SMMU_DISABLE_BMSK                              0x4000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_SMMU_DISABLE_SHFT                                 0xe
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_DSDA_DISABLE_BMSK                              0x2000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_DSDA_DISABLE_SHFT                                 0xd
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX3_DISABLE_BMSK                             0x1000
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX3_DISABLE_SHFT                                0xc
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX2_DISABLE_BMSK                              0x800
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_BBRX2_DISABLE_SHFT                                0xb
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_GLOBAL_DISABLE_BMSK                             0x400
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_GLOBAL_DISABLE_SHFT                               0xa
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DC_DISABLE_BMSK                            0x200
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DC_DISABLE_SHFT                              0x9
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_MIMO_DISABLE_BMSK                          0x100
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_MIMO_DISABLE_SHFT                            0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_NB1_ABOVE_CAT2_DISABLE_BMSK                      0x80
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_NB1_ABOVE_CAT2_DISABLE_SHFT                       0x7
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_NB1_ABOVE_CAT1_DISABLE_BMSK                      0x40
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_NB1_ABOVE_CAT1_DISABLE_SHFT                       0x6
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_TDSCDMA_DISABLE_BMSK                             0x20
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_TDSCDMA_DISABLE_SHFT                              0x5
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DISABLE_BMSK                                0x10
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_HSPA_DISABLE_SHFT                                 0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_NB1_DISABLE_BMSK                                  0x8
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_NB1_DISABLE_SHFT                                  0x3
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_WCDMA_DISABLE_BMSK                                0x4
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_WCDMA_DISABLE_SHFT                                0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_DO_DISABLE_BMSK                                   0x2
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_DO_DISABLE_SHFT                                   0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_1X_DISABLE_BMSK                                   0x1
#define HWIO_QFPROM_RAW_FEAT_CONFIG_ROW0_LSB_MODEM_1X_DISABLE_SHFT                                   0x0

#endif /* __MODEM_QFPROM_H__ */
