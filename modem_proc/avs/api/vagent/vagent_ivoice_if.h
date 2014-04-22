#ifndef __VAGENT_IVOICE_IF_H__
#define __VAGENT_IVOICE_IF_H__

/**
  @file  vagent_ivoice_if.h
  @brief This file contains voice interface definitions for Voice
         Agent.
*/

/*
  ============================================================================
  Copyright (C) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/api/vagent/vagent_ivoice_if.h#1 $
  $Author: pwbldsvc $
  ============================================================================
*/


/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "mmdefs.h"


/****************************************************************************
 * VAGENT TYPE DEFINITIONS
 ****************************************************************************/

/**
 * Event callback provided by a client of the voice services.
 *
 * The client returns VS_EOK from the callback when the client was succesful
 * in queuing the event to its own thread context for processing.
 *
 * All other error codes indicates a problem.
 *
 * The event parameter pointer is valid only within the callback context.
 */
typedef uint32_t ( *vagent_ivoice_event_callback_fn_t ) (

  uint32_t event_id,
    /**< Event ID. See VAGENT_IVOICE_EVENT_YYY. */
  void* payload,
    /**< Pointer to an event-ID specific payload structure. */
  uint32_t size,
    /**< Size of the payload structure. */
  void* client_data
    /**< Client provided data pointer from the REGISTER command. */
);


/** 
 * VAGENT enumeration for voice client types.
 */
typedef enum vagent_voice_client_type_t {

  VAGENT_VOICE_CLIENT_CDMA = 0x0,
    /**< CDMA subsystem. */
  VAGENT_VOICE_CLIENT_GSM = 0x1,
    /**< GSM subsystem. */
  VAGENT_VOICE_CLIENT_WCDMA = 0x2,
    /**< WCDMA subsystem. */
  VAGENT_VOICE_CLIENT_TDSCDMA = 0x3 ,
    /**< TDSCDMA subsystem. */
  VAGENT_VOICE_CLIENT_IMS = 0x4,
    /**< IMS subsystem. */
  VAGENT_VOICE_CLIENT_QCHAT = 0x5,
    /**< QCHAT subsystem. */
  VAGENT_VOICE_CLIENT_ECALL = 0x6,
    /**< ECALL subsystem. */
  VAGENT_VOICE_CLIENT_MAX = 0x7,
   /**< Used for internal purpose. */
  VAGENT_VOICE_CLIENT_INVALID = 0xFF,
   /**< Used for internal purpose. */
} vagent_voice_client_type_t;


/****************************************************************************
 * VAGENT CONTROL INTERFACE DEFINITION                                           
 ****************************************************************************/

/** Register command (synchronous blocking API).
 *
 * The client uses this command to register with voice agent to acquire
 * vocoder resource during voice call on a particular subscription.
 *
 * - Returns VS_EOK, denotes call was successful.
 */
#define VAGENT_IVOICE_CMD_REGISTER ( 0x00013345 )

typedef struct vagent_ivoice_cmd_register_t {

  vagent_voice_client_type_t client_id;
    /**< voice agent client type. */

  vagent_ivoice_event_callback_fn_t client_cb;
    /**<
     * Central event callback function, which receives asynchronous events
     * from the server.
     *
     * Operational contract:
     *
     * - The client may only queue the incoming event and signal a worker
     * thread to process the event. The client must not perform any other
     * processing in the callback context.
     *
     * - The client may not call any APIs on the server in the callback
     * context. This will cause synchronization issues for the driver and
     * may lead to a system failure or deadlock.
     *
     * - The client may not perform any blocking operations or acquire any
     * locks in the event callback context that lead to a system deadlock.
     *
     * - The client may spend no more than 5 us while in the callback
     * context.
     *
     * - It is highly recommended to use atomic operations for
     * synchronization needs.
     *
     * Failure to meet the operational contract may lead to an undefined
     * state with system stability and performance issues.
     */
  void* client_data;
    /**< Pointer to the client's data which is opaque to voice agent.
      *
      * The client_data is returned to the client each time the event
      * callback is triggered.
      */
} vagent_ivoice_cmd_register_t;


/** De-register command (synchronous blocking API.)
 *
 * The client uses this command to deregister from voice agent,
 * it will not able to request vocoder resource any further.
 *
 * - Returns VS_EOK, denotes call was successful.
 */
#define VAGENT_IVOICE_CMD_DEREGISTER ( 0x00013346 )

typedef struct vagent_ivoice_cmd_deregister_t {

  vagent_voice_client_type_t client_id;
    /**< voice agent client type. */
}
  vagent_ivoice_cmd_deregister_t;


/** Resource request command (asynchronous non-blocking API.)
 *
 * The client requests for a vocoder resource grant.
 *
 * If the vocoder resource is available, voice agent evaluates call state 
 * and grants resource via VAGENT_IVOICE_EVENT_RESOURCE_GRANT.
 *
 * If the vocoder resource is not available then VAGENT caches the pending
 * request and revokes the resource from current client.
 */
#define VAGENT_IVOICE_CMD_RESOURCE_REQUEST ( 0x00013347 )

typedef struct vagent_ivoice_cmd_resource_request_t {

  uint32_t asid;
    /**< Modem Active Subcription ID. */

  vagent_voice_client_type_t client_id;
    /**< voice agent client type. */
} 
  vagent_ivoice_cmd_resource_request_t;


/** Resource grant event (asynchronous event).
 *
 * Voice Agent sends this event to the client when the vocoder resource
 * is available.
 *
 * This event provides the vocoder resource ID i.e. VSID.
 */
#define VAGENT_IVOICE_EVENT_RESOURCE_GRANT ( 0x00013348 )

typedef struct vagent_ivoice_event_resource_grant_t {

  uint32_t asid;
    /**< Modem Active Subcription ID. */

  uint32_t vsid;
    /**< System level published voice system-ID. */
} 
  vagent_ivoice_event_resource_grant_t;


/** Resource revoke event (asynchronous event).
 *
 * Voice Agent sends this event to currently active client for revoking the 
 * vocoder resource. This generally happens during inter-rat handover
 * scenario which requires resource arbitration across different client on 
 * same susbcription.
 *
 * As a respone to this event, the client perform all the required cleanup 
 * and sends an acknowledgement via VAGENT_IVOICE_CMD_RESOURCE_RELEASE.
 */
#define VAGENT_IVOICE_EVENT_RESOURCE_REVOKE ( 0x00013349 )

typedef struct vagent_ivoice_event_resource_revoke_t {

  uint32_t asid;
    /**< Modem Active Subcription ID. */
} 
  vagent_ivoice_event_resource_revoke_t;
  

/** Resource release command. (aysnchronous non-blocking API)
 *
 * This commannd is usually sent by the client as a response to the event 
 * VAGENT_IVOICE_EVENT_RESOURCE_REVOKE, it indicating successful release of
 * vocoder resource by the client
 *
 * It is also possible and acceptable for the client to send this command
 * without a revoke event i.e. the client had released the vocoder resource on 
 * its own.
 */
#define VAGENT_IVOICE_CMD_RESOURCE_RELEASE ( 0x0001334A )

typedef struct vagent_ivoice_cmd_resource_release_t {

  uint32_t asid;
    /**< Modem Active Subcription ID. */

  vagent_voice_client_type_t client_id;
    /**< voice agent client type. */

} vagent_ivoice_cmd_resource_release_t;

#endif /* __VAGENT_IVOICE_IF_H__ */

