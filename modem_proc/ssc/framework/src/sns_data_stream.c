/*=============================================================================
  @file sns_data_stream.c

  Copyright (c) 2016-2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Id: //components/rel/ssc.slpi/5.1/framework/src/sns_data_stream.c#2 $
  $DateTime: 2019/11/25 00:52:24 $
  $Change: 21546044 $
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <stdlib.h>
#include "sns_assert.h"
#include "sns_fw.pb.h"
#include "sns_fw_data_stream.h"
#include "sns_fw_event_service.h"
#include "sns_fw_sensor.h"
#include "sns_fw_sensor_event.h"
#include "sns_fw_sensor_instance.h"
#include "sns_fw_stream_service.h"
#include "sns_island.h"
#include "sns_island_util.h"
#include "sns_mem_util.h"
#include "sns_memmgr.h"
#include "sns_pb_util.h"
#include "sns_printf_int.h"
#include "sns_profiler.h"
#include "sns_rc.h"
#include "sns_sensor.h"
#include "sns_sensor_instance.h"
#include "sns_sensor_util.h"
#include "sns_service.h"
#include "sns_service_manager.h"
#include "sns_std.pb.h"
#include "sns_types.h"

/*=============================================================================
  Shared Data Definitions - sns_event_service.c
  ===========================================================================*/

extern sns_fw_event_service event_service;

/*=============================================================================
  Static Data Definitions
  ===========================================================================*/

// Constant value used for validating data streams.
static const uint32_t stream_service_magic_number SNS_SECTION(".rodata.sns") = 0xABCDABCD;

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/**
 * Add integrity checksum to stream
 *
 * @param[i] stream Stream to which checksum will be added
 */
SNS_SECTION(".text.sns") static void
data_stream_add_checksum(sns_fw_data_stream *stream)
{
  if(NULL != stream)
  {
    stream->stream_integrity = (stream_service_magic_number ^
         (uint32_t)(uintptr_t)stream->data_source    ^
         (uint32_t)(uintptr_t)stream->dst_instance   ^
         (uint32_t)(uintptr_t)stream->dst_sensor     ^
         (uint32_t)(uintptr_t)stream);
  }
}

/*=============================================================================
  Data Stream API Function Definitions
  ===========================================================================*/

SNS_SECTION(".text.sns") static sns_rc
send_request(sns_data_stream *data_stream,sns_request *request)
{
  sns_fw_data_stream *stream = (sns_fw_data_stream*)data_stream;

  if(sns_data_stream_validate(stream))
    sns_stream_service_add_request(stream, request);
  else
    SNS_PRINTF(ERROR, sns_fw_printf,
        "send_request: Invalid stream 0x"SNS_DIAG_PTR" (req %i)",
        stream, request->message_id);

  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") static sns_rc
initiate_flush(sns_data_stream *data_stream)
{
  sns_fw_data_stream *stream = (sns_fw_data_stream*)data_stream;
  sns_rc rv = SNS_RC_INVALID_VALUE;

  if(sns_data_stream_validate(stream))
    rv = sns_data_stream_initiate_flush(stream);
  else
    SNS_PRINTF(ERROR, sns_fw_printf,
        "initiate_flush: Invalid stream 0x"SNS_DIAG_PTR, stream);

  return rv;
}

SNS_SECTION(".text.sns") static sns_sensor_event*
peek_input(sns_data_stream *data_stream)
{
  SNS_PROFILE(SNS_STREAM_SERVICE_PEEK_INPUT_START, 0);
  sns_fw_data_stream *stream = (sns_fw_data_stream*)data_stream;
  sns_fw_sensor_event *event = NULL;

  if(sns_data_stream_validate(stream))
  {
    if(!sns_es_low_mem_active())
      event = sns_data_stream_peek_input(stream);
    else if(0 != sns_data_stream_get_input_cnt(stream))
      sns_es_add_pending_event(stream);
  }
  else
  {
    SNS_PRINTF(ERROR, sns_fw_printf,
        "peek_input: Invalid stream 0x"SNS_DIAG_PTR, stream);
  }

  SNS_PROFILE(SNS_STREAM_SERVICE_PEEK_INPUT_END, 0);
  return (NULL == event) ? NULL : &event->event;
}

SNS_SECTION(".text.sns") static sns_sensor_event*
get_next_input(sns_data_stream *data_stream)
{
  SNS_PROFILE(SNS_STREAM_SERVICE_GET_NEXT_INPUT_START, 0);
  sns_fw_data_stream *stream = (sns_fw_data_stream*)data_stream;
  sns_fw_sensor_event *event = NULL;

  if(sns_data_stream_validate(stream))
  {
    if(!sns_es_low_mem_active())
      event = sns_data_stream_get_next_input(stream);
    else if(0 != sns_data_stream_get_input_cnt(stream))
      sns_es_add_pending_event(stream);
  }
  else
  {
    SNS_PRINTF(ERROR, sns_fw_printf,
        "get_next_input: Invalid stream 0x"SNS_DIAG_PTR, stream);
  }

  SNS_PROFILE(SNS_STREAM_SERVICE_GET_NEXT_INPUT_END, 0);
  return (NULL == event) ? NULL : &event->event;
}

SNS_SECTION(".text.sns") static uint32_t
get_input_cnt(sns_data_stream *data_stream)
{
  sns_fw_data_stream *stream = (sns_fw_data_stream*)data_stream;
  uint32_t event_cnt = 0;

  if(sns_data_stream_validate(stream))
  {
    event_cnt = sns_data_stream_get_input_cnt(stream);
    if(sns_es_low_mem_active() && 0 < event_cnt)
    {
      event_cnt = 0;
      sns_es_add_pending_event(stream);
    }
  }
  else
  {
    SNS_PRINTF(ERROR, sns_fw_printf,
        "get_input_cnt: Invalid stream 0x"SNS_DIAG_PTR, stream);
  }
  return event_cnt;
}

/*=============================================================================
  Data Stream API Definition
  ===========================================================================*/

static sns_data_stream_api const data_stream_api SNS_SECTION(".data.sns") =
  {
    .struct_len = sizeof(data_stream_api),
    .send_request = &send_request,
    .initiate_flush = &initiate_flush,
    .peek_input = &peek_input,
    .get_next_input = &get_next_input,
    .get_input_cnt = &get_input_cnt
  };

/*=============================================================================
  Internal Data Stream API Implementation
  ===========================================================================*/

/**
 * For a Stream in the SNS_ISLAND_STATE_ISLAND_DISABLED state,
 * check whether there is any non-island state which should restrict its
 * re-entry into island.  If not, update state to SNS_ISLAND_STATE_IN_ISLAND.
 *
 * PEND: If only issue is the cluster_list, consider "fixing" that here
 */
static void
update_stream_island(sns_fw_data_stream *stream)
{
  sns_list_iter iter;
  sns_list_item *curr_item = NULL;

  for(curr_item = sns_list_iter_init(&iter, &stream->clusters, true);
      NULL != curr_item;
      curr_item = sns_list_iter_advance(&iter))
  {
    sns_cluster_list *cluster_list = (sns_cluster_list*)
      sns_list_item_get_data(curr_item);

    if(!sns_island_is_island_ptr((intptr_t)cluster_list))
      return;

    for(int i = cluster_list->read_idx; i <= cluster_list->write_idx; i++)
      if(!sns_island_is_island_ptr((intptr_t)cluster_list->clusters[i]))
        return;
  }

  stream->island_operation = SNS_ISLAND_STATE_IN_ISLAND;
}

/**
 * Get the next cluster to be used for processing by this data stream.
 *
 * @param[i] stream
 *
 * @return Cluster to be processed; NULL if none available
 */
SNS_SECTION(".text.sns") sns_cluster*
get_cluster(sns_fw_data_stream *stream)
{
  sns_cluster_list *list;
  sns_cluster *cluster = NULL;

  sns_osa_lock_acquire(&event_service.lock);
  if(SNS_ISLAND_STATE_IN_ISLAND != stream->island_operation)
    SNS_ISLAND_EXIT();

  for(;;)
  {
    sns_list_iter iter;

    list = sns_list_item_get_data(
        sns_list_iter_init(&iter, &stream->clusters, true));

    if(NULL == list || list->read_idx > list->write_idx)
    {
      cluster = NULL;
      break;
    }

    cluster = list->clusters[list->read_idx];
    SNS_ASSERT(NULL != cluster);

    // UINT16_MAX implies that previous cluster has been consumed
    if(UINT16_MAX == stream->block_idx)
    {
      stream->block_idx = cluster->start_idx;
      stream->read_off = cluster->read_off;
#ifdef SNS_DEBUG_EVENT_PRINT
      SNS_PRINTF(HIGH, sns_fw_printf, "reset read off, stream off:%u, blk_idx:%u",
        stream->read_off, stream->block_idx);
#endif
    }

    if(!cluster->active && stream->block_idx == cluster->start_idx + cluster->blk_len &&
       stream->read_off == cluster->write_off)
    {
      stream->block_idx = UINT16_MAX;

      // After we call release, accesses to old clusters are illegal
      list->clusters[list->read_idx] = NULL;
      sns_es_release_cluster(cluster);

      ++list->read_idx;
      if(SNS_DATA_STREAM_CLUSTER_CNT == list->read_idx)
      {
        if(list->read_idx > list->write_idx && 1 == sns_list_iter_len(&iter))
        {
          // Try and continue to re-use a cluster-list if possible
          list->read_idx = 1;
          list->write_idx = 0;
        }
        else
        {
          sns_list_iter_remove(&iter);
          sns_free(list);
        }
      }

      if(SNS_ISLAND_STATE_ISLAND_DISABLED == stream->island_operation)
        update_stream_island(stream);
      continue;
    }

    break;
  }

  if(NULL != cluster && 0 != stream->event_cnt)
    SNS_ASSERT(stream->block_idx != cluster->start_idx + cluster->blk_len ||
      stream->read_off != cluster->write_off);

  sns_osa_lock_release(&event_service.lock);
  return cluster;
}

SNS_SECTION(".text.sns") sns_fw_sensor_event*
sns_data_stream_peek_input(sns_fw_data_stream *stream)
{
  sns_fw_sensor_event *event = NULL;

  sns_osa_lock_acquire(&stream->stream_lock);
  if(0 != stream->event_cnt)
  {
    sns_cluster *cluster;
    sns_es_heap *heap;

    cluster = get_cluster(stream);
    SNS_ASSERT(NULL != cluster);

    heap = cluster->island
      ? &event_service.heaps[SNS_ES_BUFFER_ID_ISLAND]
      : &event_service.heaps[SNS_ES_BUFFER_ID_MAIN];
    event = (sns_fw_sensor_event*)
      ((uintptr_t)&heap->buffer[SNS_BIDX(stream->block_idx)] + SNS_BOFF(stream->read_off));

#ifdef SNS_DEBUG_EVENT_SERVICE
    SNS_ASSERT(0xABCDABCD == event->header.headcode);
#endif
  }
  sns_osa_lock_release(&stream->stream_lock);

  return event;
}

SNS_SECTION(".text.sns") sns_fw_sensor_event*
sns_data_stream_get_next_input(sns_fw_data_stream *stream)
{
  sns_fw_sensor_event *event = NULL;

  sns_osa_lock_acquire(&stream->stream_lock);
  if(0 != stream->event_cnt)
  {
    sns_cluster *cluster;
    sns_es_heap *heap;

    cluster = get_cluster(stream);
    SNS_ASSERT(NULL != cluster);

    heap = cluster->island
      ? &event_service.heaps[SNS_ES_BUFFER_ID_ISLAND]
      : &event_service.heaps[SNS_ES_BUFFER_ID_MAIN];
    event = (sns_fw_sensor_event*)
      ((uintptr_t)&heap->buffer[SNS_BIDX(stream->block_idx)] + SNS_BOFF(stream->read_off));

#ifdef SNS_DEBUG_EVENT_SERVICE
    SNS_ASSERT(0xABCDABCD == event->header.headcode);
#endif

    // Update block_idx and read_off to reflect event consumption
    for(int i = SNS_BOFF(stream->read_off) + event->header.fw_event_len;
        i >= SNS_EVENT_SERVICE_BLOCK_SIZE;
        i -= SNS_EVENT_SERVICE_BLOCK_SIZE)
    {
      ++stream->block_idx;
    }
    --stream->event_cnt;
    SNS_BOFF_INC(stream->read_off, event->header.fw_event_len);
#ifdef SNS_DEBUG_EVENT_PRINT
    SNS_PRINTF(HIGH, sns_fw_printf,"stream get next input, blk_idx:%u, event cnt:%u, read off:%u",
        stream->block_idx, stream->event_cnt, stream->read_off);
#endif
    sns_es_consume(cluster, event);
    // Calling again here just to remove cluster (if empty)
    get_cluster(stream);
  }

  sns_osa_lock_release(&stream->stream_lock);

  return sns_data_stream_peek_input(stream);
}

SNS_SECTION(".text.sns") sns_rc
sns_data_stream_initiate_flush(sns_fw_data_stream *stream)
{
  sns_request request;
  uint8_t buffer[20]; // Dependent on size of sns_std_request
  uint32_t encoded_len;

  if(!sns_island_is_island_ptr((intptr_t)stream) ||
      SNS_ISLAND_STATE_IN_ISLAND != stream->island_operation)
    SNS_ISLAND_EXIT();

  encoded_len = pb_encode_request(buffer, sizeof(buffer), NULL, NULL, NULL);
  request.request = buffer;
  request.request_len = encoded_len;
  request.message_id = SNS_STD_MSGID_SNS_STD_FLUSH_REQ;
  sns_stream_service_add_request(stream, &request);

  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") uint32_t
sns_data_stream_get_input_cnt(sns_fw_data_stream *stream)
{
  uint32_t event_cnt = 0;

  sns_osa_lock_acquire(&stream->stream_lock);
  event_cnt = stream->event_cnt;
  sns_osa_lock_release(&stream->stream_lock);

  return event_cnt;
}

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

sns_rc
data_stream_init(struct sns_fw_sensor *dst_sensor,
    struct sns_fw_sensor_instance *dst_instance,
    sns_find_sensor_arg *src_sensor, sns_fw_data_stream **data_stream)
{
  SNS_PROFILE(SNS_STREAM_SERVICE_DATA_STREAM_INIT_START, 0);
  sns_rc rv = SNS_RC_NOT_AVAILABLE;
  bool island_stream;

  /* If the destination supports island mode, always attempt to malloc the stream
   * in island mode. The destination can access the stream at any time, we should
   * guarentee that the stream is always available. */
  SNS_ASSERT(NULL != (void *)((intptr_t)dst_sensor | (intptr_t)dst_instance));
  island_stream =
    (NULL != dst_sensor && SNS_ISLAND_STATE_NOT_IN_ISLAND != dst_sensor->island_operation) ||
    (NULL != dst_instance && SNS_ISLAND_STATE_NOT_IN_ISLAND != dst_instance->island_operation);

  if(src_sensor->available)
  {
    sns_fw_data_stream *stream = NULL;
    sns_isafe_list_iter iter;
    sns_osa_lock_attr attr;
    sns_rc rc;

    if(island_stream)
      stream = sns_malloc(SNS_HEAP_ISLAND, sizeof(*stream));

    if(NULL == stream)
    {
      island_stream = false;
      stream = sns_malloc(SNS_HEAP_MAIN, sizeof(*stream));
    }
    SNS_ASSERT(NULL != stream);

    stream->stream.api = (sns_data_stream_api*)&data_stream_api;
    sns_isafe_list_item_init(&stream->list_entry_client, stream);
    stream->data_source = src_sensor->sensor;
    stream->client_request = NULL;
    stream->event_priority = src_sensor->priority.event_priority;
    stream->req_priority = src_sensor->priority.req_priority;
    stream->dst_sensor = dst_sensor;
    stream->dst_instance = dst_instance;
    stream->removing = SNS_DATA_STREAM_AVAILABLE;
    stream->unprocessed_events = 0;

    stream->block_idx = UINT16_MAX;
    stream->event_cnt = 0;
    stream->read_off = 0;
    sns_list_init(&stream->clusters);

   /* If the source is not in island, this stream should not support island mode.
    * However, the stream itself can still be malloc'd in island mode because the
    * destination might attempt to access it from island mode. */
    stream->island_operation =
      !sns_island_is_island_ptr((intptr_t)src_sensor->sensor) || !island_stream
      ? SNS_ISLAND_STATE_NOT_IN_ISLAND : SNS_ISLAND_STATE_IN_ISLAND;

    rc = sns_osa_lock_attr_init(&attr);
    SNS_ASSERT(SNS_RC_SUCCESS == rc);
    rc = sns_osa_lock_attr_set_memory_partition(&attr,
        island_stream ? SNS_OSA_MEM_TYPE_ISLAND : SNS_OSA_MEM_TYPE_NORMAL);
    SNS_ASSERT(SNS_RC_SUCCESS == rc);
    rc = sns_osa_lock_init(&attr, &stream->stream_lock);
    SNS_ASSERT(SNS_RC_SUCCESS == rc);

    if(NULL != dst_sensor)
      sns_isafe_list_iter_init(&iter, &dst_sensor->data_streams, true);
    else if(NULL != dst_instance)
      sns_isafe_list_iter_init(&iter, &dst_instance->data_streams, true);
    sns_isafe_list_iter_insert(&iter, &stream->list_entry_client, true);

    sns_isafe_list_item_init(&stream->list_entry_source, stream);

    data_stream_add_checksum(stream);

    *data_stream = stream;

    rv = SNS_RC_SUCCESS;
  }
  SNS_PROFILE(SNS_STREAM_SERVICE_DATA_STREAM_INIT_END, 0);
  return rv;
}

SNS_SECTION(".text.sns") void
sns_data_stream_deinit(sns_fw_data_stream *stream)
{
  SNS_PROFILE(SNS_STREAM_SERVICE_DATA_STREAM_DEINIT_START, 1, stream);
  bool send_req = false;

  if(!sns_island_is_island_ptr((intptr_t)stream) ||
     SNS_ISLAND_STATE_IN_ISLAND != stream->island_operation)
    SNS_ISLAND_EXIT();

  SNS_PRINTF(MED, sns_fw_printf, "sns_data_stream_deinit " SNS_DIAG_PTR, stream);

  sns_osa_lock_acquire(&stream->stream_lock);
  if(SNS_DATA_STREAM_AVAILABLE != stream->removing)
  {
    SNS_PRINTF(HIGH, sns_fw_printf,
        "Stream " SNS_DIAG_PTR " already closing", stream);
  }
  else
  {
    stream->removing = SNS_DATA_STREAM_WAIT_SRC;
    send_req = true;
  }
  sns_osa_lock_release(&stream->stream_lock);

  if(send_req)
  {
    sns_request request;
    request.request = NULL;
    request.request_len = 0;
    request.message_id = SNS_FW_MSGID_SNS_DESTROY_REQ;

    sns_stream_service_add_request(stream, &request);
  }
  SNS_PROFILE(SNS_STREAM_SERVICE_DATA_STREAM_DEINIT_END, 0);
}

SNS_SECTION(".text.sns") sns_rc
sns_data_stream_destroy_complete(sns_fw_data_stream *stream)
{
  SNS_PROFILE(SNS_STREAM_SERVICE_DESTROY_COMPLETE_START, 1, stream);
  SNS_PRINTF(MED, sns_fw_printf, "handle_destroy_complete " SNS_DIAG_PTR, stream);
  sns_fw_sensor *dst_sensor = stream->dst_sensor;
  sns_fw_sensor_instance *dst_instance = stream->dst_instance;
  sns_isafe_list_iter iter;
  sns_list_iter citer;
  sns_cluster_list *list;

  // The DESTROY_COMPLETE event should be the final event
  SNS_ASSERT(1 == stream->event_cnt);
  sns_data_stream_get_next_input(stream);

  if(NULL != dst_instance)
    sns_isafe_list_iter_init(&iter, &dst_instance->data_streams, true);
  else if(NULL != dst_sensor)
    sns_isafe_list_iter_init(&iter, &dst_sensor->data_streams, true);
  else
    SNS_ASSERT(false);

  for(; NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    if(sns_isafe_list_iter_curr(&iter) == &stream->list_entry_client)
    {
      sns_isafe_list_iter_remove(&iter);
      break;
    }
  }

  if(NULL != dst_instance)
  {
    if((dst_instance->inst_removing == SNS_INST_REMOVING_WAITING) &&
        0 == sns_isafe_list_iter_len(&iter))
      sns_sensor_instance_delete(dst_instance);
    else if(sns_sensor_instance_set_island(dst_instance, true))
      SNS_PRINTF(MED, sns_fw_printf, "Instance " SNS_DIAG_PTR " marked IN_ISLAND", dst_instance);
  }
  else if(NULL != dst_sensor)
  {
    if(dst_sensor->removing)
      sns_sensor_delete(dst_sensor);
    else if(sns_sensor_set_island(dst_sensor, true))
      SNS_PRINTF(MED, sns_fw_printf, "Sensor " SNS_DIAG_PTR " marked IN_ISLAND", dst_sensor);
  }
  sns_osa_lock_deinit(&stream->stream_lock);
  sns_memzero(stream, sizeof(stream));
  sns_thread_manager_remove_pending_events(stream);

  list = sns_list_item_get_data(
          sns_list_iter_init(&citer, &stream->clusters, false));

  sns_free(stream->client_request);
  sns_free(list);
  sns_free(stream);

  SNS_PROFILE(SNS_STREAM_SERVICE_DESTROY_COMPLETE_END, 0);
  return SNS_RC_SUCCESS;
}

SNS_SECTION(".text.sns") bool
sns_data_stream_validate(sns_fw_data_stream *stream)
{
  if(NULL != stream)
  {
    return (0 == (stream_service_magic_number ^
         (uint32_t)(uintptr_t)stream->data_source    ^
         (uint32_t)(uintptr_t)stream->dst_instance   ^
         (uint32_t)(uintptr_t)stream->dst_sensor     ^
         (uint32_t)(uintptr_t)stream                 ^
         stream->stream_integrity));
  }
  return false;
}
