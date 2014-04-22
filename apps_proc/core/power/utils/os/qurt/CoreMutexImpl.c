/*==============================================================================
@file CoreMutexImpl.c


This file provides an abstraction layer for L4 specific Mutex
functions.
 
Copyright (c) 2010-2016 QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.tx/6.0/power/utils/os/qurt/CoreMutexImpl.c#1 $
==============================================================================*/
#include "CoreVerify.h"
#include "CoreMutex.h"
#include "qurt_mutex.h"
#include <stdlib.h>

static int lock_pimutex( void *mtx )
{
  qurt_mutex_lock( mtx );
  return 0;
}

static int trylock_pimutex( void *mtx )
{
  return qurt_mutex_try_lock( mtx );
}

static int unlock_pimutex( void *mtx )
{
  qurt_mutex_unlock( mtx );
  return 0;
}

static int destroy_pimutex( void *mtx )
{
  qurt_mutex_destroy( mtx );
  free( mtx );
  return 0;
}

static CoreMutexVtable pimutex_vtable = {
  lock_pimutex,
  trylock_pimutex,
  unlock_pimutex,
  destroy_pimutex
};

int Core_MutexInit( CoreMutex *mutex, CoreMutexLockType type, CoreMutexAttr attr )
{
  int res = -1;
  if ( attr & CORE_MUTEXATTR_CVCOMPAT )
  {
    CORE_VERIFY(0);	
    }
    else
    {
    CORE_VERIFY_PTR (mutex->mtx = (void *) malloc(sizeof(qurt_mutex_t)));
    qurt_mutex_init(mutex->mtx);
	mutex->vtable = &pimutex_vtable;
    return 0;
  }
  
  return res;
}
