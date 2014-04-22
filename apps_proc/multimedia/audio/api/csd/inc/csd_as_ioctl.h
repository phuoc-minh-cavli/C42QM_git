#ifndef _CSD_AS_IOCTL_H_
#define _CSD_AS_IOCTL_H_
/**
    @file       csd_as_ioctl.h
    @brief      
    Audio stream IOCTL commands
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
     Copyright (C) 2010-2012 QUALCOMM Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                      EDIT HISTORY FOR MODULE
    This section contains comments describing changes made to this module.
    Notice that changes are listed in reverse chronological order.

    $Header: //components/rel/audio.tx/2.0/audio/api/csd/inc/csd_as_ioctl.h#1 $
    $DateTime: 2018/12/25 22:39:07 $
    $Author: pwbldsvc $
    $Change: 17929171 $
    $Revision: #1 $

    when		who			what, where, why
    --------	-----		---------------------------------------------------
	04/10/13	chenche		Add digital volume query and set API 
    01/18/13    suns        Add enhanced ALLOC/FREE PMEM V2 to allow client 
                            allocates continuous or discontinuous PMEM.
    11/09/12    suns        Add csd_aud_pp_spr control for audio PP filter.
    06/04/12    sd          (Tech Pubs) Edited/added Doxygen markup and comments.
    03/15/11    sd          (Tech Pubs) Edited/added Doxygen markup and comments.	
    10/21/10	jzhou		Doxygen comments update
    08/17/10	jzhou		Doxygen comments update
    04/23/10	jzhou		initial release

    =========================================================================*/

#include "mmdefs.h"
#include "csd_as_fmt.h"
#include "csd_aud_pp.h" 

/** @addtogroup csd_as
  @{
*/

/*-------------------------------------------------------------------------*/
/*  IOCTL Command */
/*-------------------------------------------------------------------------*/

/** Minimum IOCTL ID reserved for the audio stream. */
#define CSD_AS_CMD_START				0x00011400

/** Registers the callback function for the audio stream.

  @payload 
  #csd_as_cb

  @latexonly \input{tables/CSD_AS_CMD_SET_EVT_CB.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_SET_EVT_CB			0x00011401

/**	Starts an audio stream session.	This command takes a payload that contains
  the AV synchronization time. 

  @payload 
  #csd_as_ts 

  @latexonly \input{tables/CSD_AS_CMD_START_SESSION.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_START_SESSION		0x00011402
/**	Pauses or stops an audio stream session.
 
  @payload
  None. 

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_STOP_SESSION			0x00011403
/**	Flushes an audio stream. 
 
  @payload 
  None. 

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_FLUSH_STREAM			0x00011404	
/**	Flushes only the Tx path in the Read-Write stream.
 
  @payload 
  None. 

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_FLUSH_STREAM_TX		0x00011405	
/**	Gets the volume step range.

  @payload
  #csd_as_vol_levels

  @latexonly \input{tables/CSD_AS_CMD_GET_VOL_LEVELS.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_GET_VOL_LEVELS		0x00011406
/**	Used in AV synchronization to get the current DSP time in microseconds. 

  @payload 
  #csd_as_ts 

  @latexonly \input{tables/CSD_AS_CMD_GET_DSP_CLK.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_GET_DSP_CLK			0x00011407
/**	Gets the rendered PCM sample time based on the start time of the 
  playback or flush point in microseconds. 

  @payload 
  #csd_as_ts 

  @latexonly \input{tables/CSD_AS_CMD_GET_RENDERED_TIME.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_GET_RENDERED_TIME	0x00011408
/**	Gets the session ID for an audio stream. This command returns the session
  ID in the payload.
   
  @payload 
  A uint32 number. 

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_GET_SESSION_ID		0x00011409
/**	Configures the audio stream format blocks for Read/Write/Read-Write streams. 

  @payload 
  #csd_as_fmt_cfg 

  @latexonly \input{tables/CSD_AS_CMD_SET_STREAM_FMT.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_SET_STREAM_FMT		0x00011410
/** Sends an end-of-service indication for the audio stream.

  @payload 
  None. 

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_SET_STREAM_EOS		0x00011411
/**	Sets the pre/postprocessor filters on the audio stream.
 
  @payload 
  #csd_as_pp_config 

  @latexonly \input{tables/CSD_AS_CMD_CONFIG_PP_PARAMS.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_CONFIG_PP_PARAMS		0x00011412

/**	Gets the spectrum-analyzed data for the audio stream from the driver.
  Only the Asynchronous mode is supported. 

  @payload 
  #csd_as_spa_buffer 

  @latexonly \input{tables/CSD_AS_CMD_GET_SPA_DATA_1.tex} @endlatexonly

  The SPA data buffer returned by the CSD contains the following structure:

  @latexonly \input{tables/CSD_AS_CMD_GET_SPA_DATA_2.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_GET_SPA_DATA			0x00011413

/**	Sets the dual/mono mapping configuration. This is currently used by the
ISDB-T feature only.*/
#define CSD_AS_CMD_SET_DUAL_MONO_REMAP	0x00011414

/**	Adjusts the session time. This command sets the sample number to be added
  or dropped for the ISDB-T feature. 

  @payload 
  #csd_as_adjust_session_clock 

  @latexonly \input{tables/CSD_AS_CMD_ADJUST_SESSION_CLOCK.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_ADJUST_SESSION_CLOCK	0x00011415

/** Sets the SBR flag or parametric stereo (AAC) flag for the
    AAC format.

  @payload 
  #csd_as_set_aac_flag 

  @latexonly \input{tables/CSD_AS_CMD_SET_AAC_SBR_PS.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_SET_AAC_SBR_PS		0x00011416

/**	Maps the client shared memory buffer. 

  @payload 
  #csd_as_map_smem 

  @latexonly \input{tables/CSD_AS_CMD_MAP_SHARED_MEMORY.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_MAP_SHARED_MEMORY	0x00011417

/**	Unmaps the client shared memory buffer. 

  @payload 
  #csd_as_unmap_smem 

  @latexonly \input{tables/CSD_AS_CMD_UNMAP_SHARED_MEMORY.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_UNMAP_SHARED_MEMORY	0x00011418

/**	Starts or stops the DTMF signal. 

  @payload 
  #csd_as_dtmf_ctl 

  @latexonly \input{tables/CSD_AS_CMD_DTMF_CTL.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_DTMF_CTL	            0x00011419

/**  Allocate PMEM for the stream client.  
  @payload 
  #csd_as_pmem_info 

  @latexonly \input{tables/CSD_AS_CMD_ALLOC_PMEM.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_ALLOC_PMEM          0x00011420

/**  Free PMEM for the stream client.
  @payload 
  #csd_as_pmem_info 

  @latexonly \input{tables/CSD_AS_CMD_FREE_PMEM.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_FREE_PMEM           0x00011421


/** Set the stream info properties for this session. This includes maximum buffer size supported and 
    type of memory to be passed to CSD. 

  @payload
  #csd_as_stream_info

  @latexonly \input{tables/CSD_AS_CMD_SET_STREAM_INFO.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_SET_STREAM_INFO     0x00011422

/** Get the last rendered byte offset of the bitstream.

  @payload
  uint64_t

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_GET_RENDERED_BYTE_OFFSET 	0x00011423

/** Get the MIDI sequence that is associated with a MIDI playback session.

  @payload
  uint8_t

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_GET_MIDI_SEQUENCE_ID 0x00011424

/** Configure the audio decoder to output PCM samples based on multiple channel 
  configuration defined by the client. 

  @payload
  #csd_as_stream_config_multi_channel

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_CONFIG_DECODER_MULTI_CHANNEL		0x00011425

/** Dynamic encoder bitrate change during recoding session. 

  @payload
  #csd_as_stream_enc_bit_rate_update

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_ENCODER_BIT_RATE_UPDATE			0x00011426

/** Sets the Dolby Digital Plus (DDP) extended features for EAC3.

  @payload
  #csd_as_ac3_feature_params

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_CONFIG_AC3_FEATURE_PARAMS		0x00011427

/**  Allow the clients to allocate  and map continuous or discontinuous PMEM. 
     No additional CSD_AS_CMD_MAP_SHARED_MEMORY API needs to be called.  
  @payload 
  #csd_as_pmem_info_v2 

  @latexonly \input{tables/CSD_AS_CMD_ALLOC_PMEM_V2.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_ALLOC_PMEM_V2					0x00011428

/**  Allow the clients to unmap and free the allocated memory with ALLOC V2 API.
     No additional CSD_AS_CMD_UNMAP_SHARED_MEMORY API needs to be called.
  @payload 
  #csd_as_pmem_info_v2 

  @latexonly \input{tables/CSD_AS_CMD_FREE_PMEM_V2.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_FREE_PMEM_V2						0x00011429

/**	Get the volume gain in q_factor specified format.

  @payload
  #csd_as_vol_gains

  @latexonly \input{tables/CSD_AS_CMD_GET_VOL_GAINS.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_GET_VOL_GAINS		            0x00011430

/** Allow the clients to remove number of silence samples from the beginning of the stream.

  @payload
  #csd_as_skip_silence_samples

  @latexonly \input{tables/CSD_AS_CMD_SKIP_INITIAL_SILENCE_SAMPLES.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_SKIP_INITIAL_SILENCE_SAMPLES     0x00011431

/** Allow the clients to remove number of silence samples from the end of the stream.

  @payload
  #csd_as_skip_silence_samples

  @latexonly \input{tables/CSD_AS_CMD_SKIP_TRAILING_SILENCE_SAMPLES.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AS_CMD_SKIP_TRAILING_SILENCE_SAMPLES    0x00011432


/** Allow the clients to register for the underflow events.

  @payload
  #csd_as_underflow_event_generation

  @latexonly \input{tables/CSD_AS_CMD_REGISTER_FOR_UNDERFLOW_EVENT} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
  Currently this API is supported only in RX /Play Back Streams
*/
#define CSD_AS_CMD_REGISTER_FOR_UNDERFLOW_EVENT     0x00011433

/** Allow the clients to register for the overflow events.

  @payload
  #csd_as_overflow_event_generation

  @latexonly \input{tables/CSD_AS_CMD_REGISTER_FOR_OVERFLOW_EVENT} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
  Currently this API is supported only in RX /Play Back Streams
*/
#define CSD_AS_CMD_REGISTER_FOR_OVERFLOW_EVENT      0x00011434



/**	Maximum IOCTL ID for the audio stream. */
#define CSD_AS_CMD_MAX								0x00011600
/*-------------------------------------------------------------------------*/
/*  Payload for IOCTL command */
/*-------------------------------------------------------------------------*/

/** Callback function type for the audio stream callback.

  @param[in] evnt_id      Callback event ID. For available IDs, see 
                          #CSD_AS_EVT_START and #CSD_AS_CMD_END. 
  @param[in] data         Payload for the callback event.
  @param[in] data_len     Payload length.
  @param[in] client_data  Client data for the callback function.

  @return
  None.

  @dependencies
  None.
*/
typedef void (*csd_as_cb_fn)(uint32_t evnt_id, void * data, 
                             uint32_t data_len, void * client_data);

/**
  @brief Payload for #CSD_AS_CMD_SET_EVT_CB IOCTL.
*/
struct csd_as_cb
{
   csd_as_cb_fn cb;        /**< Function pointer for the callback function. */
   void         *cb_data;  /**< Client data that is echoed back when the 
                                callback function is called. */
};

/* Type definition for #csd_as_cb_t. */
typedef struct csd_as_cb csd_as_cb_t;

/**
   @name Audio Stream Timestamp Type
   @{ 
*/ 
#define CSD_AS_TS_UNKNOWN           0   /**< Invalid timestamp. */
#define CSD_AS_TS_ABSOLUTE          1   /**< Absolute timestamp. */
#define CSD_AS_TS_RELATIVE          2   /**< Relative timestamp. */
/** @} */ /* end_name Audio Stream Timestamp Type */
/**
  @brief Payloads for IOCTLs #CSD_AS_CMD_START_SESSION, #CSD_AS_CMD_GET_DSP_CLK,
  and #CSD_AS_CMD_GET_RENDERED_TIME.
*/
struct csd_as_ts
{
    uint32_t    ts_type;        /**< Type of timestamp. Supported values: \n 
                                     - 0 -- Invalid timestamp.
                                     - 1 -- Absolute timestamp.
                                     - 2 -- Relative timestamp.*/
    uint32_t	ts_high;        /**< Upper 32 bits of the microsecond timestamp. */
    uint32_t	ts_low;         /**< Lower 32 bits of the microsecond timestamp. */
};
/* Type definition for #csd_as_ts_t. */
typedef struct csd_as_ts csd_as_ts_t;
/**
  @brief Payload for #CSD_AS_CMD_SET_STREAM_FMT. 
*/
struct csd_as_fmt_cfg
{
    /** Format block for the playback stream. */
    struct csd_as_fmt_rx	rx_fmt;	
    /** Format block for the recording stream. */
    struct csd_as_fmt_tx	tx_fmt;	
};
/* Type definition for #csd_as_fmt_cfg_t. */
typedef struct csd_as_fmt_cfg csd_as_fmt_cfg_t;

/**
  @brief Payload for #CSD_AS_CMD_GET_VOL_LEVELS.
*/
struct csd_as_vol_levels {
    uint32_t num_levels;  /**< Range for the volume level steps. */
};
/* Type definition for #csd_as_vol_levels_t. */
typedef struct csd_as_vol_levels csd_as_vol_levels_t;

/**
   @name q_factor type
   @{
*/
#define CSD_AS_VOL_GAINS_Q_FACTOR_13        0
/** @} */ /* end_name q_factor type */

/** @brief digital gain entry. */
struct csd_as_vol_gain_entry
{
	uint16_t q_factor;            /**< q factor used for digital volume gain. */
	uint16_t min_gain_digital;    /**< minimum level of the digital gain. */
    uint16_t cur_gain_digital;    /**< current level of the digital gain. */
    uint16_t max_gain_digital;    /**< maximum level of the digital gain. */
};
/* Type definition for #csd_as_vol_gain_entry_t. */
typedef struct csd_as_vol_gain_entry csd_as_vol_gain_entry_t;

/** @brief digital stereo gain. */
struct csd_as_vol_stereo_gain
{
	struct csd_as_vol_gain_entry left_ch;   /**< left channel digital gain. */
	struct csd_as_vol_gain_entry right_ch;  /**< right channel digital gain. */
};
/* Type definition for #csd_as_vol_stereo_gain_t. */
typedef struct csd_as_vol_stereo_gain csd_as_vol_stereo_gain_t;

/** @brief digital multichannel gain entry. */
struct csd_as_vol_multi_channel_gain_entry
{
    uint16_t ch_type;                     	/**< Channel type. */
    struct csd_as_vol_gain_entry ch_gain; 	/**< Channel digital gain. */
};
/* Type definition for #csd_as_vol_multi_channel_gain_entry_t. */
typedef struct csd_as_vol_multi_channel_gain_entry csd_as_vol_multi_channel_gain_entry_t;

/** @brief digital multichannel gain. */
struct csd_as_vol_multi_channel_gain
{
	uint32_t num_channels;     /**< Number of channels. */

    /** Digital gain array. */
	struct csd_as_vol_multi_channel_gain_entry* entries;
};
/* Type definition for #csd_as_vol_multi_channel_gain_t. */
typedef struct csd_as_vol_multi_channel_gain csd_as_vol_multi_channel_gain_t;

/**
   @name Digital Volume Gain Type
   @{
*/
#define CSD_AS_VOL_GAINS_MASTER             0
#define CSD_AS_VOL_GAINS_STEREO             1
#define CSD_AS_VOL_GAINS_MULTI_CHANNEL		2
/** @} */ /* end_name Digital Volume Gain Type */

/**
  @brief Payload for #CSD_AS_CMD_GET_VOL_GAINS.
*/
struct csd_as_vol_gains {

	uint32_t type; /**< Type of the digital volume gain parameter. */

	union
	{
        /** Get digital master gain. */
        struct csd_as_vol_gain_entry         master_gain;
        /** Get digital stereo gain. */
        struct csd_as_vol_stereo_gain        stereo_gain;
        /** Get digital multichannel gain. */
        struct csd_as_vol_multi_channel_gain multi_ch_gain;
	} u;
};
/* Type definition for #csd_as_vol_gains_t. */
typedef struct csd_as_vol_gains csd_as_vol_gains_t;

/** 
  @brief Payload for #CSD_AS_CMD_CONFIG_PP_PARAMS.
*/
struct csd_as_pp_config
{
    uint32_t pp_type;   /**< Pre/postprocessor filter type. */

    /** Pre/postprocesing configuration control.*/
    union 
    {
        /** Volume control structure. */
        struct csd_aud_pp_vol   vol;
        /** Digital volume control structure. */
        struct csd_aud_pp_vol_digital 	vol_digital;
        /** Equalizer structure. */
        struct csd_aud_pp_eq    eq;
        /** Spectrum analyzer structure. */
        struct csd_aud_pp_spa	spa;
        /** Time scale modification structure. */
        struct csd_aud_pp_tsm   tsm;
        /** QConcertPlus reverb structure. */
        struct csd_aud_pp_qcpr	qcpr;
        /** Soft pause/resume structure. */
        struct csd_aud_pp_spr	spr;
    }u;
};
/* Type definition for #csd_as_pp_config_t. */
typedef struct csd_as_pp_config csd_as_pp_config_t;
/**
  @brief Payload for #CSD_AS_CMD_GET_SPA_DATA. 
*/
struct csd_as_spa_buffer
{
    uint8_t		*buf_addr;			/**< Buffer pointer that points to the 
                                         storage space. */
    uint32_t	buf_size;			/**< Size of the buffer in bytes provided 
                                         by the client. */
    uint32_t	num_bytes_returned; /**< Number of valid bytes returned from 
                                         the driver. */
};
/* Type definition for #csd_as_spa_buffer_t. */
typedef struct csd_as_spa_buffer csd_as_spa_buffer_t;

/**
    @name SPA Information Header for the Audio Stream
    @{
    Following is the spectrum analyzer output buffer format:\n
    - Beginning of the return buffer -- SPA infomation header
      (struct csd_as_buf_spa_info_header).
    - Next 32-bit aligned position:\n 
      - Spectrum header (struct csd_as_buf_spa_data_header).
      - Spectrum coefficient data. The length of the SPA data
        field can be calculated from the spectrum header field.
    - Next 32-bit aligned position:\n
      - PCM header (struct csd_as_buf_spa_pcm_data_header).
      - PCM data. The length of the PCM data field can be
        calculated from the PCM header field.
*/ 
#define CSD_AS_SPA_BUF_MASK_TS_FLAG		0x80000000	
  /**< Timestamp mask for the spectrum analyzer buffer. */
#define CSD_AS_SPA_BUF_MASK_PCM_FLAG	0x40000000	
  /**< Pulse code modulation data presence in the output buffer mask. */
/** @} */ /* end_name SPA Information Header */
/**
  @brief Defines the basic infomation of the analyzed result at the
  beginning of each SPA data buffer.   
*/
 struct csd_as_buf_spa_info_header
 {
    uint32_t	flags;		 /**< Bitmask flags for the header 
                                  (CSD_AS_SPA_BUF_MASK_XX). Supported values:\n
                                  - Bit 31 -- Timestamp valid bitmask.\n
                                    - 1 -- Timestamp in the header is valid. 
                                    - 0 -- Timestamp in the header is not valid.
                                             
                                  - Bit 30 -- PCM data presence bitmask.\n
                                    - 1 -- PCM data is present in the data 
                                           buffer.
                                    - 0 -- PCM data is absent from the data 
                                           buffer.

                                  - Other bits are reserved. */ 
    uint32_t	ts_high;     /**< Upper 32 bits of the 64-bit session time for 
                                  the first sample in the buffer in microseconds. */
    uint32_t	ts_low;      /**< Lower 32 bits of the 64-bit session time for 
                                  the first sample in the buffer in microseconds.  */
    uint32_t	sample_rate; /**< Sampling rate of the analyzed audio stream in Hz. */
 };
/* Type definition for #csd_as_buf_spa_info_header_t. */
 typedef struct csd_as_buf_spa_info_header csd_as_buf_spa_info_header_t;

/** 
  @brief Defines the spectrum header information in the SPA data buffer.

  It exists in the SPA data buffer memory. Following this header in the SPA 
  data buffer is the actual coefficient data based on num_coeffs. 
*/
 struct csd_as_buf_spa_data_header
 {
     uint32_t coeff_bit_size;   /**< Size of each coefficient data in bits. */
     uint32_t num_coeffs;       /**< Number of coefficients. */
 };
/* Type definition for #csd_as_buf_spa_data_header_t. */
 typedef struct csd_as_buf_spa_data_header csd_as_buf_spa_data_header_t;

/** 
  @brief Defines the PCM header information in the SPA data buffer.
   
  It exists in the SPA data buffer memory. Following this header in the SPA 
  data buffer is the actual PCM raw data based on num_samples. 
*/
 struct csd_as_buf_spa_pcm_data_header
 {
    uint32_t sample_bit_size;   /**< Size of each PCM sample in bits. */
    uint32_t num_samples;       /**< Number of samples. */
 };
/* Type definition for #csd_as_buf_spa_pcm_data_header_t. */
 typedef struct csd_as_buf_spa_pcm_data_header csd_as_buf_spa_pcm_data_header_t;

/** 
   @name Dual Mono Mapping Types
   @{ 
*/ 

/** First SCE to the left channel, and the second SCE to the right channel. */
#define CSD_AS_DUALMONO_REMAP_TYPE1		0	

/** First SCE to the right channel, and the second SCE to the left channel.	*/  
#define CSD_AS_DUALMONO_REMAP_TYPE2		1	

/** First SCE to both the left and right channels.	*/
#define CSD_AS_DUALMONO_REMAP_TYPE3		2	

/** Second SCE to both the left and right channels. */
#define CSD_AS_DUALMONO_REMAP_TYPE4		4	

/** @} */ /* end_name Dual Mono Mapping Types */
/**
  @brief Payload for #CSD_AS_CMD_SET_DUAL_MONO_REMAP.
*/
struct csd_as_dual_mono_remap
{
    uint32_t remap_type;   /**< Dual/mono remap configuration. */
};
/* Type definition for #csd_as_dual_mono_remap_t. */
typedef struct csd_as_dual_mono_remap csd_as_dual_mono_remap_t;

/**
  @brief Payload for #CSD_AS_CMD_ADJUST_SESSION_CLOCK. 
*/
struct csd_as_adjust_session_rsp
{
    uint32_t	estimated_processing_time_low;  
      /**< Lower 32 bits of the 64-bit estimated processing time in microseconds.
           Provides the time duration the DSP needs to finish the adjustment. */ 
    uint32_t	estimated_processing_time_high;	
      /**< Upper 32 bits of the 64-bit estimated processing time in microseconds. 
           Provides the time duration the DSP needs to finish the adjustment. */
};
/* Type definition for #csd_as_adjust_session_rsp_t. */
typedef struct csd_as_adjust_session_rsp csd_as_adjust_session_rsp_t;

/**
  @brief Payload for #CSD_AS_CMD_ADJUST_SESSION_CLOCK. 
*/
struct csd_as_adjust_session_clock_req
{
    uint32_t	time_high;
      /**< Upper 32 bits of the 64-bit adjustment to the session clock in 
           microseconds. */
    uint32_t	time_low;       
      /**< Lower 32 bits of the 64-bit adjustment to the session clock in 
           microseconds. */
};
/* Type definition for #csd_as_adjust_session_clock_req_t. */
typedef struct csd_as_adjust_session_clock_req csd_as_adjust_session_clock_req_t;

/**
  @brief Payload for #CSD_AS_CMD_ADJUST_SESSION_CLOCK. 
*/
struct csd_as_adjust_session_clock
{
    /** Requests time adjustment.  */
    struct csd_as_adjust_session_clock_req adjust_time_req;
    /** Response to the time adjustment request.  */
    struct csd_as_adjust_session_rsp adjust_time_rsp;
};
/* Type definition for #csd_as_adjust_session_clock_t. */
typedef struct csd_as_adjust_session_clock csd_as_adjust_session_clock_t;
/** 
   @name AAC SBR PS Flag
   @{ 
*/
/** SBR is off, and parametric stereo (AAC) is off. */ 
#define CSD_AS_AAC_SBR_PS_TYPE1		0  

/** SBR is on, and parametric stereo (AAC) is off. */
#define CSD_AS_AAC_SBR_PS_TYPE2		1  

/** SBR is on, and parametric stereo (AAC) is on. */
#define CSD_AS_AAC_SBR_PS_TYPE3		2  
/** @} */ /* end_name AAC SBR PS Flag */
/**
  @brief Payload for #CSD_AS_CMD_SET_AAC_SBR_PS.
*/
struct csd_as_set_aac_flag
{
    uint32_t sbr_ps_type;   /**< Sets the SBR and parametric stereo (AAC) flags in an AAC stream. 
                                 Supported values: \n
                                 - 1 -- SBR is off, and parametric stereo (AAC) is off.
                                 - 2 -- SBR is on, and  parametric stereo (AAC) is off.
                                 - 3 -- SBR is on, and  parametric stereo (AAC) is on.  */
};
/* Type definition for #csd_as_set_aac_flag_t. */
typedef struct csd_as_set_aac_flag csd_as_set_aac_flag_t;
/** 
   @name Shared Memory Type
   @{ 
*/ 
#define CSD_AS_SMEM_EBI		0	/**< Shared memory for the external bus interface. */
#define CSD_AS_SMEM_SMI		1	/**< Shared memory for the system memory interface. */
/** @} */ /* end_name Shared Memory Type */
/**
  @brief Payload for #CSD_AS_CMD_MAP_SHARED_MEMORY.

  This structure is used to map the shared memory.  
*/
struct csd_as_map_smem
{
    uint32_t mem_type;  /**< Type of shared memory used. Supported values: \n 
                             - 0 -- Shared memory for the external bus interface.
                             - 1 -- Shared memory for the system memory interface. */
    uint8_t* mem_addr;  /**< Physical address for the shared memory to be mapped. 
                             This address must be 4K aligned. */
    uint32_t mem_size;  /**< Size of the shared memory; a minimum of 4K. */
};
/* Type definition for #csd_as_map_smem_t. */
typedef struct csd_as_map_smem csd_as_map_smem_t;
/**
  @brief Payload for #CSD_AS_CMD_UNMAP_SHARED_MEMORY.

  This structure is used to unmap the shared memory.
*/
struct csd_as_unmap_smem
{
    uint8_t* mem_addr;  /**< Physical address for the shared memory to be 
                             unmapped. */
};
/* Type definition for #csd_as_unmap_smem_t. */
typedef struct csd_as_unmap_smem csd_as_unmap_smem_t;
/**
  @brief Payload for #CSD_AS_CMD_DTMF_CTL. 

  This structure is used to start or stop DTMF. 
*/
struct csd_as_dtmf_ctl
{
    uint16_t                 tone_1;
      /**< First tone frequency for DTMF. Supported values: 100 to 4000 Hz. */

    uint16_t                  tone_2;
      /**< Second tone frequency for DTMF. Supported values: 100 to 4000 Hz. */

    uint16_t                  gain_index;
      /**< DTMF gain. Supported values: DTMF gain index.  */

    int16_t                  duration;
      /**< Duration of the DTMF tone in milliseconds. Supported values:\n
           - -1 -- Infinite duration. 
           -  0 -- Disables/stops the infinite tone.
           -  > 0 -- Finite duration in milliseconds. */
};
/* Type definition for #csd_as_dtmf_ctl_t. */
typedef struct csd_as_dtmf_ctl csd_as_dtmf_ctl_t;
/**
  @brief Payload of #CSD_AS_CMD_ALLOC_PMEM/#CSD_AS_CMD_FREE_PMEM IOCTLs.
*/
struct csd_as_pmem_info
{
   uint32_t nSize;      /**< Size of the PMEM buffer to be allocated or the 
                             allocated size. */
   uint32_t nPid;       /**< PID of the application. */
   uint32_t mem_type;   /**< Type of memory and associated properties (same as 
                             in map_addr). */
   uint8_t *va;         /**< Pointer to the virtual address for the allocated 
                             PMEM in the client process. */
};
typedef struct csd_as_pmem_info csd_as_pmem_info_t;

/**.
   @name PMEM Allocation Flag Type
   @{ 
*/ 
#define CSD_PMEM_FLAGS_PHYS_CONTIG       0x00000001
#define CSD_PMEM_FLAGS_PHYS_NON_CONTIG   0x00000010
/** @} */ /* end_name PMEM Allocation Flag Type */
/**
    @brief
   This structure is a payload of CSD_AS_CMD_ALLOC_PMEM_V2 and CSD_AS_CMD_FREE_PMEM_V2
*/
struct csd_as_pmem_info_v2
{
    uint32_t size;        /**< Size of Pmem Buffer to be allocated or the allocated size >*/
    uint32_t pid;         /**< PID of the Application >*/
    uint32_t mem_type;    /**< Type of shared memory used. Supported values are: \n 
                               Supported values: \n
                               - 0 -- Shared memory for the external bus interface.
                               - 1 -- Shared memory for the system memory interface. */
    uint32_t mem_flags;   /**< Flags on the type of allocation.
                                Supported values: \n
                                - CSD_PMEM_FLAGS_PHYS_ for supported flags.*/
    uint8_t *virt_addr;   /**< Virtual Address for the Alloc’ed PMEM in the client Process >*/
};
typedef struct csd_as_pmem_info_v2 csd_as_pmem_info_v2_t;

/**
  @brief Payload of #CSD_AS_CMD_SET_STREAM_INFO.
*/ 
struct csd_as_stream_info
{
    uint32_t max_buf_size;  /**< Maximum buffer size to be passed to the CSD. */
    uint32_t buff_mem_type; /**< Buffer memeory type. Supported values: CSD_AS_BUF_MEM_HEAP
                                 and CSD_AS_BUF_MEM_SHARED. */
}; 
typedef struct csd_as_stream_info csd_as_stream_info_t; 

/**
  @brief Payload of #CSD_AS_CMD_CONFIG_DECODER_MULTI_CHANNEL. 

  The client is responsible for setting up the appropriate channel 
  configuration from the decoder output depending on the device settings for 
  best quality. 
 */ 
struct csd_as_stream_config_multi_channel
{
    uint16_t num_channels;  
      /**< Number of channels to be set up for the decoder output. \n
           Supported values: 0 to 16. \n
           0 - Indicates the native channel mapping; the output of the decoder 
           is to be preserved as is. */
    uint8_t  *channel_mapping;
      /**< Pointer to an array. Element K of the array describes channel K 
           inside the decoder output buffer. Supported values for the array 
           element are defined in #csd_as_fmt_pcm_multi_channel. Do not use 
           #CSD_AS_PCM_MULTI_CHANNEL_NULL in the array. */
}; 
typedef struct csd_as_stream_config_multi_channel csd_as_stream_config_multi_channel_t; 

/**
  @brief Payload of #CSD_AS_CMD_ENCODER_BIT_RATE_UPDATE. 

  The client is responsible for setting up the appropriate channel 
  configuration from the decoder depending on the device settings for best 
  quality. 
 */ 
struct csd_as_stream_enc_bit_rate_update
{
    uint32_t fmt_type;      /**< Format type for playback. Supported values:\n 
                                 - 6 -- Format for AAC.
                                 - 15 -- Format for SBC. */
    uint32_t bit_rate;      /**< New bitrate clients want to apply to the 
                                 future incoming encoded streams. Supported 
                                 values: \n
                                 - For SBC encoder:\n 
                                   Maximum supported bitrate is:\n
                                   -- 320 kilobytes per second for the mono channel.\n
                                   -- 512 kilobytes per second for stereo channels.
                                 - For AAC encoder:\n 
                                   f_s: Input sampling frequency in Hz.\n 
                                   Minimum values:\n 
                                   -- Min(24000, 0.5*f_s)   AC_LC(mono)\n
                                   -- Min(24000, f_s)       AAC_LC(stereo)\n
                                   -- 24000                 AAC+(mono), AAC+(stereo) and eAAC+\n
                                   Maximum values:\n
                                   -- Min(192000, 6*f_s)    AAC_LC(mono)\n
                                   -- Min(192000, 12*f_s)   AAC_LC(stereo)\n
                                   -- Min(192000, 6*f_s)    AAC+(mono)\n
                                   -- Min(192000, 12*f_s)   AAC+(stereo)\n
                                   -- Min(192000, 12*f_s)   eAAC+. */
}; 
typedef struct csd_as_stream_enc_bit_rate_update csd_as_stream_enc_bit_rate_update_t; 

/** 
   @name AC3 Feature Type
   @{ 
*/ 
#define CSD_AS_AC3_FEATURE_NONE                     0
#define CSD_AS_AC3_FEATURE_KARAOKE                  1
#define CSD_AS_AC3_FEATURE_DRC                      2
#define CSD_AS_AC3_FEATURE_DRC_BOOST_SCALEFACTOR    3
#define CSD_AS_AC3_FEATURE_DRC_CUT_SCALEFACTOR      4
#define CSD_AS_AC3_FEATURE_DUAL_MONO                5
#define CSD_AS_AC3_FEATURE_STEREO_DOWNMIX           6
/** @} */ /* end_name AC3 Feature Type */

/**
  @brief AC3 Karaoke Settings.
*/
struct csd_as_ac3_karaoke
{
    uint32_t karaoke_mode;
                    /**< Specifies the vocal channels for Karaoke mode.
                         Supported values: \n
                            - 0 -- no vocal
                            - 1 -- left vocal
                            - 2 -- right vocal
                            - 3 -- both vocals [default]
                    */
};
/* Type definition for #csd_as_ac3_karaoke_t.*/
typedef struct csd_as_ac3_karaoke csd_as_ac3_karaoke_t;

/**
  @brief AC3 DRC Settings.
*/
struct csd_as_ac3_drc
{
    uint32_t drc_mode;
                    /**< Specifies the Dynamic Range Control or compression mode.
                         Supported values: \n
                            - 0 -- custom mode, analog dialnorm
                            - 1 -- custom mode, digital dialnorm
                            - 2 -- line out mode [default]
                            - 3 -- RF remod mode
                    */
};
/* Type definition for #csd_as_ac3_drc_t.*/
typedef struct csd_as_ac3_drc csd_as_ac3_drc_t;

/**
  @brief AC3 DRC Boost Scalefactor.
*/
struct csd_as_ac3_boost_scalefactor
{
    uint32_t boost_gain;
                    /**< Used to increase the level of the quiet sounds for 
                         associated DRC mode. Supported values: \n
                            - 0 to 0x7FFFFFFF (range is 0 to 1 in u31)
                            - default is 0x7FFFFFFF
                    */
};
/* Type definition for #csd_as_ac3_boost_scalefactor_t.*/
typedef struct csd_as_ac3_boost_scalefactor csd_as_ac3_boost_scalefactor_t;

/**
  @brief AC3 DRC Cut Scalefactor.
*/
struct csd_as_ac3_cut_scalefactor
{
    uint32_t cut_gain;
                    /**< Used to reduce the level of the loudest sounds for 
                         associated DRC mode. Supported values: \n
                            - 0 to 0x7FFFFFFF (range is 0 to 1 in u31)
                            - default is 0x7FFFFFFF
                    */
};
/* Type definition for #csd_as_ac3_cut_scalefactor_t.*/
typedef struct csd_as_ac3_cut_scalefactor csd_as_ac3_cut_scalefactor_t;

/**
  @brief AC3 Dual Mono Settings.
*/
struct csd_as_ac3_dual_mono
{
    uint32_t dual_mono_mode;
                    /**< Specifies how the dual mono channels are to be played
                         out. This is applicable only for frames designated as 
                         dual mono. Note that Single Channel Element (SCE) 
                         represents individual channels that are present in 
                         the bitstream. Supported values: \n
                            - 0 -- stereo [default]
                                   Left mono SCE goes to Left output and Right
                                   mono SCE goes to Right output.
                            - 1 -- left mono
                                   Left mono SCE goes to both Left and Right
                                   outputs.
                            - 2 -- right mono
                                   Right mono SCE goes to both Left and Right
                                   outputs.
                            - 3 -- mixed mono
                                   Left and Right SCEs are mixed and go to
                                   both Left and Right outputs.
                    */
};
/* Type definition for #csd_as_ac3_dual_mono_t.*/
typedef struct csd_as_ac3_dual_mono csd_as_ac3_dual_mono_t;

/**
  @brief AC3 Stereo Downmix Settings.
*/
struct csd_as_ac3_stereo_downmix
{
    uint32_t stereo_downmix_mode;
                    /**< Specifies the type of stereo downmix to be used. This
                         is applicable when downmixing to two channels. 
                         Supported values: \n
                            - 0 -- auto detect [default]
                            - 1 -- Surround Compatible (Lt/Rt)
                            - 2 -- Stereo (Lo/Ro)
                    */
};
/* Type definition for #csd_as_ac3_stereo_downmix_t.*/
typedef struct csd_as_ac3_stereo_downmix csd_as_ac3_stereo_downmix_t;

/** 
  @brief Payload for #CSD_AS_CMD_CONFIG_AC3_FEATURE_PARAMS.
*/
struct csd_as_ac3_feature_params
{
    uint32_t feature_type;  /**< Extended feature type. */

    /** Extended feature control.*/
    union 
    {
        /** Karaoke structure. */
        struct csd_as_ac3_karaoke           karaoke;
        /** DRC structure. */
        struct csd_as_ac3_drc	            drc;
        /** DRC boost scalefactor structure. */
        struct csd_as_ac3_boost_scalefactor drc_boost;
        /** DRC cut scalefactor structure. */
        struct csd_as_ac3_cut_scalefactor	drc_cut;
        /** Dual mono structure. */
        struct csd_as_ac3_dual_mono	        dual_mono;
        /** Stereo downmix structure. */
        struct csd_as_ac3_stereo_downmix	stereo_downmix;
    }u;
};
/* Type definition for #csd_as_ac3_feature_params_t. */
typedef struct csd_as_ac3_feature_params csd_as_ac3_feature_params_t;

/**
  @brief Payload for #CSD_AS_CMD_SKIP_INITIAL_SILENCE_SAMPLES and 
                     #CSD_AS_CMD_SKIP_TRAILING_SILENCE_SAMPLES.
*/
struct csd_as_skip_silence_samples {
    uint32_t num_samples;  /**< Number of silence samples to be skipped. */
};
/* Type definition for #csd_as_skip_silence_samples_t. */
typedef struct csd_as_skip_silence_samples csd_as_skip_silence_samples_t;

/*-------------------------------------------------------------------------*/
/*  event ids */
/*-------------------------------------------------------------------------*/

/**	Start of the CSD event ID, which is 0x00011600. */
#define CSD_AS_EVT_START			CSD_AS_CMD_MAX

/** Asynchronous buffer consumption. The driver publishes EVT Done once the 
  driver has finished consuming the buffer.

  @payload 
  Heap -- None.\n
  PMEM -- Payload is uint32_t* to indicate the buffer address (physical 
  address) that has been returned. 
*/
#define CSD_AS_EVT_BUF_DONE         0x00011601

/** Asynchronous buffer production. The driver publishes EVT Done once the 
  driver has finished producing the buffer.

  @payload 
  Heap -- None.\n
  PMEM -- #csd_as_buffer.
*/
#define CSD_AS_EVT_BUF_READY		0x00011602

/** End of stream event to inform the client that all the data has been 
  rendered.

  @payload
  None.

  @return
  None.

  @dependencies
  None.
*/
#define CSD_AS_EVT_EOS				0x00011603

/** Informs the client about the sample rate change or channel configuration 
  change. 

  @payload
  #csd_as_evt_sr_cm_change

  @latexonly \input{tables/CSD_AS_EVT_SR_CM_CHANGE.tex} @endlatexonly
    
  @return
  None.

  @dependencies
  None.
*/
#define CSD_AS_EVT_SR_CM_CHANGE		0x00011604

/** @xreflabel{CSD_AS_EVT_SPA_BUF_READY}
  Asynchronous spectrum-analyzed buffer production. The driver publishes
  EVT Done once the driver has finished producing the buffer with 
  spectrum-analyzed data.

  @payload
  None.

  @return
  None.

  @dependencies
  None.
*/
#define CSD_AS_EVT_SPA_BUF_READY	0x00011605

/** Maximum IOCTL ID that is reserved for the audio stream. */
#define CSD_AS_CMD_END              0x000117FF
/*-------------------------------------------------------------------------*/
/*	payload for event ids */
/*-------------------------------------------------------------------------*/
/** 
  @brief Payload of #CSD_AS_EVT_SR_CM_CHANGE.
*/
struct csd_as_evt_sr_cm_change{
    uint32_t                  sample_rate;        /**< Stream sample rate.   */
    uint16_t                  num_channels;       /**< Channel configuration */
    uint16_t                  reserved;           /**< Reserved.             */
    uint8_t                   channel_mapping[8]; /**< Channel mapping array. */
};
/* Type definition for #csd_as_evt_sr_cm_change_t. */
typedef struct csd_as_evt_sr_cm_change csd_as_evt_sr_cm_change_t;

/** @} */ /* end_addtogroup csd_as */

#endif /* _CSD_AS_IOCTL_H_ */
