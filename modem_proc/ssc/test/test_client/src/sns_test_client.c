/*=============================================================================
  @file sns_test_client.c

  SLPI resident test client for the QSocket Client interface.
  Only supports sensors which use the sns_std_sensor interface.

  PEND: File is getting a bit long, consider splitting

  Copyright (c) 2018-2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ===========================================================================*/

/*=============================================================================
  Include Files
  ===========================================================================*/
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stringl.h>
#include <inttypes.h>
#include <unistd.h>
#include "pb_encode.h"
#include "pb_decode.h"
#include "timer.h"
#include "uSleep_mode_trans.h"
#include "utimer.h"
#include "uTimetick.h"

#include "sns_client.pb.h"
#include "sns_diag_sensor.pb.h"
#include "sns_resampler.pb.h"
#include "sns_std.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_suid.pb.h"

#include "sns_client.h"
#include "sns_test_osa.h"

/*=============================================================================
  Macros
  ===========================================================================*/

#define SNS_TEST_CLIENT_ENABLE_FILENAME "/mnt/vendor/persist/sensors/test/client_enabled"

#define SNS_TEST_CLIENT_SCENARIO_FILENAME "/mnt/vendor/persist/sensors/test/test_case"

#define SNS_TEST_CLIENT_OUTPUT_DIR "/mnt/vendor/persist/sensors/test/results"

#define SNS_TEST_CLIENT_LOG_FILENAME "log.txt"

#define SNS_TEST_CLIENT_STACK_SIZE 3072

#define SNS_TEST_CLIENT_THREAD_PRIO 0x8D

/*=============================================================================
  Type Definitions
  ===========================================================================*/

/**
 * Utility structure used to encode a string or already encoded message
 * into another Protocol Buffer message.
 */
typedef struct sns_buffer_arg
{
  /* Buffer to be written, or reference to read buffer */
  void const *buf;
  /* Length of buf */
  size_t buf_len;
} sns_buffer_arg;

/**
 * Parsed test parameters as specified by the client.
 */
typedef struct sns_test_param
{
  char data_type[22];
  float sample_rate;  // In Hz
  uint32_t batch_period; // In microseconds
  uint32_t flush_period; // In microseconds
  bool has_flush_period;  // flush_period field is valid and should be sent
  bool flush_only;
  bool max_batch;
  bool passive;
  bool use_resampler; // Whether to request for resampled data

  // PEND: Currently only duration of first stream is used for all sensors
  uint64_t duration; // In microseconds; how long to stream sensor
} sns_test_param;

/**
 * A client data stream of a specific sensor.
 */
typedef struct sns_test_stream
{
  bool valid;

  struct {
    uint32_t message_id;  // Particular message ID
    uint32_t sample_cnt;  // Num of received samples since test start
    uint64_t first_ts; // Timestamp of first received event (usec)
    uint64_t last_ts; // Timestamp of last received event (usec)
  } received[5];

  sns_std_suid suid;  // SUID of the sensor under test
  sns_std_sensor_stream_type type;  // Stream type as determined from attribute
  sns_test_param param;
} sns_test_stream;

/**
 * A QSocket connection; which may support multiple concurrent data streams.
 */
typedef struct sns_test_conn
{
  bool valid;

  // PEND: Consider dynamically allocating streams/connections
  sns_test_stream streams[4];
  struct sns_client *client;  // QSocket client connection
} sns_test_conn;

/**
 * Statistics collected during the course of running a client test.
 */
typedef struct sns_test_state
{
  uint32_t island_exit_cnt; // Num of island exits during test
  uint64_t last_entry; // Last island entry timestamp (usec)
  uint64_t island_duration; // Microseconds spent in island during test
  uint64_t total_duration;  // Total microseconds from test start to finish
  uint64_t start_time;  // Timestamp of test start
  uint64_t end_time;  // Timestamp of test conclusion

  sns_std_suid resampler_suid;  // SUID of the Resampler sensor
  sns_std_suid diag_suid;  // DIAG Sensor SUID (used for memory/island logs)

  FILE *log_file; // Log file for this specific test (status and result)

  struct sns_client *client;  // Client used for SUID/Attribute lookup
  sns_test_conn connections[4];

  // PEND: Consider mutex for thread safety
} sns_test_state;

/*=============================================================================
  Static Variable Definitions
  ===========================================================================*/

/* The SUID of the SUID Lookup Sensor is a constant value, and published within
 * sns_suid.proto. */
static sns_std_suid suid_lookup = sns_suid_sensor_init_default;

/* Global state pointer is needed only for island statistic collection;
 * should not be used in any other case */
static sns_test_state *test_state;

/* Test client thread stack */
static uint8_t thread_stack[SNS_TEST_CLIENT_STACK_SIZE];

/*=============================================================================
  Static PB Utility Function Definitions
  ===========================================================================*/

/**
 * Copy an already encoded payload into a message field.  For the purpose of
 * this function, an encoded submessage is identical to a character string.
 */
static bool
encode_payload(pb_ostream_t *stream, const pb_field_t *field, void *const *arg)
{
  sns_buffer_arg *info = (sns_buffer_arg*)*arg;

  if(!pb_encode_tag_for_field(stream, field))
    return false;
  if(!pb_encode_string(stream, info->buf, info->buf_len))
    return false;

  return true;
}

/**
 * Decode a string of bytes from a field. This function is intended to be
 * used as a callback function during the decode process.
 *
 * @see pb_callback_s::decode
 */
static bool
decode_payload(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  sns_buffer_arg *data = (sns_buffer_arg*)*arg;

  data->buf_len = stream->bytes_left;
  data->buf = stream->state;
  return pb_read(stream, (pb_byte_t*)NULL, stream->bytes_left);
}

/**
 * Send a request to the specified service, using the provided param.
 *
 * @param[i] payload Encoded Sensor-specific request message
 * @param[i] resp_cb Where to direct received response message
 * @param[i] suid Destination SUID
 * @param[i] msg_id Sensor-specific message ID
 * @param[i] encode_func How to encode message
 * @param[i] encode_arg Data sent to encode function
 * @param[i] param Test specification for this stream
 *
 * @return true upon success; false otherwise
 */
static bool
send_basic_req(struct sns_client *client, sns_client_resp resp_cb, sns_std_suid *suid,
    uint32_t msg_id, void *encode_func, void *encode_arg, sns_test_param *param)
{
  sns_client_request_msg req_msg = sns_client_request_msg_init_default;
  int err;

  req_msg.suid = *suid;
  req_msg.msg_id = msg_id;
  req_msg.susp_config.client_proc_type = SNS_STD_CLIENT_PROCESSOR_SSC;
  req_msg.request.has_batching = false;
  req_msg.request.payload.funcs.encode = encode_func;
  req_msg.request.payload.arg = encode_arg;

  if(NULL != param)
  {
    req_msg.request.has_batching = true;
    req_msg.request.batching.batch_period = param->batch_period;
    req_msg.request.batching.has_flush_only = true;
    req_msg.request.batching.flush_only = param->flush_only;
    req_msg.request.batching.has_max_batch = true;
    req_msg.request.batching.max_batch = param->max_batch;
    req_msg.request.has_is_passive = true;
    req_msg.request.is_passive = param->passive;

    // For fields other than FP, it's fine to send the default value (0/false)
    if(param->has_flush_period)
    {
      req_msg.request.batching.has_flush_period = true;
      req_msg.request.batching.flush_period = param->flush_period;
    }
  }

  err = sns_client_send(client, &req_msg, resp_cb, NULL);
  if(0 != err)
  {
    SNS_LOG(ERROR, "sns_client_send error %i", err);
    return false;
  }
  return true;
}

/**
 * Get the SUID of a client request indication message.
 *
 * In theory this is inefficient, and could be optimized by using manual
 * decoding.
 *
 * @param[i] msg Encoded message of type sns_client_event_msg
 * @param[i] msg_len Length of the msg buffer
 */
static sns_std_suid
get_suid(void *msg, uint32_t msg_len)
{
  pb_istream_t stream;
  sns_client_event_msg event = sns_client_event_msg_init_default;

  stream = pb_istream_from_buffer(msg, msg_len);
  if(!pb_decode(&stream, sns_client_event_msg_fields, &event))
    SNS_LOG(ERROR, "Error decoding Event: %s", PB_GET_ERROR(&stream));

  return event.suid;
}

/*=============================================================================
  Static Test Function Definitions
  ===========================================================================*/

/*
 * Decode an element of sns_client_event_msg::events.  This function will be
 * called by nanopb once per each element in the array.
 *
 * No sensor-specific decoding is performed.  Just keep general statistics
 * regarding events received.
 *
 * @param[i] arg Sensor-specific decode function
 */
static bool
test_decode_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  bool rv = true;
  sns_client_event_msg_sns_client_event event =
    sns_client_event_msg_sns_client_event_init_default;
  void **args = (void**)*arg;
  sns_std_suid *suid = args[0];
  sns_test_conn *conn = args[1];

  if(!pb_decode(stream, sns_client_event_msg_sns_client_event_fields, &event))
  {
    SNS_LOG(ERROR, "Error decoding Event: %s", PB_GET_ERROR(stream));
    rv = false;
  }
  else
  {
    for(int j = 0; j < ARR_SIZE(conn->streams); j++)
    {
      if(!conn->streams[j].valid)
        continue;
      else if(0 != memcmp(suid, &conn->streams[j].suid, sizeof(*suid)))
        continue;

      for(int k = 0; k < ARR_SIZE(conn->streams[j].received); k++)
      {
        if(0 == conn->streams[j].received[k].message_id ||
           event.msg_id == conn->streams[j].received[k].message_id)
        {
          uint64_t curr_time = uTimetick_Get();

          conn->streams[j].received[k].message_id = event.msg_id;
          conn->streams[j].received[k].last_ts =
                uTimetick_CvtFromTicks(curr_time, T_USEC);

          if(0 == conn->streams[j].received[k].sample_cnt)
            conn->streams[j].received[k].first_ts =
                conn->streams[j].received[k].last_ts;

          ++conn->streams[j].received[k].sample_cnt;
          break;
        }
      }
    }
  }

  return rv;
}

/**
 * Handle a received indication message from the Sensor Service for a
 * Sensor under test.
 */
static void
test_ind_cb(struct sns_client *client, void *msg, uint32_t msg_len,
    void *cb_data)
{
  UNUSED_VAR(client);
  sns_test_conn *conn = (sns_test_conn*)cb_data;
  pb_istream_t stream;
  sns_client_event_msg event = sns_client_event_msg_init_default;
  sns_std_suid suid = get_suid(msg, msg_len);
  void *arg[2] = {&suid, conn};

  event.events.funcs.decode = &test_decode_event;
  event.events.arg = arg;

  stream = pb_istream_from_buffer(msg, msg_len);
  if(!pb_decode(&stream, sns_client_event_msg_fields, &event))
    SNS_LOG(ERROR, "Error decoding Event: %s", PB_GET_ERROR(&stream));
}

/**
 * QSocket/QMI Error callback.  This would be our notification if the connection
 * was lost.
 *
 * See sns_client_error */
static void
test_error_cb(struct sns_client *client, sns_std_error error, void *cb_data)
{
  UNUSED_VAR(client);
  UNUSED_VAR(cb_data);

  SNS_LOG(ERROR, "Error callback %i", error);
}

/* See sns_client_resp */
static void
test_resp_cb(struct sns_client *client, sns_std_error error, void *cb_data)
{
  UNUSED_VAR(client);
  UNUSED_VAR(cb_data);
  SNS_LOG(INFO, "Received response rc %i", error);
}

/**
 * Create a streaming sensor configuration request.
 */
static bool
encode_stream_req(pb_ostream_t *stream, const pb_field_t *field, void *const *arg)
{
  sns_std_sensor_config config_req = sns_std_sensor_config_init_default;
  sns_test_param *param = (sns_test_param*)*arg;
  bool rv = true;

  config_req.sample_rate = param->sample_rate;

  if(!pb_encode_tag_for_field(stream, field))
  {
    SNS_LOG(ERROR, "Error encoding tag: %s", PB_GET_ERROR(stream));
    rv = false;
  }
  else if(!pb_encode_submessage(stream, sns_std_sensor_config_fields, &config_req))
  {
    SNS_LOG(ERROR, "Error encoding request: %s", PB_GET_ERROR(stream));
    rv = false;
  }

  return rv;
}

/**
 * Create a Resampler sensor configuration request.
 */
static bool
encode_resampler_req(pb_ostream_t *stream, const pb_field_t *field, void *const *arg)
{
  sns_resampler_config config_req = sns_resampler_config_init_default;
  sns_test_stream *test_stream = (sns_test_stream*)*arg;
  bool rv = true;

  config_req.resampled_rate = test_stream->param.sample_rate;
  config_req.sensor_uid = test_stream->suid;
  config_req.rate_type = SNS_RESAMPLER_RATE_FIXED;
  config_req.filter = true;

  if(!pb_encode_tag_for_field(stream, field))
  {
    SNS_LOG(ERROR, "Error encoding tag: %s", PB_GET_ERROR(stream));
    rv = false;
  }
  else if(!pb_encode_submessage(stream, sns_resampler_config_fields, &config_req))
  {
    SNS_LOG(ERROR, "Error encoding request: %s", PB_GET_ERROR(stream));
    rv = false;
  }

  return rv;
}

/**
 * Start the provided test case for the specified duration.
 *
 * @return true upon success; false if test could not be begun
 */
static bool
start_test(sns_test_state *state)
{
  bool rv = true;
  int err;

  for(int i = 0; i < ARR_SIZE(state->connections); i++)
  {
    if(!state->connections[i].valid)
      continue;

    err = sns_client_init(&state->connections[i].client, 2000, &test_ind_cb,
        &state->connections[i], &test_error_cb, state);
    if(0 != err)
    {
      SNS_LOG(ERROR, "Error sns_client_init %i", err);
      rv = false;
      break;
    }

    for(int j = 0; j < ARR_SIZE(state->connections[0].streams); j++)
    {
      if(!state->connections[i].streams[j].valid)
        continue;

      if(state->connections[i].streams[j].param.use_resampler)
      {
        if(!send_basic_req(state->connections[i].client,
              &test_resp_cb, &state->resampler_suid,
              SNS_RESAMPLER_MSGID_SNS_RESAMPLER_CONFIG, &encode_resampler_req,
              &state->connections[i].streams[j],
              &state->connections[i].streams[j].param))
          SNS_LOG(ERROR, "Error sending resampler request");
      }
      else if(SNS_STD_SENSOR_STREAM_TYPE_STREAMING ==
          state->connections[i].streams[j].type)
      {
        if(!send_basic_req(state->connections[i].client,
              &test_resp_cb, &state->connections[i].streams[j].suid,
              SNS_STD_SENSOR_MSGID_SNS_STD_SENSOR_CONFIG, &encode_stream_req,
              &state->connections[i].streams[j].param,
              &state->connections[i].streams[j].param))
          SNS_LOG(ERROR, "Error sending enable request");
      }
      else
      {
        if(!send_basic_req(state->connections[i].client,
              &test_resp_cb, &state->connections[i].streams[j].suid,
              SNS_STD_SENSOR_MSGID_SNS_STD_ON_CHANGE_CONFIG, NULL, NULL,
              &state->connections[i].streams[j].param))
          SNS_LOG(ERROR, "Error sending enable request");
      }
    }
  }

  // PEND: If necessary, can consider sleeping different periods per stream
  utimer_sleep(state->connections[0].streams[0].param.duration, UT_USEC, true);
  return true;
}

/**
 * Stop all streams for the current test case.
 *
 * @return true upon success; false if test could not be stopped
 */
static bool
end_test(sns_test_state *state)
{
  for(int i = 0; i < ARR_SIZE(state->connections); i++)
  {
    if(!state->connections[i].valid)
      continue;
    sns_client_deinit(state->connections[i].client);
  }

  // Sleep 2 seconds, so all SEE clean-up can finish
  utimer_sleep(2000000, UT_USEC, true);

  return true;
}

/*=============================================================================
  Static SUID Lookup Function Definitions
  ===========================================================================*/

/**
 * Each SUID event contains an array of SUIDs.  This function will be called
 * once per each SUID in the array.
 */
static bool
decode_suid(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  sns_std_suid uid;
  sns_std_suid *out_suid = (sns_std_suid*)*arg;

  if(!pb_decode(stream, sns_std_suid_fields, &uid))
  {
    SNS_LOG(ERROR, "Error decoding SUID: %s", PB_GET_ERROR(stream));
    return false;
  }

  SNS_LOG(INFO, "Received SUID Event with SUID %llx %llx",
      uid.suid_low, uid.suid_high);

  *out_suid = uid;

  return true;
}

static void
suid_resp_cb(struct sns_client *client, sns_std_error error, void *cb_data);

/**
 * Decode the payload of the event message, the SUID Event
 */
static bool
decode_suid_event(sns_test_state *state, pb_istream_t *stream)
{
  sns_suid_event suid_event = sns_suid_event_init_default;
  sns_buffer_arg data;
  sns_std_suid suid = {0, 0};

  suid_event.suid.funcs.decode = &decode_suid;
  suid_event.suid.arg = &suid;
  suid_event.data_type.funcs.decode = &decode_payload;
  suid_event.data_type.arg = &data;

  if(!pb_decode(stream, sns_suid_event_fields, &suid_event))
  {
    SNS_LOG(ERROR, "Error decoding SUID event: %s", PB_GET_ERROR(stream));
    return false;
  }

  if(0 == strcmp((char*)data.buf, "diag_sensor"))
    state->diag_suid = suid;
  else if(0 == strcmp((char*)data.buf, "resampler"))
    state->resampler_suid = suid;
  else
  {
    for(int i = 0; i < ARR_SIZE(state->connections); i++)
    {
      if(!state->connections[i].valid)
        continue;

      for(int j = 0; j < ARR_SIZE(state->connections[0].streams); j++)
      {
        if(!state->connections[i].streams[j].valid)
          continue;

        if(0 == strcmp((char*)data.buf, state->connections[i].streams[j].param.data_type))
        {
          state->connections[i].streams[j].suid = suid;
          if(0 == suid.suid_low)
          {
            SNS_LOG(ERROR, "No SUIDs found matching type '%s'", (char*)data.buf);
          }
          else if(!send_basic_req(state->client, &suid_resp_cb, &suid,
              SNS_STD_MSGID_SNS_STD_ATTR_REQ, NULL, NULL, NULL))
          {
            SNS_LOG(ERROR, "Error sending ATTR request");
          }
        }
      }
    }
  }

  return true;
}

/**
 * Decode events during the SUID look-up process.
 */
static bool
suid_decode_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  bool rv = true;
  sns_client_event_msg_sns_client_event event =
    sns_client_event_msg_sns_client_event_init_default;
  sns_buffer_arg data;
  sns_test_state *state = (sns_test_state*)*arg;

  event.payload.funcs.decode = &decode_payload;
  event.payload.arg = &data;

  if(!pb_decode(stream, sns_client_event_msg_sns_client_event_fields, &event))
  {
    SNS_LOG(ERROR, "Error decoding Event: %s", PB_GET_ERROR(stream));
    rv = false;
  }
  else
  {
    pb_istream_t sub_stream = pb_istream_from_buffer(data.buf, data.buf_len);
    decode_suid_event(state, &sub_stream);
  }

  return rv;
}

/**
 * Decode an attribute value.
 */
static bool
decode_attr_value(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  int *value = (int*)*arg;
  sns_std_attr_value_data data = sns_std_attr_value_data_init_default;

  if(!pb_decode(stream, sns_std_attr_value_data_fields, &data))
  {
    return false;
  }

  if(data.has_sint)
    *value = data.sint;

  return true;
}

/**
 * Decode a single attribute from the attributes array.
 */
static bool
decode_attr(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  sns_test_stream *test_stream = (sns_test_stream*)*arg;
  sns_std_attr attribute = sns_std_attr_init_default;
  int value;

  attribute.value.values.funcs.decode = &decode_attr_value;
  attribute.value.values.arg = &value;

  if(!pb_decode(stream, sns_std_attr_fields, &attribute))
    return false;

  if(SNS_STD_SENSOR_ATTRID_STREAM_TYPE == attribute.attr_id)
    test_stream->type = value;

  return true;
}

/**
 * Decode an attribute event.
 */
static bool
decode_attr_event(pb_istream_t *stream, const pb_field_t *field, void **arg)
{
  UNUSED_VAR(field);
  bool rv = true;
  sns_client_event_msg_sns_client_event event =
    sns_client_event_msg_sns_client_event_init_default;
  sns_buffer_arg data;
  sns_test_stream *test_stream = (sns_test_stream*)*arg;

  event.payload.funcs.decode = &decode_payload;
  event.payload.arg = &data;

  if(!pb_decode(stream, sns_client_event_msg_sns_client_event_fields, &event))
  {
    SNS_LOG(ERROR, "Error decoding Event: %s", PB_GET_ERROR(stream));
    rv = false;
  }
  else
  {
    pb_istream_t sub_stream = pb_istream_from_buffer(data.buf, data.buf_len);
    sns_std_attr_event attr_event = sns_std_attr_event_init_default;

    attr_event.attributes.funcs.decode = &decode_attr;
    attr_event.attributes.arg = test_stream;

    if(!pb_decode(&sub_stream, sns_std_attr_event_fields, &attr_event))
    {
      SNS_LOG(ERROR, "Error decoding SUID event: %s", PB_GET_ERROR(&sub_stream));
      return false;
    }
  }

  return rv;
}

/**
 * Indication callback for the SUID look-up process.
 */
static void
suid_ind_cb(struct sns_client *client, void *msg, uint32_t msg_len,
    void *cb_data)
{
  UNUSED_VAR(client);
  sns_test_state *state = (sns_test_state*)cb_data;
  sns_std_suid suid = get_suid(msg, msg_len);

  for(int i = 0; i < ARR_SIZE(state->connections); i++)
  {
    if(!state->connections[i].valid)
      continue;

    // Note: if two clients have asked for the same data type, we might end-up
    // sending extra attribute requests; this should be fine.
    for(int j = 0; j < ARR_SIZE(state->connections[0].streams); j++)
    {
      sns_client_event_msg event = sns_client_event_msg_init_default;
      pb_istream_t stream = pb_istream_from_buffer(msg, msg_len);

      if(!state->connections[i].streams[j].valid)
        continue;
      if(0 != memcmp(&state->connections[i].streams[j].suid, &suid, sizeof(suid)))
        continue;

      event.events.funcs.decode = &decode_attr_event;
      event.events.arg = &state->connections[i].streams[j];

      if(!pb_decode(&stream, sns_client_event_msg_fields, &event))
        SNS_LOG(ERROR, "Error decoding Event: %s", PB_GET_ERROR(&stream));
    }
  }

  if(0 == memcmp(&suid, &suid_lookup, sizeof(suid)))
  {
    sns_client_event_msg event = sns_client_event_msg_init_default;
    pb_istream_t stream = pb_istream_from_buffer(msg, msg_len);

    event.events.funcs.decode = &suid_decode_event;
    event.events.arg = state;

    if(!pb_decode(&stream, sns_client_event_msg_fields, &event))
      SNS_LOG(ERROR, "Error decoding Event: %s", PB_GET_ERROR(&stream));
  }
}

static void
suid_error_cb(struct sns_client *client, sns_std_error error, void *cb_data)
{
  UNUSED_VAR(client);
  UNUSED_VAR(cb_data);

  SNS_LOG(ERROR, "Error callback %i", error);
}

/* See sns_client_resp */
static void
suid_resp_cb(struct sns_client *client, sns_std_error error, void *cb_data)
{
  UNUSED_VAR(client);
  UNUSED_VAR(cb_data);
  SNS_LOG(INFO, "Received response rc %i", error);
}

/**
 * Create a SUID lookup request for the specified data type.
 *
 * @param[i] arg Sensor data type to search for
 */
static bool
encode_suid_req(pb_ostream_t *stream, const pb_field_t *field, void *const *arg)
{
  sns_suid_req suid_req = sns_suid_req_init_default;
  char const *data_type = (char const*)*arg;
  bool rv = true;

  suid_req.data_type.funcs.encode = &encode_payload;
  suid_req.data_type.arg = &((sns_buffer_arg)
    { .buf = data_type, .buf_len = strlen(data_type) + 1 });
  suid_req.has_register_updates = true;
  suid_req.register_updates = true;

  if(!pb_encode_tag_for_field(stream, field))
  {
    SNS_LOG(ERROR, "Error encoding SUID tag: %s", PB_GET_ERROR(stream));
    rv = false;
  }
  else if(!pb_encode_submessage(stream, sns_suid_req_fields, &suid_req))
  {
    SNS_LOG(ERROR, "Error encoding SUID request: %s", PB_GET_ERROR(stream));
    rv = false;
  }

  return rv;
}

/**
 * Lookup the needed SUIDs for the test scenario.
 */
static bool
lookup_suids(sns_test_state *state)
{
  bool rv = true;
  int err;

  err = sns_client_init(&state->client, 2000, &suid_ind_cb, state, &suid_error_cb, state);
  if(0 != err)
  {
    SNS_LOG(ERROR, "Error sns_client_init %i", err);
    rv = false;
  }
  else if(!send_basic_req(state->client, &suid_resp_cb, &suid_lookup,
        SNS_SUID_MSGID_SNS_SUID_REQ, &encode_suid_req, "diag_sensor", NULL))
  {
    SNS_LOG(ERROR, "Error sending SUID lookup request");
    rv = false;
  }
  else if(!send_basic_req(state->client, &suid_resp_cb, &suid_lookup,
        SNS_SUID_MSGID_SNS_SUID_REQ, &encode_suid_req, "resampler", NULL))
  {
    SNS_LOG(ERROR, "Error sending Resampler lookup request");
    rv = false;
  }
  else
  {
    for(int i = 0; i < ARR_SIZE(state->connections); i++)
    {
      if(!state->connections[i].valid)
        continue;

      for(int j = 0; j < ARR_SIZE(state->connections[0].streams); j++)
      {
        if(!state->connections[i].streams[j].valid)
          continue;

        if(!send_basic_req(state->client, &suid_resp_cb, &suid_lookup,
              SNS_SUID_MSGID_SNS_SUID_REQ, &encode_suid_req,
              state->connections[i].streams[j].param.data_type, NULL))
        {
          SNS_LOG(ERROR, "Error sending SUID lookup request");
          rv = false;
        }
      }
    }

    // PEND: Could use timed_sem_wait to be more exact
    utimer_sleep(1000000, UT_USEC, true);

    // Determine if all SUIDs were retrieved
    for(int i = 0; i < ARR_SIZE(state->connections); i++)
    {
      if(!state->connections[i].valid)
        continue;

      for(int j = 0; j < ARR_SIZE(state->connections[0].streams); j++)
      {
        if(!state->connections[i].streams[j].valid)
          continue;

        if(0 == state->connections[i].streams[j].suid.suid_low)
          rv = false;
      }
    }

    sns_client_deinit(state->client);
  }

  return rv;
}

/*=============================================================================
  Static Test Framework Function Definitions
  ===========================================================================*/

/**
 * Island callback notifies entry/exit from island mode.
 *
 * @note No callback data is provided, so must use a global reference to the
 * test state.
 */
static void
island_cb(bool island_entry)
{
  if(0 != test_state->total_duration)
  {
    // If total_duration is non-zero, then the test has completed, and we
    // should not continue to update island statistics
  }
  else if(island_entry) // If island entry occurred
  {
    uint64_t entry_time = uTimetick_Get();
    test_state->last_entry = uTimetick_CvtFromTicks(entry_time, T_USEC);
  }
  else if(0 != test_state->last_entry)
  {
    uint64_t exit_time = uTimetick_Get();

    test_state->island_exit_cnt++;
    test_state->island_duration +=
      uTimetick_CvtFromTicks(exit_time, T_USEC) - test_state->last_entry;
    test_state->last_entry = 0;
  }
}

/**
 *  Save all test results and statistics to the log file.
 *  E.g. Time to first sample, sample rate, etc.
 *
 *  @param[i] status True if no test set-up issues occurred
 */
static void
log_results(sns_test_state *state, bool status)
{
  SNS_LOG(WARN, "SAVE OUTPUT");
  char temp[150];
  int len;
  char *result = status ? "PASS" : "FAIL";

  len = snprintf(temp, sizeof(temp),
      "total_duration: %lli\nstart_time: %lli\nend_time: %lli\n",
      state->total_duration, state->start_time, state->end_time);
  fwrite(temp, sizeof(char), len, state->log_file);

  len = snprintf(temp, sizeof(temp),
      "island_exit_cnt: %"PRIu32"\nisland_duration: %"PRIu64
      "\nisland_percent: %f\n",
      state->island_exit_cnt, state->island_duration,
      (float)state->island_duration * 100 / (float)state->total_duration);
  fwrite(temp, sizeof(char), len, state->log_file);

  for(int i = 0; i < ARR_SIZE(state->connections); i++)
  {
    if(!state->connections[i].valid)
      continue;

    len = snprintf(temp, sizeof(temp), "\nConnection: %i\n", i);
    fwrite(temp, sizeof(char), len, state->log_file);

    for(int j = 0; j < ARR_SIZE(state->connections[i].streams); j++)
    {
      if(!state->connections[i].streams[j].valid)
        continue;

      len = snprintf(temp, sizeof(temp), "Stream: %i\n", j);
      fwrite(temp, sizeof(char), len, state->log_file);

      for(int k = 0;
          k < ARR_SIZE(state->connections[i].streams[j].received) &&
          0 != state->connections[i].streams[j].received[k].message_id;
          k++)
      {
        len = snprintf(temp, sizeof(temp),
            "message_id: %"PRIu32", sample_cnt: %"PRIu32
            ", first_ts: %"PRIu64", last_ts: %"PRIu64"\n",
            state->connections[i].streams[j].received[k].message_id,
            state->connections[i].streams[j].received[k].sample_cnt,
            state->connections[i].streams[j].received[k].first_ts,
            state->connections[i].streams[j].received[k].last_ts);
        fwrite(temp, sizeof(char), len, state->log_file);
      }
    }
  }

  fwrite(result, sizeof(char), strlen(result), state->log_file);
  fclose(state->log_file);
}

/**
 * Prep the output directory; move and create files.
 *
 * @return true upon success; false otherwise
 */
static bool
prep_output(sns_test_state *state)
{
  char output_path[100];
  char output_name[100];
  char log_name[100];

  snprintf(output_path, sizeof(output_path),
      "%s/%llu", SNS_TEST_CLIENT_OUTPUT_DIR, uTimetick_Get());
  SNS_LOG(INFO, "Output directory: %s", output_path);

  if(0 != mkdir(output_path, 0777))
    SNS_LOG(WARN, "Error creating ouput directory");

  snprintf(output_name, sizeof(output_name),
      "%s/test_case", output_path);
  if(0 != rename(SNS_TEST_CLIENT_SCENARIO_FILENAME, output_name))
    SNS_LOG(ERROR, "Error renaming test case");

  snprintf(log_name, sizeof(log_name),
      "%s/%s", output_path, SNS_TEST_CLIENT_LOG_FILENAME);
  state->log_file = fopen(log_name, "w");
  if(NULL == state->log_file)
  {
    SNS_LOG(ERROR, "Unable to open log file '%s'", log_name);
    return false;
  }

  return true;
}

/**
 * Parse the test file and save the test scenario in 'param'.
 *
 * @return true upon success; false otherwise
 */
static bool
parse_test(FILE *test_file, sns_test_state *state)
{
  bool rv = true;
  char line[50];  // PEND: Maximum line length
  sns_test_stream *curr_stream = NULL;
  sns_test_conn *curr_conn = NULL;
  int stream_idx = -1, conn_idx = -1;

  while(NULL != fgets(line, sizeof(line), test_file) && rv)
  {
    char temp;
    char field1[15], field2[15];
    char value[20];

    if(3 == sscanf(line, " <%[a-z_]>%[^<]</%[a-z_]>\n%c", field1, value, field2, &temp))
    {
      if(NULL == curr_stream)
      {
        SNS_LOG(ERROR, "Invalid format on line", line);
        rv = false;
      }
      else if(0 == strcmp(field1, "sensor"))
        memscpy(curr_stream->param.data_type, sizeof(curr_stream->param.data_type),
            value, strlen(value));
      else if(0 == strcmp(field1, "sample_rate"))
        curr_stream->param.sample_rate = strtol(value, NULL, 10);
      else if(0 == strcmp(field1, "batch_period"))
        curr_stream->param.batch_period = strtol(value, NULL, 10);
      else if(0 == strcmp(field1, "flush_period"))
      {
        curr_stream->param.has_flush_period = true;
        curr_stream->param.flush_period = strtol(value, NULL, 10);
      }
      else if(0 == strcmp(field1, "flush_only"))
        curr_stream->param.flush_only = strtol(value, NULL, 10);
      else if(0 == strcmp(field1, "max_batch"))
        curr_stream->param.max_batch = strtol(value, NULL, 10);
      else if(0 == strcmp(field1, "passive"))
        curr_stream->param.passive = strtol(value, NULL, 10);
      else if(0 == strcmp(field1, "use_resampler"))
        curr_stream->param.use_resampler = strtol(value, NULL, 10);
      else if(0 == strcmp(field1, "duration"))
        curr_stream->param.duration = strtol(value, NULL, 10);
      else
        SNS_LOG(WARN, "Unknown field '%s'", field1);
    }
    else if(1 == sscanf(line, " <%[a-z_]>\n%c", field1, &temp))
    {
      if(0 == strcmp(field1, "test_case"))
      {  ; /* Do nothing */ }
      else if(NULL != curr_conn && NULL == curr_stream &&
         0 == strcmp(field1, "stream"))
      {
        curr_stream = &state->connections[conn_idx].streams[++stream_idx];
        if(stream_idx >= ARR_SIZE(state->connections[conn_idx].streams))
        {
          SNS_LOG(ERROR, "Exceeded stream count %i for connection %i",
              stream_idx, conn_idx);
          rv = false;
          break;
        }
        curr_stream->valid = true;
      }
      else if(NULL == curr_conn && 0 == strcmp(field1, "connection"))
      {
        curr_conn = &state->connections[++conn_idx];
        if(conn_idx >= ARR_SIZE(state->connections))
        {
          SNS_LOG(ERROR, "Exceeded connection count %i", conn_idx);
          rv = false;
          break;
        }
        stream_idx = -1;
        curr_conn->valid = true;
      }
      else
      {
        SNS_LOG(ERROR, "Invalid field '%s'", field1);
        rv = false;
      }
    }
    else if(1 == sscanf(line, " </%[a-z_]>\n%c", field1, &temp))
    {
      if(0 == strcmp(field1, "test_case"))
      {  ; /* Do nothing */ }
      else if(NULL != curr_stream && 0 == strcmp(field1, "stream"))
      {
         curr_stream = NULL;
      }
      else if(NULL != curr_conn && NULL == curr_stream &&
              0 == strcmp(field1, "connection"))
      {
        curr_conn = NULL;
      }
      else
      {
        SNS_LOG(ERROR, "Invalid field '%s'", field1);
        rv = false;
      }
    }
    else
    {
      SNS_LOG(ERROR, "Invalid test line: '%s'", line);
      rv = false;
    }
  }
  return rv;
}

/**
 * Create a DIAG log request of the specified type.
 *
 * @param[i] arg Which of sns_diag_triggered_log_type
 */
static bool
encode_diag_req(pb_ostream_t *stream, const pb_field_t *field, void *const *arg)
{
  sns_diag_log_trigger_req req = sns_diag_log_trigger_req_init_default;
  sns_diag_triggered_log_type type = *(sns_diag_triggered_log_type*)*arg;
  bool rv = false;

  req.log_type = type;

  if(!pb_encode_tag_for_field(stream, field))
    SNS_LOG(ERROR, "Error encoding SUID tag: %s", PB_GET_ERROR(stream));
  else if(!pb_encode_submessage(stream, sns_diag_log_trigger_req_fields, &req))
    SNS_LOG(ERROR, "Error encoding SUID request: %s", PB_GET_ERROR(stream));
  else
    rv = true;

  return rv;
}

/**
 * Request memory and island analysis log packets from the DIAG Sensor.
 *
 * @return true upon success; false otherwise
 */
static bool
request_log_packets(sns_test_state *state)
{
  bool rv = false;
  int island_log = SNS_DIAG_TRIGGERED_LOG_TYPE_ISLAND_LOG,
      memory_log = SNS_DIAG_TRIGGERED_LOG_TYPE_MEMORY_USAGE_LOG;
  struct sns_client *client;
  int err;

  err = sns_client_init(&client, 2000, NULL, NULL, NULL, NULL);
  if(0 != err)
    SNS_LOG(ERROR, "Error sns_client_init %i", err);
  else if(!send_basic_req(client, NULL, &state->diag_suid,
        SNS_DIAG_SENSOR_MSGID_SNS_DIAG_LOG_TRIGGER_REQ, &encode_diag_req,
         &island_log, NULL))
    SNS_LOG(ERROR, "Error sending DIAG island request");
  else if(!send_basic_req(client, NULL, &state->diag_suid,
        SNS_DIAG_SENSOR_MSGID_SNS_DIAG_LOG_TRIGGER_REQ, &encode_diag_req,
        &memory_log, NULL))
    SNS_LOG(ERROR, "Error sending DIAG memory request");
  else
    rv = true;

  if(client)
    sns_client_deinit(client);
  return rv;
}

/**
 * Start collecting test statistics.
 *
 * @return true upon success; false otherwise
 */
static bool
start_stats(sns_test_state *state)
{
  uint64_t timestamp;

  timestamp = uTimetick_Get();
  state->start_time = uTimetick_CvtFromTicks(timestamp, T_USEC);
  if(sns_test_osa_island_status())
    test_state->last_entry = state->start_time;
  sns_test_osa_island_reg(&island_cb);

  return true;
}

/**
 * Unregister/release any resources opened in start_stats.
 * Calculate statistics as needed.
 *
 * @return true upon success; false otherwise
 */
static bool
end_stats(sns_test_state *state)
{
  uint64_t timestamp;

  timestamp = uTimetick_Get();
  state->end_time = uTimetick_CvtFromTicks(timestamp, T_USEC);
  sns_test_osa_island_unreg(&island_cb);
  island_cb(false);
  state->total_duration = state->end_time - state->start_time;

  return true;
}

/**
 * Handle a test case found on the LA filesystem.
 */
static void
handle_test(FILE *test_file)
{
  sns_test_state *state;
  bool status = false;

  state = sns_test_osa_malloc(sizeof(*state));
  SNS_ASSERT(NULL != state);
  memset(state, 0, sizeof(*state));

  test_state = state;
  if(!prep_output(state))
  {
    SNS_LOG(ERROR, "FAILED prep_output");
  }
  else if(!parse_test(test_file, state))
  {
    SNS_LOG(ERROR, "FAILED parse_test");
  }
  else if(!lookup_suids(state))
  {
    SNS_LOG(ERROR, "FAILED lookup_suids");
  }
  else if(!request_log_packets(state))
  {
    SNS_LOG(ERROR, "FAILED request_log_packets");
  }
  else if(!start_stats(state))
  {
    SNS_LOG(ERROR, "FAILED start_stats");
  }
  else if(!start_test(state))
  {
    SNS_LOG(ERROR, "FAILED start_test");
  }
  else if(!end_stats(state))
  {
    SNS_LOG(ERROR, "FAILED end_stats");
  }
  else if(!end_test(state))
  {
    SNS_LOG(ERROR, "FAILED end_test");
  }
  else if(!request_log_packets(state))
  {
    SNS_LOG(ERROR, "FAILED request_log_packets");
  }
  else
    status = true;

  uSleep_exit();
  log_results(state, status);
  sns_test_osa_free(state);
}

/**
 * Main loop for test client module.
 * Query every 30 seconds for presence of test scenario file.
 */
static void*
main_thread(void *arg)
{
  UNUSED_VAR(arg);

  if(0 != mkdir(SNS_TEST_CLIENT_OUTPUT_DIR, 0777))
    SNS_LOG(WARN, "Error creating results directory");

  for(;;)
  {
    FILE *test_file = NULL;

    timer_sleep(30000000, T_USEC, true);
    test_file = fopen(SNS_TEST_CLIENT_SCENARIO_FILENAME, "r");
    if(NULL != test_file)
    {
      handle_test(test_file);
      fclose(test_file);
    }
  }
}

/*=============================================================================
  Public Function Definitions
  ===========================================================================*/

void
sns_test_client_init(void)
{
  FILE *enable_file = NULL;

  sns_test_osa_init();

  enable_file = fopen(SNS_TEST_CLIENT_ENABLE_FILENAME, "r");
  if(NULL == enable_file)
  {
    SNS_LOG(WARN, "Test Client disabled '%s'", SNS_TEST_CLIENT_ENABLE_FILENAME);
  }
  else
  {
    int err;
    struct sns_test_osa_thread *thread;

    err = sns_test_osa_thread_create(&thread, "CLIENT_TEST", &main_thread,
        NULL, &thread_stack[0], sizeof(thread_stack));
    SNS_ASSERT(0 == err);
    fclose(enable_file);
  }
}
