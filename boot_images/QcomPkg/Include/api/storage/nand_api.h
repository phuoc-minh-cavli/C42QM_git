#ifndef LOADER_CODE  //{
#ifndef NAND_API_H
#define NAND_API_H  //{

/**********************************************************************
 * nand_api.h
 *
 * Public interface declaration for the Flash NAND Driver.
 *
 * This file describes the Flash NAND external interface.
 *
 * Copyright (c) 2017 Qualcomm Technologies Incorporated.
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
 *
 * when         who     what, where, why
 * ----------   ---     ------------------------------------------------
 * 02-07-2017   eo      Initial version
 *=======================================================================*/

/**  \mainpage Flash NAND Functional Overview
 *
 * The Flash NAND driver provides a high-level interface for clients, such
 * as bootloaders, file systems, and tools to interact with the flash NAND devices. It
 * handles the Flash NAND device specifics and abstracts with the client from the device-
 * specific algorithms, interfaces, and OS adaptations.
 *
 * \n
 *
 */
#ifdef NAND_TYPES_API_HEADER
  #include NAND_TYPES_API_HEADER
#endif
#include "com_dtypes.h"

#define NAND_API_VERSION                (0x00000001)  /** flash NAND API version */

/** Flash operation status */
#define NAND_DEVICE_DONE                 0   /**< Operation passed */
#define NAND_DEVICE_FAIL                (-1) /**< Operation failed */
#define NAND_DEVICE_NOT_SUPPORTED       (-2) /**< Device/operation not supported */
#define NAND_DEVICE_INVALID_PARAMETER   (-3) /**< API parameters invalid */
#define NAND_DEVICE_IMAGE_NOT_FOUND     (-4) /**< FW Image ID not found */
#define NAND_DEVICE_NOT_FOUND           (-5) /**< Device not found on supported device list */
#define NAND_DEVICE_FAIL_PAGE_ERASED    (-6) /**< Page read is erased */
#define NAND_DEVICE_NO_MEM              (-7) /**< Insufficient memory */

typedef int NAND_STATUS;                  /**< Error status values used in FLASH driver */

#define NAND_DEVICE_NAME_SIZE           (32)

/** FLASH NAND device IDs to use in nand_open() API. */
#define NAND_DEVICE_ID_PARALLEL_FLASH   (0) /**< Parallel SLC NAND FLASH devices */

/** Holds the new and previous Multi-Image Boot Information Block (MIBIB)
    block number. This structure is used to keep track of current and previous
  MIBIB block numbers. \n
    @note This is primarily intended to be used in flash tools during
    software download and in flash scrub for MIBIB partition table updates. \n
 */
struct nand_mibib
{
  int new_mibib_block;                 /**< New MIBIB block number */
  int old_mibib_block;                 /**< Old MIBIB block number */
};

/** Flash nand client device data */
struct nand_info
{
  uint32  version;                      /**< Version info of this structure */
  uint8   device_name[NAND_DEVICE_NAME_SIZE];  /**< Device name string */
  uint32  maker_id;                     /**< Manufacturer Identification */
  uint32  device_id;                    /**< Device Identification */
  uint32  device_version;               /**< Device Version ID, if any */
  uint32  block_count;                  /**< Number of total blocks for this partition/image */
  uint16  pages_per_block;              /**< Number of pages in a block */
  uint16  page_size_bytes;              /**< Page size in bytes */
  uint32  total_page_size_bytes;        /**< Page size in bytes including spare */
  uint32  max_spare_udata_bytes;        /**< Max ECC covered user data bytes in spare */
  uint32  ecc_mode;                     /**< Number of bits correctable when ECC enabled */																			   	
  uint32  attributes;                   /**< Attributes of this partition. This
                                                                           field can be decoded by including mibib.h */
  struct nand_mibib  mibib_info;        /**< MIBIB information */
};

/** Type of read and write to be used with flash nand read API */
enum page_access_opcode
{
  NAND_ACCESS_MAIN = 0,        /**< Read/Write only main data. main_data is not NULL */
  NAND_ACCESS_SPARE,	       /**< Read/Write only spare data. spare_data is not NULL,
								  spare_size_bytes is set */
  NAND_ACCESS_MAIN_SPARE,      /**< Read/Write both main and spare data. main_data and
                                                           spare_data is not NULL, spare_size_bytes is set */
  NAND_ACCESS_RAW,             /**< Read/Write all RAW bytes in page(with ECC disabled). main_data
                                                          is not NULL, main_size_bytes includes main+spare space */
  NAND_ACCESS_UNKNOWN = 0x7FFFFFFF /* Force 32-bit enum */
};

/** Flash nand_block state. This indicates the state of a block. */
enum nand_block_state
{
  NAND_BLOCK_OK  = 0x1,       /**< Flash Nand Block is Good */
  NAND_BLOCK_BAD = 0x2,       /**< Flash Nand Block is Bad */
  NAND_BLOCK_STATE_UNKNOWN = 0x7FFFFFFF /* Force 32-bit enum */
};

typedef void *nand_handle_t;

/**
 * Opens handle to a flash nand partition.
 *
 * @param dev_id [IN]
 *   Device ID that specifies a FLASH NAND device.
 *
 * @param partition_name [IN]
 *   Name of the partition to open. requires null terminated string name
 *
 * @param handle [OUT]
 *   FLASH NAND interface handle.
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */
NAND_STATUS nand_open(uint32 dev_id, const uint8 *partition_name, 
  nand_handle_t *handle);

/**
 * Closes handle to a flash nand partition.
 * Clients should close the flash partition when accesses to the partition is
 * no longer needed.
 *
 * @param handle [IN]
 *   FLASH NAND interface handle.
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */
NAND_STATUS nand_close(nand_handle_t handle);

/**
 * Gets flash device information for a given partition. Flash driver assumes that the memory
 * for returned information is allocated by the driver client.
 * \n
 *
 * @param handle [IN]
 *   FLASH NAND interface handle
 *
 * @param flash_info [OUT]
 *   Pointer to the client's flash device info data.
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */
NAND_STATUS nand_get_info(nand_handle_t handle, struct nand_info *nand_info);

/**
 * Sets state to a given block.
 *
 * @param handle [IN]
 *   FLASH NAND interface handle
 *
 * @param block [IN]
 *   The logical block number
 *
 * @param block_state [IN]
 *   The block state to be set
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */
NAND_STATUS nand_block_set_state(nand_handle_t handle,
  uint32 block, enum nand_block_state block_state);

/**
 * Gets the current state of a given block.
 *
 * @param handle [IN]
 *   FLASH NAND interface handle
 *
 * @param block [IN]
 *   The logical block number
 *
 * @param block_state [OUT]
 *   Pointer to the block state buffer
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */
NAND_STATUS nand_block_get_state(nand_handle_t handle,
  uint32 block, enum nand_block_state *block_state);

/**
 * Erases given flash nand blocks
 *
 * @param handle [IN]
 *   FLASH NAND interface handle
 *
 * @param start_block [IN]
 *   A starting logical block for a number of blocks to erase.
 *
 * @param block_count [IN]
 *   Number of blocks to erase
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */
NAND_STATUS nand_erase(nand_handle_t handle,
  uint32 start_block, uint32 block_count);

/**
 * Allows write data to the flash.
 *
 * @param handle [IN]
 *   FLASH NAND interface handle
 *
 * @param opcode [IN]
 *   The flash device page opcode to use for this access
 *
 * @param start_page [IN]
 *   Starting logical page from the beginning of the partition/image to write to.
 *
 * @param page_count [IN]
 *   Number of pages to write data.
 *
 * @buffer [IN]
 *   data buffer for flash write operation.
 *
 * @buffer_size [IN]
 *   data buffer size in bytes for flash write operation.
 *
 * @return int [OUT]
 *   Result of the operation.
 *
 */
NAND_STATUS nand_write(nand_handle_t handle,
  enum page_access_opcode opcode, uint32 start_page, uint32 page_count, void *buffer, uint32 buffer_size);

/**
 * Allows read data from the flash
 *
 * @param handle [IN]
 *   FLASH NAND interface handle
 *
 * @param opcode [IN]
 *   The flash device page opcode to use for this access
 *
 * @param start_page [IN]
 *   Starting logical page from the beginning of the partition/image to read from.
 *
 * @param page_count [IN]
 *   Number of pages to read data.
 *
 * @buffer [OUT]
 *   data buffer for a flash read operation.
 *
 * @buffer_size [IN]
 *   data buffer size in bytes for flash read operation.
 *
 * @return int [OUT]
 *   Result of the operation.
 */
NAND_STATUS nand_read(nand_handle_t handle,
  enum page_access_opcode opcode, uint32 start_page, uint32 page_count, void *buffer, uint32 buffer_size);
// #else  //}{
    // #warning ###########################################
#endif  //}
#endif //}/* #ifndef NAND_API_H */
