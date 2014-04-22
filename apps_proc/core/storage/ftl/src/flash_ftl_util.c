/**********************************************************************
 * flash_ftl_util.c
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

#include <malloc.h>
#include <string.h>
#include <stringl.h>
#include "qurt_memory.h"
#include "qurt_error.h"
#include "flash_ftl_util.h"

#define FLASH_FTL_MEM_ADDR_NOT_SPECIFIED  (0xFFFFFFFF)
typedef struct
{
  qurt_addr_t virt_addr;
  qurt_mem_region_t qurt_mem_hndl;
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
  qurt_mem_region_attr_t mem_info;
  qurt_mem_region_t handle_mem;
  uint32 i = 0;

  if(0 == size)
  {
    return NULL;
  }

  for(i = 0; i < FLASH_FTL_ALIGNED_MALLOC_CALL_ACCESS_LIMIT; i++)
  {
    if(flash_ftl_aligned_malloc_pool[i].qurt_mem_hndl == NULL)
    {
      break;
    }
  }

  if(FLASH_FTL_ALIGNED_MALLOC_CALL_ACCESS_LIMIT == i)
  {
    return NULL;
  }

  qurt_mem_region_attr_init(&mem_info);
  mem_info.mapping_type=QURT_MEM_MAPPING_VIRTUAL;
  mem_info.cache_attribs=QURT_MEM_CACHE_OUTER_INNER_UNCACHED;
  mem_info.permission=QURT_PERM_FULL;
  mem_info.virtaddr=FLASH_FTL_MEM_ADDR_NOT_SPECIFIED;
  mem_info.physaddr=FLASH_FTL_MEM_ADDR_NOT_SPECIFIED;

  if(QURT_EOK == qurt_mem_region_create(  &handle_mem,
                                          size,
                                          qurt_mem_default_pool,
                                          &mem_info))
  {
    /* If the allocation is success, get the memory address information */
    qurt_mem_region_attr_get(handle_mem, &mem_info);

    /* save it for future "free" operation */
    flash_ftl_aligned_malloc_pool[i].qurt_mem_hndl = handle_mem;
    flash_ftl_aligned_malloc_pool[i].virt_addr = mem_info.virtaddr;

    return (void*)mem_info.virtaddr;
  }
  else
  {
    return NULL;
  }
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
    if((qurt_addr_t)ptr == flash_ftl_aligned_malloc_pool[i].virt_addr)
    {
      qurt_mem_region_delete(flash_ftl_aligned_malloc_pool[i].qurt_mem_hndl);
      flash_ftl_aligned_malloc_pool[i].qurt_mem_hndl = NULL;
      flash_ftl_aligned_malloc_pool[i].virt_addr = NULL;
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
  return malloc(size);
}

/**
  Frees memory allocated using flash_ftl_malloc
*/
void
flash_ftl_free(void *ptr)
{
  free(ptr);
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

  copied_bytes = memscpy(dst, dst_size, src, src_size);
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
    memset(ptr, value, num);
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

  *cmp_result = strcmp((char*)str1, (char*)str2);

  return FLASH_FTL_OK;
}

/**
 * return length of the input string
 */
uint32
flash_ftl_strlen(const uint8 *str)
{
  return strlen((char*)str);
}
