
/***********************************************************************
 * efs_scrub_config.h
 * configuration file for EFS2:APPS Partition scrub code.
 *
 * Copyright (C) 2016 QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/
 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who   what, where, why
----------   ---   ---------------------------------------------------------
2016-11-24   as    Create.

===========================================================================*/

#ifndef EFS_SCRUB_CONFIG_H
#define EFS_SCRUB_CONFIG_H


#ifndef EFS_SCRUB_STATE_FILE_PATH
  #define EFS_SCRUB_STATE_FILE_PATH   "/efs_public/fs_scrub/scrub_state.bin"
#endif

#ifndef EFS_SCRUB_DEBUG_FILE_PATH
  #define EFS_SCRUB_DEBUG_FILE_PATH  "/efs_public/fs_scrub/scrub_debug.bin"
#endif

#endif /* not EFS_SCRUB_CONFIG_H */

