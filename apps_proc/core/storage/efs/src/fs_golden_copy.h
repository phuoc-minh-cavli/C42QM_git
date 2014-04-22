/***********************************************************************
 * fs_golden_copy.h
 *
 * EFS golden copy based on files-list.
 * Copyright (C) 2016 QUALCOMM Technologies, Inc.
 *
 * Creates and restores EFS-golden-copy based on selected files-list.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/storage/efs/src/fs_golden_copy.h#1 $ $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2016-05-23   rp    Support OEMs to create their own EFS-golden-copy.
2016-04-14   rp    Create

===========================================================================*/

#ifndef __FS_GOLDEN_COPY_H__
#define __FS_GOLDEN_COPY_H__

#include "fs_config_i.h"
#include "comdef.h"
#include "fs_priv_funcs.h"

/*****************************************************************************
       File-Format of the golden copy image :-
       ------------------------------------

       The golden copy image format is as shown in the table below. The

      |-----------------------|
      | Golden-copy-Header    | Header Size = 512 Bytes.
      |-----------------------|
      | 1st File-Header       | File entry Header Size = 6 Bytes.
      | file path length      | file path length = 2 Bytes.
      | file size             | Size of the file = 4 Bytes.
      |-----------------------|
      | 1st file path         | file path.
      |-----------------------|
      |                       |
      | 1st File-Data         | Data of the file
      |                       |
      |-----------------------|
      | 2nd File-Header       | File entry Header Size = 6 Bytes.
      | file path length      | file path length = 2 Bytes.
      | file size             | Size of the file = 4 Bytes.
      |-----------------------|
      | 2nd file path         | file path.
      |-----------------------|
      |                       |
      | 2nd File-Data         | Data of the file
      |                       |
      |-----------------------|
      |        ----           |
      |        ----           | More files.
      |        ----           |
      |-----------------------|
      | Last File-Header      | File entry Header Size = 6 Bytes.
      | file path length      | file path length = 2 Bytes.
      | file size             | Size of the file = 4 Bytes.
      |-----------------------|
      | Last file path        | file path.
      |-----------------------|
      |                       |
      | Last File-Data        | Data of the file
      |                       |
      |-----------------------|

*****************************************************************************/


/* Magic numbers in the golden copy image header. */
#define FS_GOLDEN_COPY_IMAGE_HEADER_MAGIC_1     0x49475346
#define FS_GOLDEN_COPY_IMAGE_HEADER_MAGIC_2     0x6567616D

#define FS_GOLDEN_COPY_VERSION                  0x1

/***********************************************************************
 * Header for the golden copy image
 ************************************************************************/
PACKED struct fs_golden_copy_image_header
{
  uint32 magic1;
  uint32 magic2;

  uint32 version;

  uint32 age;
  uint32 files_list_type;
  uint64 current_phone_time_in_ms;

  uint32 entries_count;
  uint32 missed_files_count;

  uint8 pad[468];

  uint32 magic3;
  uint32 magic4;
} PACKED_POST;

/* Header for each file entry in the golden copy image */
PACKED struct fs_golden_copy_entry_header
{
  uint16 file_path_len;
  uint32 file_size;
} PACKED_POST;

PACKED struct fs_golden_copy_entry
{
  enum fs_golden_copy_files_list_type files_list_type;
  struct fs_golden_copy_entry_header hdr;
  uint8 *file_path;
  uint8 *file_data;
} PACKED_POST;

/***********************************************************************
 * FUNCTION      fs_golden_copy_is_okay_to_restore
 *
 * DESCRIPTION   Decider function to check if it is okay to restore the
 *               golden copy.
 *
 * RETURN VALUE  Return 1 if it is okay to restore golden copy and 0 if not
 ************************************************************************/
 int fs_golden_copy_is_okay_to_restore (void);

/***********************************************************************
 * FUNCTION      fs_golden_copy_restore_begin
 *
 * DESCRIPTION   This function will get called exactly once to start the
 *               golden copy restoration process.
 *
 * INPUT         ram_addr
 *               RAM-address which contains the golden copy image .
 *
 * INPUT         ram_size
 *               Total size of the RAM. The golden-copy image will be less
 *               than or equal to this ram-size.
 *
 * RETURN VALUE  Return 0 if it is okay to restore golden copy and -1 if not
 ************************************************************************/
int fs_golden_copy_restore_begin (uint8 *ram_addr, uint32 ram_size);

/***********************************************************************
 * FUNCTION      fs_golden_copy_restore_get_next
 *
 * DESCRIPTION   Called to iterate through all the files in the golden copy
 *               image.
 *
 * RETURN VALUE  Each time return the next file in the golden-copy image and
 *               return NULL when there are no more files.
 ************************************************************************/
struct fs_golden_copy_entry* fs_golden_copy_restore_get_next (void);

/***********************************************************************
 * FUNCTION      fs_golden_copy_restore_end
 *
 * DESCRIPTION   This function will get called exactly once to end the
 *               golden copy restoration process.
 ************************************************************************/
void fs_golden_copy_restore_end (void);




/***********************************************************************
 * FUNCTION      fs_golden_copy_create_begin
 *
 * DESCRIPTION   This function will get called exactly once to start the
 *               golden copy creation process.
 *
 * INPUT         ram_addr
 *               RAM-address onto which the golden copy image has
 *               to be created
 *
 * INPUT         ram_size
 *               Total size of the RAM. The golden-copy image should be less
 *               than or equal to this ram-size.
 ************************************************************************/
void fs_golden_copy_create_begin (uint8 *ram_addr, uint32 ram_size,
              enum fs_golden_copy_files_list_type files_list_type);

/***********************************************************************
 * FUNCTION      fs_golden_copy_create_add_file
 *
 * DESCRIPTION   Called to addall the files to the golden copy image.
 *
 * INPUT         filepath
 *               filepath in EFS.
 ************************************************************************/
void fs_golden_copy_create_add_file (const char *filepath);

/***********************************************************************
 * FUNCTION      fs_golden_copy_create_end
 *
 * DESCRIPTION   This function will get called exactly once to end the
 *               golden copy creation process.
 ************************************************************************/
uint32 fs_golden_copy_create_end (void);

#endif /* not __FS_GOLDEN_COPY_H__ */
