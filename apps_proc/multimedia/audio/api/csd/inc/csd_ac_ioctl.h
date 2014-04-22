#ifndef _CSD_AC_IOCTL_H_
#define _CSD_AC_IOCTL_H_
/**
	@file      csd_ac_ioctl.h
    @brief  
	Contains audio context IOCTL definitions
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

	$Header: //components/rel/audio.tx/2.0/audio/api/csd/inc/csd_ac_ioctl.h#1 $
	$DateTime: 2018/12/25 22:39:07 $
	$Author: pwbldsvc $
	$Change: 17929171 $
	$Revision: #1 $

	when		who			what, where, why
	--------	-----		---------------------------------------------------
    06/04/12    sd          (Tech Pubs) Edited/added Doxygen markup and comments.
    03/15/11    sd          (Tech Pubs) Edited/added Doxygen markup and comments.
	10/25/10	jzhou		Doxygen comments update
	08/17/10	jzhou		Doxygen comments update
	05/13/10	mingchen	initial draft 
=============================================================================*/
#include "mmdefs.h"
#include "csd_aud_pp.h" 
/** @addtogroup csd_ac
  @{
*/

/** 
   @name Audio Context IOCTL Commands
   @{ 
 */ 

/** Begins the audio context IOCTL commands. */
#define CSD_AC_CMD_START			0x00011800				

/** Attaches streams to the audio context. 

  @payload
  #csd_ac_as_attach

  @latexonly \input{tables/CSD_AC_CMD_AS_ATTACH.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AC_CMD_AS_ATTACH        (CSD_AC_CMD_START+1)	

/** Detaches the audio streams from the audio context. 

  @payload
  #csd_ac_as_detach

  @latexonly \input{tables/CSD_AC_CMD_AS_DETACH.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/  
#define CSD_AC_CMD_AS_DETACH        (CSD_AC_CMD_START+2)	

/** Sets the device ID information for an audio context. 

  @payload
  #csd_ac_set_dev

  @latexonly \input{tables/CSD_AC_CMD_SET_DEVICE.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AC_CMD_SET_DEVICE       (CSD_AC_CMD_START+3)	

/** Enables the audio context. 

  @payload
  None.

  @return
  CSD status code.

  @dependencies
  None.
*/  
#define CSD_AC_CMD_ENABLE           (CSD_AC_CMD_START+4)	

/** Disables the audio context. 

  @payload
  None.

  @return
  CSD status code.

  @dependencies
  None.
*/  
#define CSD_AC_CMD_DISABLE          (CSD_AC_CMD_START+5)	

/** Sets the pre/postprocessing for the audio context. 

  @payload
  #csd_ac_pp_config

  @latexonly \input{tables/CSD_AC_CMD_CONFIG_PP_PARAMS.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/  
#define CSD_AC_CMD_CONFIG_PP_PARAMS (CSD_AC_CMD_START+6)	

/** Registers the callback function for the audio context. 

  @payload
  #csd_ac_cb

  @latexonly \input{tables/CSD_AC_CMD_SET_EVT_CB.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/  
#define CSD_AC_CMD_SET_EVT_CB		(CSD_AC_CMD_START+7)	

/** Retrieves the spectrum-analyzed data from the audio context. */  
#define CSD_AC_CMD_GET_SPA_DATA		(CSD_AC_CMD_START+8)	

/** Set up multiple channels for the audio context. This command only applies to 
	an Rx device.

  @payload
  #csd_ac_config_multi_channel

  @latexonly \input{tables/CSD_AC_CMD_CONFIG_MULTI_CHANNEL.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AC_CMD_CONFIG_MULTI_CHANNEL		(CSD_AC_CMD_START+9)

/** Set audio latency on given device. 
    For example, this API can be used for the HDMI auto lipsync feature.

  @payload
  #csd_ac_config_latency

  @latexonly \input{tables/CSD_AC_CMD_CONFIG_LATENCY.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_AC_CMD_CONFIG_LATENCY   (CSD_AC_CMD_START+10)

/** Audio context asynchronous callback event ID. */  
#define CSD_AC_CMD_EVENT_ID			(CSD_AC_CMD_START+512)	

/** Ending of the audio context IOCTL commands. */  
#define CSD_AC_CMD_END				 0x00011BFF				
  
/** @} */ /* end_name Audio Context IOCTL Commands */

/**
  @brief Payload for #CSD_AC_CMD_AS_ATTACH.
*/
struct csd_ac_as_attach {
    uint32_t  num_as_handles; /**< Number of audio stream handles. */
    uint32_t* as_handles;     /**< Array of the audio stream handles. */
};
/* Type definition for #csd_ac_as_attach_t. */ 
typedef struct csd_ac_as_attach csd_ac_as_attach_t;

/**
  @brief Payload for #CSD_AC_CMD_AS_DETACH. 
*/
struct csd_ac_as_detach {
    uint32_t  num_as_handles; /**< Number of audio stream handles. */
    uint32_t* as_handles;     /**< Array of the audio stream handles. */
};
/* Type definition for #csd_ac_as_detach_t. */
typedef struct csd_ac_as_detach csd_ac_as_detach_t;
/**
  @brief Payload for #CSD_AC_CMD_SET_DEVICE.
*/
struct csd_ac_set_dev {
	uint32_t sample_rate;  /**< Sample rate for the device. */
    uint32_t dev_id;       /**< Device ID. The sample rate is always 48K. */
};
/* Type definition for #csd_ac_set_dev_t. */
typedef struct csd_ac_set_dev csd_ac_set_dev_t;
/**
 @brief Payload for #CSD_AC_CMD_CONFIG_PP_PARAMS. 
*/
struct csd_ac_pp_config
{
	uint32_t pp_type;	/**< Pre/postprocessing filter type for the audio 
                             stream. */

    /** Pre/postprocessing structure for the audio context. */
	union 
	{
        /** Volume pre/postprocessing for the audio context. */
        struct csd_aud_pp_vol   vol;	
        /** Equalizer pre/postprocessing for the audio context. */
        struct csd_aud_pp_eq    eq;		
        /** Spectrum analyzer pre/postprocessing for the audio context. */
        struct csd_aud_pp_spa	spa;
        /** QConcert Plus Reverb pre/postprocessing for the audio context. */	
        struct csd_aud_pp_qcpr  qcpr;   
	}u;
};
/* Type definition for #csd_ac_pp_config_t. */
typedef struct csd_ac_pp_config csd_ac_pp_config_t;

/** 
  Callback prototype for the audio context. 

  @param[in] evnt_id      Callback event for the audio context.
  @param[in] payload       Event payload associated with the event.
  @param[in] payload_size  Payload size for the event.
  @param[in] client_data    Client data associated with the callback function.

  @return
  None.

  @dependencies
  None. 
*/
typedef void (*csd_ac_cb_fn)(uint32_t evnt_id, void * payload, 
                             uint32_t payload_size,void * client_data);

/**
  @brief Payload for #CSD_AC_CMD_SET_EVT_CB. 
*/
struct csd_ac_cb
{
   csd_ac_cb_fn cb;       /**< Callback function pointer for the audio context
                               callback. */
   void         *cb_data; /**< Client data associated with the callback function. */
};
/* Type definition for #csd_ac_cb_t. */
typedef struct csd_ac_cb csd_ac_cb_t;
/**
  @brief Payload for #CSD_AC_CMD_GET_SPA_DATA. 
*/
struct csd_ac_spa_buffer
{
	uint8_t		*buf_addr;			/**< Pointer to the buffer start. */
	uint32_t	buf_size;			/**< Buffer size in bytes as input. */
	uint32_t	num_bytes_returned; /**< Number of bytes returned from the 
                                         driver. */
};
/* Type definition for #csd_ac_spa_buffer_t. */
typedef struct csd_ac_spa_buffer csd_ac_spa_buffer_t;

/**
    @name SPA Information Header for the Audio Context
    @{

    Following is the spectrum analyzer output buffer format:\n
    - Beginning of buffer -- SPA infomation header 
      (struct #csd_ac_buf_spa_info_header). 
    - Next 32-bit aligned position:\n
      - Spectrum header (struct #csd_ac_buf_spa_data_header). 
      - Spectrum coefficient data. The length of the SPA data field can be 
        calculated from the spectrum header field. 
    - Next 32-bit aligned position:\n
      - PCM header (struct #csd_ac_buf_spa_pcm_data_header). 
      - PCM data. The length of the PCM data field can be calculated from the 
        PCM header field.
*/ 
#define CSD_AC_SPA_BUF_MASK_TS_FLAG		0x80000000	
  /**< Timestamp mask for the spectrum analyzer buffer. */
#define CSD_AC_SPA_BUF_MASK_PCM_FLAG	0x40000000	
  /**< Pulse code modulation data presence in the output buffer mask. */
/** @} */ /* end_name SPA Information Header */

/** 
  @brief Defines the basic information of the analyzed result at the beginning 
  of each spectrum analyzer data buffer.
*/
 struct csd_ac_buf_spa_info_header
 {
	uint32_t flags;	      
      /**< Bitmask flags for the header:\n 
           - Bit 31 -- Timestamp valid bitmask.\n
             - 1 -- Timestamp valid in the header.
             - 0 -- Timestamp not valid in the header.
           - Bit 30 -- Pulse code modulation data presence bitmask.\n
             - 1 -- PCM data present in the data buffer.
             - 0 -- PCM data absent in the data buffer.
           - Other bits are reserved. */
	uint32_t ts_high;     
      /**< Upper 32 bits of the 64-bit session time of the first sample in the 
           buffer in microseconds.  */
	uint32_t ts_low;      
      /**< Lower 32 bits of the 64-bit session time of the first sample in the 
           buffer in microseconds.  */
	uint32_t sample_rate; 
      /**< Sampling rate of the analyzed audio stream in Hz. */
 };
/* Type definition for #csd_ac_buf_spa_info_header_t. */
 typedef struct csd_ac_buf_spa_info_header csd_ac_buf_spa_info_header_t;

/**
  @brief Defines the spectrum header information in the spectrum analyzer data
  buffer. 

  The information exists in the spectrum analyzer data buffer memory. 
  Following this header in the spectrum analyzer data buffer is the actual 
  coefficient data based on num_coeffs. 
*/
 struct csd_ac_buf_spa_data_header
 {
	 uint32_t coeff_bit_size;   /**< Size of each coefficient data in bits. */
 	 uint32_t num_coeffs;       /**< Number of coefficients. */
 };
/* Type definition for #csd_ac_buf_spa_data_header_t. */
 typedef struct csd_ac_buf_spa_data_header csd_ac_buf_spa_data_header_t;

/**
  @brief Defines the pulse code modulation header information in the spectrum 
  analyzer data buffer. 

  The information exists in the spectrum analyzer data buffer memory. Following
  this header in the spectrum analyzer data buffer is the actual PCM raw data 
  based on num_samples. 
*/
 struct csd_ac_buf_spa_pcm_data_header
 {
    uint32_t sample_bit_size;   /**< Size of each PCM sample in bits. */
    uint32_t num_samples;       /**< Number of samples. */
 };
/* Type definition for #csd_ac_buf_spa_pcm_data_header_t*/
 typedef struct csd_ac_buf_spa_pcm_data_header csd_ac_buf_spa_pcm_data_header_t;
 
/** 
    @name Audio Context Multi-Channel Mapping Definitions
    @{
*/ 

/**< Revision V1: Basic audio context multi-channel mapping definitions. */
#define CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_NULL	0	/**< Not a valid channel. */
#define CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_FL		1 	/**< Front left channel. */
#define CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_FR		2	/**< Front right channel. */
#define CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_FC		3	/**< Front center channel. */
#define CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_LS		4	/**< Left surround channel. */
#define CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_RS		5	/**< Right surround channel. */
#define CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_LFE	6	/**< Low-frequency effects channel. */
#define CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_CS		7	/**< Center surround channel. */
#define	CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_CB     7   /**< Center back channel. */
#define CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_LB		8	/**< Left back channel; rear left channel. */
#define CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_RB		9	/**< Right back channel; rear right channel. */
#define CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_TS		10	/**< Top surround channel. */
#define CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_CVH	11	/**< Center vertical height channel. */
#define CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_TFC    11  /**< Top front center channel. */
#define CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_MS		12	/**< Mono-surround channel. */
#define CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_FLC	13	/**< Front left center channel. */
#define CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_FRC	14	/**< Front right center channel. */
#define CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_RLC	15	/**< Rear left center channel. */
#define CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_RRC	16	/**< Rear right center channel. */

/**< Revision V2: Extended audio context multi-channel mapping definitions. */
#define	CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_LFE2   17  /**< Secondary low frequency channel. */
#define	CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_SL     18  /**< Side left channel. */
#define	CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_SR     19  /**< Side right channel. */
#define	CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_TFL    20  /**< Top front left channel. */
#define	CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_LVH    20  /**< Left vertical height channel. */
#define	CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_TFR    21  /**< Top front right channel. */
#define	CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_RVH    21  /**< Right vertical height channel. */
#define	CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_TC     22  /**< Top center channel. */
#define	CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_TBL    23  /**< Top back left channel. */
#define	CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_TBR    24  /**< Top back right channel. */
#define	CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_TSL    25  /**< Top side left channel. */
#define	CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_TSR    26  /**< Top side right channel. */
#define	CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_TBC    27  /**< Top back center channel. */
#define	CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_BFC    28  /**< Bottom front center channel. */
#define	CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_BFL    29  /**< Bottom front left channel. */
#define	CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_BFR    30  /**< Bottom front right channel. */
#define	CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_LW     31  /**< Left wide channel. */
#define	CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_RW     32  /**< Right wide channel. */
#define	CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_LSD    33  /**< Left side direct channel. */
#define	CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_RSD    34  /**< Right side direct channel. */
/** @} */ /* end_name Audio Context Multi-Channel Mapping Definitions */

/**
  @brief Payload for #CSD_AC_CMD_CONFIG_MULTI_CHANNEL.
*/
struct csd_ac_config_multi_channel {
    uint16_t num_channels;  	/**< Number of channels to be set up. \n
									 Supported values: 3 to 8. */
    uint8_t  *channel_mapping;	/**< Pointer to an array of
									 CSD_AC_MULTI_CHANNEL_MAPPING_CHANNEL_xxx,
    								 which indicates the channel layout mapping of  
									 audio device. */
};
/* Type definition for #csd_ac_config_multi_channel_t. */
typedef struct csd_ac_config_multi_channel csd_ac_config_multi_channel_t;

/**
  @brief Payload for #CSD_AC_CMD_CONFIG_LATENCY.
*/
struct csd_ac_config_latency {
    uint32_t delay;             /**< Audio latency in micro seconds.
									 Supported values: 0 to 100000. */
};
/* Type definition for #csd_ac_config_latency_t. */
typedef struct csd_ac_config_latency csd_ac_config_latency_t;

/*-------------------------------------------------------------------------*/
/*  event ids */
/*-------------------------------------------------------------------------*/

/** @xreflabel{CSD_AC_EVT_SPA_BUF_READY}
  Event ID for readiness of the spectrum analyzer data. */
#define CSD_AC_EVT_SPA_BUF_READY    (CSD_AC_CMD_EVENT_ID+1)

/** @} */ /* end_addtogroup csd_ac */

#endif /* _CSD_AC_IOCTL_H_*/
