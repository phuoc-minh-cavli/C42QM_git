#ifndef __GSM_VOICE_SERVER_H__
#define __GSM_VOICE_SERVER_H__
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                       GSM VOICE SERVER API

GENERAL DESCRIPTION

   This file contains the interface definition for the GSM Voice Sever

EXTERNALIZED FUNCTIONS



INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2001-2015 Qualcomm Technologies, Inc.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //components/rel/geran.mpss/5.2.0/api/gsm_voice_server_if.h#1 $

when       who       what, where, why
--------   ---       ----------------------------------------------------------
21/05/15   pjr       Add support for commad GSM_IVOICE_CMD_SEND_SAMPLE_RATE
10/01/15   pjr       Initial Revision
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "stdint.h"
#include "sys.h"

/*===========================================================================
  Export Macros
===========================================================================*/

#ifdef __cplusplus
  #define GSM_EXTERNAL extern "C"
#else
  #define GSM_EXTERNAL extern
#endif /* __cplusplus */
  /* This marks an external API that is intended for public use. */


/*----------------------------------------------------------------------------
  GSM Call function.
----------------------------------------------------------------------------*/

/**
 * Provides a single API entry point into the GSM Protocol SW.
 *
 * @param[in] cmd_id Command identifier to execute.
 * @param[in] params Command parameters.
 * @param[in] size Size of the command parameters in bytes.
 *
 * @return
 * GSM_EOK (0) when successful.
 */
GSM_EXTERNAL uint32_t gsm_call (

  uint32_t cmd_id,
  void* params,
  uint32_t size

);


/****************************************************************************
 * GSM COMMON INTERFACE DEFINITION                                         *
 ****************************************************************************/

/**
 * Event callback provided by a client of the server.
 *
 * The client returns GSM_EOK from the callback when the client was successful
 * in queuing the event to its own thread context for processing.
 *
 * All other error codes indicate a problem.
 *
 * The event parameter pointer is valid only within the callback context.
 */
typedef uint32_t ( *gsm_icommon_event_callback_fn_t ) (

  void* session_context,
    /**< Client-provided session_context pointer from the OPEN command. */

  uint32_t event_id,
    /**< Event ID. See GSM_IXXX_EVENT_YYY. */

  void* params,
    /**< Pointer to an associated event data structure. */

  uint32_t size
    /**< Size of the event data structure. */

);

/****************************************************************************
 * GSM VOCODER INTERFACE DEFINITION                                         *
 ****************************************************************************/

/**
 * GSM Vocoder types.
 */
#define GSM_IVOCODER_ID_AMR ( 0x20010000 )
/** Adaptive Multi-Rate (AMR) vocoder format. */

#define GSM_IVOCODER_ID_AMRWB ( 0x20010001 )
/** Adaptive Multi-Rate Wideband (AMR-WB) vocoder format. */

#define GSM_IVOCODER_ID_EFR ( 0x20010002 )
/** GSM Enhanced Full Rate (EFR) vocoder format. */

#define GSM_IVOCODER_ID_FR ( 0x20010003 )
/** GSM Full Rate (EFR) vocoder format. */

#define GSM_IVOCODER_ID_HR ( 0x20010004 )
/** GSM Half Rate (EFR) vocoder format. */


/**
 * GSM Vocoder buffer structure.
 */
typedef struct gsm_ivocoder_buffer_t gsm_ivocoder_buffer_t;

struct gsm_ivocoder_buffer_t {

  uint32_t flags;
    /**<
      * Flags:
      * - Bit 0 set: This indicates that the buffer is filled with a valid
      *   speech frame when TRUE.
      *
      *   The client sets this when delivering valid speech frame in the
      *   uplink vocoder packet callback.
      *
      *   The GSM protocol software sets this when delivering valid speech
      *   frame in the downlink vocoder packet callback.
      */

  uint32_t vocoder_id;
    /**< Vocoder ID. See GSM_IVOCODER_ID_XXX. */

  void* frame_info;
    /**<
     * Frame information payload.
     *
     * The vocoder ID determines to what frame information data type, is
     * associated to.
     *
     * See gsm_ivocxxx_frame_info_t for the data structures.
     */

  uint8_t* frame;
    /**< Pointer to the vocoder packet data. */

  uint32_t size;
    /**<
      * Actual frame size of the vocoder packet data in bytes.
      *
      * If used in the downlink packet exchange context, the actual frame
      * size is the size of what the GSM protocol software wants to send to
      * the client. The GSM protocol software must set the value of this size.
      *
      * If used in the uplink packet exchange context, the actual frame size
      * is the size of what the client wants to send to the GSM protocol
      * software. The client must set the value of size.
      */

};


/**
 * Uplink/Downlink vocoder callback function type provided by client.
 */
typedef void ( *gsm_ivocoder_callback_fn_t ) (

  void* session_context,
   /**<
     * Client-provided session context pointer from the GSM_IVOICE_CMD_OPEN
     * command.
     */

  gsm_ivocoder_buffer_t* buffer
   /**<
     * Reference to vocoder buffer for exchange between the client and the GSM
     * protocol software.
     */

);


/****************************************************************************
 * GSM AMR VOCODER INTERFACE DEFINITION                                    *
 ****************************************************************************/

/**
 * AMR vocoder frame types.
 * Reference: 3gpp 26.101; AMR IF1 format, 3gpp 26.093
 */
#define GSM_IVOCAMR_FRAME_TYPE_SPEECH_GOOD ( 0x20010005 )
  /**< Good speech frame. */

#define GSM_IVOCAMR_FRAME_TYPE_SPEECH_BAD ( 0x20010006 )
  /**< Corrupt speech frame (bad CRC). */

#define GSM_IVOCAMR_FRAME_TYPE_SID_FIRST ( 0x20010007 )
  /**<
    * Marks the beginning of a comfort noise period (silence).
    * This does not contain information bits.
    */

#define GSM_IVOCAMR_FRAME_TYPE_SID_UPDATE ( 0x20010008 )
  /**< Comfort noise frame. Contains information bits. */

#define GSM_IVOCAMR_FRAME_TYPE_SID_BAD ( 0x20010009 )
  /**< Comfort noise frame (bad CRC). */

#define GSM_IVOCAMR_FRAME_TYPE_NO_DATA ( 0x2001000A )
  /**< No useful information (nothing to transmit). */

#define GSM_IVOCAMR_FRAME_TYPE_SPEECH_DEGRADED ( 0x2001000B )
  /**< Degraded speech frame. 3gpp 26.093 only. */

#define GSM_IVOCAMR_FRAME_TYPE_SPEECH_ONSET ( 0x2001000C )
  /**<
    * Onset -- Announces the beginning of a speech burst. This
    * does not contain information bits. 3gpp 26.093 only.
    */


/**
 * AMR vocoder codec modes. This denotes all the AMR codec bit rates.
 * Reference: 3gpp 26.201
 */
 #define GSM_IVOCAMR_CODEC_MODE_0475 ( 0x2001000D )
   /**< 4.75 kbps. */

 #define GSM_IVOCAMR_CODEC_MODE_0515 ( 0x2001000E )
   /**< 5.15 kbps. */

 #define GSM_IVOCAMR_CODEC_MODE_0590 ( 0x2001000F )
   /**< 5.90 kbps. */

 #define GSM_IVOCAMR_CODEC_MODE_0670 ( 0x20010010 )
   /**< 6.70 kbps. */

 #define GSM_IVOCAMR_CODEC_MODE_0740 ( 0x20010011 )
   /**< 7.40 kbps. */

 #define GSM_IVOCAMR_CODEC_MODE_0795 ( 0x20010012 )
   /**< 7.95 kbps. */

 #define GSM_IVOCAMR_CODEC_MODE_1020 ( 0x20010013 )
   /**< 10.20 kbps. */

 #define GSM_IVOCAMR_CODEC_MODE_1220 ( 0x20010014 )
   /**< 12.20 kbps. */


/**
 * AMR vocoder frame information structure.
 */
typedef struct gsm_ivocamr_frame_info_t gsm_ivocamr_frame_info_t;

struct gsm_ivocamr_frame_info_t {

  uint32_t frame_type;
    /**< AMR frame type. */

  uint32_t codec_mode;
    /**< AMR codec mode. */

};


/****************************************************************************
 * GSM AMR-WB VOCODER INTERFACE DEFINITION                                    *
 ****************************************************************************/

/**
 * AMR-WB vocoder frame types.
 * Reference: 3gpp 26.201; AMR-WB IF1 format, 26.193
 */
#define GSM_IVOCAMRWB_FRAME_TYPE_SPEECH_GOOD ( 0x20010015 )
  /**< Good speech frame. */

#define GSM_IVOCAMRWB_FRAME_TYPE_SPEECH_BAD ( 0x20010016 )
  /**< Corrupt speech frame (bad CRC). */

#define GSM_IVOCAMRWB_FRAME_TYPE_SPEECH_LOST ( 0x20010017 )
  /**< No frame received. */

#define GSM_IVOCAMRWB_FRAME_TYPE_SID_FIRST ( 0x20010018 )
  /**<
    * Marks the beginning of a comfort noise period (silence).
    * This does not contain information bits. */

#define GSM_IVOCAMRWB_FRAME_TYPE_SID_UPDATE ( 0x20010019 )
  /**< Comfort noise frame. Contains information bits. */

#define GSM_IVOCAMRWB_FRAME_TYPE_SID_BAD ( 0x2001001A )
  /**< Comfort noise frame (bad CRC). */

#define GSM_IVOCAMRWB_FRAME_TYPE_NO_DATA ( 0x2001001B )
  /**< Comfort noise frame (bad CRC). */

#define GSM_IVOCAMRWB_FRAME_TYPE_SPEECH_DEGRADED ( 0x2001001C )
  /**<
    * Speech frame with a good CRC, but there may
    * be errors in the class2 bits. 3gpp 26.193 only.
    */

#define GSM_IVOCAMRWB_FRAME_TYPE_ONSET ( 0x2001001D )
  /**<
    * Onset -- Announces the beginning of a speech burst. This
    * does not contain information bits. 3gpp 26.193 only.
    */


/**
 * AMR-WB vocoder rate types. This denotes all the AMR-WB codec bit-rates.
 * Reference: 3gpp 26.201
 */
#define GSM_IVOCAMRWB_CODEC_MODE_0660 ( 0x2001001E )
  /**< 6.60 kbps. */

#define GSM_IVOCAMRWB_CODEC_MODE_0885 ( 0x2001001F )
  /**< 8.85 kbps. */

#define GSM_IVOCAMRWB_CODEC_MODE_1265 ( 0x20010020 )
  /**< 12.65 kbps. */

#define GSM_IVOCAMRWB_CODEC_MODE_1425 ( 0x20010021 )
  /**< 14.25 kbps. */

#define GSM_IVOCAMRWB_CODEC_MODE_1585 ( 0x20010022 )
  /**< 15.25 kbps. */

#define GSM_IVOCAMRWB_CODEC_MODE_1825 ( 0x20010023 )
  /**< 18.25 kbps. */

#define GSM_IVOCAMRWB_CODEC_MODE_1985 ( 0x20010024 )
  /**< 19.85 kbps. */

#define GSM_IVOCAMRWB_CODEC_MODE_2305 ( 0x20010025 )
  /**< 23.05 kbps. */

#define GSM_IVOCAMRWB_CODEC_MODE_2385 ( 0x20010026 )
  /**< 23.85 kbps. */


/**
 * AMR-WB vocoder frame information structure.
 */
typedef struct gsm_ivocamrwb_frame_info_t gsm_ivocamrwb_frame_info_t;

struct gsm_ivocamrwb_frame_info_t {

  uint32_t frame_type;
    /**< AMR-WB frame type. */

  uint32_t codec_mode;
    /**< AMR-WB codec mode. */
};


/****************************************************************************
 * GSM Enhanced Full Rate (EFR) VOCODER INTERFACE DEFINITION                *
 ****************************************************************************/

/**
 * GSM Enhanced Full Rate (EFR) vocoder frame information structure.
 */
typedef struct gsm_ivocefr_frame_info_t gsm_ivocefr_frame_info_t;

struct gsm_ivocefr_frame_info_t {

  uint8_t bfi : 1;
    /**<
      * BIT(0): Bad Frame Indication (BFI); used for error concealment.
      * The BFI applies to downlink only. Unused for uplink.
      */

  uint8_t sid : 2;
    /**<
      * BIT(1-2): Silence Insertion Descriptor (SID);
      * Applies to both uplink and downlink:
      *    - 3 -- Reserved.
      *    - 2 -- Valid SID frame.
      *    - 1 -- Invalid SID frame.
      *    - 0 -- Speech frame.
      */

  uint8_t taf : 1;
    /**<
      * BIT(3): Time Alignment Flag (TAF). The TAF applies to downlink only.
      * Unused for uplink:
      *  - 1 -- Position of the SID frame within the Slow Associated Control
      *         Channel (SACCH) multi-frame.
      *  - 0 -- Frame is not aligned.
      */

  uint8_t reserved_bit_4 : 1; /**< Reserved. */
  uint8_t reserved_bit_5 : 1; /**< Reserved. */
  uint8_t reserved_bit_6 : 1; /**< Reserved. */
  uint8_t reserved_bit_7 : 1; /**< Reserved. */

};


/****************************************************************************
 * GSM Full Rate (FR) VOCODER INTERFACE DEFINITION                          *
 ****************************************************************************/

/**
 * GSM Full Rate (FR) vocoder frame information structure.
 */
typedef struct gsm_ivocfr_frame_info_t gsm_ivocfr_frame_info_t;

struct gsm_ivocfr_frame_info_t {

  uint8_t bfi : 1;
    /**<
      * BIT(0): Bad Frame Indication (BFI); used for error concealment.
      * The BFI applies to downlink only. Unused for uplink.
      */

  uint8_t sid : 2;
    /**<
      * BIT(1-2): Silence Insertion Descriptor (SID);
      * Applies to both uplink and downlink:
      *    - 3 -- Reserved.
      *    - 2 -- Valid SID frame.
      *    - 1 -- Invalid SID frame.
      *    - 0 -- Speech frame.
      */

  uint8_t taf : 1;
    /**<
      * BIT(3): Time alignment flag.
      * The TAF applies to downlink only. Unused for uplink: \n
      * - 1 -- Position of the SID frame within the Slow Associated Control
      *         Channel (SACCH) multi-frame.
      * - 0 -- Frame is not aligned.
      */

  uint8_t reserved_bit_4 : 1; /**< Reserved. */
  uint8_t reserved_bit_5 : 1; /**< Reserved. */
  uint8_t reserved_bit_6 : 1; /**< Reserved. */
  uint8_t reserved_bit_7 : 1; /**< Reserved. */

};


/****************************************************************************
 * GSM Half Rate (FR) VOCODER INTERFACE DEFINITION                          *
 ****************************************************************************/

/**
 * GSM Half Rate (HR) vocoder frame information structure.
 */
typedef struct  gsm_ivochr_frame_info_t gsm_ivochr_frame_info_t;

struct gsm_ivochr_frame_info_t {

  uint8_t bfi : 1;
    /**<
      * BIT(0): Bad frame indication (BFI); used for error concealment.
      * The BFI applies to downlink only. Unused for uplink.
      */

  uint8_t sid : 2;
    /**<
      * BIT(1-2): Silence Insertion Descriptor (SID);
      * Applies to both uplink and downlink:
      *    - 3 -- Reserved.
      *    - 2 -- Valid SID frame.
      *    - 1 -- Invalid SID frame.
      *    - 0 -- Speech frame.
      */

  uint8_t taf : 1;
    /**<
      * BIT(3): Time alignment flag (TAF);
      * The TAF applies to downlink only. Unused for uplink: \n
      * - 1 -- Position of the SID frame within the Slow Associated Control
      *        Channel (SACCH) multi-frame.
      * - 0 -- Frame is not aligned.
      */

  uint8_t ufi : 1;
    /**<
      * BIT(4): Unreliable Frame Indication (UFI); error concealment on
      * non-BFI frames.
      * The UFI applies to downlink only. Unused for uplink.
      */

  uint8_t reserved_bit_5 : 1; /**< Reserved. */
  uint8_t reserved_bit_6 : 1; /**< Reserved. */
  uint8_t reserved_bit_7 : 1; /**< Reserved. */

};

/****************************************************************************
 * GSM VOICE INTERFACE DEFINITION                                           *
 ****************************************************************************/

/** Open command (synchronous API).
 *
 * Use this command to acquire a handle to a new GSM voice session instance.
 *
 * The client must use the returned handle (ret_handle) provided by
 * the Open command to execute any subsequent commands.
 *
 * The GSM protocol software sends the voice traffic channel events to the
 * client who has opened a session for a particular subscription. The client
 * will use "asid" to indicate the subscription that it is interested in
 * receiving the events for.
 *
 * The client will use these events to set up voice session thereby
 * facilitating vocoder packet exchange between the GSM protocol software and
 * the client.
 *
 * The client provides uplink and downlink vocoder callback functions as part
 * of the Open command. The uplink callback function is called by the GSM
 * protocol software every time when a vocoder packet is required to be
 * transmitted on the uplink. The downlink callback function is called by the
 * GSM protocol software every time when a vocoder packet is available from
 * the network for decoding and playback.
 *
 * The GSM protocol software would ensure providing client with
 *   - valid reference to vocoder buffers for read/write.
 *   - call context to indicate the involved subscription.
 *
 * Upon a successful open, the command shall return GSM_EOK to the caller.
 */
#define GSM_IVOICE_CMD_OPEN ( 0x20010027 )

typedef struct gsm_ivoice_cmd_open_t gsm_ivoice_cmd_open_t;

struct gsm_ivoice_cmd_open_t {

  uint32_t* ret_handle;
   /**<
     * Returns the handle that the client must use when making subsequent
     * commands. */

  sys_modem_as_id_e_type asid;
    /**< Modem Active Subscription ID. */

  gsm_icommon_event_callback_fn_t event_cb;
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
      * - The client may not call any APIs on the GSM protocol software in the
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

  gsm_ivocoder_callback_fn_t ul_cb;
    /**<
      * Uplink callback: The client shall copy the vocoder packet available to
      * the buffer provided by the GSM protocol software for OTA.
	  */

  gsm_ivocoder_callback_fn_t dl_cb;
    /**<
      * Downlink callback: The client shall copy the vocoder packet available
      * in the buffer from the GSM protocol software for decoding.
      */

  void* session_data;
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


/** Close command (synchronous blocking API).
 *
 * Use this command to close the previously opened GSM voice session instance.
 *
 * The client may not use the open handle after receiving successful close
 * indication.
 *
 * Upon a successful close, the command shall return GSM_EOK to the caller.
 */
#define GSM_IVOICE_CMD_CLOSE ( 0x20010028 )

typedef struct gsm_ivoice_cmd_close_t gsm_ivoice_cmd_close_t;

struct gsm_ivoice_cmd_close_t {

  uint32_t handle;
    /**< Open handle. */

  void* client_data;
    /**< Client-provided data for the client's use. */

};


/** Codec mode event (Asynchronous event).
 *
 * The GSM protocol software sends this event as part voice traffic set up.
 *
 * If the GSM protocol software provides the codec mode information before
 * selecting vocoder type, the client shall cache the codec mode settings,
 * and then reapply when vocoder is available.
 *
 * If the average encoder rate information is not provided, then the default
 * codec mode corresponding to the active vocoder shall be used.
 *
 * The GSM protocol software may also send this event to select a new codec
 * mode during an active voice call.
 */
#define GSM_IVOICE_EVENT_REQUEST_CODEC_MODE ( 0x2001002B )

typedef struct gsm_ivoice_event_request_codec_mode_t gsm_ivoice_event_request_codec_mode_t;

struct gsm_ivoice_event_request_codec_mode_t {

  uint32_t codec_mode;
    /**< codec mode, See GSM_IVOCXXX_CODEC_MODE_YYY */

};


/** DTX mode event (Asynchronous event).
 *
 * The GSM protocol software sends this event as part of the voice traffic
 * set up.
 *
 * If the GSM protocol software provides the DTX mode information before
 * selecting vocoder type, the client shall cache the DTX mode information, and
 * then reapply when vocoder is available.
 *
 * If the DTX mode information is not provided, then the default DTX mode
 * corresponding to the active vocoder shall be used.
 *
 * The GSM protocol software may also send this event to select a new DTX
 * mode during an active voice call.
 */
#define GSM_IVOICE_EVENT_REQUEST_DTX_MODE ( 0x2001002C )

typedef struct gsm_ivoice_event_request_dtx_mode_t gsm_ivoice_event_request_dtx_mode_t;

struct gsm_ivoice_event_request_dtx_mode_t {

  uint32_t enable_flag;
      /**<
        * Enable flag:
        * - TRUE -- Enable DTX
        * - FALSE -- Disable DTX
        */

};


/** Voice traffic starting event. (asynchronous event)
 *
 * The GSM protocol software uses this event to request client to set up vocoder
 * and start the packet exchange.
 *
 * The GSM protocol software will send this event only after VFR timing is
 * stabilized. The GSM protocol software will also provide vocoder type as
 * part of this event.
 */
#define GSM_IVOICE_EVENT_START_REQUEST ( 0x2001002E )

typedef struct gsm_ivoice_event_start_request_t gsm_ivoice_event_start_request_t;

struct gsm_ivoice_event_start_request_t {

  uint32_t vocoder_id;
    /**< vocoder identifier, See GSM_IVOCODER_ID_XXX. */

};


/** Start command ( synchronous API ).
 *
 * This command shows the client's readiness for vocoder packet exchange with
 * the GSM protocol software.
 *
 * The client sends this command to the GSM protocol software as an
 * acknowledgement for the event GSM_IVOICE_EVENT_REQUEST_START.
 *
 * The GSM protocol software need not wait for this command, and need not
 * perform any actions.
 */
#define GSM_IVOICE_CMD_START ( 0x20010029 )

typedef struct gsm_ivoice_cmd_start_t gsm_ivoice_cmd_start_t;

struct gsm_ivoice_cmd_start_t {

  uint32_t handle;
    /**< Open handle. */

};


/** Voice traffic stopping event. (asynchronous event)
 *
 * The GSM protocol software uses this event to request client to stop vocoder
 * and the packet exchange.
 *
 * The GSM protocol stack will send this event during Intra-rat handover scenarios
 * such as: time slot handover, vocoder change, inter-cell handover, etc.
 *
 * VFR timing reconfiguration shall not be done by the GSM protocol software 
 * until vocoder is stopped and GSM_IVOICE_CMD_STOP is sent by the client.
 *
 * This event does not have any parameters.
 */
#define GSM_IVOICE_EVENT_STOP_REQUEST ( 0x2001002F )


/** Stop command (synchronous API.)
 *
 * This command shows the client cannot do any more vocoder packet exchange
 * with the GSM protocol software. The client sends this command to the
 * GSM protocol software as an acknowledgement for the event
 * GSM_IVOICE_EVENT_REQUEST_STOP.
 *
 * The GSM protocol software will not do any vocoder packet exchanges
 * after receiving this command from the client.
 *
 * The GSM protocol software will wait for this command before proceeding
 * with VFR timing reconfiguration during handovers.
 */
#define GSM_IVOICE_CMD_STOP ( 0x2001002A )

typedef struct gsm_ivoice_cmd_stop_t gsm_ivoice_cmd_stop_t;

struct gsm_ivoice_cmd_stop_t {

  uint32_t handle;
    /**< Open handle. */

};


/** Select owner event (asynchronous event).
 *
 * The GSM protocol software will send this event to the client in scenarios
 * (ex: SRVCC, IRAT HO) when it wants client to take the ownership of the
 * vocoder resource.
 *
 * If the vocoder resource is readily available, the voice agent will grant the
 * vocoder to the GSM protocol software's client immediately. If the vocoder 
 * resource is being used by other client, then the voice agent will revoke the 
 * vocoder resource from the current client and grant to the GSM protocol 
 * software's client.
 *
 * If the vocoder resource is not available, then the voice agent would not
 * grant the vocoder resource to GSM protocol software's client.
 *
 * This event does not have any parameters.
 */
#define GSM_IVOICE_EVENT_SELECT_OWNER ( 0x2001002D )

/** Send sample rate command (Synchronous Blocking API).
 *
 * The client shall use this command to provide current operating mode for 
 * the configured vocoder.
 *
 * The Server shall constitute a speech codec info indication and sends it
 * to the Call Manager. The server need not be caching the sample rate information.
 * 
 * After successful processing, the command shall return GSM_EOK to the caller.
 */

#define GSM_IVOICE_CMD_SEND_SAMPLE_RATE ( 0x20010030 )

typedef struct gsm_ivoice_cmd_send_sample_rate_t gsm_ivoice_cmd_send_sample_rate_t;

struct gsm_ivoice_cmd_send_sample_rate_t  {

  uint32_t handle;
    /**< Open handle. */

  uint32_t vocoder_id;
    /**< vocoder ID, See GSM_IVOCODER_ID_XXX. */

  uint32_t sample_rate;
     /**< Indicates current operating mode of configured vocoder.
         *    8000hz   : narrowband mode.
         *    16000hz : wideband mode.
         *
         *    In future higher operating modes might be supported.
         */  
};



/****************************************************************************
 * ERROR CODES DEFINITION                                                   *
 ****************************************************************************/

#define GSM_EOK          ( 0x00000000 ) /**< Success; completed with no errors. */
#define GSM_EFAILED      ( 0x00000001 ) /**< General failure. */
#define GSM_EBADPARAM    ( 0x00000002 ) /**< Bad operation parameter(s). */
#define GSM_EUNSUPPORTED ( 0x00000003 ) /**< Unsupported routine/operation. */
#define GSM_EVERSION     ( 0x00000004 ) /**< Unsupported version. */
#define GSM_EUNEXPECTED  ( 0x00000005 ) /**< Unexpected problem encountered. */
#define GSM_EPANIC       ( 0x00000006 ) /**< Unhandled problem occurred. */

#define GSM_ENORESOURCE  ( 0x00000007 ) /**< Unable to allocate resource(s). */
#define GSM_EHANDLE      ( 0x00000008 ) /**< Invalid handle. */
#define GSM_EALREADY     ( 0x00000009 ) /**< Operation is already processed. */
#define GSM_ENOTREADY    ( 0x0000000A ) /**< Operation is not ready to be processed. */
#define GSM_EPENDING     ( 0x0000000B ) /**< Operation is pending completion. */
#define GSM_EBUSY        ( 0x0000000C ) /**< Operation could not be accepted or processed. */
#define GSM_EABORTED     ( 0x0000000D ) /**< Operation aborted due to an error. */
#define GSM_EPREEMPTED   ( 0x0000000E ) /**< Operation preempted by a higher priority. */
#define GSM_ECONTINUE    ( 0x0000000F ) /**< Operation requests intervention to complete. */
#define GSM_EIMMEDIATE   ( 0x00000010 ) /**< Operation requests immediate intervention to complete. */
#define GSM_ENOTIMPL     ( 0x00000011 ) /**< Operation is not implemented. */
#define GSM_ENEEDMORE    ( 0x00000012 ) /**< Operation needs more data or resources. */
#define GSM_ELPC         ( 0x00000013 ) /**< Operation is a local procedure call. */
#define GSM_ENOMEMORY    ( 0x00000014 ) /**< Unable to allocate enough memory. */
#define GSM_ENOTEXIST    ( 0x00000015 ) /**< Item does not exist. */



#endif /* __GSM_VOICE_SERVER_H__ */

