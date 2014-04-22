
#ifndef OS_API_QUEUE_H
#define OS_API_QUEUE_H
/*============================================================================
  @file me_queue.h
 
  This file contains routines for managing the queues. It is basically a
  wrapper around the AMSS queue.c functionality.  If we ever wanted to replace
  queue.c with something else, this will make it a little easier.
 
  Copyright (c) 2008 - 2012 Qualcomm Technologies Incorporated.
  Qualcomm Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2013 - 2014 Qualcomm Atheros, Inc.
  Qualcomm Atheros Confidential and Proprietary. All Rights Reserved.
  Copyright (c) 2015 Qualcomm Technologies , Inc.
  Qualcomm Technologies Confidential and Proprietary. All Rights Reserved. 
============================================================================*/
/* $Header: //components/rel/gnss8.mpss/9.5.1.1/gnss/os_api/inc/os_api_queue.h#1 $ */

/*----------------------------------------------------------------------------
 * Include Files
 * -------------------------------------------------------------------------*/


#ifdef FEATURE_GNSS_UIMAGE_SUPPORT
#include "uqueue.h"
#endif
#include "queue.h"


/*----------------------------------------------------------------------------
 * Preprocessor Definitions and Constants
 * -------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
 * Type Declarations
 * -------------------------------------------------------------------------*/



#ifdef FEATURE_GNSS_UIMAGE_SUPPORT
typedef uq_type            QUEUE_T; 
typedef uq_link_type       QUEUE_ELEMENT_INFO_T; 
#else
typedef q_type      QUEUE_T;
typedef q_link_type QUEUE_ELEMENT_INFO_T;
#endif
typedef void        QUEUE_ELEMENT_T;



/*----------------------------------------------------------------------------
 * Function Declarations and Documentation
 * -------------------------------------------------------------------------*/

/**
 @brief os_QueueInitializeQueue 
  
 This routine does the initialization of the queue before use.

 @param p_Queue: The queue to initialize

 @return none.
*/
void os_QueueInitializeQueue( QUEUE_T *p_Queue );


/**
 @brief os_QueueNumElements 
  
 This routine returns the number of elements in the queue.

 @param p_Queue: The desired queue

 @return Number of elements in queue.
*/
uint32 os_QueueNumElements( QUEUE_T *p_Queue );


/**
 @brief os_QueueDequeueElement 
  
 This routine removes the first element from the queue.

 @param p_Queue: The desired queue

 @return none.
*/
void * os_QueueDequeueElement( QUEUE_T *p_Queue );


/**
 @brief os_QueueEnqueueElement 
  
 This routine adds the element to the queue.

 @param p_Queue: The desired queue
 @param p_ElementInfo: The element to add

 @return none.
*/
void os_QueueEnqueueElement( QUEUE_T *p_Queue, QUEUE_ELEMENT_INFO_T *p_ElementInfo );


/**
 @brief os_QueueDeleteElement 
  
 This routine deletes a specific element from the queue.

 @param p_Queue: The desired queue
 @param p_ElementInfo: The element to delete

 @return none.
*/
void os_QueueDeleteElement( QUEUE_T *p_Queue, QUEUE_ELEMENT_INFO_T *p_ElementInfo );


/**
 @brief os_QueueNextElementPtr 
  
 This routine returns but does not delete the next element in the queue.

 @param p_Queue: The desired queue
 @param p_ElementInfo: The element after to this to return

 @return The next element in the queue.
*/
QUEUE_ELEMENT_T * os_QueueNextElementPtr( QUEUE_T *p_Queue, QUEUE_ELEMENT_INFO_T *p_Element );


/**
 @brief os_QueueHeadElementPtr 
  
 This routine returns but does not remove the first element in the queue.

 @param p_Queue: The desired queue

 @return The first element in the queue.
*/
QUEUE_ELEMENT_T * os_QueueHeadElementPtr( QUEUE_T *p_Queue );


/**
 @brief os_QueueInsertElement 
  
 This routine adds a new element before the a specified one.

 @param p_Queue: The desired queue
 @param p_ElementInfo: The element to insert
 @param p_InsertBeforeElement: The location of the new element

 @return none.
*/
void os_QueueInsertElement( QUEUE_T *p_Queue, QUEUE_ELEMENT_INFO_T *p_Element,
                            QUEUE_ELEMENT_INFO_T *p_InsertBeforeElement );


/**
 @brief os_QueueFindElement 
  
 This routine finds a element in the queue based on the supplied "compare_func"
 and "compare_val".

 @param p_Queue: The desired queue
 @param compare_func: The function to use when comparing the elements
 @param compare_val: The value to use when comparing the elements

 @return none.
*/
void *os_QueueFindElement
( 
  QUEUE_T *p_Queue,  
  q_compare_func_type compare_func,
  void               *compare_val
);



/**
 @brief os_QueueMoveElement 
  
 This routine dequeues an element from a queue and enqueues it into another
 queue.

 @param p_FromQueue: The queue to remove the element
 @param p_ToQueue: The queue to insert the element
 @param p_ElementInfo: The desired element

 @return none.
*/
void os_QueueMoveElement
( 
  QUEUE_T *p_FromQueue, 
  QUEUE_T *p_ToQueue, 
  QUEUE_ELEMENT_INFO_T *p_ElementInfo
);


/**
 @brief os_QueueDestroy 
  
  This function destroys a specified queue. It should be called if you
  do not require this queue anymore.

  Elements in the queue will  not be accessible through this queue 
  anymore. It is user's responsibility to deallocate the memory allocated 
  for the queue and its elements to avoid leaks

 @param p_Queue: The queue to destroy

 @return none.
*/
void os_QueueDestroy
( 
  QUEUE_T *p_Queue
);


#ifdef FEATURE_GNSS_UIMAGE_SUPPORT
/*----------------------------------------------------------------------------
 * Function Declarations and Documentation for uQueues
 * -------------------------------------------------------------------------*/

/**
 @brief os_uQueueInitializeQueue 
  
 This routine does the initialization of the queue before use.

 @param p_Queue: The Micro queue to initialize. Do it at bootup 

 @return none.
*/
void os_uQueueInitializeQueue( QUEUE_T *p_Queue );


/**
 @brief os_uQueueNumElements 
  
 This routine returns the number of elements in the queue.

 @param p_Queue: The desired uqueue

 @return Number of elements in micro queue.
*/
uint32 os_uQueueNumElements( QUEUE_T *p_Queue );


/**
 @brief os_uQueueDequeueElement 
  
 This routine removes the first element from the queue.

 @param p_Queue: The desired micro queue

 @return none.
*/
void * os_uQueueDequeueElement( QUEUE_T *p_Queue );


/**
 @brief os_uQueueEnqueueElement 
  
 This routine adds the element to the queue.

 @param p_Queue: The desired micro queue
 @param p_ElementInfo: The element to add

 @return none.
*/
void os_uQueueEnqueueElement( QUEUE_T *p_Queue, QUEUE_ELEMENT_INFO_T *p_ElementInfo );


#if 1
/**
 @brief os_uQueueDeleteElement 
  
 This routine deletes a specific element from the queue.

 @param p_Queue: The desired queue
 @param p_ElementInfo: The element to delete

 @return none.
*/
void os_uQueueDeleteElement( QUEUE_T *p_Queue, QUEUE_ELEMENT_INFO_T *p_ElementInfo );

/**
 @brief os_uQueueNextElementPtr 
  
 This routine returns but does not delete the next element in the queue.

 @param p_Queue: The desired queue
 @param p_ElementInfo: The element after to this to return

 @return The next element in the queue.
*/
QUEUE_ELEMENT_T * os_uQueueNextElementPtr( QUEUE_T *p_Queue, QUEUE_ELEMENT_INFO_T *p_ElementInfo );


/**
 @brief os_uQueueHeadElementPtr 
  
 This routine returns but does not remove the first element in the queue.

 @param p_Queue: The desired queue

 @return The first element in the queue.
*/
QUEUE_ELEMENT_T * os_uQueueHeadElementPtr( QUEUE_T *p_Queue );


/**
 @brief os_uQueueInsertElement 
  
 This routine adds a new element before the a specified one.

 @param p_Queue: The desired queue
 @param p_ElementInfo: The element to insert
 @param p_InsertBeforeElement: The location of the new element

 @return none.
*/
void os_uQueueInsertElement( QUEUE_T *p_Queue, QUEUE_ELEMENT_INFO_T *p_Element,
                            QUEUE_ELEMENT_INFO_T *p_InsertBeforeElement );


/**
 @brief os_uQueueMoveElement 
  
 This routine dequeues an element from a queue and enqueues it into another
 queue.

 @param p_FromQueue: The queue to remove the element
 @param p_ToQueue: The queue to insert the element
 @param p_ElementInfo: The desired element

 @return none.
*/
void os_uQueueMoveElement
( 
  QUEUE_T *p_FromQueue, 
  QUEUE_T *p_ToQueue, 
  QUEUE_ELEMENT_INFO_T *p_ElementInfo
);

#endif /*#if 1*/

/**
 @brief os_uQueueDestroy 
  
  This function destroys a specified queue. It should be called if you
  do not require this queue anymore.

  Elements in the queue will  not be accessible through this queue 
  anymore. It is user's responsibility to deallocate the memory allocated 
  for the queue and its elements to avoid leaks

 @param p_Queue: The micro queue to destroy

 @return none.
*/
void os_uQueueDestroy
( 
  QUEUE_T *p_Queue
);

#endif /* FEATURE_GNSS_UIMAGE_SUPPORT*/

#endif /* OS_API_QUEUE_H */
