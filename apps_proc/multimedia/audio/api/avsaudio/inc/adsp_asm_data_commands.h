/*========================================================================*/
/**
@file adsp_asm_data_commands.h

This file contains ASM data commands and events structures definitions.
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

$Header: //components/rel/audio.tx/2.0/audio/api/avsaudio/inc/adsp_asm_data_commands.h#1 $

when       who     what, where, why
--------   ---     -------------------------------------------------------
05/30/11   sw/leo  (Tech Pubs) Updated Doxygen comments for Interface Spec doc.
10/12/10   leo     (Tech Pubs) Edited/added Doxygen comments and markup.
04/15/10   rkc      Created file.

========================================================================== */

#ifndef _ADSP_ASM_DATA_COMMANDS_H_
#define _ADSP_ASM_DATA_COMMANDS_H_

#include "mmdefs.h"

#ifdef __cplusplus
extern "C" {
#endif /*__cplusplus*/

/** @addtogroup asm_data_cmd_write_data
@{ */
/** Delivers a data buffer to the ASM.

    If the client wants the aDSP to use valid timestamps in this command and
    the data being sent through this command is in RAW format, the last data
    byte must constitute an end of frame. The End of Frame flag (bit 4 of the
    uFlags field) must be set to 1. Otherwise, the aDSP will not be able to
    handle the timestamps correctly, which may result in gaps during audio
    playback.

    @relevant_apr_hdr_fields
	  Opcode -- ASM_DATA_CMD_WRITE_V2 \n
      Dst_port:
              - Session ID 1 -- ASM_MAX_SESSION_ID (0 is reserved)
              - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

    @newpage
	@apr_msg_payload
      @structure{asm_data_cmd_write_v2_t}
      @tablespace
      @inputtable{ASM_DATA_CMD_WRITE_APR_msg_payload.tex}

    @newpage
    @prerequisites
      The session/stream must be a valid and opened write or read/write 
      session/stream.
        @negparspace
      Each frame's buffer address must be a physical address that has been
      mapped via #ASM_DATA_EVENT_WRITE_DONE_V2.
        @negparspace
      When the buffer delivers uncompressed PCM data (i.e., MULTI_CHANNEL_PCM_V2 format),
      the buffer must contain the same
      number of PCM samples for all channels. If not, the aDSP discards the
      buffer and generates an ASM_DATA_EVENT_WRITE_DONE_V2 event with the error
      status code, ADSP_EBADPARAM.

	@return
      #ASM_DATA_EVENT_WRITE_DONE_V2 is raised when the buffer is fully consumed.
*/
#define ASM_DATA_CMD_WRITE_V2                                          0x00010DAB

/* Payload for an ASM Data Write command. */
typedef struct asm_data_cmd_write_v2_t asm_data_cmd_write_v2_t;

/** Definition of a timestamp valid flag bitmask.
*/
#define ASM_BIT_MASK_TIMESTAMP_VALID_FLAG                           (0x80000000UL)

/** Definition of a timestamp valid flag shift value.
*/
#define ASM_SHIFT_TIMESTAMP_VALID_FLAG                               31

/** Definition of the last buffer flag bitmask. The last buffer flag is part
    of the flags word in the ASM_DATA_CMD_WRITE_V2 command.
*/
#define ASM_BIT_MASK_LAST_BUFFER_FLAG                               (0x40000000UL)

/** Definition of the last buffer flag shift value. The last buffer flag is
    part of the flags word in the ASM_DATA_CMD_WRITE_V2 command.
*/
#define ASM_SHIFT_LAST_BUFFER_FLAG                                   30

/** Definition of the shift value for the timestamp continue flag.
    The timestamp continue flag is part of the flags field in the
    asm_data_cmd_write_v2_t structure.
*/
#define ASM_BIT_MASK_TS_CONTINUE_FLAG                               (0x20000000UL)

/** Definition of the shift value for the timestamp continue flag.
    The timestamp continue flag is part of the flags field in the
    asm_data_cmd_write_v2_t structure.
*/
#define ASM_SHIFT_TS_CONTINUE_FLAG                                   29

/** Definition of an End of Frame (EOF) flag bitmask.
*/
#define ASM_BIT_MASK_EOF_FLAG                                       (0x00000010UL)

/** Definition of an EOF flag shift value.
    @newpage
*/
#define ASM_SHIFT_EOF_FLAG                                           4

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_DATA_CMD_WRITE_V2 command, which delivers a data
    buffer to the ASM for decoding.\n
    \n
    Each frame's buffer address must be set to a physical address that
    has been mapped via #ASM_CMD_SHARED_MEM_MAP_REGIONS.
*/
struct asm_data_cmd_write_v2_t
{
    uint32_t                  buf_addr_lsw;
      /**< The 64 bit address msw-lsw should be a valid, mapped address. 64 bit address should be a multiple of 32 bytes*/

    uint32_t                  buf_addr_msw;
      /**< The 64 bit address msw-lsw should be a valid, mapped address. 64 bit address should be a multiple of 32 bytes.
       * -Address of the buffer containing the data to be decoded. The buffer should be aligned to a 32 byte boundary.
         -In the case of 32 bit Shared memory address, msw field must be set to zero.
         -In the case of 36 bit shared memory address, bit 31 to bit 4 of msw must be set to zero.
       * */
    uint32_t                  mem_map_handle;
    /**< memory map handle returned by DSP through ASM_CMD_SHARED_MEM_MAP_REGIONS command */

    uint32_t                  buf_size;
      /**< Number of valid bytes available in the buffer for decoding. The
           first byte starts at buf_addr. */

    uint32_t                  seq_id;
      /**< Optional buffer sequence ID. */

    uint32_t                  timestamp_lsw;
    /**< Lower 32 bits of the 64-bit session time in microseconds of the
         first buffer sample. */

    uint32_t                  timestamp_msw;
    /**< Upper 32 bits of the 64-bit session time in microseconds of the
         first buffer sample. */

    uint32_t                  flags;
    /**< Bitfield of flags.

         Supported values for bit 31:
         - 1 -- Valid timestamp.
         - 0 -- Invalid timestamp.
         - Use #ASM_BIT_MASK_TIMESTAMP_VALID_FLAG as the bitmask and
           #ASM_SHIFT_TIMESTAMP_VALID_FLAG as the shift value to set this bit.
       
         Supported values for bit 30:
         - 1 -- Last buffer.
         - 0 -- Not the last buffer.

         Supported values for bit 29:
         - 1 -- Continue the timestamp from the previous buffer.
         - 0 -- Timestamp of the current buffer is not related to the timestamp
                of the previous buffer.
         - Use #ASM_BIT_MASK_TS_CONTINUE_FLAG and #ASM_SHIFT_TS_CONTINUE_FLAG
           to set this bit.
  
         Supported values for bit 4:
         - 1 -- End of the frame.
         - 0 -- Not the end of frame, or this information is not known.
         - Use #ASM_BIT_MASK_EOF_FLAG as the bitmask and #ASM_SHIFT_EOF_FLAG
           as the shift value to set this bit.

         All other bits are reserved and must be set to 0.

         If bit 31=0 and bit 29=1: The timestamp of the first sample in
         this buffer continues from the timestamp of the last sample in the
         previous buffer. If there is no previous buffer (i.e., this is the
         first buffer sent after opening the stream or after a flush
         operation), or if the previous buffer does not have a valid timestamp,
         the samples in the current buffer also do not have a valid timestamp.
         They are played out as soon as possible.

         If bit 31=0 and bit 29=0: No timestamp is associated with the
         first sample in this buffer. The samples are played out as soon as
         possible.

         If bit 31=1 and bit 29 is ignored: The timestamp specified in this
         payload is honored.

         If bit 30=0: Not the last buffer in the stream. This is useful in
         removing trailing samples.

         For bit 4: The client can set this flag for every buffer sent in
         which the last byte is the end of a frame. If this flag is set, the
         buffer can contain data from multiple frames, but it should always
         end at a frame boundary. Restrictions allow the aDSP to detect an end
         of frame without requiring additional processing.
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_data_cmd_write_data */

/** @addtogroup asm_data_cmd_read_data
@{ */
/** Delivers an empty data buffer to be filled by the ASM.

    @relevant_apr_hdr_fields
	  Opcode -- ASM_DATA_CMD_READ_V2 \n
      Dst_port:
              - Session ID 1 -- ASM_MAX_SESSION_ID (0 is reserved)
              - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

	@apr_msg_payload
      @structure{asm_data_cmd_read_v2_t and asm_data_event_read_done_v2_t}
      @tablespace
      @inputtable{ASM_DATA_CMD_READ_APR_msg_payload.tex}

	@prerequisites
	  The session/stream must be a valid and opened write or read/write
	  session/stream.

	@return
      #ASM_DATA_EVENT_READ_DONE_V2 is raised when the buffer is consumed.
    @newpage
*/
#define ASM_DATA_CMD_READ_V2                                           0x00010DAC

/* Payload for an ASM Data Read command. */
typedef struct asm_data_cmd_read_v2_t asm_data_cmd_read_v2_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_DATA_CMD_READ_V2 command, which delivers a data
    buffer to the ASM for encoded data.\n
    \n    
    Each frame's buffer address must be to a physical address that has
    been mapped via #ASM_CMD_SHARED_MEM_MAP_REGIONS.
*/
struct asm_data_cmd_read_v2_t
{
    uint32_t                  buf_addr_lsw;
       /**< the 64 bit address msw-lsw should be a valid mapped address and should be a multiple of 32 bytes */

    uint32_t                  buf_addr_msw;
       /**< the 64 bit address msw-lsw should be a valid mapped address and should be a multiple of 32 bytes.
        * - Address of the buffer where the DSP puts the encoded data, potentially, at an offset specified by the uOffset field in ASM_DATA_EVENT_READ_DONE structure. The buffer should be aligned to a 32 byte boundary.
          - In the case of 32 bit Shared memory address, msw field must be set to zero.
          - In the case of 36 bit shared memory address, bit 31 to bit 4 of msw must be set to zero.
        *  */
    uint32_t                  mem_map_handle;
       /**< memory map handle returned by DSP through ASM_CMD_SHARED_MEM_MAP_REGIONS command. */

    uint32_t                  buf_size;
    /**< Number of bytes available for the aDSP to write. The aDSP starts
         writing from buf_addr. */

    uint32_t                  seq_id;
      /**< Optional buffer sequence ID.
           @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_data_cmd_read_data */

/** @addtogroup asm_data_cmd_read_compressed_data
@{ */
/** Delivers an empty data buffer to be filled by the ASM in compressed read mode.

    @relevant_apr_hdr_fields
	  Opcode -- ASM_DATA_CMD_READ_COMPRESSED_V2 \n
      Dst_port:
              - Session ID 1 -- ASM_MAX_SESSION_ID (0 is reserved)
              - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

	@apr_msg_payload
      @structure{asm_data_cmd_read_compressed_v2_t and asm_data_event_read_compressed_done_v2_t}
      @tablespace
      @inputtable{ASM_DATA_CMD_READ_COMPRESSED_APR_msg_payload.tex}

	@prerequisites
	  The session/stream must be a valid and opened write or read/write
	  session/stream.

	@return
      #ASM_DATA_EVENT_READ_DONE_V2 is raised when the buffer is consumed.
    @newpage
*/
#define ASM_DATA_CMD_READ_COMPRESSED_V2                                           0x00010DC1

/* Payload for an ASM Data Read Compressed command. */
typedef struct asm_data_cmd_read_compressed_v2_t asm_data_cmd_read_compressed_v2_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_DATA_CMD_READ_COMPRESSED_V2 command, which delivers a data
    buffer to the ASM for encoded data.\n
    \n    
    Each frame's buffer address must be to a physical address that has
    been mapped via #ASM_CMD_SHARED_MEM_MAP_REGIONS.
*/
struct asm_data_cmd_read_compressed_v2_t
{
    uint32_t                  buf_addr_lsw;
       /**< the 64 bit address msw-lsw should be a valid mapped address and should be a multiple of 32 bytes */

    uint32_t                  buf_addr_msw;
       /**< the 64 bit address msw-lsw should be a valid mapped address and should be a multiple of 32 bytes.
        * - Address of the buffer where the DSP puts the encoded data, potentially, at an offset specified by the uOffset field in
        * ASM_DATA_EVENT_READ_COMPRESSED_DONE_V2 structure. The buffer should be aligned to a 32 byte boundary.
          - In the case of 32 bit Shared memory address, msw field must be set to zero.
          - In the case of 36 bit shared memory address, bit 31 to bit 4 of msw must be set to zero.
        *  */
    uint32_t                  mem_map_handle;
       /**< memory map handle returned by DSP through ASM_CMD_SHARED_MEM_MAP_REGIONS command. */

    uint32_t                  buf_size;
    /**< Number of bytes available for the aDSP to write. The aDSP starts
         writing from buf_addr. */

    uint32_t                  seq_id;
      /**< Optional buffer sequence ID.
           @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_data_cmd_read_compressed_data */

/** @ingroup asm_data_cmd_eos
    Communicates an End of Stream (EOS) marker, which indicates that the last
    buffer in a stream has been delivered. For two or more streams routed to
    the same AFE port, the client sends unique token IDs in the APR header of
    the EOS command.

    Because the AFE generates #ASM_DATA_EVENT_RENDERED_EOS with the same token
    ID, the client can correlate the events with the EOS commands it sent for
    each stream.

    @relevant_apr_hdr_fields
	  Opcode -- ASM_DATA_CMD_EOS \n
      Dst_port:
              - Session ID 1 -- ASM_MAX_SESSION_ID (0 is reserved)
              - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

	@apr_msg_payload
	  None.

	@prerequisites
	  The session/stream must be a valid and opened write or read/write
	  session/stream.

	@return
      #ASM_DATA_EVENT_RENDERED_EOS is raised when the final sample before the
      EOS has been rendered.
    @newpage
*/
#define ASM_DATA_CMD_EOS                                            0x00010BDB

/** @addtogroup asm_data_cmd_media_format_update
@{ */
/** Indicates an update to the media format, which applies to future buffers
    written to this stream. This message must be properly sequenced with
    corresponding ASM_DATA_WRITE messages, i.e., it must be sent on the data
    path.

    This command is also sent by the client after opening a write stream or
    read/write stream and before sending any #ASM_DATA_CMD_WRITE_V2 commands.
    This is required to configure the decoder properly. If ASM_DATA_CMD_WRITE_V2
    commands arrive before this command, the data buffers are discarded and 
    #ASM_DATA_EVENT_WRITE_DONE_V2 events are raised with an error status for
    these commands.

    @relevant_apr_hdr_fields
	  Opcode -- ASM_DATA_CMD_MEDIA_FMT_UPDATE_V2 \n
      Dst_port:
              - Session ID 1 -- ASM_MAX_SESSION_ID (0 is reserved)
              - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

	@apr_msg_payload
      @structure{asm_data_cmd_media_fmt_update_t}
      @tablespace
      @inputtable{ASM_DATA_CMD_MEDIA_FMT_UPDATE_APR_msg_payload.tex}

	@prerequisites
	  The session/stream must be a valid and opened write or read/write
	  session/stream.

	@return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
    @newpage
*/
#define ASM_DATA_CMD_MEDIA_FMT_UPDATE_V2                            0x00010D98

/* Structure for a Media Format Update command. */
typedef struct asm_data_cmd_media_fmt_update_v2_t asm_data_cmd_media_fmt_update_v2_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_DATA_CMD_MEDIA_FMT_UPDATE_V2 command, which
    indicates an update to the media format that applies to future buffers
    written to this stream.\n
    \n    
    Immediately following this structure is a media format block of size
    fmt_blk_size.
*/
struct asm_data_cmd_media_fmt_update_v2_t
{
    uint32_t                    fmt_blk_size;
    /**< Media format block size in bytes.
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_data_cmd_media_format_update */

/** @ingroup asm_stream_event_dtmf_tone_ended
    Indicates that an ASM stream event for DTMF tone has ended. This is raised
    by the AFE for write streams.

    @relevant_apr_hdr_fields
      Opcode -- ASM_STREAM_EVENT_DTMF_TONE_ENDED \n
      Src_port:
              - Session ID 1 -- ASM_MAX_SESSION_ID
              - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION

    @apr_msg_payload
      None.

    @description
      After completion of DTMF tone generation, the aDSP sends this
      event to the client under the following conditions:
        @negparspace
      - When a finite duration tone is completely played out
      - When an infinite duration tone is stopped
      - When a tone is pre-empted by a new tone while still playing
      - By the destination AFE port when the last sample has been rendered,
        like the #ASM_DATA_EVENT_RENDERED_EOS event.
        @negparspace
      The destination port in this event is same as the source port sent in
      the #ASM_STREAM_CMD_DTMF_CTL command.
        @negparspace
      This event cannot be raised when the client sends a flush command
      immediately after sending the tone end command. The client is responsible
      for tagging each DTMF command, e.g., using a counter value in the APR
      token field so that it can safely ignore the ASM_DATA_EVENT_RENDERED_EOS
      event that should have been flushed.

    @prerequisites
      None.

    @return
      None.
    @newpage
*/
#define ASM_STREAM_EVENT_DTMF_TONE_ENDED                           0x00010C1E

/** @ingroup asm_data_event_rendered_eos
    Indicates that the last sample in the write or read/write stream has been
    successfully rendered after an EOS data command is received, and that no
    more samples will be rendered.

    This event is raised by the AFE for tunneled playback streams, or by the
    encoder service for non-tunneled streams.

	@relevant_apr_hdr_fields
	  Opcode -- ASM_DATA_EVENT_RENDERED_EOS \n
      Dst_port:
              - Session ID 1 -- ASM_MAX_SESSION_ID (0 is reserved)
              - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

	@apr_msg_payload
	  None.

	@prerequisites
	  The stream must be open for write.

	@return
	  None.
    @newpage
*/
#define ASM_DATA_EVENT_RENDERED_EOS                                 0x00010C1C

/** @ingroup asm_data_event_eos_response
    Indicates that data is being read from a read or read/write stream that
    has reached the EOS.

    @note1hang Do not confuse this event with the acknowledgment to
    #ASM_DATA_CMD_EOS received on a write stream. This ASM_DATA_EVENT_EOS is
    an independent event generated toward the client.

    @relevant_apr_hdr_fields
	  Opcode -- ASM_DATA_EVENT_EOS \n
      Dst_port:
              - Session ID 1 -- ASM_MAX_SESSION_ID (0 is reserved)
              - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

	@apr_msg_payload
	  None.

	@prerequisites
	  The stream must be open for read or read/write.

	@return
	  None.
    @newpage
*/
#define ASM_DATA_EVENT_EOS                                          0x00010BDD

/** @addtogroup asm_data_event_write_done
@{ */
/** Indicates that the referenced buffer has been fully consumed and is again
    available to the client.

    @relevant_apr_hdr_fields
	  Opcode -- ASM_DATA_EVENT_WRITE_DONE_V2 \n
      Token -- Matches the token in the #ASM_DATA_CMD_WRITE_V2 command for
               which this event is being raised \n
      Dst_port:
              - Session ID 1 -- ASM_MAX_SESSION_ID (0 is reserved)
              - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

	@apr_msg_payload
      @structure{asm_data_event_write_done_t and asm_data_read_buf_metadata_v2_t}
      @tablespace
      @inputtable{ASM_DATA_EVENT_WRITE_DONE_APR_msg_payload.tex}

	@prerequisites
	  The session/stream must be a valid and open write or read/write
      session/stream.
        @negparspace
      A corresponding #ASM_DATA_WRITE command must have been received and
      processed.

	@return
	  None.
    @newpage
*/
#define ASM_DATA_EVENT_WRITE_DONE_V2                                   0x00010D99

/* Structure for a data write done event payload. */
typedef struct asm_data_event_write_done_v2_t asm_data_event_write_done_v2_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_DATA_EVENT_WRITE_DONE_V2 event, which indicates
    that the referenced buffer has been fully consumed and is again available
    to the client.    
*/
struct asm_data_event_write_done_v2_t
{
    uint32_t                  buf_addr_lsw;
    /**< lsw of the 64 bit address */
    uint32_t                  buf_addr_msw;
    /**< msw of the 64 bit address. address given by the client in ASM_DATA_CMD_WRITE_V2 command. */
    uint32_t                  mem_map_handle;
    /**< memory map handle in the ASM_DATA_CMD_WRITE_V2  */

    uint32_t                  status;
    /**< Status message (error code) that indicates whether the referenced
         buffer has been successfully consumed.

         Supported values: Refer to @xhyperref{Q3,[Q3]}
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_data_event_write_done */

/** @addtogroup asm_data_event_read_done
@{ */
/** Indicates that the referenced buffer has been filled and is available to
    the client.

    The referenced buffer contains an optional array of metadata if the client
    requested it in an open command. This is followed by a variable amount of
    empty space, assuming metadata is present, followed by an integer number
    of encoded frames. Metadata presence is indicated by bit 30 of the flags
    field in the APR message payload structure.

    @relevant_apr_hdr_fields
	  Opcode -- ASM_DATA_EVENT_READ_DONE_V2 \n
      Token -- Matches the token in the #ASM_DATA_CMD_READ_V2 command for which
               this event is being raised \n
      Dst_port:
              - Session ID 1 -- ASM_MAX_SESSION_ID (0 is reserved)
              - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

	@apr_msg_payload
      @structure{asm_data_event_read_done_v2_t}
      @tablespace
      @inputtable{ASM_DATA_EVENT_READ_DONE_APR_msg_payload.tex}
	
	@prerequisites
	  The session/stream must be a valid and open write or read/write
      session/stream.
        @negparspace
      A corresponding #ASM_DATA_READ command must have been received and
      processed.

	@return
	  None.

    @parspace Frame metadata
      Information for each encoded frame is inside the buffer.
      @negparspace
      @structure{asm_data_read_buf_metadata_v2_t}
      @tablespace
      @inputtable{ASM_DATA_EVENT_READ_DONE_Frame_metadata.tex}

    @parspace Buffer content
      A multiframe buffer becomes an array of Hexagon frame metadata
      information structures, and then an array of audio frames. Encoded
      frames start with  <i>offset</i>.
      @tablespace
      @inputtable{ASM_DATA_EVENT_READ_DONE_Buffer_content.tex}
    @newpage
*/
#define ASM_DATA_EVENT_READ_DONE_V2                                    0x00010D9A

/** Definition of the frame metadata flag bitmask.
*/
#define ASM_BIT_MASK_FRAME_METADATA_FLAG                            (0x40000000UL)

/** Definition of the frame metadata flag shift value.
    @newpage
*/
#define ASM_SHIFT_FRAME_METADATA_FLAG                               30

/* Structure for the data read done event payload. */
typedef struct asm_data_event_read_done_v2_t asm_data_event_read_done_v2_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_DATA_EVENT_READ_DONE_V2 event, which indicates
    that the referenced buffer has been filled and is available to the client.
*/
struct asm_data_event_read_done_v2_t
{
   uint32_t                  status;
   /**< Status message (error code).

         Supported values: Refer to @xhyperref{Q3,[Q3]} */

   uint32_t                  buf_addr_lsw;
   /**< 64 bit address msw-lsw is a valid, mapped address. 64 bit address is a multiple of 32 bytes. */
   uint32_t                  buf_addr_msw;
   /**< 64 bit address msw-lsw is a valid, mapped address. 64 bit address is a multiple of 32 bytes.
    *
    * -Same address provided by the client in ASM_DATA_CMD_READ_V2
      -In the case of 32 bit Shared memory address, msw field is set to zero.
      -In the case of 36 bit shared memory address, bit 31 to bit 4 of msw is set to zero.
    * */

   uint32_t                  mem_map_handle;
   /**< memory map handle in the ASM_DATA_CMD_READ_V2  */

   uint32_t                  enc_frames_total_size;
   /**< Total size of the encoded frames in bytes.

        Supported values: >0 */

   uint32_t                  offset;
   /**< Offset (from buf_addr) to the first byte of the first encoded frame.
        All encoded frames are consecutive, starting from this offset.
          
        Supported values: > 0
        @newpage */

   uint32_t                  timestamp_lsw;
   /**< Lower 32 bits of the 64-bit session time in microseconds of the
        first sample in the buffer.
        If Bit 5 of mode_flags flag of ASM_STREAM_CMD_OPEN_READ_V2 is 1 then
        the 64 bit timestamp is absolute capture time otherwise it is
        relative session time. The absolute timestamp doesn’t reset unless
        the system is reset.*/

   uint32_t                  timestamp_msw;
   /**< Upper 32 bits of the 64-bit session time in microseconds of the
        first sample in the buffer. */

   uint32_t                  flags;
   /**< Bitfield of flags. Bit 30 indicates whether frame metadata is
        present. If frame metadata is present, num_frames consecutive
        instances of @xhyperref{hdr:FrameMetaData,Frame metadata} start at
        the buffer address.

        Supported values for bit 31:
        - 1 -- Timestamp is valid.
        - 0 -- Timestamp is invalid.
        - Use #ASM_BIT_MASK_TIMESTAMP_VALID_FLAG and
          #ASM_SHIFT_TIMESTAMP_VALID_FLAG to set this bit.

        Supported values for bit 30:
        - 1 -- Frame metadata is present.
        - 0 -- Frame metadata is absent.
        - Use #ASM_BIT_MASK_FRAME_METADATA_FLAG and
          #ASM_SHIFT_FRAME_METADATA_FLAG to set this bit.

        All other bits are reserved; the aDSP sets them to 0. */

   uint32_t                  num_frames;
   /**< Number of encoded frames in the buffer. */

   uint32_t                  seq_id;
   /**< Optional buffer sequence ID.
        @newpage */
}
#include "adsp_end_pack.h"
;

/* Structure for metadata that can be put in the data read buffer. */
typedef struct asm_data_read_buf_metadata_v2_t asm_data_read_buf_metadata_v2_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the metadata that can be put in the data read buffer.

    This is a metadata structure that can be present in the buffer when
    #ASM_DATA_EVENT_READ_DONE_V2 is issued. Following is the resulting buffer
    from the read event.

    A multiframe buffer becomes an array of Hexagon frame metadata information
    structures, then an array of audio frames:

    - MetaData 0 (optional)
    - MetaData 1 (optional) \n
        ... \n
    - MetaData n (optional)
    - Extra space determined by the Hexagon processor (optional)
    - Frame 0 Payload (this always starts at an offset from buf_addr)
    - Frame 1 Payload \n
        ... \n
    - Frame n Payload
*/
struct asm_data_read_buf_metadata_v2_t
{
   uint32_t          offset;
   /**< Offset from buf_addr in #ASM_DATA_EVENT_READ_DONE_PAYLOAD to
        the frame associated with this metadata.
          
        Supported values: > 0 */

   uint32_t          frm_size;
   /**< Size of the encoded frame in bytes.

        Supported values: > 0 */

   uint32_t          num_encoded_pcm_samples;
   /**< Number of encoded PCM samples (per channel) in the frame 
        associated with this metadata.
          
        Supported values: > 0 */

   uint32_t          timestamp_lsw;
   /**< Lower 32 bits of the 64-bit session time in microseconds of the
        first sample for this frame.
        If Bit 5 of mode_flags flag of ASM_STREAM_CMD_OPEN_READ_V2 is 1 then
        the 64 bit timestamp is absolute capture time otherwise it is
        relative session time. The absolute timestamp doesn’t reset unless
        the system is reset. */

   uint32_t          timestamp_msw;
   /**< Lower 32 bits of the 64-bit session time in microseconds of the
        first sample for this frame. */

   uint32_t          flags;
   /**< Frame flags.

        Supported values for bit 31:
        - 1 -- Time stamp is valid
        - 0 -- Time stamp is not valid
        - All other bits are reserved; the aDSP sets them to 0.
       @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_data_event_read_done */


/** @addtogroup asm_data_event_read_compressed_done
@{ */
/** Indicates that the referenced buffer has been filled and is available to
    the client.

    The referenced buffer contains an optional array of metadata if the client
    requested it in an open command. This is followed by a variable amount of
    empty space, assuming metadata is present, followed by an integer number
    of encoded frames. Metadata presence is indicated by bit 30 of the flags
    field in the APR message payload structure.

    @relevant_apr_hdr_fields
	  Opcode -- ASM_DATA_EVENT_READ_COMPRESSED_DONE_V2 \n
      Token -- Matches the token in the #ASM_DATA_CMD_READ_COMPRESSED_V2 command for which
               this event is being raised \n
      Dst_port:
              - Session ID 1 -- ASM_MAX_SESSION_ID (0 is reserved)
              - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

	@apr_msg_payload
      @structure{asm_data_event_read_done_v2_t}
      @tablespace
      @inputtable{ASM_DATA_EVENT_READ_DONE_APR_msg_payload.tex}

	@prerequisites
	  The session/stream must be a valid and open write or read/write
      session/stream.
        @negparspace
      A corresponding #ASM_DATA_READ command must have been received and
      processed.

	@return
	  None.

    @parspace Frame metadata
      Information for each encoded frame is inside the buffer.
      @negparspace
      @structure{asm_data_read_buf_metadata_compressed_v2_t}
      @tablespace
      @inputtable{ASM_DATA_EVENT_READ_COMPRESSED_DONE_Frame_metadata.tex}

    @parspace Buffer content
      A multiframe buffer becomes an array of Hexagon frame metadata
      information structures, and then an array of audio frames. Encoded
      frames start with  <i>offset</i>.
      @tablespace
      @inputtable{ASM_DATA_EVENT_READ_COMPRESSED_DONE_Buffer_content.tex}
    @newpage
*/
#define ASM_DATA_EVENT_READ_COMPRESSED_DONE_V2                                    0x00010DC2

/* Structure for the data read done event payload. */
typedef struct asm_data_event_read_compressed_done_v2_t asm_data_event_read_compressed_done_v2_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_DATA_EVENT_READ_COMPRESSED_DONE_V2 event, which indicates
    that the referenced buffer has been filled and is available to the client.
*/
struct asm_data_event_read_compressed_done_v2_t
{
   uint32_t                  status;
   /**< Status message (error code).

         Supported values: Refer to @xhyperref{Q3,[Q3]} */

   uint32_t                  buf_addr_lsw;
   /**< 64 bit address msw-lsw is a valid, mapped address. 64 bit address is a multiple of 32 bytes. */
   uint32_t                  buf_addr_msw;
   /**< 64 bit address msw-lsw is a valid, mapped address. 64 bit address is a multiple of 32 bytes.
    *
    * -Same address provided by the client in ASM_DATA_CMD_READ_COMPRESSED_V2
      -In the case of 32 bit Shared memory address, msw field is set to zero.
      -In the case of 36 bit shared memory address, bit 31 to bit 4 of msw is set to zero.
    * */

   uint32_t                  mem_map_handle;
   /**< memory map handle in the ASM_DATA_CMD_READ_COMPRESSED_V2  */

   uint32_t                  enc_frames_total_size;
   /**< Total size of the encoded frames in bytes.

        Supported values: >0 */

   uint32_t                  offset;
   /**< Offset (from buf_addr) to the first byte of the first encoded frame.
        All encoded frames are consecutive, starting from this offset.

        Supported values: > 0
        @newpage */

   uint32_t                  timestamp_lsw;
   /**< Lower 32 bits of the 64-bit session time in microseconds of the
        first sample in the buffer.
        If Bit 5 of mode_flags flag of ASM_STREAM_CMD_OPEN_READ_COMPRESSED_V2 is 1 then
        the 64 bit timestamp is absolute capture time otherwise it is
        relative session time. The absolute timestamp doesn’t reset unless
        the system is reset.*/

   uint32_t                  timestamp_msw;
   /**< Upper 32 bits of the 64-bit session time in microseconds of the
        first sample in the buffer. */

   uint32_t                  flags;
   /**< Bitfield of flags. Bit 30 indicates whether frame metadata is
        present. If frame metadata is present, num_frames consecutive
        instances of @xhyperref{hdr:FrameMetaData,Frame metadata} start at
        the buffer address.

        Supported values for bit 31:
        - 1 -- Timestamp is valid.
        - 0 -- Timestamp is invalid.
        - Use #ASM_BIT_MASK_TIMESTAMP_VALID_FLAG and
          #ASM_SHIFT_TIMESTAMP_VALID_FLAG to set this bit.

        Supported values for bit 30:
        - 1 -- Frame metadata is present.
        - 0 -- Frame metadata is absent.
        - Use #ASM_BIT_MASK_FRAME_METADATA_FLAG and
          #ASM_SHIFT_FRAME_METADATA_FLAG to set this bit.

        All other bits are reserved; the aDSP sets them to 0. */

   uint32_t                  num_frames;
   /**< Number of encoded frames in the buffer. */

   uint32_t                  seq_id;
   /**< Optional buffer sequence ID.
        @newpage */
}
#include "adsp_end_pack.h"
;

/* Structure for metadata that can be put in the data read compressed buffer. */
typedef struct asm_data_read_buf_metadata_compressed_v2_t asm_data_read_buf_metadata_compressed_v2_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the metadata that can be put in the data read compressed buffer.

    This is a metadata structure that can be present in the buffer when
    #ASM_DATA_EVENT_READ_COMPRESSED_DONE_V2 is issued. Following is the resulting buffer
    from the read event.

    A multiframe buffer becomes an array of Hexagon frame metadata information
    structures, then an array of audio frames:

    - MetaData 0 (optional)
    - MetaData 1 (optional) \n
        ... \n
    - MetaData n (optional)
    - Extra space determined by the Hexagon processor (optional)
    - Frame 0 Payload (this always starts at an offset from buf_addr)
    - Frame 1 Payload \n
        ... \n
    - Frame n Payload
*/
struct asm_data_read_buf_metadata_compressed_v2_t
{
    uint32_t          bitstream_id;
   /**< If there are multiple audio bitstreams transferred over
    * same IEC 61937 data streams. In such cases this is the
    sequence id of the stream to which this frame belongs
    */

   uint32_t          format_id;
   /**< Indicates the format of the incoming stream.
              Supported values: Any valid format ID */

   uint32_t          data_offset;
   /**< Offset from buf_addr in #ASM_DATA_EVENT_READ_COMPRESSED_DONE payload to
        the frame associated with this metadata.

        Supported values: > 0 */

   uint32_t          data_size;
   /**< Size of the encoded frame in bytes.

        Supported values: > 0 */

   uint32_t          command_offset;
      /**< Offset from buf_addr in the ASM_DATA_EVENT_READ_COMPRESSED_DONE
        payload to the command associated with this frame in bytes.
        Command offset is valid only if command size is greater than 0.
        Commands are present only for DTS Type 4 streams received through HDMI input.

        Supported values: Any*/

   uint32_t          command_size;
      /**< Size of the command in bytes. Command size = 0 indicates that
       no command is present. Currently DSP does not support these commands
       and this field will be set to 0.

       Supported values: Any*/

   uint32_t          num_encoded_pcm_samples;
   /**< Number of encoded PCM samples (per channel) in the frame
        associated with this metadata.

        Supported values: > 0 */

   uint32_t          timestamp_lsw;
   /**< Lower 32 bits of the 64-bit session time in microseconds of the
        first sample for this frame.
        If Bit 5 of mode_flags flag of ASM_STREAM_CMD_OPEN_READ_V2 is 1 then
        the 64 bit timestamp is absolute capture time otherwise it is
        relative session time. The absolute timestamp doesn’t reset unless
        the system is reset. */

   uint32_t          timestamp_msw;
   /**< Lower 32 bits of the 64-bit session time in microseconds of the
        first sample for this frame. */

   uint32_t          flags;
   /**< Frame flags.

        Supported values for bit 31:
        - 1 -- Time stamp is valid
        - 0 -- Time stamp is not valid
        - All other bits are reserved; the aDSP sets them to 0.
       @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_data_event_read_compressed_done */

/** @addtogroup asm_data_event_dec_sr_cm_update
@{ */
/** Notifies the client of a change in the data sampling rate or Channel mode.
    This event is raised by the decoder service.

    @relevant_apr_hdr_fields
	  Opcode -- ASM_DATA_EVENT_SR_CM_CHANGE_NOTIFY \n
      Dst_port:
              - Session ID 1 -- ASM_MAX_SESSION_ID (0 is reserved)
              - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION

	@apr_msg_payload
      @structure{asm_data_event_sr_cm_change_notify_t}
      @tablespace
      @inputtable{ASM_DATA_EVENT_SR_CM_CHANGE_NOTIFY_APR_msg_payload.tex}

    @keep{8}
	@description
      This event is raised when both of these conditions are true:
        @negparspace
      - The event is enabled through the mode flags of
        #ASM_STREAM_CMD_OPEN_WRITE_V2 or #ASM_STREAM_CMD_OPEN_READWRITE_V2.
      - The decoder detects a change in the output sampling frequency or the
        number/positioning of output channels, or if it is the first frame
        decoded.
        @negparspace
      The new sampling frequency or the new channel configuration is
      communicated back to the client asynchronously. Channel configuration
      consists of the number of channels and the positioning of each channel.
      The event is helpful for clients to update the user interface upon
      changes in decoded bitstream parameters.
        @negparspace
      @note1hang This event is not be raised for MIDI because there is no
                 concept of sampling frequency or number of channels in a
                 MIDI file.

	@prerequisites
      The stream must be in the Open state.
        @negparspace
      At least one complete frame of encoded data must be available to the
      decoder.

	@return
	  None.
    @newpage
*/
#define ASM_DATA_EVENT_SR_CM_CHANGE_NOTIFY                        0x00010C65

/* Payload for decoder sampling rate or channel mode change event. */
typedef struct asm_data_event_sr_cm_change_notify_t asm_data_event_sr_cm_change_notify_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_DATA_EVENT_SR_CM_CHANGE_NOTIFY event.

    This event is raised when the following conditions are both true:

    - The event is enabled through the mode_flags of
      #ASM_STREAM_CMD_OPEN_WRITE_V2 or #ASM_STREAM_CMD_OPEN_READWRITE_V2.
    - The decoder detects a change in either the output sampling frequency or
      the number/positioning of output channels, or if it is the first frame
      decoded.

    This event is not raised (even if enabled) if the decoder is MIDI, because
*/
struct asm_data_event_sr_cm_change_notify_t
{
    uint32_t                  sample_rate;
    /**< New sampling rate (in Hertz) after detecting a change in the
         bitstream.

         Supported values: 2000 to 48000 */
    
    uint16_t                  num_channels;
    /**< New number of channels after detecting a change in the bitstream.

         Supported values: 1 to 8 */
    
    uint16_t                  reserved;
    /**< Reserved for future use. This field must be set to 0.
         @newpage */
    
    uint8_t                   channel_mapping[8];
    /**< Mapping for each of the channels.
    
         Channel[i] mapping describes the position of channel I, where
         0 @le num_channels. First num_channels entries in the array can have the following
         values:
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
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_data_event_dec_sr_cm_update */

/** @addtogroup asm_data_event_enc_sr_cm_update
@{ */
/** Notifies the client of a data sampling rate or channel mode change. This
    event is raised by the encoder service.

    @relevant_apr_hdr_fields
      Opcode -- ASM_DATA_EVENT_ENC_SR_CM_CHANGE_NOTIFY \n
      Src_port:
              - Session ID 1 -- ASM_MAX_SESSION_ID
              - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION

    @apr_msg_payload
      @structure{asm_data_event_enc_sr_cm_change_notify_t}
      @tablespace
      @inputtable{ASM_DATA_EVENT_ENC_SR_CM_CHANGE_NOTIFY_APR_msg_payload.tex}
    
    @keep{6}
    @description
      This event is raised when all of the following conditions are true:
        @negparspace
      - The stream is opened through #ASM_STREAM_CMD_OPEN_READWRITE_V2 or
        #ASM_STREAM_CMD_OPEN_READ_V2.
      - The encoding format is ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2.
      - Native mode encoding was requested in the encoder configuration (i.e.,
        the channel number was 0), the sample rate was 0, or both were 0.
      - The input data frame at the encoder is the first one, or the sampling
        rate/channel mode is different from the previous input data frame.
          @negparspace
      Immediately after this event is raised, the encoder service stops
      processing input data and waits for the client to issue
      #ASM_STREAM_CMD_FLUSH_READBUFS. Data processing resumes after
      successfully handling #ASM_STREAM_CMD_FLUSH_READBUFS.

    @prerequisites
      The stream must have been opened by #ASM_STREAM_CMD_OPEN_READWRITE_V2 or
      #ASM_STREAM_CMD_OPEN_READ_V2.

    @return
      None.
    @newpage
*/
#define ASM_DATA_EVENT_ENC_SR_CM_CHANGE_NOTIFY                    0x00010BDE

/* Payload for encoder sampling rate, channel mode change event. */
typedef struct asm_data_event_enc_sr_cm_change_notify_t asm_data_event_enc_sr_cm_change_notify_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_DATA_EVENT_ENC_SR_CM_CHANGE_NOTIFY event.

    This event is raised when the following conditions are all true:

    - The stream is opened through #ASM_STREAM_CMD_OPEN_READWRITE_V2 or
      #ASM_STREAM_CMD_OPEN_READ_V2.
    - The encoding format is #ASM_MEDIA_FMT_MULTI_CHANNEL_PCM_V2.
    - Native mode encoding has been requested in the encoder configuration (i.e.,
      either channel number), the sample rate, or both are 0.
    - If the input data frame at the encoder is the first one, or if the
      sampling rate/channel mode is different from the previous input data frame.
 
    Immediately after this event is raised, the encoder service stops
    processing input data and waits for the client to issue
    #ASM_STREAM_CMD_FLUSH_READBUFS. Data processing is resumed after
    successfully handling #ASM_STREAM_CMD_FLUSH_READBUFS.
 */
struct asm_data_event_enc_sr_cm_change_notify_t
{
    uint32_t                  sample_rate;
    /**< New sampling rate (in Hertz) after detecting a change in the input
         data.

         Supported values: 2000 to 48000 */
    
    uint16_t                  num_channels;
    /**< New number of channels after detecting a change in the input data.

         Supported values: 1 to 8
         @newpage*/
    
    uint16_t                  bits_per_sample;
    /**< New bits per sample after detecting a change in the input data.

         Supported values: 16, 24 */
    
    uint8_t                   channel_mapping[8];
    /**< New channel mapping.

         First num_channels entries in the array can have the following values:
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
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_data_event_enc_sr_cm_update */

/** @addtogroup asm_data_cmd_write_spa_data
@{ */
/** @xreflabel{hdr:ASMDataCmdSPA}
    Delivers an empty data buffer to the ASM for filling with spectrum analyzer
    data.

    @relevant_apr_hdr_fields
	  Opcode -- ASM_DATA_CMD_SPA_V2 \n
      Dst_port:
              - Session ID 1 -- ASM_MAX_SESSION_ID (0 is reserved)
              - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

	@apr_msg_payload
      @structure{ASM_DATA_CMD_SPA_V2_t}
      @tablespace
      @inputtable{ASM_DATA_CMD_SPA_V2_APR_msg_payload.tex}

	@prerequisites
      The session/stream must be a valid and opened session/stream.
        @negparspace
      The spectrum analyzer feature must be enabled using
	  #ASM_STREAM_CMD_SET_PP_PARAMS_V2.

	@return
	  #ASM_DATA_EVENT_SPA_DONE_V2 is raised when the buffer is filled.
    @newpage
*/
#define ASM_DATA_CMD_SPA_V2                                          0x00010D9B

/* Payload for a Spectrum Analyzer Data command. */
typedef struct asm_data_cmd_spa_v2_t asm_data_cmd_spa_v2_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_DATA_CMD_SPA_V2 command, which delivers a data
    buffer to the Spectrum Analyzer module to be filled with spectrum data.
*/
struct asm_data_cmd_spa_v2_t
{
    uint32_t                  buf_addr_lsw;
    /**<lower 32 bits of the 64 bit valid mapped address.*/
    uint32_t                  buf_addr_msw;
    /**<upper  32 bits of the 64 bit valid mapped address.
     *   -address of the buffer where the DSP puts the spectrum analyzer data
         -In the case of 32 bit Shared memory address, msw field must be set to zero.
         -In the case of 36 bit shared memory address, bit 31 to bit 4 of msw must be set to zero
     * */
    uint32_t                  mem_map_handle;
    /**< memory map handle returned by DSP through ASM_CMD_SHARED_MEM_MAP_REGIONS command */

    uint32_t                  buf_size;
    /**< Number of valid bytes available in the buffer for decoding. The first
         byte starts at buf_addr.
         @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_data_cmd_write_spa_data */

/** @addtogroup asm_data_event_spa_done
@{ */
/** @xreflabel{hdr:ASMDataEventSpaDone}
    Sends a spectrum analyzer done event, which indicates that the referenced
    buffer has been filled with spectrum analyzer data and is available to the
    client. The format of the filled buffer is described in the
    @xhyperref{tbl:BufferContentEvtSpaDone,Buffer content} table below.

    @relevant_apr_hdr_fields
      Opcode -- ASM_DATA_EVENT_SPA_DONE_V2 \n
      Token -- Matches the token in the original ASM_DATA_CMD_SPA_V2 buffer \n
      Src_port:
              - Session ID 1 -- ASM_MAX_SESSION_ID (0 is reserved)
              - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

    @apr_msg_payload
      @structure{ASM_DATA_EVENT_SPA_DONE_V2_t}
      @tablespace
      @inputtable{ASM_DATA_EVENT_SPA_DONE_V2_APR_msg_payload.tex}
    
    @prerequisites
      The session/stream must be a valid, open session/stream.
        @negparspace
      A corresponding #ASM_DATA_CMD_SPA_V2 command must have been received and
      processed.

    @return
      None.

    @parspace Buffer content
      This buffer contains the spectral coefficients and, optionally, the PCM
      data that was used to calculate these coefficients.
      @negparspace
      @structure{asm_data_spa_spectrum_data_header_t and
      asm_data_spa_pcm_data_header_t}
      @tablespace
      @inputtable{ASM_DATA_EVENT_SPA_DONE_V2_Buffer_content.tex}
*/
#define ASM_DATA_EVENT_SPA_DONE_V2                                          0x00010D9C

/** Definition of the spectrum analyzer timestamp valid flag mask.
*/
#define ASM_BIT_MASK_SPA_TIMESTAMP_VALID_FLAG                        (0x80000000UL)

/** Definition of the spectrum analyzer timestamp valid flag shift value.
*/
#define ASM_SHIFT_SPA_TIMESTAMP_VALID_FLAG                               31

/** Definition of the spectrum analyzer PCM data flag bitmask.
*/
#define ASM_BIT_MASK_SPA_PCM_DATA_FLAG                               (0x40000000UL)

/** Definition of the spectrum analyzer PCM data flag shift value.
    @newpage
*/
#define ASM_SHIFT_SPA_PCM_DATA_FLAG                                      30

/* Payload for a spectrum analyzer done event. */
typedef struct asm_data_event_spa_done_v2_t asm_data_event_spa_done_v2_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_DATA_EVENT_SPA_DONE_V2 event, which indicates that
    the spectrum analyzer buffer has been filled and is available to the client.
*/
struct asm_data_event_spa_done_v2_t
{
   uint32_t                  status;
   /**< Status message (error code).

        Supported values:
        - ADSP_EOK -- Success case. 
        - ADSP_EUNEXPECTED -- Spectrum analyzer is disabled. 
        - Other aDSP error codes -- Memory mapping failure or general
          failure.
            
        For more details, refer to @xhyperref{Q3,[Q3]}. */

   uint32_t                  flags;
   /**< Bitfield of flags.

        Supported values for bit 31, which indicates whether the
        timestamp_msw and timestamp_lsw fields are valid:
        - 1 -- Timestamp is valid.
        - 0 -- Timestamp is not valid.
        - Use #ASM_BIT_MASK_SPA_TIMESTAMP_VALID_FLAG and
          #ASM_SHIFT_SPA_TIMESTAMP_VALID_FLAG to set this bit.

        @keep{8}
        Supported values for bit 30, which indicates whether the PCM data
        header and PCM data are present in the returned buffer:
        - 1 -- PCM data is present along with spectrum coefficients.
        - 0 -- PCM data is absent.
        - Use #ASM_BIT_MASK_SPA_PCM_DATA_FLAG and #ASM_SHIFT_SPA_PCM_DATA_FLAG
          to set this bit.

        All other bits are reserved; the aDSP sets them to 0. */

   uint32_t                  buf_addr_lsw;
   /**< lower 32 bits of 64 bit Valid, mapped address */
   uint32_t                  buf_addr_msw;
   /**< upper 32 bits of 64 bit Valid, mapped address. address provided by the client in ASM_DATA_CMD_SPA_V2 */
   uint32_t                  mem_map_handle;
   /**< memory map handle in the ASM_DATA_CMD_SPA_V2 command. */

   uint32_t                  actual_size;
   /**< Total size of the valid data in the buffer in bytes. This size
        includes the sizes of all headers, all data, and any padding.

        Supported values: @ge 0 */

   uint32_t                  timestamp_lsw;
   /**< Lower 32 bits of the 64-bit session time in microseconds of the
        first sample in the buffer. This is valid only if bit 31 of the
        flags field is 1. */

   uint32_t                  timestamp_msw;
   /**< Upper 32 bits of the 64-bit session time in microseconds of the
        first sample in the buffer. This is valid only if bit 31 of the
        flags field is 1. */

   uint32_t                  sample_rate;
   /**< Sampling rate in Hertz of the audio stream.
        @newpage */
}
#include "adsp_end_pack.h"
;

/* Structure for the coefficient header in the spectrum analyzer buffer. */
typedef struct asm_data_spa_spectrum_data_header_t asm_data_spa_spectrum_data_header_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the coefficient header in the spectrum analyzer buffer.

    Following is the format of the spectrum analyzer buffer.
   
    Beginning of the buffer:
    - Spectrum header
    - Spectrum coefficient data

    Next 32-bit-aligned position:
    - PCM header (size and number of PCM samples)
    - PCM data
*/
struct asm_data_spa_spectrum_data_header_t
{
   uint32_t                  coeff_size;
   /**< Size in bits of each spectral coefficient. */

   uint32_t                  num_coeffs;
   /**< Number of spectral coefficients.
       @newpage */
}
#include "adsp_end_pack.h"
;

/* Structure for the PCM header in the spectrum analyzer buffer. */
typedef struct asm_data_spa_pcm_data_header_t asm_data_spa_pcm_data_header_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the PCM header in the spectrum analyzer buffer.

    Following is the format of the spectrum analyzer buffer.

    Beginning of the buffer:
    - Coefficient header
    - Coefficient data

    Next 32-bit-aligned position:
    - PCM header 
    - PCM data
*/
struct asm_data_spa_pcm_data_header_t
{
   uint32_t                  sample_size;
   /**< Size in bits of each PCM sample. */

   uint32_t                  num_samples;
   /**< Number of samples.
       @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_data_event_spa_done */

/** @addtogroup asm_data_cmd_remove_init_silence
@{ */
/** Indicates the number of samples per channel to be removed from the
    beginning of the stream.

    @relevant_apr_hdr_fields
	  Opcode -- ASM_DATA_CMD_REMOVE_INITIAL_SILENCE \n
      Dst_port:
              - Session ID 1 -- ASM_MAX_SESSION_ID (0 is reserved)
              - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

	@apr_msg_payload
      @structure{asm_data_cmd_remove_initial_silence_t}
      @tablespace
      @inputtable{ASM_DATA_CMD_REMOVE_INITIAL_SILENCE_APR_msg_payload.tex}

    @prerequisites
      The session/stream must be valid.
        @negparspace
      This command must be sent before the first data buffer. If it is sent
      after the first frame, the error code ADSP_EFAILED is returned and
      samples to be returned are not updated.
        @negparspace
      If this message is sent multiple times before the first data buffer,
      only the last value sent is considered.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
    @newpage
*/
#define ASM_DATA_CMD_REMOVE_INITIAL_SILENCE                             0x00010D67

/* Structure for the number of silence samples to be removed from the
    beginning of the track. */
typedef struct asm_data_cmd_remove_initial_silence_t asm_data_cmd_remove_initial_silence_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_DATA_CMD_REMOVE_INITIAL_SILENCE command,
    which removes a specified number of silence samples from the beginning of
    the track.
*/
struct asm_data_cmd_remove_initial_silence_t
{
   uint32_t                  num_samples_to_remove;
   /**< Number of samples per channel to be removed.

        Supported values: 0 to (2<sup>32</sup>-1)
        @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_data_cmd_remove_init_silence */

/** @addtogroup asm_data_cmd_remove_trail_silence
@{ */
/** Indicates the number of samples per channel to be removed from
    the end of the stream.

    @relevant_apr_hdr_fields
	  Opcode -- ASM_DATA_CMD_REMOVE_TRAILING_SILENCE \n
      Dst_port:
              - Session ID 1 -- ASM_MAX_SESSION_ID (0 is reserved)
              - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

	@apr_msg_payload
      @structure{asm_data_cmd_remove_trailing_silence_t}
      @tablespace
      @inputtable{ASM_DATA_CMD_REMOVE_TRAILING_SILENCE_APR_msg_payload.tex}

    @prerequisites
      The session/stream must be valid.
        @negparspace
      This command must be sent before the last data buffer is sent to the
      aDSP. Also, the last buffer flag must be set for the last data buffer. If
      this command is sent after the last buffer is sent down, the error code
      ADSP_EFAILED is returned, and samples to be removed are not updated. If
      this message is sent multiple times before the last buffer, only the last
      value sent is considered.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q3,[Q3]}).
    @newpage
*/
#define ASM_DATA_CMD_REMOVE_TRAILING_SILENCE                            0x00010D68

/* Structure for the number of silence samples to be removed from the
    end of a track. */
typedef struct asm_data_cmd_remove_trailing_silence_t asm_data_cmd_remove_trailing_silence_t;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_DATA_CMD_REMOVE_TRAILING_SILENCE command, which
    removes a specified number of silence samples from end the of the track.
*/
struct asm_data_cmd_remove_trailing_silence_t
{
   uint32_t                  num_samples_to_remove;
   /**< Number of samples per channel to be removed.

        Supported values: 0 to (2<sup>32</sup>-1)
        @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_data_cmd_remove_trail_silence */

/** @addtogroup asm_data_cmd_iec_60958_frame_rate
@{ */
/** Indicates the IEC 60958 frame rate of the IEC 61937 packetized audio stream.
    This command is used when the aDSP is operating in Pass-through mode for
    compressed audio playback.\n
    \n
    Currently, sampling rate changes midstream are not supported, and client
    is not expected to issue this command in the middle of playback.

    @relevant_apr_hdr_fields
	  Opcode -- ASM_DATA_CMD_IEC_60958_FRAME_RATE \n
      Dst_port:
              - Session ID 1 -- ASM_MAX_SESSION_ID (0 is reserved)
              - Stream ID 1 -- ASM_MAX_STREAMS_PER_SESSION (0 is reserved)

	@apr_msg_payload
      @structure{asm_data_cmd_iec_60958_frame_rate}
      @tablespace
      @inputtable{ASM_DATA_CMD_IEC_60958_FRAME_RATE_APR_msg_payload.tex}

    @prerequisites
      The session/stream must be valid.
        @negparspace
      The session must be opened for compressed autio playback in the
      pass-through mode.
        @negparspace
      This command must be sent before the first data buffer. Any data buffers
      received in Pass-through mode before this message will not be processed.
      If this message is sent multiple times before the first data buffer, the
      latest value will take effect.

    @return
      APRV2_IBASIC_RSP_RESULT (refer to @xhyperref{Q2,[Q2]}).
    @newpage
*/
#define ASM_DATA_CMD_IEC_60958_FRAME_RATE                            0x00010D87

/* Structure for the number of silence samples to be removed from the
    end of a track. */
typedef struct asm_data_cmd_iec_60958_frame_rate asm_data_cmd_iec_60958_frame_rate;

#include "adsp_begin_pack.h"

/** @brief Payload of the #ASM_DATA_CMD_IEC_60958_FRAME_RATE command, which
    is used to indicate the IEC 60958 frame rate of a given packetized audio
    stream.
*/
struct asm_data_cmd_iec_60958_frame_rate
{
   uint32_t                  frame_rate;
   /**< IEC 60958 frame rate of the incoming IEC 61937 packetized stream.

        Supported values: Any valid frame rate
        @newpage */
}
#include "adsp_end_pack.h"
;
/** @} */ /* end_addtogroup asm_data_cmd_iec_60958_frame_rate */

#ifdef __cplusplus
}
#endif /*__cplusplus*/

#endif /* _ADSP_ASM_DATA_COMMANDS_H_ */
