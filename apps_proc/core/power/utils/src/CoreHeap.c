/*============================================================================
@file CoreHeap.c

Implements CoreHeap.h

Copyright (c) 2013-2016 QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.tx/6.0/power/utils/src/CoreHeap.c#1 $
============================================================================*/

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void* Core_Malloc( unsigned int size )
{
  void *ptr = NULL;
  ptr = malloc(size);
  return ptr; 
}

void Core_Free( void *ptr )
{
  if ( ptr )
  {
    free( ptr );
  }
}

#ifdef __cplusplus
}
#endif
