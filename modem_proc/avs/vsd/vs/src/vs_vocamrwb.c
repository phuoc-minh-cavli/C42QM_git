/**
  @file vs_vocamrwb.c
  @brief This file contains the definitions of the interfaces for AMR-WB
         vocoder.

*/

/*
  ============================================================================
  Copyright (C) 2013-2015, 2019 QUALCOMM Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/vsd/vs/src/vs_vocamrwb.c#1 $
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
static uint32_t vs_vocamrwb_action_set_enc_rate ( 
  vs_session_object_t* session_obj
)
{
  uint32_t rc = VS_EOK;
  vss_istream_cmd_voc_amrwb_set_enc_rate_t amr_mode;

  if( VS_PARAM_UINT8_NOT_SET == session_obj->vocoder.encoder_rate ) return VS_EOK;

  /* apply the codec_mode */
  amr_mode.mode = ( uint32_t ) session_obj->vocoder.encoder_rate;
  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_cvs_addr, session_obj->cvs_port,
         session_obj->header.handle, VSS_ISTREAM_CMD_VOC_AMRWB_SET_ENC_RATE,
         &amr_mode, sizeof( amr_mode ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "VS:VOCAMR_WB: Failed to communicate with CVS with rc=0x%08x", rc );
  }

  return VS_EOK;
}


static uint32_t vs_vocamrwb_publish_params (
  void* this
)
{
  //vs_session_object_t* session_obj = (vs_session_object_t*)this;

  //TBD
  return VS_EOK;
}


static uint32_t vs_vocamrwb_cache_encoder_rate (
  vs_session_object_t* session_obj,
  vs_param_enc_rate_t* enc_rate
)
{
  uint32_t rc = VS_EOK;
  vs_vocamrwb_codec_mode_t codec_mode = (vs_vocamrwb_codec_mode_t) enc_rate->rate;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
        "SET_PARAM: VS_PARAM_ENC_RATE(AMR-WB): codec_mode=(%d) "
        "(0-6.60, 1-8.85, 2-12.65, 3-14.25, 4-15.85, 5-18.25, 6-19.85, 7-23.05, 8-23.85),"
        "is_stream_ready=(%d)", codec_mode, session_obj->is_stream_ready  );

  for( ;; )
  {
    if ( ( codec_mode < VS_VOCAMRWB_CODEC_MODE_0660 ) ||
         ( codec_mode > VS_VOCAMRWB_CODEC_MODE_2385 ) )
    {
      rc = VS_EFAILED;
      break;
    }

    session_obj->vocoder.encoder_rate = enc_rate->rate;

    if( TRUE == session_obj->is_stream_ready )
    {
      ( void ) vs_vocamrwb_action_set_enc_rate( session_obj );
      ( void ) vs_vocamrwb_publish_params( session_obj );
    }

    break;
  }

  return rc;
}

static uint32_t vs_vocamrwb_set_cached_params_handler (
  void* this
)
{
  vs_session_object_t* session_obj = (vs_session_object_t*)this;

  ( void ) vs_voc_action_set_dtx_v2( session_obj );
  ( void ) vs_vocamrwb_action_set_enc_rate( session_obj );

  return VS_EOK;
}


static uint32_t vs_vocamrwb_set_param_handler (
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
     rc = vs_vocamrwb_cache_encoder_rate( session_obj, (vs_param_enc_rate_t*) param_data ) ;
     break;

    default:
      rc = VS_EUNSUPPORTED;
      break;
  }

  return rc;
}


VS_INTERNAL uint32_t vs_vocamrwb_init (
  vs_session_object_t* session_obj
)
{
  uint32_t rc = VS_EOK;

  session_obj->vocoder.set_param_fn = vs_vocamrwb_set_param_handler;
  session_obj->vocoder.set_cached_param_fn = vs_vocamrwb_set_cached_params_handler;
  session_obj->vocoder.publish_param_fn = vs_vocamrwb_publish_params;

  return rc;
}



