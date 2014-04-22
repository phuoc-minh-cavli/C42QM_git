/***********************************************************************
 * fs_flash_usage_stats.h
 *
 * This file holds the counters to track flash operations.
 * It also holds flash block related info.
 * Copyright (c) 2022 by QUALCOMM Technologies, Inc.  All Rights Reserved.
 *
 * This file exposes the counters to track the statistics of write/erase
 * operations on the flash.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/dev/core.mpss/3.10/shukum.core.mpss.3.10.twizzy_flash_counter_fr/api/storage/fs_flash_usage_stats.h#1 $ $DateTime: 2019/12/24 01:37:59 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2022-03-31   sk    Create

===========================================================================*/

#ifndef __FS_FLASH_STATS_H__
#define __FS_FLASH_STATS_H__

#include "comdef.h"

/* Same as NUM_FS_FLASHOP_CATEGORIES in enum fs_flashop_category */
#define NUM_FLASHOP_CATEGORIES 11

/* Below are the NUM_FLASHOP_CATEGORIES types of flash categorizations. */
/******************************
  FS_FOP_SUPER,
  FS_FOP_USER,
  FS_FOP_LOG,
  FS_FOP_DATA_GC_MOVE,
  FS_FOP_DATA_GC_RECOVERY,
  FS_FOP_USER_RECOVERY,
  FS_FOP_PTABLE_GC_MOVE,
  FS_FOP_PTABLE,
  FS_FOP_FACTORY_RECOVERY,
  FS_FOP_UNKNOWN,
  FS_FOP_BIRDVIEW
******************************/


/* Write, Erase stat info */
PACKED struct fs_flash_usage_stats_info
{
  uint32 flash_write_page_stats; /* Overall Write counter */
  uint32 flash_erase_block_stats; /* Overall Erase counter */
  /* Write counter for each category */
  uint32 write_region_stats[NUM_FLASHOP_CATEGORIES];
  /* Erase counter for each category */
  uint32 erase_region_stats[NUM_FLASHOP_CATEGORIES];
  uint32 write_time; /* Write time counter */
  uint32 erase_time; /* Erase time counter */
}PACKED_POST;

/* Block info */
PACKED struct fs_block_info_t
{
  uint32 pages_per_block;
  uint32 log_block_count;
}PACKED_POST;


/* Get the flash counter stats
 *  @param fs_flash_stats_info
 *    The structure having stat info.
 *  stat info refers to flash_write_page_stats, flash_erase_block_stats,
 *  write_region_stats for each category, erase_region_stats for each
 *  category, write_time and erase_time. The stat info is not persistent
 *  across reboots.
 *
 * @return
 *  This function returns 0 on success or -EINVAL in case of error.
 *
 * @note
 *  This feature is flash device specific and may not be supported on all
 *  flash devices.
 *
 */
int fs_get_flash_usage_stats_info(struct fs_flash_usage_stats_info
                                        *fs_flash_stats_info);

/* Get the log region block count and number of pages per block.
 *  @param fs_block_info
 *    The structure having block info.
 *
 * @return
 *  This function returns 0 on success or error code (-EINVAL or -EPERM) on
 *  failure.
 *
 * @note
 *  This feature is flash device specific and may not be supported on all
 *  flash devices.
 *
 */
int fs_get_block_info(struct fs_block_info_t *fs_block_info);

#endif /* not __FS_FLASH_STATS_H__ */
