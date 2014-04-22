/***********************************************************************
 * fs_errno.c
 *
 * Implements function to get the address of efs_errno variable.
 *
 * Copyright (C) 2011,2016 QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/storage/efs/src/fs_errno.c#1 $ $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2016-09-21   as    Added errno get_address code for supporting both rex and qurt.
2016-08-08   rp    Enhance fs-osal to support both rex and qurt.
2011-11-03   dks   Fix compiler error for boot.
2011-10-03   dks   Create.

===========================================================================*/

#include "fs_public.h"
#include "fs_err.h"
#include "fs_osal.h"
#include "fs_config_i.h"


void
efs_errno_init (void)
{
  fs_os_efs_errno_init ();
}

/* Get the errno pointer value */
int*
efs_get_errno_address (void)
{
#if defined (FEATURE_EFS_COMPILE_WITHOUT_REX)
  fs_os_tcb_type *tcb;

  tcb = fs_os_self ();
  if (tcb == NULL)
  {
    FS_ERR_FATAL ("null tcb !!!",0,0,0);
  }

  return &(tcb->err_num);

#else
  return fs_os_efs_errno_get_address();
#endif
}
