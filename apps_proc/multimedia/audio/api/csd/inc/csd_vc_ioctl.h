#ifndef __CSD_VC_IOCTL_H__
#define __CSD_VC_IOCTL_H__
/**
  @file  csd_vc_ioctl.h
  @brief Contains voice context IOCTL definitions. 
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
     Copyright (C) 2011-12 Qualcomm Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary
-----------------------------------------------------------------------------*/
/*=============================================================================
                      EDIT HISTORY FOR MODULE
    This section contains comments describing changes made to this module.
    Notice that changes are listed in reverse chronological order.

    $Header: //components/rel/audio.tx/2.0/audio/api/csd/inc/csd_vc_ioctl.h#1 $
    $DateTime: 2018/12/25 22:39:07 $
    $Author: pwbldsvc $
    $Change: 17929171 $
    $Revision: #1 $
when       who     what, where, why
--------   ---     ------------------------------------------------------------
05/30/14   rp      Added topology feature id
08/16/13   jy      Update the comment for CSD_VC_IOCTL_SET_DEVICE_CONFIG and 
                   CSD_VC_IOCTL_SET_DEVICE_CONFIG_V2 to explicitely allowing 
                   this being executed before CSD_DEV_CMD_ENABLE
03/01/13   ts      Added CSD_VC_IOCTL_SET_VOLUME_STEP API
11/16/12   jy      Added 32-bit compiler supoort for mem map version 2 and VPCM
                   version 2
07/17/12   jk      Added support for VPCM version 2 and mem map version 2 due 
                   to CVD changes
06/04/12   sd      (Tech Pubs) Edited/added Doxygen markup and comments.
03/01/12   gl      Remove deprecation tags version 1 of APIs as code base may not
                   support V2s
10/19/11   sr      Added Volume step and Mute_V2 APIs. 
08/19/11   jk       added CSD_VC_IOCTL_SET_DEVICE_CONFIG_V2 for
                    support of ec ref source
03/15/11   sd      (Tech Pubs) Edited/added Doxygen markup and comments.
=============================================================================*/

/** @addtogroup csd_vc
@{ */

/** 
  @name Voice Context IOCTL Commands
  @{
*/

/****************************************************************************
 * CSD VC CORE IOCTL COMMANDS                                               *
 ****************************************************************************/

/** 
  Sets the device configuration on the voice processing context.
  
  @payload
  #csd_vc_ioctl_set_device_config_t
  
  @latexonly \input{tables/CSD_VC_IOCTL_SET_DEVICE_CONFIG.tex} @endlatexonly

  @return
  #CSD_VC_EVENTID_COMMAND_RSP

  @dependencies
  The voice context must be disabled.
  
  @comments
  Client can choose to execute CSD_VC_IOCTL_SET_DEVICE_CONFIG before or after
  the devices are enabled by CSD_DEV_CMD_ENABLE.
*/
#define CSD_VC_IOCTL_SET_DEVICE_CONFIG ( 0x00011170 )

/**
  Sets the device configuration on the voice processing context.
   
  @payload
  #csd_vc_ioctl_set_device_config_v2_t
   
  @latexonly \input{tables/CSD_VC_IOCTL_SET_DEVICE_CONFIG_V2.tex} @endlatexonly

  @return
  #CSD_VC_EVENTID_COMMAND_RSP

  @dependencies
  The voice context must be disabled.
  
  @comments
  Client can choose to execute CSD_VC_IOCTL_SET_DEVICE_CONFIG_V2 before or after
  the devices are enabled by CSD_DEV_CMD_ENABLE.
*/
#define CSD_VC_IOCTL_SET_DEVICE_CONFIG_V2 ( 0x000112DA ) 

/**
  When attached to the Voice Manager module, the Voice 
  Processing module enters the Enable state after both the 
  client and the MVM trigger voice processing to be enabled.
  Otherwise, the Voice Processing module is enabled by the 
  client trigger.

  @payload
  uint32_t cmd_token: Transaction identifier provided by the client
  that allows the client to identify which command completed.\n
  @note1hang The upper 8 bits are reserved for internal
  processing.

  @return
  #CSD_VC_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VC_IOCTL_ENABLE ( 0x00011143 )

/**
  Disables the voice processing context.
 
  @payload
  uint32_t cmd_token: Transaction identifier provided by the client
  that allows the client to identify which command completed.\n
  @note1hang The upper 8 bits are reserved for internal
  processing.

  @return
  #CSD_VC_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VC_IOCTL_DISABLE ( 0x00011144 )

/****************************************************************************
 * CSD VC VOLUME/MUTE IOCTL COMMANDS                                        *
 ****************************************************************************/

/**
  Sets the Rx volume calibration based on the Rx volume index.
 
  @payload
  #csd_vc_ioctl_set_rx_volume_index_t
  
  @latexonly \input{tables/CSD_VC_IOCTL_SET_RX_VOLUME_INDEX.tex} @endlatexonly

  @return
  #CSD_VC_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VC_IOCTL_SET_RX_VOLUME_INDEX ( 0x00011145 )

/**
  Sets the maximum number of volume steps.
 
  @payload
  #csd_vc_ioctl_set_number_of_steps_t

  @return
  #CSD_VC_EVENTID_COMMAND_RSP

  @dependencies
  #CSD_VC_IOCTL_SET_NUMBER_OF_VOLUME_STEPS must be called after the volume
  calibration data registration to override the default number of volume
  steps.

  @description
  Additional important information includes:
  - The default number of volume steps is set to the number of volume
    calibration indices found in the registered volume calibration data.
  - The volume calibration data contains n volume indices (0 to n-1 with
    0 being the lowest volume), where n is defined by client. This applies
    to #CSD_VC_IOCTL_SET_RX_VOLUME_INDEX and #CSD_VC_IOCTL_SET_RX_VOLUME_STEP.
    #CSD_VC_IOCTL_SET_RX_VOLUME_STEP behaves the same as
    #CSD_VC_IOCTL_SET_RX_VOLUME_INDEX when the number of volume steps
    is n.
  - When the number of volume steps is set equal to the number of indices in
    the registered volume calibration data, #CSD_VC_IOCTL_SET_RX_VOLUME_STEP 
    is effectively a linear mapping.
  - When the number of volume steps is set greater than the number of
    indices in the registered volume calibration data,
    #CSD_VC_IOCTL_SET_RX_VOLUME_STEP causes the interpolation of the volume 
    level and a search of the closest matching volume level in the registered
    volume calibration data. The closest matching volume level is then applied.
  - When the number of volume steps is set to less than the number of indices
    in the registered volume calibration data,
    #CSD_VC_IOCTL_SET_NUMBER_OF_VOLUME_STEPS and 
    #CSD_VC_IOCTL_SET_RX_VOLUME_STEP will fail.
  - The OEM is responsible for ensuring the client-required number of volume
    steps and the number of volume indices in the volume calibration data
    match unless interpolation is intended.
  - #CSD_VC_IOCTL_SET_RX_VOLUME_STEP applies the volume setting over the 
    specified ramp duration in milliseconds.
*/
#define CSD_VC_IOCTL_SET_NUMBER_OF_VOLUME_STEPS ( 0x000112C1 )

/**
  Sets the Rx volume calibration based on the Rx volume index.
 
  @payload
  #csd_vc_ioctl_set_rx_volume_step_t
  
  @latexonly \input{tables/CSD_VC_IOCTL_SET_RX_VOLUME_STEP.tex} 
             @endlatexonly
  
  @return
  #CSD_VC_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VC_IOCTL_SET_RX_VOLUME_STEP ( 0x000112F6 )


/**
  Sets the mute control.

  @payload
  #csd_vc_ioctl_set_mute_t
  
  @latexonly \input{tables/CSD_VC_IOCTL_SET_MUTE.tex} @endlatexonly

  @return
  #CSD_VC_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VC_IOCTL_SET_MUTE ( 0x00011146 )

/**
  Sets the mute control.
 
  @payload
  #csd_vc_ioctl_set_mute_v2_t
  
  @latexonly \input{tables/CSD_VC_IOCTL_SET_MUTE_V2.tex} @endlatexonly

  @return
  #CSD_VC_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VC_IOCTL_SET_MUTE_V2 ( 0x000112F5 )

/****************************************************************************
 * CSD VC DTMF IOCTL COMMAND                                                *
 ****************************************************************************/

/**
  Enables/disables Tx DTMF detection. DTMF detection status is sent
  to the client sending this command via #CSD_VC_EVENTID_TX_DTMF_DETECTED
  events.
 
  @payload
  #csd_vc_ioctl_set_tx_dtmf_detection_t

  @description
  Only one client can request Tx DTMF detection on a given context at one time. 
  If another client requests Tx DTMF detection while the
  previous client's Tx DTMF detection is still active, the request
  fails.
  
  @return
  #CSD_VC_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VC_IOCTL_SET_TX_DTMF_DETECTION ( 0x00011033 )

/****************************************************************************
 * CSD VC SET/GET UI IOCTL COMMANDS                                        *
 ****************************************************************************/
/**
  Sets a UI-controlled property.
 
  @payload
  #csd_vc_ioctl_set_ui_property_t
  
  @latexonly \input{tables/CSD_VC_IOCTL_SET_UI_PROPERTY.tex} @endlatexonly

  @return
  #CSD_VC_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VC_IOCTL_SET_UI_PROPERTY ( 0x00011147 )

/**
  Gets the current values of a UI-controlled property.
 
  @payload
  #csd_vc_ioctl_get_ui_property_t
  
  @latexonly \input{tables/CSD_VC_IOCTL_SET_UI_PROPERTY.tex} @endlatexonly

  @return
  #CSD_VC_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VC_IOCTL_GET_UI_PROPERTY ( 0x00011148 )

/**
  Gets the number of volume steps for an Rx device. 
 
  @payload
  #csd_vc_ioctl_get_number_of_steps_t
  
  @latexonly \input{tables/CSD_VC_IOCTL_GET_NUMBER_OF_VOLUME_STEPS.tex} @endlatexonly

  @return
  #CSD_VC_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VC_IOCTL_GET_NUMBER_OF_VOLUME_STEPS ( 0x00011301 )
/**
  This command sets the volume step.

  @payload
  #csd_vc_ioctl_set_volume_step_t

  @latexonly \input{tables/CSD_VC_IOCTL_SET_VOLUME_STEP.tex} @endlatexonly

  @return
  #CSD_VC_EVENTID_COMMAND_RSP

  @dependencies
  None.

  @description
  1. The default number of volume steps will be set to the number of volume
     calibration indices found in the registered volume calibration data.

  2. #CSD_VC_IOCTL_GET_NUMBER_OF_VOLUME_STEPS should be called after volume
     calibration data registration to override the default number of volume
     steps.

  3. The volume calibration data will contain n volume indices (0 to n-1 with
     0 being the lowest volume), where n is defined by client. This applies
     to #CSD_VC_IOCTL_SET_RX_VOLUME_INDEX and #CSD_VC_IOCTL_SET_VOLUME_STEP.
     #CSD_VC_IOCTL_SET_VOLUME_STEP should behave equivalently to
     #CSD_VC_IOCTL_SET_RX_VOLUME_INDEX when the number of volume steps
     is n.

  4. When the number of volume steps is set equal to the number of indices in
     the registered volume calibration data, #CSD_VC_IOCTL_SET_VOLUME_STEP is
     effectively a linear mapping.

  5. When the number of volume steps is set greater than the number of
     indices in the registered volume calibration data,
     #CSD_VC_IOCTL_SET_VOLUME_STEP causes interpolation of the volume level
     and search of the closest matching volume level in the registered
     volume calibration data to apply.

  6. When the number of volume steps is set lesser than the number of indices
     in the registered volume calibration data,
     #CSD_VC_IOCTL_GET_NUMBER_OF_VOLUME_STEPS and #CSD_VC_IOCTL_SET_VOLUME_STEP will
     fail.

  7. The OEM is responsible for ensuring the client required number of volume
     steps and the number of volume indices in the volume calibration data
     match unless interpolation is intended.

  8. #CSD_VC_IOCTL_SET_VOLUME_STEP applies the volume setting over the specified
     ramp duration in milliseconds.
*/
#define CSD_VC_IOCTL_SET_VOLUME_STEP ( 0x000112C2 )

/** @} */ /* end_name Voice Context IOCTL Commands */

/****************************************************************************
 * CSD VC CORE IOCTL PAYLOAD STRUCTURES                                     *
 ****************************************************************************/

/* Type definition for #csd_vc_ioctl_set_device_config_t. */
typedef struct csd_vc_ioctl_set_device_config_t csd_vc_ioctl_set_device_config_t;

/** @brief Payload for #CSD_VC_IOCTL_SET_DEVICE_CONFIG. */
struct csd_vc_ioctl_set_device_config_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint32_t tx_dev_num;
    /**< CSD Tx device number. */
  uint32_t rx_dev_num;
    /**< CSD Rx device number. */
  uint32_t tx_dev_sr;
    /**< CSD Tx device sampling rate in Hz. */
  uint32_t rx_dev_sr;
    /**< CSD Rx device sampling rate in Hz. */
};

/* Type definition for #csd_vc_ioctl_set_device_config_v2_t. */
typedef struct csd_vc_ioctl_set_device_config_v2_t csd_vc_ioctl_set_device_config_v2_t;

/** @brief Payload for #CSD_VC_IOCTL_SET_DEVICE_CONFIG_V2. */
struct csd_vc_ioctl_set_device_config_v2_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint32_t tx_dev_id;
    /**< CSD Tx device ID. */
  uint32_t rx_dev_id;
    /**< CSD Rx device ID. */
  uint32_t tx_dev_sr;
    /**< CSD Tx device sampling rate in Hz. */
  uint32_t rx_dev_sr;
    /**< CSD Rx device sampling rate in Hz. */
  uint32_t ec_ref_dev_id;
    /**< CSD echo cancellation reference device ID.  
         Use CSD_OEM_DEVICE_ID_NONE if EC is not used. */
};

/****************************************************************************
 * CSD VC VOLUME/MUTE IOCTL PAYLOAD STRUCTURES                              *
 ****************************************************************************/

/** @brief Payload for #CSD_VC_IOCTL_SET_RX_VOLUME_INDEX. */
struct csd_vc_ioctl_set_rx_volume_index_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint32_t vol_index;
    /**< Rx target volume index to be set to context. */
};

/* Type definition for #csd_vc_ioctl_set_rx_volume_index_t. */
typedef struct csd_vc_ioctl_set_rx_volume_index_t csd_vc_ioctl_set_rx_volume_index_t;

/** @brief Payload for #CSD_VC_IOCTL_SET_NUMBER_OF_VOLUME_STEPS. */
struct csd_vc_ioctl_set_number_of_steps_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint32_t value;
    /**< The number of volume steps. */    
};

/* Type definition for #csd_vc_ioctl_set_number_of_steps_t. */
typedef struct csd_vc_ioctl_set_number_of_steps_t csd_vc_ioctl_set_number_of_steps_t;

/** @brief Payload for #CSD_VC_IOCTL_GET_NUMBER_OF_VOLUME_STEPS. */
struct csd_vc_ioctl_get_number_of_steps_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint32_t* num_steps;
    /**< Pointer to a uint32 that holds the total volume steps for the Rx device. 
         This value is populated when the client receives the callback. */
};

/* Type definition for #csd_vc_ioctl_get_number_of_steps_t. */
typedef struct csd_vc_ioctl_get_number_of_steps_t csd_vc_ioctl_get_number_of_steps_t;

/** @brief Payload for #CSD_VC_IOCTL_SET_RX_VOLUME_STEP. */
struct csd_vc_ioctl_set_rx_volume_step_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint32_t vol_step;
    /**< Rx target volume index to be set to context. */
  uint16_t ramp_duration;
    /**< Ramp duration for Mute disable/enable:\n 
         - 0 to 5000 ms. */
};

/* Type definition for #csd_vc_ioctl_set_rx_volume_step_t. */
typedef struct csd_vc_ioctl_set_rx_volume_step_t csd_vc_ioctl_set_rx_volume_step_t;

/** @brief Payload for #CSD_VC_IOCTL_SET_MUTE. */
struct csd_vc_ioctl_set_mute_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint16_t direction;
    /**< Direction in which the stream is flowing. Supported values:\n 
         - 0 -- Tx only. 
         - 1 -- Rx only. 
         - 2 -- Tx and Rx. */
  uint16_t mute_flag;
    /**< Mute disable/enable. Supported values:\n 
         - 0 -- Unmute. 
         - 1 -- Mute. */
};

/* Type definition for #csd_vc_ioctl_set_mute_t. */
typedef struct csd_vc_ioctl_set_mute_t csd_vc_ioctl_set_mute_t;

/** 
  @name Voice Context MUTE_V2 Definitions
  @{ 
*/

/** Selects the Tx path. */
#define CSD_VC_IOCTL_DIRECTION_TX ( 0 )

/** Selects the Rx path. */
#define CSD_VC_IOCTL_DIRECTION_RX ( 1 )

/** Turns off the Mute feature. */
#define CSD_VC_IOCTL_MUTE_OFF ( 0 )

/** Turns on the Mute feature. */
#define CSD_VC_IOCTL_MUTE_ON ( 1 )

/** @} */ /* end_name Voice Context MUTE_V2 Definitions */

/** @brief Payload for #CSD_VC_IOCTL_SET_MUTE_V2. */
struct csd_vc_ioctl_set_mute_v2_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint16_t direction;
    /**< Direction of mute :\n 
         - #CSD_VC_IOCTL_DIRECTION_RX -- Only Rx Mute is supported.  */
  uint16_t mute_flag;
    /**< Mute disable/enable:\n 
         - #CSD_VC_IOCTL_MUTE_OFF -- Unmute. 
         - #CSD_VC_IOCTL_MUTE_ON -- Mute. */
  uint16_t ramp_duration;
    /**< Ramp duration for Mute disable/enable. Range: 0 to 5000 ms. */
};

/* Type definition for #csd_vc_ioctl_set_mute_v2_t. */
typedef struct csd_vc_ioctl_set_mute_v2_t csd_vc_ioctl_set_mute_v2_t;

/****************************************************************************
 * CSD VC DTMF IOCTL PAYLOAD STRUCTURE                                      *
 ****************************************************************************/

/** 
  @name Voice Context DTMF Definitions
  @{ 
*/

/** DTMF detection enable flag. */
#define CSD_VC_TX_DTMF_DETECTION_ENABLE ( 1 )

/** DTMF detection disable flag. */
#define CSD_VC_TX_DTMF_DETECTION_DISABLE ( 0 )

/** @} */ /* end_name Voice Context DTMF Definitions */

/** @brief Payload for #CSD_VC_IOCTL_SET_TX_DTMF_DETECTION. */
struct csd_vc_ioctl_set_tx_dtmf_detection_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint32_t enable;
    /**< Enables/disables Tx DTMF detection. Supported values:\n
         - 1 -- Enables Tx DTMF detection.  
         - 0 -- Disables Tx DTMF detection.   */
};

/* Type definition for #csd_vc_ioctl_set_tx_dtmf_detection_t. */
typedef struct csd_vc_ioctl_set_tx_dtmf_detection_t csd_vc_ioctl_set_tx_dtmf_detection_t;

/****************************************************************************
 * CSD VC SET/GET UI IOCTL PAYLOAD STRUCTURES                               *
 ****************************************************************************/

/** @brief Payload for #CSD_VC_IOCTL_SET_UI_PROPERTY. */
struct csd_vc_ioctl_set_ui_property_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint32_t module_id;
    /**< ID of the module to be configured. */
  uint32_t param_id;
    /**< ID of the parameter to be configured. */
  uint32_t param_size;
    /**< Data size of this module ID and parameter ID combination. */
  void* param_data;
    /**< Actual data for the module ID and parameter ID. */
};

/* Type definition for #csd_vc_ioctl_set_ui_property_t. */
typedef struct csd_vc_ioctl_set_ui_property_t csd_vc_ioctl_set_ui_property_t;

/**  @brief Payload for #CSD_VC_IOCTL_GET_UI_PROPERTY. */
struct csd_vc_ioctl_get_ui_property_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint32_t module_id;
    /**< ID of the module to be configured. */
  uint32_t param_id;
    /**< ID of the parameter to be configured. */
  uint32_t param_size;
    /**< Data size of this module ID and parameter ID combination. */
  void* param_data;
    /**< Actual data for the module ID and parameter ID. */
};

/* Type definition for #csd_vc_ioctl_get_ui_property_t. */
typedef struct csd_vc_ioctl_get_ui_property_t csd_vc_ioctl_get_ui_property_t;
/** 
  Starts a vocoder PCM session.

  @par Payload
  #csd_vc_ioctl_ivpcm_cmd_start_t

  @dependencies
  None.

  @return
  #CSD_VC_EVENTID_COMMAND_RSP

  @description
  This command is used to start a vocoder PCM session by specifying one or more 
  PCM tap points. Only one vocoder PCM start is supported by the module session 
  at any given time. During device switch, the client must stop and 
  re-configure the vocoder PCM. Otherwise, for any configuration changes during 
  device switch, the client must check the new configuration information from 
  the #CSD_VC_IVPCM_EVT_NOTIFY event.
*/
#define CSD_VC_IOCTL_IVPCM_CMD_START ( 0x000112AA )

/** Default tap point location on the Tx path. */
#define CSD_VC_IVPCM_TAP_POINT_TX_DEFAULT ( 0x00011289 )

/** Default tap point location on the Rx path. */
#define CSD_VC_IVPCM_TAP_POINT_RX_DEFAULT ( 0x0001128A )

/** @name Vocoder PCM Sampling Rate IDs 
  @{
*/
/** Sampling rate matches the operating sampling rate of the post-processing 
  chain on the vDSP at the location of the tap point. */
#define CSD_VC_IVPCM_SAMPLING_RATE_AUTO ( 0 )
     
/** Sampling rate of 8 kHz.*/     
#define CSD_VC_IVPCM_SAMPLING_RATE_8K   ( 8000 )  /* 8 kHz */

/** Sampling rate of 16 kHz.*/
#define CSD_VC_IVPCM_SAMPLING_RATE_16K  ( 16000 ) /* 16 kHz */

/** Output tap point. */
#define CSD_VC_IVPCM_TAP_POINT_DIR_OUT  ( 0 ) /* Output tap point */

/** Input tap point. */
#define CSD_VC_IVPCM_TAP_POINT_DIR_IN   ( 1 ) /* Input tap point */

/** Output-input tap point. */
#define CSD_VC_IVPCM_TAP_POINT_DIR_OUT_IN   ( 2 ) /* Output-input tap point */
/** @} */ /* end_name Vocoder PCM sampling rate IDs */

/* Type definition for #csd_vc_ioctl_ivpcm_tap_point_t. 
*/
typedef struct csd_vc_ioctl_ivpcm_tap_point_t csd_vc_ioctl_ivpcm_tap_point_t;

/** @brief Structure for specifying tap point characteristics.
*/
struct csd_vc_ioctl_ivpcm_tap_point_t
{

  uint32_t tap_point;	
    /**< Tap point location GUID. Supported values:
         - CSD_VC_IVPCM_TAP_POINT_TX_DEFAULT 
         - CSD_VC_IVPCM_TAP_POINT_RX_DEFAULT */
  uint16_t direction;
    /**< Data flow direction of the tap point. Supported values:
         - CSD_VC_IVPCM_TAP_POINT_DIR_OUT -- Output. 
         - CSD_VC_IVPCM_TAP_POINT_DIR_IN -- Input.
         - CSD_VC_IVPCM_TAP_POINT_DIR_OUT_IN -- Output-input. */
  uint16_t sampling_rate;
    /**< Sampling rate of the tap point. If the tap point is output-input, 
         then output sampling rate and the input sampling rate are the same.
         Supported values:
         - CSD_VC_IVPCM_SAMPLING_RATE_AUTO
         - CSD_VC_IVPCM_SAMPLING_RATE_8K
         - CSD_VC_IVPCM_SAMPLING_RATE_16K */
  uint16_t duration;
    /**< Duration of buffer in milliseconds. When set to 0, the buffer is not 
         supported. */
};

/* Type definition for #csd_vc_ioctl_ivpcm_cmd_start_t.
*/
typedef struct csd_vc_ioctl_ivpcm_cmd_start_t csd_vc_ioctl_ivpcm_cmd_start_t;

/** @brief Payload for #CSD_VC_IOCTL_IVPCM_CMD_START.
*/
struct csd_vc_ioctl_ivpcm_cmd_start_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client  
         to identify which command completed. */
  uint32_t num_tap_points;
    /**< Number of tap points being specified in this vocoder PCM Start 
         command. */
  csd_vc_ioctl_ivpcm_tap_point_t *tap_points;
    /**< Pointer to an array of #csd_vc_ioctl_ivpcm_tap_point_t of size 
         num_tap_points. */
};

#define CSD_VC_IOCTL_IVPCM_CMD_START_V2 ( 0x00012A03 ) 
/** @brief Type definition for #csd_vc_ivpcm_cmd_start_v2_t.
*/
typedef struct csd_vc_ioctl_ivpcm_cmd_start_v2_t csd_vc_ioctl_ivpcm_cmd_start_v2_t;

/** @brief Payload structure for the #CSD_VC_IVPCM_CMD_START_V2
 *         command.
*/
struct csd_vc_ioctl_ivpcm_cmd_start_v2_t
{
  uint32_t cmd_token;
    /**< The transaction identifier provided by a client that allows a client to identify which command completed. */
  uint32_t mem_handle;
    /**< Handle to the shared memory that will be used for vocoder PCM data 
         buffer transfers. See @latexonly \hyperref[shmem_guideline]{Shared 
         Memory Usage Guideline} @endlatexonly. */
  uint32_t num_tap_points;
    /**< Number of tap point being specified in this vocoder PCM 
         start command. */
  csd_vc_ioctl_ivpcm_tap_point_t *tap_points;
    /**< Pointer to an array of csd_vc_ioctl_ivpcm_tap_point_t of size num_tap_points. */
};

/** exchange buffer event masks. */
#define CSD_VC_IOCTL_IVPCM_EXCHANGE_BUFFER_MASK_OUTPUT_BUFFER ( 1 ) /* output buffer filled. */

/** Input buffer is consumed. */
#define CSD_VC_IOCTL_IVPCM_EXCHANGE_BUFFER_MASK_INPUT_BUFFER ( 2 ) /* input buffer consumed. */
/** @} */ /* end_name Exchange Buffer Event Masks */

/**
  Provides a vocoder PCM input or output buffer, or both to the CSD.

  The client specifies whether it is providing an input buffer, an output 
  buffer, or both input and output buffers using the exchange_buf_mask bitmask 
  in the payload.
   
  @par Payload
  #csd_vc_ioctl_ivpcm_exchange_buffer_t

  @return
  #CSD_VC_EVENTID_COMMAND_RSP

  @dependencies
  The out_buf_addr and in_buff_addr must first be mapped with the CSD
  using the #CSD_VC_IOCTL_CMD_MAP_MEMORY command.\n 
  @vertspace
  The memory regions allocated for out_buf_addr and in_buf_addr must not 
  overlap.\n
  @vertspace
  Buffer properties:\n
  - The buffer size is in bytes.
  - In case of multi-channel input/output, the buffer is non-interleaved
    and partitioned to hold data from each channel.
  - All buffers are instantaneous and are consumed in real time.
  - Only one buffer is consumed in a timetick. All additional buffers in the 
    same timetick are dropped, and notification is sent back using the 
    corresponding error bit set.
  - Multiple buffers exchanged are consumed in subsequent timeticks.
  - The buffer format must always match the expected format:\n
      - Excess data in the buffer is discarded
      - Partial data buffers are dropped.
*/
#define CSD_VC_IOCTL_IVPCM_EXCHANGE_BUFFER ( 0x000112AB )

/* Type definition for #csd_vc_ioctl_ivpcm_exchange_buffer_t.
*/
typedef struct csd_vc_ioctl_ivpcm_exchange_buffer_t csd_vc_ioctl_ivpcm_exchange_buffer_t;

/** @brief Payload for #CSD_VC_IOCTL_IVPCM_EXCHANGE_BUFFER.
*/
struct csd_vc_ioctl_ivpcm_exchange_buffer_t
{
 uint32_t tap_point;
   /**< GUID of the tap point for which the buffer(s) is(are) provided. */
 uint32_t exchange_buf_mask;
   /**< Bitmask indicating whether an output buffer or input buffer is being 
        provided, or both. Supported values:
        - Bit 0 -- Output buffer; use 
          #CSD_VC_IOCTL_IVPCM_EXCHANGE_BUFFER_MASK_OUTPUT_BUFFER.
        - Bit 1 -- Input buffer; use 
          #CSD_VC_IOCTL_IVPCM_EXCHANGE_BUFFER_MASK_INPUT_BUFFER. */
 uint32_t out_buf_addr;
   /**< If bit 0 of the exchange_buf_mask is set, this field indicates the 
        physical address of the output buffer. Otherwise, it is ingored. */
 uint32_t in_buf_addr;
   /**< If bit 1 of the exchange_buf_mask is set, this field indicates the 
        physical address of the input buffer. Otherwise, it is ignored. */
 uint16_t out_buf_size;
   /**< If bit 0 of the exchange_buf_mask is set, this field indicates the size
        of the buffer at out_buf_addr. Otherwise, it is ignored.
        The client allocates the output buffer to accommodate the maximum 
        expected sampling rate. */
 uint16_t in_buf_size;
   /**< If bit 1 of the exchange_buf_mask is set, this field indicates the size 
        of the input buffer at in_buff_addr. Otherwise, it is ignored. */
 uint16_t sampling_rate;
   /**< If bit 1 of the exchange_buf_mask is set, this field indicates the 
        sampling rate of the input buffer. Otherwise, it is ignored. Supported 
        values:
        - #CSD_VC_IVPCM_SAMPLING_RATE_8K -- 8 kHz.
        - #CSD_VC_IVPCM_SAMPLING_RATE_16K -- 16 kHz. */
 uint16_t num_in_channels;
   /**< If bit 1 of the exchange_buf_mask is set, this field indicates the 
        number of channels contained in the input buffer. Otherwise, it is 
        ignored. Supported value: 1 */
};

#define CSD_VC_IOCTL_IVPCM_EXCHANGE_BUFFER_V2 ( 0x00012A04 ) 

/** Default tap point location on the TX path. */
#define CSD_VC_IVPCM_TAP_POINT_TX_DEFAULT ( 0x00011289 )

/** Default tap point location on the RX path. */
#define CSD_VC_IVPCM_TAP_POINT_RX_DEFAULT ( 0x0001128A )

/** Push buffer event mask indicating output buffer is filled. */
#define CSD_VC_IVPCM_PUSH_BUFFER_MASK_OUTPUT_BUFFER ( 1 )

/** Push buffer event mask indicating input buffer is consumed. */
#define CSD_VC_IVPCM_PUSH_BUFFER_MASK_INPUT_BUFFER ( 2 )

/** @brief Type definition for #csd_vc_ivpcm_evt_exchange_buffer_t.
*/
typedef struct csd_vc_ioctl_ivpcm_exchange_buffer_v2_t csd_vc_ioctl_ivpcm_exchange_buffer_v2_t;

/** @brief Payload structure for
 *         #CSD_VC_IVPCM_EXCHANGE_BUFFER_V2 command.
*/
struct csd_vc_ioctl_ivpcm_exchange_buffer_v2_t
{
  uint32_t tap_point;
    /**< GUID identifying tap point for which the buffer(s) is(are) provided.  
         Supported values:\n
         #CSD_VC_IVPCM_TAP_POINT_TX_DEFAULT \n
         #CSD_VC_IVPCM_TAP_POINT_RX_DEFAULT */
  uint32_t push_buf_mask;
    /**< Bitmask inticating whether an output buffer is being provided or an
         input buffer or both. Supported values:\n
         bit 0 - Output buffer, use 
                 #CSD_VC_IVPCM_PUSH_BUFFER_MASK_OUTPUT_BUFFER \n
         bit 1 - Input buffer, use #CSD_VC_IVPCM_PUSH_BUFFER_MASK_INPUT_BUFFER. */
  uint32_t out_buf_mem_address_lsw;
    /**< Least significant word of the output buffer memory address.
         If bit 0 of the push_buf_mask is set, this field indicates the 
         location of the output buffer. Otherwise it is ignored. See 
         @latexonly \hyperref[shmem_guideline]{Shared Memory Usage Guideline}
         @endlatexonly on how the address is intrepreted. */
  uint32_t out_buf_mem_address_msw;
    /**< Most significant word of the output buffer memory address.
         If bit 0 of the push_buf_mask is set, this field indicates the 
         location of the output buffer. Otherwise it is ignored. See 
         @latexonly \hyperref[shmem_guideline]{Shared Memory Usage Guideline}
         @endlatexonly on how the address is intrepreted. */
  uint16_t out_buf_mem_size;
    /**< If bit 0 of the push_buf_mask is set, this field indicates the size of
         the buffer at out_buf_mem_address. Otherwise it is ignored. The client
         must allocate the output buffer to accommodate the maximum expected
         sampling rate and also meet the size requirement as specified in 
         @latexonly \hyperref[shmem_guideline]{Shared Memory Usage Guideline}
         @endlatexonly. */
  uint32_t in_buf_mem_address_lsw;
    /**< Least significant word of the input buffer memory address.
         If bit 1 of the push_buf_mask is set, this field indicates the 
         location of the input buffer. Otherwise it is ingored. See 
         @latexonly \hyperref[shmem_guideline]{Shared Memory Usage Guideline}
         @endlatexonly on how the address is intrepreted. */
  uint32_t in_buf_mem_address_msw;
    /**< Most significant word of the input buffer memory address.
         If bit 1 of the push_buf_mask is set, this field indicates the 
         location of the input buffer. Otherwise it is ingored. See 
         @latexonly \hyperref[shmem_guideline]{Shared Memory Usage Guideline}
         @endlatexonly on how the address is intrepreted. */
  uint16_t in_buf_mem_size;
    /**< If bit 1 of the push_buf_mask is set, this field indicates the size of
         the input buffer at in_buf_mem_address. Otherwise it is ignored. The 
         size of memory allocated at in_buf_mem_address must meet the 
         requirement as specified in @latexonly \hyperref[shmem_guideline]
         {Shared Memory Usage Guideline} @endlatexonly. */
  uint16_t sampling_rate;
    /**< If bit 1 of the push_buf_mask is set, this field indicates the 
         sampling rate of the input buffer. Otherwise it is ignored. Supported 
         values:\n
         #CSD_VC_IVPCM_SAMPLING_RATE_8K
         #CSD_VC_IVPCM_SAMPLING_RATE_16K \n */
  uint16_t num_in_channels;
    /**< If bit 1 of the push_buf_mask is set, this field indicates the number of
         channels contained in the input buffer. Otherwise it is ignored.
         Supported values:\n
         1 - One channel. */
};


/** 
  Stops an active vocoder PCM session.

  @par Payload
   User can provide a uint32_t token to track this command.

  @return
  #CSD_VC_EVENTID_COMMAND_RSP

  @dependencies
  None.

  @description
  Any buffers still in the ownership of the CSD module at the time of the Stop 
  command can be reclaimed by the client upon receiving a response to the Stop 
  command. If destroying the CSD module session while vocoder PCM is running, 
  any buffers still in the ownership of the CSD module can be reclaimed by the 
  client upon receiving a response to the #CSD_VC_EVENTID_COMMAND_RSP command. 
*/
#define CSD_VC_IOCTL_IVPCM_CMD_STOP ( 0x000112AC )

/** General shared memory; byte-accessible, 4-kilobyte aligned. */
#define CSD_VC_MAP_MEMORY_SHMEM8_4K_POOL ( 3 )


/**
  Maps a physical memory region with a CSD component.

  @par Payload
  #csd_vc_ioctl_cmd_map_memory_t

  @return
  #CSD_VC_EVENTID_COMMAND_RSP 

  @dependencies
  None.
*/
#define CSD_VC_IOCTL_CMD_MAP_MEMORY ( 0x000112AD )

/* Type definition for #csd_vc_ioctl_cmd_map_memory_t.
*/
typedef struct csd_vc_ioctl_cmd_map_memory_t csd_vc_ioctl_cmd_map_memory_t;


//#include "apr_pack_begin.h"

/** @brief Payload for #CSD_VC_IOCTL_CMD_MAP_MEMORY.
*/
struct csd_vc_ioctl_cmd_map_memory_t
{
   uint32_t cmd_token;
     /**< Transaction identifier provided by the client that allows the client 
          to identify which command completed. */
 
   uint32_t phys_addr;
     /**< Physical address of a memory region; must be at least
          4 kB aligned. */

   uint32_t mem_size;
     /**< Number of bytes in the region; a multiple of 32. */

   uint16_t mem_pool_id;
     /**< Type of memory being provided. The memory ID implicitly defines
          the characteristics of the memory. The characteristics might include
          alignment type, permissions, etc.

          Memory pool ID. Supported values:
          - 3 -- CSD_VC_MAP_MEMORY_SHMEM8_4K_POOL.
     */
};


/**
  Unmaps a physical memory region with a CSD component.

  @par Payload
  #csd_vc_ioctl_cmd_unmap_memory_t

  @return
  #CSD_VC_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VC_IOCTL_CMD_UNMAP_MEMORY ( 0x000112AE )


/* Type definition for #csd_vc_ioctl_cmd_unmap_memory_t.
*/
typedef struct csd_vc_ioctl_cmd_unmap_memory_t csd_vc_ioctl_cmd_unmap_memory_t;


/** @brief Payload for #CSD_VC_IOCTL_CMD_UNMAP_MEMORY.
*/
struct csd_vc_ioctl_cmd_unmap_memory_t
{
   uint32_t cmd_token;
   /**< Transaction identifier provided by the client that allows the client to 
        identify which command completed. */

   uint32_t phys_addr;
     /**< Physical address of a memory region; must be at least
          4 kB aligned. */
};

/**
  Maps a physical memory region with a CSD component.

  @par Payload
  #csd_vc_ioctl_cmd_map_memory_v2_t

  @return
  #CSD_VC_EVENTID_COMMAND_RSP 

  @dependencies
  None.
*/
#define CSD_VC_IOCTL_CMD_MAP_MEMORY_V2 ( 0x00012A05 ) 

/** @brief Type definition for #csd_vc_table_descriptor_t.
*/
typedef struct csd_vc_imemory_table_descriptor_t csd_vc_imemory_table_descriptor_t;

struct csd_vc_imemory_table_descriptor_t
{
  uint32_t mem_address_lsw;
    /**< Least significant word of the base physical address of the table. 
         The base physical address must be aligned to LCM( cache_line_size, 
         page_align, max_data_width ), where the attributes are specified 
         in #CSD_VC_IOCTL_CMD_MAP_MEMORY_V2, and LCM = Least Common Multiple. 
         The table at the address must have the format specified by 
         #csd_vc_imemory_table_t. */
  uint32_t mem_address_msw;
    /**< Most significant word of the base physical address of the table. 
         The base physical address must be aligned to LCM( cache_line_size, 
         page_align, max_data_width ), where the attributes are specified 
         in #CSD_VC_IOCTL_CMD_MAP_MEMORY_V2, and LCM = Least Common Multiple. 
         The table at the address must have the format specified by 
         #csd_vc_imemory_table_t. */
  uint32_t mem_size;
    /**< Size in bytes of the table. */
};


/** @brief Type definition for #csd_vc_imemory_block_t.
*/
typedef struct csd_vc_imemory_block_t csd_vc_imemory_block_t;

/** @brief Structure for specifying a single block of contiguous memory.
*/
struct csd_vc_imemory_block_t
{
  uint32_t mem_address_lsw;
    /**< Least significant word of the base address of the memory block. 
         The base address is virtual for virtual memory and physical for 
         physical memory. The address must be aligned to LCM( cache_line_size, 
         page_align, max_data_width ), where the attributes are specified in 
         #CSD_VC_IOCTL_CMD_MAP_MEMORY_V2, and LCM = Least Common Multiple. */
  uint32_t mem_address_msw;
    /**< Most significant word of the base address of the memory block. 
         The address is virtual for virtual memory and physical for physical 
         memory. The address must be aligned to LCM( cache_line_size, 
         page_align, max_data_width ), where the attributes are specified in 
         #CSD_VC_IOCTL_CMD_MAP_MEMORY_V2, and LCM = Least Common Multiple. */
  uint32_t mem_size;
    /**< Size in bytes of the memory block. The size must be multiple of 
         page_align, where page_align is specified in 
         #CSD_VC_IOCTL_CMD_MAP_MEMORY_V2. */
};

/** @brief Type definition for #csd_vc_imemory_table_t.
*/
typedef struct csd_vc_imemory_table_t csd_vc_imemory_table_t;

/** @brief Structure for specifying the format of a table that contains
    information on memory blocks.
*/
struct csd_vc_imemory_table_t
{
  csd_vc_imemory_table_descriptor_t next_table_descriptor;
    /**< Specifies the next table. If there is no next table, set the size of
         the table to 0 and the table address is ignored. */
#if __STDC_VERSION__ >= 199901L
  csd_vc_imemory_block_t blocks[];
    /**< Specifies one ore more memory blocks. */
#endif /* __STDC_VERSION__ >= 199901L */
};

/** @brief Type definition for #csd_vc_ioctl_cmd_map_memory_v2_t.
*/
typedef struct csd_vc_ioctl_cmd_map_memory_v2_t csd_vc_ioctl_cmd_map_memory_v2_t;

/** @brief Payload structure for the #CSD_VC_IOCTL_CMD_MAP_MEMORY_V2 command.
*/
struct csd_vc_ioctl_cmd_map_memory_v2_t
{
  uint32_t cmd_token;
    /**< The transaction identifier provided by a client that allows a client to identify which command completed. */

  csd_vc_imemory_table_descriptor_t table_descriptor;
    /**< Specifies the location and size of a table that contains information
         on one or more memory blocks. */
  bool_t is_cached;
    /**< Indicates cached or uncached memory. Supported values:\n
         TRUE - Cached. */
  uint16_t cache_line_size;
    /**< Cache line size in bytes. Supported values:\n
         128. */
  uint32_t access_mask;
    /**< access permission to the memory while it is mapped. Supported
         values:\n
         bit 0 - If set, the memory is readable.\n
         bit 1 - If set, the memory is writable.\n */
  uint32_t page_align;
    /**< Page frame alignment in bytes. Supported values:\n
         4096. */
  uint8_t min_data_width;
    /**< Minimum native data type width in bits that can be accessed.
         Supported values:\n
         8. */
  uint8_t max_data_width;
    /**< Maximum native data type width in bits that can be accessed.
         Supported values:\n
         64. */
};


/**
  Unmaps a physical memory region with a CSD component.

  @par Payload
  #csd_vc_ioctl_cmd_unmap_memory_v2_t

  @return
  #CSD_VC_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VC_IOCTL_CMD_UNMAP_MEMORY_V2 ( 0x00012A06 ) 


/** @brief Type definition for #csd_vc_ioctl_cmd_unmap_memory_v2_t.
*/
typedef struct csd_vc_ioctl_cmd_unmap_memory_v2_t csd_vc_ioctl_cmd_unmap_memory_v2_t;


/** @brief Payload structure for the
 *         #CSD_VC_IOCTL_CMD_UNMAP_MEMORY_V2 command.
*/
struct csd_vc_ioctl_cmd_unmap_memory_v2_t
{
   uint32_t cmd_token;
    /**< The transaction identifier provided by a client that allows a client to identify which command completed. */

   uint32_t mem_handle;
    /**< Memory handle returned by #CSD_VC_COMMAND_RSP_MAP. */
};

/** @brief Type definition for #csd_vc_ioctl_set_volume_step_t.
*/
typedef struct csd_vc_ioctl_set_volume_step_t csd_vc_ioctl_set_volume_step_t;

/** @brief Payload structure for the #CSD_VC_IOCTL_SET_VOLUME_STEP command.
*/
struct csd_vc_ioctl_set_volume_step_t
{
  uint32_t cmd_token;
    /**< The transaction identifier provided by a client that allows a client to identify which command completed. */
  uint16_t direction;
    /**< The direction field sets the direction to apply the volume command.
         The supported values: \n
         - 1 -- Rx only.  */
  uint32_t value;
    /**< Volume step used to find the corresponding linear volume and
         the best match index in the registered volume calibration table. */
  uint16_t ramp_duration_ms;
    /**< Volume change ramp duration in milliseconds.
         The supported values: 0 to 5000. */
}
;

/**
  Sets a feature set ID used for voice calibration on a device pair 

  The client can use this IOCTL before or after CSD_VC_IOCTL_SET_DEVICE_CONFIG and CSD_VC_IOCTL_SET_DEVICE_CONFIG_V2.

  If used before, the feature ID will be cached in the session and when CSD_VC_IOCTL_SET_DEVICE_CONFIG or CSD_VC_IOCTL_SET_DEVICE_CONFIG_V2
  occurs, the device will be set to the calibration of the specified cached feature ID.  If the IOCTL is not called, the feature ID is 
  defaulted to 0x0. 

  If client calls this CSD_VC_IOCTL_ENUM_CAL_VOLUME feature ID IOCTL after CSD_VC_IOCTL_SET_DEVICE_CONFIG or
  CSD_VC_IOCTL_SET_DEVICE_CONFIG_V2, the device will be re-calibrated to the specified Feature ID.

  If a device switch occurs, CSD_VC_IOCTL_DISABLE should be invoked, which clears the CSD_VC_IOCTL_ENUM_CAL_VOLUME feature ID
  cached in the session, restoring the feature ID to the default value 0x0.

  Client needs to define how many calibration tables they will be apply Feature IDs to using num_fid_in_table and defining those features in
  fid_table[].  In fid_table[] the client defines the feature ID and the associated calibration table which it will be effecting.  The types
  of calibrations that support feature IDs is listed in csd_vc_ioctl_calibration_table_enum_t.

  For  CSD_VC_IOCTL_ENUM_TOPOLOGY Feature ID, Client need to set topology feature id before CSD_VC_IOCTL_SET_DEVICE_CONFIG or
  CSD_VC_IOCTL_SET_DEVICE_CONFIG_V2. CSD caches the topology feature ID when Client set the topology feature id.. When next
  CSD_VC_IOCTL_SET_DEVICE_CONFIG_V2 occurs, the device topology will be set to specific cached topology feature ID.
  Topology feature id is applicable only once, after it is applied to device, cached value will be set to default value ie 0x0.

  @return
  CSD_EOK -- Success.\n
  Error code on failure.

  @dependencies
  None.
*/

#define CSD_VC_IOCTL_SET_CAL_FID ( 0x0001305C )

#define CSD_VC_IOCTL_CAL_FID_ARRAY_MAX            3

/** @brief possible topology values CSD_VC_IOCTL_ENUM_TOPOLOGY.
*/
typedef enum {
  CSD_FID_TOPOLOGY_DEFAULT = 0,
  /**< Set default device topologies obtained from ACDB. */
  CSD_FID_TOPOLOGY_NONE
  /**< Set None topology. */
} csd_fid_topology_enum_t;

typedef enum {
  CSD_VC_IOCTL_ENUM_CAL_VOLUME = 0,
  CSD_VC_IOCTL_ENUM_TOPOLOGY,
} csd_vc_ioctl_calibration_table_enum_t;

typedef struct csd_vc_ioctl_fid_table_t csd_vc_ioctl_fid_table_t;
  struct csd_vc_ioctl_fid_table_t
  {
    uint32_t feature_id;
    csd_vc_ioctl_calibration_table_enum_t cal_table;
  };

typedef struct csd_vc_ioctl_set_cal_fid_t csd_vc_ioctl_set_cal_fid_t;
struct csd_vc_ioctl_set_cal_fid_t
{
  uint32_t cmd_token;
  uint32_t num_fid_in_table;
  csd_vc_ioctl_fid_table_t fid_table[CSD_VC_IOCTL_CAL_FID_ARRAY_MAX];
};
/** @} */ /* end_addtogroup csd_vc */

#endif /* __CSD_VC_IOCTL_H__ */

