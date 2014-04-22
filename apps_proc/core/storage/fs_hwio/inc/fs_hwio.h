/***********************************************************************
 * fs_hwio.h
 *
 * FS HWIO Interface
 * Copyright (c) 2015 Qualcomm Technologies, Inc.  All Rights Reserved.
 * Qualcomm Technologies Proprietary and Confidential.
 *
 * HWIO Interface for all Filesystem modules to use
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/storage/fs_hwio/inc/fs_hwio.h#1 $ $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-03-05   dks   Create

===========================================================================*/
#ifndef __FS_HWIO_H__
#define __FS_HWIO_H__

#include "comdef.h"

uint32 fs_hwio_get_device_serial_number (void);

#endif
