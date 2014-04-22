/*==============================================================================
@file CoreThreadImpl.c

Implementation of l4 specific thread routines for CoreThread

Copyright (c) 2009-2016 QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.tx/6.0/power/utils/os/qurt/CoreThreadImpl.c#1 $
==============================================================================*/
#include "CoreThread.h"
#include "CoreVerify.h"
#include "qurt.h"

unsigned int Core_GetThreadSelfPriority( void )
{
  return (255 - qurt_thread_get_priority( qurt_thread_get_id() ));
}

void Core_SetThreadPriority( unsigned int tid,
                                                unsigned int priority)
{
  qurt_thread_set_priority( tid, (255 - priority) );
}

