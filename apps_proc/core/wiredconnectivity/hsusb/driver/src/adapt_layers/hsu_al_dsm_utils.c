/*==============================================================================

  High Speed USB DSM Utilities
  
  GENERAL DESCRIPTION
    Implementation of various functions that assist with DSM related operations. 
    For example: DSM allocations, scattering and gathering using memcpy, etc...
      
  EXTERNALIZED FUNCTIONS
    hsu_al_dsm_utils_get_dsm_chain_capacity
    hsu_al_dsm_utils_get_dsm_chain_used_bytes
    hsu_al_dsm_utils_generate_dsm_chain
    hsu_al_dsm_utils_scatter_buffer_without_dmov
    hsu_al_dsm_utils_gather_buffer_without_dmov
    hsu_al_dsm_utils_peek_wmq
    hsu_al_dsm_utils_split_dsm_chain
    hsu_al_dsm_utils_alloc_contiguous_buf
    hsu_al_dsm_utils_free_contiguous_buf
    hsu_al_dsm_utils_optimize_dsm_chain_item_alloc

  INITALIZATION AND SEQUENCING REQUIREMENTS
    None.

  Copyright (c) 2006 - 2010 by Qualcomm Technologies, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //components/rel/core.tx/6.0/wiredconnectivity/hsusb/driver/src/adapt_layers/hsu_al_dsm_utils.c#1 $
  $DateTime: 2020/01/30 22:49:35 $ $Author: pwbldsvc $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  06/27/06  ke      Initial version
    
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE
  
==============================================================================*/
#include "hsu_al_dsm_utils.h"
#include "hsu_common_int.h"
#include "dsm_item.h"
#include "dsm_pool.h"
#include "diagdsm.h"

#include "hsu_al_ser.h" /* for hsu_al_ser_pool_many_cb */
#include "hsu_al_ser_acm_int.h"
#ifdef FEATURE_HS_USB_ECM
#include "hsu_al_ecm_int.h"
#endif

#include "hsu_al_ser_i.h"   /* To obtain SER TX buffer count */
#include "hsu_al_task.h"

#include "hsu_conf_sel_stack_utils_int.h"

#include "dsm_pool.h"
#include "dsm_item.h"
#include <stringl/stringl.h>

#include "hsu_qurt_util.h"
/*==============================================================================

                          DEFINITIONS AND DECLARATIONS FOR MODULE
  
==============================================================================*/

#if 0
#ifdef FEATURE_HS_USB_ECM
#define HSU_CONTIGUOUS_BUF_ARR_SIZE HS_USB_ACM_TX_MAX_PENDING_BUFFERS + \
                                    HS_USB_ECM_TX_MAX_PENDING_BUFFERS
#else
#define HSU_CONTIGUOUS_BUF_ARR_SIZE HS_USB_ACM_TX_MAX_PENDING_BUFFERS
#endif
#endif
#define HSU_CONTIGUOUS_BUF_ARR_SIZE (1)

/* Quantity of the supported DSM pools */
#define HSU_AL_DSM_UTILS_POOLS_MAX sizeof(hsu_al_dsm_utils_pool_info)/sizeof(hsu_al_dsm_utils_pool_info_type)

/* The DSM LEVEL FEW callback will be triggered when NEW allocation
 * of DSM item occurs.  For ACM and ECM, this value has to be greater 
 * than ACM and ECM's max RX HWQ buffer.
 */
#ifndef HS_USB_LARGE_DSM_LEVEL_FEW
  #define HS_USB_LARGE_DSM_LEVEL_FEW   (30)
#endif /* HS_USB_LARGE_DSM_LEVEL_FEW */

#ifndef HS_USB_DIAG_RX_DSM_LEVEL_FEW
  #define HS_USB_DIAG_RX_DSM_LEVEL_FEW   (1)
#endif /* HS_USB_DIAG_RX_DSM_LEVEL_FEW */

/* The DSM LEVEL MANY callback will be triggered when FREE operation
 * of DSM item occurs.  For ACM and ECM, this value has to be greater
 * than ACM and ECM's max RX HWQ buffer.
 */
#ifndef HS_USB_LARGE_DSM_LEVEL_MANY
  #define HS_USB_LARGE_DSM_LEVEL_MANY  (40)
#endif /* HS_USB_LARGE_DSM_LEVEL_MANY */

#ifndef HS_USB_DIAG_RX_DSM_LEVEL_MANY
  #define HS_USB_DIAG_RX_DSM_LEVEL_MANY  (3)
#endif /* HS_USB_DIAG_RX_DSM_LEVEL_MANY */


// ECM DSM LEVELs
#define HS_USB_UNCACHED_DSM_LEVEL_FEW   (9)
#define HS_USB_UNCACHED_DSM_LEVEL_MANY  (13)

// Uncachec DSM pool for ECM
#define UNCACHED_DSM_ITEM_CNT    hsu_conf_sel_get_ecm_rx_dsm_item_cnt()
#define UNCACHED_DSM_MEM_SIZE   ((UNCACHED_DSM_ITEM_CNT *\
  (UNCACHED_DSM_ITEM_SIZE + DSM_ITEM_HEADER_SIZE)) +  31)


/*==============================================================================
  Typedefs
==============================================================================*/
/* contiguous buffer */
typedef struct hsu_dsm_utils_contiguous_buf_type
{
  uint8 buf[HSU_DSM_UTILS_CONTIGUOUS_BUF_SIZE];
} hsu_dsm_utils_contiguous_buf_type;

/* contiguous buffer pool */
typedef struct hsu_dsm_utils_contiguous_buf_pool_type
{
  rex_crit_sect_type pool_cs;
  hsu_dsm_utils_contiguous_buf_type buf_arr[HSU_CONTIGUOUS_BUF_ARR_SIZE];
  boolean is_free[HSU_CONTIGUOUS_BUF_ARR_SIZE];
  uint32 free_count; /* for debugging */
  uint32 min_free_count; /* for debugging */
} hsu_dsm_utils_contiguous_buf_pool_type;

hsu_dsm_utils_contiguous_buf_pool_type hsu_dsm_utils_contiguous_buf_pool;


static boolean is_initialized = FALSE;

/* The DSM pool information */
typedef struct
{
  dsm_mempool_id_enum_type  pool_id;
  /* pool id */
  uint32 level_count_few;
  /* DSM_MEM_LEVEL_USB_FEW level count */
  uint32 level_count_many;
  /* DSM_MEM_LEVEL_USB_MANY level count */
  int32 use_counter;

  /* keep track of flow control*/
  uint32 few_counter;
  uint32 many_counter;

  /* Determines how many times a pool is used */
  boolean is_not_available;
  /* Determines whether a pool is not available */
} hsu_al_dsm_utils_pool_info_type;

/* The supported DSM pools info */
static hsu_al_dsm_utils_pool_info_type hsu_al_dsm_utils_pool_info[] = 
  {{0}, // LARGE DSM ITEM 
   {0}, // DIAG RX DSM ITEM 
   {0}  // UNCACHED DSM ITEM
  };

// Uncached pool of DSM items
static uint32*  uncached_dsm_item_array;
static dsm_pool_mgmt_table_type uncached_dsm_item_pool;
static jbool_t run_once = FALSE;

/*==============================================================================

                          FUNCTION DECLARATIONS FOR MODULE
  
==============================================================================*/

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
)
{
  uint32 returned_value = 0;
  while (dsm_chain_ptr != NULL)
  {
    /* VC Compiler warns about this:
    returned_value += (dsm_chain_ptr->size); */
    returned_value = (dsm_chain_ptr->size) + returned_value;
    dsm_chain_ptr = dsm_chain_ptr->pkt_ptr;
  }
  return returned_value;
} /* hsu_al_dsm_utils_get_dsm_chain_capacity */

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
 )
{
  uint32 returned_value = 0;
  while (dsm_chain_ptr != NULL)
  {
    returned_value = (dsm_chain_ptr->used) + returned_value;
    dsm_chain_ptr = dsm_chain_ptr->pkt_ptr;
  }
  return returned_value;
} /* hsu_al_dsm_utils_get_dsm_chain_used_bytes */

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
)
{
  uint32 dsm_item_itor;
  uint32 num_of_dsm_items;

  dsm_item_type* returned_dsm_chain_ptr = NULL;
  dsm_item_type* dsm_chain_curr_ptr = NULL;

  uint16 size_of_dsm_item;

  size_of_dsm_item = DSM_POOL_ITEM_SIZE(pool_id);
  /* Protect against div. by zero */
  if (size_of_dsm_item == 0)
  {
    HSU_ERR_FATAL("hsu_*_generate_dsm_chain: DSM item size for pool_id 0x%X is "
                  "zero. No chain will be generated.", pool_id, 0, 0);
    return NULL;
  }

  if (desired_dsm_chain_size == 0)
  {
    HSU_MSG_ERROR("hsu_*_generate_dsm_chain: desired_dsm_chain_size is zero. "
                  "No chain will be generated.");
    return NULL;
  }

  num_of_dsm_items = 
    ((desired_dsm_chain_size - 1) / size_of_dsm_item) + 1; 
  /* always round up */

  returned_dsm_chain_ptr = dsm_new_buffer(pool_id);

  if (returned_dsm_chain_ptr == NULL)
  {
    HSU_MSG_ERROR_1("hsu_*_generate_dsm_chain: No more memory to allocate DSM "
                  "items. Pool ID: 0x%X", pool_id);
    return NULL;
  }

  for (dsm_item_itor = 1 ; dsm_item_itor < num_of_dsm_items ; ++dsm_item_itor)
  {
    dsm_chain_curr_ptr = dsm_new_buffer(pool_id);
    if (dsm_chain_curr_ptr == NULL)
    {
      HSU_MSG_ERROR_1("hsu_*_generate_dsm_chain: No more memory to allocate DSM "
                    "items.  Pool ID: 0x%X", pool_id);
      dsm_free_packet(&returned_dsm_chain_ptr);
      return NULL;        
    }
    dsm_append(&returned_dsm_chain_ptr, &dsm_chain_curr_ptr);
  }
  return returned_dsm_chain_ptr;
} /* hsu_al_dsm_utils_generate_dsm_chain */

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
)
{
  uint32 dsm_chain_capacity;
  uint32 bytes_copied = 0;
  dsm_item_type* current_item = NULL;

  if (src_buffer == NULL)
  {
    HSU_MSG_ERROR(
      "hsu_al_dsm_utils_scatter_buffer_without_dmov(): src_buffer is NULL");
    return FALSE;
  }
  if (src_buffer_size == 0)
  {
    HSU_MSG_ERROR(
      "hsu_al_dsm_utils_scatter_buffer_without_dmov(): src_buffer_size is 0");
    return FALSE;
  }
  if (dest_chain == NULL)
  {
    HSU_MSG_ERROR(
      "hsu_al_dsm_utils_scatter_buffer_without_dmov(): dest_chain is NULL");
    return FALSE;
  }

  dsm_chain_capacity = hsu_al_dsm_utils_get_dsm_chain_capacity(dest_chain);
  /* dsm_length_packet returns how much actual data is in a chain, NOT its capacity! */

  if (dsm_chain_capacity == 0)
  {
    HSU_MSG_ERROR("hsu_al_dsm_utils_scatter_buffer_without_dmov: "
                  "dest_chain has zero length");
    return FALSE;
  }

  if (src_buffer_size > dsm_chain_capacity)
  {
    HSU_MSG_ERROR_2("hsu_al_dsm_utils_scatter_buffer_without_dmov: "
                  "src_buffer_size (%u) is bigger than dest_chain (%u)", 
                  src_buffer_size, dsm_chain_capacity);
    return FALSE;
  }

  current_item = dest_chain;

  do 
  {
    uint16 how_much_to_copy = 0;

    HSU_ASSERT(current_item != NULL);
    HSU_ASSERT(current_item->data_ptr != NULL);
    HSU_ASSERT(current_item->used == 0); /* DSM item ought to be empty ! */

    if (current_item->size < (src_buffer_size - bytes_copied))
    {
      how_much_to_copy = current_item->size;
    }
    else
    {
      /* current_item->size (which is uint16) is bigger than 
      (src_buffer_size - bytes_copied), so it is safe to cast to uint16 */
      how_much_to_copy = (uint16)(src_buffer_size - bytes_copied);
    }

    memscpy(current_item->data_ptr, how_much_to_copy, (void*)(((uint32)src_buffer)+bytes_copied), 
      how_much_to_copy);

    /* Update the used field in the DSM item */
    current_item->used = how_much_to_copy;

    bytes_copied += how_much_to_copy;

    /* Go to next packet */
    current_item = current_item->pkt_ptr;
  } while (bytes_copied < src_buffer_size);

  return TRUE;
} /* hsu_al_dsm_utils_scatter_buffer_without_dmov */

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
)
{
  uint32 bytes_copied = 0;
  dsm_item_type* current_item = NULL;

  HSU_ASSERT(chain_to_gather != NULL);
  HSU_ASSERT(dest_buffer != NULL);

  current_item = chain_to_gather;

  if (chain_to_gather_length == 0)
  {
    HSU_MSG_ERROR("hsu_al_dsm_utils_gather_buffer_without_dmov(): "
                  "chain_to_gather has zero length");
    return 0;
  }

  if (dest_buffer_size < chain_to_gather_length)
  {
    HSU_MSG_ERROR_2("hsu_al_dsm_utils_gather_buffer_without_dmov(): "
                  "chain_to_gather (%u) is bigger than dest_buffer_size (%u)", 
                  chain_to_gather_length, dest_buffer_size);
    return 0;
  }

  do 
  {
    HSU_ASSERT(current_item != NULL);
    HSU_ASSERT(current_item->data_ptr != NULL);

    if (current_item->used == 0)
    {
      HSU_MSG_HIGH("hsu_*_gather_buffer_without_dmov(): current_item->used = 0");
    }
    else /*current_item->used != 0*/
    {
       memscpy((void*)(((uint32)dest_buffer)+bytes_copied), current_item->used,
        current_item->data_ptr, current_item->used);
       bytes_copied += current_item->used;
    }

    /* Go to next packet */
    current_item = current_item->pkt_ptr;
  } while (bytes_copied < chain_to_gather_length);

  return bytes_copied;
} /* hsu_al_dsm_utils_gather_buffer_without_dmov */

/*==============================================================================

FUNCTION      HSU_AL_DSM_UTILS_PEEK_WMQ

DESCRIPTION   
  Gets a pointer to the first DSM chain in the given Watermark Queue, without 
  dequeuing it.

DEPENDENCIES  
  wmq_ptr must not be NULL

RETURN VALUE  
  Pointer to the first DSM chain in the given wmq. NULL if queue is empty

SIDE EFFECTS    
  None.

==============================================================================*/
dsm_item_type* hsu_al_dsm_utils_peek_wmq
(
  const dsm_watermark_type* wmq_ptr
)
{
  dsm_item_type* chain_to_return = NULL;

  HSU_ASSERT(wmq_ptr != NULL);
  HSU_ASSERT(wmq_ptr->q_ptr != NULL);

  chain_to_return = q_check(wmq_ptr->q_ptr);

  return chain_to_return;
} /* hsu_al_dsm_utils_peek_wmq */

/*=============================================================================

FUNCTION      HSU_AL_DSM_UTILS_SPLIT_DSM_CHAIN

DESCRIPTION   
  Splits a given DSM chain in the following fashion:
  The original given chain will have no more than amount_to_leave_in_orig bytes
  , being the closest multiple of the DSM item size specified by pool ID.
  The returned DSM chain's length will be the remainder of the above value from
  the given chain's original length.

  So, if:
  DSMI_DS_LARGE_ITEM_SIZ is 668
  orig_chain has a length of DSMI_DS_LARGE_ITEM_SIZ*10 + 5
  Then the following call:

  hsu_al_dsm_utils_split_dsm_chain(DSM_DS_LARGE_ITEM_POOL, orig_chain, 1400)

  Would cause orig_chain to be trimmed to contain only it's first two DSM items
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

=============================================================================*/
boolean hsu_al_dsm_utils_split_dsm_chain
(
  dsm_mempool_id_enum_type  pool_id,
  dsm_item_type*            orig_dsm_chain_ptr,
  uint32                    orig_chain_length,
  dsm_item_type**           dest_dsm_chain_ptr_ptr,
  uint32                    amount_to_leave_in_orig_chain_in_bytes
)
{
  uint32 rounded_off_amount_to_leave_in_orig;
  uint32 dsm_split_result;
  dsm_item_type* current_item_ptr = NULL;

  HSU_ASSERT(orig_dsm_chain_ptr != NULL);
  HSU_ASSERT(dest_dsm_chain_ptr_ptr != NULL);
  HSU_ASSERT(amount_to_leave_in_orig_chain_in_bytes != 0);

  HSU_ASSERT(orig_chain_length != 0);

  /* Ensure that at least the contents of the first DSM item will remain
  in the orig DSM chain */
  HSU_ASSERT(orig_dsm_chain_ptr->used <= amount_to_leave_in_orig_chain_in_bytes);

  if (orig_chain_length <= amount_to_leave_in_orig_chain_in_bytes)
  {
    /* original chain already fulfills the requirement that it be no
    longer than amount_to_leave_in_orig_chain_in_bytes*/
    return TRUE;
  }
  /* else: orig_chain_length > amount_to_leave_in_orig_chain_in_bytes */
  
  /* In order to avoid duplication of DSM items, make sure no DSM item will be split
  in the middle:
  Since dsm_split_packet is using the ->used of each DSM item in the chain
  and NOT ->size, and since a DSM chain doesn't mean all the items except 
  the last, are full, then:
  If the chain has 3 items, with lengths: l1 (the first), l2 and l3 (the last), where:

  l1+l2 < amount_to_leave_in_orig_chain_in_bytes
  l1+l2+l3 > amount_to_leave_in_orig_chain_in_bytes

  then to insure no dup occurs, the desired amount to split must be l1+l2,
  where l1 and/or l2 could be smaller than the size of one DSM item.
  */

  rounded_off_amount_to_leave_in_orig = 0;
  current_item_ptr = orig_dsm_chain_ptr;

  while (current_item_ptr != NULL)
  {
    if (rounded_off_amount_to_leave_in_orig + current_item_ptr->used > 
      amount_to_leave_in_orig_chain_in_bytes)
    {
      break;
    }
    rounded_off_amount_to_leave_in_orig += current_item_ptr->used;
    current_item_ptr = current_item_ptr->pkt_ptr;
  }

  /* pool_id is provided only for hypothetical reasons, as we already made sure
  no DSM item would be split in the middle, by the round off.
  In addition, the dsm_split_packet returns how many bytes remain in the original
  */
  dsm_split_result = dsm_split_packet(&orig_dsm_chain_ptr, dest_dsm_chain_ptr_ptr, 
    rounded_off_amount_to_leave_in_orig, pool_id);
  HSU_ASSERT(dsm_split_result == rounded_off_amount_to_leave_in_orig);

  /* Make sure split function didn't mess any of the args: */
  HSU_ASSERT(orig_dsm_chain_ptr != NULL);
  HSU_ASSERT((*dest_dsm_chain_ptr_ptr) != NULL);

  return TRUE;
} /* hsu_al_dsm_utils_split_dsm_chain */

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

dsm_mempool_id_type hsu_al_get_uncached_dsm_pool(void)
{
  return (dsm_mempool_id_type)(&uncached_dsm_item_pool);
}

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

void hsu_al_uncached_dsm_init(void)
{

  // Make sure this API is called only once.
  if(run_once == FALSE)
  {
    run_once = TRUE;
  }
  else
  {
    return;
  }

  uncached_dsm_item_array = (uint32*)hsu_conf_sel_alloc_uncached_memory(UNCACHED_DSM_MEM_SIZE);

  dsm_init_pool(UNCACHED_DSM_ITEM_POOL, 
    (uint8*)uncached_dsm_item_array,
    UNCACHED_DSM_MEM_SIZE,
    UNCACHED_DSM_ITEM_SIZE);

}

/*==============================================================================

FUNCTION      HSU_AL_DSM_UTILS_INIT_CONTIGUOUS_BUF_POOL

DESCRIPTION   
  Initializes the contiguous buffer pool when called for the first time.

DEPENDENCIES  
  None.

RETURN VALUE  
  None.

SIDE EFFECTS    
  None.
==============================================================================*/
void hsu_al_dsm_utils_init_contiguous_buf_pool(void)
{
  int i;

  if (!is_initialized)
  {
    /* Initialize DSM level-based flow control for RX
     * Currenty the DSM level-based flow control by HS-USB is only implmented 
     * for LARTGE DSM items. DSM level-based flow control by HS-USB is fail-safe 
     * mechanism.  The flow control is done by the client with IOCTL.
     */
    hsu_al_dsm_utils_pool_info[0].pool_id = DSM_DS_LARGE_ITEM_POOL;
    hsu_al_dsm_utils_pool_info[0].level_count_few = HS_USB_LARGE_DSM_LEVEL_FEW;
    hsu_al_dsm_utils_pool_info[0].level_count_many = HS_USB_LARGE_DSM_LEVEL_MANY;
    hsu_al_dsm_utils_pool_info[0].use_counter = 0;
    hsu_al_dsm_utils_pool_info[0].many_counter = 0;
    hsu_al_dsm_utils_pool_info[0].few_counter = 0;
    hsu_al_dsm_utils_pool_info[0].is_not_available = FALSE;

    hsu_al_dsm_utils_pool_info[1].pool_id = DSM_DIAG_SIO_RX_ITEM_POOL;
    hsu_al_dsm_utils_pool_info[1].level_count_few = HS_USB_DIAG_RX_DSM_LEVEL_FEW;
    hsu_al_dsm_utils_pool_info[1].level_count_many = HS_USB_DIAG_RX_DSM_LEVEL_MANY;
    hsu_al_dsm_utils_pool_info[1].use_counter = 0;
    hsu_al_dsm_utils_pool_info[1].many_counter = 0;
    hsu_al_dsm_utils_pool_info[1].few_counter = 0;    
    hsu_al_dsm_utils_pool_info[1].is_not_available = FALSE;

    hsu_al_dsm_utils_pool_info[2].pool_id = UNCACHED_DSM_ITEM_POOL;
    hsu_al_dsm_utils_pool_info[2].level_count_few = HS_USB_UNCACHED_DSM_LEVEL_FEW;
    hsu_al_dsm_utils_pool_info[2].level_count_many = HS_USB_UNCACHED_DSM_LEVEL_MANY;
    hsu_al_dsm_utils_pool_info[2].use_counter = 0;
    hsu_al_dsm_utils_pool_info[2].many_counter = 0;
    hsu_al_dsm_utils_pool_info[2].few_counter = 0;    
    hsu_al_dsm_utils_pool_info[2].is_not_available = FALSE;

    /* Initialize contiguous buffer information */
    rex_init_crit_sect(&hsu_dsm_utils_contiguous_buf_pool.pool_cs);
    hsu_dsm_utils_contiguous_buf_pool.free_count = HSU_CONTIGUOUS_BUF_ARR_SIZE;
    hsu_dsm_utils_contiguous_buf_pool.min_free_count = HSU_CONTIGUOUS_BUF_ARR_SIZE;

    for (i=0; i<HSU_CONTIGUOUS_BUF_ARR_SIZE; ++i)
    {
      hsu_dsm_utils_contiguous_buf_pool.is_free[i] = TRUE;
    }
    
    is_initialized = TRUE;
  }
}

/*==============================================================================

FUNCTION      HSU_AL_DSM_UTILS_ALLOC_CONTIGUOUS_BUF

DESCRIPTION   
  Allocate a buffer from the contiguous buffer pool.
  This function initializes the contiguous buffer pool when called for the
  first time.

DEPENDENCIES  
  None.

RETURN VALUE  
  Pointer to allocated buffer. NULL if no buffer available.

SIDE EFFECTS    
  None.
==============================================================================*/
uint8* hsu_al_dsm_utils_alloc_contiguous_buf(void)
{
  int i;

  if (!is_initialized)
  {
    hsu_al_dsm_utils_init_contiguous_buf_pool();
  }

  rex_enter_crit_sect(&hsu_dsm_utils_contiguous_buf_pool.pool_cs);
  if (hsu_dsm_utils_contiguous_buf_pool.free_count == 0)
  {
    HSU_MSG_HIGH("hsu_al_dsm_utils_alloc_contiguous_buf: No free buffers");
    return NULL;
  }
  for (i=0; i<HSU_CONTIGUOUS_BUF_ARR_SIZE; ++i)
  {
    if (hsu_dsm_utils_contiguous_buf_pool.is_free[i])
    {
      hsu_dsm_utils_contiguous_buf_pool.is_free[i] = FALSE;

      --hsu_dsm_utils_contiguous_buf_pool.free_count;
      if (hsu_dsm_utils_contiguous_buf_pool.free_count < hsu_dsm_utils_contiguous_buf_pool.min_free_count)
      {
        hsu_dsm_utils_contiguous_buf_pool.min_free_count = hsu_dsm_utils_contiguous_buf_pool.free_count;
      }

      rex_leave_crit_sect(&hsu_dsm_utils_contiguous_buf_pool.pool_cs);
      return hsu_dsm_utils_contiguous_buf_pool.buf_arr[i].buf;
    }
  }

  rex_leave_crit_sect(&hsu_dsm_utils_contiguous_buf_pool.pool_cs);
  return NULL;
}

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
)
{
  int i;

  rex_enter_crit_sect(&hsu_dsm_utils_contiguous_buf_pool.pool_cs);
  for (i=0; i<HSU_CONTIGUOUS_BUF_ARR_SIZE; ++i)
  {
    if (hsu_dsm_utils_contiguous_buf_pool.buf_arr[i].buf == buf_ptr)
    {
      hsu_dsm_utils_contiguous_buf_pool.is_free[i] = TRUE;
      ++hsu_dsm_utils_contiguous_buf_pool.free_count;
      rex_leave_crit_sect(&hsu_dsm_utils_contiguous_buf_pool.pool_cs);
      return;
    }
  }
  rex_leave_crit_sect(&hsu_dsm_utils_contiguous_buf_pool.pool_cs);

  HSU_ERR_FATAL("Invalid buf_ptr (%u)", 
    buf_ptr, 0, 0);
}

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
uint32 hsu_al_dsm_utils_get_contiguous_buf_free_count(void)
{
  return hsu_dsm_utils_contiguous_buf_pool.free_count;
}


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
)
{
  dsm_item_type *curr_item = NULL;
  dsm_item_type *prev_item = NULL;
  dsm_item_type *next_item = NULL;
  dsm_item_type *small_item = NULL;

  if (use_small_item_threshold > DSM_DS_SMALL_ITEM_SIZ)
  {
    HSU_MSG_MED_2("hsu_al_dsm_utils_optimize_dsm_chain_item_alloc: Can't optimize to "
      " small DSm items since use_small_item_threshold (%d) > DSM_DS_SMALL_ITEM_SIZ (%d)",
      use_small_item_threshold,DSM_DS_SMALL_ITEM_SIZ);
    /*TODO: perhaps we want to optimize to MED DSM items???*/
  }

  if (!dsm_chain)
  {
    HSU_MSG_ERROR("hsu_al_dsm_utils_optimize_dsm_chain_item_alloc: invalid input"
                  "dsm_chain = null");
    return ;
  }
  curr_item = *dsm_chain; /*now curr_item point to the first dsm item in the chain*/
  while(curr_item)
  {
    /* perform ZLP dropping anyway */
    if (curr_item->used == 0)
    {
      HSU_MSG_MED("hsu_al_dsm_utils_optimize_dsm_chain_item_alloc: ZLP received. dropping");
      next_item = curr_item->pkt_ptr;
      if (!prev_item)
      {
        *dsm_chain = next_item ;
      }
      else
      {
        prev_item->pkt_ptr = next_item;
      }
      curr_item->pkt_ptr = NULL;
      dsm_free_packet(&curr_item);
      curr_item = next_item;
      continue;
    }

    /* Verify that the dsm_item is from DSM_DS_LARGE_ITEM_POOL */
    /* and the threshold is bigger than DSM_DS_SMALL_ITEM_SIZ */
    /* (otherwise optimization could not be performed) */
    if (curr_item->pool_id != DSM_DS_LARGE_ITEM_POOL || use_small_item_threshold > DSM_DS_SMALL_ITEM_SIZ)
    {
      prev_item = curr_item;
      curr_item = curr_item->pkt_ptr;
      continue;
    }

    if (curr_item->used < use_small_item_threshold)
    {
      /*We need to replace the LARGE item with SMALL one*/
      small_item = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);
      if (small_item == NULL)
      {
        HSU_MSG_ERROR("hsu_al_dsm_utils_optimize_dsm_chain_item_alloc: "
                      "No more memory to allocate SMALL DSM items.");
        return;
      }
      
      small_item->app_field = curr_item->app_field;
      small_item->app_ptr = curr_item->app_ptr;
      small_item->used = curr_item->used;
#ifdef FEATURE_DSM_MEM_CHK_QUEUE_CACHE_LENGTH
      small_item->enqueued_packet_length = curr_item->enqueued_packet_length;
#endif  /*FEATURE_DSM_MEM_CHK_QUEUE_CACHE_LENGTH*/
#ifdef DSM_KIND
      small_item->kind = curr_item->kind;
#endif /*DSM_KIND*/
      small_item->pkt_ptr = curr_item->pkt_ptr;
      memscpy(small_item->data_ptr, curr_item->used, curr_item->data_ptr, curr_item->used);
      if (prev_item)
      {
        prev_item->pkt_ptr = small_item; 
      }
      else
      {
        *dsm_chain = small_item ;
      }
      curr_item->pkt_ptr = NULL;
      dsm_free_packet(&curr_item);
      curr_item = small_item;
    }
    prev_item = curr_item;
    curr_item = curr_item->pkt_ptr;
  }
} /*hsu_al_dsm_utils_optimize_dsm_chain_item_alloc*/

/*===========================================================================

FUNCTION      hsu_al_dsm_utils_pool_few_cb

DESCRIPTION
This function invokes the appropriate actions when the memory pool arrives 
to DSM_MEM_LEVEL_USB_FEW on DSM_MEM_OP_NEW.

DEPENDENCIES
Assume that MANY call-back and FEW call-back is synchronized in DSM with
DSMI_POOL_LOCK/UNLOCK, so MANY call-back cannot be called in the middle of
FEW call-back.

MANY and FEW callback updates "hsu_al_dsm_utils_pool_info[i]->is_not_available" 
which is protected by DSMI_POOL_LOCK/UNLOCK.

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static void hsu_al_dsm_utils_pool_few_cb
(
 dsm_mempool_id_enum_type pool_id,  /* memory pool ID */
 dsm_mem_level_enum_type level,     /* memory pool level */
 dsm_mem_op_enum_type op            /* memory pool operation */
)
{
  int i;

  HSU_USE_PARAM(level);
  HSU_USE_PARAM(op);

  for (i=0; i<HSU_AL_DSM_UTILS_POOLS_MAX; i++)
  {
    hsu_al_dsm_utils_pool_info_type* ptr = &hsu_al_dsm_utils_pool_info[i];
    if ((dsm_mempool_id_enum_type)ptr->pool_id == pool_id)
    {
      ptr->is_not_available = TRUE;
      ptr->few_counter++;
      HSU_MSG_ERROR_3("hsu_*_pool_few_cb: pool 0x%X DSM_MEM_LEVEL_USB_FEW reached: "
                   "it has %d free items, few_counter = %d", pool_id, DSM_POOL_FREE_CNT(pool_id), ptr->few_counter);
      return;
    }
  }
  HSU_MSG_ERROR_1("hsu_*_pool_few_cb: pool 0x%X is unknown", pool_id);
} /*hsu_al_dsm_utils_pool_few_cb*/


/*===========================================================================

FUNCTION      hsu_al_dsm_utils_pool_many_cb_invoke_clients

DESCRIPTION
This function invokes the clients callback in AL TASK when the memory pool 
arrives to DSM_MEM_LEVEL_USB_MANY on DSM_MEM_OP_FREE.

DEPENDENCIES
None

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static void hsu_al_dsm_utils_pool_many_cb_invoke_clients(void *param)
{
  int i;
  dsm_mempool_id_enum_type pool_id = (dsm_mempool_id_enum_type)param;

  for (i=0; i<HSU_AL_DSM_UTILS_POOLS_MAX; i++)
  {
    hsu_al_dsm_utils_pool_info_type* ptr = &hsu_al_dsm_utils_pool_info[i];
    if ((dsm_mempool_id_enum_type)ptr->pool_id == pool_id)
    {
      /* If the is_not_available is set to TRUE, this means that FEW 
       * call-back has been called right after MANY call-back but just 
       * before the AL TASK executed this function from its queue.  In this 
       * case, do not resume the flow.
       */
      if (ptr->is_not_available == TRUE)
      {
        HSU_MSG_HIGH_2("DSM item not available when trying to resume UL flow " 
                     "for DSM_MEM_LEVEL_USB_MANY: pool 0x%X, free items %u", 
                     pool_id, DSM_POOL_FREE_CNT(pool_id));
      }
      else
      {
        HSU_MSG_HIGH_2("Calling client callback in AL TASK to resume UL flow "
                     "for DSM_MEM_LEVEL_USB_MANY: pool 0x%X, free items %u", 
                     pool_id, DSM_POOL_FREE_CNT(pool_id));

        hsu_al_ser_pool_many_cb(pool_id);
#ifdef FEATURE_HS_USB_ECM
        hsu_al_ecm_pool_many_cb(pool_id);
#endif /* FEATURE_HS_USB_ECM */
      }
      return;
    }
  }
}


/*===========================================================================

FUNCTION      hsu_al_dsm_utils_pool_many_cb

DESCRIPTION
This function invokes the appropriate actions when the memory pool arrives 
to DSM_MEM_LEVEL_USB_MANY on DSM_MEM_OP_FREE.  
This callback can be invoked in any task.  The function will set the flag 
"is_not_available" to FALSE immediately to enable the flow, 
and enqueue clients' (SER/ECM) callback into AL TASK.

MANY and FEW callback updates "hsu_al_dsm_utils_pool_info[i]->is_not_available" 
which is protected by DSMI_POOL_LOCK/UNLOCK.

DEPENDENCIES
Assume that MANY call-back and FEW call-back is synchronized in DSM with
DSMI_POOL_LOCK/UNLOCK, so FEW call-back cannot be called in the middle of
MANY call-back.

RETURN VALUE
None

SIDE EFFECTS
None
===========================================================================*/
static void hsu_al_dsm_utils_pool_many_cb
(
 dsm_mempool_id_enum_type pool_id,  /* memory pool ID */
 dsm_mem_level_enum_type level,     /* memory pool level */
 dsm_mem_op_enum_type op            /* memory pool operation */
 )
{
  int i;

  HSU_USE_PARAM(level);
  HSU_USE_PARAM(op);

  for (i=0; i<HSU_AL_DSM_UTILS_POOLS_MAX; i++)
  {
    hsu_al_dsm_utils_pool_info_type* ptr = &hsu_al_dsm_utils_pool_info[i];
    if ((dsm_mempool_id_enum_type)ptr->pool_id == pool_id)
    {
      /* Only enqueue the callback for client if current state was not 
       * is_not_available (FEW was recieved */
      if (ptr->is_not_available)
      {

        HSU_MSG_HIGH_3("hsu_*_pool_many_cb: pool 0x%X DSM_MEM_LEVEL_USB_MANY reached, "
                     "it has %d free items, many hit count = %d", 
                     pool_id, DSM_POOL_FREE_CNT(pool_id), ptr->many_counter++);

        ptr->is_not_available = FALSE;
        /* Enqueue to AL TASK since the MANY callback can be called on
         * any task calling DSM API, and resuming RX requires DSM API calls.
         */
        hsu_al_task_enqueue_cmd(
          hsu_al_dsm_utils_pool_many_cb_invoke_clients, 
          (void *)pool_id);
      }

      /* There is only one type of DSM item in the pool, so there is no point
       * of looping additional times */
      return;
    }
  }

  HSU_MSG_ERROR_1("hsu_*_pool_many_cb: pool 0x%X is unknown", pool_id);
} /*hsu_al_dsm_utils_pool_many_cb*/

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
)
{
  int i;

  for (i=0; i<HSU_AL_DSM_UTILS_POOLS_MAX; i++)
  {
    hsu_al_dsm_utils_pool_info_type* ptr = &hsu_al_dsm_utils_pool_info[i];
    if ((dsm_mempool_id_enum_type)ptr->pool_id == pool_id)
    {
      /* Allow registration only once */
      if (ptr->use_counter == 0)
      {
        ptr->is_not_available = 
          (DSM_POOL_FREE_CNT(pool_id) <= ptr->level_count_few)
          ? TRUE : FALSE;
        dsm_reg_mem_event_level(pool_id,
          DSM_MEM_LEVEL_USB_FEW,
          ptr->level_count_few);
        dsm_reg_mem_event_cb(
          pool_id,
          DSM_MEM_LEVEL_USB_FEW,
          DSM_MEM_OP_NEW,
          hsu_al_dsm_utils_pool_few_cb);

        dsm_reg_mem_event_level(pool_id,
          DSM_MEM_LEVEL_USB_MANY,
          ptr->level_count_many);
        dsm_reg_mem_event_cb(
          pool_id,
          DSM_MEM_LEVEL_USB_MANY,
          DSM_MEM_OP_FREE,
          hsu_al_dsm_utils_pool_many_cb);
      }
      ptr->use_counter++;

      HSU_MSG_HIGH_3("hsu_al_dsm_utils_init: registered cb for pool 0x%X: "
        "FEW (%u), MANY (%u)", 
        pool_id, ptr->level_count_few, ptr->level_count_many);

      HSU_MSG_HIGH_1("hsu_al_dsm_utils_init: registered use counter = %u",
        ptr->use_counter);

      /* Since there is only one matching pool id in the table, 
       * return immdiately.*/
      return;
    }
  }
  HSU_MSG_ERROR_1("hsu_al_dsm_utils_init: pool 0x%X is unknown", pool_id);
} /*hsu_al_dsm_utils_init*/

/*===========================================================================

FUNCTION      hsu_al_dsm_utils_uninit

DESCRIPTION
Un-initializes internal DSM utilities data.

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
)
{
  int i;

  for (i=0; i<HSU_AL_DSM_UTILS_POOLS_MAX; i++)
  {
    hsu_al_dsm_utils_pool_info_type* ptr = &hsu_al_dsm_utils_pool_info[i];
    if ((dsm_mempool_id_enum_type)ptr->pool_id == pool_id)
    {
      ptr->few_counter = 0;
      ptr->many_counter = 0;
      switch (ptr->use_counter)
      {
      case 0:
        break;
      case 1:
        dsm_unreg_mem_event_cb(
          pool_id,
          DSM_MEM_LEVEL_USB_MANY,
          DSM_MEM_OP_FREE);

        dsm_unreg_mem_event_cb(
          pool_id,
          DSM_MEM_LEVEL_USB_FEW,
          DSM_MEM_OP_NEW);
        ptr->is_not_available = FALSE;
      default:
        ptr->use_counter--;
        HSU_MSG_HIGH_1("hsu_al_dsm_utils_uninit: registered use counter = %u",
          ptr->use_counter);
        break;
      }
      return;
    }
  }
  HSU_MSG_ERROR_1("hsu_al_dsm_utils_uninit: pool 0x%X is unknown", pool_id);
} /*hsu_al_dsm_utils_uninit*/

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
)
{
  int i;

  for (i=0; i<HSU_AL_DSM_UTILS_POOLS_MAX; i++)
  {
    hsu_al_dsm_utils_pool_info_type* ptr = &hsu_al_dsm_utils_pool_info[i];
    if ((dsm_mempool_id_enum_type)ptr->pool_id == pool_id)
    {
      return ptr->is_not_available;
    }
  }
  HSU_MSG_ERROR_1("hsu_*_pool_is_not_available: pool 0x%X is unknown", pool_id);
  return FALSE;
} /*hsu_al_dsm_utils_pool_is_not_available*/


/*===========================================================================
FUNCTION      hsu_al_ecm_print_f3
==============================================================================*/
void hsu_al_dsm_utils_f3(void)
{
  uint32                            index;
  hsu_al_dsm_utils_pool_info_type*  ptr;

  for (index = 0; index < HSU_AL_DSM_UTILS_POOLS_MAX; index++)
  {
    ptr = &hsu_al_dsm_utils_pool_info[index];
    if (NULL != ptr->pool_id)
    {
      HSU_MSG_HIGH("----------------------------------------------------");
      switch(index)
      {
        case 0:   HSU_MSG_HIGH(" DSM_DS_LARGE_ITEM_POOL    (0)"); break;
        case 1:   HSU_MSG_HIGH(" DSM_DIAG_SIO_RX_ITEM_POOL (1)"); break;
        case 2:   HSU_MSG_HIGH(" UNCACHED_DSM_ITEM_POOL    (2)"); break;
        default:  HSU_MSG_HIGH(" UKNOWN_DSM_ITEM_POOL      (?)"); break;
      }   
      HSU_MSG_HIGH("----------------------------------------------------");

      HSU_MSG_HIGH_3("item_size       (%4u), total_item_count (%4u), free_item_count  (%4u)",
        DSM_POOL_ITEM_SIZE(ptr->pool_id),
        DSM_POOL_ITEM_CNT(ptr->pool_id),
        DSM_POOL_FREE_CNT(ptr->pool_id));

      HSU_MSG_HIGH_3("level_count_few (%4u), level_count_many (%4u), use_counter      (%4u)",
        ptr->level_count_few, ptr->level_count_many, ptr->use_counter);

      HSU_MSG_HIGH_3("few_counter     (%4u), many_counter     (%4u), is_not_available (%4u)",
        ptr->few_counter, ptr->many_counter, ptr->is_not_available);
    }
  }
}

