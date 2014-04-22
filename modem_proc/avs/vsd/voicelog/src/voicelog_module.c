/*
   DESCRIPTION
   This file contains the defintions of voicelog utility.

   Copyright (C) 2015-2018 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   $Header: //components/rel/avs.mpss/8.0.c12/vsd/voicelog/src/voicelog_module.c#1 $
   $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

/* SYSTEM UTILITY APIs. */
#include "comdef.h"
#include "msg.h"
#include "err.h"
#include "mmstd.h"
#include "log.h"
#include "event.h"

/* VSD APIs. */
#include "vs.h"
#include "vss_common_public_if.h"
#include "voicecfg_api.h"
#include "voicecfg_items.h"

/* VOICELOG APIs. */
#include "voicelog_if.h"
#include "voicelog_utils.h"
#include "voicelog_i.h"

#ifdef AVS_MPSS_TEST
#include "wcdma_ext_api.h"
#include "wcdmamvsif.h"
#include "gsm_voice_server_if.h"
#include "tds_ext_api.h"
#include "onex_voice_if.h"
#endif

/****************************************************************************
  DEFINITIONS
****************************************************************************/
/* Enable this Flag to write all UL/DL vocoder packets to a binary file while running test cases
 * in DSP Simulator */
// #define LOG_MODEM_VOICE_SERVICES_VOCODER_PACKET

/****************************************************************************
  VOICELOG GLOBAL VARIABLES
****************************************************************************/

static voicelog_session_obj_t voicelog_sessions[VOICELOG_MAX_NUM_OF_VOCODER_SESSIONS_V] =
                                 {
                                   { VOICELOG_VSID_UNDEFINED_V,
                                     VOICELOG_CALL_NUMBER_DEFAULT_V,
                                     VOICELOG_VOCODER_SESSION_NUMBER_DEFAULT_V,
                                     VOICELOG_PKT_SEQ_NUMBER_DEFAULT_V,
                                     VOICELOG_PKT_SEQ_NUMBER_DEFAULT_V,
                                     VOICELOG_LOG_DROP_COUNT_DEFAULT_V,
                                     APR_NULL_V
                                   },
                                   { VOICELOG_VSID_UNDEFINED_V,
                                     VOICELOG_CALL_NUMBER_DEFAULT_V,
                                     VOICELOG_VOCODER_SESSION_NUMBER_DEFAULT_V,
                                     VOICELOG_PKT_SEQ_NUMBER_DEFAULT_V,
                                     VOICELOG_PKT_SEQ_NUMBER_DEFAULT_V,
                                     VOICELOG_LOG_DROP_COUNT_DEFAULT_V,
                                     APR_NULL_V
                                   }
                                 };

/****************************************************************************
 * VOICELOG UTILITY FUNCTIONS                                                   *
 ****************************************************************************/

static voicelog_session_obj_t* voicelog_get_session (
  uint32_t vsid
)
{
 uint32_t index = 0;
 voicelog_session_obj_t *session_obj = NULL;

 for( index = 0; index < VOICELOG_MAX_NUM_OF_VOCODER_SESSIONS_V; ++index )
 {
   if( vsid == voicelog_sessions[index].vsid )
   {
     session_obj = &voicelog_sessions[index];
     break;
   }
 }

 return session_obj;
}


/**
 * Initializes the voicelog session.
 */
uint32_t voicelog_session_init ( 
  uint32_t vsid
)
{
  uint32_t rc = APR_EFAILED;
  uint32_t index = 0;
  voicelog_session_obj_t *session_obj = NULL;

  for(index = 0; index < VOICELOG_MAX_NUM_OF_VOCODER_SESSIONS_V; ++index)
  {
    if( VOICELOG_VSID_UNDEFINED_V == voicelog_sessions[index].vsid )
    {
      session_obj = &voicelog_sessions[index];
      session_obj->vsid = vsid;
      session_obj->vocoder_session_number = VOICELOG_VOCODER_SESSION_NUMBER_DEFAULT_V;
      session_obj->packet_seq_number_uplink = VOICELOG_PKT_SEQ_NUMBER_DEFAULT_V;
      session_obj->packet_seq_number_downlink = VOICELOG_PKT_SEQ_NUMBER_DEFAULT_V;
      session_obj->call_number = VOICELOG_CALL_NUMBER_DEFAULT_V;

      rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &session_obj->session_lock);
      VOICELOG_REPORT_FATAL_ON_ERROR( rc );

      break;
    }
  }

  if ( NULL == session_obj )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "voicelog_voc_session_init() - FAILED for vsid=(0X%08X)", vsid );
  }

  return rc;
}


uint32_t voicelog_session_update ( 
 uint32_t vsid,
 voicelog_info_enum_t info_id
)
{
  uint32_t rc = APR_EOK;
  voicelog_session_obj_t *session_obj = voicelog_get_session( vsid );

  for ( ;; )
  {
    if( NULL == session_obj )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "voicelog_session_update() - FAILED for vsid=(0X%08X)", vsid );
      rc = APR_EBADPARAM;
      break;
    }

    ( void ) apr_lock_enter( session_obj->session_lock );

    switch ( info_id )
    {
     case VOICELOG_INFO_ENUM_VOCODER_SESSION_NUMBER:
       {
         /* Reset the packet sequence number for the new vocoder session. */
         ++session_obj->vocoder_session_number;
         session_obj->packet_seq_number_uplink = 0;
         session_obj->packet_seq_number_downlink = 0;
       }
       break;

     case VOICELOG_INFO_ENUM_CALL_NUMBER:
       {
         ++session_obj->call_number;
         session_obj->vocoder_session_number = 0;
         session_obj->packet_seq_number_uplink = 0;
         session_obj->packet_seq_number_downlink = 0;
         session_obj->log_drop_count = 0;
       }
       break;

     default:
       {
         MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                "voicelog_session_update() - FAILED for invalid "
                "info_id=(0X%08X)", info_id );
         rc = APR_EBADPARAM;
       }
       break;
    }

    break;
  }

  if( NULL != session_obj )
  {
    ( void ) apr_lock_leave( session_obj->session_lock );
  }

  return rc;
}


uint32_t voicelog_event_report ( 
  sys_modem_as_id_e_type asid,
  uint32_t vsid,
  uint8_t  vocoder_state,
  uint32_t vs_network_id,
  uint32_t vs_media_id
)
{
  uint32_t rc = APR_EOK;
  voicelog_ievent_vocoder_state_t event_data;

  event_data.asid = asid;
  event_data.vsid = vsid;
  event_data.vocoder_state = vocoder_state;
  event_data.network_id = vs_network_id;
  event_data.media_id = vs_media_id;

#ifdef AVS_MPSS_TEST

  switch ( vs_network_id ) 
  {
   case VOICELOG_ICOMMON_NETWORK_ID_GSM:
     gsm_call ( ( event_id_enum_type ) VOICELOG_IEVENT_ID_VOCODER_STATE,
                                   &event_data, sizeof( event_data ) );
     break;

   case VOICELOG_ICOMMON_NETWORK_ID_WCDMA:
     wcdma_ext_audio_api ( ( event_id_enum_type ) VOICELOG_IEVENT_ID_VOCODER_STATE,
                                   &event_data, sizeof( event_data ) );
     break;

   case VOICELOG_ICOMMON_NETWORK_ID_TDSCDMA:
     tds_ext_audio_api ( ( event_id_enum_type ) VOICELOG_IEVENT_ID_VOCODER_STATE,
                                   &event_data, sizeof( event_data ) );
     break;

   case VOICELOG_ICOMMON_NETWORK_ID_CDMA:
     onex_voice_call ( ( event_id_enum_type ) VOICELOG_IEVENT_ID_VOCODER_STATE,
                                   &event_data, sizeof( event_data ) );
     break;

   default:
     break;
  }
#else
  event_report_payload ( ( event_id_enum_type ) VOICELOG_IEVENT_ID_VOCODER_STATE,
                                  sizeof( event_data ), &event_data );
#endif

  return rc;
}


uint32_t voicelog_packet_commit_data ( 
 void* params,
 uint32_t size
)
{
  uint32_t rc = APR_EOK;
  voicelog_ipacket_cmd_commit_data_t* cmd_params = NULL;
  voicelog_session_obj_t* session_obj = NULL;
  
  cmd_params = ( voicelog_ipacket_cmd_commit_data_t* ) params;
  if ( NULL == cmd_params )
  {      
    VOICELOG_REPORT_FATAL_ON_ERROR( APR_EBADPARAM );
    return APR_EBADPARAM;
  }

  session_obj = voicelog_get_session( cmd_params->vsid );
  if ( NULL == session_obj )
  {
    VOICELOG_REPORT_FATAL_ON_ERROR( APR_EBADPARAM );
    return APR_EBADPARAM;
  }

#ifndef SIM_DEFINED
  voicelog_voice_packet_t* log_ptr = NULL;

  log_ptr = ( voicelog_voice_packet_t* )log_alloc( cmd_params->log_code, 
                                          cmd_params->frame_size + 
                                          sizeof( voicelog_voice_packet_t ) );

  if ( NULL != log_ptr )
  {
    log_ptr->voice.container_size = 
      sizeof( voicelog_voice_container_t ) + cmd_params->frame_size;
    log_ptr->voice.container_id = VOICELOG_VOICE_CONTAINER_ID;
    log_ptr->voice.minor_version = VOICELOG_VOICE_CONTAINER_MINOR_VERSION_V;

    /* voice control starts here. */
    log_ptr->voice.control.container_size = sizeof( voicelog_control_container_t );
    log_ptr->voice.control.container_id = VOICELOG_CONTROL_CONTAINER_ID;
    log_ptr->voice.control.minor_version = VOICELOG_CONTROL_CONTAINER_MINOR_VERSION_V;
    log_ptr->voice.control.call_number = session_obj->call_number;
    log_ptr->voice.control.vsid = cmd_params->vsid;
    log_ptr->voice.control.network_id = cmd_params->network_id;
    log_ptr->voice.control.timestamp = cmd_params->timestamp;

    ( void ) apr_lock_enter( session_obj->session_lock );

    /* Update packet sequence number */
    if ( VOICELOG_IPACKET_CODE_DL_VOCODER_PACKET == cmd_params->log_code )
    {
      session_obj->packet_seq_number_downlink++;
      log_ptr->voice.control.packet_sequence_number = session_obj->packet_seq_number_downlink;
    }
    else
    {
      session_obj->packet_seq_number_uplink++;
      log_ptr->voice.control.packet_sequence_number = session_obj->packet_seq_number_uplink;
    }
    log_ptr->voice.control.vocoder_session_number = session_obj->vocoder_session_number;

    ( void ) apr_lock_leave( session_obj->session_lock );

    log_ptr->voice.control.log_drop_count = session_obj->log_drop_count;
    log_ptr->voice.control.tap_point_id = cmd_params->tap_point_id;

    /* voice data starts here. */
    log_ptr->voice.data.container_size =
      sizeof( voicelog_data_container_t ) + cmd_params->frame_size;

    log_ptr->voice.data.container_id = cmd_params->media_id;

    log_ptr->voice.data.minor_version = VOICELOG_DATA_CONTAINER_MINOR_VERSION_V;
    log_ptr->voice.data.frame_header = cmd_params->frame_header;
    ( void ) mmstd_memcpy( ( uint8_t* ) log_ptr + sizeof( voicelog_voice_packet_t ),
                           cmd_params->frame_size, cmd_params->frame, 
                           cmd_params->frame_size );

    log_commit( log_ptr );
  }
  else
  {
    rc = APR_ENORESOURCE;
    /* Update log alloc failure count */
    ++session_obj->log_drop_count;
  }

  if( ( APR_ENORESOURCE != rc ) && ( APR_EOK != rc ) && ( NULL != session_obj ) )
  {

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vlog_voc_data_commit() - Failed with rc=(0x%08x)", rc );

    /* Update log drop count in case of cmd param failure. */
    ++session_obj->log_drop_count;
  }

#elif defined( LOG_MODEM_VOICE_SERVICES_VOCODER_PACKET )
  
  FILE *fp = NULL;
  char filename[100];
  uint32_t media_id;
  
  media_id = cmd_params->media_id;

  snprintf( filename, 100, (const char*)"aDSPTest/TestOutput/LOG(%04X)_VSID(%08X)_NW(%08X)_CN(%d)_VSN(%d)_MEDIA(%08X).bin",
            cmd_params->log_code,
            cmd_params->vsid,
            cmd_params->network_id,
            session_obj->call_number,
            session_obj->vocoder_session_number,
            media_id
            );

  fp = fopen( filename,"ab" );
  if ( NULL == fp )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "cvd_cal_data_writetofile():"
           " Failed to open file to write for usecase_id" );
    return APR_EFAILED;
  }
  
  fwrite(( void* )(&cmd_params->frame_header ), sizeof( uint8_t ),
                    sizeof( uint8_t ), fp );
  fwrite(( void* )( cmd_params->frame ), sizeof( uint8_t ),
                    cmd_params->frame_size, fp );

  fclose( fp );
  
#endif /* SIM_DEFINED */

  return rc;
}

/**
 *
 */
uint32_t voicelog_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
)
{
  uint32_t rc;

  switch ( cmd_id )
  {
    case VOICELOG_IPACKET_CMD_COMMIT_DATA:
      rc = voicelog_packet_commit_data( params, size );
      break;

    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "voicelog_call(): Unsupported cmd ID (0x%08x)", cmd_id );
      rc = APR_EUNSUPPORTED;
      break;
  }

  return rc;
}



