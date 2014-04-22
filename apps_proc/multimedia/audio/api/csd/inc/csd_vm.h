#ifndef __CSD_VM_H__
#define __CSD_VM_H__
/**
  @file  csd_vm.h
  @brief Contains voice manager API definitions.
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

    $Header: //components/rel/audio.tx/2.0/audio/api/csd/inc/csd_vm.h#1 $
    $DateTime: 2018/12/25 22:39:07 $
    $Author: pwbldsvc $
    $Change: 17929171 $
    $Revision: #1 $

when		who			what, where, why
--------	-----		---------------------------------------------------
06/04/12    sd          (Tech Pubs) Edited/added Doxygen markup and comments.
03/15/11    sd          (Tech Pubs) Edited/added Doxygen markup and comments.

=============================================================================*/

#include "mmdefs.h"
#include "csd_vcommon.h"

/** @addtogroup csd_vm
@{ */

/****************************************************************************
 * CSD VM EVENT DEFINITIONS                                                 *
 ****************************************************************************/

/** 
  @name Voice Manager Event Definitions
  @{
*/

/**
  Event callback function type definition.
  
  @param[in] cmd_token  Transaction identifier provided by the 
                        client that allows the client to
                        identify which command completed.\n
                        @note1hang The upper 8 bits are reserved
                        for internal processing.
  @param[in] event_id   ID of the callback event.
  @param[in] param      Payload data of the callback event.
  @param[in] param_size Size of the payload data for the callback event.
  @param[in] cb_data    Callback data.
*/
typedef uint32_t ( *csd_vm_event_callback_fn_t ) (
  uint32_t cmd_token,
  uint32_t event_id,
  void* param,
  uint32_t param_size,
  void* cb_data
);

/**
  Issued by the voice manager to its client in response to an IOCTL that 
  indicates the success or failure status of the command. 

  @payload
  #csd_vm_event_command_rsp_t

  @return
  CSD_EOK -- Success.\n
  CSD_EFAILED -- Otherwise.

  @dependencies
  None.
*/
#define CSD_VM_EVENTID_COMMAND_RSP ( 0x00012710 )

/** @} */ /* end_name Voice Manager Event Definitions */

/****************************************************************************
 * CSD VM OPEN IDS                                                          *
 ****************************************************************************/

/** 
  @name Voice Manager Open IDs
  @{
*/

/**
  Opens a voice manager session for passive control (synchronous).
 
  Passive voice manager sessions provide clients access to UI 
  controls such as TTY toggle and WideVoice toggle, as well as connections
  between streams and contexts. The corresponding full control sessions
  internally manage additional system policies such as network setting and
  voice timing parameter setting.
 
  @payload
  #csd_vm_open_passive_control_t

  @description
  Use the session name "default modem voice" to obtain a handle to the default 
  voice manager session for circuit-switched voice calls. The client passes 
  this ID to the open_id field of #csd_vm_open_t to indicate the opening of a 
  passive control session. 

  @return
  Valid CSD handle on success.

  @dependencies
  None.
*/
#define CSD_VM_OPENID_PASSIVE_CONTROL ( 0x00012711 )

/**
  Opens a voice manager session for full control (synchronous).
 
  Full voice manager sessions allow clients full control over the voice system,
  including:
  - Network configuration
  - Voice timing configuration
  - UI control selection
  - Stream-context connection management

  @payload
  #csd_vm_open_full_control_t

  @description
  The session name "default modem voice" is reserved for the default voice
  manager session for circuit-switched voice calls and cannot be given to any
  new full control sessions. The creation of two or more full control
  voice manager sessions with the same name is not allowed. The client 
  passes this ID to the open_id field of #csd_vm_open_t to indicate the
  opening of a full control session. 

  @return
  Valid CSD handle on success.

  @dependencies
  None.
*/
#define CSD_VM_OPENID_FULL_CONTROL ( 0x00012712 )

/** @} */ /* end_name Voice Manager Open IDs */

/****************************************************************************
 * CSD VM EVENT PAYLOAD STRUCTURE                                           *
 ****************************************************************************/

/** @brief Payload for #CSD_VM_EVENTID_COMMAND_RSP. */
struct csd_vm_event_command_rsp_t
{
  uint32_t status_id; 
    /**< Status ID of the response. */
};

/* Type definition for #csd_vm_event_command_rsp_t. */
typedef struct csd_vm_event_command_rsp_t csd_vm_event_command_rsp_t;

/****************************************************************************
 * CSD VM OPEN PAYLOAD STRUCTURES to be used with #csd_open()               *
 ****************************************************************************/

/** @brief Payload for #CSD_VM_OPENID_PASSIVE_CONTROL. */
struct csd_vm_open_passive_control_t
{
  csd_vm_event_callback_fn_t callback_fn;
    /**< Event callback function. */
  void* cb_data;
    /**< Callback data. */
  const char_t* name;
    /**< Session name. */
  uint32_t name_size;
    /**< Session name size. */
};

/* Type definition for #csd_vm_open_passive_control_t. */
typedef struct csd_vm_open_passive_control_t csd_vm_open_passive_control_t;

/** @brief Payload for #CSD_VM_OPENID_FULL_CONTROL. */
struct csd_vm_open_full_control_t
{
  csd_vm_event_callback_fn_t callback_fn;
    /**< Event callback function. */
  void* cb_data;
    /**< Callback data. */
  const char_t* name;
    /**< Session name. */
  uint32_t name_size;
    /**< Session name size. */
};

/* Type definition for #csd_vm_open_full_control_t. */
typedef struct csd_vm_open_full_control_t csd_vm_open_full_control_t;

/** @brief Payload for #CSD_OPEN_VOICE_MANAGER. 

  This structure is passed into the csd_open() call. */

struct csd_vm_open_t
{
  uint32_t  open_id;
    /**< Voice manager open ID. */
  uint32_t* ret_status;
    /**< Returns the open status. */

  /** Full and passive control structures. */
  union {
    csd_vm_open_full_control_t full_control;        
      /**< Enables full control over streams. This allows clients to manage 
            data exchange and additional UI controls. */
    csd_vm_open_passive_control_t passive_control;  
      /**< Enables passive control over streams. This allows clients to manage 
            UI controls such as muting and volume levels.  */
  }u;
};

/* Type definition for #csd_vm_open_t. */
typedef struct csd_vm_open_t csd_vm_open_t;

/** @} */ /* end_addtogroup csd_vm */

#endif /* __CSD_VM_H__ */

