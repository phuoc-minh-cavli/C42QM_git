#ifndef __VS_VOC_H__
#define __VS_VOC_H__

/**
  @file  vs_voc.h
  @brief This file contains vocoder interface definitions of the Vocoder
         Services (VS).
*/

/*
  ============================================================================
  Copyright (C) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/api/vs/vs_voc.h#1 $
  $Author: pwbldsvc $
  ============================================================================
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "vs_common.h"

/****************************************************************************
 * VS TYPE DEFINITIONS
 ****************************************************************************/

/**
 * Vocoder operating mode.
 */
typedef enum vs_voc_bandwidth_t {

  VS_VOC_BANDWIDTH_NB,
    /**< 8000 Khz. */
  VS_VOC_BANDWIDTH_WB,
    /**< 16000 Khz. */
  VS_VOC_BANDWIDTH_SWB,
    /**< 32000 Khz. */
  VS_VOC_BANDWIDTH_FB,
    /**< 48000 Khz. */
} vs_voc_bandwidth_t;


/****************************************************************************
 *  VS VOC COMMAND INTERFACE DEFINITIONS
 ****************************************************************************/
 
/** 
 * Open command (synchronous blocking API).
 *
 * Use this command to acquire a handle to a new vocoder session.
 *
 * The client must use the returned handle (ret_handle) provided by
 * the Open command to execute any subsequent commands on the vocoder session.
 *
 * The client is permitted to send any vocoder data commands to the vocoder
 * session after successfully calling the Open command. The client is
 * permitted to call vocoder data commands before enabling the vocoder
 * session.
 *
 * Each voice system controls at most one vocoder resource. Only one
 * client per voice system is granted the vocoder resource. The capability 
 * to run multiple vocoder resources across multiple voice systems
 * is a product-specific implementation and may not be the same across
 * products.
 */
#define VS_VOC_CMD_OPEN ( 0x000132BD )

typedef struct vs_voc_cmd_open_t {

  uint32_t* ret_handle;
    /**< Returns the handle that client uses for subsequent commands. */
  uint32_t vsid;
    /**< System level published voice system-ID. */
  uint32_t network_id;
   /**< Specifies the network type.
     *
     *  @values, refer to VS_COMMON_NETWORK_ID_XXXX definitions.
     */
  vs_common_event_callback_fn_t event_cb;
    /**< Central event callback function, which receives
      *  asynchronous events from the voice driver.
      *
      * Operational contract:
      *
      * - The client may only queue the incoming event and signal a worker
      * thread to process the event. The client must not perform any other
      * processing in the callback context.
      *
      * - The client may not call any APIs on the voice driver in the
      * callback context. This will cause synchronization issues for the
      * driver and may lead to a system failure or deadlock.
      *
      * - The client may not perform any blocking operations or acquire any
      * locks in the callback context that lead to a system deadlock.
      *
      * - The client may spend no more than 5 us while in the callback
      * context.
      *
      * - It is highly recommended to use atomic operations for
      * synchronization needs.
      *
      * Failure to meet the operational contract may lead to an undefined
      * state with system stability and performance issues. Voice Services
      * does not provide any warnings for the above items.
      */
  void* session_context;
    /**< Pointer to the session context. The client stores its session
      *  context pointer here to retrieve its session control data structure,
      *  which the client uses to queue and signal events into its worker
      *  thread.
      *
      * The session_context is returned to the client each time the event
      * callback is triggered.
      */
} vs_voc_cmd_open_t;


/** 
 * Close command (asynchronous nonblocking API).
 *
 * The client uses this command to closes the already opened session, 
 * open VS handle shall not be used after receiving a successful close indication.
 *
 * The VS_COMMON_EVENT_CMD_RESPONSE event is sent to the client to 
 * indicate success or failure.
 */
#define VS_VOC_CMD_CLOSE ( 0x000132BE )

typedef struct vs_voc_cmd_close_t {

  uint32_t handle;
    /**< Open VS session handle. */
  void* token; 
    /**< Client provided command token for the client's to track command
      *  response for close command.
      */
} vs_voc_cmd_close_t;


/** 
 * Enable command (asynchronous nonblocking API).
 *
 * Requests that a vocoder resource to be enabled.
 *
 * The vocoding does not actually begin until the client receives a
 * VS_VOC_EVENT_READY event.
 * 
 * The vocoding stops when the voice driver generates VS_VOC_EVENT_NOT_READY
 * event to the client. This can occur at any time. Some reasons can be due to,
 * but not limited to, a DSP crash, preemptions, calls to the VS_VOC_CMD_DISABLE
 * commands, VS_VOC_CMD_CLOSE command, a device switch, and others.
 *
 * Calling VS_VOC_CMD_ENABLE more than once is not permitted. The client must
 * first call VS_VOC_CMD_DISABLE before calling VS_VOC_CMD_ENABLE again to
 * request a different vocoder resource.
 *
 * The VS_COMMON_EVENT_CMD_RESPONSE event is sent to the client to indicate
 * success or failure.
 */
#define VS_VOC_CMD_ENABLE ( 0x000132BF )

typedef struct vs_voc_cmd_enable_t {

  uint32_t handle;
    /**< Open VS session handle. */
  void* token; 
    /**< Client provided command token for the client's to track command
      *  response for enable command.
      */
} vs_voc_cmd_enable_t;


/** 
 * Disable command (asynchronous nonblocking API).
 *
 * Disables the vocoder resource.
 *
 * The VS_VOC_EVENT_NOT_READY event is sent to the client only if the
 * client has previously received a VS_VOC_EVENT_READY event.
 *
 * The VS_COMMON_EVENT_CMD_RESPONSE event is sent to the client to indicate
 * success or failure.
 */
#define VS_VOC_CMD_DISABLE ( 0x000132C0 )

/** Vocoder Disable command. */
typedef struct vs_voc_cmd_disable_t {

  uint32_t handle;
    /**< Open VS session handle. */
  void* token;
    /**< Client provided command token for the client's to track command
      *  response for disable command.
      */
} vs_voc_cmd_disable_t;


/****************************************************************************
 *  VS VOC EVENT INTERFACE DEFINITIONS
 ****************************************************************************/

/**
 * Vocoder ready event (asynchronous API).
 *
 * It indicates the vocoder readiness of packet exchange.
 *
 * There are no parameters to this event.
 */
#define VS_VOC_EVENT_READY ( 0x000132C1 )


/**
 * Vocoder not ready event (asynchronous API).
 *
 * It indicates the vocoder packet exchange is stalled from
 * vocoder perspective, it could be due to device switch, PDR, 
 * SSR, etc.
 *
 * There are no parameters to this event.
 */
#define VS_VOC_EVENT_NOT_READY ( 0x000132C2 )


/**
 * Operating mode update (asynchronous event).
 * 
 * VS notifies the operating TX/RX bandwidth to client.
 */
#define VS_VOC_EVENT_OPERATING_MODE_UPDATE ( 0x000132C3 )

typedef struct vs_voc_event_operating_mode_update_t {

  vs_voc_bandwidth_t rx_mode;
    /**< Decoder/RX bandwidth. */ 
  vs_voc_bandwidth_t tx_mode;
    /**< Encoder/TX bandwidth. */ 
} vs_voc_event_operating_mode_update_t;


/**
 * Audio path delay event (asynchronous API)
 *
 * Voice services notifies the client with audio processing delay for 
 * TX & RX path during:
 * 	  - vocoder set-up.
 *    - Any subsequent change that is identified runtime.
 */
#define VS_VOC_EVENT_PATH_DELAY ( 0x000132C4 )

typedef struct vs_voc_event_path_delay_t {

  uint32_t rx_path_delay_us;
    /**< Rx voice path delay in microseconds. */
  uint32_t tx_path_delay_us;
    /**< Tx voice path delay in microseconds. */
} vs_voc_event_path_delay_t;

#endif /* __VS_VOC_H__ */

