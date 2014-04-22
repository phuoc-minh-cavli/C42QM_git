#ifndef __VSS_IPKTEXG_PUBLIC_IF_H__
#define __VSS_IPKTEXG_PUBLIC_IF_H__

/**
  @file vss_ipktexg_public_if.h
  @brief This file contains the APR API definitions for CVD vocoder packet
  exchange.
*/

/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear
      in the PDF.

      The CVD_mainpage.dox file contains all file/group descriptions
      that are in the output PDF generated using Doxygen and Latex. To edit or
      update any of the file/group text in the PDF, edit the
      CVD_mainpage.dox file or contact Tech Pubs.

      The above description for this file is part of the vss_ipktexg_public_if
      group description in the CVD_mainpage.dox file.
=============================================================================*/
/*===========================================================================
Copyright (C) 2013 QUALCOMM Technologies Incorporated.
All rights reserved.
Qualcomm Confidential and Proprietary
===========================================================================*/

/*===========================================================================

  $Header: //components/rel/audio.tx/2.0/audio/api/cvd/vss_ipktexg_public_if.h#1 $
  $Author: pwbldsvc $

===========================================================================*/


#include "apr_comdef.h"

/** @addtogroup vss_ipktexg_public_if
@{ */

/****************************************************************************
 * VOCODER PACKET EXCHANGE MODE SELECTION APIS                              *
 ****************************************************************************/

/** In-band packet exchange mode. */
#define VSS_IPKTEXG_MODE_IN_BAND ( 0 )

/** Out-of-band packet exchange mode. */
#define VSS_IPKTEXG_MODE_OUT_OF_BAND ( 1 )

/** Mailbox packet exchange mode. */
#define VSS_IPKTEXG_MODE_MAILBOX ( 2 )

/**
  Sets the vocoder packet exchange mode.
 
  @par Payload
  #vss_ipktexg_cmd_set_mode_t
 
  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  None.

  @comments
  Properties of vocoder packet exchange mode selection:
  1. The default mode is #VSS_IPKTEXG_MODE_IN_BAND.
  2. The mode can be changed only when the stream is in disabled state.
  3. If the mode being selected requires any configuration parameters, they
     must be provided via the correspoding configuration command (while the
     stream is in disabled state). If the configuration information is not
     provided, packet exchanges do not occur.
*/
#define VSS_IPKTEXG_CMD_SET_MODE ( 0x0001136A )

/** @brief Type definition for #vss_ipktexg_cmd_set_mode_t.
*/
typedef struct vss_ipktexg_cmd_set_mode_t vss_ipktexg_cmd_set_mode_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IPKTEXG_CMD_SET_MODE command.
*/
struct vss_ipktexg_cmd_set_mode_t
{
   uint32_t mode;
      /**< Vocoder packet exchange mode. Supported values:
           #VSS_IPKTEXG_MODE_IN_BAND \n
           #VSS_IPKTEXG_MODE_OUT_OF_BAND \n
           #VSS_IPKTEXG_MODE_MAILBOX */
}
#include "apr_pack_end.h"
;


/****************************************************************************
 * IN-BAND VOCODER PACKET EXCHANGE APIS                                     *
 ****************************************************************************/

/**
  Event that provides an encoded packet when sent by the stream to its client.

  @par Payload
  #vss_ipktexg_evt_in_band_send_enc_buffer_t

  @return
  None.

  @dependencies
  None.
*/
#define VSS_IPKTEXG_EVT_IN_BAND_SEND_ENC_BUFFER ( 0x00011015 )

/** @brief Type definition for #vss_ipktexg_evt_in_band_send_enc_buffer_t.
*/
typedef struct vss_ipktexg_evt_in_band_send_enc_buffer_t vss_ipktexg_evt_in_band_send_enc_buffer_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IPKTEXG_EVT_IN_BAND_SEND_ENC_BUFFER
    event.
*/
struct vss_ipktexg_evt_in_band_send_enc_buffer_t
{
   uint32_t media_id;
      /**< Media ID (see @latexonly \hyperref[media_ID]{Media Type IDs} 
           @endlatexonly) of the packet. */

#if __STDC_VERSION__ >= 199901L
   uint8_t content[];
      /**< Data content. */
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;


/**
  Event that requests a decoder packet when sent by the stream to its client.
  The client responds with a #VSS_IPKTEXG_EVT_IN_BAND_SEND_DEC_BUFFER event.

  @par Payload
  None.

  @return
  None.

  @dependencies
  None.
*/
#define VSS_IPKTEXG_EVT_IN_BAND_REQUEST_DEC_BUFFER ( 0x00011017 )


/**
  Event sent by the client to the stream in response to a
  #VSS_IPKTEXG_EVT_IN_BAND_REQUEST_DEC_BUFFER event that provides a decoder
  packet.

  @par Payload
  #vss_ipktexg_evt_in_band_send_dec_buffer_t

  @return
  None.

  @dependencies
  None.
*/
#define VSS_IPKTEXG_EVT_IN_BAND_SEND_DEC_BUFFER ( 0x00011016 )

/** @brief Type definition for #vss_ipktexg_evt_in_band_send_dec_buffer_t.
*/
typedef struct vss_ipktexg_evt_in_band_send_dec_buffer_t vss_ipktexg_evt_in_band_send_dec_buffer_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IPKTEXG_EVT_IN_BAND_SEND_DEC_BUFFER
    event.
*/
struct vss_ipktexg_evt_in_band_send_dec_buffer_t
{
   uint32_t media_id;
      /**< Media ID (see @latexonly \hyperref[media_ID]{Media Type IDs}
           @endlatexonly) of the packet. */

#if __STDC_VERSION__ >= 199901L
   uint8_t content[];
      /**< Data content. */
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;


/****************************************************************************
 * OUT-OF-BAND VOCODER PACKET EXCHANGE APIS                                 *
 ****************************************************************************/

/**
  Sets the configuration information for #VSS_IPKTEXG_MODE_OUT_OF_BAND.
 
  @par Payload
  #vss_ipktexg_cmd_oob_set_config_t
 
  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  The vocoder packet exchange mode must be successfully set on the stream to
  #VSS_IPKTEXG_MODE_OUT_OF_BAND via #VSS_IPKTEXG_CMD_SET_MODE and the memory
  for the encoder and decoder buffers must first be mapped with CVD by 
  following @latexonly \hyperref[shmem_guideline]{Shared Memory Usage 
  Guideline} @endlatexonly.
  
  @comments
  Properties of out-of-band vocoder packet exchange:
  1. The encoder and decoder buffers provided must be large enough to
     accommodate the maximum possible vocoder packet size.
  2. The encoder and decoder buffers can be re-claimed by the client only after
     the vocoder packet exchange mode has been successfully set on the stream
     to something other than #VSS_IPKTEXG_MODE_OUT_OF_BAND.
  3. If the current vocoder packet exchange mode set on the stream is not
     #VSS_IPKTEXG_MODE_OUT_OF_BAND, the configuration information is ignored
     and a failure is returned.
*/
#define VSS_IPKTEXG_CMD_OOB_SET_CONFIG ( 0x0001136B )

/** @brief Type definition for #vss_ipktexg_cmd_oob_set_config_t.
*/
typedef struct vss_ipktexg_cmd_oob_set_config_t vss_ipktexg_cmd_oob_set_config_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IPKTEXG_CMD_OOB_SET_CONFIG command.
*/
struct vss_ipktexg_cmd_oob_set_config_t
{
  uint32_t mem_handle;
    /**< Handle to the shared memory that holds encoder and decoder buffers.
         See @latexonly \hyperref[shmem_guideline]{Shared Memory Usage 
         Guideline} @endlatexonly. */
  uint64_t enc_buf_addr;
    /**< Location of memory that holds the encoder buffer. See @latexonly
         \hyperref[shmem_guideline]{Shared Memory Usage Guideline}
         @endlatexonly on how the address is interpreted. */
  uint32_t enc_buf_size;
    /**< Size of the encoder buffer. The amount of memory allocated at
         enc_buf_addr must meet the requirement as specified in @latexonly
         \hyperref[shmem_guideline]{Shared Memory Usage Guideline}
         @endlatexonly. */
  uint64_t dec_buf_addr;
    /**< Location of memory that holds the decoder buffer. See @latexonly
         \hyperref[shmem_guideline]{Shared Memory Usage Guideline}
         @endlatexonly on how the address is interpreted. */
  uint32_t dec_buf_size;
    /**< Size of the decoder buffer. The amount of memory allocated at
         dec_buf_addr must meet the requirement as specified in @latexonly
         \hyperref[shmem_guideline]{Shared Memory Usage Guideline} 
         @endlatexonly. */
}
#include "apr_pack_end.h"
;


/**
  Event sent by the stream to its client notifying that an encoder packet is
  ready to be consumed by the client from shared memory.

  Encoder buffer format in shared memory:

  @verbatim

   <-------- 32 bits -------->

    -------------------------
   | timestamp               |  ---> Set to zero if not used
    -------------------------
   | media_id                |
    -------------------------
   | enc_data_size           |
    -------------------------
   | enc_data                |
   |                         |
   | ...                     |
    -------------------------

  @endverbatim

  @payload
  None.  
 
  @return
  None.

  @dependencies
  The vocoder packet exchange mode must be set to #VSS_IPKTEXG_MODE_OUT_OF_BAND
  and the shared memory encoder buffer and size must be provided via 
  #VSS_IPKTEXG_CMD_OOB_SET_CONFIG.

  @comments
  With the exception of the first encoder packet, if
  a #VSS_IPKTEXG_EVT_OOB_NOTIFY_ENC_BUFFER_CONSUMED event is not received from
  the client in response to the previous
  #VSS_IPKTEXG_EVT_OOB_NOTIFY_ENC_BUFFER_READY event by the time a new encoder
  packet is ready for delivery, the stream will drop the new encoder packet.
*/
#define VSS_IPKTEXG_EVT_OOB_NOTIFY_ENC_BUFFER_READY ( 0x0001136C )


/**
  Event sent by the client to the stream notifying that the encoder packet has
  been consumed from shared memory and the encoder buffer is available to the
  stream for the next encoder packet.
 
  @payload
  None.

  @return
  None.

  @dependencies
  The vocoder packet exchange mode must be set to #VSS_IPKTEXG_MODE_OUT_OF_BAND
  and the shared memory encoder buffer and size must be provided via
  #VSS_IPKTEXG_CMD_OOB_SET_CONFIG.

  @comments
  With the exception of the first encoder packet, if this event is not received
  from the client in response to the previous 
  #VSS_IPKTEXG_EVT_OOB_NOTIFY_ENC_BUFFER_READY event by the time a new encoder
  packet is ready for delivery, the stream will drop the new encoder packet.
*/
#define VSS_IPKTEXG_EVT_OOB_NOTIFY_ENC_BUFFER_CONSUMED ( 0x0001136D )


/**
  Event sent by the stream to its client notifying that the stream is
  requesting the next decoder packet in shared memory.
  
  @payload
  None.

  @return
  None.

  @dependencies
  The vocoder packet exchange mode must be set to #VSS_IPKTEXG_MODE_OUT_OF_BAND
  and the shared memory decoder buffer and size must be provided via
  #VSS_IPKTEXG_CMD_OOB_SET_CONFIG.

  @comments
  If a #VSS_IPKTEXG_EVT_OOB_NOTIFY_DEC_BUFFER_READY event is not received from
  the client in response to this event by the time the decoder must begin 
  decoding the packet, then an erasure is produced by the decoder.
*/
#define VSS_IPKTEXG_EVT_OOB_NOTIFY_DEC_BUFFER_REQUEST ( 0x0001136E )


/**
  Event sent by the client to the stream, notifying that the decoder packet is
  available and ready to be consumed from shared memory.
 
  Expected decoder buffer format in shared memory:

  @verbatim

   <-------- 32 bits -------->

    -------------------------
   | timestamp               |  ---> Set to zero if not used
    -------------------------
   | media_id                |
    -------------------------
   | dec_data_size           |
    -------------------------
   | dec_data                |
   |                         |
   | ...                     |
    -------------------------

  @endverbatim

  @payload
  None.
 
  @return
  None.

  @dependencies
  The vocoder packet exchange mode must be set to #VSS_IPKTEXG_MODE_OUT_OF_BAND
  and the shared memory decoder buffer and size must be provided via 
  #VSS_IPKTEXG_CMD_OOB_SET_CONFIG.

  @comments
  If this event is not received from the client in response to the previous
  #VSS_IPKTEXG_EVT_OOB_NOTIFY_DEC_BUFFER_REQUEST event by the time the decoder
  must begin decoding the packet, then an erasure is produced by the decoder.
*/
#define VSS_IPKTEXG_EVT_OOB_NOTIFY_DEC_BUFFER_READY ( 0x0001136F )


/****************************************************************************
 * MAILBOX VOCODER PACKET EXCHANGE APIS                                     *
 ****************************************************************************/

/**
  Sets the configuration information for #VSS_IPKTEXG_MODE_MAILBOX.
 
  @par Payload
  #vss_ipktexg_cmd_mailbox_set_config_t
 
  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies

  - The vocoder packet exchange mode must be successfully set on the stream to
    #VSS_IPKTEXG_MODE_MAILBOX via the #VSS_IPKTEXG_CMD_SET_MODE command.

  - The memory to be used for mailbox vocoder packet exchange must first be 
    mapped with CVD by following @latexonly \hyperref[shmem_guideline]{Shared
    Memory Usage Guideline} @endlatexonly.

  - This command can be issued to the stream only when it is in disabled state.

  - A valid media ID (see @latexonly \hyperref[media_ID]{Media Type IDs}
    @endlatexonly) must have been set on each direction of the stream via the
    #VSS_ISTREAM_CMD_SET_MEDIA_TYPE command.

  - The client must handle the #VSS_IPKTEXG_EVT_MAILBOX_REQUEST_TO_START,
    #VSS_ISTREAM_EVT_READY and #VSS_ISTREAM_EVT_NOT_READY events as described
    in the procedure for handling the start and stop of the mailbox vocoder 
    packet exchange in the comments section of this API.

  @comments

  - The client shall provide a separate circular buffer 
    (#vss_ipktexg_mailbox_voc_req_circ_buffer_t) for each direction
    of the stream. The circular buffer will be used to hold the client's
    vocoding requests (see @latexonly \hyperref[voc_req]{Vocoding (encoding or
    decoding) Request}@endlatexonly).

  - Any circular buffer (#vss_ipktexg_mailbox_voc_req_circ_buffer_t) provided
    can be re-claimed by the client only after the vocoder packet exchange mode
    has been successfully set on the stream to something other than 
    #VSS_IPKTEXG_MODE_MAILBOX, or after the stream session has been 
    successfully destroyed by the client.

  - The following is the procedure for handling the start of the mailbox
    vocoder packet exchange:

    1. The stream will send the #VSS_IPKTEXG_EVT_MAILBOX_REQUEST_TO_START 
       event to request its client to start the mailbox vocoder packet
       exchange.
    2. The client may call certain CVD APIs such as the API to get CVD's 
       current time.
    3. The client shall issue the #VSS_IPKTEXG_CMD_MAILBOX_START command to
       provide the its vocoder packet exchange time reference to the stream.
    4. The stream will clear all the circular buffers 
       (#vss_ipktexg_mailbox_voc_req_circ_buffer_t) provided by its client,
       i.e. sets each circular buffer's read offset = write offset = 0.
    5. The stream will send the #VSS_ISTREAM_EVT_READY event to notify its
       client that the stream is active and ready to receive the client's
       vocoding requests (see @latexonly \hyperref[voc_req]{Vocoding (encoding
       or decoding) Request }@endlatexonly). The client shall make vocoding
       requests to the stream only after the #VSS_ISTREAM_EVT_READY event is
       received.

  - To make a vocoding request (see @latexonly \hyperref[voc_req]{Vocoding 
    (encoding or decoding) Request}@endlatexonly) on a particular stream 
    direction, the client shall place the request inside the corresponding
    circular buffer (#vss_ipktexg_mailbox_voc_req_circ_buffer_t) and then 
    advance the circular buffer's write offset by the size of the request.

  - When a vocoding request (see @latexonly \hyperref[voc_req]{Vocoding 
    (encoding or decoding) Request}@endlatexonly) is completed by the stream,
    the stream will advance the circular buffer's
    (#vss_ipktexg_mailbox_voc_req_circ_buffer_t) read offset by the 
    size of the request. When the stream is active, the vocoder packet 
    associated with a vocoding request can only be re-claimed by the client 
    after the request is completed.

  - The following is the procedure for handling the stop of the mailbox vocoder
    packet exchange:

    1. The stream will send the #VSS_ISTREAM_EVT_NOT_READY event to notify its
       client that the stream has become inactive. Any pending vocoding 
       requests (see @latexonly \hyperref [voc_req]{Vocoding (encoding or 
       decoding) Request}@endlatexonly) in either of the Tx and/or Rx circular
       buffer (#vss_ipktexg_mailbox_voc_req_circ_buffer_t) will not be
       processed by the stream after the stream becomes inactive.
    2. After receiving the #VSS_ISTREAM_EVT_NOT_READY event, the client shall
       stop making vocoding reuqests (see @latexonly \hyperref[voc_req]
       {Vocoding (encoding or decoding) Request} @endlatexonly).
    3. After generating the #VSS_ISTREAM_EVT_NOT_READY event, the stream will 
       not modify the circular buffer
       (#vss_ipktexg_mailbox_voc_req_circ_buffer_t) content (until the next
       #VSS_IPKTEXG_CMD_MAILBOX_START command). The client may examine the 
       circular buffer content, for example, to identify which decoding 
       requests (#vss_ipktexg_mailbox_dec_request_t) are not completed by the
       stream and to re-claim the decoder packets associated with these 
       requests.
*/
#define VSS_IPKTEXG_CMD_MAILBOX_SET_CONFIG ( 0x00013054 )

/** @brief Type definition for #vss_ipktexg_mailbox_enc_packet_t.
*/
typedef struct vss_ipktexg_mailbox_enc_packet_t vss_ipktexg_mailbox_enc_packet_t;

#include "apr_pack_begin.h"

/** @brief Structure for specifying an encoder packet.
*/
struct vss_ipktexg_mailbox_enc_packet_t
{
  uint16_t minor_version;
    /**< The minor version of the encoder packet. Supported values: \n
         0. */
  uint64_t capture_timestamp_us;
    /**< Capture timestamp at microphone in microseconds. The clock source to
         be used for interpreting the timestamp is product requirement 
         specific. */
  uint32_t status_mask;
    /**< Bitmask indicating the encoder packet status. Supported values: \n
         Bit 0 - If set, indicates a general encoding error, informing the 
                 client that the packet has been dropped. The data_size will be
                 set to 0. \n
         Bits 1 to 31 - Reserved, set to 0. */
  uint32_t media_id;
    /**< Media ID (see @latexonly \hyperref[media_ID]{Media Type IDs}
         @endlatexonly) of the encoder packet. */
  uint32_t data_size;
    /**< Actual size of the encoder media data in bytes. */
#if __STDC_VERSION__ >= 199901L
  uint8_t data[];
    /**< Encoder media data of size data_size. The media data format is 
         specified by DCN 80-N8828-1. */
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;

/** @brief Type definition for #vss_ipktexg_mailbox_dec_packet_t.
*/
typedef struct vss_ipktexg_mailbox_dec_packet_t vss_ipktexg_mailbox_dec_packet_t;

#include "apr_pack_begin.h"

/** @brief Structure for specifying a decoder packet.
*/
struct vss_ipktexg_mailbox_dec_packet_t
{
  uint16_t minor_version;
    /**< The minor version of the decoder packet. Supported values: \n 
         0. */
  uint64_t render_timestamp_us;
    /**< Reserved. Set to 0. */
  uint32_t status_mask;
    /**< Bitmask indicating the decoder packet status. Supported values: \n
         Bit 0 - If set, indicates a general error, informing the stream that
                 the packet has been dropped. The data_size shall be set to
                 0. \n
         Bits 1 to 31 - Reserved, set to 0. */
  uint32_t media_id;
    /**< Media ID (see @latexonly \hyperref[media_ID]{Media Type IDs}
         @endlatexonly) of the decoder packet. */
  uint32_t data_size;
    /**< Actual size of the decoder media data in bytes. */
#if __STDC_VERSION__ >= 199901L
  uint8_t data[];
    /**< Decoder media data of size data_size. The media data format is 
         specified by DCN 80-N8828-1. */
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;

/** @name Vocoding (encoding or decoding) Request
  @latexonly \label{voc_req} @endlatexonly
  @{ */

typedef struct vss_ipktexg_mailbox_enc_request_t vss_ipktexg_mailbox_enc_request_t;

#include "apr_pack_begin.h"

/** @brief Structure for specifying an encoding request.

    - An encoding request allows the client to specify the timestamp of when an
      encoder packet should be delivered into shared memory.

    - The client shall ensure that the encoding request is available in shared
      memory at least 5 milliseconds prior to the requested timestamp. The
      client is recommended to provide the next encoding request immediately
      after picking up an encoder packet.

    - If the difference in timestamp values between two adjacent encoding
      requests is not exactly an encoder packet duration in microseconds, it 
      indicates a skew between the client timeline and the stream timeline.
*/
struct vss_ipktexg_mailbox_enc_request_t
{
  uint64_t timestamp_us;
    /**< Timestamp in microseconds specifying when an encoder packet should be
         delivered into shared memory. The clock source to be used for 
         interpreting the timestamp is product requirement specific. */
  uint64_t mem_address;
    /**< Location of the encoder packet. See @latexonly \hyperref 
         [shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly on how 
         the address is interpreted. The format of the encoder packet is 
         specified by #vss_ipktexg_mailbox_enc_packet_t. */
  uint32_t mem_size;
    /**< Size of the encoder packet in bytes. The amount of memory allocated
         at mem_address must meet the requirement as specified in @latexonly 
         \hyperref[shmem_guideline]{Shared Memory Usage Guideline}
         @endlatexonly. The amount of memory allocated shall be sufficient to 
         hold the largest possible encoder packet for the media ID (see 
         @latexonly \hyperref[media_ID]{Media Type IDs}@endlatexonly) currently
         configured on the stream. The size of the largest possible encoder 
         packet can be queried via the
         #VSS_IPKTEXG_CMD_MAILBOX_GET_VOC_PACKET_PROPERTIES command. */
}
#include "apr_pack_end.h"
;

/** @brief Type definition for #vss_ipktexg_mailbox_dec_request_t.
*/
typedef struct vss_ipktexg_mailbox_dec_request_t vss_ipktexg_mailbox_dec_request_t;

#include "apr_pack_begin.h"

/** @brief Structure for specifying a decoding request.

    - A decoding request allows the client to specify the timestamp of when the
      decoding should start for a decoder packet.

    - The client shall ensure that the decoding request is available in shared
      memory by the requested timestamp. The client shall not modify the
      content of the decoder packet associated with a request after the request
      is made and before it is completed. Although the client may provide the 
      decoding request at any time before the requested timestamp, the client 
      is recommended to provide it as close to the requested timestamp as 
      possible for reducing voice call round trip delay.

    - If the difference in timestamp values between two adjacent decoding
      requests is not exactly a decoder packet duration in microseconds, it
      indicates a skew between the client timeline and the stream timeline.
*/
struct vss_ipktexg_mailbox_dec_request_t
{
  uint64_t timestamp_us;
    /**< Timestamp in microseconds specifying when the decoding should start
         for a decoder packet. The clock source to be used for interpreting 
         the timestamp is product requirement specific. */
  uint64_t mem_address;
    /**< Location of the decoder packet. See @latexonly \hyperref 
         [shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly on how 
         the address is interpreted. The format of the decoder packet is 
         specified by #vss_ipktexg_mailbox_dec_packet_t. */
  uint32_t mem_size;
    /**< Size of the decoder packet in bytes. The amount of memory allocated
         at mem_address must meet the requirement as specified in @latexonly
         \hyperref[shmem_guideline]{Shared Memory Usage Guideline}
         @endlatexonly. */
}
#include "apr_pack_end.h"
;

/** @} */ /* end_name_group Vocoding (encoding or decoding) Request */

/** @brief Type definition for #vss_ipktexg_mailbox_voc_req_circ_buffer_t.
*/
typedef struct vss_ipktexg_mailbox_voc_req_circ_buffer_t vss_ipktexg_mailbox_voc_req_circ_buffer_t;

#include "apr_pack_begin.h"

/** @brief Structure for specifying the format of a circular buffer that holds
    vocoding requests (see @latexonly \hyperref[voc_req]{Vocoding (encoding or
    decoding) Request}@endlatexonly).

    - The data[] holds vocoding requests (see @latexonly \hyperref[voc_req]
      {Vocoding (encoding or decoding) Request}@endlatexonly).

    - The total size of data[] in bytes is ( ( circular buffer mem_size ) -
      sizeof( vss_ipktexg_mailbox_voc_req_circ_buffer_t ) ).

    - Each time after a vocoding request (see @latexonly \hyperref[voc_req]
      {Vocoding (encoding or decoding) Request}@endlatexonly) is written into
      data[], the write offset shall be advanced by the size of the request.
      Each time after a vocoding request is read from data[], the read offset
      shall be advanced by the size of the request. Any request written into
      data[] shall not be modified before it is read.

    - The read offset and the write offset shall wraparound to 0 when they
      reach the end of data[].

    - The data[] is empty when the read offset and the write offset are equal.
      The data[] is full when there is space available to hold only one 
      vocoding request (see @latexonly \hyperref[voc_req]{Vocoding (encoding or
      decoding) Request}@endlatexonly). The client shall not write any new
      vocoding request when the data[] is full.
*/
struct vss_ipktexg_mailbox_voc_req_circ_buffer_t
{
  uint64_t read_offset;
    /**< The offset in bytes from the beginning of data[] where the next
         vocoding request (see @latexonly \hyperref[voc_req]{Vocoding (encoding
         or decoding) Request}@endlatexonly) will be read. */
  uint8_t padding_1[ 120 ];
    /**< Padding for cache line alignment. The content in the padding is 
         ignored. */
  uint64_t write_offset;
    /**< The offset in bytes from the beginning of data[] where the next
         vocoding request (see @latexonly \hyperref[voc_req]{Vocoding (encoding
         or decoding) Request}@endlatexonly) will be written. */
  uint8_t padding_2[ 120 ];
    /**< Padding for cache line alignment. The content in the padding is 
         ignored. */
#if __STDC_VERSION__ >= 199901L
  uint8_t data[];
    /**< Circular buffer data. */
#endif /* __STDC_VERSION__ >= 199901L */
}
#include "apr_pack_end.h"
;

/** @brief Type definition for #vss_ipktexg_cmd_mailbox_set_config_t.
*/
typedef struct vss_ipktexg_cmd_mailbox_set_config_t vss_ipktexg_cmd_mailbox_set_config_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IPKTEXG_CMD_MAILBOX_SET_CONFIG
    command.
*/
struct vss_ipktexg_cmd_mailbox_set_config_t
{
  uint32_t mem_handle;
    /**< Handle to the shared memory to be used for mailbox vocoder packet 
         exchange. See @latexonly \hyperref[shmem_guideline]{Shared Memory 
         Usage Guideline} @endlatexonly. */
  uint64_t tx_circ_buf_mem_address;
    /**< Location of the Tx circular buffer 
         (#vss_ipktexg_mailbox_voc_req_circ_buffer_t) for holding encoding 
         requests (#vss_ipktexg_mailbox_enc_request_t). See @latexonly \hyperref
         [shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly on how
         the address is interpreted. This field is ignored if the stream 
         direction is Rx only and shall be set to 0. */
  uint32_t tx_circ_buf_mem_size;
    /**< Size of the Tx circular buffer 
         (#vss_ipktexg_mailbox_voc_req_circ_buffer_t) in bytes. The amount of
         memory allocated at tx_circ_buf_mem_address must meet the requirement
         as specified in @latexonly \hyperref[shmem_guideline]{Shared Memory 
         Usage Guideline}@endlatexonly. The amount of memory available to hold
         encoding requests (#vss_ipktexg_mailbox_enc_request_t) shall be an
         integer multiple of the size of a single encoding request data 
         structure. This field is ignored if the stream direction is Rx only 
         and shall be set to 0. */
  uint64_t rx_circ_buf_mem_address;
    /**< Location of the Rx circular buffer 
         (#vss_ipktexg_mailbox_voc_req_circ_buffer_t) for holding decoding 
         requests (#vss_ipktexg_mailbox_dec_request_t). See @latexonly \hyperref
         [shmem_guideline]{Shared Memory Usage Guideline} @endlatexonly on how
         the address is interpreted. This field is ignored if the stream 
         direction is Tx only and shall be set to 0. */
  uint32_t rx_circ_buf_mem_size;
    /**< Size of the Rx circular buffer 
         (#vss_ipktexg_mailbox_voc_req_circ_buffer_t) in bytes. The amount of
         memory allocated at rx_circ_buf_mem_address must meet the requirement
         as specified in @latexonly \hyperref[shmem_guideline]{Shared Memory 
         Usage Guideline}@endlatexonly. The amount of memory available to hold
         decoding requests (#vss_ipktexg_mailbox_dec_request_t) shall be an
         integer multiple of the size of a single decoding request data 
         structure. This field is ignored if the stream direction is Tx only 
         and shall be set to 0. */
}
#include "apr_pack_end.h"
;


/**
  Event sent by the stream to its client to request the client to start the 
  mailbox vocoder packet exchange.

  @par Payload
  None.

  @return
  None.

  @dependencies
  In order for the client to receive this event, the vocoder packet exchange
  mode must be set to #VSS_IPKTEXG_MODE_MAILBOX and the mailbox vocoder packet
  exchange must be configured via the #VSS_IPKTEXG_CMD_MAILBOX_SET_CONFIG
  command.

  @comments
  This event is sent to the source address and the source port of the 
  client who issued the #VSS_IPKTEXG_CMD_MAILBOX_SET_CONFIG command. See the
  comments for the #VSS_IPKTEXG_CMD_MAILBOX_SET_CONFIG command for more 
  information.
*/
#define VSS_IPKTEXG_EVT_MAILBOX_REQUEST_TO_START ( 0x00013055 )


/**
  Starts the mailbox vocoder packet exchange operation.
  
  This command is issued by the client to the stream and it allows the client 
  to provide its vocoder packet exchange time reference to the stream.

  This command is mandatory for the mailbox vocoder packet exchange start
  handling procedure.

  @par Payload
  #vss_ipktexg_cmd_mailbox_start_t

  @return
  #APRV2_IBASIC_RSP_RESULT

  @dependencies
  The vocoder packet exchange mode must be set to #VSS_IPKTEXG_MODE_MAILBOX
  and the mailbox vocoder packet exchange must be configured via the 
  #VSS_IPKTEXG_CMD_MAILBOX_SET_CONFIG command.

  This command shall be issued exactly once after the client has received the
  #VSS_IPKTEXG_EVT_MAILBOX_REQUEST_TO_START event.

  @comments
  The time reference provided in the payload of this command will be used to 
  align the stream's timeline to the client's timeline at the beginning of the 
  mailbox vocoder packet exchange operation. See the comments for the 
  #VSS_IPKTEXG_CMD_MAILBOX_SET_CONFIG command for more information.
*/
#define VSS_IPKTEXG_CMD_MAILBOX_START ( 0x00013056 )

/** @brief Type definition for #vss_ipktexg_cmd_mailbox_start_t.
*/
typedef struct vss_ipktexg_cmd_mailbox_start_t vss_ipktexg_cmd_mailbox_start_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the #VSS_IPKTEXG_CMD_MAILBOX_START command.
*/
struct vss_ipktexg_cmd_mailbox_start_t
{
  uint64_t tx_ref_timstamp_us;
    /**< The client's encoder packet exchange time reference in microseconds.
         The time reference will be used to align the encoder's timeline to the
         client's timeline at the beginning of the mailbox vocoder packet 
         exchange operation. The first encoder packet will be ready for
         delivery at an integer multiple of a single encoder packet duration 
         away from the tx_ref_timstamp_us. The clock source to be used for 
         interpreting the timestamp is product requirement specific. This field
         will be ignored if the stream direction is Rx only and shall be set to
         0. */
  uint64_t rx_ref_timstamp_us;
    /**< The client's decoder packet exchange time reference in microseconds.
         The time reference will be used to align the decoder's timeline to the
         client's timeline at the beginning of the mailbox vocoder packet 
         exchange operation. The decoding of the first decoder packet will
         start at an integer multiple of a single decoder packet duration away
         from the rx_ref_timstamp_us. The clock source to be used for
         interpreting the timestamp is product requirement specific. This field
         will be ignored if the stream direction is Tx only and shall be set to
         0. */
}
#include "apr_pack_end.h"
;


/**
  Gets the properties of the vocoder packets used in mailbox vocoder packet
  exchange for the media ID (see @latexonly \hyperref[media_ID]{Media Type IDs}
  @endlatexonly) currently configured on each direction of the stream.

  @par Payload
  None.

  @return
  #VSS_IPKTEXG_RSP_MAILBOX_GET_VOC_PACKET_PROPERTIES -- In case of success.
  #APRV2_IBASIC_RSP_RESULT -- In case of failure.

  @dependencies
  A valid media ID (see @latexonly \hyperref[media_ID]{Media Type IDs}
  @endlatexonly) must have been configured on each direction of the stream via
  the #VSS_ISTREAM_CMD_SET_MEDIA_TYPE command.
*/
#define VSS_IPKTEXG_CMD_MAILBOX_GET_VOC_PACKET_PROPERTIES ( 0x00013057 )


/**
  Response to the #VSS_IPKTEXG_CMD_MAILBOX_GET_VOC_PACKET_PROPERTIES command.

  @par Payload
  #vss_ipktexg_rsp_mailbox_get_voc_packet_properties_t

  @return
  None.

  @dependencies
  None.
*/
#define VSS_IPKTEXG_RSP_MAILBOX_GET_VOC_PACKET_PROPERTIES ( 0x00013058 )

/** @brief Type definition for 
    #vss_ipktexg_rsp_mailbox_get_voc_packet_properties_t.
*/
typedef struct vss_ipktexg_rsp_mailbox_get_voc_packet_properties_t vss_ipktexg_rsp_mailbox_get_voc_packet_properties_t;

#include "apr_pack_begin.h"

/** @brief Payload structure for the 
    #VSS_IPKTEXG_RSP_MAILBOX_GET_VOC_PACKET_PROPERTIES command response.
*/
struct vss_ipktexg_rsp_mailbox_get_voc_packet_properties_t
{
  uint32_t tx_duration_us;
    /**< Duration in microseconds of the media data contained in a single
         encoder packet. This field is set to 0 if the stream direction is Rx
         only, and shall be ignored. */
  uint32_t tx_max_size;
    /**< Maximum encoder packet size in bytes. The size includes the encoder
         packet header (#vss_ipktexg_mailbox_enc_packet_t) as well as the 
         encoder media data. This field is set to 0 if the stream direction is
         Rx only, and shall be ignored. */
  uint32_t rx_duration_us;
    /**< Duration in microseconds of the media data contained in a single
         decoder packet. This field is set to 0 if the stream direction is Tx
         only, and shall be ignored. */
  uint32_t rx_max_size;
    /**< Maximum decoder packet size in bytes. The size includes the decoder
         packet header (#vss_ipktexg_mailbox_dec_packet_t) as well as the 
         decoder media data. This field is set to 0 if the stream direction is
         Rx only, and shall be ignored. */
}
#include "apr_pack_end.h"
;


/** @} */ /* end_addtogroup vss_ipktexg_public_if */

#endif /* __VSS_IPKTEXG_PUBLIC_IF_H__ */

