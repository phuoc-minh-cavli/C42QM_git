/***********************************************************************
 * fs_flash_usage_stats.c
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

  $Header: //components/dev/core.mpss/3.10/shukum.core.mpss.3.10.twizzy_flash_counter_fr/storage/efs/src/fs_flash_usage_stats.c#1  $ $DateTime: 2019/12/24 01:37:59 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2022-03-31   sk    Create

===========================================================================*/

#include "fs_flash_usage_stats.h"
#include "fs_err.h"
#include "fs_errno.h"
#include "fs_pm_super.h"
#include "fs_global_lock.h"

int fs_get_flash_usage_stats_info(struct fs_flash_usage_stats_info
                                        *fs_flash_stats_info)
{
	uint8 i = 0;
	if (fs_flash_stats_info == NULL)
	  return -EINVAL;
    FS_GLOBAL_LOCK_INIT();
	fs_global_lock();

	fs_flash_stats_info->flash_write_page_stats =
	fs_flash_counter.flash_write_page_counter;
	fs_flash_stats_info->flash_erase_block_stats =
	fs_flash_counter.flash_erase_block_counter;
	for(i=0; i < NUM_FS_FLASHOP_CATEGORIES; i++)
	{
	  fs_flash_stats_info->write_region_stats[i] =
	  fs_flash_color_counter.write_region_counters[i];
	  fs_flash_stats_info->erase_region_stats[i] =
	  fs_flash_color_counter.erase_region_counters[i];
	}
	fs_flash_stats_info->write_time =
	fs_flash_time_counter.write_timings.total_time;
	fs_flash_stats_info->erase_time =
	fs_flash_time_counter.erase_block_timings.total_time;

    fs_global_unlock();

	return 0;
}

int fs_get_block_info(struct fs_block_info_t *fs_block_info)
{
  int res = -EINVAL;
  if (fs_block_info == NULL)
    return res;
  res = fs_super_get_info (fs_block_info);
  if (res != 0)
    res = -EPERM;

  return res;
}
