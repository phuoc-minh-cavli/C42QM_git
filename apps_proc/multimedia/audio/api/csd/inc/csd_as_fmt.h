#ifndef _CSD_AS_FMT_H_
#define _CSD_AS_FMT_H_
/**
    @file      csd_as_fmt.h
    @brief
    Audio stream format block definitions
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

    $Header: //components/rel/audio.tx/2.0/audio/api/csd/inc/csd_as_fmt.h#1 $
    $DateTime: 2018/12/25 22:39:07 $
    $Author: pwbldsvc $
    $Change: 17929171 $
    $Revision: #1 $

    when        who       what, where, why
    --------    -----     ---------------------------------------------------
	04/10/13	Chenche	  Add MP2 decoder support
    04/04/13    jy        Add ADPCM encoder support
    01/18/13    suns      Add multiple channel PCM encode support.
    01/18/13    suns      Add AAC V2 format type for AAC playback.
    06/04/12    sd        (Tech Pubs) Edited/added Doxygen markup and comments.
    03/15/11    sd        (Tech Pubs) Edited/added Doxygen markup and comments.
    01/20/11    ls        GSM-FR, Flac and Vorbis updates.
    10/25/10    jzhou     Doxygen comments update
    08/17/10    jzhou     Doxygen comments update
    04/23/10    jzhou     initial release

    =========================================================================*/
#include "mmdefs.h"

/** @addtogroup csd_as
  @{
*/

/*-------------------------------------------------------------------------*/
/* Supportted Formats */
/*-------------------------------------------------------------------------*/
/**
   @name Audio Stream Format
   @{ 
*/ 

/** Format type for an unknown format. */
#define CSD_FORMAT_UNKNOWN			0   

/** Format block for the PCM recording/playback.

  @par Payload for Recording
  #csd_as_fmt_tx_pcm

  @par Payload for Playback
  #csd_as_fmt_rx_pcm
 
  @latexonly \input{tables/CSD_FORMAT_PCM.tex} @endlatexonly

  @return
  None. 

  @dependencies
  None.
*/
#define CSD_FORMAT_PCM				1   

/** Format block for the adaptive differential pulse code modulation playback. 

  @par Payload for Recording
  #csd_as_fmt_tx_adpcm
  
  @par Payload for Playback
  #csd_as_fmt_rx_adpcm
 
  @latexonly \input{tables/CSD_FORMAT_ADPCM.tex} @endlatexonly

  @return
  None. 

  @dependencies
  None.
*/
#define CSD_FORMAT_ADPCM			2   
/** No decoder format block for MP3 playback. This is used for the decoder only. */
#define CSD_FORMAT_MP3				3	
/** Format is not supported. */
#define CSD_FORMAT_RA				4   
/** Format block for Windows Media Audio 9 playback. 

  @payload
  #csd_as_fmt_rx_wma_v9

  @latexonly \input{tables/CSD_FORMAT_WMA.tex} @endlatexonly

  @return
  None. 

  @dependencies
  None.
*/
#define CSD_FORMAT_WMA				5	
/** Format block for the advanced audio codec recording/playback.

  @par Payload for Recording
  #csd_as_fmt_rx_aac

  @latexonly \input{tables/CSD_FORMAT_AAC_rx.tex} @endlatexonly

  @par Payload for Playback
  #csd_as_fmt_tx_aac
 
  @latexonly \input{tables/CSD_FORMAT_AAC_tx.tex} @endlatexonly

  @return
  None. 

  @dependencies
  None.
*/
#define CSD_FORMAT_AAC				6   

/** Format block for musical instrument digital interface playback. 

  @payload
  #csd_as_fmt_rx_midi

  @latexonly \input{tables/CSD_FORMAT_MIDI.tex} @endlatexonly

  @return
  None. 

  @dependencies
  None.
*/ 
#define CSD_FORMAT_MIDI				7	

/** Format is not supported. */
#define CSD_FORMAT_YADPCM			8   

/** Format block for Qualcomm code excited linear prediction 8K recording.

  @note1 There is no format block for QCELP 8K playback. 

  @payload
  #csd_as_fmt_tx_qcelp

  @latexonly \input{tables/CSD_FORMAT_QCELP8K_13K.tex} @endlatexonly

  @return
  None. 

  @dependencies
  None.
*/
#define CSD_FORMAT_QCELP8K			9	

/** Format block for adaptive multirate codec narrowband recording.

  @note1 There is no format block for AMR-NB playback.

  @payload
  #csd_as_fmt_tx_amr

  @latexonly \input{tables/CSD_FORMAT_AMRNB_WB.tex} @endlatexonly

  @return
  None. 

  @dependencies
  None.
*/
#define CSD_FORMAT_AMRNB			10	 
/** 
  Format block for adaptive multirate codec wideband recording.

  @note1 There is no format block for the AMR-WB playback.

  @payload
  #csd_as_fmt_tx_amr

  @latexonly \input{tables/CSD_FORMAT_AMRNB_WB.tex} @endlatexonly

  @return
  None. 

  @dependencies
  None.
*/
#define CSD_FORMAT_AMRWB			11	 
/** Format block for enhanced variable rate coder recording.

  @note1 There is no format block for EVRC playback.
   
  @payload
  #csd_as_fmt_tx_evrc

  @latexonly \input{tables/CSD_FORMAT_EVRC_B_WB.tex} @endlatexonly

  @return
  None. 

  @dependencies
  None.
*/
#define CSD_FORMAT_EVRC				12	 
/** Format block for Windows Media Audio 10 Pro playback.

  @payload
  #csd_as_fmt_rx_wma_v10

  @latexonly \input{tables/CSD_FORMAT_WMA_PRO.tex} @endlatexonly

  @return
  None. 

  @dependencies
  None.
*/
#define CSD_FORMAT_WMA_PRO			13	
/** Format block for Qualcomm code excited linear prediction 13K recording.\n
  @note1 There is no format block for QCELP 13K playback. 

  @payload
  #csd_as_fmt_tx_qcelp

  @latexonly \input{tables/CSD_FORMAT_QCELP8K_13K.tex} @endlatexonly

  @return
  None. 

  @dependencies
  None.
*/
#define CSD_FORMAT_QCELP13K			14	 
/** Format block for subband coded recording.

  @payload
  #csd_as_fmt_tx_sbc

  @latexonly \input{tables/CSD_FORMAT_SBC.tex} @endlatexonly

  @return
  None. 

  @dependencies
  None.
*/
#define CSD_FORMAT_SBC				15  
/** Format block for enhanced variable rate coder B recording.

  @note1 There is no format block for EVRC-B playback.
   
  @payload
  #csd_as_fmt_tx_evrc

  @latexonly \input{tables/CSD_FORMAT_EVRC_B_WB.tex} @endlatexonly

  @return
  None. 

  @dependencies
  None.
*/
#define CSD_FORMAT_EVRCB			16   
/* The payload information for this define is the same as for 
   #define CSD_FORMAT_AMRNB. To customize the information for this define, 
   create a new table (CSD_FORMAT_AMRWBPLUS.tex) and then update the information */
/** Format block for the AMR wideband plus.

  @payload
  #csd_as_fmt_tx_amr 
   
  @latexonly \input{tables/CSD_FORMAT_AMRNB_WB.tex}
@endlatexonly

  @return
  None. 

  @dependencies
  None.
*/
#define CSD_FORMAT_AMRWBPLUS		17  

/** Dolby decoder format. */
#define CSD_FORMAT_AC3				18  

/** Format block for enhanced variable rate coder wideband recording.

  @note1 There is no format block for EVRC-WB playback.
   
  @payload
  #csd_as_fmt_tx_evrc

  @latexonly \input{tables/CSD_FORMAT_EVRC_B_WB.tex} @endlatexonly

  @return
  None. 

  @dependencies
  None.
*/
#define CSD_FORMAT_EVRCWB			19  

/** No decoder format block for free lossless audio codec playback. */
#define CSD_FORMAT_FLAC				20  

/** Format block for Vorbis playback. 

  @payload
  #csd_as_fmt_rx_vorbis

  @latexonly \input{tables/CSD_FORMAT_VORBIS.tex} @endlatexonly

  @return
  None. 

  @dependencies
  None.
*/
#define CSD_FORMAT_VORBIS			21  

/** G.711 A-law format for recording/playback. 

  @par Payload for Recording
  #csd_as_fmt_tx_g711

  @par Payload for Playback
  #csd_as_fmt_rx_g711

  @latexonly \input{tables/CSD_FORMAT_G711_ALAW_MULAW.tex} @endlatexonly

  @return
  None. 

  @dependencies
  None.
*/
#define CSD_FORMAT_G711_ALAW		22  

/** G.711 mu-law format for recording/playback. 

  @par Payload for Recording
  #csd_as_fmt_tx_g711

  @par Payload for Playback
  #csd_as_fmt_rx_g711

  @latexonly \input{tables/CSD_FORMAT_G711_ALAW_MULAW.tex} @endlatexonly

  @return
  None. 

  @dependencies
  None.
*/
#define CSD_FORMAT_G711_MULAW		23  

/** Not supported. */
#define CSD_FORMAT_G729A			24  

/** No format block for dual-tone multifrequency generation. */
#define CSD_FORMAT_DTMF			    25  

/** No format block for GSM-FR playback. */
#define CSD_FORMAT_GSM_FR           26  

/** No format block for Enhanced AC3 playback. */
#define CSD_FORMAT_EAC3				27  

/** Format block for multiple channel PCM playback.

  @par Payload for Record
  #csd_as_fmt_tx_multi_channel_pcm

  @par Payload for Playback
  #csd_as_fmt_rx_multi_channel_pcm
 
  @latexonly \input{tables/CSD_FORMAT_MULTI_CHANNEL_PCM.tex} @endlatexonly

  @return
  None. 

  @dependencies
  None.
*/
#define CSD_FORMAT_MULTI_CHANNEL_PCM	28  
/** Format block for the advanced audio codec recording/playback.

  @par Payload for Recording
  #csd_as_fmt_tx_aac

  @latexonly \input{tables/CSD_FORMAT_AAC_tx.tex} @endlatexonly

  @par Payload for Playback
  #csd_as_fmt_rx_aac_v2
 
  @latexonly \input{tables/CSD_FORMAT_AAC_rx.tex} @endlatexonly

  @return
  None. 

  @dependencies
  None.
*/
#define CSD_FORMAT_AAC_V2				29   

/** No decoder format block for MP2 playback. */
#define CSD_FORMAT_MP2				    30

/** @} */ /* end_name Audio Stream Format */

/**
   @name Audio Stream Channel Configuration
   @{ 
*/ 
#define CSD_AS_FMT_CHANNEL_MODE_MONO			1
  /**< Mono channel mode. */
#define CSD_AS_FMT_CHANNEL_MODE_STEREO          2
  /**< Stereo channel mode. */
#define CSD_AS_FMT_CHANNEL_MODE_DUAL_MONO       8
  /**< Dual/mono channel mode. */
#define CSD_AS_FMT_CHANNEL_MODE_JOINT_STEREO	9
  /**< Joint stereo channel mode. */
/** @} */ /* end_name Audio Stream Channel Configuration */

/**
   @name Audio Stream CDMA Frame Rate
   @{ 
*/ 
#define CSD_AS_FMT_FR_EIGHTH		1	/**< CDMA encoded frame rate in eighth rate. */
#define CSD_AS_FMT_FR_QUARTER		2   /**< CDMA encoded frame rate in quarter rate. */
#define CSD_AS_FMT_FR_HALF			3   /**< CDMA encoded frame rate in half rate. */
#define CSD_AS_FMT_FR_FULL			4   /**< CDMA encoded frame rate in full rate. */
/** @} */ /* end_name Audio Stream CDMA Frame Rate */

/*-------------------------------------------------------------------------*/
/* Rx format block structures */
/*-------------------------------------------------------------------------*/

/**	@brief Configures pulse code modulation on the MSM&reg;&nbsp;device. */
struct csd_as_fmt_rx_pcm
{
    uint32_t sample_rate;       /**< Sample rate for the PCM stream. Supported 
                                     values are 2000 to 48000, 96000,
                                     192000 Hz. */
    uint16_t channels;          /**< Channel allocation:\n
                                     - 1 -- Mono.
                                     - 2 -- Stereo. */
    uint16_t bit_per_sample;    /**< Bits per sample setup. */
    uint16_t sign_flag;         /**< Sign flag for PCM:\n
                                     - 0 -- Unsigned.
                                     - 1 -- Signed. */
    uint16_t interleave_flag;   /**< Interleaved flag for PCM:\n
                                     - 0 -- Noninterleaved.
                                     - 1 -- Interleaved. */
};
/* Type definition for #csd_as_fmt_rx_pcm_t. */
typedef struct csd_as_fmt_rx_pcm csd_as_fmt_rx_pcm_t;

/** @brief Configures the adaptive differential pulse code modulation format on 
    the MSM device. */
struct csd_as_fmt_rx_adpcm
{
    uint32_t		channels;       /**< Channel configuration:\n 
                                         - 1 -- Mono.
                                         - 2 -- Stereo. */
    uint32_t		bit_per_sample; /**< Bits per sample setup. */
    uint32_t        sample_rate;    /**< Sign flag for the sample.\n 
                                         - 0 -- Unsigned.
                                         - 1 -- Signed. */
    uint32_t		nBlockSize;     /**< Block size for the ADPCM; not used by 
                                         the YADPCM. */  
};
/* Type definition for #csd_as_fmt_rx_adpcm_t. */
typedef struct csd_as_fmt_rx_adpcm csd_as_fmt_rx_adpcm_t;

/**
   @name Audio Stream MIDI Mode
   @{ 
*/ 
#define CSD_AS_MIDI_MODE_0			0	
  /**< All file formats not included in MIDI mode 1 and mode 2: MIDI, SMAF, and PMD. */
#define CSD_AS_MIDI_MODE_1			1	
  /**< MA2 or MA3 synthetic music mobile application format type. */
#define CSD_AS_MIDI_MODE_2			2	
  /**< MA5 synthetic music mobile application format type. */
/** @} */ /* end_name Audio Stream MIDI Mode */

/** @brief Configures the musical instrument digital interface on the MSM device. */
struct csd_as_fmt_rx_midi
{
    uint32_t	mode;  /**< Musical instrument digital interface mode. Supported
                            values: \n
                            - 0 -- All other MIDI/SMAF/PMD file formats.
                            - 1 -- MA2 or MA3 SMAF type.
                            - 2 -- MA5 SMAF type. */
};
/* Type definition for #csd_as_fmt_rx_midi_t. */
typedef struct csd_as_fmt_rx_midi csd_as_fmt_rx_midi_t;

/** @brief Configures the Windows Media&reg;&nbsp;Audio on the MSM device. */
struct csd_as_fmt_rx_wma_v9
{
    uint16_t	tag;			/**< WMA 9 tag field. Specifies the unique ID 
                                     of the codec used to encode the audio data. 
                                     Supported values:\n
                                     - 0x161 -- Standard.
                                     - 0x162 -- Professional.
                                     - 0x163 -- Lossless. */
    uint16_t	channels;		/**< Channel allocation:\n
                                     - 1 -- Mono.
                                     - 2 -- Stereo. */
    uint32_t	sample_rate;    /**< Sample rate for the WMA stream. Supported 
                                     values: 8000, 16000, and 48000. */
    uint32_t	byte_per_second;/**< Average compressed stream rate in bytes 
                                     per second. */
    uint16_t	block_align;    /**< Alignment for the stream. */
    uint16_t	valid_bit_per_sample; /**< Valid bit width per sample. */
    uint32_t	channel_mask;   /**< Channel mask. */
    uint16_t	encode_opt;     /**< Encoding option. */
    uint16_t	reserved1;      /**< For internal use only; do not use. */
    uint32_t	reserved2;          /**< For internal use only; do not use. */
    uint32_t	drc_peak_ref;       /**< Peak reference for dynamic range compression. */
    uint32_t	drc_peak_target;    /**< Peak target for dynamic range compression. */
    uint32_t	drc_average_ref;    /**< Average reference for dynamic range compression. */
    uint32_t	drc_average_target; /**< Average target for dynamic range compression. */
    uint16_t	version;            /**< Windows Media Audio version 9. */
    uint16_t	virtual_pkt_len;    /**< Virtual packet length. */
};
/* Type definition for #csd_as_fmt_rx_wma_v9_t. */
typedef struct csd_as_fmt_rx_wma_v9 csd_as_fmt_rx_wma_v9_t;

/** @brief Configures the Windows Media Audio 10 Pro on the MSM device. */
struct csd_as_fmt_rx_wma_v10
{
    uint16_t	tag;                   /**< WMA 10 Pro tag field. Specifies the
                                            unique ID of the codec used to 
                                            encode the audio data. Supported 
                                            values: \n
                                            - 0x161 -- Standard.
                                            - 0x162 -- Professional.
                                            - 0x163 -- Lossless.*/
    uint16_t	channels;              /**< Channel allocation. Supported
                                            values: \n
                                            - 1 -- Mono.
                                            - 2 -- Stereo. */
    uint32_t	sample_rate;           /**< Sample rate for the WMA stream.
                                            Supported values: 8000, 16000, 
                                            and 48000. */
    uint32_t	byte_per_second;       /**< Average compressed stream rate in 
                                            bytes per second. */
    uint16_t	block_align;           /**< Block alignment for the stream. */
    uint16_t	valid_bit_per_sample;  /**< Valid bit width per sample. */
    uint32_t	channel_mask;          /**< Channel mask. */
    uint16_t	encode_opt;            /**< Encoding option. */
    uint16_t	adv_encode_opt;        /**< Advanced encode option. */
    uint32_t	adv_encode_opt2;       /**< Advanced encode option 2. */
    uint32_t	drc_peak_ref;          /**< Peak reference for dynamic range 
                                            compression. */
    uint32_t	drc_peak_target;       /**< Peak target for dynamic range 
                                            compression. */
    uint32_t	drc_average_ref;       /**< Average reference for dynamic range 
                                            compression. */
    uint32_t	drc_average_target;    /**< Average target for dynamic range 
                                            compression. */
    uint16_t	version;               /**< Windows Media Audio version 10. */
    uint16_t	virtual_pkt_len;       /**< Virtual packet length. */
};
/* Type definition for #csd_as_fmt_rx_wma_v10_t. */
typedef struct csd_as_fmt_rx_wma_v10 csd_as_fmt_rx_wma_v10_t;

/**
   @name Audio Stream AAC Format
   @{ 
*/ 
#define CSD_AS_FMT_AAC_ADTS         0   
  /**< Audio data transport stream advanced audio codec format. */
#define CSD_AS_FMT_AAC_LOAS         1   
  /**< Low overhead audio stream advanced audio codec format. */
#define CSD_AS_FMT_AAC_ADIF         2   
  /**< Audio data interchange advanced audio codec format. */
#define CSD_AS_FMT_AAC_RAW          3   
  /**< Raw advanced audio codec format. */
/** @} */ /* end_name Audio Stream AAC Format */

/** 
   @name Audio Stream AAC Object Type
   @{ 
*/ 
#define CSD_AS_FMT_AAC_OT_LC		2		/**< Lossy compression object type. */
#define CSD_AS_FMT_AAC_OT_SBR		5		/**< Spectral band replication 
                                                 object type. */
#define CSD_AS_FMT_AAC_OT_PS		29		/**< Parametric stereo (AAC) object 
                                                 type. */
#define CSD_AS_FMT_AAC_OT_BSAC		22		/**< Bit-sliced arithmetic coding 
                                                 object type. */
/** @} */ /* end_name  Audio Stream AAC Object Type */

/** @brief Configures the advanced audio codec for Rx on the MSM device. */
struct csd_as_fmt_rx_aac
{
    uint32_t	sample_rate;    
      /**< Sample rate for the AAC stream. Supported values: 8000, 16000, 
           and 48000.*/
    uint16_t	channels;		
      /**< Channel configuration. Supported values:\n
           - 1 -- Mono.
           - 2 -- Stereo. */
    uint16_t	format;			
      /**< AAC format type. Supported values:\n
           - 0 -- Audio data transport stream (ADTS).
           - 1 -- Low overhead audio stream (LOAS).
           - 2 -- Audio data interchange format (ADIF).
           - 3 -- Raw. */
    uint16_t	aud_obj_type;   
      /**< Audio object type. Supported values: \n
           - 2 -- Lossy compression object type. 
           - 5 -- Spectral band replication object type. 
           - 22 -- Bit-sliced arithmetic coding object type. 
           - 29 -- Parametric stereo (AAC) object type. */
    uint16_t	ep_cfg;     
      /**< Indicates the configuration of the error protection scheme 
           (0, 1, 2, 3). This information is retrieved from the MP4 header 
           and is required by the DSP only when the value of AOT is 17. 
           Currently, only epConfig=0 is supported. */
    uint16_t	section_DRF; 
      /**< Indicates whether the Virtual CodeBook (VCB11) error resilience tool 
           is used. Supported values:\n
           - 1 -- VCB11 is used.
           - 0 -- VCB11 is not used.
                                 
           This information is retrieved from the MP4 header.\n 
           @note1hang This field must be zero if (AOT!=17). */
    uint16_t	scale_factor_DRF;
      /**< Indicates whether the Reversible Variable Length Coding (RVLC) 
           error resilience tool is used. Supported values:\n
           - 1 -- RVLC is used.
           - 0 -- RVLC is not used.

           This information is retrieved from the MP4 header.\n
           @note1hang This field must be zero if (AOT!=17). */
    uint16_t	spectral_DRF;  
      /**< Indicates whether the Huffman Codeword Reordering (HCR) error 
           resilience tool is used. Supported values:\n
           - 1 -- HCR is used.
           - 0 -- HCR is not used.

           This information is retrieved from the MP4 header.\n
           @note1hang This field must be zero if (AOT!=17). */
    uint16_t	sbr_on_flag;    
      /**< Enables/disables spectral band replication. Supported values:\n
           - 1 -- Turns on SBR if present in the bitstream.
           - 0 -- Turns off SBR.  */
    uint16_t	sbr_ps_flag;    
      /**< Enables/disables the parametric stereo flag. Supported values:\n
           - 1 -- Turns on parametric stereo (AAC) if present in the bitstream.
           - 0 -- Turns off parametric stereo (AAC). */
    uint32_t	bit_rate;       
      /**< Bit rate.  */
};
/* Type definition for #csd_as_fmt_rx_aac_t. */
typedef struct csd_as_fmt_rx_aac csd_as_fmt_rx_aac_t;

/** @brief Configures the advanced audio codec for Rx on the MSM device. */
struct csd_as_fmt_rx_aac_v2
{
    uint32_t    sample_rate;    
      /**< Sample rate for the AAC stream. Supported values: 8000, 11025, 16000, 
           22050, 24000, 32000, 441000, 48000.*/
    uint16_t    channels;
      /**< Channel configuration. Supported values:\n
           - 0 -- No channel info, PCE (Programmable Configuration Element) 
                  data is available.
           - 1 -- Mono.
           - 2 -- Stereo.
           - 6 -- 5.1 multiple channels. */
    uint16_t    format;
      /**< AAC format type. Supported values:\n
           - 0 -- Audio data transport stream (ADTS).
           - 1 -- Low overhead audio stream (LOAS).
           - 2 -- Audio data interchange format (ADIF).
           - 3 -- Raw. */
    uint16_t    aud_obj_type;   
      /**< Audio object type. Supported values: \n
           - 2 -- Lossy compression object type. 
           - 5 -- Spectral band replication object type. 
           - 22 -- Bit-sliced arithmetic coding object type. 
           - 29 -- Parametric stereo (AAC) object type. */
    uint16_t    total_size_of_PCE_bits;
      /**< In case of RAW AAC formats and if channel = 0 (PCE),
           the client needs to send the PCE bitstream to CSD 
           driver in order for DSP properly decoding bit stream. 
           Support values: \n
             - =0 -- PCE information is assumed to be available in the audio bitstream.
             - >0 -- PCE information is assumed to be stored at PCE_data_addr.*/
    uint8_t*    PCE_data_addr;
      /**< Pointer to the buffer in which PCE data is stored. 
           Supported values: >=0 */
};
/* Type definition for #csd_as_fmt_rx_aac_v2_t. */
typedef struct csd_as_fmt_rx_aac_v2 csd_as_fmt_rx_aac_v2_t;

/** @brief Configures G.711 (PCM, A-law, and mu-law) on the MSM device. */
struct csd_as_fmt_rx_g711
{
    uint32_t	sample_rate; /**< Sample rate for the G.711 stream. 
                                  Supported value: 8000. */
};
/* Type definition for #csd_as_fmt_rx_g711_t. */
typedef struct csd_as_fmt_rx_g711 csd_as_fmt_rx_g711_t;

/** @brief Configures the free lossless audio codec on the MSM device. */
struct csd_as_fmt_rx_flac
{	
    uint16_t	strm_info_present; 
    /**< Specifies whether the METADAT_BLOCK_STREAMINFO is parsed 
         successfully. bStrmInfoPresent is set to 1 if 
         METADAT_BLOCK_STREAMINFO is successfully parsed. When set to 
         0, the FLAC decoder tries to get the stream information from the 
         frame header. */  
     
    uint16_t	min_blk_size; 
    /**< Minimum block size (in samples) used in the stream. */  
     
    uint16_t	max_blk_size; 
    /**< Maximum block size (in samples) used in the stream. If 
         minBlkSize == maxBlkSize, it implies a fixed block size stream.  */  
     
    uint16_t	channels; 
    /**< Specifies the number of channels. FLAC supports 1 to 8 channels. */  
     
    uint16_t	sample_size; 
    /**< Specifies bits per sample. FLAC supports 4 to 32 bits per sample. 
         Currently, the firmware only supports up to 24 bits per sample. */  
     
    uint32_t	sample_rate;
    /**< Sample rate for FLAC. */ 
      
    uint32_t min_frame_size;		
    /**< Minimum frame size in bytes used in the stream. When set to 0, the  
         value is unknown. */  
     
    uint32_t max_frame_size;	   
    /**< Maximum frame size in bytes used in the stream. When set to 0, the  
         value is unknown. */     
     
    uint16_t	md5_sum[8]; 
    /**< MD5 signature of the unencoded audio data. This allows the decoder 
         to determine whether an error exists in the audio data even when the 
         error does not result in an invalid bitstream.  */
};
/* Type definition for #csd_as_fmt_rx_flac_t. */
typedef struct csd_as_fmt_rx_flac csd_as_fmt_rx_flac_t;

/** @brief Configures Vorbis on the MSM device. */
struct csd_as_fmt_rx_vorbis
{    

   uint32_t  sample_rate;
   /**< Sample rate. Supported values: 48000, 44100, 32000, 24000, 22050, 16000, 12000, 11025, 
        8000. */

   uint16_t  channels;
    /**< Specifies the number of channels encoded in the Vorbis stream. 
         Supported values:\n
         - 1 -- Mono 
         - 2 -- Stereo. */

   uint32_t  bit_rate;
    /**< Nominal bitrate of the encoded data. */

   uint32_t  min_bit_rate;
    /**< Minimim bitrate of the encoded data. */

   uint32_t  max_bit_rate;
    /**< Maximum bitrate of the encoded data. */

   uint16_t  bits_per_sample;
    /**< PCM width resolution to be played by the decoder. Supported values: 
         16 bit/24 bit; currrently, the firmware only supports a 16-bit PCM 
         resoltuion. */

    uint32_t          bit_stream_fmt;
      /**< Bit stream format. Supported values:\n
           - 0 -- Raw bitstream. (Default)
           - 1 -- Transcoded bitstream. */
};
/* Type definition for #csd_as_fmt_rx_vorbis_t. */
typedef struct csd_as_fmt_rx_vorbis csd_as_fmt_rx_vorbis_t;

/** Line spectral frequency. 
*/
typedef enum csd_as_fmt_amrwbplus_isf_index {
        CSD_AS_FMT_AMR_WB_PLUS_ISF_0 = 0,       /**< AMR-WB+ Internal Sampling Frequency -- N/A. */
        CSD_AS_FMT_AMR_WB_PLUS_ISF_1,           /**< AMR-WB+ Internal Sampling Frequency -- 12800 Hz. */
        CSD_AS_FMT_AMR_WB_PLUS_ISF_2,           /**< AMR-WB+ Internal Sampling Frequency -- 14400 Hz. */
        CSD_AS_FMT_AMR_WB_PLUS_ISF_3,           /**< AMR-WB+ Internal Sampling Frequency -- 16000 Hz. */
        CSD_AS_FMT_AMR_WB_PLUS_ISF_4,           /**< AMR-WB+ Internal Sampling Frequency -- 17067 Hz. */
        CSD_AS_FMT_AMR_WB_PLUS_ISF_5,           /**< AMR-WB+ Internal Sampling Frequency -- 19200 Hz. */
        CSD_AS_FMT_AMR_WB_PLUS_ISF_6,           /**< AMR-WB+ Internal Sampling Frequency -- 21333 Hz. */
        CSD_AS_FMT_AMR_WB_PLUS_ISF_7,           /**< AMR-WB+ Internal Sampling Frequency -- 24000 Hz. */
        CSD_AS_FMT_AMR_WB_PLUS_ISF_8,           /**< AMR-WB+ Internal Sampling Frequency -- 25600 Hz. */
        CSD_AS_FMT_AMR_WB_PLUS_ISF_9,           /**< AMR-WB+ Internal Sampling Frequency -- 28800 Hz. */
        CSD_AS_FMT_AMR_WB_PLUS_ISF_10,          /**< AMR-WB+ Internal Sampling Frequency -- 32000 Hz. */
        CSD_AS_FMT_AMR_WB_PLUS_ISF_11,          /**< AMR-WB+ Internal Sampling Frequency -- 34133 Hz. */
        CSD_AS_FMT_AMR_WB_PLUS_ISF_12,          /**< AMR-WB+ Internal Sampling Frequency -- 36000 Hz. */
        CSD_AS_FMT_AMR_WB_PLUS_ISF_13,          /**< AMR-WB+ Internal Sampling Frequency -- 38400 Hz. */            
}csd_as_fmt_amrwbplus_isf_index_e;

typedef csd_as_fmt_amrwbplus_isf_index_e csd_as_fmt_amrwbplus_lsf_index_e;

/** AMR frame formats.
*/
typedef enum csd_as_fmt_amr_frame_format {
        CSD_AS_FMT_AMR_FF_CONFORMANCE = 0,     /**< AMR conformance (standard) format. */
        CSD_AS_FMT_AMR_FF_IF1,                 /**< AMR interface format 1. */
        CSD_AS_FMT_AMR_FF_IF2,                 /**< AMR interface format 2. */
        CSD_AS_FMT_AMR_FF_FSF,                 /**< AMR file storage format. */
        CSD_AS_FMT_AMR_FF_RTP,                 /**< Real-time Transport Protocol payload. */
        CSD_AS_FMT_AMR_FF_ITU,                 /**< ITU format. */
        CSD_AS_FMT_AMR_WB_PLUS_FF_TIF,         /**< AMR-WB+ transport interface format. */
        CSD_AS_FMT_AMR_WB_PLUS_FF_FSF,         /**< AMR-WB+ file storage format. */
}csd_as_fmt_amr_frame_format_e;

/** AMR-WB and AMR-WB+ band modes. This enumerator defines band modes supported 
    for AMR-WB+ Rx settings. For backward compatibility, definitions for AMR-WB 
    modes are maintained as is.
*/
typedef enum csd_as_fmt_amrwb_band_mode {
        CSD_AS_FMT_AMR_BM_WB0,                /**< AMR-WB mode 0 --  6600 bps. */
        CSD_AS_FMT_AMR_BM_WB1,                /**< AMR-WB mode 1 --  8850 bps. */
        CSD_AS_FMT_AMR_BM_WB2,                /**< AMR-WB mode 2 -- 12650 bps. */
        CSD_AS_FMT_AMR_BM_WB3,                /**< AMR-WB mode 3 -- 14250 bps. */
        CSD_AS_FMT_AMR_BM_WB4,                /**< AMR-WB mode 4 -- 15850 bps. */
        CSD_AS_FMT_AMR_BM_WB5,                /**< AMR-WB mode 5 -- 18250 bps. */
        CSD_AS_FMT_AMR_BM_WB6,                /**< AMR-WB mode 6 -- 19850 bps. */
        CSD_AS_FMT_AMR_BM_WB7,                /**< AMR-WB mode 7 -- 23050 bps. */
        CSD_AS_FMT_AMR_BM_WB8,                /**< AMR-WB mode 8 -- 23850 bps. */
        CSD_AS_FMT_AMR_BM_WB9,                /**< AMR-WB mode 9 -- Silence indicator. */
        CSD_AS_FMT_AMR_BM_WB_PLUS10,          /**< AMR-WB+ mode 10 -- 13600 bps. */
        CSD_AS_FMT_AMR_BM_WB_PLUS11,          /**< AMR-WB+ mode 11 -- 18000 bps stereo. */
        CSD_AS_FMT_AMR_BM_WB_PLUS12,          /**< AMR-WB+ mode 12 -- 24000 bps. */
        CSD_AS_FMT_AMR_BM_WB_PLUS13,          /**< AMR-WB+ mode 13 -- 24000 bps stereo. */
        CSD_AS_FMT_AMR_BM_WB_PLUS14,          /**< AMR-WB+ mode 14 -- Frame erasure. */
        CSD_AS_FMT_AMR_BM_WB_PLUS15,          /**< AMR-WB+ mode 15 -- No data. */
        CSD_AS_FMT_AMR_BM_WB_PLUS16,          /**< AMR-WB+ mode 16 */
        CSD_AS_FMT_AMR_BM_WB_PLUS17,          /**< AMR-WB+ mode 17 */
        CSD_AS_FMT_AMR_BM_WB_PLUS18,          /**< AMR-WB+ mode 18 */
        CSD_AS_FMT_AMR_BM_WB_PLUS19,          /**< AMR-WB+ mode 19 */
        CSD_AS_FMT_AMR_BM_WB_PLUS20,          /**< AMR-WB+ mode 20 */
        CSD_AS_FMT_AMR_BM_WB_PLUS21,          /**< AMR-WB+ mode 21 */
        CSD_AS_FMT_AMR_BM_WB_PLUS22,          /**< AMR-WB+ mode 22 */
        CSD_AS_FMT_AMR_BM_WB_PLUS23,          /**< AMR-WB+ mode 23 */
        CSD_AS_FMT_AMR_BM_WB_PLUS24,          /**< AMR-WB+ mode 24 */
        CSD_AS_FMT_AMR_BM_WB_PLUS25,          /**< AMR-WB+ mode 25 */
        CSD_AS_FMT_AMR_BM_WB_PLUS26,          /**< AMR-WB+ mode 26 */
        CSD_AS_FMT_AMR_BM_WB_PLUS27,          /**< AMR-WB+ mode 27 */
        CSD_AS_FMT_AMR_BM_WB_PLUS28,          /**< AMR-WB+ mode 28 */
        CSD_AS_FMT_AMR_BM_WB_PLUS29,          /**< AMR-WB+ mode 29 */
        CSD_AS_FMT_AMR_BM_WB_PLUS30,          /**< AMR-WB+ mode 30 */
        CSD_AS_FMT_AMR_BM_WB_PLUS31,          /**< AMR-WB+ mode 31 */
        CSD_AS_FMT_AMR_BM_WB_PLUS32,          /**< AMR-WB+ mode 32 */
        CSD_AS_FMT_AMR_BM_WB_PLUS33,          /**< AMR-WB+ mode 33 */
        CSD_AS_FMT_AMR_BM_WB_PLUS34,          /**< AMR-WB+ mode 34 */
        CSD_AS_FMT_AMR_BM_WB_PLUS35,          /**< AMR-WB+ mode 35 */
        CSD_AS_FMT_AMR_BM_WB_PLUS36,          /**< AMR-WB+ mode 36 */
        CSD_AS_FMT_AMR_BM_WB_PLUS37,          /**< AMR-WB+ mode 37 */
        CSD_AS_FMT_AMR_BM_WB_PLUS38,          /**< AMR-WB+ mode 38 */
        CSD_AS_FMT_AMR_BM_WB_PLUS39,          /**< AMR-WB+ mode 39 */
        CSD_AS_FMT_AMR_BM_WB_PLUS40,          /**< AMR-WB+ mode 40 */
        CSD_AS_FMT_AMR_BM_WB_PLUS41,          /**< AMR-WB+ mode 41 */
        CSD_AS_FMT_AMR_BM_WB_PLUS42,          /**< AMR-WB+ mode 42 */
        CSD_AS_FMT_AMR_BM_WB_PLUS43,          /**< AMR-WB+ mode 43 */
        CSD_AS_FMT_AMR_BM_WB_PLUS44,          /**< AMR-WB+ mode 44 */
        CSD_AS_FMT_AMR_BM_WB_PLUS45,          /**< AMR-WB+ mode 45 */
        CSD_AS_FMT_AMR_BM_WB_PLUS46,          /**< AMR-WB+ mode 46 */
        CSD_AS_FMT_AMR_BM_WB_PLUS47,          /**< AMR-WB+ mode 47 */
}csd_as_fmt_amrwb_band_mode_e;

/** @brief Configures the Rx leg of the AMR-WB+ mode on the MSM device.
*/
struct csd_as_fmt_rx_amrwbplus
{
    uint16_t          channels;
    /**< Number of channels. Supported values:\n
         - 1 -- Mono
         - 2 -- Stereo */
      
    uint32_t          amr_band_mode;
    /**< AMR band mode value. Supported values: 0 to 47 as defined in 
         #csd_as_fmt_amrwb_band_mode_e. */
      
    uint32_t          amr_dtx_mode;
    /**< AMR DTX mode value. Supported value: 0. */
      
    uint32_t          amr_frame_fmt;
    /**< AMR frame format value. Defined in #csd_as_fmt_amr_frame_format_e. */

    uint32_t          amr_lsf_idx;
    /**< AMR Line Spectral Frequency (LSF) index value. Supported values: 0 to 
         13 as defined in #csd_as_fmt_amrwbplus_isf_index_e. */
};
typedef struct csd_as_fmt_rx_amrwbplus csd_as_fmt_rx_amrwbplus_t;

/** \xreflabel{csd_as_fmt_pcm_multi_channel}
  Audio stream decoder output multi-channel mapping definitions 
  (CSD_AS_PCM_CHANNEL_XXX). 
*/ 
typedef enum csd_as_fmt_pcm_multi_channel {

	/**< Revision V1: Basic audio stream decoder output multi-channel mapping definitions. */
	CSD_AS_PCM_MULTI_CHANNEL_NULL =	0,	      /**< Not a valid channel. */
	CSD_AS_PCM_MULTI_CHANNEL_FL   = 1,        /**< Front left channel. */
	CSD_AS_PCM_MULTI_CHANNEL_FR   = 2,        /**< Front right channel. */
	CSD_AS_PCM_MULTI_CHANNEL_FC   = 3,        /**< Front center channel. */
	CSD_AS_PCM_MULTI_CHANNEL_LS   = 4,        /**< Left surround channel. */
	CSD_AS_PCM_MULTI_CHANNEL_RS   = 5,        /**< Right surround channel. */
	CSD_AS_PCM_MULTI_CHANNEL_LFE  = 6,        /**< Low-frequency effects channel. */
	CSD_AS_PCM_MULTI_CHANNEL_CS   = 7,        /**< Center surround channel. */
	CSD_AS_PCM_MULTI_CHANNEL_CB   = 7,        /**< Center back channel. */
	CSD_AS_PCM_MULTI_CHANNEL_LB   = 8,        /**< Left back channel; rear left channel. */
	CSD_AS_PCM_MULTI_CHANNEL_RB   = 9,        /**< Right back channel; rear right channel. */
	CSD_AS_PCM_MULTI_CHANNEL_TS   = 10,       /**< Top surround channel. */
	CSD_AS_PCM_MULTI_CHANNEL_CVH  = 11,       /**< Center vertical height channel. */
	CSD_AS_PCM_MULTI_CHANNEL_TFC  = 11,       /**< Top front center channel. */
	CSD_AS_PCM_MULTI_CHANNEL_MS   = 12,       /**< Mono surround channel. */
	CSD_AS_PCM_MULTI_CHANNEL_FLC  = 13,       /**< Front left center channel. */
	CSD_AS_PCM_MULTI_CHANNEL_FRC  = 14,       /**< Front right center channel. */
	CSD_AS_PCM_MULTI_CHANNEL_RLC  = 15,       /**< Rear left center channel. */
	CSD_AS_PCM_MULTI_CHANNEL_RRC  = 16,       /**< Rear right center channel. */

    /**< Revision V2: Extended audio stream decoder output multi-channel mapping definitions. */
	CSD_AS_PCM_MULTI_CHANNEL_LFE2 = 17,       /**< Secondary low frequency channel. */
	CSD_AS_PCM_MULTI_CHANNEL_SL   = 18,       /**< Side left channel. */
	CSD_AS_PCM_MULTI_CHANNEL_SR   = 19,       /**< Side right channel. */
	CSD_AS_PCM_MULTI_CHANNEL_TFL  = 20,       /**< Top front left channel. */
	CSD_AS_PCM_MULTI_CHANNEL_LVH  = 20,       /**< Left vertical height channel. */
	CSD_AS_PCM_MULTI_CHANNEL_TFR  = 21,       /**< Top front right channel. */
	CSD_AS_PCM_MULTI_CHANNEL_RVH  = 21,       /**< Right vertical height channel. */
	CSD_AS_PCM_MULTI_CHANNEL_TC   = 22,       /**< Top center channel. */
	CSD_AS_PCM_MULTI_CHANNEL_TBL  = 23,       /**< Top back left channel. */
	CSD_AS_PCM_MULTI_CHANNEL_TBR  = 24,       /**< Top back right channel. */
	CSD_AS_PCM_MULTI_CHANNEL_TSL  = 25,       /**< Top side left channel. */
	CSD_AS_PCM_MULTI_CHANNEL_TSR  = 26,       /**< Top side right channel. */
	CSD_AS_PCM_MULTI_CHANNEL_TBC  = 27,       /**< Top back center channel. */
	CSD_AS_PCM_MULTI_CHANNEL_BFC  = 28,       /**< Bottom front center channel. */
	CSD_AS_PCM_MULTI_CHANNEL_BFL  = 29,       /**< Bottom front left channel. */
	CSD_AS_PCM_MULTI_CHANNEL_BFR  = 30,       /**< Bottom front right channel. */
	CSD_AS_PCM_MULTI_CHANNEL_LW   = 31,       /**< Left wide channel. */
	CSD_AS_PCM_MULTI_CHANNEL_RW   = 32,       /**< Right wide channel. */
	CSD_AS_PCM_MULTI_CHANNEL_LSD  = 33,       /**< Left side direct channel. */
	CSD_AS_PCM_MULTI_CHANNEL_RSD  = 34,       /**< Right side direct channel. */

}/** @cond */ csd_as_fmt_pcm_multi_channel_e /** @endcond */;


/**	@brief Configures pulse code modulation on the MSM device. */
struct csd_as_fmt_rx_multi_channel_pcm
{
    uint32_t sample_rate;			/**< Sample rate for the PCM stream. 
                                         Supported values: 2000 to 48000,
                                         96000, 192000 Hz. */
    uint16_t num_channels;			/**< Number of channels. Supported values: 
                                         1 to 8. */
    uint16_t bits_per_sample;		/**< Bits per sample setup. Supported 
                                         values: 16 and 24. */
    uint16_t sign_flag;				/**< Sign flag for PCM. Supported values: 
                                         1 -- Signed. */
    uint16_t interleave_flag;		/**< Interleaved flag for PCM. Supported 
                                         values: \n
                                         - 0 -- Noninterleaved.
                                         - 1 -- Interleaved. */
    uint8_t channel_mapping[8];		/**< Element i describes channel i. 
                                         Supported values  are defined in 
                                         #csd_as_fmt_pcm_multi_channel. 
                                         Unused channels are set to 0. */
};
typedef struct csd_as_fmt_rx_multi_channel_pcm csd_as_fmt_rx_multi_channel_pcm_t;

/**
  @brief Top-level structure for all the Rx formats.
*/
struct csd_as_fmt_rx
{
    uint32_t    fmt_type;  /**< Rx format type. */
    uint32_t    fmt_size;  /**< Rx format block size. */

    /** Playback format. The right format block is decided by the fmt_type 
      field. */
    union
    {
        /** Pulse code modulation format block. */
        struct csd_as_fmt_rx_pcm		pcm;	
        /** Adaptive differential pulse code modulation format block. */  
        struct csd_as_fmt_rx_adpcm		adpcm;	
        /** Windows Media Audio 9 format block. */  
        struct csd_as_fmt_rx_wma_v9		wma_v9;	
        /** Windows Media Audio 10 Pro format block. */  
        struct csd_as_fmt_rx_wma_v10	wma_v10;
        /** Advanced audio codec format block. */  
        struct csd_as_fmt_rx_aac		aac;	
        /** G.711 format block. */  
        struct csd_as_fmt_rx_g711		g711;	
        /** Musical instrument digital interface format block. */  
        struct csd_as_fmt_rx_midi		midi;	
        /** Free lossless audio codec format block. */  
        struct csd_as_fmt_rx_flac		flac;	
        /** Vorbis format block. */  
        struct csd_as_fmt_rx_vorbis		vorbis;
        /** AMR-WB+ format block. */
        struct csd_as_fmt_rx_amrwbplus amrwbplus;
        /** Multiple channel linear PCM format block. */
        struct csd_as_fmt_rx_multi_channel_pcm	multi_channel_pcm;
        /** Advanced audio codec format block V2. */  
        struct csd_as_fmt_rx_aac_v2		aac_v2;	
        
    }u;
};
/* Type definition for #csd_as_fmt_rx_t. */
typedef struct csd_as_fmt_rx csd_as_fmt_rx_t;
/*-------------------------------------------------------------------------*/
/* Tx format block structures */
/*-------------------------------------------------------------------------*/

/** @brief Configures the pulse code modulation for Tx on the MSM device. 
*/
struct csd_as_fmt_tx_pcm
{
    uint32_t sample_rate;       /**< Real sample rate. */
    uint16_t channels;          /**< Channel configuration for the PCM. 
                                     Supported values:\n
                                     - 1 -- Mono.
                                     - 2 -- Stereo. */
    uint16_t bit_per_sample;    /**< Real bit per sample number. */
    uint16_t sign_flag;         /**< Sign flag for the PCM sample. 
                                     Supported values:\n
                                     - 0 -- Unsigned.
                                     - 1 -- Signed. */
    uint16_t interleave_flag;   /**< Interleave flag for the PCM sample. 
                                     Supported values:\n
                                     - 0 -- Non-interleaved.
                                     - 1 -- Interleaved. */
};
/* Type definition for #csd_as_fmt_tx_pcm_t. */
typedef struct csd_as_fmt_tx_pcm csd_as_fmt_tx_pcm_t;

/** @brief Configures the adaptive differential pulse code modulation format on 
    the MSM device. */
struct csd_as_fmt_tx_adpcm
{
    uint32_t    channels;         /**< Channel configuration:\n 
                                      - 0 -- Native mode
                                      - 1 -- Mono.
                                      - 2 -- Stereo. */
    uint32_t    bits_per_sample;  /**< Bits per sample setup. */                      
    uint32_t    sample_rate;      /**< Number of samples per second.\n 
                                      - 0 -- Native mode.
                                      - 1 -- 2000 to 48000. */
    uint32_t    block_size;       /**< Block size for the ADPCM. Supported
                                           values: 20 to 4096.  */  
};

/* Type definition for #csd_as_fmt_tx_adpcm_t. */
typedef struct csd_as_fmt_tx_adpcm csd_as_fmt_tx_adpcm_t;

/**	@brief Configures pulse code modulation on the MSM device. */
struct csd_as_fmt_tx_multi_channel_pcm
{
    uint32_t sample_rate;			/**< Sample rate for the PCM stream. 
                                         Supported values: 2000 to 48000, 
                                         96000, 192000 Hz.*/
    uint16_t num_channels;			/**< Number of channels. 
                                         Supported values: 1 to 8. */
    uint16_t bits_per_sample;		/**< Bit width per sample. 
                                         Supported values: 16 and 24. */
    uint16_t sign_flag;				/**< Sign flag for PCM.  
                                         Supported values: 1 (Signed flag).*/
    uint8_t channel_mapping[8];		/**< Element i describes channel i. 
                                         Supported values  are defined in 
                                         #csd_as_fmt_pcm_multi_channel. 
                                         Unused channels are set to 0. */
};
typedef struct csd_as_fmt_tx_multi_channel_pcm csd_as_fmt_tx_multi_channel_pcm_t;

/** @brief Configures the advanced audio codec on the MSM device. */
struct csd_as_fmt_tx_aac
{
    uint32_t	sample_rate;    /**< Sample rate for the AAC stream. Supported 
                                     values: 8000, 16000, and 48000. */
    uint16_t	channels;       /**< Channel configuration for the AAC. 
                                     Supported values:\n
                                     - 1 -- Mono.
                                     - 2 -- Stereo. */
    uint16_t	format;         
      /**< AAC format. Supported values: \n
           - 0 -- Audio data transport stream advanced audio codec format. 
           - 1 -- Low overhead audio stream advanced audio codec format. 
           - 2 -- Audio data interchange advanced audio codec format. 
           - 3 -- Raw advanced audio codec format. */
    uint32_t	bit_rate;       
      /**< Bit rate of the AAC stream.  */
    uint32_t	encoder_mode;   
      /**< AAC encoder mode. Supported values: \n
           - 2 -- Lossy compression object type. 
           - 5 -- Spectral band replication object type. 
           - 22 -- Bit-sliced arithmetic coding object type. 
           - 29 -- Parametric stereo (AAC) object type. */
};
/* Type definition for #csd_as_fmt_tx_aac_t. */
typedef struct csd_as_fmt_tx_aac csd_as_fmt_tx_aac_t;

/** @brief Configures G.711 (PCM, A-law, mu-law) for Tx on the MSM 
  device. */
struct csd_as_fmt_tx_g711
{
    uint32_t	sample_rate;  /**< Sample rate for the G.711 stream. Supported 
                                   values: 8000 */
};
/* Type definition for #csd_as_fmt_tx_g711_t. */
typedef struct csd_as_fmt_tx_g711 csd_as_fmt_tx_g711_t;

/**
   @name Audio Stream AMR-NB Mode
   @{ 
*/ 

#define CSD_AS_AMR_MODE_NB_0	0   /**< AMR-NB mode 0 --  4750 bps. */
#define CSD_AS_AMR_MODE_NB_1	1   /**< AMR-NB mode 1 --  5150 bps. */
#define CSD_AS_AMR_MODE_NB_2	2   /**< AMR-NB mode 2 --  5900 bps. */
#define CSD_AS_AMR_MODE_NB_3	3   /**< AMR-NB mode 3 --  6700 bps. */
#define CSD_AS_AMR_MODE_NB_4	4   /**< AMR-NB mode 4 --  7400 bps. */
#define CSD_AS_AMR_MODE_NB_5	5   /**< AMR-NB mode 5 --  7950 bps. */
#define CSD_AS_AMR_MODE_NB_6	6   /**< AMR-NB mode 6 -- 10200 bps. */
#define CSD_AS_AMR_MODE_NB_7	7   /**< AMR-NB mode 7 -- 12200 bps. */
/** @} */ /* end_name Audio Stream AMR-NB Mode */

/**
   @name Audio Stream AMR-WB Mode
   @{ 
*/ 
#define CSD_AS_AMR_MODE_WB_0	0   /**< AMR-WB mode 0 --  6600 bps. */
#define CSD_AS_AMR_MODE_WB_1	1   /**< AMR-WB mode 1 --  8850 bps. */
#define CSD_AS_AMR_MODE_WB_2	2   /**< AMR-WB mode 2 -- 12650 bps. */
#define CSD_AS_AMR_MODE_WB_3	3   /**< AMR-WB mode 3 -- 14250 bps. */
#define CSD_AS_AMR_MODE_WB_4	4   /**< AMR-WB mode 4 -- 15850 bps. */
#define CSD_AS_AMR_MODE_WB_5	5   /**< AMR-WB mode 5 -- 18250 bps. */
#define CSD_AS_AMR_MODE_WB_6	6   /**< AMR-WB mode 6 -- 19850 bps. */
#define CSD_AS_AMR_MODE_WB_7	7   /**< AMR-WB mode 7 -- 23050 bps. */
#define CSD_AS_AMR_MODE_WB_8	8   /**< AMR-WB mode 8 -- 23850 bps. */
/** @} */ /* end_name Audio Stream AMR-WB Mode */

/**
   @name Audio Stream AMR DTX Mode
   @{ 
*/ 
#define CSD_AS_AMR_DTX_OFF	0   /**< Discontinuous transmission is disabled.*/
#define CSD_AS_AMR_DTX_VAD1	1   /**< Voice activity detector 1 is enabled. */
#define CSD_AS_AMR_DTX_VAD2	2   /**< Voice activity detector 2 is enabled. */
#define CSD_AS_AMR_DTX_AUTO	3   /**< Codec selects automatically. */
#define CSD_AS_AMR_DTX_EFR	4   /**< Discontinuous transmission is in Enhanced 
                                     Full Rate mode instead of Adaptive Multirate 
                                     Codec Standard mode. */
/** @} */ /* end_name Audio Stream AMR DTX Mode */

/** @brief Configures AMR-NB/AMR-WB on the MSM device. */
struct csd_as_fmt_tx_amr
{
    uint16_t	encoder_mode; 
      /**< Encoder mode for the audio stream. Supported values: \n

           For AMR_NB:
           - 0 -- AMR-NB mode 0 (4750 bps). 
           - 1 -- AMR-NB mode 1 (5150 bps).
           - 2 -- AMR-NB mode 2 (5900 bps).
           - 3 -- AMR-NB mode 3 (6700 bps).
           - 4 -- AMR-NB mode 4 (7400 bps).
           - 5 -- AMR-NB mode 5 (7950 bps).
           - 6 -- AMR-NB mode 6 (10200 bps).
           - 7 -- AMR-NB mode 7 (12220 bps).

           For AMR_WB:
           - 0 -- AMR-WB mode 0 (6600 bps).
           - 1 -- AMR-WB mode 1 (8850 bps).
           - 2 -- AMR-WB mode 2 (12650 bps).
           - 3 -- AMR-WB mode 3 (14250 bps).
           - 4 -- AMR-WB mode 4 (15850 bps).
           - 5 -- AMR-WB mode 5 (18250 bps).
           - 6 -- AMR-WB mode 6 (19850 bps).
           - 7 -- AMR-WB mode 7 (23050 bps).
           - 8 -- AMR-WB mode 8 (23850 bps). */
    uint16_t	dtx_mode;       
      /**< Discontinuous transmission mode. Supported values:\n
           - 0 -- Disables discontinuous transmission.
           - 1 -- Enables voice activity detector 1.
           - 2 -- Enables voice activity detector 2.
           - 3 -- Codec selects automatically. 
           - 4 -- DTX uses enhanced full rate instead of adaptive multirate 
                  codec standard. */
};
/* Type definition for #csd_as_fmt_tx_amr_t. */
typedef struct csd_as_fmt_tx_amr csd_as_fmt_tx_amr_t;
/**
   @name Audio Stream QCELP 13K Bitrate
   @{ 
*/ 
#define CSD_AS_FMT_QCELP_BR_0		0 /**< QCELP 13K bitrate 0 -- 14.4 kbps. */
#define CSD_AS_FMT_QCELP_BR_1		1 /**< QCELP 13K bitrate 1 -- 12.2 kbps. */
#define CSD_AS_FMT_QCELP_BR_2		2 /**< QCELP 13K bitrate 2 -- 11.2 kbps. */
#define CSD_AS_FMT_QCELP_BR_3		3 /**< QCELP 13K bitrate 3 -- 9 kbps. */
#define CSD_AS_FMT_QCELP_BR_4		4 /**< QCELP 13K bitrate 4 -- 7.2 kbps. */
/** @} */ /* end_name Audio Stream QCELP 13K Bit Rate */

/** @brief Configures QCELP 13K on the MSM device. */
struct csd_as_fmt_tx_qcelp
{
    uint32_t	min_frame_rate;     
      /**< Minimum CDMA encoded frame rate. Supported values: \n 
           - 1 -- Eighth rate.
           - 2 -- Quarter rate.
           - 3 -- Half rate.
           - 4 -- Full rate. */
    uint32_t	max_frame_rate;     
      /**< Maximum CDMA encoded frame rate. Supported values: \n
           - 1 -- Eighth rate.
           - 2 -- Quarter rate.
           - 3 -- Half rate.
           - 4 -- Full rate. */
    uint16_t	reduce_rate_cmd;    
      /**< Reduced CDMA encoded frame rate. Supported values: \n
           - 0 -- 14.4 kbps.
           - 1 -- 12.2 kbps.
           - 2 -- 11.2 kbps.
           - 3 -- 9 kbps.
           - 4 -- 7.2 kbps. */
    uint16_t	rate_mod_cmd;  
      /**< Rate modulation. \n
           - Bit 0:\n
             - 1 -- Rate control is enabled.
           - Bit 1:\n
             - 1 -- Limits the maximum number of consecutive full rate frames 
                    with the number supplied in bits 2 to 9. 
             - 0 -- Forces the minimum number of non-full rate frames in 
                    between two full rate frames to the number supplied in bits 2 
                    to 9.\n  
                    @note1hang In both cases, half rate is used to substitute 
                    full rate when necessary.    
           - Bits 15..10 are reserved and set to 0. */
};
/* Type definition for #csd_as_fmt_tx_qcelp_t. */
typedef struct csd_as_fmt_tx_qcelp csd_as_fmt_tx_qcelp_t;
/**
   @name Audio Stream EVRC Rate Control
   @{ 
*/ 
#define CSD_AS_FMT_EVRC_BR_DISABLE	0  /**< Rate modulation control disabled. */
#define CSD_AS_FMT_EVRC_BR_ENABLE	1  /**< Rate modulation control enable. */
/** @} */ /* end_name Audio Stream EVRC Rate Control */
/**
   @name Audio Stream EVRCB Bitrate
   @{ 
*/ 
#define CSD_AS_FMT_EVRCB_BR_0		0   /**< EVRCB bitrate 0 -- 9.3 kbps. */
#define CSD_AS_FMT_EVRCB_BR_1		1   /**< EVRCB bitrate 1 -- 8.5 kbps. */
#define CSD_AS_FMT_EVRCB_BR_2		2   /**< EVRCB bitrate 2 -- 7.5 kbps. */
#define CSD_AS_FMT_EVRCB_BR_3		3   /**< EVRCB bitrate 3 -- 7.0 kbps. */
#define CSD_AS_FMT_EVRCB_BR_4		4   /**< EVRCB bitrate 4 -- 6.6 kbps. */
#define CSD_AS_FMT_EVRCB_BR_5		5   /**< EVRCB bitrate 5 -- 6.2 kbps. */
#define CSD_AS_FMT_EVRCB_BR_6		6   /**< EVRCB bitrate 6 -- 5.8 kbps. */
#define CSD_AS_FMT_EVRCB_BR_7		7   /**< EVRCB bitrate 7 -- 4.8 kbps. */
/** @} */ /* end_name Audio Stream EVRCB Bit Rate */
/**
   @name Audio Stream EVRCWB Bitrate
   @{ 
*/ 
#define CSD_AS_FMT_EVRCWB_BR_0		0   /**< EVRCWB bitrate 0 -- 8.5 kbps. */
#define CSD_AS_FMT_EVRCWB_BR_4		4   /**< EVRCWB bitrate 4 -- 9.3 kbps. */
#define CSD_AS_FMT_EVRCWB_BR_7		7   /**< EVRCWB bitrate 7 -- 4.8 kbps. */
/** @} */ /* end_name Audio Stream EVRCWB Bit Rate */

/** @brief Configures EVRC, EVRCB, and EVRCWB on the MSM device. */
struct csd_as_fmt_tx_evrc
{
    uint16_t	bit_rate_ctrl;
      /**< Bit rate control command:\n
           For EVRC, used as the average bit rate control command:\n 
           - Bit 0:\n
             - 0 -- Rate control is disabled.
             - 1 -- Rate control is enabled.
           - Bit 1:\n
             - 1 -- Limits the maximum number of consecutive full rate 
                    frames with the number supplied in bits 2 to 9.\n
             - 0 -- Forces the minimum number of non-full rate frames in 
                    between two full rate frames to the number supplied in 
                    bits 2 to 9. 
           - Bit 9..2 -- Number of frames.
           - Bits 15..10 -- Reserved and set to 0.

           For EVRC-B, uses the pre-defined values:\n
           - 0 -- 9.3 kbps.
           - 1 -- 8.5 kbps.
           - 2 -- 7.5 kbps.
           - 3 -- 7.0 kbps.
           - 4 -- 6.6 kbps.
           - 5 -- 6.2 kbps.
           - 6 -- 5.8 kbps.
           - 7 -- 4.8 kbps.

           For EVRC-WB, uses the pre-defined values:\n
           - 0 -- 9.3 kbps.
           - 4 -- 6.6 kbps.
           - 7 -- 4.8 kbps.*/
    uint16_t	min_frame_rate; /**< Minimum CDMA encoded frame rate. Supported values: \n
                                     - 1 -- Eighth rate.
                                     - 2 -- Quarter rate.
                                     - 3 -- Half rate.
                                     - 4 -- Full rate.   */
    uint16_t	max_frame_rate; /**< Maximum CDMA encoded frame rate. Supported values: \n
                                     - 1 -- Eighth rate.
                                     - 2 -- Quarter rate.
                                     - 3 -- Half rate.
                                     - 4 -- Full rate.   */
    uint16_t	dtx_mode;       /**< DTX mode enable flag:\n
                                     - 0 -- Disable.
                                     - > 0 -- Enable. */
};
/* Type definition for #csd_as_fmt_tx_evrc_t. */
typedef struct csd_as_fmt_tx_evrc csd_as_fmt_tx_evrc_t;
/**
   @name Audio Stream SBC Allocation Method
   @{ 
*/ 
#define CSD_AS_FMT_SBC_ALLOC_LOUDNESS       0 /**< Loudness mode for subband 
                                                   coding. */
#define CSD_AS_FMT_SBC_ALLOC_SNR			1 /**< Signal-to-Noise Ratio mode 
                                                   for subband coding. */
/** @} */ /* end_name Audio Stream SBC Allocation Method */

/** @brief Configures the subband coding on the MSM device. */
struct csd_as_fmt_tx_sbc
{
    uint32_t	sub_bands;      /**< Number of subbands. Supported values: 4 or 8. */
    uint32_t	block_len;      /**< Block length. Supported values: 4, 8, 12, or 16. */
    uint32_t	channel_mode;   /**< Channel allocation mode for SBC. Supported
                                     values: \n
                                     - 1 -- Mono channel.
                                     - 2 -- Stereo channel.
                                     - 8 -- Dual/mono mode.
                                     - 9 -- Joint stereo mode.*/
    uint32_t	alloc_method;   /**< SBC Allocation mode. Supported values:\n
                                     - 0 -- Loudness.
                                     - 1 -- Signal-to-noise ratio. */
    uint32_t	bit_rate;       /**< Bits per second. */
    uint32_t	sample_rate;    /**< Sample rate. Supported values:\n
                                     - 0 -- Native mode.
                                     - Specify a sample rate. */
};
/* Type definition for #csd_as_fmt_tx_sbc_t. */
typedef struct csd_as_fmt_tx_sbc csd_as_fmt_tx_sbc_t;
/** 
  @brief Top-level structure for all the Tx formats. 
*/
struct csd_as_fmt_tx
{
    uint32_t    fmt_type;   /**< Format block type. */
    uint32_t    fmt_size;   /**< Format block size. */

    /** Recording formats. The right format block is determined by the fmt_type 
        field. */
    union
    {
        /** Pulse code modulation format block. */
        struct csd_as_fmt_tx_pcm		pcm;	
        
        /** Adaptive differential pulse code modulation format block. */  
        struct csd_as_fmt_tx_adpcm		adpcm;	
        
        /** Advanced audio codec format block. */
        struct csd_as_fmt_tx_aac		aac;	

        /** G.711 format block. */
        struct csd_as_fmt_tx_g711		g711;	

        /** Adaptive multirate codec format block. */
        struct csd_as_fmt_tx_amr		amr;	

        /** Qualcomm code excited linear prediction format block. */
        struct csd_as_fmt_tx_qcelp		qcelp;	

        /** Enhanced variable rate coder format block. */
        struct csd_as_fmt_tx_evrc		evrc;	

        /** Subband coding format block. */
        struct csd_as_fmt_tx_sbc		sbc;	

        /** Multiple channel Pulse code modulation format block. */
        struct csd_as_fmt_tx_multi_channel_pcm	multi_channel_pcm;
    }u;
};
/* Type definition for #csd_as_fmt_tx_t. */
typedef struct csd_as_fmt_tx csd_as_fmt_tx_t;

/** @} */ /* end_addtogroup csd_as */

#endif  /* _CSD_AS_FMT_H_*/
