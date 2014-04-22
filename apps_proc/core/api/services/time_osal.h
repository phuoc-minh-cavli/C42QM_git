 /***********************************************************************
 * time_osal.h
 * This file maps the timer osal types to underlying native kernel types.
 *
 * Copyright (C) 2013 Qualcomm Technologies, Inc.
 *
 *
 ***********************************************************************/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  
===========================================================================*/

#ifndef __TIME_OSAL_H__
#define __TIME_OSAL_H__

#include "qurt.h"

typedef void *                           time_osal_notify_obj_ptr;

typedef unsigned long int                time_osal_notify_data;

typedef qurt_thread_t					     time_osal_thread_id;

typedef qurt_mutex_t					     time_osal_mutex_t;

#define TIME_OSAL_GET_THREAD_ID() qurt_thread_get_id()

/* Time-services mutex lock / unlock macro's */
#define TIME_OSAL_MUTEX_INIT(X)  \
	  qurt_mutex_init(&X); \
	
#define TIME_OSAL_MUTEX_LOCK(X)  \
	  qurt_mutex_lock(&X); \
	
#define TIME_OSAL_MUTEX_UNLOCK(X)  \
	  qurt_mutex_unlock(&X); \

#endif /* __TIME_OSAL_H__*/

