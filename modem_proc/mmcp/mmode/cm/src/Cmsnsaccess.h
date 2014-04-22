#ifndef CMSNSACCESS_H
#define CMSNSACCESS_H


#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "cm.h"
#include "custmmode.h"
#ifdef FEATURE_REF_TRACKER 

#include "qmi_client.h"
#include "pb_decode.h"
#include "pb_encode.h"

#include "sns_client_api_v01.h"
#include "sns_std.pb.h"
#include "sns_std_sensor.pb.h"
#include "sns_suid.pb.h"
#include "sns_std.pb.h"
#include "sns_std_type.pb.h"
#include "sns_sig_motion.pb.h"
#include "sns_motion_detect.pb.h"
#include "sns_client.pb.h"
#include "sns_pb_util.h"
#ifdef FEATURE_MMODE_TX_21
#include "sns_flight_mode.pb.h"
#include "sns_freefall_detect.pb.h"
#include "sns_tilt_tracker.pb.h"
#include "sns_mcd_tracker.pb.h"
#include "sns_registry_util.h"
#include "sns_pb_util.h"
#include "sns_registry.pb.h"

#endif

#endif

#define QMI_NO_ERR                  0

#ifdef FEATURE_REF_TRACKER

#if 0
/* Enum definitions */
typedef enum _sns_client_msgid {
    SNS_CLIENT_MSGID_SNS_CLIENT_DISABLE_REQ = 10
} sns_client_msgid;
#define _sns_client_msgid_MIN SNS_CLIENT_MSGID_SNS_CLIENT_DISABLE_REQ
#define _sns_client_msgid_MAX SNS_CLIENT_MSGID_SNS_CLIENT_DISABLE_REQ
#define _sns_client_msgid_ARRAYSIZE ((sns_client_msgid)(SNS_CLIENT_MSGID_SNS_CLIENT_DISABLE_REQ+1))
#define sns_client_msgid_SNS_CLIENT_MSGID_SNS_CLIENT_DISABLE_REQ SNS_CLIENT_MSGID_SNS_CLIENT_DISABLE_REQ

typedef enum _sns_client_delivery {
    SNS_CLIENT_DELIVERY_WAKEUP = 0,
    SNS_CLIENT_DELIVERY_NO_WAKEUP = 1
} sns_client_delivery;
#define _sns_client_delivery_MIN SNS_CLIENT_DELIVERY_WAKEUP
#define _sns_client_delivery_MAX SNS_CLIENT_DELIVERY_NO_WAKEUP
#define _sns_client_delivery_ARRAYSIZE ((sns_client_delivery)(SNS_CLIENT_DELIVERY_NO_WAKEUP+1))
#define sns_client_delivery_SNS_CLIENT_DELIVERY_WAKEUP SNS_CLIENT_DELIVERY_WAKEUP
#define sns_client_delivery_SNS_CLIENT_DELIVERY_NO_WAKEUP SNS_CLIENT_DELIVERY_NO_WAKEUP

/* Struct definitions */
typedef struct _sns_client_event_msg {
    sns_std_suid suid;
    pb_callback_t events;
/* @@protoc_insertion_point(struct:sns_client_event_msg) */
} sns_client_event_msg;

typedef struct _sns_client_event_msg_sns_client_event {
    uint32_t msg_id;
    uint64_t timestamp;
    pb_callback_t payload;
/* @@protoc_insertion_point(struct:sns_client_event_msg_sns_client_event) */
} sns_client_event_msg_sns_client_event;

typedef struct _sns_client_request_msg_suspend_config {
    sns_std_client_processor client_proc_type;
    sns_client_delivery delivery_type;
    pb_callback_t nowakeup_msg_ids;
/* @@protoc_insertion_point(struct:sns_client_request_msg_suspend_config) */
} sns_client_request_msg_suspend_config;

typedef struct _sns_client_request_msg {
    sns_std_suid suid;
    uint32_t msg_id;
    sns_client_request_msg_suspend_config susp_config;
    sns_std_request request;
/* @@protoc_insertion_point(struct:sns_client_request_msg) */
} sns_client_request_msg;

/* Default values for struct fields */
extern const sns_std_client_processor sns_client_request_msg_suspend_config_client_proc_type_default;
extern const sns_client_delivery sns_client_request_msg_suspend_config_delivery_type_default;

/* Initializer values for message structs */
#define sns_client_request_msg_init_default      {sns_std_suid_init_default, 0, sns_client_request_msg_suspend_config_init_default, sns_std_request_init_default}
#define sns_client_request_msg_suspend_config_init_default {SNS_STD_CLIENT_PROCESSOR_APSS, SNS_CLIENT_DELIVERY_WAKEUP, {{NULL}, NULL}}
#define sns_client_event_msg_init_default        {sns_std_suid_init_default, {{NULL}, NULL}}
#define sns_client_event_msg_sns_client_event_init_default {0, 0, {{NULL}, NULL}}
#define sns_client_request_msg_init_zero         {sns_std_suid_init_zero, 0, sns_client_request_msg_suspend_config_init_zero, sns_std_request_init_zero}
#define sns_client_request_msg_suspend_config_init_zero {(sns_std_client_processor)0, (sns_client_delivery)0, {{NULL}, NULL}}
#define sns_client_event_msg_init_zero           {sns_std_suid_init_zero, {{NULL}, NULL}}
#define sns_client_event_msg_sns_client_event_init_zero {0, 0, {{NULL}, NULL}}

/* Field tags (for use in manual encoding/decoding) */
#define sns_client_event_msg_suid_tag            1
#define sns_client_event_msg_events_tag          2
#define sns_client_event_msg_sns_client_event_msg_id_tag 1
#define sns_client_event_msg_sns_client_event_timestamp_tag 2
#define sns_client_event_msg_sns_client_event_payload_tag 3
#define sns_client_request_msg_suspend_config_client_proc_type_tag 1
#define sns_client_request_msg_suspend_config_delivery_type_tag 2
#define sns_client_request_msg_suspend_config_nowakeup_msg_ids_tag 3
#define sns_client_request_msg_suid_tag          1
#define sns_client_request_msg_msg_id_tag        2
#define sns_client_request_msg_susp_config_tag   3
#define sns_client_request_msg_request_tag       4

/* Struct field encoding specification for nanopb */
extern const pb_field_t sns_client_request_msg_fields[5];
extern const pb_field_t sns_client_request_msg_suspend_config_fields[4];
extern const pb_field_t sns_client_event_msg_fields[3];
extern const pb_field_t sns_client_event_msg_sns_client_event_fields[4];

/* Maximum encoded size of messages (where known) */
#define sns_client_request_msg_size              (37 + sns_client_request_msg_suspend_config_size + sns_std_request_size)



/*=============================================================================
  Macro Definitions
  ===========================================================================*/
#if 0
#ifndef ARR_SIZE
#define ARR_SIZE(arr) (sizeof(arr)/sizeof(arr[0]))
#endif
#endif

#ifndef UNUSED_VAR
#define UNUSED_VAR(var) ((void)(var));
#endif

/*=============================================================================
  Data Type Definitions
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
 * Structure to be used with pb_decode_float_arr_cb
 */
typedef struct pb_float_arr_arg
{
  /* Where to place the decoded float array */
  float *arr;
  /* Length of the float array */
  uint8_t arr_len;
  /* Array index used for decoding.
     Must be initialized to 0 prior to calling pb_decode_float_arr_cb*/
  uint8_t *arr_index;
} pb_float_arr_arg;

#endif
#endif

#ifndef UNUSED_VAR
#define UNUSED_VAR(var) ((void)(var));
#endif

#define MAX_SENSOR_DATA  5 // 5 sensor data is more than expected as we are getting 3 sensor data max at this time

#define CM_OS_FLAG_QMI_CCI_EVENT 0x00400000


#endif
