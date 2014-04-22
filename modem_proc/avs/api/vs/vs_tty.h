#ifndef __VS_OOBTTY_H__
#define __VS_OOBTTY_H__

/**
  @file  vs_tty.h
  @brief This file contains tty interface definitions of the Vocoder 
         Services (VS).
*/

/*
  ============================================================================
  Copyright (C) 2018 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/api/vs/vs_tty.h#1 $
  $Author: pwbldsvc $
  ============================================================================
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include "mmdefs.h"

/****************************************************************************
 * VS OOBTTY CMD INTERFACE DEFINITIONS                          
 ****************************************************************************/

/** 
 * OOBTTY char push command (asynchronous API).
 *
 * The command is used by client during OOB-TTY scenrios to play out
 * the TTY character recieved from far-end.
 *
 * The Client shall issue this command only after recieving 
 * VS_VOC_EVENT_READY. Vocoder services notifies client with 
 * VS_COMMON_EVENT_CMD_RESPONSE event indicating a success or a failure. 
 * 
 * The client shall push new OOBTTY character only after recieving response
*  for previous OOBTTY char.
 */
#define VS_OOBTTY_CMD_PUSH_CHAR ( 0x000132D8 )

typedef struct vs_oobtty_cmd_push_char_t {

  uint32_t handle;
    /**< Open VS session handle. */
  uint16_t tty_char;
    /**< - LSB 8 bits -- Character in UTF-8 format. 
         - MSB 8 bits -- Not used. Set to 0. */

} vs_oobtty_cmd_push_char_t;
  

/****************************************************************************
 * VS OOBTTY EVENT INTERFACE DEFINITIONS                          
 ****************************************************************************/

/** 
 * OOBTTY Char notification event (asynchronous API).
 *
 * Vocoder service generates this event to the client when a
 * valid TTY character is detected on uplink(TX).
 *
 * The Client shall receive this notification only after vocoder is 
 * enabled and VS_VOC_EVENT_READY is notified by vocoder services.
 */
#define VS_OOBTTY_EVENT_NOTIFY_CHAR ( 0x000132D9 )

typedef struct vs_oobtty_event_notify_char_t {

  uint16_t tty_char;
    /**< - LSB 8 bits -- Character in UTF-8 format. 
         - MSB 8 bits -- Not used. Set to 0. */

} vs_oobtty_event_notify_char_t;

#endif /* __VS_OOBTTY_H__ */