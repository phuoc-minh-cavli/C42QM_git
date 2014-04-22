/*===========================================================================

                Shared Memory Partition Implementation File


  Copyright (c) 2013, 2018 Qualcomm Technologies, Inc.  All Rights Reserved.
  Qualcomm Technologies Proprietary and Confidential.
===========================================================================*/


/*===========================================================================

                           EDIT HISTORY FOR FILE

$Header: //components/rel/core.tx/6.0/mproc/smem/src/smem_partition.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/11/18   rv      Handle SMEM_INVALID_HOST 0xFFFF value.
06/04/14   rv      params.flags is properly updated while returning from 
                   smem_alloc_ex()/smem_get_addr_ex_locked()
03/13/14   sm      64-bit support.
02/19/14   an      smem refactored to prevent deadlock in qnx
12/06/13   an      Fix address calculation for preallocated items in cached partition.
12/12/13   rv      SMEM Partition backwards compatibility is removed.
10/21/13   bt      Check remote_host range before accessing smem_info_prt.
10/09/13   bt      Ensure to map partition pages before accessing.
08/09/13   bt      Add smem_get_part_sz, for internal support.
07/29/13   pa      Make SMD use secure SMEM partitions, with BW compatibility.
07/02/13   pa      Enable caller to use remote_host == SMEM_THIS_HOST to
                   access default partition.  This makes it easier to loop
                   through SMEM hosts.
06/25/13   pa      Remove local list usage; instead just search SMEM.
                   Use smem_get_base_addr and smem_get_size for SMEM base and
                   size.
                   Changed documentation and naming from 'upwards' to 
                   'uncached' and 'downwards' to 'cached' to avoid confusion.
                   Added canary and padding size to allocation header.
                   Hide padding and actual SMEM allocation from caller by 
                   using saved padding values.
                   Minor changes to support backwards compatibility with 
                   legacy SMEM allocation table.
                   Other misc cleanup.
06/06/13   bt      Resolve some type conversion warnings.
05/30/13   pa      Extern smem_init_info to get virtual SMEM base.
04/24/13   pa      Initial version.
===========================================================================*/

/*===========================================================================
                        INCLUDE FILES
===========================================================================*/
#include "smem_os.h"
#include "smem.h"
#include "smem_type.h"
#include "smem_target.h"
#include "smem_partition.h"
#include "smem_internal.h"
#include "smem_toc.h"
#include "spinlock.h"
#include "smem_os_common.h"
#include "smem_v.h"

/*===========================================================================
                           MACRO DEFINITIONS
===========================================================================*/
#define SMEM_PRT_ASSERT(cond)  ASSERT(cond)

/*===========================================================================
                           GLOBAL DATA
===========================================================================*/

/** The main info structure for SMEM partitions. 
 ** Contains local copy of allocation info from each of the SMEM partitions.
 ** Updated on calls to smem_get_addr_ex() */
smem_partition_info_type smem_info_prt[SMEM_NUM_HOSTS];

/*===========================================================================
                LOCAL FUNCTION DEFINITIONS
===========================================================================*/
static boolean smem_process_item
(
  smem_alloc_params_type * params,
  smem_partition_allocation_header_type ** item_hdr,
  uint16 padding_header,
  uint32 * size_remaining,
  boolean cached
);

static int32 smem_get_addr_ex_locked
(
  smem_alloc_params_type * params
);

/*===========================================================================
FUNCTION      smem_ensure_uncached_mapping
===========================================================================*/
/**
  Compares the current offset index for the mapped uncached pages in the 
  partition against the new offset of actual uncached SMEM allocations.  If 
  the new allocation(s) fall outside the currently mapped pages (new_offset is 
  greater than the current mapped offset), the difference will be mapped, 
  rounded up to the nearest minimum page size multiple.

  @param[in]    info        Pointer to the local partition info structure.
  @param[in]    new_offset  The new offset that must be mapped up to 
                            (as uncached memory).

  @return
  None.
*/
/*=========================================================================*/
static void smem_ensure_uncached_mapping
(
  smem_partition_info_type *info, 
  uint32                    new_offset
)
{
  if (new_offset > info->offset_mapped_uncached)
  {
    uint32 mapping_size = new_offset - info->offset_mapped_uncached;

    mapping_size = smem_map_page_attr(
                        (uint32)(((char *)info->header - smem_get_base_addr()) 
                        + info->offset_mapped_uncached), 
                           mapping_size, FALSE);
    info->offset_mapped_uncached += mapping_size;
  }
}

/*===========================================================================
FUNCTION      smem_ensure_cached_mapping
===========================================================================*/
/**
  Compares the current offset index for the mapped cached pages in the 
  partition against the new offset of actual cached SMEM allocations.  If 
  the new allocation(s) fall outside the currently mapped pages (new_offset is 
  less than the current mapped offset), the difference will be mapped, 
  rounded up to the nearest minimum page size multiple.

  @param[in]    info        Pointer to the local partition info structure.
  @param[in]    new_offset  The new offset that must be mapped down to 
                            (as cached memory).

  @return
  None.
*/
/*=========================================================================*/
static void smem_ensure_cached_mapping
(
  smem_partition_info_type *info,
  uint32                    new_offset
)
{
  if (new_offset < info->offset_mapped_cached)
  {
    uint32 mapping_size = info->offset_mapped_cached - new_offset;

    mapping_size = smem_map_page_attr(
      (uint32)(((char *)info->header - smem_get_base_addr()) + new_offset),
                           mapping_size, TRUE);
    info->offset_mapped_cached -= mapping_size;
  }
}

/*===========================================================================
                  PUBLIC FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      smem_partition_init
===========================================================================*/
/**
  Initializes the SMEM partitioning structures. 

  This function checks for the existence of the SMEM table of contents and then
  reads it to determine partition locations and size.  Data structures are 
  initialized to enable SMEM allocation from protected partitions.

  @return
  SMEM_STATUS_SUCCESS          - Initialization was successful
  SMEM_STATUS_FAILURE          - Initialization failed
*/
/*=========================================================================*/
int32 smem_partition_init(void)
{
  uint32 i;
  uint32 size;
  int32 status;
  char* smem_base;
  uint32 smem_size;
  smem_alloc_params_type params;
  smem_toc_type * toc;

  /* Initialize size to 0 */
  for (i=0; i<SMEM_NUM_HOSTS; i++)
  {
    /* Init size to zero.  After this routine is finished, size of 0 indicates
     * that there is no protected partition for the edge */
    smem_info_prt[i].size = 0;
  }

  /* Calculate SMEM TOC location and map it if necessary. */
  smem_base = smem_get_base_addr();
  smem_size = smem_get_size();
  (void)smem_map_page_attr(smem_size - SMEM_TOC_SIZE, SMEM_TOC_SIZE, FALSE);
  toc = (smem_toc_type *)(smem_base + smem_size - SMEM_TOC_SIZE);

  /* Check if we have a valid TOC. If not, nothing to do.
   * Absence of TOC is supported as the Secure SMEM feature is rolled out.
   * When there is no TOC, calls to smem_alloc_ex and smem_get_addr_ex will 
   * call the legacy functions which operate on non-protected SMEM. */
  if (SMEM_READ_SMEM_4(&toc->identifier) != SMEM_TOC_IDENTIFIER)
  {
    return SMEM_STATUS_SUCCESS;
  }
  
  /* Check that TOC has the expected fields */
  SMEM_PRT_ASSERT(SMEM_READ_SMEM_4(&toc->version) == 1);
  SMEM_PRT_ASSERT(SMEM_READ_SMEM_4(&toc->num_entries) <= SMEM_MAX_PARTITIONS);

  /* Loop through the TOC and grab the following info:
   * From the TOC, get the base addr and subsystem IDs of the partition owners.
   * From the partition headers, get the partition size since this value has 
   * the exclusion ranges already subtracted out. */
  for (i=0; i<toc->num_entries; i++)
  {
    smem_host_type remote_host;
    smem_host_type host0 = (smem_host_type)
        SMEM_READ_SMEM_2(&toc->entry[i].host0);
    smem_host_type host1 = (smem_host_type)
        SMEM_READ_SMEM_2(&toc->entry[i].host1);

    /* Check for common partition, host ids should be 0xFFFE */
    if (host0 == SMEM_COMMON_HOST && host1 == SMEM_COMMON_HOST)
    {
      /* Use local host for common partition */
      remote_host = SMEM_THIS_HOST;
    }
    else
    {
      /* Determine if the local host is part of this entry, and what the remote
       * host is as well. If local host is not part of this entry, the variable
       * is set to invalid. */
      remote_host = host0 == SMEM_THIS_HOST ? host1 :
                    host1 == SMEM_THIS_HOST ? host0 :
                    SMEM_INVALID_HOST;
    }
    /* ignore remote host if local host does not know about remote host ID */
    if (remote_host >= SMEM_NUM_HOSTS)
    {
      remote_host = SMEM_INVALID_HOST;
    }

    /* Read size of the partition from the TOC entry */
    size = SMEM_READ_SMEM_4(&toc->entry[i].size);
    
    /* Skip this entry if TOC size is 0 or host is invalid */
    if (size != 0 && remote_host != SMEM_INVALID_HOST)
    {
      uint32 partition_offset = SMEM_READ_SMEM_4(&toc->entry[i].offset);
      uint32 size_cacheline = SMEM_READ_SMEM_4(&toc->entry[i].size_cacheline);
      smem_partition_header_type * hdr;
      uint32 offset_mapped_uncached;
      
      /* Map first page of partition, in order to access the header. */
      offset_mapped_uncached = smem_map_page_attr(partition_offset, 
          sizeof(smem_partition_header_type), FALSE);
      hdr = (smem_partition_header_type * )(smem_base + partition_offset);

      SMEM_PRT_ASSERT(((unsigned int)hdr & SMEM_PAGE_ALIGN_MASK) == 0);
      SMEM_PRT_ASSERT(hdr->identifier == SMEM_PARTITION_HEADER_ID);
      SMEM_PRT_ASSERT(partition_offset > 0);
      SMEM_PRT_ASSERT(partition_offset < smem_size - SMEM_TOC_SIZE);

      /* Read size from partition header.
       * This size equals size in TOC minus size of all the exclusion ranges. */
      size = SMEM_READ_SMEM_4(&hdr->size);
      SMEM_PRT_ASSERT((size & SMEM_PAGE_ALIGN_MASK) == 0);

      /* If size is non-zero, then we have a valid partition */
      if (size)
      {
        /* Init local info structure to default values indicating that 
         * there are no known SMEM items. 
         * These will be updated on the call to smem_get_addr_ex. */
        smem_info_prt[remote_host].header               = hdr;
        smem_info_prt[remote_host].size                 = size;
        smem_info_prt[remote_host].size_cacheline       = size_cacheline;
        smem_info_prt[remote_host].offset_free_uncached = 
            sizeof(smem_partition_header_type);
        smem_info_prt[remote_host].offset_free_cached   = size;
        smem_info_prt[remote_host].offset_mapped_uncached = 
            offset_mapped_uncached;
        smem_info_prt[remote_host].offset_mapped_cached = 
            smem_info_prt[remote_host].offset_free_cached;

        /* Clear out debug info */
        memset(&smem_info_prt[remote_host].debug_info, 0, 
          sizeof(smem_info_prt[remote_host].debug_info));

        /* Scan SMEM to read the current state of partition allocations.  
         * This call will update the info struct for this edge.
         * It doesn't matter what item ID we search for since the allocation list 
         * is empty at this point.  The only needed param is the remote_host
         * so that we scan the right partition. */
        params.remote_host = remote_host;
        params.smem_type   = SMEM_MEM_FIRST;
        status = smem_get_addr_ex(&params);
        if (status != SMEM_STATUS_SUCCESS && status != SMEM_STATUS_NOT_FOUND)
        {
          ERR_FATAL("SMEM Partition %d init failed with status 0x%x", 
            remote_host, status, 0);
          return SMEM_STATUS_FAILURE;
        }
      }
    }
  }

  return SMEM_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      smem_alloc_ex
===========================================================================*/
/**
  Requests a pointer to a buffer in shared memory.  Upon success, params.buffer
  points to the allocation in shared memory.  

  @param[inout] params  See definition of smem_alloc_params_type for details.

  @return
  SMEM_STATUS_SUCCESS          - Allocation was successful,
                                 or already exists with specified size.
                                 Pointer to SMEM buffer is saved in 
                                 params.buffer.
                                 params.size contains the requested size rounded
                                 up to 8 bytes.
                                 params.flags contains the status of the cached
                                 flag, which may not match what was 
                                 requested if the item has been allocated 
                                 previously.
  SMEM_STATUS_SIZE_MISMATCH    - Allocation exists, but has different size.
                                 This is possible when another processor has 
                                 already allocated an item with this SMEM ID and
                                 a different size.
                                 Pointer to SMEM buffer is saved in 
                                 params.buffer.
                                 params.size contains the originally allocated
                                 size rounded up to 8 bytes.
                                 params.flags contains the status of the cached
                                 flag, which may not match what was 
                                 requested if the item has been allocated 
                                 previously.
  SMEM_STATUS_INVALID_PARAM    - Invalid parameter
  SMEM_STATUS_OUT_OF_RESOURCES - Not enough SMEM to allocate this buffer
  SMEM_STATUS_FAILURE          - General failure

  @sideeffects
  This function uses spinlocks for exclusive access to the shared memory heap.
*/
/*=========================================================================*/
int32 smem_alloc_ex
(
  smem_alloc_params_type * params
)
{
  int32  status;
  uint32 size_rounded8;
  uint32 size_remaining;
  uint32 size_item;
  uint32 size_total;
  uint16 padding_data;
  boolean use_legacy = FALSE;
  
  smem_partition_allocation_header_type * item_hdr;
  smem_partition_info_type * info = NULL;

  /* Check remote_host field */
  if (params->remote_host >= SMEM_NUM_HOSTS)
  {
    if (params->remote_host == SMEM_INVALID_HOST)
    {
      use_legacy = TRUE;
    }
    else
    {
      return SMEM_STATUS_INVALID_PARAM;
    }
  }

  /* Check smem_type and size fields for valid params . 
   * Note that size is checked in more detail further below. */
  if (params->smem_type >= SMEM_INVALID ||
      params->size == 0 || 
      params->size > smem_get_size())
  {
    return SMEM_STATUS_INVALID_PARAM;
  }

  /* If remote host is invalid or local host, call legacy routine.
   * Caller can use SMEM_INVALID_HOST to allocate unprotected SMEM. 
   * Also, if there is no partition for this edge, call legacy routine */  
  if (params->remote_host < SMEM_NUM_HOSTS)
  {
    /* Set local info pointer to make the code less wordy */
    info = &smem_info_prt[params->remote_host];
    if (params->remote_host == SMEM_THIS_HOST || info->size == 0)
    {
      use_legacy = TRUE;
    }
    else
    {
      use_legacy = FALSE;
    }
  }
  
  /* Save the size that the caller requested.
   * Round up to be consistent with legacy smem_alloc(). */
  size_rounded8 = ROUND_UP(params->size, 8);

  /* If allocating from the top (cached range), add padding size */
  if (info != NULL && (params->flags & SMEM_ALLOC_FLAG_CACHED))
  {
    /* Round size up to a cacheline */
    size_item = ROUND_UP(params->size, info->size_cacheline);

    /* Total size is rounded size, plus header size, plus padding */
    size_total = size_item + sizeof(smem_partition_allocation_header_type) + 
      SMEM_PARTITION_ITEM_PADDING(info->size_cacheline);
  }
  else
  {
    /* Round size to 8 bytes */
    size_item = size_rounded8;

    /* Total size is rounded size param plus size of the header */
    size_total = size_item + sizeof(smem_partition_allocation_header_type);
  }

  /* Save the amount that we've rounded up.  This is used to hide the rounding
   * up from the caller, who may ignore the updated size param that is returned */
  padding_data = size_item - params->size;

  /* Grab SMEM spin lock.
   * Spinlock is acquired here to make the scan + allocation atomic.  
   * Unlocking inbetween the scan and allocation would allow a remote processor
   * to modify smem and we would be out of sync. */
  smem_spin_lock(SMEM_SPINLOCK_SMEM_ALLOC);

  /* Check if item already exists.  Note that this function will update the 
   * params structure if found. */
  status = smem_get_addr_ex_locked(params);
  if (SMEM_STATUS_SUCCESS == status)
  {
    smem_spin_unlock(SMEM_SPINLOCK_SMEM_ALLOC);

    /* If size does not match, indicate so in the return value */
    return params->size == size_rounded8 ?
           SMEM_STATUS_SUCCESS : 
           SMEM_STATUS_SIZE_MISMATCH;
  }

  /* If using default partition, call legacy routine. */
  if (use_legacy)
  {
    void * ret = smem_alloc_locked(params->smem_type, size_rounded8);
    smem_spin_unlock(SMEM_SPINLOCK_SMEM_ALLOC);

    if (ret)
    {
      params->buffer = ret;
      params->size = size_rounded8;
      params->flags &= ~(SMEM_ALLOC_FLAG_CACHED | 
                         SMEM_ALLOC_FLAG_PARTITION_ONLY);
      return SMEM_STATUS_SUCCESS;
    }
    return SMEM_STATUS_FAILURE;
  }

  if (params->flags & SMEM_ALLOC_FLAG_CACHED)
  {
    /* Calculate size remaining, which is the difference between the cached 
     * offset and the furthest uncached page that has been mapped.  Note that 
     * these pointers are updated in the info struct in smem_get_addr_ex(). */
    size_remaining = info->offset_free_cached - info->offset_mapped_uncached;

    /* Check for room in the partition heap.  Do this after checking for smem 
     * item already existing to ensure that size_remaining is up to date. */
    if (size_total > size_remaining)
    {
      smem_spin_unlock(SMEM_SPINLOCK_SMEM_ALLOC);
      return SMEM_STATUS_OUT_OF_RESOURCES;
    }

    /* Allocate from cached side.
     * Note that the padding exists above the item header and below the previous
     * item. */
    item_hdr = (smem_partition_allocation_header_type*)
                    ((size_t)info->header + 
                             info->offset_free_cached -
                             sizeof(smem_partition_allocation_header_type) - 
                             SMEM_PARTITION_ITEM_PADDING(info->size_cacheline));
    params->buffer = (void *)((size_t)info->header + 
                                      info->offset_free_cached -
                                      size_total);
    params->size = size_rounded8;

    /* We must map any additional required pages at least down to the bottom 
     * of this new item. */
    smem_ensure_cached_mapping(info, info->offset_free_cached - size_total);

    /* Fill in the item header in SMEM before updating the offset */
    SMEM_WRITE_SMEM_2(&item_hdr->canary,          SMEM_ALLOC_HDR_CANARY);
    SMEM_WRITE_SMEM_2(&item_hdr->smem_type,       params->smem_type);
    SMEM_WRITE_SMEM_4(&item_hdr->size,            size_item);
    SMEM_WRITE_SMEM_2(&item_hdr->padding_data,    padding_data);
    SMEM_WRITE_SMEM_2(&item_hdr->padding_header,  
      SMEM_PARTITION_ITEM_PADDING(info->size_cacheline));
    
    /* Update the offset, both locally and in smem */
    info->offset_free_cached -= size_total;
    SMEM_WRITE_SMEM_4(&info->header->offset_free_cached, info->offset_free_cached);

    /* Increment debug count */
    info->debug_info.num_alloc_cached++;
  }
  else
  {
    /* Calculate size remaining, which is the difference between the uncached
     * offset and the furthest cached page that has been mapped.  Note that 
     * these pointers are updated in the info struct in smem_get_addr_ex(). */
    size_remaining = info->offset_mapped_cached - info->offset_free_uncached;

    /* Check for room in the partition heap.  Do this after checking for smem 
     * item already existing to ensure that size_remaining is up to date. */
    if (size_total > size_remaining)
    {
      smem_spin_unlock(SMEM_SPINLOCK_SMEM_ALLOC);
      return SMEM_STATUS_OUT_OF_RESOURCES;
    }

    /* Allocate from non-cached side */
    item_hdr = (smem_partition_allocation_header_type*)
                    ((size_t)info->header + 
                             info->offset_free_uncached);
    params->buffer = (void *)((size_t)info->header + 
                                      info->offset_free_uncached +
                                      sizeof(smem_partition_allocation_header_type));
    params->size = size_rounded8;
    
    /* We must map any additional required pages at least up to the end of this
     * new item. */
    smem_ensure_uncached_mapping(info, info->offset_free_uncached + size_total);

    /* fill in the item header in SMEM before updating the offset */
    SMEM_WRITE_SMEM_2(&item_hdr->canary,          SMEM_ALLOC_HDR_CANARY);
    SMEM_WRITE_SMEM_2(&item_hdr->smem_type,       params->smem_type);
    SMEM_WRITE_SMEM_4(&item_hdr->size,            size_item);
    SMEM_WRITE_SMEM_2(&item_hdr->padding_data,    padding_data);
    SMEM_WRITE_SMEM_2(&item_hdr->padding_header,  0);

    /* update the offset, both locally and in smem */
    info->offset_free_uncached += size_total;
    SMEM_WRITE_SMEM_4(&info->header->offset_free_uncached, info->offset_free_uncached);

    /* Increment debug count */
    info->debug_info.num_alloc_uncached++;
  }

  SMEM_MEMORY_BARRIER();

  /* End critical section */
  smem_spin_unlock(SMEM_SPINLOCK_SMEM_ALLOC);
  
  return SMEM_STATUS_SUCCESS;
}

/*===========================================================================
FUNCTION      smem_get_addr_ex_locked
===========================================================================*/
/**
  Requests the address and size of an allocated buffer in shared memory.  
  If found, sets the buffer and size fields of the params struct.

  @param[inout] params  params.smem_type must be set to the ID to search for.
                        params.remote_host must be set to the ID of the remote
                        host of the partition.
                        If params.flags has the SMEM_ALLOC_FLAG_PARTITION_ONLY 
                        flag set, then this function will not search the default
                        partition for the item.  This is useful when SMEM item
                        X is possible in both the default partition and an edge-
                        pair partition, and the caller wants to find out if the
                        item exists in the edge-pair partition only.

  @return
  SMEM_STATUS_SUCCESS       - Allocated buffer found
                              params.buffer contains a pointer to the allocation.
                              params.size contains the size of the allocation
                              as originally requested, rounded up to 8 bytes.
                              params.flags contains the status of the cached
                              flag.
  SMEM_STATUS_NOT_FOUND     - Allocated buffer not found
  SMEM_STATUS_INVALID_PARAM - Invalid parameter
  SMEM_STATUS_FAILURE       - Failure occurred

  @sideeffects
  This function uses spinlocks for exclusive access to the shared memory heap.
  This function updates the size_remaining field for the partition.
*/
/*=========================================================================*/
static int32 smem_get_addr_ex_locked
(
  smem_alloc_params_type * params
)
{
  smem_partition_info_type * info;
  smem_partition_allocation_header_type * item_hdr;
  uint32 size_remaining;
  size_t limit_addr;
  uint16 padding_header;

  /* Check smem_type field */
  if (params->smem_type >= SMEM_INVALID)
  {
    return SMEM_STATUS_INVALID_PARAM;
  }

  /* Check remote_host field */
  if (params->remote_host >= SMEM_NUM_HOSTS)
  {
    /* If remote host is invalid, call legacy routine. */
    if (params->remote_host == SMEM_INVALID_HOST)
    {
      /* save callers size since smem_get_addr will set it to 0
       * if not found. */
      uint32 local_size;
      void * ret = smem_get_addr_locked(params->smem_type, &local_size);
      if (ret)
      {
        params->buffer = ret;
        params->size = local_size;
        params->flags &= ~(SMEM_ALLOC_FLAG_CACHED | 
                           SMEM_ALLOC_FLAG_PARTITION_ONLY);
        return SMEM_STATUS_SUCCESS;
      }
      return SMEM_STATUS_NOT_FOUND;
    }
    else
    {
      return SMEM_STATUS_INVALID_PARAM;
    }
  }

  /* If remote host is invalid or local host, call legacy routine.
   * Caller can use SMEM_INVALID_HOST to search unprotected SMEM. 
   * Also, if there is no partition for this edge, call legacy routine */
  /* Set local info pointer to make the code less wordy */
  info = &smem_info_prt[params->remote_host];

  if (params->remote_host == SMEM_THIS_HOST || info->size == 0)
  {
    /* save callers size since smem_get_addr will set it to 0
     * if not found. */
    uint32 local_size;
    void * ret = smem_get_addr_locked(params->smem_type, &local_size);
    if (ret)
    {
      params->buffer = ret;
      params->size = local_size;
      params->flags &= ~(SMEM_ALLOC_FLAG_CACHED | 
                         SMEM_ALLOC_FLAG_PARTITION_ONLY);
      return SMEM_STATUS_SUCCESS;
    }
    return SMEM_STATUS_NOT_FOUND;
  }

  /* Read the heap pointers in SMEM. */
  info->offset_free_cached   = SMEM_READ_SMEM_4(
      &info->header->offset_free_cached);
  info->offset_free_uncached = SMEM_READ_SMEM_4(
      &info->header->offset_free_uncached);

  /* Check pointer values.
   * Uncached offset must always be less than or equal to cached offset. 
   * Both offsets must be less than the size of the partition. 
   * This protects against a possible overflow outside of the partition. */
  if ((info->offset_free_uncached > info->offset_free_cached) ||
      (info->offset_free_uncached > info->size) || 
      (info->offset_free_cached   > info->size))
  {
    smem_spin_unlock(SMEM_SPINLOCK_SMEM_ALLOC);
    ERR_FATAL("Invalid SMEM heap pointers. Uncached: 0x%X, cached: 0x%X", 
      info->offset_free_uncached, info->offset_free_cached, 0);
    return SMEM_STATUS_OUT_OF_RESOURCES;
  }

  /* We must map any unmapped pages at least up to the offset_free_uncached, 
   * before accessing those items and their headers. */
  smem_ensure_uncached_mapping(info, info->offset_free_uncached);

  /* Init size.  We will use this to check for valid item sizes during the 
   * scan. */
  size_remaining = info->size - sizeof(smem_partition_header_type);

  /* Padding is 0 for uncached allocations */
  padding_header = 0;

  /* Point to first item */
  item_hdr = (smem_partition_allocation_header_type *)
                ((size_t)info->header + sizeof(smem_partition_header_type));

  /* Set limit of scan */
  limit_addr = (size_t)info->header + info->offset_free_uncached;

  /* Scan from uncached side of heap */
  while ((size_t)item_hdr < limit_addr)
  {
    /* Process the next item, and check if it is the item we are searching 
     * for. */
    if (smem_process_item(
          params,           /* callers parameters */
          &item_hdr,        /* pointer to item header in SMEM */
          padding_header,   /* size of padding */
          &size_remaining,  /* size remaining */
          FALSE))           /* cached flag */
    {
      return SMEM_STATUS_SUCCESS;
    }
  }

  /* Now map any required pages in the cached direction. */
  smem_ensure_cached_mapping(info, info->offset_free_cached);

  /* Padding size is fixed for cached allocations based on the TOC */
  padding_header = SMEM_PARTITION_ITEM_PADDING(info->size_cacheline);
  
  /* Scan from cacheable side of heap.  
   * Point to first item */
  item_hdr = (smem_partition_allocation_header_type *)
                ((size_t)info->header + info->size - padding_header - 
                sizeof(smem_partition_allocation_header_type));

  /* Set limit of scan */
  limit_addr = (size_t)info->header + info->offset_free_cached;

  /* Scan from cached side of heap */
  while ((size_t)item_hdr >= limit_addr)
  {
    /* Process the next item, and check if it is the item we are searching 
     * for. */
    if (smem_process_item(
          params,           /* callers parameters */
          &item_hdr,        /* pointer to item header in SMEM */
          padding_header,   /* size of padding */
          &size_remaining,  /* size remaining */
          TRUE))            /* cached flag */
    {
      return SMEM_STATUS_SUCCESS;
    }
  }

  return SMEM_STATUS_NOT_FOUND;
}

/*===========================================================================
FUNCTION      smem_get_addr_ex
===========================================================================*/
/**
  Requests the address and size of an allocated buffer in shared memory.  
  If found, sets the buffer and size fields of the params struct.

  @param[inout] params  params.smem_type must be set to the ID to search for.
                        params.remote_host must be set to the ID of the remote
                        host of the partition.
                        If params.flags has the SMEM_ALLOC_FLAG_PARTITION_ONLY 
                        flag set, then this function will not search the default
                        partition for the item.  This is useful when SMEM item
                        X is possible in both the default partition and an edge-
                        pair partition, and the caller wants to find out if the
                        item exists in the edge-pair partition only.

  @return
  SMEM_STATUS_SUCCESS       - Allocated buffer found
                              params.buffer contains a pointer to the allocation.
                              params.size contains the size of the allocation
                              as originally requested, rounded up to 8 bytes.
                              params.flags contains the status of the cached
                              flag.
  SMEM_STATUS_NOT_FOUND     - Allocated buffer not found
  SMEM_STATUS_INVALID_PARAM - Invalid parameter
  SMEM_STATUS_FAILURE       - Failure occurred

  @sideeffects
  This function uses spinlocks for exclusive access to the shared memory heap.
  This function updates the size_remaining field for the partition.
*/
/*=========================================================================*/
int32 smem_get_addr_ex
(
  smem_alloc_params_type * params
)
{
  int32 ret;

  /* Grab multiprocessor lock on SMEM allocations */
  smem_spin_lock(SMEM_SPINLOCK_SMEM_ALLOC);

  ret = smem_get_addr_ex_locked(params);

  /* End critical section */
  smem_spin_unlock(SMEM_SPINLOCK_SMEM_ALLOC);

  return ret;
}

/*===========================================================================
FUNCTION      smem_process_item
===========================================================================*/
/**
  Reads info about an allocation from SMEM.

  This function was added to combine and reuse code in smem_get_addr_ex. 
  It shouldn't be called from other functions.

  @param[in]    params    If new allocation matches the smem type here,
                          the function returns TRUE and params.buffer and 
                          params.size are updated.
  @param[inout] item_hdr  Pointer to item allocation header
  @param[in]    padding_header  Number of bytes of padding on the item header
  @param[inout] size_remaining  Number of bytes remaining in the heap.
                                Used to validate size of new item.
  @param[in]    cached    Flag indicating if this is a cached item

  @return
  TRUE  - if item matches searched for item
  FALSE - otherwise
*/
/*=========================================================================*/
static boolean smem_process_item
(
  smem_alloc_params_type * params,
  smem_partition_allocation_header_type ** item_hdr,
  uint16 padding_header,
  uint32 * size_remaining,
  boolean cached
)
{
  uint32 size_item;
  uint16 smem_type;
  uint16 canary;
  uint32 size_total;
  boolean found = FALSE;

  /* Read details from SMEM */
  canary    = SMEM_READ_SMEM_2(&((*item_hdr)->canary));
  smem_type = SMEM_READ_SMEM_2(&((*item_hdr)->smem_type));
  size_item = SMEM_READ_SMEM_4(&((*item_hdr)->size));

  /* Calculate total size of the item */
  size_total = size_item + padding_header + 
               sizeof(smem_partition_allocation_header_type);

  /* Canary check */
  if (canary != SMEM_ALLOC_HDR_CANARY)
  {
    smem_spin_unlock(SMEM_SPINLOCK_SMEM_ALLOC);
    ERR_FATAL("Invalid SMEM canary 0x%X at location 0x%X", 
      canary, (uint32)(*item_hdr), 0);
    return FALSE;
  }

  /* Size check */
  if (size_total > *size_remaining)
  {
    smem_spin_unlock(SMEM_SPINLOCK_SMEM_ALLOC);
    ERR_FATAL("Invalid SMEM item size. Total size: %d, remaining: %d", 
      size_total, *size_remaining, 0);
    return FALSE;
  }

  /* Update remaining size */
  *size_remaining -= size_total;

  /* Finally, check if this is the item we are searching for */
  if (smem_type == params->smem_type)
  {
    if (cached)
    {
      /* buffer is directly below the header */
      params->buffer = (void *)((size_t)(*item_hdr) - size_item);
      params->flags |= SMEM_ALLOC_FLAG_CACHED;
    }
    else
    {
      /* buffer is after header */
      params->buffer = (void *)((size_t)(*item_hdr) + 
        sizeof(smem_partition_allocation_header_type));
      params->flags &= ~SMEM_ALLOC_FLAG_CACHED;
    }

    /* remove padding size when returning to caller */
    params->size = size_item - SMEM_READ_SMEM_2(&(*item_hdr)->padding_data);

    /* but round up to 8 bytes to be consistent with legacy smem_get_addr */
    params->size = ROUND_UP(params->size, 8);

    found = TRUE;
  }

  /* Point to the next item */
  if (cached)
  {
    *item_hdr = (smem_partition_allocation_header_type *)
                   ((size_t)(*item_hdr) - size_total);
  }
  else
  {
    *item_hdr = (smem_partition_allocation_header_type *)
                   ((size_t)(*item_hdr) + size_total);
  }

  return found;
}

/*===========================================================================
FUNCTION      smem_get_partition_sz
===========================================================================*/
/**
  Return the size of the SMEM partition between the specified remote_host and
  the local processor.

  @param[in] remote_host  The remote processor's smem_mem_type.

  @return
  The size of the SMEM partition, if any, shared with remote_host.
  0 if no partition for this edge has been reserved.
*/
/*=========================================================================*/
uint32 smem_get_partition_sz( smem_host_type remote_host )
{
  return smem_info_prt[remote_host].size;
}

