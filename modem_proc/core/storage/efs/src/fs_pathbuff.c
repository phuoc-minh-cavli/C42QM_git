/***********************************************************************
 * fs_pathbuff.c
 *
 * Provides array of FS_PATH_MAX sized buffers, a lock is also available
 * to mutually access these buffers.
 *
 * Copyright (C) 2006-2007, 2009 QUALCOMM Technologies, Inc.
 *
 * There are an array of fs_path_buff[] buffers provided, use
 * FS_PATH_BUFF_LOCK() and FS_PATH_BUFF_UNLOCK() to exclusively
 * access these arrays.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.10/storage/efs/src/fs_pathbuff.c#1 $ $DateTime: 2018/06/26 03:18:17 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2009-06-12   ebb   Moved rex_xxx calls to fs_osal.h abstraction.
2007-03-02   umr   Updated copyright year.
2007-01-31    rp   Added fs_temp_buff.
2006-11-02    rp   Create

===========================================================================*/

#include "fs_pathbuff.h"
#include "fs_osal.h"


char fs_path_buff[EFS_MAX_PATH_BUFF_CNT][FS_PATH_MAX + 1];

uint8 fs_temp_buff[EFS_MAX_TEMP_BUFF_CNT][EFS_MAX_TEMP_BUFF_ITEM_SIZE];

/* This instance holds most information about the current rolling-back file
   while playback. Also used while writing write-records. This varilable
   is protected by the FS_PATH_BUFF_LOCK() */
struct fs_rollback_file_info g_fs_rbk_file;


/* Lock for the above buffer */
fs_os_crit_sect_type fs_path_buff_crit_sec;

