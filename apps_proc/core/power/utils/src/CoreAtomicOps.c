/*==============================================================================
@file CoreAtomicOps.c

Implements an atomic increment or decrement

Copyright (c) 2010-2016 QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.tx/6.0/power/utils/src/CoreAtomicOps.c#1 $
==============================================================================*/
#include "CoreAtomicOps.h"
#include "qurt_mutex.h"

#if defined(UTILS_USE_BLAST) || defined(UTILS_USE_QURT)
#include "atomic_ops.h"

qurt_mutex_t hMutex;
/**
   @brief Core_AtomicInc - Atomic increment
 
   Increments by one the value of the specified 32-bit variable
   as an atomic operation. 
  
   @param pCount : pointer to the variable to be incremented
   @return unsigned : the resulting incremented value
 */
unsigned Core_AtomicInc(volatile uint32_t * pCount) 
{
  return(atomic_inc_return((atomic_word_t *)pCount));
}

/**
   @brief Core_AtomicDec - Atomic decrement
 
   Decrements by one the value of the specified 32-bit variable 
   as an atomic operation. 
  
   @param pCount : pointer to the variable to be decremented
   @return unsigned : the resulting decremented value
 */
unsigned Core_AtomicDec(volatile uint32_t * pCount) 
{
  return(atomic_dec_return((atomic_word_t *)pCount));
}

#elif defined(WIN8EA)
#include <wdm.h>

unsigned Core_AtomicInc(volatile uint32_t * pCount) 
{
  return InterlockedIncrement( (volatile long *)pCount );
}

unsigned Core_AtomicDec(volatile uint32_t * pCount) 
{
  return InterlockedDecrement( (volatile long *)pCount );
}

#elif !USES_QNX  

#define AtomicEnter() \
  if ( NULL == atomic_sync )                                                 \
  {                                                                          \
    qurt_mutex_init(&hMutex);                                                 \ 
  }                                                                          \
  qurt_mutex_lock(&hMutex);
#define AtomicLeave() \
  qurt_mutex_unlock(&hMutex);

unsigned Core_AtomicInc( volatile uint32_t * pCount )
{
   unsigned newval;

   AtomicEnter();
   newval = ++(*pCount);
   AtomicLeave();

   return newval;
}

unsigned Core_AtomicDec( volatile uint32_t * pCount )
{
   unsigned newval;

   AtomicEnter();
   newval = --(*pCount);
   AtomicLeave();

   return newval;
}

#else 

#error "ERROR: CoreAtomicOps not defined for platform"

#endif
