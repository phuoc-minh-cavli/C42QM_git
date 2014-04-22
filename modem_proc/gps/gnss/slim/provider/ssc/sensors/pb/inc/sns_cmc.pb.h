/* Automatically generated nanopb header */
/* Generated by nanopb-0.3.6 at Thu Jul 27 02:11:25 2017. */

#ifndef PB_SNS_CMC_PB_H_INCLUDED
#define PB_SNS_CMC_PB_H_INCLUDED
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
typedef enum _sns_cmc_msgid {
    SNS_CMC_MSGID_SNS_CMC_EVENT = 772
} sns_cmc_msgid;
#define _sns_cmc_msgid_MIN SNS_CMC_MSGID_SNS_CMC_EVENT
#define _sns_cmc_msgid_MAX SNS_CMC_MSGID_SNS_CMC_EVENT
#define _sns_cmc_msgid_ARRAYSIZE ((sns_cmc_msgid)(SNS_CMC_MSGID_SNS_CMC_EVENT+1))
#define sns_cmc_msgid_SNS_CMC_MSGID_SNS_CMC_EVENT SNS_CMC_MSGID_SNS_CMC_EVENT

typedef enum _sns_cmc_motion_state {
    SNS_CMC_STATIONARY = 1,
    SNS_CMC_MOVE = 2,
    SNS_CMC_FIDDLE = 3,
    SNS_CMC_PEDESTRIAN = 4,
    SNS_CMC_VEHICLE = 5,
    SNS_CMC_WALK = 6,
    SNS_CMC_RUN = 7,
    SNS_CMC_BIKE = 8
} sns_cmc_motion_state;
#define _sns_cmc_motion_state_MIN SNS_CMC_STATIONARY
#define _sns_cmc_motion_state_MAX SNS_CMC_BIKE
#define _sns_cmc_motion_state_ARRAYSIZE ((sns_cmc_motion_state)(SNS_CMC_BIKE+1))
#define sns_cmc_motion_state_SNS_CMC_STATIONARY SNS_CMC_STATIONARY
#define sns_cmc_motion_state_SNS_CMC_MOVE SNS_CMC_MOVE
#define sns_cmc_motion_state_SNS_CMC_FIDDLE SNS_CMC_FIDDLE
#define sns_cmc_motion_state_SNS_CMC_PEDESTRIAN SNS_CMC_PEDESTRIAN
#define sns_cmc_motion_state_SNS_CMC_VEHICLE SNS_CMC_VEHICLE
#define sns_cmc_motion_state_SNS_CMC_WALK SNS_CMC_WALK
#define sns_cmc_motion_state_SNS_CMC_RUN SNS_CMC_RUN
#define sns_cmc_motion_state_SNS_CMC_BIKE SNS_CMC_BIKE

typedef enum _sns_cmc_motion_state_event {
    SNS_MS_UNKNOWN = 0,
    SNS_MS_ACTIVE = 1,
    SNS_MS_INACTIVE = 2
} sns_cmc_motion_state_event;
#define _sns_cmc_motion_state_event_MIN SNS_MS_UNKNOWN
#define _sns_cmc_motion_state_event_MAX SNS_MS_INACTIVE
#define _sns_cmc_motion_state_event_ARRAYSIZE ((sns_cmc_motion_state_event)(SNS_MS_INACTIVE+1))
#define sns_cmc_motion_state_event_SNS_MS_UNKNOWN SNS_MS_UNKNOWN
#define sns_cmc_motion_state_event_SNS_MS_ACTIVE SNS_MS_ACTIVE
#define sns_cmc_motion_state_event_SNS_MS_INACTIVE SNS_MS_INACTIVE

/* Struct definitions */
typedef struct _sns_cmc_event {
    pb_callback_t events;
/* @@protoc_insertion_point(struct:sns_cmc_event) */
} sns_cmc_event;

typedef struct _sns_cmc_event_data {
    sns_cmc_motion_state ms_state;
    sns_cmc_motion_state_event ms_state_event;
/* @@protoc_insertion_point(struct:sns_cmc_event_data) */
} sns_cmc_event_data;

/* Default values for struct fields */
extern const sns_cmc_motion_state_event sns_cmc_event_data_ms_state_event_default;

/* Initializer values for message structs */
#define sns_cmc_event_init_default               {{{NULL}, NULL}}
#define sns_cmc_event_data_init_default          {(sns_cmc_motion_state)0, SNS_MS_UNKNOWN}
#define sns_cmc_event_init_zero                  {{{NULL}, NULL}}
#define sns_cmc_event_data_init_zero             {(sns_cmc_motion_state)0, (sns_cmc_motion_state_event)0}

/* Field tags (for use in manual encoding/decoding) */
#define sns_cmc_event_events_tag                 1
#define sns_cmc_event_data_ms_state_tag          1
#define sns_cmc_event_data_ms_state_event_tag    2

/* Struct field encoding specification for nanopb */
extern const pb_field_t sns_cmc_event_fields[2];
extern const pb_field_t sns_cmc_event_data_fields[3];

/* Maximum encoded size of messages (where known) */
/* sns_cmc_event_size depends on runtime parameters */
#define sns_cmc_event_data_size                  4

/* Message IDs (where set with "msgid" option) */
#ifdef PB_MSGID

#define SNS_CMC_MESSAGES \


#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
/* @@protoc_insertion_point(eof) */

#endif
