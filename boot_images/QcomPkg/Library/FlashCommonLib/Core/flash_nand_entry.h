#ifndef FLASH_NAND_ENTRY_H
#define FLASH_NAND_ENTRY_H

/*=============================================================================
 *
 * FILE:      flash_nand_entry.h
 *
 * DESCRIPTION: Function and data structure declarations common to all
 *              NAND devices
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2008-2013 Qualcomm Technologies Incorporated.
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
 * when         who     what, where, why
 * ----------   ---     ----------------------------------------------
 * 06/21/13     sb      Expose write pages and erase blocks APIs
 * 09/05/12     sv      Add read pages, get blk state & get entry tbl fns
 * 07/27/12     sv      Remove DM and 2x mode codes, add BAM support
 * 05/20/11     bb      Flash driver Optimization
 * 08/11/10     nr      Added stats framework support
 * 04/15/10     jz      Added macros for 2X support
 * 03/15/10     bb      Support for isolating NOR build from NAND
 * 02/10/10     sb      Bad block optimization
 * 02/08/10     bb      Clean up and moved common code to nand entry layer
 * 10/01/09     bb      Added support multi r/w and copy APIs
 * 05/12/09     bb      Added support for Babylon
 * 03/22/09     bb      Added support for HAL optimizations
 * 03/12/09     bb      Add  DAL CacheCommand wrapper
 * 01/30/09     bb      Moved memory init from driver to nand entry
 * 10/06/08     bb      Initial Revision
 *==================================================================*/

/*===================================================================
 *
 *                     Include Files
 *
 ====================================================================*/
#include "DALFramework.h"

#include "flash_hal.h"
#include "flash_osal.h"
#include "flash_dal_util.h"
#include "flash_properties.h"
#include "flash_nand_partition.h"
#include "flash_otp_config.h"
#include "flash_nand_config.h"
//#include "flash_profile_common.h"

/** Flash NAND block state. This indicates the state of a NAND or 
    OneNAND block. */
enum flash_nand_block_state
{
  FLASH_NAND_BLOCK_UNINIT  = 0x0,   /**< Flash Block is Uninitilized */
  FLASH_NAND_BLOCK_OK  = 0x1,       /**< Flash Block is Good */
  FLASH_NAND_BLOCK_BAD = 0x2,       /**< Flash Block is Bad */
  FLASH_NAND_BLOCK_RESERVED = 0x3   /**< Reserved State */
} ;

/** Flash Page Data Type  */
struct flash_page_data_vector
{
  void   *main_data;       /**< Pointer to main data buffer, if any */
  void   *spare_data;      /**< Pointer to spare data buffer, if any */
  uint32 main_size_bytes;  /**< -- Length of main data to read in bytes */
  uint32 spare_size_bytes; /**< -- Length of spare data to read in bytes */
};

/* Flash memory information  */
struct nand_flash_meminfo
{
  struct flash_osal_meminfo    hal_mem_info;     /* HAL Workspace mem info */
  struct flash_osal_meminfo    drv_mem_info;     /* Driver Workspace mem info */
};


/* Data common to all NAND drivers (NAND, OneNAND) */
struct nand_entry_data
{
  enum flash_hal_dma_state dma_state;    /* Data mover state */
  uint32 nand_mem_initialized;         /* Flag to indicate flash
                                          memory initialization */
  uint32 multi_rw_support_count;       /* Number of pages/blocks
                                          operated between sync enter
                                          and sync release */
  uint32 build_env;                    /* Build environment ID */
  struct nand_flash_meminfo meminfo;   /* Flash memory allocation info */
};

/******************************************************************
 * Temp data types: begin
 ******************************************************************/
struct nand_page_iovec
{
  uint32 start_page;       /**< Logical page used to start the operation */
  uint32 total_page_count;       /**< Number of pages to operate */
  uint32 main_size_bytes;  /**< Length of main data to read/write from a page in bytes */
  uint32 spare_size_bytes; /**< Length of spare data to read from a page in bytes */
};

struct nand_block_vector
{
  uint32 start_block;         /**< Logical start block number */
  uint32 block_count;         /**< Number of blocks to operate */
  int    *result_vector;      /**< Vector to store result of block operation */
};

struct nand_mem_desc_buf
{
   uintnt VirtualAddr;
   uintnt PhysicalAddr;
   uint32 size;
   uint32 user;
};
/******************************************************************
 * Temp data types: end
******************************************************************/

/* Allocating 2 descriptors ( 1 main & 1 spare) */
#define FLASH_MAX_NUM_BUFS    2

struct nand_entry_client_data
{
  /* Handle for DMA friendly buffer descriptor. */
  DALSYSMemHandle nand_phys_handle;

  /* Create memory descriptor for DMA friendly memory. */
  DALSYS_MEM_DESC_LIST_OBJECT(nand_phys_buf_list, FLASH_MAX_NUM_BUFS);
};

/* Macro to test DM enabled or not */
#define FLASH_DMA_ENABLED  (FLASH_HAL_DMA_ENABLE == nand_entry_init_data.dma_state)

/* Macro to test cache maintanence required or not */
#define DO_CACHE_MAINTENANCE \
  (nand_entry_init_data.meminfo.hal_mem_info.props != DALSYS_MEM_PROPS_UNCACHED)

/* Logical block to Physical block translation */
#define NAND_LB_TO_PB(client_ctxt, lb)                                      \
  (client_ctxt->client_data.partition_block_start + lb)


/* NAND Probe function */
int nand_entry_device_probe(flash_client_ctxt *client_ctxt);

int nand_entry_mem_alloc(uint32 hal_mem_size, uint32 dal_mem_size);

int nand_probe(flash_client_ctxt *client_ctxt);

int nand_entry_get_info(flash_client_ctxt *client_ctxt,
  struct nand_info *get_data);

int nand_entry_read_pages(flash_client_ctxt *client_ctxt,
  enum page_access_opcode read_type, uint32 start_page, uint32 page_count, void *buffer,
  uint32 buffer_size);

int nand_entry_write_pages(flash_client_ctxt *client_ctxt,
  enum page_access_opcode write_type, uint32 start_page, uint32 page_count, void *buffer,
  uint32 buffer_size);
  
int nand_entry_erase_blocks(flash_client_ctxt *client_ctxt,
  uint32 start_block, uint32 block_count);
  
int nand_entry_block_get_state(flash_client_ctxt *client_ctxt,
  uint32 block, enum nand_block_state *block_state);

int nand_entry_block_set_state(flash_client_ctxt *client_ctxt, 
  uint32 block, enum nand_block_state block_state);
  
int nand_entry_open_partition(flash_client_ctxt *client_ctxt,
  const unsigned char *partition_name);

int nand_entry_close(flash_client_ctxt *client_ctxt);
int nand_entry_deinit (flash_client_ctxt *client_ctxt);

#endif /* FLASH_NAND_ENTRY_H */
