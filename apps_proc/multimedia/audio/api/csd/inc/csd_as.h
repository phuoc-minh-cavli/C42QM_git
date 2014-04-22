#ifndef _CSD_AS_H_
#define _CSD_AS_H_
/**
    @file      csd_as.h
    @brief
    Audio stream definitions
*/
/*===========================================================================
NOTE: The @brief description and any detailed descriptions above do not appear 
      in the PDF. 

      The CSD_mainpage.dox file contains all file/group descriptions that are 
      in the output PDF generated using Doxygen and LaTeX. To edit or update 
      any of the file/group text in the PDF, edit the CSD_mainpage.dox file or 
      contact Tech Pubs.
===========================================================================*/
/*-----------------------------------------------------------------------------
     Copyright (C) 2010-2012 Qualcomm Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary.
-----------------------------------------------------------------------------*/
/*=============================================================================
                      EDIT HISTORY FOR MODULE
    This section contains comments describing changes made to this module.
    Notice that changes are listed in reverse chronological order.
    
    $Header: //components/rel/audio.tx/2.0/audio/api/csd/inc/csd_as.h#1 $
    $DateTime: 2018/12/25 22:39:07 $
    $Author: pwbldsvc $
    $Change: 17929171 $
    $Revision: #1 $

    when		who			what, where, why
    --------	-----		---------------------------------------------------
    01/18/13	suns		Add low latency and 24 bit Process Pipeline support.
    06/04/12    sd          (Tech Pubs) Edited/added Doxygen markup and comments.
    03/15/11    sd          (Tech Pubs) Edited/added Doxygen markup and comments.
    10/25/10	jzhou		Doxygen comments update
    08/17/10	jzhou		Doxygen comments update
    04/23/10	jzhou		initial release

    =========================================================================*/
#include "mmdefs.h"
#include "csd_as_ioctl.h"

/** @addtogroup csd_as
  @{
*/

/*-------------------------------------------------------------------------*/
/* open structures for audio stream open */
/*-------------------------------------------------------------------------*/

/**
   @name Audio Stream Opcode
   @{ 
*/

#define CSD_OPEN_OP_R		0x01	/**< Open for read. */ 
#define CSD_OPEN_OP_W       0x02	/**< Open for write. */
#define CSD_OPEN_OP_RW     (CSD_OPEN_OP_R | CSD_OPEN_OP_W) 
                                    /**< Open for read and write. */
#define CSD_OPEN_OP_LOOPBACK 0x04   /**< Open for loopback (Tx to Rx).    */
/** @} */ /* end_name Audio Stream Opcode */
/**
    @name Audio Stream Application Type
    @{ 
*/ 
#define CSD_AS_APP_UNKNOWN          0x00  /**< Application type is unknown. */
#define CSD_AS_APP_PLAYBACK         0x01  /**< Media player.        */
#define CSD_AS_APP_RINGER           0x02  /**< Ringer player.       */
#define CSD_AS_APP_SYSTEM_SOUND     0x03  /**< System sound player. */
#define CSD_AS_APP_RECORD           0x04  /**< Recorder.            */
/** @} */ /* end_name Audio Stream Application Type */
/** 
    @name Audio Stream Memory Type
    @{
*/ 
#define CSD_AS_BUF_MEM_UNKNOWN		0x00	/**< Invalid memory type. */
#define CSD_AS_BUF_MEM_HEAP			0x01    /**< Client uses local heap. */
#define CSD_AS_BUF_MEM_SHARED       0x02    /**< Client uses shared memory. */
/** @} */ /* end_name Audio Stream Memory Type */
/** 
    @name Audio Stream Call Mode Type
    @{ 
*/ 
#define CSD_AS_DATA_MODE_ASYNC       0		/**< Asynchronous data call. */
/** @} */ /* end_name Audio Stream Call Mode Type */
/** 
   @name Audio Stream Open Mask
   @{ 
*/ 
#define CSD_AS_OPEN_MASK_SESSION_ID	  0x00000001 
  /**< Session ID mask. */
#define CSD_AS_OPEN_MASK_SR_CM_NOTIFY 0x00000002 
  /**< Sample rate or channel mode change notification mask. */
#define CSD_AS_OPEN_MASK_META_FRAMES  0x00000004 
  /**< Multiframe support mask. This mask is only valid on Tx streams. */
#define CSD_AS_OPEN_MASK_GAPLESS      0x00000008 
  /**< Gapless mode mask. This mask is only valid for playback. */
#define CSD_AS_OPEN_MASK_LOW_POWER_MODE   0x00000010 
  /**< Low Power mode. The session is to open in Low Power mode. */
#define CSD_AS_OPEN_MASK_LOW_LATENCY_MODE   0x00000020 
  /**< Low Latency Mode. The session is to open in low latency mode. */
/** @} */ /* end_name Audio Stream Open Mask */

/**
  @brief Opens an audio stream.
*/
struct csd_as_open
{
    uint32_t    op_code;		/**< Operation code for the audio stream. 
                                     Supported values:\n
                                     - 0x1 -- Open for read.
                                     - 0x2 -- Open for write.
                                     - 0x3 -- Open for read and write. 
                                     - 0x4 -- Open for Loopback (Tx to Rx) */
    uint32_t    buf_mem_type;	/**< Memory type for the data buffer. Supported
                                     values:
                                     - 0x00 -- Invalid memory type.
                                     - 0x01 -- Client uses local heap.
                                     - 0x02 -- Client uses shared memory.                                       */
    uint32_t    data_mode;		/**< Defines the behavior of data path APIs. 
                                     Currently only supports asynchronous 
                                     calls:\n
                                     - 0 -- Asynchronous.
                                     - > 0 -- Reserved. */
    uint32_t	format_type_rx;	/**< Format type for playback. Supported values:\n 
                                     - 0 -- Format is unknown. 
                                     - 1 -- Format for PCM.
                                     - 2 -- Format for ADPCM.
                                     - 3 -- Format for MP3.
                                     - 4 -- Format for RA.
                                     - 5 -- Format for WMA.
                                     - 6 -- Format for AAC.
                                     - 7 -- Format for MIDI.
                                     - 8 -- Format for YADPCM.
                                     - 9 -- Format for QCELP8K.
                                     - 10 -- Format for AMRNB.
                                     - 11 -- Format for AMRWB.
                                     - 12 -- Format for EVRC.
                                     - 13 -- Format for WMA PRO.
                                     - 14 -- Format for QCELP13K. 
                                     - 15 -- Format for SBC.
                                     - 16 -- Format for EVRCB.
                                     - 17 -- Format for AMRWBPLUS.
                                     - 18 -- Format for AC3.
                                     - 19 -- Format for EVRCWB.
                                     - 20 -- Format for FLAC.
                                     - 21 -- Format for Vorbis.
                                     - 22 -- Format for G.711 A-law.
                                     - 23 -- Format for G.711 Mu-law.
                                     - 24 -- Format for G.729A.                  
                                     - 25 -- Format for DTMF.
                                     - 26 -- Format for GSM FR. 
                                     - 27 -- Format for EAC3. 
                                     - 28 -- Format for Multi-channel linear PCM.
                                     - 30 -- Format for MP2.
                                     */
    uint32_t	format_type_tx;	/**< Format type for recording. Supported values
                                     are the same as for format_type_rx. */ 
    uint32_t	open_mask;		/**< Indicates whether the optional fields
                                     are valid. Supported values:\n
                                     - 0 -- Field does not exist. 
                                     - 1 -- Field exists.\n
                                            - Bit 0 -- Session ID mask.
                                            - Bit 1 -- Sample rate or channel mode 
                                                       change notification mask.
                                            - Bit 2 -- Multiframe support mask. This 
                                                       mask is only valid on Tx 
                                                       streams.
                                            - Bit 3 -- Gapless mode mask. This mask  
                                                       is only valid for playback.
                                            - Bits 4 to 31 -- Reserved = 0. */
    uint32_t	session_id;		/**< Session ID for the stream. (Optional) */
    uint32_t    frames_per_buf;	/**< Number of encoded frames that can be packed 
                                     into each encoder buffer. Default: 1. 
                                     (Optional) */
};
/* Type definition for #csd_as_open_t. */
typedef struct csd_as_open csd_as_open_t;

/** 
    @name Audio Stream Post/Pre Process Pipeline Bit Width 
    @{ 
*/ 
#define CSD_AS_OPEN_PP_BITWIDTH_16       16    /**< 16 bits Post/Pre Processing Pipeline.*/
#define CSD_AS_OPEN_PP_BITWIDTH_24       24    /**< 24 bits Post/Pre Processing Pipeline.*/
/** @} */ /* end_name Audio Stream Post/Pre Process Pipeline Bit Width. */
/**
  @brief Opens an audio stream V2.
*/
struct csd_as_open_v2
{
    uint32_t    op_code;		/**< Operation code for the audio stream. 
                                     Supported values:\n
                                     - 0x1 -- Open for read.
                                     - 0x2 -- Open for write.
                                     - 0x3 -- Open for read and write. 
                                     - 0x4 -- Open for Loopback (Tx to Rx) */
    uint32_t    buf_mem_type;	/**< Memory type for the data buffer. Supported
                                     values:
                                     - 0x00 -- Invalid memory type.
                                     - 0x01 -- Client uses local heap.
                                     - 0x02 -- Client uses shared memory.                                       */
    uint32_t    data_mode;		/**< Defines the behavior of data path APIs. 
                                     Currently only supports asynchronous 
                                     calls:\n
                                     - 0 -- Asynchronous.
                                     - > 0 -- Reserved. */
    uint32_t	format_type_rx;	/**< Format type for playback. Supported values:\n 
                                     - 0 -- Format is unknown. 
                                     - 1 -- Format for PCM.
                                     - 2 -- Format for ADPCM.
                                     - 3 -- Format for MP3.
                                     - 4 -- Format for RA.
                                     - 5 -- Format for WMA.
                                     - 6 -- Format for AAC.
                                     - 7 -- Format for MIDI.
                                     - 8 -- Format for YADPCM.
                                     - 9 -- Format for QCELP8K.
                                     - 10 -- Format for AMRNB.
                                     - 11 -- Format for AMRWB.
                                     - 12 -- Format for EVRC.
                                     - 13 -- Format for WMA PRO.
                                     - 14 -- Format for QCELP13K. 
                                     - 15 -- Format for SBC.
                                     - 16 -- Format for EVRCB.
                                     - 17 -- Format for AMRWBPLUS.
                                     - 18 -- Format for AC3.
                                     - 19 -- Format for EVRCWB.
                                     - 20 -- Format for FLAC.
                                     - 21 -- Format for Vorbis.
                                     - 22 -- Format for G.711 A-law.
                                     - 23 -- Format for G.711 Mu-law.
                                     - 24 -- Format for G.729A.                  
                                     - 25 -- Format for DTMF.
                                     - 26 -- Format for GSM FR. 
                                     - 27 -- Format for EAC3. 
                                     - 28 -- Format for Multi-channel linear PCM. 
                                     - 29 -- Format for AAC V2. 
                                     - 30 -- Format for MP2.
                                     */
    uint32_t    format_type_tx;	/**< Format type for recording. Supported values
                                     are the same as for format_type_rx. */ 
    uint32_t    open_mask;		/**< Indicates whether the optional fields
                                     are valid. Supported values:\n
                                     - 0 -- Field does not exist. 
                                     - 1 -- Field exists.\n
                                            - Bit 0 -- Session ID mask.
                                            - Bit 1 -- Sample rate or channel mode 
                                                       change notification mask.
                                            - Bit 2 -- Multiframe support mask. This 
                                                       mask is only valid on Tx 
                                                       streams.
                                            - Bit 3 -- Gapless mode mask. This mask  
                                                       is only valid for playback.
                                            - Bit 4 -- Low Power Audio mode mode mask.
                                                       This mask is only valid for playback.
                                            - Bit 5 -- Low Latency Mode mask.
                                            - Bits 6 to 31 -- Reserved = 0. */
    uint32_t	session_id;		/**< Session ID for the stream. (Optional) */
    uint32_t    frames_per_buf;	/**< Number of encoded frames that can be packed 
                                     into each encoder buffer. Default: 1. 
                                     (Optional) */
    uint32_t    bit_width;		/**< Post/Pre Processing Pipeline bit width of the bit starem 
                                     after the decoder output or before the encoder input.
                                     Supported values:  16 or 24. */
    uint32_t    ret_status;     /**< Returns the Open status. */
};
/* Type definition for #csd_as_open_v2_t. */
typedef struct csd_as_open_v2 csd_as_open_v2_t;
/*-------------------------------------------------------------------------*/
/*  buffer structure for csd_read(), csd_write() */
/*-------------------------------------------------------------------------*/
/**
    @name Audio Stream Data Buffer
    @{
*/
/** End of frame marker: Bit 4. */ 
#define CSD_AS_BUF_MASK_EOF_FLAG           0x00000010

/** Extended timestamp mask: Bit 29. */	
#define CSD_AS_BUF_MASK_TS_CONTINUED_FLAG  0x20000000
/** @} */ /* end_name Audio Stream Data Buffer */

/** Last buffer mask: Bit 30. */
#define CSD_AS_BUF_MASK_LAST_BUFFER_FLAG   0x40000000
/** @} */ /* end_name last buffer mask. */

/** Timestamp Valid mask: Bit 31.*/
#define CSD_AS_BUF_MASK_TS_FLAG		       0x80000000

/**
  @brief Structure accepted by csd_write() and csd_read().
*/
struct csd_as_buffer{
    uint8_t		*buf_addr;	/**< Pointer to the buffer start. */
    uint32_t	buf_size;	/**< Buffer size in bytes. */
    uint32_t	buf_mask;	/**< Bitmask for the buffer.  */
    uint32_t	ts_high;	/**< Upper 32 bits of the microsecond timestamp.  
                                 Valid if TS_FLAG exists. */
    uint32_t	ts_low;		/**< Lower 32 bits of the microsecond timestamp. 
                                 Valid if TS_FLAG exists. */
    uint32_t	num_of_frames;
                            /**< Number of frames in the buffer in Tx.  
                                 Used in the recording return only. */
    uint32_t	offset;		/**< Offset from the start of the buffer in bytes.
                                 This field provides the metaframe information
                                 in the data buffer. The structure of the 
                                 metaframe information is defined in the 
                                 csd_as_buf_frame_info structure. This field is 
                                 only used in the recording return. */
};
/* Type definition for #csd_as_buffer_t. */
typedef struct csd_as_buffer csd_as_buffer_t;

/**
  @brief Defines the metaframe information item in the data buffer. 

  This structure is only used in multiframe support. It exists in the data 
  buffer memory. There can be multiples of this structure in the data buffer 
  memory based on the frame_num field in the csd_as_buffer structure.
*/
struct csd_as_buf_frame_info
{
    uint32_t	offset;             /**< Offset from the buffer start. */
    uint32_t    frame_size;         /**< Encoded frame size in bytes. */
    uint32_t	pcm_samples;        /**< Encoded PCM samples in this frame. */
    uint32_t	ts_high;            /**< Upper 32 bits of the microsecond 
                                         timestamp. */
    uint32_t	ts_low;             /**< Lower 32 bits of the microsecond 
                                         timestamp. */
    uint32_t    flag;               /**< Buffer flags for Bit 31:
                                         - 1 -- Timestamp is valid.
                                         - 0 -- Timestamp is invalid.
                                    */
};
/* Type definition for #csd_as_buf_frame_info_t. */
typedef struct csd_as_buf_frame_info csd_as_buf_frame_info_t;

/** @} */ /* end_addtogroup csd_as */

#endif /* _CSD_AS_H_ */
