#ifndef __HALSYSMHWIO_H__
#define __HALSYSMHWIO_H__
/*
===========================================================================
*/
/**
  @file HALsysmHWIO.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    MDM9x55 (McLaren) [mclaren_v1.0_p3q2r11.0]
 
  This file contains HWIO register definitions for the following modules:
    TLMM_CSR

  'Include' filters applied: TLMM_GPIO_IN_OUT[TLMM_CSR] 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2015 Qualcomm Technologies Incorporated.
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

  $Header: //components/rel/core.tx/6.0/debugtools/sys_m/src/lib/9x06/apps/HALsysmHWIO.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: TLMM_CSR
 *--------------------------------------------------------------------------*/

#define TLMM_CSR_REG_BASE                                                                   (TLMM_BASE      + 0x00000000)

#define HWIO_TLMM_GPIO_IN_OUTn_ADDR(n)                                                      (TLMM_CSR_REG_BASE      + 0x00000004 + 0x1000 * (n))
#define HWIO_TLMM_GPIO_IN_OUTn_RMSK                                                                0x3
#define HWIO_TLMM_GPIO_IN_OUTn_MAXn                                                                 99
#define HWIO_TLMM_GPIO_IN_OUTn_INI(n)        \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUTn_ADDR(n), HWIO_TLMM_GPIO_IN_OUTn_RMSK)
#define HWIO_TLMM_GPIO_IN_OUTn_INMI(n,mask)    \
        in_dword_masked(HWIO_TLMM_GPIO_IN_OUTn_ADDR(n), mask)
#define HWIO_TLMM_GPIO_IN_OUTn_OUTI(n,val)    \
        out_dword(HWIO_TLMM_GPIO_IN_OUTn_ADDR(n),val)
#define HWIO_TLMM_GPIO_IN_OUTn_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TLMM_GPIO_IN_OUTn_ADDR(n),mask,val,HWIO_TLMM_GPIO_IN_OUTn_INI(n))
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_OUT_BMSK                                                       0x2
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_OUT_SHFT                                                       0x1
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_IN_BMSK                                                        0x1
#define HWIO_TLMM_GPIO_IN_OUTn_GPIO_IN_SHFT                                                        0x0


#endif /* __HALSYSMHWIO_H__ */
