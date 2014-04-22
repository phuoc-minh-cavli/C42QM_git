/*
  Copyright (C) 2009-2010 Qualcomm Technologies Incorporated.
  All rights reserved.
  QUALCOMM Proprietary/GTDR.

  $Header: //components/rel/audio.tx/2.0/apr/osal/variant/qurt/src/apr_lock.c#1 $
  $Author: pwbldsvc $
*/

#include <stddef.h>
#include "apr_errcodes.h"
#include "apr_lock.h"
#include "qurt_mutex.h"
#include "malloc.h"

typedef struct apr_lock_internal_t
{
  qurt_mutex_t lock;
}apr_lock_internal_t;


APR_INTERNAL void apr_malloc(uint32_t size, void **ptr )
{
	*ptr = (void*) malloc(size);
}

APR_INTERNAL void apr_free(void *ptr)
{
	free(ptr);
	ptr=NULL;
}


APR_INTERNAL int32_t apr_lock_create (
  uint32_t lock_type,
  apr_lock_t* ret_lock
)
{
  apr_lock_internal_t* the_lock;

  if ( ret_lock == NULL )
  {
    return APR_EBADPARAM;
  }

  switch ( lock_type )
  {
  case APR_LOCK_TYPE_MUTEX:
  case APR_LOCK_TYPE_INTERRUPT:
    break;

  default:
    return APR_EBADPARAM;
  }

  apr_malloc( sizeof( apr_lock_internal_t ), ( ( void** ) &the_lock ) );
  if ( the_lock == NULL )
  {
    return APR_ENORESOURCE;
  }

  qurt_mutex_init( &the_lock->lock );

  *ret_lock = the_lock;

  return APR_EOK;
}


APR_INTERNAL int32_t apr_lock_destroy (
  apr_lock_t lock
)
{
  apr_lock_internal_t* the_lock = lock;

  if ( the_lock == NULL )
  {
    return APR_EBADPARAM;
  }

  qurt_mutex_destroy( &the_lock->lock );

  apr_free( the_lock );
  
  return APR_EOK;
}


APR_INTERNAL int32_t apr_lock_enter (
  apr_lock_t lock
)
{
  apr_lock_internal_t* the_lock = lock;

  if ( the_lock == NULL )
  {
    return APR_EBADPARAM;
  }

  qurt_mutex_lock( &the_lock->lock );

  return APR_EOK;
  
}



APR_INTERNAL int32_t apr_lock_leave (
  apr_lock_t lock
)
{
  apr_lock_internal_t* the_lock = lock;

  if ( the_lock == NULL )
  {
    return APR_EBADPARAM;
  }

  qurt_mutex_unlock( &the_lock->lock );

  return APR_EOK;
}

