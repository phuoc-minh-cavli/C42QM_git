/*==============================================================================
@file CoreCond.c

Implements the create and destroy functions for a condition variable.  All other
methods for the condition variable are implemented in os/<os>/CoreCondImpl.c

Copyright (c) 2009-2016 QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.tx/6.0/power/utils/src/CoreCond.c#1 $
==============================================================================*/
#include "CoreCond.h"
#include "CoreVerify.h"
#include <stdlib.h>

CoreCondType* Core_CondCreate( CoreCondAttrType attr )
{
  CoreCondType *cond;

  CORE_VERIFY ( cond = malloc( sizeof(CoreCondType) ) );

  memset( cond, 0, sizeof(CoreCondType) );
  cond->attr = attr;

  if ( Core_CondInit( cond, attr ) != 0 )
  {
    free( cond );
   
    cond = NULL;
  }
  return cond;
}

void Core_CondDestroy( CoreCondType *cond )
{
  /* Be tolerant of NULL input */
  if ( cond )
  {
    CORE_VERIFY_PTR( cond->vtable->destroy );
    cond->vtable->destroy( cond->cv );
    free( cond );
  }
}
