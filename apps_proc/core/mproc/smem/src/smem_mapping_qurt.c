/*===========================================================================

                 S H A R E D   M E M O R Y   M A P P I N G

DESCRIPTION
    Defines the OS specific helper functions for SMEM internal functions 
    regarding the OS mapping of the SMEM region(s) and its pages.

Copyright (c) 2013-2016 by Qualcomm Technologies, Incorporated.
All Rights Reserved.

===========================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/02/15   db      Check if page is already mapped before mapping it again
10/01/13   bt      Initial revision to map SMEM pages with Qurt.
===========================================================================*/
/*===========================================================================
                  INCLUDE FILES
===========================================================================*/
#include "qurt.h"
#include "qurt_memory.h"
#include "smem_os.h"
#include "smem_os_common.h"
#include "smem_type.h"
#include "smem_internal.h"
#include "smem_toc.h"
#include "smem_target.h"
#include "smem_v.h"


/*===========================================================================
                  LOCAL DATA DECLARATIONS
===========================================================================*/

/*===========================================================================
                  EXTERNAL DATA DECLARATIONS
===========================================================================*/
extern smem_init_info_type smem_init_info;

/*===========================================================================
                  PRIVATE FUNCTION DECLARATIONS
===========================================================================*/
/*===========================================================================
  FUNCTION  smem_map_generic_pages
===========================================================================*/
/**
  First map the static SMEM items only, then map the entire generic region
  after reading its size from the static heap info structure.

  The generic region (including static items) will be mapped as uncached.
  This function must be called before attempting to access anything in the
  SMEM region whatsoever, including reads and allocations.  Preferably, it
  should be called as soon as we have reserved a virtual address range for
  the SMEM region.

  @return
  None
*/
/*=========================================================================*/
static void smem_map_generic_pages( void )
{
  uint32 mapping_offset, mapping_size;
  volatile smem_heap_info_type *heap_info;

  /* IMPORTANT: Must map at least as much of static SMEM to cover the heap info
   * structure, before we can dereference any pointer to it! */

  /* 1.) First map the static SMEM items only, since we know they are always 
   * there.  This will include the heap info structure as well as the 
   * allocation table.  The region of static items always starts at offset 0. */
  mapping_size = smem_get_offset(SMEM_LAST_FIXED_BUFFER) + 
                   smem_get_static_size(SMEM_LAST_FIXED_BUFFER);
  mapping_offset = smem_map_page_attr(0, mapping_size, FALSE);

  /* 2.) Then map all of the generic unprotected SMEM region as uncached.  We 
   * can dereference the heap info structure now. */
  heap_info = smem_get_heap_info();

  /* The dynamic generic SMEM region always starts after the static items. */
  mapping_size = heap_info->free_offset + heap_info->heap_remaining - 
                   mapping_offset;
  (void)smem_map_page_attr(mapping_offset, mapping_size, FALSE);
}

/*===========================================================================
                  PUBLIC FUNCTION DECLARATIONS
===========================================================================*/
/*===========================================================================
  FUNCTION  smem_map_base_va
===========================================================================*/
/**
  Map the memory region and return the virtual address of the mapped (physical)
  base address of SMEM.

  @param[in]    phys_addr     Memory region base (physical) address.
  @param[in]    size          Size of the SMEM region.

  @return
  Virtual address of the mapped SMEM region.
*/
/*=========================================================================*/
char *smem_map_base_va( char *phys_addr, uint32 size )
{
  qurt_mem_region_t       smem_mem_region;
  qurt_mem_region_attr_t  smem_region_attr;
  int                     ret;
  qurt_mem_pool_t         pool_id;

  qurt_mem_region_attr_init(&smem_region_attr);
  smem_region_attr.mapping_type = QURT_MEM_MAPPING_NONE;
  smem_region_attr.physaddr = (qurt_addr_t)phys_addr;

  ret = qurt_mem_pool_attach("STATIC_POOL", &pool_id);
  if (ret != QURT_EOK)
  {
    ERR_FATAL("smem_map_base_va: SMEM qurt_mem_pool_attach create failed! ret=%d.",
              ret, 0, 0);
    return(NULL);
  }

  ret = qurt_mem_region_create(&smem_mem_region, (qurt_size_t)size,
                               pool_id, &smem_region_attr);
  if (ret != QURT_EOK)
  {
    ERR_FATAL("smem_map_base_va: SMEM mem_region create failed! paddr=%x, "
              "size=%x", phys_addr, size, 0);
  }

  /* Reretrieve attributes struct since qurt_mem_region_create may have altered
   * the base addresses or size to fit page alignment (hopefully alignment is 
   * unnecessary). */
  ret = qurt_mem_region_attr_get(smem_mem_region, &smem_region_attr);
  ASSERT(ret == QURT_EOK);

  /* The virtual base pointer must be assigned here so that the mapping 
   * functions that follow can use it, before this function returns. */
  smem_init_info.smem_data_base_addr = (char *)smem_region_attr.virtaddr;
  
  smem_map_generic_pages();

  return (char *)smem_region_attr.virtaddr;
}

/*===========================================================================
  FUNCTION  smem_map_page_attr
===========================================================================*/
/**
  Map the page of SMEM (or a whole address range) at offset page_offset and 
  size of at least page_size.  The cached argument allows the SMEM module to 
  dynamically decide upon each address range's cacheability.
  
  The mapping will be contiguous, from SMEM's base physical address + 
  page_offset to SMEM's base virtual address + page_offset, with "RW"
  permisions.  Use the returned actual page-aligned size to determine where
  the mapping stops and when a new page must be mapped.

  @param[in]    page_offset   The offset from the base address to map (will be 
                              rounded down to a 4kB alignment).
  @param[in]    page_size     Size of this SMEM page or address range (will be
                              rounded up to a 4kB multiple).
  @param[in]    cached        Should this address range be cached memory?

  @return
  The actual page size mapped, rounded up to a 4kB multiple.
*/
/*=========================================================================*/
uint32 smem_map_page_attr
(
  uint32                page_offset, 
  uint32                page_size, 
  boolean               cached
)
{
  uint32 aligned_offset;
  qurt_addr_t page_paddr, page_vaddr;
  qurt_mem_cache_mode_t cacheability;
  int ret;

  /* Round offset down to be 4kB aligned.  This really only is for downward
   * allocations; caller must make sure that the page has not been mapped 
   * already. */
  aligned_offset = page_offset & (~SMEM_PAGE_ALIGN_MASK);
  /* Adjust the size accounting for the offset adjustment. */
  page_size += (page_offset - aligned_offset);
  /* Round page size up to a 4kB multiple.  Caller should confirm this 
   * returned value in order to map the next page starting at the correct 
   * aligned offset (whether it is upwards or downwards). */
  page_size = (page_size + SMEM_PAGE_ALIGN_MASK) & (~SMEM_PAGE_ALIGN_MASK);

  page_paddr = (qurt_addr_t)smem_get_phys_base_addr() + aligned_offset;
  page_vaddr = (qurt_addr_t)smem_get_base_addr() + aligned_offset;
  /* I don't always cache, but when I do, I prefer Writethrough. 
   * (Writethrough makes more sense for an interprocessor usecase.) */
  cacheability = (cached ? QURT_MEM_CACHE_WRITETHROUGH : 
                           QURT_MEM_CACHE_NONE_SHARED);

  /* Map this page's virtual to physical address, making sure to keep the same
   * offset in order to maintain contiguity.  Permissions should be "RW". */
  ret = QURT_EOK;
  if (qurt_lookup_physaddr(page_vaddr) == 0)
  {
    ret = qurt_mapping_create(page_vaddr, page_paddr, page_size, cacheability,
                              QURT_PERM_READ_WRITE);
  }
  if (ret != QURT_EOK)
  {
    ERR_FATAL("smem_map_page_attr: Page mapping failed! offset=%x, size=%x, "
              "$=%d", aligned_offset, page_size, cacheability);
  }
  
  return page_size;
}
/*=========================================================================*/
/*
  The locations where it is required to call smem_map_page_attr():

  1./2.) The static SMEM items and entire rest of generic partition.
  (Done from smem_map_generic_pages().)

  3.) Also map the TOC page at the end of SMEM.
  (Done from smem_partition_init().)

  4.) Map each partition header during partition initialization, before 
  any accesses or other processing within the partition can continue.
  (Done from smem_partition_init().)

  5.) During partitions' initialization and every subsequent scan of an 
  SMEM partition for new allocations, map enough pages to cover the 
  current SMEM item allocations, based on the cached and uncached offsets.
  (Done from smem_get_addr_ex().)

  6.) Whenever a new SMEM item is allocated in a partition, ensure that 
  enough pages are mapped to cover it.
  (Done from smem_alloc_ex().)
*/
/*=========================================================================*/

/*===========================================================================
  FUNCTION  smem_map_memory_va
===========================================================================*/
/**
  Map the memory region and return the virtual address of the mapped (physical)
  memory region.

  @return
  Virtual address of the mapped memory region.

  @param[in]    mem_region   Memory region base (physical) address.
  @param[in]    size         Size of the memory region.

  @dependencies
  None.

  @sideeffects
  None
*/
/*=========================================================================*/
void *smem_map_memory_va(void* phys_addr, uint32 size)
{
  if (phys_addr == (void*)SMEM_RPM_MSG_RAM_BASE_PHYS)
  {
    return (void*)SMEM_RPM_MSG_RAM_BASE;
  }
  else
  {
    qurt_mem_region_t      mem_region;
    qurt_mem_region_attr_t region_attr;
    qurt_addr_t            virt_addr;
    qurt_addr_t            phys_addr_rounded;
    qurt_size_t            align_shift;
    qurt_size_t            region_size;
    int                    ret;
    qurt_mem_pool_t        pool_id;

    /* Round SMEM physical address to 4kB aligned. */
    phys_addr_rounded = ((qurt_addr_t)phys_addr) & (~SMEM_PAGE_ALIGN_MASK);
    /* Adjust the region_size accounting for the alignment. */
    align_shift = (qurt_size_t)((qurt_addr_t)phys_addr - phys_addr_rounded);
    region_size = size + align_shift;
    /* Round region_size up to a page region_size multiple. */
    region_size = 
        (region_size + SMEM_PAGE_ALIGN_MASK) & (~SMEM_PAGE_ALIGN_MASK);

    qurt_mem_region_attr_init(&region_attr);
    region_attr.mapping_type = QURT_MEM_MAPPING_NONE;
    region_attr.physaddr     = phys_addr_rounded;

    ret = qurt_mem_pool_attach("STATIC_POOL", &pool_id);
    if (ret != QURT_EOK)
    {
      ERR_FATAL("smem_map_memory_va: SMEM qurt_mem_pool_attach create failed! ret=%d.",
                ret, 0, 0);
      return(NULL);
    }

    ret = qurt_mem_region_create(&mem_region, region_size, 
                                 pool_id, &region_attr);
    if (ret != QURT_EOK)
    {
      ERR_FATAL("smem_map_memory_va: qurt_mem_region_attr_get failed! "
                "paddr=0x%x, size=%d, ret=%d.", phys_addr, size, ret);
      return NULL;
    }

    /* Reretrieve attributes struct since qurt_mem_region_create may have altered
    * the base addresses or region_size to fit page alignment (hopefully alignment is
    * unnecessary). */
    ret = qurt_mem_region_attr_get(mem_region, &region_attr);
    if (ret != QURT_EOK)
    {
      ERR_FATAL("qurt_mem_region_attr_get failed! phys_addr_rounded=0x%x, "
                "region_size=%d, ret=%d.", phys_addr_rounded, region_size, ret);
      return NULL;
    }
    virt_addr = region_attr.virtaddr;

    ret = QURT_EOK;
    if (qurt_lookup_physaddr(virt_addr) == 0)
    {
      ret = qurt_mapping_create(virt_addr, phys_addr_rounded, region_size,
                                QURT_MEM_CACHE_NONE_SHARED,
                                QURT_PERM_READ_WRITE);
    }
    if (ret != QURT_EOK)
    {
      ERR_FATAL("qurt_mapping_create failed! virt_addr=0x%X, phys_addr_rounded=0x%X,"
                "region_size=%d.", virt_addr, phys_addr_rounded, region_size);
      return NULL;
    }

    virt_addr = (qurt_addr_t)((qurt_size_t)virt_addr + align_shift);
    
    return (void *)virt_addr;
  }
}
