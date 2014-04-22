/**********************************************************************
 * flash_ops_setup.c
 *
 * SERVICES:    Provides flash operation setup API's used by EFS
 *
 * DESCRIPTION: Provides flash operation setup API's used by EFS
 *
 * PUBLIC CLASSES:  Not Applicable
 *
 * INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A
 *
 * Copyright (c) 2016 QUALCOMM Technologies Incorporated.
 * All Rights Reserved.
 * QUALCOMM Confidential and Proprietary
 *===================================================================
 *
 *                   EDIT HISTORY FOR MODULE
 *
 * This section contains comments describing changes made to the module.
 * Notice that changes are listed in reverse chronological order.
 *
 * $Header: //components/rel/core.tx/6.0/storage/flash/src/dal/flash_ops_setup.c#1 $ $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $
 *
 * when         who     what, where, why
 * --------     ---     ----------------------------------------------
 * 09/22/16     sb      Initial Version
 *===================================================================*/

 /*===================================================================

===========================================================================*/

#include "flash.h"
#include "flash_ops_setup.h"
#include "flash_clocks.h"

#include "qurt_mutex.h"

#define FLASH_OPS_ENABLED 0

#if FLASH_OPS_ENABLED
/* Structure for flash ops syncronization */
static struct 
{
  qurt_mutex_t handle_flash_ops_sync;    /* Mutex to sync */
  uint32 ops_synclock_init;
} flash_ops_locks = {NULL, FALSE};
#endif


/* Flash Operations init */
void flash_ops_init(void)
{
#if FLASH_OPS_ENABLED
  if (flash_ops_locks.ops_synclock_init == FALSE)
  { 
    flash_ops_locks.ops_synclock_init = TRUE;

    qurt_mutex_init(&(flash_ops_locks.handle_flash_ops_sync));
    flash_clocks_init();
  }
#endif
}

/* Flash Operations deinit */
void flash_ops_deinit(void)
{
#if FLASH_OPS_ENABLED

  if (flash_ops_locks.ops_synclock_init == TRUE)
  { 
    flash_ops_locks.ops_synclock_init = FALSE;

    flash_clocks_deinit();
    qurt_mutex_destroy(&(flash_ops_locks.handle_flash_ops_sync));
  }
#endif
}

/* Flash Operations start - will be called by EFS before any
 * Flash operation
 */
void flash_ops_start(void)
{
#if FLASH_OPS_ENABLED
  /* Synchronize with other clients */
  qurt_mutex_lock(&(flash_ops_locks.handle_flash_ops_sync));

  flash_clocks_request();

  /* Leave Synchronization */
  qurt_mutex_unlock(&(flash_ops_locks.handle_flash_ops_sync));
#endif
}

/* Flash Operations end - will be called by EFS after any
 * Flash operation
 */
void flash_ops_end(void)
{
#if FLASH_OPS_ENABLED
  /* Synchronize with other clients */
  qurt_mutex_lock(&(flash_ops_locks.handle_flash_ops_sync));

  /* Release vote on all related npa nodes */
  flash_clocks_request_complete();

  /* Leave Synchronization */
  qurt_mutex_unlock(&(flash_ops_locks.handle_flash_ops_sync));
#endif
}
