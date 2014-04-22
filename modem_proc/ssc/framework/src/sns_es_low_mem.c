/*=============================================================================
  @file sns_es_low_mem.c

  Provides Sensor Instances the ability to publish events, which will be
  temporarily stored by this service, and ultimately delivered to all
  applicable clients.

  This file contains all public and private functions for the low memory
  handling of the Event Service buffers.

  Copyright (c) 2016-2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "pb_encode.h"
#include "sns_assert.h"
#include "sns_fw_data_stream.h"
#include "sns_fw_diag_service.h"
#include "sns_fw_event_service.h"
#include "sns_fw_sensor.h"
#include "sns_fw_sensor_event.h"
#include "sns_fw_sensor_instance.h"
#include "sns_isafe_list.h"
#include "sns_island.h"
#include "sns_osa_thread.h"
#include "sns_profiler.h"
#include "sns_rc.h"
#include "sns_thread_prio.h"
#include "sns_types.h"

/*=============================================================================
  Macro Definitions
  ===========================================================================*/

/* Signals known to SNS_ES_LOW_MEM Thread */
#define SNS_ES_THREAD_LOW_MEM  0x01

extern sns_fw_event_service event_service;

/*=============================================================================
  Static Functions
  ===========================================================================*/

/**
 * Drop all samples on an incoming data stream up to the specified end-point.
 *
 * No-op unless sns_fw_event_service::low_mem_active is true.
 *
 * @param[i] sensor Sensor from which we should drop all data
 * @param[i] service Event Service
 *
 * @return Always true
 */
SNS_SECTION(".text.sns") static bool
sns_es_drop_tail(sns_fw_sensor *sensor, void *service)
{
  sns_fw_event_service *this = service;

  sns_es_drop(this, &sensor->data_streams, SNS_STREAM_REMAINDER);

  for(sns_sensor_instance *temp = sensor->sensor.cb->get_sensor_instance(&sensor->sensor, true);
      NULL != temp;
      temp = sensor->sensor.cb->get_sensor_instance(&sensor->sensor, false))
  {
    sns_fw_sensor_instance *instance = (sns_fw_sensor_instance*)temp;
    sns_es_drop(this, &instance->data_streams, SNS_STREAM_REMAINDER);
  }
  return true;
}

/* See sns_diag_encode_log_cb */
SNS_SECTION(".text.sns") static sns_rc
sns_event_service_log_buffer_usage_encode_cb(void *log, size_t log_size,
    size_t encoded_log_size, void *encoded_log, size_t *bytes_written)
{
  UNUSED_VAR(log_size);
  sns_rc rc = SNS_RC_SUCCESS;
  pb_ostream_t stream = pb_ostream_from_buffer(encoded_log, encoded_log_size);

  if(!pb_encode(&stream, sns_diag_mem_utilization_log_fields, log))
  {
    SNS_SPRINTF(ERROR, sns_fw_printf,
        "Error encoding log: %s", PB_GET_ERROR(&stream));
    rc = SNS_RC_FAILED;
  }

  if(SNS_RC_SUCCESS == rc)
    *bytes_written = stream.bytes_written;

  return rc;
}

/* See sns_diag_encode_log_cb */
SNS_SECTION(".text.sns") static sns_rc
sns_event_service_log_encode_cb(void *log, size_t log_size,
    size_t encoded_log_size, void *encoded_log, size_t *bytes_written)
{
  UNUSED_VAR(log_size);
  sns_rc rc = SNS_RC_SUCCESS;
  pb_ostream_t stream = pb_ostream_from_buffer(encoded_log, encoded_log_size);

  if(!pb_encode(&stream, sns_diag_event_service_log_fields, log))
  {
    SNS_SPRINTF(ERROR, sns_fw_printf,
        "Error encoding log: %s", PB_GET_ERROR(&stream));
    rc = SNS_RC_FAILED;
  }

  if(SNS_RC_SUCCESS == rc)
    *bytes_written = stream.bytes_written;

  return rc;
}

/**
 * Low memory handling thread for the Event Service.
 *
 * Upon notification of low memory, drop the oldest unprocessed events from
 * all data streams.
 */
static void
low_mem_thread(void *arg)
{
  sns_fw_event_service *this = (sns_fw_event_service*)arg;
  uint32_t sigs;
  sns_osa_thread_active();

  for(;;)
  {
    sigs = 0;
    sns_osa_thread_idle();
    sns_osa_thread_sigs_wait(0XFFFFFFFF, &sigs);
    sns_osa_thread_active();

    if(sigs & SNS_ES_THREAD_LOW_MEM)
    {
      SNS_PRINTF(ERROR, sns_fw_printf, "Main buffer low memory, dropping data");
      sns_osa_lock_acquire(this->low_mem_lock);
      sns_thread_manager_disable(true);

      atomic_store(&this->low_mem_active, 1);
      sns_sensor_foreach(&sns_es_drop_tail, this);
      atomic_store(&this->low_mem_active, 0);
      
      sns_osa_lock_release(this->low_mem_lock);
      sns_thread_manager_disable(false);
      SNS_PRINTF(ERROR, sns_fw_printf, "low_mem_thread_func complete");
    }
  }
}

/**
 * Encode the heap-specific statistics within the sns_diag_event_service_log
 * log packet.
 */
bool
encode_log_cb(pb_ostream_t *stream, const pb_field_t *field,
    void *const *arg)
{
  sns_fw_event_service *this = (sns_fw_event_service*)*arg;
  bool rv = true;

  for(int i = 0; i < SNS_ES_BUFFER_ID_LAST; i++)
  {
    sns_diag_event_service_log_heap_stats stat;

    sns_osa_lock_acquire(&this->lock);
    stat.heap_id = i;
    stat.total_memory = this->heaps[i].tot_blk_cnt;
    stat.used_memory = this->heaps[i].current_alloc_cnt;
    stat.max_memory = this->heaps[i].max_alloc_cnt;
    stat.total_clusters = this->heaps[i].tot_cluster_cnt;
    stat.used_clusters = this->heaps[i].current_cluster_cnt;
    stat.max_clusters = this->heaps[i].max_cluster_cnt;
    stat.alloc_failures = this->heaps[i].alloc_failures;
    stat.max_size = this->heaps[i].max_size;

    if(!pb_encode_tag_for_field(stream, field))
      rv = false;
    else if(!pb_encode_submessage(stream, sns_diag_event_service_log_heap_stats_fields, &stat))
      rv = false;

    if(this->heaps[i].clear_max)
    {
      this->heaps[i].max_alloc_cnt = 0;
      this->heaps[i].max_cluster_cnt = 0;
      this->heaps[i].alloc_failures = 0;
      this->heaps[i].max_size = 0;
    }
    this->heaps[i].clear_max = !this->heaps[i].clear_max;
    sns_osa_lock_release(&this->lock);
  }

  return rv;
}

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

SNS_SECTION(".text.sns") bool
sns_es_low_mem_active(void)
{
  return atomic_load(&event_service.low_mem_active);
}

SNS_SECTION(".text.sns") void
sns_es_low_mem_block(void)
{
  if(sns_es_low_mem_active())
  {
    sns_osa_lock_acquire(event_service.low_mem_lock);
    sns_osa_lock_release(event_service.low_mem_lock);
  }
}

void
sns_es_log_buffer_use(uint64_t cookie)
{
  sns_fw_event_service *this = (sns_fw_event_service*)&event_service;

  {
    sns_diag_mem_utilization_log *log = sns_diag_log_alloc(
        sizeof(sns_diag_mem_utilization_log), SNS_LOG_MEMORY_UTILIZATION);
    if(NULL != log)
    {
      log->heap_id = SNS_DIAG_HEAP_ID_EVENT_BUFFER;
      log->timestamp = sns_get_system_time();
      log->has_cookie = true;
      log->cookie = cookie;

      sns_osa_lock_acquire(&this->lock);
      log->total_memory = SNS_EVENT_SERVICE_BUFFER_ISLAND + SNS_EVENT_SERVICE_BUFFER;
      log->used_memory = SNS_EVENT_SERVICE_BLOCK_SIZE *
        (this->heaps[SNS_ES_BUFFER_ID_ISLAND].current_alloc_cnt +
        this->heaps[SNS_ES_BUFFER_ID_MAIN].current_alloc_cnt);
      sns_osa_lock_release(&this->lock);

      sns_diag_publish_fw_log(SNS_LOG_MEMORY_UTILIZATION,
          sizeof(sns_diag_mem_utilization_log), log,
          sns_diag_mem_utilization_log_size,
          sns_event_service_log_buffer_usage_encode_cb);
    }
  }

  {
    sns_diag_event_service_log *log = sns_diag_log_alloc(
        sizeof(sns_diag_event_service_log), SNS_LOG_ES_LOG);
    if(NULL != log)
    {
      int encoded_size =
        sns_diag_event_service_log_heap_stats_size * (SNS_ES_BUFFER_ID_LAST + 1);
      log->timestamp = sns_get_system_time();
      log->has_cookie = true;
      log->cookie = cookie;

      log->stats.funcs.encode = &encode_log_cb;
      log->stats.arg = this;

      sns_diag_publish_fw_log(SNS_LOG_ES_LOG,
          sizeof(sns_diag_event_service_log), log,
          encoded_size, sns_event_service_log_encode_cb);
    }
  }
}

void
sns_es_drop(sns_fw_event_service *this, sns_isafe_list *list,
    uint32_t remainder)
{
  UNUSED_VAR(this);
  sns_isafe_list_iter iter;

  SNS_PROFILE(SNS_EVENT_SERVICE_DROP_TAIL_START, 0);
  for(sns_isafe_list_iter_init(&iter, list, true);
      NULL != sns_isafe_list_iter_curr(&iter);
      sns_isafe_list_iter_advance(&iter))
  {
    uint32_t drop_cnt = 0;
    sns_fw_data_stream *stream = (sns_fw_data_stream*)
      sns_isafe_list_iter_get_curr_data(&iter);
    sns_fw_sensor_event *event = sns_data_stream_peek_input(stream);
    uint32_t event_cnt = sns_data_stream_get_input_cnt(stream);

    while(NULL != event && event_cnt > remainder)
    {
      drop_cnt++;
      event = sns_data_stream_get_next_input(stream);
      event_cnt--;
    }
    SNS_PRINTF(ERROR, sns_fw_printf,
        "Dropped %i events from " SNS_DIAG_PTR ", %i events remain",
        drop_cnt, stream, sns_data_stream_get_input_cnt(stream));
  }
  SNS_PROFILE(SNS_EVENT_SERVICE_DROP_TAIL_END, 0);
}

void
sns_es_handle_low_mem(sns_fw_event_service *this)
{
  SNS_ISLAND_EXIT();
  sns_osa_thread_sigs_set(this->low_mem_thread, SNS_ES_THREAD_LOW_MEM);
}

/**
 *  Initialize the low memory thread, and all other Event Service state
 *  needed to handle low memory.
 */
sns_rc
sns_es_low_mem_init(sns_fw_event_service *this)
{
  sns_rc rc = SNS_RC_SUCCESS;
  sns_osa_thread_attr thread_attr;
  sns_osa_lock_attr lock_attr;

  atomic_init(&event_service.low_mem_active, 0);
  
  rc = sns_osa_lock_attr_init(&lock_attr);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_lock_attr_set_memory_partition(&lock_attr, SNS_OSA_MEM_TYPE_NORMAL);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  rc = sns_osa_lock_create(&lock_attr,&event_service.low_mem_lock);
  SNS_ASSERT(SNS_RC_SUCCESS == rc);
  
  rc |= sns_osa_thread_attr_init(&thread_attr);
  rc |= sns_osa_thread_attr_set_name(&thread_attr, "SNS_ES_LOW_MEM");
  rc |= sns_osa_thread_attr_set_stack(&thread_attr, (uintptr_t)NULL, 4096);
  rc |= sns_osa_thread_attr_set_memory_partition(&thread_attr, SNS_OSA_MEM_TYPE_NORMAL);
  rc |= sns_osa_thread_attr_set_priority(&thread_attr, SNS_LOW_MEM_THREAD_PRIO);
  if(SNS_RC_SUCCESS != rc)
  {
    SNS_PRINTF(ERROR, sns_fw_printf, "Error initializing attributes %i", rc);
  }
  else
  {
    rc = sns_osa_thread_create(&low_mem_thread, this,
        &thread_attr, &this->low_mem_thread);
    if(SNS_RC_SUCCESS != rc)
      SNS_PRINTF(ERROR, sns_fw_printf, "Error creating ES low mem thread %i", rc);
  }

  return rc;
}
