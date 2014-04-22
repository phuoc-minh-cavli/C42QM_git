/**
  @file mhi_sio_ser.c
  @brief
  This file contains the implementation of the MHI SIO serial device adaptation  
    

*/
/*
===============================================================================

                             Edit History

 $Header: 

when       who     what, where, why
--------   ---     ------------------------------------------------------------
01/31/14   nk      Added WMQ peek API
01/15/13   nk      Created
===============================================================================
                   Copyright (c) 2013-2014 QUALCOMM Technologies Incorporated.
                          All Rights Reserved.
                        Qualcomm Confidential and Proprietary.
===============================================================================
*
*/

#ifndef MHI_SIO_UTIL_H
#define MHI_SIO_UTIL_H

#include "mhi_sio_i.h" 

/* Memory Pool Event Callback */
typedef void (*mem_event_cb_t)(dsm_mempool_id_type,
                               dsm_mem_level_enum_type,
                               dsm_mem_op_enum_type);

/* DSM Pool Info */
typedef struct _mhi_sio_dsm_pool
{
   uint32  poolid; 
   boolean initialized;  
   boolean available; 
   mem_event_cb_t event_cb; 
}mhi_sio_dsm_pool_t; 

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
void mhi_sio_dsm_init_pool(mhi_sio_dsm_pool_t *pool, uint32 pool_id, mem_event_cb_t cb);

/** 
 * @brief      mhi_sio_dsm_generate_dsm_chain
 *
 *             Generate a DSM chain 
 *              
 * 
 * @param[in]  pool_id   - pool id
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
dsm_item_type* mhi_sio_dsm_generate_dsm_chain(dsm_mempool_id_enum_type pool_id, uint32 chain_size);

/** 
 * @brief      mhi_sio_dsm_split_dsm_chain
 *
 *             Splits the DSM chain after the packet number (1 to n) specified by packet_to_split_at. For ex:
 *             if we have a chain with 3 buffers, and we want to split the chain after first buffer
 *             b1 (SPLIT) -> b2 -> b3 , we will have original chain with b1 and new chain with b2 -> b3
 *              
 * 
 * @param[in]  dsm_head - Pointer to start of DSM chain 
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
void mhi_sio_dsm_split_dsm_chain(dsm_item_type* dsm_head, uint32 packet_to_split_at, dsm_item_type** dsm_new_chain);

/** 
 * @brief      mhi_sio_dsm_utils_optimize_dsm_chain
 *
 *             This function optimizes a DSM chain by freeing packets that are using less bytes than the 
 *             use_small_item_threshold paramater.  
 *              
 * 
 * @param[in/out]  **dsm_chain - Pointer to DSM chain to optimize
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
void mhi_sio_dsm_optimize_dsm_chain(dsm_item_type **dsm_chain, uint32 use_small_item_threshold);


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
dsm_item_type* mhi_sio_peek_wmq(const dsm_watermark_type* wmq_ptr);

#endif
