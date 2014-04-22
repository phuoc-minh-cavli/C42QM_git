#ifndef __CSD_VS_H__
#define __CSD_VS_H__
/**
  @file  csd_vs.h
  @brief Contains voice stream API definitions.
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
     Copyright (C) 2011-2012 Qualcomm Technologies Incorporated.
     All rights reserved.
     Qualcomm Confidential and Proprietary.
-----------------------------------------------------------------------------*/
/*=============================================================================
                      EDIT HISTORY FOR MODULE
    This section contains comments describing changes made to this module.
    Notice that changes are listed in reverse chronological order.

    $Header: //components/rel/audio.tx/2.0/audio/api/csd/inc/csd_vs.h#1 $
    $DateTime: 2018/12/25 22:39:07 $
    $Author: pwbldsvc $
    $Change: 17929171 $
    $Revision: #1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
06/04/12   sd      (Tech Pubs) Edited/added Doxygen markup and comments.
03/15/11   sd      (Tech Pubs) Edited/added Doxygen markup and comments.
=============================================================================*/

#include "mmdefs.h"
#include "csd_vcommon.h"

/** @addtogroup csd_vs
@{ */

/****************************************************************************
 * CSD VS EVENT DEFINITIONS                                                 *
 ****************************************************************************/

/** 
  @name Voice Stream Event Definitions
  @{
*/

/**
  Registers a callback function with the voice stream handle.

  @param[in] cmd_token  Transaction identifier provided by the 
                        client that allows the client to
                        identify which command completed.\n
                        @note1hang The upper 8 bits are reserved
                        for internal processing.
  @param[in] event_id   Callback event ID.
  @param[in] param      Pointer to the payload data of the callback event.
  @param[in] param_size Size of the payload data for the callback event.
  @param[in] cb_data    Pointer to the callback data.
*/
typedef uint32_t ( *csd_vs_event_callback_fn_t ) (
  uint32_t cmd_token,
  uint32_t event_id,
  void* param,
  uint32_t param_size,
  void* cb_data
);

/**
  Callback event ID for the voice stream callback function that indicates the
  callback is a response.

  @payload
  #csd_vs_event_command_rsp_t

  @latexonly \input{tables/CSD_VS_EVENTID_COMMAND_RSP.tex} @endlatexonly

  @return
  CSD_EOK -- Success.\n 
  CSD_EFAILED -- Otherwise.

  @dependencies
  None.
*/
#define CSD_VS_EVENTID_COMMAND_RSP ( 0x0001114A )

/**
  Event issued by the stream to its client that indicates a transition
  to/from the Run state. 

  @payload
  #csd_vs_event_state_notify_t

  @latexonly \input{tables/CSD_VS_EVENTID_STATE_NOTIFY.tex} @endlatexonly

  @return
  None.

  @dependencies
  None.
*/
#define CSD_VS_EVENTID_STATE_NOTIFY ( 0x00011183 )

/** 
  Indicates to the stream client that an encoder buffer is available for 
  pickup via the csd_read() function. The media type of the buffer is the type  
  that was passed to the stream in #CSD_VS_OPENID_FULL_CONTROL or 
  #CSD_VS_IOCTL_SET_MEDIA_TYPE.  

  @payload
  None.

  @return
  None.

  @dependencies
  None.
*/
#define CSD_VS_EVENTID_SIGNAL_ENC_BUFFER ( 0x00012715 )

/**
  Indicates to the stream client that a decoder buffer via the csd_write() 
  function must be provided. The media type of the buffer must be the type that 
  was passed to the stream in #CSD_VS_OPENID_FULL_CONTROL or 
  #CSD_VS_IOCTL_SET_MEDIA_TYPE. 

  @payload
  None.

  @return
  None.

  @dependencies
  None.
*/
#define CSD_VS_EVENTID_SIGNAL_DEC_BUFFER ( 0x00012716 )

/**
  Indicates that the generation of the tones has ended. This event is sent by the 
  stream to the client that enabled DTMF generation when the client issues a 
  Stop command or the duration requested by the client has elapsed.

  @payload
  #csd_vs_event_dtmf_generation_ended_t

  @return
  None.

  @dependencies
  None.
*/
#define CSD_VS_EVENTID_DTMF_GENERATION_ENDED ( 0x00011032 )

/**
  Sent by the stream to the client that enabled Rx DTMF detection when DTMF 
  is detected in the Rx path. 

  @payload
  #csd_vs_event_rx_dtmf_detected_t

  @description
  The DTMF detection feature can only be used to detect DTMF frequencies as 
  listed in ITU-T Recommendation Q.23 
  @latexonly \hyperref[S5]{[S5]}@endlatexonly, Section 6. The supported values 
  for the low_freq and high_freq parameters are provided in 
  #csd_vs_event_rx_dtmf_detected_t.

  @return
  None.

  @dependencies
  None.
*/
#define CSD_VS_EVENTID_RX_DTMF_DETECTED ( 0x0001101A )

/** @} */ /* end_name Voice Stream Event Definitions */

/****************************************************************************
 * CSD VS OPEN IDS                                                          *
 ****************************************************************************/

/** 
  @name Voice Stream Open IDs
  @{
*/

/**
  Opens a voice stream for passive control (synchronous).
 
  @payload
  #csd_vs_open_passive_control_t

  @latexonly \input{tables/CSD_VS_OPENID_PASSIVE_CONTROL.tex} @endlatexonly
  
  @description
  Passive stream sessions are intended for clients to manage UI controls
  such as muting and volume levels while the corresponding full control
  sesssions manage data exchange.
  @par
  This command uses the session name "default modem voice" to obtain a handle 
  to the default stream session for circuit-switched voice calls. The client 
  passes this ID to the open_id field of #csd_vs_open_t to indicate the
  opening of a passive control session.

  @return
  Valid CSD handle on success. 

  @dependencies
  None.
*/
#define CSD_VS_OPENID_PASSIVE_CONTROL ( 0x0001114B )

/**
  Opens a voice stream for full control (synchronous).
 
  Full stream sessions allow clients full control over the stream including
  data exchange in addition to UI controls.
   
  @payload
  #csd_vs_open_full_control_t
  
  @description
  The session name "default modem voice" is reserved for the default
  stream session for circuit-switched voice calls and cannot be given to
  any new full control sessions. The creation of two or more full control
  stream sessions with the same name is not allowed. The client 
  passes this ID to the open_id field of #csd_vs_open_t to indicate the
  opening of a full control session. 

  @return
  Valid CSD handle on success.

  @dependencies
  None.
*/
#define CSD_VS_OPENID_FULL_CONTROL ( 0x00012717 )

/** @} */ /* end_name Voice Stream Open IDs */

/****************************************************************************
 * CSD VS EVENT PAYLOAD STRUCTURES                                          *
 ****************************************************************************/

/**
  @brief Payload for #CSD_VS_EVENTID_COMMAND_RSP.
*/
struct csd_vs_event_command_rsp_t
{
  uint32_t status_id; 
    /**< Status ID of the response. */
};

/* Type definition for #csd_vs_event_command_rsp_t. */
typedef struct csd_vs_event_command_rsp_t csd_vs_event_command_rsp_t;

/** @brief Payload for #CSD_VS_EVENTID_STATE_NOTIFY. */
struct csd_vs_event_state_notify_t
{
  uint32_t state_id; 
    /**< Current state of the voice stream. */
};

/* Type definition for #csd_vs_event_state_notify_t. */
typedef struct csd_vs_event_state_notify_t csd_vs_event_state_notify_t;

/** @brief Payload for #CSD_VS_EVENTID_DTMF_GENERATION_ENDED. */
struct csd_vs_event_dtmf_generation_ended_t
{
  uint16_t direction;
     /**< Direction in which the DTMF tone has been generated.
          Supported values:\n
          #CSD_VS_DTMF_GENERATION_DIRECTION_TX */
};

/* Type definition for #csd_vs_event_dtmf_generation_ended_t. */
typedef struct csd_vs_event_dtmf_generation_ended_t csd_vs_event_dtmf_generation_ended_t;

/** @brief Payload for #CSD_VS_EVENTID_RX_DTMF_DETECTED. */
struct csd_vs_event_rx_dtmf_detected_t
{
  uint16_t low_freq;
    /**< Detected low frequency in Hz. Supported values:\n
         - 697 Hz
         - 770 Hz 
         - 852 Hz 
         - 941 Hz   */
  uint16_t high_freq;
    /**< Detected high frequency in Hz. Supported values:\n 
         - 1209 Hz 
         - 1336 Hz 
         - 1477 Hz 
         - 1633 Hz  */
};

/* Type definition for #csd_vs_event_rx_dtmf_detected_t. */
typedef struct csd_vs_event_rx_dtmf_detected_t csd_vs_event_rx_dtmf_detected_t;

/****************************************************************************
 * CSD VS OPEN PAYLOAD STRUCTURES to be used with csd_open()                *
 ****************************************************************************/


/** @brief Payload for #CSD_VS_OPENID_PASSIVE_CONTROL. */
struct csd_vs_open_passive_control_t
{
  csd_vs_event_callback_fn_t callback_fn;
    /**< Event callback function. */
  void* cb_data;
    /**< Callback data. */
  const char_t* name;
    /**< Session name. */
  uint32_t name_size;
    /**< Session name size. */
};

/* Type definition for #csd_vs_open_passive_control_t. */
typedef struct csd_vs_open_passive_control_t csd_vs_open_passive_control_t;

/** @brief Payload for #CSD_VS_OPENID_FULL_CONTROL. */
struct csd_vs_open_full_control_t
{
  csd_vs_event_callback_fn_t callback_fn;
    /**< Event callback function. */
  void* cb_data;
    /**< Callback data. */
  const char_t* name;
    /**< Session name. */
  uint32_t name_size;
    /**< Session name size. */
  uint16_t direction;
    /**< Direction in which the stream is flowing. Supported values:\n
         - 0 -- Tx only. 
         - 1 -- Rx only. 
         - 2 -- Tx and Rx. 
         - 3 -- Tx and Rx loopback. */
  uint32_t enc_media_type;
    /**< Tx vocoder type.  */
  uint32_t dec_media_type;
    /**< Rx vocoder type.  */
  uint32_t network_id;
     /**< Network ID (see @latexonly
          \hyperref[network_ID]{Network IDs}@endlatexonly). */
};

/* Type definition for #csd_vs_open_full_control_t. */
typedef struct csd_vs_open_full_control_t csd_vs_open_full_control_t;

/** @brief Payload for #CSD_OPEN_VOICE_STREAM. 

  This structure is passed into the csd_open() call. */
struct csd_vs_open_t
{
  uint32_t  open_id;
    /**< Voice stream open ID. */
  uint32_t* ret_status;
    /**< Returns the Open status. */

  /** Full and passive control structures. */
  union {
    csd_vs_open_full_control_t    full_control;    
      /**< Enables full control over voice streams. This allows clients to 
           manage data exchange and additional UI controls. */
    csd_vs_open_passive_control_t passive_control;
      /**< Enables passive control over voice streams. This allows clients to 
           manage UI controls such as muting and volume levels. */
  }u;
};

/* Type definition for #csd_vs_open_t. */
typedef struct csd_vs_open_t csd_vs_open_t;

/** @brief Payload for #csd_read(). */
struct csd_vs_read_t
{
  uint32_t* pmedia_id;
    /**< Media ID of the packet. */
  uint8_t* pbuf;
    /**< Pointer to the buffer. */
  uint32_t buf_size;
    /**< Size of the buffer to be filled. */
};

/* Type definition for #csd_read() for VS. */
typedef struct csd_vs_read_t csd_vs_read_t;

/** @brief Payload for #csd_write() for VS. */
struct csd_vs_write_t
{
  uint32_t media_id;
    /**< Media ID of the packet. */
  const uint8_t* pbuf;
    /**< Pointer to the buffer. */
  uint32_t buf_size;
    /**< Size of the buffer to be written. */
};

/* Type definition for #csd_write for VS. */
typedef struct csd_vs_write_t csd_vs_write_t;

/** @} */ /* end_addtogroup csd_vs */

#endif /* __CSD_VS_H__ */

