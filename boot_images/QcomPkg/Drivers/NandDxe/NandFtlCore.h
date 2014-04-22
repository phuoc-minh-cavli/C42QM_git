#ifndef NAND_FTL_CORE_H
#define NAND_FTL_CORE_H

/**********************************************************************
 * NandFtlCore.h
 *
 * Core layer for Flash Translation layer (FTL).
 * This layer does platform specific flash operations
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
 * 2016-12-02   svl     added new lba_sized buffer for flash dma operations
 * 2016-11-11   svl     Review comments incorporated.
 * 2016-08-24   svl     FTL layer support
 *=======================================================================*/

#include <nand_api.h>
#include <NandFtlTypes.h>

typedef struct flash_ftl_client
{
  nand_handle_t   flash_handle;   /* for flash open/read/write APIs */
  flash_ftl_info_t info;           /* partition info */
  uint8            *bbt;           /* bad block table for each partition */
  uint8            part_name[FTL_DEVICE_NAME_SIZE];  /* partitn name for identification */
  uint8            clnt_cnt;       /* number of clients for this partition */
  uint8            *dma_buffer;    /* lba sized aligned buffer to be used only for dma operations */
}ftl_pvt_ctx_t;

#define FLASH_FTL_MAX_PARTITIONS_SUPPORTED        (32)

/******************************************************************************
 * APIs
 *****************************************************************************/
/**
 * flash_ftl_core_api_set_block - mark the specified block as good 
 * It can have additional checks for debugging
 *
 * @param client [IN]
 *   FLASH interface client handle. the same handle returned from flash_ftl_open
 *
 * @param block [IN]
 *	  block number to mark.
 *
 * @param block_state [IN]
 *   The block state value of GOOD or BAD
 *
 * @return FLASH_FTL_STATUS [OUT]
 *   FLASH_FTL_INVALID_PARAM     - if handle is NULL or block is out of partition range
 *   FLASH_FTL_FAIL			   - if internal failure occured
 *   FLASH_FTL_OK			   - if success
 */
FLASH_FTL_STATUS
flash_ftl_core_api_set_block(ftl_pvt_ctx_t *client, uint32 block, enum nand_block_state block_state);

/**
 * flash_ftl_core_open - open a partition and return handle to it
 * It can have additional checks for debugging
 *
 * @param part_name [IN]
 *    name of the partition to be opened
 *
 * @param part_handle [OUT]
 *    pointer to the handle to the partition
 *
 * @return FLASH_FTL_STATUS [OUT]
 *    FLASH_FTL_INVALID_PARAM      - if handle, or part_name is NULL or if
 *                                  part_name is invalid
 *    FLASH_FTL_FAIL               - if internal failure occured
 *    FLASH_FTL_OUT_OF_GOOD_BLOCKS - if partition is not usable
 *    FLASH_FTL_OK                 - if success
 */
FLASH_FTL_STATUS
flash_ftl_core_open(  ftl_pvt_ctx_t **part_handle,
                      const uint8  *part_name);

/**
 * flash_ftl_core_close - close the partition w.r.t. part_handle
 *
 * @param part_handle [IN]
 *   handle of the partition to be closed
 *
 * @return FLASH_FTL_STATUS [OUT]
 *   FLASH_FTL_INVALID_PARAM      - if handle, or part_name is NULL or
 *                                  if part_name is invalid
 *   FLASH_FTL_FAIL               - if internal failure occured
 *   FLASH_FTL_OK                 - if success
 */
FLASH_FTL_STATUS
flash_ftl_core_close( ftl_pvt_ctx_t **part_handle );

/**
 * Reads data from LBA(s)
 *
 * @param handle [IN]
 *   FLASH interface handle. the same handle returned from flash_ftl_open
 *
 * @param lba [IN]
 *   LBA offset of data to be read w.r.t. start of partition
 *   lba is a chunk of data equal to a page (2048/4096)
 *
 * @param lba_count [IN]
 *   Number of LBAs to read
 *
 * @param buffer [OUT]
 *   user data pointer where the read data will be stored
 *
 * @return FLASH_FTL_STATUS [OUT]
 *   FLASH_FTL_INVALID_PARAM      - if handle, or part_name is NULL
 *   FLASH_FTL_FAIL               - if internal failure occured
 *   FLASH_FTL_OUT_OF_GOOD_BLOCKS - if partition is not usable
 *   FLASH_FTL_OK                 - if success
 */
FLASH_FTL_STATUS
flash_ftl_core_read_lbas( ftl_pvt_ctx_t *handle,
                          uint32 lba,
                          uint32 lba_count,
                          uint8 *buffer );

/**
 * Write data to LBA(s)
 *
 * @param handle [IN]
 *   FLASH interface handle. the same handle returned from flash_ftl_open
 *
 * @param lba [IN]
 *   lba to write
 *
 * @param lba_count [IN]
 *   Number of LBAs to program
 *
 * @param buffer [IN]
 *   user data pointer to the buffer which is to be written
 *
 * @return FLASH_FTL_STATUS [OUT]
 *   FLASH_FTL_INVALID_PARAM      - if handle, or part_name is NULL
 *   FLASH_FTL_FAIL               - if internal failure occured
 *   FLASH_FTL_OUT_OF_GOOD_BLOCKS - if partition is not usable
 *   FLASH_FTL_OK                 - if success
 */
FLASH_FTL_STATUS
flash_ftl_core_write_lbas(  ftl_pvt_ctx_t *handle,
                            uint32 lba,
                            uint32 lba_count,
                            uint8 *buffer );

/**
 * This API will erase the erase-region containing the erase_offset
 *
 * @param handle [IN]
 *   FLASH interface handle. the same handle returned from flash_ftl_open
 *
 * @param erase_block [IN]
 *   Start erase block
 *
 * @param erase_block_count [IN]
 *   number of blocks to be erased from flash starting from erase_block
 *
 * @return FLASH_FTL_STATUS [OUT]
 *   FLASH_FTL_INVALID_PARAM      - if handle, or part_name is NULL
 *   FLASH_FTL_FAIL               - if internal failure occured
 *   FLASH_FTL_OUT_OF_GOOD_BLOCKS - if partition is not usable
 *   FLASH_FTL_OK                 - if success
 */
FLASH_FTL_STATUS
flash_ftl_core_erase_block( ftl_pvt_ctx_t *handle,
                            uint32 erase_block,
                            uint32 erase_block_count );

/**
 * This API provides the total number of blocks of a given partition opened by a client.
 *
 * @param handle [IN]
 *   Nand driver's handle. the same handle returned from nand_open
 *
 * @param block_num [OUT]
 *   Total number of blocks in a partition opened by a client.
 *
 * @return FLASH_FTL_STATUS [OUT]
 *   FLASH_FTL_INVALID_PARAM      - if handle, or part_name is NULL
 *   FLASH_FTL_FAIL               - if internal failure occured
 *   FLASH_FTL_OK                 - if success
 */
FLASH_FTL_STATUS
flash_ftl_core_get_block_count(ftl_pvt_ctx_t *handle, uint32 *erase_block_count);

#endif /* #ifndef NAND_FTL_CORE_H */
