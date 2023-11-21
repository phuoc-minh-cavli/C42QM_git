/***********************************************************************
 * fs_rmts.h
 *
 * The rmts common header module.
 * Copyright (C) 2012,2016 QUALCOMM Technologies, Inc.
 *
 * This module contains all the stuctures and definitions of the rmts module
 * that serve as input and return parameters of the device layer functions.
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/core.tx/6.0/storage/efs/src/fs_rmts.h#1 $ $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $

when         who   what, where, why
----------   ---   ---------------------------------------------------------
2016-05-23   rp   Support OEMs to create their own EFS-golden-copy.
2012-10-03   nr   Create

===========================================================================*/

#ifndef __FS_RMTS_H__
#define __FS_RMTS_H__

enum fs_rmts_modem_type
{
  FS_RMTS_MODEM_TYPE_STANDALONE,
  FS_RMTS_MODEM_TYPE_FUSION,
  FS_RMTS_MODEM_TYPE_MAX /* This should be the last member. */
};

enum fs_rmts_partition_id
{
  FS_RMTS_PARTITION_ID_FS1,
  FS_RMTS_PARTITION_ID_FS2,
  FS_RMTS_PARTITION_ID_FSG,
  FS_RMTS_PARTITION_ID_FSC,
  FS_RMTS_PARTITION_ID_OEM_1,
  FS_RMTS_PARTITION_ID_OEM_2,
  FS_RMTS_PARTITION_ID_MAX /* This should be the last member. */
};

#endif /* not __FS_RMTS_H__ */

