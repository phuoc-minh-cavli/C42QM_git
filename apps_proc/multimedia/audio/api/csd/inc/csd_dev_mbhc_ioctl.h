#ifndef _CSD_DEV_MBHC_IOCTL_H_
#define _CSD_DEV_MBHC_IOCTL_H_
/**
    @file       csd_dev_mbhc_ioctl.h
    @brief
    Structure   Interface definition for MBHC
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
     Copyright (C) 2010-2013 Qualcomm Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary.
-----------------------------------------------------------------------------*/
/*=============================================================================
                      EDIT HISTORY FOR MODULE
    This section contains comments describing changes made to this module.
    Notice that changes are listed in reverse chronological order.

    $Header: //components/rel/audio.tx/2.0/audio/api/csd/inc/csd_dev_mbhc_ioctl.h#1 $
    $DateTime: 2018/12/25 22:39:07 $
    $Author: pwbldsvc $
    $Change: 17929171 $
    $Revision: #1 $

=============================================================================*/
#include "mmdefs.h"

/** @addtogroup csd_dev
  @{
*/

/**
   @name MBHC Type
   @{ 
*/ 

/** Enable MBHC detection. 
    This enables MBHC detection for plug insertion/removal and button 
    press/release.

    There is no payload.
*/
#define CSD_DEV_MBHC_CMD_ENABLE                             1

/** Disable MBHC detection.
    All detection functionalities in MBHC will be disabled.

    There is no payload.
*/
#define CSD_DEV_MBHC_CMD_DISABLE                            2

/** Set button detection mode.
    This provides the client the ability to select non-default button 
    detection modes for certain accessories or to disable button
    detection altogether for accessories that support buttons. Note
    that each accessory has a default button detection mode once
    detected according to the type of accessory, e.g. headsets will
    default to passive button detection and headphones will default 
    to no button detection. For a PTT accesory that is detected as a
    headset device, the client needs to manually set the button 
    detection mode to PTT.

    See csd_dev_mbhc_button_detection_mode for payload.
*/
#define CSD_DEV_CMD_MBHC_SET_BUTTON_DETECTION_MODE          3

/** Force manual re-detection .
    This allows the client to trigger a re-detection after an accessory has
    already been inserted. This may be required in certain situations where
    the accessory detection result may be considered to be incorrect, e.g. 
    during slow or partial insertion. A new event notification will be provided
    with the detection result if this API is called and if a plug is detected.

    There is no payload.
*/
#define CSD_DEV_MBHC_CMD_FORCE_REDETECTION                  4

/** @} */ /* end_name MBHC Type  */


/**
   @name Button Detection Mode Type
   @{ 
*/ 
typedef enum csd_dev_mbhc_button_detection_mode_type
{
    /** Disable button detection */
    CSD_DEV_MBHC_BUTTON_DETECTION_MODE_TYPE_DISABLE         = 0,
    /** Passive button detection is enabled */
    CSD_DEV_MBHC_BUTTON_DETECTION_MODE_TYPE_PASSIVE_BUTTONS = 1,
    /** PTT (push-to-talk) button detection is enabled */
    CSD_DEV_MBHC_BUTTON_DETECTION_MODE_TYPE_PTT_BUTTON      = 2,
    /** Max number of modes */
    CSD_DEV_MBHC_BUTTON_DETECTION_MODE_MAX
} csd_dev_mbhc_button_detection_mode_type;

/** @} */ /* end_name Button Detection Mode Type */

/**
  @brief defines the detection mode structure for CSD_DEV_CMD_MBHC_SET_BUTTON_DETECTION_MODE
*/
struct csd_dev_mbhc_button_detection_mode
{
   /** Button detection mode type to be configured. */
   csd_dev_mbhc_button_detection_mode_type type;
};

/** @brief Type definition for csd_dev_mbhc_button_detection_mode_t. */
typedef struct csd_dev_mbhc_button_detection_mode csd_dev_mbhc_button_detection_mode_t;

/*-------------------------------------------------------------------------*/
/*  MBHC callback event ids */
/*-------------------------------------------------------------------------*/

/**    Start of the CSD event ID, which is 0x00011600. */
#define CSD_DEV_MBHC_EVT_START                  0x00012E7E

/** Headset accessory plug insert event.

  @payload
  #csd_mbhc_evt_payload

  @return
  None.

  @dependencies
  None.
*/
#define CSD_DEV_MBHC_EVT_ACCESSORY_INSERT       (CSD_DEV_MBHC_EVT_START + 1)

/** Headset accessory plug remove event.

  @payload
  #csd_mbhc_evt_payload

  @return
  None.

  @dependencies
  None.
*/
#define CSD_DEV_MBHC_EVT_ACCESSORY_REMOVE       (CSD_DEV_MBHC_EVT_START + 2)

/** Headset button press event.

  @payload
  #csd_mbhc_evt_payload

  @return
  None.

  @dependencies
  None.
*/
#define CSD_DEV_MBHC_EVT_BUTTON_PRESS           (CSD_DEV_MBHC_EVT_START + 3)

/** Headset button release event.

  @payload
  #csd_mbhc_evt_payload

  @return
  None.

  @dependencies
  None.
*/
#define CSD_DEV_MBHC_EVT_BUTTON_RELEASE         (CSD_DEV_MBHC_EVT_START + 4)

/** Headset button press and release event for very short press and release.

  @payload
  #csd_mbhc_evt_payload

  @return
  None.

  @dependencies
  None.
*/
#define CSD_DEV_MBHC_EVT_BUTTON_PRESS_RELEASE   (CSD_DEV_MBHC_EVT_START + 5)

/** Headset insert/remove detection in progress event. This event indicates
    that h/w has detected either a plug insertion or removal and that the
    detection algorithm is in the process of determining whether this is a 
    valid event. If this insert/remove is deemed to be valid, then the event
    CSD_DEV_MBHC_EVT_ACCESSORY_INSERT/CSD_DEV_MBHC_EVT_ACCESSORY_REMOVE will 
    also be provided shortly after the CSD_DEV_MBHC_EVT_DETECTION_IN_PROGRESS
    event.

  @payload
  None.

  @return
  None.

  @dependencies
  None.
*/
#define CSD_DEV_MBHC_EVT_DETECTION_IN_PROGRESS  (CSD_DEV_MBHC_EVT_START + 6)

/** Maximum ID reserved for MBHC. */  
#define CSD_DEV_MBHC_ID_END                     0x00012EBE

/*-------------------------------------------------------------------------*/
/*  payload for event ids */
/*-------------------------------------------------------------------------*/

/**
   @name Headset Accessory Type
   @{ 
*/ 

typedef enum CSD_DEV_MBHC_ACCESSORY_ID
{
    /** Invalid accessory type. */
    CSD_DEV_MBHC_ACCESSORY_ID_INVALID                                   = 0,
    /** No accessory is connected. */
    CSD_DEV_MBHC_ACCESSORY_ID_NO_PLUG                                   = 1,
    /** Stereo headset with microphone on sleever (Tip=HPL, Ring1=HPR, Ring2=G, Sleeve=M) */
    CSD_DEV_MBHC_ACCESSORY_ID_NA_STEREO_HEADSET_WITH_MICROPHONE_HHGM    = 2,
    /** Stereo headset for EU/China with microphone on sleever (Tip=HPL, Ring1=HPR, Ring2=M, Sleeve=G) */
    CSD_DEV_MBHC_ACCESSORY_ID_EU_STEREO_HEADSET_WITH_MICROPHONE_HHMG    = 3,
    /** Mono headset with R = Ground (Tip=HPL, Ring1=G, Ring2=G, Sleeve=M) */
    CSD_DEV_MBHC_ACCESSORY_ID_NA_MONO_HEADSET_WITH_MICROPHONE_HGGM      = 4,
    /** Mono headset with R = L (Tip=HPL, Ring1=HPL, Ring2=G, Sleeve=M) */
    CSD_DEV_MBHC_ACCESSORY_ID_NA_MONO_HEADSET_WITH_MICROPHONE_HHGM      = 5,
    /** Mono headset with R = Open (Tip=HPL, Ring1=F, Ring2=G, Sleeve=M) */
    CSD_DEV_MBHC_ACCESSORY_ID_NA_MONO_HEADSET_WITH_MICROPHONE_HFGM      = 6,
    /** Mono headset for EU/China with R = Ground (Tip=HPL, Ring1=G, Ring2=M, Sleeve=G) */
    CSD_DEV_MBHC_ACCESSORY_ID_EU_MONO_HEADSET_WITH_MICROPHONE_HGMG      = 7,
    /** Mono headset for EU/China with R = L (Tip=HPL, Ring1=HPL, Ring2=M, Sleeve=G) */
    CSD_DEV_MBHC_ACCESSORY_ID_EU_MONO_HEADSET_WITH_MICROPHONE_HHMG      = 8,
    /** Mono headset for EU/China with R = Open (Tip=HPL, Ring1=F, Ring2=M, Sleeve=G) */
    CSD_DEV_MBHC_ACCESSORY_ID_EU_MONO_HEADSET_WITH_MICROPHONE_HFMG      = 9,
    /** Stereo headphone (Tip=HPL, Ring1=HPR, Ring2=G, Sleeve=G) */
    CSD_DEV_MBHC_ACCESSORY_ID_STEREO_HEADPHONE_WITHOUT_MICROPHONE_HHGG  = 10,
    /** Mono headphone with only L (Tip=HPL, Ring1=F, Ring2=G, Sleeve=G) */
    CSD_DEV_MBHC_ACCESSORY_ID_MONO_HEADPHONE_WITHOUT_MICROPHONE_HFGG    = 11,
    /** Mono headphone with only R (Tip=F, Ring1=HPR, Ring2=G, Sleeve=G) */
    CSD_DEV_MBHC_ACCESSORY_ID_MONO_HEADPHONE_WITHOUT_MICROPHONE_FHGG    = 12,
    /** Mono headphone with only L = R (Tip=HPL, Ring1=HPR, Ring2=G, Sleeve=G)   */
    CSD_DEV_MBHC_ACCESSORY_ID_MONO_HEADPHONE_WITHOUT_MICROPHONE_HHGG    = 13,
    /** Audio lineout with L on Tip and R on Ring1 (Tip=L-lineout, Ring1=R-lineout, Ring2=G, Sleeve=G) */
    CSD_DEV_MBHC_ACCESSORY_ID_AUDIO_LINEOUT_LLGG                        = 14,
    /** Privacy security plug (Tip=G, Ring1=G, Ring2=G, Sleeve=G) */
    CSD_DEV_MBHC_ACCESSORY_ID_PLUG_GGGG                                 = 18,
    /** Special headset (Tip=HPL, Ring1=HPR, Ring2=G, Sleeve=M) */
    CSD_DEV_MBHC_ACCESSORY_ID_SPECIAL_HEADSET_HHGM                      = 19,
    /** TTY device (Tip=N/A, Ring1=G, Ring2=G, Sleeve=N/A) */
    CSD_DEV_MBHC_ACCESSORY_ID_PLUG_XGGX                                 = 20,
    /** 4 Pole extension cable (Tip=F, Ring1=F, Ring2=F, Sleeve=F) */
    CSD_DEV_MBHC_ACCESSORY_ID_4POLE_EXTENSION_CABLE_FFFF                = 21,
    /** Max number of CSD_MBHC_ACCESSORY_IDs */
    CSD_DEV_MBHC_ACCESSORY_ID_MAX
} CSD_DEV_MBHC_ACCESSORY_ID;


/** @} */ /* end_name Headset Accessory Type */

/**
   @name Pinout Impedence
   @{ 
*/ 
typedef enum CSD_DEV_MBHC_PINOUT
{
    /** Pin is Unknown */
    CSD_DEV_MBHC_PINOUT_UNKNOWN         = 0,
    /** Pin is Ground */
    CSD_DEV_MBHC_PINOUT_GROUND          = 1,
    /** Pin is Mic */
    CSD_DEV_MBHC_PINOUT_MIC             = 2,
    /** Pin is Headphone */
    CSD_DEV_MBHC_PINOUT_HPH             = 3,
    /** Pin is Floating */
    CSD_DEV_MBHC_PINOUT_FLOATING        = 4,
    /** Max number of CSD_MBHC_PINOUTs */
    CSD_DEV_MBHC_PINOUT_MAX
} CSD_DEV_MBHC_PINPOUT;

/** @} */ /* end_name Pinout Impedence */

/**
  @brief Payload of #CSD_DEV_MBHC_EVT_ACCESSORY_INSERT,
    #CSD_DEV_MBHC_EVT_ACCESSORY_REMOVE
*/
struct csd_dev_mbhc_evt_accessory_payload
{
    uint32_t type;                  /**< Accessory type or ID. See 
                                         CSD_MBHC_ACCESSORY for list of 
                                         supported accessories. */
    struct
    {
        uint32_t tip;               /**< Impedance of tip or PIN1. */
        uint32_t ring1;             /**< Impedance of ring1 or PIN2. */
        uint32_t ring2;             /**< Impedance of ring2 or PIN3. */
        uint32_t sleeve;            /**< Impedance of sleeve or PIN4. */
    } pinout_impedance;             /**< See CSD_MBHC_PINOUT for list of 
                                         supported impedance values. */
};

/** @brief Type definition for csd_mbhc_evt_payload. */
typedef struct csd_dev_mbhc_evt_accessory_payload csd_dev_mbhc_evt_accessory_payload_t;

/**
  @brief Payload of #CSD_DEV_MBHC_EVT_BUTTON_PRESS, #CSD_DEV_MBHC_EVT_BUTTON_RELEASE,
    CSD_DEV_MBHC_EVT_BUTTON_PRESS_RELEASE
*/
struct csd_dev_mbhc_evt_button_payload
{
    uint32_t   id;                  /**< Button ID. The IDs are numbered 
                                         sequentially from lowest to highest in
                                         terms of impedance measurements. */
    uint32_t   duration;            /**< Button press duration in ms. Valid for
                                         #CSD_DEV_MBHC_EVT_BUTTON_RELEASE. */
};

/** @brief Type definition for csd_mbhc_evt_payload. */
typedef struct csd_dev_mbhc_evt_button_payload csd_dev_mbhc_evt_button_payload_t;


/** @} */ /* end_addtogroup csd_dev */

#endif /* _CSD_DEV_MBHC_IOCTL_H_*/
