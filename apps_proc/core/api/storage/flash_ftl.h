#ifndef __FLASH_FTL_H
#define __FLASH_FTL_H

/**********************************************************************
 * flash_ftl.h
 *
 * Public interface declaration for the Flash Translation layer (FTL).
 * FTL core provides APIs for raw NAND read/write/erase access
 * and is responsible for bad block management and logical to
 * physical block conversation etc.
 *
 *
 * Copyright (c) 2016-2017 Qualcomm Technologies Incorporated.
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
 * 2017-03-10   svl     Updated comments to APIs
 * 2016-11-11   svl     Review comments incorporated.
 * 2016-10-03   svl     Updated comments to APIs
 * 2016-08-24   svl     FTL layer support
 *=======================================================================*/

/**  \mainpage Flash FTL Functional Overview
 *
 * In NAND page read/write and block erase can be done
 * The Flash FTL layer extend APIs to read/write in bytes/lpas
 * and erase in blocks on the existing flash layer, with taking care of
 * bad blocks.
 * LPA (logic page address) stand for a page to be read/written
 * \n
 *
 */

#include <flash_ftl_types.h>

/****************************************************************
 * Data structure definitions
 ***************************************************************/

#define FTL_DEVICE_NAME_SIZE                (32)

typedef struct flash_ftl_info
{
  uint8  device_name[FTL_DEVICE_NAME_SIZE];  /**< Device name string */
  uint32 maker_id;                   /**< Manufacturer Identification */
  uint32 device_id;                  /**< Device Identification */
  uint32 lpa_count;                  /**< Number of LPAs/pages in device */
  uint32 lpa_size_in_kbytes;         /**< LPA/page size in Kilo bytes */
  uint32 erase_block_count;          /**< Number of eraseable units in partition */
  uint32 erase_block_size_in_kbytes; /**< Erase unit size in kbyets) */
}flash_ftl_info_t;

/** handle returned to the client.
    per partition one handle is returned
*/
typedef void *flash_ftl_client_t;

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
FLASH_FTL_STATUS
  flash_ftl_open( flash_ftl_client_t *handle,
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
FLASH_FTL_STATUS
  flash_ftl_close(flash_ftl_client_t *handle);


/**
 * return partition and client specific info in format specified
 * by "flash_ftl_info_t"
 * flash_ftl_info_t can be used to get partition information like size
 * NOTE : total usable partition size in kbytes = lpa_size_in_kbytes*lpa_count
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
FLASH_FTL_STATUS
  flash_ftl_get_info( flash_ftl_client_t  handle,
                      flash_ftl_info_t    *info);

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
FLASH_FTL_STATUS
  flash_ftl_read_bytes( flash_ftl_client_t handle,
                        uint32 byte_offset,
                        uint32 byte_count,
                        uint8  *buffer);

/**
 * Reads data in multiples of LPA(s)/pages
 *
 * @param handle [IN]
 *   FLASH interface handle. the same handle returned from flash_ftl_open
 *
 * @param lpa [IN]
 *   Logical Page Address (or page number) from which the data is to be read.
 *   the LPA is w.r.t. start of partition
 *
 * @param lpa_count [IN]
 *   Number of LPAs/pages to read
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
  flash_ftl_read_lpa( flash_ftl_client_t handle,
                      uint32 lpa,
                      uint32 lpa_count,
                      uint8  *buffer);

/**
 * Write data in multiples of LPA(s)/pages sequentially.
 * Number of lpa's in a block = (erase_block_size_in_kbytes/lpa_size_in_kbytes)
 * Data can only be written in an erased block, so before writing 
 * in an lpa, the block which it correspond to should be erased by calling
 * flash_ftl_erase_block().
 * for e.g. if a block has 4 lpas and the block is not erased and the user
 * wants to write in lpa 0 then user has to erase the whole block first and then
 * write. Now because the whole block is erased user doesn't need to erase before
 * writing in lpa1-lpa3.
 * Note: Only sequential write are allowed.
 * If the user wants to write in lpa0 after writing in lpa1 then user needs to erase
 * whole block; the data in the whole block is lost in this case.
 * if user wants to write into previously written lpa then backup of whole block
 * erase and again copy of backedup data is users responsibility.
 * i.e. if user wants to write in lpa0 after writing in lpa3 in above
 * e.g. then user needs to do below sequence.
 *  - backup whole block if required,
 *  - erase whole block using flash_ftl_erase_block()
 *  - write into lpa0
 *  - write back lpa1-3 if a backup was taken before
 *
 * FTL does not take ownership of data loss in case where
 * sequential write is not followed.
 *
 * Ideally user should erase the whole partition first and
 * then start writing data sequentially.
 *
 * @param handle [IN]
 *   FLASH interface handle. the same handle returned from flash_ftl_open
 *
 * @param lpa [IN]
 *   Logical Page Address (or page number) where the data is to be written.
 *   the LPA is w.r.t. start of partition
 *
 * @param lpa_count [IN]
 *   Number of LPAs/pages to write
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
  flash_ftl_write_lpa( flash_ftl_client_t handle,
                       uint32 lpa,
                       uint32 lpa_count,
                       uint8  *buffer);

/**
 * This API will erase a block.
 * block size is defined by erase_block_size_in_kbytes
 * Number of lpa's in a block = (erase_block_size_in_kbytes/lpa_size_in_kbytes)
 * Data can only be written in an erased block, so before writing 
 * in an lpa, the block which it correspond to should be erased with this API.
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
  flash_ftl_erase_block(  flash_ftl_client_t handle,
                          uint32 erase_block,
                          uint32 erase_block_count);

#endif /* __FLASH_FTL_H */
