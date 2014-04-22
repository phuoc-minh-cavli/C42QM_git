/**********************************************************************
 * NandFtlOsal.c
 *
 * Utility tools for Flash Translation layer (FTL).
 * This layer provides utilities tools for FTL core APIs
 *
 *
 * Copyright (c) 2016 QUALCOMM Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *
 **********************************************************************/
/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *
 * when         who     what, where, why
 * ----------   ---     ------------------------------------------------
 * 2016-11-11   svl     Review comments incorporated.
 * 2016-09-14   svl     FTL utility layer support added
 *=======================================================================*/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/BaseLib.h>
#include <Library/CacheMaintenanceLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/QcomBaseLib.h>

#include "NandFtlOsal.h"
#include "NandFtlUtil.h"

/******************************************************************************
 * Data structure definitions
 *****************************************************************************/

//BaseTools/Source/C/Include/common/UefiBaseTypes.h
//typedef UINTN         EFI_TPL;
#define FTL_TPL                   TPL_CALLBACK

static UINTN ftl_sync_lock;

//static qurt_mutex_t flash_ftl_os_lock;

/*****************************************************************************
 * APIs
 *****************************************************************************/

void
flash_ftl_osal_init(void)
{
  return;
}

void
flash_ftl_osal_enter_critical_section(void)
{
  //qurt_mutex_lock(&flash_ftl_os_lock);
  ftl_sync_lock = gBS->RaiseTPL(FTL_TPL);
}

void
flash_ftl_osal_leave_critical_section(void)
{
  //qurt_mutex_unlock(&flash_ftl_os_lock);  
  gBS->RestoreTPL(ftl_sync_lock);
}

void
flash_ftl_osal_deinit(void)
{
  //qurt_mutex_destroy(&flash_ftl_os_lock);
  return;
}
