/**
  @file mhi_sio_util.c
  @brief
  This file contains the implementation of the MHI SIO utilities such as :
  dsm
    

*/
/*
===============================================================================

                             Edit History

 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
04/21/14   nk      Klokwork fixes
01/31/14   nk      Added WMQ peek API
08/02/13   nk      Created
===============================================================================
                   Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*/

#include "dsm.h"
#include "mhi_sio_util.h" 
#include "mhi_osal.h" 

/* Define the pool low and high watermarks */
#define POOL_LOW 10 
#define POOL_HIGH 15


/** 
 * @brief      mhi_sio_dsm_init_pool
 *
 *             #desc 
 *              
 * 
 * @param[in]  *pool - pool object that stores pool configuration  
 * @param[in]  pool_id  - poolid of DSM pool to initialize
 * @param[in]  cb - cb that will receive pool notifications
 *
 * @dependencies
 *              
 * 
 * @return       
 *              
 * 
 * @sideeffect  None 
 */
void mhi_sio_dsm_init_pool(mhi_sio_dsm_pool_t *pool, uint32 pool_id, mem_event_cb_t cb)
{
   if(pool->initialized == FALSE)
   {
      pool->poolid   = pool_id; 
      pool->event_cb =cb; 

      /* Mark the pool as available if there is enough items */  
      pool->available=DSM_POOL_FREE_CNT(pool->poolid) <= POOL_LOW? FALSE : TRUE; 

      dsm_reg_mem_event_level(pool->poolid,
            DSM_MEM_LEVEL_MHI_FEW,
            POOL_LOW);
      dsm_reg_mem_event_cb(
            pool_id,
            DSM_MEM_LEVEL_MHI_FEW,
            DSM_MEM_OP_NEW,
            pool->event_cb);

      dsm_reg_mem_event_level(pool_id,
            DSM_MEM_LEVEL_MHI_MANY,
            POOL_HIGH);
      dsm_reg_mem_event_cb(
            pool_id,
            DSM_MEM_LEVEL_MHI_MANY,
            DSM_MEM_OP_FREE,
            pool->event_cb);

      pool->initialized=TRUE; 
   }
   else
   {
      mhi_osal_debug(MSG_HIGH, "mhi_sio_dsm_init_pool: Pool already initialized"); 
   }

   return; 
}

/** 
 * @brief      mhi_sio_dsm_deinit_pool
 *
 *             De-init a pool 
 *              
 * 
 * @param[in]  *pool - pointer to pool
 *
 * @dependencies
 *              
 * 
 * @return       
 *              
 * 
 * @sideeffect  None 
 */
void mhi_sio_dsm_deinit_pool(mhi_sio_dsm_pool_t *pool)
{
   dsm_unreg_mem_event_cb(
         pool->poolid,
         DSM_MEM_LEVEL_MHI_MANY,
         DSM_MEM_OP_FREE);

   dsm_unreg_mem_event_cb(
         pool->poolid,
         DSM_MEM_LEVEL_MHI_FEW,
         DSM_MEM_OP_NEW);

   pool->available=FALSE; 
   pool->initialized=FALSE; 
}

/** 
 * @brief      mhi_sio_dsm_generate_dsm_chain
 *
 *             Generate a DSM chain 
 *              
 * 
 * @param[in]  pool_id   - Pool id
 * @param[in]  chain_size - Size of chain to generate
 *
 * @dependencies
 *              
 * 
 * @return       
 *              Pointer to DSM item if succesfful, otherwise NULL 
 * 
 * @sideeffect  None 
 */
dsm_item_type* mhi_sio_dsm_generate_dsm_chain(dsm_mempool_id_enum_type pool_id, uint32 chain_size)
{
  uint32 dsm_item_itor;
  uint32 num_of_dsm_items;

  dsm_item_type* ret_dsm_chain_ptr = NULL;
  dsm_item_type* dsm_chain_curr_ptr = NULL;

  uint16 size_of_dsm_item;

  size_of_dsm_item = DSM_POOL_ITEM_SIZE(pool_id);

  /* Protect against div. by zero */
  if (size_of_dsm_item == 0)
  {
    mhi_osal_debug(MSG_FATAL, "mhi_sio_dsm_generate_dsm_chain: DSM item size for pool_id 0x%X is "
                  "zero. No chain will be generated.", pool_id, 0, 0);
    return NULL;
  }

  /* round up */
  num_of_dsm_items = 
    ((chain_size - 1) / size_of_dsm_item) + 1; 

  ret_dsm_chain_ptr = dsm_new_buffer(pool_id);

  if (ret_dsm_chain_ptr == NULL)
  {
    mhi_osal_debug(MSG_ERR, "mhi_sio_dsm_generate_dsm_chain: No more memory to allocate DSM "
                  "items. Pool ID: 0x%X", pool_id, 0, 0);
    return NULL;
  }

  for (dsm_item_itor = 1 ; dsm_item_itor < num_of_dsm_items ; ++dsm_item_itor)
  {
    dsm_chain_curr_ptr = dsm_new_buffer(pool_id);
    if (dsm_chain_curr_ptr == NULL)
    {
      mhi_osal_debug(MSG_ERR,"mhi_sio_dsm_generate_dsm_chain No more memory to allocate DSM "
                    "items.  Pool ID: 0x%X", pool_id, 0, 0);
      dsm_free_packet(&ret_dsm_chain_ptr);
      return NULL;        
    }
    dsm_append(&ret_dsm_chain_ptr, &dsm_chain_curr_ptr);
  }
  return ret_dsm_chain_ptr;
} 

/** 
 * @brief      mhi_sio_dsm_split_dsm_chain
 *
 *             Splits the dsm chain after the packet number (1 to n) specified by packet_to_split_at. For ex:
 *             if we have a chain with 3 buffers, and we want to split the chain after first buffer
 *             b1 (SPLIT) -> b2 -> b3 , we will have original chain with b1 and new chain with b2 -> b3
 *              
 * 
 * @param[in]  pool_id - Pool id to which new chain will belong 
 * @param[in]  dsm_head - Pointer to start of dsm chain 
 * @param[in]  packet_to_split_at - Packet to split after starting from 1 (1st packet)
 * @param[out]  dsm_new_chain - Points to new chain
 *
 * @dependencies
 *              
 * 
 * @return       
 *              
 * 
 * @sideeffect  None 
 */
void mhi_sio_dsm_split_dsm_chain(dsm_item_type* dsm_head, uint32 packet_to_split_at, dsm_item_type** dsm_new_chain)
{
  uint32 i; 
  dsm_item_type* dsm_item = NULL;

  if(packet_to_split_at==0)
  {
    /* Nothing to split, set the new chain to head */
    *dsm_new_chain=dsm_head; 
    return; 
  }
  
  dsm_item = dsm_head; 
  i=1; 

  /* Traverse chain until dsm_item points to packet we wish to split */
  while(i++ < packet_to_split_at && dsm_item!=NULL )
  {
     dsm_item=dsm_item->pkt_ptr;
  }

  if(dsm_item == NULL) 
  {
     mhi_osal_debug(MSG_FATAL, "mhi_sio_dsm_split_dsm_chain: Could not locate packet to split "); 
     *dsm_new_chain=NULL; 
     return; 
  }

  /* Set the next pkt ptr to NULL this will split the chain after dsm_item, and set the new dsm chain
   * to poin to next packet */
  *dsm_new_chain=dsm_item->pkt_ptr;
  dsm_item->pkt_ptr=NULL; 

  return;
} 

/** 
 * @brief      mhi_sio_dsm_utils_optimize_dsm_chain
 *
 *             This function optimizes a dsm chain by freeing packets that are using less bytes than the 
 *             use_small_item_threshold paramater.  
 *              
 * 
 * @param[in/out]  **dsm_chain - Pointer to dsm chain to optimize
 * @param[in]  use_small_item_threshold - Packet buffer utilization below this threshold will result in optimization of
 *             the buffers in a packet
 *
 * @dependencies
 *              
 * 
 * @return       
 *              
 * 
 * @sideeffect  None 
 */
void mhi_sio_dsm_optimize_dsm_chain(dsm_item_type **dsm_chain, uint32 use_small_item_threshold)
{
  dsm_item_type *curr_item = NULL;
  dsm_item_type *prev_item = NULL;
  dsm_item_type *small_item = NULL;

  if (use_small_item_threshold > DSM_DS_SMALL_ITEM_SIZ)
  {
    mhi_osal_debug(MSG_HIGH, "mhi_sio_dsm_utils_optimize_dsm_chain: Can't optimize to"
                   "small DSm items since use_small_item_threshold (%d) > DSM_DS_SMALL_ITEM_SIZ (%d)",
                    use_small_item_threshold,DSM_DS_SMALL_ITEM_SIZ);
  }

  curr_item = *dsm_chain; /*now curr_item point to the first dsm item in the chain*/
  while(curr_item)
  {
    
    /* Verify that the dsm_item is from DSM_DS_LARGE_ITEM_POOL */
    if (curr_item->pool_id != DSM_DS_LARGE_ITEM_POOL)
    {
      prev_item = curr_item;
      curr_item = curr_item->pkt_ptr;
      continue;
    }

    if (curr_item->used < use_small_item_threshold)
    {
      /*Attempt to replace a large DSM item with a small one*/
      small_item = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);
      if (small_item == NULL)
      {
        mhi_osal_debug(MSG_HIGH, "mhi_sio_dsm_utils_optimize_dsm_chain: No more memory to allocate SMALL DSM items.");
        return;
      }
      
      small_item->app_field = curr_item->app_field;
      small_item->app_ptr = curr_item->app_ptr;
      small_item->used = curr_item->used;
#ifdef DSM_KIND
      small_item->kind = curr_item->kind;
#endif /*DSM_KIND*/

      small_item->pkt_ptr = curr_item->pkt_ptr;

      mhi_osal_memcpy(small_item->data_ptr, curr_item->data_ptr, curr_item->used);
      if (prev_item)
      {
        prev_item->pkt_ptr = small_item; 
      }
      else
      {
        *dsm_chain = small_item;
      }
      curr_item->pkt_ptr = NULL;
      dsm_free_packet(&curr_item);
      curr_item = small_item;
    }
    prev_item = curr_item;
    curr_item = curr_item->pkt_ptr;
  }
} 

 /** 
 * @brief      mhi_sio_peek_wmq
 *
 *             Get next element off of watermark queue without removing it  
 *              
 * 
 * @param[in]  *wmq_ptr - Watermark queue
 *
 * @dependencies
 *             None
 *              
 * 
 * @return       
 *             dsm_item_type* - Pointer to next DSM item on queue
 * 
 * @sideeffect None 
 */        
dsm_item_type* mhi_sio_peek_wmq(const dsm_watermark_type* wmq_ptr)
{
  return(q_check(wmq_ptr->q_ptr));
}



