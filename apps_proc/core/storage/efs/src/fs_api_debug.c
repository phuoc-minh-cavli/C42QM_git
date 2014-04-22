/***********************************************************************
 * fs_api_debug.c
 *
 * Functionality to debug calls for EFS APIs.
 * Copyright (C) 2017-2019 QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2019-10-17   eo   Enhance EFS API Debug framework. Version=2.
2019-10-09   rp   Fix the mutex locking order between fs_lib and rest.
2018-10-10   eo   Add FS API debug info support
2017-09-22   np   Create

===========================================================================*/

#include "fs_config_i.h"
#include "fs_api_debug.h"

#ifdef FS_API_DEBUG_DISABLE
  int fs_api_debug_disable = 1;
#else
  int fs_api_debug_disable = 0;
#endif

#ifndef FS_API_DEBUG_DISABLE

#include "fs_public.h"
#include "fs_priv_funcs.h"
#include "fs_util.h"
#include "fs_osal.h"
#include "fs_err.h"
#include "fs_errno.h"
#include "fs_timetick.h"
#include "stringl/stringl.h"

#include <stdlib.h>


/* Magic and Version for EFS Counters file */
#ifndef FS_API_DEBUG_COUNTERS_MAGIC
  #define FS_API_DEBUG_COUNTERS_MAGIC   (0x44525443)
#endif
#ifndef FS_API_DEBUG_COUNTERS_VERSION
  #define FS_API_DEBUG_COUNTERS_VERSION (1)
#endif

/* Magic and Version for EFS API logs file */
#ifndef FS_API_DEBUG_EFS_API_MAGIC
  #define FS_API_DEBUG_EFS_API_MAGIC (0x44534645)
#endif
#ifndef FS_API_DEBUG_EFS_API_VERSION
  #define FS_API_DEBUG_EFS_API_VERSION (1)
#endif

/* Magic and Version for EFS Sync logs file */
#ifndef FS_API_DEBUG_EFS_SYNC_MAGIC
  #define FS_API_DEBUG_EFS_SYNC_MAGIC (0x434E5953)
#endif
#ifndef FS_API_DEBUG_EFS_SYNC_VERSION
  #define FS_API_DEBUG_EFS_SYNC_VERSION (1)
#endif


extern fs_os_tcb_type *fs_task_tcb;

static uint8 fs_api_debug_apis_to_track[FS_API_DEBUG_ID_MAX] =
{
  0, /* FS_API_DEBUG_ID_UNUSED                          = 0,  */
  1, /* FS_API_DEBUG_ID_EFS_OPEN                        = 1,  */
  1, /* FS_API_DEBUG_ID_EFS_CLOSE                       = 2,  */
  0, /* FS_API_DEBUG_ID_EFS_READ                        = 3,  */
  1, /* FS_API_DEBUG_ID_EFS_WRITE                       = 4,  */
  1, /* FS_API_DEBUG_ID_EFS_MKDIR                       = 5,  */
  1, /* FS_API_DEBUG_ID_EFS_PUT                         = 6,  */
  0, /* FS_API_DEBUG_ID_EFS_GET                         = 7,  */
  1, /* FS_API_DEBUG_ID_EFS_SYNC                        = 8,  */
  1, /* FS_API_DEBUG_ID_EFS_SYNC_NO_WAIT                = 9, */
  0, /* FS_API_DEBUG_ID_EFS_SYNC_GET_STATUS             = 10, */
  0, /* FS_API_DEBUG_ID_EFS_STATVFS                     = 11, */
  0, /* FS_API_DEBUG_ID_EFS_FSTATVFS                    = 12, */
  1, /* FS_API_DEBUG_ID_EFS_RENAME                      = 13, */
  1, /* FS_API_DEBUG_ID_EFS_UNLINK                      = 14, */
  1, /* FS_API_DEBUG_ID_EFS_RMDIR                       = 15, */
  1, /* FS_API_DEBUG_ID_EFS_DELTREE                     = 16, */
  1, /* FS_API_DEBUG_ID_EFS_SYMLINK                     = 17, */
  0, /* FS_API_DEBUG_ID_EFS_READLINK                    = 18, */
  1, /* FS_API_DEBUG_ID_EFS_CHMOD                       = 19, */
  1, /* FS_API_DEBUG_ID_EFS_CHOWN                       = 20, */
  1, /* FS_API_DEBUG_ID_CHOWN_RECURSIVE                 = 21, */
  1, /* FS_API_DEBUG_ID_CHMOD_RECURSIVE                 = 22, */
  1, /* FS_API_DEBUG_ID_EFS_UTIME                       = 23, */
  0, /* FS_API_DEBUG_ID_DIAG_FS_REGISTER_ACCESS_CHECK   = 24, */
  0, /* FS_API_DEBUG_ID_EFS_OPENDIR                     = 25, */
  0, /* FS_API_DEBUG_ID_EFS_READDIR                     = 26, */
  0, /* FS_API_DEBUG_ID_EFS_CLOSEDIR                    = 27, */
  1, /* FS_API_DEBUG_ID_EFS_PREPARE_SHUTDOWN            = 28, */
  1, /* FS_API_DEBUG_ID_EFS_RCINIT_GROUP7_SHUTDOWN_CB   = 29, */
  1, /* FS_API_DEBUG_ID_EFS_RCINIT_SHUTDOWN_CB          = 30, */
  1, /* FS_API_DEBUG_ID_EFS_MAKE_GOLDEN_COPY            = 31, */
  1, /* FS_API_DEBUG_ID_EFS_MAKE_GOLDEN_COPY_NO_WAIT    = 32, */
  1, /* FS_API_DEBUG_ID_EFS_GOLDEN_COPY_SET_FILES_LIST  = 33, */
  1, /* FS_API_DEBUG_ID_EFS_STORE_DATA_IN_SUPERBLOCK    = 34, */
  1, /* FS_API_DEBUG_ID_EFS_FILESYSTEM_IMAGE_OPEN       = 35, */
  1, /* FS_API_DEBUG_ID_EFS_FILESYSTEM_IMAGE_PREPARE    = 36, */
  0, /* FS_API_DEBUG_ID_EFS_SCRUB                       = 37, */
  1, /* FS_API_DEBUG_ID_EFS_FULL_SCRUB                  = 38, */
  1, /* FS_API_DEBUG_ID_EFS_SHRED                       = 39, */
  1, /* FS_API_DEBUG_ID_EFS_CREAT                       = 40, */
  0, /* FS_API_DEBUG_ID_EFS_STAT                        = 41, */
  0, /* FS_API_DEBUG_ID_EFS_FSTAT                       = 42, */
  0, /* FS_API_DEBUG_ID_EFS_LSTAT                       = 43, */
  0, /* FS_API_DEBUG_ID_EFS_LSEEK                       = 44, */
  0, /* FS_API_DEBUG_ID_EFS_LSEEK64                     = 45, */
  1, /* FS_API_DEBUG_ID_EFS_TRUNCATE                    = 46, */
  1, /* FS_API_DEBUG_ID_EFS_TRUNCATE64                  = 47, */
  1, /* FS_API_DEBUG_ID_EFS_FTRUNCATE                   = 48, */
  1, /* FS_API_DEBUG_ID_EFS_FTRUNCATE64                 = 49, */
  1, /* FS_API_DEBUG_ID_EFS_LOCK_FS                     = 50, */
  1, /* FS_API_DEBUG_ID_EFS_UNLOCK_FS                   = 51, */
};

static struct fs_api_debug_info_type fs_api_debug_info;


static int
fs_api_debug_read_uint32_from_efs_file (const char *filename, uint32 *ret_val)
{
  int result, oflag, fd = -1;
  uint32 val, val_size;

  FS_ASSERT (ret_val != NULL);
  *ret_val = 0;

  oflag = O_RDONLY;

  val_size = sizeof(uint32);

  result = efs_open (filename, oflag);
  if (result < 0)
  {
    result = efs_errno;
    if (result != ENOENT)
    {
      FS_MSG_ERROR_1("efs_open failed [%d]", result);
    }
    return -1;
  }

  fd = result;
  result = efs_read (fd, &val, val_size);
  if (result < 0)
  {
    result = efs_errno;
    FS_MSG_ERROR_1("efs_read failed [%d]", result);
    goto cleanup;
  }

  if ((uint32)result != val_size)
  {
    result = -1;
    goto cleanup;
  }

  *ret_val = val;
  result = 0;


cleanup:
  if (fd >= 0)
  {
    (void) efs_close (fd);
  }

  return result;
}

static int
fs_api_debug_read_counters_file (struct fs_api_debug_info_type *api_dbg)
{
  const char *filename;
  int result, create_file;

  FS_ASSERT (api_dbg != NULL);

  create_file = 0;
  filename = FS_API_DEBUG_COUNTERS_FILE;
  result = efs_read_file_helper (filename,
           &api_dbg->counters, sizeof(api_dbg->counters));
  if (result == 0)
  {
    if (api_dbg->counters.magic != FS_API_DEBUG_COUNTERS_MAGIC)
      create_file = 1;
    if (api_dbg->counters.version != FS_API_DEBUG_COUNTERS_VERSION)
      create_file = 1;
    if (api_dbg->counters.struct_size != sizeof(api_dbg->counters))
      create_file = 1;
  }
  else
  {
    create_file = 1;
    result = efs_errno;
    if (result != ENOENT)
    {
      FS_MSG_ERROR_1("efs_read_file_helper failed [%d]", result);
    }
  }

  if (create_file)
  {
    memset(&api_dbg->counters, 0, sizeof(api_dbg->counters));
    api_dbg->counters.magic = FS_API_DEBUG_COUNTERS_MAGIC;
    api_dbg->counters.version = FS_API_DEBUG_COUNTERS_VERSION;
    api_dbg->counters.struct_size = sizeof(api_dbg->counters);
  }

  return 0;
}

static int
fs_api_debug_read_efs_api_file (struct fs_api_debug_info_type *api_dbg)
{
  const char *filename;
  uint32 hdr_size, slots_count, one_slot_size, create_file, val;
  struct fs_api_debug_efs_api_info_type *api_info;
  int result;

  slots_count = FS_API_DEBUG_EFS_API_SLOTS_COUNT_DEFAULT;
  filename =  FS_API_DEBUG_EFS_API_SLOTS_COUNT_FILENAME;
  result = fs_api_debug_read_uint32_from_efs_file (filename, &val);
  if ((result == 0) &&
      (val > 0) && (val <= FS_API_DEBUG_EFS_API_SLOTS_COUNT_MAX))
  {
    slots_count = val;
  }

  hdr_size = sizeof (struct fs_api_debug_efs_api_info_type);
  one_slot_size = sizeof (struct fs_api_debug_efs_api_slot_type);
  api_dbg->api_buf_size = hdr_size + (one_slot_size * slots_count);

  api_dbg->api_buf = malloc (api_dbg->api_buf_size);
  if (api_dbg->api_buf == NULL)
    return -1;

  memset(api_dbg->api_buf, 0, api_dbg->api_buf_size);
  api_dbg->api_info =
    (struct fs_api_debug_efs_api_info_type *)api_dbg->api_buf;
  api_dbg->api_slot_head =
    (struct fs_api_debug_efs_api_slot_type *)(api_dbg->api_buf + hdr_size);

  api_info = api_dbg->api_info;

  memset(api_dbg->api_buf, 0, api_dbg->api_buf_size);
  api_info->magic = FS_API_DEBUG_EFS_API_MAGIC;
  api_info->version = FS_API_DEBUG_EFS_API_VERSION;
  api_info->struct_size = api_dbg->api_buf_size;
  api_info->one_slot_size = one_slot_size;
  api_info->slots_count = slots_count;
  api_info->cur_slot_idx = 0;
  api_info->max_path_len = FS_API_DEBUG_MAX_PATH_LEN;
  api_info->max_task_name_len = FS_API_DEBUG_MAX_TASK_NAME_LEN;

  filename = FS_API_DEBUG_EFS_API_SAVE_TO_FILE;
  result = fs_api_debug_read_uint32_from_efs_file (filename, &val);
  if ((result != 0) || (val != 1))
    return 0;

  create_file = 0;
  filename = FS_API_DEBUG_EFS_API_CALLS_FILE;
  result = efs_read_file_helper (filename,
           api_dbg->api_buf, api_dbg->api_buf_size);
  if (result == 0)
  {
    if (api_info->magic != FS_API_DEBUG_EFS_API_MAGIC)
      create_file = 1;
    if (api_info->version != FS_API_DEBUG_EFS_API_VERSION)
      create_file = 1;
    if (api_info->struct_size != api_dbg->api_buf_size)
      create_file = 1;
    if (api_info->one_slot_size != one_slot_size)
      create_file = 1;
    if (api_info->slots_count != slots_count)
      create_file = 1;
  }
  else
  {
    create_file = 1;
    result = efs_errno;
    if (result != ENOENT)
    {
      FS_MSG_ERROR_1("efs_read_file_helper failed [%d]", result);
    }
  }

  if (create_file)
  {
    memset(api_dbg->api_buf, 0, api_dbg->api_buf_size);
    api_info->magic = FS_API_DEBUG_EFS_API_MAGIC;
    api_info->version = FS_API_DEBUG_EFS_API_VERSION;
    api_info->struct_size = api_dbg->api_buf_size;
    api_info->one_slot_size = one_slot_size;
    api_info->slots_count = slots_count;
    api_info->cur_slot_idx = 0;
    api_info->max_path_len = FS_API_DEBUG_MAX_PATH_LEN;
    api_info->max_task_name_len = FS_API_DEBUG_MAX_TASK_NAME_LEN;
  }

  api_dbg->save_efs_api_calls_to_file = 1;

  filename = FS_API_DEBUG_EFS_API_RECORD_EFS_TRACKING_FILES;
  result = fs_api_debug_read_uint32_from_efs_file (filename, &val);
  if ((result == 0) && (val == 1))
    api_dbg->record_efs_tracking_files = 1;

  return 0;
}

static int
fs_api_debug_read_efs_sync_file (struct fs_api_debug_info_type *api_dbg)
{
  const char *filename;
  uint32 hdr_size, slots_count, one_slot_size, create_file, val;
  struct fs_api_debug_efs_sync_info_type *sync_info;
  int result;

  slots_count = FS_API_DEBUG_EFS_SYNC_SLOTS_COUNT_DEFAULT;
  filename = FS_API_DEBUG_EFS_SYNC_SLOTS_COUNT_FILE;
  result = fs_api_debug_read_uint32_from_efs_file (filename, &val);
  if ((result == 0) &&
      (val > 0) && (val <= FS_API_DEBUG_EFS_SYNC_SLOTS_COUNT_MAX))
  {
    slots_count = val;
  }

  hdr_size = sizeof (struct fs_api_debug_efs_sync_info_type);
  one_slot_size = sizeof (struct fs_api_debug_efs_sync_slot_type);
  api_dbg->sync_buf_size = hdr_size + (one_slot_size * slots_count);

  api_dbg->sync_buf = malloc (api_dbg->sync_buf_size);
  if (api_dbg->sync_buf == NULL)
    return -1;

  memset(api_dbg->sync_buf, 0, api_dbg->sync_buf_size);
  api_dbg->sync_info =
    (struct fs_api_debug_efs_sync_info_type *)api_dbg->sync_buf;
  api_dbg->sync_slot_head =
    (struct fs_api_debug_efs_sync_slot_type *)(api_dbg->sync_buf + hdr_size);

  sync_info = api_dbg->sync_info;

  create_file = 0;
  filename = FS_API_DEBUG_EFS_SYNC_CALLS_FILE;
  result = efs_read_file_helper (filename,
           api_dbg->sync_buf, api_dbg->sync_buf_size);
  if (result == 0)
  {
    if (sync_info->magic != FS_API_DEBUG_EFS_SYNC_MAGIC)
      create_file = 1;
    if (sync_info->version != FS_API_DEBUG_EFS_SYNC_VERSION)
      create_file = 1;
    if (sync_info->struct_size != api_dbg->sync_buf_size)
      create_file = 1;
    if (sync_info->one_slot_size != one_slot_size)
      create_file = 1;
    if (sync_info->slots_count != slots_count)
      create_file = 1;
  }
  else
  {
    create_file = 1;
    result = efs_errno;
    if (result != ENOENT)
    {
      FS_MSG_ERROR_1("efs_read_file_helper failed [%d]", result);
    }
  }

  if (create_file)
  {
    memset(api_dbg->sync_buf, 0, api_dbg->sync_buf_size);
    sync_info->magic = FS_API_DEBUG_EFS_SYNC_MAGIC;
    sync_info->version = FS_API_DEBUG_EFS_SYNC_VERSION;
    sync_info->struct_size = api_dbg->sync_buf_size;
    sync_info->one_slot_size = one_slot_size;
    sync_info->slots_count = slots_count;
    sync_info->cur_slot_idx = 0;
    sync_info->max_path_len = FS_API_DEBUG_MAX_PATH_LEN;
    sync_info->max_task_name_len = FS_API_DEBUG_MAX_TASK_NAME_LEN;
  }

  return 0;
}

static int
fs_api_debug_read_tracking_filepaths(struct fs_api_debug_info_type *api_dbg)
{
  struct fs_api_file_track_node_type *new_node, *prev_node;
  struct fs_string_iter str_iter;
  struct fs_stat sbuf;
  uint32 node_size;
  char *cur_str;
  int result = -1;

  FS_ASSERT (api_dbg != NULL);

  memset (&sbuf, 0, sizeof(sbuf));
  result = efs_stat (FS_API_DEBUG_TRACKING_FILE, &sbuf);
  if (result < 0)
  {
    result = 0;
    goto End;
  }

  if (sbuf.st_size > FS_API_DEBUG_TRACKING_FILE_SIZE_MAX_BYTES)
    sbuf.st_size = FS_API_DEBUG_TRACKING_FILE_SIZE_MAX_BYTES;

  api_dbg->file_track_buf_size = sbuf.st_size;
  api_dbg->file_track_buf = malloc (api_dbg->file_track_buf_size);
  if (api_dbg->file_track_buf == NULL)
  {
    FS_MSG_ERROR_1("[%d] malloc failed", api_dbg->file_track_buf_size);
    result = -1;
    goto End;
  }
  memset(api_dbg->file_track_buf, 0, api_dbg->file_track_buf_size);

  result = efs_read_file_helper (FS_API_DEBUG_TRACKING_FILE,
           api_dbg->file_track_buf, api_dbg->file_track_buf_size);
  if (result != 0)
  {
    result = efs_errno;
    FS_MSG_ERROR_1("[%d] efs_read_file_helper failed", result);
    free(api_dbg->file_track_buf);
    api_dbg->file_track_buf = NULL;
    result = -1;
    goto End;
  }

  memset (&str_iter, 0, sizeof (str_iter));
  str_iter.str_buf = (char *)api_dbg->file_track_buf;
  str_iter.str_buf_len = api_dbg->file_track_buf_size;
  str_iter.str_seperator1 = '\r';
  str_iter.str_seperator2 = '\n';
  str_iter.new_str_seperator = 0;

  api_dbg->file_track_head = new_node = prev_node = NULL;
  node_size = sizeof (*new_node);

  cur_str = fs_string_iter_get_next (&str_iter);
  while (cur_str != NULL)
  {
    new_node = malloc (node_size);
    if (new_node == NULL)
    {
      FS_MSG_ERROR_1("[%d] malloc failed", node_size);
      result = -1;
      goto End;
    }

    memset(new_node, 0, node_size);
    new_node->path = cur_str;
    new_node->path_len = strlen(new_node->path);
    new_node->fd = -1;

    if (api_dbg->file_track_head == NULL)
      api_dbg->file_track_head = new_node;

    if (prev_node != NULL)
      prev_node->next = new_node;

    prev_node = new_node;
    ++api_dbg->file_track_count;

    cur_str = fs_string_iter_get_next (&str_iter);
  }
  result = 0;

End:
  return result;
}

static int
fs_api_debug_add_check (struct fs_api_debug_info_type *api_dbg,
                        int fd, const char *path)
{
  struct fs_api_file_track_node_type *node;
  uint32 path_len = 0;
  int result;

  FS_ASSERT (api_dbg != NULL);

  if (api_dbg->file_track_count == 0)
    return 1;

  if (path != NULL)
    path_len = strlen (path);

  node = api_dbg->file_track_head;
  while (node != NULL)
  {
    if (path != NULL)
    {
      /* For directories, check only till directory name */
      if (node->path[node->path_len - 1] == '/')
      {
        if (path_len >= node->path_len)
        {
          result = strncmp (node->path, path, node->path_len);
          if (result == 0)
            return 1;
        }
      }
      else
      {
        /* for files, check full path match */
        if (path_len == node->path_len)
        {
          result = strncmp (node->path, path, path_len);
          if (result == 0)
            return 1;
        }
      }
    }

    if (fd >= 0)
    {
      if (fd == node->fd)
        return 1;
    }

    node = node->next;
  }

  return 0;
}

void fs_api_debug_init (void)
{
  struct fs_api_debug_info_type *api_dbg;
  struct fs_api_debug_counters_type *dbgc;
  int result;

  api_dbg = &fs_api_debug_info;
  memset (&fs_api_debug_info, 0, sizeof (fs_api_debug_info));

  api_dbg->boot_time = fs_timetick_get_timetick();

  if (fs_api_debug_disable)
    return;

  result = fs_api_debug_read_counters_file (api_dbg);
  if (result != 0)
    goto Error;

  dbgc = &api_dbg->counters;

  dbgc->read_bytes_count = 0;
  dbgc->read_bytes_count_in_this_boot = 0;

  dbgc->write_bytes_count = 0;
  dbgc->write_bytes_count_in_this_boot = 0;

  memset ((void*)&dbgc->api_counter[0], 0, sizeof (dbgc->api_counter));
  memset ((void*)&dbgc->api_counter_in_this_boot[0], 0,
          sizeof (dbgc->api_counter_in_this_boot));
  memset ((void*)&dbgc->flash_counters_in_this_boot, 0,
          sizeof (dbgc->flash_counters_in_this_boot));

  result = fs_api_debug_read_efs_api_file (api_dbg);
  if (result != 0)
    goto Error;

  result = fs_api_debug_read_tracking_filepaths (api_dbg);
  if (result != 0)
    goto Error;

  result = fs_api_debug_read_efs_sync_file (api_dbg);
  if (result != 0)
    goto Error;

  ++dbgc->boot_count;
  api_dbg->is_api_tracking_enabled = 1;

Error:
  return;
}

static void
fs_api_debug_update_data_counters (struct fs_api_debug_info_type *api_dbg,
                  enum fs_api_debug_id api_id, int result, uint32 bytes_count)
{
  struct fs_api_debug_counters_type *dbgc;

  FS_ASSERT (api_dbg != NULL);

  dbgc = &api_dbg->counters;

  ++dbgc->event_counter;

  ++dbgc->api_counter[api_id];
  ++dbgc->api_counter_in_this_boot[api_id];
  ++dbgc->api_counter_lifetime[api_id];

  switch (api_id)
  {
    case FS_API_DEBUG_ID_EFS_READ:
    case FS_API_DEBUG_ID_EFS_GET:
      if (result >= 0)
      {
        if (api_id == FS_API_DEBUG_ID_EFS_READ)
          bytes_count = result;

        api_dbg->counters.read_bytes_count += bytes_count;
        api_dbg->counters.read_bytes_count_in_this_boot += bytes_count;
        api_dbg->counters.read_bytes_count_lifetime += bytes_count;
      }
      break;

    case FS_API_DEBUG_ID_EFS_WRITE:
    case FS_API_DEBUG_ID_EFS_PUT:
      if (result >= 0)
      {
        if (!api_dbg->is_efs_dirty)
          api_dbg->is_efs_dirty = 1;

        if (api_id == FS_API_DEBUG_ID_EFS_WRITE)
          bytes_count = result;

        api_dbg->counters.write_bytes_count += bytes_count;
        api_dbg->counters.write_bytes_count_in_this_boot += bytes_count;
        api_dbg->counters.write_bytes_count_lifetime += bytes_count;
      }
      break;

    case FS_API_DEBUG_ID_EFS_MKDIR:
    case FS_API_DEBUG_ID_EFS_UNLINK:
    case FS_API_DEBUG_ID_EFS_RMDIR:
    case FS_API_DEBUG_ID_EFS_DELTREE:
    case FS_API_DEBUG_ID_EFS_SYMLINK:
    case FS_API_DEBUG_ID_EFS_CHMOD:
    case FS_API_DEBUG_ID_EFS_CHOWN:
    case FS_API_DEBUG_ID_CHOWN_RECURSIVE:
    case FS_API_DEBUG_ID_CHMOD_RECURSIVE:
    case FS_API_DEBUG_ID_EFS_UTIME:
    case FS_API_DEBUG_ID_EFS_STORE_DATA_IN_SUPERBLOCK:
    case FS_API_DEBUG_ID_EFS_CREAT:
    case FS_API_DEBUG_ID_EFS_TRUNCATE:
    case FS_API_DEBUG_ID_EFS_TRUNCATE64:
    case FS_API_DEBUG_ID_EFS_FTRUNCATE:
    case FS_API_DEBUG_ID_EFS_FTRUNCATE64:
      if (!api_dbg->is_efs_dirty)
        api_dbg->is_efs_dirty = 1;
      break;

    default:
      break;
  }
}

void fs_api_debug_add (enum fs_api_debug_id api_id, int result, int fd,
                       const char *file_path, uint32 bytes_count)
{
  struct fs_api_debug_info_type *api_dbg;
  struct fs_api_debug_counters_type *dbgc;
  struct fs_api_debug_efs_api_info_type *api_info;
  struct fs_api_debug_efs_api_slot_type *api_dbg_slot;
  struct fs_api_debug_efs_sync_info_type *sync_info;
  struct fs_api_debug_efs_sync_slot_type *sync_dbg_slot;
  fs_os_tcb_type *curr_tcb = NULL;
  uint64 end_time, time_diff;
  int is_ok_to_add;

  FS_ASSERT(api_id < FS_API_DEBUG_ID_MAX);
  if (api_id >= FS_API_DEBUG_ID_MAX)
    return;

  api_dbg = &fs_api_debug_info;
  if (!api_dbg->is_api_tracking_enabled)
    return;

  fs_api_debug_update_data_counters (api_dbg, api_id, result, bytes_count);

  if (!fs_api_debug_apis_to_track[api_id])
    goto End;

  is_ok_to_add = fs_api_debug_add_check (api_dbg, fd, file_path);
  if (!is_ok_to_add)
    goto End;

  dbgc = &api_dbg->counters;
  api_info = api_dbg->api_info;
  sync_info = api_dbg->sync_info;

  api_dbg_slot = &api_dbg->api_slot_head[api_info->cur_slot_idx++];
  if (api_info->cur_slot_idx >= api_info->slots_count)
    api_info->cur_slot_idx = 0;

  end_time = fs_timetick_get_timetick ();
  time_diff = fs_timetick_timetick_diff_to_us (end_time - api_dbg->boot_time);

  memset (api_dbg_slot, 0, sizeof (*api_dbg_slot));
  api_dbg_slot->event_counter = dbgc->event_counter;
  api_dbg_slot->api_id = (uint8)api_id;
  api_dbg_slot->result = result;
  api_dbg_slot->fd = fd;
  api_dbg_slot->efs_errno_val = efs_errno;
  api_dbg_slot->time_stamp_in_us = time_diff;

  if (file_path != NULL)
    strlcpy ((char *)api_dbg_slot->path, file_path,
            sizeof (api_dbg_slot->path));

  curr_tcb = fs_os_self ();
  FS_ASSERT (curr_tcb != NULL);
  
  /* Not supported in threadx */
  /*fs_os_get_task_name (curr_tcb, api_dbg_slot->task_name,
                       sizeof (api_dbg_slot->task_name), NULL);*/

  if ((api_id == FS_API_DEBUG_ID_EFS_SYNC) ||
      (api_id == FS_API_DEBUG_ID_EFS_SYNC_NO_WAIT))
  {
    if (curr_tcb != fs_task_tcb)
    {
      sync_dbg_slot = &api_dbg->sync_slot_head[sync_info->cur_slot_idx++];
      if (sync_info->cur_slot_idx >= sync_info->slots_count)
        sync_info->cur_slot_idx = 0;

      memset (sync_dbg_slot, 0, sizeof (*sync_dbg_slot));
      sync_dbg_slot->event_counter = dbgc->event_counter;
      sync_dbg_slot->time_stamp_in_us = time_diff;
      sync_dbg_slot->read_bytes_count = dbgc->read_bytes_count;
      sync_dbg_slot->write_bytes_count = dbgc->write_bytes_count;
     /* fs_os_get_task_name (curr_tcb, sync_dbg_slot->task_name,
                          sizeof (sync_dbg_slot->task_name), NULL);*/
    }
  }

End:
  return;
}

static void
fs_api_debug_add_flash_counters (struct fs_flash_counters *dst,
                                 struct fs_flash_counters*src)
{
  FS_ASSERT (dst != NULL);
  FS_ASSERT (src != NULL);

  dst->flash_write_page_counter += src->flash_write_page_counter;
  dst->flash_write_partial_counter += src->flash_write_partial_counter;
  dst->flash_read_page_counter += src->flash_read_page_counter;
  dst->flash_erase_block_counter += src->flash_erase_block_counter;
  dst->flash_erase_begin_counter += src->flash_erase_begin_counter;
  dst->flash_erase_suspend_counter += src->flash_erase_suspend_counter;
}

static void
fs_api_debug_save_flash_counters (struct fs_api_debug_info_type *api_dbg)
{
  struct fs_api_debug_counters_type *dbgc;
  struct fs_flash_counters *this_boot, *lifetime;

  dbgc = &api_dbg->counters;
  this_boot = &api_dbg->counters.flash_counters_in_this_boot;
  lifetime = &api_dbg->counters.flash_counters_lifetime;

  fs_memscpy (&dbgc->flash_counters_in_this_sync,
              sizeof (dbgc->flash_counters_in_this_sync),
              &fs_flash_counter, sizeof (fs_flash_counter));

  fs_api_debug_add_flash_counters (this_boot, &fs_flash_counter);
  fs_api_debug_add_flash_counters (lifetime, &fs_flash_counter);

  memset (&fs_flash_counter, 0, sizeof (fs_flash_counter));
}

void fs_api_debug_save_data_to_efs(
  enum fs_api_debug_save_data_to_efs_context save_context, int force_sync)
{
  const char *filename;
  struct fs_api_debug_info_type *api_dbg;
  struct fs_api_debug_counters_type *dbgc;

  api_dbg = &fs_api_debug_info;
  if (!api_dbg->is_api_tracking_enabled)
    return;

  dbgc = &api_dbg->counters;

  if (!force_sync)
  {
    if (!api_dbg->is_efs_dirty)
      return;
  }
  api_dbg->is_efs_dirty = 0;

  if (!api_dbg->record_efs_tracking_files)
    api_dbg->is_api_tracking_enabled = 0;

  if (save_context == FS_API_DEBUG_SAVE_DATA_TO_EFS_CONTEXT_RCINIT_SHUTDOWN)
  {
    ++dbgc->shutdown_count;
    ++dbgc->rcinit_shutdown_count;
  }
  if (save_context == FS_API_DEBUG_SAVE_DATA_TO_EFS_CONTEXT_CM_SHUTDOWN)
  {
    ++dbgc->shutdown_count;
    ++dbgc->cm_shutdown_count;
  }

  filename = FS_API_DEBUG_EFS_SYNC_CALLS_FILE;
  (void) efs_write_file_helper (filename, api_dbg->sync_buf,
                                  api_dbg->sync_buf_size);

  if (api_dbg->save_efs_api_calls_to_file)
  {
    filename = FS_API_DEBUG_EFS_API_CALLS_FILE;
    (void) efs_write_file_helper (filename, api_dbg->api_buf,
                                        api_dbg->api_buf_size);
  }

  fs_memscpy ((void*)&dbgc->api_counter_in_this_sync[0],
              sizeof (dbgc->api_counter_in_this_sync),
              (void*)&dbgc->api_counter[0], sizeof (dbgc->api_counter));

  fs_api_debug_save_flash_counters (api_dbg);

  filename = FS_API_DEBUG_COUNTERS_FILE;
  (void) efs_write_file_helper (filename, dbgc, sizeof (*dbgc));

  dbgc->read_bytes_count = 0;
  dbgc->write_bytes_count = 0;
  memset ((void*)&dbgc->api_counter[0], 0, sizeof (dbgc->api_counter));

  api_dbg->is_api_tracking_enabled = 1;
}

#ifdef FS_UNIT_TEST
struct fs_api_debug_info_type* sim_fs_api_debug_get_debug_info (void)
{
  return &fs_api_debug_info;
}
#endif

#endif /* FS_API_DEBUG_DISABLE */
