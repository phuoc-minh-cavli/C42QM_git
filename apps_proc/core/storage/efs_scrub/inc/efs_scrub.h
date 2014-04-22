
/***********************************************************************
 * efs_scrub.h
 * Header for EFS2:APPS Partition scrub code.
 *
 * Copyright (C) 2016, 2019 QUALCOMM Technologies, Inc.
 *
 ***********************************************************************/
 
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.


when         who   what, where, why
----------   ---   ---------------------------------------------------------
2019-08-22   as    Add support to featurise efs_scrub
2016-11-17   as    Create.

===========================================================================*/

#ifndef EFS_SCRUB_H
#define EFS_SCRUB_H

#ifdef FEATURE_ENABLE_EFS_SCRUB

/**----------------------------------------------------------------------------
 * Initialises scrub for APPS:EFS2 partition.
 *
 * This function will initialise scrub for efs partition on APPS side .It is 
 * called from efs_initialisation.
 *
 * @return
 *  Once initialised ,Client is ready to recieve a signal from server side,
 *  for when to start srubbing.
----------------------------------------------------------------------------**/
void efs_scrub_init (void);

/**----------------------------------------------------------------------------
 * This API is called by FS task when scrub signal gets set.
----------------------------------------------------------------------------**/
void efs_scrub_signal_received(void);

#else

static inline void efs_scrub_init (void)
{
}
static inline void efs_scrub_signal_received (void)
{
}
#endif

#endif /* not EFS_SCRUB_H */
