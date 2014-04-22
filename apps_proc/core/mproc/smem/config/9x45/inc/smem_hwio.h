#ifndef __SMEM_HWIO_H__
#define __SMEM_HWIO_H__

/*===========================================================================

                    Shared Memory Target Specific Header File


 Copyright (c) 2014-2015 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smem/config/9x45/inc/smem_hwio.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/12/15   db      Removed target specific dependencies. This header file
                   brings common HWIO and target specific HWIO under same
                   header file
03/06/15   db      Add HW Mutex support for 9x55. 
04/10/14   rv      Add HW TZ_WONCE REG support for 9x45.
04/02/14   bm      Add HW MUTEEX REG support for 9x45.
06/07/12   rs      Updated from latest HWIO Gen
01/25/12   bt      Updated HWIO macro and base names as of Virtio v2.0.0. 
01/05/12   bt      Initial revision for 8974.
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "smem_hwio_common.h"

/*-------------------------------------------------------------
   RPM Message RAM Memory physical/virt base address and size
 ------------------------------------------------------------- */

#ifdef SMEM_RPM_PROC
/* define RPM message RAM size */
#define SMEM_RPM_MSG_RAM_SIZE        RPM_MSG_RAM_BASE_SIZE
 
/** Defines the RPM Msg RAM starting virtual address 
 * +0x8000 as address in the msmhwiobase.h for RPM is
 * wrong */
#define SMEM_RPM_MSG_RAM_BASE        (RPM_MSG_RAM_BASE)

/** Defines the RPM Msg RAM starting physical address 
 * +0x8000 as address in the msmhwiobase.h for RPM is
 * wrong */
#define SMEM_RPM_MSG_RAM_BASE_PHYS   (RPM_MSG_RAM_BASE_PHYS & 0x0fffffff)

#else  /** For all other peripherals */
/* define RPM message RAM size */
#define SMEM_RPM_MSG_RAM_SIZE        RPM_SS_MSG_RAM_START_ADDRESS_BASE_SIZE

/** Different macro names are being used in msmhwiobase.h on different
 *  procs to export RPM MSG RAM's base/size info */

/** Defines the RPM Msg RAM starting virtual address */
#define SMEM_RPM_MSG_RAM_BASE        RPM_SS_MSG_RAM_START_ADDRESS_BASE

/** Defines the RPM Msg RAM starting physical address */
#define SMEM_RPM_MSG_RAM_BASE_PHYS   RPM_SS_MSG_RAM_START_ADDRESS_BASE_PHYS

#endif

/*----------------------------------------------------------------------------
 * MODULE: Macro to identify if target is using TZ_WONCE_n registers to 
           store SMEM_TARG_INFO address
 *--------------------------------------------------------------------------*/
 #define FEATURE_TZ_WONCE_REG
 
/*----------------------------------------------------------------------------
 * MODULE: TCSR_TCSR_MUTEX
 *--------------------------------------------------------------------------*/
 #define TCSR_TCSR_MUTEX_REG_BASE        (CORE_TOP_CSR_BASE_MUTEX + 0x00005000)
#define TCSR_TCSR_MUTEX_REG_BASE_PHYS   (CORE_TOP_CSR_BASE_PHYS  + 0x00005000)

#define HWIO_TCSR_MUTEX_REG_n_ADDR(n)   (TCSR_TCSR_MUTEX_REG_BASE + \
                                          0x00000000 + 0x1000 * (n))

/*----------------------------------------------------------------------------
 * MODULE: TCSR_TCSR_REGS
 *--------------------------------------------------------------------------*/										  
#if defined(SMEM_BOOT_IMAGE) || defined(SMEM_TZ_IMAGE)
#include "msmhwiobase.h"
#define CORE_TOP_CSR_BASE_TARG_INFO_ADDR  CORE_TOP_CSR_BASE
#else
extern uint8 *smem_targ_info_addr_base_ptr;
#define CORE_TOP_CSR_BASE_TARG_INFO_ADDR  smem_targ_info_addr_base_ptr
#endif										  

/* Needed only for the DAL MapRegion function, on procs that use DAL. */
#define SMEM_TARG_INFO_ADDR_BASE_NAME     ("CORE_TOP_CSR")

#define TCSR_TCSR_REGS_REG_BASE           (CORE_TOP_CSR_BASE_TARG_INFO_ADDR + \
                                            0x00037000)
#define TCSR_TCSR_REGS_REG_BASE_PHYS      (CORE_TOP_CSR_BASE_PHYS + 0x00037000)

#define HWIO_TCSR_TZ_WONCE_n_ADDR(n)      (TCSR_TCSR_REGS_REG_BASE + \
                                            0x00006000 + 0x4 * (n))
#define HWIO_TCSR_TZ_WONCE_n_RMSK                           0xffffffff
#define HWIO_TCSR_TZ_WONCE_n_MAXn                           15
#define HWIO_TCSR_TZ_WONCE_n_INI(n)        \
        in_dword_masked(HWIO_TCSR_TZ_WONCE_n_ADDR(n), HWIO_TCSR_TZ_WONCE_n_RMSK)
#define HWIO_TCSR_TZ_WONCE_n_INMI(n,mask)    \
        in_dword_masked(HWIO_TCSR_TZ_WONCE_n_ADDR(n), mask)
#define HWIO_TCSR_TZ_WONCE_n_OUTI(n,val)    \
        out_dword(HWIO_TCSR_TZ_WONCE_n_ADDR(n),val)
#define HWIO_TCSR_TZ_WONCE_n_OUTMI(n,mask,val) \
        out_dword_masked_ns(HWIO_TCSR_TZ_WONCE_n_ADDR(n),mask,val, \
                            HWIO_TCSR_TZ_WONCE_n_INI(n))
#define HWIO_TCSR_TZ_WONCE_n_TZ_WONCE_ADDRESS_BMSK          0xffffffff
#define HWIO_TCSR_TZ_WONCE_n_TZ_WONCE_ADDRESS_SHFT          0x0

/*-------------------------------------------------------------
   Address where SMEM target info like physical base address and 
   size will be saved by SBL.
 ------------------------------------------------------------- */
/* Index of the TCSR_TZ_WONCE_n register where the address of the SMEM target 
 * info will be saved by SBL. */
#define SMEM_TARG_INFO_REG_BASE_IDX     0

/* Bear family: SMEM Base Address + SMEM Size - SMEM_TOC_SIZE + 
 *              sizeof(smem_toc_type) + 
*              (sizeof(smem_toc_entry_type) * toc->num_entries)
 */
#define SMEM_TARG_INFO_ADDR             \
  (uintptr_t)(HWIO_INI(TCSR_TZ_WONCE_n, SMEM_TARG_INFO_REG_BASE_IDX) | \
    (((uint64)HWIO_INI(TCSR_TZ_WONCE_n, SMEM_TARG_INFO_REG_BASE_IDX+1)) << 32))  
	
#endif /* __SMEM_HWIO_H__ */
