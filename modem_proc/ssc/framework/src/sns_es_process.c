/*=============================================================================
  @file sns_es_process.c

  Provides Sensor Instances the ability to publish events, which will be
  temporarily stored by this service, and ultimately delivered to all
  applicable clients.

  This file contains all public and private functions for the processing
  of pending events incoming to a Sensor or Sensor Instance.

  Copyright (c) 2016-2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include "pb_encode.h"
#include "sns_assert.h"
#include "sns_fw.pb.h"
#include "sns_fw_data_stream.h"
#include "sns_fw_diag_service.h"
#include "sns_fw_event_service.h"
#include "sns_fw_sensor_event.h"
#include "sns_isafe_list.h"
#include "sns_printf_int.h"
#include "sns_profiler.h"
#include "sns_rc.h"
#include "sns_types.h"

extern sns_fw_event_service event_service;

/*=============================================================================
  Static Function Definitions
  ===========================================================================*/

/**
 * Process a pending event for a Sensor that was just taken from the queue.
 *
 * @param[i] this Reference to the Event Service
 * @param[i] stream Data Stream with event(s) to process
 */
SNS_SECTION(".text.sns") static void
process_pending_event_sensor(sns_fw_event_service *this,
    sns_fw_data_stream *stream)
{
  SNS_PROFILE(SNS_EVENT_SERVICE_PROCESS_SENSOR, 0);
  sns_rc rc;
  sns_fw_sensor *sensor = stream->dst_sensor;

#ifdef SNS_EVENT_SERVICE_VERBOSE_LOG
  SNS_PRINTF(LOW, sns_fw_printf,
      "Notifying Sensor "SNS_DIAG_PTR" of event on "SNS_DIAG_PTR,
      sensor, stream);
#endif

  SNS_PROFILE(SNS_EVENT_SERVICE_NOTIFY_EVENT_START, 1, sensor);
  if(!sns_island_is_island_ptr((intptr_t)sensor->sensor.sensor_api->notify_event) ||
     (NULL != sensor->sensor.instance_api &&
      !sns_island_is_island_ptr((intptr_t)sensor->sensor.instance_api->set_client_config)))
  {
    SNS_ISLAND_EXIT();
  }
  rc = sensor->sensor.sensor_api->notify_event((sns_sensor*)sensor);
  SNS_PROFILE(SNS_EVENT_SERVICE_NOTIFY_EVENT_END, 0);

  if(SNS_RC_SUCCESS != rc)
  {
    SNS_PRINTF(MED, sns_fw_printf,
        "notify_event error %i from sensor " SNS_DIAG_PTR, rc, sensor);
  }

  if(SNS_RC_NOT_AVAILABLE == rc)
  {
    SNS_ISLAND_EXIT();
    sns_es_drop(this, &sensor->data_streams, SNS_STREAM_REMAINDER);
  }
  else if(SNS_RC_INVALID_LIBRARY_STATE == rc)
  {
    SNS_ISLAND_EXIT();
    sns_sensor_library_deinit(sensor->library);
  }
  else if(SNS_RC_SUCCESS != rc)
  {
    SNS_ISLAND_EXIT();
    sns_sensor_deinit(sensor);
  }
}

/**
 * Process a pending event for a Sensor Instance that was just taken from the
 * queue.
 *
 * @param[i] this Reference to the Event Service
 * @param[i] stream Data Stream with event(s) to process
 */
SNS_SECTION(".text.sns") static void
process_pending_event_instance(sns_fw_event_service *this,
    sns_fw_data_stream *stream)
{
  SNS_PROFILE(SNS_EVENT_SERVICE_PROCESS_SENSOR_INSTANCE, 0);
  sns_rc rc;
  sns_fw_sensor_instance *instance = stream->dst_instance;

#ifdef SNS_EVENT_SERVICE_VERBOSE_LOG
  SNS_PRINTF(LOW, sns_fw_printf,
      "Notifying Instance "SNS_DIAG_PTR" of event on "SNS_DIAG_PTR,
      instance, stream);
#endif

  SNS_PROFILE(SNS_EVENT_SERVICE_NOTIFY_EVENT_START, 1, instance);
  rc = instance->sensor->sensor.instance_api->notify_event(
      (sns_sensor_instance*)instance);
  SNS_PROFILE(SNS_EVENT_SERVICE_NOTIFY_EVENT_END, 0);

  if(SNS_RC_SUCCESS != rc)
  {
    SNS_PRINTF(ERROR, sns_fw_printf,
        "notify_event error %i from instance " SNS_DIAG_PTR, rc, instance);
  }

  if(SNS_RC_NOT_AVAILABLE == rc)
  {
    SNS_ISLAND_EXIT();
    sns_es_drop(this, &instance->data_streams, SNS_STREAM_REMAINDER);
    sns_es_publish_error(instance, SNS_RC_NOT_AVAILABLE);
  }
  else if(SNS_RC_INVALID_STATE == rc)
  {
    SNS_ISLAND_EXIT();
    sns_sensor_instance_deinit(&instance->instance);
  }
  else if(SNS_RC_SUCCESS != rc)
  {
    sns_sensor_deinit(instance->sensor);
  }
}

/**
 * Process a single event stream.
 *
 * See sns_thread_mgr_task_func().
 */
SNS_SECTION(".text.sns") static sns_rc
sns_event_service_process(void *func_arg)
{
  SNS_PROFILE(SNS_EVENT_SERVICE_PROCESS_EVENTS_START, 0);
  sns_fw_event_service *this = (sns_fw_event_service*)&event_service;
  sns_fw_data_stream *stream = func_arg;

  if(!sns_island_is_island_ptr((intptr_t)stream) ||
     SNS_ISLAND_STATE_IN_ISLAND != stream->island_operation ||
     ((NULL != stream->dst_sensor &&
        SNS_ISLAND_STATE_IN_ISLAND != stream->dst_sensor->island_operation) ||
      (NULL != stream->dst_instance &&
        SNS_ISLAND_STATE_IN_ISLAND != stream->dst_instance->island_operation)))
    SNS_ISLAND_EXIT();

  if(sns_data_stream_validate(stream))
  {
    int removing;

    sns_osa_lock_acquire(&stream->stream_lock);
    removing = stream->removing;
    sns_osa_lock_release(&stream->stream_lock);

    atomic_store(&stream->unprocessed_events, 0);
    if(SNS_DATA_STREAM_AVAILABLE != removing)
    {
      SNS_PROFILE(SNS_EVENT_SERVICE_REMOVE_STREAM_START, 0);
      sns_fw_sensor_event *event;

      for(event = sns_data_stream_peek_input(stream);
          NULL != event;
          event = sns_data_stream_get_next_input(stream))
      {
        if(SNS_FW_MSGID_SNS_DESTROY_COMPLETE_EVENT == event->event.message_id)
        {
          sns_data_stream_destroy_complete(stream);
          break;
        }
      }
      SNS_PROFILE(SNS_EVENT_SERVICE_REMOVE_STREAM_END, 0);
    }
    else
    {
      if(NULL != stream->dst_sensor)
        process_pending_event_sensor(this, stream);
      else
        process_pending_event_instance(this, stream);
    }
  }

  SNS_PROFILE(SNS_EVENT_SERVICE_PROCESS_EVENTS_END, 0);
  return SNS_RC_SUCCESS;
}

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

SNS_SECTION(".text.sns") void
sns_es_add_pending_event(sns_fw_data_stream *stream)
{
  unsigned int disabled = 0;

  if(atomic_compare_exchange_strong(&stream->unprocessed_events, &disabled, 1))
  {
    sns_sensor_library *dest = NULL != stream->dst_sensor
      ? stream->dst_sensor->library : stream->dst_instance->sensor->library;
    SNS_ASSERT(NULL != dest);

    sns_thread_manager_add(dest, sns_event_service_process,
        stream, stream->event_priority);
  }
}

SNS_SECTION(".text.sns") sns_fw_sensor_event*
sns_es_get_fw_event(struct sns_sensor_event *event)
{
  sns_fw_sensor_event *rv = (sns_fw_sensor_event*)
    ((uintptr_t)event - sizeof(rv->header));

  return rv;
}
