#ifndef SMMUTYPES_H
#define SMMUTYPES_H
/*=========================================================================

                       S M M U  D A L  D R I V E R 

FILE:      SMMUTypes.h

GENERAL DESCRIPTION
  This file contains data structures needed for driver configuration

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS
 
===============================================================================
Copyright (c) 2010 - 2018
Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.tz/2.1.3/kernel/smmu/smmu500/inc/SMMUTypes.h#1 $
$DateTime: 2019/05/23 05:43:45 $
$Author: pwbldsvc $
$Change: 19292165 $

when       who     what, where, why
--------   ---     ---------------------------------------------------------
12/14/10   kcr     Initial Version
==========================================================================*/
#include "com_dtypes.h"

typedef  enum {
   SMMU_ERR_OK = 0,
   SMMU_ERR_HAL_SUCCESS = 0,
   SMMU_ERR_HAL_NO_ACCESS,
   SMMU_ERR_GENERIC_ERROR,
   SMMU_ERR_ALLOC_FAILED,
   SMMU_ERR_FREE_FAILED,
   SMMU_ERR_ALLOC_PHYS_FAILED,
   SMMU_ERR_INVALID_HANDLE,
   SMMU_ERR_INVALID_ID,
   SMMU_ERR_INVALID_VA,
   SMMU_ERR_VA_OVERLAP,
   SMMU_ERR_MAP_FAILED,
   SMMU_ERR_UNMAP_FAILED,
   SMMU_ERR_SMMU_CTX_BOUND,
   SMMU_ERR_INVALID_INDEX,
   SMMU_ERR_INVALID_CACHE_ATTRIBUTE,
   SMMU_ERR_INVALID_PERMISSION_ATTRIBUTE,
   SMMU_ERR_NO_MORE_VA,
   SMMU_ERR_NO_PHYS_MEM,
   SMMU_ERR_ADDRESS_RANGE_MISMATCH,
   SMMU_ERR_DAL_HANDLE_STILL_BOUND_TO_CTX_BANK,
   SMMU_ERR_DAL_HANDLE_NOT_BOUND_TO_CTX_BANK,
   SMMU_ERR_PAGE_TABLES_STILL_BOUND_TO_CTX_BANK,
   SMMU_ERR_LOW_RANGE_PAGE_TABLE_STILL_BOUND_TO_CTX_BANK,
   SMMU_ERR_HIGH_RANGE_PAGE_TABLE_STILL_BOUND_TO_CTX_BANK,
   SMMU_ERR_PAGE_TABLES_ADDRESS_RANGE_OVERLAP,
   SMMU_ERR_PAGE_TABLE_RANGE_INVALID,
   SMMU_ERR_PAGE_TABLE_NOT_BOUND_TO_CTX_BANK,
   SMMU_ERR_PAGE_TABLE_DESCRIPTOR_MEMORY_NOT_FREED,
   SMMU_ERR_SETTING_ATTRIBUTE_ON_UNMAPPED_PAGE,
   SMMU_ERR_L1_PAGE_TABLE_MEMORY_NOT_FREED,
   SMMU_ERR_L1_PAGE_TABLE_FREE_FAILED,
   SMMU_ERR_L1_PAGE_TABLE_MEMORY_DESCRIPTOR_NOT_FREED,
   SMMU_ERR_L2_PAGE_TABLE_MEMORY_NOT_FREED,
   SMMU_ERR_L2_PAGE_TABLE_MEMORY_DESCRIPTOR_NOT_FREED,
   SMMU_ERR_L2_PAGE_TABLES_FREE_FAILED,
   SMMU_ERR_L2_PAGE_TABLE_ALLOC_FAILED,
   SMMU_ERR_PAGE_SIZE_NOT_SUPPORTED,
   SMMU_ERR_ADDRESS_NOT_ALIGNED,
   SMMU_ERR_VIRT_ADDRESS_NOT_ALIGNED,
   SMMU_ERR_PHYS_ADDRESS_NOT_ALIGNED,
   SMMU_ERR_HAL_SET_TTB_FAILED,
   SMMU_ERR_HAL_INVALIDATE_VA_ALL_ASIDS_FAILED,
   SMMU_ERR_HAL_INVALID_VP2XX_FLAG,
   SMMU_ERR_INVALID_CTX_BANK_ID,
   SMMU_ERR_CTX_BANK_HANDLE_ALLOC_FAILED,
   SMMU_ERR_CTX_BANK_HANDLE_FREE_FAILED,
   SMMU_ERR_CTX_BANK_HANDLE_MEMORY_FREE_FAILED,
   SMMU_ERR_CTX_BANK_ATTACH_TO_PT_FAILED,
   SMMU_ERR_CTX_BANK_DETACH_TO_PT_FAILED,
   SMMU_ERR_INVALID_CTX_BANK_PROPERTY,
   SMMU_ERR_INVALID_ASID,
   SMMU_ERR_INVALID_TLB_PREFETCH_FLAG,
   SMMU_ERR_LINK_NOT_FOUND,
   SMMU_ERR_SET_ASID_FAILED,
   SMMU_ERR_PAGETABLE_FORMAT,
   SMMU_ERR_ADRESS_ALIGNMENT,
   SMMU_ERR_NO_REMAP,
   SMMU_ERR_NULL_BUFFER,
   SMMU_ERR_UNSUPPORTED_CONFIG,
   SMMU_ERR_INVALID_UNMAP,
   SMMU_ERR_MEMORY_REGION_NOT_AVAIL,
   SMMU_ERR_L3_PAGE_TABLE_ALLOC_FAILED,
   SMMU_ERR_L3_PAGE_TABLE_FREE_FAILED,
   SMMU_ERR_INIT_CLOCK_FAILED,
   SMMU_ERR_ENABLE_CLOCK_FAILED,
   SMMU_ERR_DISABLE_CLOCK_FAILED,
} SMMUResult_t;

// allow setting whether the SMMU fault serviced by TZ is fatal or not
typedef enum
{
  SMMU_FAULT_ERR_FATAL_ENABLE  = 0, /**< SMMU faults are error fatal. */
  SMMU_FAULT_ERR_FATAL_DISABLE = 1, /**< SMMU faults are not error fatal. */
  //SMMU_FAULT_ERR_FATAL_NOOP    = 2, /**< No operation. */
} smmu_fault_err_fatal_t;

typedef uint32 SMMUAddr_t;

#define SMMU_INVALID_ADDR                  0xFFFFFFFF
#endif /*SMMUTYPES_H*/
