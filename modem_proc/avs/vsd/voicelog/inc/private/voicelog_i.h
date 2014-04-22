#ifndef __VOICELOG_I_H__
#define __VOICELOG_I_H__

/**
  @file  voicelog_i.h
  @brief This file contains private definitions of voice log utility.
*/

/*
  ============================================================================

   Copyright (C) 2015 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

  ============================================================================

                             Edit History

  $Header: //components/rel/avs.mpss/8.0.c12/vsd/voicelog/inc/private/voicelog_i.h#1 $
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

/* APR APIs. */
#include "apr_comdef.h"
#include "apr_lock.h"


/****************************************************************************
  VOICELOG DEFINES                                                          *
*****************************************************************************/

#define VOICELOG_MAX_NUM_OF_VOCODER_SESSIONS_V ( 2 )
#define VOICELOG_VSID_UNDEFINED_V ( 0xFFFFFFFF )
#define VOICELOG_CALL_NUMBER_DEFAULT_V ( 0 )
#define VOICELOG_VOCODER_SESSION_NUMBER_DEFAULT_V ( 0 )
#define VOICELOG_PKT_SEQ_NUMBER_DEFAULT_V ( 0 )
#define VOICELOG_LOG_DROP_COUNT_DEFAULT_V ( 0 )

/** Container ID and Minor Version together identifies the overall structure
 * of a voice container on modem processor.
 */
#define VOICELOG_VOICE_CONTAINER_ID ( 0x000131E0 )
#define VOICELOG_VOICE_CONTAINER_MINOR_VERSION_V ( 0x0001 )

/** Container ID and Minor Version together identifies the overall structure
 * of a control container for a voice log packet.
 */
#define VOICELOG_CONTROL_CONTAINER_ID ( 0x000131E1 )
#define VOICELOG_CONTROL_CONTAINER_MINOR_VERSION_V ( 0x0001 )

/** Identifies overall structure of a data container. */
#define VOICELOG_DATA_CONTAINER_MINOR_VERSION_V ( 0x0001 )

/** Error defines of voice log utility. */
#define VOICELOG_REPORT_FATAL_ON_ERROR( rc ) \
  { if ( rc ) { MSG_1( MSG_SSID_DFLT, MSG_LEGACY_FATAL, "Error[0x%08x]", rc ); } }

/****************************************************************************
  VOICELOG DEFINITION                                                       *
*****************************************************************************/

/**
 * VOICELOG VOCODER SESSION OBJECT DEFINITION 
 */
typedef struct voicelog_session_obj_t  voicelog_session_obj_t;

struct voicelog_session_obj_t {

  uint32_t vsid;
    /**< System level published/documented Voice System ID. */
  uint16_t call_number;
    /**< This is used to keep track of voice calls on a subscription. */
  uint16_t vocoder_session_number;
    /**< This is used to keep track of handovers during a call. */
  uint32_t packet_seq_number_uplink;
    /**< 
     * This keeps track of uplink vocoder packet count corresponding to a 
     * vocoder session. This shall be made zero for each vocoder session.
     */
  uint32_t packet_seq_number_downlink;
    /**< 
     * This keeps track of downlink vocoder packet count corresponding to a 
     * vocoder session. This shall be made zero for each vocoder session.
     */
  uint16_t log_drop_count;
    /**< 
     * This is used to keep track of log_alloc failures during voice call.
     * This shall be made zero for each vocoder session.
     */
  apr_lock_t session_lock;
    /**< This is to ensure synchronisation while accessing session object. */
};

/**
 * VOICELOG DATA CONTAINER DEFINITION.
 */
typedef struct voicelog_data_container_t  voicelog_data_container_t;

#include "apr_pack_begin.h"

struct voicelog_data_container_t {

  uint16_t container_size;
  uint32_t container_id;
  uint16_t minor_version;
  uint8_t frame_header;
#if __STDC_VERSION__ >= 199901L
  uint8_t frame[];
#endif /* __STDC_VERSION__ >= 199901L */ 
}
#include "apr_pack_end.h"
;


/**
 * VOICELOG CONTROL CONTAINER DEFINITION.
 */
typedef struct voicelog_control_container_t  voicelog_control_container_t;

#include "apr_pack_begin.h"

struct voicelog_control_container_t {

  uint16_t container_size;
  uint32_t container_id;
  uint16_t minor_version;
  uint32_t vsid;
  uint16_t call_number;
  uint16_t vocoder_session_number;
  uint32_t packet_sequence_number;
  uint32_t network_id;
  uint16_t log_drop_count;
  uint64_t timestamp;
  uint32_t tap_point_id;
}
#include "apr_pack_end.h"
;


/**
 * VOICELOG VOCIE CONTAINER DEFINITION.
 */
typedef struct voicelog_voice_container_t  voicelog_voice_container_t;

#include "apr_pack_begin.h"

struct voicelog_voice_container_t {

  uint16_t container_size;
  uint32_t container_id;
  uint16_t minor_version;
  voicelog_control_container_t control;
  voicelog_data_container_t data;
}
#include "apr_pack_end.h"
;

/**
 * VOICELOG VOICE PACKET DEFINITION.
 */
typedef struct voicelog_voice_packet_t  voicelog_voice_packet_t;

#include "apr_pack_begin.h"

struct voicelog_voice_packet_t {

  log_hdr_type hdr;
  voicelog_voice_container_t voice;
}
#include "apr_pack_end.h"
;

#endif /* __VOICELOG_I_H__ */

