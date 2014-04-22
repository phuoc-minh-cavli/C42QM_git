#ifndef _HSU_AL_DSM_UTILS_H_
#define _HSU_AL_DSM_UTILS_H_

/*==============================================================================

  High Speed USB DSM Utilities - H E A D E R   F I L E
  
  GENERAL DESCRIPTION
    Implementation of various function that assist with DSM related operations. 
    For example: DSM allocations, scattering and gathering using memcpy, etc...
  
  EXTERNALIZED FUNCTIONS
    hsu_al_dsm_utils_get_dsm_chain_capacity
    hsu_al_dsm_utils_get_dsm_chain_used_bytes
    hsu_al_dsm_utils_generate_dsm_chain
    hsu_al_dsm_utils_scatter_buffer_without_dmov
    hsu_al_dsm_utils_gather_buffer_without_dmov
    hsu_al_dsm_utils_peek_wmq
    hsu_al_dsm_utils_alloc_contiguous_buf
    hsu_al_dsm_utils_free_contiguous_buf
    hsu_al_dsm_utils_optimize_dsm_chain_item_alloc

  INITALIZATION AND SEQUENCING REQUIREMENTS
    None.
    
  Copyright (c) 2006 - 2009 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/adapt_layers/hsu_al_dsm_utils.h#1 $
  $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  06/27/06  ke      Initial version
    
==============================================================================*/
#include "dsm.h"
#include "comdef.h"

/*==============================================================================
  Forward Declarations
==============================================================================*/

/*==============================================================================
  Constants
==============================================================================*/

/*==============================================================================
  Macros
==============================================================================*/
/* HSU_DSM_UTILS_CONTIGUOUS_BUF_SIZE must be enough for complete ethernet packet.
   Also, we add 8 bytes due to the following:
   1. In case of ACM, if the chain will be very long it will be splitted into 
   several buffers. If each buffer will be 1536 bytes we will send a redundant 
   zero length packet on each buffer.
   2. We would like to give HW buffers aligned to 4 bytes
   3. Used to be for having enough space in the buffer for the 4 bytes added in case of 
   FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND (need at least 6 for the 1 or 2 residue 
   + 4 bytes padding). This is no longer applicable starting from branch 2.2. 
*/

#if 0
#ifndef HSU_DSM_UTILS_CONTIGUOUS_BUF_SIZE
#ifndef FEATURE_HS_USB_FS_ONLY
	#ifdef FEATURE_HS_USB_MBIM
  /* MBIM requires minimum of ~6K.  To give head room for future update
  ** allocate 8K for contiguous buffer 
  */
  #define HSU_DSM_UTILS_CONTIGUOUS_BUF_SIZE (8*1024)
  #else /* HSU_DSM_UTILS_CONTIGUOUS_BUF_SIZE */
  #define HSU_DSM_UTILS_CONTIGUOUS_BUF_SIZE (2048+8)
  #endif
#else
  /* In case of FS-USB the contiguous buffer size was enlarge to enable gathering of
     large small transfers, that are typical to FS-USB, to one large transfer and
     increasing the down link throughput */
  #define HSU_DSM_UTILS_CONTIGUOUS_BUF_SIZE (16*1024)
#endif /* FEATURE_HS_USB_FS_ONLY */
#elif (HSU_DSM_UTILS_CONTIGUOUS_BUF_SIZE < (2048+8))
  #error "HSU_DSM_UTILS_CONTIGUOUS_BUF_SIZE must be at least (2048+8)"
#endif /* ! HSU_DSM_UTILS_CONTIGUOUS_BUF_SIZE */
#endif
#define HSU_DSM_UTILS_CONTIGUOUS_BUF_SIZE (2048)

/* For HW queuing we use LARGE_DSM_ITEMS. If the amount data received from the host 
   is smaller than HSU_USE_SMALL_DSM_ITEMS_TREASHOLD, it will be copied to small 
   DSM item instead. This was done in order to optimize the use of DSM items in the 
   system. 
   NOTE: this optimization is performed only on the RX data */
#ifdef HS_USB_USE_SMALL_DSM_ITEMS_TREASHOLD
#define HSU_USE_SMALL_DSM_ITEMS_TREASHOLD HS_USB_USE_SMALL_DSM_ITEMS_TREASHOLD
#else
#define HSU_USE_SMALL_DSM_ITEMS_TREASHOLD 10
#endif /* HSU_USE_SMALL_DSM_ITEMS_TREASHOLD */

#define UNCACHED_DSM_ITEM_SIZE  hsu_conf_sel_get_ecm_rx_dsm_item_size()
#define UNCACHED_DSM_ITEM_POOL  hsu_al_get_uncached_dsm_pool()

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*==============================================================================

FUNCTION      HSU_AL_GET_UNCACHED_DSM_POOL

DESCRIPTION   
  Returns DSM pool base address / id for uncached dsm pool.

DEPENDENCIES  
  None.

RETURN VALUE  
  Returns DSM pool base address / id for uncached dsm pool.

SIDE EFFECTS    
  None.
==============================================================================*/
  
dsm_mempool_id_type hsu_al_get_uncached_dsm_pool(void);
  
  
/*==============================================================================

FUNCTION      HSU_AL_UNCACHED_DSM_INIT

DESCRIPTION   
  Initializes the uncached buffer pool when called for the first time.

DEPENDENCIES  
  None.

RETURN VALUE  
  None.

SIDE EFFECTS    
  None.
==============================================================================*/

void hsu_al_uncached_dsm_init(void);

/*==============================================================================

FUNCTION      HSU_AL_DSM_UTILS_GET_DSM_CHAIN_CAPACITY

DESCRIPTION   
  Returns the capacity of the DSM chain (number of bytes allocated, not those 
  actually used)

DEPENDENCIES  
  None.

RETURN VALUE  
  The number of bytes allocated in the given chain.

SIDE EFFECTS    
  None.

==============================================================================*/
uint32 hsu_al_dsm_utils_get_dsm_chain_capacity
(
  const dsm_item_type* dsm_chain_ptr
);

/*==============================================================================

FUNCTION      HSU_AL_DSM_UTILS_GET_DSM_CHAIN_USED_BYTES

DESCRIPTION   
  Returns the number of used bytes in a DSM chain 

DEPENDENCIES  
  None.

RETURN VALUE  
  The number of used bytes in the given chain.

SIDE EFFECTS    
  None.

==============================================================================*/
uint32 hsu_al_dsm_utils_get_dsm_chain_used_bytes
(
 const dsm_item_type* dsm_chain_ptr
 );
/*===========================================================================

FUNCTION      HSU_AL_DSM_UTILS_GENERATE_DSM_CHAIN

DESCRIPTION
  Based on the given size and a DSM mem pool ID, generates a DSM item chain.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to a dsm_item_type representing the chain if successful.
  NULL - Otherwise.

SIDE EFFECTS
  None
===========================================================================*/
dsm_item_type* hsu_al_dsm_utils_generate_dsm_chain
(
  dsm_mempool_id_enum_type  pool_id,
  uint32                    desired_dsm_chain_size
);

/*==============================================================================

FUNCTION      HSU_AL_DSM_UTILS_SCATTER_BUFFER_WITHOUT_DMOV

DESCRIPTION   
  Scatters a contiguous buffer into the given (pre-allocated) DSM item chain.

DEPENDENCIES  
  dest_chain should be other than NULL, each DSM item in the chain should 
  have memory allocated for it (data_ptr should be other than NULL), and should 
  be empty (current_item->used should be 0).

RETURN VALUE  
  TRUE for success, FALSE for failure

SIDE EFFECTS    
  None.

==============================================================================*/
boolean hsu_al_dsm_utils_scatter_buffer_without_dmov
(
  const uint8 *   src_buffer, 
  uint32          src_buffer_size,
  dsm_item_type * dest_chain
);

/*==============================================================================

FUNCTION      HSU_AL_DSM_UTILS_GATHER_BUFFER_WITHOUT_DMOV

DESCRIPTION   
  Aggregates a DSM item chain into a contiguous buffer, using memcpy().

DEPENDENCIES  
  chain_to_gather should be other than NULL, each DSM item in the chain should 
  have memory allocated for it (data_ptr should be other than NULL), and should 
  not be empty (current_item->used should not be 0).

RETURN VALUE  
  number of bytes copied.

SIDE EFFECTS    
  None.

==============================================================================*/
uint32 hsu_al_dsm_utils_gather_buffer_without_dmov
(
  dsm_item_type * chain_to_gather,
  uint32          chain_to_gather_length,
  void *          dest_buffer,
  uint32          dest_buffer_size
);

/*==============================================================================

FUNCTION      HSU_AL_DSM_UTILS_PEEK_WMQ

DESCRIPTION   
  Gets the first DSM chain in the given Watermark Queue, without dequeuing it.

DEPENDENCIES  
  wmq_ptr must not be NULL

RETURN VALUE  
  The first DSM chain in the given wmq. NULL if queue is empty

SIDE EFFECTS    
  None.

==============================================================================*/
dsm_item_type* hsu_al_dsm_utils_peek_wmq
(
  const dsm_watermark_type* wmq_ptr
);

/*==============================================================================

FUNCTION      HSU_AL_DSM_UTILS_SPLIT_DSM_CHAIN

DESCRIPTION   
  Splits a given DSM chain in the following fashion:
  The original given chain will have no more than amount_to_leave_in_orig bytes,
  being the closest multiple of the DSM item size specified by pool ID.
  The returned DSM chain's length will be the remainder of the above value from
  the given chain's original length.

  So, if:
  DSMI_DS_LARGE_ITEM_SIZ is 668
  orig_chain has a length of DSMI_DS_LARGE_ITEM_SIZ*10 + 5
  Then the following call:

  hsu_al_dsm_utils_split_dsm_chain(DSM_DS_LARGE_ITEM_POOL, orig_chain, 1400)

  Would cause orig_chain to be trimmed to contain only it's first two DSM items,
  and the rest of the chain will be returned, having a length of:
  DSMI_DS_LARGE_ITEM_SIZ*8 + 5.

DEPENDENCIES  
  pool_id must be a valid DSM pool
  orig_dsm_chain_ptr mustn't be NULL

RETURN VALUE  
  TRUE - Operation successful. FALSE - Otherwise.

SIDE EFFECTS    
  orig_dsm_chain_ptr is trimmed (unless its length is less or equal than
  amount_to_leave_in_orig_chain_in_bytes, in which case dest_dsm_chain_ptr is
  NULL).

==============================================================================*/
boolean hsu_al_dsm_utils_split_dsm_chain
(
  dsm_mempool_id_enum_type  pool_id,
  dsm_item_type*            orig_dsm_chain_ptr,
  uint32                    orig_chain_length,
  dsm_item_type**           dest_dsm_chain_ptr_ptr,
  uint32                    amount_to_leave_in_orig_chain_in_bytes
);

/*==============================================================================

FUNCTION      HSU_AL_DSM_UTILS_INIT_CONTIGUOUS_BUF_POOL

DESCRIPTION   
  Initializes the contiguous buffer pool when called for the
  first time.

DEPENDENCIES  
  None.

RETURN VALUE  
  None.

SIDE EFFECTS    
  None.
==============================================================================*/
void hsu_al_dsm_utils_init_contiguous_buf_pool(void);

/*==============================================================================

FUNCTION      HSU_AL_DSM_UTILS_ALLOC_CONTIGUOUS_BUF

DESCRIPTION   
  Allocate a buffer from the contiguous buffer pool

DEPENDENCIES  
  None.

RETURN VALUE  
  Pointer to allocated buffer. NULL if no buffer available.

SIDE EFFECTS    
  None.
==============================================================================*/
uint8* hsu_al_dsm_utils_alloc_contiguous_buf(void);

/*==============================================================================

FUNCTION      HSU_AL_DSM_UTILS_FREE_CONTIGUOUS_BUF

DESCRIPTION   
  Deallocate a buffer from the contiguous buffer pool

DEPENDENCIES  
  None.

RETURN VALUE  
  None.

SIDE EFFECTS    
  None.
==============================================================================*/
void hsu_al_dsm_utils_free_contiguous_buf
(
  uint8* buf_ptr
);

/*==============================================================================

FUNCTION      HSU_AL_DSM_UTILS_GET_CONTIGUOUS_BUF_FREE_COUNT

DESCRIPTION   
This function returns the number of free buffers.

DEPENDENCIES  
None.

RETURN VALUE  
Number of free buffers. 0 if there is no buffer available.

SIDE EFFECTS    
None.
==============================================================================*/
uint32 hsu_al_dsm_utils_get_contiguous_buf_free_count(void);

/*===========================================================================

FUNCTION      HSU_AL_DSM_UTILS_GENERATE_DSM_CHAIN_STACK_MEM

DESCRIPTION
  Based on the given size and a DSM mem pool ID, generates a DSM item chain,
  which is based on memory given to USB stack, rather than DSM pool memory.

  The implementation of this function is compiled only under FEATURE_HSU_EBI 
  flag.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to a dsm_item_type representing the chain if successful.
  NULL - Otherwise.

SIDE EFFECTS
  None
===========================================================================*/
dsm_item_type* hsu_al_dsm_utils_generate_dsm_chain_stack_mem
(
 dsm_mempool_id_enum_type  pool_id,
 uint32                    desired_dsm_chain_size
);

/*==============================================================================

FUNCTION      HSU_AL_DSM_UTILS_DUP_DSM_CHAIN

DESCRIPTION   
  Create a new DSM chain from a given pool, which is duplicated from a given DSM
  chain.

  The implementation of this function is compiled only under FEATURE_HSU_EBI 
  flag.

DEPENDENCIES  
  None.

RETURN VALUE  
  New DSM chain.

SIDE EFFECTS    
  None.
==============================================================================*/
dsm_item_type* hsu_al_dsm_utils_dup_dsm_chain(dsm_item_type* dsm_chain_ptr,
                                              uint32         desired_dsm_chain_size,
                                              dsm_mempool_id_enum_type  pool_id);

/*==============================================================================

FUNCTION      HSU_AL_DSM_UTILS_RELEASE_DSM_CHAIN_STACK_MEM

DESCRIPTION   
  Release a DSM chain which is based on stack memory.
  First free all stack buffers, and then free the DSM chain itself.

  The implementation of this function is compiled only under FEATURE_HSU_EBI 
  flag.

DEPENDENCIES  
  None.

RETURN VALUE  
  None.

SIDE EFFECTS    
  None.
==============================================================================*/
void hsu_al_dsm_utils_release_dsm_chain_stack_mem(dsm_item_type* dsm_chain_ptr);

/*==============================================================================

FUNCTION      HSU_AL_DSM_UTILS_OPTIMIZE_DSM_CHAIN_ITEM_ALLOC

DESCRIPTION   
This function is used for large DSM item allocation optimization: 
if the used field of a large dsm item is lower then a given threshold - copy the 
large DSM item into a small one and release the large DSM item
This function goes over the received DSm chain and performs the above on all of 
the chain items.

NOTE: ZLP packets will be dropped!!!

DEPENDENCIES  
None.

RETURN VALUE  
None.

SIDE EFFECTS    
The received DSM chain is modified (see description)
==============================================================================*/
void hsu_al_dsm_utils_optimize_dsm_chain_item_alloc
(
 dsm_item_type **dsm_chain,  /*pointer to the DSM chain to optimize*/
 uint32          use_small_item_threshold
 );

/*===========================================================================

FUNCTION      hsu_al_dsm_utils_init

DESCRIPTION
Initializes internal DSM utilities data.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void hsu_al_dsm_utils_init
(
 dsm_mempool_id_enum_type  pool_id
);

/*===========================================================================

FUNCTION      hsu_al_dsm_utils_uninit

DESCRIPTION
Uninitializes internal DSM utilities data.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
void hsu_al_dsm_utils_uninit
(
 dsm_mempool_id_enum_type  pool_id
);

/*===========================================================================

FUNCTION      hsu_al_dsm_utils_pool_is_not_available

DESCRIPTION
Reports whether a pool is not available

DEPENDENCIES
None

RETURN VALUE
TRUE  - a pool is not available
FALSE - a pool is available

SIDE EFFECTS
None
===========================================================================*/
boolean hsu_al_dsm_utils_pool_is_not_available
(
 dsm_mempool_id_enum_type  pool_id
);

/*===========================================================================

FUNCTION      hsu_al_dsm_utils_f3

DESCRIPTION
Prints DSM utility statistics to F3 DIAG window

DEPENDENCIES
None

RETURN VALUE
TRUE  - a pool is not available
FALSE - a pool is available

SIDE EFFECTS
None
===========================================================================*/
void hsu_al_dsm_utils_f3(void);

#endif /* _HSU_AL_DSM_UTILS_H_ */
