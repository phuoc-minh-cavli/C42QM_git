#ifndef __MSMHWIOBASE_H__
#define __MSMHWIOBASE_H__
/*
===========================================================================
*/
/**
  @file msmhwiobase.h
  @brief Auto-generated HWIO base include file.
*/
/*
  ===========================================================================

  Copyright (c) 2018 Qualcomm Technologies Incorporated.
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

  $Header: //components/rel/rpm.bf/2.1.3/core/api/systemdrivers/hwio/mdm9x05/msmhwiobase.h#1 $
  $DateTime: 2018/12/26 21:36:05 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * BASE: RPM
 *--------------------------------------------------------------------------*/

#define RPM_BASE                                                    0x00000000
#define RPM_BASE_SIZE                                               0x00100000
#define RPM_BASE_PHYS                                               0x00000000

/*----------------------------------------------------------------------------
 * BASE: RPM_CODE_RAM_START_ADDRESS
 *--------------------------------------------------------------------------*/

#define RPM_CODE_RAM_START_ADDRESS_BASE                             0x00200000
#define RPM_CODE_RAM_START_ADDRESS_BASE_SIZE                        0x00000000
#define RPM_CODE_RAM_START_ADDRESS_BASE_PHYS                        0x00200000

/*----------------------------------------------------------------------------
 * BASE: RPM_MSG_RAM
 *--------------------------------------------------------------------------*/

#define RPM_MSG_RAM_BASE                                            0x60060000
#define RPM_MSG_RAM_BASE_SIZE                                       0x00003000
#define RPM_MSG_RAM_BASE_PHYS                                       0x60060000

/*----------------------------------------------------------------------------
 * BASE: SECURITY_CONTROL
 *--------------------------------------------------------------------------*/

#define SECURITY_CONTROL_BASE                                       0x600a0000
#define SECURITY_CONTROL_BASE_SIZE                                  0x00010000
#define SECURITY_CONTROL_BASE_PHYS                                  0x600a0000

/*----------------------------------------------------------------------------
 * BASE: PRNG_PRNG_TOP
 *--------------------------------------------------------------------------*/

#define PRNG_PRNG_TOP_BASE                                          0x600e0000
#define PRNG_PRNG_TOP_BASE_SIZE                                     0x00010000
#define PRNG_PRNG_TOP_BASE_PHYS                                     0x600e0000

/*----------------------------------------------------------------------------
 * BASE: BOOT_ROM_START_ADDRESS
 *--------------------------------------------------------------------------*/

#define BOOT_ROM_START_ADDRESS_BASE                                 0x60100000
#define BOOT_ROM_START_ADDRESS_BASE_SIZE                            0x00000000
#define BOOT_ROM_START_ADDRESS_BASE_PHYS                            0x60100000

/*----------------------------------------------------------------------------
 * BASE: BIMC
 *--------------------------------------------------------------------------*/

#define BIMC_BASE                                                   0x60400000
#define BIMC_BASE_SIZE                                              0x00080000
#define BIMC_BASE_PHYS                                              0x60400000

/*----------------------------------------------------------------------------
 * BASE: MPM2_MPM
 *--------------------------------------------------------------------------*/

#define MPM2_MPM_BASE                                               0x604a0000
#define MPM2_MPM_BASE_SIZE                                          0x00010000
#define MPM2_MPM_BASE_PHYS                                          0x604a0000

/*----------------------------------------------------------------------------
 * BASE: DEHR_WRAPPER_8K_R01
 *--------------------------------------------------------------------------*/

#define DEHR_WRAPPER_8K_R01_BASE                                    0x604b0000
#define DEHR_WRAPPER_8K_R01_BASE_SIZE                               0x00008000
#define DEHR_WRAPPER_8K_R01_BASE_PHYS                               0x604b0000

/*----------------------------------------------------------------------------
 * BASE: PC_NOC
 *--------------------------------------------------------------------------*/

#define PC_NOC_BASE                                                 0x60500000
#define PC_NOC_BASE_SIZE                                            0x00017080
#define PC_NOC_BASE_PHYS                                            0x60500000

/*----------------------------------------------------------------------------
 * BASE: CRYPTO0_CRYPTO_TOP
 *--------------------------------------------------------------------------*/

#define CRYPTO0_CRYPTO_TOP_BASE                                     0x60700000
#define CRYPTO0_CRYPTO_TOP_BASE_SIZE                                0x00040000
#define CRYPTO0_CRYPTO_TOP_BASE_PHYS                                0x60700000

/*----------------------------------------------------------------------------
 * BASE: TLMM
 *--------------------------------------------------------------------------*/

#define TLMM_BASE                                                   0x61000000
#define TLMM_BASE_SIZE                                              0x00400000
#define TLMM_BASE_PHYS                                              0x61000000

/*----------------------------------------------------------------------------
 * BASE: CLK_CTL
 *--------------------------------------------------------------------------*/

#define CLK_CTL_BASE                                                0x61800000
#define CLK_CTL_BASE_SIZE                                           0x00100000
#define CLK_CTL_BASE_PHYS                                           0x61800000

/*----------------------------------------------------------------------------
 * BASE: CORE_TOP_CSR
 *--------------------------------------------------------------------------*/

#define CORE_TOP_CSR_BASE                                           0x61900000
#define CORE_TOP_CSR_BASE_SIZE                                      0x00100000
#define CORE_TOP_CSR_BASE_PHYS                                      0x61900000

/*----------------------------------------------------------------------------
 * BASE: PMIC_ARB
 *--------------------------------------------------------------------------*/

#define PMIC_ARB_BASE                                               0x62000000
#define PMIC_ARB_BASE_SIZE                                          0x01c00000
#define PMIC_ARB_BASE_PHYS                                          0x62000000

/*----------------------------------------------------------------------------
 * BASE: QDSS_SOC_DBG
 *--------------------------------------------------------------------------*/

#define QDSS_SOC_DBG_BASE                                           0x66000000
#define QDSS_SOC_DBG_BASE_SIZE                                      0x01000000
#define QDSS_SOC_DBG_BASE_PHYS                                      0x66000000

/*----------------------------------------------------------------------------
 * BASE: BLSP1_BLSP
 *--------------------------------------------------------------------------*/

#define BLSP1_BLSP_BASE                                             0x67880000
#define BLSP1_BLSP_BASE_SIZE                                        0x00040000
#define BLSP1_BLSP_BASE_PHYS                                        0x67880000


#endif /* __MSMHWIOBASE_H__ */
