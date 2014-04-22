/***********************************************************************
 * fs_api_debug.h
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
2018-10-10   eo   Add FS API debug info support
2017-09-01   np   Create

===========================================================================*/

#ifndef __FS_API_DEBUG_H__
#define __FS_API_DEBUG_H__

#include "fs_config_i.h"
#include "comdef.h"
#include "fs_counters.h"

/*
 * EFS API debug framework:
 * ------------------------
 *
 * This EFS API debug framework will help track the following:
 *   1. last few EFS API calls made.
 *   2. How many bytes are read/written in EFS in current boot and lifetime.
 *   3. Track efs-sync API callers
 *
 * Currently only those EFS APIs mentioned in the enum [enum fs_api_debug_id]
 * can be tracked. The [array fs_api_debug_apis_to_track] fine tunes further
 * which APIs can be tracked. By default only those EFS APIs that create/change
 * data in EFS are tracked.
 *
 * The EFS API calls are recorded in an circular RAM-buffer and the
 * below #defines can be tweaked to configure this RAM-buffer size etc.
 *
 * By default all the filenames will be tracked. If there is an need to track
 * only few files/directories of interest, then create an below file specifying
 * which exact filenames/directories to track. [FS_API_DEBUG_TRACKING_FILE]
 *
 */

/* Currently only these below EFS API tracking is supported. */
enum fs_api_debug_id
{
  FS_API_DEBUG_ID_UNUSED                          = 0,
  FS_API_DEBUG_ID_EFS_OPEN                        = 1,
  FS_API_DEBUG_ID_EFS_CLOSE                       = 2,
  FS_API_DEBUG_ID_EFS_READ                        = 3,
  FS_API_DEBUG_ID_EFS_WRITE                       = 4,
  FS_API_DEBUG_ID_EFS_MKDIR                       = 5,
  FS_API_DEBUG_ID_EFS_PUT                         = 6,
  FS_API_DEBUG_ID_EFS_GET                         = 7,
  FS_API_DEBUG_ID_EFS_SYNC                        = 8,
  FS_API_DEBUG_ID_EFS_SYNC_NO_WAIT                = 9,
  FS_API_DEBUG_ID_EFS_SYNC_GET_STATUS             = 10,
  FS_API_DEBUG_ID_EFS_STATVFS                     = 11,
  FS_API_DEBUG_ID_EFS_FSTATVFS                    = 12,
  FS_API_DEBUG_ID_EFS_RENAME                      = 13,
  FS_API_DEBUG_ID_EFS_UNLINK                      = 14,
  FS_API_DEBUG_ID_EFS_RMDIR                       = 15,
  FS_API_DEBUG_ID_EFS_DELTREE                     = 16,
  FS_API_DEBUG_ID_EFS_SYMLINK                     = 17,
  FS_API_DEBUG_ID_EFS_READLINK                    = 18,
  FS_API_DEBUG_ID_EFS_CHMOD                       = 19,
  FS_API_DEBUG_ID_EFS_CHOWN                       = 20,
  FS_API_DEBUG_ID_CHOWN_RECURSIVE                 = 21,
  FS_API_DEBUG_ID_CHMOD_RECURSIVE                 = 22,
  FS_API_DEBUG_ID_EFS_UTIME                       = 23,
  FS_API_DEBUG_ID_DIAG_FS_REGISTER_ACCESS_CHECK   = 24,
  FS_API_DEBUG_ID_EFS_OPENDIR                     = 25,
  FS_API_DEBUG_ID_EFS_READDIR                     = 26,
  FS_API_DEBUG_ID_EFS_CLOSEDIR                    = 27,
  FS_API_DEBUG_ID_EFS_PREPARE_SHUTDOWN            = 28,
  FS_API_DEBUG_ID_EFS_RCINIT_GROUP7_SHUTDOWN_CB   = 29,
  FS_API_DEBUG_ID_EFS_RCINIT_SHUTDOWN_CB          = 30,
  FS_API_DEBUG_ID_EFS_MAKE_GOLDEN_COPY            = 31,
  FS_API_DEBUG_ID_EFS_MAKE_GOLDEN_COPY_NO_WAIT    = 32,
  FS_API_DEBUG_ID_EFS_GOLDEN_COPY_SET_FILES_LIST  = 33,
  FS_API_DEBUG_ID_EFS_STORE_DATA_IN_SUPERBLOCK    = 34,
  FS_API_DEBUG_ID_EFS_FILESYSTEM_IMAGE_OPEN       = 35,
  FS_API_DEBUG_ID_EFS_IMAGE_PREPARE               = 36,
  FS_API_DEBUG_ID_EFS_SCRUB                       = 37,
  FS_API_DEBUG_ID_EFS_FULL_SCRUB                  = 38,
  FS_API_DEBUG_ID_EFS_SHRED                       = 39,
  FS_API_DEBUG_ID_EFS_CREAT                       = 40,
  FS_API_DEBUG_ID_EFS_STAT                        = 41,
  FS_API_DEBUG_ID_EFS_FSTAT                       = 42,
  FS_API_DEBUG_ID_EFS_LSTAT                       = 43,
  FS_API_DEBUG_ID_EFS_LSEEK                       = 44,
  FS_API_DEBUG_ID_EFS_LSEEK64                     = 45,
  FS_API_DEBUG_ID_EFS_TRUNCATE                    = 46,
  FS_API_DEBUG_ID_EFS_TRUNCATE64                  = 47,
  FS_API_DEBUG_ID_EFS_FTRUNCATE                   = 48,
  FS_API_DEBUG_ID_EFS_FTRUNCATE64                 = 49,
  FS_API_DEBUG_ID_EFS_LOCK_FS                     = 50,
  FS_API_DEBUG_ID_EFS_UNLOCK_FS                   = 51,
  FS_API_DEBUG_ID_MAX                             = 52,
};

/* Set this variable to disable the EFS API debug feature. */
extern int fs_api_debug_disable;

/* Configure how many EFS APIs to record in the circular buffer.
 *
 * FS_API_DEBUG_DEFAULT_SLOTS_COUNT is used to configure how many of the
 * last few EFS API calls to track. By default 50 last few EFS API calls
 * will be tracked.
 *
 * Each slot here will take this below struct space in RAM:
 *   sizeof(struct fs_api_debug_efs_api_slot_type)
 * This above RAM will be malloced during the EFS-initialization.
 *
 * The slots-count can also be provided at runtime via this below file:
 * [FS_API_DEBUG_SLOTS_COUNT_FILE]
 *
 * FS_API_DEBUG_MAX_SLOTS_COUNT is safe-guard to limit the number of
 * maximum slots allowed.
 */
#ifndef FS_API_DEBUG_EFS_API_SLOTS_COUNT_DEFAULT
  #define FS_API_DEBUG_EFS_API_SLOTS_COUNT_DEFAULT (50)
#endif
#ifndef FS_API_DEBUG_EFS_API_SLOTS_COUNT_FILENAME
  #define FS_API_DEBUG_EFS_API_SLOTS_COUNT_FILENAME       \
           "/.efs/api_debug/config/efs_api_slots_count"
#endif
#ifndef FS_API_DEBUG_EFS_API_SLOTS_COUNT_MAX
  #define FS_API_DEBUG_EFS_API_SLOTS_COUNT_MAX (500)
#endif


/* Specify which exact file names to track in this file.
 *
 * Create this below file in EFS to list all the filepaths to track.
 * This can be an multiple lines file, where each line specify the exact
 * filepath or an directory-name in which case all the files under this
 * directory will be tracked.
 *
 * If this file is absent/empty, then all the filenames will be tracked.
 */
#ifndef FS_API_DEBUG_TRACKING_FILE
  #define FS_API_DEBUG_TRACKING_FILE \
            "/.efs/api_debug/config/files_to_track"
#endif
/* Size in bytes for the above file. */
#ifndef FS_API_DEBUG_TRACKING_FILE_SIZE_MAX_BYTES
  #define FS_API_DEBUG_TRACKING_FILE_SIZE_MAX_BYTES (10 * 1024)
#endif

struct fs_api_file_track_node_type
{
  const char *path;
  uint32 path_len;
  int fd;
  struct fs_api_file_track_node_type *next;
};

/* Record the EFS API Debug circular buffer to an EFS-file
 *
 * By default the EFS API debug logs are kept only in RAM. Create
 * this file to save the EFS API calls RAM-buffer to an EFS-file
 */
#ifndef FS_API_DEBUG_EFS_API_SAVE_TO_FILE
  #define FS_API_DEBUG_EFS_API_SAVE_TO_FILE \
           "/.efs/api_debug/config/save_efs_api_calls"
#endif
/* Create this file to also record EFS-internal tracking files */
#ifndef FS_API_DEBUG_EFS_API_RECORD_EFS_TRACKING_FILES
  #define FS_API_DEBUG_EFS_API_RECORD_EFS_TRACKING_FILES \
    "/.efs/api_debug/config/record_efs_tracking_files"
#endif
#ifndef FS_API_DEBUG_EFS_API_CALLS_FILE
  #define FS_API_DEBUG_EFS_API_CALLS_FILE \
           "/.efs/api_debug/logs/efs_api_calls"
#endif

/* Only this many starting bytes of the file-path will be tracked. */
#ifndef FS_API_DEBUG_MAX_PATH_LEN
  #define FS_API_DEBUG_MAX_PATH_LEN (48)
#endif

/* Only this many starting bytes of the calling task name will be tracked */
#ifndef FS_API_DEBUG_MAX_TASK_NAME_LEN
  #define FS_API_DEBUG_MAX_TASK_NAME_LEN (12)
#endif

PACKED struct fs_api_debug_efs_api_info_type
{
  uint32 magic;
  uint32 version;
  uint32 struct_size;

  uint32 one_slot_size;
  uint32 slots_count;
  uint32 cur_slot_idx;

  uint32 max_path_len;
  uint32 max_task_name_len;
} PACKED_POST;

/* struct for one EFS API debug slot */
PACKED struct fs_api_debug_efs_api_slot_type
{
  uint32 event_counter;
  uint64 time_stamp_in_us;
  uint8 api_id;
  int fd;
  int result;
  int efs_errno_val;
  char path[FS_API_DEBUG_MAX_PATH_LEN];
  char task_name[FS_API_DEBUG_MAX_TASK_NAME_LEN];
} PACKED_POST;

/* Track efs-sync API calls and its callers
 *
 * FS_API_DEBUG_DEFAULT_EFS_SYNC_SLOTS_COUNT is used to configure how many
 * last few EFS API calls to track. By default 50 last few EFS-SYNC api calls
 * will be tracked
 * Each slot here will take this below struct space in RAM:
 *   sizeof(struct fs_api_debug_efs_sync_slot_type)
 * This above RAM will be malloced during the EFS-initialization.
 *
 * The slots-count can also be provided at runtime via this below file:
 * [FS_API_DEBUG_EFS_SYNC_SLOTS_COUNT_FILE]
 *
 * FS_API_DEBUG_MAX_EFS_SYNC_SLOTS_COUNT is safe-guard to limit the number of
 * maximum slots allowed.
 */
#ifndef FS_API_DEBUG_EFS_SYNC_SLOTS_COUNT_DEFAULT
  #define FS_API_DEBUG_EFS_SYNC_SLOTS_COUNT_DEFAULT (50)
#endif
#ifndef FS_API_DEBUG_EFS_SYNC_SLOTS_COUNT_FILE
  #define FS_API_DEBUG_EFS_SYNC_SLOTS_COUNT_FILE \
            "/.efs/api_debug/config/efs_sync_slots_count"
#endif
#ifndef FS_API_DEBUG_EFS_SYNC_SLOTS_COUNT_MAX
  #define FS_API_DEBUG_EFS_SYNC_SLOTS_COUNT_MAX (500)
#endif
#ifndef FS_API_DEBUG_EFS_SYNC_CALLS_FILE
  #define FS_API_DEBUG_EFS_SYNC_CALLS_FILE \
            "/.efs/api_debug/logs/efs_sync_api_calls"
#endif

PACKED struct fs_api_debug_efs_sync_info_type
{
  uint32 magic;
  uint32 version;
  uint32 struct_size;
  uint32 one_slot_size;
  uint32 slots_count;
  uint32 cur_slot_idx;

  uint32 max_path_len;
  uint32 max_task_name_len;
} PACKED_POST;

/* struct for one EFS Sync debug slot */
PACKED struct fs_api_debug_efs_sync_slot_type
{
  uint32 event_counter;
  uint64 time_stamp_in_us;
  uint32 read_bytes_count;
  uint32 write_bytes_count;
  char task_name[FS_API_DEBUG_MAX_TASK_NAME_LEN];
} PACKED_POST;

/* Track EFS API counter and READ/WRITE data counters.
 *
 * Track how many times EFS APIs are called as below:
 *   (a) since previous efs-sync
 *   (b) in the current boot session
 *   (c) lifetime counter
 */
#ifndef FS_API_DEBUG_COUNTERS_FILE
  #define FS_API_DEBUG_COUNTERS_FILE \
            "/.efs/api_debug/logs/efs_counters"
#endif

PACKED struct fs_api_debug_counters_type
{
  uint32 magic;
  uint32 version;
  uint32 struct_size;

  uint32 boot_count;
  uint32 shutdown_count;
  uint32 rcinit_shutdown_count;
  uint32 cm_shutdown_count;

  uint32 event_counter;

  uint32 read_bytes_count;
  uint32 read_bytes_count_in_this_boot;
  uint32 read_bytes_count_lifetime;

  uint32 write_bytes_count;
  uint32 write_bytes_count_in_this_boot;
  uint32 write_bytes_count_lifetime;

  uint32 api_counter[FS_API_DEBUG_ID_MAX];
  uint32 api_counter_in_this_sync[FS_API_DEBUG_ID_MAX];
  uint32 api_counter_in_this_boot[FS_API_DEBUG_ID_MAX];
  uint32 api_counter_lifetime[FS_API_DEBUG_ID_MAX];

  struct fs_flash_counters flash_counters_in_this_sync;
  struct fs_flash_counters flash_counters_in_this_boot;
  struct fs_flash_counters flash_counters_lifetime;

} PACKED_POST;

struct fs_api_debug_info_type
{
  int is_api_tracking_enabled;
  uint64 boot_time;

  struct fs_api_debug_counters_type counters;

  int save_efs_api_calls_to_file;
  int record_efs_tracking_files;
  int is_efs_dirty;

  uint8  *api_buf;
  uint32 api_buf_size;
  struct fs_api_debug_efs_api_info_type *api_info;
  struct fs_api_debug_efs_api_slot_type *api_slot_head;

  uint8  *file_track_buf;
  uint32 file_track_buf_size;
  uint32 file_track_count;
  struct fs_api_file_track_node_type *file_track_head;

  uint8  *sync_buf;
  uint32 sync_buf_size;
  struct fs_api_debug_efs_sync_info_type *sync_info;
  struct fs_api_debug_efs_sync_slot_type *sync_slot_head;
};

enum fs_api_debug_save_data_to_efs_context
{
  FS_API_DEBUG_SAVE_DATA_TO_EFS_CONTEXT_SYNC = 1,
  FS_API_DEBUG_SAVE_DATA_TO_EFS_CONTEXT_RCINIT_SHUTDOWN = 2,
  FS_API_DEBUG_SAVE_DATA_TO_EFS_CONTEXT_CM_SHUTDOWN = 3,
};

#ifdef FS_API_DEBUG_DISABLE

static inline void fs_api_debug_init (void)
{
}

static inline void fs_api_debug_add (enum fs_api_debug_id api_id, int result,
                            int fd, const char *file_path, uint32 bytes_count)
{
  (void)api_id; (void)result; (void)fd; (void)file_path; (void)bytes_count;
}

static inline void fs_api_debug_save_data_to_efs (
  enum fs_api_debug_save_data_to_efs_context save_context, int force_sync)
{
  (void)save_context; (void)force_sync;
}

#else /* FS_API_DEBUG_DISABLE */

void fs_api_debug_init (void);
void fs_api_debug_add (enum fs_api_debug_id api_id, int result, int fd,
                       const char *file_path, uint32 bytes_count);
void fs_api_debug_save_data_to_efs(
  enum fs_api_debug_save_data_to_efs_context save_context, int force_sync);

#endif /* FS_API_DEBUG_DISABLE */

#ifdef FS_UNIT_TEST
struct fs_api_debug_info_type* sim_fs_api_debug_get_debug_info (void);
#endif

#endif /* not __FS_API_DEBUG_H__ */
