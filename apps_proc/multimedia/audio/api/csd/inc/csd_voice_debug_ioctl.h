#ifndef __CSD_VOICE_DEBUG_IOCTL_H__
#define __CSD_VOICE_DEBUG_IOCTL_H__

/**
    @file  csd_voice_debug_ioctl.h
    @brief Contains voice debug IOCTL definitions.
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

    $Header: //components/rel/audio.tx/2.0/audio/api/csd/inc/csd_voice_debug_ioctl.h#1 $
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
/** @cond
*/

/****************************************************************************
 * CSD VOICE DEBUG IOCTL COMMANDS                                           *
 ****************************************************************************/

/** 
   @name Voice Debug IOCTL Commands
   @{ 
*/

/**
  Gets the voice stream sessions currently in the system.
 
  @payload
  #csd_debug_voice_ioctl_get_stream_list

  @return
  CSD_EOK -- Success.\n
  Error code on failure.

  @dependencies
  None.
*/
#define CSD_DEBUG_VOICE_IOCTL_GET_STREAM_LIST ( 0x0001126D )

/**
  Gets the voice context sessions attached to a specific voice
  stream. 
 
  @payload
  #csd_debug_voice_ioctl_get_context_list

  @return
  CSD_EOK -- Success.\n
  Error code on failure.

  @dependencies
  None.
*/
#define CSD_DEBUG_VOICE_IOCTL_GET_CONTEXT_LIST ( 0x0001126E )

/**
  Retrieves the device IDs for a given voice context.
 
  @payload
  #csd_debug_voice_ioctl_get_device_ids

  @return
  CSD_EOK -- Success.\n
  Error code on failure.

  @dependencies
  None.
*/
#define CSD_DEBUG_VOICE_IOCTL_GET_DEVICE_IDS ( 0x0001126F )
/** @} */ /* end_name_group Voice Debug IOCTL Commands */

/**
  Retrieves the internal CVP handle given a CSD_VC handle.

  @return
  CSD_EOK -- Success.\n
  Error code on failure.

  @dependencies
  None.
*/
#define CSD_DEBUG_VOICE_IOCTL_GET_CVP_HANDLE ( 0x00012A7D )

typedef struct csd_debug_voice_ioctl_get_cvp_handle_t csd_debug_voice_ioctl_get_cvp_handle_t;
struct csd_debug_voice_ioctl_get_cvp_handle_t
{
  uint32_t  csd_vc_handle;
  uint16_t* ret_cvp_handle;
};

/**
  Gets the internal CVS handle from the CSD_VS handle.

  @return
  CSD_EOK -- Success.\n
  Error code on failure.

  @dependencies
  None.
*/
#define CSD_DEBUG_VOICE_IOCTL_GET_CVS_HANDLE ( 0x00012A7C )

typedef struct csd_debug_voice_ioctl_get_cvs_handle_t csd_debug_voice_ioctl_get_cvs_handle_t;
struct csd_debug_voice_ioctl_get_cvs_handle_t
{
  uint32_t  csd_vs_handle;
  uint16_t* ret_cvs_handle;
};

/**
    @brief Payload structure for #CSD_DEBUG_VOICE_IOCTL_GET_STREAM_LIST.
*/
struct csd_debug_voice_ioctl_get_stream_list_t
{
  uint32_t* buffer;
    /**< Buffer to be filled. 
  @verbatim
  <----- 32 bits ----->
   -------------------
  | csd vs handle     |   
   -------------------
  | csd vs handle     |   
   -------------------    
  |                   |   
  .                   .
  .                   . 
  @endverbatim
     */
  uint32_t buffer_size;
    /**< Size of the buffer in bytes. */
  uint32_t* ret_write_size;
    /**< Number of bytes written. */
  uint32_t* ret_total_size;
    /**< Number of bytes required. */
};

/* Type definition for #csd_debug_voice_ioctl_get_stream_list_t. */
typedef struct csd_debug_voice_ioctl_get_stream_list_t csd_debug_voice_ioctl_get_stream_list_t;

/**
  @brief Payload for #CSD_DEBUG_VOICE_IOCTL_GET_CONTEXT_LIST.
*/
struct csd_debug_voice_ioctl_get_context_list_t
{
  uint32_t csd_vs_handle;
    /**< CSD voice stream handle. */
  uint32_t* buffer;
    /**< Buffer to be filled. 
  @verbatim
  <----- 32 bits ----->
   -------------------
  | csd vs handle     |   
   -------------------
  | csd vs handle     |   
   -------------------    
  |                   |   
  .                   .
  .                   . 
  @endverbatim
     */
  uint32_t buffer_size;
    /**< Size of the buffer in bytes. */
  uint32_t* ret_write_size;
    /**< Number of bytes written. */
  uint32_t* ret_total_size;
    /**< Number of bytes required. */
};

/* Type definition for #csd_debug_voice_ioctl_get_context_list_t. */
typedef struct csd_debug_voice_ioctl_get_context_list_t csd_debug_voice_ioctl_get_context_list_t;

/**
  @brief Payload for #CSD_DEBUG_VOICE_IOCTL_GET_DEVICE_IDS.
*/
struct csd_debug_voice_ioctl_get_device_ids_t
{
  uint32_t csd_vc_handle;
    /**< CSD voice context handle. */
  uint32_t* ret_rx_dev_id;
    /**< Rx device ID. */
  uint32_t* ret_tx_dev_id;
    /**< Tx device ID. */
};

/* Type definition for #csd_debug_voice_ioctl_get_device_ids_t. */
typedef struct csd_debug_voice_ioctl_get_device_ids_t csd_debug_voice_ioctl_get_device_ids_t;

/**
  Gets the AFE Tx, Rx and EC Ref ports from the CSD_VC handle.

  @return
  CSD_EOK -- Success.\n
  Error code on failure.

  @dependencies
  None.
*/
#define CSD_DEBUG_VOICE_IOCTL_GET_AFE_PORT ( 0x00013051 )

typedef struct csd_debug_voice_ioctl_get_afe_port_t csd_debug_voice_ioctl_get_afe_port_t;
struct csd_debug_voice_ioctl_get_afe_port_t
{
  uint32_t  csd_vc_handle;
  uint16_t* ret_rx_afe_port;
  uint16_t* ret_tx_afe_port;
  uint16_t* ret_ec_afe_port;
};

/** @endcond */

#endif /* __CSD_VOICE_DEBUG_IOCTL_H__ */
