/*============================================================================
@file CoreObjectQueue.c

Implementation of a Object Queue that can be used to store Objects to use when request made.

Copyright (c) 2016 QUALCOMM Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

$Header: //components/rel/core.tx/6.0/power/utils/src/CoreObjectQueue.c#1 $
============================================================================*/

#include "CoreObjectQueue.h"
#include "CoreVerify.h"
#include "CoreHeap.h"
#include "dsr.h"
#include "qurt_mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

#define INITIAL_NUM_OBJECTS_REQUIRED         10
#define SUBSEQUENT_NUM_OBJECTS_REQUIRED      10
#define MIN_NUM_OBJECTS_TO_BE_AVAILBLE       10

/*status of Dsr Object in DSR ObjectQueue*/
typedef enum
{
  CORE_OBJECT_STATUS_FREE,
  CORE_OBJECT_STATUS_IN_USE
  
}Core_NodeObjectStatus_Type;

/* DSR ObjectQueue */
struct Core_Object_Queue
{
   struct Core_Object_Queue *next;
   struct Core_Object_Queue *prev;
   Core_Object_List *list_head;
   dsr_obj_t *object;
   Core_NodeObjectStatus_Type objectStatus;
	
};

/* Payload containing a node with DSR object, and user data*/
typedef struct Payload 
{
   Core_Object_Queue *ObjectNode;
   void *pData;
	
}Core_Payload;

/*Dsr Object lock*/
qurt_mutex_t object_lock;

/*Process callback lock*/
qurt_mutex_t process_lock;

/*==============================================================================
                       INTERNAL FUNCTION DECLARATIONS
 =============================================================================*/
static void Core_PoolAllocObjectMemory(int num_objects, Core_Object_List *list, unsigned int where);  
static Core_Object_Queue *Core_PoolFindFreeObject (Core_Object_List *);   	                          
static void destroy_object(Core_Object_List *list, Core_Object_Queue *Node);    	                   
static void Core_ObjectProcessCB(dsr_obj_t *dsr_obj, void *ctxt, void *payload);
static void insert_to_list(Core_Object_List *list, Core_Object_Queue *start_node, Core_Object_Queue *end_node, int num_objects, unsigned int where);



/**
 * Core_CreateObjectQueue
 *
 * @brief Allocates and initalizes a DSR ObjectQueue.
 * These ObjectQueues are used to store Dsr objects which can be enqueued later to defer work to thread context.
 */
void Core_CreateObjectQueue (Core_Object_List **ObjectQueue_list, CoreObjectProcessFcn PfnProcess)
{
  Core_Object_List *list = NULL;
  list = (Core_Object_List *) Core_Malloc (sizeof( Core_Object_List ));
  CORE_VERIFY_PTR (list);
  memset (list, 0, sizeof(Core_Object_List));
	
  qurt_mutex_lock(&object_lock);
  Core_PoolAllocObjectMemory (INITIAL_NUM_OBJECTS_REQUIRED, list, CORE_WORKQ_HEAD);
  qurt_mutex_unlock(&object_lock);

  list->pfnProcess = PfnProcess;
	  
  *ObjectQueue_list = list;
  return ;
}

/**
 * Core_PoolAllocObjectMemory
 *
 * @brief Creates "num_objects" number of DSR objects and stores them in "list"(DSR ObjectQueue)
 */
 
static void Core_PoolAllocObjectMemory(int num_objects, Core_Object_List *list,unsigned int where) 
{
  
  int ObjectCount = 0;
  boolean  dsr_ret = FALSE;
  dsr_obj_t *dsr_obj = NULL;
  Core_Object_Queue *newObjectNode = NULL, *start_node = NULL, *end_node = NULL;
	
  CORE_VERIFY(num_objects > 0);

  while(ObjectCount != num_objects )
  {
    newObjectNode = ( Core_Object_Queue * ) Core_Malloc (sizeof( Core_Object_Queue )); 
    CORE_VERIFY_PTR ( newObjectNode ); 
    if(0 == ObjectCount) 
    {
      start_node = newObjectNode;
      end_node = newObjectNode;
      newObjectNode->next = NULL;
      newObjectNode->prev = NULL; 
    }
    else 
    {
      end_node->next      = newObjectNode;
      newObjectNode->next = NULL;
      newObjectNode->prev = end_node;
      end_node = newObjectNode;
    }       
		
    dsr_ret = dsr_create ( &dsr_obj, Core_ObjectProcessCB, NULL, DSR_PRI_MEDIUM, DSR_CONTEXT_WORKLOOP );
    if(!dsr_ret)
    {
      if(0 == ObjectCount) 
      {
        CORE_VERIFY(0);
      }
      else
      {
        (newObjectNode->prev)->next = NULL;
  	end_node = newObjectNode->prev;
        Core_Free(newObjectNode);
        break;
      }
    }    
    newObjectNode->object = (dsr_obj_t *)dsr_obj;       
    ++ObjectCount;
  }
    
  insert_to_list(list, start_node, end_node, ObjectCount, where); 
	
}

/**
 * Core_WorkQueueInsert
 *
 * @brief Inserts the DSR object to existing object list at position "where"(head/tail).
 */
static void insert_to_list(Core_Object_List *list,Core_Object_Queue *start_node,Core_Object_Queue *end_node,int num_objects,unsigned int where) 
{
    CORE_VERIFY_PTR(list);
    CORE_VERIFY_PTR(start_node);
    CORE_VERIFY_PTR(end_node);
	
    if( list->head == NULL) 
    {
      list->head = start_node;
      list->tail = end_node;
    }
    else if(where == CORE_WORKQ_TAIL)
    {
       (list->tail)->next = start_node;
       start_node->prev = list->tail;
       list->tail = end_node;
    }
    else //if(ADD at_head)
    {
       end_node->next = list->head;
       (list->head)->prev = end_node;
       list->head = start_node;
    }
    list->numObjectsCreated += num_objects;
    list->numObjectsAvailable += num_objects;
    list->FreeObjects += num_objects;
	
}

/**
 * Core_CreteObjectQueue
 *
 * @brief Obtains the free DSR object from DSR Objectqueue and enqueues it into the DSR workloop.
 */
DALResult Core_ObjectEnqueue (Core_Object_List *list, void *pdata,unsigned int no_signal)
{	
  int ret = 1;
  dsr_obj_t *Object  = NULL;
  Core_Object_Queue *ObjectNode = NULL;
  Core_Payload *payload = NULL;
  CORE_VERIFY_PTR( list );
  
  payload = (Core_Payload*) Core_Malloc (sizeof (Core_Payload));
  CORE_VERIFY_PTR( payload);

  ObjectNode = Core_PoolFindFreeObject (list);
  ObjectNode->list_head = list;
  
  payload->ObjectNode = ObjectNode;
  payload->pData = pdata;
  
  Object = ObjectNode->object;

  ret = dsr_enqueue(Object, payload);
  if( ret ) 
  {
    return  DAL_SUCCESS;
  }
  else 
  {
    return DAL_ERROR;
  }
} 

/**
 * Core_PoolFindFreeObject
 *
 * @brief Obtains and returns the free dsr object from DSR Objectqueue.
 * @param list: DSR Objectqueue.
 */
static Core_Object_Queue * Core_PoolFindFreeObject  (Core_Object_List *list) 
{
    
  Core_Object_Queue *temp = NULL;
	
  CORE_VERIFY_PTR(list);

  temp = list->head;
	
  qurt_mutex_lock(&object_lock);
  
  while ((temp != NULL) && (temp->objectStatus  == CORE_OBJECT_STATUS_IN_USE )) 
  {
    temp = temp->next;
  }
  if(temp == NULL)
  {
    Core_PoolAllocObjectMemory(SUBSEQUENT_NUM_OBJECTS_REQUIRED, list, CORE_WORKQ_HEAD);
    if( ((list->head)->objectStatus)  == CORE_OBJECT_STATUS_FREE )
    {
      temp = list->head;
    }
    else 
    {
      qurt_mutex_unlock(&object_lock);
      temp = Core_PoolFindFreeObject (list);
      temp->objectStatus = CORE_OBJECT_STATUS_IN_USE;
      list->FreeObjects-- ;
      return temp;
    }
  }
  temp->objectStatus = CORE_OBJECT_STATUS_IN_USE;
  list->FreeObjects -- ;
  qurt_mutex_unlock(&object_lock);
	
  return temp;
}

/**
 * Core_ObjectProcessCB
 *
 * @brief Passed as callback function to DSR, and is later invoked in DSR thread context to process requests.
 */
void Core_ObjectProcessCB(dsr_obj_t *dsr_obj, void *ctxt, void *payload)
{
    DALResult ret;
    Core_Object_List *list = NULL;   
    Core_Object_Queue *ObjectNode = NULL;
    void *pData = NULL;
    Core_Payload *ptr = (Core_Payload *) payload;
    
    CORE_VERIFY_PTR(ptr);
    pData = (void *) ptr->pData;
    ObjectNode =  (Core_Object_Queue *)ptr->ObjectNode;
    list = ObjectNode->list_head;
    CORE_VERIFY_PTR(list);
    CORE_VERIFY_PTR(list->pfnProcess);
	
    qurt_mutex_lock(&process_lock);
    ret = list->pfnProcess(pData);               
    CORE_DAL_VERIFY( ret );
	
    qurt_mutex_lock(&object_lock);
    if( (list->numObjectsAvailable <= MIN_NUM_OBJECTS_TO_BE_AVAILBLE) || (list->FreeObjects < 10 ) )
    {
      ObjectNode->objectStatus = CORE_OBJECT_STATUS_FREE;
      list->FreeObjects ++;
      qurt_mutex_unlock(&object_lock);
    }
    else 
    {
      destroy_object(list, ObjectNode);
    }
    Core_Free(pData);
    Core_Free(payload);
    qurt_mutex_unlock(&process_lock);
    return ;
}

/* destroy_object
 *
 * @brief Pops the dsr object from DSR ObjectQueue & destroys it. 
 */
static void destroy_object(Core_Object_List *list,Core_Object_Queue *ObjectNode)
{
   CORE_VERIFY_PTR(ObjectNode);
   CORE_VERIFY_PTR(ObjectNode->object);
   CORE_VERIFY_PTR(list);
	
   if(ObjectNode->prev == NULL && ObjectNode->next == NULL)
   {
     list->head = NULL;
     list->tail = NULL;
   }
   else if(ObjectNode->prev == NULL)
   {
     list->head = ObjectNode->next;
     (ObjectNode->next)->prev = NULL;
     qurt_mutex_unlock(&object_lock);
   }
   else if(ObjectNode->next == NULL)
   {
     (ObjectNode->prev)->next = NULL;
     list->tail = ObjectNode->prev;
     qurt_mutex_unlock(&object_lock);
   }
   else 
   {
     (ObjectNode->prev)->next = ObjectNode->next;
     (ObjectNode->next)->prev = ObjectNode->prev;
     qurt_mutex_unlock(&object_lock);
   }
 	
   if(!dsr_destroy(ObjectNode->object))
   {
     CORE_VERIFY(0);
   }
   Core_Free(ObjectNode);
	
   list->numObjectsAvailable--;
   list->DestroyedObjects++;
   return ;
}

/**
 * Core_ObjectDequeue
 *
 * @brief This function dequeues a DSR object from the DSR pending list 
 */
DALResult Core_ObjectDequeue (Core_Object_List *ObjectQueue_list,Core_Object_Queue *ObjectNode, void *pdata)
{
/* TODO 
 *  dsr_dequeue(ObjectNode->object);
 *  set the object status to free to reuse it (or) free and delete from queue.	
*/
    return DAL_SUCCESS;
}

#ifdef __cplusplus
}
#endif

