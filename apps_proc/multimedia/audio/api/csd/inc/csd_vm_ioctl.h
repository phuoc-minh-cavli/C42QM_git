#ifndef __CSD_VM_IOCTL_H__
#define __CSD_VM_IOCTL_H__
/**
  @file  csd_vm_ioctl.h
  @brief Contains voice manager IOCTL definitions.
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

    $Header: //components/rel/audio.tx/2.0/audio/api/csd/inc/csd_vm_ioctl.h#1 $
    $DateTime: 2018/12/25 22:39:07 $
    $Author: pwbldsvc $
    $Change: 17929171 $
    $Revision: #1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------------
03/13/15   rp      Added modes to CSD HDon API to control the beAMR and 
                   Wide Voice2 enable and disable independently. 
01/23/12   jk      added support for CSD_VM_IOCTL_PAUSE_VOICE API
06/04/12   sd      (Tech Pubs) Edited/added Doxygen markup and comments.
03/01/12   gl      Remove deprecation tags version 1 of APIs as code base may not
                   support V2s
03/15/11   sd      (Tech Pubs) Edited/added Doxygen markup and comments.
=============================================================================*/

/** @addtogroup csd_vm
@{ */

/** 
   @name Voice Manager IOCTL Commands
   @{
*/

/****************************************************************************
 * CSD VM CORE IOCTL COMMANDS                                               *
 ****************************************************************************/

/**
  Attaches a voice stream to the voice manager.

  @payload
  #csd_vm_ioctl_attach_stream_t

  @return
  #CSD_VM_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VM_IOCTL_ATTACH_STREAM ( 0x00012713 )

/**
  Detaches a voice stream from the voice manager.

  @payload
  #csd_vm_ioctl_detach_stream_t

  @return
  #CSD_VM_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VM_IOCTL_DETACH_STREAM ( 0x00012714 )

/**
  Attaches a voice processing context to the voice manager.

  @payload
  #csd_vm_ioctl_attach_context_t

  @return
  #CSD_VM_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VM_IOCTL_ATTACH_CONTEXT ( 0x0001114D )

/**
  Detaches a voice processing context from the voice manager.

  @payload
  #csd_vm_ioctl_detach_context_t

  @return
  #CSD_VM_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VM_IOCTL_DETACH_CONTEXT ( 0x0001114E )

/**
  Starts voice on the voice manager.

  @payload
  uint32_t cmd_token: Transaction identifier provided by the client
  that allows the client to identify which command completed.\n
  @note1hang The upper 8 bits are reserved for internal
  processing.

  @return
  #CSD_VM_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VM_IOCTL_START_VOICE ( 0x000110FA )

/**
  Stops voice on the voice manager.

  @payload
  uint32_t cmd_token: Transaction identifier provided by the client
  that allows the client to identify which command completed.\n
  @note1hang The upper 8 bits are reserved for internal
  processing.

  @return
  #CSD_VM_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VM_IOCTL_STOP_VOICE ( 0x000111A6 )

/**
  Places a voice call into Standby mode using the voice manager.

  @payload
  uint32_t cmd_token: Transaction identifier provided by the client
  that allows the client to identify which command completed.\n
  @note1hang The upper 8 bits are reserved for internal
  processing.

  @return
  #CSD_VM_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VM_IOCTL_STANDBY_VOICE ( 0x00011316 )

/**
  Places a voice call on pause using the voice manager.

  @payload
  uint32_t cmd_token: Transaction identifier provided by the client
  that allows the client to identify which command completed.\n
  @note1hang The upper 8 bits are reserved for internal
  processing.

  @return
  #CSD_VM_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VM_IOCTL_PAUSE_VOICE ( 0x00012E4D )
/****************************************************************************
 * CSD VM NETWORK SYSTEM IOCTL COMMANDS                                     *
 ****************************************************************************/

/**
  Sets the network type on the voice manager.

  @payload
  #csd_vm_ioctl_set_network_t

  @return
  #CSD_VM_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VM_IOCTL_SET_NETWORK ( 0x0001119C )

/**
  Sets the voice timing parameters on the voice manager.

  @payload
  #csd_vm_ioctl_set_voice_timing_t

  @return
  #CSD_VM_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VM_IOCTL_SET_VOICE_TIMING ( 0x000111E0 )

/****************************************************************************
 * CSD VM TELETYPEWRITER (TTY) IOCTL COMMAND                                *
 ****************************************************************************/

/**
  Sets the TTY mode on the voice manager.

  @payload
  #csd_vm_ioctl_set_tty_mode_t

  @return
  #CSD_VM_EVENTID_COMMAND_RSP

  @dependencies
  None.
*/
#define CSD_VM_IOCTL_SET_TTY_MODE ( 0x00011150 )

/****************************************************************************
 * CSD VM WIDEVOICE IOCTL COMMAND                                           *
 ****************************************************************************/

/**
  Sets WideVoice on the voice manager.  

  @payload
  #csd_vm_ioctl_set_widevoice_t

  @return
  #CSD_VM_EVENTID_COMMAND_RSP

  @dependencies
  Calls cannot be active while setting WideVoice.
*/
#define CSD_VM_IOCTL_SET_WIDEVOICE ( 0x00011220 )

/****************************************************************************
 * CSD VM HDVOICE IOCTL COMMAND                                             *
 ****************************************************************************/

/**
  Set HDVoice mode on the voice manager.  

  @payload
  #csd_vm_ioctl_set_hdvoice_mode_t

  @return
  #CSD_VM_EVENTID_COMMAND_RSP

  @dependencies
  None
*/
#define CSD_VM_IOCTL_SET_HDVOICE_MODE ( 0x000130EE )

/** @} */ /* end_name Voice Manager IOCTL Commands */

/****************************************************************************
 * CSD VM CORE IOCTL PAYLOAD STRUCTURES                                     *
 ****************************************************************************/

/** @brief Payload for #CSD_VM_IOCTL_ATTACH_STREAM. */
struct csd_vm_ioctl_attach_stream_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint32_t stream_handle;
    /**< Stream to attach. */
};

/* Type definition for #csd_vm_ioctl_attach_stream_t. */
typedef struct csd_vm_ioctl_attach_stream_t csd_vm_ioctl_attach_stream_t;

/** @brief Payload for #CSD_VM_IOCTL_DETACH_STREAM. */
struct csd_vm_ioctl_detach_stream_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint32_t stream_handle;
    /**< Stream to detach. */
};

/* Type definition for #csd_vm_ioctl_detach_stream_t. */
typedef struct csd_vm_ioctl_detach_stream_t csd_vm_ioctl_detach_stream_t;

/** @brief Payload for #CSD_VM_IOCTL_ATTACH_CONTEXT. */
struct csd_vm_ioctl_attach_context_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint32_t context_handle;
    /**< Voice processing context to attach. */
};

/* Type definition for #csd_vm_ioctl_attach_context_t. */
typedef struct csd_vm_ioctl_attach_context_t csd_vm_ioctl_attach_context_t;

/** @brief Payload for #CSD_VM_IOCTL_DETACH_CONTEXT. */
struct csd_vm_ioctl_detach_context_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint32_t context_handle;
    /**< Voice processing context to detach. */
};

/* Type definition for #csd_vm_ioctl_detach_context_t. */
typedef struct csd_vm_ioctl_detach_context_t csd_vm_ioctl_detach_context_t;

/****************************************************************************
 * CSD VM NETWORK SYSTEM IOCTL PAYLOAD STRUCTURES                           *
 ****************************************************************************/

/** @brief Payload for #CSD_VM_IOCTL_SET_NETWORK. */
struct csd_vm_ioctl_set_network_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint32_t network_id;
    /**< Network ID. */
};

/* Type definition for #csd_vm_ioctl_set_network_t. */
typedef struct csd_vm_ioctl_set_network_t csd_vm_ioctl_set_network_t;

/** @brief Payload for #CSD_VM_IOCTL_SET_VOICE_TIMING.
*/
struct csd_vm_ioctl_set_voice_timing_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/ 
  uint16_t mode;
    /**< Vocoder frame synchronization mode. Supported values:\n
         - 0 -- No frame synchronization. 
         - 1 -- Hard vocoder frame reference interrupt; 20 ms. */
  uint16_t enc_offset;
    /**< Offset in microseconds from the vocoder frame reference to deliver a 
         Tx vocoder packet. The offset must be less than 20 ms. */
  uint16_t dec_req_offset;
    /**< Offset in microseconds from the vocoder frame reference to request for 
         an Rx vocoder packet. The offset must be less than 20 ms. */
  uint16_t dec_offset;
    /**< Offset in microseconds from the vocoder frame reference to indicate 
         the deadline to receive an Rx vocoder packet. The offset must be less 
         than 20 ms. Rx vocoder packets received after this deadline are not 
         guaranteed to be processed. */
};

/* Type definition for #csd_vm_ioctl_set_voice_timing_t. */
typedef struct csd_vm_ioctl_set_voice_timing_t csd_vm_ioctl_set_voice_timing_t;

/****************************************************************************
 * CSD VM TELETYPEWRITER (TTY) IOCTL PAYLOAD STRUCTURE                      *
 ****************************************************************************/

/** @brief Payload for #CSD_VM_IOCTL_SET_TTY_MODE. */
struct csd_vm_ioctl_set_tty_mode_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint32_t mode;
    /**< Mode type. Supported values:\n
         - 0 -- Teletypewriter (TTY) disabled. 
         - 1 -- Hearing Carry Over (HCO). 
         - 2 -- Voice Carry Over (VCO). 
         - 3 -- Full. */
};

/* Type definition for #csd_vm_ioctl_set_tty_mode_t. */
typedef struct csd_vm_ioctl_set_tty_mode_t csd_vm_ioctl_set_tty_mode_t;

/****************************************************************************
 * CSD VM WIDEVOICE IOCTL PAYLOAD STRUCTURE                                 *
 ****************************************************************************/

/** @brief Payload for #CSD_VM_IOCTL_SET_WIDEVOICE. */
struct csd_vm_ioctl_set_widevoice_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint32_t enable;
    /**< WideVoice enable/disable. Supported values:\n
         - 1 -- Enables WideVoice. 
         - 0 -- Disables WideVoice. 
*/
};

/* Type definition for #csd_vm_ioctl_set_widevoice_t. */
typedef struct csd_vm_ioctl_set_widevoice_t csd_vm_ioctl_set_widevoice_t;

/****************************************************************************
 * CSD VM HDVOICE IOCTL PAYLOAD STRUCTURE                                 *
 ****************************************************************************/
#define CSD_VM_IOCTL_HDVOICE_OFF          (0)
#define CSD_VM_IOCTL_HDVOICE_ON           (1)
#define CSD_VM_IOCTL_HDVOICE_ON_WV2_OFF   (2)
#define CSD_VM_IOCTL_HDVOICE_ON_BEAMR_OFF (3)

/** @brief Payload for #CSD_VM_IOCTL_SET_HDVOICE_MODE. */
struct csd_vm_ioctl_set_hdvoice_mode_t
{
  uint32_t cmd_token;
    /**< Transaction identifier provided by the client that allows the client 
         to identify which command completed.\n 
         @note1hang The upper 8 bits are reserved for internal processing.*/
  uint32_t mode;
    /**< HDVoice enable/disable. Supported values:\n
         - 3 -- Enables HDVoice and force BeAMR to be disabled. This setting 
                can be applied only before voice call starts.
         - 2 -- Enables HDVoice and force widevoice2 to be disabled. This 
                setting can be applied only before voice call starts.
         - 1 -- Enables HDVoice. This setting can be applied both before 
                voice call starts and in the middle of a voice call. 
         - 0 -- Disables HDVoice. This setting can be applied both before 
                voice call starts and in the middle of a voice call. 
	*/
};

/* Type definition for #csd_vm_ioctl_set_hdvoice_mode_t. */
typedef struct csd_vm_ioctl_set_hdvoice_mode_t csd_vm_ioctl_set_hdvoice_mode_t;

/** @} */ /* end_addtogroup csd_vm */

#endif /* __CSD_VM_IOCTL_H__ */

