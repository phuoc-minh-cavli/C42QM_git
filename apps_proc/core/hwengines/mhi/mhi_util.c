/**
  @file mhi_util.c
  @brief
  This file contains the implementation of various utility functions used by MHI
  such as queue management etc..
    

*/
/*
===============================================================================

                             Edit History

 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/03/14   am      Added support for collecting time stats
10/22/13   nk      Created
===============================================================================
                   Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "mhi_util.h"
#include "mhi_i.h"
#include "mhi_osal.h"

/** 
 * @brief      mhi_q_init
 *
 *             Initializes the specified queue 
 *              
 * 
 * @param[in]  *q - Queue to initialize 
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             None
 * 
 * @sideeffect None 
 */
void mhi_q_init(list_t *q)
{
   if(NULL == q) 
      return; 

   q->next=q; 
   q->prev=q; 
}

/** 
 * @brief      mhi_q_queue
 *
 *             Queues an element on to the specified queue
 *              
 * 
 * @param[in]  *q - Queue to queue on 
 * @param[in]  *i - Element to queue
 *
 * @dependencies
 *             mhi_q_init must be called first
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded, otherwise ERROR
 * 
 * @sideeffect None 
 */
uint32 mhi_q_queue(list_t *q, list_t *i) 
{
   if (NULL == i || NULL == q)
      return ERROR; 

   i->next=q->prev->next; 
   q->prev->next = i; 
   i->prev=q->prev;
   q->prev=i;

   return SUCCESS;
}

/** 
 * @brief      mhi_q_queue_head
 *
 *             Queues an element to the head of a queue 
 *              
 * 
 * @param[in]  *q - Queue to use
 * @param[in]  *i - Element to queue
 *
 * @dependencies
 *             mhi_q_init must be called first
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded, otherwise ERROR 
 * 
 * @sideeffect None 
 */
int32 mhi_q_queue_head(list_t *q, list_t *i) 
{
   if (NULL == i || NULL == q)
      return ERROR; 

   i->next=q->next; 
   i->prev=q;
   i->next->prev=i;  
   q->next = i;  

   return SUCCESS;
}

/** 
 * @brief      mhi_q_insert_before
 *
 *             Insert a new element to before the specified queue element
 *              
 * 
 * @param[in]  *at - Element to insert before
 * @param[in]  *i  - Element to insert
 *
 * @dependencies
 *             mhi_q_init must be called first
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded, otherwise ERROR 
 * 
 * @sideeffect None 
 */
int32 mhi_q_insert_before(list_t *at, list_t *i) 
{
   if (NULL == i || NULL == at)
      return ERROR; 

   i->next=at; 
   i->prev=at->prev; 
   i->prev->next=i; 
   at->prev=i; 

   return SUCCESS;
}

/** 
 * @brief      mhi_q_peek_head
 *
 *             Sets the paramater **i to the next element of the specified queue 
 *              
 * 
 * @param[in]  *q   - Queue 
 * @param[out]  **i - Set to next element on queue
 *
 * @dependencies
 *             mhi_q_init must be called first
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded, otherwise ERROR 
 * 
 * @sideeffect None 
 */
int32 mhi_q_peek_head(list_t *q, list_t **i)
{
   if(NULL == q || NULL == i)
      return ERROR;

   /* If queue is empty return NULL */
   if(q->next == q)
   {
      *i=NULL;
      return SUCCESS; 
   }
   *i=q->next;

   return SUCCESS;
}

/** 
 * @brief      mhi_q_dequeue
 *
 *             Dequeues an element from the queue and sets the paramater **i to point to that element 
 *              
 * 
 * @param[in]  *q  - Queue 
 * @param[out] **i - Set to the next element on the queue  
 *
 * @dependencies
 *             mhi_q_init must be called first
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded, otherwise ERROR 
 * 
 * @sideeffect None 
 */
int32 mhi_q_dequeue(list_t *q, list_t **i)
{
   if(NULL == q || NULL == i)
   {
      return ERROR; 
   }
   if(q->next == q)
   {
      *i=NULL; 
      return ERROR; 
   }
   *i=q->next; 
   q->next=q->next->next; 
   q->next->prev=q; 
   (*i)->next=NULL; 
   (*i)->prev=NULL; 
   return SUCCESS;
}

/** 
 * @brief      mhi_q_get_next
 *
 *             Gets the next element on queue starting at the **current element. Will not remove 
 *             the element off the queue
 *              
 * 
 * @param[in]  *q - Queue 
 * @param[in/out]  **current - Specifies the current element, and is set to the next element
 *
 * @dependencies
 *             mhi_q_init must be called first
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded, otherwise ERROR 
 * 
 * @sideeffect None 
 */
int32 mhi_q_get_next(list_t *q, list_t** current)
{
   if(NULL == q || NULL == current)
      return ERROR; 

   if(NULL == *current) 
      *current=q->next;
   else
      *current=(*current)->next;

   /* Check for end of queue */        
   if(*current == q)
      *current=NULL; 

   return SUCCESS;
}

/** 
 * @brief      mhi_q_remove
 *
 *             Remove the specified element from the queue
 *              
 * 
 * @param[in]  *i - Element to remove 
 *
 * @dependencies
 *             mhi_q_init must be called first
 *              
 * 
 * @return       
 *             SUCCESS if operation succeeded, otherwise ERROR 
 * 
 * @sideeffect None 
 */
int32 mhi_q_remove(list_t *i)
{
   if(NULL == i)
      return ERROR;

   i->prev->next=i->next; 
   i->next->prev=i->prev; 

   i->next=NULL; 
   i->prev=NULL; 

   return SUCCESS; 
}

/** 
 * @brief      mhi_q_is_queue_empty
 *
 *             Returns TRUE if the queue is empty
 *              
 * 
 * @param[in]  *q - Queue
 *
 * @dependencies
 *             mhi_q_init must be called first
 *              
 * 
 * @return       
 *             TRUE if queue is empty
 *             FALSE if not 
 *             ERROR if q is invalid 
 * 
 * @sideeffect None 
 */
uint32 mhi_q_is_queue_empty(list_t *q)
{
   if(NULL == q) 
      return ERROR; 

   if(q->next == q) 
      return TRUE; 
   else 
      return FALSE; 
}

/*
* @brief      mhi_util_update_rolling_avg
*
*             Compute rolling average
*             
* 
* @param[in]  avg - Pointer to structure containing rolling average
* @param[in]  el - Element used for computing the rolling average
*
* @dependencies
*             None
* 
* @return       
*             SUCCESS/ERROR
* 
* @sideeffect 
*             None 
*/
uint32 mhi_util_update_rolling_avg(rolling_avg_t *avg,uint32 el)
{
   if (NULL == avg)
      return ERROR;

   if (ROLLING_AVG_MAX_SAMPLE_SIZE == avg->count)
   {
      /* Re-init if count reaches the pre-defined max sample size */
      avg->total=0;
      avg->count=0;
   }
   
   avg->count++;
   avg->total+=el;
   avg->avg=avg->total/avg->count;

   return SUCCESS;
}

/*
* @brief      mhi_util_record_time_stats_ms      
*
*             Record time elapsed in ms. Records the latest elapsed time and 
*             also updates the rolling average  
* 
* @param[in]  time_stats - Pointer to structure that records time stats
*
* @dependencies
*             None
* 
* @return       
*             None
* 
* @sideeffect 
*             None 
*/
void mhi_util_record_time_stats_ms(mhi_time_stats_t *time_stats)
{
   if (NULL == time_stats)
     mhi_osal_debug(MSG_FATAL,"mhi_util_record_time_stats_ms: time_stats is NULL");  

   time_stats->time_elapsed_ms=mhi_osal_get_systime_ms() - time_stats->start_Time_ms;
   if (time_stats->min_time_elapsed_ms > time_stats->time_elapsed_ms)
      time_stats->min_time_elapsed_ms=time_stats->time_elapsed_ms;
   if (time_stats->max_time_elapsed_ms < time_stats->time_elapsed_ms)
      time_stats->max_time_elapsed_ms=time_stats->time_elapsed_ms;
   mhi_util_update_rolling_avg(&time_stats->avg_time_elapsed_ms,time_stats->time_elapsed_ms);
}

