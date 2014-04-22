#ifndef TARGACFNAAGZA_H
#define TARGACFNAAGZA_H
/* ========================================================================
FILE: TARGACFNAAGZA

Copyright (c) 2023 by Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.        
=========================================================================== */

#define T_MDM9X15
#define FEATURE_RDEVMAP_DS_DEFAULT_TO_USB
#define FEATURE_NEW_SLEEP_API
#define T_SURF
#define FEATURE_PMEM
#define TCM_RAM_BASE 0x80000000
#define TCM_RAM_SIZE 0x0001000
#define DDR_BASE_PHYS 0x80000000
#define APSS_DDR_OFFSET 0x12C0000 
#define APSS_USER_TZAPSS_SIZE 0x00E0000
#define APSS_USER_DAMS_SIZE 0x0300000
#define APSS_DDR_END 0x81E22000
#define L2_PGTABLE_POOL_SIZE 0x0040000 
#define QURT_MEM_RGN_SIZE 0x0080000
#define AMSS_HEAP_SIZE 0 
#define ACDB_RAM_BASE_PHYS 0x81E22000
#define ACDB_RAM_BASE_VIRT 0x81E22000
#define ACDB_RAM_SIZE 0x001F400
#define SMEM_DDR_OFFSET 0x1E52000
#define APSS_KERNEL_DDR_SIZE (APSS_TOTAL_DDR_SIZE - (APSS_USER_TZAPSS_SIZE + APSS_USER_DAMS_SIZE))
#define APSS_USER_DDR_SIZE (APSS_TOTAL_DDR_SIZE - APSS_KERNEL_DDR_SIZE)
#define APSS_DDR_BASE (DDR_BASE_PHYS + APSS_DDR_OFFSET)
#define TZ_APPS_START (APSS_DDR_END - APSS_USER_TZAPSS_SIZE)
#define TZ_APPS_END (APSS_DDR_END - 0x1)
#define OEM_POOL_END TZ_APPS_START
#define OEM_POOL_START (APSS_DDR_BASE + APSS_KERNEL_DDR_SIZE)
#define OEM_POOL_SIZE (TZ_APPS_START - OEM_POOL_START)
#define QURT_MEM_RGN_END (APSS_DDR_BASE + APSS_KERNEL_DDR_SIZE) 
#define QURT_MEM_RGN_BASE (QURT_MEM_RGN_END - QURT_MEM_RGN_SIZE)
#define SCL_SHARED_RAM_BASE_PHYS (DDR_BASE_PHYS + SMEM_DDR_OFFSET)
#define SCL_SHARED_RAM_BASE SCL_SHARED_RAM_BASE_PHYS
#define SCL_SHARED_RAM_SIZE 0x6E000
#define SYSDBG_MAGIC_NUMBER 0x42445953
#define SCL_OCIMEM_BASE_PHYS 0x08600000
#define SCL_SHARED_IMEM_OFFSET 0x28000
#define SCL_OCIMEM_BASE SCL_OCIMEM_BASE_PHYS
#define SCL_SHARED_IMEM_BASE (SCL_OCIMEM_BASE_PHYS + SCL_SHARED_IMEM_OFFSET)
#define SCL_APPS_KERNEL_BASE APSS_DDR_BASE
#define SCL_APPS_TOTAL_SIZE APSS_KERNEL_DDR_SIZE
#define TX_SHARED_MEM_SIZE 0x20000
#define TX_SHARED_MEM_START (OEM_POOL_END - TX_SHARED_MEM_SIZE)
#define APSS_TOTAL_DDR_SIZE (APSS_DDR_END - APSS_DDR_BASE)
#define XBL_CRASHDUMP_IMG_BASE (DDR_BASE_PHYS + 0x1500000) 
#define XBL_CRASHDUMP_IMG_SIZE 0x0080000
#define SCL_APPS_KERNEL_SIZE SCL_APPS_TOTAL_SIZE 
#define SCL_APPS_SERIAL_DEV_BASE (SCL_APPS_KERNEL_BASE + SCL_APPS_KERNEL_SIZE) 
#define SCL_APPS_SERIAL_DEV_SIZE 0x0 
#define SCL_APPS_CODE_USER_BASE (SCL_APPS_KERNEL_BASE) 
#define SCL_APPS_CODE_USER_SIZE (APSS_KERNEL_DDR_SIZE) 
#define MPSS_PIL_META_DATA_BASE 0 
#define MPSS_PIL_META_DATA_SIZE 0 
#define MPSS_MBA_RAM_BASE 0 
#define MPSS_MBA_RAM_SIZE 0 
#define MPSS_RAM_SIZE 0
#define MPSS_RAM_BASE 0
#define QSEECOM_POOL_START 0
#define QSEECOM_POOL_SIZE 0




#endif /* TARGACFNAAGZA_H */
