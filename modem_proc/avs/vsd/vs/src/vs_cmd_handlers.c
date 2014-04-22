/**
  @file vs_cmd_handlers.c
  @brief This file contains the definitions of the internal functions of VS.
*/

/*
  Copyright (C) 2018-2019 QUALCOMM Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.mpss/8.0.c12/vsd/vs/src/vs_cmd_handlers.c#1 $
  $Author: pwbldsvc $
*/

/****************************************************************************
 * HEADER FILES                                                     *
 ****************************************************************************/

#include "mmstd.h"
#include "vs_i.h"

/****************************************************************************
 * VS EXTERN DECLARATIONS
 ****************************************************************************/

extern apr_objmgr_t vs_objmgr;
extern apr_lock_t vs_service_lock;
extern vs_session_object_t* vs_session_obj_list[VS_MAX_NUM_SESSIONS_V];


/****************************************************************************
 * Global variables
 ****************************************************************************/

/* MMODE SUB1 voice system. */
static char_t vs_vsid_core_voice_sub1_stream_name[] = "11C05000";

/* MMODE SUB1 voice system. */
static char_t vs_vsid_core_voice_sub2_stream_name[] = "11DC5000";


/****************************************************************************
 * VS ROUTIINES FOR CONVERSION AND VALIDATION                               *
 ****************************************************************************/

static void vs_voc_reset_vocoder_params (
  vs_session_object_t* session_obj
)
{
  session_obj->vocoder.media_id = VSS_MEDIA_ID_NONE;
  session_obj->vocoder.tty_enable = FALSE;
  session_obj->vocoder.pktexg_event_enable = FALSE;
  session_obj->vocoder.tx_sample_rate = 0;
  session_obj->vocoder.rx_sample_rate = 0;

  session_obj->vocoder.dtx = VS_PARAM_UINT32_NOT_SET;
  session_obj->vocoder.plc = VS_PARAM_UINT8_NOT_SET;
  session_obj->vocoder.encoder_rate = VS_PARAM_UINT8_NOT_SET;
  session_obj->vocoder.encoder_bw = VS_PARAM_UINT8_NOT_SET;
  session_obj->vocoder.decoder_bw = VS_PARAM_UINT8_NOT_SET;
  session_obj->vocoder.min_rate = VS_PARAM_UINT8_NOT_SET;
  session_obj->vocoder.max_rate = VS_PARAM_UINT8_NOT_SET;
  session_obj->vocoder.fec_offset = VS_PARAM_UINT8_NOT_SET;
  session_obj->vocoder.fer_rate = VS_PARAM_UINT8_NOT_SET;
  session_obj->vocoder.channel_awareness_mode = VS_PARAM_UINT8_NOT_SET;

  return;
}

static uint32_t vs_voc_validate_media_id (
  vs_session_object_t* session_obj,
  uint32_t media_id
)
{
  uint32_t rc = VS_EOK;

  switch ( media_id )
  {
    case VS_VOCAMR_MEDIA_ID:
    case VS_VOCEAMR_MEDIA_ID:
     rc = vs_vocamr_init( session_obj );
     break;

    case VS_VOCAMRWB_MEDIA_ID:
     rc = vs_vocamrwb_init( session_obj );
     break;

    case VS_VOCEFR_MEDIA_ID:
      rc = vs_vocefr_init( session_obj );
      break;

    case VS_VOCFR_MEDIA_ID:
      rc = vs_vocfr_init( session_obj );
      break;

    case VS_VOCHR_MEDIA_ID:
      rc = vs_vochr_init( session_obj );
      break;

    case VS_VOCEVS_MEDIA_ID:
      rc = vs_vocevs_init( session_obj );
      break;

    case VS_VOCEVRCB_MEDIA_ID:
      rc = vs_vocevrcb_init( session_obj );
      break;

    case VS_VOCEVRCWB_MEDIA_ID:
      rc = vs_vocevrcwb_init( session_obj );
      break;

    case VS_VOCEVRCNW_MEDIA_ID:
      rc = vs_vocevrcnw_init( session_obj );
      break;

    case VS_VOCEVRCNW2K_MEDIA_ID:
      rc = vs_vocevrcnw2k_init( session_obj );
      break;

    case VS_VOCQCELP13K_MEDIA_ID:
      rc = vs_vocqcelp13k_init( session_obj );
      break;

    case VS_VOCEVRCA_MEDIA_ID:
      rc = vs_vocevrca_init( session_obj );
      break;

    case VS_VOCG711ALAW_V2_MEDIA_ID:
      rc = vs_vocg711alaw_v2_init( session_obj );
      break;

    case VS_VOCG711MULAW_V2_MEDIA_ID:
      rc = vs_vocg711mulaw_v2_init( session_obj );
      break;

#if(defined VS_PCM_VOCODER)
    case VSS_MEDIA_ID_PCM_8_KHZ:
      rc = VS_EOK;
      break;

    case VSS_MEDIA_ID_PCM_16_KHZ:
      rc = VS_EOK;
      break;

    case VSS_MEDIA_ID_PCM_32_KHZ:
      rc = VS_EOK;
      break;

    case VSS_MEDIA_ID_PCM_48_KHZ:
      rc = VS_EOK;
      break;
#endif

    case VS_VOCAMRWB_2_MEDIA_ID:
      rc = vs_vocamrwb2_init( session_obj );
      break;

    default:
      rc = VS_EUNSUPPORTED;
      break;
  }

  return rc;
}


/****************************************************************************
 * GENERIC SET PARAM HANDLERS
 ****************************************************************************/

static uint32_t vs_voc_set_dtx (
  vs_session_object_t* session_obj,
  vs_param_enc_dtx_t* param
)
{
  uint32_t rc = VS_EOK;
  uint8_t dtx_flag= param->dtx;

  session_obj->vocoder.dtx = ( uint32_t )dtx_flag;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "SET_PARAM: VS_PARAM_ENC_DTX media_id=(0x%08x), dtx_flag=(%d) (0-Disable, 1-Enable)",
         session_obj->vocoder.media_id, dtx_flag);

  if( TRUE == session_obj->is_stream_ready )
  {
    ( void ) vs_voc_action_set_dtx( session_obj );
    if( NULL != session_obj->vocoder.publish_param_fn )
    {
      session_obj->vocoder.publish_param_fn( session_obj );
    }
  }

  return rc;
}

static uint32_t vs_voc_set_dtx_v2 (
  vs_session_object_t* session_obj,
  vs_param_enc_dtx_v2_t* param
)
{
  uint32_t rc = VS_EOK;

  session_obj->vocoder.dtx = param->dtx_mode;
  session_obj->vocoder.dtx_param_version = param->version;

  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "VS: dtx_mode=(%d) and version=(%d)", param->dtx_mode ,param->version );

  if( TRUE == session_obj->is_stream_ready )
  {
    ( void ) vs_voc_action_set_dtx_v2( session_obj );
    if( NULL != session_obj->vocoder.publish_param_fn )
    {
      session_obj->vocoder.publish_param_fn( session_obj );
    }
  }

  return rc;
}


static uint32_t vs_voc_set_oobtty (
  vs_session_object_t* session_obj,
  vs_param_oobtty_enable_t* tty
)
{
  uint32_t rc = VS_EOK;

  if ( VS_CLIENT_STATE_ENABLED == session_obj->vocoder.client_state ) return VS_EFAILED;

  session_obj->vocoder.tty_enable = tty->enable_flag;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "SET_PARAM: VS_PARAM_OOBTTY_ENABLE. enable_flag=(%d) (0-Disable, 1-Enable)",
         tty->enable_flag );

  return rc;
}

static uint32_t vs_voc_set_pktexg_event (
  vs_session_object_t* session_obj,
  vs_param_voc_pktexg_event_enable_t* pktexg
)
{
  uint32_t rc = VS_EOK;

  if ( VS_CLIENT_STATE_ENABLED == session_obj->vocoder.client_state ) return VS_EFAILED;

  session_obj->vocoder.pktexg_event_enable = pktexg->enable_flag;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "SET_PARAM: VS_PARAM_VOC_PKTEXG_EVENT_ENABLE. enable_flag=(%d) (0-Disable, 1-Enable)",
         pktexg->enable_flag );

  return rc;
}

static uint32_t vs_voc_set_hpcm_config (
  vs_session_object_t* session_obj,
  vs_param_hpcm_tap_point_config_t* hpcm
)
{
  uint32_t rc = VS_EOK;

  if ( VS_CLIENT_STATE_ENABLED == session_obj->hpcm.client_state ) return VS_EFAILED;

  if( VS_HPCM_TAP_POINT_STREAM_TX == hpcm->tap_point_id )
  {
    session_obj->hpcm.txMode = hpcm->mode;
    session_obj->hpcm.txSampleRate = hpcm->sample_rate;
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "SET_PARAM: VS_PARAM_HPCM_TAP_POINT_CONFIG. tap_point_id=(STREAM_TX),"
           "mode=(%d) (0-r, 1-w, 2-rw), sample_rate=(%d)", hpcm->mode, hpcm->sample_rate );
  }

  if( VS_HPCM_TAP_POINT_STREAM_RX == hpcm->tap_point_id )
  {
    session_obj->hpcm.rxMode = hpcm->mode;
    session_obj->hpcm.rxSampleRate = hpcm->sample_rate;
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "SET_PARAM: VS_PARAM_HPCM_TAP_POINT_CONFIG. tap_point_id=(STREAM_RX),"
           "mode=(%d) (0-r, 1-w, 2-rw), sample_rate=(%d)", hpcm->mode, hpcm->sample_rate );
  }

  return rc;
}

static uint32_t vs_voc_set_timing_offsets (
  vs_session_object_t* session_obj,
  vs_param_voc_timing_offsets_t* timing
)
{
  uint32_t rc = VS_EOK;

  if ( VS_CLIENT_STATE_ENABLED == session_obj->vocoder.client_state ) return VS_EFAILED;

  session_obj->vocoder.timing_config.is_timing_ref_available = FALSE;
  session_obj->vocoder.timing_config.enc_offset_us = timing->enc_offset;
  session_obj->vocoder.timing_config.dec_req_offset_us = timing->dec_req_offset;
  session_obj->vocoder.timing_config.dec_offset_us = timing->dec_offset;
  session_obj->vocoder.vfr_config.vfr_mode = timing->vfr_mode;
  session_obj->vocoder.vfr_config.vfr_cycle_duration_ms = 20;

  MSG_4( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
        "SET_PARAM:VS_PARAM_VOC_TIMING_OFFSETS: enc_offset_us=(%d), dec_req_offset_us=(%d)"
        "dec_offset_us=(%d), vfr_mode=(%d) (0-soft, 1-hard), vfr_cycle_ms=(20)", timing->enc_offset,
         timing->dec_req_offset, timing->dec_offset, timing->vfr_mode );


  return rc;
}

static uint32_t vs_voc_set_timing_reference (
  vs_session_object_t* session_obj,
  vs_param_voc_timing_reference_t* timing
)
{
  uint32_t rc = VS_EOK;

  if ( VS_CLIENT_STATE_ENABLED == session_obj->vocoder.client_state ) return VS_EFAILED;

  session_obj->vocoder.timing_config.is_timing_ref_available = TRUE;
  session_obj->vocoder.timing_config.tx_ref_timestamp_us = timing->read_ref_timestamp_us;
  session_obj->vocoder.timing_config.rx_ref_timestamp_us = timing->write_ref_timestamp_us;
  session_obj->vocoder.vfr_config.vfr_mode = timing->vfr_mode;
  session_obj->vocoder.vfr_config.vfr_cycle_duration_ms = timing->vfr_cycle_duration_ms;

  MSG_4( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
        "SET_PARAM:VS_PARAM_VOC_TIMING_REFERENCES: read_ref_timestamp_us=(%d), write_ref_timestamp_us=(%d)"
        "vfr_mode=(%d) (0-soft, 1-hard), vfr_cycle_duration_ms=(%d)", timing->read_ref_timestamp_us,
         timing->write_ref_timestamp_us, timing->vfr_mode, timing->vfr_cycle_duration_ms );

  return rc;
}

static uint32_t vs_voc_set_media_id (
  vs_session_object_t* session_obj,
  vs_param_voc_media_id_t* media
)
{
  uint32_t rc = VS_EOK;

  if ( VS_CLIENT_STATE_ENABLED == session_obj->vocoder.client_state ) return VS_EFAILED;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "SET_PARAM:VS_PARAM_VOC_MEDIA_ID: media_id=(0x%08x)", media->media_id );

  rc = vs_voc_validate_media_id( session_obj, media->media_id );
  if( VS_EOK == rc )
  {
    session_obj->vocoder.media_id = media->media_id;
  }

  return rc;
}

/****************************************************************************
 * VS COMMAND HANDLERS 
 ****************************************************************************/

VS_INTERNAL uint32_t vs_hpcm_open_cmd_ctrl (
  vs_hpcm_cmd_open_t* params
)
{
  uint32_t rc = VS_EOK;
  uint32_t index = 0;
  vs_session_object_t* session_obj = NULL;

  ( void ) apr_lock_enter( vs_service_lock );

  for ( ;; )
  {
    if ( ( NULL == params ) || ( NULL == params->ret_handle ) )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EBADPARAM );
      rc = VS_EBADPARAM;
      break;
    }

    if( ( VS_VSID_SUB1 != params->vsid ) &&
        ( VS_VSID_SUB2 != params->vsid ) )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VS: CMD_OPEN: Unsupported VSID=(0x%08x)", params->vsid );
      rc = VS_EBADPARAM;
      break;
    }

    /** Search available session object with VS_SYSTEM_ID_NONE. */
    for( index = 0; index < VS_MAX_NUM_SESSIONS_V; ++index )
    {
      session_obj = vs_session_obj_list[index];
      if ( params->vsid == session_obj->vsid )
       break;
      else if ( VS_PARAM_UINT32_NOT_SET == session_obj->vsid )
      {
        session_obj->vsid = params->vsid;
        break;
      }
      else 
        session_obj = NULL;
    }

    if ( NULL == session_obj )
    {
      rc = VS_EFAILED;
    }
    else
    {
      session_obj->hpcm.client_cb = params->event_cb;
      session_obj->hpcm.session_context = params->session_context;
      session_obj->hpcm.client_state = VS_CLIENT_STATE_DISABLED;
      session_obj->hpcm.txMode = VS_PARAM_UINT8_NOT_SET;
      session_obj->hpcm.rxMode = VS_PARAM_UINT8_NOT_SET;

      *params->ret_handle = session_obj->header.handle;
      rc = VS_EOK;
    }

    break;
  }

  ( void )apr_lock_leave( vs_service_lock );

  return rc;
}

VS_INTERNAL uint32_t vs_hpcm_enable_cmd_ctrl (
  vs_gating_control_t* ctrl
)
{
  uint32_t rc = APR_EOK;
  uint32_t session_handle = APR_NULL_V;
  vs_hpcm_cmd_enable_t* cmd_params = NULL;
  vs_session_object_t* session_obj = NULL;
  vs_common_event_cmd_response_t cmd_rsp;

  for ( ;; )
  {
    if ( NULL == ctrl->packet  )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rc = APR_EFAILED;
      break;
    }

    cmd_params = APRV2_PKT_GET_PAYLOAD( vs_hpcm_cmd_enable_t, ctrl->packet );
    if ( NULL == cmd_params )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rc = APR_EFAILED;
      break;
    }

    cmd_rsp.token= cmd_params->token;
    session_handle = cmd_params->handle;
    rc = vs_get_typed_object( session_handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              (vs_object_t**)&session_obj );
    if( NULL == session_obj )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rc = APR_EFAILED;
      break;
    }

    if( ( VS_PARAM_UINT8_NOT_SET == session_obj->hpcm.txMode ) &&
        ( VS_PARAM_UINT8_NOT_SET == session_obj->hpcm.rxMode ) )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rc = VS_EFAILED;
      break;
    }

    VS_ACQUIRE_LOCK( session_obj->session_lock );

    if ( ctrl->state == VS_GATING_CMD_STATE_ENUM_EXECUTE )
    {
      session_obj->hpcm.client_state = VS_CLIENT_STATE_ENABLED;

      if( VS_CLIENT_STATE_ENABLED != session_obj->vocoder.client_state )
      {
        session_obj->session_ctrl.status = VS_EOK;
      }
      else
      {
        session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;
        ( void ) vs_voc_action_hpcm_start( session_obj );
      }
    }

    if( APR_UNDEFINED_ID_V == session_obj->session_ctrl.status )
    {
      rc = VS_EPENDING;
    }
    else
    {
      rc = session_obj->session_ctrl.status;
    }

    VS_RELEASE_LOCK( session_obj->session_lock );

    break;
  }

  if( APR_EPENDING != rc )
  {
    cmd_rsp.cmd_id = VS_HPCM_CMD_ENABLE;
    cmd_rsp.status = rc;
    vs_common_send_event( session_handle, VS_COMMON_EVENT_CMD_RESPONSE,
                          &cmd_rsp, sizeof( vs_common_event_cmd_response_t ) );
    rc = VS_EOK;
  }

  return rc;
}


VS_INTERNAL uint32_t vs_hpcm_disable_cmd_ctrl (
  vs_gating_control_t* ctrl
)
{
  uint32_t rc = VS_EOK;
  uint32_t session_handle = APR_NULL_V;
  vs_hpcm_cmd_disable_t* cmd_params = NULL;
  vs_session_object_t* session_obj = NULL;
  vs_common_event_cmd_response_t cmd_rsp;

  for ( ;; )
  {
    if ( NULL == ctrl->packet  )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rc = APR_EFAILED;
      break;
    }

    cmd_params = APRV2_PKT_GET_PAYLOAD( vs_hpcm_cmd_disable_t, ctrl->packet );
    if ( NULL == cmd_params )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rc = APR_EFAILED;
      break;
    }

    cmd_rsp.token= cmd_params->token;
    session_handle = cmd_params->handle;
    rc = vs_get_typed_object( session_handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              (vs_object_t**)&session_obj );
    if( NULL == session_obj )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rc = APR_EFAILED;
      break;
    }

    VS_ACQUIRE_LOCK( session_obj->session_lock );

    if ( ctrl->state == VS_GATING_CMD_STATE_ENUM_EXECUTE )
    {
      session_obj->hpcm.client_state = VS_CLIENT_STATE_DISABLED;

      if( VS_CLIENT_STATE_ENABLED != session_obj->vocoder.client_state )
      {
        session_obj->session_ctrl.status = VS_EOK;
      }
      else
      {
        session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;
        ( void ) vs_voc_action_hpcm_stop( session_obj );
      }
    }

    if( APR_UNDEFINED_ID_V == session_obj->session_ctrl.status )
    {
      rc = VS_EPENDING;
    }
    else
    {
      rc = session_obj->session_ctrl.status;
    }

    VS_RELEASE_LOCK( session_obj->session_lock );

    break;
  }

  if( APR_EPENDING != rc )
  {
    cmd_rsp.cmd_id = VS_HPCM_CMD_DISABLE;
    cmd_rsp.status = rc;
    vs_common_send_event( session_handle, VS_COMMON_EVENT_CMD_RESPONSE,
                          &cmd_rsp, sizeof( vs_common_event_cmd_response_t ) );
    rc = VS_EOK;
  }

  return rc;
}


VS_INTERNAL uint32_t vs_hpcm_close_cmd_ctrl (
  vs_gating_control_t* ctrl
)
{
  uint32_t rc = APR_EOK;
  uint32_t session_handle = APR_NULL_V;
  vs_hpcm_cmd_close_t* cmd_params = NULL;
  vs_session_object_t* session_obj = NULL;
  vs_common_event_cmd_response_t cmd_rsp;

  for ( ;; )
  {
    if ( NULL == ctrl->packet  )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rc = APR_EFAILED;
      break;
    }

    cmd_params = APRV2_PKT_GET_PAYLOAD( vs_hpcm_cmd_close_t, ctrl->packet );
    if ( NULL == cmd_params )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rc = APR_EFAILED;
      break;
    }

    cmd_rsp.token= cmd_params->token;
    session_handle = cmd_params->handle;
    rc = vs_get_typed_object( session_handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              (vs_object_t**)&session_obj );
    if( NULL == session_obj )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rc = APR_EFAILED;
      break;
    }

    VS_ACQUIRE_LOCK( session_obj->session_lock );

    if ( ctrl->state == VS_GATING_CMD_STATE_ENUM_EXECUTE )
    {
      session_obj->hpcm.client_state = VS_CLIENT_STATE_DISABLED;

      if( VS_CLIENT_STATE_ENABLED != session_obj->vocoder.client_state )
      {
        session_obj->session_ctrl.status = VS_EOK;
      }
      else
      {
        session_obj->session_ctrl.status = APR_UNDEFINED_ID_V;
        ( void ) vs_voc_action_hpcm_stop( session_obj );
      }
    }

    if( APR_UNDEFINED_ID_V == session_obj->session_ctrl.status )
    {
      rc = VS_EPENDING;
    }
    else
    {
      rc = session_obj->session_ctrl.status;
    }

    VS_RELEASE_LOCK( session_obj->session_lock );

    break;
  }

  if( APR_EPENDING != rc )
  {
    cmd_rsp.cmd_id = VS_HPCM_CMD_CLOSE;
    cmd_rsp.status = rc;
    vs_common_send_event( session_handle, VS_COMMON_EVENT_CMD_RESPONSE,
                          &cmd_rsp, sizeof( vs_common_event_cmd_response_t ) );

    if( NULL != session_obj )
    {
      session_obj->hpcm.client_state = VS_CLIENT_STATE_CLOSED;
      session_obj->hpcm.client_cb = NULL;
      session_obj->hpcm.session_context = NULL;
    }
    rc = VS_EOK;
  }

  return rc;
}


VS_INTERNAL uint32_t vs_voc_open_cmd_ctrl (
  vs_voc_cmd_open_t* params
)
{
  uint32_t rc = VS_EOK;
  uint32_t index = 0;
  vs_session_object_t* session_obj = NULL;
#if (!defined AVS_MPSS_TEST)   
  ipa_dma_ch_param_t ipa_param_ul, ipa_param_dl;
  ipa_err_code_e ipa_rc;
#endif

  ( void ) apr_lock_enter( vs_service_lock );

  for ( ;; )
  {
    if ( ( NULL == params ) || ( NULL == params->ret_handle ) )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EBADPARAM );
      rc = VS_EBADPARAM;
      break;
    }

    if( ( VS_VSID_SUB1 != params->vsid ) &&
        ( VS_VSID_SUB2 != params->vsid ) )
     {
       MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
              "VS: CMD_OPEN: Unsupported VSID=(0x%08x)", params->vsid );
       rc = VS_EBADPARAM;
       break;
     }

    /** Search available session object with VS_SYSTEM_ID_NONE. */
    for( index = 0; index < VS_MAX_NUM_SESSIONS_V; ++index )
    {
      session_obj = vs_session_obj_list[index];
      if ( params->vsid == session_obj->vsid )
       break;
      else if ( VS_PARAM_UINT32_NOT_SET == session_obj->vsid )
      {
        session_obj->vsid = params->vsid;
        break;
      }
      else 
        session_obj = NULL;
    }

    if ( NULL == session_obj )
    {
      rc = VS_EFAILED;
    }
    else
    {
      //session_obj->vsid = params->vsid;
      session_obj->vocoder.client_id = params->network_id;
      session_obj->vocoder.client_cb = params->event_cb;
      session_obj->vocoder.session_context = params->session_context;
      session_obj->vocoder.client_state = VS_CLIENT_STATE_DISABLED;

      switch ( session_obj->vsid )
      {
        case VS_VSID_SUB1:
          mmstd_strlcpy( session_obj->session_name,
                         vs_vsid_core_voice_sub1_stream_name, VS_MAX_CVD_SESSION_NAME_SIZE_V );
          break;

        case VS_VSID_SUB2:
          mmstd_strlcpy( session_obj->session_name,
                         vs_vsid_core_voice_sub2_stream_name, VS_MAX_CVD_SESSION_NAME_SIZE_V );
          break;
      }

      ( void ) vs_voc_reset_vocoder_params( session_obj );

      session_obj->vocoder.vfr_config.vfr_cycle_duration_ms = 20;
      session_obj->vocoder.vfr_config.vfr_mode = VS_VOC_VFR_MODE_SOFT_V;

      session_obj->vocoder.timing_config.is_timing_ref_available = FALSE;
      session_obj->vocoder.timing_config.dec_offset_us = VS_VOC_TIMING_DEC_OFFSET_V;
      session_obj->vocoder.timing_config.dec_req_offset_us = VS_VOC_TIMING_DEC_REQ_OFFSET_V;
      session_obj->vocoder.timing_config.enc_offset_us = VS_VOC_TIMING_ENC_OFFSET_V;
      session_obj->vocoder.timing_config.rx_ref_timestamp_us = 0;
      session_obj->vocoder.timing_config.tx_ref_timestamp_us = 0;

      *params->ret_handle = session_obj->header.handle;
      if( vs_platform_is_fusion )
      {
#if (!defined AVS_MPSS_TEST)
        ipa_param_ul.cb = NULL;
        ipa_param_ul.client_type = IPA_DMA_CLIENT_TYPE_AUDIO_UL;
        ipa_param_ul.ch_irq_mode_type = IPA_DMA_CH_IRQ_MODE_TYPE_IRQ;
        ipa_param_ul.options = 0;
		ipa_param_ul.size = 0;
        ipa_param_ul.is_async = FALSE;    
        ipa_rc = ipa_dma_open( &ipa_param_ul );
        VS_PANIC_ON_ERROR(ipa_rc);
        session_obj->ipa_ul_handle = ipa_param_ul.ch_handle;
        ipa_rc = ipa_vote( IPA_VOTE_CLIENT_TYPE_AUDIO_UL, 0 );
        VS_PANIC_ON_ERROR(ipa_rc);
    
        ipa_param_dl.cb = NULL;
        ipa_param_dl.client_type = IPA_DMA_CLIENT_TYPE_AUDIO_DL;
        ipa_param_dl.ch_irq_mode_type = IPA_DMA_CH_IRQ_MODE_TYPE_IRQ;
        ipa_param_dl.options = 0;
        ipa_param_dl.size = 0;
        ipa_param_dl.is_async = FALSE;
        ipa_rc = ipa_dma_open( &ipa_param_dl );
        VS_PANIC_ON_ERROR(ipa_rc);
        session_obj->ipa_dl_handle = ipa_param_dl.ch_handle;
        ipa_rc = ipa_vote( IPA_VOTE_CLIENT_TYPE_AUDIO_DL, 0 );
        VS_PANIC_ON_ERROR(ipa_rc);
#endif
      }
      rc = VS_EOK;
    }
    break;
  }
  ( void )apr_lock_leave( vs_service_lock );

  return rc;
}


VS_INTERNAL uint32_t vs_set_param_cmd_ctrl (
  vs_common_cmd_set_param_t* cmd_params
)
{
  uint32_t rc = VS_EOK;
  vs_session_object_t* session_obj = NULL;

  for ( ;; )
  {

    if ( ( NULL == cmd_params ) || ( NULL == cmd_params->param_data ) )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EBADPARAM );
      rc = VS_EBADPARAM;
      break;
    }

    rc = vs_get_typed_object( cmd_params->handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &session_obj ) );
    if( NULL == session_obj )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VS_COMMON_CMD_SET_PARAM: Invalid handle[0x%08x] passed by client.",
             cmd_params->handle );
      rc = VS_EHANDLE;
      break;
    }


    VS_ACQUIRE_LOCK( session_obj->session_lock );

    switch ( cmd_params->param_id )
    {
      case VS_PARAM_OOBTTY_ENABLE:
        rc = vs_voc_set_oobtty( session_obj, (vs_param_oobtty_enable_t*)cmd_params->param_data );
        break;

      case VS_PARAM_VOC_PKTEXG_EVENT_ENABLE:
        rc = vs_voc_set_pktexg_event( session_obj, (vs_param_voc_pktexg_event_enable_t*)cmd_params->param_data );
        break;

      case VS_PARAM_HPCM_TAP_POINT_CONFIG:
        rc = vs_voc_set_hpcm_config( session_obj, (vs_param_hpcm_tap_point_config_t*)cmd_params->param_data );
        break;

      case VS_PARAM_VOC_TIMING_OFFSETS:
        rc = vs_voc_set_timing_offsets( session_obj, (vs_param_voc_timing_offsets_t*)cmd_params->param_data );
        break;

      case VS_PARAM_VOC_TIMING_REFERENCES:
        rc = vs_voc_set_timing_reference( session_obj, (vs_param_voc_timing_reference_t*)cmd_params->param_data );
        break;

      case VS_PARAM_VOC_MEDIA_ID:
        rc = vs_voc_set_media_id( session_obj, (vs_param_voc_media_id_t*)cmd_params->param_data );
        break;

      case VS_PARAM_ENC_DTX:
        rc = vs_voc_set_dtx( session_obj, (vs_param_enc_dtx_t*) cmd_params->param_data );
        break;

      case VS_PARAM_ENC_OPERATING_MODE:
      case VS_PARAM_MAX_PP_SAMPLE_RATE:
      case VS_PARAM_ENC_RATE:
      case VS_PARAM_ENC_MINMAX_FRAME_RATE:
      case VS_PARAM_ENC_CHANNEL_AWARE_MODE:
      case VS_PARAM_VOC_PLC:
      {
        if( NULL != session_obj->vocoder.set_param_fn )
          rc = session_obj->vocoder.set_param_fn( session_obj, cmd_params->param_id, cmd_params->param_data );
        else
          rc = VS_EFAILED;
      } break;

      case VS_PARAM_ENC_DTX_V2:
        rc = vs_voc_set_dtx_v2( session_obj, (vs_param_enc_dtx_v2_t*) cmd_params->param_data );
        break;

      default:
        rc = VS_EUNSUPPORTED;
        break;
    }

    VS_RELEASE_LOCK( session_obj->session_lock );

    break;
  }

  return rc;
}


VS_INTERNAL uint32_t vs_voc_enable_cmd_ctrl (
  vs_gating_control_t* ctrl
)
{
  uint32_t rc = APR_EOK;
  uint32_t session_handle = APR_NULL_V;
  vs_voc_cmd_enable_t* cmd_params = NULL;
  vs_session_object_t* session_obj = NULL;
  vs_sequencer_job_object_t* seq_obj = NULL;
  vs_common_event_cmd_response_t cmd_rsp;

  for ( ;; )
  {
    if ( NULL == ctrl->packet  )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rc = APR_EFAILED;
      break;
    }

    cmd_params = APRV2_PKT_GET_PAYLOAD( vs_voc_cmd_enable_t, ctrl->packet );
    if ( NULL == cmd_params )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rc = APR_EFAILED;
      break;
    }

    cmd_rsp.token= cmd_params->token;
    session_handle = cmd_params->handle;
    rc = vs_get_typed_object( session_handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              (vs_object_t**)&session_obj );
    if( NULL == session_obj )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rc = APR_EFAILED;
      break;
    }

    if ( ctrl->state == VS_GATING_CMD_STATE_ENUM_EXECUTE )
    {
      if ( FALSE == vs_is_cvd_up() )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "VS: session Handle=(0X%08X) enable failed as CVD service is Down",
               cmd_params->handle );
        rc = VS_EFAILED;
        break;
      }

      session_obj->vocoder.client_state = VS_CLIENT_STATE_ENABLED;

      /* Create the sequencer job object to track CVD setup. */
      rc = vs_create_sequencer_job_object(
             ( vs_sequencer_job_object_t** ) &ctrl->rootjob_obj );
      VS_PANIC_ON_ERROR( rc );

      seq_obj = &ctrl->rootjob_obj->sequencer_job;
      seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_CREATE;

      if ( VS_VOC_STATE_ENUM_IDLE == session_obj->session_ctrl.state )
      {
        seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_CONFIGURE;
      }
    }

    VS_ACQUIRE_LOCK( session_obj->session_lock );
		
    seq_obj = &ctrl->rootjob_obj->sequencer_job;
    rc = vs_voc_helper_vocoder_setup_ctrl( seq_obj, session_obj );

    VS_RELEASE_LOCK( session_obj->session_lock );

    break;
  }

  if( APR_EPENDING != rc )
  {
    cmd_rsp.cmd_id = VS_VOC_CMD_ENABLE;
    cmd_rsp.status = rc;
    vs_common_send_event( session_handle, VS_COMMON_EVENT_CMD_RESPONSE,
                          &cmd_rsp, sizeof( vs_common_event_cmd_response_t ) );
    ( void ) vs_mem_free_object( ( vs_object_t* ) seq_obj );
    rc = VS_EOK;
  }

  return rc;
}


VS_INTERNAL uint32_t vs_voc_disable_cmd_ctrl (
  vs_gating_control_t* ctrl
)
{
  uint32_t rc = APR_EOK;
  uint32_t session_handle = APR_NULL_V;
  vs_voc_cmd_disable_t* cmd_params = NULL;
  vs_session_object_t* session_obj = NULL;
  vs_sequencer_job_object_t* seq_obj = NULL;
  vs_common_event_cmd_response_t cmd_rsp;

  for ( ;; )
  {
    if ( NULL == ctrl->packet  )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rc = APR_EFAILED;
      break;
    }

    cmd_params = APRV2_PKT_GET_PAYLOAD( vs_voc_cmd_disable_t, ctrl->packet );
    if ( NULL == cmd_params )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rc = APR_EFAILED;
      break;
    }

    cmd_rsp.token= cmd_params->token;
    session_handle = cmd_params->handle;
    rc = vs_get_typed_object( session_handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              (vs_object_t**)&session_obj );
    if( NULL == session_obj )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rc = APR_EFAILED;
      break;
    }

    if ( ctrl->state == VS_GATING_CMD_STATE_ENUM_EXECUTE )
    {
      if ( FALSE == vs_is_cvd_up() )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "VS: session Handle=(0X%08X) disable failed as CVD service is Down",
               session_handle );
        rc = VS_EFAILED;
        break;
      }

      session_obj->vocoder.client_state = VS_CLIENT_STATE_DISABLED;

      rc = vs_create_sequencer_job_object(
             ( vs_sequencer_job_object_t** ) &ctrl->rootjob_obj );
      VS_PANIC_ON_ERROR(rc);

      seq_obj = &ctrl->rootjob_obj->sequencer_job;
      seq_obj->state = VS_HELPER_SEQUENCER_ENUM_COMPLETE;
      if ( VS_VOC_STATE_ENUM_RUN == session_obj->session_ctrl.state )
      {
        seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_DISABLE;
      }
    }

    VS_ACQUIRE_LOCK( session_obj->session_lock );

    seq_obj = &ctrl->rootjob_obj->sequencer_job;
    rc = vs_voc_helper_vocoder_setup_ctrl( seq_obj, session_obj );

    VS_RELEASE_LOCK( session_obj->session_lock );

    break;
  }

  if( APR_EPENDING != rc )
  {
    cmd_rsp.cmd_id = VS_VOC_CMD_DISABLE;
    cmd_rsp.status = rc;
    vs_common_send_event( session_handle, VS_COMMON_EVENT_CMD_RESPONSE,
                          &cmd_rsp, sizeof( vs_common_event_cmd_response_t ) );

    ( void ) vs_mem_free_object( (vs_object_t*)seq_obj );

    if( NULL != session_obj )
    {
      ( void ) vs_voc_reset_vocoder_params( session_obj );
    }
    rc = VS_EOK;
  }

  return rc;
}


VS_INTERNAL uint32_t vs_voc_close_cmd_ctrl (
  vs_gating_control_t* ctrl
)
{
  uint32_t rc = APR_EOK;
  uint32_t session_handle = APR_NULL_V;
  vs_voc_cmd_close_t* cmd_params = NULL;
  vs_session_object_t* session_obj = NULL;
  vs_sequencer_job_object_t* seq_obj = NULL;
  vs_common_event_cmd_response_t cmd_rsp;
#if (!defined AVS_MPSS_TEST)   
    ipa_err_code_e ipa_rc;
#endif

  for ( ;; )
  {
    if ( NULL == ctrl->packet  )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rc = APR_EFAILED;
      break;
    }

    cmd_params = APRV2_PKT_GET_PAYLOAD( vs_voc_cmd_close_t, ctrl->packet );
    if ( NULL == cmd_params )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EBADPARAM );
      rc = APR_EFAILED;
      break;
    }

    cmd_rsp.token= cmd_params->token;
    session_handle = cmd_params->handle;
    rc = vs_get_typed_object( session_handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              (vs_object_t**)&session_obj );
    if( NULL == session_obj )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      rc = APR_EFAILED;
      break;
    }

    VS_ACQUIRE_LOCK( session_obj->session_lock );

    if ( ctrl->state == VS_GATING_CMD_STATE_ENUM_EXECUTE )
    {
      session_obj->vocoder.client_state = VS_CLIENT_STATE_CLOSED;

      rc = vs_create_sequencer_job_object(
             ( vs_sequencer_job_object_t** ) &ctrl->rootjob_obj );
      VS_PANIC_ON_ERROR(rc);

      seq_obj = &ctrl->rootjob_obj->sequencer_job;

      if ( VS_VOC_STATE_ENUM_RUN == session_obj->session_ctrl.state )
      {
        seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_DISABLE;
      }
      else if ( VS_VOC_STATE_ENUM_IDLE == session_obj->session_ctrl.state )
      {
        seq_obj->state = VS_HELPER_SEQUENCER_ENUM_GOAL_DESTROY;
      }
      else
      {
        seq_obj->state = VS_HELPER_SEQUENCER_ENUM_COMPLETE;
        seq_obj->status = VS_EOK;
      }
      if( vs_platform_is_fusion )
      {
#if (!defined AVS_MPSS_TEST) 	
        ipa_rc = ipa_devote( IPA_VOTE_CLIENT_TYPE_AUDIO_UL );
        VS_PANIC_ON_ERROR(ipa_rc);
        ipa_rc = ipa_devote( IPA_VOTE_CLIENT_TYPE_AUDIO_DL );
        VS_PANIC_ON_ERROR(ipa_rc);

        ipa_rc = ipa_dma_close(session_obj->ipa_ul_handle );
        VS_PANIC_ON_ERROR(ipa_rc);
        ipa_rc = ipa_dma_close(session_obj->ipa_dl_handle );
        VS_PANIC_ON_ERROR(ipa_rc);
#endif
      }
    }

    seq_obj = &ctrl->rootjob_obj->sequencer_job;
    rc = vs_voc_helper_vocoder_setup_ctrl( seq_obj, session_obj );
    VS_RELEASE_LOCK( session_obj->session_lock );

    break;
  }

  if( APR_EPENDING != rc )
  {
    ( void ) vs_mem_free_object( (vs_object_t*)seq_obj );
    cmd_rsp.cmd_id = VS_VOC_CMD_CLOSE;
    cmd_rsp.status = rc;
    vs_common_send_event( session_handle, VS_COMMON_EVENT_CMD_RESPONSE,
                          &cmd_rsp, sizeof( vs_common_event_cmd_response_t ) );

    if( NULL != session_obj )
    {
      session_obj->vocoder.client_cb = NULL;
      session_obj->vocoder.client_id = VS_PARAM_UINT32_NOT_SET;
      session_obj->vocoder.session_context = NULL;
    }
    rc = VS_EOK;
  }

  return rc;
}


VS_INTERNAL uint32_t vs_voc_cleanup_adsp_reference_cmd_ctrl (
  vs_gating_control_t* ctrl
)
{
  uint32_t rc = APR_EOK;
  uint32_t index;
  vs_session_object_t* session_obj = NULL;
  //vs_ssr_pdr_state_change_t* cmd_params = NULL;

  for ( ;; )
  {
    if ( NULL == ctrl->packet )
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      return APR_EUNEXPECTED;
    }

    VS_ACQUIRE_LOCK( vs_service_lock );

    for( index = 0; index < VS_MAX_NUM_SESSIONS_V; index++ )
    {
      session_obj = vs_session_obj_list[index];
      session_obj->cvs_port = APR_NULL_V;
      session_obj->mvm_port = APR_NULL_V;
      session_obj->memmap_handle = APR_NULL_V;
      session_obj->session_ctrl.state = VS_VOC_STATE_ENUM_RESET;

      if ( TRUE == session_obj->is_stream_ready )
      {
        session_obj->is_stream_ready = FALSE;
        ( void ) vs_common_send_event( session_obj->header.handle,
                                       VS_VOC_EVENT_NOT_READY, NULL, 0 );
      }
    }

    ( void ) vs_free_all_tracked_objects();

    vs_cvd_cleanup_is_done = TRUE;

    VS_RELEASE_LOCK( vs_service_lock );

    break;
  }

  return rc;
}


VS_INTERNAL uint32_t vs_get_encoder_buffer_cmd_ctrl (
  vs_pktexg_cmd_get_encoder_buffer_t* cmd_params
)
{
  uint32_t rc = VS_EOK;
  uint32_t session_handle = 0;
  vs_session_object_t* session_obj = NULL;
  vs_pktexg_buffer_t** mboxBuffer = NULL;

  if ( NULL == cmd_params )
  {
    VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return VS_EBADPARAM;
  }

  session_handle = cmd_params->handle;
  rc = vs_get_typed_object( session_handle, VS_OBJECT_TYPE_ENUM_SESSION,
                            (vs_object_t**)&session_obj );
  if( NULL == session_obj )
  {
    VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return VS_EHANDLE;
  }

  VS_ACQUIRE_LOCK( session_obj->session_lock );

  if( FALSE == session_obj->is_stream_ready )
  {
    rc = APR_ENOTEXIST;
  }
  else
  {
    mboxBuffer = (vs_pktexg_buffer_t**)cmd_params->buffer;
    rc = vs_voc_action_read_enc_mailbox( session_obj, mboxBuffer );
  }

  VS_RELEASE_LOCK( session_obj->session_lock );

  return rc;
}


VS_INTERNAL uint32_t vs_return_encoder_buffer_cmd_ctrl (
  vs_pktexg_cmd_return_encoder_buffer_t* cmd_params
)
{
  uint32_t rc = VS_EOK;
  uint32_t session_handle = 0;
  vs_session_object_t* session_obj = NULL;
  vs_shmem_block_list_item_t* encItem;

  if ( NULL == cmd_params )
  {
    VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return VS_EBADPARAM;
  }

  if ( NULL == cmd_params->buffer )
  {
    VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return VS_EBADPARAM;
  }

  session_handle = cmd_params->handle;
  rc = vs_get_typed_object( session_handle, VS_OBJECT_TYPE_ENUM_SESSION,
                            (vs_object_t**)&session_obj );
  if( NULL == session_obj )
  {
    VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return VS_EHANDLE;
  }

  VS_ACQUIRE_LOCK( session_obj->session_lock );

  rc = apr_list_remove_head( &session_obj->vocoder.mbox_config.usedEncBufferQ,
                             ( ( apr_list_node_t** ) &encItem ) );
  if ( rc )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "VS: usedEncBufferQ is empty" );
  }
  else
  {
    apr_list_add_tail( &session_obj->vocoder.mbox_config.freeEncBufferQ, &encItem->link );
    ( void ) vs_voc_action_put_mailbox_enc_request ( session_obj );
  }

  VS_RELEASE_LOCK( session_obj->session_lock );

  return VS_EOK;
}


VS_INTERNAL uint32_t vs_get_decoder_buffer_cmd_ctrl (
  vs_pktexg_cmd_get_decoder_buffer_t* cmd_params
)

{
  uint32_t rc = VS_EOK;
  uint32_t session_handle = 0;
  vs_session_object_t* session_obj = NULL;
  vs_pktexg_buffer_t** mboxBuffer = NULL;
  vs_shmem_block_list_item_t* decItem;

  if ( NULL == cmd_params )
  {
    VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return VS_EBADPARAM;
  }
  
  session_handle = cmd_params->handle;
  rc = vs_get_typed_object( session_handle, VS_OBJECT_TYPE_ENUM_SESSION,
                            (vs_object_t**)&session_obj );
  if( NULL == session_obj )
  {
    VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return VS_EHANDLE;
  }

  VS_ACQUIRE_LOCK( session_obj->session_lock );

  mboxBuffer = (vs_pktexg_buffer_t**)cmd_params->buffer;
  rc = apr_list_peak_head( &session_obj->vocoder.mbox_config.freeDecBufferQ,
                           ( ( apr_list_node_t** ) &decItem ) );
  if ( rc )
  {
    *mboxBuffer= NULL;
     MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,"VS: freeDecBufferQ is empty" );
  }
  else
  {
    *mboxBuffer = (vs_pktexg_buffer_t*)decItem->buf.virtAddr;
  }

  VS_RELEASE_LOCK( session_obj->session_lock );

  return rc;
}


VS_INTERNAL uint32_t vs_put_decoder_buffer_cmd_ctrl (
  vs_pktexg_cmd_put_decoder_buffer_t* cmd_params
)
{
  uint32_t rc = VS_EOK;
  uint32_t session_handle = 0;
  vs_session_object_t* session_obj = NULL;
  vs_pktexg_buffer_t* mboxBuffer;

  if ( NULL == cmd_params )
  {
    VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return VS_EBADPARAM;
  }

  if ( NULL == cmd_params->buffer )
  {
    VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return VS_EBADPARAM;
  }

  session_handle = cmd_params->handle;
  rc = vs_get_typed_object( session_handle, VS_OBJECT_TYPE_ENUM_SESSION,
                            (vs_object_t**)&session_obj );
  if( NULL == session_obj )
  {
    VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return VS_EHANDLE;
  }

  VS_ACQUIRE_LOCK( session_obj->session_lock );

  mboxBuffer = (vs_pktexg_buffer_t*)cmd_params->buffer;
  rc = vs_voc_action_put_mailbox_dec_request( session_obj );

  VS_RELEASE_LOCK( session_obj->session_lock );

  return rc;
}


VS_INTERNAL uint32_t vs_flush_buffer_cmd_ctrl (
  vs_pktexg_cmd_flush_buffers_t* cmd_params
)
{
  uint32_t rc = VS_EOK;
  uint32_t session_handle = 0;
  vs_session_object_t* session_obj = NULL;

  if ( NULL == cmd_params )
  {
    VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return VS_EBADPARAM;
  }

  session_handle = cmd_params->handle;
  rc = vs_get_typed_object( session_handle, VS_OBJECT_TYPE_ENUM_SESSION,
                            (vs_object_t**)&session_obj );
  if( NULL == session_obj )
  {
    VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return VS_EHANDLE;
  }

  VS_ACQUIRE_LOCK( session_obj->session_lock );

#if UNCOMMENT
  MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
         "VS: Flushing buffers isEncFlushed[%d] isDecFlushed[%d]",
         cmd_params->enc_flush, cmd_params->dec_flush );
#endif

  if( TRUE == cmd_params->dec_flush )
  {
    ( void ) vs_voc_action_flush_dec_mailbox( session_obj );
  }

  if( TRUE == cmd_params->enc_flush )
  {
    ( void ) vs_voc_action_flush_enc_mailbox( session_obj );
  }

  VS_RELEASE_LOCK( session_obj->session_lock );

  return VS_EOK;
}


VS_INTERNAL uint32_t vs_hpcm_push_buffer_cmd_ctrl (
  vs_hpcm_cmd_push_buffer_t* cmd_params
)
{
  uint32_t rc = VS_EOK;
  uint32_t session_handle = 0;
  vs_session_object_t* session_obj = NULL;
  vss_ivpcm_evt_push_buffer_v2_t eventPayload;
  voicemem_cmd_cache_flush_t cmdPayload;
#if (!defined AVS_MPSS_TEST)   
  ipa_dma_req_t req;
#endif  
  voicemem_cmd_get_iova_t params;
  uint64_t host_iova;
  uint64_t device_addr;

  if ( NULL == cmd_params )
  {
    VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return VS_EBADPARAM;
  }

  session_handle = cmd_params->handle;
  rc = vs_get_typed_object( session_handle, VS_OBJECT_TYPE_ENUM_SESSION,
                            (vs_object_t**)&session_obj );
  if( NULL == session_obj )
  {
    VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return VS_EHANDLE;
  }

  VS_ACQUIRE_LOCK( session_obj->session_lock );

  if( VS_HPCM_TAP_POINT_STREAM_RX == cmd_params->tap_point_id )
  {
    eventPayload.tap_point = cmd_params->tap_point_id;
    eventPayload.push_buf_mask = cmd_params->push_mask;
    eventPayload.sampling_rate = session_obj->hpcm.rxSampleRate;
    eventPayload.num_in_channels = 1;

    eventPayload.out_buf_mem_address = session_obj->hpcm.rxReadBlk.phyAddr;
    eventPayload.out_buf_mem_size = VS_HPCM_BUF_SIZE;
    eventPayload.in_buf_mem_address = session_obj->hpcm.rxWriteBlk.phyAddr;
    eventPayload.in_buf_mem_size = session_obj->hpcm.rxWriteBuf.data_size;
    if( vs_platform_is_fusion )
    {
      device_addr = session_obj->hpcm.rxReadBlk.phyAddr;
      params.smem_addr = device_addr;
      params.mem_handle = session_obj->voicemem_handle;
      params.adsp_iova = &host_iova; 
      params.pcie_iova = NULL;
      ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );
      eventPayload.out_buf_mem_address = host_iova;

      device_addr = session_obj->hpcm.rxWriteBlk.phyAddr;
      params.smem_addr = device_addr;
      params.mem_handle = session_obj->voicemem_handle;
      params.adsp_iova = &host_iova; 
      params.pcie_iova = NULL;
      ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );
      eventPayload.in_buf_mem_address = host_iova;
    }

    //inputbuffer is filled
    if( eventPayload.push_buf_mask & VSS_HPCM_MASK_WRITE_BUFFER )
    {
      cmdPayload.voicemem_handle = session_obj->voicemem_handle;
      cmdPayload.virt_addr = (void*)session_obj->hpcm.rxWriteBlk.virtAddr;
      cmdPayload.size = session_obj->hpcm.rxWriteBlk.size;
      ( void ) voicemem_call( VOICEMEM_CMD_CACHE_FLUSH, (void*)&cmdPayload, sizeof( cmdPayload ) );
      if( vs_platform_is_fusion )
      {
        device_addr = session_obj->hpcm.rxWriteBlk.phyAddr;
        params.smem_addr = device_addr;
        params.mem_handle = session_obj->voicemem_handle;
        params.adsp_iova = NULL;
        params.pcie_iova = &host_iova;
        ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );
#if (!defined AVS_MPSS_TEST)
        req.device_addr = session_obj->hpcm.rxWriteBlk.phyAddr;
        req.host_addr = host_iova;
        req.size = session_obj->hpcm.rxWriteBlk.size;
        req.options = 0;
        req.user_data = ( void* )NULL;
        ipa_dma_device_to_host(session_obj->ipa_dl_handle, &req);
#else
        mmstd_memcpy( ( void* )host_iova, session_obj->hpcm.rxWriteBlk.size, 
                        ( void* )session_obj->hpcm.rxWriteBlk.phyAddr, session_obj->hpcm.rxWriteBlk.size );
#endif

     }
    }
  }

  if( VS_HPCM_TAP_POINT_STREAM_TX == cmd_params->tap_point_id )
  {
    eventPayload.tap_point = cmd_params->tap_point_id;
    eventPayload.push_buf_mask = cmd_params->push_mask;
    eventPayload.sampling_rate = session_obj->hpcm.txSampleRate;
    eventPayload.num_in_channels = 1;

    eventPayload.out_buf_mem_address = session_obj->hpcm.txReadBlk.phyAddr;
    eventPayload.out_buf_mem_size = VS_HPCM_BUF_SIZE;
    eventPayload.in_buf_mem_address = session_obj->hpcm.txWriteBlk.phyAddr;
    eventPayload.in_buf_mem_size = session_obj->hpcm.txWriteBuf.data_size;
    if( vs_platform_is_fusion )
    {
      device_addr = session_obj->hpcm.txReadBlk.phyAddr;
      params.smem_addr = device_addr;
      params.mem_handle = session_obj->voicemem_handle;
      params.adsp_iova = &host_iova; 
      params.pcie_iova = NULL;
      ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );
      eventPayload.out_buf_mem_address = host_iova;

      device_addr = session_obj->hpcm.txWriteBlk.phyAddr;
      params.smem_addr = device_addr;
      params.mem_handle = session_obj->voicemem_handle;
      params.adsp_iova = &host_iova; 
      params.pcie_iova = NULL;
      ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );
      eventPayload.in_buf_mem_address = host_iova;
    }

    //inputbuffer is filled
    if( eventPayload.push_buf_mask & VSS_HPCM_MASK_WRITE_BUFFER )
    {
      cmdPayload.voicemem_handle = session_obj->voicemem_handle;
      cmdPayload.virt_addr = (void*)session_obj->hpcm.txWriteBlk.virtAddr;
      cmdPayload.size = session_obj->hpcm.txWriteBlk.size;
      ( void ) voicemem_call( VOICEMEM_CMD_CACHE_FLUSH, (void*)&cmdPayload, sizeof( cmdPayload ) );
      if( vs_platform_is_fusion )
      {
        device_addr = session_obj->hpcm.txWriteBlk.phyAddr;
        params.smem_addr = device_addr;
        params.mem_handle = session_obj->voicemem_handle;
        params.adsp_iova = NULL;
        params.pcie_iova = &host_iova;
        ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );
#if (!defined AVS_MPSS_TEST)
        req.device_addr = device_addr;
        req.host_addr = host_iova;
        req.size = session_obj->hpcm.txWriteBlk.size;
        req.options = 0;
        req.user_data = ( void* )NULL;
        ipa_dma_device_to_host(session_obj->ipa_ul_handle, &req);
#else
        mmstd_memcpy( ( void* )host_iova, session_obj->hpcm.txWriteBlk.size, 
                      ( void* )session_obj->hpcm.txWriteBlk.phyAddr, session_obj->hpcm.txWriteBlk.size );
#endif
      }
    }
  }

  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_EVENT_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_cvs_addr, session_obj->cvs_port,
         0, VSS_IVPCM_EVT_PUSH_BUFFER_V2,
         &eventPayload, sizeof(eventPayload) );
  VS_REPORT_FATAL_ON_ERROR(rc);

  VS_RELEASE_LOCK( session_obj->session_lock );

  return VS_EOK;
}


VS_INTERNAL uint32_t vs_push_oobtty_char_cmd_ctrl (
  vs_oobtty_cmd_push_char_t* cmd_params
)
{
  uint32_t rc = VS_EOK;
  uint32_t session_handle = 0;
  vs_simple_job_object_t* simple_job = NULL;
  vs_session_object_t* session_obj = NULL;
  vss_ittyoob_cmd_rx_char_t cmdPayload;

  if ( NULL == cmd_params )
  {
    VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return VS_EBADPARAM;
  }

  session_handle = cmd_params->handle;
  rc = vs_get_typed_object( session_handle, VS_OBJECT_TYPE_ENUM_SESSION,
                            (vs_object_t**)&session_obj );
  if( NULL == session_obj )
  {
    VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return VS_EHANDLE;
  }

  VS_ACQUIRE_LOCK( session_obj->session_lock );

  //TBD: Send error if stream is not enabled


  rc = vs_create_simple_job_object( session_obj->header.handle, &simple_job );
  simple_job->fn_table[ VS_RESPONSE_FN_ENUM_RESULT ] = vs_voc_track_result_rsp_fn;

  cmdPayload.tty_char = cmd_params->tty_char;
  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_cvs_addr, session_obj->cvs_port,
         simple_job->header.handle, VSS_ITTYOOB_CMD_SEND_RX_CHAR,
         &cmdPayload, sizeof(cmdPayload) );
  if ( rc )
  {
    VS_REPORT_FATAL_ON_ERROR(rc);
    vs_mem_free_object( (vs_object_t*)simple_job );
  }

  VS_RELEASE_LOCK( session_obj->session_lock );

  return rc;
}


VS_INTERNAL uint32_t vs_cmd_mem_use_stat_cmd_ctrl (
  vs_voc_mem_use_stat_t* param
)
{
  param->objects_used = VS_MAX_OBJECTS_V - vs_objmgr.free_q.size - VS_MAX_NUM_SESSIONS_V;

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "VS: MEM_USE_STAT: Objects_used=(%d)", param->objects_used );

  return APR_EOK;
}

