#ifndef TRACER_MUTEX_H
#define TRACER_MUTEX_H
/*===========================================================================
  @file tracer_mutex.h

   tracer_lock mutex
   To be included by only one file unless modified.

               Copyright (c) 2012 Qualcomm Technologies Incorporated.
               All rights reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/* $Header: */

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/
#include "qurt_mutex.h"
#include "qdss_osal.h"

extern qurt_mutex_t hMutex;

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/

__inline void tracer_mutex_init(void)
{
   qdss_osal_mutex_init(&hMutex);
   return;
}

__inline void tracer_mutex_destroy(void)
{
   qdss_osal_mutex_destroy(&hMutex);	
   return;
}

__inline void tracer_mutex_lock(void)
{
   qdss_osal_mutex_lock(&hMutex);
}

__inline void tracer_mutex_unlock(void)
{
   qdss_osal_mutex_unlock(&hMutex);
}

#endif /* #ifndef TRACER_MUTEX_H */

