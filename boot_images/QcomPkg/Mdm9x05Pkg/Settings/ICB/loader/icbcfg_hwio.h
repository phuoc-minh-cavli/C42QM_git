#ifndef __ICBCFG_HWIO_H__
#define __ICBCFG_HWIO_H__
/*
===========================================================================
*/
/**
  @file icbcfg_hwio.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    MDM9205 (Twizy) [twizy_v1.0_p3q1r18]
 
  This file contains HWIO register definitions for the following modules:
    OCIMEM_CSR

  'Include' filters applied: OCIMEM_GEN_CTL[OCIMEM_CSR] 

  Generation parameters: 
  { u'filename': u'icbcfg_hwio.h',
    u'header': u'#include "../../../Include/msmhwiobase.h"',
    u'module-filter-exclude': { },
    u'module-filter-include': { u'OCIMEM_CSR': [u'OCIMEM_GEN_CTL']},
    u'modules': [u'OCIMEM_CSR']}
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

  $Header: //components/rel/boot.xf/0.2/QcomPkg/Mdm9x05Pkg/Settings/ICB/loader/icbcfg_hwio.h#2 $
  $DateTime: 2018/09/24 06:08:04 $
  $Author: pwbldsvc $

  ===========================================================================
*/

#include "../../../Include/msmhwiobase.h"

/*----------------------------------------------------------------------------
 * MODULE: OCIMEM_CSR
 *--------------------------------------------------------------------------*/

#define OCIMEM_CSR_REG_BASE                                         (OCIMEM_WRAPPER_CSR_BASE      + 0x00000000)
#define OCIMEM_CSR_REG_BASE_SIZE                                    0x800
#define OCIMEM_CSR_REG_BASE_USED                                    0x68

#define HWIO_OCIMEM_GEN_CTL_ADDR                                    (OCIMEM_CSR_REG_BASE      + 0x00000008)
#define HWIO_OCIMEM_GEN_CTL_RMSK                                      0x1f000f
#define HWIO_OCIMEM_GEN_CTL_IN          \
        in_dword(HWIO_OCIMEM_GEN_CTL_ADDR)
#define HWIO_OCIMEM_GEN_CTL_INM(m)      \
        in_dword_masked(HWIO_OCIMEM_GEN_CTL_ADDR, m)
#define HWIO_OCIMEM_GEN_CTL_OUT(v)      \
        out_dword(HWIO_OCIMEM_GEN_CTL_ADDR,v)
#define HWIO_OCIMEM_GEN_CTL_OUTM(m,v) \
        out_dword_masked_ns(HWIO_OCIMEM_GEN_CTL_ADDR,m,v,HWIO_OCIMEM_GEN_CTL_IN)
#define HWIO_OCIMEM_GEN_CTL_EXMON_GRAN_MASK_BMSK                      0x1f0000
#define HWIO_OCIMEM_GEN_CTL_EXMON_GRAN_MASK_SHFT                          0x10
#define HWIO_OCIMEM_GEN_CTL_CLREXMONREQ_PULSE_EN_BMSK                      0x8
#define HWIO_OCIMEM_GEN_CTL_CLREXMONREQ_PULSE_EN_SHFT                      0x3
#define HWIO_OCIMEM_GEN_CTL_EXMON_GRAN_128B_EN_BMSK                        0x4
#define HWIO_OCIMEM_GEN_CTL_EXMON_GRAN_128B_EN_SHFT                        0x2
#define HWIO_OCIMEM_GEN_CTL_CSR_AHB_BRIDGE_POSTWR_DIS_BMSK                 0x2
#define HWIO_OCIMEM_GEN_CTL_CSR_AHB_BRIDGE_POSTWR_DIS_SHFT                 0x1
#define HWIO_OCIMEM_GEN_CTL_SW_STM_FLAG_BMSK                               0x1
#define HWIO_OCIMEM_GEN_CTL_SW_STM_FLAG_SHFT                               0x0


#endif /* __ICBCFG_HWIO_H__ */
