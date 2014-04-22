/*======================================================================== */
/**
@file adsp_asm_session_commands.h

This file contains ASM session commands and events structures definitions.
*/

/*===========================================================================
NOTE: The description above does not appear in the PDF. 

      The audio_mainpage.dox file contains all file/group descriptions that are 
      in the output PDF generated using Doxygen and Latex. To edit or update 
      any of the file/group text in the PDF, edit the audio_mainpage.dox file 
      contact Tech Pubs.
===========================================================================*/

/*===========================================================================
Copyright (c) 2012 Qualcomm Technologies, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
======================================================================== */

/*========================================================================
Edit History

$Header: //components/rel/audio.tx/2.0/audio/api/avsaudio/inc/adsp_asm_session_commands.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
05/30/11   sw/leo  (Tech Pubs) Updated Doxygen comments for Interface Spec doc.
10/12/10   leo     (Tech Pubs) Edited/added Doxygen comments and markup.
04/15/10   rkc      Created file.

========================================================================== */

#ifndef _ADSP_ASM_SESSION_COMMANDS_H_
#define _ADSP_ASM_SESSION_COMMANDS_H_

#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/** @addtogroup asm_session_constants_macros
@{ */
/** Maximum number of possible session IDs. The IDs start from 1; 0 is
    reserved.
*/
#define ASM_MAX_SESSION_ID                                          (8)

/** Maximum number of sessions.
*/
#define ASM_MAX_NUM_SESSIONS                                        ASM_MAX_SESSION_ID

/** Maximum number of streams per session.
    @newpage
*/
#define ASM_MAX_STREAMS_PER_SESSION                                 (8)

/** @} */ /* end_addtogroup asm_session_constants_macros */

/** @addtogroup asm_session_cmd_run
@{ */
/** Starts all streams in a session. The command can take effect
    immediately, or it can be set to run at a specific absolute time.

    If the first buffer has a nonzero timestamp, it is rendered at the
    appropriate offset from the absolute start time according to its
    timestamp.

    The time value specified in this command is interpreted as a signed
    64-bit quantity. If the current wall clock is at a very small value and
    the client wants to set the start time in the past, this command allows
    negative time offsets or negative absolute time to be specified.

    @relevant_apr_hdr_fields
      Opcode -- ASM_SESSION_CMD_RUN_V2 \n
      Dst_port:
              - Session ID 1 -- ASM_MAX_SESSION_ID
              - Stream ID is ignored

    @apr_msg_payload
      @structure{asm_session_cmd_run_t}
      @tablespace
      @inputtable{ASM_SESSION_CMD_RUN_APR_msg_payload.tex}

    @prerequisites
      At least one stream in this session must have already been opened and
      not yet closed.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

    API_developer_note:ASM_SESSION_CMD_RUN from A family is replaced with ASM_SESSION_CMD_RUN_V2. Reason: endianness.
*/
#define ASM_SESSION_CMD_RUN_V2                                         0x00010DAA

/** @xreflabel{hdr:RunImmediate} Indicates that ASM_SESSION_CMD_RUN_V2 is run
    immediately.
*/
#define ASM_SESSION_CMD_RUN_START_TIME_RUN_IMMEDIATE                0

/** Indicates that ASM_SESSION_CMD_RUN_V2 is run at a given absolute time.
*/
#define ASM_SESSION_CMD_RUN_START_TIME_RUN_AT_ABSOLUTE_TIME         1

/** Indicates that ASM_SESSION_CMD_RUN_V2 is run at a specified offset from the
    current absolute time
*/
#define ASM_SESSION_CMD_RUN_START_TIME_RUN_AT_RELATIVE_TIME         2

/** @xreflabel{hdr:RunTimeReserved}
    Reserved value.
    Note: The use of ASM_SESSION_CMD_RUN_START_TIME_RESERVED is deprecated.
    Value 3 is no longer reserved, but used for ASM_SESSION_CMD_RUN_START_TIME_RUN_WITH_DELAY.
*/
#define ASM_SESSION_CMD_RUN_START_TIME_RESERVED                     3

/** @xreflabel{hdr:RunWithDelay}
    Indicates that ASM_SESSION_CMD_RUN is run with a delay.
    In other words, a delay is added to incoming timestamps and
    then rendering is done at the resulting time.
    Currently, this is applicable for ASM loopback (opened through ASM_STREAM_CMD_OPEN_LOOPBACK_V2) only.
*/
#define ASM_SESSION_CMD_RUN_START_TIME_RUN_WITH_DELAY               3

/** Bitmask used to specify the start time for the ASM_SESSION_CMD_RUN_V2
    command.
*/
#define ASM_BIT_MASK_RUN_START_TIME                                 (0x00000003UL)

/** Bit shift value used to specify the start time for the ASM_SESSION_CMD_RUN_V2
    command.
    @newpage
*/
#define ASM_SHIFT_RUN_START_TIME                                    0

/* Payload structure for an ASM Session Run command. */
typedef struct asm_session_cmd_run_v2_t asm_session_cmd_run_v2_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_SESSION_CMD_RUN_V2 command, which starts all
    streams in a playback session.\n
    \n
    This command can take effect immediately, or it can be set up to run at a
    specific absolute time. If the first buffer has a nonzero timestamp, it
    is rendered at the appropriate offset from the Absolute Start Time,
    according to whichever timestamp it has.

    'Run with delay' mode is useful to introduce delays in loopback path.
    This is particularly useful for AV sync, where audio needs to be delayed to match video delays.
    Before issuing this command the client needs to query the audio path delay using ASM_SESSION_CMD_GET_PATH_DELAY_V2 &
    compare with video delay. If video delay is greater, then the video delay is sent in this command.
*/
struct asm_session_cmd_run_v2_t
{
    uint32_t                  flags;
    /**< Specifies whether to run immediately or at a specific rendering
         time or with a specified delay. Run with delay is useful for delaying in case of ASM loopback
         opened through ASM_STREAM_CMD_OPEN_LOOPBACK_V2.
         Use #ASM_BIT_MASK_RUN_START_TIME and #ASM_SHIFT_RUN_START_TIME
         to set this 2-bit flag.

         Bits 0 and 1 can take one of four possible values:

         - #ASM_SESSION_CMD_RUN_START_TIME_RUN_IMMEDIATE
         - #ASM_SESSION_CMD_RUN_START_TIME_RUN_AT_ABSOLUTE_TIME
         - #ASM_SESSION_CMD_RUN_START_TIME_RUN_AT_RELATIVE_TIME
         - #ASM_SESSION_CMD_RUN_START_TIME_RUN_WITH_DELAY

        All other bits are reserved; clients must set them to zero. */

    uint32_t                  time_lsw;
    /**< Lower 32 bits of the time in microseconds used to align the session
         origin time.  When bits 0-1 of flags is ASM_SESSION_CMD_RUN_START_RUN_WITH_DELAY, time lsw is
         the lsw of the delay in us. For ASM_SESSION_CMD_RUN_START_RUN_WITH_DELAY, maximum value of the 64 bit delay is 150 ms.*/

    uint32_t                  time_msw;
    /**< Upper 32 bits of the time in microseconds used to align the session
         origin time. When bits 0-1 of flags is ASM_SESSION_CMD_RUN_START_RUN_WITH_DELAY, time msw is
         the msw of the delay in us. For ASM_SESSION_CMD_RUN_START_RUN_WITH_DELAY, maximum value of the 64 bit delay is 150 ms.*/
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_session_cmd_run */

/** @ingroup asm_session_cmd_pause
    Pauses all streams in a specified playback session.

    @relevant_apr_hdr_fields
      Opcode -- ASM_SESSION_CMD_PAUSE \n
      Dst_port:
              - Session ID 1 -- ASM_MAX_SESSION_ID
              - Stream ID is ignored

    @apr_msg_payload
      None.

    @prerequisites
      The specified session ID must have one or more streams assigned to it.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
    @newpage
*/
#define ASM_SESSION_CMD_PAUSE                                       0x00010BD3

/** @ingroup asm_session_cmd_get_session_time
    This command queries for the session time currently being rendered for the specified session as well as the absolute
    time in terms of a wallclock reading when the audio sample with the given session time was rendered to hardware.
    Note that the delays in the codec or in the hardware buffers etc are not included here because they are not visible to ADSP.

    @relevant_apr_hdr_fields
      Opcode -- ASM_SESSION_CMD_GET_SESSION_TIME_V3 \n
      Dst_port:
              - Session ID 1 -- ASM_MAX_SESSION_ID (0 is reserved)
              - Stream ID is ignored

    @apr_msg_payload
      None.

    @prerequisites
      This command must be applied to a valid session.

    @return
      #ASM_SESSION_CMDRSP_GET_SESSION_TIME_V3 with the status and 64-bit session
      time.

      API_developer_note: A family also had ASM_SESSION_CMD_GET_SESSION_TIME & ASM_SESSION_CMD_GET_SESSION_TIME_V2.
      V3 is introduced in B family because change in order of LSW and MSW in the response.

    @newpage
*/
#define ASM_SESSION_CMD_GET_SESSION_TIME_V3                            0x00010D9D

/** @addtogroup asm_session_cmd_register_rx_underflow
@{ */
/** Allows a client to register for an Rx session underflow event.

    @relevant_apr_hdr_fields
      Opcode -- ASM_SESSION_CMD_REGISTER_FOR_RX_UNDERFLOW_EVENTS \n
      Dst_port:
              - Session ID 1 -- ASM_MAX_SESSION_ID
              - Stream ID is ignored

    @apr_msg_payload
      @structure{asm_session_cmd_rgstr_rx_underflow_t}
      @tablespace
      @inputtable{ASM_SESSION_CMD_REG_FOR_RX_UNDERFLOW_EVENTS_APR_msg_payload.tex}

    @prerequisites
      This command must be applied to a valid session.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
    @newpage
*/
#define ASM_SESSION_CMD_REGISTER_FOR_RX_UNDERFLOW_EVENTS            0x00010BD5

/* Payload for an ASM session command for registering Rx underflow events. */
typedef struct asm_session_cmd_rgstr_rx_underflow_t asm_session_cmd_rgstr_rx_underflow_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_SESSION_CMD_REGISTER_FOR_RX_UNDERFLOW_EVENTS
    command, which is used to register Rx underflow events.\n
    \n
    A client can register for #ASM_SESSION_EVENT_RX_UNDERFLOW. This event is
    sent to the client handle present in the received Asynchronous Packet
    Router (APR) header.
*/
struct asm_session_cmd_rgstr_rx_underflow_t
{
    uint16_t                  enable_flag;
    /**< Specifies whether a client is to receive events when an Rx session
         underflows.

         Supported values:
         - 0 -- Do not send underflow events
         - 1 -- Send underflow events */
 
    uint16_t                  reserved;
    /**< Reserved. This field must be set to zero.
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_session_cmd_register_rx_underflow */

/** @addtogroup asm_session_cmd_register_tx_underflow
@{ */
/** Allows a client to register for a Tx session underflow event.

    @relevant_apr_hdr_fields
      Opcode -- ASM_SESSION_CMD_REGISTER_FOR_RX_UNDERFLOW_EVENTS \n
      Dst_port:
              - Session ID 1 -- ASM_MAX_SESSION_ID
              - Stream ID is ignored

    @apr_msg_payload
      @structure{asm_session_cmd_reg_tx_overflow_t}
      @tablespace
      @inputtable{ASM_SESSION_CMD_REG_FOR_TX_OVERFLOW_EVENTS_APR_msg_payload.tex}

    @prerequisites
      This command must be applied to a valid session.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
    @newpage
*/
#define ASM_SESSION_CMD_REGISTER_FOR_TX_OVERFLOW_EVENTS             0x00010BD6

/* Payload for an ASM session command for registering Tx overflow events. */
typedef struct asm_session_cmd_reg_tx_overflow_t asm_session_cmd_reg_tx_overflow_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_SESSION_CMD_REGISTER_FOR_TX_OVERFLOW_EVENTS
    command, which is used to register Tx overflow events.\n
    \n
    A client can register for #ASM_SESSION_EVENT_TX_OVERFLOW. This event
    is sent to the client handle present in the received APR header.
*/
struct asm_session_cmd_reg_tx_overflow_t
{
    uint16_t                  enable_flag;
    /**< Specifies whether a client is to receive events when a Tx session
         overflows.

         Supported values:
         - 0 -- Do not send overflow events
         - 1 -- Send overflow events */

    uint16_t                  reserved;
    /**< Reserved. This field must be set to zero.
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_session_cmd_register_tx_underflow */

/** @ingroup asm_session_event_rx_underflow
    Reports a data starvation in a session, resulting in insertion of silence
    to the renderer.

    @relevant_apr_hdr_fields
      Opcode -- ASM_SESSION_EVENT_RX_UNDERFLOW \n
      Src_port -- 0

    @apr_msg_payload
      None.

    @prerequisites
      None.

    @return
      None.
    @newpage
*/
#define ASM_SESSION_EVENT_RX_UNDERFLOW                              0x00010C17

/** @ingroup asm_session_event_tx_overflow
    Reports that the client has not provided enough empty read buffers and
    some samples had to be discarded.

    @relevant_apr_hdr_fields
      Opcode -- ASM_SESSION_EVENT_TX_UNDERFLOW \n
      Src_port -- 0

    @apr_msg_payload
      None.

    @prerequisites
      None.

    @return
      None.
    @newpage
*/
#define ASM_SESSION_EVENT_TX_OVERFLOW                               0x00010C18

/** @addtogroup asm_session_resp_get_session_time
@{ */
/** This command reports the time of the session that is currently rendering as well as the absolute time
 *  when the audio sample with the given session time gets rendered to hardware.
 *  Note that the delays in the codec or in the hardware buffers etc are not included here because they are not visible to ADSP.

    The time value in this command must be interpreted by clients as a signed
    64-bit value. A negative session time can be returned in cases where the
    client wants to start running a session in the future but first queries
    for the session clock.

    Presence of absolute timestamp helps in accurate AV-sync by allowing the client to obtain
    a more fine grained render time and also account for any messaging latency.

	The absolute time can be slightly in the past or future.

    @relevant_apr_hdr_fields
      Opcode -- ASM_SESSION_CMDRSP_GET_SESSION_TIME_V3 \n
      Dst_port:
              - Session ID 1 -- ASM_MAX_SESSION_ID (0 is reserved)
              - Stream ID is ignored

    @apr_msg_payload
      @structure{asm_session_cmdrsp_get_session_time_v3_t}
      @tablespace
      @inputtable{ASM_SESSION_CMDRSP_GET_SESSION_TIME_APR_msg_payload.tex}

    @prerequisites
      None.

    @return
      None.

      API_developer_note: A family also had ASM_SESSION_CMDRSP_GET_SESSION_TIME & ASM_SESSION_CMDRSP_GET_SESSION_TIME_V2.
      V3 is introduced in B family because change in order of LSW and MSW.

    @newpage
*/
#define ASM_SESSION_CMDRSP_GET_SESSION_TIME_V3                         0x00010D9E

/* Payload for an ASM session get time event. */
typedef struct asm_session_cmdrsp_get_session_time_v3_t asm_session_cmdrsp_get_session_time_v3_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_SESSION_CMDRSP_GET_SESSION_TIME_V3 message, which
    reports the rendering session time of a current session and the absolute time at which it gets rendered.
*/
struct asm_session_cmdrsp_get_session_time_v3_t
{
    uint32_t                  status;
    /**< Status message (error code).

         Supported values: Refer to @xhyperref{Q3,[Q3]} */

    uint32_t                  session_time_lsw;
    /**< Lower 32 bits of the current session time in microseconds.
         @newpage */

    uint32_t                  session_time_msw;
    /**< Upper 32 bits of the current session time in microseconds. */

    uint32_t                  absolute_time_lsw;
    /**< Lower 32 bits in µs of the absolute time at which the sample corresponding
     * to the above session time gets rendered to hardware. This absolute time may
     * be slightly in the future or past.
          */

    uint32_t                  absolute_time_msw;
    /**< Upper 32 bits in µs of the absolute time at which the sample corresponding
     * to the above session time gets rendered to hardware. This absolute time may be slightly
     * in the future or past.*/


}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_session_resp_get_session_time */

/** @addtogroup asm_session_cmd_adj_session_clock
@{ */
/** Adjusts the audio session clock for a specified session.

    The audio session clock adjustment is specified in microseconds.
    Positive values result in advancement of the clock, and negative values
    result in delay of the clock.

    @note1hang This command applies only to tunneled playback sessions;
               otherwise, an error is returned.

    @relevant_apr_hdr_fields
      Opcode -- ASM_SESSION_CMD_ADJUST_SESSION_CLOCK_V2 \n
      Dst_port:
              - Session ID 1 -- ASM_MAX_SESSION_ID
              - Stream ID is ignored
z
    @apr_msg_payload
      @structure{asm_session_cmd_adjust_session_clock_t}
      @tablespace
      @inputtable{ASM_SESSION_CMD_CMD_ADJUST_SESSION_CLOCK_APR_msg_payload.tex}

    @description
      When the client finds that the clock returned by 
      #ASM_SESSION_CMD_ADJUST_SESSION_CLOCK_V2 is behind the master A/V
      synchronization clock, the client can issue this command with a positive
      value to advance The clock or a negative value to delay the clock.
      Because the time specified in this command may not translate exactly
      into an integral number of audio samples to insert or delete, the
      command response indicates the actual time that the aDSP will adjust
      (accurate to the nearest microsecond). The client may notice the
      returned value and use the feedback loop to adjust The clock correctly.
        @negparspace
      Command processing latency (i.e., the time required for full adjustment
      of the session clock) is implementation specific. Typically, the larger
      The absolute value of the time specified in this command, the longer it
      takes to adjust the session clock. However, the command response is
      issued immediately, which provides latency to the command processing.
        @negparspace
      The client can issue this command any number of times and at any rate.
      When a new command is issued and if at this time the aDSP is still
      adjusting the session clock in response to a previous command, this
      adjustment is terminated and the aDSP starts processing the new command.
        @negparspace
      For command timing:
        @negparspace
      - If the command is issued when the session is running, the session
        clock adjustment begins immediately.
      - If the command is issued when the session is paused, the session clock
        does not begin adjusting until the session is moved to the Run state.
      - If the command is issued when the session is paused and then the
        session is flushed, the session clock adjustment is canceled. 

    @prerequisites
      At least one stream in this session must have already been opened and
      not yet closed.
      The streams must be tunneled playback streams opened through
      #ASM_STREAM_CMD_OPEN_WRITE_V2 or ASM_STREAM_CMD_OPEN_WRITE_COMPRESSED.

    @return
      #ASM_SESSION_CMDRSP_ADJUST_SESSION_CLOCK_V2

    API_developer_note: ASM_SESSION_CMD_ADJUST_SESSION_CLOCK from A family is deleted. Reason: endianness.
    B family starts with V2.

    @newpage
*/
#define ASM_SESSION_CMD_ADJUST_SESSION_CLOCK_V2                        0x00010D9F

/* Payload for the ASM session clock adjustment command. */
typedef struct asm_session_cmd_adjust_session_clock_v2_t asm_session_cmd_adjust_session_clock_v2_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_SESSION_CMD_ADJUST_SESSION_CLOCK_V2 command.\n
    \n
    This is a signed 64-bit quantity that specifies an adjustment to the
    session clock. Positive values indicate advancement of the session clock,
    and negative values indicate delay of the session clock.
*/
struct asm_session_cmd_adjust_session_clock_v2_t
{
   uint32_t                  adjust_time_lsw;
   /**< Lower 32 bits of the signed 64-bit quantity that specifies the
        adjustment time in microseconds to the session clock.

        Positive values indicate advancement of the session clock. Negative
        values indicate delay of the session clock. */

    uint32_t                  adjust_time_msw;
    /**< Upper 32 bits of the signed 64-bit quantity that specifies the
         adjustment time in microseconds to the session clock.

         Positive values indicate advancement of the session clock. Negative
         values indicate delay of the session clock. */
        /* @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_session_cmd_adj_session_clock */

/** @addtogroup asm_session_resp_adj_session_clock
@{ */
/** Response to an #ASM_SESSION_CMD_ADJUST_SESSION_CLOCK_V2 command. The response
    is returned immediately after receiving the command.

    @relevant_apr_hdr_fields
      Opcode -- ASM_SESSION_CMDRSP_ADJUST_SESSION_CLOCK_V2 \n
      Dst_port:
              - Session ID 1 -- ASM_MAX_SESSION_ID
              - Stream ID is ignored

    @apr_msg_payload
      @structure{asm_session_cmdrsp_adjust_session_clock_t}
      @tablespace
      @inputtable{ASM_SESSION_CMDRSP_ADJUST_SESSION_CLOCK_APR_msg_payload.tex}

    @description
      An error in the status code can happen if no streams are opened in
      The session or if the session is not a tunneled playback case.
        @negparspace
      Because the time specified in #ASM_SESSION_CMD_ADJUST_SESSION_CLOCK_V2 may
      not translate exactly into an integral number of audio samples to insert
      or delete, the command response indicates the actual time that the aDSP
      will adjust (accurate to the nearest microsecond). The client may note
      the returned value and use the feedback loop to adjust the clock
      correctly.
        @negparspace
      The command response also specifies the command processing latency,
      i.e., the amount of time (in microseconds) that the aDSP takes to
      complete the session clock adjustment.

    @prerequisites
      #ASM_SESSION_CMD_ADJUST_SESSION_CLOCK_V2 must have been issued.

    @return
      None.

      API_developer_note: ASM_SESSION_CMDRSP_ADJUST_SESSION_CLOCK from A family is deleted. Reason: endianness.
    B family starts with V2.

    @newpage
*/
#define ASM_SESSION_CMDRSP_ADJUST_SESSION_CLOCK_V2                     0x00010DA0

/* Payload for an ASM session clock adjustment command response. */
typedef struct asm_session_cmdrsp_adjust_session_clock_v2_t asm_session_cmdrsp_adjust_session_clock_v2_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_SESSION_CMDRSP_ADJUST_SESSION_CLOCK_V2 message.\n
    \n
    The signed 64-bit quantity specifies the actual adjustment to the session
    clock that is performed by the aDSP. Positive values indicate advancement
    of the session clock. Negative values indicate delay of the session clock.
    \n
    The unsigned 64-bit command latency (the time taken to perform the session
    clock adjustment) is also specified.
*/
struct asm_session_cmdrsp_adjust_session_clock_v2_t
{
    uint32_t                  status;
    /**< Status message (error code).

         Supported values: Refer to @xhyperref{Q3,[Q3]}

         An error means the session clock is not adjusted. In this case, the
         next two fields are irrelevant. */

    uint32_t                  actual_adjust_time_lsw;
    /**< Lower 32 bits of the signed 64-bit quantity that specifies the actual
         adjustment in microseconds performed by the aDSP.

         A positive value indicates advancement of the session clock. A
         negative value indicates delay of the session clock. */

    uint32_t                  actual_adjust_time_msw;
    /**< Upper 32 bits of the signed 64-bit quantity that specifies the actual
         adjustment in microseconds performed by the aDSP.

         A positive value indicates advancement of the session clock. A
         negative value indicates delay of the session clock. */

    uint32_t                  cmd_latency_lsw;
    /**< Lower 32 bits of the unsigned 64-bit quantity that specifies the
         amount of time in microseconds taken to perform the session clock
         adjustment.
         @newpage */

    uint32_t                  cmd_latency_msw;
    /**< Upper 32 bits of the unsigned 64-bit quantity that specifies the
         amount of time in microseconds taken to perform the session clock
         adjustment. */
 
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_session_resp_adj_session_clock */


/** @ingroup asm_session_cmd_get_path_delay
    This command is used to obtain the audio delay in this session for the specified stream.

   This command is used by the client for accurate AV sync purposes. Currently, this command is
   applicable for ASM loopback alone. In loopback path, this command gives the delay
   experienced by the audio data as it propagates from TX port to the RX port.
   Client can use this information to check if audio has to be delayed with respect
   to video or not.

   Note that change of calibration at a later point could change the actual audio delay.
   Hence this command gives delay at the time of query only. The client needs to make sure the session is established
   and is calibrated before the query.

    @apr_hdr_fields
      Opcode -- ASM_SESSION_CMD_GET_PATH_DELAY_V2  \n
      Dst_port:
              - Session ID 1 -- #ASM_MAX_SESSION_ID (0 is reserved)
              - Stream ID is ignored

    @apr_msg_payload
      None.

    @dependencies
      This command must be applied to a valid session. Entire session has to be established & calibrated before the query.

    @return
      #ASM_SESSION_CMDRSP_GET_PATH_DELAY with the status and delay values
    @newpage
*/
#define ASM_SESSION_CMD_GET_PATH_DELAY_V2                         0x00010DAF

/** @} */


/** @addtogroup asm_session_cmdresp_get_path_delay
@{ */
/**
   This is the response to the query ASM_SESSION_CMD_GET_PATH_DELAY_V2
   This response contains the audio delay for this session for the specified stream.
    @apr_hdr_fields
      Opcode -- ASM_SESSION_CMDRSP_GET_PATH_DELAY_V2 \n
      Dst_port:
              - Session ID 1 -- #ASM_MAX_SESSION_ID (0 is reserved)
              - Stream ID is ignored

    @apr_msg_payload
      @structure{asm_session_cmdrsp_get_path_delay_v2_t}
      @tablespace
      @inputtable{ASM_SESSION_CMDRSP_GET_PATH_DELAY_APR_msg_payload.tex}

    @dependencies
      ASM_SESSION_CMD_GET_PATH_DELAY_V2 command must be issued.

    @return
      None.
    @newpage
*/
#define ASM_SESSION_CMDRSP_GET_PATH_DELAY_V2                         0x00010DB0

/* Payload for an ASM session get path delay command response. */
typedef struct asm_session_cmdrsp_get_path_delay_v2_t asm_session_cmdrsp_get_path_delay_v2_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_SESSION_CMDRSP_GET_PATH_DELAY_V2 message, which
    reports the audio delay of this session.
*/
struct asm_session_cmdrsp_get_path_delay_v2_t
{
    uint32_t                  status;
    /**< Status message (error code). Whether this get delay operation is successful or not.
       Delay value is valid only if status is success.

       Supported values: Refer to @xhyperref{Q5,[Q5]} */

    uint32_t                  audio_delay_lsw;
    /**< Upper 32 bits of the aDSP delay in microseconds. */

    uint32_t                  audio_delay_msw;
    /**< Lower 32 bits of the aDSP delay  in microseconds. */

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_session_cmdresp_get_path_delay */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_ASM_SESSION_COMMANDS_H_ */
