#ifndef __HWIO_H__
#define __HWIO_H__
/*
===========================================================================
*/
/**
  @file hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    MDM9205 (Twizy) [twizy_v1.0_p3q3r35_MTO]
 
  This file contains HWIO register definitions for the following modules:
    DEHR_BIMC
    DEHR_VMIDMT
    DEHR_WRAPPER_8K_R01
    DEHR_XPU


  Generation parameters: 
  { u'filename': 'hwio.h',
    u'module-filter-exclude': { },
    u'module-filter-include': { },
    u'modules': [ u'DEHR_BIMC',
                  u'DEHR_VMIDMT',
                  u'DEHR_WRAPPER_8K_R01',
                  u'DEHR_XPU']}
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

  $Header: //components/rel/core.mpss/3.10/products/dehr/inc/dehr_hwio.h#1 $
  $DateTime: 2019/06/05 23:33:15 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: DEHR_BIMC
 *--------------------------------------------------------------------------*/
#define DEHR_WRAPPER_8K_R01_BASE                                    0xe11b0000
#define DEHR_WRAPPER_8K_R01_BASE_SIZE                               0x00008000
#define DEHR_WRAPPER_8K_R01_BASE_PHYS                               0x004b0000
#define DEHR_BIMC_REG_BASE                                               (DEHR_WRAPPER_8K_R01_BASE      + 0x00002000)
#define DEHR_BIMC_REG_BASE_SIZE                                          0x2000
#define DEHR_BIMC_REG_BASE_USED                                          0x1000

#define HWIO_DEHR_BIMC_DEHR_HW_VERSION_ADDR                              (DEHR_BIMC_REG_BASE      + 0x00000000)
#define HWIO_DEHR_BIMC_DEHR_HW_VERSION_RMSK                                0xffffff
#define HWIO_DEHR_BIMC_DEHR_HW_VERSION_IN          \
        in_dword(HWIO_DEHR_BIMC_DEHR_HW_VERSION_ADDR)
#define HWIO_DEHR_BIMC_DEHR_HW_VERSION_INM(m)      \
        in_dword_masked(HWIO_DEHR_BIMC_DEHR_HW_VERSION_ADDR, m)
#define HWIO_DEHR_BIMC_DEHR_HW_VERSION_MAJOR_BMSK                          0xff0000
#define HWIO_DEHR_BIMC_DEHR_HW_VERSION_MAJOR_SHFT                              0x10
#define HWIO_DEHR_BIMC_DEHR_HW_VERSION_MINOR_BMSK                            0xff00
#define HWIO_DEHR_BIMC_DEHR_HW_VERSION_MINOR_SHFT                               0x8
#define HWIO_DEHR_BIMC_DEHR_HW_VERSION_STEP_BMSK                               0xff
#define HWIO_DEHR_BIMC_DEHR_HW_VERSION_STEP_SHFT                                0x0

#define HWIO_DEHR_BIMC_DEHR_HW_INFO_ADDR                                 (DEHR_BIMC_REG_BASE      + 0x00000004)
#define HWIO_DEHR_BIMC_DEHR_HW_INFO_RMSK                                 0xffffffff
#define HWIO_DEHR_BIMC_DEHR_HW_INFO_IN          \
        in_dword(HWIO_DEHR_BIMC_DEHR_HW_INFO_ADDR)
#define HWIO_DEHR_BIMC_DEHR_HW_INFO_INM(m)      \
        in_dword_masked(HWIO_DEHR_BIMC_DEHR_HW_INFO_ADDR, m)
#define HWIO_DEHR_BIMC_DEHR_HW_INFO_MAJOR_BMSK                           0xff000000
#define HWIO_DEHR_BIMC_DEHR_HW_INFO_MAJOR_SHFT                                 0x18
#define HWIO_DEHR_BIMC_DEHR_HW_INFO_BRANCH_BMSK                            0xff0000
#define HWIO_DEHR_BIMC_DEHR_HW_INFO_BRANCH_SHFT                                0x10
#define HWIO_DEHR_BIMC_DEHR_HW_INFO_MINOR_BMSK                               0xff00
#define HWIO_DEHR_BIMC_DEHR_HW_INFO_MINOR_SHFT                                  0x8
#define HWIO_DEHR_BIMC_DEHR_HW_INFO_ECO_BMSK                                   0xff
#define HWIO_DEHR_BIMC_DEHR_HW_INFO_ECO_SHFT                                    0x0

#define HWIO_DEHR_BIMC_DEHR_CGC_CFG_ADDR                                 (DEHR_BIMC_REG_BASE      + 0x00000010)
#define HWIO_DEHR_BIMC_DEHR_CGC_CFG_RMSK                                        0x1
#define HWIO_DEHR_BIMC_DEHR_CGC_CFG_IN          \
        in_dword(HWIO_DEHR_BIMC_DEHR_CGC_CFG_ADDR)
#define HWIO_DEHR_BIMC_DEHR_CGC_CFG_INM(m)      \
        in_dword_masked(HWIO_DEHR_BIMC_DEHR_CGC_CFG_ADDR, m)
#define HWIO_DEHR_BIMC_DEHR_CGC_CFG_OUT(v)      \
        out_dword(HWIO_DEHR_BIMC_DEHR_CGC_CFG_ADDR,v)
#define HWIO_DEHR_BIMC_DEHR_CGC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_BIMC_DEHR_CGC_CFG_ADDR,m,v,HWIO_DEHR_BIMC_DEHR_CGC_CFG_IN)
#define HWIO_DEHR_BIMC_DEHR_CGC_CFG_SW_CGC_DISABLE_BMSK                         0x1
#define HWIO_DEHR_BIMC_DEHR_CGC_CFG_SW_CGC_DISABLE_SHFT                         0x0

#define HWIO_DEHR_BIMC_DEHR_MSA_CFG_ADDR                                 (DEHR_BIMC_REG_BASE      + 0x00000104)
#define HWIO_DEHR_BIMC_DEHR_MSA_CFG_RMSK                                       0x11
#define HWIO_DEHR_BIMC_DEHR_MSA_CFG_IN          \
        in_dword(HWIO_DEHR_BIMC_DEHR_MSA_CFG_ADDR)
#define HWIO_DEHR_BIMC_DEHR_MSA_CFG_INM(m)      \
        in_dword_masked(HWIO_DEHR_BIMC_DEHR_MSA_CFG_ADDR, m)
#define HWIO_DEHR_BIMC_DEHR_MSA_CFG_MSA_DEHR_ENA_BMSK                          0x10
#define HWIO_DEHR_BIMC_DEHR_MSA_CFG_MSA_DEHR_ENA_SHFT                           0x4
#define HWIO_DEHR_BIMC_DEHR_MSA_CFG_MSA_DEHR_LOCK_BMSK                          0x1
#define HWIO_DEHR_BIMC_DEHR_MSA_CFG_MSA_DEHR_LOCK_SHFT                          0x0

#define HWIO_DEHR_BIMC_DEHR_RAM_CFG_ADDR                                 (DEHR_BIMC_REG_BASE      + 0x00000140)
#define HWIO_DEHR_BIMC_DEHR_RAM_CFG_RMSK                                     0xffff
#define HWIO_DEHR_BIMC_DEHR_RAM_CFG_IN          \
        in_dword(HWIO_DEHR_BIMC_DEHR_RAM_CFG_ADDR)
#define HWIO_DEHR_BIMC_DEHR_RAM_CFG_INM(m)      \
        in_dword_masked(HWIO_DEHR_BIMC_DEHR_RAM_CFG_ADDR, m)
#define HWIO_DEHR_BIMC_DEHR_RAM_CFG_OUT(v)      \
        out_dword(HWIO_DEHR_BIMC_DEHR_RAM_CFG_ADDR,v)
#define HWIO_DEHR_BIMC_DEHR_RAM_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_BIMC_DEHR_RAM_CFG_ADDR,m,v,HWIO_DEHR_BIMC_DEHR_RAM_CFG_IN)
#define HWIO_DEHR_BIMC_DEHR_RAM_CFG_CFG_BASE_ADDR_BMSK                       0xffff
#define HWIO_DEHR_BIMC_DEHR_RAM_CFG_CFG_BASE_ADDR_SHFT                          0x0

#define HWIO_DEHR_BIMC_DEHR_DMA_STATUS_ADDR                              (DEHR_BIMC_REG_BASE      + 0x00000200)
#define HWIO_DEHR_BIMC_DEHR_DMA_STATUS_RMSK                                0x110011
#define HWIO_DEHR_BIMC_DEHR_DMA_STATUS_IN          \
        in_dword(HWIO_DEHR_BIMC_DEHR_DMA_STATUS_ADDR)
#define HWIO_DEHR_BIMC_DEHR_DMA_STATUS_INM(m)      \
        in_dword_masked(HWIO_DEHR_BIMC_DEHR_DMA_STATUS_ADDR, m)
#define HWIO_DEHR_BIMC_DEHR_DMA_STATUS_OUT(v)      \
        out_dword(HWIO_DEHR_BIMC_DEHR_DMA_STATUS_ADDR,v)
#define HWIO_DEHR_BIMC_DEHR_DMA_STATUS_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_BIMC_DEHR_DMA_STATUS_ADDR,m,v,HWIO_DEHR_BIMC_DEHR_DMA_STATUS_IN)
#define HWIO_DEHR_BIMC_DEHR_DMA_STATUS_RESTORE_IN_PROGRESS_BMSK            0x100000
#define HWIO_DEHR_BIMC_DEHR_DMA_STATUS_RESTORE_IN_PROGRESS_SHFT                0x14
#define HWIO_DEHR_BIMC_DEHR_DMA_STATUS_RESTORE_DONE_BMSK                    0x10000
#define HWIO_DEHR_BIMC_DEHR_DMA_STATUS_RESTORE_DONE_SHFT                       0x10
#define HWIO_DEHR_BIMC_DEHR_DMA_STATUS_SAVE_IN_PROGRESS_BMSK                   0x10
#define HWIO_DEHR_BIMC_DEHR_DMA_STATUS_SAVE_IN_PROGRESS_SHFT                    0x4
#define HWIO_DEHR_BIMC_DEHR_DMA_STATUS_SAVE_DONE_BMSK                           0x1
#define HWIO_DEHR_BIMC_DEHR_DMA_STATUS_SAVE_DONE_SHFT                           0x0

#define HWIO_DEHR_BIMC_DEHR_DBG_CFG_ADDR                                 (DEHR_BIMC_REG_BASE      + 0x00000400)
#define HWIO_DEHR_BIMC_DEHR_DBG_CFG_RMSK                                        0x1
#define HWIO_DEHR_BIMC_DEHR_DBG_CFG_IN          \
        in_dword(HWIO_DEHR_BIMC_DEHR_DBG_CFG_ADDR)
#define HWIO_DEHR_BIMC_DEHR_DBG_CFG_INM(m)      \
        in_dword_masked(HWIO_DEHR_BIMC_DEHR_DBG_CFG_ADDR, m)
#define HWIO_DEHR_BIMC_DEHR_DBG_CFG_OUT(v)      \
        out_dword(HWIO_DEHR_BIMC_DEHR_DBG_CFG_ADDR,v)
#define HWIO_DEHR_BIMC_DEHR_DBG_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_BIMC_DEHR_DBG_CFG_ADDR,m,v,HWIO_DEHR_BIMC_DEHR_DBG_CFG_IN)
#define HWIO_DEHR_BIMC_DEHR_DBG_CFG_DBG_ENA_BMSK                                0x1
#define HWIO_DEHR_BIMC_DEHR_DBG_CFG_DBG_ENA_SHFT                                0x0

#define HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_ADDR                              (DEHR_BIMC_REG_BASE      + 0x00000404)
#define HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_RMSK                                   0x101
#define HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_IN          \
        in_dword(HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_ADDR)
#define HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_INM(m)      \
        in_dword_masked(HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_ADDR, m)
#define HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_OUT(v)      \
        out_dword(HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_ADDR,v)
#define HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_ADDR,m,v,HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_IN)
#define HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_SW_RESTORE_REQ_BMSK                    0x100
#define HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_SW_RESTORE_REQ_SHFT                      0x8
#define HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_SW_SAVE_REQ_BMSK                         0x1
#define HWIO_DEHR_BIMC_DEHR_SW_DMA_CFG_SW_SAVE_REQ_SHFT                         0x0

#define HWIO_DEHR_BIMC_DEHR_DBG_ADDR_ADDR                                (DEHR_BIMC_REG_BASE      + 0x00000408)
#define HWIO_DEHR_BIMC_DEHR_DBG_ADDR_RMSK                                0xffffffff
#define HWIO_DEHR_BIMC_DEHR_DBG_ADDR_IN          \
        in_dword(HWIO_DEHR_BIMC_DEHR_DBG_ADDR_ADDR)
#define HWIO_DEHR_BIMC_DEHR_DBG_ADDR_INM(m)      \
        in_dword_masked(HWIO_DEHR_BIMC_DEHR_DBG_ADDR_ADDR, m)
#define HWIO_DEHR_BIMC_DEHR_DBG_ADDR_OUT(v)      \
        out_dword(HWIO_DEHR_BIMC_DEHR_DBG_ADDR_ADDR,v)
#define HWIO_DEHR_BIMC_DEHR_DBG_ADDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_BIMC_DEHR_DBG_ADDR_ADDR,m,v,HWIO_DEHR_BIMC_DEHR_DBG_ADDR_IN)
#define HWIO_DEHR_BIMC_DEHR_DBG_ADDR_LINK_END_ADDR_BMSK                  0xffff0000
#define HWIO_DEHR_BIMC_DEHR_DBG_ADDR_LINK_END_ADDR_SHFT                        0x10
#define HWIO_DEHR_BIMC_DEHR_DBG_ADDR_LINK_START_ADDR_BMSK                    0xffff
#define HWIO_DEHR_BIMC_DEHR_DBG_ADDR_LINK_START_ADDR_SHFT                       0x0

#define HWIO_DEHR_BIMC_DEHR_SEC_CFG_ADDR                                 (DEHR_BIMC_REG_BASE      + 0x00001000)
#define HWIO_DEHR_BIMC_DEHR_SEC_CFG_RMSK                                       0x11
#define HWIO_DEHR_BIMC_DEHR_SEC_CFG_IN          \
        in_dword(HWIO_DEHR_BIMC_DEHR_SEC_CFG_ADDR)
#define HWIO_DEHR_BIMC_DEHR_SEC_CFG_INM(m)      \
        in_dword_masked(HWIO_DEHR_BIMC_DEHR_SEC_CFG_ADDR, m)
#define HWIO_DEHR_BIMC_DEHR_SEC_CFG_OUT(v)      \
        out_dword(HWIO_DEHR_BIMC_DEHR_SEC_CFG_ADDR,v)
#define HWIO_DEHR_BIMC_DEHR_SEC_CFG_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_BIMC_DEHR_SEC_CFG_ADDR,m,v,HWIO_DEHR_BIMC_DEHR_SEC_CFG_IN)
#define HWIO_DEHR_BIMC_DEHR_SEC_CFG_DEHR_ENA_BMSK                              0x10
#define HWIO_DEHR_BIMC_DEHR_SEC_CFG_DEHR_ENA_SHFT                               0x4
#define HWIO_DEHR_BIMC_DEHR_SEC_CFG_DEHR_LOCK_BMSK                              0x1
#define HWIO_DEHR_BIMC_DEHR_SEC_CFG_DEHR_LOCK_SHFT                              0x0

/*----------------------------------------------------------------------------
 * MODULE: DEHR_VMIDMT
 *--------------------------------------------------------------------------*/

#define DEHR_VMIDMT_REG_BASE                                      (DEHR_WRAPPER_8K_R01_BASE      + 0x00001000)
#define DEHR_VMIDMT_REG_BASE_SIZE                                 0x1000
#define DEHR_VMIDMT_REG_BASE_USED                                 0xe00

#define HWIO_DEHR_VMIDMT_SCR0_ADDR                                (DEHR_VMIDMT_REG_BASE      + 0x00000000)
#define HWIO_DEHR_VMIDMT_SCR0_RMSK                                0x3fd707f5
#define HWIO_DEHR_VMIDMT_SCR0_IN          \
        in_dword(HWIO_DEHR_VMIDMT_SCR0_ADDR)
#define HWIO_DEHR_VMIDMT_SCR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SCR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_SCR0_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_SCR0_ADDR,v)
#define HWIO_DEHR_VMIDMT_SCR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_SCR0_ADDR,m,v,HWIO_DEHR_VMIDMT_SCR0_IN)
#define HWIO_DEHR_VMIDMT_SCR0_NSCFG_BMSK                          0x30000000
#define HWIO_DEHR_VMIDMT_SCR0_NSCFG_SHFT                                0x1c
#define HWIO_DEHR_VMIDMT_SCR0_WACFG_BMSK                           0xc000000
#define HWIO_DEHR_VMIDMT_SCR0_WACFG_SHFT                                0x1a
#define HWIO_DEHR_VMIDMT_SCR0_RACFG_BMSK                           0x3000000
#define HWIO_DEHR_VMIDMT_SCR0_RACFG_SHFT                                0x18
#define HWIO_DEHR_VMIDMT_SCR0_SHCFG_BMSK                            0xc00000
#define HWIO_DEHR_VMIDMT_SCR0_SHCFG_SHFT                                0x16
#define HWIO_DEHR_VMIDMT_SCR0_MTCFG_BMSK                            0x100000
#define HWIO_DEHR_VMIDMT_SCR0_MTCFG_SHFT                                0x14
#define HWIO_DEHR_VMIDMT_SCR0_MEMATTR_BMSK                           0x70000
#define HWIO_DEHR_VMIDMT_SCR0_MEMATTR_SHFT                              0x10
#define HWIO_DEHR_VMIDMT_SCR0_USFCFG_BMSK                              0x400
#define HWIO_DEHR_VMIDMT_SCR0_USFCFG_SHFT                                0xa
#define HWIO_DEHR_VMIDMT_SCR0_GSE_BMSK                                 0x200
#define HWIO_DEHR_VMIDMT_SCR0_GSE_SHFT                                   0x9
#define HWIO_DEHR_VMIDMT_SCR0_STALLD_BMSK                              0x100
#define HWIO_DEHR_VMIDMT_SCR0_STALLD_SHFT                                0x8
#define HWIO_DEHR_VMIDMT_SCR0_TRANSIENTCFG_BMSK                         0xc0
#define HWIO_DEHR_VMIDMT_SCR0_TRANSIENTCFG_SHFT                          0x6
#define HWIO_DEHR_VMIDMT_SCR0_GCFGFIE_BMSK                              0x20
#define HWIO_DEHR_VMIDMT_SCR0_GCFGFIE_SHFT                               0x5
#define HWIO_DEHR_VMIDMT_SCR0_GCFGERE_BMSK                              0x10
#define HWIO_DEHR_VMIDMT_SCR0_GCFGERE_SHFT                               0x4
#define HWIO_DEHR_VMIDMT_SCR0_GFIE_BMSK                                  0x4
#define HWIO_DEHR_VMIDMT_SCR0_GFIE_SHFT                                  0x2
#define HWIO_DEHR_VMIDMT_SCR0_CLIENTPD_BMSK                              0x1
#define HWIO_DEHR_VMIDMT_SCR0_CLIENTPD_SHFT                              0x0

#define HWIO_DEHR_VMIDMT_SCR1_ADDR                                (DEHR_VMIDMT_REG_BASE      + 0x00000004)
#define HWIO_DEHR_VMIDMT_SCR1_RMSK                                 0x1000100
#define HWIO_DEHR_VMIDMT_SCR1_IN          \
        in_dword(HWIO_DEHR_VMIDMT_SCR1_ADDR)
#define HWIO_DEHR_VMIDMT_SCR1_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SCR1_ADDR, m)
#define HWIO_DEHR_VMIDMT_SCR1_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_SCR1_ADDR,v)
#define HWIO_DEHR_VMIDMT_SCR1_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_SCR1_ADDR,m,v,HWIO_DEHR_VMIDMT_SCR1_IN)
#define HWIO_DEHR_VMIDMT_SCR1_GASRAE_BMSK                          0x1000000
#define HWIO_DEHR_VMIDMT_SCR1_GASRAE_SHFT                               0x18
#define HWIO_DEHR_VMIDMT_SCR1_NSNUMSMRGO_BMSK                          0x100
#define HWIO_DEHR_VMIDMT_SCR1_NSNUMSMRGO_SHFT                            0x8

#define HWIO_DEHR_VMIDMT_SCR2_ADDR                                (DEHR_VMIDMT_REG_BASE      + 0x00000008)
#define HWIO_DEHR_VMIDMT_SCR2_RMSK                                      0x1f
#define HWIO_DEHR_VMIDMT_SCR2_IN          \
        in_dword(HWIO_DEHR_VMIDMT_SCR2_ADDR)
#define HWIO_DEHR_VMIDMT_SCR2_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SCR2_ADDR, m)
#define HWIO_DEHR_VMIDMT_SCR2_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_SCR2_ADDR,v)
#define HWIO_DEHR_VMIDMT_SCR2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_SCR2_ADDR,m,v,HWIO_DEHR_VMIDMT_SCR2_IN)
#define HWIO_DEHR_VMIDMT_SCR2_BPVMID_BMSK                               0x1f
#define HWIO_DEHR_VMIDMT_SCR2_BPVMID_SHFT                                0x0

#define HWIO_DEHR_VMIDMT_SACR_ADDR                                (DEHR_VMIDMT_REG_BASE      + 0x00000010)
#define HWIO_DEHR_VMIDMT_SACR_RMSK                                0x70000013
#define HWIO_DEHR_VMIDMT_SACR_IN          \
        in_dword(HWIO_DEHR_VMIDMT_SACR_ADDR)
#define HWIO_DEHR_VMIDMT_SACR_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SACR_ADDR, m)
#define HWIO_DEHR_VMIDMT_SACR_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_SACR_ADDR,v)
#define HWIO_DEHR_VMIDMT_SACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_SACR_ADDR,m,v,HWIO_DEHR_VMIDMT_SACR_IN)
#define HWIO_DEHR_VMIDMT_SACR_BPRCNSH_BMSK                        0x40000000
#define HWIO_DEHR_VMIDMT_SACR_BPRCNSH_SHFT                              0x1e
#define HWIO_DEHR_VMIDMT_SACR_BPRCISH_BMSK                        0x20000000
#define HWIO_DEHR_VMIDMT_SACR_BPRCISH_SHFT                              0x1d
#define HWIO_DEHR_VMIDMT_SACR_BPRCOSH_BMSK                        0x10000000
#define HWIO_DEHR_VMIDMT_SACR_BPRCOSH_SHFT                              0x1c
#define HWIO_DEHR_VMIDMT_SACR_BPREQPRIORITYCFG_BMSK                     0x10
#define HWIO_DEHR_VMIDMT_SACR_BPREQPRIORITYCFG_SHFT                      0x4
#define HWIO_DEHR_VMIDMT_SACR_BPREQPRIORITY_BMSK                         0x3
#define HWIO_DEHR_VMIDMT_SACR_BPREQPRIORITY_SHFT                         0x0

#define HWIO_DEHR_VMIDMT_SIDR0_ADDR                               (DEHR_VMIDMT_REG_BASE      + 0x00000020)
#define HWIO_DEHR_VMIDMT_SIDR0_RMSK                               0x88001eff
#define HWIO_DEHR_VMIDMT_SIDR0_IN          \
        in_dword(HWIO_DEHR_VMIDMT_SIDR0_ADDR)
#define HWIO_DEHR_VMIDMT_SIDR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SIDR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_SIDR0_SES_BMSK                           0x80000000
#define HWIO_DEHR_VMIDMT_SIDR0_SES_SHFT                                 0x1f
#define HWIO_DEHR_VMIDMT_SIDR0_SMS_BMSK                            0x8000000
#define HWIO_DEHR_VMIDMT_SIDR0_SMS_SHFT                                 0x1b
#define HWIO_DEHR_VMIDMT_SIDR0_NUMSIDB_BMSK                           0x1e00
#define HWIO_DEHR_VMIDMT_SIDR0_NUMSIDB_SHFT                              0x9
#define HWIO_DEHR_VMIDMT_SIDR0_NUMSMRG_BMSK                             0xff
#define HWIO_DEHR_VMIDMT_SIDR0_NUMSMRG_SHFT                              0x0

#define HWIO_DEHR_VMIDMT_SIDR1_ADDR                               (DEHR_VMIDMT_REG_BASE      + 0x00000024)
#define HWIO_DEHR_VMIDMT_SIDR1_RMSK                                   0x9f00
#define HWIO_DEHR_VMIDMT_SIDR1_IN          \
        in_dword(HWIO_DEHR_VMIDMT_SIDR1_ADDR)
#define HWIO_DEHR_VMIDMT_SIDR1_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SIDR1_ADDR, m)
#define HWIO_DEHR_VMIDMT_SIDR1_SMCD_BMSK                              0x8000
#define HWIO_DEHR_VMIDMT_SIDR1_SMCD_SHFT                                 0xf
#define HWIO_DEHR_VMIDMT_SIDR1_SSDTP_BMSK                             0x1000
#define HWIO_DEHR_VMIDMT_SIDR1_SSDTP_SHFT                                0xc
#define HWIO_DEHR_VMIDMT_SIDR1_NUMSSDNDX_BMSK                          0xf00
#define HWIO_DEHR_VMIDMT_SIDR1_NUMSSDNDX_SHFT                            0x8

#define HWIO_DEHR_VMIDMT_SIDR2_ADDR                               (DEHR_VMIDMT_REG_BASE      + 0x00000028)
#define HWIO_DEHR_VMIDMT_SIDR2_RMSK                                     0xff
#define HWIO_DEHR_VMIDMT_SIDR2_IN          \
        in_dword(HWIO_DEHR_VMIDMT_SIDR2_ADDR)
#define HWIO_DEHR_VMIDMT_SIDR2_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SIDR2_ADDR, m)
#define HWIO_DEHR_VMIDMT_SIDR2_OAS_BMSK                                 0xf0
#define HWIO_DEHR_VMIDMT_SIDR2_OAS_SHFT                                  0x4
#define HWIO_DEHR_VMIDMT_SIDR2_IAS_BMSK                                  0xf
#define HWIO_DEHR_VMIDMT_SIDR2_IAS_SHFT                                  0x0

#define HWIO_DEHR_VMIDMT_SIDR4_ADDR                               (DEHR_VMIDMT_REG_BASE      + 0x00000030)
#define HWIO_DEHR_VMIDMT_SIDR4_RMSK                               0xffffffff
#define HWIO_DEHR_VMIDMT_SIDR4_IN          \
        in_dword(HWIO_DEHR_VMIDMT_SIDR4_ADDR)
#define HWIO_DEHR_VMIDMT_SIDR4_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SIDR4_ADDR, m)
#define HWIO_DEHR_VMIDMT_SIDR4_MAJOR_BMSK                         0xf0000000
#define HWIO_DEHR_VMIDMT_SIDR4_MAJOR_SHFT                               0x1c
#define HWIO_DEHR_VMIDMT_SIDR4_MINOR_BMSK                          0xfff0000
#define HWIO_DEHR_VMIDMT_SIDR4_MINOR_SHFT                               0x10
#define HWIO_DEHR_VMIDMT_SIDR4_STEP_BMSK                              0xffff
#define HWIO_DEHR_VMIDMT_SIDR4_STEP_SHFT                                 0x0

#define HWIO_DEHR_VMIDMT_SIDR5_ADDR                               (DEHR_VMIDMT_REG_BASE      + 0x00000034)
#define HWIO_DEHR_VMIDMT_SIDR5_RMSK                                 0xff03ff
#define HWIO_DEHR_VMIDMT_SIDR5_IN          \
        in_dword(HWIO_DEHR_VMIDMT_SIDR5_ADDR)
#define HWIO_DEHR_VMIDMT_SIDR5_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SIDR5_ADDR, m)
#define HWIO_DEHR_VMIDMT_SIDR5_NUMMSDRB_BMSK                        0xff0000
#define HWIO_DEHR_VMIDMT_SIDR5_NUMMSDRB_SHFT                            0x10
#define HWIO_DEHR_VMIDMT_SIDR5_MSAE_BMSK                               0x200
#define HWIO_DEHR_VMIDMT_SIDR5_MSAE_SHFT                                 0x9
#define HWIO_DEHR_VMIDMT_SIDR5_QRIBE_BMSK                              0x100
#define HWIO_DEHR_VMIDMT_SIDR5_QRIBE_SHFT                                0x8
#define HWIO_DEHR_VMIDMT_SIDR5_NVMID_BMSK                               0xff
#define HWIO_DEHR_VMIDMT_SIDR5_NVMID_SHFT                                0x0

#define HWIO_DEHR_VMIDMT_SIDR7_ADDR                               (DEHR_VMIDMT_REG_BASE      + 0x0000003c)
#define HWIO_DEHR_VMIDMT_SIDR7_RMSK                                     0xff
#define HWIO_DEHR_VMIDMT_SIDR7_IN          \
        in_dword(HWIO_DEHR_VMIDMT_SIDR7_ADDR)
#define HWIO_DEHR_VMIDMT_SIDR7_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SIDR7_ADDR, m)
#define HWIO_DEHR_VMIDMT_SIDR7_MAJOR_BMSK                               0xf0
#define HWIO_DEHR_VMIDMT_SIDR7_MAJOR_SHFT                                0x4
#define HWIO_DEHR_VMIDMT_SIDR7_MINOR_BMSK                                0xf
#define HWIO_DEHR_VMIDMT_SIDR7_MINOR_SHFT                                0x0

#define HWIO_DEHR_VMIDMT_SGFAR0_ADDR                              (DEHR_VMIDMT_REG_BASE      + 0x00000040)
#define HWIO_DEHR_VMIDMT_SGFAR0_RMSK                              0xffffffff
#define HWIO_DEHR_VMIDMT_SGFAR0_IN          \
        in_dword(HWIO_DEHR_VMIDMT_SGFAR0_ADDR)
#define HWIO_DEHR_VMIDMT_SGFAR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SGFAR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_SGFAR0_SGFEA0_BMSK                       0xffffffff
#define HWIO_DEHR_VMIDMT_SGFAR0_SGFEA0_SHFT                              0x0

#define HWIO_DEHR_VMIDMT_SGFSR_ADDR                               (DEHR_VMIDMT_REG_BASE      + 0x00000048)
#define HWIO_DEHR_VMIDMT_SGFSR_RMSK                               0xc0000022
#define HWIO_DEHR_VMIDMT_SGFSR_IN          \
        in_dword(HWIO_DEHR_VMIDMT_SGFSR_ADDR)
#define HWIO_DEHR_VMIDMT_SGFSR_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SGFSR_ADDR, m)
#define HWIO_DEHR_VMIDMT_SGFSR_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_SGFSR_ADDR,v)
#define HWIO_DEHR_VMIDMT_SGFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_SGFSR_ADDR,m,v,HWIO_DEHR_VMIDMT_SGFSR_IN)
#define HWIO_DEHR_VMIDMT_SGFSR_MULTI_CLIENT_BMSK                  0x80000000
#define HWIO_DEHR_VMIDMT_SGFSR_MULTI_CLIENT_SHFT                        0x1f
#define HWIO_DEHR_VMIDMT_SGFSR_MULTI_CFG_BMSK                     0x40000000
#define HWIO_DEHR_VMIDMT_SGFSR_MULTI_CFG_SHFT                           0x1e
#define HWIO_DEHR_VMIDMT_SGFSR_CAF_BMSK                                 0x20
#define HWIO_DEHR_VMIDMT_SGFSR_CAF_SHFT                                  0x5
#define HWIO_DEHR_VMIDMT_SGFSR_USF_BMSK                                  0x2
#define HWIO_DEHR_VMIDMT_SGFSR_USF_SHFT                                  0x1

#define HWIO_DEHR_VMIDMT_SGFSRRESTORE_ADDR                        (DEHR_VMIDMT_REG_BASE      + 0x0000004c)
#define HWIO_DEHR_VMIDMT_SGFSRRESTORE_RMSK                        0xc0000022
#define HWIO_DEHR_VMIDMT_SGFSRRESTORE_IN          \
        in_dword(HWIO_DEHR_VMIDMT_SGFSRRESTORE_ADDR)
#define HWIO_DEHR_VMIDMT_SGFSRRESTORE_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SGFSRRESTORE_ADDR, m)
#define HWIO_DEHR_VMIDMT_SGFSRRESTORE_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_SGFSRRESTORE_ADDR,v)
#define HWIO_DEHR_VMIDMT_SGFSRRESTORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_SGFSRRESTORE_ADDR,m,v,HWIO_DEHR_VMIDMT_SGFSRRESTORE_IN)
#define HWIO_DEHR_VMIDMT_SGFSRRESTORE_MULTI_CLIENT_BMSK           0x80000000
#define HWIO_DEHR_VMIDMT_SGFSRRESTORE_MULTI_CLIENT_SHFT                 0x1f
#define HWIO_DEHR_VMIDMT_SGFSRRESTORE_MULTI_CFG_BMSK              0x40000000
#define HWIO_DEHR_VMIDMT_SGFSRRESTORE_MULTI_CFG_SHFT                    0x1e
#define HWIO_DEHR_VMIDMT_SGFSRRESTORE_CAF_BMSK                          0x20
#define HWIO_DEHR_VMIDMT_SGFSRRESTORE_CAF_SHFT                           0x5
#define HWIO_DEHR_VMIDMT_SGFSRRESTORE_USF_BMSK                           0x2
#define HWIO_DEHR_VMIDMT_SGFSRRESTORE_USF_SHFT                           0x1

#define HWIO_DEHR_VMIDMT_SGFSYNDR0_ADDR                           (DEHR_VMIDMT_REG_BASE      + 0x00000050)
#define HWIO_DEHR_VMIDMT_SGFSYNDR0_RMSK                                0x132
#define HWIO_DEHR_VMIDMT_SGFSYNDR0_IN          \
        in_dword(HWIO_DEHR_VMIDMT_SGFSYNDR0_ADDR)
#define HWIO_DEHR_VMIDMT_SGFSYNDR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SGFSYNDR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_SGFSYNDR0_MSSSELFAUTH_BMSK                    0x100
#define HWIO_DEHR_VMIDMT_SGFSYNDR0_MSSSELFAUTH_SHFT                      0x8
#define HWIO_DEHR_VMIDMT_SGFSYNDR0_NSATTR_BMSK                          0x20
#define HWIO_DEHR_VMIDMT_SGFSYNDR0_NSATTR_SHFT                           0x5
#define HWIO_DEHR_VMIDMT_SGFSYNDR0_NSSTATE_BMSK                         0x10
#define HWIO_DEHR_VMIDMT_SGFSYNDR0_NSSTATE_SHFT                          0x4
#define HWIO_DEHR_VMIDMT_SGFSYNDR0_WNR_BMSK                              0x2
#define HWIO_DEHR_VMIDMT_SGFSYNDR0_WNR_SHFT                              0x1

#define HWIO_DEHR_VMIDMT_SGFSYNDR1_ADDR                           (DEHR_VMIDMT_REG_BASE      + 0x00000054)
#define HWIO_DEHR_VMIDMT_SGFSYNDR1_RMSK                            0x1010001
#define HWIO_DEHR_VMIDMT_SGFSYNDR1_IN          \
        in_dword(HWIO_DEHR_VMIDMT_SGFSYNDR1_ADDR)
#define HWIO_DEHR_VMIDMT_SGFSYNDR1_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SGFSYNDR1_ADDR, m)
#define HWIO_DEHR_VMIDMT_SGFSYNDR1_MSDINDEX_BMSK                   0x1000000
#define HWIO_DEHR_VMIDMT_SGFSYNDR1_MSDINDEX_SHFT                        0x18
#define HWIO_DEHR_VMIDMT_SGFSYNDR1_SSDINDEX_BMSK                     0x10000
#define HWIO_DEHR_VMIDMT_SGFSYNDR1_SSDINDEX_SHFT                        0x10
#define HWIO_DEHR_VMIDMT_SGFSYNDR1_STREAMINDEX_BMSK                      0x1
#define HWIO_DEHR_VMIDMT_SGFSYNDR1_STREAMINDEX_SHFT                      0x0

#define HWIO_DEHR_VMIDMT_SGFSYNDR2_ADDR                           (DEHR_VMIDMT_REG_BASE      + 0x00000058)
#define HWIO_DEHR_VMIDMT_SGFSYNDR2_RMSK                           0x3f1fffff
#define HWIO_DEHR_VMIDMT_SGFSYNDR2_IN          \
        in_dword(HWIO_DEHR_VMIDMT_SGFSYNDR2_ADDR)
#define HWIO_DEHR_VMIDMT_SGFSYNDR2_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SGFSYNDR2_ADDR, m)
#define HWIO_DEHR_VMIDMT_SGFSYNDR2_ATID_BMSK                      0x3f000000
#define HWIO_DEHR_VMIDMT_SGFSYNDR2_ATID_SHFT                            0x18
#define HWIO_DEHR_VMIDMT_SGFSYNDR2_AVMID_BMSK                       0x1f0000
#define HWIO_DEHR_VMIDMT_SGFSYNDR2_AVMID_SHFT                           0x10
#define HWIO_DEHR_VMIDMT_SGFSYNDR2_ABID_BMSK                          0xe000
#define HWIO_DEHR_VMIDMT_SGFSYNDR2_ABID_SHFT                             0xd
#define HWIO_DEHR_VMIDMT_SGFSYNDR2_APID_BMSK                          0x1f00
#define HWIO_DEHR_VMIDMT_SGFSYNDR2_APID_SHFT                             0x8
#define HWIO_DEHR_VMIDMT_SGFSYNDR2_AMID_BMSK                            0xff
#define HWIO_DEHR_VMIDMT_SGFSYNDR2_AMID_SHFT                             0x0

#define HWIO_DEHR_VMIDMT_VMIDMTSCR0_ADDR                          (DEHR_VMIDMT_REG_BASE      + 0x00000090)
#define HWIO_DEHR_VMIDMT_VMIDMTSCR0_RMSK                                 0x1
#define HWIO_DEHR_VMIDMT_VMIDMTSCR0_IN          \
        in_dword(HWIO_DEHR_VMIDMT_VMIDMTSCR0_ADDR)
#define HWIO_DEHR_VMIDMT_VMIDMTSCR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_VMIDMTSCR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_VMIDMTSCR0_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_VMIDMTSCR0_ADDR,v)
#define HWIO_DEHR_VMIDMT_VMIDMTSCR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_VMIDMTSCR0_ADDR,m,v,HWIO_DEHR_VMIDMT_VMIDMTSCR0_IN)
#define HWIO_DEHR_VMIDMT_VMIDMTSCR0_CLKONOFFE_BMSK                       0x1
#define HWIO_DEHR_VMIDMT_VMIDMTSCR0_CLKONOFFE_SHFT                       0x0

#define HWIO_DEHR_VMIDMT_CR0_ADDR                                 (DEHR_VMIDMT_REG_BASE      + 0x00000000)
#define HWIO_DEHR_VMIDMT_CR0_RMSK                                  0xfd70ff5
#define HWIO_DEHR_VMIDMT_CR0_IN          \
        in_dword(HWIO_DEHR_VMIDMT_CR0_ADDR)
#define HWIO_DEHR_VMIDMT_CR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_CR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_CR0_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_CR0_ADDR,v)
#define HWIO_DEHR_VMIDMT_CR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_CR0_ADDR,m,v,HWIO_DEHR_VMIDMT_CR0_IN)
#define HWIO_DEHR_VMIDMT_CR0_WACFG_BMSK                            0xc000000
#define HWIO_DEHR_VMIDMT_CR0_WACFG_SHFT                                 0x1a
#define HWIO_DEHR_VMIDMT_CR0_RACFG_BMSK                            0x3000000
#define HWIO_DEHR_VMIDMT_CR0_RACFG_SHFT                                 0x18
#define HWIO_DEHR_VMIDMT_CR0_SHCFG_BMSK                             0xc00000
#define HWIO_DEHR_VMIDMT_CR0_SHCFG_SHFT                                 0x16
#define HWIO_DEHR_VMIDMT_CR0_MTCFG_BMSK                             0x100000
#define HWIO_DEHR_VMIDMT_CR0_MTCFG_SHFT                                 0x14
#define HWIO_DEHR_VMIDMT_CR0_MEMATTR_BMSK                            0x70000
#define HWIO_DEHR_VMIDMT_CR0_MEMATTR_SHFT                               0x10
#define HWIO_DEHR_VMIDMT_CR0_VMIDPNE_BMSK                              0x800
#define HWIO_DEHR_VMIDMT_CR0_VMIDPNE_SHFT                                0xb
#define HWIO_DEHR_VMIDMT_CR0_USFCFG_BMSK                               0x400
#define HWIO_DEHR_VMIDMT_CR0_USFCFG_SHFT                                 0xa
#define HWIO_DEHR_VMIDMT_CR0_GSE_BMSK                                  0x200
#define HWIO_DEHR_VMIDMT_CR0_GSE_SHFT                                    0x9
#define HWIO_DEHR_VMIDMT_CR0_STALLD_BMSK                               0x100
#define HWIO_DEHR_VMIDMT_CR0_STALLD_SHFT                                 0x8
#define HWIO_DEHR_VMIDMT_CR0_TRANSIENTCFG_BMSK                          0xc0
#define HWIO_DEHR_VMIDMT_CR0_TRANSIENTCFG_SHFT                           0x6
#define HWIO_DEHR_VMIDMT_CR0_GCFGFIE_BMSK                               0x20
#define HWIO_DEHR_VMIDMT_CR0_GCFGFIE_SHFT                                0x5
#define HWIO_DEHR_VMIDMT_CR0_GCFGERE_BMSK                               0x10
#define HWIO_DEHR_VMIDMT_CR0_GCFGERE_SHFT                                0x4
#define HWIO_DEHR_VMIDMT_CR0_GFIE_BMSK                                   0x4
#define HWIO_DEHR_VMIDMT_CR0_GFIE_SHFT                                   0x2
#define HWIO_DEHR_VMIDMT_CR0_CLIENTPD_BMSK                               0x1
#define HWIO_DEHR_VMIDMT_CR0_CLIENTPD_SHFT                               0x0

#define HWIO_DEHR_VMIDMT_CR2_ADDR                                 (DEHR_VMIDMT_REG_BASE      + 0x00000008)
#define HWIO_DEHR_VMIDMT_CR2_RMSK                                       0x1f
#define HWIO_DEHR_VMIDMT_CR2_IN          \
        in_dword(HWIO_DEHR_VMIDMT_CR2_ADDR)
#define HWIO_DEHR_VMIDMT_CR2_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_CR2_ADDR, m)
#define HWIO_DEHR_VMIDMT_CR2_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_CR2_ADDR,v)
#define HWIO_DEHR_VMIDMT_CR2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_CR2_ADDR,m,v,HWIO_DEHR_VMIDMT_CR2_IN)
#define HWIO_DEHR_VMIDMT_CR2_BPVMID_BMSK                                0x1f
#define HWIO_DEHR_VMIDMT_CR2_BPVMID_SHFT                                 0x0

#define HWIO_DEHR_VMIDMT_ACR_ADDR                                 (DEHR_VMIDMT_REG_BASE      + 0x00000010)
#define HWIO_DEHR_VMIDMT_ACR_RMSK                                 0x70000013
#define HWIO_DEHR_VMIDMT_ACR_IN          \
        in_dword(HWIO_DEHR_VMIDMT_ACR_ADDR)
#define HWIO_DEHR_VMIDMT_ACR_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_ACR_ADDR, m)
#define HWIO_DEHR_VMIDMT_ACR_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_ACR_ADDR,v)
#define HWIO_DEHR_VMIDMT_ACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_ACR_ADDR,m,v,HWIO_DEHR_VMIDMT_ACR_IN)
#define HWIO_DEHR_VMIDMT_ACR_BPRCNSH_BMSK                         0x40000000
#define HWIO_DEHR_VMIDMT_ACR_BPRCNSH_SHFT                               0x1e
#define HWIO_DEHR_VMIDMT_ACR_BPRCISH_BMSK                         0x20000000
#define HWIO_DEHR_VMIDMT_ACR_BPRCISH_SHFT                               0x1d
#define HWIO_DEHR_VMIDMT_ACR_BPRCOSH_BMSK                         0x10000000
#define HWIO_DEHR_VMIDMT_ACR_BPRCOSH_SHFT                               0x1c
#define HWIO_DEHR_VMIDMT_ACR_BPREQPRIORITYCFG_BMSK                      0x10
#define HWIO_DEHR_VMIDMT_ACR_BPREQPRIORITYCFG_SHFT                       0x4
#define HWIO_DEHR_VMIDMT_ACR_BPREQPRIORITY_BMSK                          0x3
#define HWIO_DEHR_VMIDMT_ACR_BPREQPRIORITY_SHFT                          0x0

#define HWIO_DEHR_VMIDMT_IDR0_ADDR                                (DEHR_VMIDMT_REG_BASE      + 0x00000020)
#define HWIO_DEHR_VMIDMT_IDR0_RMSK                                 0x8001eff
#define HWIO_DEHR_VMIDMT_IDR0_IN          \
        in_dword(HWIO_DEHR_VMIDMT_IDR0_ADDR)
#define HWIO_DEHR_VMIDMT_IDR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_IDR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_IDR0_SMS_BMSK                             0x8000000
#define HWIO_DEHR_VMIDMT_IDR0_SMS_SHFT                                  0x1b
#define HWIO_DEHR_VMIDMT_IDR0_NUMSIDB_BMSK                            0x1e00
#define HWIO_DEHR_VMIDMT_IDR0_NUMSIDB_SHFT                               0x9
#define HWIO_DEHR_VMIDMT_IDR0_NUMSMRG_BMSK                              0xff
#define HWIO_DEHR_VMIDMT_IDR0_NUMSMRG_SHFT                               0x0

#define HWIO_DEHR_VMIDMT_IDR1_ADDR                                (DEHR_VMIDMT_REG_BASE      + 0x00000024)
#define HWIO_DEHR_VMIDMT_IDR1_RMSK                                    0x9f00
#define HWIO_DEHR_VMIDMT_IDR1_IN          \
        in_dword(HWIO_DEHR_VMIDMT_IDR1_ADDR)
#define HWIO_DEHR_VMIDMT_IDR1_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_IDR1_ADDR, m)
#define HWIO_DEHR_VMIDMT_IDR1_SMCD_BMSK                               0x8000
#define HWIO_DEHR_VMIDMT_IDR1_SMCD_SHFT                                  0xf
#define HWIO_DEHR_VMIDMT_IDR1_SSDTP_BMSK                              0x1000
#define HWIO_DEHR_VMIDMT_IDR1_SSDTP_SHFT                                 0xc
#define HWIO_DEHR_VMIDMT_IDR1_NUMSSDNDX_BMSK                           0xf00
#define HWIO_DEHR_VMIDMT_IDR1_NUMSSDNDX_SHFT                             0x8

#define HWIO_DEHR_VMIDMT_IDR2_ADDR                                (DEHR_VMIDMT_REG_BASE      + 0x00000028)
#define HWIO_DEHR_VMIDMT_IDR2_RMSK                                      0xff
#define HWIO_DEHR_VMIDMT_IDR2_IN          \
        in_dword(HWIO_DEHR_VMIDMT_IDR2_ADDR)
#define HWIO_DEHR_VMIDMT_IDR2_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_IDR2_ADDR, m)
#define HWIO_DEHR_VMIDMT_IDR2_OAS_BMSK                                  0xf0
#define HWIO_DEHR_VMIDMT_IDR2_OAS_SHFT                                   0x4
#define HWIO_DEHR_VMIDMT_IDR2_IAS_BMSK                                   0xf
#define HWIO_DEHR_VMIDMT_IDR2_IAS_SHFT                                   0x0

#define HWIO_DEHR_VMIDMT_IDR4_ADDR                                (DEHR_VMIDMT_REG_BASE      + 0x00000030)
#define HWIO_DEHR_VMIDMT_IDR4_RMSK                                0xffffffff
#define HWIO_DEHR_VMIDMT_IDR4_IN          \
        in_dword(HWIO_DEHR_VMIDMT_IDR4_ADDR)
#define HWIO_DEHR_VMIDMT_IDR4_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_IDR4_ADDR, m)
#define HWIO_DEHR_VMIDMT_IDR4_MAJOR_BMSK                          0xf0000000
#define HWIO_DEHR_VMIDMT_IDR4_MAJOR_SHFT                                0x1c
#define HWIO_DEHR_VMIDMT_IDR4_MINOR_BMSK                           0xfff0000
#define HWIO_DEHR_VMIDMT_IDR4_MINOR_SHFT                                0x10
#define HWIO_DEHR_VMIDMT_IDR4_STEP_BMSK                               0xffff
#define HWIO_DEHR_VMIDMT_IDR4_STEP_SHFT                                  0x0

#define HWIO_DEHR_VMIDMT_IDR5_ADDR                                (DEHR_VMIDMT_REG_BASE      + 0x00000034)
#define HWIO_DEHR_VMIDMT_IDR5_RMSK                                  0xff03ff
#define HWIO_DEHR_VMIDMT_IDR5_IN          \
        in_dword(HWIO_DEHR_VMIDMT_IDR5_ADDR)
#define HWIO_DEHR_VMIDMT_IDR5_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_IDR5_ADDR, m)
#define HWIO_DEHR_VMIDMT_IDR5_NUMMSDRB_BMSK                         0xff0000
#define HWIO_DEHR_VMIDMT_IDR5_NUMMSDRB_SHFT                             0x10
#define HWIO_DEHR_VMIDMT_IDR5_MSAE_BMSK                                0x200
#define HWIO_DEHR_VMIDMT_IDR5_MSAE_SHFT                                  0x9
#define HWIO_DEHR_VMIDMT_IDR5_QRIBE_BMSK                               0x100
#define HWIO_DEHR_VMIDMT_IDR5_QRIBE_SHFT                                 0x8
#define HWIO_DEHR_VMIDMT_IDR5_NVMID_BMSK                                0xff
#define HWIO_DEHR_VMIDMT_IDR5_NVMID_SHFT                                 0x0

#define HWIO_DEHR_VMIDMT_IDR7_ADDR                                (DEHR_VMIDMT_REG_BASE      + 0x0000003c)
#define HWIO_DEHR_VMIDMT_IDR7_RMSK                                      0xff
#define HWIO_DEHR_VMIDMT_IDR7_IN          \
        in_dword(HWIO_DEHR_VMIDMT_IDR7_ADDR)
#define HWIO_DEHR_VMIDMT_IDR7_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_IDR7_ADDR, m)
#define HWIO_DEHR_VMIDMT_IDR7_MAJOR_BMSK                                0xf0
#define HWIO_DEHR_VMIDMT_IDR7_MAJOR_SHFT                                 0x4
#define HWIO_DEHR_VMIDMT_IDR7_MINOR_BMSK                                 0xf
#define HWIO_DEHR_VMIDMT_IDR7_MINOR_SHFT                                 0x0

#define HWIO_DEHR_VMIDMT_GFAR0_ADDR                               (DEHR_VMIDMT_REG_BASE      + 0x00000040)
#define HWIO_DEHR_VMIDMT_GFAR0_RMSK                               0xffffffff
#define HWIO_DEHR_VMIDMT_GFAR0_IN          \
        in_dword(HWIO_DEHR_VMIDMT_GFAR0_ADDR)
#define HWIO_DEHR_VMIDMT_GFAR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_GFAR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_GFAR0_GFEA0_BMSK                         0xffffffff
#define HWIO_DEHR_VMIDMT_GFAR0_GFEA0_SHFT                                0x0

#define HWIO_DEHR_VMIDMT_GFSR_ADDR                                (DEHR_VMIDMT_REG_BASE      + 0x00000048)
#define HWIO_DEHR_VMIDMT_GFSR_RMSK                                0xc00000a2
#define HWIO_DEHR_VMIDMT_GFSR_IN          \
        in_dword(HWIO_DEHR_VMIDMT_GFSR_ADDR)
#define HWIO_DEHR_VMIDMT_GFSR_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_GFSR_ADDR, m)
#define HWIO_DEHR_VMIDMT_GFSR_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_GFSR_ADDR,v)
#define HWIO_DEHR_VMIDMT_GFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_GFSR_ADDR,m,v,HWIO_DEHR_VMIDMT_GFSR_IN)
#define HWIO_DEHR_VMIDMT_GFSR_MULTI_CLIENT_BMSK                   0x80000000
#define HWIO_DEHR_VMIDMT_GFSR_MULTI_CLIENT_SHFT                         0x1f
#define HWIO_DEHR_VMIDMT_GFSR_MULTI_CFG_BMSK                      0x40000000
#define HWIO_DEHR_VMIDMT_GFSR_MULTI_CFG_SHFT                            0x1e
#define HWIO_DEHR_VMIDMT_GFSR_PF_BMSK                                   0x80
#define HWIO_DEHR_VMIDMT_GFSR_PF_SHFT                                    0x7
#define HWIO_DEHR_VMIDMT_GFSR_CAF_BMSK                                  0x20
#define HWIO_DEHR_VMIDMT_GFSR_CAF_SHFT                                   0x5
#define HWIO_DEHR_VMIDMT_GFSR_USF_BMSK                                   0x2
#define HWIO_DEHR_VMIDMT_GFSR_USF_SHFT                                   0x1

#define HWIO_DEHR_VMIDMT_GFSRRESTORE_ADDR                         (DEHR_VMIDMT_REG_BASE      + 0x0000004c)
#define HWIO_DEHR_VMIDMT_GFSRRESTORE_RMSK                         0xc00000a2
#define HWIO_DEHR_VMIDMT_GFSRRESTORE_IN          \
        in_dword(HWIO_DEHR_VMIDMT_GFSRRESTORE_ADDR)
#define HWIO_DEHR_VMIDMT_GFSRRESTORE_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_GFSRRESTORE_ADDR, m)
#define HWIO_DEHR_VMIDMT_GFSRRESTORE_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_GFSRRESTORE_ADDR,v)
#define HWIO_DEHR_VMIDMT_GFSRRESTORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_GFSRRESTORE_ADDR,m,v,HWIO_DEHR_VMIDMT_GFSRRESTORE_IN)
#define HWIO_DEHR_VMIDMT_GFSRRESTORE_MULTI_CLIENT_BMSK            0x80000000
#define HWIO_DEHR_VMIDMT_GFSRRESTORE_MULTI_CLIENT_SHFT                  0x1f
#define HWIO_DEHR_VMIDMT_GFSRRESTORE_MULTI_CFG_BMSK               0x40000000
#define HWIO_DEHR_VMIDMT_GFSRRESTORE_MULTI_CFG_SHFT                     0x1e
#define HWIO_DEHR_VMIDMT_GFSRRESTORE_PF_BMSK                            0x80
#define HWIO_DEHR_VMIDMT_GFSRRESTORE_PF_SHFT                             0x7
#define HWIO_DEHR_VMIDMT_GFSRRESTORE_CAF_BMSK                           0x20
#define HWIO_DEHR_VMIDMT_GFSRRESTORE_CAF_SHFT                            0x5
#define HWIO_DEHR_VMIDMT_GFSRRESTORE_USF_BMSK                            0x2
#define HWIO_DEHR_VMIDMT_GFSRRESTORE_USF_SHFT                            0x1

#define HWIO_DEHR_VMIDMT_GFSYNDR0_ADDR                            (DEHR_VMIDMT_REG_BASE      + 0x00000050)
#define HWIO_DEHR_VMIDMT_GFSYNDR0_RMSK                                 0x132
#define HWIO_DEHR_VMIDMT_GFSYNDR0_IN          \
        in_dword(HWIO_DEHR_VMIDMT_GFSYNDR0_ADDR)
#define HWIO_DEHR_VMIDMT_GFSYNDR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_GFSYNDR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_GFSYNDR0_MSSSELFAUTH_BMSK                     0x100
#define HWIO_DEHR_VMIDMT_GFSYNDR0_MSSSELFAUTH_SHFT                       0x8
#define HWIO_DEHR_VMIDMT_GFSYNDR0_NSATTR_BMSK                           0x20
#define HWIO_DEHR_VMIDMT_GFSYNDR0_NSATTR_SHFT                            0x5
#define HWIO_DEHR_VMIDMT_GFSYNDR0_NSSTATE_BMSK                          0x10
#define HWIO_DEHR_VMIDMT_GFSYNDR0_NSSTATE_SHFT                           0x4
#define HWIO_DEHR_VMIDMT_GFSYNDR0_WNR_BMSK                               0x2
#define HWIO_DEHR_VMIDMT_GFSYNDR0_WNR_SHFT                               0x1

#define HWIO_DEHR_VMIDMT_GFSYNDR1_ADDR                            (DEHR_VMIDMT_REG_BASE      + 0x00000054)
#define HWIO_DEHR_VMIDMT_GFSYNDR1_RMSK                             0x1010001
#define HWIO_DEHR_VMIDMT_GFSYNDR1_IN          \
        in_dword(HWIO_DEHR_VMIDMT_GFSYNDR1_ADDR)
#define HWIO_DEHR_VMIDMT_GFSYNDR1_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_GFSYNDR1_ADDR, m)
#define HWIO_DEHR_VMIDMT_GFSYNDR1_MSDINDEX_BMSK                    0x1000000
#define HWIO_DEHR_VMIDMT_GFSYNDR1_MSDINDEX_SHFT                         0x18
#define HWIO_DEHR_VMIDMT_GFSYNDR1_SSDINDEX_BMSK                      0x10000
#define HWIO_DEHR_VMIDMT_GFSYNDR1_SSDINDEX_SHFT                         0x10
#define HWIO_DEHR_VMIDMT_GFSYNDR1_STREAMINDEX_BMSK                       0x1
#define HWIO_DEHR_VMIDMT_GFSYNDR1_STREAMINDEX_SHFT                       0x0

#define HWIO_DEHR_VMIDMT_GFSYNDR2_ADDR                            (DEHR_VMIDMT_REG_BASE      + 0x00000058)
#define HWIO_DEHR_VMIDMT_GFSYNDR2_RMSK                            0x3f1fffff
#define HWIO_DEHR_VMIDMT_GFSYNDR2_IN          \
        in_dword(HWIO_DEHR_VMIDMT_GFSYNDR2_ADDR)
#define HWIO_DEHR_VMIDMT_GFSYNDR2_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_GFSYNDR2_ADDR, m)
#define HWIO_DEHR_VMIDMT_GFSYNDR2_ATID_BMSK                       0x3f000000
#define HWIO_DEHR_VMIDMT_GFSYNDR2_ATID_SHFT                             0x18
#define HWIO_DEHR_VMIDMT_GFSYNDR2_AVMID_BMSK                        0x1f0000
#define HWIO_DEHR_VMIDMT_GFSYNDR2_AVMID_SHFT                            0x10
#define HWIO_DEHR_VMIDMT_GFSYNDR2_ABID_BMSK                           0xe000
#define HWIO_DEHR_VMIDMT_GFSYNDR2_ABID_SHFT                              0xd
#define HWIO_DEHR_VMIDMT_GFSYNDR2_APID_BMSK                           0x1f00
#define HWIO_DEHR_VMIDMT_GFSYNDR2_APID_SHFT                              0x8
#define HWIO_DEHR_VMIDMT_GFSYNDR2_AMID_BMSK                             0xff
#define HWIO_DEHR_VMIDMT_GFSYNDR2_AMID_SHFT                              0x0

#define HWIO_DEHR_VMIDMT_VMIDMTCR0_ADDR                           (DEHR_VMIDMT_REG_BASE      + 0x00000090)
#define HWIO_DEHR_VMIDMT_VMIDMTCR0_RMSK                                  0x1
#define HWIO_DEHR_VMIDMT_VMIDMTCR0_IN          \
        in_dword(HWIO_DEHR_VMIDMT_VMIDMTCR0_ADDR)
#define HWIO_DEHR_VMIDMT_VMIDMTCR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_VMIDMTCR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_VMIDMTCR0_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_VMIDMTCR0_ADDR,v)
#define HWIO_DEHR_VMIDMT_VMIDMTCR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_VMIDMTCR0_ADDR,m,v,HWIO_DEHR_VMIDMT_VMIDMTCR0_IN)
#define HWIO_DEHR_VMIDMT_VMIDMTCR0_CLKONOFFE_BMSK                        0x1
#define HWIO_DEHR_VMIDMT_VMIDMTCR0_CLKONOFFE_SHFT                        0x0

#define HWIO_DEHR_VMIDMT_VMIDMTACR_ADDR                           (DEHR_VMIDMT_REG_BASE      + 0x0000009c)
#define HWIO_DEHR_VMIDMT_VMIDMTACR_RMSK                           0xffffffff
#define HWIO_DEHR_VMIDMT_VMIDMTACR_IN          \
        in_dword(HWIO_DEHR_VMIDMT_VMIDMTACR_ADDR)
#define HWIO_DEHR_VMIDMT_VMIDMTACR_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_VMIDMTACR_ADDR, m)
#define HWIO_DEHR_VMIDMT_VMIDMTACR_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_VMIDMTACR_ADDR,v)
#define HWIO_DEHR_VMIDMT_VMIDMTACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_VMIDMTACR_ADDR,m,v,HWIO_DEHR_VMIDMT_VMIDMTACR_IN)
#define HWIO_DEHR_VMIDMT_VMIDMTACR_RWE_BMSK                       0xffffffff
#define HWIO_DEHR_VMIDMT_VMIDMTACR_RWE_SHFT                              0x0

#define HWIO_DEHR_VMIDMT_NSCR0_ADDR                               (DEHR_VMIDMT_REG_BASE      + 0x00000400)
#define HWIO_DEHR_VMIDMT_NSCR0_RMSK                                0xfd70ff5
#define HWIO_DEHR_VMIDMT_NSCR0_IN          \
        in_dword(HWIO_DEHR_VMIDMT_NSCR0_ADDR)
#define HWIO_DEHR_VMIDMT_NSCR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_NSCR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_NSCR0_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_NSCR0_ADDR,v)
#define HWIO_DEHR_VMIDMT_NSCR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_NSCR0_ADDR,m,v,HWIO_DEHR_VMIDMT_NSCR0_IN)
#define HWIO_DEHR_VMIDMT_NSCR0_WACFG_BMSK                          0xc000000
#define HWIO_DEHR_VMIDMT_NSCR0_WACFG_SHFT                               0x1a
#define HWIO_DEHR_VMIDMT_NSCR0_RACFG_BMSK                          0x3000000
#define HWIO_DEHR_VMIDMT_NSCR0_RACFG_SHFT                               0x18
#define HWIO_DEHR_VMIDMT_NSCR0_SHCFG_BMSK                           0xc00000
#define HWIO_DEHR_VMIDMT_NSCR0_SHCFG_SHFT                               0x16
#define HWIO_DEHR_VMIDMT_NSCR0_MTCFG_BMSK                           0x100000
#define HWIO_DEHR_VMIDMT_NSCR0_MTCFG_SHFT                               0x14
#define HWIO_DEHR_VMIDMT_NSCR0_MEMATTR_BMSK                          0x70000
#define HWIO_DEHR_VMIDMT_NSCR0_MEMATTR_SHFT                             0x10
#define HWIO_DEHR_VMIDMT_NSCR0_VMIDPNE_BMSK                            0x800
#define HWIO_DEHR_VMIDMT_NSCR0_VMIDPNE_SHFT                              0xb
#define HWIO_DEHR_VMIDMT_NSCR0_USFCFG_BMSK                             0x400
#define HWIO_DEHR_VMIDMT_NSCR0_USFCFG_SHFT                               0xa
#define HWIO_DEHR_VMIDMT_NSCR0_GSE_BMSK                                0x200
#define HWIO_DEHR_VMIDMT_NSCR0_GSE_SHFT                                  0x9
#define HWIO_DEHR_VMIDMT_NSCR0_STALLD_BMSK                             0x100
#define HWIO_DEHR_VMIDMT_NSCR0_STALLD_SHFT                               0x8
#define HWIO_DEHR_VMIDMT_NSCR0_TRANSIENTCFG_BMSK                        0xc0
#define HWIO_DEHR_VMIDMT_NSCR0_TRANSIENTCFG_SHFT                         0x6
#define HWIO_DEHR_VMIDMT_NSCR0_GCFGFIE_BMSK                             0x20
#define HWIO_DEHR_VMIDMT_NSCR0_GCFGFIE_SHFT                              0x5
#define HWIO_DEHR_VMIDMT_NSCR0_GCFGERE_BMSK                             0x10
#define HWIO_DEHR_VMIDMT_NSCR0_GCFGERE_SHFT                              0x4
#define HWIO_DEHR_VMIDMT_NSCR0_GFIE_BMSK                                 0x4
#define HWIO_DEHR_VMIDMT_NSCR0_GFIE_SHFT                                 0x2
#define HWIO_DEHR_VMIDMT_NSCR0_CLIENTPD_BMSK                             0x1
#define HWIO_DEHR_VMIDMT_NSCR0_CLIENTPD_SHFT                             0x0

#define HWIO_DEHR_VMIDMT_NSCR2_ADDR                               (DEHR_VMIDMT_REG_BASE      + 0x00000408)
#define HWIO_DEHR_VMIDMT_NSCR2_RMSK                                     0x1f
#define HWIO_DEHR_VMIDMT_NSCR2_IN          \
        in_dword(HWIO_DEHR_VMIDMT_NSCR2_ADDR)
#define HWIO_DEHR_VMIDMT_NSCR2_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_NSCR2_ADDR, m)
#define HWIO_DEHR_VMIDMT_NSCR2_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_NSCR2_ADDR,v)
#define HWIO_DEHR_VMIDMT_NSCR2_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_NSCR2_ADDR,m,v,HWIO_DEHR_VMIDMT_NSCR2_IN)
#define HWIO_DEHR_VMIDMT_NSCR2_BPVMID_BMSK                              0x1f
#define HWIO_DEHR_VMIDMT_NSCR2_BPVMID_SHFT                               0x0

#define HWIO_DEHR_VMIDMT_NSACR_ADDR                               (DEHR_VMIDMT_REG_BASE      + 0x00000410)
#define HWIO_DEHR_VMIDMT_NSACR_RMSK                               0x70000013
#define HWIO_DEHR_VMIDMT_NSACR_IN          \
        in_dword(HWIO_DEHR_VMIDMT_NSACR_ADDR)
#define HWIO_DEHR_VMIDMT_NSACR_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_NSACR_ADDR, m)
#define HWIO_DEHR_VMIDMT_NSACR_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_NSACR_ADDR,v)
#define HWIO_DEHR_VMIDMT_NSACR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_NSACR_ADDR,m,v,HWIO_DEHR_VMIDMT_NSACR_IN)
#define HWIO_DEHR_VMIDMT_NSACR_BPRCNSH_BMSK                       0x40000000
#define HWIO_DEHR_VMIDMT_NSACR_BPRCNSH_SHFT                             0x1e
#define HWIO_DEHR_VMIDMT_NSACR_BPRCISH_BMSK                       0x20000000
#define HWIO_DEHR_VMIDMT_NSACR_BPRCISH_SHFT                             0x1d
#define HWIO_DEHR_VMIDMT_NSACR_BPRCOSH_BMSK                       0x10000000
#define HWIO_DEHR_VMIDMT_NSACR_BPRCOSH_SHFT                             0x1c
#define HWIO_DEHR_VMIDMT_NSACR_BPREQPRIORITYCFG_BMSK                    0x10
#define HWIO_DEHR_VMIDMT_NSACR_BPREQPRIORITYCFG_SHFT                     0x4
#define HWIO_DEHR_VMIDMT_NSACR_BPREQPRIORITY_BMSK                        0x3
#define HWIO_DEHR_VMIDMT_NSACR_BPREQPRIORITY_SHFT                        0x0

#define HWIO_DEHR_VMIDMT_NSGFAR0_ADDR                             (DEHR_VMIDMT_REG_BASE      + 0x00000440)
#define HWIO_DEHR_VMIDMT_NSGFAR0_RMSK                             0xffffffff
#define HWIO_DEHR_VMIDMT_NSGFAR0_IN          \
        in_dword(HWIO_DEHR_VMIDMT_NSGFAR0_ADDR)
#define HWIO_DEHR_VMIDMT_NSGFAR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_NSGFAR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_NSGFAR0_GFEA0_BMSK                       0xffffffff
#define HWIO_DEHR_VMIDMT_NSGFAR0_GFEA0_SHFT                              0x0

#define HWIO_DEHR_VMIDMT_NSGFSR_ADDR                              (DEHR_VMIDMT_REG_BASE      + 0x00000448)
#define HWIO_DEHR_VMIDMT_NSGFSR_RMSK                              0xc00000a2
#define HWIO_DEHR_VMIDMT_NSGFSR_IN          \
        in_dword(HWIO_DEHR_VMIDMT_NSGFSR_ADDR)
#define HWIO_DEHR_VMIDMT_NSGFSR_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_NSGFSR_ADDR, m)
#define HWIO_DEHR_VMIDMT_NSGFSR_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_NSGFSR_ADDR,v)
#define HWIO_DEHR_VMIDMT_NSGFSR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_NSGFSR_ADDR,m,v,HWIO_DEHR_VMIDMT_NSGFSR_IN)
#define HWIO_DEHR_VMIDMT_NSGFSR_MULTI_CLIENT_BMSK                 0x80000000
#define HWIO_DEHR_VMIDMT_NSGFSR_MULTI_CLIENT_SHFT                       0x1f
#define HWIO_DEHR_VMIDMT_NSGFSR_MULTI_CFG_BMSK                    0x40000000
#define HWIO_DEHR_VMIDMT_NSGFSR_MULTI_CFG_SHFT                          0x1e
#define HWIO_DEHR_VMIDMT_NSGFSR_PF_BMSK                                 0x80
#define HWIO_DEHR_VMIDMT_NSGFSR_PF_SHFT                                  0x7
#define HWIO_DEHR_VMIDMT_NSGFSR_CAF_BMSK                                0x20
#define HWIO_DEHR_VMIDMT_NSGFSR_CAF_SHFT                                 0x5
#define HWIO_DEHR_VMIDMT_NSGFSR_USF_BMSK                                 0x2
#define HWIO_DEHR_VMIDMT_NSGFSR_USF_SHFT                                 0x1

#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_ADDR                       (DEHR_VMIDMT_REG_BASE      + 0x0000044c)
#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_RMSK                       0xc00000a2
#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_IN          \
        in_dword(HWIO_DEHR_VMIDMT_NSGFSRRESTORE_ADDR)
#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_NSGFSRRESTORE_ADDR, m)
#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_NSGFSRRESTORE_ADDR,v)
#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_NSGFSRRESTORE_ADDR,m,v,HWIO_DEHR_VMIDMT_NSGFSRRESTORE_IN)
#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_MULTI_CLIENT_BMSK          0x80000000
#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_MULTI_CLIENT_SHFT                0x1f
#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_MULTI_CFG_BMSK             0x40000000
#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_MULTI_CFG_SHFT                   0x1e
#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_PF_BMSK                          0x80
#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_PF_SHFT                           0x7
#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_CAF_BMSK                         0x20
#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_CAF_SHFT                          0x5
#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_USF_BMSK                          0x2
#define HWIO_DEHR_VMIDMT_NSGFSRRESTORE_USF_SHFT                          0x1

#define HWIO_DEHR_VMIDMT_NSGFSYNDR0_ADDR                          (DEHR_VMIDMT_REG_BASE      + 0x00000450)
#define HWIO_DEHR_VMIDMT_NSGFSYNDR0_RMSK                               0x132
#define HWIO_DEHR_VMIDMT_NSGFSYNDR0_IN          \
        in_dword(HWIO_DEHR_VMIDMT_NSGFSYNDR0_ADDR)
#define HWIO_DEHR_VMIDMT_NSGFSYNDR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_NSGFSYNDR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_NSGFSYNDR0_MSSSELFAUTH_BMSK                   0x100
#define HWIO_DEHR_VMIDMT_NSGFSYNDR0_MSSSELFAUTH_SHFT                     0x8
#define HWIO_DEHR_VMIDMT_NSGFSYNDR0_NSATTR_BMSK                         0x20
#define HWIO_DEHR_VMIDMT_NSGFSYNDR0_NSATTR_SHFT                          0x5
#define HWIO_DEHR_VMIDMT_NSGFSYNDR0_NSSTATE_BMSK                        0x10
#define HWIO_DEHR_VMIDMT_NSGFSYNDR0_NSSTATE_SHFT                         0x4
#define HWIO_DEHR_VMIDMT_NSGFSYNDR0_WNR_BMSK                             0x2
#define HWIO_DEHR_VMIDMT_NSGFSYNDR0_WNR_SHFT                             0x1

#define HWIO_DEHR_VMIDMT_NSGFSYNDR1_ADDR                          (DEHR_VMIDMT_REG_BASE      + 0x00000454)
#define HWIO_DEHR_VMIDMT_NSGFSYNDR1_RMSK                           0x1010001
#define HWIO_DEHR_VMIDMT_NSGFSYNDR1_IN          \
        in_dword(HWIO_DEHR_VMIDMT_NSGFSYNDR1_ADDR)
#define HWIO_DEHR_VMIDMT_NSGFSYNDR1_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_NSGFSYNDR1_ADDR, m)
#define HWIO_DEHR_VMIDMT_NSGFSYNDR1_MSDINDEX_BMSK                  0x1000000
#define HWIO_DEHR_VMIDMT_NSGFSYNDR1_MSDINDEX_SHFT                       0x18
#define HWIO_DEHR_VMIDMT_NSGFSYNDR1_SSDINDEX_BMSK                    0x10000
#define HWIO_DEHR_VMIDMT_NSGFSYNDR1_SSDINDEX_SHFT                       0x10
#define HWIO_DEHR_VMIDMT_NSGFSYNDR1_STREAMINDEX_BMSK                     0x1
#define HWIO_DEHR_VMIDMT_NSGFSYNDR1_STREAMINDEX_SHFT                     0x0

#define HWIO_DEHR_VMIDMT_NSGFSYNDR2_ADDR                          (DEHR_VMIDMT_REG_BASE      + 0x00000458)
#define HWIO_DEHR_VMIDMT_NSGFSYNDR2_RMSK                          0x3f1fffff
#define HWIO_DEHR_VMIDMT_NSGFSYNDR2_IN          \
        in_dword(HWIO_DEHR_VMIDMT_NSGFSYNDR2_ADDR)
#define HWIO_DEHR_VMIDMT_NSGFSYNDR2_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_NSGFSYNDR2_ADDR, m)
#define HWIO_DEHR_VMIDMT_NSGFSYNDR2_ATID_BMSK                     0x3f000000
#define HWIO_DEHR_VMIDMT_NSGFSYNDR2_ATID_SHFT                           0x18
#define HWIO_DEHR_VMIDMT_NSGFSYNDR2_AVMID_BMSK                      0x1f0000
#define HWIO_DEHR_VMIDMT_NSGFSYNDR2_AVMID_SHFT                          0x10
#define HWIO_DEHR_VMIDMT_NSGFSYNDR2_ABID_BMSK                         0xe000
#define HWIO_DEHR_VMIDMT_NSGFSYNDR2_ABID_SHFT                            0xd
#define HWIO_DEHR_VMIDMT_NSGFSYNDR2_APID_BMSK                         0x1f00
#define HWIO_DEHR_VMIDMT_NSGFSYNDR2_APID_SHFT                            0x8
#define HWIO_DEHR_VMIDMT_NSGFSYNDR2_AMID_BMSK                           0xff
#define HWIO_DEHR_VMIDMT_NSGFSYNDR2_AMID_SHFT                            0x0

#define HWIO_DEHR_VMIDMT_NSVMIDMTCR0_ADDR                         (DEHR_VMIDMT_REG_BASE      + 0x00000490)
#define HWIO_DEHR_VMIDMT_NSVMIDMTCR0_RMSK                                0x1
#define HWIO_DEHR_VMIDMT_NSVMIDMTCR0_IN          \
        in_dword(HWIO_DEHR_VMIDMT_NSVMIDMTCR0_ADDR)
#define HWIO_DEHR_VMIDMT_NSVMIDMTCR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_NSVMIDMTCR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_NSVMIDMTCR0_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_NSVMIDMTCR0_ADDR,v)
#define HWIO_DEHR_VMIDMT_NSVMIDMTCR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_NSVMIDMTCR0_ADDR,m,v,HWIO_DEHR_VMIDMT_NSVMIDMTCR0_IN)
#define HWIO_DEHR_VMIDMT_NSVMIDMTCR0_CLKONOFFE_BMSK                      0x1
#define HWIO_DEHR_VMIDMT_NSVMIDMTCR0_CLKONOFFE_SHFT                      0x0

#define HWIO_DEHR_VMIDMT_SSDR0_ADDR                               (DEHR_VMIDMT_REG_BASE      + 0x00000080)
#define HWIO_DEHR_VMIDMT_SSDR0_RMSK                                      0x1
#define HWIO_DEHR_VMIDMT_SSDR0_IN          \
        in_dword(HWIO_DEHR_VMIDMT_SSDR0_ADDR)
#define HWIO_DEHR_VMIDMT_SSDR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_SSDR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_SSDR0_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_SSDR0_ADDR,v)
#define HWIO_DEHR_VMIDMT_SSDR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_SSDR0_ADDR,m,v,HWIO_DEHR_VMIDMT_SSDR0_IN)
#define HWIO_DEHR_VMIDMT_SSDR0_RWE_BMSK                                  0x1
#define HWIO_DEHR_VMIDMT_SSDR0_RWE_SHFT                                  0x0

#define HWIO_DEHR_VMIDMT_MSDR0_ADDR                               (DEHR_VMIDMT_REG_BASE      + 0x00000480)
#define HWIO_DEHR_VMIDMT_MSDR0_RMSK                                      0x1
#define HWIO_DEHR_VMIDMT_MSDR0_IN          \
        in_dword(HWIO_DEHR_VMIDMT_MSDR0_ADDR)
#define HWIO_DEHR_VMIDMT_MSDR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_MSDR0_ADDR, m)
#define HWIO_DEHR_VMIDMT_MSDR0_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_MSDR0_ADDR,v)
#define HWIO_DEHR_VMIDMT_MSDR0_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_MSDR0_ADDR,m,v,HWIO_DEHR_VMIDMT_MSDR0_IN)
#define HWIO_DEHR_VMIDMT_MSDR0_RWE_BMSK                                  0x1
#define HWIO_DEHR_VMIDMT_MSDR0_RWE_SHFT                                  0x0

#define HWIO_DEHR_VMIDMT_MCR_ADDR                                 (DEHR_VMIDMT_REG_BASE      + 0x00000494)
#define HWIO_DEHR_VMIDMT_MCR_RMSK                                        0x7
#define HWIO_DEHR_VMIDMT_MCR_IN          \
        in_dword(HWIO_DEHR_VMIDMT_MCR_ADDR)
#define HWIO_DEHR_VMIDMT_MCR_INM(m)      \
        in_dword_masked(HWIO_DEHR_VMIDMT_MCR_ADDR, m)
#define HWIO_DEHR_VMIDMT_MCR_OUT(v)      \
        out_dword(HWIO_DEHR_VMIDMT_MCR_ADDR,v)
#define HWIO_DEHR_VMIDMT_MCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_MCR_ADDR,m,v,HWIO_DEHR_VMIDMT_MCR_IN)
#define HWIO_DEHR_VMIDMT_MCR_CLKONOFFE_BMSK                              0x4
#define HWIO_DEHR_VMIDMT_MCR_CLKONOFFE_SHFT                              0x2
#define HWIO_DEHR_VMIDMT_MCR_BPMSACFG_BMSK                               0x2
#define HWIO_DEHR_VMIDMT_MCR_BPMSACFG_SHFT                               0x1
#define HWIO_DEHR_VMIDMT_MCR_BPSMSACFG_BMSK                              0x1
#define HWIO_DEHR_VMIDMT_MCR_BPSMSACFG_SHFT                              0x0

#define HWIO_DEHR_VMIDMT_S2VRn_ADDR(n)                            (DEHR_VMIDMT_REG_BASE      + 0x00000c00 + 0x4 * (n))
#define HWIO_DEHR_VMIDMT_S2VRn_RMSK                               0x30ff7b1f
#define HWIO_DEHR_VMIDMT_S2VRn_MAXn                                        0
#define HWIO_DEHR_VMIDMT_S2VRn_INI(n)        \
        in_dword_masked(HWIO_DEHR_VMIDMT_S2VRn_ADDR(n), HWIO_DEHR_VMIDMT_S2VRn_RMSK)
#define HWIO_DEHR_VMIDMT_S2VRn_INMI(n,mask)    \
        in_dword_masked(HWIO_DEHR_VMIDMT_S2VRn_ADDR(n), mask)
#define HWIO_DEHR_VMIDMT_S2VRn_OUTI(n,val)    \
        out_dword(HWIO_DEHR_VMIDMT_S2VRn_ADDR(n),val)
#define HWIO_DEHR_VMIDMT_S2VRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_S2VRn_ADDR(n),mask,val,HWIO_DEHR_VMIDMT_S2VRn_INI(n))
#define HWIO_DEHR_VMIDMT_S2VRn_TRANSIENTCFG_BMSK                  0x30000000
#define HWIO_DEHR_VMIDMT_S2VRn_TRANSIENTCFG_SHFT                        0x1c
#define HWIO_DEHR_VMIDMT_S2VRn_WACFG_BMSK                           0xc00000
#define HWIO_DEHR_VMIDMT_S2VRn_WACFG_SHFT                               0x16
#define HWIO_DEHR_VMIDMT_S2VRn_RACFG_BMSK                           0x300000
#define HWIO_DEHR_VMIDMT_S2VRn_RACFG_SHFT                               0x14
#define HWIO_DEHR_VMIDMT_S2VRn_NSCFG_BMSK                            0xc0000
#define HWIO_DEHR_VMIDMT_S2VRn_NSCFG_SHFT                               0x12
#define HWIO_DEHR_VMIDMT_S2VRn_TYPE_BMSK                             0x30000
#define HWIO_DEHR_VMIDMT_S2VRn_TYPE_SHFT                                0x10
#define HWIO_DEHR_VMIDMT_S2VRn_MEMATTR_BMSK                           0x7000
#define HWIO_DEHR_VMIDMT_S2VRn_MEMATTR_SHFT                              0xc
#define HWIO_DEHR_VMIDMT_S2VRn_MTCFG_BMSK                              0x800
#define HWIO_DEHR_VMIDMT_S2VRn_MTCFG_SHFT                                0xb
#define HWIO_DEHR_VMIDMT_S2VRn_SHCFG_BMSK                              0x300
#define HWIO_DEHR_VMIDMT_S2VRn_SHCFG_SHFT                                0x8
#define HWIO_DEHR_VMIDMT_S2VRn_VMID_BMSK                                0x1f
#define HWIO_DEHR_VMIDMT_S2VRn_VMID_SHFT                                 0x0

#define HWIO_DEHR_VMIDMT_AS2VRn_ADDR(n)                           (DEHR_VMIDMT_REG_BASE      + 0x00000e00 + 0x4 * (n))
#define HWIO_DEHR_VMIDMT_AS2VRn_RMSK                              0x70000013
#define HWIO_DEHR_VMIDMT_AS2VRn_MAXn                                       0
#define HWIO_DEHR_VMIDMT_AS2VRn_INI(n)        \
        in_dword_masked(HWIO_DEHR_VMIDMT_AS2VRn_ADDR(n), HWIO_DEHR_VMIDMT_AS2VRn_RMSK)
#define HWIO_DEHR_VMIDMT_AS2VRn_INMI(n,mask)    \
        in_dword_masked(HWIO_DEHR_VMIDMT_AS2VRn_ADDR(n), mask)
#define HWIO_DEHR_VMIDMT_AS2VRn_OUTI(n,val)    \
        out_dword(HWIO_DEHR_VMIDMT_AS2VRn_ADDR(n),val)
#define HWIO_DEHR_VMIDMT_AS2VRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_DEHR_VMIDMT_AS2VRn_ADDR(n),mask,val,HWIO_DEHR_VMIDMT_AS2VRn_INI(n))
#define HWIO_DEHR_VMIDMT_AS2VRn_RCNSH_BMSK                        0x40000000
#define HWIO_DEHR_VMIDMT_AS2VRn_RCNSH_SHFT                              0x1e
#define HWIO_DEHR_VMIDMT_AS2VRn_RCISH_BMSK                        0x20000000
#define HWIO_DEHR_VMIDMT_AS2VRn_RCISH_SHFT                              0x1d
#define HWIO_DEHR_VMIDMT_AS2VRn_RCOSH_BMSK                        0x10000000
#define HWIO_DEHR_VMIDMT_AS2VRn_RCOSH_SHFT                              0x1c
#define HWIO_DEHR_VMIDMT_AS2VRn_REQPRIORITYCFG_BMSK                     0x10
#define HWIO_DEHR_VMIDMT_AS2VRn_REQPRIORITYCFG_SHFT                      0x4
#define HWIO_DEHR_VMIDMT_AS2VRn_REQPRIORITY_BMSK                         0x3
#define HWIO_DEHR_VMIDMT_AS2VRn_REQPRIORITY_SHFT                         0x0

/*----------------------------------------------------------------------------
 * MODULE: DEHR_XPU
 *--------------------------------------------------------------------------*/

#define DEHR_XPU_REG_BASE                                         (DEHR_WRAPPER_8K_R01_BASE      + 0x00000000)
#define DEHR_XPU_REG_BASE_SIZE                                    0x300
#define DEHR_XPU_REG_BASE_USED                                    0x2d4

#define HWIO_DEHR_XPU_SCR_ADDR                                    (DEHR_XPU_REG_BASE      + 0x00000000)
#define HWIO_DEHR_XPU_SCR_RMSK                                         0x77f
#define HWIO_DEHR_XPU_SCR_IN          \
        in_dword(HWIO_DEHR_XPU_SCR_ADDR)
#define HWIO_DEHR_XPU_SCR_INM(m)      \
        in_dword_masked(HWIO_DEHR_XPU_SCR_ADDR, m)
#define HWIO_DEHR_XPU_SCR_OUT(v)      \
        out_dword(HWIO_DEHR_XPU_SCR_ADDR,v)
#define HWIO_DEHR_XPU_SCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_XPU_SCR_ADDR,m,v,HWIO_DEHR_XPU_SCR_IN)
#define HWIO_DEHR_XPU_SCR_SCLEIE_BMSK                                  0x400
#define HWIO_DEHR_XPU_SCR_SCLEIE_SHFT                                    0xa
#define HWIO_DEHR_XPU_SCR_SCFGEIE_BMSK                                 0x200
#define HWIO_DEHR_XPU_SCR_SCFGEIE_SHFT                                   0x9
#define HWIO_DEHR_XPU_SCR_DYNAMIC_CLK_EN_BMSK                          0x100
#define HWIO_DEHR_XPU_SCR_DYNAMIC_CLK_EN_SHFT                            0x8
#define HWIO_DEHR_XPU_SCR_NSRGCLEE_BMSK                                 0x40
#define HWIO_DEHR_XPU_SCR_NSRGCLEE_SHFT                                  0x6
#define HWIO_DEHR_XPU_SCR_NSCFGE_BMSK                                   0x20
#define HWIO_DEHR_XPU_SCR_NSCFGE_SHFT                                    0x5
#define HWIO_DEHR_XPU_SCR_SDCDEE_BMSK                                   0x10
#define HWIO_DEHR_XPU_SCR_SDCDEE_SHFT                                    0x4
#define HWIO_DEHR_XPU_SCR_SEIE_BMSK                                      0x8
#define HWIO_DEHR_XPU_SCR_SEIE_SHFT                                      0x3
#define HWIO_DEHR_XPU_SCR_SCLERE_BMSK                                    0x4
#define HWIO_DEHR_XPU_SCR_SCLERE_SHFT                                    0x2
#define HWIO_DEHR_XPU_SCR_SCFGERE_BMSK                                   0x2
#define HWIO_DEHR_XPU_SCR_SCFGERE_SHFT                                   0x1
#define HWIO_DEHR_XPU_SCR_XPUNSE_BMSK                                    0x1
#define HWIO_DEHR_XPU_SCR_XPUNSE_SHFT                                    0x0

#define HWIO_DEHR_XPU_SWDR_ADDR                                   (DEHR_XPU_REG_BASE      + 0x00000004)
#define HWIO_DEHR_XPU_SWDR_RMSK                                          0x1
#define HWIO_DEHR_XPU_SWDR_IN          \
        in_dword(HWIO_DEHR_XPU_SWDR_ADDR)
#define HWIO_DEHR_XPU_SWDR_INM(m)      \
        in_dword_masked(HWIO_DEHR_XPU_SWDR_ADDR, m)
#define HWIO_DEHR_XPU_SWDR_OUT(v)      \
        out_dword(HWIO_DEHR_XPU_SWDR_ADDR,v)
#define HWIO_DEHR_XPU_SWDR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_XPU_SWDR_ADDR,m,v,HWIO_DEHR_XPU_SWDR_IN)
#define HWIO_DEHR_XPU_SWDR_SCFGWD_BMSK                                   0x1
#define HWIO_DEHR_XPU_SWDR_SCFGWD_SHFT                                   0x0

#define HWIO_DEHR_XPU_SEAR0_ADDR                                  (DEHR_XPU_REG_BASE      + 0x00000040)
#define HWIO_DEHR_XPU_SEAR0_RMSK                                  0xffffffff
#define HWIO_DEHR_XPU_SEAR0_IN          \
        in_dword(HWIO_DEHR_XPU_SEAR0_ADDR)
#define HWIO_DEHR_XPU_SEAR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_XPU_SEAR0_ADDR, m)
#define HWIO_DEHR_XPU_SEAR0_PA_BMSK                               0xffffffff
#define HWIO_DEHR_XPU_SEAR0_PA_SHFT                                      0x0

#define HWIO_DEHR_XPU_SESR_ADDR                                   (DEHR_XPU_REG_BASE      + 0x00000048)
#define HWIO_DEHR_XPU_SESR_RMSK                                   0x8000000f
#define HWIO_DEHR_XPU_SESR_IN          \
        in_dword(HWIO_DEHR_XPU_SESR_ADDR)
#define HWIO_DEHR_XPU_SESR_INM(m)      \
        in_dword_masked(HWIO_DEHR_XPU_SESR_ADDR, m)
#define HWIO_DEHR_XPU_SESR_OUT(v)      \
        out_dword(HWIO_DEHR_XPU_SESR_ADDR,v)
#define HWIO_DEHR_XPU_SESR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_XPU_SESR_ADDR,m,v,HWIO_DEHR_XPU_SESR_IN)
#define HWIO_DEHR_XPU_SESR_MULTI_BMSK                             0x80000000
#define HWIO_DEHR_XPU_SESR_MULTI_SHFT                                   0x1f
#define HWIO_DEHR_XPU_SESR_CLMULTI_BMSK                                  0x8
#define HWIO_DEHR_XPU_SESR_CLMULTI_SHFT                                  0x3
#define HWIO_DEHR_XPU_SESR_CFGMULTI_BMSK                                 0x4
#define HWIO_DEHR_XPU_SESR_CFGMULTI_SHFT                                 0x2
#define HWIO_DEHR_XPU_SESR_CLIENT_BMSK                                   0x2
#define HWIO_DEHR_XPU_SESR_CLIENT_SHFT                                   0x1
#define HWIO_DEHR_XPU_SESR_CFG_BMSK                                      0x1
#define HWIO_DEHR_XPU_SESR_CFG_SHFT                                      0x0

#define HWIO_DEHR_XPU_SESRRESTORE_ADDR                            (DEHR_XPU_REG_BASE      + 0x0000004c)
#define HWIO_DEHR_XPU_SESRRESTORE_RMSK                            0x8000000f
#define HWIO_DEHR_XPU_SESRRESTORE_IN          \
        in_dword(HWIO_DEHR_XPU_SESRRESTORE_ADDR)
#define HWIO_DEHR_XPU_SESRRESTORE_INM(m)      \
        in_dword_masked(HWIO_DEHR_XPU_SESRRESTORE_ADDR, m)
#define HWIO_DEHR_XPU_SESRRESTORE_OUT(v)      \
        out_dword(HWIO_DEHR_XPU_SESRRESTORE_ADDR,v)
#define HWIO_DEHR_XPU_SESRRESTORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_XPU_SESRRESTORE_ADDR,m,v,HWIO_DEHR_XPU_SESRRESTORE_IN)
#define HWIO_DEHR_XPU_SESRRESTORE_MULTI_BMSK                      0x80000000
#define HWIO_DEHR_XPU_SESRRESTORE_MULTI_SHFT                            0x1f
#define HWIO_DEHR_XPU_SESRRESTORE_CLMULTI_BMSK                           0x8
#define HWIO_DEHR_XPU_SESRRESTORE_CLMULTI_SHFT                           0x3
#define HWIO_DEHR_XPU_SESRRESTORE_CFGMULTI_BMSK                          0x4
#define HWIO_DEHR_XPU_SESRRESTORE_CFGMULTI_SHFT                          0x2
#define HWIO_DEHR_XPU_SESRRESTORE_CLIENT_BMSK                            0x2
#define HWIO_DEHR_XPU_SESRRESTORE_CLIENT_SHFT                            0x1
#define HWIO_DEHR_XPU_SESRRESTORE_CFG_BMSK                               0x1
#define HWIO_DEHR_XPU_SESRRESTORE_CFG_SHFT                               0x0

#define HWIO_DEHR_XPU_SESYNR0_ADDR                                (DEHR_XPU_REG_BASE      + 0x00000050)
#define HWIO_DEHR_XPU_SESYNR0_RMSK                                0xffffffff
#define HWIO_DEHR_XPU_SESYNR0_IN          \
        in_dword(HWIO_DEHR_XPU_SESYNR0_ADDR)
#define HWIO_DEHR_XPU_SESYNR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_XPU_SESYNR0_ADDR, m)
#define HWIO_DEHR_XPU_SESYNR0_ATID_BMSK                           0xff000000
#define HWIO_DEHR_XPU_SESYNR0_ATID_SHFT                                 0x18
#define HWIO_DEHR_XPU_SESYNR0_AVMID_BMSK                            0xff0000
#define HWIO_DEHR_XPU_SESYNR0_AVMID_SHFT                                0x10
#define HWIO_DEHR_XPU_SESYNR0_ABID_BMSK                               0xe000
#define HWIO_DEHR_XPU_SESYNR0_ABID_SHFT                                  0xd
#define HWIO_DEHR_XPU_SESYNR0_APID_BMSK                               0x1f00
#define HWIO_DEHR_XPU_SESYNR0_APID_SHFT                                  0x8
#define HWIO_DEHR_XPU_SESYNR0_AMID_BMSK                                 0xff
#define HWIO_DEHR_XPU_SESYNR0_AMID_SHFT                                  0x0

#define HWIO_DEHR_XPU_SESYNR1_ADDR                                (DEHR_XPU_REG_BASE      + 0x00000054)
#define HWIO_DEHR_XPU_SESYNR1_RMSK                                0xffffffff
#define HWIO_DEHR_XPU_SESYNR1_IN          \
        in_dword(HWIO_DEHR_XPU_SESYNR1_ADDR)
#define HWIO_DEHR_XPU_SESYNR1_INM(m)      \
        in_dword_masked(HWIO_DEHR_XPU_SESYNR1_ADDR, m)
#define HWIO_DEHR_XPU_SESYNR1_DCD_BMSK                            0x80000000
#define HWIO_DEHR_XPU_SESYNR1_DCD_SHFT                                  0x1f
#define HWIO_DEHR_XPU_SESYNR1_AC_BMSK                             0x40000000
#define HWIO_DEHR_XPU_SESYNR1_AC_SHFT                                   0x1e
#define HWIO_DEHR_XPU_SESYNR1_BURSTLEN_BMSK                       0x20000000
#define HWIO_DEHR_XPU_SESYNR1_BURSTLEN_SHFT                             0x1d
#define HWIO_DEHR_XPU_SESYNR1_ARDALLOCATE_BMSK                    0x10000000
#define HWIO_DEHR_XPU_SESYNR1_ARDALLOCATE_SHFT                          0x1c
#define HWIO_DEHR_XPU_SESYNR1_ABURST_BMSK                          0x8000000
#define HWIO_DEHR_XPU_SESYNR1_ABURST_SHFT                               0x1b
#define HWIO_DEHR_XPU_SESYNR1_AEXCLUSIVE_BMSK                      0x4000000
#define HWIO_DEHR_XPU_SESYNR1_AEXCLUSIVE_SHFT                           0x1a
#define HWIO_DEHR_XPU_SESYNR1_AWRITE_BMSK                          0x2000000
#define HWIO_DEHR_XPU_SESYNR1_AWRITE_SHFT                               0x19
#define HWIO_DEHR_XPU_SESYNR1_AFULL_BMSK                           0x1000000
#define HWIO_DEHR_XPU_SESYNR1_AFULL_SHFT                                0x18
#define HWIO_DEHR_XPU_SESYNR1_ARDBEADNDXEN_BMSK                     0x800000
#define HWIO_DEHR_XPU_SESYNR1_ARDBEADNDXEN_SHFT                         0x17
#define HWIO_DEHR_XPU_SESYNR1_AOOO_BMSK                             0x400000
#define HWIO_DEHR_XPU_SESYNR1_AOOO_SHFT                                 0x16
#define HWIO_DEHR_XPU_SESYNR1_APREQPRIORITY_BMSK                    0x380000
#define HWIO_DEHR_XPU_SESYNR1_APREQPRIORITY_SHFT                        0x13
#define HWIO_DEHR_XPU_SESYNR1_ASIZE_BMSK                             0x70000
#define HWIO_DEHR_XPU_SESYNR1_ASIZE_SHFT                                0x10
#define HWIO_DEHR_XPU_SESYNR1_AMSSSELFAUTH_BMSK                       0x8000
#define HWIO_DEHR_XPU_SESYNR1_AMSSSELFAUTH_SHFT                          0xf
#define HWIO_DEHR_XPU_SESYNR1_ALEN_BMSK                               0x7f00
#define HWIO_DEHR_XPU_SESYNR1_ALEN_SHFT                                  0x8
#define HWIO_DEHR_XPU_SESYNR1_AINST_BMSK                                0x80
#define HWIO_DEHR_XPU_SESYNR1_AINST_SHFT                                 0x7
#define HWIO_DEHR_XPU_SESYNR1_APROTNS_BMSK                              0x40
#define HWIO_DEHR_XPU_SESYNR1_APROTNS_SHFT                               0x6
#define HWIO_DEHR_XPU_SESYNR1_APRIV_BMSK                                0x20
#define HWIO_DEHR_XPU_SESYNR1_APRIV_SHFT                                 0x5
#define HWIO_DEHR_XPU_SESYNR1_AINNERSHARED_BMSK                         0x10
#define HWIO_DEHR_XPU_SESYNR1_AINNERSHARED_SHFT                          0x4
#define HWIO_DEHR_XPU_SESYNR1_ASHARED_BMSK                               0x8
#define HWIO_DEHR_XPU_SESYNR1_ASHARED_SHFT                               0x3
#define HWIO_DEHR_XPU_SESYNR1_AMEMTYPE_BMSK                              0x7
#define HWIO_DEHR_XPU_SESYNR1_AMEMTYPE_SHFT                              0x0

#define HWIO_DEHR_XPU_SESYNR2_ADDR                                (DEHR_XPU_REG_BASE      + 0x00000058)
#define HWIO_DEHR_XPU_SESYNR2_RMSK                                       0x7
#define HWIO_DEHR_XPU_SESYNR2_IN          \
        in_dword(HWIO_DEHR_XPU_SESYNR2_ADDR)
#define HWIO_DEHR_XPU_SESYNR2_INM(m)      \
        in_dword_masked(HWIO_DEHR_XPU_SESYNR2_ADDR, m)
#define HWIO_DEHR_XPU_SESYNR2_MODEM_PRT_HIT_BMSK                         0x4
#define HWIO_DEHR_XPU_SESYNR2_MODEM_PRT_HIT_SHFT                         0x2
#define HWIO_DEHR_XPU_SESYNR2_SECURE_PRT_HIT_BMSK                        0x2
#define HWIO_DEHR_XPU_SESYNR2_SECURE_PRT_HIT_SHFT                        0x1
#define HWIO_DEHR_XPU_SESYNR2_NONSECURE_PRT_HIT_BMSK                     0x1
#define HWIO_DEHR_XPU_SESYNR2_NONSECURE_PRT_HIT_SHFT                     0x0

#define HWIO_DEHR_XPU_MCR_ADDR                                    (DEHR_XPU_REG_BASE      + 0x00000100)
#define HWIO_DEHR_XPU_MCR_RMSK                                         0x71f
#define HWIO_DEHR_XPU_MCR_IN          \
        in_dword(HWIO_DEHR_XPU_MCR_ADDR)
#define HWIO_DEHR_XPU_MCR_INM(m)      \
        in_dword_masked(HWIO_DEHR_XPU_MCR_ADDR, m)
#define HWIO_DEHR_XPU_MCR_OUT(v)      \
        out_dword(HWIO_DEHR_XPU_MCR_ADDR,v)
#define HWIO_DEHR_XPU_MCR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_XPU_MCR_ADDR,m,v,HWIO_DEHR_XPU_MCR_IN)
#define HWIO_DEHR_XPU_MCR_CLEIE_BMSK                                   0x400
#define HWIO_DEHR_XPU_MCR_CLEIE_SHFT                                     0xa
#define HWIO_DEHR_XPU_MCR_CFGEIE_BMSK                                  0x200
#define HWIO_DEHR_XPU_MCR_CFGEIE_SHFT                                    0x9
#define HWIO_DEHR_XPU_MCR_DYNAMIC_CLK_EN_BMSK                          0x100
#define HWIO_DEHR_XPU_MCR_DYNAMIC_CLK_EN_SHFT                            0x8
#define HWIO_DEHR_XPU_MCR_DCDEE_BMSK                                    0x10
#define HWIO_DEHR_XPU_MCR_DCDEE_SHFT                                     0x4
#define HWIO_DEHR_XPU_MCR_EIE_BMSK                                       0x8
#define HWIO_DEHR_XPU_MCR_EIE_SHFT                                       0x3
#define HWIO_DEHR_XPU_MCR_CLERE_BMSK                                     0x4
#define HWIO_DEHR_XPU_MCR_CLERE_SHFT                                     0x2
#define HWIO_DEHR_XPU_MCR_CFGERE_BMSK                                    0x2
#define HWIO_DEHR_XPU_MCR_CFGERE_SHFT                                    0x1
#define HWIO_DEHR_XPU_MCR_XPUMSAE_BMSK                                   0x1
#define HWIO_DEHR_XPU_MCR_XPUMSAE_SHFT                                   0x0

#define HWIO_DEHR_XPU_MEAR0_ADDR                                  (DEHR_XPU_REG_BASE      + 0x00000140)
#define HWIO_DEHR_XPU_MEAR0_RMSK                                  0xffffffff
#define HWIO_DEHR_XPU_MEAR0_IN          \
        in_dword(HWIO_DEHR_XPU_MEAR0_ADDR)
#define HWIO_DEHR_XPU_MEAR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_XPU_MEAR0_ADDR, m)
#define HWIO_DEHR_XPU_MEAR0_PA_BMSK                               0xffffffff
#define HWIO_DEHR_XPU_MEAR0_PA_SHFT                                      0x0

#define HWIO_DEHR_XPU_MESR_ADDR                                   (DEHR_XPU_REG_BASE      + 0x00000148)
#define HWIO_DEHR_XPU_MESR_RMSK                                   0x8000000f
#define HWIO_DEHR_XPU_MESR_IN          \
        in_dword(HWIO_DEHR_XPU_MESR_ADDR)
#define HWIO_DEHR_XPU_MESR_INM(m)      \
        in_dword_masked(HWIO_DEHR_XPU_MESR_ADDR, m)
#define HWIO_DEHR_XPU_MESR_OUT(v)      \
        out_dword(HWIO_DEHR_XPU_MESR_ADDR,v)
#define HWIO_DEHR_XPU_MESR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_XPU_MESR_ADDR,m,v,HWIO_DEHR_XPU_MESR_IN)
#define HWIO_DEHR_XPU_MESR_MULTI_BMSK                             0x80000000
#define HWIO_DEHR_XPU_MESR_MULTI_SHFT                                   0x1f
#define HWIO_DEHR_XPU_MESR_CLMULTI_BMSK                                  0x8
#define HWIO_DEHR_XPU_MESR_CLMULTI_SHFT                                  0x3
#define HWIO_DEHR_XPU_MESR_CFGMULTI_BMSK                                 0x4
#define HWIO_DEHR_XPU_MESR_CFGMULTI_SHFT                                 0x2
#define HWIO_DEHR_XPU_MESR_CLIENT_BMSK                                   0x2
#define HWIO_DEHR_XPU_MESR_CLIENT_SHFT                                   0x1
#define HWIO_DEHR_XPU_MESR_CFG_BMSK                                      0x1
#define HWIO_DEHR_XPU_MESR_CFG_SHFT                                      0x0

#define HWIO_DEHR_XPU_MESRRESTORE_ADDR                            (DEHR_XPU_REG_BASE      + 0x0000014c)
#define HWIO_DEHR_XPU_MESRRESTORE_RMSK                            0x8000000f
#define HWIO_DEHR_XPU_MESRRESTORE_IN          \
        in_dword(HWIO_DEHR_XPU_MESRRESTORE_ADDR)
#define HWIO_DEHR_XPU_MESRRESTORE_INM(m)      \
        in_dword_masked(HWIO_DEHR_XPU_MESRRESTORE_ADDR, m)
#define HWIO_DEHR_XPU_MESRRESTORE_OUT(v)      \
        out_dword(HWIO_DEHR_XPU_MESRRESTORE_ADDR,v)
#define HWIO_DEHR_XPU_MESRRESTORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_XPU_MESRRESTORE_ADDR,m,v,HWIO_DEHR_XPU_MESRRESTORE_IN)
#define HWIO_DEHR_XPU_MESRRESTORE_MULTI_BMSK                      0x80000000
#define HWIO_DEHR_XPU_MESRRESTORE_MULTI_SHFT                            0x1f
#define HWIO_DEHR_XPU_MESRRESTORE_CLMULTI_BMSK                           0x8
#define HWIO_DEHR_XPU_MESRRESTORE_CLMULTI_SHFT                           0x3
#define HWIO_DEHR_XPU_MESRRESTORE_CFGMULTI_BMSK                          0x4
#define HWIO_DEHR_XPU_MESRRESTORE_CFGMULTI_SHFT                          0x2
#define HWIO_DEHR_XPU_MESRRESTORE_CLIENT_BMSK                            0x2
#define HWIO_DEHR_XPU_MESRRESTORE_CLIENT_SHFT                            0x1
#define HWIO_DEHR_XPU_MESRRESTORE_CFG_BMSK                               0x1
#define HWIO_DEHR_XPU_MESRRESTORE_CFG_SHFT                               0x0

#define HWIO_DEHR_XPU_MESYNR0_ADDR                                (DEHR_XPU_REG_BASE      + 0x00000150)
#define HWIO_DEHR_XPU_MESYNR0_RMSK                                0xffffffff
#define HWIO_DEHR_XPU_MESYNR0_IN          \
        in_dword(HWIO_DEHR_XPU_MESYNR0_ADDR)
#define HWIO_DEHR_XPU_MESYNR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_XPU_MESYNR0_ADDR, m)
#define HWIO_DEHR_XPU_MESYNR0_ATID_BMSK                           0xff000000
#define HWIO_DEHR_XPU_MESYNR0_ATID_SHFT                                 0x18
#define HWIO_DEHR_XPU_MESYNR0_AVMID_BMSK                            0xff0000
#define HWIO_DEHR_XPU_MESYNR0_AVMID_SHFT                                0x10
#define HWIO_DEHR_XPU_MESYNR0_ABID_BMSK                               0xe000
#define HWIO_DEHR_XPU_MESYNR0_ABID_SHFT                                  0xd
#define HWIO_DEHR_XPU_MESYNR0_APID_BMSK                               0x1f00
#define HWIO_DEHR_XPU_MESYNR0_APID_SHFT                                  0x8
#define HWIO_DEHR_XPU_MESYNR0_AMID_BMSK                                 0xff
#define HWIO_DEHR_XPU_MESYNR0_AMID_SHFT                                  0x0

#define HWIO_DEHR_XPU_MESYNR1_ADDR                                (DEHR_XPU_REG_BASE      + 0x00000154)
#define HWIO_DEHR_XPU_MESYNR1_RMSK                                0xffffffff
#define HWIO_DEHR_XPU_MESYNR1_IN          \
        in_dword(HWIO_DEHR_XPU_MESYNR1_ADDR)
#define HWIO_DEHR_XPU_MESYNR1_INM(m)      \
        in_dword_masked(HWIO_DEHR_XPU_MESYNR1_ADDR, m)
#define HWIO_DEHR_XPU_MESYNR1_DCD_BMSK                            0x80000000
#define HWIO_DEHR_XPU_MESYNR1_DCD_SHFT                                  0x1f
#define HWIO_DEHR_XPU_MESYNR1_AC_BMSK                             0x40000000
#define HWIO_DEHR_XPU_MESYNR1_AC_SHFT                                   0x1e
#define HWIO_DEHR_XPU_MESYNR1_BURSTLEN_BMSK                       0x20000000
#define HWIO_DEHR_XPU_MESYNR1_BURSTLEN_SHFT                             0x1d
#define HWIO_DEHR_XPU_MESYNR1_ARDALLOCATE_BMSK                    0x10000000
#define HWIO_DEHR_XPU_MESYNR1_ARDALLOCATE_SHFT                          0x1c
#define HWIO_DEHR_XPU_MESYNR1_ABURST_BMSK                          0x8000000
#define HWIO_DEHR_XPU_MESYNR1_ABURST_SHFT                               0x1b
#define HWIO_DEHR_XPU_MESYNR1_AEXCLUSIVE_BMSK                      0x4000000
#define HWIO_DEHR_XPU_MESYNR1_AEXCLUSIVE_SHFT                           0x1a
#define HWIO_DEHR_XPU_MESYNR1_AWRITE_BMSK                          0x2000000
#define HWIO_DEHR_XPU_MESYNR1_AWRITE_SHFT                               0x19
#define HWIO_DEHR_XPU_MESYNR1_AFULL_BMSK                           0x1000000
#define HWIO_DEHR_XPU_MESYNR1_AFULL_SHFT                                0x18
#define HWIO_DEHR_XPU_MESYNR1_ARDBEADNDXEN_BMSK                     0x800000
#define HWIO_DEHR_XPU_MESYNR1_ARDBEADNDXEN_SHFT                         0x17
#define HWIO_DEHR_XPU_MESYNR1_AOOO_BMSK                             0x400000
#define HWIO_DEHR_XPU_MESYNR1_AOOO_SHFT                                 0x16
#define HWIO_DEHR_XPU_MESYNR1_APREQPRIORITY_BMSK                    0x380000
#define HWIO_DEHR_XPU_MESYNR1_APREQPRIORITY_SHFT                        0x13
#define HWIO_DEHR_XPU_MESYNR1_ASIZE_BMSK                             0x70000
#define HWIO_DEHR_XPU_MESYNR1_ASIZE_SHFT                                0x10
#define HWIO_DEHR_XPU_MESYNR1_AMSSSELFAUTH_BMSK                       0x8000
#define HWIO_DEHR_XPU_MESYNR1_AMSSSELFAUTH_SHFT                          0xf
#define HWIO_DEHR_XPU_MESYNR1_ALEN_BMSK                               0x7f00
#define HWIO_DEHR_XPU_MESYNR1_ALEN_SHFT                                  0x8
#define HWIO_DEHR_XPU_MESYNR1_AINST_BMSK                                0x80
#define HWIO_DEHR_XPU_MESYNR1_AINST_SHFT                                 0x7
#define HWIO_DEHR_XPU_MESYNR1_APROTNS_BMSK                              0x40
#define HWIO_DEHR_XPU_MESYNR1_APROTNS_SHFT                               0x6
#define HWIO_DEHR_XPU_MESYNR1_APRIV_BMSK                                0x20
#define HWIO_DEHR_XPU_MESYNR1_APRIV_SHFT                                 0x5
#define HWIO_DEHR_XPU_MESYNR1_AINNERSHARED_BMSK                         0x10
#define HWIO_DEHR_XPU_MESYNR1_AINNERSHARED_SHFT                          0x4
#define HWIO_DEHR_XPU_MESYNR1_ASHARED_BMSK                               0x8
#define HWIO_DEHR_XPU_MESYNR1_ASHARED_SHFT                               0x3
#define HWIO_DEHR_XPU_MESYNR1_AMEMTYPE_BMSK                              0x7
#define HWIO_DEHR_XPU_MESYNR1_AMEMTYPE_SHFT                              0x0

#define HWIO_DEHR_XPU_MESYNR2_ADDR                                (DEHR_XPU_REG_BASE      + 0x00000158)
#define HWIO_DEHR_XPU_MESYNR2_RMSK                                       0x7
#define HWIO_DEHR_XPU_MESYNR2_IN          \
        in_dword(HWIO_DEHR_XPU_MESYNR2_ADDR)
#define HWIO_DEHR_XPU_MESYNR2_INM(m)      \
        in_dword_masked(HWIO_DEHR_XPU_MESYNR2_ADDR, m)
#define HWIO_DEHR_XPU_MESYNR2_MODEM_PRT_HIT_BMSK                         0x4
#define HWIO_DEHR_XPU_MESYNR2_MODEM_PRT_HIT_SHFT                         0x2
#define HWIO_DEHR_XPU_MESYNR2_SECURE_PRT_HIT_BMSK                        0x2
#define HWIO_DEHR_XPU_MESYNR2_SECURE_PRT_HIT_SHFT                        0x1
#define HWIO_DEHR_XPU_MESYNR2_NONSECURE_PRT_HIT_BMSK                     0x1
#define HWIO_DEHR_XPU_MESYNR2_NONSECURE_PRT_HIT_SHFT                     0x0

#define HWIO_DEHR_XPU_CR_ADDR                                     (DEHR_XPU_REG_BASE      + 0x00000080)
#define HWIO_DEHR_XPU_CR_RMSK                                        0x1071f
#define HWIO_DEHR_XPU_CR_IN          \
        in_dword(HWIO_DEHR_XPU_CR_ADDR)
#define HWIO_DEHR_XPU_CR_INM(m)      \
        in_dword_masked(HWIO_DEHR_XPU_CR_ADDR, m)
#define HWIO_DEHR_XPU_CR_OUT(v)      \
        out_dword(HWIO_DEHR_XPU_CR_ADDR,v)
#define HWIO_DEHR_XPU_CR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_XPU_CR_ADDR,m,v,HWIO_DEHR_XPU_CR_IN)
#define HWIO_DEHR_XPU_CR_MSAE_BMSK                                   0x10000
#define HWIO_DEHR_XPU_CR_MSAE_SHFT                                      0x10
#define HWIO_DEHR_XPU_CR_CLEIE_BMSK                                    0x400
#define HWIO_DEHR_XPU_CR_CLEIE_SHFT                                      0xa
#define HWIO_DEHR_XPU_CR_CFGEIE_BMSK                                   0x200
#define HWIO_DEHR_XPU_CR_CFGEIE_SHFT                                     0x9
#define HWIO_DEHR_XPU_CR_DYNAMIC_CLK_EN_BMSK                           0x100
#define HWIO_DEHR_XPU_CR_DYNAMIC_CLK_EN_SHFT                             0x8
#define HWIO_DEHR_XPU_CR_DCDEE_BMSK                                     0x10
#define HWIO_DEHR_XPU_CR_DCDEE_SHFT                                      0x4
#define HWIO_DEHR_XPU_CR_EIE_BMSK                                        0x8
#define HWIO_DEHR_XPU_CR_EIE_SHFT                                        0x3
#define HWIO_DEHR_XPU_CR_CLERE_BMSK                                      0x4
#define HWIO_DEHR_XPU_CR_CLERE_SHFT                                      0x2
#define HWIO_DEHR_XPU_CR_CFGERE_BMSK                                     0x2
#define HWIO_DEHR_XPU_CR_CFGERE_SHFT                                     0x1
#define HWIO_DEHR_XPU_CR_XPUVMIDE_BMSK                                   0x1
#define HWIO_DEHR_XPU_CR_XPUVMIDE_SHFT                                   0x0

#define HWIO_DEHR_XPU_RPU_ACRn_ADDR(n)                            (DEHR_XPU_REG_BASE      + 0x000000a0 + 0x4 * (n))
#define HWIO_DEHR_XPU_RPU_ACRn_RMSK                               0xffffffff
#define HWIO_DEHR_XPU_RPU_ACRn_MAXn                                        0
#define HWIO_DEHR_XPU_RPU_ACRn_INI(n)        \
        in_dword_masked(HWIO_DEHR_XPU_RPU_ACRn_ADDR(n), HWIO_DEHR_XPU_RPU_ACRn_RMSK)
#define HWIO_DEHR_XPU_RPU_ACRn_INMI(n,mask)    \
        in_dword_masked(HWIO_DEHR_XPU_RPU_ACRn_ADDR(n), mask)
#define HWIO_DEHR_XPU_RPU_ACRn_OUTI(n,val)    \
        out_dword(HWIO_DEHR_XPU_RPU_ACRn_ADDR(n),val)
#define HWIO_DEHR_XPU_RPU_ACRn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_DEHR_XPU_RPU_ACRn_ADDR(n),mask,val,HWIO_DEHR_XPU_RPU_ACRn_INI(n))
#define HWIO_DEHR_XPU_RPU_ACRn_RWE_BMSK                           0xffffffff
#define HWIO_DEHR_XPU_RPU_ACRn_RWE_SHFT                                  0x0

#define HWIO_DEHR_XPU_EAR0_ADDR                                   (DEHR_XPU_REG_BASE      + 0x000000c0)
#define HWIO_DEHR_XPU_EAR0_RMSK                                   0xffffffff
#define HWIO_DEHR_XPU_EAR0_IN          \
        in_dword(HWIO_DEHR_XPU_EAR0_ADDR)
#define HWIO_DEHR_XPU_EAR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_XPU_EAR0_ADDR, m)
#define HWIO_DEHR_XPU_EAR0_PA_BMSK                                0xffffffff
#define HWIO_DEHR_XPU_EAR0_PA_SHFT                                       0x0

#define HWIO_DEHR_XPU_ESR_ADDR                                    (DEHR_XPU_REG_BASE      + 0x000000c8)
#define HWIO_DEHR_XPU_ESR_RMSK                                    0x8000000f
#define HWIO_DEHR_XPU_ESR_IN          \
        in_dword(HWIO_DEHR_XPU_ESR_ADDR)
#define HWIO_DEHR_XPU_ESR_INM(m)      \
        in_dword_masked(HWIO_DEHR_XPU_ESR_ADDR, m)
#define HWIO_DEHR_XPU_ESR_OUT(v)      \
        out_dword(HWIO_DEHR_XPU_ESR_ADDR,v)
#define HWIO_DEHR_XPU_ESR_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_XPU_ESR_ADDR,m,v,HWIO_DEHR_XPU_ESR_IN)
#define HWIO_DEHR_XPU_ESR_MULTI_BMSK                              0x80000000
#define HWIO_DEHR_XPU_ESR_MULTI_SHFT                                    0x1f
#define HWIO_DEHR_XPU_ESR_CLMULTI_BMSK                                   0x8
#define HWIO_DEHR_XPU_ESR_CLMULTI_SHFT                                   0x3
#define HWIO_DEHR_XPU_ESR_CFGMULTI_BMSK                                  0x4
#define HWIO_DEHR_XPU_ESR_CFGMULTI_SHFT                                  0x2
#define HWIO_DEHR_XPU_ESR_CLIENT_BMSK                                    0x2
#define HWIO_DEHR_XPU_ESR_CLIENT_SHFT                                    0x1
#define HWIO_DEHR_XPU_ESR_CFG_BMSK                                       0x1
#define HWIO_DEHR_XPU_ESR_CFG_SHFT                                       0x0

#define HWIO_DEHR_XPU_ESRRESTORE_ADDR                             (DEHR_XPU_REG_BASE      + 0x000000cc)
#define HWIO_DEHR_XPU_ESRRESTORE_RMSK                             0x8000000f
#define HWIO_DEHR_XPU_ESRRESTORE_IN          \
        in_dword(HWIO_DEHR_XPU_ESRRESTORE_ADDR)
#define HWIO_DEHR_XPU_ESRRESTORE_INM(m)      \
        in_dword_masked(HWIO_DEHR_XPU_ESRRESTORE_ADDR, m)
#define HWIO_DEHR_XPU_ESRRESTORE_OUT(v)      \
        out_dword(HWIO_DEHR_XPU_ESRRESTORE_ADDR,v)
#define HWIO_DEHR_XPU_ESRRESTORE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_DEHR_XPU_ESRRESTORE_ADDR,m,v,HWIO_DEHR_XPU_ESRRESTORE_IN)
#define HWIO_DEHR_XPU_ESRRESTORE_MULTI_BMSK                       0x80000000
#define HWIO_DEHR_XPU_ESRRESTORE_MULTI_SHFT                             0x1f
#define HWIO_DEHR_XPU_ESRRESTORE_CLMULTI_BMSK                            0x8
#define HWIO_DEHR_XPU_ESRRESTORE_CLMULTI_SHFT                            0x3
#define HWIO_DEHR_XPU_ESRRESTORE_CFGMULTI_BMSK                           0x4
#define HWIO_DEHR_XPU_ESRRESTORE_CFGMULTI_SHFT                           0x2
#define HWIO_DEHR_XPU_ESRRESTORE_CLIENT_BMSK                             0x2
#define HWIO_DEHR_XPU_ESRRESTORE_CLIENT_SHFT                             0x1
#define HWIO_DEHR_XPU_ESRRESTORE_CFG_BMSK                                0x1
#define HWIO_DEHR_XPU_ESRRESTORE_CFG_SHFT                                0x0

#define HWIO_DEHR_XPU_ESYNR0_ADDR                                 (DEHR_XPU_REG_BASE      + 0x000000d0)
#define HWIO_DEHR_XPU_ESYNR0_RMSK                                 0xffffffff
#define HWIO_DEHR_XPU_ESYNR0_IN          \
        in_dword(HWIO_DEHR_XPU_ESYNR0_ADDR)
#define HWIO_DEHR_XPU_ESYNR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_XPU_ESYNR0_ADDR, m)
#define HWIO_DEHR_XPU_ESYNR0_ATID_BMSK                            0xff000000
#define HWIO_DEHR_XPU_ESYNR0_ATID_SHFT                                  0x18
#define HWIO_DEHR_XPU_ESYNR0_AVMID_BMSK                             0xff0000
#define HWIO_DEHR_XPU_ESYNR0_AVMID_SHFT                                 0x10
#define HWIO_DEHR_XPU_ESYNR0_ABID_BMSK                                0xe000
#define HWIO_DEHR_XPU_ESYNR0_ABID_SHFT                                   0xd
#define HWIO_DEHR_XPU_ESYNR0_APID_BMSK                                0x1f00
#define HWIO_DEHR_XPU_ESYNR0_APID_SHFT                                   0x8
#define HWIO_DEHR_XPU_ESYNR0_AMID_BMSK                                  0xff
#define HWIO_DEHR_XPU_ESYNR0_AMID_SHFT                                   0x0

#define HWIO_DEHR_XPU_ESYNR1_ADDR                                 (DEHR_XPU_REG_BASE      + 0x000000d4)
#define HWIO_DEHR_XPU_ESYNR1_RMSK                                 0xffffffff
#define HWIO_DEHR_XPU_ESYNR1_IN          \
        in_dword(HWIO_DEHR_XPU_ESYNR1_ADDR)
#define HWIO_DEHR_XPU_ESYNR1_INM(m)      \
        in_dword_masked(HWIO_DEHR_XPU_ESYNR1_ADDR, m)
#define HWIO_DEHR_XPU_ESYNR1_DCD_BMSK                             0x80000000
#define HWIO_DEHR_XPU_ESYNR1_DCD_SHFT                                   0x1f
#define HWIO_DEHR_XPU_ESYNR1_AC_BMSK                              0x40000000
#define HWIO_DEHR_XPU_ESYNR1_AC_SHFT                                    0x1e
#define HWIO_DEHR_XPU_ESYNR1_BURSTLEN_BMSK                        0x20000000
#define HWIO_DEHR_XPU_ESYNR1_BURSTLEN_SHFT                              0x1d
#define HWIO_DEHR_XPU_ESYNR1_ARDALLOCATE_BMSK                     0x10000000
#define HWIO_DEHR_XPU_ESYNR1_ARDALLOCATE_SHFT                           0x1c
#define HWIO_DEHR_XPU_ESYNR1_ABURST_BMSK                           0x8000000
#define HWIO_DEHR_XPU_ESYNR1_ABURST_SHFT                                0x1b
#define HWIO_DEHR_XPU_ESYNR1_AEXCLUSIVE_BMSK                       0x4000000
#define HWIO_DEHR_XPU_ESYNR1_AEXCLUSIVE_SHFT                            0x1a
#define HWIO_DEHR_XPU_ESYNR1_AWRITE_BMSK                           0x2000000
#define HWIO_DEHR_XPU_ESYNR1_AWRITE_SHFT                                0x19
#define HWIO_DEHR_XPU_ESYNR1_AFULL_BMSK                            0x1000000
#define HWIO_DEHR_XPU_ESYNR1_AFULL_SHFT                                 0x18
#define HWIO_DEHR_XPU_ESYNR1_ARDBEADNDXEN_BMSK                      0x800000
#define HWIO_DEHR_XPU_ESYNR1_ARDBEADNDXEN_SHFT                          0x17
#define HWIO_DEHR_XPU_ESYNR1_AOOO_BMSK                              0x400000
#define HWIO_DEHR_XPU_ESYNR1_AOOO_SHFT                                  0x16
#define HWIO_DEHR_XPU_ESYNR1_APREQPRIORITY_BMSK                     0x380000
#define HWIO_DEHR_XPU_ESYNR1_APREQPRIORITY_SHFT                         0x13
#define HWIO_DEHR_XPU_ESYNR1_ASIZE_BMSK                              0x70000
#define HWIO_DEHR_XPU_ESYNR1_ASIZE_SHFT                                 0x10
#define HWIO_DEHR_XPU_ESYNR1_AMSSSELFAUTH_BMSK                        0x8000
#define HWIO_DEHR_XPU_ESYNR1_AMSSSELFAUTH_SHFT                           0xf
#define HWIO_DEHR_XPU_ESYNR1_ALEN_BMSK                                0x7f00
#define HWIO_DEHR_XPU_ESYNR1_ALEN_SHFT                                   0x8
#define HWIO_DEHR_XPU_ESYNR1_AINST_BMSK                                 0x80
#define HWIO_DEHR_XPU_ESYNR1_AINST_SHFT                                  0x7
#define HWIO_DEHR_XPU_ESYNR1_APROTNS_BMSK                               0x40
#define HWIO_DEHR_XPU_ESYNR1_APROTNS_SHFT                                0x6
#define HWIO_DEHR_XPU_ESYNR1_APRIV_BMSK                                 0x20
#define HWIO_DEHR_XPU_ESYNR1_APRIV_SHFT                                  0x5
#define HWIO_DEHR_XPU_ESYNR1_AINNERSHARED_BMSK                          0x10
#define HWIO_DEHR_XPU_ESYNR1_AINNERSHARED_SHFT                           0x4
#define HWIO_DEHR_XPU_ESYNR1_ASHARED_BMSK                                0x8
#define HWIO_DEHR_XPU_ESYNR1_ASHARED_SHFT                                0x3
#define HWIO_DEHR_XPU_ESYNR1_AMEMTYPE_BMSK                               0x7
#define HWIO_DEHR_XPU_ESYNR1_AMEMTYPE_SHFT                               0x0

#define HWIO_DEHR_XPU_ESYNR2_ADDR                                 (DEHR_XPU_REG_BASE      + 0x000000d8)
#define HWIO_DEHR_XPU_ESYNR2_RMSK                                        0x7
#define HWIO_DEHR_XPU_ESYNR2_IN          \
        in_dword(HWIO_DEHR_XPU_ESYNR2_ADDR)
#define HWIO_DEHR_XPU_ESYNR2_INM(m)      \
        in_dword_masked(HWIO_DEHR_XPU_ESYNR2_ADDR, m)
#define HWIO_DEHR_XPU_ESYNR2_MODEM_PRT_HIT_BMSK                          0x4
#define HWIO_DEHR_XPU_ESYNR2_MODEM_PRT_HIT_SHFT                          0x2
#define HWIO_DEHR_XPU_ESYNR2_SECURE_PRT_HIT_BMSK                         0x2
#define HWIO_DEHR_XPU_ESYNR2_SECURE_PRT_HIT_SHFT                         0x1
#define HWIO_DEHR_XPU_ESYNR2_NONSECURE_PRT_HIT_BMSK                      0x1
#define HWIO_DEHR_XPU_ESYNR2_NONSECURE_PRT_HIT_SHFT                      0x0

#define HWIO_DEHR_XPU_IDR0_ADDR                                   (DEHR_XPU_REG_BASE      + 0x00000074)
#define HWIO_DEHR_XPU_IDR0_RMSK                                   0xc000bfff
#define HWIO_DEHR_XPU_IDR0_IN          \
        in_dword(HWIO_DEHR_XPU_IDR0_ADDR)
#define HWIO_DEHR_XPU_IDR0_INM(m)      \
        in_dword_masked(HWIO_DEHR_XPU_IDR0_ADDR, m)
#define HWIO_DEHR_XPU_IDR0_CLIENTREQ_HALT_ACK_HW_EN_BMSK          0x80000000
#define HWIO_DEHR_XPU_IDR0_CLIENTREQ_HALT_ACK_HW_EN_SHFT                0x1f
#define HWIO_DEHR_XPU_IDR0_SAVERESTORE_HW_EN_BMSK                 0x40000000
#define HWIO_DEHR_XPU_IDR0_SAVERESTORE_HW_EN_SHFT                       0x1e
#define HWIO_DEHR_XPU_IDR0_BLED_BMSK                                  0x8000
#define HWIO_DEHR_XPU_IDR0_BLED_SHFT                                     0xf
#define HWIO_DEHR_XPU_IDR0_XPUT_BMSK                                  0x3000
#define HWIO_DEHR_XPU_IDR0_XPUT_SHFT                                     0xc
#define HWIO_DEHR_XPU_IDR0_PT_BMSK                                     0x800
#define HWIO_DEHR_XPU_IDR0_PT_SHFT                                       0xb
#define HWIO_DEHR_XPU_IDR0_MV_BMSK                                     0x400
#define HWIO_DEHR_XPU_IDR0_MV_SHFT                                       0xa
#define HWIO_DEHR_XPU_IDR0_NRG_BMSK                                    0x3ff
#define HWIO_DEHR_XPU_IDR0_NRG_SHFT                                      0x0

#define HWIO_DEHR_XPU_IDR1_ADDR                                   (DEHR_XPU_REG_BASE      + 0x00000078)
#define HWIO_DEHR_XPU_IDR1_RMSK                                   0x7f3ffeff
#define HWIO_DEHR_XPU_IDR1_IN          \
        in_dword(HWIO_DEHR_XPU_IDR1_ADDR)
#define HWIO_DEHR_XPU_IDR1_INM(m)      \
        in_dword_masked(HWIO_DEHR_XPU_IDR1_ADDR, m)
#define HWIO_DEHR_XPU_IDR1_AMT_HW_ENABLE_BMSK                     0x40000000
#define HWIO_DEHR_XPU_IDR1_AMT_HW_ENABLE_SHFT                           0x1e
#define HWIO_DEHR_XPU_IDR1_CLIENT_ADDR_WIDTH_BMSK                 0x3f000000
#define HWIO_DEHR_XPU_IDR1_CLIENT_ADDR_WIDTH_SHFT                       0x18
#define HWIO_DEHR_XPU_IDR1_CONFIG_ADDR_WIDTH_BMSK                   0x3f0000
#define HWIO_DEHR_XPU_IDR1_CONFIG_ADDR_WIDTH_SHFT                       0x10
#define HWIO_DEHR_XPU_IDR1_QRIB_EN_BMSK                               0x8000
#define HWIO_DEHR_XPU_IDR1_QRIB_EN_SHFT                                  0xf
#define HWIO_DEHR_XPU_IDR1_ASYNC_MODE_BMSK                            0x4000
#define HWIO_DEHR_XPU_IDR1_ASYNC_MODE_SHFT                               0xe
#define HWIO_DEHR_XPU_IDR1_CONFIG_TYPE_BMSK                           0x2000
#define HWIO_DEHR_XPU_IDR1_CONFIG_TYPE_SHFT                              0xd
#define HWIO_DEHR_XPU_IDR1_CLIENT_PIPELINE_ENABLED_BMSK               0x1000
#define HWIO_DEHR_XPU_IDR1_CLIENT_PIPELINE_ENABLED_SHFT                  0xc
#define HWIO_DEHR_XPU_IDR1_MSA_CHECK_HW_ENABLE_BMSK                    0x800
#define HWIO_DEHR_XPU_IDR1_MSA_CHECK_HW_ENABLE_SHFT                      0xb
#define HWIO_DEHR_XPU_IDR1_XPU_SYND_REG_ABSENT_BMSK                    0x400
#define HWIO_DEHR_XPU_IDR1_XPU_SYND_REG_ABSENT_SHFT                      0xa
#define HWIO_DEHR_XPU_IDR1_TZXPU_BMSK                                  0x200
#define HWIO_DEHR_XPU_IDR1_TZXPU_SHFT                                    0x9
#define HWIO_DEHR_XPU_IDR1_NVMID_BMSK                                   0xff
#define HWIO_DEHR_XPU_IDR1_NVMID_SHFT                                    0x0

#define HWIO_DEHR_XPU_REV_ADDR                                    (DEHR_XPU_REG_BASE      + 0x0000007c)
#define HWIO_DEHR_XPU_REV_RMSK                                    0xffffffff
#define HWIO_DEHR_XPU_REV_IN          \
        in_dword(HWIO_DEHR_XPU_REV_ADDR)
#define HWIO_DEHR_XPU_REV_INM(m)      \
        in_dword_masked(HWIO_DEHR_XPU_REV_ADDR, m)
#define HWIO_DEHR_XPU_REV_MAJOR_BMSK                              0xf0000000
#define HWIO_DEHR_XPU_REV_MAJOR_SHFT                                    0x1c
#define HWIO_DEHR_XPU_REV_MINOR_BMSK                               0xfff0000
#define HWIO_DEHR_XPU_REV_MINOR_SHFT                                    0x10
#define HWIO_DEHR_XPU_REV_STEP_BMSK                                   0xffff
#define HWIO_DEHR_XPU_REV_STEP_SHFT                                      0x0

#define HWIO_DEHR_XPU_RGn_RACRm_ADDR(n,m)                         (DEHR_XPU_REG_BASE      + 0x00000200 + 0x80 * (n) + 0x4 * (m))
#define HWIO_DEHR_XPU_RGn_RACRm_RMSK                               0x3ff03ff
#define HWIO_DEHR_XPU_RGn_RACRm_MAXn                                       1
#define HWIO_DEHR_XPU_RGn_RACRm_MAXm                                       0
#define HWIO_DEHR_XPU_RGn_RACRm_INI2(n,m)        \
        in_dword_masked(HWIO_DEHR_XPU_RGn_RACRm_ADDR(n,m), HWIO_DEHR_XPU_RGn_RACRm_RMSK)
#define HWIO_DEHR_XPU_RGn_RACRm_INMI2(n,m,mask)    \
        in_dword_masked(HWIO_DEHR_XPU_RGn_RACRm_ADDR(n,m), mask)
#define HWIO_DEHR_XPU_RGn_RACRm_OUTI2(n,m,val)    \
        out_dword(HWIO_DEHR_XPU_RGn_RACRm_ADDR(n,m),val)
#define HWIO_DEHR_XPU_RGn_RACRm_OUTMI2(n,m,mask,val) \
        out_dword_masked_ns(HWIO_DEHR_XPU_RGn_RACRm_ADDR(n,m),mask,val,HWIO_DEHR_XPU_RGn_RACRm_INI2(n,m))
#define HWIO_DEHR_XPU_RGn_RACRm_ROGE_BMSK                          0x2000000
#define HWIO_DEHR_XPU_RGn_RACRm_ROGE_SHFT                               0x19
#define HWIO_DEHR_XPU_RGn_RACRm_ROE_BMSK                           0x1000000
#define HWIO_DEHR_XPU_RGn_RACRm_ROE_SHFT                                0x18
#define HWIO_DEHR_XPU_RGn_RACRm_ROVMID_BMSK                         0xff0000
#define HWIO_DEHR_XPU_RGn_RACRm_ROVMID_SHFT                             0x10
#define HWIO_DEHR_XPU_RGn_RACRm_RWGE_BMSK                              0x200
#define HWIO_DEHR_XPU_RGn_RACRm_RWGE_SHFT                                0x9
#define HWIO_DEHR_XPU_RGn_RACRm_RWE_BMSK                               0x100
#define HWIO_DEHR_XPU_RGn_RACRm_RWE_SHFT                                 0x8
#define HWIO_DEHR_XPU_RGn_RACRm_RWVMID_BMSK                             0xff
#define HWIO_DEHR_XPU_RGn_RACRm_RWVMID_SHFT                              0x0

#define HWIO_DEHR_XPU_RGn_SCR_ADDR(n)                             (DEHR_XPU_REG_BASE      + 0x00000250 + 0x80 * (n))
#define HWIO_DEHR_XPU_RGn_SCR_RMSK                                      0x3f
#define HWIO_DEHR_XPU_RGn_SCR_MAXn                                         1
#define HWIO_DEHR_XPU_RGn_SCR_INI(n)        \
        in_dword_masked(HWIO_DEHR_XPU_RGn_SCR_ADDR(n), HWIO_DEHR_XPU_RGn_SCR_RMSK)
#define HWIO_DEHR_XPU_RGn_SCR_INMI(n,mask)    \
        in_dword_masked(HWIO_DEHR_XPU_RGn_SCR_ADDR(n), mask)
#define HWIO_DEHR_XPU_RGn_SCR_OUTI(n,val)    \
        out_dword(HWIO_DEHR_XPU_RGn_SCR_ADDR(n),val)
#define HWIO_DEHR_XPU_RGn_SCR_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_DEHR_XPU_RGn_SCR_ADDR(n),mask,val,HWIO_DEHR_XPU_RGn_SCR_INI(n))
#define HWIO_DEHR_XPU_RGn_SCR_SCLROE_BMSK                               0x20
#define HWIO_DEHR_XPU_RGn_SCR_SCLROE_SHFT                                0x5
#define HWIO_DEHR_XPU_RGn_SCR_VMIDCLROE_BMSK                            0x10
#define HWIO_DEHR_XPU_RGn_SCR_VMIDCLROE_SHFT                             0x4
#define HWIO_DEHR_XPU_RGn_SCR_MSACLROE_BMSK                              0x8
#define HWIO_DEHR_XPU_RGn_SCR_MSACLROE_SHFT                              0x3
#define HWIO_DEHR_XPU_RGn_SCR_VMIDCLRWE_BMSK                             0x4
#define HWIO_DEHR_XPU_RGn_SCR_VMIDCLRWE_SHFT                             0x2
#define HWIO_DEHR_XPU_RGn_SCR_MSACLRWE_BMSK                              0x2
#define HWIO_DEHR_XPU_RGn_SCR_MSACLRWE_SHFT                              0x1
#define HWIO_DEHR_XPU_RGn_SCR_NS_BMSK                                    0x1
#define HWIO_DEHR_XPU_RGn_SCR_NS_SHFT                                    0x0

#define HWIO_DEHR_XPU_RGn_MCR_ADDR(n)                             (DEHR_XPU_REG_BASE      + 0x00000254 + 0x80 * (n))
#define HWIO_DEHR_XPU_RGn_MCR_RMSK                                      0x3f
#define HWIO_DEHR_XPU_RGn_MCR_MAXn                                         1
#define HWIO_DEHR_XPU_RGn_MCR_INI(n)        \
        in_dword_masked(HWIO_DEHR_XPU_RGn_MCR_ADDR(n), HWIO_DEHR_XPU_RGn_MCR_RMSK)
#define HWIO_DEHR_XPU_RGn_MCR_INMI(n,mask)    \
        in_dword_masked(HWIO_DEHR_XPU_RGn_MCR_ADDR(n), mask)
#define HWIO_DEHR_XPU_RGn_MCR_OUTI(n,val)    \
        out_dword(HWIO_DEHR_XPU_RGn_MCR_ADDR(n),val)
#define HWIO_DEHR_XPU_RGn_MCR_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_DEHR_XPU_RGn_MCR_ADDR(n),mask,val,HWIO_DEHR_XPU_RGn_MCR_INI(n))
#define HWIO_DEHR_XPU_RGn_MCR_MSACLROE_BMSK                             0x20
#define HWIO_DEHR_XPU_RGn_MCR_MSACLROE_SHFT                              0x5
#define HWIO_DEHR_XPU_RGn_MCR_VMIDCLROE_BMSK                            0x10
#define HWIO_DEHR_XPU_RGn_MCR_VMIDCLROE_SHFT                             0x4
#define HWIO_DEHR_XPU_RGn_MCR_SCLROE_BMSK                                0x8
#define HWIO_DEHR_XPU_RGn_MCR_SCLROE_SHFT                                0x3
#define HWIO_DEHR_XPU_RGn_MCR_VMIDCLE_BMSK                               0x4
#define HWIO_DEHR_XPU_RGn_MCR_VMIDCLE_SHFT                               0x2
#define HWIO_DEHR_XPU_RGn_MCR_SCLE_BMSK                                  0x2
#define HWIO_DEHR_XPU_RGn_MCR_SCLE_SHFT                                  0x1
#define HWIO_DEHR_XPU_RGn_MCR_MSAE_BMSK                                  0x1
#define HWIO_DEHR_XPU_RGn_MCR_MSAE_SHFT                                  0x0


#endif /* __HWIO_H__ */
