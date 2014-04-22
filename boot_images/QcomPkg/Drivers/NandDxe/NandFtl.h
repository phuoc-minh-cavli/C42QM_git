#ifndef __NAND_FTL_H
#define __NAND_FTL_H

/**********************************************************************
 * NandFtl.h
 *
 * Public interface declaration for the Flash Translation layer (FTL).
 * This layer is responsible for bad block management and logical to
 * physical block conversation etc.
 *
 *
 * Copyright (c) 2016 Qualcomm Technologies Incorporated.
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
 * 2016-10-03   svl     Updated comments to APIs
 * 2016-08-24   svl     FTL layer support
 *=======================================================================*/

/**  \mainpage Flash FTL Functional Overview
 *
 * The Flash FTL layer provides read/write in bytes/lbas and erase in blocks
 *
 * \n
 *
 */

#include <NandFtlTypes.h>

/****************************************************************
 * Data structure definitions
 ***************************************************************/

#define FTL_DEVICE_NAME_SIZE                (32)

typedef struct flash_ftl_info
{
  uint8                 device_name[FTL_DEVICE_NAME_SIZE];  /**< Device name string */
  uint32                maker_id;                   /**< Manufacturer Identification */
  uint32                device_id;                  /**< Device Identification */
  uint32                lba_count;                  /**< Number of LBAs in device */
  uint32                lba_size_in_kbytes;         /**< LBA size in Kilo bytes */
  uint32                erase_block_count;          /**< Number of eraseable units in partition */
  uint32                erase_block_size_in_kbytes; /**< Erase unit size in kbyets) */
}flash_ftl_info_t;

typedef void *flash_ftl_client_t; /**< handle returned to the client. */
                                  /**< per partition one handle is returned */

/****************************************************************
 * APIs
 ***************************************************************/

/**
 * This is the first API that client should call before any other call to this module
 * This API opens the requested partition and returns handle to that partition
 * This handle will be a void pointer and won't have any data exposed by itself
 * The handle should be used with FTL APIs to get work done.
 * for e.g. use this handle with flash_ftl_get_info to get ftl info in format flash_ftl_info_t
 * similarly to read/write data this handle should be passed with correct offset and size.
 * NOTE: one handle will be returned per partition.
 *
 * @param part_name [IN]
 *   This is the name of the partition that client wants to use
 *
 * @param handle [OUT]
 *   This is the handle that will be passed to client for further usage
 *   client needs to pass adress of the pointer in which this handle is to be stored
 *   if the return status is FLASH_FTL_OK then "handle" will contain handle to the partition
 *   and as described above this handle is used for any read/write operation on partition part_name
 *
 * @return FLASH_FTL_STATUS [OUT]
 *   FLASH_FTL_INVALID_PARAM      - if handle, or part_name is NULL or if part_name is invalid
 *   FLASH_FTL_FAIL               - if internal failure occured
 *   FLASH_FTL_OUT_OF_GOOD_BLOCKS - if partition is not usable
 *   FLASH_FTL_OK                 - if success
 */
FLASH_FTL_STATUS flash_ftl_open(flash_ftl_client_t *handle,
                                  const uint8 *part_name);

/**
 * Client should call this API to close the partition once it is done with it.
 *
 * @param handle [IN]
 *   handle of the partition to be closed
 *
 * @return FLASH_FTL_STATUS [OUT]
 *   FLASH_FTL_INVALID_PARAM      - if handle, or part_name is NULL or
 *                                  if part_name is invalid
 *   FLASH_FTL_FAIL               - if internal failure occured
 *   FLASH_FTL_OK                 - if success
 */
FLASH_FTL_STATUS flash_ftl_close(flash_ftl_client_t *handle);


/**
 * return partition and client specific info in format specified
 * by "flash_ftl_info_t"
 * flash_ftl_info_t can be used to get partition information like size
 * NOTE : total usable partition size in kbytes = lba_size_in_kbytes*lba_count
 *
 * @param handle [IN]
 *   the same handle returned from flash_ftl_open
 *
 * @param info [OUT]
 *   pointer where the info would be stored
 *
 * @return FLASH_FTL_STATUS [OUT]
 *   FLASH_FTL_INVALID_PARAM      - if handle, or info is NULL
 *   FLASH_FTL_OK                 - if success
 */
FLASH_FTL_STATUS flash_ftl_get_info(  flash_ftl_client_t handle,
                                      flash_ftl_info_t *info);

/**
 * This API reads data (in bytes) starting from "byte_offset" (offset w.r.t. to start of partition)
 *
 * @param handle [IN]
 *   FLASH interface handle. the same handle returned from flash_ftl_open
 *
 * @param byte_offset [IN]
 *   offset of data to be read w.r.t. start of partition
 *
 * @param byte_count [IN]
 *   size of data to be read in bytes 
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
FLASH_FTL_STATUS flash_ftl_read_bytes( flash_ftl_client_t handle,
                                       uint32 byte_offset,
                                       uint32 byte_count,
                                       uint8 *buffer);

/**
 * Reads data from LBA(s)
 *
 * @param handle [IN]
 *   FLASH interface handle. the same handle returned from flash_ftl_open
 *
 * @param lba [IN]
 *   LBA offset of data to be read w.r.t. start of partition
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
FLASH_FTL_STATUS flash_ftl_read_lba( flash_ftl_client_t handle,
                                     uint32 lba,
                                     uint32 lba_count,
                                     uint8 *buffer);

/**
 * Write data to LBA(s) sequentially.
 * Number of lba's in a block = (erase_block_size_in_kbytes/lba_size_in_kbytes)
 * Data can only be written in an erased block, so before writing 
 * in an lba, the block which it correspond to should be erased by calling
 * flash_ftl_erase_block().
 * for e.g. if a block has 4 lbas. if the block is not erased and the user
 * wants to write in lba 0 then user has to erase the whole block first and then
 * write. Now because the whole block is going to be erased it is the users
 * responsibility to backup lba1-lba3. once the block is erased user can write
 * back lba1-lba3 without calling erase again as the block is just erased.
 * Note: after writing in lba0 if the user wants to write again in the same
 * location then user has to repeat above procedure i.e.
 *  - backup whole block if required,
 *  - erase whole block using flash_ftl_erase_block()
 *  - write into lba0
 *  - write back lba1-3 if a backup was taken before
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
FLASH_FTL_STATUS flash_ftl_write_lba(flash_ftl_client_t handle,
                                     uint32 lba,
                                     uint32 lba_count,
                                     uint8 *buffer);

/**
 * This API will erase a block whose size is defined by
 * erase_block_size_in_kbytes
 * Number of lba's in a block = (erase_block_size_in_kbytes/lba_size_in_kbytes)
 * Data can only be written in an erased block, so before writing 
 * in an lba, the block which it correspond to should be erased with this API.
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
FLASH_FTL_STATUS flash_ftl_erase_block( flash_ftl_client_t handle,
                                        uint32 erase_block,
                                        uint32 erase_block_count);

#endif /* __NAND_FTL_H */
