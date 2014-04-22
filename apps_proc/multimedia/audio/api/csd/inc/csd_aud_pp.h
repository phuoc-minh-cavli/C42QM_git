#ifndef _CSD_AUD_PP_H_
#define _CSD_AUD_PP_H_
/**
	@file       csd_aud_pp.h
    @brief      
	Structure definitions for audio pre/post processing
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
	 Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                      EDIT HISTORY FOR MODULE
	This section contains comments describing changes made to this module.
	Notice that changes are listed in reverse chronological order.

	$Header: //components/rel/audio.tx/2.0/audio/api/csd/inc/csd_aud_pp.h#1 $
	$DateTime: 2018/12/25 22:39:07 $
	$Author: pwbldsvc $
	$Change: 17929171 $
	$Revision: #1 $

	when		who			what, where, why
	--------	-----		---------------------------------------------------
	04/10/13	chenche		Add digital set volume API for audio stream.
	11/09/12    suns        Add Soft Pause Resume Volume Control for audio stream.
    06/04/12    sd          (Tech Pubs) Edited/added Doxygen markup and comments.
    03/15/11    sd          (Tech Pubs) Edited/added Doxygen markup and comments.
	10/21/10	jzhou		Doxygen comments update
	08/17/10	jzhou		Doxygen comments update
	04/23/10	jzhou		initial release

	=========================================================================*/

#include "mmdefs.h"

/** @addtogroup csd_audiopp
  @{
*/

/*-------------------------------------------------------------------------*/
/*  audio post/pre processing type defines */
/*-------------------------------------------------------------------------*/
/** 
   @name Audio Pre/Postprocessing Type
   @{ 
*/ 
/** Volume control filter in the audio stream.

  @payload
  #csd_aud_pp_vol

  @latexonly \input{tables/CSD_AUD_PP_TYPE_VOL.tex} @endlatexonly

  @return
  None.

  @dependencies
  None.
*/
#define CSD_AUD_PP_TYPE_VOL		0x00012701		

/**
   @name Audio Pre/Postprocessing Type
   @{
*/
/** Digital volume control filter in the audio stream.

  @payload
  #csd_aud_pp_vol_digital

  @latexonly \input{tables/CSD_AUD_PP_TYPE_VOL_DIGITAL.tex} @endlatexonly

  @return
  None.

  @dependencies
  None.
*/
#define CSD_AUD_PP_TYPE_VOL_DIGITAL		0x00012EBF

/** QConcert&reg;&nbsp;Plus reverb filter types. 

  @payload
  #csd_aud_pp_qcpr

  @latexonly \input{tables/CSD_AUD_PP_TYPE_QCPR.tex} @endlatexonly

  @return
  None.

  @dependencies
  None.
*/
#define CSD_AUD_PP_TYPE_QCPR    0x00012702		

/** Configuration for the equalizer controls. 

  @payload
  #csd_aud_pp_eq

  @latexonly \input{tables/CSD_AUD_PP_TYPE_EQ.tex} @endlatexonly

  @return
  None.

  @dependencies
  None.
*/  
#define CSD_AUD_PP_TYPE_EQ      0x00012703		

/** Configuration for the spectrum analyzer filters. 

  @payload
  #csd_aud_pp_spa

  @latexonly \input{tables/CSD_AUD_PP_TYPE_SPA.tex} @endlatexonly

  @return
  None.

  @dependencies
  None.
*/
#define CSD_AUD_PP_TYPE_SPA     0x00012300		

/** Time scale modification filter type. 

  @payload
  #csd_aud_pp_tsm

  @latexonly \input{tables/CSD_AUD_PP_TYPE_TSM.tex} @endlatexonly

  @return
  None.

  @dependencies
  None.
*/  
#define CSD_AUD_PP_TYPE_TSM		0x00012705	
	
/** Soft pause/resume configuration for the audio stream.

  @payload
  #csd_aud_pp_spr

  @latexonly \input{tables/CSD_AUD_PP_TYPE_SPR.tex} @endlatexonly

  @return
  None.

  @dependencies
  None.
*/
#define CSD_AUD_PP_TYPE_SPR		0x00012A76
  
/** @} */ /* end_name Audio Pre/Postprocessing Type */
/*-------------------------------------------------------------------------*/
/*  volume control filter*/
/*-------------------------------------------------------------------------*/

/**	Sets the master volume control. 

  @payload
  #csd_aud_pp_vol_master_gain

  @latexonly \input{tables/CSD_AUD_PP_VOL_PARAM_ID_MASTER_GAIN.tex} @endlatexonly

  @return
  None.

  @dependencies
  None.
*/
#define CSD_AUD_PP_VOL_PARAM_ID_MASTER_GAIN        0x00012706

/** Sets the stereo gain ID. 

  @payload
  #csd_aud_pp_vol_stereo_gain

  @latexonly \input{tables/CSD_AUD_PP_VOL_PARAM_ID_STEREO_GAIN.tex} @endlatexonly

  @return
  None.

  @dependencies
  None.
*/
#define CSD_AUD_PP_VOL_PARAM_ID_STEREO_GAIN        0x00012707

/** Sets the mute control. 

  @payload
  #csd_aud_pp_vol_mute

  @latexonly \input{tables/CSD_AUD_PP_VOL_PARAM_ID_MUTE.tex} @endlatexonly

  @return
  None.

  @dependencies
  None.
*/
#define CSD_AUD_PP_VOL_PARAM_ID_MUTE               0x00012708

/** Sets the multichannel volume control. 

  @payload
  #csd_aud_pp_vol_multi_channel_gain

  @latexonly \input{tables/CSD_AUD_PP_VOL_PARAM_ID_MULTI_CHANNEL_GAIN.tex} @endlatexonly

  @return
  None.

  @dependencies
  None.
*/
#define CSD_AUD_PP_VOL_PARAM_ID_MULTI_CHANNEL_GAIN 0x00012709

/** @brief Sets the master gain. */
struct csd_aud_pp_vol_master_gain
{
    uint16_t master_gain_step;    /**< Step level of the master gain. */
    uint16_t reserved;            /**< For 32-bit alignment. */
};
/* Type definition for #csd_aud_pp_vol_master_gain_t. */
typedef struct csd_aud_pp_vol_master_gain csd_aud_pp_vol_master_gain_t;

/** @brief Sets the stereo gain. */
struct csd_aud_pp_vol_stereo_gain
{
    uint16_t left_ch_gain_step;  /**< Step level of the left channel gain. */
    uint16_t right_ch_gain_step; /**< Step level of the right channel gain. */
};
/* Type definition for #csd_aud_pp_vol_stereo_gain_t. */
typedef struct csd_aud_pp_vol_stereo_gain csd_aud_pp_vol_stereo_gain_t;

/**
   @name Channel Types
   @{ 
*/ 
/** Left channel type. */
#define CSD_AUD_PP_MULTI_CH_VOL_LEFT            1
/** Right channel type. */
#define CSD_AUD_PP_MULTI_CH_VOL_RIGHT           2
/** Center channel type. */
#define CSD_AUD_PP_MULTI_CH_VOL_CENTER          3
/** Left surround channel type. */
#define CSD_AUD_PP_MULTI_CH_VOL_LEFT_SURROUND   4
/** Right surround channel type. */
#define CSD_AUD_PP_MULTI_CH_VOL_RIGHT_SURROUND  5
/** Left back channel type. */
#define CSD_AUD_PP_MULTI_CH_VOL_LEFT_BACK       6
/** Right back channel type. */
#define CSD_AUD_PP_MULTI_CH_VOL_RIGHT_BACK      7
/** Subwoofer channel type. */
#define CSD_AUD_PP_MULTI_CH_VOL_SUBWOOFER       8
/** @} */ /* end_name Channel Types */

/** @brief Sets the multichannel gain. */
struct csd_aud_pp_vol_multi_channel_gain_entry
{
    uint16_t ch_type;  /**< Channel type. */
	uint16_t gain_idx; /**< Gain index. */
};
/* Type definition for #csd_aud_pp_vol_multi_channel_gain_entry_t. */
typedef struct csd_aud_pp_vol_multi_channel_gain_entry csd_aud_pp_vol_multi_channel_gain_entry_t;

/** @brief Sets the multichannel gain. */
struct csd_aud_pp_vol_multi_channel_gain
{
	uint32_t num_channels;     /**< Number of channels. */

    /** Gain array. */
	struct csd_aud_pp_vol_multi_channel_gain_entry* entries; 
};
/* Type definition for #csd_aud_pp_vol_multi_channel_gain_t. */
typedef struct csd_aud_pp_vol_multi_channel_gain csd_aud_pp_vol_multi_channel_gain_t;

/**
   @name Volume Mute Value
   @{ 
*/ 
#define CSD_AUD_PP_VOL_MUTE				1   /**< Volume is muted. */
#define CSD_AUD_PP_VOL_UNMUTE           0   /**< Volume is not muted. */
/** @} */ /* end_name Volume Mute Value */

/** @brief Mutes the pre/postprocessor. */
struct csd_aud_pp_vol_mute
{
	uint32_t mute;        /**< Mute disable/enable:\n 
                               - 0 -- Unmute. 
                               - 1 -- Mute. */
};
/* Type definition for #csd_aud_pp_vol_mute_t. */
typedef struct csd_aud_pp_vol_mute csd_aud_pp_vol_mute_t;
/**	
  @brief Contains the volume commands. 
*/
struct csd_aud_pp_vol {
    uint32_t param_id;    /**< ID of the volume control parameter. */

    /** Pre/postprocessing volume control. */
	union {
        /** Master gain control. */
        struct csd_aud_pp_vol_master_gain        master_gain;
        /** Stereo gain control. */
        struct csd_aud_pp_vol_stereo_gain        stereo_gain;
        /** Multichannel gain control. */
        struct csd_aud_pp_vol_multi_channel_gain multi_ch_gain;
        /** Mute control. */
        struct csd_aud_pp_vol_mute               mute;
	} u;
};
/* Type definition for #csd_aud_pp_vol_t. */
typedef struct csd_aud_pp_vol csd_aud_pp_vol_t;
/** @brief Sets the digital master gain. */
struct csd_aud_pp_vol_digital_master_gain
{
    uint16_t master_gain_digital;     /**< master gain in q_factor specified format. */
    uint16_t reserved;                /**< For 32-bit alignment. */
};
/* Type definition for #csd_aud_pp_vol_digital_master_gain_t. */
typedef struct csd_aud_pp_vol_digital_master_gain csd_aud_pp_vol_digital_master_gain_t;

/** @brief Sets the digital stereo gain. */
struct csd_aud_pp_vol_digital_stereo_gain
{
    uint16_t left_ch_gain_digital;   /**< left channel gain in q_factor specified format. */
    uint16_t right_ch_gain_digital;  /**< right channel gain in q_factor specified format. */
};
/* Type definition for #csd_aud_pp_vol_digital_stereo_gain_t. */
typedef struct csd_aud_pp_vol_digital_stereo_gain csd_aud_pp_vol_digital_stereo_gain_t;

/** @brief Sets the digital multichannel gain entry. */
struct csd_aud_pp_vol_digital_multi_channel_gain_entry
{
    uint16_t ch_type;          /**< Channel type. */
	uint16_t ch_gain_digital;  /**< channel gain in q_factor specified format. */
};
/* Type definition for #csd_aud_pp_vol_digital_multi_channel_gain_entry_t. */
typedef struct csd_aud_pp_vol_digital_multi_channel_gain_entry csd_aud_pp_vol_digital_multi_channel_gain_entry_t;

/** @brief Sets the digital multichannel gain. */
struct csd_aud_pp_vol_digital_multi_channel_gain
{
	uint32_t num_channels;     /**< Number of channels. */

    /** Digital gain array. */
	struct csd_aud_pp_vol_digital_multi_channel_gain_entry* entries;
};
/* Type definition for #csd_aud_pp_vol_digital_multi_channel_gain_t. */
typedef struct csd_aud_pp_vol_digital_multi_channel_gain csd_aud_pp_vol_digital_multi_channel_gain_t;

/*-------------------------------------------------------------------------*/
/*  digital volume control filter */
/*-------------------------------------------------------------------------*/
/**	Sets the digital master volume control.

  @payload
  #csd_aud_pp_vol_digital_master_gain

  @latexonly \input{tables/CSD_AUD_PP_VOL_DIGITAL_PARAM_ID_MASTER_GAIN.tex} @endlatexonly

  @return
  None.

  @dependencies
  None.
*/
#define CSD_AUD_PP_VOL_DIGITAL_PARAM_ID_MASTER_GAIN        0x00012EC0

/** Sets the digital stereo gain ID.

  @payload
  #csd_aud_pp_vol_digital_stereo_gain

  @latexonly \input{tables/CSD_AUD_PP_VOL_DIGITAL_PARAM_ID_STEREO_GAIN.tex} @endlatexonly

  @return
  None.

  @dependencies
  None.
*/
#define CSD_AUD_PP_VOL_DIGITAL_PARAM_ID_STEREO_GAIN        0x00012EC1

/** Sets the digital multichannel volume control.

  @payload
  #csd_aud_pp_vol_digital_multi_channel_gain

  @latexonly \input{tables/CSD_AUD_PP_VOL_DIGITAL_PARAM_ID_MULTI_CHANNEL_GAIN.tex} @endlatexonly

  @return
  None.

  @dependencies
  None.
*/
#define CSD_AUD_PP_VOL_DIGITAL_PARAM_ID_MULTI_CHANNEL_GAIN 0x00012EC2

/**
  @brief contains digital volume control commands.
*/
struct csd_aud_pp_vol_digital {

    uint32_t param_id;    /**< ID of the digital volume control parameter. */

    /** Pre/postprocessing digital volume control. */
	union {
        /** Digital master gain control. */
        struct csd_aud_pp_vol_digital_master_gain        master_gain;
        /** Digital stereo gain control. */
        struct csd_aud_pp_vol_digital_stereo_gain        stereo_gain;
        /** Digital multichannel gain control. */
        struct csd_aud_pp_vol_digital_multi_channel_gain multi_ch_gain;
	} u;
};
/* Type definition for #csd_aud_pp_vol_digital_t. */
typedef struct csd_aud_pp_vol_digital csd_aud_pp_vol_digital_t;

/**
   @name Soft Pause/Resume Enable Value
   @{
*/
#define CSD_AUD_PP_SPR_DISABLE         	0   /**< Soft pause/resume disable. */
#define CSD_AUD_PP_SPR_ENABLE           1   /**< Soft pause/resume enable. */
/** @} */ /* end_name Soft Pause/Resume Enable Value */

/**
   @name Soft Pause/Resume Update Params Value
   @{
*/
#define CSD_AUD_PP_SPR_USE_DEFAULT	            0   /**< Use default ACDB settings. */
#define CSD_AUD_PP_SPR_UPDATE_PARAMS            1   /**< Use client specified settings. */
/** @} */ /* end_name Soft Pause/Resume Update Params Value */

/**
   @name Soft Pause/Resume Ramping Curve Predefined Types
   @{
*/
#define CSD_AUD_PP_SPR_RAMPING_CURVE_LINEAR         0   /**< Linear ramping curve. */
#define CSD_AUD_PP_SPR_RAMPING_CURVE_EXP            1   /**< Exponential ramping curve. */
#define CSD_AUD_PP_SPR_RAMPING_CURVE_LOG            2   /**< Logarithmic ramping curve. */
/** @} */ /* end_name Soft Pause/Resume Ramping Curve Predefined Types */

/**
    @brief Contains the soft pause/resume commands.
*/
struct csd_aud_pp_spr {

    uint32_t enable;                            /**< Enable flag:\n
                               	   	   	   	   	   	 - 0 -- Disable.
                               	   	   	   	   	   	 - 1 -- Enable. */
    uint32_t update_params;                     /**< Flag to indicate whether client settings are 
													 used for period, step and ramping curve. 
													 Supported Values: 
                               	   	   	   	   	   	 	- 0 -- Use default settings in ACDB.
                               	   	   	   	   	   	 	- 1 -- Use client specified settings in this structure. */
    uint32_t period;                            /**< Period in milliseconds.
                                                     Supported values: 0 to 15000 */
    uint32_t step_size;                         /**< Step in microseconds.
                                                     Supported values: 0 to 15000000 */
    uint32_t ramping_curve;                     /**< Predefined ramping curve type. 
													 Supported values: CSD_AUD_PP_SPR_RAMPING types. */
};
/* Type definition for #csd_aud_pp_spr_t. */
typedef struct csd_aud_pp_spr csd_aud_pp_spr_t;

/*-------------------------------------------------------------------------*/
/*  eq control filter*/
/*-------------------------------------------------------------------------*/

/**	Payload for the equalizer enable command. This equalizer enables param_id. 

  @payload
  #csd_aud_pp_eq_enable

  @latexonly \input{tables/CSD_AUD_PP_EQ_PARAM_ID_ENABLE.tex} @endlatexonly

  @return
  None.

  @dependencies
  None.
*/
#define CSD_AUD_PP_EQ_PARAM_ID_ENABLE                   0x00012704
/** 
   @name Equalizer Filter Type
   @{ 
*/ 
#define CSD_AUD_PP_EQ_FILTER_TYPE_NONE					0
  /**< Equalizer filter type unknown. */
#define CSD_AUD_PP_EQ_FILTER_TYPE_BASS_BOOST            1
  /**< Equalizer filter type bass boost. */
#define CSD_AUD_PP_EQ_FILTER_TYPE_BASS_CUT              2
  /**< Equalizer filter type bass cut. */
#define CSD_AUD_PP_EQ_FILTER_TYPE_TREBLE_BOOST          3
  /**< Equalizer filter type treble boost. */
#define CSD_AUD_PP_EQ_FILTER_TYPE_TREBLE_CUT            4
  /**< Equalizer filter type treble cut. */
#define CSD_AUD_PP_EQ_FILTER_TYPE_BAND_BOOST            5
  /**< Equalizer filter type band boost. */
#define CSD_AUD_PP_EQ_FILTER_TYPE_BAND_CUT              6
  /**< Equalizer filter type band cut. */
/** @} */ /* end_name EQ Filter Type */

/**	Maximum subband number for the equalizer filter. */
#define CSD_AUD_PP_EQ_SUB_BAND_MAX_NUM    12 

/** @brief Defines the equalizer band structure. */
struct csd_aud_pp_eq_subband
{
    uint32_t	band_idx;           /**< Band index. */
    uint32_t	filter_type;        /**< Equalizer filter type. */
    uint32_t	center_freq_in_hz;  /**< Filter band center frequency. */
    int32_t     filter_gain;        /**< Filter band initial gain (dB). 
                                         Supported values: +12 dB to -12 dB 
                                         with 1-dB increments. */
    int32_t		lq_factor;          /**< Filter band quality factor expressed 
                                         as a q-8 number (i.e., fixed point 
                                         number with a q factor of 8 
                                         (e.g., 3000/(2^8)). */
};
/* Type definition for #csd_aud_pp_eq_subband_t. */
typedef struct csd_aud_pp_eq_subband csd_aud_pp_eq_subband_t;

/** @brief Enables the equalizer feature. */
struct csd_aud_pp_eq_enable
{
    uint32_t enable;
      /**< Indicates whether to enable the equalizer:\n
           - 0 -- Disable.
           - Non-zero -- Enable. */    

    uint32_t num_bands;
      /**< Number of bands. Maximum value: 12. */

    /** Equalizer band information list. */
    struct csd_aud_pp_eq_subband bands[CSD_AUD_PP_EQ_SUB_BAND_MAX_NUM];
};
/* Type definition for #csd_aud_pp_eq_enable_t. */
typedef struct csd_aud_pp_eq_enable csd_aud_pp_eq_enable_t;

/**
  @brief Contains the equalizer filter commands.
*/
struct csd_aud_pp_eq {
    uint32_t param_id;    /**< ID of the parameter to be enabled by the 
                               equalizer. */

    /** Equalizer control. */
	union {
        /** Enables the EQ command. */
		struct csd_aud_pp_eq_enable enable;	
	}u;
};
/* Type definition for #csd_aud_pp_eq_t. */
typedef struct csd_aud_pp_eq csd_aud_pp_eq_t;
/*-------------------------------------------------------------------------*/
/*  qcpr filter configurations */
/*-------------------------------------------------------------------------*/

/** Enables the QCPR filter. 

  @payload 
  #csd_aud_pp_qcpr_config

  @latexonly \input{tables/CSD_AUD_PP_QCPR_PARAM_ID_ENABLE.tex} @endlatexonly

  @return
  None.

  @dependencies
  None.
*/
#define CSD_AUD_PP_QCPR_PARAM_ID_ENABLE                 0x0001270C

/**	Configures the QCPR filter.

  @payload 
  #csd_aud_pp_qcpr_config

  @latexonly \input{tables/CSD_AUD_PP_QCPR_PARAM_ID_ENABLE.tex} @endlatexonly

  @return
  None. 

  @dependencies
  None.
*/ 
#define CSD_AUD_PP_QCPR_PARAM_ID_CONFIG                 0x00012311

/**	Disables the QCPR filter. 

  @payload 
  None. 

  @return
  None. 

  @dependencies
  None.
*/ 
#define CSD_AUD_PP_QCPR_PARAM_ID_DISABLE				0x00012312
/** 
   @name QCPR Preset Type
   @{ 
*/ 
#define CSD_AUD_PP_QCPR_PRESET_MEDIUMROOM			1
  /**< Medium room QCPR preset. */
#define CSD_AUD_PP_QCPR_PRESET_MEDIUMHALL           2
  /**< Medium hall QCPR preset. */
#define CSD_AUD_PP_QCPR_PRESET_CONCERTHALL          3
  /**< Concert hall QCPR preset. */ 
#define CSD_AUD_PP_QCPR_PRESET_SURROUND             4
  /**< Surround QCPR preset. */ 
#define CSD_AUD_PP_QCPR_PRESET_WARMSTAGE            5
  /**< Warm stage QCPR preset.*/ 
#define CSD_AUD_PP_QCPR_PRESET_CRYSTAL              6
  /**< Crystal QCPR preset. */ 
#define CSD_AUD_PP_QCPR_PRESET_LIVINGROOM           7
  /**< Living room QCPR preset. */
/** @} */ /* end_name QCPR Preset Type */
/**
   @name QCPR Strength
   @{ 
*/ 
#define CSD_AUD_PP_QCPR_STRENGTH_TWO				1
  /**< QCPR strength two. */  
#define CSD_AUD_PP_QCPR_STRENGTH_THREE              2
  /**< QCPR strength three. */ 
#define CSD_AUD_PP_QCPR_STRENGTH_FOUR               3
  /**< QCPR strength four. */ 
#define CSD_AUD_PP_QCPR_STRENGTH_FIVE               4
  /**< QCPR strength five. */ 
#define CSD_AUD_PP_QCPR_STRENGTH_SIX                5
  /**< QCPR strength six. */ 
#define CSD_AUD_PP_QCPR_STRENGTH_SEVEN              6
  /**< QCPR strength seven. */ 
#define CSD_AUD_PP_QCPR_STRENGTH_EIGHT              7
  /**< QCPR strength eight. */ 
#define CSD_AUD_PP_QCPR_STRENGTH_NINE               8
  /**< QCPR strength nine. */ 
#define CSD_AUD_PP_QCPR_STRENGTH_TEN                9
  /**< QCPR strength ten. */ 
/** 
   @}
*/
/**	@brief Configures the QCPR parameters. */
struct csd_aud_pp_qcpr_config
{
	uint32_t preset;     /**< Specifies the preset value for the QCPR. */
	uint32_t strength;   /**< Specifies the strength value for the QCPR. */
};
/* Type definition for #csd_aud_pp_qcpr_config_t.*/
typedef struct csd_aud_pp_qcpr_config csd_aud_pp_qcpr_config_t;

/** 
  @brief Contains command arrays for the QCPR.
*/
struct csd_aud_pp_qcpr 
{
    uint32_t param_id;     /**< ID of the QCPR command parameter. */

    /** Pre/postprocessing QCPR control.*/
	union {
        /** Enables the QCPR command. */
		struct csd_aud_pp_qcpr_config enable;
        /** Configures the QCPR. */
		struct csd_aud_pp_qcpr_config config;	
	}u;
};
/* Type definition for #csd_aud_pp_qcpr_t.*/
typedef struct csd_aud_pp_qcpr csd_aud_pp_qcpr_t;
/*-------------------------------------------------------------------------*/
/* SPA filter configurations */
/*-------------------------------------------------------------------------*/
/** Enables the spectrum analyzer. 

  @payload 
  #csd_aud_pp_spa_config 

  @latexonly \input{tables/CSD_AUD_PP_SPA_PARAM_ID_ENABLE.tex} @endlatexonly

  @return
  None.

  @dependencies
  None.
*/
#define CSD_AUD_PP_SPA_PARAM_ID_ENABLE                   0x00012301

/** Configuration parameters for the spectrum analyzer setup. 

  @payload 
  #csd_aud_pp_spa_config 

  @latexonly \input{tables/CSD_AUD_PP_SPA_PARAM_ID_ENABLE.tex} @endlatexonly

  @return
  None.

  @dependencies
  None.
*/
#define CSD_AUD_PP_SPA_PARAM_ID_CONFIG                   0x00012302

/** Disables the spectrum analyzer.
 
  @payload
  None. 

  @return
  None. 

  @dependencies
  None.
*/ 
#define CSD_AUD_PP_SPA_PARAM_ID_DISABLE					 0x00012303
  

/**
   @name SPA Sample Point Type
   @{ 
*/ 
#define CSD_AUD_PP_SPA_SAMPLEPOINT_32			   32
  /**< SPA sample point 32. */
#define CSD_AUD_PP_SPA_SAMPLEPOINT_64              64
  /**< SPA sample point 64. */
#define CSD_AUD_PP_SPA_SAMPLEPOINT_128             128
  /**< SPA sample point 128. */
#define CSD_AUD_PP_SPA_SAMPLEPOINT_256             256
  /**< SPA sample point 256. */
/** @} */  /* end_name SPA Sample Point Type */

/**	@brief Configures the spectrum analyzer parameters. */
struct csd_aud_pp_spa_config
{
    uint32_t sample_interval; /**< Sample interval. Supported values: 
                                   @ge 512. */
    /* Tech Pubs: @ge displays in the PDF as the greater-than-or-equal-to symbol */

    uint16_t sample_points;	  /**< Specifies the sample points for the SPA 
                                   filter. Supported values: 32, 64, 128, and 
                                   256. */
};
/* Type definition for #csd_aud_pp_spa_config_t. */
typedef struct csd_aud_pp_spa_config csd_aud_pp_spa_config_t;

/**
  @brief Contains the command arrays for the spectrum analyzer.
*/
struct csd_aud_pp_spa
{
    uint32_t param_id;   
      /**< Parameter ID. */

    /** Pre/postprocessing SPA control. */
	union {
        /** Enables the spectrum analyzer command. */
		struct csd_aud_pp_spa_config enable;	
        /** Configures the spectrum analyzer parameters. */  
		struct csd_aud_pp_spa_config config;	
	}u;
};
/* Type definition for #csd_aud_pp_spa_t. */
typedef struct csd_aud_pp_spa csd_aud_pp_spa_t;
/*-------------------------------------------------------------------------*/
/*  TSM filter configurations */
/*-------------------------------------------------------------------------*/

/** Enables/disables a time scale modification filter. 

  @payload
  #csd_aud_pp_tsm_enable

  @latexonly \input{tables/CSD_AUD_PP_TSM_PARAM_ID_ENABLE.tex} @endlatexonly

  @return
  None.

  @dependencies
  None.
*/
#define CSD_AUD_PP_TSM_PARAM_ID_ENABLE                   0x0001270A

/** Configures time scale modification parameters. 

  @payload
  #csd_aud_pp_tsm_config

  @latexonly \input{tables/CSD_AUD_PP_TSM_PARAM_ID_CONFIG.tex} @endlatexonly

  @return
  None.

  @dependencies
  None.
*/
#define CSD_AUD_PP_TSM_PARAM_ID_CONFIG                   0x0001270B

/** @brief Enables the time scale modification command. */
struct csd_aud_pp_tsm_enable
{
    uint32_t enable;        /**< Enables/disables the time scale modification 
                                 feature.\n
                                 - Non-zero -- Enabled.
                                 - 0 -- Disabled.  */
    uint16_t tsm_factor;    /**< Time scale modification factor in Q11. 
                                 Supported values: 1024 to 16384. */
};
/* Type definition for #csd_aud_pp_tsm_enable_t. */
typedef struct csd_aud_pp_tsm_enable csd_aud_pp_tsm_enable_t;

/**	@brief Configures the time scale modification filter. */
struct csd_aud_pp_tsm_config
{
    uint16_t tsm_factor;    /**< Time scale modification factor in Q11. 
                                 Supported values: 1024 to 16384. */
};
/* Type definition for #csd_aud_pp_tsm_config_t. */
typedef struct csd_aud_pp_tsm_config csd_aud_pp_tsm_config_t;

/**
  @brief Contains the command arrays for time scale modification.
*/
struct csd_aud_pp_tsm 
{
    uint32_t param_id;
    /**< Specifies the TSM control command ID. */

    /** Pre/postprocessing time scale modification control. */
	union {
        /** Enables the time scale modification command. */
		struct csd_aud_pp_tsm_enable enable;
        /** Configures time scale modification. */  
		struct csd_aud_pp_tsm_config config;
	}u;
};
/* Type definition for #csd_aud_pp_tsm_t. */
typedef struct csd_aud_pp_tsm csd_aud_pp_tsm_t;

/** @} */ /* end_addtogroup csd_audiopp */

#endif /* _CSD_AUD_PP_H_ */

