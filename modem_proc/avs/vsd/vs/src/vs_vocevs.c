/**
  @file vs_vocevs.c
  @brief This file contains the definitions of the interfaces for EVS vocoder.
*/

/*
  ============================================================================
  Copyright (C) 2016-2019 Qualcomm Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.
  ============================================================================
  $Header: //components/rel/avs.mpss/8.0.c12/vsd/vs/src/vs_vocevs.c#1 $
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

static uint32_t vs_vocevs_action_set_channel_awareness_mode ( 
   vs_session_object_t* session_obj
)
{
  uint32_t rc = VS_EOK;
  vss_istream_cmd_set_evs_enc_channel_aware_mode_enable_t chAwarenessMode;

  if( session_obj->vocoder.channel_awareness_mode >= VS_PARAM_UINT8_NOT_SET ) return VS_EOK;

  if( TRUE == session_obj->vocoder.channel_awareness_mode )
  {
    chAwarenessMode.fec_offset = session_obj->vocoder.fec_offset;
    chAwarenessMode.fer_rate = session_obj->vocoder.fer_rate;
    rc = __aprv2_cmd_alloc_send(
           vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
           vs_my_addr, ( (uint16_t) session_obj->header.handle ),
           vs_cvs_addr, session_obj->cvs_port,
           session_obj->header.handle,
           VSS_ISTREAM_CMD_SET_EVS_ENC_CHANNEL_AWARE_MODE_ENABLE,
           &chAwarenessMode, sizeof( chAwarenessMode ) );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VS:VOCEVS: Failed to communicate with CVS with rc=0x%08x", rc );
    }

  }
  else
  {
    rc = __aprv2_cmd_alloc_send(
           vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
           vs_my_addr, ( (uint16_t) session_obj->header.handle ),
           vs_cvs_addr, session_obj->cvs_port,
           session_obj->header.handle,
           VSS_ISTREAM_CMD_SET_EVS_ENC_CHANNEL_AWARE_MODE_DISABLE,
           NULL, 0 );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VS:VOCEVS: Failed to communicate with CVS with rc=0x%08x", rc );
    }
  }


  return rc;
}

static uint32_t vs_vocevs_action_set_operating_mode ( 
   vs_session_object_t* session_obj
)
{
  uint32_t rc = VS_EOK;
  vss_istream_cmd_set_evs_voc_enc_operating_mode_t opMode;

  if( session_obj->vocoder.encoder_rate >= VS_PARAM_UINT8_NOT_SET ) return VS_EOK;

  opMode.mode = session_obj->vocoder.encoder_rate;
  opMode.bandwidth = session_obj->vocoder.encoder_bw;
  
  /* apply the operating mode */
  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_cvs_addr, session_obj->cvs_port,
         session_obj->header.handle, VSS_ISTREAM_CMD_SET_EVS_VOC_ENC_OPERATING_MODE,
         &opMode, sizeof( opMode ) );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VS:VOCEVS: Failed to communicate with CVS with rc=0x%08x", rc );
    }

  return rc;
}


static uint32_t vs_vocevs_publish_params (
  void* this
)
{
  //vs_session_object_t* session_obj = (vs_session_object_t*)this;
  //TBD
  return VS_EOK;
}


static bool_t vs_vocevs_is_codec_mode_and_bandwidth_valid (
  vs_vocevs_codec_mode_t codec_mode,
  vs_voc_bandwidth_t bandwidth
)
{
  switch ( bandwidth )
  {
    case VS_VOC_BANDWIDTH_NB:
    {
      if ( codec_mode < VS_VOCEVS_CODEC_MODE_0590 || codec_mode > VS_VOCEVS_CODEC_MODE_2440 )
      {
        return FALSE;
      }
    }
    break;

   case VS_VOC_BANDWIDTH_WB:
   {
     if ( codec_mode < VS_VOCEVS_CODEC_MODE_0660 || codec_mode > VS_VOCEVS_CODEC_MODE_12800 )
     {
       return FALSE;
     }
   }
   break;

   case VS_VOC_BANDWIDTH_SWB:
   {
     if ( codec_mode < VS_VOCEVS_CODEC_MODE_0960 || codec_mode > VS_VOCEVS_CODEC_MODE_12800 )
     {
       return FALSE;
     }
   }
   break;

   case VS_VOC_BANDWIDTH_FB:
   {
     if ( codec_mode < VS_VOCEVS_CODEC_MODE_1640 || codec_mode > VS_VOCEVS_CODEC_MODE_12800 )
     {
       return FALSE;
     }
   }
   break;

   default:
     return FALSE;
     break;
  }

  return TRUE;
} 


static uint32_t vs_vocevs_cache_max_pp_sample_rate (
  vs_session_object_t* session_obj,
  vs_param_max_pp_sample_rate_t* sampleRate
)
{
  uint32_t rc = VS_EOK;
  uint32_t tx_sr = sampleRate->tx_pp_sample_rate;
  uint32_t rx_sr = sampleRate->rx_pp_sample_rate;

  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
        "SET_PARAM: VS_PARAM_MAX_PP_SAMPLE_RATE(EVS): tx_pp_sample_rate=(%d), rx_pp_sample_rate=(%d) "
        "(8k, 16k, 32k, 48K), is_stream_ready=(%d)", tx_sr, rx_sr, session_obj->is_stream_ready  );

  switch ( rx_sr)
  {
    case 8000:
    case 16000:
    case 32000:
    case 48000:
      break;

    default:
      return VS_EBADPARAM;
      break;
  }

  switch ( tx_sr)
  {
    case 8000:
    case 16000:
    case 32000:
    case 48000:
      break;

    default:
      return VS_EBADPARAM;
      break;
  }

  session_obj->vocoder.tx_sample_rate = tx_sr;
  session_obj->vocoder.rx_sample_rate = rx_sr;

  return rc;
}


static uint32_t vs_vocevs_cache_enc_operating_mode (
  vs_session_object_t* session_obj,
  vs_param_enc_operating_mode_t* encOperatingMode
)
{
  uint32_t rc = VS_EOK;
  uint8_t bandwidth = encOperatingMode->bandwidth;
  uint8_t codecMode = encOperatingMode->rate;

  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
        "SET_PARAM: VS_PARAM_ENC_OPERATING_MODE(EVS): bandwidth=(%d) (0-3), codec_mode=(%d) (0-20)"
        "is_stream_ready=(%d)", bandwidth, codecMode, session_obj->is_stream_ready  );

  if ( FALSE == vs_vocevs_is_codec_mode_and_bandwidth_valid ( codecMode, bandwidth  ) )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "VS: EVS: Invalid codecMode=(%d) for BandWidth=(%d)", codecMode, bandwidth );
    session_obj->vocoder.encoder_bw = VS_PARAM_UINT8_NOT_SET;
    session_obj->vocoder.encoder_rate = VS_PARAM_UINT8_NOT_SET;
    return VS_EBADPARAM;
  }

  /* Channel awareness is not applicable to non 13.2 kbps mode. */
  if( VS_VOCEVS_CODEC_MODE_1320 != session_obj->vocoder.encoder_rate )
  {
    session_obj->vocoder.channel_awareness_mode = VS_PARAM_UINT8_NOT_APPLICABLE;
    session_obj->vocoder.fec_offset = VS_PARAM_UINT8_NOT_APPLICABLE;
    session_obj->vocoder.fer_rate = VS_PARAM_UINT8_NOT_APPLICABLE;
  }

  session_obj->vocoder.encoder_rate = codecMode;
  session_obj->vocoder.encoder_bw = bandwidth;

  if( TRUE == session_obj->is_stream_ready )
  {
    ( void ) vs_vocevs_action_set_operating_mode( session_obj );
    ( void ) vs_vocevs_publish_params( session_obj );
  }

  return rc;
}

static uint32_t vs_vocevs_cache_channel_awareness_mode (
  vs_session_object_t* session_obj,
  vs_param_enc_channel_aware_mode_t* chAwarenessMode
)
{
  uint32_t rc = VS_EOK;
  uint8_t fer_rate = chAwarenessMode->fer_rate;
  uint8_t fec_offset = chAwarenessMode->fec_offset;
  uint8_t ch_enable = chAwarenessMode->channel_awareness_mode;

  MSG_4( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
        "SET_PARAM: VS_PARAM_ENC_CHANNEL_AWARE_MODE(EVS): ch_enable=(%d) (0,1), fer_rate=(%d) (0,1)"
        "fec_offset=(%d) (2,3,5,7), is_stream_ready=(%d)", ch_enable, fer_rate, fec_offset, session_obj->is_stream_ready  );

  switch ( ch_enable )
  {
    /* CHANNEL_AWARE_DISABLE */
    case 0:
    {
      /* Do nothing */
      break;
    }

    /* CHANNEL_AWARE_ENABLE */
    case 1:
    {
      /* Validate the channel aware parameters */

  switch ( fer_rate )
  {
    case 0:
    case 1:
      break;

    default:
      session_obj->vocoder.fer_rate = VS_PARAM_UINT8_NOT_SET;
          MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "VS: EVS: Invalid Channel Aware parameters: fer_rate=(%d), fec_offset=(%d)",
                 fer_rate, fec_offset );
      return VS_EBADPARAM;
      break;
  }

  switch ( fec_offset )
  {
    case 2:
    case 3:
    case 5:
    case 7:
      break;

    default:
      session_obj->vocoder.fec_offset = VS_PARAM_UINT8_NOT_SET;
          MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "VS: EVS: Invalid Channel Aware parameters: fer_rate=(%d), fec_offset=(%d)",
                 fer_rate, fec_offset );
          return VS_EBADPARAM;
          break;
      }

      break;
    }

    default:
      session_obj->vocoder.channel_awareness_mode = VS_PARAM_UINT8_NOT_SET;
      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VS: EVS: Invalid Channel Aware parameters: ch_enable=(%d), "
             "fer_rate=(%d), fec_offset=(%d)", ch_enable, fer_rate, fec_offset );
      return VS_EBADPARAM;
      break;
  }

  session_obj->vocoder.channel_awareness_mode = ch_enable;
  session_obj->vocoder.fec_offset = fec_offset;
  session_obj->vocoder.fer_rate = fer_rate;

  if( FALSE == session_obj->vocoder.channel_awareness_mode )
  { //reset the vocoder properties to default.
    session_obj->vocoder.fec_offset = VS_PARAM_UINT8_NOT_SET;
    session_obj->vocoder.fer_rate = VS_PARAM_UINT8_NOT_SET;
  }

  if( TRUE == session_obj->is_stream_ready )
  {
    ( void ) vs_vocevs_action_set_channel_awareness_mode( session_obj );
    ( void ) vs_vocevs_publish_params( session_obj );
  }

  return rc;
}


static uint32_t vs_vocevs_set_cached_params_handler (
   void* this
)
{
  vs_session_object_t* session_obj = (vs_session_object_t*)this;

  ( void ) vs_voc_action_set_dtx_v2( session_obj );
  ( void ) vs_vocevs_action_set_operating_mode( session_obj );
  ( void ) vs_vocevs_action_set_channel_awareness_mode( session_obj );

  return VS_EOK;
}
 
 
static uint32_t vs_vocevs_set_param_handler (
 void* this,
 uint32_t param_id,
 void* param_data
)
{
  uint32_t rc = VS_EOK;
  vs_session_object_t* session_obj = (vs_session_object_t*)this;

  switch( param_id )
  {
    case VS_PARAM_ENC_OPERATING_MODE:
     rc = vs_vocevs_cache_enc_operating_mode( session_obj, (vs_param_enc_operating_mode_t*)param_data );
     break;

    case VS_PARAM_ENC_CHANNEL_AWARE_MODE:
     rc =  vs_vocevs_cache_channel_awareness_mode( session_obj, (vs_param_enc_channel_aware_mode_t*)param_data );
     break;

    case VS_PARAM_MAX_PP_SAMPLE_RATE:
     rc = vs_vocevs_cache_max_pp_sample_rate( session_obj, (vs_param_max_pp_sample_rate_t*)param_data );
     break;

    default:
      rc = VS_EUNSUPPORTED;
      break;
  }

  return rc;
}


VS_INTERNAL uint32_t vs_vocevs_init (
 vs_session_object_t* session_obj
)
{
  uint32_t rc = VS_EOK;

  session_obj->vocoder.set_param_fn = vs_vocevs_set_param_handler;
  session_obj->vocoder.set_cached_param_fn = vs_vocevs_set_cached_params_handler;
  session_obj->vocoder.publish_param_fn = vs_vocevs_publish_params;

  return rc;
}


