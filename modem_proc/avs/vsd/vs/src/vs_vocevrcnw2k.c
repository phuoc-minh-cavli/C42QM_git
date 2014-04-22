/**
  @file vs_vocevrcnw2k.c
  @brief This file contains the definitions of the interfaces for EVRC-NW2K vocoder.

*/

/*
  ============================================================================
  Copyright (C) 2014-2016, 2019 QUALCOMM Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/vsd/vs/src/vs_vocevrcnw2k.c#1 $
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

VS_INTERNAL bool_t vs_vocevrcnw2k_is_frame_rate_valid (
  vs_vocevrcnw2k_frame_rate_t rate
)
{
  bool_t rc = FALSE;

  switch ( rate )
  {
    case VS_VOCEVRCNW2K_FRAME_BLANK_RATE:
    case VS_VOCEVRCNW2K_FRAME_EIGHTH_RATE:
    case VS_VOCEVRCNW2K_FRAME_QUARTER_RATE:
    case VS_VOCEVRCNW2K_FRAME_HALF_RATE:
    case VS_VOCEVRCNW2K_FRAME_FULL_RATE:
    case VS_VOCEVRCNW2K_FRAME_EIGHTH_RATE_NONCRITICAL:
      rc = TRUE;
      break;

    default:
      rc = FALSE;
      break;
  }

  return rc;
}


static uint32_t vs_vocevrcnw2k_action_set_enc_rate ( 
  vs_session_object_t* session_obj
)
{
  uint32_t rc = VS_EOK;
  vss_istream_cmd_voc_4gvnw2k_set_rate_t evrcnw2k_rate;

  if( VS_PARAM_UINT8_NOT_SET == session_obj->vocoder.encoder_rate ) return VS_EOK;

  evrcnw2k_rate.rate = session_obj->vocoder.encoder_rate;
  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_cvs_addr, session_obj->cvs_port,
         session_obj->header.handle, VSS_ISTREAM_CMD_VOC_4GVNW2K_SET_RATE,
         &evrcnw2k_rate, sizeof( evrcnw2k_rate ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "VS:VOCEVRCNW_2K: Failed to communicate with CVS with rc=0x%08x", rc );
  }

  return VS_EOK;
}


static uint32_t vs_vocevrcnw2k_publish_params (
  void* this
)
{
  //vs_session_object_t* session_obj = (vs_session_object_t*)this;

  //TBD
  return VS_EOK;
}


static uint32_t vs_vocevrcnw2k_cache_encoder_rate (
  vs_session_object_t* session_obj,
  vs_param_enc_rate_t* enc_rate
)
{
  uint32_t rc = VS_EOK;
  vs_vocevrcnw2k_avg_enc_rate_t rate = (vs_vocevrcnw2k_avg_enc_rate_t)enc_rate->rate;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
        "SET_PARAM: VS_PARAM_ENC_RATE(EVRCNW2k): rate=(%d) (0 - COP 0, ... 7 - COP 7),"
        "is_stream_ready=(%d)", rate, session_obj->is_stream_ready  );

  if ( rate > VS_VOCEVRCNW2K_AVG_RATE_COP_7 ) return VS_EBADPARAM;

  session_obj->vocoder.encoder_rate = enc_rate->rate;

  if( TRUE == session_obj->is_stream_ready )
  {
    ( void ) vs_vocevrcnw2k_action_set_enc_rate( session_obj );
    ( void ) vs_vocevrcnw2k_publish_params( session_obj );
  }

  return rc;
}


static uint32_t vs_vocevrcnw2k_cache_minmax_rate (
  vs_session_object_t* session_obj,
  vs_param_enc_minmax_rate_t* minmax
)
{
  uint32_t rc = VS_EOK;
  uint8_t min_rate = minmax->min_rate;
  uint8_t max_rate = minmax->max_rate;

  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
        "SET_PARAM: VS_PARAM_ENC_MINMAX_FRAME_RATE(EVRCNW2K): min_rate=(%d), max_rate=(%d)"
        " (0-blank, 1-eighth, 2-quarter, 3-half, 4-full, 14-erasure, 255 - eighth non critical),"
        "is_stream_ready=(%d)", min_rate, max_rate, session_obj->is_stream_ready  );

  if( ( FALSE == vs_vocevrcnw2k_is_frame_rate_valid( min_rate ) ) ||
      ( FALSE == vs_vocevrcnw2k_is_frame_rate_valid( max_rate ) ) )
  {
    return VS_EBADPARAM;
  }

  session_obj->vocoder.min_rate = min_rate;
  session_obj->vocoder.max_rate = max_rate;

  if( TRUE == session_obj->is_stream_ready )
  {
    ( void ) vs_voc_action_set_min_max_rate( session_obj );
    ( void ) vs_vocevrcnw2k_publish_params( session_obj );
  }

  return rc;
}

static uint32_t vs_vocevrcnw2k_set_cached_params_handler (
  void* this
)
{
  vs_session_object_t* session_obj = (vs_session_object_t*)this;

  ( void ) vs_voc_action_set_dtx_v2( session_obj );
  ( void ) vs_voc_action_set_min_max_rate( session_obj );
  ( void ) vs_vocevrcnw2k_action_set_enc_rate( session_obj );

  return VS_EOK;
}


static uint32_t vs_vocevrcnw2k_set_param_handler (
 void* this,
 uint32_t param_id,
 void* param_data
)
{
  uint32_t rc = VS_EOK;
  vs_session_object_t* session_obj = (vs_session_object_t*)this;

  switch( param_id )
  {
    case VS_PARAM_ENC_RATE:
     rc = vs_vocevrcnw2k_cache_encoder_rate( session_obj, (vs_param_enc_rate_t*) param_data ) ;
     break;

    case VS_PARAM_ENC_MINMAX_FRAME_RATE:
      rc = vs_vocevrcnw2k_cache_minmax_rate( session_obj, (vs_param_enc_minmax_rate_t*)param_data );
      break;

    default:
      rc = VS_EUNSUPPORTED;
      break;
  }

  return rc;
}


VS_INTERNAL uint32_t vs_vocevrcnw2k_init (
  vs_session_object_t* session_obj
)
{
  uint32_t rc = VS_EOK;

  session_obj->vocoder.set_param_fn = vs_vocevrcnw2k_set_param_handler;
  session_obj->vocoder.set_cached_param_fn = vs_vocevrcnw2k_set_cached_params_handler;
  session_obj->vocoder.publish_param_fn = vs_vocevrcnw2k_publish_params;

  return rc;
}

