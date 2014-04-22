#ifndef _CSD_DEV_IOCTL_H_
#define _CSD_DEV_IOCTL_H_
/**
  @file      csd_dev_ioctl.h
    @brief
  Device management IOCTLs
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
        when        who      what, where, why
      --------     -----     ---------------------------------------------------
      08/27/2015   jk        Added support for 32khz sample rate
      02/10/2014   jk        Added support for CSD_DEV_CMD_RESTART  
      07/09/13     unni      Correct wrong GUIDs, Indentations
      02/04/13     rp        Added support for AANC device
      10/17/12     rp        Increased the number of max device entries to 8
      06/04/12     sd       (Tech Pubs) Edited/added Doxygen markup and comments.
      08/18/11     jk        added command CSD_DEV_CMD_CONNECT_DEV used to attach 
                             two devices 
      03/15/11     sd       (Tech Pubs) Edited/added Doxygen markup and comments.
      10/25/10     jzhou     Doxygen comments update
      08/17/10     jzhou     Doxygen comments update
      05/12/10     suns       initial draft 

=============================================================================*/
#include "mmdefs.h"
#include "csd_dev_mbhc_ioctl.h"

/** @addtogroup csd_dev
  @{
*/

/**
   @name Device Sample Rate
   @{ 
*/ 
#define CSD_DEV_SR_UNKNOWN          ( 0 )
  /**< Unknown sample rate. */
#define CSD_DEV_SR_8000             ( 8000 )
  /**< Device sample rate -- 8 kHz.    */
#define CSD_DEV_SR_16000            ( 16000 )
  /**< Device sample rate -- 16 kHz.   */
#define CSD_DEV_SR_32000            ( 32000 )
  /**< Device sample rate -- 32 kHz.   */
#define CSD_DEV_SR_48000            ( 48000 )
  /**< Device sample rate -- 48 kHz.   */
#define CSD_DEV_SR_96000            ( 96000 )
  /**< Device sample rate -- 96 kHz.   */
#define CSD_DEV_SR_192000           ( 192000 )
  /**< Device sample rate -- 192 kHz.   */
/** @} */ /* end_name Device Sample Rate */

/**
   @name Device Bit Per Sample
   @{ 
*/ 
#define CSD_DEV_BPS_UNKNOWN         ( 0  )
  /**< Unknown bits per sample. */
#define CSD_DEV_BPS_16              ( 16 )
  /**< 16 bits per sample. */
#define CSD_DEV_BPS_24              ( 24 )
  /**< 24 bits per sample. */
/** @} */ /* end_name Device Bit Per Sample */
/**
   @name Device Sample Rate Mask
   @{ 
*/ 
#define CSD_DEV_SR_MASK_8000        (0x00000001) /**< 8 kHz frequency.  */
#define CSD_DEV_SR_MASK_11025       (0x00000002) /**< 11.025 kHz frequency. */
#define CSD_DEV_SR_MASK_12000       (0x00000004) /**< 12 kHz frequency. */
#define CSD_DEV_SR_MASK_16000       (0x00000008) /**< 16 kHz frequency. */
#define CSD_DEV_SR_MASK_22050       (0x00000010) /**< 22.05 kHz frequency. */
#define CSD_DEV_SR_MASK_24000       (0x00000020) /**< 24 kHz frequency. */
#define CSD_DEV_SR_MASK_32000       (0x00000040) /**< 32 kHz frequency. */
#define CSD_DEV_SR_MASK_44100       (0x00000080) /**< 44.1 kHz frequency. */
#define CSD_DEV_SR_MASK_48000       (0x00000100) /**< 48 kHz frequency. */
#define CSD_DEV_SR_MASK_96000       (0x00000200) /**< 96 kHz frequency. */
#define CSD_DEV_SR_MASK_192000      (0x00000400) /**< 192 kHz frequency. */
/** @} */ /* end_name Device Sample Rate Mask */
/**
   @name Device Sample Width Mask
   @{ 
*/ 
#define CSD_DEV_BPS_MASK_16          (0x00000001) /**< 16 bits per sample. */
#define CSD_DEV_BPS_MASK_24          (0x00000002) /**< 24 bits per sample. */
#define CSD_DEV_BPS_MASK_32          (0x00000004) /**< 32 bits per sample. */
/** @} */ /* end_name Device Sample Width Mask */
/*-------------------------------------------------------------------------*/
/* IOCTL Command */
/*-------------------------------------------------------------------------*/

/** Device control IOCTL command start. */
#define CSD_DEV_CMD_START                        0x00011C00

/** Enables an audio device.

  @payload 
  #csd_dev_enable

  @return
  None.

  @dependencies
  None.
*/
#define CSD_DEV_CMD_ENABLE                       0x00011C01

/** Disables one or two audio devices. 

  @payload 
  #csd_dev_disable

  @latexonly \input{tables/CSD_DEV_CMD_DISABLE.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_DEV_CMD_DISABLE                      0x00011C02

/** Enables the AFE loopback on an Rx and a Tx device. 

  @payload 
  #csd_dev_afe_loopback

  @latexonly \input{tables/CSD_DEV_CMD_AFE_LOOPBACK.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_DEV_CMD_AFE_LOOPBACK                 0x00011C03

/** Enables/disables active noise cancellation on an Rx device. 

  @payload 
  #csd_cmd_dev_anc

  @latexonly \input{tables/CSD_DEV_CMD_ANC_CONTROL.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_DEV_CMD_ANC_CONTROL                  0x00011C04

/** Enables/disables companding on an Rx device. 

  @payload 
  #csd_dev_companding

  @latexonly \input{tables/CSD_DEV_CMD_COMPANDING_CONTROL.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_DEV_CMD_COMPANDING_CONTROL           0x00011C05

/** Gets the maximum number of supported devices. 

  @payload 
  #csd_dev_max_dev_nums

  @latexonly \input{tables/CSD_DEV_CMD_GET_MAX_DEV_NUMS.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_DEV_CMD_GET_MAX_DEV_NUMS             0x00011C06

/** Gets the full list of device capabilities.

  @payload 
  #csd_dev_caps

  @latexonly \input{tables/CSD_DEV_CMD_GET_DEV_CAPS.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_DEV_CMD_GET_DEV_CAPS                 0x00011C07

/** Enables/disables DTMF on the device. 

  @payload 
  #csd_dev_dtmf_ctrl

  @latexonly \input{tables/CSD_DEV_CMD_DTMF_CONTROL.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_DEV_CMD_DTMF_CONTROL                 0x00011C08

/** Enables/disables sidetone on the Rx/Tx device pair.

  @payload 
  #csd_dev_sidetone_ctrl

  @latexonly \input{tables/CSD_DEV_CMD_SIDETONE_CONTROL.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_DEV_CMD_SIDETONE_CONTROL             0x00011C09

/** Device configuration from the client for the Proxy port device type.

  @payload 
  #csd_dev_configure

  @latexonly \input{tables/CSD_DEV_CMD_CONFIGURE.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_DEV_CMD_CONFIGURE                    0x00011C0A

/** GUID header for the RT Proxy port configuration type.

  @payload 
  #csd_dev_rt_proxy_port_cfg

  @latexonly \input{tables/CSD_DEV_CFG_PROXY_PORT_HEADER.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  This header is used in association with #CSD_DEV_CMD_CONFIGURE to indicate 
  RT Proxy port device type configuration.
*/
#define CSD_DEV_CFG_PROXY_PORT_HEADER            0x00011C0B

/** GUID header to configure multiple channel output for the HDMI device.

  @payload 
  #csd_dev_hdmi_multi_channel_cfg

  @latexonly \input{tables/CSD_DEV_CMD_HDMI_MULTI_CHANNEL_CONFIG.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  This command must be issued before sending #CSD_DEV_CMD_ENABLE to the HDMI device. 
*/
#define CSD_DEV_CMD_HDMI_MULTI_CHANNEL_CONFIG    0x00011C0C

/** Enables/disables Adaptive active noise cancellation on rx
    device.

  @payload 
  #csd_cmd_dev_aanc

  @latexonly \input{tables/CSD_DEV_CMD_AANC_CONTROL.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/

#define CSD_DEV_CMD_AANC_CONTROL                 0x00011C0D

/** Control MBHC on the device. 

  @payload 
  #csd_dev_mbhc_config

  @latexonly \input{tables/CSD_DEV_CMD_CONFIG_MBHC_PARAMS.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_DEV_CMD_CONFIG_MBHC_PARAMS           0x00011C0E

/** Registers the callback function for the device.

  @payload 
  #csd_dev_cb

  @latexonly \input{tables/CSD_DEV_CMD_SET_EVT_CB.tex} @endlatexonly

  @return
  CSD status code.

  @dependencies
  None.
*/
#define CSD_DEV_CMD_SET_EVT_CB                   0x00011C0F

/** Connects two device together one as a source and one as a
    sink.

  @payload 
  #csd_dev_connect_device_t

  @latexonly \input{tables/CSD_DEV_CMD_CONNECT_DEVICE.tex} 
             @endlatexonly
  @par 
  #csd_dev_pair_t.

  @latexonly \input{tables/CSD_DEV_CMD_CONNECT_DEVICE_2.tex} 
             @endlatexonly

  @return
  CSD status code.

  @dependencies
  Both devices specified are enabled.
*/
#define CSD_DEV_CMD_CONNECT_DEVICE               0x00011C10

/** Device control IOCTL command end. */
#define CSD_DEV_CMD_END                          0x00011FFF

/** Constant for device ID entries. One for Rx, and one for Tx.
    other are for EC reference devices, incall recording etc
*/


/** Reconfigures an audio device sampling rate without stopping
    the mclk and WS.

  @payload 
  #csd_cmd_dev_restart

  @return
  CSD status code.

  @dependencies
  Device passed must be enabled 
*/
#define CSD_DEV_CMD_RESTART                 0x00013077

#define CSD_DEVICE_MAX_ENTRIES    8
/*-------------------------------------------------------------------------*/
/* Payload for IOCTL command */
/*-------------------------------------------------------------------------*/
/**  @brief Structure for configuring the rt_proxy_port device type. */
struct csd_dev_rt_proxy_port_cfg
{
  uint32_t                    cfg_hdr;
    /**< GUID header to configure the structure type.*/
  
  uint32_t                  num_channels;
  /**< Number of channels.*/

  uint16_t                  interleaved;
  /**< Data exchanged between the AFE and RT ports is interleaved.*/

  uint16_t                  frame_size;
 /**< Maximum transaction buffer size in bytes, including all channels.*/

  uint16_t                  jitter_allowance;
  /**< Configures the amount of jitter for the port to allow.
  For example, if @pm 10 msec of jitter is anticipated in the timing of sending
  frames to the port, and the configuration is 16 kHz Mono, 16-bit samples,
  this field should be 10 msec * 16 samples/msec * 2 bytes/sample = 320. */
    /* Tech Pubs: @pm displays in the PDF as the plus-or-minus symbol. */

  uint16_t                  low_water_mark;
   /**< Low water mark in bytes, including all channels.*/

  uint16_t                  high_water_mark;
  /**< High water mark in bytes, including all channels.*/
};
/* Type definition for #csd_dev_rt_proxy_port_cfg_t. */
typedef struct csd_dev_rt_proxy_port_cfg csd_dev_rt_proxy_port_cfg_t;

/**  @brief Device attributes. */
struct csd_dev_attrib
{
  uint32_t sample_rate;        /**< Sample rate. */
  uint32_t bits_per_sample;    /**< Number of bits per sample 
                                    (e.g., 16, 24, and 32). */
};
/* Type definition for #csd_dev_attrib_t. */
typedef struct csd_dev_attrib csd_dev_attrib_t;

/** @brief Device information. */
struct csd_dev_entry
{
  uint32_t dev_id;                    /**< Device ID. */
  struct csd_dev_attrib dev_attrib;   /** Device attributes. */
};
/* Type definition for #csd_dev_entry_t. */
typedef struct csd_dev_entry csd_dev_entry_t;

/**
  @brief Enables the CSD device.
*/
struct csd_dev_enable
{
  uint32_t num_devs;             /**< Number of devices to be enabled. */

  /** Array of the device information for the devices to be enabled. */
  struct csd_dev_entry devs[CSD_DEVICE_MAX_ENTRIES];
};
/* Type definition for #csd_dev_enable_t. */
typedef struct csd_dev_enable csd_dev_enable_t;

/** @brief Union for individual device configuration parameter information. */
union csd_dev_cfg_param
{
  uint32_t cfg_hdr;
  /**< GUID header for configuration structure type.*/
  struct csd_dev_rt_proxy_port_cfg rt_proxy_port_cfg;
  /**< Structure for configuring the rt_proxy_port device type. */
};
/* Type definition for #csd_dev_cfg_param_t. */
typedef union csd_dev_cfg_param csd_dev_cfg_param_t;

/** @brief Structure for device configuration parameter information. */
struct csd_dev_config_param
{
  uint32_t dev_id;          
  /**< Device ID. */
  csd_dev_cfg_param_t  csd_dev_cfg;
  /**< Configuration structure for dev_id */

};
/* Type definition for #csd_dev_config_param_t. */
typedef struct csd_dev_config_param csd_dev_config_param_t;

/**  @brief  Structure for device configuration. */
struct csd_dev_configure
{
    uint32_t num_devs;          
    /**< Number of devices to be enabled. */
    struct csd_dev_config_param devs[CSD_DEVICE_MAX_ENTRIES];
    /**< Configuration parameter array for the number of devices. */
};
/* Type definition for #csd_dev_configure_t. */
typedef struct csd_dev_configure csd_dev_configure_t;

/**
  @brief Disables the device.
*/
struct csd_dev_disable
{
  uint32_t num_devs;
  /**< Number of devices to be disabled. Supported values: 1 and 2. */
  uint32_t dev_ids[CSD_DEVICE_MAX_ENTRIES];
  /**< Array of the device IDs to be disabled. Supported values: 0, 1, and 2. */
};
/* Type definition for #csd_dev_disable_t. */
typedef struct csd_dev_disable csd_dev_disable_t;
/**
   @name Device AFE Loopback Type
   @{ 
*/ 
#define CSD_DEV_AFE_LOOPBACK_ENABLE       1
  /**< Enables the audio front end loopback. */
#define CSD_DEV_AFE_LOOPBACK_DISABLE      0
  /**< Disables the audio front end loopback. */
#define CSD_DEV_AFE_LOOPBACKMODE_DEFAULT  1
  /**< Default setting for the audio front end loopback. */
/** @} */ /* end_name Device AFE Loopback Type */

/**
  @brief Sets the audio front end loopback.
*/
struct csd_dev_afe_loopback
{
  uint32_t tx_dev_id;  /**< Device ID for the recording (Tx) device. */
  uint32_t rx_dev_id;  /**< Device ID for the playback (Rx) device. */
  bool_t   enable;     /**< Indicates whether the audio front end is
                              enabled:\n
                            - 1 -- Enables the audio front end. (Default)
                            - 0 -- Disables the audio front end. */
  uint16_t afe_mode;   /**< AFE loopback mode. Default: 1; other values are
                            reserved. */
};
/* Type definition for #csd_dev_afe_loopback_t. */
typedef struct csd_dev_afe_loopback csd_dev_afe_loopback_t;
/**
   @name Device Connect Type
   @{ 
*/ 

/* Type definition for #csd_dev_pair_t. */
typedef struct csd_dev_pair_t csd_dev_pair_t;

/**
  @brief Pair of devices.
*/
struct csd_dev_pair_t
{
   uint32_t source_dev_id; /**< Device ID for the source device. */
   uint32_t sink_dev_id;   /**< Device ID for the sink device. */
};

/* Type definition for #csd_dev_connect_device_t. */
typedef struct csd_dev_connect_device_t csd_dev_connect_device_t;

/**
  @brief Connects two devices together.
*/
struct csd_dev_connect_device_t
{
  uint32_t num_pairs;
  /**< Number of pairs of devices to be connected together.*/
  bool_t connect_flag;  /**< Indicates whether the deviced are attached:\n
                        - TRUE -- Connected. (Default)
                        - FALSE  -- Disconnected. */
  csd_dev_pair_t  *pairs;
  /**< Pointer to an array of csd_dev_pair_t of the size num_pairs.*/
};

/**
   @name Device Codec ANC Command
   @{ 
*/ 
#define CSD_DEV_CODEC_ANC_DISABLE     0
  /**< Disables the active noise cancellation feature. */
#define CSD_DEV_CODEC_ANC_ENABLE      1 
  /**< Enables the active noise cancellation feature. */
/** @} */ /* end_name Device Codec ANC Command */

/**
  @brief Controls the active noise cancellation feature.
*/
struct csd_cmd_dev_anc
{
  uint32_t rx_dev_id;    /**< Playback (Rx) device ID. */
  bool_t enable;         /**< Indicates whether the active noise cancellation
                              feature is enabled:\n
                              - 1 -- Enables active noise cancellation.
                              - 0 -- Disables active noise cancellation. */
};
/* Type definition for #csd_cmd_dev_anc_t. */
typedef struct csd_cmd_dev_anc csd_cmd_dev_anc_t;
/**
   @name Device Codec Companding Type
   @{ 
*/ 
#define CODEC_COMP_DISABLE          0   /**< Disables companding. */ 
#define CODEC_COMP_ENABLE_STATIC    1   /**< Enables static companding. */ 
#define CODEC_COMP_ENABLE_DYNAMIC   2   /**< Enables dynamic companding. */
/** @} */ /* end_name Device Codec Companding Type */

/**
  @brief Controls the companding.
*/
struct csd_dev_companding
{
  uint32_t rx_dev_id;       /**< Playback (Rx) device ID. */
  uint32_t comp_options;    /**< Indicates the companding option: Disable,
                                  Static, or Dynamic. */
};
/* Type definition for #csd_dev_companding_t. */
typedef struct csd_dev_companding csd_dev_companding_t;

/**
  @brief Gets the active device number.
*/
struct csd_dev_max_dev_nums
{
  uint32_t  max_num_devs;   /**< Maximum number of supported devices in
                                  the CSD driver. */
};
/* Type definition for #csd_dev_max_dev_nums_t. */
typedef struct csd_dev_max_dev_nums csd_dev_max_dev_nums_t;

/** @brief Creates the capabilities record for a single device. */
struct csd_dev_caps_entry
{
  uint32_t  dev_id;         /**< Device ID. */
  uint32_t  sr_bitmask;     /**< Bitmask of sample rates supported. */
  uint32_t  bps_bitmask;    /**< Bitmask of bits per sample supported. */
};
/* Type definition for #csd_dev_caps_entry_t. */
typedef struct csd_dev_caps_entry csd_dev_caps_entry_t;

/**
  @brief Gets the device capabilities.
*/
struct csd_dev_caps
{
  uint32_t  num_devs;                 /**< Number of devices to query. */

    /** Pointer to the returned buffer that contains the full list of queried 
        device capabilities. */
  struct csd_dev_caps_entry *entries;   
};
/* Type definition for #csd_dev_caps_t. */
typedef struct csd_dev_caps csd_dev_caps_t;

/**
   @name DTMF High Frequency Type
   @{ 
*/ 
/** DTMF high frequency -- 1209 Hz. */
#define DTMF_HIGH_FREQ_1209      1209
    
/** DTMF high frequency -- 1336 Hz. */    
#define DTMF_HIGH_FREQ_1336      1336
    
/** DTMF high frequency -- 1477 Hz. */     
#define DTMF_HIGH_FREQ_1477      1477
  
/** DTMF high frequency -- 1633 Hz. */  
#define DTMF_HIGH_FREQ_1633      1633
/** @} */ /* end_name DTMF High Frequency Type */
/**
   @name DTMF Low Frequency Type
   @{ 
*/ 
/** DTMF low frequency -- 697 Hz. */
#define DTMF_LOW_FREQ_697        697
    
/** DTMF low frequency -- 770 Hz. */    
#define DTMF_LOW_FREQ_770        770  
  
/** DTMF low frequency -- 852 Hz. */  
#define DTMF_LOW_FREQ_852        852 
   
/** DTMF low frequency -- 941 Hz. */   
#define DTMF_LOW_FREQ_941        941    
/** @} */ /* end_name DTMF Low Frequency Type */
/**
   @name DTMF Gain Type
   @{ 
*/
#define DTMF_DEFAULT_GAIN        0x4000 
  /**< DTMF gain values and Q13 value. */
/** @} */ /* end_name DTMF Gain Type */

/**
  @brief Controls DTMF generation.
*/
struct csd_dev_dtmf_ctrl
{
  int64_t dtmf_duration_in_ms;
  /**< Duration of the DTMF tone in milliseconds. The value must be @ge -1.
       Supported values:\n
       - -1 -- Continuous DTMF of infinite duration.
       - 0 -- Stops a continuous DTMF (if it was started).
       - > 0 -- Duration in milliseconds. */
  /* Tech Pubs: @ge displays in the PDF as the greater-than-or-equal-to symbol*/

  uint16_t dtmf_high_freq;
  /**< DTMF high tone frequency. Supported values: 100 to 4000 Hz.*/
  uint16_t dtmf_low_freq;
  /**< DTMF low tone frequency. Supported values: 100 to 400 Hz. */
  uint16_t dtmf_gain;
  /**< DTMF volume setting: Q13 gain values.  */
  uint16_t num_devs;
  /**< Number of devices to enable/disable DTMF. */
  uint32_t* p_dev_ids;
  /**< Pointer to a list of device IDs that must be enabled/disabled for DTMF.*/
};
/* Type definition for #csd_dev_dtmf_ctrl_t. */
typedef struct csd_dev_dtmf_ctrl csd_dev_dtmf_ctrl_t;

/**
   @name Device Sidetone Type
   @{ 
*/ 
#define CSD_DEV_SIDETONE_DEFAULT  0  /**< Default setting for the sidetone. */
#define CSD_DEV_SIDETONE_ENABLE   1  /**< Enables the sidetone. */
#define CSD_DEV_SIDETONE_DISABLE  2  /**< Disables the sidetone. */

/** @} */ /* end_name Device Sidetone Type  */

/**
  @brief Payload for #CSD_DEV_CMD_SIDETONE_CONTROL. 
*/
struct csd_dev_sidetone_ctrl
{
  uint32_t ctrl;      /**< Command ID for the sidetone control.
                                     Supported values:\n
                                     - 0 (Default)
                                     - 1 -- Enables the sidetone.
                                     - 2 -- Disables the sidetone. */
  uint32_t rx_dev_id;    /**< Device ID for Rx path. */
  uint32_t tx_dev_id;    /**< Device ID for Tx path. */
};
/* Type definition for #csd_dev_sidetone_ctrl_t. */
typedef struct csd_dev_sidetone_ctrl csd_dev_sidetone_ctrl_t;

/**
   @name HDMI Multi-Channel Data Type
   @{ 
*/ 
/** HDMI linear data type. */
#define CSD_DEV_HDMI_DATA_TYPE_LINEAR      0x0  
  
/** HDMI non-linear data type. */  
#define CSD_DEV_HDMI_DATA_TYPE_NON_LINEAR  0x1    
/** @} */ /* end_name HDMI Multi-Channel Data Type  */

/**
    @brief
  Payload for #CSD_DEV_CMD_HDMI_MULTI_CHANNEL_CONFIG.
*/
struct csd_dev_hdmi_multi_channel_cfg
{
  uint32_t dev_id;            /**< Device ID for the HDMI device. */
  uint16_t num_channels;      /**< Number of channels to be set up.
                                       Supported values: 2 to 8. */
  uint16_t data_type;          /**< Data type for the HDMI multi-channel.
                                     Supported values:\n
                                     - 0 -- Linear format.
                                     - 1 -- Non-linear format.  */
  uint16_t channel_allocation; /**< Channel allocation value defined in
                                                the HDMI standard (CEA-861-D).                                        .
                                   Supported values:\n
                                   - 0x0 -- 0x1f.
                                   - 0 -- Stereo. */
};
/* Type definition for #csd_dev_hdmi_multi_channel_cfg_t. */
typedef struct csd_dev_hdmi_multi_channel_cfg csd_dev_hdmi_multi_channel_cfg_t;

/** @} */ /* end_addtogroup csd_dev */

/**
   @name Device afe AANC Command
   @{ 
*/ 
enum  csd_dev_afe_aanc_ctrl_id
{
  CSD_DEV_AFE_AANC_DEFAULT = 0,          /**< Default setting for the AANC. */
  CSD_DEV_AFE_AANC_ENABLE  = 1,          /**< Enables the AFE AANC. */
  CSD_DEV_AFE_AANC_DISABLE = 2,          /**< Disables the AFE AANC. */
  CSD_DEV_AFE_AANC_INVALID = 0x7FFFFFFF  /* Invalid AANC control value */
};

typedef enum csd_dev_afe_aanc_ctrl_id  csd_dev_afe_aanc_ctrl_id_e;
/** @} */ /* end_name Device AFE ANC Command */

/**
  @brief Controls the adaptive active noise cancellation feature.
*/
struct csd_cmd_dev_aanc
{
  csd_dev_afe_aanc_ctrl_id_e  aanc_ctrl;
  /**< Command ID for the aanc control. Supported values:\n
     - CSD_DEV_AFE_AANC_DEFAULT (0) -- Default setting for the aanc.\n
     - CSD_DEV_AFE_AANC_ENABLE  (1) -- Enables the aanc.\n
     - CSD_DEV_AFE_AANC_DISABLE (2) -- Disables the aanc.
  */
  uint32_t tx_dev_id;    /**< Tx device ID. */
  uint32_t rx_dev_id;    /**< Rx device ID. */
  uint32_t ref_dev_id;   /**< Reference device ID. */
};
/* Type definition for #csd_cmd_dev_anc_t. */
typedef struct csd_cmd_dev_aanc csd_cmd_dev_aanc_t;

/** 
  @brief Payload for #CSD_DEV_CMD_CONFIG_MBHC_PARAMS.
*/
struct csd_dev_mbhc_config
{
  uint32_t mbhc_type;   /**< MBHC param ID. */

  /** MBHC control.*/
  union
  {
    /** set button detection mode. */
    struct csd_dev_mbhc_button_detection_mode  button_detection_mode;
  }u;
};
/* Type definition for #csd_dev_mbhc_config_t. */
typedef struct csd_dev_mbhc_config csd_dev_mbhc_config_t;

/** Callback function type for Device callback.

  @param[in] evnt_id     Event id.
  @param[in] data        Payload for the callback event.
  @param[in] data_len    Payload length.
  @param[in] client_data Client provided data returned in callback function.

  @return
  None.

  @dependencies
  None.
*/
typedef void (*csd_dev_cb_fn)(uint32_t    evnt_id,
                              void        *data,
                              uint32_t    data_len,
                              void        *client_data);

/**
  @brief defines the callback structure for CSD_DEV_CMD_SET_EVT_CB
*/
struct csd_dev_cb
{
  csd_dev_cb_fn        cb;     /**< Callback function provided by client. */
  void                 *data;  /**< Client data returned during callback. */
};

/** @brief Type definition for csd_dev_cb. */
typedef struct csd_dev_cb csd_dev_cb_t;

/**
  @brief type used for switching sample rates on an I2S device 
         without stopping mclk and WS.
*/
struct csd_cmd_dev_restart
{
   uint32_t tx_dev_id;    /**< Tx device ID. */
   uint32_t rx_dev_id;    /**< Rx device ID. */
   uint32_t sample_rate;   /**< new sample rate */
};
/* Type definition for #csd_cmd_dev_restart_t. */
typedef struct csd_cmd_dev_restart csd_cmd_dev_restart_t;


/** @} */ /* end_addtogroup csd_dev */


#endif /* _CSD_DEV_IOCTL_H_*/
