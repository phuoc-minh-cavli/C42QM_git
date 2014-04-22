/**
  @file vs_vocg711mulaw_v2.c
  @brief This file contains the definitions of the interfaces for G.711 MU-LAW V2 vocoder.
*/

/*
  ============================================================================
  Copyright (C) 2016 , 2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/vsd/vs/src/vs_vocg711mulaw_v2.c#1 $
  $Author: pwbldsvc $
  ============================================================================
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/
#include "vs_i.h"


/****************************************************************************
 * FUNCTIONS                                                                *
 ****************************************************************************/

bool_t vs_vocg711mulaw_v2_is_frame_header_valid (
  vs_vocg711mulaw_v2_frame_type_t frame_type,
  uint8_t companding_type
)
{
  if( frame_type >= VS_VOCG711MULAW_V2_FRAME_SPEECH_TYPE ||
      frame_type <= VS_VOCG711MULAW_V2_FRAME_ERASURE_TYPE )
  {
    if ( ( frame_type == VS_VOCG711MULAW_V2_FRAME_SPEECH_TYPE ) && 
         ( companding_type != 0 ) )
    {
      return FALSE;
    }
  }    
  else
  {
    return FALSE;
  }
  
  return TRUE;
}

static uint32_t vs_vocg711mulaw_v2_publish_params (
  void* this
)
{
  //vs_session_object_t* session_obj = (vs_session_object_t*)this;

  //TBD
  return VS_EOK;
}

static uint32_t vs_vocg711mulaw_v2_cache_plc_mode (
  vs_session_object_t* session_obj,
  vs_param_voc_plc_t* param
)
{
  uint32_t rc = VS_EOK;

  for( ;; )
  {
    if ( (param->plc != FALSE) && (param->plc != TRUE) )
    {
      rc = VS_EBADPARAM;
      break;
    }

    session_obj->vocoder.plc = param->plc;

    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
          "SET_PARAM: VS_PARAM_VOC_PLC(G711_MULAW): plc=(%d) (0 - Disable, 1- Enable),"
          "is_stream_ready=(%d)", param->plc, session_obj->is_stream_ready  );


    if( TRUE == session_obj->is_stream_ready )
    {
      ( void ) vs_voc_action_set_plc( session_obj );
      ( void ) vs_vocg711mulaw_v2_publish_params( session_obj );
    }

    break;
  }

  return rc;
}


static uint32_t vs_vocg711mulaw_v2_set_cached_params_handler (
  void* this
)
{
  vs_session_object_t* session_obj = (vs_session_object_t*)this;

  ( void ) vs_voc_action_set_dtx_v2( session_obj );
  ( void ) vs_voc_action_set_plc( session_obj );

  return VS_EOK;
}


static uint32_t vs_vocg711mulaw_v2_set_param_handler (
 void* this,
 uint32_t param_id,
 void* param_data
)
{
  uint32_t rc = VS_EOK;
  vs_session_object_t* session_obj = (vs_session_object_t*)this;

  switch( param_id )
  {
    case VS_PARAM_VOC_PLC:
     rc = vs_vocg711mulaw_v2_cache_plc_mode( session_obj, (vs_param_voc_plc_t*) param_data ) ;
     break;

    default:
      rc = VS_EUNSUPPORTED;
      break;
  }

  return rc;
}

VS_INTERNAL uint32_t vs_vocg711mulaw_v2_init(
  vs_session_object_t* session_obj
)
{
  uint32_t rc = VS_EOK;

  session_obj->vocoder.set_param_fn = vs_vocg711mulaw_v2_set_param_handler;
  session_obj->vocoder.set_cached_param_fn = vs_vocg711mulaw_v2_set_cached_params_handler;
  session_obj->vocoder.publish_param_fn = vs_vocg711mulaw_v2_publish_params;

  return rc;

}

