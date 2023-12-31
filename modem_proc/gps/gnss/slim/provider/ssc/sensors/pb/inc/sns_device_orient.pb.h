/* Automatically generated nanopb header */
/* Generated by nanopb-0.3.7 at Fri Jul 14 02:01:36 2017. */

#ifndef PB_SNS_DEVICE_ORIENT_PB_H_INCLUDED
#define PB_SNS_DEVICE_ORIENT_PB_H_INCLUDED
#include <pb.h>

#include "sns_std_sensor.pb.h"

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Enum definitions */
typedef enum _sns_device_orient_msgid {
    SNS_DEVICE_ORIENT_MSGID_SNS_DEVICE_ORIENT_EVENT = 776
} sns_device_orient_msgid;
#define _sns_device_orient_msgid_MIN SNS_DEVICE_ORIENT_MSGID_SNS_DEVICE_ORIENT_EVENT
#define _sns_device_orient_msgid_MAX SNS_DEVICE_ORIENT_MSGID_SNS_DEVICE_ORIENT_EVENT
#define _sns_device_orient_msgid_ARRAYSIZE ((sns_device_orient_msgid)(SNS_DEVICE_ORIENT_MSGID_SNS_DEVICE_ORIENT_EVENT+1))
#define sns_device_orient_msgid_SNS_DEVICE_ORIENT_MSGID_SNS_DEVICE_ORIENT_EVENT SNS_DEVICE_ORIENT_MSGID_SNS_DEVICE_ORIENT_EVENT

typedef enum _sns_device_orient_event_type {
    SNS_DEVICE_ORIENT_EVENT_TYPE_UNKNOWN = -1,
    SNS_DEVICE_ORIENT_EVENT_TYPE_DEFAULT = 0,
    SNS_DEVICE_ORIENT_EVENT_TYPE_POS_1 = 1,
    SNS_DEVICE_ORIENT_EVENT_TYPE_POS_2 = 2,
    SNS_DEVICE_ORIENT_EVENT_TYPE_POS_3 = 3
} sns_device_orient_event_type;
#define _sns_device_orient_event_type_MIN SNS_DEVICE_ORIENT_EVENT_TYPE_UNKNOWN
#define _sns_device_orient_event_type_MAX SNS_DEVICE_ORIENT_EVENT_TYPE_POS_3
#define _sns_device_orient_event_type_ARRAYSIZE ((sns_device_orient_event_type)(SNS_DEVICE_ORIENT_EVENT_TYPE_POS_3+1))
#define sns_device_orient_event_type_SNS_DEVICE_ORIENT_EVENT_TYPE_UNKNOWN SNS_DEVICE_ORIENT_EVENT_TYPE_UNKNOWN
#define sns_device_orient_event_type_SNS_DEVICE_ORIENT_EVENT_TYPE_DEFAULT SNS_DEVICE_ORIENT_EVENT_TYPE_DEFAULT
#define sns_device_orient_event_type_SNS_DEVICE_ORIENT_EVENT_TYPE_POS_1 SNS_DEVICE_ORIENT_EVENT_TYPE_POS_1
#define sns_device_orient_event_type_SNS_DEVICE_ORIENT_EVENT_TYPE_POS_2 SNS_DEVICE_ORIENT_EVENT_TYPE_POS_2
#define sns_device_orient_event_type_SNS_DEVICE_ORIENT_EVENT_TYPE_POS_3 SNS_DEVICE_ORIENT_EVENT_TYPE_POS_3

/* Struct definitions */
typedef struct _sns_device_orient_event {
    sns_device_orient_event_type state;
/* @@protoc_insertion_point(struct:sns_device_orient_event) */
} sns_device_orient_event;

/* Default values for struct fields */

/* Initializer values for message structs */
#define sns_device_orient_event_init_default     {(sns_device_orient_event_type)0}
#define sns_device_orient_event_init_zero        {(sns_device_orient_event_type)0}

/* Field tags (for use in manual encoding/decoding) */
#define sns_device_orient_event_state_tag        1

/* Struct field encoding specification for nanopb */
extern const pb_field_t sns_device_orient_event_fields[2];

/* Maximum encoded size of messages (where known) */
#define sns_device_orient_event_size             11

/* Message IDs (where set with "msgid" option) */
#ifdef PB_MSGID

#define SNS_DEVICE_ORIENT_MESSAGES \


#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
/* @@protoc_insertion_point(eof) */

#endif
