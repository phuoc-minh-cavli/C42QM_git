#ifndef __NAND_FTL_API_H
#define __NAND_FTL_API_H

/**********************************************************************
 * NandFtlApi.h
 *
 * Flash translation layer (FTL) extended functions used internally in the nand DXE driver.
 *
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
 * when         who     what, where, why
 * ----------   ---     ------------------------------------------------
 * 2017-03-03   eo     FTL layer support
 *=======================================================================*/

/**  \mainpage Flash FTL Functional Overview
 *
 * The Flash FTL layer provides read/write in bytes/lbas and erase in blocks
 *
 * \n
 *
 */

#include <NandFtlTypes.h>
#include <nand_api.h>


/****************************************************************
 * APIs
 ***************************************************************/
/**
 * This API will close all flash ftl client partitions and deinit the flash nand and bam drivers.
 *
 * @no param
 *
 * @return FLASH_FTL_STATUS [OUT]
 *     FLASH_FTL_FAIL             - if internal failure occured
 *     FLASH_FTL_OK               - if success
 */
FLASH_FTL_STATUS flash_ftl_api_deinit( void );

/**
 * This API will mark the specified block's state in the FTL's bad block table(bbt) as GOOD or BAD. If the
 *  given partition is already opened in FTL.
 *
 * @param handle [IN]
 *   Nand driver's handle. the same handle returned from nand_open
 *
 * @param block_num [IN]
 *   The block number to mark.
 *
 * @param block_state [IN]
 *   The block state value of GOOD or BAD
 *
 * @return FLASH_FTL_STATUS [OUT]
 *   FLASH_FTL_INVALID_PARAM      - if handle, or part_name is NULL
 *   FLASH_FTL_FAIL               - if internal failure occured
 *   FLASH_FTL_OUT_OF_GOOD_BLOCKS - if partition is not usable
 *   FLASH_FTL_OK                 - if success
 */
FLASH_FTL_STATUS flash_ftl_api_set_block( nand_handle_t nand_handle,
                                            uint32 block_num,
                                            enum nand_block_state block_state);

#endif /* __NAND_FTL_API_H */
