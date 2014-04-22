/***********************************************************************
 * fs_golden_copy.c
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

  $Header: //components/rel/core.tx/6.0/storage/efs/src/fs_golden_copy.c#1 $ $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2016-05-23   rp    Support OEMs to create their own EFS-golden-copy.
2016-04-14   rp    Create

===========================================================================*/

#include "fs_config_i.h"
#include "fs_golden_copy.h"
#include "fs_public.h"
#include "fs_err.h"
#include "fs_util.h"

#include "stringl/stringl.h"
#include "assert.h"

#include <string.h>

struct fs_golden_copy_image_info
{
  uint8 *ram_addr;
  uint32 ram_size;

  struct fs_golden_copy_image_header *hdr;
  uint32 cur_entry;
  uint32 cur_posn;
  uint32 last_known_age;

  struct fs_golden_copy_entry entry;
};
static struct fs_golden_copy_image_info fs_golden_copy_image;


int
fs_golden_copy_is_okay_to_restore (void)
{
  return 1; /* Always vote for restore */
}

int
fs_golden_copy_restore_begin (uint8 *ram_addr, uint32 ram_size)
{
  struct fs_golden_copy_image_header *hdr;

  ASSERT (ram_addr != NULL);
  ASSERT (ram_size > 0);

  memset (&fs_golden_copy_image, 0, sizeof(fs_golden_copy_image));

  fs_golden_copy_image.ram_addr = ram_addr;
  fs_golden_copy_image.ram_size = ram_size;
  fs_golden_copy_image.hdr = (struct fs_golden_copy_image_header *)ram_addr;
  fs_golden_copy_image.cur_entry = 0;
  fs_golden_copy_image.cur_posn = sizeof(struct fs_golden_copy_image_header);

  hdr = (struct fs_golden_copy_image_header *)ram_addr;
  if ((hdr->magic1 != FS_GOLDEN_COPY_IMAGE_HEADER_MAGIC_1) ||
      (hdr->magic2 != FS_GOLDEN_COPY_IMAGE_HEADER_MAGIC_2) ||
      (hdr->magic3 != FS_GOLDEN_COPY_IMAGE_HEADER_MAGIC_1) ||
      (hdr->magic4 != FS_GOLDEN_COPY_IMAGE_HEADER_MAGIC_2))
  {
    return -1;
  }
  if (hdr->version != FS_GOLDEN_COPY_VERSION)
  {
    return -1;
  }

  fs_golden_copy_image.last_known_age = hdr->age;

  return 0;
}

struct fs_golden_copy_entry*
fs_golden_copy_restore_get_next (void)
{
  struct fs_golden_copy_entry_header *entry_hdr;
  uint8 *ram_addr;

  ASSERT (fs_golden_copy_image.cur_entry <=
          fs_golden_copy_image.hdr->entries_count);
  ASSERT (fs_golden_copy_image.cur_posn <
          fs_golden_copy_image.ram_size);

  if (fs_golden_copy_image.cur_entry ==
      fs_golden_copy_image.hdr->entries_count)
  {
    return NULL;
  }

  entry_hdr = (struct fs_golden_copy_entry_header *)
                &fs_golden_copy_image.ram_addr[fs_golden_copy_image.cur_posn];

  memset(&fs_golden_copy_image.entry, 0, sizeof(fs_golden_copy_image.entry));

  fs_golden_copy_image.entry.files_list_type =
    (enum fs_golden_copy_files_list_type )
    fs_golden_copy_image.hdr->files_list_type;

  fs_golden_copy_image.entry.hdr.file_path_len = entry_hdr->file_path_len;
  fs_golden_copy_image.entry.hdr.file_size = entry_hdr->file_size;

  fs_golden_copy_image.cur_posn += sizeof (struct fs_golden_copy_entry_header);
  ram_addr =
    (uint8 *)&fs_golden_copy_image.ram_addr[fs_golden_copy_image.cur_posn];
  fs_golden_copy_image.entry.file_path = ram_addr;

  fs_golden_copy_image.cur_posn += entry_hdr->file_path_len;
  ram_addr =
    (uint8 *)&fs_golden_copy_image.ram_addr[fs_golden_copy_image.cur_posn];
  fs_golden_copy_image.entry.file_data = ram_addr;

  fs_golden_copy_image.cur_posn += entry_hdr->file_size;
  ASSERT (fs_golden_copy_image.cur_posn <
          fs_golden_copy_image.ram_size);

  ++fs_golden_copy_image.cur_entry;

  return &fs_golden_copy_image.entry;
}

void fs_golden_copy_restore_end (void)
{
  ASSERT (fs_golden_copy_image.cur_entry ==
          fs_golden_copy_image.hdr->entries_count);
}


void
fs_golden_copy_create_begin (uint8 *ram_addr, uint32 ram_size,
          enum fs_golden_copy_files_list_type files_list_type)
{
  struct fs_golden_copy_image_header *hdr;
  uint32 last_known_age;

  last_known_age = fs_golden_copy_image.last_known_age;
  memset (&fs_golden_copy_image, 0, sizeof(fs_golden_copy_image));

  memset (ram_addr, 0xFF, ram_size);

  fs_golden_copy_image.ram_addr = ram_addr;
  fs_golden_copy_image.ram_size = ram_size;
  fs_golden_copy_image.hdr = (struct fs_golden_copy_image_header *)ram_addr;
  fs_golden_copy_image.cur_entry = 0;
  fs_golden_copy_image.cur_posn = sizeof(struct fs_golden_copy_image_header);

  hdr = (struct fs_golden_copy_image_header *)ram_addr;
  memset (hdr, 0, sizeof (struct fs_golden_copy_image_header));
  memset (hdr->pad, 0xFF, sizeof (hdr->pad));
  hdr->magic1 = FS_GOLDEN_COPY_IMAGE_HEADER_MAGIC_1;
  hdr->magic2 = FS_GOLDEN_COPY_IMAGE_HEADER_MAGIC_2;
  hdr->version = FS_GOLDEN_COPY_VERSION;
  hdr->age = last_known_age;
  hdr->files_list_type = (uint32)files_list_type;
  hdr->current_phone_time_in_ms = fs_time ();
}


void
fs_golden_copy_create_add_file (const char *filepath)
{
  struct fs_golden_copy_entry_header *entry_hdr;
  uint8 *ram_addr;
  struct fs_stat sbuf;
  int result;

  ASSERT (fs_golden_copy_image.cur_posn <
          fs_golden_copy_image.ram_size);

  memset(&sbuf, 0, sizeof(sbuf));
  result = efs_stat (filepath, &sbuf);
  if (result != 0)
  {
    ++fs_golden_copy_image.hdr->missed_files_count;
    return;
  }

  entry_hdr = (struct fs_golden_copy_entry_header *)
                &fs_golden_copy_image.ram_addr[fs_golden_copy_image.cur_posn];

  entry_hdr->file_path_len = strlen (filepath) + 1;
  entry_hdr->file_size = sbuf.st_size;

  fs_golden_copy_image.cur_posn += sizeof (struct fs_golden_copy_entry_header);
  ram_addr =
    (uint8 *)&fs_golden_copy_image.ram_addr[fs_golden_copy_image.cur_posn];
  strlcpy((char *)ram_addr, filepath, entry_hdr->file_path_len);

  fs_golden_copy_image.cur_posn += entry_hdr->file_path_len;
  ram_addr =
    (uint8 *)&fs_golden_copy_image.ram_addr[fs_golden_copy_image.cur_posn];

  result = efs_get(filepath, ram_addr, entry_hdr->file_size);
  if (result != (int)entry_hdr->file_size)
  {
    FS_ERR_FATAL ("get failed : %d : %d : %d", efs_errno, result, 0);
  }

  fs_golden_copy_image.cur_posn += entry_hdr->file_size;
  ASSERT (fs_golden_copy_image.cur_posn <
          fs_golden_copy_image.ram_size);

  ++fs_golden_copy_image.hdr->entries_count;
}

uint32
fs_golden_copy_create_end (void)
{
  uint32 pad_size;

  ASSERT (fs_golden_copy_image.cur_posn <=
          fs_golden_copy_image.ram_size);

  fs_golden_copy_image.hdr->magic3 = FS_GOLDEN_COPY_IMAGE_HEADER_MAGIC_1;
  fs_golden_copy_image.hdr->magic4 = FS_GOLDEN_COPY_IMAGE_HEADER_MAGIC_2;

  pad_size = fs_golden_copy_image.cur_posn % 512;
  fs_golden_copy_image.cur_posn += (512 - pad_size);

  FS_MSG_HIGH_3("make fsg status: %d, %d, %d",
    fs_golden_copy_image.hdr->entries_count,
    fs_golden_copy_image.hdr->missed_files_count,
    fs_golden_copy_image.cur_posn);

  return fs_golden_copy_image.cur_posn;
}

