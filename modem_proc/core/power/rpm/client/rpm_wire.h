/*===========================================================================

  rpm_events.h - runtime-replacable wrapper the RPM wire protocol

  Copyright (c) 2012 Qualcomm Technologies Incorporated.  
  All Rights Reserved.
  QUALCOMM Proprietary and Confidential.

  ===========================================================================*/

#ifndef RPM_WIRE_H
#define RPM_WIRE_H

#include "rpm.h"
#include "rpm_mq.h"
#include "rpm_resource.h"
#include "kvp.h"

typedef struct
{
    unsigned short size    :16;
    unsigned short type    :8;
    unsigned short version :8;
} RPMRequestHeaderV1;

typedef struct
{
    unsigned short data_len   :16;
    unsigned short resource_id :12;
    unsigned short set        :4;
} RPMRequestDetailsV1;

typedef struct
{
    rpm_service_type service;
    unsigned         size;
    unsigned msg_id;
    unsigned set;
    unsigned resource_type;
    unsigned resource_id;
    unsigned data_len;
    void *           kvp_buffer;
} RPMMessagePacket;

typedef struct
{
    RPMRequestHeaderV1 msg_header; 	
    unsigned msg_id;
    unsigned resource_type;
    RPMRequestDetailsV1 request_details;
} RPMMessageHeaderV1;

typedef struct
{
    RPMRequestHeaderV1 msg_header; 
    unsigned msg_id; //msg_id if ACK, error enum if NACK
} RPMAckPacketV1;

typedef struct
{
    RPMRequestHeaderV1     msg_header;
    unsigned               msg_id; 
    rpm_message_error_type msg_err; 
} RPMNackPacketV1;

//V1 Message header unpack macros
//
//  V0 protocol                ver  service
//    unsigned msg_header -> 0x00   000000
//
//  V1 protocol                ver  type  size
//    unsigned msg_header -> 0x01   00    0000
#define MESSAGING_UNPACK_VERSION(x) (x >> 24)
#define MESSAGING_V1_UNPACK_TYPE(x) ((x >> 16) & 0xFF)

typedef struct
{
    rpm_service_type service;
    unsigned         data_len;
    void *           kvp_buffer;
} RPMCommandPacket;


/* Returns 0 if the message sent nothing; otherwise nonzero. */
extern unsigned (*rpm_wire_send)(unsigned msg_id, rpm_set_type set, rpm_resource_type resource, unsigned id, rpm_resource_t *keys);

void rpm_wire_recv(kvp_t *message, rpm_mq_t *mq);

void rpm_wire_recv_ack(RPMAckPacketV1 *msg, rpm_mq_t *mq);

void rpm_wire_recv_nack(RPMNackPacketV1 *msg, rpm_mq_t *mq);

/*
 * Top-level RPM service API
 *
 * This API allows you to send messages to and receive messages as a top level
 * service.  Most drivers do not want to use this API; instead they use the RPM
 * resource request API shown above.  That API will wrap your request in the
 * appropriate header to ensure its delivery to the request handling service.
 *
 * Since the main RPM service (request handling) is already implemented, the
 * non-internal use of these functions is generally limited to system tests or
 * target-specific hacks.
 */

typedef void (*rpmclient_service_cb)(kvp_t *message, void *context);
void rpmclient_register_handler(rpm_service_type service, rpmclient_service_cb cb, void *context);

void rpmclient_put(RPMMessagePacket *msg);
void rpmclient_put_nas(RPMMessagePacket *msg);
void rpmclient_v1_put(RPMMessageHeaderV1 *msg, char *data);
void rpmclient_v1_put_nas(RPMMessageHeaderV1 *msg, char *data);

#endif // RPM_WIRE_H

