/*==============================================================================

Dynamic Loading Support Libs

Copyright (c) 2014 by QUALCOMM Technologies Incorporated.
All Rights Reserved.
QUALCOMM Proprietary/GTDR

--------------------------------------------------------------------------------
 EDIT HISTORY FOR FILE

 This section contains comments describing changes made to the module.
 Notice that changes are listed in reverse chronological order.

 when               who         what, where, why
 --------           ---         -----------------------------------------------
 1/22/15          amitkulk      created 
==============================================================================*/

#include <errno.h>
#include <sys/stat.h>

#include "dlfs.h"

int sys_open(const char *name, unsigned int mode)
{
  return dlfs_open(name, mode);
}

int sys_close(int fd)
{
  return dlfs_close(fd);
}

int sys_seek(int fd, int offset)
{
  return dlfs_seek(fd, offset);
}

int sys_read(int fd, char *buffer, int count)
{
  return dlfs_read(fd, buffer, count);
}

int sys_flen(int fd)
{
  return dlfs_flen(fd);
}

int sys_ftell(int fd)
{
  return dlfs_ftell(fd);
}

int sys_fstat(int fd, void* buffer)
{
  return dlfs_fstat(fd, buffer);
}



