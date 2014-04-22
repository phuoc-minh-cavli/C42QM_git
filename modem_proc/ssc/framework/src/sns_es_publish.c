/*=============================================================================
  @file sns_es_publish.c

  Provides Sensor Instances the ability to publish events, which will be
  temporarily stored by this service, and ultimately delivered to all
  applicable clients.

  This file contains all public and private functions for the allocation
  and publishing of sensor events.

  Copyright (c) 2016-2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/

#include <stdlib.h>
#include "pb_encode.h"
#include "sns_atomic.h"
#include "sns_assert.h"
#include "sns_fw_attribute_service.h"
#include "sns_fw_data_stream.h"
#include "sns_fw_diag_service.h"
#include "sns_fw_event_service.h"
#include "sns_fw_log.h"
#include "sns_fw_sensor.h"
#include "sns_fw_sensor_event.h"
#include "sns_fw_sensor_instance.h"
#include "sns_fw_stream_service.h"
#include "sns_list.h"
#include "sns_island.h"
#include "sns_island_util.h"
#include "sns_math_util.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_printf_int.h"
#include "sns_profiler.h"
#include "sns_pwr_sleep_mgr.h"
#include "sns_rc.h"
#include "sns_sensor_event.h"
#include "sns_thread_manager.h"
#include "sns_time.h"
#include "sns_types.h"

/*============================================================================
  Macro Definitions
  ===========================================================================*/

#define SNS_EVENT_SERVICE_FREE_PATTERN 0xD0
#define SNS_EVENT_SERVICE_DEAD_PATTERN 0xA5

/* When the main buffer has used more than 65% of available space, drop data */
#define SNS_FW_ES_BUFFER_LOW_MEM_CB_THRESH .65f
/* If island is full, how many blocks should be freed before re-using */
#define SNS_EVENT_SERVICE_ISLAND_FULL_DROP_CNT \
  SNS_EVENT_SERVICE_BLOCK_CNT_ISLAND / 10

/*=============================================================================
  Static Data Definitions
  ===========================================================================*/

/* Fast look-up table for bit ranges; don't allow allocations larger than 8 blocks */
// PEND: This ideally would be const (and in .rodata.sns), but then it oddly
// is placed in non-island memory
static sns_bfield bit_range[] SNS_SECTION(".data.sns") =
  {0b1, 0b11, 0b111, 0b1111, 0b11111, 0b111111, 0b1111111, 0b11111111};

static sns_event_service_api event_service_api SNS_SECTION(".data.sns");

static const uint16_t version SNS_SECTION(".rodata.sns") = 1;

/* Pre-allocated event buffer space */
static uint8_t event_buffer_island[SNS_EVENT_SERVICE_BUFFER_ISLAND]
  SNS_SECTION(".data.sns") __attribute__((aligned(8)));
static uint8_t event_buffer[SNS_EVENT_SERVICE_BUFFER]
  __attribute__((aligned(8)));

/* Pre-allocated clusters; referenced to sns_fw_event_service::clusters */
static sns_cluster event_clusters[SNS_EVENT_SERVICE_CLUSTER_CNT];
static sns_cluster event_clusters_island[SNS_EVENT_SERVICE_CLUSTER_CNT_ISLAND]
  SNS_SECTION(".data.sns");

/* Whether a particular block in the buffer is available for use;
 * BIT_SET to mark as available; BIT_CLR to mark in-use */
static sns_bfield available_blocks[SNS_EVENT_SERVICE_BITMAP_LEN];
static sns_bfield available_blocks_island[SNS_EVENT_SERVICE_BITMAP_LEN_ISLAND]
  SNS_SECTION(".data.sns");

/* For T32 debugging purposes */
#if defined(SNS_DEBUG_EVENT_SERVICE)
const int sns_debug_event_service = 1;
#else
const int sns_debug_event_service = 0;
#endif

/*=============================================================================
  Shared Data Definitions - sns_data_stream.c
  ===========================================================================*/

sns_fw_event_service event_service SNS_SECTION(".data.sns");

/*=============================================================================
  Forward Declarations
  ===========================================================================*/

// Implemented in sns_es_low_mem.c
sns_rc sns_es_low_mem_init(sns_fw_event_service *this);

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/* See sns_event_service_api::get_max_event_size */
SNS_SECTION(".text.sns") static uint32_t
get_max_event_size(sns_event_service const *this)
{
  UNUSED_VAR(this);
  return (SNS_EVENT_SERVICE_BLOCK_ALLOC_MAX * SNS_EVENT_SERVICE_BLOCK_SIZE)
    - sizeof(sns_fw_sensor_event);
}

/**
 * Generate a sudo-random number for use by Event Service.
 *
 * @param[i] max Maximum value to return
 */
SNS_SECTION(".text.sns") static inline uint32_t
sns_rand(int max)
{
  static unsigned int rand_seed SNS_SECTION(".data.sns") = 1;

  rand_seed = 1103515245 * rand_seed + 12345;
  return (uint32_t)(rand_seed % max);
}

/**
 * Determine whether the specified range of blocks is available.
 *
 * PEND: Could be optimized/replaced by the BIT_GETR macro.
 *
 * @param[i] blocks Either island or non-island block arrays
 * @param[i] start First block to check
 * @param[i] num How many blocks past the first to check
 *
 * @return True if all are available; false otherwise
 */
SNS_SECTION(".text.sns") static bool
check_range(sns_bfield *blocks, uint32_t start, int num)
{
  UNUSED_VAR(bit_range);
  for(int i = 0; i <= num; i++)
    if(!BIT_GET(blocks, start + i))
      return false;

  return true;
}

/**
 * Find and initialize a new cluster of blocks, for an event of at least
 * size, preferably within island (if flag set).
 *
 * @note The event service lock should be held when calling this function
 *
 * @param[i] this Event Service
 * @param[i] size Length in bytes of the event needing a home
 * @param[i] island Whether to try and find a cluster/blocks in island
 *
 * @return New cluster (never NULL)
 */
SNS_SECTION(".text.sns") static sns_cluster*
new_cluster(sns_fw_event_service *this, uint32_t size, bool island)
{
  sns_cluster *cluster = NULL;
  // size-1 because we only want to allocate one block if size == BLOCK_SIZE
  uint32_t alloc_cnt = ((size - 1) / SNS_EVENT_SERVICE_BLOCK_SIZE) + 1;
  int32_t found_blocks = -1;
  uint32_t cluster_idx = 0;
  sns_es_heap *heap;

  while(-1 == found_blocks)
  {
    uint32_t guess;

    if(island && 0 == this->island_full)
    {
      heap = &this->heaps[SNS_ES_BUFFER_ID_ISLAND];
    }
    else
    {
      SNS_ISLAND_EXIT();
      heap = &this->heaps[SNS_ES_BUFFER_ID_MAIN];
      island = false;
    }

    guess = sns_rand(heap->tot_blk_cnt - alloc_cnt - 1);
    for(int idx = guess + 1;; idx++)
    {
      if(check_range(heap->available_blocks, idx, alloc_cnt - 1))
      {
        found_blocks = idx;
        break;
      }

      if(idx >= heap->tot_blk_cnt - alloc_cnt)
        idx = 0;
      if(idx != guess)
        continue;

      heap->alloc_failures++;
      if(island)
        this->island_full = SNS_EVENT_SERVICE_ISLAND_FULL_DROP_CNT;
      else
        SNS_ASSERT(false);
      break;
    }

    if(-1 == found_blocks)
      continue;

    cluster_idx = sns_rand(heap->tot_cluster_cnt - 1);
    for(int i = 0; 0 != heap->clusters[cluster_idx].ref_cnt; i++)
    {
      if(i == heap->tot_cluster_cnt - 1)
      {
        if(island)
        {
          found_blocks = -1;
          this->island_full = SNS_EVENT_SERVICE_ISLAND_FULL_DROP_CNT;
          break;
        }
        SNS_ASSERT(false);
      }

      if(++cluster_idx >= heap->tot_cluster_cnt)
        cluster_idx = 0;
    }
  }

  cluster = &heap->clusters[cluster_idx];
  cluster->start_idx = found_blocks;
  cluster->blk_len = alloc_cnt - 1;
  cluster->read_off = 0;
  cluster->write_off = 0;
  cluster->active = true;
  cluster->island = island;
  cluster->ref_cnt = 1;

  if(++heap->current_cluster_cnt > heap->max_cluster_cnt)
    heap->max_cluster_cnt = heap->current_cluster_cnt;

  for(int i = 0; i <= cluster->blk_len; i++)
  {
    SNS_ASSERT(cluster->start_idx + i < heap->tot_blk_cnt);
    SNS_ASSERT(BIT_GET(heap->available_blocks, cluster->start_idx + i));
    if(++heap->current_alloc_cnt > heap->max_alloc_cnt)
      heap->max_alloc_cnt = heap->current_alloc_cnt;
    BIT_CLR(heap->available_blocks, cluster->start_idx + i);
  }
#ifdef SNS_DEBUG_EVENT_PRINT
  SNS_PRINTF(HIGH, sns_fw_printf,
      "new_cluster:0x%x, start_idx:%u, blk_len:%u",
      cluster, cluster->start_idx, cluster->blk_len);
#endif
  return cluster;
}

/**
 * Determine the appropriate write index for the allocated event.
 *
 * The current cluster may have been allocated in DDR (if island memory
 * was exhausted), try and move it back to Island if possible.
 *
 * @param[i] this Event Service
 * @param[io] active Current active cluster (if present); modify if changed
 * @param[io] write_idx Block index of where to place event
 * @param[io] available_len Amount of available space (in bytes) in block_idx
 * @param[i] struct_len Allocated event+hdr length
 * @param[i] island Whether the event will be allocated in island
 */
SNS_SECTION(".text.sns") void
get_write_idx(sns_fw_event_service *restrict this,
    sns_cluster **restrict active, int *restrict write_idx,
    size_t *restrict available_len, size_t struct_len, bool island)
{
  if(NULL == *active || !(*active)->active ||
     (island && !(*active)->island && 0 == this->island_full))
  {
    if(NULL != *active)
      sns_es_release_cluster((*active));

    *active = new_cluster(this, struct_len, island);
    *write_idx = (*active)->start_idx;

    // If we just created a new cluster, we for sure have enough space
    *available_len = struct_len;
#ifdef SNS_DEBUG_EVENT_PRINT
    SNS_PRINTF(HIGH, sns_fw_printf,
        "alloc_event, write_idx:%u, read_off:%u, write_off:%u",
        *write_idx, (*active)->read_off, (*active)->write_off);
#endif
  }
  else
  {
    *write_idx = (*active)->start_idx + (*active)->blk_len;

    // There is a special case when write_off is '0'.  It means that the
    // previously allocated event exactly filled the block.  From the client
    // side, we need to differentiate this from the "you haven't read anything"
    // case.  Therefore we "pretend" increase the blk_len, without actually
    // allocating another block.  Therefore we need to tweak our values here
    // in order to undo this "pretend".
    if(0 == (*active)->write_off)
    {
      *available_len = 0;
      --(*write_idx);
    }
    else
      *available_len = SNS_EVENT_SERVICE_BLOCK_SIZE - SNS_BOFF((*active)->write_off);
#ifdef SNS_DEBUG_EVENT_PRINT
      SNS_PRINTF(HIGH, sns_fw_printf,
          "alloc_event, write_idx:%u, start_idx:%u, blk_len:%u, available_len:%u, write_off:%u",
          *write_idx, (*active)->start_idx, (*active)->blk_len,
          *available_len, (*active)->write_off);
#endif
  }
}

/**
 * Check remaining Event Service DDR space, and invoke the low memory handler
 * if threshold is reached.
 */
SNS_SECTION(".text.sns") void
check_free_mem(sns_fw_event_service *this)
{
  if(this->heaps[SNS_ES_BUFFER_ID_MAIN].current_alloc_cnt >
      SNS_EVENT_SERVICE_BLOCK_CNT * SNS_FW_ES_BUFFER_LOW_MEM_CB_THRESH)
    sns_es_handle_low_mem(this);
}

/**
 * Allocate an event from the global buffer.
 *
 * @note The event service lock should be held when calling this function
 *
 * @param[i] this Event Service
 * @param[io] cluster Cluster on which this event should be allocated; or new
 * @param[i] island Whether island memory is preferred for cluster allocation
 * @param[i] event_len Length of the event-specific payload
 *
 * See sns_event_service_api::alloc_event
 */
SNS_SECTION(".text.sns") static sns_fw_sensor_event*
alloc_event(sns_fw_event_service *this, sns_cluster **cluster, bool island,
    uint32_t event_len)
{
  SNS_PROFILE(SNS_EVENT_SERVICE_ALLOC_EVENT_START, 0);
  sns_fw_sensor_event *fw_event = NULL;
  size_t struct_len = sizeof(*fw_event) - sizeof(fw_event->event.event) + event_len;
  sns_cluster *active = *cluster;
  int write_idx;
  size_t available_len;
  sns_es_heap *heap;

  // Align to 8-byte boundary (to ensure events are properly aligned in array)
  struct_len = ALIGN_8(struct_len);
  if(!sns_island_is_island_ptr((intptr_t)active))
    SNS_ISLAND_EXIT();

  sns_osa_lock_acquire(&this->lock);
  get_write_idx(this, &active, &write_idx, &available_len, struct_len, island);
  SNS_ASSERT(NULL != active);
  heap = active->island
        ? &this->heaps[SNS_ES_BUFFER_ID_ISLAND]
        : &this->heaps[SNS_ES_BUFFER_ID_MAIN];

  if(available_len < struct_len)
  {
    int needed_blk = ((struct_len - available_len - 1)
        / SNS_EVENT_SERVICE_BLOCK_SIZE) + 1;

    if(write_idx + needed_blk < heap->tot_blk_cnt &&  // Don't exceed heap
       active->blk_len < UINT8_MAX - needed_blk && // Don't allow blk_len to wrap around
       check_range(heap->available_blocks, write_idx + 1, needed_blk - 1))
    {
      if(0 == available_len)
      {
        --active->blk_len;
        ++write_idx;
      }

      for(int i = 0; i < needed_blk; i++)
      {
        ++active->blk_len;
        SNS_ASSERT(active->start_idx + active->blk_len < heap->tot_blk_cnt);
        SNS_ASSERT(BIT_GET(heap->available_blocks, active->start_idx + active->blk_len));
        if(++heap->current_alloc_cnt > heap->max_alloc_cnt)
          heap->max_alloc_cnt = heap->current_alloc_cnt;
        BIT_CLR(heap->available_blocks, active->start_idx + active->blk_len);
      }
    }
    else
    {
      sns_es_release_cluster(active);
      active = new_cluster(this, struct_len, island);
      write_idx = active->start_idx;

      heap = active->island
      ? &this->heaps[SNS_ES_BUFFER_ID_ISLAND]
      : &this->heaps[SNS_ES_BUFFER_ID_MAIN];
    }
  }

  fw_event = (sns_fw_sensor_event*)
    ((uintptr_t)&heap->buffer[SNS_BIDX(write_idx)] + SNS_BOFF(active->write_off));
  SNS_BOFF_INC(active->write_off, struct_len);

  // See comment above
  if(0 == active->write_off)
    active->blk_len++;
  if(heap->max_size < struct_len)
    heap->max_size = struct_len;

  check_free_mem(this);

  fw_event->header.client_cnt = INT16_MAX;
  fw_event->header.fw_event_len = struct_len;
  fw_event->event.event_len = event_len;
#ifdef SNS_DEBUG_EVENT_SERVICE
  fw_event->header.headcode = 0xABCDABCD;
  fw_event->header.caller = ((intptr_t)__builtin_return_address(2));
#endif

  SNS_PROFILE(SNS_EVENT_SERVICE_ALLOC_EVENT_END, 0);

#ifdef SNS_DEBUG_EVENT_PRINT
  SNS_PRINTF(HIGH, sns_fw_printf, "alloc_event, cluster:0x%x, start_idx:%u, blk_len:%u, readoff:%u, writeoff:%u,refcnt:%u,",
    active, active->start_idx, active->blk_len, active->read_off, active->write_off, active->ref_cnt);
#endif
  *cluster = active;
  sns_osa_lock_release(&this->lock);

  return fw_event;
}

/* See sns_event_service_api::alloc_event */
SNS_SECTION(".text.sns") static sns_sensor_event*
alloc_event_for_instance(sns_event_service *this, sns_sensor_instance *instance,
    uint32_t event_len)
{
  UNUSED_VAR(this);
  sns_fw_sensor_instance *fw_instance = (sns_fw_sensor_instance*)instance;
  sns_sensor_event *event = NULL;
  bool island = SNS_ISLAND_STATE_NOT_IN_ISLAND != fw_instance->island_operation;

  if(NULL != fw_instance->unpublished_event)
  {
    if(!sns_island_is_island_ptr((intptr_t)fw_instance->unpublished_event))
      SNS_ISLAND_EXIT();
    sns_free(fw_instance->unpublished_event);
  }

  if(event_len < get_max_event_size(this))
  {
    if(island)
    {
      event = sns_malloc(SNS_HEAP_ISLAND, sizeof(*event) + event_len);
    }
    if(NULL == event)
    {
      SNS_ISLAND_EXIT();
      event = sns_malloc(SNS_HEAP_MAIN, sizeof(*event) + event_len);
    }
    SNS_ASSERT(NULL != event);

    event->event_len = event_len;
    fw_instance->unpublished_event = event;
  }

  return event;
}

/**
 * Allocate and initialize a new sns_cluster_list for the given data stream.
 *
 * @param[i] stream Add the created cluster list to the end of this stream
 */
SNS_SECTION(".text.sns") static sns_cluster_list*
add_cluster_list(sns_fw_data_stream *stream)
{
  sns_cluster_list *rv = NULL;
  sns_list_iter iter;
  bool island_operation = (SNS_ISLAND_STATE_NOT_IN_ISLAND != stream->island_operation);

  if(island_operation)
  {
    rv = sns_malloc(SNS_HEAP_ISLAND, sizeof(*rv));
  }
  if(NULL == rv)
  {
    SNS_ISLAND_EXIT();
    rv = sns_malloc(SNS_HEAP_MAIN, sizeof(*rv));

    if(SNS_ISLAND_STATE_IN_ISLAND == stream->island_operation)
      stream->island_operation = SNS_ISLAND_STATE_ISLAND_DISABLED;
  }
  SNS_ASSERT(NULL != rv);

  sns_list_item_init(&rv->list_entry, rv);
  sns_list_iter_init(&iter, &stream->clusters, false);
  sns_list_iter_insert(&iter, &rv->list_entry, true);

  rv->read_idx = rv->write_idx = 0;

  return rv;
}

/**
 * Publish a data cluster to the given stream. Increment the event_cnt by one.
 *
 * @param[i] stream Stream on which to publish the event
 * @param[i] active Active cluster on which this event resides
 */
SNS_SECTION(".text.sns") static void
publish_cluster(sns_fw_data_stream *stream, sns_cluster *active)
{
  bool inc_ref = false;
  sns_list_iter iter;
  sns_cluster_list *cluster_list;

  SNS_ASSERT(sns_data_stream_validate(stream));
  sns_osa_lock_acquire(&stream->stream_lock);

  ++stream->event_cnt;

  cluster_list = sns_list_item_get_data(
      sns_list_iter_init(&iter, &stream->clusters, false));

  if(!sns_island_is_island_ptr((intptr_t)active) &&
     SNS_ISLAND_STATE_IN_ISLAND == stream->island_operation)
    stream->island_operation = SNS_ISLAND_STATE_ISLAND_DISABLED;

  if(NULL == cluster_list)
  {
    cluster_list = add_cluster_list(stream);
    cluster_list->clusters[cluster_list->write_idx] = active;
    inc_ref = true;
  }
  else if(cluster_list->clusters[cluster_list->write_idx] != active)
  {
    // Usually alloc_event will mark previous cluster as inactive, but
    // other code paths may not.  No references are removed here, so no
    // need to call release
    if(NULL != cluster_list->clusters[cluster_list->write_idx])
      cluster_list->clusters[cluster_list->write_idx]->active = false;

    if(SNS_DATA_STREAM_CLUSTER_CNT == cluster_list->write_idx + 1)
      cluster_list = add_cluster_list(stream);
    else
      ++cluster_list->write_idx;

    cluster_list->clusters[cluster_list->write_idx] = active;
    inc_ref = true;
  }
  SNS_ASSERT(cluster_list->write_idx >= cluster_list->read_idx);
#ifdef SNS_DEBUG_EVENT_PRINT
  SNS_PRINTF(HIGH, sns_fw_printf,
      "publish cluster, stream:0x%x, blk_len:%u, current cluster0x%x, active:0x%x",
    stream, cluster_list->clusters[cluster_list->write_idx]->blk_len,
    cluster_list->clusters[cluster_list->write_idx], active);
#endif

  sns_es_add_pending_event(stream);
  sns_osa_lock_release(&stream->stream_lock);

  if(inc_ref)
  {
    sns_osa_lock_acquire(&event_service.lock);
    ++active->ref_cnt;
    sns_osa_lock_release(&event_service.lock);
  }
}

/**
 * Publish an event to all applicable client data streams.
 *
 * @param[i] this
 * @param[i] instance Sensor Instance which produced the event
 * @param[i] req_list SUID-specific list of clients to publish to
 * @param[i] event Event to be published
 */
SNS_SECTION(".text.sns") static void
publish_req_list(sns_fw_event_service *this, sns_fw_sensor_instance *instance,
    sns_client_req_list *req_list, sns_sensor_event *event)
{
  uint32_t client_cnt;
  sns_isafe_list_iter iter;
  sns_isafe_list_item *req_item =
    sns_isafe_list_iter_init(&iter, &req_list->client_requests, true);

  client_cnt = sns_isafe_list_iter_len(&iter);
  if(0 != client_cnt)
  {
    sns_fw_sensor_event *fw_event = NULL;
    bool island = SNS_ISLAND_STATE_NOT_IN_ISLAND != instance->island_operation;

    fw_event = alloc_event(this, &req_list->active_cluster, island, event->event_len);

    fw_event->header.client_cnt = client_cnt;
    sns_memscpy(fw_event->event.event, event->event_len, &event->event, event->event_len);
    fw_event->event.timestamp = event->timestamp;
    fw_event->event.message_id = event->message_id;

    for(; NULL != req_item;
        req_item = sns_isafe_list_iter_advance(&iter))
    {
      sns_fw_data_stream *stream = (sns_fw_data_stream*)
        sns_isafe_list_item_get_data(req_item);

      if(!sns_island_is_island_ptr((intptr_t)stream) ||
         SNS_ISLAND_STATE_IN_ISLAND != stream->island_operation)
        SNS_ISLAND_EXIT();

      SNS_PROFILE(SNS_EVENT_SERVICE_PUBLISH_EVENT_TO_CLIENT_START, 2,
          stream, (NULL == stream->dst_sensor) ?
          (void*)stream->dst_instance : (void*)stream->dst_sensor);

      publish_cluster(stream, req_list->active_cluster);
#ifdef SNS_DEBUG_EVENT_PRINT
      SNS_PRINTF(HIGH, sns_fw_printf, "Publish_req_list, stream:0x%x, event:0x%x, cluster:0x%x",
        stream, fw_event, req_list->active_cluster);
#endif
#ifdef SNS_EVENT_SERVICE_VERBOSE_LOG
      SNS_PRINTF(LOW, sns_fw_printf, "Event by Instance " SNS_DIAG_PTR
          " for Stream " SNS_DIAG_PTR " (size %i)",
          instance, stream, event->event_len);
#endif

      SNS_PROFILE(SNS_EVENT_SERVICE_PUBLISH_EVENT_TO_CLIENT_END, 0);
    }
  }
}

/* See sns_event_service_api::publish_event_suid */
SNS_SECTION(".text.sns") static sns_rc
publish_event(sns_event_service *this, sns_sensor_instance *instance,
              sns_sensor_event *event, sns_sensor_uid const *sensor_uid)
{
  SNS_PROFILE(SNS_EVENT_SERVICE_PUBLISH_EVENT_START, 0);
  sns_fw_sensor_instance *fw_instance = (sns_fw_sensor_instance*)instance;

  if(fw_instance->unpublished_event != event)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Illegal event published");
  }
  else
  {
    sns_fw_sensor *fw_sensor = (sns_fw_sensor*)fw_instance->sensor;
    sns_isafe_list_iter iter;

    sns_pwr_sleep_mgr_check_config_event(sensor_uid, instance, event,
        sns_attr_info_get_is_physical_sensor(fw_sensor->attr_info));

    // An instance may produce multiple data types; find the correct SUID
    for(sns_isafe_list_item *req_list_item =
          sns_isafe_list_iter_init(&iter, &fw_instance->client_req_lists, true);
        NULL != req_list_item;
        req_list_item = sns_isafe_list_iter_advance(&iter))
    {
      sns_client_req_list *req_list = (sns_client_req_list*)
        sns_isafe_list_item_get_data(req_list_item);

      if(NULL == sensor_uid || sns_sensor_uid_compare(req_list->suid, sensor_uid))
      {
        // We're supposed to log the event before we send it.  So we have to
        // determine whether it's likely to be sent.  Mostly unnecessary work.
        sns_isafe_list_iter iter;
        sns_isafe_list_iter_init(&iter, &req_list->client_requests, true);
        if(0 != sns_isafe_list_iter_len(&iter))
          sns_fw_log_sensor_inst_api_event(instance, sensor_uid, event);

        publish_req_list((sns_fw_event_service*)this, fw_instance, req_list, event);
        if(NULL != sensor_uid)
          break;
      }
    }

    fw_instance->unpublished_event = NULL;
    sns_free(event);
  }

  SNS_PROFILE(SNS_EVENT_SERVICE_PUBLISH_EVENT_END, 0);
  return SNS_RC_SUCCESS;
}

/* See sns_event_service_api::publish_error */
SNS_SECTION(".text.sns") static sns_rc
publish_error(sns_event_service *this, sns_sensor_instance *instance,
    sns_rc reason)
{
  pb_ostream_t stream;
  sns_sensor_event *event;
  sns_std_error_event error_event = sns_std_error_event_init_default;

  SNS_PRINTF(MED, sns_fw_printf, "Publishing error %i for instance " SNS_DIAG_PTR,
      reason, instance);

  error_event.error = (sns_std_error)reason;
  event = alloc_event_for_instance(this, instance, sns_std_error_event_size);
  event->message_id = SNS_STD_MSGID_SNS_STD_ERROR_EVENT;
  event->timestamp = sns_get_system_time();

  stream = pb_ostream_from_buffer((pb_byte_t*)event->event, event->event_len);
  if(!pb_encode(&stream, sns_std_error_event_fields, &error_event))
    SNS_ASSERT(false);

  publish_event(this, instance, event, NULL);

  return SNS_RC_SUCCESS;
}

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

sns_fw_event_service*
sns_event_service_init(void)
{
  sns_osa_lock_attr attr;
  sns_rc rc;
  event_service_api = (sns_event_service_api)
  {
    .struct_len = sizeof(event_service_api),
    .get_max_event_size = &get_max_event_size,
    .alloc_event = &alloc_event_for_instance,
    .publish_event = &publish_event,
    .publish_error = &publish_error
  };

  event_service.service.service.version = version;
  event_service.service.service.type = SNS_EVENT_SERVICE;
  event_service.service.api = &event_service_api;

  rc = sns_osa_lock_attr_init(&attr);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_lock_attr_set_memory_partition(&attr, SNS_OSA_MEM_TYPE_ISLAND);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_lock_init(&attr, &event_service.lock);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);

  sns_memset(event_buffer_island, SNS_EVENT_SERVICE_FREE_PATTERN,
      sizeof(event_buffer_island));
  sns_memset(event_buffer, SNS_EVENT_SERVICE_FREE_PATTERN,
      sizeof(event_buffer));

  sns_memset(available_blocks, -1, sizeof(available_blocks));
  sns_memset(available_blocks_island, -1, sizeof(available_blocks_island));

  sns_memzero(event_service.heaps, sizeof(event_service.heaps));
  event_service.heaps[SNS_ES_BUFFER_ID_MAIN].buffer = event_buffer;
  event_service.heaps[SNS_ES_BUFFER_ID_ISLAND].buffer = event_buffer_island;
  event_service.heaps[SNS_ES_BUFFER_ID_MAIN].clusters = event_clusters;
  event_service.heaps[SNS_ES_BUFFER_ID_ISLAND].clusters = event_clusters_island;
  event_service.heaps[SNS_ES_BUFFER_ID_MAIN].available_blocks = available_blocks;
  event_service.heaps[SNS_ES_BUFFER_ID_ISLAND].available_blocks = available_blocks_island;
  event_service.heaps[SNS_ES_BUFFER_ID_MAIN].tot_blk_cnt = SNS_EVENT_SERVICE_BLOCK_CNT;
  event_service.heaps[SNS_ES_BUFFER_ID_ISLAND].tot_blk_cnt = SNS_EVENT_SERVICE_BLOCK_CNT_ISLAND;
  event_service.heaps[SNS_ES_BUFFER_ID_MAIN].tot_cluster_cnt = SNS_EVENT_SERVICE_CLUSTER_CNT;
  event_service.heaps[SNS_ES_BUFFER_ID_ISLAND].tot_cluster_cnt = SNS_EVENT_SERVICE_CLUSTER_CNT_ISLAND;

  event_service.island_client = sns_island_aggregator_register_client("event service");
  sns_es_low_mem_init(&event_service);

  return &event_service;
}

SNS_SECTION(".text.sns") sns_rc
sns_es_publish_error(struct sns_fw_sensor_instance *instance,
    sns_rc reason)
{
  return publish_error((sns_event_service*)&event_service,
      (sns_sensor_instance*)instance, reason);
}

SNS_SECTION(".text.sns") sns_rc
sns_es_send_event(sns_fw_data_stream *stream, uint32_t message_id,
  uint32_t buffer_len, uint8_t *buffer)
{
  sns_fw_event_service *this = (sns_fw_event_service*)&event_service;
  sns_cluster *active = NULL;
  sns_fw_sensor_event *fw_event;
  int removing = 0;
  bool island = SNS_ISLAND_STATE_NOT_IN_ISLAND != stream->island_operation;

  // Since this event is sent only to a single client, we need to create
  // a new cluster for this purpose
  fw_event = alloc_event(&event_service, &active, island, buffer_len);

  sns_osa_lock_acquire(&stream->stream_lock);
  removing = stream->removing;
  sns_osa_lock_release(&stream->stream_lock);

  fw_event->event.message_id = message_id;
  fw_event->event.timestamp = sns_get_system_time();
  sns_memscpy(fw_event->event.event, buffer_len, buffer, buffer_len);
  fw_event->header.client_cnt = 1;

  publish_cluster(stream, active);
  if(SNS_DATA_STREAM_AVAILABLE == removing)
    sns_fw_log_sensor_api_event(stream, &fw_event->event);

  sns_osa_lock_acquire(&this->lock);
  // Since we never "saved" this cluster within the sensor instance, we need
  // to be sure to release that "unused" reference here
  sns_es_release_cluster(active);
  sns_osa_lock_release(&this->lock);

  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") sns_rc
sns_es_send_error_event(sns_fw_data_stream *stream, sns_rc reason)
{
  sns_rc rv = SNS_RC_FAILED;
  sns_std_error_event event = sns_std_error_event_init_default;
  uint8_t buffer[sns_std_error_event_size];
  pb_ostream_t ostream = pb_ostream_from_buffer(buffer, sns_std_error_event_size);

  event.error = (sns_std_error)reason;

  if(pb_encode(&ostream, sns_std_error_event_fields, &event))
  {
    rv = sns_es_send_event(stream, SNS_STD_MSGID_SNS_STD_ERROR_EVENT,
        sns_std_error_event_size, buffer);
  }
  else
  {
    SNS_SPRINTF(ERROR, sns_fw_printf, "Error encoding event: %s",
        PB_GET_ERROR(&ostream));
  }
  return rv;
}

SNS_SECTION(".text.sns") void
sns_es_deinit_instance(struct sns_fw_sensor_instance *instance)
{
  if(NULL != instance->unpublished_event)
  {
    if(!sns_island_is_island_ptr((intptr_t)instance->unpublished_event))
      SNS_ISLAND_EXIT();
    sns_free(instance->unpublished_event);
  }
}

SNS_SECTION(".text.sns") void
sns_es_release_cluster(sns_cluster *cluster)
{
  if(NULL != cluster)
  {
    sns_osa_lock_acquire(&event_service.lock);
    cluster->active = false;
#ifdef SNS_DEBUG_EVENT_PRINT
    SNS_PRINTF(HIGH, sns_fw_printf, "release cluster:0x%x, ref_cnt:%u", cluster, cluster->ref_cnt);
#endif
    if(1 == cluster->ref_cnt)
    {
      sns_es_heap *heap = cluster->island
        ? &event_service.heaps[SNS_ES_BUFFER_ID_ISLAND]
        : &event_service.heaps[SNS_ES_BUFFER_ID_MAIN];

      --heap->current_cluster_cnt;
      SNS_ASSERT((0 == cluster->blk_len && cluster->read_off == cluster->write_off) ||
                 (1 == cluster->blk_len && 0 == cluster->write_off));

      // If write_off is zero, then the last block was exactly filled
      // And hence would have been freed as part of consume().  So we
      // shouldn't attempt to set again here
      if(0 != cluster->write_off)
      {
        SNS_ASSERT(cluster->start_idx < heap->tot_blk_cnt);
        SNS_ASSERT(!BIT_GET(heap->available_blocks, cluster->start_idx));
        --heap->current_alloc_cnt;
        BIT_SET(heap->available_blocks, cluster->start_idx);
      }
    }
    --cluster->ref_cnt;
    sns_osa_lock_release(&event_service.lock);
  }
}

SNS_SECTION(".text.sns") void
sns_es_consume(sns_cluster *cluster, sns_fw_sensor_event *event)
{
  sns_osa_lock_acquire(&event_service.lock);
  --event->header.client_cnt;
  if(0 == event->header.client_cnt)
  {
    sns_es_heap *heap = cluster->island
      ? &event_service.heaps[SNS_ES_BUFFER_ID_ISLAND]
      : &event_service.heaps[SNS_ES_BUFFER_ID_MAIN];

    for(int i = SNS_BOFF(cluster->read_off) + event->header.fw_event_len;
        i >= SNS_EVENT_SERVICE_BLOCK_SIZE && cluster->blk_len > 0;
        // Always leave the final block on the cluster
        // Note that if write_off == 0, last block isn't actually "owned" by this cluster
        i -= SNS_EVENT_SERVICE_BLOCK_SIZE)
    {
#ifdef SNS_DEBUG_EVENT_PRINT
      SNS_PRINTF(HIGH, sns_fw_printf, "Consume event, cluster:0x%x, start_idx:%u, blk_len:%u",
        cluster, cluster->start_idx, cluster->blk_len);
#endif
      SNS_ASSERT(cluster->start_idx < heap->tot_blk_cnt);
      SNS_ASSERT(!BIT_GET(heap->available_blocks, cluster->start_idx));
      --heap->current_alloc_cnt;
      BIT_SET(heap->available_blocks, cluster->start_idx);
      ++cluster->start_idx;
      --cluster->blk_len;

      if(0 != event_service.island_full && cluster->island)
        --event_service.island_full;
    }

    // PEND: Last block will not be freed; could be optimized
#ifdef SNS_DEBUG_EVENT_PRINT
    SNS_PRINTF(HIGH, sns_fw_printf, "Consume event, cluster read off: %u, event len:%u",
        cluster->read_off, event->header.fw_event_len);
#endif
    SNS_BOFF_INC(cluster->read_off, event->header.fw_event_len);
  }
  sns_osa_lock_release(&event_service.lock);
}
