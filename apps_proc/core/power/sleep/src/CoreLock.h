#ifndef SLEEP_CORELOCK_H
#define SLEEP_CORELOCK_H

/*=============================================================================
  FILE:         CoreLock.h
  
  OVERVIEW:     This file provides lock abstraction for different type of OS
                locks.
 
                Copyright (c) 2016 QUALCOMM Technologies Incorporated.
                All Rights Reserved.
                Qualcomm Confidential and Proprietary
=============================================================================*/
/*=============================================================================
$Header: 
$DateTime: 
$Author: 
=============================================================================*/

#include "CoreVerify.h"

#ifdef __cplusplus
extern "C" {
#endif 

#define CoreLockType qurt_mutex_t

#define Core_LockCreateObj(handle_ptr) qurt_mutex_init(handle_ptr))
  
#define Core_LockAcquire(handle) qurt_mutex_lock(handle)

#define Core_LockRelease(handle) qurt_mutex_unlock(handle)


#ifdef __cplusplus
}
#endif

#endif

