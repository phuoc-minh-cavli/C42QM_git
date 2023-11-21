/***********************************************************************
 * fs_timings.h
 *
 * This file holds the timings measured in EFS module.
 * Copyright (C) 2012, 2015 QUALCOMM Technologies, Inc.
 *
 * This file exposes the timings measured in EFS module.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/storage/efs/src/fs_timings.h#1 $ $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-02-23   vm    Add support to compress EFS RAM into shared RAM on sync
2012-11-09   nr    Update rmts to use new api's.
2012-04-30   rp    Create

===========================================================================*/

#ifndef __FS_TIMINGS_H__
#define __FS_TIMINGS_H__

#include "comdef.h"

struct fs_timings_efs_sync_timings
{
  uint32 compression_time;
  uint32 encryption_time;
  uint32 write_iovec_time;
};

struct fs_timings_type
{
  struct fs_timings_efs_sync_timings    efs_sync_timings;
};



/**************************************************************************//**
 * @par Name:
 *   fs_timings_init - Initialize the fs_timings module.
 *
 * @par Description:
 *  Initalize the fs_timings module, called from efs_initialize()
 *
******************************************************************************/
void fs_timings_init (void);


/**************************************************************************//**
 * @par Name:
 *   fs_timings_get_instance - Returns the timings structure instance.
 *
 * @par Description:
 *  Returns the timings structure instance.
 *
 * @return
 *  Returns the timings structure instance.
 *
******************************************************************************/
struct fs_timings_type* fs_timings_get_instance (void);


#endif /* not __FS_TIMINGS_H__ */
