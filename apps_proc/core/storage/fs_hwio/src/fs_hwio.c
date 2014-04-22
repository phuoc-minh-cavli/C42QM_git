/***********************************************************************
 * fs_hwio.c
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

  $Header: //components/rel/core.tx/6.0/storage/fs_hwio/src/fs_hwio.c#1 $ $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2015-03-05   dks   Create

===========================================================================*/

#include "HALhwio.h"
#include "fs_hwio.h"
#include "assert.h"

#include "security_control.h"

struct fs_hwio_info_type
{
  uint32 serial_number;
};
struct fs_hwio_info_type fs_hwio_info;

static void
fs_hwio_read_device_serial_number (void)
{
  fs_hwio_info.serial_number = 0xDEADBEEF;

  fs_hwio_info.serial_number = HWIO_INF (QFPROM_CORR_SERIAL_NUM, SERIAL_NUM);
  ASSERT (fs_hwio_info.serial_number != 0xDEADBEEF);
}

uint32
fs_hwio_get_device_serial_number (void)
{
  return fs_hwio_info.serial_number;
}

void
fs_hwio_init (void)
{
  fs_hwio_read_device_serial_number ();
}

