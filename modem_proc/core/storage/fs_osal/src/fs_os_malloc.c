/***********************************************************************
 * fs_os_malloc.c
 *
 * Malloc wrapper
 * Copyright (C) 2015 QUALCOMM Technologies, Inc.
 *
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.mpss/3.10/storage/fs_osal/src/fs_os_malloc.c#1 $ $DateTime: 2018/06/26 03:18:17 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-05-15   dks   Create

===========================================================================*/

#include "fs_os_config_i.h"
#include "fs_os_malloc.h"
#include <stdlib.h>

void*
fs_os_malloc (uint32 size)
{
  return malloc (size);
}

void
fs_os_free (void *ptr)
{
  free (ptr);
}


