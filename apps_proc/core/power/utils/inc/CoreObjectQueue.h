/*============================================================================
@file CoreObjectQueue.h

Core Work Queue API.

Copyright (c) 2016 QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.tx/6.0/power/utils/inc/CoreObjectQueue.h#1 $
============================================================================*/
#ifndef COREOBJECTQUEUE_H
#define COREOBJECTQUEUE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DALStdDef.h"

#define CORE_WORKQ_HEAD            0
#define CORE_WORKQ_TAIL            1

typedef struct Core_Object_List  Core_Object_List;
typedef struct Core_Object_Queue Core_Object_Queue;

/*User callback function pointer*/
typedef int (*CoreObjectProcessFcn)(void *);

/*ObjectQueueList containing DSR ObjectQueue info and user callback function */
struct Core_Object_List
{
  Core_Object_Queue *head;
  Core_Object_Queue *tail;	
  int numObjectsCreated;
  int numObjectsAvailable;
  int FreeObjects;
  int DestroyedObjects;
  CoreObjectProcessFcn pfnProcess;
	
};

/*==============================================================================
                       EXTERNAL FUNCTION DECLARATIONS
 =============================================================================*/
void Core_CreateObjectQueue (Core_Object_List **ObjectQueue_list, CoreObjectProcessFcn PfnProcess);
DALResult Core_ObjectEnqueue (Core_Object_List *list, void *pdata, unsigned int no_signal);
DALResult Core_ObjectDequeue (Core_Object_List *list, Core_Object_Queue *ObjectNode, void *pdata);

#ifdef __cplusplus
}
#endif

#endif

