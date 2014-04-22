/***********************************************************************
 * fs_golden_copy_files_list.c
 *
 * OEM's EFS-golden-copy files-list.
 * Copyright (C) 2016 QUALCOMM Technologies, Inc.
 *
 * OEM's EFS-golden-copy files-list.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/storage/efs/src/fs_golden_copy_files_list.c#1 $ $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2016-05-23   rp    Create

===========================================================================*/
#include "fs_config_i.h"
#include "fs_golden_copy_files_list.h"
#include "fs_priv_funcs.h"
#include "fs_err.h"
#include "fs_util.h"

#include "assert.h"

const char *fs_golden_copy_oem_1_files_list[] = {
  "/oem_1/home/oem_1_file_1",
  "/oem_1/home/oem_1_file_2",
};

const char *fs_golden_copy_oem_2_files_list[] = {
  "/oem_2/home/oem_2_file_1",
  "/oem_2/home/oem_2_file_2",
  "/oem_2/home/oem_2_file_3",
  "/oem_2/home/oem_2_file_4",
  "/oem_2/home/oem_2_file_5",
};

void fs_golden_copy_files_list_init (void)
{
  uint32 count;
  int result;

  count = (sizeof(fs_golden_copy_oem_1_files_list) /
           sizeof(fs_golden_copy_oem_1_files_list[0]));

  result = efs_golden_copy_set_oem_files_list (FS_GOLDEN_COPY_FILES_LIST_OEM_1,
                                       count, fs_golden_copy_oem_1_files_list);
  ASSERT (result == 0);

  count = (sizeof(fs_golden_copy_oem_2_files_list) /
           sizeof(fs_golden_copy_oem_2_files_list[0]));

  result = efs_golden_copy_set_oem_files_list (FS_GOLDEN_COPY_FILES_LIST_OEM_2,
                                       count, fs_golden_copy_oem_2_files_list);
  ASSERT (result == 0);
}

