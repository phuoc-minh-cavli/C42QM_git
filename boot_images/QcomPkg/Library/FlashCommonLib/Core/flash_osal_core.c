/*====================================================================
 *
 * FILE:        flash_osal_core.c
 *
 * SERVICES:   OS wrapper function implementation for nand driver
 *
 * DESCRIPTION: OS wrapper function implementation for nand driverL
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2017 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * Qualcomm Confidential and Proprietary
 *==================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/boot.xf/0.2/QcomPkg/Library/FlashCommonLib/Core/flash_osal_core.c#1 $ $DateTime: 2018/08/01 23:51:05 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 03/03/17     eo      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/
#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Protocol/LoadedImage.h> 
#include <Library/CacheMaintenanceLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UncachedMemoryAllocationLib.h>
#include <Library/TimerLib.h>
#include <Library/ArmLib.h>
#include <api/services/crc.h>

#include "flash_osal.h"

//BaseTools/Source/C/Include/common/UefiBaseTypes.h
//typedef uint32         EFI_TPL;
#define NAND_TPL                   TPL_CALLBACK

/* memory space marker */
#define DEVICE_SPACE_MAGIC_NUMBER  0xDEADCAFE

/* memory alignment */
#define FLASH_MEM_ALIGN_32         0x20//0x1F

static UINTN nand_sync_lock;

/*
 * 32 bit CRC calculation for flash
 */
uint32 flash_crc32_calc(uint8 *mem, uint16 len, uint32 seed)
{
  return crc_32_calc(mem, len, seed);
}


/*
 * Safe string copy function for flash
 */
uint32 flash_strcpy(uint8 *dest, const uint8 *src, uint32 len)
{
  uint32  status;
  
  //DALSYS_memcpy(dest, src, len);
  status = (uint32)AsciiStrCpyS( (char *)dest, (UINTN)len, (const char *)src);
  dest[len-1] ='\0';
  
  return status;
}

/*
 *  This function does a wait for a desired time in us.
 */
void flash_busy_wait(uint32 wait_time_us)
{
  //DALSYS_BusyWait(wait_time_us);
  //gBS->stall(wait_time_us);
  MicroSecondDelay(wait_time_us);
}

/* This function does cache invalidation on the given address */
void flash_dcache_clean(uintnt addr, uint32 len)
{
  //DALSYS_CacheCommand(DALSYS_CACHE_CMD_CLEAN, (uint32)addr, len);
  WriteBackInvalidateDataCacheRange( (void *)addr, len);
}

/* This function does cache clean on the given address */
void flash_dcache_inval(uintnt addr, uint32 len)
{
  //DALSYS_CacheCommand(DALSYS_CACHE_CMD_INVALIDATE, (uint32)addr, len);
  InvalidateDataCacheRange( (void *)addr, len);
}

uint32 flash_sync_create(void *sync_obj)
{
  NAND_STATUS   status = NAND_DEVICE_DONE;
 
  //DALSYS_SyncCreate();
  return status;
}

/* This function does sync enter */
void flash_sync_enter(void *sync_handle)
{
  //DALSYS_SyncEnter();
  nand_sync_lock = gBS->RaiseTPL(NAND_TPL);
}

/* This function does sync enter */
void flash_sync_leave(void *sync_handle)
{
  //DALSYS_SyncLeave();
  gBS->RestoreTPL(nand_sync_lock);
}

void flash_memset(void *mem, uint8 val, uint32 size)
{
  //DALSYS_memset();
  SetMem(mem, size, val);
}

void flash_memscpy(void *dst, uint32 dst_size, void *src, uint32 src_size)
{
  //DALSYS_memscpy()
  return (void)CopyMemS(dst, dst_size, src, src_size);
}

void flash_memcpy(void *dst, void *src, uint32 size)
{
  //DALSYS_memscpy()
  return (void)CopyMemS(dst, size, src, size);
}

uint32 flash_malloc(uint32 size, void **mem)
{
  NAND_STATUS     status = NAND_DEVICE_DONE;

  //DALSYS_Malloc();
  if (EFI_SUCCESS != gBS->AllocatePool(EfiBootServicesData,size,mem))
  {
    status = NAND_DEVICE_FAIL;
    DEBUG((EFI_D_INFO, "Allocate memory of size 0x%x	   [FAILED]\n",size));
  }
  return status;
}

void flash_free(void *mem)
{
  //DALSYS_Free();
  gBS->FreePool(mem);
}

uint32 flash_memregion_alloc(struct flash_osal_meminfo *meminfo, uint32 size)
{
  NAND_STATUS  status = NAND_DEVICE_DONE;
  
  //DALSYS_MemregionAlloc();
  if (meminfo == NULL)
  {
    status = NAND_DEVICE_FAIL;
    return status;
  }

  /* in UEFI for device space VA:PA mapping is 1:1 and it's pre-mapped */
  /* if the physical address is given: indicates that we want to map an
   * address in the device space
   */
  if (meminfo->physical_addr)
  {
    meminfo->virtual_addr = meminfo->physical_addr;
    meminfo->size = size;
    meminfo->handle = (void*)DEVICE_SPACE_MAGIC_NUMBER ;
    return status;
  }
  meminfo->handle = UncachedAllocateAlignedPool(size, FLASH_MEM_ALIGN_32);
  
  if (NULL == meminfo->handle )
  {
    DEBUG((EFI_D_INFO, "Failed: Allocate memory of size 0x%x\n",size));
	status = NAND_DEVICE_FAIL;
    return status;
  }
  
  meminfo->size          = size;
  meminfo->physical_addr = ConvertToPhysicalAddress(meminfo->handle);
  meminfo->virtual_addr  = (UINTN)meminfo->handle;
  
  return status;
}


uint32 flash_free_memregion(struct flash_osal_meminfo *meminfo)
{
  NAND_STATUS	status = NAND_DEVICE_DONE;
  
  //DALSYS_DestroyObject();
  if (meminfo == NULL)
  {
    status = NAND_DEVICE_FAIL;
    return status;
  }  
  if (meminfo->handle && (meminfo->handle != (void*)DEVICE_SPACE_MAGIC_NUMBER))
  {
    UncachedFreePool(meminfo->handle);
  }
  meminfo->physical_addr = 0;
  meminfo->virtual_addr = 0;
  meminfo->size = 0;
  meminfo->handle = 0;
  
  return status;
}
