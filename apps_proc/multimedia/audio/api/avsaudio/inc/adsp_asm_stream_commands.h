/*========================================================================*/
/**
@file adsp_asm_stream_commands.h

This file contains ASM stream commands and events structures definitions.
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

$Header: //components/rel/audio.tx/2.0/audio/api/avsaudio/inc/adsp_asm_stream_commands.h#1 $

when       who     what, where, why
--------   ---      -------------------------------------------------------
05/30/11   sw/leo  (Tech Pubs) Updated Doxygen comments for Interface Spec doc.
04/22/11    ss     Introducing Audproc API file that would hold all post processing 
                   Module IDs, Parameter IDs and Topology IDs
10/12/10   leo     (Tech Pubs) Edited/added Doxygen comments and markup.
04/15/10   rkc      Created file.

========================================================================== */

#ifndef _ADSP_ASM_STREAM_COMMANDS_H_
#define _ADSP_ASM_STREAM_COMMANDS_H_

#include "mmdefs.h"
#include "adsp_audproc_api.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/** @addtogroup asm_strm_cmd_open_write_v2
@{ */

/** Enumeration for a device matrix as endpoint.
*/
#define ASM_STREAM_END_POINT_DEVICE_MTX                             0

/** @xreflabel{hdr:AsmBitMaskSrCmChgNotFlag}
    Bitmask for an SR/CM change notification flag.
*/
#define ASM_BIT_MASK_SR_CM_CHANGE_NOTIFY_FLAG                       (0x00000004UL)

/** @xreflabel{hdr:AsmShiftSrCmChgNotFlag}
    Bit shift for an SR/CM change notification flag.
*/
#define ASM_SHIFT_SR_CM_CHANGE_NOTIFY_FLAG                          2

/** @xreflabel{hdr:AsmBitMaskGaplessModeFlag}
    Bitmask for Gapless mode.
*/
#define ASM_BIT_MASK_GAPLESS_MODE_FLAG                              (0x80000000UL)

/** @xreflabel{hdr:AsmShiftGaplessModeFlag}
    Bit shift for Gapless mode.
    @newpage
*/
#define ASM_SHIFT_GAPLESS_MODE_FLAG                                 31

/** @xreflabel{hdr:ASMStreamCmdOpenWriteV2}
    Creates an audio stream for playback. There is one decoder and one
    postprocessor chain. For PCM playback, there is no decoder.

    This command allows 16 and 24 bits per sample.

    @relevant_apr_hdr_fields
      Opcode -- ASM_STREAM_CMD_OPEN_WRITE_V2 \n
      Dst_port:
               - Session ID 1 -- ASM_MAX_SESSION_ID (0 is reserved)
               - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

    @newpage
    @apr_msg_payload
      @structure{asm_stream_cmd_open_write_v2_t}
      @tablespace
      @inputtable{ASM_STREAM_CMD_OPEN_WRITE_V2_APR_msg_payload.tex}

    @description
      When creating a stream, the client must specify the group and stream
      IDs. The current release supports up to ASM_MAX_SESSION_ID groups, and
      each group can have up to ASM_MAX_STREAMS_PER_SESSION streams.\n
        @vertspace
      This command informs of the media format of the incoming data. The media
      format block must be communicated to the aDSP through the
      ASM_DATA_CMD_MEDIA_FMT_UPDATE_V2 command before sending the actual data.\n
        @vertspace
      To achieve gapless playback, the client must open successive streams
      with the same session ID, but different stream IDs. Up to two streams
      can be open at the same time per session in Gapless mode. If the client 
      tries to open a stream when two streams are already open in the session,
      an ADSP_EALREADY error code is returned. MIDI and DTMF formats are not
      supported in gapless playback. Any attempt to open streams in these
      formats in Gapless Playback mode returns the error code, ADSP_EUNSUPPORTED.

    @prerequisites
      None.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).\n
      The handle is defined by the client.

    API_developer_note: A family also had ASM_STREAM_CMD_OPEN_WRITE. Since
    ASM_STREAM_CMD_OPEN_WRITE_V2 is more general,  ASM_STREAM_CMD_OPEN_WRITE is removed in
    B family.

    @newpage
*/
#define ASM_STREAM_CMD_OPEN_WRITE_V2                                   0x00010D8F

/* Payload format for a stream open write command. */
typedef struct asm_stream_cmd_open_write_v2_t asm_stream_cmd_open_write_v2_t;

#include "adsp_begin_pack.h"

/** NEW @brief Payload of the #ASM_STREAM_CMD_OPEN_WRITE_V2 command, which
    opens a write stream for a given session ID and stream ID.\n
    \n
    The dst_port field in the APR packet contains both the session ID and the
    stream ID.\n
    \n
    After successfully opening the stream, a media format update command must
    be sent in the data path to configure the decoder before sending any data.
*/
struct asm_stream_cmd_open_write_v2_t
{
    
    uint32_t                    mode_flags;
    /**< Mode flags that configure the stream to notify the client whenever
         it detects an SR/CM change at the input to its POPP.

         Supported values for bits 0 to 1:
         - Reserved; clients must set them to zero.

         Supported values for bit 2:
         - 0 -- SR/CM change notification event is disabled.
         - 1 -- SR/CM change notification event is enabled.
         - Use #ASM_BIT_MASK_SR_CM_CHANGE_NOTIFY_FLAG and
           #ASM_SHIFT_SR_CM_CHANGE_NOTIFY_FLAG to set or get this bit.

         Supported values for bit 31:
         - 0 -- Stream to be opened in on-Gapless mode.
         - 1 -- Stream to be opened in Gapless mode. In Gapless mode,
                successive streams must be opened with same session ID but
                different stream IDs.
         - Use #ASM_BIT_MASK_GAPLESS_MODE_FLAG and #ASM_SHIFT_GAPLESS_MODE_FLAG
           to set or get this bit.

        @note1hang MIDI and DTMF streams cannot be opened in Gapless mode. */

    uint16_t                    sink_endpoint_type;
    /**< Sink point type.

         Supported values:
         - 0 -- Device matrix
         - Other values are reserved.

         The device matrix is the gateway to the hardware ports. */

    uint16_t                    bits_per_sample;
     /**< Number of bits per sample processed by ASM modules.
      
            Supported values: 16 and 24 bits per sample */

    uint32_t                    postproc_topo_id;
    /**< Specifies the topology (order of processing) of postprocessing
         algorithms. <i>None</i> means no postprocessing.

         Supported values:
         - #ASM_STREAM_POSTPROC_TOPO_ID_DEFAULT
         - #ASM_STREAM_POSTPROC_TOPO_ID_MCH_PEAK_VOL
         - #ASM_STREAM_POSTPROC_TOPO_ID_NONE
         
         This field can also be enabled through SetParams flags. */

    uint32_t                    dec_fmt_id;
    /**< Configuration ID of the decoder media format.

         Supported values:
         - #ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2
         - #ASM_MEDIA_FMT_ADPCM
         - #ASM_MEDIA_FMT_MP3
         - #ASM_MEDIA_FMT_AAC_V2
         - #ASM_MEDIA_FMT_DOLBY_AAC
         - #ASM_MEDIA_FMT_AMRNB_FS
         - #ASM_MEDIA_FMT_AMRWB_FS
         - #ASM_MEDIA_FMT_AMR_WB_PLUS_V2
         - #ASM_MEDIA_FMT_V13K_FS
         - #ASM_MEDIA_FMT_EVRC_FS
         - #ASM_MEDIA_FMT_EVRCB_FS
         - #ASM_MEDIA_FMT_EVRCWB_FS
         - #ASM_MEDIA_FMT_SBC
         - #ASM_MEDIA_FMT_WMA_V10PRO_V2
         - #ASM_MEDIA_FMT_WMA_V9_V2
         - #ASM_MEDIA_FMT_AC3_DEC
         - #ASM_MEDIA_FMT_EAC3_DEC
         - #ASM_MEDIA_FMT_G711_ALAW_FS
         - #ASM_MEDIA_FMT_G711_MLAW_FS
         - #ASM_MEDIA_FMT_G729A_FS
         - #ASM_MEDIA_FMT_FR_FS
         - #ASM_MEDIA_FMT_VORBIS
           - #ASM_MEDIA_FMT_FLAC
         - #ASM_MEDIA_FMT_EXAMPLE
         - #ASM_MEDIA_FMT_DTS
         - #ASM_MEDIA_FMT_DTS_LBR
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_strm_cmd_open_write_v2 */

/** @addtogroup asm_strm_cmd_open_write_v3
@{ */
/** @xreflabel{hdr:ASMStreamCmdOpenWriteV3}
    Creates an audio stream for playback. There is one decoder and one
    postprocessor chain. For PCM playback, there is no decoder.

    This command allows 16 and 24 bits per sample.
    Unlike ASM_STREAM_CMD_OPEN_WRITE_V2 command, In this command, a new sub field
    "stream_perf_mode" is added to the "mode_flags" field. This new sub field indicates
    which performance mode this playback stream needs to be opened.

    @relevant_apr_hdr_fields
      Opcode -- ASM_STREAM_CMD_OPEN_WRITE_V3 \n
      Dst_port:
               - Session ID 1 -- ASM_MAX_SESSION_ID (0 is reserved)
               - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

    @newpage
    @apr_msg_payload
      @structure{asm_stream_cmd_open_write_v3_t}
      @tablespace
      @inputtable{ASM_STREAM_CMD_OPEN_WRITE_V3_APR_msg_payload.tex}

    @description
      When creating a stream, the client must specify the group and stream
      IDs. The current release supports up to ASM_MAX_SESSION_ID groups, and
      each group can have up to ASM_MAX_STREAMS_PER_SESSION streams.\n
        @vertspace
      This command informs of the media format of the incoming data. The media
      format block must be communicated to the aDSP through the
      ASM_DATA_CMD_MEDIA_FMT_UPDATE_V2 command before sending the actual data.\n
        @vertspace
      To achieve gapless playback, the client must open successive streams
      with the same session ID, but different stream IDs. Up to two streams
      can be open at the same time per session in Gapless mode. If the client 
      tries to open a stream when two streams are already open in the session,
      an ADSP_EALREADY error code is returned. MIDI and DTMF formats are not
      supported in gapless playback. Any attempt to open streams in these
      formats in Gapless Playback mode returns the error code, ADSP_EUNSUPPORTED.

    @prerequisites
      None.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).\n
      The handle is defined by the client.

    API_developer_note: A family also had ASM_STREAM_CMD_OPEN_WRITE. Since
    ASM_STREAM_CMD_OPEN_WRITE_V2 is more general,  ASM_STREAM_CMD_OPEN_WRITE is removed in
    B family.

    @newpage
*/
#define ASM_STREAM_CMD_OPEN_WRITE_V3                                   0x00010DB3

/** @xreflabel{hdr:AsmBitMaskStreamPerfModeOpenWriteFlag}
    Bitmask for stream_perf mode.
*/
#define ASM_BIT_MASK_STREAM_PERF_MODE_FLAG_IN_OPEN_WRITE                (0x70000000UL)

/** @xreflabel{hdr:AsmBitShiftStreamPerfModeOpenWriteFlag}
    Bit shift for stream_perf mode.
    @newpage
*/
#define ASM_SHIFT_STREAM_PERF_MODE_FLAG_IN_OPEN_WRITE                     28


/** A Legacy stream session is identified by this value.
*/
#define ASM_LEGACY_STREAM_SESSION                                      0

/** A Low latency stream session is identified by this value.
*/
#define ASM_LOW_LATENCY_STREAM_SESSION                                  1


/* Payload format for a stream open write command. */
typedef struct asm_stream_cmd_open_write_v3_t asm_stream_cmd_open_write_v3_t;

#include "adsp_begin_pack.h"

/** NEW @brief Payload of the #ASM_STREAM_CMD_OPEN_WRITE_V3 command, which
    opens a write stream for a given session ID and stream ID.\n
    \n
    The dst_port field in the APR packet contains both the session ID and the
    stream ID.\n
    \n
    After successfully opening the stream, a media format update command must
    be sent in the data path to configure the decoder before sending any data.
*/
struct asm_stream_cmd_open_write_v3_t
{
    
    uint32_t                    mode_flags;
    /**< Mode flags that configure the stream to notify the client whenever
         it detects an SR/CM change at the input to its POPP.

         Supported values for bits 0 to 1:
         - Reserved; clients must set them to zero.

         Supported values for bit 2:
         - 0 -- SR/CM change notification event is disabled.
         - 1 -- SR/CM change notification event is enabled.
         - Use #ASM_BIT_MASK_SR_CM_CHANGE_NOTIFY_FLAG and
           #ASM_SHIFT_SR_CM_CHANGE_NOTIFY_FLAG to set or get this bit.

         Supported values for bit 31:
         - 0 -- Stream to be opened in on-Gapless mode.
         - 1 -- Stream to be opened in Gapless mode. In Gapless mode,
                successive streams must be opened with same session ID but
                different stream IDs.
         - Use #ASM_BIT_MASK_GAPLESS_MODE_FLAG and #ASM_SHIFT_GAPLESS_MODE_FLAG
           to set or get this bit.

         Supported values for the field stream_perf_mode (bit 30:28):
         Indicates which performance mode this stream needs to be opened.

         - #ASM_LEGACY_STREAM_SESSION -- This mode ensures backward
                  compatibility to the original behavior of ASM_STREAM_CMD_OPEN_WRITE_V2.
         - #ASM_LOW_LATENCY_STREAM_SESSION -- Opens a playback session
                  using shortened buffers in Low Latency POPP. It is not recommended to enable
                  High latency algorithms as they might negate the benefits of opening
                  a low latency stream and might also suffer quality degradation from unexpected jitter.
                  adsp data path output latency(In a steady state playback, it is measured as
                  the average sample time difference between the sample that is currently being read
                  from the shared memory and the sample that is currently rendered to DMA)
                  contributed by stream side can vary from 1 to 3msec.
         - Other values -- reserved for future use
         - Use #ASM_BIT_MASK_STREAM_PERF_MODE_FLAG_IN_OPEN_WRITE and
           #ASM_SHIFT_STREAM_PERF_MODE_FLAG_IN_OPEN_WRITE to configure this sub field.

         All other bits are reserved; clients must set them to zero.


        @note1hang MIDI and DTMF streams cannot be opened in Gapless mode. */

    uint16_t                    sink_endpoint_type;
    /**< Sink point type.

         Supported values:
         - 0 -- Device matrix
         - Other values are reserved.

         The device matrix is the gateway to the hardware ports. */

    uint16_t                    bits_per_sample;
     /**< Number of bits per sample processed by ASM modules.
      
            Supported values: 16 and 24 bits per sample */

    uint32_t                    postproc_topo_id;
    /**< Specifies the topology (order of processing) of postprocessing
         algorithms. <i>None</i> means no postprocessing.

         Supported values:
         - #ASM_STREAM_POSTPROC_TOPO_ID_DEFAULT
         - #ASM_STREAM_POSTPROC_TOPO_ID_MCH_PEAK_VOL
         - #ASM_STREAM_POSTPROC_TOPO_ID_NONE
         
         This field can also be enabled through SetParams flags. */

    uint32_t                    dec_fmt_id;
    /**< Configuration ID of the decoder media format.

         Supported values:
         - #ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2
         - #ASM_MEDIA_FMT_ADPCM
         - #ASM_MEDIA_FMT_MP3
         - #ASM_MEDIA_FMT_AAC_V2
         - #ASM_MEDIA_FMT_DOLBY_AAC
         - #ASM_MEDIA_FMT_AMRNB_FS
         - #ASM_MEDIA_FMT_AMRWB_FS
         - #ASM_MEDIA_FMT_AMR_WB_PLUS_V2
         - #ASM_MEDIA_FMT_V13K_FS
         - #ASM_MEDIA_FMT_EVRC_FS
         - #ASM_MEDIA_FMT_EVRCB_FS
         - #ASM_MEDIA_FMT_EVRCWB_FS
         - #ASM_MEDIA_FMT_SBC
         - #ASM_MEDIA_FMT_WMA_V10PRO_V2
         - #ASM_MEDIA_FMT_WMA_V9_V2
         - #ASM_MEDIA_FMT_AC3_DEC
         - #ASM_MEDIA_FMT_EAC3_DEC
         - #ASM_MEDIA_FMT_G711_ALAW_FS
         - #ASM_MEDIA_FMT_G711_MLAW_FS
         - #ASM_MEDIA_FMT_G729A_FS
         - #ASM_MEDIA_FMT_FR_FS
         - #ASM_MEDIA_FMT_VORBIS
           - #ASM_MEDIA_FMT_FLAC
         - #ASM_MEDIA_FMT_EXAMPLE
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_strm_cmd_open_write_v3 */



/** @addtogroup asm_strm_cmd_open_read_v2
@{ */

/** @xreflabel{hdr:AsmBitMaskMetaInfoFlag}
    Definition of the metadata information flag bitmask.
*/
#define ASM_BIT_MASK_META_INFO_FLAG                                 (0x00000010UL)

/** @xreflabel{hdr:AsmShiftMetaInfoFlag}
    Definition of the metadata information flag shift value.
    @newpage
*/
#define ASM_SHIFT_META_INFO_FLAG                                    4

/** @xreflabel{hdr:ASMStreamCmdOpenReadV2}
    Creates an audio stream for recording. There is one preprocessing chain and
    one encoder. If the encoder type is PCM playback, it is a pass-through
    encoder.
    
    This command allows 16 and 24 bits per sample.

    @relevant_apr_hdr_fields
      Opcode -- ASM_STREAM_CMD_OPEN_READ_V2 \n
      Dst_port:
               - Session ID 1 -- ASM_MAX_SESSION_ID
               - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION

    @apr_msg_payload
      @structure{asm_stream_cmd_open_read_v2_t}
      @tablespace
      @inputtable{ASM_STREAM_CMD_OPEN_READ_V2_APR_msg_payload.tex}

    @description
      This command creates a new record stream. It specifies the media format
      of the encoding to be performed by the aDSP. The encoder configuration
      block must be communicated to the aDSP through the
      #ASM_STREAM_CMD_SET_ENCDEC_PARAM command before sending data buffers to
      get the encoded data from the aDSP.\n
        @negparspace
      Sending multiple ASM_STREAM_CMD_SET_ENCDEC_PARAM commands for changing
      the encoder configuration is not allowed.\n
        @negparspace
      24-bit recording is supported only for the Linear PCM media format.

    @prerequisites
      None.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).\n
      The handle is defined by the client as an input.

    API_developer_note: A family also had ASM_STREAM_CMD_OPEN_READ. Since
    ASM_STREAM_CMD_OPEN_READ_V2 is more general,  ASM_STREAM_CMD_OPEN_READ is removed in
    B family.

    @newpage
*/
#define ASM_STREAM_CMD_OPEN_READ_V2                                    0x00010D8C

/** Definition of the timestamp type flag bitmask
*/
#define ASM_BIT_MASK_TIMESTAMP_TYPE_FLAG                               (0x00000020UL)

/** Definition of the timestamp type flag shift value.
*/
#define ASM_SHIFT_TIMESTAMP_TYPE_FLAG                                    5

/** Relative timestamp is identified by this value.
*/
#define ASM_RELATIVE_TIMESTAMP                                          0

/** Absolute timestamp is identified by this value.
*/
#define ASM_ABSOLUTE_TIMESTAMP                                          1

/* Payload format for a stream open read command. */
typedef struct asm_stream_cmd_open_read_v2_t asm_stream_cmd_open_read_v2_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_STREAM_CMD_OPEN_READ_V2 command, which opens a
    read stream for a given session ID and stream ID.\n
    \n
    The dst_port field in the APR packet contains both the session ID and the
    stream ID.\n
    \n
    After successfully opening the stream, a set_encdec_params command must be
    sent in the command path to configure the encoder before encoded frames
    can be sent back to the client.
*/
struct asm_stream_cmd_open_read_v2_t
{
    uint32_t                    mode_flags;
    /**< Mode flags that indicate whether meta information per encoded frame
         is to be provided.

         Supported values for bit 4:
         - 0 -- Return data buffer contains all encoded frames only; it does
                not contain frame metadata.
         - 1 -- Return data buffer contains an array of metadata and encoded
                frames.
         - Use #ASM_BIT_MASK_META_INFO_FLAG as the bitmask and
           #ASM_SHIFT_META_INFO_FLAG as the shift value for this bit.

         Supported values for bit 5:
         - ASM_RELATIVE_TIMESTAMP -- ASM_DATA_EVENT_READ_DONE_V2 will have relative time-stamp.
         - ASM_ABSOLUTE_TIMESTAMP -- ASM_DATA_EVENT_READ_DONE_V2 will have absolute time-stamp.

         - Use #ASM_BIT_MASK_TIMESTAMP_TYPE_FLAG as the bitmask and
           #ASM_SHIFT_TIMESTAMP_TYPE_FLAG as the shift value for this bit.

         All other bits are reserved; clients must set them to zero. */

    uint32_t                    src_endpoint_type;
    /**< Specifies the endpoint providing the input samples.

         Supported values:
         - 0 -- Device matrix
         - All other values are reserved; clients must set them to zero.
           Otherwise, an error is returned.

         The device matrix is the gateway from the tunneled Tx ports. */

    uint32_t                    preproc_topo_id;
    /**< Specifies the topology (order of processing) of preprocessing
         algorithms. <i>None</i> means no preprocessing.

         Supported values:
         - #ASM_STREAM_PREPROC_TOPO_ID_DEFAULT
         - #ASM_STREAM_PREPROC_TOPO_ID_NONE

         This field can also be enabled through SetParams flags. */

    uint32_t                    enc_cfg_id;
    /**< Media configuration ID for encoded output.

         Supported values:
         - #ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2
         - #ASM_MEDIA_FMT_AAC_V2
         - #ASM_MEDIA_FMT_AMRNB_FS
         - #ASM_MEDIA_FMT_AMRWB_FS
         - #ASM_MEDIA_FMT_V13K_FS
         - #ASM_MEDIA_FMT_EVRC_FS
         - #ASM_MEDIA_FMT_EVRCB_FS
         - #ASM_MEDIA_FMT_EVRCWB_FS
         - #ASM_MEDIA_FMT_SBC
         - #ASM_MEDIA_FMT_G711_ALAW_FS
         - #ASM_MEDIA_FMT_G711_MLAW_FS
         - #ASM_MEDIA_FMT_G729A_FS
         - #ASM_MEDIA_FMT_EXAMPLE
         - #ASM_MEDIA_FMT_WMA_V8 */

     uint16_t                    bits_per_sample;
     /**< Number of bits per sample processed by ASM modules.

            Supported values: 16 and 24 bits per sample */

     uint16_t                    reserved;
    /**< Reserved for future use. This field must be set to zero.
       @newpage*/
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_strm_cmd_open_read_v2 */


/** @addtogroup asm_strm_cmd_open_read_v3
@{ */

/** @xreflabel{hdr:ASMStreamCmdOpenReadV3}
    Creates an audio stream for recording. There is one preprocessing chain and
    one encoder. If the encoder type is PCM playback, it is a pass-through
    encoder.
    
    This command allows 16 and 24 bits per sample.
    Unlike ASM_STREAM_CMD_OPEN_READ_V2 command, In this command, a new sub field
    "stream_perf_mode" is added to the "mode_flags" field. This new sub field indicates
    which performance mode this recording stream needs to be opened.

    @relevant_apr_hdr_fields
      Opcode -- ASM_STREAM_CMD_OPEN_READ_V3 \n
      Dst_port:
               - Session ID 1 -- ASM_MAX_SESSION_ID
               - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION

    @apr_msg_payload
      @structure{asm_stream_cmd_open_read_v3_t}
      @tablespace
      @inputtable{ASM_STREAM_CMD_OPEN_READ_V3_APR_msg_payload.tex}

    @description
      This command creates a new record stream. It specifies the media format
      of the encoding to be performed by the aDSP. The encoder configuration
      block must be communicated to the aDSP through the
      #ASM_STREAM_CMD_SET_ENCDEC_PARAM command before sending data buffers to
      get the encoded data from the aDSP.\n
        @negparspace
      Sending multiple ASM_STREAM_CMD_SET_ENCDEC_PARAM commands for changing
      the encoder configuration is not allowed.\n
        @negparspace
      24-bit recording is supported only for the Linear PCM media format.

    @prerequisites
      None.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).\n
      The handle is defined by the client as an input.

    API_developer_note: A family also had ASM_STREAM_CMD_OPEN_READ. Since
    ASM_STREAM_CMD_OPEN_READ_V2 is more general,  ASM_STREAM_CMD_OPEN_READ is removed in
    B family.

    @newpage
*/
#define ASM_STREAM_CMD_OPEN_READ_V3                                    0x00010DB4

/** @xreflabel{hdr:AsmBitMaskStreamPerfModeOpenReadFlag}
    Bitmask for stream_perf mode.
*/
#define ASM_BIT_MASK_STREAM_PERF_MODE_FLAG_IN_OPEN_READ                 (0xe0000000UL)

/** @xreflabel{hdr:AsmBitShiftStreamPerfModeOpenReadFlag}
    Bit shift for stream_perf mode.
    @newpage
*/
#define ASM_SHIFT_STREAM_PERF_MODE_FLAG_IN_OPEN_READ                     29


/* Payload format for a stream open read command. */
typedef struct asm_stream_cmd_open_read_v3_t asm_stream_cmd_open_read_v3_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_STREAM_CMD_OPEN_READ_V3 command,
    which opens a read stream for a given session ID and stream
    ID.\n
    \n
    The dst_port field in the APR packet contains both the session ID and the
    stream ID.\n
    \n
    After successfully opening the stream, a set_encdec_params command must be
    sent in the command path to configure the encoder before encoded frames
    can be sent back to the client.
*/
struct asm_stream_cmd_open_read_v3_t
{
    uint32_t                    mode_flags;
    /**< Mode flags that indicate whether meta information per encoded frame
         is to be provided.

         Supported values for bit 4:
         - 0 -- Return data buffer contains all encoded frames only; it does
                not contain frame metadata.
         - 1 -- Return data buffer contains an array of metadata and encoded
                frames.
         - Use #ASM_BIT_MASK_META_INFO_FLAG as the bitmask and
           #ASM_SHIFT_META_INFO_FLAG as the shift value for this bit.

         Supported values for bit 5:
         - ASM_RELATIVE_TIMESTAMP -- ASM_DATA_EVENT_READ_DONE_V2 will have relative time-stamp.
         - ASM_ABSOLUTE_TIMESTAMP -- ASM_DATA_EVENT_READ_DONE_V2 will have absolute time-stamp.

         - Use #ASM_BIT_MASK_TIMESTAMP_TYPE_FLAG as the bitmask and
           #ASM_SHIFT_TIMESTAMP_TYPE_FLAG as the shift value for this bit.
     
         Supported values for the field stream_perf_mode( bit 31:29):
         Indicates which performance mode this stream needs to be opened.

         - #ASM_LEGACY_STREAM_SESSION -- This mode ensures backward
                  compatibility to the original behavior of ASM_STREAM_CMD_OPEN_READ_V2
         - #ASM_LOW_LATENCY_STREAM_SESSION -- Opens a record session
                  using shortened buffers in Low latency POPreP. It is not recommended to enable
                  High latency algorithms as they might  negate the benefits of opening
                  a low latency stream and might also suffer quality degradation from
                  unexpected jitter. adsp data path input latency(In a steady state recording,
                  it is measured as the sample time difference between the sample that is
                  currently being read from the DMA and the sample that is currently written
                  to the shared memory) contributed by stream side can vary from 1 to 2msec.
         - Other values -- reserved for future use
         - Use #ASM_BIT_MASK_STREAM_PERF_MODE_FLAG_IN_OPEN_READ and
           #ASM_SHIFT_STREAM_PERF_MODE_FLAG_IN_OPEN_READ to configure this sub field.

     
         All other bits are reserved; clients must set them to zero. */

    uint32_t                    src_endpoint_type;
    /**< Specifies the endpoint providing the input samples.

         Supported values:
         - 0 -- Device matrix
         - All other values are reserved; clients must set them to zero.
           Otherwise, an error is returned.

         The device matrix is the gateway from the tunneled Tx ports. */

    uint32_t                    preproc_topo_id;
    /**< Specifies the topology (order of processing) of preprocessing
         algorithms. <i>None</i> means no preprocessing.

         Supported values:
         - #ASM_STREAM_PREPROC_TOPO_ID_DEFAULT
         - #ASM_STREAM_PREPROC_TOPO_ID_NONE

         This field can also be enabled through SetParams flags. */

    uint32_t                    enc_cfg_id;
    /**< Media configuration ID for encoded output.

         Supported values:
         - #ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2
         - #ASM_MEDIA_FMT_AAC_V2
         - #ASM_MEDIA_FMT_AMRNB_FS
         - #ASM_MEDIA_FMT_AMRWB_FS
         - #ASM_MEDIA_FMT_V13K_FS
         - #ASM_MEDIA_FMT_EVRC_FS
         - #ASM_MEDIA_FMT_EVRCB_FS
         - #ASM_MEDIA_FMT_EVRCWB_FS
         - #ASM_MEDIA_FMT_SBC
         - #ASM_MEDIA_FMT_G711_ALAW_FS
         - #ASM_MEDIA_FMT_G711_MLAW_FS
         - #ASM_MEDIA_FMT_G729A_FS
         - #ASM_MEDIA_FMT_WMA_V8
         - #ASM_MEDIA_FMT_EXAMPLE */

     uint16_t                    bits_per_sample;
     /**< Number of bits per sample processed by ASM modules.

            Supported values: 16 and 24 bits per sample */

     uint16_t                    reserved;
    /**< Reserved for future use. This field must be set to zero.
       @newpage*/
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_strm_cmd_open_read_v3 */



/** @addtogroup asm_strm_cmd_open_readwrite_v2
@{ */


/** Enumeration for native sampling rate signaling at the POPP output.
*/
#define ASM_POPP_OUTPUT_SR_NATIVE_RATE                                  0

/** Enumeration for the maximum sampling rate at the POPP output.
    @newpage
*/
#define ASM_POPP_OUTPUT_SR_MAX_RATE                                     48000


/** Opens a read/write stream with configurable bits per sample.

    This command allows 16 and 24 bits per sample. The

    @relevant_apr_hdr_fields
      Opcode -- ASM_STREAM_CMD_OPEN_READWRITE_V2 \n
      Dst_port:
               - Session ID 1 -- ASM_MAX_SESSION_ID
               - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION

    @apr_msg_payload
      @structure{asm_stream_cmd_open_readwrite_v2_t}
      @tablespace
      @inputtable{ASM_STREAM_CMD_OPEN_READWRITE_V2_APR_msg_payload.tex}

    @description
      This command specifies the media format of the encoding to be performed
      by the Hexagon processor. The encoder configuration block must be
      communicated to the Hexagon processor through the
      #ASM_STREAM_CMD_SET_ENCDEC_PARAM command before sending data buffers to
      get the encoded data from the Hexagon processor.\n
        @vertspace
      Sending multiple ASM_STREAM_CMD_SET_ENCDEC_PARAM commands for changing
      the encoder configuration is not allowed.

    @prerequisites
      None.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).
      The handle is defined by the client as an input.

    API_developer_note: A family also had ASM_STREAM_CMD_OPEN_READWRITE. Since
    ASM_STREAM_CMD_OPEN_READWRITE_V2 is more general,  ASM_STREAM_CMD_OPEN_READWRITE is removed in
    B family.

    @newpage
*/
#define ASM_STREAM_CMD_OPEN_READWRITE_V2                                   0x00010D8D

/* Payload format for a stream open read/write command. */
typedef struct asm_stream_cmd_open_readwrite_v2_t asm_stream_cmd_open_readwrite_v2_t;

#include "adsp_begin_pack.h"

/** NEW @brief Payload of the #ASM_STREAM_CMD_OPEN_READWRITE_V2 command.\n
    \n
    A read/write stream is opened for a given session ID and stream ID. The 
    Dst_port field in the APR packet contains both the session ID and the
    stream ID.\n
    \n
    After successfully opening the stream, a set_encdec_params command must be
    sent in the command path to configure the encoder before encoded frames
    can be sent back to the client (even if it is a PCM encoder used for NT
    decoding). Also, a media format update command must be sent in the data
    path to configure the decoder before sending any data.
*/
struct asm_stream_cmd_open_readwrite_v2_t
{
    uint32_t                    mode_flags;
    /**< Mode flags. 

         Supported values for bit 2:
         - 0 -- SR/CM change notification event is disabled.
         - 1 -- SR/CM change notification event is enabled. Use
                #ASM_BIT_MASK_SR_CM_CHANGE_NOTIFY_FLAG and
                #ASM_SHIFT_SR_CM_CHANGE_NOTIFY_FLAG to set or
                getting this flag. \n

         Supported values for bit 4:
         - 0 -- Return read data buffer contains all encoded frames only; it
                does not contain frame metadata.
         - 1 -- Return read data buffer contains an array of metadata and
                encoded frames.\n
           
         All other bits are reserved; clients must set them to zero. */

    uint32_t                    postproc_topo_id;
    /**< Specifies the topology (order of processing) of postprocessing
         algorithms. <i>None</i> means no postprocessing.
           
         Supported values:
         - #ASM_STREAM_POSTPROC_TOPO_ID_DEFAULT
         - #ASM_STREAM_POSTPROC_TOPO_ID_MCH_PEAK_VOL
         - #ASM_STREAM_POSTPROC_TOPO_ID_NONE */

    uint32_t                    dec_fmt_id;
    /**< Specifies the media type of the input data. PCM indicates that no
         decoding must be performed, e.g., this is an NT encoder session. 

         Supported values:
         - #ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2
         - #ASM_MEDIA_FMT_ADPCM
         - #ASM_MEDIA_FMT_MP3
         - #ASM_MEDIA_FMT_AAC_V2
         - #ASM_MEDIA_FMT_DOLBY_AAC 
         - #ASM_MEDIA_FMT_AMRNB_FS
         - #ASM_MEDIA_FMT_AMRWB_FS
         - #ASM_MEDIA_FMT_V13K_FS
         - #ASM_MEDIA_FMT_EVRC_FS
         - #ASM_MEDIA_FMT_EVRCB_FS
         - #ASM_MEDIA_FMT_EVRCWB_FS
         - #ASM_MEDIA_FMT_SBC
         - #ASM_MEDIA_FMT_WMA_V10PRO_V2
         - #ASM_MEDIA_FMT_WMA_V9_V2
         - #ASM_MEDIA_FMT_AMR_WB_PLUS_V2
         - #ASM_MEDIA_FMT_AC3_DEC
         - #ASM_MEDIA_FMT_G711_ALAW_FS
         - #ASM_MEDIA_FMT_G711_MLAW_FS
         - #ASM_MEDIA_FMT_G729A_FS
         - #ASM_MEDIA_FMT_EXAMPLE */

    uint32_t                    enc_cfg_id;
    /**< Specifies the media type for the output of the stream. PCM indicates
         that no encoding must be performed, e.g., this is an NT decoder
         session.

         Supported values:
         - #ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2
         - #ASM_MEDIA_FMT_AAC_V2
         - #ASM_MEDIA_FMT_AMRNB_FS
         - #ASM_MEDIA_FMT_AMRWB_FS
         - #ASM_MEDIA_FMT_V13K_FS
         - #ASM_MEDIA_FMT_EVRC_FS
         - #ASM_MEDIA_FMT_EVRCB_FS
         - #ASM_MEDIA_FMT_EVRCWB_FS
         - #ASM_MEDIA_FMT_SBC
         - #ASM_MEDIA_FMT_G711_ALAW_FS
         - #ASM_MEDIA_FMT_G711_MLAW_FS
         - #ASM_MEDIA_FMT_G729A_FS
         - #ASM_MEDIA_FMT_EXAMPLE 
         - #ASM_MEDIA_FMT_WMA_V8 */
         
     uint16_t                    bits_per_sample;
     /**< Number of bits per sample processed by ASM modules.

            Supported values: 16 and 24 bits per sample */

     uint16_t                    reserved;
    /**< Reserved for future use. This field must be set to zero.
       @newpage*/

}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_strm_cmd_open_readwrite_v2 */

/** @cond
*/
/* Opens a loopback stream for a given session ID and stream ID.

    This command sets up the stream portion of the ASM Loopback mode
    topology. This is different from a read stream and a write stream in that
    there are no buffers exchanged between DSP and client. The session to
    which this stream belongs cannot have any other streams.

    The POPP topology ID must be specified. This command sets up the
    connection from the Tx Matrix output port to the POPP input port, and
    from the POPP output port to the Rx Matrix input port.

    @relevant_apr_hdr_fields
      Opcode -- ASM_STREAM_CMD_OPEN_LOOPBACK_V2 \n
      Dst_port:
               - Session ID 1 -- ASM_MAX_SESSION_ID
               - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION

    @apr_msg_payload
      @structure{asm_stream_cmd_open_loopback_t}
      @tablespace
      @inputtable{ASM_STREAM_CMD_OPEN_LOOPBACK_APR_msg_payload.tex}

    @prerequisites
      None.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
      The handle is defined by the client as an input.

    API_developer_note: A family also had ASM_STREAM_CMD_OPEN_LOOPBACK. Since
    ASM_STREAM_CMD_OPEN_LOOPBACK_V2 is more general,  ASM_STREAM_CMD_OPEN_LOOPBACK is removed in
    B family.

    @newpage
*/
/* Operation code for an ASM stream open loopback with configurable
    bits per sample
*/
#define ASM_STREAM_CMD_OPEN_LOOPBACK_V2                              0x00010D8E
                    
/* Payload format for stream open loopback */                    
typedef struct asm_stream_cmd_open_loopback_v2_t asm_stream_cmd_open_loopback_v2_t;                    

#include "adsp_begin_pack.h"

/* Payload format for a stream loopback command.

    Opens a LOOPBACK stream for a given session ID and stream
    ID. The dst_port field in the APR packet contains both the
    session ID and the stream ID. 
*/
struct asm_stream_cmd_open_loopback_v2_t
{
    uint32_t                    mode_flags;
     /* Mode flags. 
           Bit 0-31: reserved; client should set these bits to 0 */
    uint16_t                    src_endpoint_type;
     /* Endpoint type. 0 = Tx Matrix */
    uint16_t                    sink_endpoint_type;
     /* Endpoint type. 0 = Rx Matrix */           
    uint32_t                    postproc_topo_id;
     /* Postprocessor topology ID. Specifies the topology of postprocessing 
           algorithms.*/
     uint16_t                    bits_per_sample;
     /* The number of bits per sample processed by ASM modules
         Supported values: 16 and 24 bits per sample */
     uint16_t                    reserved;
    /* Reserved for future use. This field must be set to zero. */
}
#include "adsp_end_pack.h"
;
/** @endcond */

#define ASM_STREAM_CMD_OPEN_TRANSCODE_LOOPBACK                              0x00010DBA

/** @xreflabel{hdr:AsmBitMaskStreamPerfModeOpenTranscodeLoopbackFlag}
    Bitmask for stream_perf mode.
*/
#define ASM_BIT_MASK_STREAM_PERF_MODE_FLAG_IN_OPEN_TRANSCODE_LOOPBACK       (0x70000000UL)

/** @xreflabel{hdr:AsmBitShiftStreamPerfModeOpenTranscodeLoopbackFlag}
    Bit shift for stream_perf mode.
    @newpage
*/
#define ASM_SHIFT_STREAM_PERF_MODE_FLAG_IN_OPEN_TRANSCODE_LOOPBACK           28


/* Payload format for stream open loopback */
typedef struct asm_stream_cmd_open_transcode_loopback_t asm_stream_cmd_open_transcode_loopback_t;

#include "adsp_begin_pack.h"

/** @brief Payload of #ASM_STREAM_CMD_OPEN_TRANSCODE_LOOPBACK
    command, which opens a loopback stream with configurable
    bits per sample.
*/
struct asm_stream_cmd_open_transcode_loopback_t
{
    uint32_t                    mode_flags;
    /**< Supported values for the field stream_perf_mode (bit 30:28):
			Indicates which performance mode this stream needs to be opened.

			 000  --  #ASM_LEGACY_STREAM_SESSION -- This mode ensures backward
			          compatibility to the original behavior of ASM_STREAM_CMD_OPEN_TRANSCODE_LOOPBACK                              .

			 001  --  #ASM_LOW_LATENCY_STREAM_SESSION -- Opens a loopback session using shortened buffers
			          in Low Latency POPP. It is not recommended to enable High latency algorithms as they
			          might negate the benefits of opening a low latency stream and might also suffer quality
			          degradation from unexpected jitter. This low latency mode is supported only for
			          PCM In and PCM Out loopbacks. An error will be returned incase low latency mode is opened
			          for other transcode loopback modes.

			Other values -- reserved for future use
			Other bits - should be set to 0.
		    - Use # ASM_BIT_MASK_STREAM_PERF_MODE_FLAG_IN_OPEN_TRANSCODE_LOOPBACK  and
			  # ASM_SHIFT_STREAM_PERF_MODE_FLAG_IN_OPEN_TRANSCODE_LOOPBACK to configure this sub field*/

    uint32_t                    src_format_id;
    /**< Specifies the media format of the input audio stream.

         Supported values:
         - #ASM_MEDIA_FMT_LINEAR_PCM
         - #ASM_MEDIA_FMT_MULTI_CHANNEL_PCM */


    uint32_t                    sink_format_id;
    /**< Specifies the media format of the output stream.

         Supported values:
         - #ASM_MEDIA_FMT_LINEAR_PCM
         - #ASM_MEDIA_FMT_MULTI_CHANNEL_PCM
         - #ASM_MEDIA_FMT_DTS(not supported in Low latency mode) */

    uint32_t                    audproc_topo_id;
    /**< Postprocessing topology ID, which specifies the topology (order of
         processing) of postprocessing algorithms.

         Supported values:
         - #ASM_STREAM_POSTPROC_TOPO_ID_DEFAULT
         - #ASM_STREAM_POSTPROC_TOPO_ID_PEAKMETER
         - #ASM_STREAM_POSTPROC_TOPO_ID_NONE
         - #ASM_STREAM_POSTPROC_TOPO_ID_MCH_PEAK_VOL */

    uint16_t                    src_endpoint_type;
    /**< Specifies the source endpoint that provides the input samples.

         Supported values:
         - 0 -- Tx device matrix or stream router (gateway to the hardware ports)
         - All other values are reserved

         Clients must set this field to zero. Otherwise, an error is returned. */

    uint16_t                    sink_endpoint_type;
    /**< Specifies the sink endpoint type.

         Supported values:
         - 0 -- Rx device matrix or stream router (gateway to the hardware ports)
         - All other values are reserved

         Clients must set this field to zero. Otherwise, an error is returned.
         @newpage */

     uint16_t                    bits_per_sample;
     /**< Number of bits per sample processed by the ASM modules.

          Supported values: 16, 24 */

     uint16_t                    reserved;
     /**< This field must be set to zero.
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_strm_cmd_open_loopback_v2 */

/** @ingroup asm_strm_cmd_close
    Stops and closes a specified stream.

    @relevant_apr_hdr_fields
      Opcode -- ASM_STREAM_CMD_CLOSE \n
      Dst_port:
               - Session ID 1 -- ASM_MAX_SESSION_ID
               - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION

    @apr_msg_payload
      None.

    @prerequisites
      The stream must be a valid session stream.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
    @newpage
*/
#define ASM_STREAM_CMD_CLOSE                                        0x00010BCD

/** @ingroup asm_strm_cmd_flush
    Flushes a specified stream.

    @relevant_apr_hdr_fields
      Opcode -- ASM_STREAM_CMD_FLUSH \n
      Dst_port:
               - Session ID 1 -- ASM_MAX_SESSION_ID
               - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION

    @apr_msg_payload
      None.

    @prerequisites
      The stream's session must be paused; otherwise, an error is returned.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
    @newpage
*/
#define ASM_STREAM_CMD_FLUSH                                        0x00010BCE

/** @ingroup asm_strm_cmd_flush_readbufs
    Flushes a tunneled Tx or non-tunneled stream's output buffers.

    This command is used in read and read/write streams to allow the client
    to retrieve all shared memory buffers for size reallocation, without
    flushing any of the data pipelined in the stream. An example is a sample
    rate or channel configuration change in a non-tunneled decoder.

    The behavior is to deliver first any partially filled output buffers
    with #ASM_DATA_EVENT_READ_DONE_V2, followed by all the queued empty output
    buffers.

    @relevant_apr_hdr_fields
      Opcode -- ASM_STREAM_CMD_FLUSH_READBUFS \n
      Dst_port:
               - Session ID 1 -- ASM_MAX_SESSION_ID
               - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION

    @apr_msg_payload
      None.

    @prerequisites
      The stream must be a tunneled Tx stream opened by
      #ASM_STREAM_CMD_OPEN_READ_V2, or a non-tunneled stream opened by
      #ASM_STREAM_CMD_OPEN_READWRITE_V2.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
    @newpage
*/
#define ASM_STREAM_CMD_FLUSH_READBUFS                               0x00010C09

/** @addtogroup asm_strm_cmd_set_pp_params
@{ */
/** @xreflabel{hdr:ASMStrmCmdSetPPParams}
    Sets one or more ASM stream parameters to a POPP chain. Supported module
    and parameter IDs are provided in Section @xref{hdr:AudioPostProcIDs}.

    @relevant_apr_hdr_fields
      Opcode -- ASM_STREAM_CMD_SET_PP_PARAMS_V2 \n
      Dst_port:
               - Session ID 1 -- ASM_MAX_SESSION_ID
               - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION

    @apr_msg_payload
      @structure{asm_stream_cmd_set_pp_params_v2_t}
      @tablespace
      @inputtable{ASM_STREAM_CMD_SET_PP_PARAMS_APR_msg_payload.tex}

    @parspace Parameter data variable payload
      @structure{asm_stream_param_data_v2_t}
      @tablespace
      @inputtable{Shared_param_data_variable_payload.tex}

    @prerequisites
      This command must be applied to a valid stream.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).

    API_developer_note: A family API ASM_STREAM_CMD_SET_PP_PARAMS is replaced by ASM_STREAM_CMD_SET_PP_PARAMS_V2.
    @newpage
*/
#define ASM_STREAM_CMD_SET_PP_PARAMS_V2                                0x00010DA1

/* Payload format for a stream Set Parameters command. */
typedef struct asm_stream_cmd_set_pp_params_v2_t asm_stream_cmd_set_pp_params_v2_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_STREAM_CMD_SET_PP_PARAMS_V2 command, which allows
    one or more parameters to be set on a stream.\n
    \n    
    This command is typically used to apply settings to the POPP or
    Per-Group-Object Pre/postprocessing (PGOPP) chains.\n
    \n  
    If 64 bit data_payload_addr=NULL, a series of asm_stream_param_data_v2_t structures
    immediately follow, whose total size is data_payload_size bytes.
*/
struct asm_stream_cmd_set_pp_params_v2_t
{
    uint32_t                  data_payload_addr_lsw;
    /**< LSW of parameter data payload address. Supported values: any. */
    uint32_t                  data_payload_addr_msw;
   /**< MSW of Parameter data payload address. Supported values: any.

        - Must be set to zero for in-band data.
        - In the case of 32 bit Shared memory address, msw  field must be set to zero.
        - In the case of 36 bit shared memory address, bit 31 to bit 4 of msw must be set to zero.
 */
    uint32_t                  mem_map_handle;
    /**< Supported Values: Any.
     * memory map handle returned by DSP through ASM_CMD_SHARED_MEM_MAP_REGIONS command.
     * if mmhandle is NULL, the ParamData payloads are within the message payload (in-band).                                                                                                                                 
     * If mmhandle is non-NULL, the ParamData payloads begin at the address specified in the.                                       
     * address msw and lsw (out-of-band). */

    uint32_t                  data_payload_size;
    /**< Size in bytes of the variable payload accompanying the message, or in
         shared memory. This field is used for parsing the parameter payload.
         @newpage*/
}
#include "adsp_end_pack.h"
;

/* Payload format for stream parameter data. */
typedef struct asm_stream_param_data_v2_t asm_stream_param_data_v2_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the stream parameter data of the
    #ASM_STREAM_CMD_SET_PP_PARAMS_V2 command.\n
    \n
    Immediately following this structure are param_size bytes of parameter
    data.
*/
struct asm_stream_param_data_v2_t
{
    uint32_t                  module_id;
    /**< Unique module ID. */

    uint32_t                  param_id;
    /**< Unique parameter ID. */

    uint16_t                  param_size;
    /**< Data size of the param_id/module_id combination. This is
         a multiple of 4 bytes. */

    uint16_t                  reserved;
    /**< Reserved for future enhancements. This field must be set to zero.
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_strm_cmd_set_pp_params */

/** @addtogroup asm_strm_cmd_get_pp_params
@{ */
/** @xreflabel{hdr:ASMStrmCmdGetPPParams}
    Queries for an audio processing service parameter of a stream.

    @relevant_apr_hdr_fields
      Opcode -- ASM_STREAM_CMD_GET_PP_PARAMS_V2 \n
      Dst_port:
               - Session ID 1 -- ASM_MAX_SESSION_ID
               - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION

    @apr_msg_payload
      @structure{asm_stream_cmd_get_pp_params_t}
      @tablespace
      @inputtable{ASM_STREAM_CMD_GET_PP_PARAMS_APR_msg_payload.tex}

    @keep{4}
    @prerequisites
      Must be applied to a valid stream.

    @return
      #ASM_STREAM_CMDRSP_GET_PP_PARAMS_V2.
    @newpage
*/
#define ASM_STREAM_CMD_GET_PP_PARAMS_V2                                0x00010DA2

/* Payload format for a stream get parameters command. */
typedef struct asm_stream_cmd_get_pp_params_v2_t asm_stream_cmd_get_pp_params_v2_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_STREAM_CMD_GET_PP_PARAMS_V2 command, which allows
    a query for one pre/postprocessing parameter of a stream.
*/
struct asm_stream_cmd_get_pp_params_v2_t
{
    uint32_t                  data_payload_addr_lsw;
    /**< LSW of the parameter data payload address. */
    uint32_t                  data_payload_addr_msw;
    /**< MSW of the parameter data payload address.

         - Size of the shared memory, if specified, shall be large enough to contain the whole ParamData payload, including Module ID, Param ID, Param Size, and Param Values
         - Must be set to zero for in-band data
         - In the case of 32 bit Shared memory address, msw  field must be set to zero.
        - In the case of 36 bit shared memory address, bit 31 to bit 4 of msw must be set to zero.
 */

    uint32_t                  mem_map_handle;
    /**< Supported Values: Any.
     * memory map handle returned by DSP through ASM_CMD_SHARED_MEM_MAP_REGIONS command.
     * if mmhandle is NULL, the ParamData payloads in the ACK are within the message payload (in-band).                                                                                                                                                       
     * If mmhandle is non-NULL, the ParamData payloads in the ACK begin at the address specified in the address msw and lsw.                                       
     * (out-of-band). */

    uint32_t                  module_id;
    /**< Unique module ID. */

    uint32_t                  param_id;
    /**< Unique parameter ID. */

    uint16_t                  param_max_size;
    /**< Maximum data size of the module_id/param_id combination. This is a
         multiple of 4 bytes. */

    uint16_t                  reserved;
    /**< Reserved for backward compatibility. Clients must set this field to
         zero.
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_strm_cmd_get_pp_params */

/** @addtogroup asm_strm_cmd_set_encdec_params
@{ */
/** Sets one parameter for the encoder/decoder on a stream. This is typically
    used to change encoder/decoder settings during runtime.

    @relevant_apr_hdr_fields
      Opcode -- ASM_STREAM_CMD_SET_ENCDEC_PARAM \n
      Dst_port:
               - Session ID 1 -- ASM_MAX_SESSION_ID
               - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION

    @apr_msg_payload
      @structure{asm_stream_cmd_set_encdec_param_t}
      @tablespace
      @inputtable{ASM_STREAM_CMD_SET_ENCDEC_PARAM_APR_msg_payload.tex}

    @prerequisites
      This command must be applied to a valid stream.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
    @newpage
*/
#define ASM_STREAM_CMD_SET_ENCDEC_PARAM                             0x00010C10

/* Payload format for an ASM stream Set Encoder/Decoder Parameter
    command. */
typedef struct asm_stream_cmd_set_encdec_param_t asm_stream_cmd_set_encdec_param_t;

#include "adsp_begin_pack.h"

/** @brief Payload format of the #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.\n
    \n    
    Immediately following this structure are usParamSize bytes representing
    the parameter data. The size depends on the param_id/param_size
    combination.
*/
struct asm_stream_cmd_set_encdec_param_t
{
    uint32_t                  param_id;
    /**< ID of the parameter. */

    uint32_t                  param_size;
    /**< Data size of this parameter, in bytes. The size is a multiple of
         4 bytes.
         @newpage */
}
#include "adsp_end_pack.h"
;

/** ID of the audio encoder/decoder bitrate parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    @messagepayload
      @structure{asm_bitrate_param_t}
      @tablespace
      @inputtable{ASM_STREAM_CMD_SET_ENCDEC_PARAM_Audio_encoders_bitrate.tex}
*/
#define ASM_PARAM_ID_ENCDEC_BITRATE                                 0x00010C13

/* Structure for the bitrate parameter. */
typedef struct asm_bitrate_param_t asm_bitrate_param_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_PARAM_ID_ENCDEC_BITRATE parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
*/
struct asm_bitrate_param_t
{
    uint32_t                  bitrate;
    /**< Maximum supported bitrate. Only the AAC encoder is supported.

         Supported values: See the
         @xhyperref{tbl:AACencoderAllowedBitrates,AAC encoder allowed bitrates}
         table
         @newpage */
}
#include "adsp_end_pack.h"
;

/** Parameter ID of the encoder configuration block in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    @messagepayload
      @structure{asm_enc_cfg_blk_param_v2_t}
      @tablespace
      @inputtable{ASM_STREAM_CMD_SET_ENCDEC_PARAM_Encoder_config_block.tex}
    @newpage
*/
#define ASM_PARAM_ID_ENCDEC_ENC_CFG_BLK_V2                             0x00010DA3

/* Structure for an encoder configuration block. */
typedef struct asm_enc_cfg_blk_param_v2_t asm_enc_cfg_blk_param_v2_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_PARAM_ID_ENCDEC_ENC_CFG_BLK_V2 parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.\n
    \n
    Immediately following this structure is an encoder configuration block
    of size enc_cfg_blk_size bytes.
*/
struct asm_enc_cfg_blk_param_v2_t
{
    uint32_t                  frames_per_buf;
    /**< Number of encoded frames to pack into each buffer.
      
         @note1hang This is only guidance information for the aDSP. The number
                    of encoded frames put into each buffer (specified by the
                    client) is less than or equal to this number. */

    uint32_t                  enc_cfg_blk_size;
    /**< Size in bytes of the encoder configuration block that follows this
         member.
         @newpage */
}
#include "adsp_end_pack.h"
;

/** Parameter ID of the AAC decoder SBR/PS enable flag in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    @messagepayload
      @structure{asm_aac_sbr_ps_flag_param_t}
      @tablespace
      @inputtable{ASM_STREAM_CMD_SET_ENCDEC_PARAM_AAC_SBR_PS_enable_flags.tex}
*/
#define ASM_PARAM_ID_AAC_SBR_PS_FLAG                                0x00010C63

/** @xreflabel{hdr:AsmAacSbrOffPsOff}
    Flag to turn off both SBR and PS processing, if they are present in the
    bitstream.
*/
#define ASM_AAC_SBR_OFF_PS_OFF                                      (2)

/** @xreflabel{hdr:AsmAacSbrOnPsOff}
    Flag to turn on SBR but turn off PS processing,if they are present in the
    bitstream.
*/
#define ASM_AAC_SBR_ON_PS_OFF                                       (1)

/** @xreflabel{hdr:AsmAacSbrOnPsOn}
    Flag to turn on both SBR and PS processing, if they are present in the
    bitstream (default behavior).
    @newpage
*/
#define ASM_AAC_SBR_ON_PS_ON                                        (0)

/* Structure for an AAC SBR PS processing flag. */
typedef struct asm_aac_sbr_ps_flag_param_t asm_aac_sbr_ps_flag_param_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_PARAM_ID_AAC_SBR_PS_FLAG parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
*/
struct asm_aac_sbr_ps_flag_param_t
{
    uint32_t                  sbr_ps_flag;
    /**< Control parameter to enable or disable SBR/PS processing in the AAC
         bitstream. Use the following macros to set this field:

         - #ASM_AAC_SBR_OFF_PS_OFF -- Turn off both SBR and PS processing,
           if they are present in the bitstream.
         - #ASM_AAC_SBR_ON_PS_OFF -- Turn on SBR processing, but not PS
           processing, if they are present in the bitstream.
         - #ASM_AAC_SBR_ON_PS_ON -- Turn on both SBR and PS processing, if
           they are present in the bitstream (default behavior).
         - All other values are invalid.

         Changes are applied to the next decoded frame.
         @newpage */
}
#include "adsp_end_pack.h"
;

/** ID of the AAC decoder dual mono channel mapping parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    This parameter applies only to AAC dual mono bitstream decoding. The
    command can be sent any time after opening a write or read/write stream,
    and the changes take effect from the next decoded frame.

    The same SCE can be routed to both he left and right channels. The
    following table shows the message payload for AAC dual mono channel
    mapping, where all the channels are valid.

    @messagepayload
      @structure{asm_aac_dual_mono_mapping_param_t}
      @tablespace
      @inputtable{ASM_STREAM_CMD_SET_ENCDEC_PARAM_AAC_dual_mono_ch_mapping.tex}
*/
#define ASM_PARAM_ID_AAC_DUAL_MONO_MAPPING                      0x00010C64

/** @xreflabel{hdr:AsmAacDualMonoMapSce1}
    First single channel element in a dual mono bitstream.
*/
#define ASM_AAC_DUAL_MONO_MAP_SCE_1                                 (1)

/** @xreflabel{hdr:AsmAacDualMonoMapSce2}
    Second single channel element in a dual mono bitstream.
    @newpage
*/
#define ASM_AAC_DUAL_MONO_MAP_SCE_2                                 (2)

/* Structure for AAC decoder dual mono channel mapping. */
typedef struct asm_aac_dual_mono_mapping_param_t asm_aac_dual_mono_mapping_param_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_PARAM_ID_AAC_DUAL_MONO_MAPPING parameter in
    the #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.\n
    \n
    This parameter is applicable only for an AAC decoder decoding dual mono
    bitstreams. This command can be sent any time after opening a write or
    read/write stream, and the changes take effect from the next decoded frame.\n
    \n
    The same SCE can be routed to both the left and right channels.
*/
struct asm_aac_dual_mono_mapping_param_t
{
    uint16_t                  left_channel_sce;
    /**< Specifies which SCE is connected to the left channel.

         Supported values:
         - #ASM_AAC_DUAL_MONO_MAP_SCE_1 (Default)
         - #ASM_AAC_DUAL_MONO_MAP_SCE_2 */

    uint16_t                  right_channel_sce;
    /**< Specifies which SCE is connected to the right channel.

         Supported values:
         - #ASM_AAC_DUAL_MONO_MAP_SCE_1 (Default)
         - #ASM_AAC_DUAL_MONO_MAP_SCE_2
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_strm_cmd_set_encdec_params */

/** @addtogroup asm_stream_resp_get_pp_params
@{ */
/** Returns parameter values in response to an #ASM_STREAM_CMD_GET_PP_PARAMS
    command.
    
    @relevant_apr_hdr_fields
      Opcode -- ASM_STREAM_CMDRSP_GET_PP_PARAMS_V2 \n
      Src_port:
               - Session ID 1 -- ASM_MAX_SESSION_ID
               - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION

    @apr_msg_payload
      @structure{asm_stream_cmdrsp_get_pp_params_v2_t}
      @tablespace
      @inputtable{ASM_STREAM_CMDRSP_GET_PP_PARAMS_APR_msg_payload.tex}

    @parspace Acknowledgment parameter payload
      @structure{asm_stream_param_data_v2_t}
      @tablespace
      @inputtable{Shared_param_data_variable_payload.tex}

    @prerequisites
      This command must be applied to a valid stream.

    @return
      None.
    @newpage
*/
#define ASM_STREAM_CMDRSP_GET_PP_PARAMS_V2                             0x00010DA4

/* Payload for an ASM stream get pre/postprocessing parameters ACK event. */
typedef struct asm_stream_cmdrsp_get_pp_params_v2_t asm_stream_cmdrsp_get_pp_params_v2_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_STREAM_CMDRSP_GET_PP_PARAMS_V2 message, which
    returns parameter values in response to an #ASM_STREAM_CMD_GET_PP_PARAMS_V2
    command.\n
    \n
    Immediately following this structure is the asm_stream_param_data_v2_t
    structure containing the pre/postprocessing parameter data.
*/
struct asm_stream_cmdrsp_get_pp_params_v2_t
{
    uint32_t                  status;
    /**< Status message (error code).

         Supported values: Refer to @xhyperref{Q3,[Q3]}
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_stream_resp_get_pp_params */

/** @addtogroup asm_strm_cmd_dtmf_ctl
@{ */
/** Starts or stops an ASM stream DTMF tone. This command is sent to the ASM
    when a DTMF tone is to be generated.

    @relevant_apr_hdr_fields
      Opcode -- ASM_STREAM_CMD_DTMF_CTL \n
      Dst_port:
               - Session ID 1 -- ASM_MAX_SESSION_ID
               - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION

    @apr_msg_payload
      @structure{asm_stream_cmd_dtmf_ctl_t}
      @tablespace
      @inputtable{ASM_STREAM_CMD_DTMF_CTL_APR_msg_payload.tex}

    @description
      The client populates the APR token field with a unique value for each
      DTMF APR payload. The aDSP returns that token in the
      #ASM_STREAM_EVENT_DTMF_TONE_ENDED event to the client. The client uses
      the returned token to look up which DTMF tone has just ended.
        @negparspace
      After completion of DTMF tone generation, the aDSP sends the
      ASM_STREAM_EVENT_DTMF_TONE_ENDED to the client. This event is raised:
        @negparspace
      - When a finite duration tone is completely played out
      - When an infinite duration tone is stopped
      - When a tone is pre-empted by a new tone while still playing
      - By the destination AFE port when the last sample has been rendered, like
        the #ASM_DATA_EVENT_RENDERED_EOS event.
        @negparspace
      If the client sends a new DTMF command before the previous tone is fully
      played out, the new tone will pre-empt the old tone and tone generation
      will continue with the new parameters specified in the payload.
        @negparspace
      To stop the infinite tone, the client must send the
      #ASM_STREAM_CMD_DTMF_CTL with duration=0 in the payload. After
      receiving this command, the DTMF performs proper ramp down of the tone
      to avoid any pops.

    @prerequisites
      This command must be applied to a valid stream.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
    @newpage
*/
#define ASM_STREAM_CMD_DTMF_CTL                     0x00010D69

/* Payload for an ASM stream DTMF command. */
typedef struct asm_stream_cmd_dtmf_ctl_t asm_stream_cmd_dtmf_ctl_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_STREAM_CMD_DTMF_CTL command, which starts/stops
    the DTMF tone. To stop, the infinite tone must send the same command with
    duration=0.\n
    \n    
    For finite tones, there is no need to send a stop command. The library 
    stops the generation of samples after a finite duration is reached.
*/
struct asm_stream_cmd_dtmf_ctl_t
{
    uint16_t                 tone_1;
    /**< Frequency of DTMF tone 1 (in Hertz).

         Supported values: 100 to 4000 */

    uint16_t                  tone_2;
    /**< Frequency of DTMF tone 2 (in Hertz).

         Supported values: 100 to 4000 */

    uint16_t                  gain;
    /**< DTMF linear gain in Q13 format. */

    int16_t                  duration;
    /**< Duration of the DTMF tone in milliseconds. Duration includes both
         ramp up and ramp down.

         Supported values:
         - -1 -- Infinite tone
         - 0 -- Disable/stop infinite tone
         - >0 -- Finite duration
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_strm_cmd_dtmf_ctl */

/* The following AC3 parameters make use of the generic
 *  structure asm_ac3_generic_param_t
 */ 

/** ID of the AC3 Karaoke mode parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    This parameter is relevant only for karaoke streams, and it 
    applies only to the E-AC3 decoder. The command can be sent
    any time after opening a write or read/write stream, and the change takes
    effect from the next decoded frame.

    @messagepayload
      @structure{asm_ac3_generic_param_t}
      @tablespace
      @inputtable{ASM_STREAM_CMD_SET_ENCDEC_PARAM_AC3_karaoke_mode.tex}
*/
#define ASM_PARAM_ID_AC3_KARAOKE_MODE                               0x00010D73

/** Enumeration for both vocals in a karaoke stream.
*/
#define AC3_KARAOKE_MODE_NO_VOCAL                                   (0)

/** Enumeration for only the left vocal in a karaoke stream.
*/
#define AC3_KARAOKE_MODE_LEFT_VOCAL                                 (1)

/** Enumeration for only the right vocal in a karaoke stream.
*/
#define AC3_KARAOKE_MODE_RIGHT_VOCAL                                (2)

/** Enumeration for both vocal channels in a karaoke stream.
    @newpage 
*/
#define AC3_KARAOKE_MODE_BOTH_VOCAL                                 (3)

/** ID of the AC3 DRC mode parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    This parameter applies only to the E-AC3 decoder. The command can be sent
    any time after opening a write or read/write stream, and the change takes
    effect from the next decoded frame.

    @messagepayload
      @structure{asm_ac3_generic_param_t}
      @tablespace
      @inputtable{ASM_STREAM_CMD_SET_ENCDEC_PARAM_AC3_drc_mode.tex}
*/
#define ASM_PARAM_ID_AC3_DRC_MODE                                   0x00010D74

/** Enumeration for the Custom Analog mode.
*/
#define AC3_DRC_MODE_CUSTOM_ANALOG                                  (0)

/** Enumeration for the Custom Digital mode.
*/
#define AC3_DRC_MODE_CUSTOM_DIGITAL                                 (1)
 
/** Enumeration for the Line Out mode (light compression).
*/
#define AC3_DRC_MODE_LINE_OUT                                       (2)

/** Enumeration for the RF remodulation mode (heavy compression).
    @newpage 
*/
#define AC3_DRC_MODE_RF_REMOD                                       (3)

/** ID of the AC3 Dual Mono mode parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    This parameter is relevant only for dual mono streams, and it
    applies only to the E-AC3 decoder. The command can be sent
    any time after opening a write or read/write stream, and the change takes
    effect from the next decoded frame.

    @messagepayload
      @structure{asm_ac3_generic_param_t}
      @tablespace
      @inputtable{ASM_STREAM_CMD_SET_ENCDEC_PARAM_AC3_dual_mono_mode.tex}
*/
#define ASM_PARAM_ID_AC3_DUAL_MONO_MODE                             0x00010D75

/** Enumeration for playing dual mono in stereo mode.
*/
#define AC3_DUAL_MONO_MODE_STEREO                                   (0)

/** Enumeration for playing left mono.
*/
#define AC3_DUAL_MONO_MODE_LEFT_MONO                                (1)

/** Enumeration for playing right mono.
*/
#define AC3_DUAL_MONO_MODE_RIGHT_MONO                               (2)

/** Enumeration for mixing both dual mono channels and playing them.
    @newpage
*/
#define AC3_DUAL_MONO_MODE_MIXED_MONO                               (3)

/** ID of the AC3 stereo Downmix mode parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    This parameter applies only to the E-AC3 decoder. The command can be sent
    any time after opening a write or read/write stream, and the change takes
    effect from the next decoded frame.

    @messagepayload
      @structure{asm_ac3_generic_param_t}
      @tablespace
      @inputtable{ASM_STREAM_CMD_SET_ENCDEC_PARAM_AC3_downmix_mode.tex}
*/
#define ASM_PARAM_ID_AC3_STEREO_DOWNMIX_MODE                        0x00010D76

/** Enumeration for using the Downmix mode indicated in the bitstream.
*/
#define AC3_STEREO_DOWNMIX_MODE_AUTO_DETECT                         (0)

/** Enumeration for Surround Compatible mode (preserves the surround
    information).
*/
#define AC3_STEREO_DOWNMIX_MODE_LT_RT                               (1)
 
/** Enumeration for Mono Compatible mode (if the output is to be further
    downmixed to mono).
    @newpage
*/
#define AC3_STEREO_DOWNMIX_MODE_LO_RO                               (2)

/** ID of the AC3 PCM scale factor parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    This parameter is a gain setting for the output audio, and it
    applies only to the E-AC3 decoder. The command can be sent
    any time after opening a write or read/write stream, and the change takes
    effect from the next decoded frame.

    @messagepayload
      @structure{asm_ac3_generic_param_t}
      @tablespace
      @inputtable{ASM_STREAM_CMD_SET_ENCDEC_PARAM_AC3_PCM_scale_factor.tex}
*/
#define ASM_PARAM_ID_AC3_PCM_SCALEFACTOR                            0x00010D78

/** ID of the AC3 DRC boost scale factor parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    This parameter is used to scale the DRC boost parameter, which is useful
    in controlling the extent to which DRC is applied.

    This parameter applies only to the E-AC3 decoder. The command can be sent
    any time after opening a write or read/write stream, and the change takes
    effect from the next decoded frame.

    @messagepayload
      @structure{asm_ac3_generic_param_t}
      @tablespace
      @inputtable{ASM_STREAM_CMD_SET_ENCDEC_PARAM_AC3_DRC_boost_scale_factor.tex}
    @newpage
*/
#define ASM_PARAM_ID_AC3_DRC_BOOST_SCALEFACTOR                      0x00010D79

/** ID of the AC3 DRC cut scale factor parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    This parameter is used to scale the DRC cut parameter, which is useful
    in controlling the extent to which DRC is applied.

    This parameter applies only to the E-AC3 decoder. The command can be sent
    any time after opening a write or read/write stream, and the change takes
    effect from the next decoded frame.

    @messagepayload
      @structure{asm_ac3_generic_param_t}
      @tablespace
      @inputtable{ASM_STREAM_CMD_SET_ENCDEC_PARAM_AC3_DRC_cut_scale_factor.tex}
*/
#define ASM_PARAM_ID_AC3_DRC_CUT_SCALEFACTOR                        0x00010D7A

/* Structure for AC3 Generic Parameter. */
typedef struct asm_ac3_generic_param_t asm_ac3_generic_param_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the AC3 parameters in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
*/
struct asm_ac3_generic_param_t
{
    uint32_t                  generic_parameter;
    /**< AC3 generic parameter. Select from one of the following possible
         values.
       
         For #ASM_PARAM_ID_AC3_KARAOKE_MODE, supported values are:
         - AC3_KARAOKE_MODE_NO_VOCAL
         - AC3_KARAOKE_MODE_LEFT_VOCAL
         - AC3_KARAOKE_MODE_RIGHT_VOCAL
         - AC3_KARAOKE_MODE_BOTH_VOCAL
       
         For #ASM_PARAM_ID_AC3_DRC_MODE, supported values are:
         - AC3_DRC_MODE_CUSTOM_ANALOG
         - AC3_DRC_MODE_CUSTOM_DIGITAL
         - AC3_DRC_MODE_LINE_OUT
         - AC3_DRC_MODE_RF_REMOD
       
         For #ASM_PARAM_ID_AC3_DUAL_MONO_MODE, supported values are:
         - AC3_DUAL_MONO_MODE_STEREO
         - AC3_DUAL_MONO_MODE_LEFT_MONO
         - AC3_DUAL_MONO_MODE_RIGHT_MONO
         - AC3_DUAL_MONO_MODE_MIXED_MONO

         For #ASM_PARAM_ID_AC3_STEREO_DOWNMIX_MODE, supported values are:
         - AC3_STEREO_DOWNMIX_MODE_AUTO_DETECT
         - AC3_STEREO_DOWNMIX_MODE_LT_RT
         - AC3_STEREO_DOWNMIX_MODE_LO_RO
       
         For #ASM_PARAM_ID_AC3_PCM_SCALEFACTOR, supported values are
         0 to 1 in Q31 format.
       
         For #ASM_PARAM_ID_AC3_DRC_BOOST_SCALEFACTOR, supported values are
         0 to 1 in Q31 format.
       
         For #ASM_PARAM_ID_AC3_DRC_CUT_SCALEFACTOR, supported values are
         0 to 1 in Q31 format.
         @newpage */
}
#include "adsp_end_pack.h"
;

/** @xreflabel{hdr:WMAProSpeakerPositions}
    Enumeration for Raw mode (no downmixing), which specifies that all channels
    in the bitstream are to be played out as is without any downmixing.
    (Default)
*/
#define WMAPRO_CHANNEL_MASK_RAW                                      (-1)

/** Enumeration for setting the channel mask to 0. The 7.1 mode (Home
    Theater) is assigned.
*/ 
#define WMAPRO_CHANNEL_MASK_ZERO                                     0x0000

/** Speaker layout mask for one channel (Home Theater, mono).

    - Speaker front center
*/
#define WMAPRO_CHANNEL_MASK_1_C                                      0x0004

/** Speaker layout mask for two channels (Home Theater, stereo).

    - Speaker front left
    - Speaker front right
*/ 
#define WMAPRO_CHANNEL_MASK_2_L_R                                    0x0003

/** Speaker layout mask for three channels (Home Theater).

    - Speaker front left
    - Speaker front right
    - Speaker front center
*/ 
#define WMAPRO_CHANNEL_MASK_3_L_C_R                                  0x0007

/** Speaker layout mask for two channels (stereo).

    - Speaker back left
    - Speaker back right
*/
#define WMAPRO_CHANNEL_MASK_2_Bl_Br                                  0x0030

/** Speaker layout mask for four channels.

    - Speaker front left
    - Speaker front right
    - Speaker back left
    - Speaker back right
*/
#define WMAPRO_CHANNEL_MASK_4_L_R_Bl_Br                              0x0033

/** Speaker layout mask for four channels (Home Theater).

    - Speaker front left
    - Speaker front right
    - Speaker front center
    - Speaker back center
*/
#define WMAPRO_CHANNEL_MASK_4_L_R_C_Bc_HT                            0x0107
 
/** Speaker layout mask for five channels.

    - Speaker front left
    - Speaker front right
    - Speaker front center
    - Speaker back left
    - Speaker back right
*/
#define WMAPRO_CHANNEL_MASK_5_L_C_R_Bl_Br                            0x0037

/** Speaker layout mask for five channels (5 mode, Home Theater).

    - Speaker front left
    - Speaker front right
    - Speaker front center
    - Speaker side left
    - Speaker side right
*/
#define WMAPRO_CHANNEL_MASK_5_L_C_R_Sl_Sr_HT                         0x0607
 
/** Speaker layout mask for six channels (5.1 mode).

    - Speaker front left
    - Speaker front right
    - Speaker front center
    - Speaker low frequency
    - Speaker back left
    - Speaker back right
*/
#define WMAPRO_CHANNEL_MASK_5DOT1_L_C_R_Bl_Br_SLF                    0x003F
 
/** Speaker layout mask for six channels (5.1 mode, Home Theater).

    - Speaker front left
    - Speaker front right
    - Speaker front center
    - Speaker low frequency
    - Speaker side left
    - Speaker side right
*/ 
#define WMAPRO_CHANNEL_MASK_5DOT1_L_C_R_Sl_Sr_SLF_HT                 0x060F
 
/** Speaker layout mask for six channels (5.1 mode, no LFE).

    - Speaker front left
    - Speaker front right
    - Speaker front center
    - Speaker back left
    - Speaker back right
    - Speaker back center
*/
#define WMAPRO_CHANNEL_MASK_5DOT1_L_C_R_Bl_Br_Bc                     0x0137
 
/** Speaker layout mask for six channels (5.1 mode, Home Theater, no LFE).

    - Speaker front left
    - Speaker front right
    - Speaker front center
    - Speaker back center
    - Speaker side left
    - Speaker side right
*/  
#define WMAPRO_CHANNEL_MASK_5DOT1_L_C_R_Sl_Sr_Bc_HT                  0x0707

/** Speaker layout mask for seven channels (6.1 mode).

    - Speaker front left
    - Speaker front right
    - Speaker front center
    - Speaker low frequency
    - Speaker back left
    - Speaker back right
    - Speaker back center
*/
#define WMAPRO_CHANNEL_MASK_6DOT1_L_C_R_Bl_Br_Bc_SLF                 0x013F

/** Speaker layout mask for seven channels (6.1 mode, Home Theater).

    - Speaker front left
    - Speaker front right
    - Speaker front center
    - Speaker low frequency
    - Speaker back center
    - Speaker side left
    - Speaker side right
*/
#define WMAPRO_CHANNEL_MASK_6DOT1_L_C_R_Sl_Sr_Bc_SLF_HT              0x070F

/** Speaker layout mask for seven channels (6.1 mode, no LFE).

    - Speaker front left
    - Speaker front right
    - Speaker front center
    - Speaker back left
    - Speaker back right
    - Speaker front left of center
    - Speaker front right of center
*/
#define WMAPRO_CHANNEL_MASK_6DOT1_L_C_R_Bl_Br_SFLOC_SFROC            0x00F7

/** Speaker layout mask for seven channels (6.1 mode, Home Theater, no LFE).

    - Speaker front left
    - Speaker front right
    - Speaker front center
    - Speaker side left
    - Speaker side right
    - Speaker front left of center
    - Speaker front right of center
*/
#define WMAPRO_CHANNEL_MASK_6DOT1_L_C_R_Sl_Sr_SFLOC_SFROC_HT         0x0637

/** Speaker layout mask for eight channels (7.1 mode).

    - Speaker front left
    - Speaker front right
    - Speaker front center
    - Speaker back left
    - Speaker back right
    - Speaker low frequency
    - Speaker front left of center
    - Speaker front right of center
*/
#define WMAPRO_CHANNEL_MASK_7DOT1_L_C_R_Bl_Br_SLF_SFLOC_SFROC        0x00FF

/** Speaker layout mask for eight channels (7.1 mode, Home Theater).

    - Speaker front left
    - Speaker front right
    - Speaker front center
    - Speaker side left
    - Speaker side right
    - Speaker low frequency
    - Speaker front left of center
    - Speaker front right of center
    @newpage 
*/
#define WMAPRO_CHANNEL_MASK_7DOT1_L_C_R_Sl_Sr_SLF_SFLOC_SFROC_HT     0x063F

/** Parameter ID for decoder output channel mapping in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    The client can use this command to match the channel mapping at the output
    of the decoder to the channel mapping supported by the rendering device.
    If the same stream is to be routed to multiple rendering devices with
    different supported speaker positions, the client can use this command to
    specify a channel mapping that is a union of the speaker positions
    supported by each of the devices.
    
    This command is optional. In the absence of this command, the decoder
    performs a conversion from its internal channel representation, which might
    be codec specific, to the standard channel representation by preserving as
    many channels as possible.
    
    In non-tunneled mode playback mode, the client can retain the decoder
    output channel mapping. In this case, the number of channels should be set
    to zero. This option is not valid for tunneled mode.
    
    This command can be issued multiple times with the last command taking
    precedence.

    @messagepayload
      @structure{asm_dec_out_chan_map_param_t}
      @tablespace
      @inputtable{ASM_STREAM_CMD_SET_ENCDEC_PARAM_Decoder_output_channel_map.tex}
*/
#define ASM_PARAM_ID_DEC_OUTPUT_CHAN_MAP                   0x00010D82

/** @xreflabel{hdr:MaxChMapChs}
    Maximum number of decoder output channels.
    @newpage
*/
#define MAX_CHAN_MAP_CHANNELS                               16

/* Structure for decoder output channel mapping. */
typedef struct asm_dec_out_chan_map_param_t asm_dec_out_chan_map_param_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_PARAM_ID_DEC_OUTPUT_CHAN_MAP parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
*/
struct asm_dec_out_chan_map_param_t
{
    uint32_t                 num_channels;
    /**< Number of decoder output channels.
        
         Supported values: 0 to #MAX_CHAN_MAP_CHANNELS

         A value of 0 indicates native channel mapping, which is valid only
         for NT mode. This means the output of the decoder is to be
         preserved as is. */

    uint8_t                  channel_mapping[MAX_CHAN_MAP_CHANNELS];
    /**< Channel array of size num_channels.

         Supported values:
         - #PCM_CHANNEL_L
         - #PCM_CHANNEL_R
         - #PCM_CHANNEL_C
         - #PCM_CHANNEL_LS
         - #PCM_CHANNEL_RS
         - #PCM_CHANNEL_LFE
         - #PCM_CHANNEL_CS
         - #PCM_CHANNEL_LB
         - #PCM_CHANNEL_RB
         - #PCM_CHANNEL_TS
         - #PCM_CHANNEL_CVH
         - #PCM_CHANNEL_MS
         - #PCM_CHANNEL_FLC
         - #PCM_CHANNEL_FRC
         - #PCM_CHANNEL_RLC
         - #PCM_CHANNEL_RRC

         Channel[i] mapping describes channel I inside the decoder output
         buffer. Valid channel mapping values are to be present at the
         beginning of the array.

         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_strm_cmd_set_encdec_params */

/** @addtogroup asm_strm_cmd_open_write_compress
@{ */
/** Opens a playback channel to route compressed audio streams to an external
    receiver after applying the necssary packetization as specified by
    IEC 61937 and IEC 60958 standards.

    For this command, only one stream per session is allowed. Otherwise, an
    error is returned. The client can perform the packetization itself or
    have the aDSP perform packetization.

    If IEC 61937 packetization is performed by the client, this mode of
    operation is called Pass-through mode. The media format block must be
    communicated, if applicable, to the aDSP through the
    #ASM_DATA_CMD_MEDIA_FORMAT_UPDATE command before sending the actual data.

    @relevant_apr_hdr_fields
      Opcode -- ASM_STREAM_CMD_OPEN_WRITE_COMPRESSED \n
      Dst_port:
              - Session ID 1 -- ASM_MAX_SESSION_ID (0 is reserved)
              - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

    @apr_msg_payload
      @structure{asm_stream_cmd_open_write_compressed_t}
      @tablespace
      @inputtable{ASM_STREAM_CMD_OPEN_WRITE_COMPRESSED_APR_msg_payload.tex}

    @prerequisites
      None.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
      The handle is defined by the client.
*/
#define ASM_STREAM_CMD_OPEN_WRITE_COMPRESSED                       0x00010D84
               
/** Bitmask for the IEC 61937 enable flag.
*/
#define ASM_BIT_MASK_IEC_61937_STREAM_FLAG                         (0x00000001UL)

/** Shift value for the IEC 61937 enable flag.
*/
#define ASM_SHIFT_IEC_61937_STREAM_FLAG                             0

/** Bitmask for the IEC 60958 enable flag.
*/
#define ASM_BIT_MASK_IEC_60958_STREAM_FLAG                          (0x00000002UL)

/** Shift value for the IEC 60958 enable flag.
    @newpage 
*/
#define ASM_SHIFT_IEC_60958_STREAM_FLAG                              1       
               
/* Payload format for open write compressed comand */
typedef struct asm_stream_cmd_open_write_compressed_t asm_stream_cmd_open_write_compressed_t;

#include "adsp_begin_pack.h"

/** @brief Payload format for the #ASM_STREAM_CMD_OPEN_WRITE_COMPRESSED comand,
    which opens a stream for a given session ID and stream ID to be
    rendered in the compressed format.
*/
struct asm_stream_cmd_open_write_compressed_t
{
    uint32_t                    flags;
    /**< Mode flags that configure the stream for a specific format. 

         Supported values:
         - Bit 0 -- IEC 61937 compatibility
                    - 0 -- Stream is not in IEC 61937 format
                    - 1 -- Stream is in IEC 61937 format
         - Bit 1 -- IEC 60958 compatibility
                    - 0 -- Stream is not in IEC 60958 format
                    - 1 -- Stream is in IEC 60958 format
         - Bits 2 to 31 -- 0 (Reserved)

         For the same stream, bit 0 cannot be set to 0 and bit 1 cannot be set
         to 1. A compressed stream connot have IEC 60958 packetization applied
         without IEC 61937 packetization.

         @note1hang Currently, IEC 60958 packetized input streams are not
         supported. */

    uint32_t                    fmt_id;
    /**< Specifies the media type of the HDMI stream to be opened.

         Supported values:
         - #ASM_MEDIA_FMT_AC3_DEC
         - #ASM_MEDIA_FMT_EAC3_DEC
         - #ASM_MEDIA_FMT_DTS
         - #ASM_MEDIA_FMT_ATRAC
         - #ASM_MEDIA_FMT_MAT

         @note1hang This field must be set to a valid media type even if
         IEC 61937 packetization is not performed by the aDSP.
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_strm_cmd_open_write_compress */

/** @addtogroup asm_strm_cmd_open_read_compressed
@{ */
/** Opens a channel to receive compressed audio streams from an external
    receiver after removing the IEC 61937 and IEC 60958 packetization.

    For this command, only one stream per session is allowed. Otherwise, an
    error is returned.

    @relevant_apr_hdr_fields
      Opcode -- ASM_STREAM_CMD_OPEN_READ_COMPRESSED \n
      Dst_port:
              - Session ID 1 -- ASM_MAX_SESSION_ID (0 is reserved)
              - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

    @apr_msg_payload
      @structure{asm_stream_cmd_open_read_compressed_t}
      @tablespace
      @inputtable{ASM_STREAM_CMD_OPEN_READ_COMPRESSED_APR_msg_payload.tex}

    @prerequisites
      None.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
      The handle is defined by the client.
*/
#define ASM_STREAM_CMD_OPEN_READ_COMPRESSED                        0x00010D95

/* Payload format for open read compressed comand */
typedef struct asm_stream_cmd_open_read_compressed_t asm_stream_cmd_open_read_compressed_t;

#include "adsp_begin_pack.h"

/** @brief Payload format for the #ASM_STREAM_CMD_OPEN_READ_COMPRESSED comand,
    which opens a stream for a given session ID and stream ID to be received from an
    external source.
*/
struct asm_stream_cmd_open_read_compressed_t
{
     uint32_t                    mode_flags;
        /**< Mode flags that indicate whether meta information per encoded frame
             is to be provided.

             Supported values for bit 4:
             - 0 -- Return data buffer contains all encoded frames only; it does
                    not contain frame metadata.
             - 1 -- Return data buffer contains an array of metadata and encoded
                    frames.
             - Use #ASM_BIT_MASK_META_INFO_FLAG to set the bitmask and
               #ASM_SHIFT_META_INFO_FLAG to set the shift value for this bit.

             All other bits are reserved; clients must set them to zero. */

    uint32_t                    frames_per_buf;
    /**< Indicates the number of frames that need to be returned per read buffer

         Supported values:
         should be greater than 0
         */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_strm_cmd_open_read_compressed */

/** @addtogroup asm_dts_params
@{ */

/** ID of the DTS mix LFE channel to front channels parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    @messagepayload
      @structure{asm_dts_generic_param_t}
      @tablespace
      @inputtable{ASM_PARAM_ID_DTS_MIX_LFE_TO_FRONT_Message_payload.tex}
    @newpage
*/
#define ASM_PARAM_ID_DTS_MIX_LFE_TO_FRONT                          0x00010DB6

/** ID of the DTS enable parse REV2AUX parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    @messagepayload
      @structure{asm_dts_generic_param_t}
      @tablespace
      @inputtable{ASM_PARAM_ID_DTS_ENABLE_PARSE_REV2AUX_Message_payload.tex}
    @newpage
*/
#define ASM_PARAM_ID_DTS_ENABLE_PARSE_REV2AUX                         0x00010DB9

/* Structure for DTS Generic Parameter. */
typedef struct asm_dts_generic_param_t asm_dts_generic_param_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the DTS parameters in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.
*/
struct asm_dts_generic_param_t
{
    int32_t                  generic_parameter;
    /**< generic parameter used by the DTS parameter IDs.
         @vertspace{12} #ASM_PARAM_ID_DTS_MIX_LFE_TO_FRONT:
         - if enabled, mixes LFE channel to front while downmixing (if necessary).
         - Supported values: 1-> enable, 0-> disable
         - Default: disabled

         @vertspace{12} #ASM_PARAM_ID_DTS_ENABLE_PARSE_REV2AUX:
         - flag to enable parsing of rev2aux chunk in the bitstream. This chunk contains broadcast metadata.
         - Supported values: 1-> enable, 0-> disable.
         - Default: disabled.


         @newpage */
}
#include "adsp_end_pack.h"
;

/** @addtogroup asm_dts_lbr_params
@{ */

/** ID of the DTS LBR mix LFE channel to front channels parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    @messagepayload
      @structure{asm_dts_lbr_generic_param_t}
      @tablespace
      @inputtable{ASM_PARAM_ID_DTS_LBR_MIX_LFE_TO_FRONT_Message_payload.tex}
    @newpage
*/
#define ASM_PARAM_ID_DTS_LBR_MIX_LFE_TO_FRONT                           0x00010DBC

/** ID of the DTS LBR enable parse REV2AUX parameter in the
    #ASM_STREAM_CMD_SET_ENCDEC_PARAM command.

    @messagepayload
      @structure{asm_dts_lbr_generic_param_t}
      @tablespace
      @inputtable{ASM_PARAM_ID_DTS_LBR_ENABLE_PARSE_REV2AUX_Message_payload.tex}
    @newpage
*/
#define ASM_PARAM_ID_DTS_LBR_ENABLE_PARSE_REV2AUX                         0x00010DBD

/* Structure for DTS LBR Generic Parameter. */
typedef struct asm_dts_lbr_generic_param_t asm_dts_lbr_generic_param_t;

#include "adsp_begin_pack.h"
struct asm_dts_lbr_generic_param_t
{
    int32_t                  generic_parameter;
    /**< generic parameter used by the DTS parameter IDs.
         @vertspace{12} #ASM_PARAM_ID_DTS_LBR_MIX_LFE_TO_FRONT:
         - if enabled, mixes LFE channel to front while downmixing (if necessary).
         - Supported values: 1-> enable, 0-> disable
         - Default: disabled

         @vertspace{12} #ASM_PARAM_ID_DTS_LBR_ENABLE_PARSE_REV2AUX:
         - flag to enable parsing of rev2aux chunk in the bitstream. This chunk contains broadcast metadata.
         - Supported values: 1-> enable, 0-> disable.
         - Default: disabled.

         @newpage */
}
#include "adsp_end_pack.h"
;

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_ASM_STREAM_COMMANDS_H_ */
