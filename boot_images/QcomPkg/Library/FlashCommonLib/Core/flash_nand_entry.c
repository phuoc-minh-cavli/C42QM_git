/*=============================================================================
 *
 * FILE:      flash_nand_entry.c
 *
 * DESCRIPTION: Function and data structure declarations specific to the NAND
 *             entry layer
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2008-2013,2015 Qualcomm Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *===========================================================================*/

/*===================================================================
 *
 *                       EDIT HISTORY FOR FILE
 *
 *   This section contains comments describing changes made to the
 *   module. Notice that changes are listed in reverse chronological
 *   order.
 *
 *  $Header: //components/rel/boot.xf/0.2/QcomPkg/Library/FlashCommonLib/Core/flash_nand_entry.c#1 $ $DateTime: 2018/08/01 23:51:05 $ $Author: pwbldsvc $
 *
 *
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 01/28/15     sb      Fix Klocwork errors
 * 06/21/13     sb      Expose write pages and erase blocks APIs
 * 11/01/12     sv      Call DALSYS_MemRegionAlloc only if size is not 0
 * 09/05/12     bb/sv   cleanups
 * 07/27/12     sv      BAM based NAND support/Remove 2x support codes
 * 03/19/12     sv      Adapt to changes for SPI NOR power management
 * 03/19/12     eo      Support for erase blocks by size
 * 05/20/11     bb      Flash driver Optimization
 * 11/04/10     jz      Added check of device_type_vtbl before calling its deinit
 * 10/21/10     sv      set the hal property for wait function config
 * 10/07/10     eo      Return Not Supported status for unsupported operations
 * 08/11/10     nr      Added stats framework support
 * 04/25/10     jz      Updated for 2X support, fixed block_set_state logic
 * 04/22/10     bb      Support for isolating NOR build from NAND
 * 04/05/10     bb      Fix for bad block optimization
 * 01/28/10     sb      Bad block optimization
 * 02/12/10     wt      Modified Babylon Marker for 2X support
 * 02/08/10     bb      Clean up and moved common code to nand entry layer
 * 12/28/09     sc      Added 2x support in DAL
 * 10/25/09     mh      Rename nand_partittion_open to nand_partition_update_limit
 * 10/15/09     sv      Update Babylon marker na
 * 08/13/09     bb      Added support for optimized copy & multi r/w APIs
 * 09/24/09     eo      Remove status check in phys to user buffers copy
 * 06/21/09     sv      HAL memutil changes/allocate cached memory for non DM
 * 06/03/09     mh      Fix bug in check for page within partition
 * 05/11/09     jm,bb   Support for babylon
 * 04/20/09     bb      Updates for DAL/HAL optimizations
 * 03/24/09     bb      Used safe string copy function
 * 02/23/09     bb      Read/Write API changes with iovec design changes
 * 01/30/09     bb      Moved memory init from driver to nand entry
 * 11/15/08     mm      Add properties for NAND and NOR Probe table
 * 08/28/08     dp      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/

#include "flash_nand_entry.h"
#include "flash_nand.h"
#include "flash_osal.h"
#include <string.h>

#define NAND_ENTRY_MAX_NUM_BUFS               (2)
#define NAND_ENTRY_MAIN_BUF_DESC              (0)
#define NAND_ENTRY_SPARE_BUF_DESC             (1)

#define FLASH_MEM_ADDR_NOT_SPECIFIED          0xFFFFFFFF

// initialize page data io vectors
#define INIT_PAGE_DATA_IOVEC(iovec,page_size,spare_size,page,page_count)       \
                                        iovec.main_size_bytes   = page_size;   \
                                        iovec.spare_size_bytes  = spare_size;  \
                                        iovec.total_page_count  = page_count;  \
                                        iovec.start_page        = page;

// initialize page data descriptors
#define INIT_BUF_DESC(desc, iovec, data, sdata)                                \
      desc[NAND_ENTRY_MAIN_BUF_DESC].user        = iovec.total_page_count;     \
      desc[NAND_ENTRY_MAIN_BUF_DESC].size        =                             \
        iovec.main_size_bytes * iovec.total_page_count;                        \
      desc[NAND_ENTRY_MAIN_BUF_DESC].VirtualAddr = (uint32)data;               \
      desc[NAND_ENTRY_SPARE_BUF_DESC].user       = iovec.total_page_count;     \
      desc[NAND_ENTRY_SPARE_BUF_DESC].size       =                             \
        iovec.spare_size_bytes * iovec.total_page_count;                       \
      desc[NAND_ENTRY_SPARE_BUF_DESC].VirtualAddr= (uint32)sdata;


/* NAND entry init data */
struct nand_entry_data nand_entry_init_data;

/* A variable to allow flash tool utilities to reset client config
 * if needed via write breakpoint on 0:ALL partition based
 * reads and writes */
static byte tools_can_update_config = 0;
/*===================================================================
 *
 *   F L A S H   D R I V E R   I N I T   R O U T I  N E S
 *
 ====================================================================*/
/*
 * Initializes page data parameters
 */
static __inline void init_page_param(flash_client_ctxt *client_ctxt, uint32 op_type, 
  struct nand_page_iovec *page_data, uint32 page, uint32 page_count)
{
  switch (op_type)
  {
    case NAND_ACCESS_RAW:
      page_data->main_size_bytes =
        client_ctxt->client_data.total_page_size_bytes;
      page_data->spare_size_bytes = 0;
      break;
    case NAND_ACCESS_MAIN:
      page_data->main_size_bytes =
        client_ctxt->client_data.page_size_bytes;
      page_data->spare_size_bytes = 0;
      break;
    case NAND_ACCESS_MAIN_SPARE:
      page_data->main_size_bytes =
        client_ctxt->client_data.page_size_bytes;
      page_data->spare_size_bytes = 
        client_ctxt->client_data.max_spare_udata_bytes;
      break;
    case NAND_ACCESS_SPARE:
      page_data->main_size_bytes = 0;
      page_data->spare_size_bytes =
        client_ctxt->client_data.max_spare_udata_bytes;
      break;
  }

  page_data->start_page = page;
  page_data->total_page_count = page_count;
}

/*
 * De-allocate all flash driver allocated memory
 */
static int nand_entry_mem_deinit(void)
{
  int result = NAND_DEVICE_DONE;
  struct nand_flash_meminfo *meminfo;

  meminfo = &nand_entry_init_data.meminfo;

  /* Free memory allocated for the HAL */
  if (meminfo->hal_mem_info.handle)
  {
    if (flash_free_memregion(&meminfo->hal_mem_info))
    {
      result = NAND_DEVICE_FAIL;
    }
#if 0
    if (DAL_SUCCESS != DALSYS_DestroyObject(
      (DALSYSObjHandle) meminfo->handle_hal_mem))
    {
      result = NAND_DEVICE_FAIL;
    }
#endif
  }

  /* Free memory allocated by the driver */
  if (meminfo->drv_mem_info.handle)
  {
    if (flash_free_memregion(&meminfo->drv_mem_info))
    {
      result = NAND_DEVICE_FAIL;
    }
#if 0
    if (DAL_SUCCESS != DALSYS_DestroyObject(
      (DALSYSObjHandle) meminfo->handle_drv_mem))
    {
      result = NAND_DEVICE_FAIL;
    }
#endif
  }

  /* Reset all memory handles and flags */
  flash_memset((void *)&meminfo->drv_mem_info, 0x0, sizeof(struct flash_osal_meminfo));
  flash_memset((void *)&meminfo->hal_mem_info, 0x0, sizeof(struct flash_osal_meminfo));

  nand_entry_init_data.nand_mem_initialized = FALSE;

  return result;
}

/*
 * Allocate memory for all driver usage
 */
int nand_entry_mem_alloc(uint32 hal_mem_size, uint32 dal_mem_size)
{
  uint32 alloc_attr;
  int result = NAND_DEVICE_DONE;
  struct nand_flash_meminfo *meminfo;

  /* If the memory is already initialized, then do nothin */
  if (TRUE == nand_entry_init_data.nand_mem_initialized)
  {
    result = nand_entry_mem_deinit();
  }

  meminfo = &nand_entry_init_data.meminfo;

  /* Clear the virtual to physical list */
  flash_vtop_init_list();

  /* Set the allocation attribute to allocate cached - physically
   * contiguous region for non DM and uncached - physically contiguous
   * region incase of DM
   */
  alloc_attr = DALSYS_MEM_PROPS_PHYS_CONT;

  if (FLASH_HAL_DMA_ENABLE == nand_entry_init_data.dma_state)
  {
    alloc_attr |= DALSYS_MEM_PROPS_UNCACHED;
  }

  /* Allocate Memory for HAL usage */
  if ((NAND_DEVICE_DONE == result) && hal_mem_size)
  {
    if (flash_memregion_alloc(&meminfo->hal_mem_info, hal_mem_size))
    {
      FLASHLOG(2, ("Flash: HALWS Memory allocation failed!\n"));
      result = NAND_DEVICE_FAIL;
    }
    else
    {
      flash_memset((void *)meminfo->hal_mem_info.virtual_addr, 0xFF, 10); //<TODOS> why 10
    }
#if 0
    if (DAL_SUCCESS != DALSYS_MemRegionAlloc(alloc_attr,
      DALSYS_MEM_ADDR_NOT_SPECIFIED, DALSYS_MEM_ADDR_NOT_SPECIFIED,
      hal_mem_size, &meminfo->handle_hal_mem, NULL))
    {
      FLASHLOG(2, ("Flash: HALWS Memory allocation failed!\n"));
      result = NAND_DEVICE_FAIL;
    }
    else
    {
      /* If the allocation is success, get the memory address
       * information */
      DALSYS_MemInfo(meminfo->handle_hal_mem,
        &meminfo->hal_mem_info);

      /*  Need to touch the memory  for it to be configured in
       *  MMU table for ARM11 */
      DALSYS_memset((void *)meminfo->hal_mem_info.VirtualAddr,
        0xFF, 10);
    }
#endif
  }

  /* Workspace Memory for flash driver usage */
  if ((NAND_DEVICE_DONE == result) && dal_mem_size)
  {
    if (flash_memregion_alloc(&meminfo->drv_mem_info, dal_mem_size))
    {
      FLASHLOG(2, ("Flash: DRVWS Memory allocation failed!\n"));
      result = NAND_DEVICE_FAIL;
    }
    else
    {
      /* Set the memory initialization flag to TRUE */
      nand_entry_init_data.nand_mem_initialized = TRUE;
    }

#if 0
    if (DAL_SUCCESS != DALSYS_MemRegionAlloc(alloc_attr,
      DALSYS_MEM_ADDR_NOT_SPECIFIED, DALSYS_MEM_ADDR_NOT_SPECIFIED,
      dal_mem_size, &meminfo->handle_drv_mem, NULL))
    {
      FLASHLOG(2, ("Flash: DRVWS Memory allocation failed!\n"));
      result = NAND_DEVICE_FAIL;
    }
    else
    {
      /* If the allocation is success, get the memory address
       * information */
      DALSYS_MemInfo(meminfo->handle_drv_mem, &meminfo->drv_mem_info);

      /* Set the memory initialization flag to TRUE */
      nand_entry_init_data.nand_mem_initialized = TRUE;
    }
#endif
  }

  /* If the memory allocation fails, de-allocate if any
   * allocated memory */
  if (NAND_DEVICE_DONE != result)
  {
    nand_entry_mem_deinit();
  }
  else
  {
    /* Add the HAL / DAL memory details to the virtual
     * to physical conversion list */
    flash_vtop_add_item(
      meminfo->hal_mem_info.virtual_addr,
      meminfo->hal_mem_info.physical_addr,
      meminfo->hal_mem_info.size);

    flash_vtop_add_item(
      meminfo->drv_mem_info.virtual_addr,
      meminfo->drv_mem_info.physical_addr,
      meminfo->drv_mem_info.size);
  }

  return result;
}

/*
 * Allocate memory for all driver usage
 */
static int nand_entry_mem_init(void)
{
  uint32 hal_mem_size = 0;
  uint32 dal_mem_size = 0;
  uint32 temp_val;
  int result = NAND_DEVICE_DONE;

  /* If the memory is already initialized, then do nothin */
  if (TRUE == nand_entry_init_data.nand_mem_initialized)
  {
    return NAND_DEVICE_DONE;
  }

  /* Get the HAL memory size from property table */
  hal_mem_size = flash_get_property(FLASH_HAL_WS_BUFF_SIZE_ID);

  /* Get the driver workspace memory size from property table */
  dal_mem_size = flash_get_property(FLASH_DRV_WS_BUFF_SIZE_ID);

  if ((FLASH_PROPERTY_NOT_DEFINED == hal_mem_size) ||
      (FLASH_PROPERTY_NOT_DEFINED == dal_mem_size))
  {
    FLASHLOG(2, ("Flash: Get HALWS Memory property failed!\n"));
    result = NAND_DEVICE_FAIL;
  }

  result = nand_entry_mem_alloc(hal_mem_size, dal_mem_size);

  /* Get the memory utils api type to be used in HAL */
  temp_val = flash_get_property(FLASH_HAL_MEM_UTIL_TYPE_ID);

  if(FLASH_HAL_USE_INT_MEMUTIL == temp_val)
  {
    /* Set the HAL property to use Hal defined mem util which handles
     * issues with memory operations on uncached memory */
    flash_hal_set_property(FLASH_HAL_PROPERTY_USE_INT_MEMUTIL, NULL);
  }
  else
  {
    /* Use standard mem util */
    flash_hal_set_property(FLASH_HAL_PROPERTY_USE_INT_ASM_MEMUTIL, NULL);
  }

  return result;
}

/*
 * Common initializations during NAND probe
 */
static int nand_entry_init(flash_client_ctxt *client_ctxt)
{
  uint32 temp_val;
  int result;

  /* Get the default DMA state from property table */
  temp_val = flash_get_property(FLASH_DMA_STATE_ID);

  if (FLASH_PROPERTY_NOT_DEFINED != temp_val)
  {
    if (FLASH_HAL_DMA_ENABLE == temp_val)
	{
      nand_entry_init_data.dma_state = FLASH_HAL_DMA_ENABLE;
	}
  }
  /* Initialize memory for probe */
  result = nand_entry_mem_init();

  if (NAND_DEVICE_DONE != result)
  {
    FLASHLOG(3, ("Flash: Failed to initialize memory.\n"));
  }
  else
  {
    nand_entry_init_data.multi_rw_support_count = 1;
    /* Set the wait function to be used by Flash HAL */
    flash_hal_set_property(FLASH_HAL_PROPERTY_WAIT_CFG,
      (void *)&flash_busy_wait);

    if (DO_CACHE_MAINTENANCE)
    {
      /* Set the cache invalidate function to be used by Flash HAL */
      flash_hal_set_property(FLASH_HAL_PROPERTY_CACHE_INVAL_CFG,
        (void *)&flash_dcache_inval);

      /* Set the cache clean function to be used by Flash HAL */
      flash_hal_set_property(FLASH_HAL_PROPERTY_CACHE_CLEAN_CFG,
        (void *)&flash_dcache_clean);
    }
  }

  return result;
}

/*===================================================================
 *
 *   F L A S H   D R I V E R   V A L I D A T I O N   R O U T I N E S
 *
 ====================================================================*/

/*
 * Create the DMA friendly memory buffer descriptors
 */
static __inline int flash_get_dma_buffers(
  struct nand_entry_client_data *client_data, struct nand_mem_desc_buf *buff_list,
  struct nand_mem_desc_buf **phys_buff_list, uint32 operation)
{
  int result = NAND_DEVICE_DONE;

  /* Create the DMA friendly memory from the given user buffer list.*/
  /* note: nand_phys_handle is initialized once (when any of the buffer is not
   *       dma friendly (not 32 byte aligned or not physically contiguous
   *       This is then reused each time
   */
  if (FLASH_HAL_DMA_ENABLE == nand_entry_init_data.dma_state)
  {
    (*phys_buff_list)->VirtualAddr = buff_list->VirtualAddr;
    (*phys_buff_list)->PhysicalAddr = buff_list->VirtualAddr; // qurt_lookup_physaddr(buff_list->VirtualAddr);
  
    (*phys_buff_list)->size = buff_list->size;
    (*phys_buff_list)->user = buff_list->user;
 
    (*phys_buff_list+1)->VirtualAddr = (buff_list+1)->VirtualAddr;
    (*phys_buff_list+1)->PhysicalAddr = (buff_list+1)->VirtualAddr; // qurt_lookup_physaddr((buff_list+1)->VirtualAddr);  
    (*phys_buff_list+1)->size = (buff_list+1)->size;
    (*phys_buff_list+1)->user = (buff_list+1)->user;
  }
  else
  {
    *phys_buff_list = buff_list;
  }

  return result;
}

/*
 * Copy data between two buffer descriptors
 */
static __inline int flash_copy_dma_buffers(struct nand_mem_desc_buf * dest_list,
  struct nand_mem_desc_buf * src_list)
{
  int result = NAND_DEVICE_DONE;

  /* Copy the buffer descriptor, only when we use DAL buffer
     descriptors, ie, when DMA is enabled */
  if (FLASH_HAL_DMA_ENABLE == nand_entry_init_data.dma_state)
  {
    //not required now as buffer is filled directly
  }

  return result;
}

/*
 * Check Page lies within the partition
 */
static int flash_nand_check_page_in_partition(flash_client_ctxt *client_ctxt,
  uint32 start_page, uint32 page_count)
{
  uint32 total_pages = 0;
  int result = NAND_DEVICE_DONE;
  uint32 blk_size_shift = client_ctxt->client_data.block_size_shift;

  /* Get total number of pages in the partition */
  total_pages = (client_ctxt->client_data.partition_block_limit -
    client_ctxt->client_data.partition_block_start) << blk_size_shift ;

  if(1 == page_count)
  {
    /* Ensure requested page does not exceed number of pages in partition */
    if (start_page >= total_pages)
    {
       result = FLASH_DEVICE_INVALID_PARAMETER;
    }
  }
  else
  {
    /* Ensure the following
       - start and last page of the requested page_range does not exceed
         number of pages in partition
       - the page range does not lie cross two blocks

    */
    if ((start_page >= total_pages) ||
     ((start_page + page_count) > total_pages) ||
     (((start_page & (client_ctxt->client_data.pages_per_block - 1)) +
      page_count) > client_ctxt->client_data.pages_per_block))
    {
      result = FLASH_DEVICE_INVALID_PARAMETER;
    }
  }

  return result;
}

/*===================================================================
 *
 *   F L A S H   D R I V E R   E N T R Y   R O U T I N E S
 *
 ====================================================================*/

/*
 * Handle to the driver close API  At present this API does not do anything
 */
int nand_entry_close(flash_client_ctxt *client_ctxt)
{
  int result = NAND_DEVICE_DONE;

  /* Synchronize with other clients */
  flash_sync_enter(client_ctxt->flash_dev_ctxt->handle_dev_sync);
  result = nand_init_close(client_ctxt);
  flash_sync_leave(client_ctxt->flash_dev_ctxt->handle_dev_sync);

  /* Set the driver operation status to debug */
  SET_FLASH_DBG_OP_STATUS(result);

  return result;
}

/*
 * Handle to the driver deinit API  At present this API does not do anything
 */
int nand_entry_deinit (flash_client_ctxt *client_ctxt)
{
  int result = NAND_DEVICE_DONE;

  /* Synchronize with other clients */
  flash_sync_enter(client_ctxt->flash_dev_ctxt->handle_dev_sync);

  nand_init_close(client_ctxt);
  nand_deinit(client_ctxt);

  flash_sync_leave(client_ctxt->flash_dev_ctxt->handle_dev_sync);

  /* Set the driver operation status to debug */
  SET_FLASH_DBG_OP_STATUS(result);

  /* TBD: do the cleanup of the DAL critical section/ synchronization
   * handles */

  return result;
}

/*
 * Handle to the Open partiton request
 */
int nand_entry_open_partition(flash_client_ctxt *client_ctxt,
  const unsigned char *partition_name)
{
  int result = NAND_DEVICE_DONE;

  FLASHLOG (3, (" nand_entry_open_partition %s\n", partition_name));

  /* Synchronize with other clients */
  //DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);
  flash_sync_enter(client_ctxt->flash_dev_ctxt->handle_dev_sync);

  /* Interface with the NAND Partion Layer */
  result = nand_partition_update_limits(client_ctxt, partition_name);

  tools_can_update_config = 1;

  (void)tools_can_update_config;

  if (NAND_DEVICE_DONE == result)
  {
    result = nand_open_partition(client_ctxt, partition_name);
  }

  //DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);
  flash_sync_leave(client_ctxt->flash_dev_ctxt->handle_dev_sync);

  /* Set the driver operation status to debug */
  SET_FLASH_DBG_OP_STATUS(result);

  return result;
}

/*
 * This API gets device and parition specific information
 */
int nand_entry_get_info(flash_client_ctxt *client_ctxt,
  struct nand_info *get_data)
{
  int result = NAND_DEVICE_DONE;

  if (client_ctxt->refs)
  {
    struct nand_info *info = get_data;

    /* Copy the device name */
    flash_strcpy((uint8 *) info->device_name,
      (uint8 *)client_ctxt->flash_dev_ctxt->dev_info.device_name,32);

    /* Get Device type and width  */
    info->block_count = client_ctxt->flash_dev_ctxt->dev_info.block_count;

    /* Get Device manufacturer/make ID, device ID, version  */
    info->maker_id = client_ctxt->flash_dev_ctxt->dev_info.maker_id;
    info->device_id = client_ctxt->flash_dev_ctxt->dev_info.device_id;
    info->device_version = client_ctxt->flash_dev_ctxt->dev_info.version_id;

    /* Get Partition specific information  */
    info->block_count = 
      client_ctxt->client_data.block_count;
    info->pages_per_block = 
      client_ctxt->client_data.pages_per_block;
    info->page_size_bytes =
      client_ctxt->client_data.page_size_bytes;
    info->total_page_size_bytes =
      client_ctxt->client_data.total_page_size_bytes;
    info->max_spare_udata_bytes =
      client_ctxt->client_data.max_spare_udata_bytes;
    info->ecc_mode = 
      client_ctxt->client_data.ecc_state;
    info->attributes =
      client_ctxt->client_data.partition_attributes;

    /* Get MIBIB block numbers */
    info->mibib_info.old_mibib_block =
      client_ctxt->flash_dev_ctxt->mibib_info.old_mibib_block;
    info->mibib_info.new_mibib_block =
      client_ctxt->flash_dev_ctxt->mibib_info.new_mibib_block;
  }
  else
  {
    get_data = NULL;
    result = NAND_DEVICE_FAIL;
  }

  /* Set the driver operation status to debug */
  SET_FLASH_DBG_OP_STATUS(NAND_DEVICE_DONE);

  return result;
}

/*
 * This API calls the handle to to erase a range of blocks with a partition
 */
int nand_entry_erase_blocks(flash_client_ctxt *client_ctxt,
  uint32 start_block, uint32 block_count)
{
  int result = NAND_DEVICE_DONE;
  struct nand_block_vector tmp_block_vector;

  /* Lock access to other clients */
  FLASH_CORE_PROFILE_LOCK();

  /* Synchronize with other clients */
  //DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);
  flash_sync_enter(client_ctxt->flash_dev_ctxt->handle_dev_sync);

  FLASH_CORE_PROFILE_START(FLASH_PROFILE_ERASE_BLOCKS, \
    FLASH_PROFILE_DEFAULT_NPAGES);

  /* Alocates memory for the block erase status */
  //if (DAL_SUCCESS != DALSYS_Malloc((sizeof(int)*block_count),(void **)&tmp_block_vector.result_vector))
  if (NAND_DEVICE_DONE != flash_malloc((sizeof(int)*block_count),(void **)&tmp_block_vector.result_vector))
  {
	return NAND_DEVICE_FAIL;
  }
  tmp_block_vector.start_block = start_block;
  tmp_block_vector.block_count = block_count;

  /* Ensure the block range is valid and lies within the partition */
  if ((tmp_block_vector.start_block + tmp_block_vector.block_count - 1) >=
      (client_ctxt->client_data.partition_block_limit -
       client_ctxt->client_data.partition_block_start))
  {
    result = FLASH_DEVICE_INVALID_PARAMETER;
  }
    
  /* If valid range of blocks, start  erasing the blocks */
  if (NAND_DEVICE_DONE == result)
  {
    result = nand_erase_blocks(client_ctxt, &tmp_block_vector, 1);
  }

  /* Set the driver operation status to debug */
  SET_FLASH_DBG_OP_STATUS(result);

  FLASH_CORE_PROFILE_END(FLASH_PROFILE_ERASE_BLOCKS);

  /* Leave Synchronization */
  //DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);
  flash_sync_leave(client_ctxt->flash_dev_ctxt->handle_dev_sync);

  /* UnLock access to other clients */
  FLASH_CORE_PROFILE_UNLOCK();

  return result;
}

/*
 * This API calls the handle to set the status of a block
 */
int nand_entry_block_set_state(flash_client_ctxt *client_ctxt, uint32 block,
  enum nand_block_state block_state)
{
  int result = NAND_DEVICE_DONE;

  /* Lock access to other clients */
  FLASH_CORE_PROFILE_LOCK();

  /* Synchronize with other clients */
  //DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);
  flash_sync_enter(client_ctxt->flash_dev_ctxt->handle_dev_sync);

  /* Ensure the block is valid and lies within the partition */
  if (block >= (client_ctxt->client_data.partition_block_limit -
      client_ctxt->client_data.partition_block_start))
  {
    result = FLASH_DEVICE_INVALID_PARAMETER;
  }

  result = nand_set_block_state(client_ctxt, block, block_state);

  /* Set the driver operation status to debug */
  SET_FLASH_DBG_OP_STATUS(result);

  //DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);
  flash_sync_leave(client_ctxt->flash_dev_ctxt->handle_dev_sync);

    /* UnLock access to other clients */
  FLASH_CORE_PROFILE_UNLOCK();

  return result;
}

/*
 * This API calls the handle to get the status of a block
 */
int nand_entry_block_get_state(flash_client_ctxt *client_ctxt,
  uint32 block, enum nand_block_state *block_state)
{
  int result = NAND_DEVICE_DONE;

  /* Lock access to other clients */
  FLASH_CORE_PROFILE_LOCK();

  /* Synchronize with other clients */
  //DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);
  flash_sync_enter(client_ctxt->flash_dev_ctxt->handle_dev_sync);

  /* Ensure the block is valid and lies within the partition */
  if (block >= (client_ctxt->client_data.partition_block_limit -
      client_ctxt->client_data.partition_block_start))
  {
    result = NAND_DEVICE_INVALID_PARAMETER;
  }

  result = nand_get_block_state(client_ctxt, block, block_state);

  /* Set the driver operation status to debug */
  SET_FLASH_DBG_OP_STATUS(result);

  //DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);
  flash_sync_leave(client_ctxt->flash_dev_ctxt->handle_dev_sync);

    /* UnLock access to other clients */
  FLASH_CORE_PROFILE_UNLOCK();

  return result;
}

/*
 * This API calls the handle to write page(s) within a partition
 */
int nand_entry_write_pages(flash_client_ctxt *client_ctxt,
  enum page_access_opcode write_type, uint32 start_page, uint32 page_count, void *buffer,
  uint32 buffer_size)
{
  int result = NAND_DEVICE_DONE;
  uint32 page_size = 0;
  void   *sbuffer = NULL;
  struct nand_entry_client_data *client_data;
  struct nand_page_iovec         page_data_iovec;
  struct nand_mem_desc_buf       buffer_desc[2];
  struct nand_mem_desc_buf      *phys_buf_list;

  /* Lock access to other clients */
  FLASH_CORE_PROFILE_LOCK();

  /* Synchronize with other clients */
  //DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);
  flash_sync_enter(client_ctxt->flash_dev_ctxt->handle_dev_sync);
  
  /* Clear buffer descriptor structure */
  flash_memset((void *)&buffer_desc[0], 0, sizeof(struct nand_mem_desc_buf)*2);  
  
  page_size = client_ctxt->client_data.page_size_bytes;
  init_page_param(client_ctxt, write_type, &page_data_iovec, start_page, page_count);
  
  sbuffer = (buffer_size > (page_size * page_count))? 
              (void *)((uintnt)buffer + (page_size * page_count)): NULL;
  INIT_BUF_DESC(buffer_desc, page_data_iovec, buffer, sbuffer)
  
  FLASH_CORE_PROFILE_START((FLASH_PROFILE_WRITE_MAIN + (int)write_type), \
    page_data_iovec->total_page_count);

  /* For each IOVEC, Ensure requested page(s) lies within the partition */
  if (NAND_DEVICE_DONE != flash_nand_check_page_in_partition(client_ctxt,
    page_data_iovec.start_page, page_data_iovec.total_page_count))
  {
    result = NAND_DEVICE_INVALID_PARAMETER;
  }

  if (NAND_DEVICE_DONE == result)
  {
    if (buffer_desc->size != 0)
    {
      flash_dcache_clean(buffer_desc->VirtualAddr, buffer_desc->size);
    }
    if ((buffer_desc+1)->size != 0)
    {
      flash_dcache_clean((buffer_desc+1)->VirtualAddr, (buffer_desc+1)->size);
    }
    
    client_data = (struct nand_entry_client_data *)
      client_ctxt->client_data.client_specific_data;

    phys_buf_list = (struct nand_mem_desc_buf *) &client_data->nand_phys_buf_list;

    /* Get the DMA friendly memory, if Data mover is present */
    result = flash_get_dma_buffers(client_data, buffer_desc, &phys_buf_list,
      ALLOC_PHYS_MEMDESC_OP_WRITE);

    if (NAND_DEVICE_DONE == result)
    {
      /* Copy the data from user buffers to physical (DMA friendly) buffers */
      result = flash_copy_dma_buffers ((struct nand_mem_desc_buf *)
        phys_buf_list, buffer_desc);

      if (NAND_DEVICE_DONE == result)
      {
        result = nand_write_pages(client_ctxt, write_type,
          (struct nand_mem_desc_buf *)phys_buf_list, &page_data_iovec);
      }
    }
  }

  /* Set the driver operation status to debug */
  SET_FLASH_DBG_OP_STATUS(result);

  FLASH_CORE_PROFILE_END((FLASH_PROFILE_WRITE_MAIN + (int)write_type));

  /* Leave synchornization */
  //DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);
  flash_sync_leave(client_ctxt->flash_dev_ctxt->handle_dev_sync);

    /* UnLock access to other clients */
  FLASH_CORE_PROFILE_UNLOCK();

  return result;
}

/*
 * This API calls the handle to read page(s) within a partition
 */
int nand_entry_read_pages(flash_client_ctxt *client_ctxt,
  enum page_access_opcode read_type, uint32 start_page, uint32 page_count, void *buffer,
  uint32 buffer_size)
{
  int result = NAND_DEVICE_DONE;
  uint32 page_size = 0;
  void   *sbuffer;
  struct nand_entry_client_data *client_data;
  struct nand_page_iovec        page_data_iovec;
  struct nand_mem_desc_buf       buffer_desc[2];
  struct nand_mem_desc_buf       *phys_buf_list;

  /* Lock access to other clients */
  FLASH_CORE_PROFILE_LOCK();

  /* Synchronize with other clients */
  flash_sync_enter(client_ctxt->flash_dev_ctxt->handle_dev_sync);

  /* Clear buffer descriptor structure */
  flash_memset((void *)&buffer_desc[0], 0, sizeof(struct nand_mem_desc_buf)*2);  

  page_size = client_ctxt->client_data.page_size_bytes;
  init_page_param(client_ctxt, read_type, &page_data_iovec, start_page, page_count);

  sbuffer = (buffer_size > (page_size * page_count))? 
              (void *)((uintnt)buffer + (page_size * page_count)): NULL;
  INIT_BUF_DESC(buffer_desc, page_data_iovec, buffer, sbuffer)

  FLASH_CORE_PROFILE_START((FLASH_PROFILE_READ_MAIN + (int)read_type), \
    page_data_iovec->total_page_count);

  /* For each IOVEC, Ensure requested page(s) lies within the partition */
  if (NAND_DEVICE_DONE != flash_nand_check_page_in_partition(client_ctxt,
    page_data_iovec.start_page, page_data_iovec.total_page_count))
  {
    result = NAND_DEVICE_INVALID_PARAMETER;
  }

  if (NAND_DEVICE_DONE == result)
  {	  
    client_data = (struct nand_entry_client_data *)
      client_ctxt->client_data.client_specific_data;

    phys_buf_list = (struct nand_mem_desc_buf *) &client_data->nand_phys_buf_list;

    /* Get the DMA friendly memory, if Data mover is present */
    result = flash_get_dma_buffers(client_data, buffer_desc, &phys_buf_list,
      ALLOC_PHYS_MEMDESC_OP_READ);

    if (NAND_DEVICE_DONE == result)
    {
      if (buffer_desc->size != 0)
      {
        flash_dcache_inval(buffer_desc->VirtualAddr, buffer_desc->size);
      }
      if ((buffer_desc+1)->size != 0)
      {
        flash_dcache_inval((buffer_desc+1)->VirtualAddr, (buffer_desc+1)->size);
      }
	  
      result = nand_read_pages(client_ctxt, read_type,
        (struct nand_mem_desc_buf *)phys_buf_list, &page_data_iovec);

      if (NAND_DEVICE_DONE != flash_copy_dma_buffers(buffer_desc,
        (struct nand_mem_desc_buf *) phys_buf_list))
      {
        result = NAND_DEVICE_FAIL;
      }
    }
  }

  /* Set the driver operation status to debug */
  SET_FLASH_DBG_OP_STATUS(result);
  FLASH_CORE_PROFILE_END((FLASH_PROFILE_READ_MAIN + (int)read_type));

  /* Leave synchornization */
  flash_sync_leave(client_ctxt->flash_dev_ctxt->handle_dev_sync);

    /* UnLock access to other clients */
  FLASH_CORE_PROFILE_UNLOCK();

  return result;
}

/*
 * Probe the NAND device
 */
int nand_entry_device_probe(flash_client_ctxt *client_ctxt)
{
  int status = NAND_DEVICE_NOT_SUPPORTED;
  int init_status = NAND_DEVICE_FAIL;

  init_status = nand_entry_init(client_ctxt);

  if (NAND_DEVICE_DONE != init_status)
  {
    /* Error Fatal */
    //DALSYS_LogEvent(0, DALSYS_LOGEVENT_FATAL_ERROR,
    //                "Flash: Failed to do initialization for probe!");
  }
  else
  {
    /* Synchronize with other clients */
    //DALSYS_SyncEnter(client_ctxt->flash_dev_ctxt->handle_dev_sync);
	flash_sync_enter(client_ctxt->flash_dev_ctxt->handle_dev_sync);

    /* Init the babylon */
    FLASH_CORE_PROFILE_INIT();

    status = nand_probe(client_ctxt);

    //DALSYS_SyncLeave(client_ctxt->flash_dev_ctxt->handle_dev_sync);
	flash_sync_leave(client_ctxt->flash_dev_ctxt->handle_dev_sync);

    /* Get device's info to initialize stats framework */
    if (NAND_DEVICE_DONE == status)
    {
      FLASH_CORE_PROFILE_SET_INFO(client_ctxt);
    }
    else
    {
      /* flash failure detected */
      nand_entry_mem_deinit();
    }
  }

  /* Set the driver operation status to debug */
  SET_FLASH_DBG_OP_STATUS(status);

  return status;
}
