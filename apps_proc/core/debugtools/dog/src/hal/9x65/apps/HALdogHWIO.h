#ifndef __HALDOGHWIO_H__
#define __HALDOGHWIO_H__
/*
===========================================================================
*/
/**
  @file HALdogHWIO.h
  @brief Auto-generated HWIO interface include file.

  Reference chip release:
    MDM9x65 (Stingray) [stingray_v1.0_p2q1r10]
 
  This file contains HWIO register definitions for the following modules:
    APCS_WDT

  'Include' filters applied: 
  'Exclude' filters applied: RESERVED DUMMY 
*/
/*
  ===========================================================================

  Copyright (c) 2016 Qualcomm Technologies Incorporated.
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

  $Header: //components/rel/core.tx/6.0/debugtools/dog/src/hal/9x65/apps/HALdogHWIO.h#1 $
  $DateTime: 2020/01/30 22:49:35 $
  $Author: pwbldsvc $

  ===========================================================================
*/

/*----------------------------------------------------------------------------
 * MODULE: APCS_WDT
 *--------------------------------------------------------------------------*/

#define APCS_WDT_REG_BASE                                    (A7SS_BASE      + 0x00017000)

#define HWIO_WDOG_SECURE_ADDR                                (APCS_WDT_REG_BASE      + 0x00000000)
#define HWIO_WDOG_SECURE_RMSK                                       0x1
#define HWIO_WDOG_SECURE_IN          \
        in_dword_masked(HWIO_WDOG_SECURE_ADDR, HWIO_WDOG_SECURE_RMSK)
#define HWIO_WDOG_SECURE_INM(m)      \
        in_dword_masked(HWIO_WDOG_SECURE_ADDR, m)
#define HWIO_WDOG_SECURE_OUT(v)      \
        out_dword(HWIO_WDOG_SECURE_ADDR,v)
#define HWIO_WDOG_SECURE_OUTM(m,v) \
        out_dword_masked_ns(HWIO_WDOG_SECURE_ADDR,m,v,HWIO_WDOG_SECURE_IN)
#define HWIO_WDOG_SECURE_WDT_BMSK                                   0x1
#define HWIO_WDOG_SECURE_WDT_SHFT                                   0x0

#define HWIO_WDOG_RESET_ADDR                                 (APCS_WDT_REG_BASE      + 0x00000004)
#define HWIO_WDOG_RESET_RMSK                                        0x1
#define HWIO_WDOG_RESET_OUT(v)      \
        out_dword(HWIO_WDOG_RESET_ADDR,v)
#define HWIO_WDOG_RESET_RESET_BMSK                                  0x1
#define HWIO_WDOG_RESET_RESET_SHFT                                  0x0

#define HWIO_WDOG_CTL_ADDR                                   (APCS_WDT_REG_BASE      + 0x00000008)
#define HWIO_WDOG_CTL_RMSK                                          0x3
#define HWIO_WDOG_CTL_OUT(v)      \
        out_dword(HWIO_WDOG_CTL_ADDR,v)
#define HWIO_WDOG_CTL_UNMASKED_INT_ENABLE_BMSK                      0x2
#define HWIO_WDOG_CTL_UNMASKED_INT_ENABLE_SHFT                      0x1
#define HWIO_WDOG_CTL_ENABLE_BMSK                                   0x1
#define HWIO_WDOG_CTL_ENABLE_SHFT                                   0x0

#define HWIO_WDOG_STATUS_ADDR                                (APCS_WDT_REG_BASE      + 0x0000000c)
#define HWIO_WDOG_STATUS_RMSK                                0x7fffffff
#define HWIO_WDOG_STATUS_IN          \
        in_dword_masked(HWIO_WDOG_STATUS_ADDR, HWIO_WDOG_STATUS_RMSK)
#define HWIO_WDOG_STATUS_INM(m)      \
        in_dword_masked(HWIO_WDOG_STATUS_ADDR, m)
#define HWIO_WDOG_STATUS_COUNT_BMSK                          0x7ffffffc
#define HWIO_WDOG_STATUS_COUNT_SHFT                                 0x2
#define HWIO_WDOG_STATUS_FROZEN_BMSK                                0x2
#define HWIO_WDOG_STATUS_FROZEN_SHFT                                0x1
#define HWIO_WDOG_STATUS_EXPIRED_STATUS_BMSK                        0x1
#define HWIO_WDOG_STATUS_EXPIRED_STATUS_SHFT                        0x0

#define HWIO_WDOG_BARK_TIME_ADDR                             (APCS_WDT_REG_BASE      + 0x00000010)
#define HWIO_WDOG_BARK_TIME_RMSK                                0xfffff
#define HWIO_WDOG_BARK_TIME_IN          \
        in_dword_masked(HWIO_WDOG_BARK_TIME_ADDR, HWIO_WDOG_BARK_TIME_RMSK)
#define HWIO_WDOG_BARK_TIME_INM(m)      \
        in_dword_masked(HWIO_WDOG_BARK_TIME_ADDR, m)
#define HWIO_WDOG_BARK_TIME_OUT(v)      \
        out_dword(HWIO_WDOG_BARK_TIME_ADDR,v)
#define HWIO_WDOG_BARK_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_WDOG_BARK_TIME_ADDR,m,v,HWIO_WDOG_BARK_TIME_IN)
#define HWIO_WDOG_BARK_TIME_DATA_BMSK                           0xfffff
#define HWIO_WDOG_BARK_TIME_DATA_SHFT                               0x0

#define HWIO_WDOG_BITE_TIME_ADDR                             (APCS_WDT_REG_BASE      + 0x00000014)
#define HWIO_WDOG_BITE_TIME_RMSK                                0xfffff
#define HWIO_WDOG_BITE_TIME_IN          \
        in_dword_masked(HWIO_WDOG_BITE_TIME_ADDR, HWIO_WDOG_BITE_TIME_RMSK)
#define HWIO_WDOG_BITE_TIME_INM(m)      \
        in_dword_masked(HWIO_WDOG_BITE_TIME_ADDR, m)
#define HWIO_WDOG_BITE_TIME_OUT(v)      \
        out_dword(HWIO_WDOG_BITE_TIME_ADDR,v)
#define HWIO_WDOG_BITE_TIME_OUTM(m,v) \
        out_dword_masked_ns(HWIO_WDOG_BITE_TIME_ADDR,m,v,HWIO_WDOG_BITE_TIME_IN)
#define HWIO_WDOG_BITE_TIME_DATA_BMSK                           0xfffff
#define HWIO_WDOG_BITE_TIME_DATA_SHFT                               0x0

#define HWIO_WDOG_TEST_LOAD_STATUS_ADDR                      (APCS_WDT_REG_BASE      + 0x00000018)
#define HWIO_WDOG_TEST_LOAD_STATUS_RMSK                             0x1
#define HWIO_WDOG_TEST_LOAD_STATUS_IN          \
        in_dword_masked(HWIO_WDOG_TEST_LOAD_STATUS_ADDR, HWIO_WDOG_TEST_LOAD_STATUS_RMSK)
#define HWIO_WDOG_TEST_LOAD_STATUS_INM(m)      \
        in_dword_masked(HWIO_WDOG_TEST_LOAD_STATUS_ADDR, m)
#define HWIO_WDOG_TEST_LOAD_STATUS_SYNC_STATUS_BMSK                 0x1
#define HWIO_WDOG_TEST_LOAD_STATUS_SYNC_STATUS_SHFT                 0x0

#define HWIO_WDOG_TEST_LOAD_ADDR                             (APCS_WDT_REG_BASE      + 0x0000001c)
#define HWIO_WDOG_TEST_LOAD_RMSK                                    0x1
#define HWIO_WDOG_TEST_LOAD_OUT(v)      \
        out_dword(HWIO_WDOG_TEST_LOAD_ADDR,v)
#define HWIO_WDOG_TEST_LOAD_LOAD_BMSK                               0x1
#define HWIO_WDOG_TEST_LOAD_LOAD_SHFT                               0x0

#define HWIO_WDOG_TEST_ADDR                                  (APCS_WDT_REG_BASE      + 0x00000020)
#define HWIO_WDOG_TEST_RMSK                                     0xfffff
#define HWIO_WDOG_TEST_IN          \
        in_dword_masked(HWIO_WDOG_TEST_ADDR, HWIO_WDOG_TEST_RMSK)
#define HWIO_WDOG_TEST_INM(m)      \
        in_dword_masked(HWIO_WDOG_TEST_ADDR, m)
#define HWIO_WDOG_TEST_OUT(v)      \
        out_dword(HWIO_WDOG_TEST_ADDR,v)
#define HWIO_WDOG_TEST_OUTM(m,v) \
        out_dword_masked_ns(HWIO_WDOG_TEST_ADDR,m,v,HWIO_WDOG_TEST_IN)
#define HWIO_WDOG_TEST_LOAD_VALUE_BMSK                          0xfffff
#define HWIO_WDOG_TEST_LOAD_VALUE_SHFT                              0x0


#endif /* __HALDOGHWIO_H__ */
