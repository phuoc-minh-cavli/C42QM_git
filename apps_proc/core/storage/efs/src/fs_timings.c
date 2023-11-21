/***********************************************************************
 * fs_timings.c
 *
 * This file holds the timings measured in EFS module.
 * Copyright (C) 2012 QUALCOMM Technologies, Inc.
 *
 * This file exposes the timings measured in EFS module.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/storage/efs/src/fs_timings.c#1 $ $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2012-04-30   rp    Create

===========================================================================*/

#include "fs_timings.h"

#include <string.h>

static struct fs_timings_type fs_timings;

void
fs_timings_init (void)
{
  memset (&fs_timings, 0, sizeof (fs_timings));
}

struct fs_timings_type*
fs_timings_get_instance (void)
{
  return &fs_timings;
}
