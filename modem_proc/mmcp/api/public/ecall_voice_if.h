#ifndef __ECALL_VOICE_IF_H__
#define __ECALL_VOICE_IF_H__

/**
  @file  ecall_voice_if.h
  @brief This file contains resource interface definitions and 
         single entry point from Voice Agent to eCall.
*/

/*===========================================================================

                         E C A L L _ V O I C E _ I F . H

DESCRIPTION
  Resource interface definitions and single entry point from Voice Agent 
  to eCall.

REFERENCES

EXTERNALIZED FUNCTIONS  

INITIALIZATION AND SEQUENCING REQUIREMENTS
  
Copyright (c) 2009-2015 Qualcomm Technologies Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.

$Header: //components/rel/mmcp.mpss/6.1.10/api/public/ecall_voice_if.h#1 $
$DateTime: 2023/04/25 14:39:18 $
$Author: pwbldsvc $
===========================================================================*/


/*----------------------------------------------------------------------------
  Include files for Module
----------------------------------------------------------------------------*/

#include "sys.h"

/****************************************************************************
 * ERROR CODES DEFINITION                                                   *
 ****************************************************************************/

#define ECALL_VOICE_EOK          ( 0x00000000 ) /**< Success; completed with no errors. */
#define ECALL_VOICE_EFAILED      ( 0x00000001 ) /**< General failure. */
#define ECALL_VOICE_EBADPARAM    ( 0x00000002 ) /**< Bad operation parameter(s). */
#define ECALL_VOICE_EUNSUPPORTED ( 0x00000003 ) /**< Unsupported routine/operation. */
#define ECALL_VOICE_EVERSION     ( 0x00000004 ) /**< Unsupported version. */
#define ECALL_VOICE_EUNEXPECTED  ( 0x00000005 ) /**< Unexpected problem encountered. */
#define ECALL_VOICE_EPANIC       ( 0x00000006 ) /**< Unhandled problem occurred. */

#define ECALL_VOICE_ENORESOURCE  ( 0x00000007 ) /**< Unable to allocate resource(s). */
#define ECALL_VOICE_EHANDLE      ( 0x00000008 ) /**< Invalid handle. */
#define ECALL_VOICE_EALREADY     ( 0x00000009 ) /**< Operation is already processed. */
#define ECALL_VOICE_ENOTREADY    ( 0x0000000A ) /**< Operation is not ready to be processed. */
#define ECALL_VOICE_EPENDING     ( 0x0000000B ) /**< Operation is pending completion. */
#define ECALL_VOICE_EBUSY        ( 0x0000000C ) /**< Operation could not be accepted or processed. */
#define ECALL_VOICE_EABORTED     ( 0x0000000D ) /**< Operation aborted due to an error. */
#define ECALL_VOICE_EPREEMPTED   ( 0x0000000E ) /**< Operation preempted by a higher priority. */
#define ECALL_VOICE_ECONTINUE    ( 0x0000000F ) /**< Operation requests intervention to complete. */
#define ECALL_VOICE_EIMMEDIATE   ( 0x00000010 ) /**< Operation requests immediate intervention to complete. */
#define ECALL_VOICE_ENOTIMPL     ( 0x00000011 ) /**< Operation is not implemented. */
#define ECALL_VOICE_ENEEDMORE    ( 0x00000012 ) /**< Operation needs more data or resources. */
#define ECALL_VOICE_ELPC         ( 0x00000013 ) /**< Operation is a local procedure call. */
#define ECALL_VOICE_ENOMEMORY    ( 0x00000014 ) /**< Unable to allocate enough memory. */
#define ECALL_VOICE_ENOTEXIST    ( 0x00000015 ) /**< Item does not exist. */

/****************************************************************************
 * ECALL COMMON INTERFACE DEFINITION                                 *
 ****************************************************************************/

/**
 * Event callback provided by a client of the server.
 *
 * The client returns ECALL_VOICE_EOK from the callback when it was successful in
 * queuing the event to its own thread context for processing.
 *
 * All other error codes indicate a problem.
 *
 * The event parameter pointer is valid only within the callback context.
 */
typedef uint32 ( *ecall_icommon_event_callback_fn_t ) (

  void* session_context_ptr,
    /**< Client-provided session_context pointer from the OPEN command. */

  uint32 event_id,
    /**< Event ID. See ECALL_VOICE_IXXX_EVENT_YYY. */

  void* params_ptr,
    /**< Pointer to an associated event data structure. */

  uint32 size
    /**< Size of the event data structure. */

);
  /****************************************************************************
 * ECALL VOICE RESOURCE INTERFACE DEFINITION                                  *
 ****************************************************************************/

/** Register command (synchronous API).
 *
 * Use this command to acquire a handle to a ECALL VOICE instance.
 *
 * The client must use the returned handle (ret_handle) provided by
 * the Open command to execute any subsequent commands.
 *
 * The client will provide "as_id" as an indication of associated modem active
 * subscription.
 *
 * The client will take control of the voice resource to be used during
 * CVD session up.
 *
 * Upon a successful Open, the command shall return ECALL_VOICE_EOK to the client.
 *
 */
#define ECALL_VOICE_IRESOURCE_CMD_REGISTER ( 0x00010001 )

typedef struct ecall_voice_iresource_cmd_register_t ecall_voice_iresource_cmd_register_t;

struct ecall_voice_iresource_cmd_register_t {

  uint32* ret_handle_ptr;
    /**<
      * Returns the handle that the client must use when making subsequent
      * commands. */

  sys_vsid_type vsid;
    /**< System VSID. */

  ecall_icommon_event_callback_fn_t event_cb;
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

  void* session_context_ptr;
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

/** De-register command (synchronous API.)
 *
 * Use this command to de-register with ECALL module.
 *
 * ECALL shall be closing the voice instance opened during
 * ECALL_VOICE_IRESOURCE_CMD_REGISTER.
 *
 * The client shall not use the open handle after receiving successful
 * de-register indication.
 *
 */
#define ECALL_VOICE_IRESOURCE_CMD_DEREGISTER ( 0x00010002 )

typedef struct ecall_voice_iresource_cmd_deregister_t ecall_voice_iresource_cmd_deregister_t;

struct ecall_voice_iresource_cmd_deregister_t {

  uint32 handle;
    /**< Registration Handle. */

};

/**
 * The client sends this command to the ECALL for CVD session set-up.
 *
 * The client shall grant resource on availability basis given ECALL
 * has sent ECALL_VOICE_IRESOURCE_EVENT_REQUEST requesting 
 * resource. 
 */
#define ECALL_VOICE_IRESOURCE_CMD_GRANT ( 0x00010003 )

typedef struct ecall_voice_iresource_cmd_grant_t ecall_voice_iresource_cmd_grant_t;

struct ecall_voice_iresource_cmd_grant_t {

  uint32 handle;
    /**< Registration Handle. */

};


/**
 * ECALL sends this event requesting resources access for CVD
 * session set-up. 
 *
 * This does not have any parameter list.
 */
#define ECALL_VOICE_IRESOURCE_EVENT_REQUEST ( 0x00020001 )

/**
 * The ECALL sends this event to the client after successful release
 * and tear down of CVD sessions.
 * 
 * The ECALL sends this events also as acknowledgement of
 * ECALL_VOICE_IRESOURCE_CMD_REVOKE command from the client.
 *
 * This does not have any parameter list.
 */
#define ECALL_VOICE_IRESOURCE_EVENT_RELEASED ( 0x00020002 )


/*----------------------------------------------------------------------------
  ECALL VOICE external api function.
----------------------------------------------------------------------------*/

/**
 * Provides a single API entry point into the ECALL module.
 *
 * @param[in] cmd_id      Command identifier to execute.
 * @param[in] params_ptr  Command parameters.
 * @param[in] size        Size of the command parameters in bytes.
 *
 * @return
 * ECALL_VOICE_EOK (0) when successful
 */
uint32 ecall_voice_api
(
  uint32  cmd_id,
  void*     params_ptr,
  uint32  size
);

#endif /* __ECALL_VOICE_IF_H__ */

