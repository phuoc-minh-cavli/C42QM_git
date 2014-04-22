#ifndef __GVA_IRESOURCE_IF_H__
#define __GVA_IRESOURCE_IF_H__

/**
  @file  gva_iresource_if.h
  @brief This file contains resource interface definitions of the GSM Voice
         Adapter.
*/

/*
  ============================================================================

   Copyright (C) 2015 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================

                             Edit History

  $Header: //components/rel/avs.mpss/8.0.c12/vsd/vadapter/inc/CT/protected/gva_iresource_if.h#1 $
  $Author: pwbldsvc $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------


  ============================================================================
*/


/*----------------------------------------------------------------------------
  Include files for Module
----------------------------------------------------------------------------*/

#include "gva_icommon_if.h"

/****************************************************************************
 * GVA VOICE INTERFACE DEFINITION                                           *
 ****************************************************************************/

/** Register command (synchronous API).
 *
 * Voice agent shall use this command to acquire a handle to a GVA instance.
 *
 * Voice agent must use the returned handle (ret_handle) provided by the Open 
 * command to execute any subsequent commands.
 *
 * GVA shall be requesting resource token to voice agent in response of traffic 
 * channel start request from access stratum.
 *
 * If resource is available voice agent grants the token immediately.
 * If resource are not available, voice agent caches the request (LIFO based)
 * for GVA, as grants the voice resource to GVA as soon as its available. 
 *
 * Upon a successful register, the command shall return GVA_EOK to the client.
 *
 */
#define GVA_IRESOURCE_CMD_REGISTER ( 0x00013177 )

typedef struct gva_iresource_cmd_register_t gva_iresource_cmd_register_t;

struct gva_iresource_cmd_register_t {

  uint32_t* ret_handle;
   /**<
     * Returns the handle that the client must use when making subsequent
     * commands. */

  uint32_t vsid;
    /**< Voice System ID as defined by DCN 80-NF711-1 Rev E. */

  gva_icommon_event_callback_fn_t event_cb;
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
      * - The client may not call any APIs on the GSM voice adapter in the
      * callback context. This will cause synchronization issues for the driver
      * and may lead to a system failure or deadlock.
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

  void* session_context;
    /**<
      * Pointer to the session data. The client stores its session
      * context pointer here to retrieve its session control data structure,
      * which the client uses to queue and signal events into its worker
      * thread.
      *
      * The session_data is returned to the client each time an event
      * callback is triggered.
      */

};


/** De-register command (synchronous blocking API.)
 *
 * This command deregisters from GSM Voice Adapter instance.
 *
 * Voice agent shall not use the open handle after receiving successful close
 * indication.
 *
 * Upon a successful deregister, the command shall return GVA_EOK to the caller.
 */
#define GVA_IRESOURCE_CMD_DEREGISTER ( 0x00013178 )

typedef struct gva_iresource_cmd_deregister_t gva_iresource_cmd_deregister_t;

struct gva_iresource_cmd_deregister_t {

  uint32_t handle;
    /**< Registration Handle. */

};


/**
 * GVA sends this event to the client requesting to enable the voice 
 * resource created using GVA_IRESOURCE_CMD_REGISTER.
 *
 * This event does not have any parameters.
 */
#define GVA_IRESOURCE_EVENT_REQUEST ( 0x00013179 )


/**
 * Voice agent sends this command to the GVA for vocoder session set-up.
 *
 * Voice agent shall grant voice resource provided GVA has requested for
 * the resource token via #GVA_IRESOURCE_EVENT_REQUEST. 
 */
#define GVA_IRESOURCE_CMD_GRANT ( 0x0001317A )

typedef struct gva_iresource_cmd_grant_t gva_iresource_cmd_grant_t;

struct gva_iresource_cmd_grant_t {

  uint32_t handle;
    /**< Registration Handle. */

};


/**
 * Voice agent shall issue this command to GVA, to tear down the vocoder 
 * session and release the voice resource.
 *
 * GVA shall send GVA_IRESOURCE_EVENT_RELEASED notifying voice agent that
 * the voice resource is successfully released. 
 */
#define GVA_IRESOURCE_CMD_REVOKE ( 0x0001317B )

typedef struct gva_iresource_cmd_revoke_t gva_iresource_cmd_revoke_t;

struct gva_iresource_cmd_revoke_t {

  uint32_t handle;
    /**< Registration Handle. */

};


/**
 * This events is an indication that the voice resource has been
 * successfully released.
 *
 * GVA shall notify this event to voice agent: Either Voice agent sends 
 * GVA_IRESOURCE_CMD_REVOKE to GVA or GVA tears down the vocoder and releases 
 * the voice resource without any revoke from voice agent.
 *
 * This event does not have any parameters.
 */
#define GVA_IRESOURCE_EVENT_RELEASED ( 0x0001317C )

#endif /* __GVA_IRESOURCE_IF_H__ */

