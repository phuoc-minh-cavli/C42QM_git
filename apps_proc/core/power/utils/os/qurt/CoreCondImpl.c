/*==============================================================================
@file CoreCondImpl.c

Implementation for L4 OS specific condition varables.

Copyright (c) 2009-2012 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.tx/6.0/power/utils/os/qurt/CoreCondImpl.c#1 $
==============================================================================*/
#include "CoreCond.h"
#include "CoreVerify.h"
#include <kernel.h>

#include "DALStdDef.h"
#include "DALSysCmn.h"

static int destroy_condvar( void *cv )
{
  okl4_condvar_destroy( cv );
  free( cv );
  return DAL_SUCCESS;
  //TODO : 
}

static CoreCondVtable cond_vtable =
{
  (CoreCondMethodType2) okl4_condvar_wait,
  (CoreCondMethodType1) okl4_condvar_signal,
  (CoreCondMethodType1) okl4_condvar_broadcast,
  destroy_condvar
};

int Core_CondInit( CoreCondType *cond, CoreCondAttrType attr )
{
  (void) attr; /* unused */
	
	CORE_DAL_VERIFY( cond->cv = malloc( sizeof(okl4_condvar_t) ) );
   
  okl4_condvar_init( cond->cv );
  cond->vtable = &cond_vtable;
  return 0;
}
