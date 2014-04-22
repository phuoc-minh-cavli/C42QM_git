/**********************************************************************
 * NandFtlUtil.c
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
 * 2016-12-02   svl     new malloc/free func to provide qurt region for flash
                        operations
 * 2016-11-11   svl     Review comments incorporated.
 * 2016-09-14   svl     FTL utility layer support added
 *=======================================================================*/

#include <Uefi.h>
#include <Library/PcdLib.h>
#include <Library/BaseLib.h>
#include <Library/DebugLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/BaseMemoryLib.h>
#include <Library/MemoryAllocationLib.h>
#include <Library/QcomBaseLib.h>
#include <Library/UefiBootServicesTableLib.h>
#include <Library/UncachedMemoryAllocationLib.h>

#include "NandFtlUtil.h"

#define FLASH_FTL_MEM_ADDR_NOT_SPECIFIED  (0xFFFFFFFF)

/* memory alignment */
#define FLASH_MEM_ALIGN_32                (0x10)

typedef struct flash_ftl_meminfo
{
  UINTN    physical_addr;   /* Physical address of the memory */
  UINTN    virtual_addr;    /* Virtual address of the memory */
  uint32   size;            /* Size of the memory */
  uint32   props;           /* memory properties */
  void     *handle;          /* Target specific handle to the memory (reserved) */
}flash_ftl_meminfo_t;

typedef struct
{
  UINTN               virt_addr;
  flash_ftl_meminfo_t *mem_hndl;
}flash_ftl_aligned_malloc_t;

static flash_ftl_aligned_malloc_t
  flash_ftl_aligned_malloc_pool[FLASH_FTL_ALIGNED_MALLOC_CALL_ACCESS_LIMIT];

/**
  flash_ftl_aligned_malloc - allocate cpu aligned memory of size bytes and
  returns a pointer to it.
*/
void*
flash_ftl_aligned_malloc(uint32 size)
{
  flash_ftl_meminfo_t mem_info;
  uint32 i = 0;

  if(0 == size)
  {
    return NULL;
  }

  for(i = 0; i < FLASH_FTL_ALIGNED_MALLOC_CALL_ACCESS_LIMIT; i++)
  {
    if(flash_ftl_aligned_malloc_pool[i].mem_hndl == NULL)
    {
      break;
    }
  }

  if(FLASH_FTL_ALIGNED_MALLOC_CALL_ACCESS_LIMIT == i)
  {
    if((flash_ftl_aligned_malloc_pool[i-1].mem_hndl != NULL))
    {
      return (void *)flash_ftl_aligned_malloc_pool[i-1].virt_addr;
    }
    return NULL;
  }

  
  //mem_info.handle = FLASH_FTL_MEM_ADDR_NOT_SPECIFIED;
  mem_info.handle = NULL;
  mem_info.size   = 0;
  mem_info.virtual_addr  = FLASH_FTL_MEM_ADDR_NOT_SPECIFIED;
  mem_info.physical_addr = FLASH_FTL_MEM_ADDR_NOT_SPECIFIED;

  mem_info.handle = UncachedAllocateAlignedPool(size, FLASH_MEM_ALIGN_32);
  if (NULL == mem_info.handle )
  {
    DEBUG((EFI_D_INFO, "Failed: Allocate memory of size 0x%x\n",size));
    return NULL;
  }

  /* If the allocation is success, get the memory address information */
  mem_info.size          = size;
  mem_info.physical_addr = ConvertToPhysicalAddress(mem_info.handle);
  mem_info.virtual_addr  = (UINTN)mem_info.handle;

  /* save it for future "free" operation */
  flash_ftl_aligned_malloc_pool[i].mem_hndl  = mem_info.handle;
  flash_ftl_aligned_malloc_pool[i].virt_addr = mem_info.virtual_addr;

  return (void*)mem_info.virtual_addr;
}

/**
  Frees memory allocated using flash_ftl_aligned_malloc
*/
void
flash_ftl_aligned_free(void *ptr)
{
  uint8 i = 0;
  for(i = 0; i < FLASH_FTL_ALIGNED_MALLOC_CALL_ACCESS_LIMIT; i++)
  {
    if((UINTN)ptr == flash_ftl_aligned_malloc_pool[i].virt_addr)
    {
      UncachedFreePool(flash_ftl_aligned_malloc_pool[i].mem_hndl->handle);
	  flash_ftl_aligned_malloc_pool[i].mem_hndl->physical_addr = 0;
	  flash_ftl_aligned_malloc_pool[i].mem_hndl->virtual_addr = 0;
	  flash_ftl_aligned_malloc_pool[i].mem_hndl->size = 0;
	  flash_ftl_aligned_malloc_pool[i].mem_hndl->props = 0;
      flash_ftl_aligned_malloc_pool[i].mem_hndl = NULL;
      flash_ftl_aligned_malloc_pool[i].virt_addr = 0;
      break;
    }
  }
}

/**
  flash_ftl_malloc - allocate the size bytes and
  return a pointer to it.
*/
void*
flash_ftl_malloc(uint32 size)
{
  void *mem = NULL;

  if (EFI_SUCCESS != gBS->AllocatePool(EfiBootServicesData,size,(VOID**)&mem))
  {
    DEBUG((EFI_D_INFO, "Allocate memory of size 0x%x	   [FAILED]\n",size));
    return NULL;
  }
  return mem;
}

/**
  Frees memory allocated using flash_ftl_malloc
*/
void
flash_ftl_free(void *ptr)
{
  gBS->FreePool(ptr);
}

/**
  flash_ftl_memscpy - Size bounded memory copy.
  Copies the source string to the destination buffer.
  This function ensures to copy only a maximum of
  destination buffer bytes.
*/
FLASH_FTL_STATUS
flash_ftl_memscpy(      void *dst, uint32 dst_size,
                  const void *src, uint32 src_size)
{
  uint32 copied_bytes = 0;
  if((NULL == dst) || (NULL == src) || (dst_size < src_size))
  {
    return FLASH_FTL_INVALID_PARAM;
  }

  /*
    Check memory region overlap.
    We use the src_size for checks as that is the
    max size that will be copied.
    Replace memscpy with memmove if you encounter this ASSERT.
  */
  if ((((uint8*)src <= (uint8*)dst) &&
       ((uint8*)dst < ((uint8*)src + src_size))) ||
      (((uint8*)dst <= (uint8*)src) &&
       ((uint8*)src < ((uint8*)dst + src_size))))
  {
    return FLASH_FTL_INVALID_PARAM;
  }

  if(dst_size < src_size)
  {
    src_size = dst_size;
  }

  copied_bytes = (uint32)CopyMemS(dst, dst_size, src, src_size);
  if(copied_bytes != src_size)
  {
    return FLASH_FTL_FAIL;
  }
  return FLASH_FTL_OK;
}

/**
  flash_ftl_memset - initialize memory with given pattern.

  Copies the "value" to the ptr buffer.
*/
FLASH_FTL_STATUS
flash_ftl_memset( void *ptr, uint32 value, uint32 num )
{
  if(ptr && num)
  {
    SetMem(ptr, num, value);
  }
  else
  {
    return FLASH_FTL_INVALID_PARAM;
  }
  return FLASH_FTL_OK;
}

/**
 * compares two strings and returns if they are equal
 */
FLASH_FTL_STATUS
flash_ftl_strcmp(const uint8 *str1, const uint8 *str2, uint32 *cmp_result)
{
  if((NULL == str1) || (NULL == str2))
  {
    return FLASH_FTL_INVALID_PARAM;
  }

  *cmp_result = AsciiStrCmp((CONST CHAR8 *)str1, (CONST CHAR8 *)str2);

  return FLASH_FTL_OK;
}

/**
 * return length of the input string
 */
uint32
flash_ftl_strlen(const uint8 *str)
{
  return AsciiStrLen((CONST CHAR8 *)str);
}

static inline void 
flash_ftl_log(uint8 errorLevel, const char *Format, ...)
{
  //return DebugPrint((UINTN)errorLevel, *Format);
}
