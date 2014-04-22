#ifndef MHI_UTIL_H
#define MHI_UTIL_H
/**
  @file mhi_util.h
  @brief
  This file defines the interface for the MHI utility API's 
    

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
#include "comdef.h"  

/** Defines generic linked list */ 
typedef struct _list_t
{
  struct _list_t *next;
  struct _list_t *prev;
}list_t;

/** Defines generic type for calculating rolling average */
typedef struct _rolling_avg
{
  uint32 count;       /**< Number of samples */
  uint64 total;       /**< Total number of measurements */
  uint64 avg;         /**< Rolling average */
}rolling_avg_t;

/* Generic structure used for time profiling */
typedef struct _mhi_time_stats
{
  uint64 start_Time_ms;              /**< Starting time in ms */
  uint64 time_elapsed_ms;            /**< Time elapsed in ms since start tick */
  uint64 min_time_elapsed_ms;        /**< Min time elapse recorded */
  uint64 max_time_elapsed_ms;        /**< Max time elapse recorded */
  rolling_avg_t avg_time_elapsed_ms; /**< Rolling average of time elaspsed in ms */
} mhi_time_stats_t;

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
void mhi_q_init(list_t *q);

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
uint32 mhi_q_queue(list_t *q, list_t *i);

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
int32 mhi_q_queue_head(list_t *q, list_t *i);

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
int32 mhi_q_insert_before(list_t *at, list_t *i);

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
int32 mhi_q_peek_head(list_t *q, list_t **i);

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
int32 mhi_q_dequeue(list_t *q, list_t **i);

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
int32 mhi_q_get_next(list_t *q, list_t** current);

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
int32 mhi_q_remove(list_t *i); 

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
uint32 mhi_q_is_queue_empty(list_t *q);

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
uint32 mhi_util_update_rolling_avg(rolling_avg_t *avg,uint32 el);

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
void mhi_util_record_time_stats_ms(mhi_time_stats_t *time_stats);


#endif
