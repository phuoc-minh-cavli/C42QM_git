#ifndef __VOICELOG_IEVENT_IF_H__
#define __VOICELOG_IEVENT_IF_H__

/**
  @file  voicelog_ievent_if.h
  @brief This file contains interface definitions for logging voice subsystem
         events. 
*/

/*
  ============================================================================

   Copyright (C) 2015 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================

                             Edit History

  $Header: //components/rel/avs.mpss/8.0.c12/api/voicelog/voicelog_ievent_if.h#1 $
  $Author: pwbldsvc $

  when      who   what, where, why
  --------  ---   ------------------------------------------------------------


  ============================================================================
*/

/***************************************************************************
  Include files for Module                                                 *
****************************************************************************/

/* SYSTEM UTILITY APIs. */
#include "mmdefs.h"
#include "sys.h"

/****************************************************************************
  VOICE LOG EVENT INTERFACE DEFINES                                         *
*****************************************************************************/


/** Vocoder State: This identifies the state of vocoder during a voice call
 */
#define VOICELOG_IEVENT_VOCODER_STATE_REQUESTED ( 0x00)
#define VOICELOG_IEVENT_VOCODER_STATE_GRANTED ( 0x01 )
#define VOICELOG_IEVENT_VOCODER_STATE_RELEASED ( 0x02 )
#define VOICELOG_IEVENT_VOCODER_STATE_READY ( 0x03 )
#define VOICELOG_IEVENT_VOCODER_STATE_RUNNING ( 0x04 )
#define VOICELOG_IEVENT_VOCODER_STATE_NOT_READY ( 0x05 )


/** Event ID definitions supported by voice.
 *
 *  Event IDs are allocated and maintained by DIAG team. Please refer to 
 *  <root>/modem_proc/core/api/services/event_defs.h for more details.
 *
 *  The client shall call the diag api directly for logging the event. * 
 *  Diag API:
 *  ( void ) event_report_payload ( event_id_enum_type event_id, 
 *                                 uint8 length, 
 *                                 void* data );
 */
#define VOICELOG_IEVENT_ID_VOCODER_STATE ( 0x0AB4 )

/** @brief Type definition for #voicelog_ievent_cmd_commit_event_t.
*/
typedef struct voicelog_ievent_vocoder_state_t voicelog_ievent_vocoder_state_t;

#include "apr_pack_begin.h"
/** @brief Payload structure for the #VOICELOG_IEVENT_VOCODER_STATE .
*/
struct voicelog_ievent_vocoder_state_t {
 
   sys_modem_as_id_e_type asid;
     /**< Identifies the subscription on which voice call is active. */
   uint32_t vsid;
     /**< Identifies the voice-subsystem involved. */
   uint8_t vocoder_state;
     /**< 
      * Identifies the vocoder state during a voice call.
      * See VOICELOG_IEVENT_VOCODER_STATE_XXX. 
      */
   uint32_t network_id;
     /**< Identifies the network involved. See VOICELOG_ICOMMON_NETWORK_ID_XXX. */
   uint32_t media_id;
     /**<
      * The media ID determines the active vocoder.
      * See VOICELOG_ICOMMON_MEDIA_ID_XXX.
      */
 }

#include "apr_pack_end.h"
;

#endif /* __VOICELOG_IEVENT_IF_H__ */