#ifndef CUSTACFNAADZA_H
#define CUSTACFNAADZA_H
/* ========================================================================
FILE: CUSTACFNAADZA

Copyright (c) 2023 by Qualcomm Technologies, Inc.  All Rights Reserved.
Qualcomm Technologies Proprietary and Confidential.        
=========================================================================== */

#ifndef TARGACFNAADZA_H
   #include "targacfnaadza.h"
#endif

#define FEATURE_EFS2
#define FEATURE_EFS_OSAL_IS_QURT
#define FS_OP_COMPLETE_SIG 0x40000000
#define EFS_DIAG_USE_THIS_SUBSYS_ID 62
#define FLASH_ENABLE_NAND_SPI
#define FS_MAX_PAGES_PER_BLOCK 64
#define EFS_MAX_FILE_DESCRIPTORS 10
#define FS_PATH_MAX 256
#define FS_NAME_MAX 256
#define EFS_PAGE_SIZE 2048
#define FS_API_DEBUG_DISABLE
#define FEATURE_DATA_MM
#define IMAGE_KEY_ACDB_SIZE ACDB_RAM_SIZE
#define IMAGE_KEY_ACDB_IMG_DEST_ADDR ACDB_RAM_BASE_PHYS
#define FEATURE_AUDIO_CSD_QMI_SERVICE
#define FEATURE_APSS_AUDIO_INIT
#define FEATURE_PSM_TARGET_9205
#define FEATURE_PSM_USE_NEW_PMIC_RTC_API
#define FEATURE_FLASH_ALIGN_DEST_BUFFER
#define FEATURE_FLASH_MPU
#define FEATURE_STANDALONE
#define DOG_KEEPALIVE_100MS_NUM_SLOTS 2
#define FEATURE_ERROR_MESSAGE_STANDALONE
#define FEATURE_SIO_RUMI
#define FEATURE_FIRST_UART 
#define FEATURE_SECOND_UART 
#define FEATURE_USES_FIRST_UART
#define FEATURE_DATA_MUX 
#define FEATURE_USES_SECOND_UART
#define FEATURE_FIRST_UART_ON_ANY_PROC
#define FEATURE_SECOND_UART_ON_ANY_PROC 
#define FEATURE_USE_UART1DM 
#define FEATURE_DMOV_HS_UART_ON_APPS
#define FEATURE_NO_REX
#define FEATURE_DAL
#define FEATURE_DMOV
#define FEATURE_DIAG_FS_ACCESS_VALIDATION
#define FEATURE_MEMHEAP_MT
#define FEATURE_MEM_DEBUG
#define FEATURE_I2C
#define FEATURE_I2C_IO_CTRL
#define FEATURE_MULTIPROCESSOR 
#define FEATURE_CMI
#define FEATURE_USB_CDC_ACM
#define FEATURE_USB_DIAG
#define FEATURE_USB_DIAG_NMEA
#define FEATURE_NO_DB
#define FEATURE_AUTH_DIGITS
#define FEATURE_RCINIT
#define AMSS_PD
#define SEC_PD
#define FEATURE_CDSMS
#define FEATURE_APP_TEST_AUTOMATION
#define FEATURE_FTM_HWTC
#define FEATURE_PBM_USE_EFS_PB
#define FEAURE_CMI_MM
#define FEATURE_REX_DPC
#define FEATURE_BIO_INIT_DURING_BOOT
#define FEATURE_STANDALONE_APPS
#define FEATURE_ESTK
#define FEATURE_ESTK_CM_VOICE_CALL_INTERFACE
#define FEATURE_ESTK_CM_SUPPS_INTERFACE
#define FEATURE_ESTK_WMS_INTERFACE
#define FEATURE_DIAG_8K_QDSP6
#define FEATURE_ERR_HAS_F3_TRACE
#define FEATURE_ERR_SERVICES
#define FEATURE_DLOAD_HW_RESET_DETECT
#define FEATURE_DOG_BARK
#define FEATURE_SYS_M
#define ELAN_A5SS_BASE 0
#define FEATURE_QURT
#define FEATURE_SMEM
#define FEATURE_SMSM
#define FEATURE_SMEM_LOG
#define FEATURE_SMD
#define FEATURE_DSM_QURT
#define FEATURE_DSM_DUP_ITEMS 
#define FEATURE_DSM_LARGE_ITEMS
#define FEATURE_DSM_DYNAMIC_POOL_SELECTION
#define DSMI_DS_SMALL_ITEM_CNT 100
#define DSMI_DS_LARGE_ITEM_CNT 100
#define DSMI_DS_LARGE_ITEM_SIZ 1024
#define DSMI_DUP_ITEM_CNT 100
#define FEATURE_DSM_ALIGN_64
#define FEATURE_SPINLOCK
#define FEATURE_ARM_ASM_SPINLOCK
#define FEATURE_IPC_ROUTER
#define FEATURE_QCCI
#define FEATURE_QCSI
#define FEATURE_SLEEP
#define FEATURE_RPM_INIT_CALL_FROM_TMC
#define FEATURE_APPS_TCXO_SLEEP
#define FEATURE_POWER_COLLAPSE
#define FEATURE_APPS_POWER_COLLAPSE
#define FEATURE_TIME_POWER_COLLAPSE
#define FEATURE_CORECPUINIT_CALL_FROM_TMC
#define FEATURE_DCVSINIT_CALL_FROM_TMC
#define DCVS_BOOT_UP_DELAY_TIME_MS 6000
#define FEATURE_USE_TIME_VU
#define FEATURE_GENOFF_STANDALONE
#define FEATURE_TIMER_TASK
#define FEATURE_ERR_HAS_F3_TRACE
#define FEATURE_ERR_EXTENDED_STORE
#define FEATURE_SAVE_DEBUG_TRACE
#define ERR_MASTER
#define FEATURE_AUTO_DLOAD_ON_ERROR
#define ERR_DEFAULT_ACTION ERR_DLOAD
#define FEATURE_DIAG_FS_ACCESS_VALIDATION
#define DEVCFG_DATA_SEG_SIZE 0x18000
#define FEATURE_MMU_ENABLED


#include "custiguana.h"
#include "custtarget.h"
#include "custsurf.h"
#include "custdiag.h"
#include "custdmss.h"
#include "custrex.h"
#include "custsio_9x15.h"
#include "custhsusb_9x35.h"
#include "custdrivers.h"
#include "custsec.h"
#include "custfta.h"

#ifdef FEATURE_NV
   #undef FEATURE_NV
#endif
#ifdef FEATURE_SENSORS
   #undef FEATURE_SENSORS
#endif
#ifdef FEATURE_EXPORT_WLAN_RAPI
   #undef FEATURE_EXPORT_WLAN_RAPI
#endif
#ifdef FEATURE_HS_USB_OTG
   #undef FEATURE_HS_USB_OTG
#endif
#ifdef FEATURE_HSU_MTP
   #undef FEATURE_HSU_MTP
#endif
#ifdef FEATURE_PICTBRIDGE_HS
   #undef FEATURE_PICTBRIDGE_HS
#endif
#ifdef FEATURE_EFS_COMPILE_WITHOUT_DIAG
   #undef FEATURE_EFS_COMPILE_WITHOUT_DIAG
#endif
#ifdef FEATURE_QCLI_SOURCE_SHIP
   #undef FEATURE_QCLI_SOURCE_SHIP
#endif
#ifdef FEATURE_NO_HOLDOFF_TIME
   #undef FEATURE_NO_HOLDOFF_TIME
#endif

#endif /* CUSTACFNAADZA_H */
