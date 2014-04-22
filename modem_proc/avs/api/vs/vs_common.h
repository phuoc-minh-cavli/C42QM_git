#ifndef __VS_COMMON_H__
#define __VS_COMMON_H__

/**
  @file  vs_voc.h
  @brief This file contains common interface definitions of the Vocoder
         Services (VS).
*/

/*
  ============================================================================
  Copyright (C) 2017 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/api/vs/vs_common.h#1 $
  $Author: pwbldsvc $
  ============================================================================
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "mmdefs.h"

/****************************************************************************
 * VS TYPE DEFINITIONS
 ****************************************************************************/

/** 
 * Network-IDs definition which are currently supported by VS.
 */
#define VS_COMMON_NETWORK_ID_CDMA     ( 0x000130E4 )

#define VS_COMMON_NETWORK_ID_GSM      ( 0x00012E26 )

#define VS_COMMON_NETWORK_ID_WCDMA    ( 0x000130E2 )

#define VS_COMMON_NETWORK_ID_TDSCDMA  ( 0x000130E3 )

#define VS_COMMON_NETWORK_ID_LTE      ( 0x000131E4 )

#define VS_COMMON_NETWORK_ID_WLAN     ( 0x000131E5 )

#define VS_COMMON_NETWORK_ID_NR       ( 0x0001334B )


/**
 * Event callback provided by a client of the voice services.
 *
 * The client returns VS_EOK from the callback when the client was succesful
 * in queuing the event to its own thread context for processing.
 *
 * All other error codes indicates a problem.
 *
 * The event parameter pointer is valid only within the callback context.
 */
typedef uint32_t ( *vs_common_event_callback_fn_t ) (

  uint32_t event_id,
    /**< Event ID. See VS_XXX_EVENT_YYY. */
  void* payload,
    /**< Reference to event-ID specific payload structure. */
  uint32_t size,
    /**< Size of the payload. */
  void* session_context
    /**< Client provided session context pointer from the OPEN command. */
);


/****************************************************************************
 * VS COMMAND INTERFACE DEFINITIONS
 ****************************************************************************/

/** 
 * Set param command (synchronous blocking API).
 *
 * The client uses this command to set the required configuration parameters 
 * before enabling services.
 *
 * - Returns VS_EOK, If set param is successful.
 * - Returns VS_EUNSUPPORTED, If param ID is not supported by vocoder services.
 * - Returns VS_EFAILED, If dependecies for a particular param are not met,
 *                       please refer to VS_XXXX_PARAM_YYYY description.
 */
#define VS_COMMON_CMD_SET_PARAM ( 0x000132BB )

typedef struct vs_common_cmd_set_param_t {

  uint32_t handle;
    /**< Open VS session handle. */
  uint32_t param_id;
    /**< Supproted Parameter-ID. */
  void* param_data;
    /**< Reference to param-ID specific param_data structure.
      *  Client is responsible allocating memory required for 
      *  param_data. */
  uint32_t size;
    /**< Size of the param_data. */
} vs_common_cmd_set_param_t;


/****************************************************************************
 * VS EVENT INTERFACE DEFINITIONS
 ****************************************************************************/
 
/**
 * Generic command response event (asynchronous API).
 */
#define VS_COMMON_EVENT_CMD_RESPONSE ( 0x000132BC )

typedef struct vs_common_event_cmd_response_t {

  uint32_t cmd_id;
    /**< ID of the command that completed. */
  uint32_t status;
    /**< Error code returned.
      *
      *  @values, refer to VS_EXXXX definitions.
      */
  void* token;
    /**< Client provided command token for asynchronous command call. */
} vs_common_event_cmd_response_t;

#endif /* __VS_COMMON_H__ */

