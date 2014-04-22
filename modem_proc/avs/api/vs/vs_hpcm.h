#ifndef __VS_HPCM_H__
#define __VS_HPCM_H__

/**
  @file  vs_hpcm.h
  @brief This file contains vocoder hpcm interface definitions of the Vocoder 
         Services (VS).
*/

/*
  ============================================================================
  Copyright (C) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/api/vs/vs_hpcm.h#1 $
  $Author: pwbldsvc $
  ============================================================================
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "vs_common.h"

/****************************************************************************
 * VS HPCM TYPE DEFINITIONS
 ****************************************************************************/

#define VS_HPCM_TAP_POINT_STREAM_TX (0x00011289)
  /**< Default tap point location on the Tx stream path. */

#define VS_HPCM_TAP_POINT_STREAM_RX (0x0001128A)
  /**< Default tap point location on the Rx stream path. */

/** Mask that indicates the read buffer is filled. */
#define VSS_HPCM_MASK_READ_BUFFER ( 0x1 )

/** Mask that indicates the write buffer is consumed. */
#define VSS_HPCM_MASK_WRITE_BUFFER ( 0x2 )

/** Mask that indicates a timetick. */
#define VSS_HPCM_MASK_TIMETICK ( 0x4 )

/** Mask that indicates an error during read buffer operation. */
#define VSS_HPCM_MASK_READ_ERROR ( 0x8 )

/** Mask that indicates an error during write buffer operation. */
#define VSS_HPCM_MASK_WRITE_ERROR ( 0x10 )

/**
 * Vocoder HPCM tap-point mode configuration.
 */
typedef enum vs_hpcm_mode_t {

  VS_HPCM_READ = 0,
    /**< Client can read pcm data. */
  VS_HPCM_WRITE = 1,
    /**< Client can write pcm data. */
  VS_HPCM_READ_WRITE = 2,
    /**< Client can both read and write pcm data. */
} vs_hpcm_mode_t;


/** 
 * VS HPCM buffer structure.
 *
 * HPCM buffer allocation and de-allocation is responsibility of vocoder 
 * services, any memory management is not required by client.
 
 * The client shall read/write to these buffers as required.
 */
typedef struct vs_hpcm_buffer_t {

  uint32_t max_size;
    /**< It is a read-only field which is initialized by DSP.
      * 
      * This specifies the maximum possible data size. Any read/write by client 
      * shall not exceed beyond this value.
      */
  uint16_t sample_rate;
    /**< Indicates the sample rate of the buffer data.
      *  This field is updated by DSP for read buffer and by CLIENT for 
      *  write buffer.
      *
      *  @values:
      *    - 8000  (NB),
      *    - 16000 (WB).
      *    - 32000 (SWB).
      *    - 48000 (FB).
      */
  uint16_t num_channels;
    /**< It indicates the number of channels contained in the buffer data.
      *  This field is updated by DSP for read buffer and by CLIENT for 
      *  write buffer.
      *
      *  @values
      *  - 1 channel
      */
  uint32_t data_size;
    /**< Actual size of the pcm buffer data.
      *  This field is updated by DSP for read buffer indicating amount of data
      *  available from DSP.
      *
      *  This field is updated by DSP for write buffer indicating amount of data
      *  requested by DSP.
      */
  uint8_t* data;
    /**< It specifies the address of the buffer data. 
      *  This field is updated by DSP for read buffer and by CLIENT for 
      *  write buffer.
      */
} vs_hpcm_buffer_t;


/****************************************************************************
 *  VS HPCM COMMAND INTERFACE DEFINITIONS
 ****************************************************************************/
 
/** 
 * Open command (synchronous blocking API).
 *
 * Use this command to acquire a handle to vocoder HPCM session.
 *
 * The client must use the returned handle (ret_handle) provided by
 * the Open command to execute any subsequent commands on the vocoder HPCM 
 * session.
 */
#define VS_HPCM_CMD_OPEN ( 0x000132DA )

typedef struct vs_hpcm_cmd_open_t {

  uint32_t* ret_handle;
    /**< Returns the handle that the client must use when making subsequent
      * commands.
      */
  uint32_t vsid;
    /**< System level published/documented voice system ID. */
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
      * - It is highly recommended to use atomic operations for
      * synchronization needs.
      *
      * Failure to meet the operational contract may lead to an undefined
      * state with system stability and performance issues. Voice Services
      * does not provide any warnings for the above items.
      */
  void* session_context;
    /**< Pointer to the client's session context. The client stores its session
      *  context pointer here to retrieve its session control data structure,
      *  which the client uses to queue and signal events into its worker
      *  thread.
      *
      * The session_context is returned to the client each time the event
      * callback is triggered.
      */
} vs_hpcm_cmd_open_t;


/** 
 * Close command (asynchronous nonblocking API).
 *
 * Closes the already opened session.
 *
 * The client shall not use the open handle after receiving a 
 * successful Close indication.
 *
 * The VS_COMMON_EVENT_CMD_RESPONSE event is sent to the client to 
 * indicate success or failure.
 */
#define VS_HPCM_CMD_CLOSE ( 0x000132DB )

typedef struct vs_hpcm_cmd_close_t {

  uint32_t handle;
    /**< Open VS session handle. */
  void* token; 
    /**< Client provided command token for the client's use. */

} vs_hpcm_cmd_close_t;


/** 
 * Enable command (asynchronous nonblocking API).
 *
 * Requests to enabled vocoder hpcm.
 * 
 * Dependency: Client must set the required tap-point configuration
 * before enabling vocoder hpcm.
 */
#define VS_HPCM_CMD_ENABLE ( 0x000132DC )

typedef struct vs_hpcm_cmd_enable_t {

  uint32_t handle;
    /**< Open VS session handle. */
  void* token;
    /**< Client provided command token for the client's use. */

} vs_hpcm_cmd_enable_t;


/** 
 * Disable command (asynchronous nonblocking API).
 *
 * Disables the vocoder hpcm.
 *
 * The VS_COMMON_EVENT_CMD_RESPONSE event is sent to the client
 * to indicate success or failure.
 */
#define VS_HPCM_CMD_DISABLE ( 0x000132DD )

typedef struct vs_hpcm_cmd_disable_t {

  uint32_t handle;
    /**< Open VS session handle. */
  void* token;
    /**< Client provided command token for the client's use. */

} vs_hpcm_cmd_disable_t;


/** 
 * HPCM Buffer push command (asynchronous blocking API).
 *
 * The Client starts pushing first pcm buffer for a tap-point 
 * after recieving VS_HPCM_EVENT_NOTIFY_BUFFER for which Bit 2 is 
 * set.
 * 
 * The client set the push_mask appropriately while pushing pcm buffers
 * to vocoder services.
 */
#define VS_HPCM_CMD_PUSH_BUFFER ( 0x000132DE )

typedef struct vs_hpcm_cmd_push_buffer_t {

  uint32_t handle;
    /**< Open handle. */
  uint32_t tap_point_id;
    /**< Identifies the tap location for which this buffers are provided. */
  uint32_t push_mask;
    /**< Bitmask that indicates the buffer status.
      *
      *  @Bitmask description:
      *    Bit 0 - Denotes read buffer for reading pcm data.
      *    Bit 1 - Denotes write buffer for writing pcm data.
      */
  vs_hpcm_buffer_t* read_buffer;
    /**< vocoder-hpcm read buffer structure. */
  vs_hpcm_buffer_t* write_buffer;
    /**< vocoder-hpcm write buffer structure. */
} vs_hpcm_cmd_push_buffer_t;


/****************************************************************************
 *  VS HPCM EVENT INTERFACE DEFINITIONS
 ****************************************************************************/

/** 
 * HPCM Buffer notification (asynchronous event).
 *
 * This event notifies the input/output buffer status to the client
 * during steady state. 
 */
#define VS_HPCM_EVENT_NOTIFY_BUFFER ( 0x000132DF )

typedef struct vs_hpcm_event_notify_buffer_t {

  uint32_t tap_point_id;
    /**< Identifies the tap location for which this notification is issued. */
  uint32_t notify_mask;
    /**< Bitmask that indicates the buffer status.
      *
      *  @Bitmask description:
      *    Bit 0 - Denotes read buffer is filled.
      *    Bit 1 - Denotes write buffer is consumed.
      *    Bit 2 - Denotes Event corresponds to a timetick.
      *    Bit 3 - Denotes error during read buffer operation.
      *            It is set along with bit-0 during error operation. 
      *    Bit 4 - Denotes error during write buffer operation.
      *            It is set along with bit-1 during error operation.
      */
  vs_hpcm_buffer_t* read_buffer;
    /**< vocoder-hpcm read buffer structure. */
  vs_hpcm_buffer_t* write_buffer;
    /**< vocoder-hpcm write buffer structure. */
} vs_hpcm_event_notify_buffer_t;

#endif /* __VS_HPCM_H__ */

