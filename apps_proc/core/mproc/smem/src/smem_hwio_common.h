#ifndef __SMEM_HWIO_COMMON_H__
#define __SMEM_HWIO_COMMON_H__
/*=========================================================================*/
/**
  @file smem_hwio_common.h
  @brief Common HWIO used in all targets.

*/
/*===========================================================================

  Copyright (c) 2014-2015 Qualcomm Technologies Incorporated.
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

=============================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smem/src/smem_hwio_common.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/11/15   db      Initial version
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#include <stdint.h>
#include "msmhwiobase.h"
#include "HALhwio.h"

/*===========================================================================
                           MACRO DEFINITIONS
===========================================================================*/
#ifdef CORE_TOP_CSR_BASE
#undef CORE_TOP_CSR_BASE
#endif
extern uint8 *smem_hw_mutex_base_ptr;
#define CORE_TOP_CSR_BASE_MUTEX                     smem_hw_mutex_base_ptr
#define smem_hw_mutex_reg_base_phys                 CORE_TOP_CSR_BASE_PHYS
#define smem_hw_mutex_reg_base_size                 CORE_TOP_CSR_BASE_SIZE

/* Needed only for the DAL MapRegion function, on procs that use DAL. */
#define SMEM_HW_MUTEX_BASE_NAME ("CORE_TOP_CSR")

/*----------------------------------------------------------------------------
 * MODULE: TCSR_TCSR_MUTEX
 *--------------------------------------------------------------------------*/
#define HWIO_TCSR_MUTEX_REG_n_RMSK                      0xff
#define HWIO_TCSR_MUTEX_REG_n_MAXn                        31
#define HWIO_TCSR_MUTEX_REG_n_INI(n)        \
        in_dword_masked(HWIO_TCSR_MUTEX_REG_n_ADDR(n), \
                        HWIO_TCSR_MUTEX_REG_n_RMSK)
#define HWIO_TCSR_MUTEX_REG_n_INMI(n,mask)    \
        in_dword_masked(HWIO_TCSR_MUTEX_REG_n_ADDR(n), mask)
#define HWIO_TCSR_MUTEX_REG_n_OUTI(n,val)    \
        out_dword(HWIO_TCSR_MUTEX_REG_n_ADDR(n),val)
#define HWIO_TCSR_MUTEX_REG_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TCSR_MUTEX_REG_n_ADDR(n),mask,val, \
          HWIO_TCSR_MUTEX_REG_n_INI(n))
#define HWIO_TCSR_MUTEX_REG_n_MUTEX_BMSK                0xff
#define HWIO_TCSR_MUTEX_REG_n_MUTEX_SHFT                 0x0

#endif /* __SMEM_HWIO_COMMON_H__ */
