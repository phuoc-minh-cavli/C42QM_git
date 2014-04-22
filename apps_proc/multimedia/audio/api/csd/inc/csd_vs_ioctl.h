#ifndef __CSD_VS_IOCTL_H__
#define __CSD_VS_IOCTL_H__
/**
  @file  csd_vs_ioctl.h
  @brief Contains voice stream IOCTL definitions.
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
     Qualcomm Confidential and Proprietary.
-----------------------------------------------------------------------------*/
/*=============================================================================
                      EDIT HISTORY FOR MODULE
    This section contains comments describing changes made to this module.
    Notice that changes are listed in reverse chronological order.

    $Header: //components/rel/audio.tx/2.0/audio/api/csd/inc/csd_vs_ioctl.h#1 $
    $DateTime: 2018/12/25 22:39:07 $
    $Author: pwbldsvc $
    $Change: 17929171 $
    $Revision: #1 $
when       who     what, where, why
--------   ---     ------------------------------------------------------------
11/21/12   rp      Updated the comments for possible mute direction values.
06/04/12   sd      (Tech Pubs) Edited/added Doxygen markup and comments.
03/01/12   gl      Remove deprecation tags version 1 of APIs as code base may not
                   support V2s

08/18/11   sr      Added new apis MUTE_V2 , RECORD_V2 , PLAYBACK_V2. Deprecating
                   old apis.
03/15/11   sd      (Tech Pubs) Edited/added Doxygen markup and comments.
=============================================================================*/

/** @addtogroup csd_vs
@{ */

/**
  @name Voice Stream IOCTL Commands
  @{
*/

/****************************************************************************
 * CSD VS SET MEDIA TYPE IOCTL COMMAND                                      *
 ****************************************************************************/

/**
  Sets the vocoder media type on the stream.

  @payload
  #csd_vs_ioctl_set_media_type_t

  @return
  #CSD_VS_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VS_IOCTL_SET_MEDIA_TYPE ( 0x00011186 )

/****************************************************************************
 * CSD VS SET MUTE IOCTL COMMAND                                            *
 ****************************************************************************/

/**
  Sets the mute control to a voice stream. 
   
  @payload
  #csd_vs_ioctl_set_mute_t

  @latexonly \input{tables/CSD_VS_IOCTL_SET_MUTE.tex} @endlatexonly

  @return
  #CSD_VS_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VS_IOCTL_SET_MUTE ( 0x0001114F )

/**
  Sets the smooth mute control to a voice stream.

  @payload
  #csd_vs_ioctl_set_mute_v2_t

  @latexonly \input{tables/CSD_VS_IOCTL_SET_MUTE_V22.tex} @endlatexonly

  @return
  #CSD_VS_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VS_IOCTL_SET_MUTE_V2 ( 0x000112DC )

/****************************************************************************
 * CSD VS COMMON VOCODER IOCTL COMMANDS                                     *
 ****************************************************************************/

/**
  Sets the common encoder DTX mode.

  @payload
  #csd_vs_ioctl_set_encoder_dtx_mode_t

  @return
  #CSD_VS_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VS_IOCTL_SET_ENCODER_DTX_MODE ( 0x0001101D )

/**
  Sets the common decoder time warping parameters. This command can be 
  sent on a per frame basis depending on the compression and expansion
  requirements. 

  @payload
  #csd_vs_ioctl_set_dec_timewarp_t

  @return
  #CSD_VS_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VS_IOCTL_SET_DEC_TIMEWARP ( 0x0001101E )

/****************************************************************************
 * CSD VS CDMA SPECIFFIC VOCODER IOCTL COMMANDS                             *
 ****************************************************************************/

/**
  Sets the CDMA-specific encoder minimum and maximum rate.

  @payload
  #csd_vs_ioctl_set_enc_minmax_rate_t

  @return
  #CSD_VS_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VS_IOCTL_SET_ENC_MINMAX_RATE ( 0x00011019 )

/**
  Sets the CDMA-specific encoder rate modulation.

  @payload
  #csd_vs_ioctl_set_enc_rate_modulation_t

  @return
  #CSD_VS_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VS_IOCTL_SET_ENC_RATE_MODULATION ( 0x0001101C )

/****************************************************************************
 * CSD VS QCELP 13K VOCODER IOCTL COMMAND                                   *
 ****************************************************************************/

/**
  Sets the QCELP 13K encoder rate.

  @payload
  #csd_vs_ioctl_voc_qcelp13k_set_rate_t

  @return
  #CSD_VS_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VS_IOCTL_VOC_QCELP13K_SET_RATE ( 0x0001113B  )

/****************************************************************************
 * CSD VS 4GV-NB VOCODER IOCTL COMMAND                                      *
 ****************************************************************************/

/**
  Sets the 4GV-NB average encoder rate.

  @payload
  #csd_vs_ioctl_voc_4gvnb_set_rate_t

  @return
  #CSD_VS_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VS_IOCTL_VOC_4GVNB_SET_RATE ( 0x0001113C )

/****************************************************************************
 * CSD VS 4GV-WB VOCODER IOCTL COMMAND                                      *
 ****************************************************************************/

/**
  Sets the 4GV-WB average encoder rate.

  @payload
  #csd_vs_ioctl_voc_4gvwb_set_rate_t

  @return
  #CSD_VS_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VS_IOCTL_VOC_4GVWB_SET_RATE ( 0x0001113D )

/****************************************************************************
 * CSD VS AMR VOCODER IOCTL COMMAND                                         *
 ****************************************************************************/

/**
  Sets the adaptive multirate encoder rate.

  @payload
  #csd_vs_ioctl_voc_amr_set_enc_rate_t

  @return
  #CSD_VS_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VS_IOCTL_VOC_AMR_SET_ENC_RATE ( 0x0001113E )

/****************************************************************************
 * CSD VS AMR-WB VOCODER IOCTL COMMAND                                      *
 ****************************************************************************/

/**
  Sets the adaptive multirate codec wideband encoder rate.

  @payload
  #csd_vs_ioctl_voc_amrwb_set_enc_rate_t

  @return
  #CSD_VS_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VS_IOCTL_VOC_AMRWB_SET_ENC_RATE ( 0x0001113F )

/****************************************************************************
 * CSD VS DTMF GENERATION/DETECTION IOCTL COMMANDS                          *
 ****************************************************************************/

/**
  Starts/stops DTMF generation. The completion of DTMF generation (either due 
  to a Stop command or because the requested duration has elapsed) is indicated 
  to the client via a #CSD_VS_EVENTID_DTMF_GENERATION_ENDED event.
 
  Refer to @latexonly \hyperref[S5]{[S5]}@endlatexonly for the tone values.

  @payload
  #csd_vs_ioctl_set_dtmf_generation_t

  @description
  A new DTMF generation command replaces a previously unfinished command
  in the same direction. To generate a single tone, set tone_1 and
  tone_2 parameters to the same value. When closing the stream before the requested
  tone duration has elapsed, the client first stops the currently playing
  tone to ensure proper rampdown and avoid audio artifacts (pops).

  @return
  #CSD_VS_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VS_IOCTL_SET_DTMF_GENERATION ( 0x00011023 )

/**
  Enables/disables Rx DTMF detection. The DTMF detection status is sent to the 
  client sending this command via #CSD_VS_EVENTID_RX_DTMF_DETECTED events.

  @payload
  #csd_vs_ioctl_set_rx_dtmf_detection_t

  @description
  Only one client can request Rx DTMF detection on a given stream at a
  given time. If another client requests Rx DTMF detection while the
  previous client's Rx DTMF detection is still active, the request
  fails.

  @return 
  #CSD_VS_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VS_IOCTL_SET_RX_DTMF_DETECTION ( 0x00011027 )

/****************************************************************************
 * CSD VS SET/GET UI IOCTL COMMANDS                                         *
 ****************************************************************************/

/**
  Sets a UI-controlled property of the voice stream.

  @payload
  #csd_vs_ioctl_set_ui_property_t

  @latexonly \input{tables/CSD_VS_IOCTL_SET_UI_PROPERTY.tex} @endlatexonly

  @return
  #CSD_VS_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VS_IOCTL_SET_UI_PROPERTY ( 0x00011151 )

/**
  Gets a UI-controlled property of the voice stream.

  @payload
  #csd_vs_ioctl_get_ui_property_t

  @return
  #CSD_VS_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VS_IOCTL_GET_UI_PROPERTY ( 0x0001114C )

/** @} */ /* end_name Voice Stream IOCTL Commands */

/****************************************************************************
 * CSD VS SET MEDIA TYPE IOCTL PAYLOAD STRUCTURE                            *
 ****************************************************************************/

/** @brief Payload for #CSD_VS_IOCTL_SET_MEDIA_TYPE. */
struct csd_vs_ioctl_set_media_type_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint32_t rx_media_id;
    /**< Set the Rx vocoder type.  */
  uint32_t tx_media_id;
    /**< Set the Tx vocoder type. */
};

/* Type definition for #csd_vs_ioctl_set_media_type_t. */
typedef struct csd_vs_ioctl_set_media_type_t csd_vs_ioctl_set_media_type_t;

/****************************************************************************
 * CSD VS SET MUTE IOCTL PAYLOAD STRUCTURE                                  *
 ****************************************************************************/

/** @brief Payload for #CSD_VS_IOCTL_SET_MUTE. */
struct csd_vs_ioctl_set_mute_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint16_t direction;
    /**< Direction in which the stream is flowing. Supported values:\n
         - 0    -- Tx only. 
         - 1    -- Rx only. 
         - 2    -- Tx and Rx.
     */
  uint16_t mute_flag;
    /**< Mute status. Supported values:\n
         - 0 -- Unmute. 
         - 1 -- Mute with silence. 
         - 2 -- Mute with comfort noise generation.
     */
};

/* Type definition for #csd_vs_ioctl_set_mute_t. */
typedef struct csd_vs_ioctl_set_mute_t csd_vs_ioctl_set_mute_t;

/** 
  @name Voice Stream MUTE_V2 Definitions
  @{ 
*/

/** Selects the Tx path. */
#define CSD_VS_IOCTL_DIRECTION_TX ( 0 )

/** Selects the Rx path. */
#define CSD_VS_IOCTL_DIRECTION_RX ( 1 )

/** Selects the Tx and Rx paths. */
#define CSD_VS_IOCTL_DIRECTION_TX_RX ( 2 )

/** Turns off the Mute feature. */
#define CSD_VS_IOCTL_MUTE_OFF ( 0 )

/** Turns on the Mute feature. */
#define CSD_VS_IOCTL_MUTE_ON ( 1 )

/** Turns on the Mute Comfort Noise Generation feature. */
#define CSD_VS_IOCTL_MUTE_CNG_ENABLE ( 2 )

/** @} */ /* end_name Voice Context MUTE_V2 Definitions */

/** @brief Payload for #CSD_VS_IOCTL_SET_MUTE_V2. */
struct csd_vs_ioctl_set_mute_v2_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint16_t direction;
    /**< Direction of mute:\n
         - #CSD_VS_IOCTL_DIRECTION_TX -- Tx Mute. 
         - #CSD_VS_IOCTL_DIRECTION_RX -- Rx Mute.
     */
  uint16_t mute_flag;
    /**< Mute status. Supported values:\n
         - #CSD_VS_IOCTL_MUTE_OFF        -- Unmute. 
         - #CSD_VS_IOCTL_MUTE_ON         -- Mute with silence. 
         - #CSD_VS_IOCTL_MUTE_CNG_ENABLE -- Mute with comfort noise generation.
     */
  uint16_t ramp_duration;
      /**< Ramp duration for mute/unmute. Range: 0 to 5000 ms.
      */
};

/* Type definition for #csd_vs_ioctl_set_mute_v2_t. */
typedef struct csd_vs_ioctl_set_mute_v2_t csd_vs_ioctl_set_mute_v2_t;

/****************************************************************************
 * CSD VS COMMON VOCODER IOCTL PAYLOAD STRUCTURES                           *
 ****************************************************************************/

/** @brief Payload for #CSD_VS_IOCTL_SET_ENCODER_DTX_MODE. */
struct csd_vs_ioctl_set_encoder_dtx_mode_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint32_t enable;
    /**< Toggle DTX on or off:\n
         - 0 -- Disables DTX. 
         - 1 -- Enables DTX. */
};

/* Type definition for #csd_vs_ioctl_set_encoder_dtx_mode_t. */
typedef struct csd_vs_ioctl_set_encoder_dtx_mode_t csd_vs_ioctl_set_encoder_dtx_mode_t;

/** @brief Payload for #CSD_VS_IOCTL_SET_DEC_TIMEWARP. */
struct csd_vs_ioctl_set_dec_timewarp_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint16_t enable_time_warp;
    /**< Toggle time warping on or off:\n
         - 0x0000 -- Disable. 
         - 0x0001 -- Enable. */
  uint16_t factor;
    /**< Sets the playback compression and expansion factor. This factor is
         also known as the time warping expansion length. Supported values:

         Narrowband:\n
         - 80 to 160 -- Compression. 
         - 160 to 320 -- Expansion. 

         Wideband:\n
         - 160 to 320 -- Compression. 
         - 320 to 640 -- Expansion. */
  uint16_t enable_phase_match;
    /**< Toggle phase matching on or off. Supported values:\n
         - 0x0000 -- Disable. 
         - 0x0001 -- Enable.      */
  uint16_t run_length;
    /**< Run length is equal to the number of consecutive erasures the
         decoder has decoded immediately prior to the decoding of the current
         packet. Supported values: > 0.  */
  int16_t phase_offset;
    /**< Phase offset is equal to the difference between the number of
         frames encoded and decoded. Supported values: -2, -1, 0, 1, 2. */
};

/* Type definition for #csd_vs_ioctl_set_dec_timewarp_t. */
typedef struct csd_vs_ioctl_set_dec_timewarp_t csd_vs_ioctl_set_dec_timewarp_t;

/****************************************************************************
 * CSD VS CDMA SPECIFFIC VOCODER IOCTL PAYLOAD STRUCTURES                   *
 ****************************************************************************/

/** @brief Payload for #CSD_VS_IOCTL_SET_ENC_MINMAX_RATE. */
struct csd_vs_ioctl_set_enc_minmax_rate_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  int16_t min_rate;
    /**< Sets the lower bound encoder rate. Supported values:\n
         - 0x0000 -- Blank frame. 
         - 0x0001 -- Eighth rate. 
         - 0x0002 -- Quarter rate. 
         - 0x0003 -- Half rate. 
         - 0x0004 -- Full rate.      */
  uint16_t max_rate;
    /**< Sets the upper bound encoder rate Supported values:\n
         - 0x0000 -- Blank frame. 
         - 0x0001 -- Eighth rate. 
         - 0x0002 -- Quarter rate. 
         - 0x0003 -- Half rate. 
         - 0x0004 -- Full rate.      */
};

/* Type definition for #csd_vs_ioctl_set_enc_minmax_rate_t. */
typedef struct csd_vs_ioctl_set_enc_minmax_rate_t csd_vs_ioctl_set_enc_minmax_rate_t;

/** @brief Payload for #CSD_VS_IOCTL_SET_ENC_RATE_MODULATION. */
struct csd_vs_ioctl_set_enc_rate_modulation_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint32_t mode;
    /**< Sets the vocoder reduced rate modulation mode.

     The mode's bit structure is:

     @verbatim
           b0    = Vocoder rate modulation is enabled when 1 and disabled
                   when 0.
           b1    = Select X=S when 1 and select X=1/S when 0.
         b9..b2  = Rate limit factor is the value of S.
        b31..b10 = Reserved; keep as 0's.
     @endverbatim
     The equation for the vocoder rate modulation ratio is:
     @verbatim
                  2X+1
        ratio = --------
                 2(X+1)
     @endverbatim
     where X is S or 1/S depending on the option selected. S is the
     rate limit factor. S is an integer that causes the ratio, when
     multiplied by the 14.4 kbps (full rate), to become the desired
     average bitrate. \n

     For convenience some pre-calculated modes for EVRC are: \n

     @verbatim

        0x00000000 : Target 14.4 kbps (8/8 rate) on the average.
                     Bit values OR'ed:
                     0x00000000 : Vocoder rate modulation disabled.

        0x0000000F : Target 12.2 kbps (7/8 rate) on the average.
                     Bit values OR'ed:
                     0x00000001 : Vocoder rate modulation enabled.
                     0x00000002 : Select X=S.
                     0x0000000C : Rate limit factor S=3.

        0x00000007 : Target 11.2 kbps (6/8 rate) on the average.
                     Bit values OR'ed:
                     0x00000001 : Vocoder rate modulation enabled.
                     0x00000002 : Select X=S.
                     0x00000004 : Rate limit factor S=1.

        0x00000005 : Target 9.0 kbps (5/8 rate) on the average.
                     Bit values OR'ed:
                     0x00000001 : Vocoder rate modulation enabled.
                     0x00000000 : Select X=1/S.
                     0x00000004 : Rate limit factor S=3.

        0x00000003 : Target 7.2 kbps (4/8 rate) on the average.
                    (1/2 rate is not supported for EVRC; must use 0x0000).
                     Bit values OR'ed:
                     0x00000001 : Vocoder rate modulation enabled.
                     0x00000001 : Select X=S.
                     0x00000000 : Rate limit factor S=0.
     @endverbatim
    */
};

/* Type definition for #csd_vs_ioctl_set_enc_rate_modulation_t. */
typedef struct csd_vs_ioctl_set_enc_rate_modulation_t csd_vs_ioctl_set_enc_rate_modulation_t;

/****************************************************************************
 * CSD VS QCELP 13K VOCODER IOCTL PAYLOAD STRUCTURE                         *
 ****************************************************************************/

/** @brief Payload for #CSD_VS_IOCTL_VOC_QCELP13K_SET_RATE. */
struct csd_vs_ioctl_voc_qcelp13k_set_rate_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint32_t rate;
    /**< Sets the vocoder rate. Supported values:\n
         - 0x00000000 -- 14.4 kbps. 
         - 0x00000001 -- 12.2 kbps. 
         - 0x00000002 -- 11.2 kbps. 
         - 0x00000003 --  9.0 kbps. 
         - 0x00000004 --  7.2 kbps. */
};

/* Type definition for #csd_vs_ioctl_voc_qcelp13k_set_rate_t. */
typedef struct csd_vs_ioctl_voc_qcelp13k_set_rate_t csd_vs_ioctl_voc_qcelp13k_set_rate_t;

/****************************************************************************
 * CSD VS 4GV-NB VOCODER IOCTL PAYLOAD STRUCTURE                            *
 ****************************************************************************/

/** @brief Payload for #CSD_VS_IOCTL_VOC_4GVNB_SET_RATE. */
struct csd_vs_ioctl_voc_4gvnb_set_rate_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint32_t rate;
    /**< Sets the vocoder rate:\n
         - 0x00000000 -- 10.0 kbps. 
         - 0x00000001 --  8.5 kbps. 
         - 0x00000002 --  7.5 kbps. 
         - 0x00000003 --  7.0 kbps. 
         - 0x00000004 --  6.6 kbps. 
         - 0x00000005 --  6.2 kbps. 
         - 0x00000006 --  5.8 kbps. 
         - 0x00000007 --  4.8 kbps.  */
};

/* Type definition for #csd_vs_ioctl_voc_4gvnb_set_rate_t. */
typedef struct csd_vs_ioctl_voc_4gvnb_set_rate_t csd_vs_ioctl_voc_4gvnb_set_rate_t;

/****************************************************************************
 * CSD VS 4GV-WB VOCODER IOCTL PAYLOAD STRUCTURE                            *
 ****************************************************************************/

/** @brief Payload for #CSD_VS_IOCTL_VOC_4GVWB_SET_RATE. */
struct csd_vs_ioctl_voc_4gvwb_set_rate_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint32_t rate;
    /**< Sets the vocoder rate:\n
         - 0x00000000 --  8.5 kbps. 
         - 0x00000004 -- 10.0 kbps. 
         - 0x00000007 --  4.8 kbps. */
};

/* Type definition for #csd_vs_ioctl_voc_4gvwb_set_rate_t. */
typedef struct csd_vs_ioctl_voc_4gvwb_set_rate_t csd_vs_ioctl_voc_4gvwb_set_rate_t;

/****************************************************************************
 * CSD VS AMR VOCODER IOCTL PAYLOAD STRUCTURE                               *
 ****************************************************************************/

/** @brief Payload for #CSD_VS_IOCTL_VOC_AMR_SET_ENC_RATE. */
struct csd_vs_ioctl_voc_amr_set_enc_rate_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint32_t mode;
    /**< Sets the AMR encoder rate:\n
         - 0x00000000 -- 4.75 kbps. 
         - 0x00000001 -- 5.15 kbps. 
         - 0x00000002 -- 5.90 kbps. 
         - 0x00000003 -- 6.70 kbps. 
         - 0x00000004 -- 7.40 kbps. 
         - 0x00000005 -- 7.95 kbps. 
         - 0x00000006 -- 10.2 kbps. 
         - 0x00000007 -- 12.2 kbps.  */
};

/* Type definition for #csd_vs_ioctl_voc_amr_set_enc_rate_t. */
typedef struct csd_vs_ioctl_voc_amr_set_enc_rate_t csd_vs_ioctl_voc_amr_set_enc_rate_t;

/****************************************************************************
 * CSD VS AMR-WB VOCODER IOCTL PAYLOAD STRUCTURE                            *
 ****************************************************************************/

/** @brief Payload for #CSD_VS_IOCTL_VOC_AMRWB_SET_ENC_RATE. */
struct csd_vs_ioctl_voc_amrwb_set_enc_rate_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint32_t mode;
    /**< Sets the AMR-WB encoder rate:\n
         - 0x00000000 --  6.60 kbps. 
         - 0x00000001 --  8.85 kbps. 
         - 0x00000002 -- 12.65 kbps. 
         - 0x00000003 -- 14.25 kbps. 
         - 0x00000004 -- 15.85 kbps. 
         - 0x00000005 -- 18.25 kbps. 
         - 0x00000006 -- 19.85 kbps. 
         - 0x00000007 -- 23.05 kbps. 
         - 0x00000008 -- 23.85 kbps.  */
};

/* Type definition for #csd_vs_ioctl_voc_amrwb_set_enc_rate_t. */
typedef struct csd_vs_ioctl_voc_amrwb_set_enc_rate_t csd_vs_ioctl_voc_amrwb_set_enc_rate_t;

/****************************************************************************
 * CSD VS DTMF GENERATION/DETECTION IOCTL PAYLOAD STRUCTURE                 *
 ****************************************************************************/

/**
  @name Voice stream DTMF definitions
  @{
*/

/** DTMF generation direction. */
#define CSD_VS_DTMF_GENERATION_DIRECTION_TX ( 0 )

/** DTMF generation mix with speech disable flag. */
#define CSD_VS_DTMF_GENERATION_MIX_WITH_SPEECH_DISABLE ( 0 )

/** DTMF generation mix with speech enable flag. */
#define CSD_VS_DTMF_GENERATION_MIX_WITH_SPEECH_ENABLE ( 1 )

/** DTMF detection enable flag. */
#define CSD_VS_RX_DTMF_DETECTION_ENABLE ( 1 )

/** DTMF detection disable flag. */
#define CSD_VS_RX_DTMF_DETECTION_DISABLE ( 0 )

/** @} */ /* end_name Voice stream DTMF definitions */

/** @brief Payload for #CSD_VS_IOCTL_SET_DTMF_GENERATION. */
struct csd_vs_ioctl_set_dtmf_generation_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint16_t direction;
    /**< DTMF generation direction. Supported values: 
         #CSD_VS_DTMF_GENERATION_DIRECTION_TX.

         @note1hang Rx DTMF generation is available on the audio stream side 
         using the #CSD_AS_CMD_DTMF_CTL command.   */
  uint16_t mix_flag;
    /**< Mix with speech flag. Supported values:\n
         - 1 -- Generated DTMF is mixed with the speech.
         - 0 -- Generated DTMF replaces the speech. */
  uint16_t tone_1;
    /**< DTMF tone 1. Supported values: 100 to 4000 Hz. */
  uint16_t tone_2;
    /**< DTMF tone 2. Supported values: 100 to 4000 Hz. */
  uint16_t gain;
    /**< DTMF tone gain. Supported values: Linear value in Q13 format. This 
         value must be set to a negative gain because the level of tone 
         generation is fixed at 0 dBFS. */
  int16_t duration;
    /**< Duration of the tone. Duration includes ramp-up and ramp-down periods.
         The ramp-up and ramp-down periods are 1 ms and 2 ms, respectively. 
         Supported values:\n
         - -1 -- Infinite duration. The client sends 0 (stops the infinite tone) 
                 duration to end the tone. 
         -  0 -- Stops the infinite tone. 
         - > 0 -- Finite duration in milliseconds.     */
};

/* Type definition for #csd_vs_ioctl_set_dtmf_generation_t. */
typedef struct csd_vs_ioctl_set_dtmf_generation_t csd_vs_ioctl_set_dtmf_generation_t;

/** @brief Payload for #CSD_VS_IOCTL_SET_RX_DTMF_DETECTION. */
struct csd_vs_ioctl_set_rx_dtmf_detection_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint32_t enable;
    /**< Enables/disables Rx DTMF detection. Supported values:\n
         - 1 -- Enables Rx DTMF detection.
         - 0 -- Disables Rx DTMF detection.  */
};

/* Type definition for #csd_vs_ioctl_set_rx_dtmf_detection_t. */
typedef struct csd_vs_ioctl_set_rx_dtmf_detection_t csd_vs_ioctl_set_rx_dtmf_detection_t;

/****************************************************************************
 * CSD VS SET/GET UI IOCTL PAYLOAD STRUCTURE                            *
 ****************************************************************************/

/** @brief Payload for #CSD_VS_IOCTL_SET_UI_PROPERTY. */
struct csd_vs_ioctl_set_ui_property_t
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

/* Type definition for #csd_vs_ioctl_set_ui_property_t. */
typedef struct csd_vs_ioctl_set_ui_property_t csd_vs_ioctl_set_ui_property_t;

/** @brief Payload for #CSD_VS_IOCTL_GET_UI_PROPERTY. */
struct csd_vs_ioctl_get_ui_property_t
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

/* Type definition for #csd_vs_ioctl_get_ui_property_t. */
typedef struct csd_vs_ioctl_get_ui_property_t csd_vs_ioctl_get_ui_property_t;


/****************************************************************************
 * In-call voice recording APIs                                             *
 ****************************************************************************/

/**
  Starts recording the conversation based on the specified direction of the 
  recording. The Rx path of the conversation is available on 
  the device with the CSD_OEM_IN_CALL_VOICE_RECORD_RX device ID. The Tx path of 
  the conversation is available on the device with the 
  CSD_OEM_IN_CALL_VOICE_RECORD_TX device ID. 
  @note For a list of OEM device IDs, see enum csd_oem_device_id. 

  Any changes to rx_tap_point or 
  tx_tap_point requires #CSD_VS_IOCTL_STOP_RECORD to be called before the 
  changes can be applied using a new call to #CSD_VS_IOCTL_START_RECORD with 
  modified values. The voice recording is performed from the audio path by calling
  csd_read() using a properly configured audio stream handle. 

  @payload
  #csd_vs_ioctl_start_record_t

  @return
  #CSD_VS_EVENTID_COMMAND_RSP

  @dependencies
  The recording must be stopped before sending this command.
*/
#define CSD_VS_IOCTL_START_RECORD ( 0x00011232 )

/**
  Starts recording the conversation based on the specified direction of the 
  recording. The recorded voice will be available on the provided device ID.
  The Rx path of the conversation is available on the device with the 
  CSD_OEM_IN_CALL_VOICE_RECORD_RX device ID. The Tx path of the conversation is 
  available on the device with the CSD_OEM_IN_CALL_VOICE_RECORD_TX device ID. 
  Any changes to rx_tap_point or tx_tap_point requires 
  #CSD_VS_IOCTL_STOP_RECORD to be called before the changes can be applied 
  using a new call to #CSD_VS_IOCTL_START_RECORD_V2 with modified values. The 
  voice recording is performed from the audio path by the csd_read() command 
  using a properly configured audio stream handle based on two modes: 
  #CSD_VS_RECORD_MODE_TX_RX_STEREO and #CSD_VS_RECORD_MODE_TX_RX_MIXING.
  The recommended mode is #CSD_VS_RECORD_MODE_TX_RX_STEREO.

  @payload
  #csd_vs_ioctl_start_record_v2_t

  @return
  #CSD_VS_EVENTID_COMMAND_RSP

  @dependencies
  The recording must be stopped before sending this command.
*/
#define CSD_VS_IOCTL_START_RECORD_V2 ( 0x000112DD )

/** Indicates no tapping for the specified path. */
#define CSD_VS_TAP_POINT_NONE ( 0x00010F78 )

/** Indicates that the specified path is tapped at the end of the stream. */
#define CSD_VS_TAP_POINT_STREAM_END ( 0x00010F79 )

/** Indicates that recorded audio is in the L,R format; separate Left and Right 
  channels. */
#define CSD_VS_RECORD_MODE_TX_RX_STEREO ( 0x00010F7A )

/** Indicates that the recorded audio is L+R format; mixed Left and Right 
  channels. */
#define CSD_VS_RECORD_MODE_TX_RX_MIXING ( 0x00010F7B )

/* Type definition for #csd_vs_ioctl_start_record_t. */
typedef struct csd_vs_ioctl_start_record_t csd_vs_ioctl_start_record_t;

/** @brief Payload for #CSD_VS_IOCTL_START_RECORD. */
struct csd_vs_ioctl_start_record_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/

  uint32_t rx_tap_point;
    /**< Tap point to use on the Rx path. Supported values:\n
         - #CSD_VS_TAP_POINT_NONE -- Do not record the Rx path.
         - #CSD_VS_TAP_POINT_STREAM_END -- Rx tap point is at the end of the 
           stream. */
  uint32_t tx_tap_point;
    /**< Tap point to use on the Tx path. Supported values:\n
         - #CSD_VS_TAP_POINT_NONE -- Do not record the Tx path.
         - #CSD_VS_TAP_POINT_STREAM_END -- Tx tap point is at the end of the 
           stream. */
};

/* Type definition for #csd_vs_ioctl_start_record_v2_t. */
typedef struct csd_vs_ioctl_start_record_v2_t csd_vs_ioctl_start_record_v2_t;


/** @brief Payload for #CSD_VS_IOCTL_START_RECORD_V2. */
struct csd_vs_ioctl_start_record_v2_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/

  uint32_t rx_tap_point;
    /**< Tap point to use on the Rx path. Supported values:\n
         - #CSD_VS_TAP_POINT_NONE -- Do not record the Rx path.
         - #CSD_VS_TAP_POINT_STREAM_END -- Rx tap point is at the end of the 
           stream. */
  uint32_t tx_tap_point;
    /**< Tap point to use on the Tx path. Supported values:\n
         - #CSD_VS_TAP_POINT_NONE -- Do not record the Tx path.
         - #CSD_VS_TAP_POINT_STREAM_END -- Tx tap point is at the end of the 
           stream. */
  uint32_t dev_id;
    /**< Conversation data is available on the recording device ID.  \n
         Data will be routed to the AFE port of the device ID mentioned. */
  uint32_t mode;
    /**< Recording Mode. Supported values \n 
       #CSD_VS_RECORD_MODE_TX_RX_STEREO -- (L,R) format is recorded from AFE.\n
       #CSD_VS_RECORD_MODE_TX_RX_MIXING -- (L+R) format is recorded from AFE.\n
      */
};

/**
  Stops recording the conversation.

  @payload
  The client can provide a uint32_t token to track this command.

  @return
  #CSD_VS_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VS_IOCTL_STOP_RECORD ( 0x00011233 )

/****************************************************************************
 * In-call playback APIs                                                    *
 ****************************************************************************/

/**
  Starts the playback injected into the voice call. This command starts the 
  playback being played on the CSD_OEM_IN_CALL_MUSIC_DELIVER 
  device that is injected into the Tx path of the conversation on the voice 
  call. The device is listed as an Rx device because the audio buffers are 
  being delivered for playback. 

  @payload
  The client can provide a uint32_t token to track this command.

  @return
  #CSD_VS_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VS_IOCTL_START_PLAYBACK ( 0x00011234 )

/**
  Starts the injection of playback into the voice call. The plaback on the 
  device ID provided is injected into the Tx path of the conversation on the 
  voice call. The device is listed as an Rx device because the audio buffers 
  are being delivered for playback. 

  @payload
  #csd_vs_ioctl_start_playback_v2_t 

  @return
  #CSD_VS_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VS_IOCTL_START_PLAYBACK_V2 ( 0x000112DE )


/* Type definition for #csd_vs_ioctl_start_playback_v2_t  */ 
typedef struct csd_vs_ioctl_start_playback_v2_t csd_vs_ioctl_start_playback_v2_t; 

/** @brief Payload for #CSD_VS_IOCTL_START_PLAYBACK_V2. */
struct csd_vs_ioctl_start_playback_v2_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint32_t dev_id;
    /**< The audio received at the device ID is delivered to the Tx voice call 
         path. */
};

/**
  Stops the playback injected into the voice call. This command stops injecting 
  the playback into the conversation.

  @payload
  The client can provide a uint32_t token to track this command.

  @return
  #CSD_VS_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VS_IOCTL_STOP_PLAYBACK ( 0x00011235 )

/** @} */ /* end_addtogroup csd_vs */

#endif /* __CSD_VS_IOCTL_H__ */
