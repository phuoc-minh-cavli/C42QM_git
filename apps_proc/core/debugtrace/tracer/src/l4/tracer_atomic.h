#ifndef TRACER_ATOMIC_H
#define TRACER_ATOMIC_H
/*===========================================================================
  @file tracer_atomic.h

  Atomic operations.

               Copyright (c) 2012 Qualcomm Technologies Incorporated.
               All rights reserved.
               Qualcomm Confidential and Proprietary
===========================================================================*/
/* $Header: //components/rel/core.tx/6.0/debugtrace/tracer/src/l4/tracer_atomic.h#1 $ */

/*---------------------------------------------------------------------------
 * Include Files
 * ------------------------------------------------------------------------*/
#include "atomic_ops.h"

/*---------------------------------------------------------------------------
 * Function Declarations and Documentation
 * ------------------------------------------------------------------------*/

#define TRACER_ATOMIC_INC(_pVal)          atomic_inc((atomic_word_t*)_pVal)
#define TRACER_ATOMIC_DEC(_pVal)          atomic_dec((atomic_word_t*)_pVal)
#define TRACER_ATOMIC_SET(_pVal, _iVal)   atomic_set((atomic_word_t*)_pVal,_iVal)

#define TRACER_ATOMIC_CAS(_pVar, _oVar, _nVar) \
   atomic_compare_and_set((atomic_word_t*)_pVar, _oVar, _nVar)

#define TRACER_ATOMIC_ADD_RETURN(_pVal, _iVal) \
   atomic_add_return((atomic_word_t*)_pVal,_iVal)

/*-------------------------------------------------------------------------*/

// 64-bit support not available.
#define TRACER_ATOMIC64_SET(_pVal, _iVal) \
   TRACER_ATOMIC_SET(_pVal,(_iVal&0xFFFFFFFF)); \
   TRACER_ATOMIC_SET(((uint32*)_pVal) + 1,((_iVal>>32)&0xFFFFFFFF))


#endif /* #ifndef TRACER_ATOMIC_H */

