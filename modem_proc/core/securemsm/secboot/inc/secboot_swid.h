#ifndef _SECBOOT_SWID_H_
#define _SECBOOT_SWID_H_
/*===========================================================================

  Define image sw type

  Copyright (c) 2017 Qualcomm Technologies, Inc.
  All rights reserved.
  Qualcomm Technologies, Inc. Confidential and Proprietary.

===========================================================================*/

/*=========================================================================

                            EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //components/rel/core.mpss/3.10/securemsm/secboot/inc/secboot_swid.h#1 $
$DateTime: 2018/11/08 03:29:06 $
$Author: pwbldsvc $

when       who                  what, where, why
--------   ----                 ------------------------------------------- 
02/13/17    hw                  initial version

===========================================================================*/

/**
@brief List of Software IDs
**/

/* SW Types for each of the subsystems; used during image authentication */

#define SECBOOT_SBL_SW_TYPE         0x0
#define SECBOOT_MBA_SW_TYPE         0x1
#define SECBOOT_MPSS_SW_TYPE        0x2
#define SECBOOT_EHOSTDL_SW_TYPE     0x3
#define SECBOOT_ADSP_SW_TYPE        0x4
#define SECBOOT_QSEE_DEVCFG_SW_TYPE 0x5
#define SECBOOT_TZ_KERNEL_SW_TYPE   0x7
#define SECBOOT_QSEE_SW_TYPE        0x7
#define SECBOOT_APPSBL_SW_TYPE      0x9
#define SECBOOT_RPM_FW_SW_TYPE      0xA
#define SECBOOT_APPS_SEC_SW_TYPE    0xB
#define SECBOOT_TZ_APP_SW_TYPE      0xC
#define SECBOOT_WCNSS_SW_TYPE       0xD
#define SECBOOT_VIDEO_SW_TYPE       0xE
#define SECBOOT_VPU_SW_TYPE         0xF
#define SECBOOT_BCSS_SW_TYPE        0x10
#define SECBOOT_STI_SW_TYPE         0x11
#define SECBOOT_WDT_SW_TYPE         0x12
#define SECBOOT_GPU_UCODE_SW_TYPE   0x14
#define SECBOOT_QHEE_SW_TYPE        0x15
#define SECBOOT_PMIC_SW_TYPE        0x16
#define SECBOOT_CDSP_SW_TYPE        0x17
#define SECBOOT_SLPI_SW_TYPE        0x18
#define SECBOOT_EOSFW_SW_TYPE       0x19
#define SECBOOT_EFSTAR_SW_TYPE      0x1B
#define SECBOOT_ABLFV_SW_TYPE       0x1C
#define SECBOOT_IPA_GSI_SW_TYPE     0x1D
#define SECBOOT_TZ_TEE_SW_TYPE      0x1E
#define SECBOOT_TZ_LIB_SW_TYPE      0x1F
#define SECBOOT_SHRM_SW_TYPE        0x20
#define SECBOOT_AOP_SW_TYPE         0x21
#define SECBOOT_OEM_MISC_SW_TYPE    0x22
#define SECBOOT_QTI_MISC_SW_TYPE    0x23
#define SECBOOT_QUP_SW_TYPE         0x24
#define SECBOOT_XBL_CONFIG_SW_TYPE  0x25
#define SECBOOT_APDP_SW_TYPE        0x200

#define SECBOOT_INVALID_SW_TYPE     (-1)
#define SECBOOT_MAX_SW_TYPE         0x7FFFFFFF
#endif /* _SECBOOT_SWID_H_ */

