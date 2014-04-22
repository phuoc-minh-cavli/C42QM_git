/**
  @file vs_actions.c
  @brief This file contains the definitions of the internal functions of VS.
*/

/*
  Copyright (C) 2018-2019 QUALCOMM Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.mpss/8.0.c12/vsd/vs/src/vs_actions.c#1 $
  $Author: pwbldsvc $
*/

/****************************************************************************
 * HEADER FILES                                                     *
 ****************************************************************************/
#include "mmstd.h"
#include "voicecfg_api.h"
#include "voicecfg_items.h"
#include "vs_i.h"


/****************************************************************************
 * STATIC DEFINIIONS                                                      *
 ****************************************************************************/
#if ( !defined AVS_MPSS_TEST )
static vstmr_vfr_e vs_voc_get_vfr_id (
 uint32_t vsid
)
{
  vstmr_vfr_e vfr_id = VSTMR_VFR_AUTO;
  uint32_t mux_id = 0;
  
  /* 18 to 21 bit positions in VSID constitute the Mux ID. */
  mux_id = ( vsid >> VS_MUX_ID_MASK_BIT_POS ) & VS_MUX_ID_MASK_VAL;
  
  switch( mux_id )
  {
    case 0:
      vfr_id = VSTMR_VFR_PRI;
      break;

    case 7:
      vfr_id = VSTMR_VFR_SEC;
      break;

    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_get_vfr_id(): Invalid "
           "mux_id=%d", mux_id );
      break;
  }
  return vfr_id;
}
#endif  
static uint32_t vs_voc_get_cvd_network_id (
  uint32_t vs_client_id
)
{
  uint32_t cvd_network_id = VS_PARAM_UINT32_NOT_SET;

  switch ( vs_client_id )
  {
    case VS_COMMON_NETWORK_ID_CDMA:
      cvd_network_id = VSS_ICOMMON_CAL_NETWORK_ID_CDMA;
      break;

    case VS_COMMON_NETWORK_ID_GSM:
      cvd_network_id = VSS_ICOMMON_CAL_NETWORK_ID_GSM;
      break;

    case VS_COMMON_NETWORK_ID_WCDMA:
    case VS_COMMON_NETWORK_ID_TDSCDMA:
      cvd_network_id = VSS_ICOMMON_CAL_NETWORK_ID_WCDMA;
      break;

    case VS_COMMON_NETWORK_ID_LTE:
    case VS_COMMON_NETWORK_ID_WLAN:
      cvd_network_id = VSS_ICOMMON_CAL_NETWORK_ID_LTE;
      break;

    default:
      break;
  }

  return cvd_network_id;
}


/****************************************************************************
 * FUNCTION DEFINIIONS                                                      *
 ****************************************************************************/

uint32_t vs_voc_action_create_mvm (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  vs_simple_job_object_t* job_obj;

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );

  job_obj->fn_table[VS_RESPONSE_FN_ENUM_RESULT] = vs_voc_create_mvm_session_result_rsp_fn;

  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;

#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:VSS_IMVM_CMD_CREATE_FULL_CONTROL_SESSION(): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_mvm_addr, APR_NULL_V,
         job_obj->header.handle, VSS_IMVM_CMD_CREATE_FULL_CONTROL_SESSION,
         session_obj->session_name, sizeof( session_obj->session_name ) );
  if ( rc )
  {
    VS_REPORT_FATAL_ON_ERROR(rc);
    session_obj->session_ctrl.status = rc;
    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );
    return APR_EIMMEDIATE;
  }

  return APR_EOK;
}

uint32_t vs_voc_action_destroy_mvm (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  vs_simple_job_object_t* job_obj = NULL;

  if ( session_obj->mvm_port == APR_NULL_V )
  {
    session_obj->session_ctrl.status = APR_EOK;
    return APR_EIMMEDIATE;
  }

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );

  job_obj->fn_table[VS_RESPONSE_FN_ENUM_RESULT] = vs_voc_transition_result_rsp_fn;

#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:APRV2_IBASIC_CMD_DESTROY_SESSION(MVM): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_mvm_addr, session_obj->mvm_port,
         job_obj->header.handle, APRV2_IBASIC_CMD_DESTROY_SESSION,
         NULL, 0 );
  if ( rc )
  {
    VS_REPORT_FATAL_ON_ERROR(rc);
    session_obj->session_ctrl.status = rc;
    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );
    return APR_EIMMEDIATE;
  }
  return APR_EOK;
}

uint32_t vs_voc_action_request_mailbox_memory_config (
  vs_session_object_t* session_obj
)
{

  int32_t rc = APR_EOK;
  vs_simple_job_object_t* job_obj = NULL;

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );
  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;
  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_RESULT] = vs_voc_transition_result_rsp_fn;
  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_MBOX_MEMORY_CONFIG ] = vs_voc_mailbox_memory_config_rsp_fn;

#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:VSS_IPKTEXG_CMD_REQ_MAILBOX_MEMORY_CONFIG(): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_mvm_addr, session_obj->mvm_port,
         job_obj->header.handle, VSS_IPKTEXG_CMD_REQ_MAILBOX_MEMORY_CONFIG,
         NULL, 0 );
  if ( rc )
  {
    VS_REPORT_FATAL_ON_ERROR(rc);
    session_obj->session_ctrl.status = rc;
    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );
    return APR_EIMMEDIATE;
  }
  return APR_EOK;
}

uint32_t vs_voc_action_create_cvs (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  vs_simple_job_object_t* job_obj = NULL;
  vss_istream_cmd_create_full_control_session_t* cvs_full_ctrl = NULL;
  aprv2_packet_t* p_packet = NULL;
  char_t* p_stream_name = NULL;

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );

  job_obj->fn_table[VS_RESPONSE_FN_ENUM_RESULT] =
     vs_voc_create_cvs_session_result_rsp_fn;

  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;

#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:VSS_ISTREAM_CMD_CREATE_FULL_CONTROL_SESSION(): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  rc = __aprv2_cmd_alloc_ext(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_cvs_addr, APR_NULL_V,
         job_obj->header.handle, VSS_ISTREAM_CMD_CREATE_FULL_CONTROL_SESSION,
         ( sizeof( vss_istream_cmd_create_full_control_session_t ) +
         sizeof( session_obj->session_name ) ), &p_packet );
  VS_REPORT_FATAL_ON_ERROR( rc );

  if( ( p_packet == NULL ) || ( rc ) )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_voc_action_create_cvs(): APR packet alloc failed  with "
           "result=(0X%08X)", rc );
    /* Lock not needed as cmd failed & no response expected */
    session_obj->session_ctrl.status = rc;
    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );

    return APR_EIMMEDIATE;
  }

  cvs_full_ctrl = APRV2_PKT_GET_PAYLOAD(
                    vss_istream_cmd_create_full_control_session_t, p_packet );

  cvs_full_ctrl->direction = 2;
  cvs_full_ctrl->dec_media_type = VSS_MEDIA_ID_NONE;
  cvs_full_ctrl->enc_media_type = VSS_MEDIA_ID_NONE;
  cvs_full_ctrl->network_id = VSS_NETWORK_ID_DEFAULT;

  p_stream_name =
    ( char* ) APR_PTR_END_OF( cvs_full_ctrl,
                sizeof( vss_istream_cmd_create_full_control_session_t ) );

  mmstd_memcpy( p_stream_name, sizeof( session_obj->session_name ),
                session_obj->session_name, sizeof( session_obj->session_name ) );

  rc = __aprv2_cmd_forward( vs_apr_handle, p_packet );
  if ( rc )
  {
    VS_REPORT_FATAL_ON_ERROR( rc );
    session_obj->session_ctrl.status = rc;
    ( void ) __aprv2_cmd_free( vs_apr_handle, p_packet );
    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );
    return APR_EIMMEDIATE;
  }
  
  return APR_EOK;
}

uint32_t vs_voc_action_destroy_cvs (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  vs_simple_job_object_t* job_obj = NULL;

  if ( session_obj->cvs_port == APR_NULL_V )
  {
    session_obj->session_ctrl.status = VS_EOK;
    return APR_EIMMEDIATE;
  }

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );

  job_obj->fn_table[VS_RESPONSE_FN_ENUM_RESULT] = vs_voc_transition_result_rsp_fn;

#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:APRV2_IBASIC_CMD_DESTROY_SESSION(CVS): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_cvs_addr, session_obj->cvs_port,
         job_obj->header.handle, APRV2_IBASIC_CMD_DESTROY_SESSION,
         NULL, 0 );
  if ( rc )
  {
    VS_REPORT_FATAL_ON_ERROR(rc);
    session_obj->session_ctrl.status = rc;
    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );
    return APR_EIMMEDIATE;
  }

  return APR_EOK;
}

uint32_t vs_voc_action_attach_stream (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  vs_simple_job_object_t* job_obj = NULL;

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );
  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_RESULT ] = vs_voc_transition_result_rsp_fn;
  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;

#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:VSS_IMVM_CMD_ATTACH_STREAM(): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( ( uint16_t ) session_obj->header.handle ),
         vs_mvm_addr, session_obj->mvm_port,
         job_obj->header.handle, VSS_IMVM_CMD_ATTACH_STREAM,
         &session_obj->cvs_port, sizeof( session_obj->cvs_port ) );
  if ( rc )
  {
    VS_REPORT_FATAL_ON_ERROR(rc);
    session_obj->session_ctrl.status = rc;
    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );
    return APR_EIMMEDIATE;
  }

  return APR_EOK;
}

uint32_t vs_voc_action_detach_stream (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  vs_simple_job_object_t* job_obj = NULL;

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );
  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_RESULT ] = vs_voc_transition_result_rsp_fn;
  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;

#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:VSS_IMVM_CMD_DETACH_STREAM(): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( ( uint16_t ) session_obj->header.handle ),
         vs_mvm_addr, session_obj->mvm_port,
         job_obj->header.handle, VSS_IMVM_CMD_DETACH_STREAM,
         &session_obj->cvs_port, sizeof( session_obj->cvs_port ) );
  if ( rc )
  {
    VS_REPORT_FATAL_ON_ERROR(rc);
    session_obj->session_ctrl.status = rc;
    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );
    return APR_EIMMEDIATE;
  }

  return APR_EOK;
}


uint32_t vs_voc_action_listen_eamr (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  int32_t _rc = APR_EOK;
  vs_token_object_t* p_token_obj = NULL;
  vss_inotify_cmd_listen_for_event_class_t ctrl_args;
  voicecfg_cmd_get_bool_item_t item;

  item.id = VOICECFG_CFG_IS_EAMR_REPORT_MODE_CHANGE_ENABLED;
  item.ret_value = FALSE;
  _rc = voicecfg_call( VOICECFG_CMD_GET_BOOL_ITEM, &item, sizeof ( item ) );

  if ( ( !_rc ) && ( item.ret_value == TRUE ) )
  {
    rc = vs_create_autofree_token_object( &p_token_obj );

#if defined (VS_PROFILING)
    vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "VS_PROFILING:VSS_INOTIFY_CMD_LISTEN_FOR_EVENT_CLASS(): vs_cmd_ticks_in_us=(%ld)",
           vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

    ctrl_args.class_id = VSS_ISTREAM_EVENT_CLASS_EAMR_MODE_CHANGE;
    rc = __aprv2_cmd_alloc_send(
           vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
           vs_my_addr, ( ( uint16_t ) session_obj->header.handle ),
           vs_cvs_addr, session_obj->cvs_port,
           p_token_obj->header.handle, VSS_INOTIFY_CMD_LISTEN_FOR_EVENT_CLASS,
           &ctrl_args, sizeof( ctrl_args ) );
    if (rc)
    {
      VS_REPORT_FATAL_ON_ERROR(rc);
      session_obj->session_ctrl.status = rc;
      ( void ) vs_mem_free_object( ( vs_object_t* ) p_token_obj );
      return APR_EIMMEDIATE;
    }
  }

  session_obj->session_ctrl.status = APR_EOK;
  return APR_EIMMEDIATE;
}


uint32_t vs_voc_action_cancel_eamr (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  int32_t _rc = APR_EOK;
  vs_token_object_t* p_token_obj = NULL;
  vss_inotify_cmd_cancel_event_class_t ctrl_args;
  voicecfg_cmd_get_bool_item_t item;

  item.id = VOICECFG_CFG_IS_EAMR_REPORT_MODE_CHANGE_ENABLED;
  item.ret_value = FALSE;
  _rc = voicecfg_call( VOICECFG_CMD_GET_BOOL_ITEM, &item, sizeof ( item ) );

  if ( ( !_rc ) && ( item.ret_value == TRUE ) )
  {
    rc = vs_create_autofree_token_object( &p_token_obj );

    ctrl_args.class_id = VSS_ISTREAM_EVENT_CLASS_EAMR_MODE_CHANGE;

#if defined (VS_PROFILING)
    vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "VS_PROFILING:VSS_INOTIFY_CMD_CANCEL_EVENT_CLASS(): vs_cmd_ticks_in_us=(%ld)",
           vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

    rc = __aprv2_cmd_alloc_send(
           vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
           vs_my_addr, ( ( uint16_t ) session_obj->header.handle ),
           vs_cvs_addr, session_obj->cvs_port,
           p_token_obj->header.handle, VSS_INOTIFY_CMD_CANCEL_EVENT_CLASS,
           &ctrl_args, sizeof( ctrl_args ) );
    if (rc)
    {
      VS_REPORT_FATAL_ON_ERROR(rc);
      session_obj->session_ctrl.status = rc;
      ( void ) vs_mem_free_object( ( vs_object_t* ) p_token_obj );
      return APR_EIMMEDIATE;
    }
  }

  session_obj->session_ctrl.status = APR_EOK;
  return APR_EIMMEDIATE;
}


uint32_t vs_voc_action_set_media_type_on_stream (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  vs_simple_job_object_t* job_obj;
  vss_istream_cmd_set_media_type_t set_media_type;

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );
  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_RESULT ] = vs_voc_transition_result_rsp_fn;
  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;

  set_media_type.rx_media_id = session_obj->vocoder.media_id;
  set_media_type.tx_media_id = set_media_type.rx_media_id;

#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:VSS_ISTREAM_CMD_SET_MEDIA_TYPE(): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_cvs_addr, session_obj->cvs_port,
         job_obj->header.handle, VSS_ISTREAM_CMD_SET_MEDIA_TYPE,
         &set_media_type, sizeof( set_media_type) );
  if ( rc )
  {
    VS_REPORT_FATAL_ON_ERROR(rc);
    session_obj->session_ctrl.status = rc;
    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );
    return APR_EIMMEDIATE;
  }

  return APR_EOK;
}


/**
 * Set network on MVM to be used for calibrating the attached
 * stream/vocproc.
 */
uint32_t vs_voc_action_set_network_type_on_mvm (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  vs_simple_job_object_t* job_obj = NULL;
  vss_imvm_cmd_set_cal_network_t set_network;

  set_network.network_id = vs_voc_get_cvd_network_id( session_obj->vocoder.client_id );
  if( VS_PARAM_UINT32_NOT_SET == set_network.network_id )
  {
    VS_REPORT_FATAL_ON_ERROR( VS_EBADPARAM );
    session_obj->session_ctrl.status = VS_EBADPARAM;
    return APR_EIMMEDIATE;
  }

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );
  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_RESULT ] = vs_voc_transition_result_rsp_fn;
  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;

#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:VSS_IMVM_CMD_SET_CAL_NETWORK(): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_mvm_addr, session_obj->mvm_port,
         job_obj->header.handle, VSS_IMVM_CMD_SET_CAL_NETWORK,
         &set_network, sizeof( set_network ) );
  if ( rc )
  {
    VS_REPORT_FATAL_ON_ERROR(rc);
    session_obj->session_ctrl.status = rc;
    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );
    return APR_EIMMEDIATE;
  }

  return rc;
}


uint32_t vs_voc_action_set_media_type_on_mvm (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  vs_simple_job_object_t* job_obj = NULL;
  vss_imvm_cmd_set_cal_media_type_t set_media_type;

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );
  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_RESULT ] = vs_voc_transition_result_rsp_fn;
  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;

  set_media_type.media_id = session_obj->vocoder.media_id;
  
#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:VSS_IMVM_CMD_SET_CAL_MEDIA_TYPE(): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_mvm_addr, session_obj->mvm_port,
         job_obj->header.handle, VSS_IMVM_CMD_SET_CAL_MEDIA_TYPE,
         &set_media_type, sizeof( set_media_type ) );
  if ( rc )
  {
    VS_REPORT_FATAL_ON_ERROR(rc);
    session_obj->session_ctrl.status = rc;
    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );
    return APR_EIMMEDIATE;
  }

  return rc;
}


uint32_t vs_voc_action_set_voice_timing_params (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  vs_simple_job_object_t* job_obj = NULL;
  vss_icommon_cmd_set_voice_timing_v2_t timing_param_v2;

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );
  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_RESULT ] = vs_voc_transition_result_rsp_fn;
  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;

  timing_param_v2.mode = session_obj->vocoder.vfr_config.vfr_mode;
  timing_param_v2.vsid = session_obj->vsid;
  timing_param_v2.enc_offset = session_obj->vocoder.timing_config.enc_offset_us;
  timing_param_v2.dec_req_offset = session_obj->vocoder.timing_config.dec_req_offset_us;
  timing_param_v2.dec_offset = session_obj->vocoder.timing_config.dec_offset_us;

  MSG_6( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "VS: TIMING_OFFSETS: handle[0x%08x], vsid[0x%08x], vfr_mode[%d], enc_off[%d], "
         "dec_req_off[%d], dec_off[%d]", session_obj->header.handle, timing_param_v2.vsid, 
         timing_param_v2.mode, timing_param_v2.enc_offset, timing_param_v2.dec_req_offset,
         timing_param_v2.dec_offset );

#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:VSS_ICOMMON_CMD_SET_VOICE_TIMING_V2(): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_mvm_addr, session_obj->mvm_port,
         job_obj->header.handle, VSS_ICOMMON_CMD_SET_VOICE_TIMING_V2,
         &timing_param_v2, sizeof( timing_param_v2 ) );
  if ( rc )
  {
    VS_REPORT_FATAL_ON_ERROR(rc);
    session_obj->session_ctrl.status = rc;
    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );
    return APR_EIMMEDIATE;
  }

  return rc;
}

uint32_t vs_voc_action_calculate_timing_offsets (
  vs_session_object_t* session_obj
)
{
  uint32_t enc_offset, dec_req_offset, dec_offset;
  uint32_t vfr_offset, client_tx_offset, client_rx_offset;
  uint32_t vfr_cycle_duration_us;

  vfr_cycle_duration_us = session_obj->vocoder.vfr_config.vfr_cycle_duration_ms * 1000;
#if ( defined AVS_MPSS_TEST )
  session_obj->vocoder.timing_config.vfr_start_timestamp_us = session_obj->vocoder.timing_config.tx_ref_timestamp_us + 3000;
#endif  
  vfr_offset = session_obj->vocoder.timing_config.vfr_start_timestamp_us % vfr_cycle_duration_us;
  client_tx_offset = session_obj->vocoder.timing_config.tx_ref_timestamp_us % vfr_cycle_duration_us;
  client_rx_offset = session_obj->vocoder.timing_config.rx_ref_timestamp_us % vfr_cycle_duration_us;

  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,         
         "VS: TIMING_OFFSETS: vfr_start_timestamp_us:(0x%lu) tx_ref_timestamp_us:(0x%lu) rx_ref_timestamp_us:(0x%lu)",
           session_obj->vocoder.timing_config.vfr_start_timestamp_us,
           session_obj->vocoder.timing_config.tx_ref_timestamp_us,
           session_obj->vocoder.timing_config.rx_ref_timestamp_us );

  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,         
         "VS: TIMING_OFFSETS: vfr_offset:(%d) client_tx_offset:(%d) client_rx_offset:(%d)",
           vfr_offset, client_tx_offset, client_rx_offset );

  { /* Encoder offset calculation. */
    enc_offset = ( vfr_cycle_duration_us + client_tx_offset - vfr_offset ) %
                    vfr_cycle_duration_us;

    /* Adding margin to encoder offset. */
    enc_offset = ( enc_offset + vfr_cycle_duration_us - VS_VOC_TIMING_ENC_OFFSET_MARGIN_V ) %
                    vfr_cycle_duration_us;

  /**
   * Encoder offset should not be less than MIN timing offset value.
   */
    enc_offset = ( enc_offset >= VS_VOC_MIN_TIMING_OFFSET_V )? enc_offset : ( vfr_cycle_duration_us - VS_VOC_MIN_TIMING_OFFSET_V);
  }

  { /* Decoder request offset calculation. */
    dec_req_offset = ( vfr_cycle_duration_us + client_rx_offset - vfr_offset ) %
                       vfr_cycle_duration_us;
  /**
   * Decoder request offset should not be less than MIN timing offset value.
   */
    dec_req_offset = ( dec_req_offset >= VS_VOC_MIN_TIMING_OFFSET_V )? dec_req_offset : (dec_req_offset + VS_VOC_MIN_TIMING_OFFSET_V);
  }

  { /* Decoder offset calculation. */
    dec_offset = ( dec_req_offset + VS_VOC_TIMING_DEC_OFFSET_MARGIN_V ) % vfr_cycle_duration_us;
    
  /**
   * Decoder offset should not be less than MIN timing offset value.
   */
    dec_offset = ( dec_offset >= VS_VOC_MIN_TIMING_OFFSET_V )? dec_offset : (dec_offset + VS_VOC_MIN_TIMING_OFFSET_V);
  }

  /* Populate the evaluated timing params. */
  session_obj->vocoder.timing_config.enc_offset_us = enc_offset;
  session_obj->vocoder.timing_config.dec_req_offset_us = dec_req_offset;
  session_obj->vocoder.timing_config.dec_offset_us = dec_offset;

  MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,         
         "VS: TIMING_OFFSETS: enc_offset:(%d) dec_req_offset:(%d) dec_offset:(%d)",
         enc_offset, dec_req_offset, dec_offset );

  return APR_EOK;
}

uint32_t vs_voc_action_set_timing_references (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  vs_simple_job_object_t* job_obj = NULL;
  vss_icommon_cmd_set_voice_timing_reference_t voiceTimingReference;

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );
  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_RESULT ] = vs_voc_transition_result_rsp_fn;
  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;

  /* Offset timing references w.r.t to avtimer. */
  voiceTimingReference.tx_ref_timstamp_us = 
    session_obj->vocoder.timing_config.tx_ref_timestamp_us +
    session_obj->vocoder.timing_config.avtimerOffsetUs;

  voiceTimingReference.rx_ref_timstamp_us = 
   session_obj->vocoder.timing_config.rx_ref_timestamp_us +
   session_obj->vocoder.timing_config.avtimerOffsetUs;

#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:VSS_ICOMMON_CMD_SET_VOICE_TIMING_REFERENCE(): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_mvm_addr, session_obj->mvm_port,
         job_obj->header.handle, VSS_ICOMMON_CMD_SET_VOICE_TIMING_REFERENCE,
         &voiceTimingReference, sizeof( voiceTimingReference ) );
  if ( rc )
  {
    VS_REPORT_FATAL_ON_ERROR(rc);
    session_obj->session_ctrl.status = rc;
    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );
    return APR_EIMMEDIATE;
  }

  return rc;
}


uint32_t vs_voc_action_set_vfr_config (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  vs_simple_job_object_t* job_obj = NULL;
  vss_icommon_cmd_set_vfr_config_t vfrConfig;

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );
  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_RESULT ] = vs_voc_transition_result_rsp_fn;
  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;

  vfrConfig.vfr_mode = session_obj->vocoder.vfr_config.vfr_mode;
  vfrConfig.vfr_cycle_duration_ms = session_obj->vocoder.vfr_config.vfr_cycle_duration_ms;
  vfrConfig.vsid = session_obj->vsid;

  MSG_4( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "VS: VFR_CONFIG: handle[0x%08x] vsid[0x%08x] vfr_mode[%d] vfr_cycle_duration_ms[%d] ",
         session_obj->header.handle, vfrConfig.vsid, vfrConfig.vfr_mode,
         vfrConfig.vfr_cycle_duration_ms );

#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:VSS_ICOMMON_CMD_SET_VFR_CONFIG(): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_mvm_addr, session_obj->mvm_port,
         job_obj->header.handle, VSS_ICOMMON_CMD_SET_VFR_CONFIG,
         &vfrConfig, sizeof( vfrConfig ) );
  if ( rc )
  {
    VS_REPORT_FATAL_ON_ERROR(rc);
    session_obj->session_ctrl.status = rc;
    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );
    return APR_EIMMEDIATE;
  }

  return rc;
}


uint32_t vs_voc_action_send_resync_ctm (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  vs_simple_job_object_t* job_obj = NULL;

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );
  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_RESULT ] = vs_voc_transition_result_rsp_fn;
  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;

#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:VSS_ISTREAM_CMD_RESYNC_CTM(): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_cvs_addr, session_obj->cvs_port,
         job_obj->header.handle, VSS_ISTREAM_CMD_RESYNC_CTM,
         NULL, 0 );
  if ( rc )
  {
    VS_REPORT_FATAL_ON_ERROR(rc);
    session_obj->session_ctrl.status = rc;
    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );
    return APR_EIMMEDIATE;
  }

  return rc;
}

uint32_t vs_voc_action_configure_vfr (
  vs_session_object_t* session_obj
)
{
#if ( !defined AVS_MPSS_TEST )
  vstmr_vfr_e vfr_id = VSTMR_VFR_AUTO;
  vstmr_vfr_e vfr_id_allocated = VSTMR_VFR_AUTO;
  uint64_t vfr_xo_cnt = 0;
#endif
  
  if ( ( VS_COMMON_NETWORK_ID_LTE != session_obj->vocoder.client_id ) ||
       ( VS_VOC_VFR_MODE_HARD_V != session_obj->vocoder.vfr_config.vfr_mode ) )
  {
    return APR_EIMMEDIATE;
  }
#if ( !defined AVS_MPSS_TEST )
  vfr_id = vs_voc_get_vfr_id( session_obj->vsid );
  vfr_id_allocated = vstmr_xo_vfr_allocate (  vfr_id );
  if ( vfr_id != vfr_id_allocated )
  {
    session_obj->session_ctrl.status = APR_EFAILED;
    return APR_EFAILED;
  }
  
  /* Reading the current XO TS and adding margin to derive the VFR Start TS. */
  session_obj->vocoder.timing_config.vfr_start_timestamp_us = VSTMR_XO_TO_US( VSTMR_XO_READ_FULL() ) + VS_VOC_TIMING_VFR_START_MARGIN_V;
  vfr_xo_cnt = VSTMR_US_TO_XO (session_obj->vocoder.timing_config.vfr_start_timestamp_us );
  vstmr_xo_vfr_start( vfr_id, vfr_xo_cnt,
                      VSTMR_MS_TO_XO( session_obj->vocoder.vfr_config.vfr_cycle_duration_ms ) );
#endif
  return APR_EIMMEDIATE;
}

uint32_t vs_voc_action_release_vfr (
  vs_session_object_t* session_obj
)
{
#if ( !defined AVS_MPSS_TEST )
  vstmr_vfr_e vfr_id = VSTMR_VFR_AUTO;
#endif

  if ( ( VS_COMMON_NETWORK_ID_LTE != session_obj->vocoder.client_id ) ||
       ( VS_VOC_VFR_MODE_HARD_V != session_obj->vocoder.vfr_config.vfr_mode ) )
  {
    return APR_EIMMEDIATE;
  }
#if ( !defined AVS_MPSS_TEST )
  vfr_id = vs_voc_get_vfr_id( session_obj->vsid );  
  vstmr_xo_vfr_release( vfr_id ); 
#endif  
  return APR_EIMMEDIATE;
}

uint32_t vs_voc_action_send_start_voice (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  vs_simple_job_object_t* job_obj = NULL;

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );
  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_RESULT ] = vs_voc_transition_result_rsp_fn;
  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;

#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:VSS_IMVM_CMD_MODEM_START_VOICE(): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_mvm_addr, session_obj->mvm_port,
         job_obj->header.handle, VSS_IMVM_CMD_MODEM_START_VOICE,
         NULL, 0 );
  if ( rc )
  {
    VS_REPORT_FATAL_ON_ERROR(rc);
    session_obj->session_ctrl.status = rc;
    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );
    return APR_EIMMEDIATE;
  }

  return APR_EOK;
}

uint32_t vs_voc_action_send_stop_voice (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  vs_simple_job_object_t* job_obj;

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );
  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_RESULT ] = vs_voc_transition_result_rsp_fn;
  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;

#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:VSS_IMVM_CMD_MODEM_STOP_VOICE(): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_mvm_addr, session_obj->mvm_port,
         job_obj->header.handle, VSS_IMVM_CMD_MODEM_STOP_VOICE,
         NULL, 0 );
  if ( rc )
  {
    VS_REPORT_FATAL_ON_ERROR(rc);
    session_obj->session_ctrl.status = rc;
    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );
    return APR_EIMMEDIATE;
  }

  return APR_EOK;
}

uint32_t vs_voc_action_set_plc ( 
  vs_session_object_t* session_obj
)
{
  uint32_t rc = VS_EOK;
  aprv2_packet_t* packet = NULL;
  vss_istream_cmd_set_voc_param_t* payload = NULL;
  vss_istream_voc_param_data_t* param = NULL;
  uint32_t* paramData = NULL;

  if( VS_PARAM_UINT8_NOT_SET == session_obj->vocoder.plc ) return VS_EOK;

#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:VSS_ISTREAM_CMD_SET_VOC_PARAM(PLC): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  rc = __aprv2_cmd_alloc_ext(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_cvs_addr, session_obj->cvs_port,
         session_obj->header.handle, VSS_ISTREAM_CMD_SET_VOC_PARAM,
         sizeof(vss_istream_cmd_set_voc_param_t) +
         sizeof(vss_istream_voc_param_data_t) + sizeof(vss_istream_param_plc_t), &packet );
  if( packet == NULL )
  {
    VS_REPORT_FATAL_ON_ERROR( rc );
    return VS_EFAILED;
  }

  payload = APRV2_PKT_GET_PAYLOAD( vss_istream_cmd_set_voc_param_t, packet );
  payload->mem_handle = 0;
  payload->mem_address = 0;
  payload->mem_size = sizeof(vss_istream_voc_param_data_t) + sizeof(uint32_t);

  param = (vss_istream_voc_param_data_t*)( (uint8_t*)payload + sizeof(vss_istream_cmd_set_voc_param_t) );
  param->media_id = session_obj->vocoder.media_id;
  param->param_id = VSS_ISTREAM_PARAM_PLC;
  param->direction_id = VSS_ISTREAM_DIRECTION_RX;
  param->param_size = sizeof(uint32_t);

  paramData = (uint32_t*)( (uint8_t*)param + sizeof(vss_istream_voc_param_data_t) );
  *paramData = (uint32_t) session_obj->vocoder.plc;

  rc = __aprv2_cmd_forward( vs_apr_handle, packet );
  if (rc)
  {
    VS_REPORT_FATAL_ON_ERROR(rc);
    ( void ) __aprv2_cmd_free( vs_apr_handle, packet );
  }

  return VS_EOK;
}

uint32_t vs_voc_action_set_dtx ( 
  vs_session_object_t* session_obj
)
{
  uint32_t rc =VS_EOK;
  vss_istream_cmd_set_enc_dtx_mode_t dtx;

  if( VS_PARAM_UINT32_NOT_SET == session_obj->vocoder.dtx )
  {
    return VS_EOK;
  }

  if( VS_CLIENT_STATE_ENABLED == session_obj->hpcm.client_state )
  {
    dtx.enable = FALSE;
  }
  else
  {
    dtx.enable = session_obj->vocoder.dtx;
  }

  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED, "VS: SET DTX[%d] ", dtx.enable );

#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:VSS_ISTREAM_CMD_SET_ENC_DTX_MODE(DTX): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_cvs_addr, session_obj->cvs_port,
         session_obj->header.handle, VSS_ISTREAM_CMD_SET_ENC_DTX_MODE,
         &dtx, sizeof( dtx ) );
  VS_REPORT_FATAL_ON_ERROR(rc);

  return VS_EOK;
}

uint32_t vs_voc_action_set_dtx_v2 ( 
  vs_session_object_t* session_obj
)
{
  uint32_t rc = VS_EOK;
  aprv2_packet_t* packet = NULL;
  vss_istream_cmd_set_voc_param_t* payload = NULL;
  vss_istream_voc_param_data_t* param = NULL;
  uint32_t param_size = sizeof(vss_istream_param_dtx_t);
  char_t cvd_version[] = "2.4";

  if( VSS_MEDIA_ID_EVS == session_obj->vocoder.media_id)
  {
    param_size = sizeof(vss_istream_param_dtx_v2_t);
  }

  if( VS_PARAM_UINT32_NOT_SET == session_obj->vocoder.dtx ) return VS_EOK;

  if( mmstd_strncmp(session_obj->cvd_version, sizeof(session_obj->cvd_version), 
                     cvd_version, sizeof(cvd_version)) >=0 )
  {
    rc = __aprv2_cmd_alloc_ext(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_cvs_addr, session_obj->cvs_port,
         session_obj->header.handle, VSS_ISTREAM_CMD_SET_VOC_PARAM,
         sizeof(vss_istream_cmd_set_voc_param_t) + 
         sizeof(vss_istream_voc_param_data_t) + param_size, &packet );

    if( packet == NULL )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      return VS_EFAILED;
    }

    payload = APRV2_PKT_GET_PAYLOAD( vss_istream_cmd_set_voc_param_t, packet );
    payload->mem_handle = 0;
    payload->mem_address = 0;
    payload->mem_size = sizeof(vss_istream_voc_param_data_t) + param_size;

    param = (vss_istream_voc_param_data_t*)( (uint8_t*)payload + sizeof(vss_istream_cmd_set_voc_param_t) );
    param->media_id = session_obj->vocoder.media_id;
    param->direction_id = VSS_ISTREAM_DIRECTION_TX;
    param->param_size = param_size;

    if( VSS_MEDIA_ID_EVS == session_obj->vocoder.media_id )
    {
      vss_istream_param_dtx_v2_t* EvsparamData = (vss_istream_param_dtx_v2_t*)( (uint8_t*)param + sizeof(vss_istream_voc_param_data_t) );
      param->param_id = VSS_ISTREAM_PARAM_DTX_V2;
      EvsparamData->version = session_obj->vocoder.dtx_param_version;
      EvsparamData->dtx_mode = session_obj->vocoder.dtx;

    }
    else
    {
      uint32_t* paramData = (uint32_t*)( (uint8_t*)param + sizeof(vss_istream_voc_param_data_t) );
      param->param_id = VSS_ISTREAM_PARAM_DTX;
      *paramData = session_obj->vocoder.dtx;
    }
  
    rc = __aprv2_cmd_forward( vs_apr_handle, packet );
  }
  else
  {
    ( void ) vs_voc_action_set_dtx( session_obj );
  }

  if (rc)
  {
    VS_REPORT_FATAL_ON_ERROR(rc);
    ( void ) __aprv2_cmd_free( vs_apr_handle, packet );
  }

  return VS_EOK;
}

uint32_t vs_voc_action_set_min_max_rate ( 
  vs_session_object_t* session_obj
)
{
  uint32_t rc = VS_EOK;
  vss_istream_cmd_cdma_set_enc_minmax_rate_t minmax_rate;

  if( ( VS_PARAM_UINT8_NOT_SET == session_obj->vocoder.min_rate ) ||
      ( VS_PARAM_UINT8_NOT_APPLICABLE == session_obj->vocoder.min_rate ) )
  {
    return VS_EOK;
  }

  minmax_rate.min_rate = session_obj->vocoder.min_rate;
  minmax_rate.max_rate = session_obj->vocoder.max_rate;

#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:VSS_ISTREAM_CMD_CDMA_SET_ENC_MINMAX_RATE(): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_cvs_addr, session_obj->cvs_port,
         session_obj->header.handle, VSS_ISTREAM_CMD_CDMA_SET_ENC_MINMAX_RATE,
         &minmax_rate, sizeof( minmax_rate ) );
  VS_REPORT_FATAL_ON_ERROR(rc);

  return VS_EOK;
}


uint32_t vs_voc_action_set_max_sample_rate ( 
   vs_session_object_t* session_obj
)
{
  uint32_t rc = VS_EOK;
  vs_simple_job_object_t* job_obj;
  vss_imvm_cmd_set_max_var_voc_sampling_rate_t maxSampleRate;

  if ( VS_VOCEVS_MEDIA_ID != session_obj->vocoder.media_id )
  {
    session_obj->session_ctrl.status = VS_EOK;
    return VS_EIMMEDIATE;
  }

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );
  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_RESULT ] = vs_voc_transition_result_rsp_fn;
  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;

  maxSampleRate.tx = session_obj->vocoder.tx_sample_rate;
  maxSampleRate.rx = session_obj->vocoder.rx_sample_rate;

#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:VSS_IMVM_CMD_SET_MAX_VAR_VOC_SAMPLING_RATE(): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  rc = __aprv2_cmd_alloc_send(
     vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
     vs_my_addr, ( (uint16_t) session_obj->header.handle ),
     vs_mvm_addr, session_obj->mvm_port,
     job_obj->header.handle, VSS_IMVM_CMD_SET_MAX_VAR_VOC_SAMPLING_RATE,
     &maxSampleRate, sizeof( maxSampleRate ) );
  if (rc)
  {
    VS_REPORT_FATAL_ON_ERROR(rc);
    session_obj->session_ctrl.status = rc;
    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );
    rc = APR_EIMMEDIATE;
  }

  return rc;
}

uint32_t vs_voc_action_register_oobtty ( 
   vs_session_object_t* session_obj
)
{
  uint32_t rc = VS_EOK;
  vs_simple_job_object_t* job_obj;

  if ( FALSE == session_obj->vocoder.tty_enable )
  {
    session_obj->session_ctrl.status = VS_EOK;
    return VS_EIMMEDIATE;
  }

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );
  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_RESULT ] = vs_voc_transition_result_rsp_fn;
  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;

#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:VSS_ITTYOOB_CMD_REGISTER(): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  rc = __aprv2_cmd_alloc_send(
     vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
     vs_my_addr, ( (uint16_t) session_obj->header.handle ),
     vs_cvs_addr, session_obj->cvs_port,
     job_obj->header.handle, VSS_ITTYOOB_CMD_REGISTER,
     NULL, 0 );
  if (rc)
  {
    VS_REPORT_FATAL_ON_ERROR(rc);
    session_obj->session_ctrl.status = rc;
    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );
    rc = APR_EIMMEDIATE;
  }

  return rc;
}


uint32_t vs_voc_action_deregister_oobtty ( 
   vs_session_object_t* session_obj
)
{
  uint32_t rc = VS_EOK;
  vs_simple_job_object_t* job_obj;

  if ( FALSE == session_obj->vocoder.tty_enable )
  {
    session_obj->session_ctrl.status = VS_EOK;
    return VS_EIMMEDIATE;
  }

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );
  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_RESULT ] = vs_voc_transition_result_rsp_fn;
  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;

#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:VSS_ITTYOOB_CMD_DEREGISTER(): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_cvs_addr, session_obj->cvs_port,
         job_obj->header.handle, VSS_ITTYOOB_CMD_DEREGISTER,
         NULL, 0 );
  if (rc)
  {
    VS_REPORT_FATAL_ON_ERROR(rc);
    session_obj->session_ctrl.status = rc;
    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );
    rc = APR_EIMMEDIATE;
  }

  return rc;
}


uint32_t vs_voc_action_map_memory (
  vs_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_simple_job_object_t* job_obj;
  uint32_t opcode;
  uint64_t host_iova;
#if (!defined AVS_MPSS_TEST)
  voicemem_cmd_cache_flush_t cmdPayload;
  vss_imemory_cmd_map_physical_t memMapCmd;
  vss_imemory_table_descriptor_t* memTable = NULL;
  ipa_dma_req_t req;
#else
  vss_imemory_cmd_map_virtual_t memMapCmd;
#endif
  voicemem_cmd_get_iova_t params;

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );
  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;
  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_RESULT] = vs_voc_transition_result_rsp_fn;
  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_MEMORY_MAP ] = vs_voc_memory_map_result_rsp_fn;

#if (!defined AVS_MPSS_TEST)
  {
    opcode = VSS_IMEMORY_CMD_MAP_PHYSICAL;

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "VS: memory mapping memTablePhyAddr: MSW[0x%08X] LSW[0x%08X] memTableSize[0x%08x]",
           (uint32_t)(vs_shared_mem_table_phy_addr_start >> 32),
           (uint32_t)vs_shared_mem_table_phy_addr_start, vs_shared_mem_table_size );

    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "VS: memory mapping memTableVirtAddr: MSW[0x%08X]LSW[0x%08X]",
           (uint32_t)(vs_shared_mem_table_virt_addr_start >> 32), (uint32_t)vs_shared_mem_table_virt_addr_start );

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "VS: memory mapping memBlockPhyAddr: MSW[0x%08X] LSW[0x%08X] memBlockSize[0x%08x]",
           (uint32_t)(session_obj->mem_phy_addr_start >> 32),
           (uint32_t)session_obj->mem_phy_addr_start, session_obj->mem_size );

    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "VS: memory mapping memBlockVirtAddr: MSW[0x%08X]LSW[0x%08X]",
           (uint32_t)(session_obj->mem_virt_addr_start >> 32),
           (uint32_t)session_obj->mem_virt_addr_start );

    //Updating the mem page table as there are is no subsequent mem tables.
    memTable = (vss_imemory_table_descriptor_t*)vs_shared_mem_table_virt_addr_start;
    memTable->mem_address = 0x0;
    memTable->mem_size = 0;
  
    //Updating the mem page table with subsequent memory block information.
    memTable = memTable + 1; //move ahead block location
    if ( TRUE == vs_platform_is_fusion )
    {
      params.mem_handle = session_obj->voicemem_handle;
      params.smem_addr = session_obj->mem_phy_addr_start;
      params.adsp_iova = &host_iova;
      params.pcie_iova = NULL;
      ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );
      memTable->mem_address = host_iova;
    }
    else
    {
      memTable->mem_address = session_obj->mem_phy_addr_start;
    }
    memTable->mem_size = session_obj->mem_size;
  
    cmdPayload.voicemem_handle = vs_shared_mem_table_handle;
    cmdPayload.virt_addr = (void*)vs_shared_mem_table_virt_addr_start;
    cmdPayload.size = vs_shared_mem_table_size;
    ( void ) voicemem_call( VOICEMEM_CMD_CACHE_FLUSH, &cmdPayload, sizeof( cmdPayload ) );
    if ( TRUE == vs_platform_is_fusion )
    {
      params.mem_handle = vs_shared_mem_table_handle;
      params.smem_addr = vs_shared_mem_table_phy_addr_start;
      params.adsp_iova = &host_iova;
      params.pcie_iova = NULL;
      ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );
      memMapCmd.table_descriptor.mem_address = host_iova;
    }
    else
    {
      memMapCmd.table_descriptor.mem_address = vs_shared_mem_table_phy_addr_start;
    }
    memMapCmd.table_descriptor.mem_size = sizeof(vss_imemory_table_descriptor_t) + sizeof(vss_imemory_block_t);
    memMapCmd.is_cached = TRUE;
    memMapCmd.cache_line_size = VS_CACHE_LINE_SIZE;
    memMapCmd.access_mask = 0x3;
    memMapCmd.page_align = VS_PAGE_ALIGN_SIZE;
    memMapCmd.min_data_width = VS_MIN_DATA_WIDTH;
    memMapCmd.max_data_width = VS_MAX_DATA_WIDTH;
  }
#else
  {
    opcode = VSS_IMEMORY_CMD_MAP_VIRTUAL;
    if ( TRUE == vs_platform_is_fusion )
    {
      params.mem_handle = session_obj->voicemem_handle;
      params.smem_addr = session_obj->mem_virt_addr_start;
      params.adsp_iova =  &host_iova;
      params.pcie_iova = NULL;
      ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );
      memMapCmd.block.mem_address = host_iova;
    }
    else
    {
      memMapCmd.block.mem_address = session_obj->mem_virt_addr_start;
    }
    memMapCmd.block.mem_size = session_obj->mem_size;
    memMapCmd.is_cached = TRUE;
    memMapCmd.cache_line_size = VS_CACHE_LINE_SIZE;
    memMapCmd.access_mask = 0x3;
    memMapCmd.page_align = VS_PAGE_ALIGN_SIZE;
    memMapCmd.min_data_width = VS_MIN_DATA_WIDTH;
    memMapCmd.max_data_width = VS_MAX_DATA_WIDTH;
  }
#endif
  if ( TRUE == vs_platform_is_fusion )
  {
    params.mem_handle = vs_shared_mem_table_handle;
    params.smem_addr = vs_shared_mem_table_phy_addr_start;
    params.adsp_iova = NULL;
    params.pcie_iova = &host_iova;
    ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );

#if (!defined AVS_MPSS_TEST)
    req.host_addr = host_iova;
    req.device_addr = (uint64_t) vs_shared_mem_table_phy_addr_start;
    req.size = vs_shared_mem_table_size; 
    req.options = 0;
    req.user_data = NULL;

#if defined (VS_PROFILING_DATA_PCIE)
  vs_data_begin_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
#endif /* VS_PROFILING_DATA_PCIE */

    ipa_dma_device_to_host( session_obj->ipa_dl_handle, &req);

#if defined (VS_PROFILING_DATA_PCIE)
  vs_data_end_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_4( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING-PCIe: MAP_MEMORY(): write memmap_table: MODEM_to_ADSP: tick1_us=(%ld), tick2_us =(%ld), diff_us=(%ld), size=(%d)",
         vs_data_begin_ticks_in_us, vs_data_end_ticks_in_us, 
         (vs_data_end_ticks_in_us - vs_data_begin_ticks_in_us), req.size );
#endif /* VS_PROFILING_DATA_PCIE */

#else
    mmstd_memcpy( (void*)host_iova, vs_shared_mem_table_size,
                  (void*)vs_shared_mem_table_phy_addr_start , vs_shared_mem_table_size );
#endif
  }

#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:VSS_IMEMORY_CMD_MAP_PHYSICAL(): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_mvm_addr, APR_NULL_V,
         job_obj->header.handle, opcode,
         &memMapCmd, sizeof( memMapCmd ) );
  if (rc)
  {
    VS_REPORT_FATAL_ON_ERROR(rc);
    session_obj->session_ctrl.status = rc;
    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );
    return APR_EIMMEDIATE;
  }

  return APR_EOK;
}

uint32_t vs_voc_action_unmap_memory (
  vs_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_simple_job_object_t* job_obj;
  vss_imemory_cmd_unmap_t unmapCmd;

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );
  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;
  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_RESULT] = vs_voc_transition_result_rsp_fn;

  unmapCmd.mem_handle = session_obj->memmap_handle;

#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:VSS_IMEMORY_CMD_UNMAP(): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_mvm_addr, APR_NULL_V,
         job_obj->header.handle, VSS_IMEMORY_CMD_UNMAP,
         &unmapCmd, sizeof( unmapCmd ) );
  if (rc)
  {
    VS_REPORT_FATAL_ON_ERROR(rc);
    session_obj->session_ctrl.status = rc;
    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );
    return APR_EIMMEDIATE;
  }

  return APR_EOK;
}

uint32_t vs_voc_action_get_avtimer_offset ( 
   vs_session_object_t* session_obj
)
{
  uint32_t rc = VS_EOK;
  vs_simple_job_object_t* job_obj;

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );
  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_RESULT] = vs_voc_transition_result_rsp_fn;
  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_AVTIMER_OFFSET] = vs_voc_get_avtimer_offset_rsp_fn;
  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;

#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:VSS_IAVTIMER_CMD_GET_OFFSET(): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  rc = __aprv2_cmd_alloc_send(
     vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
     vs_my_addr, ( (uint16_t) session_obj->header.handle ),
     vs_mvm_addr, APR_NULL_V,
     job_obj->header.handle, VSS_IAVTIMER_CMD_GET_OFFSET,
     NULL, 0 );
  if (rc)
  {
    VS_REPORT_FATAL_ON_ERROR(rc);
    session_obj->session_ctrl.status = rc;
    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );
    rc = APR_EIMMEDIATE;
  }

  return rc;
}


uint32_t vs_voc_action_hpcm_start (
  vs_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_simple_job_object_t* job_obj = NULL;
  aprv2_packet_t* packet = NULL;
  vss_ivpcm_cmd_start_v2_t* payload = NULL;
  uint8_t payloadSize = 0;
  vss_ivpcm_tap_point_t* tapPointInfo = NULL;

  /* Diable DTX for HPCM client if ENABLED by VOC client. */
  ( void ) vs_voc_action_set_dtx_v2( session_obj );

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );
  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;
  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_RESULT] = vs_voc_transition_result_rsp_fn;

  /* Update the paylaod size. */
  payloadSize = sizeof(vss_ivpcm_cmd_start_v2_t);
  if( VS_PARAM_UINT8_NOT_SET != session_obj->hpcm.txMode )
  {
    payloadSize += sizeof(vss_ivpcm_tap_point_t);
  }
  if( VS_PARAM_UINT8_NOT_SET != session_obj->hpcm.rxMode )
  {
    payloadSize += sizeof(vss_ivpcm_tap_point_t);
  }

#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:VSS_IVPCM_CMD_START_V2(): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  rc = __aprv2_cmd_alloc_ext(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_cvs_addr, session_obj->cvs_port,
         job_obj->header.handle, VSS_IVPCM_CMD_START_V2,
         payloadSize, &packet );
  VS_REPORT_FATAL_ON_ERROR( rc );
  if( packet == NULL )
  {
    /* Lock not needed as cmd failed & no response expected */
    session_obj->session_ctrl.status = rc;
    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );
    return APR_EIMMEDIATE;
  }

  payload = APRV2_PKT_GET_PAYLOAD( vss_ivpcm_cmd_start_v2_t, packet );
  payload->mem_handle = session_obj->memmap_handle;
  tapPointInfo = (vss_ivpcm_tap_point_t*)( (uint8_t*)payload + sizeof(vss_ivpcm_cmd_start_v2_t) );
  payload->num_tap_points = 0;

  if( VS_PARAM_UINT8_NOT_SET != session_obj->hpcm.txMode )
  {
    payload->num_tap_points++;
    tapPointInfo->tap_point = VSS_IVPCM_TAP_POINT_TX_DEFAULT;
    tapPointInfo->direction = session_obj->hpcm.txMode;
    tapPointInfo->sampling_rate = session_obj->hpcm.txSampleRate;
    tapPointInfo->duration = 0;
    tapPointInfo++;
  }

  if( VS_PARAM_UINT8_NOT_SET != session_obj->hpcm.rxMode )
  {
    payload->num_tap_points++;
    tapPointInfo->tap_point = VSS_IVPCM_TAP_POINT_RX_DEFAULT;
    tapPointInfo->direction = session_obj->hpcm.rxMode;
    tapPointInfo->sampling_rate = session_obj->hpcm.rxSampleRate;
    tapPointInfo->duration = 0;
    tapPointInfo++;
  }

  rc = __aprv2_cmd_forward( vs_apr_handle, packet );
  if (rc)
  {
    VS_REPORT_FATAL_ON_ERROR(rc);
    session_obj->session_ctrl.status = rc;
    ( void ) __aprv2_cmd_free( vs_apr_handle, packet );
    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );
    return APR_EIMMEDIATE;
  }

  return APR_EOK;
}


uint32_t vs_voc_action_hpcm_stop (
  vs_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_simple_job_object_t* job_obj = NULL;

  //Reset the DTX setting to default.
  ( void ) vs_voc_action_set_dtx_v2( session_obj );

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );
  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;
  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_RESULT] = vs_voc_transition_result_rsp_fn;

#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:VSS_IVPCM_CMD_STOP(): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_cvs_addr, session_obj->cvs_port,
         job_obj->header.handle, VSS_IVPCM_CMD_STOP,
         NULL, 0 );
  if (rc)
  {
    VS_REPORT_FATAL_ON_ERROR(rc);
    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );
    return APR_EIMMEDIATE;
  }

  return APR_EOK;
}


uint32_t vs_voc_action_send_pktexg_mode (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  vs_simple_job_object_t* job_obj;
  vss_ipktexg_cmd_set_mode_t pktexgMode;

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );
  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_RESULT ] = vs_voc_transition_result_rsp_fn;
  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;

  /*IMP: Check if we can add some hooks to flip to INBAND.*/

  pktexgMode.mode = VSS_IPKTEXG_MODE_MAILBOX;

#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:VSS_IPKTEXG_CMD_SET_MODE(): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_cvs_addr, session_obj->cvs_port,
         job_obj->header.handle, VSS_IPKTEXG_CMD_SET_MODE,
         &pktexgMode, sizeof( pktexgMode ) );
  if ( rc )
  {
    VS_REPORT_FATAL_ON_ERROR(rc);
    session_obj->session_ctrl.status = rc;
    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );
    return APR_EIMMEDIATE;
  }

  return APR_EOK;
}

uint32_t vs_voc_action_set_mbox_config (
  vs_session_object_t* session_obj
)
{
  int32_t rc = APR_EOK;
  vs_simple_job_object_t* job_obj;
  vss_ipktexg_cmd_mailbox_set_config_t mboxConfig;
  voicemem_cmd_get_iova_t params;
  uint64_t host_iova;

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );
  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_RESULT ] = vs_voc_transition_result_rsp_fn;
  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;

  /* Set the configuration information for Mailbox packet exchange mode */
  mboxConfig.mem_handle = session_obj->memmap_handle;
  if ( TRUE == vs_platform_is_fusion )
  {
    params.mem_handle = session_obj->voicemem_handle;
    params.smem_addr = session_obj->vocoder.mbox_config.encCircBuf.phyAddr;
    params.adsp_iova = &host_iova;
    params.pcie_iova = NULL;
    ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );
    mboxConfig.tx_circ_buf_mem_address = host_iova;

    params.smem_addr = session_obj->vocoder.mbox_config.decCircBuf.phyAddr;
    params.adsp_iova = &host_iova;
    params.pcie_iova = NULL;
    ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );
    mboxConfig.rx_circ_buf_mem_address = host_iova;
  }
  else
  {
    mboxConfig.tx_circ_buf_mem_address = session_obj->vocoder.mbox_config.encCircBuf.phyAddr;
    mboxConfig.rx_circ_buf_mem_address = session_obj->vocoder.mbox_config.decCircBuf.phyAddr;
  }
  mboxConfig.tx_circ_buf_mem_size = session_obj->vocoder.mbox_config.encCircBuf.size;
  mboxConfig.rx_circ_buf_mem_size = session_obj->vocoder.mbox_config.decCircBuf.size;

#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:VSS_IPKTEXG_CMD_MAILBOX_SET_CONFIG(): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  rc = __aprv2_cmd_alloc_send(
         vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
         vs_my_addr, ( (uint16_t) session_obj->header.handle ),
         vs_cvs_addr, session_obj->cvs_port,
         job_obj->header.handle, VSS_IPKTEXG_CMD_MAILBOX_SET_CONFIG,
         &mboxConfig, sizeof( mboxConfig ) );
  if ( rc )
  {
    VS_REPORT_FATAL_ON_ERROR(rc);
    session_obj->session_ctrl.status = rc;
    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );
    return APR_EIMMEDIATE;
  }

  return APR_EOK;
}

uint32_t vs_voc_action_put_mailbox_enc_request (
  vs_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  uint64_t virtAddr;
  /* Host represents the APQ and device represents the SDX. 
   * So host IOVA represents the APQ DDR address and 
   * device address represent the SMEM address.
   */
  uint64_t host_iova;
  uint64_t device_addr;
  vss_ipktexg_mailbox_voc_req_circ_buffer_t* encCircBuf;
  vss_ipktexg_mailbox_enc_request_t* encRequest;
  vs_shmem_block_list_item_t* encItem;
  voicemem_cmd_cache_flush_t cmdPayload;
#if (!defined AVS_MPSS_TEST)
  ipa_dma_req_t req;
#endif  
  voicemem_cmd_get_iova_t params;

  virtAddr = session_obj->vocoder.mbox_config.encCircBuf.virtAddr;
  encCircBuf = (vss_ipktexg_mailbox_voc_req_circ_buffer_t*)virtAddr;

#if VS_PROFILING
  MSG_4( MSG_SSID_DFLT, MSG_LEGACY_LOW,
         "VS: PUT_ENC_REQUEST: encCircBuf:[0x%08x] readOffset[%d] writeOffset[%d] reclaimOffset[%d]",
         encCircBuf, (uint32_t)encCircBuf->read_offset, (uint32_t) encCircBuf->write_offset, 
         (uint32_t)session_obj->vocoder.mbox_config.encReclaimOffset );
#endif /* VS_PROFILING */

  if( VS_IS_CURCULAR_BUF_FULL( encCircBuf, sizeof(vss_ipktexg_mailbox_enc_request_t),
                               session_obj->vocoder.mbox_config.encCircBufDataSize,
                               session_obj->vocoder.mbox_config.encReclaimOffset ) )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW, "VS: encCircBuffer is FULL!!");
    return VS_EOK;
  }

  rc = apr_list_remove_head( &session_obj->vocoder.mbox_config.freeEncBufferQ,
                             ( ( apr_list_node_t** ) &encItem ) );
  if ( rc )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,"VS: freeEncBufferQ is empty" );
    return VS_EOK;
  }
  apr_list_add_tail( &session_obj->vocoder.mbox_config.usedEncBufferQ, &encItem->link );

  /*  Populate encoder request & Queue the encoder request onto circular buffer. */
  encRequest = (vss_ipktexg_mailbox_enc_request_t*) ( (uint8_t*)encCircBuf +
                                                       sizeof(vss_ipktexg_mailbox_voc_req_circ_buffer_t) +
                                                       encCircBuf->write_offset );
  encRequest->timestamp_us = 0;
  if ( TRUE == vs_platform_is_fusion )
  {
    params.mem_handle = session_obj->voicemem_handle;
    params.smem_addr = encItem->buf.phyAddr;
    params.adsp_iova = &host_iova;
    params.pcie_iova = NULL;
    ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );
    encRequest->mem_address = host_iova;
  }
  else
  {
    encRequest->mem_address = encItem->buf.phyAddr;
  }
  encRequest->mem_size = VS_MBOX_BUF_SIZE;

  /* Increment circular buffer write offset. */
  VS_MODULO_INCREMENT( encCircBuf->write_offset, sizeof(vss_ipktexg_mailbox_enc_request_t),
                       session_obj->vocoder.mbox_config.encCircBufDataSize );

  cmdPayload.voicemem_handle = session_obj->voicemem_handle;
  cmdPayload.virt_addr = (void*)encRequest;
  cmdPayload.size = sizeof(vss_ipktexg_mailbox_enc_request_t);
  ( void ) voicemem_call( VOICEMEM_CMD_CACHE_FLUSH, &cmdPayload, sizeof( cmdPayload ) );

  cmdPayload.voicemem_handle = session_obj->voicemem_handle;
  cmdPayload.virt_addr = (void*)&encCircBuf->write_offset;
  cmdPayload.size = sizeof(uint64_t);
  ( void ) voicemem_call( VOICEMEM_CMD_CACHE_FLUSH, &cmdPayload, sizeof( cmdPayload ) );
  
  if( vs_platform_is_fusion )
  {
    params.mem_handle = session_obj->voicemem_handle;
    
#if (!defined AVS_MPSS_TEST)
    device_addr = (uint64_t) qurt_lookup_physaddr( (qurt_addr_t) encRequest );
    params.smem_addr = device_addr;
    params.adsp_iova = NULL;
    params.pcie_iova = &host_iova;
    ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );

    req.device_addr = device_addr;
    req.host_addr  = host_iova;
    req.size = sizeof(vss_ipktexg_mailbox_enc_request_t);
    req.options = 0;
    req.user_data = (void*)NULL;

#if defined (VS_PROFILING_DATA_PCIE)
  vs_data_begin_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
#endif /* VS_PROFILING_DATA_PCIE */

    ipa_dma_device_to_host( session_obj->ipa_ul_handle, &req);

#if defined (VS_PROFILING_DATA_PCIE)
  vs_data_end_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_4( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING-PCIe: PUT_ENC_REQUEST(): write encoding_request: MODEM_to_ADSP: tick1_us=(%ld), tick2_us =(%ld), diff_us=(%ld), size=(%d)",
         vs_data_begin_ticks_in_us, vs_data_end_ticks_in_us, 
         (vs_data_end_ticks_in_us - vs_data_begin_ticks_in_us), req.size );
#endif /* VS_PROFILING_DATA_PCIE */

    
    device_addr = (uint64_t) qurt_lookup_physaddr( (qurt_addr_t)(&encCircBuf->write_offset) );
    params.smem_addr = device_addr;
    params.adsp_iova = NULL;
    params.pcie_iova = &host_iova;
    ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );
    req.device_addr = device_addr;
    req.host_addr  = host_iova;
    req.size = sizeof(uint64_t); 
    req.options = 0;
    req.user_data = (void*)NULL;

#if defined (VS_PROFILING_DATA_PCIE)
  vs_data_begin_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
#endif /* VS_PROFILING_DATA_PCIE */

    ipa_dma_device_to_host( session_obj->ipa_ul_handle, &req);  

#if defined (VS_PROFILING_DATA_PCIE)
  vs_data_end_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_4( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING-PCIe: PUT_ENC_REQUEST(): write encCircBuf->write_offset: MAP_MEMORY: MODEM_to_ADSP: tick1_us=(%ld), tick2_us =(%ld), diff_us=(%ld), size=(%d)",
         vs_data_begin_ticks_in_us, vs_data_end_ticks_in_us, 
         (vs_data_end_ticks_in_us - vs_data_begin_ticks_in_us), req.size );
#endif /* VS_PROFILING_DATA_PCIE */


 #else
    device_addr = (uint64_t)encRequest;
    params.smem_addr = device_addr;
    params.adsp_iova = NULL;
    params.pcie_iova = &host_iova;
    ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );
    mmstd_memcpy( (void*)host_iova, sizeof(vss_ipktexg_mailbox_enc_request_t),
                  ((void*)encRequest ), sizeof(vss_ipktexg_mailbox_enc_request_t) );

    device_addr = (uint64_t)&encCircBuf->write_offset;
    params.smem_addr = device_addr;
    params.adsp_iova = NULL;
    params.pcie_iova = &host_iova;
    ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );
    mmstd_memcpy( (void*)host_iova, sizeof(uint64_t),
                  (void*)device_addr, sizeof(uint64_t) );
#endif
  }
  return rc;
}


uint32_t vs_voc_action_read_enc_mailbox (
  vs_session_object_t* session_obj,
  vs_pktexg_buffer_t** ret_buffer
)
{
  uint32_t rc = APR_EOK;
  uint64_t virtAddr;
  /* Host represents the APQ and device represents the SDX. 
   * So host IOVA represents the APQ DDR address and 
   * device address represent the SMEM address.
   */
  uint64_t host_iova;
  uint64_t device_addr;
  vss_ipktexg_mailbox_voc_req_circ_buffer_t* encCircBuf;
  vs_shmem_block_list_item_t* encItem;
  voicemem_cmd_cache_invalidate_t cmdPayload;
#if (!defined AVS_MPSS_TEST)
  ipa_dma_req_t req;
#endif  
  voicemem_cmd_get_iova_t params;

  virtAddr = session_obj->vocoder.mbox_config.encCircBuf.virtAddr;
  encCircBuf = (vss_ipktexg_mailbox_voc_req_circ_buffer_t*)virtAddr;
  if( vs_platform_is_fusion )
  {
#if (!defined AVS_MPSS_TEST)
    device_addr = (uint64_t) qurt_lookup_physaddr( (qurt_addr_t)(&encCircBuf->read_offset) );
    params.smem_addr = device_addr;
    params.mem_handle = session_obj->voicemem_handle;
    params.adsp_iova = NULL;
    params.pcie_iova = &host_iova;
    ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );

    req.device_addr = device_addr;
    req.host_addr  = host_iova;
    req.size = sizeof(uint64_t);
    req.options = 0;
    req.user_data = (void*)NULL;

#if defined (VS_PROFILING_DATA_PCIE)
  vs_data_begin_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
#endif /* VS_PROFILING_DATA_PCIE */

    ipa_dma_host_to_device( session_obj->ipa_ul_handle, &req);

#if defined (VS_PROFILING_DATA_PCIE)
  vs_data_end_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_4( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING-PCIe: READ_ENC_MAILBOX() read encCircBuf->read_offset: MAP_MEMORY: ADSP_to_MODEM: tick1_us=(%ld), tick2_us =(%ld), diff_us=(%ld), size=(%d)",
         vs_data_begin_ticks_in_us, vs_data_end_ticks_in_us, 
         (vs_data_end_ticks_in_us - vs_data_begin_ticks_in_us), req.size );
#endif /* VS_PROFILING_DATA_PCIE */


#else
    device_addr = (uint64_t)&encCircBuf->read_offset;
    params.smem_addr = device_addr;
    params.mem_handle = session_obj->voicemem_handle;
    params.adsp_iova = NULL;
    params.pcie_iova = &host_iova;
    ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );

    mmstd_memcpy( (void*)&encCircBuf->read_offset, sizeof(uint64_t),
                       (void*)host_iova , sizeof(uint64_t));
#endif
  }
  cmdPayload.voicemem_handle = session_obj->voicemem_handle;
  cmdPayload.virt_addr = (void*)&encCircBuf->read_offset;
  cmdPayload.size = sizeof(uint64_t);
  ( void ) voicemem_call( VOICEMEM_CMD_CACHE_INVALIDATE, &cmdPayload, sizeof( cmdPayload ) );

#if VS_PROFILING
  MSG_4( MSG_SSID_DFLT, MSG_LEGACY_LOW,
         "VS: READ_ENC_MBOX: encCircBuf:[0x%08x] readOffset[%d] writeOffset[%d] reclaimOffset[%d]",
         encCircBuf, (uint32_t)encCircBuf->read_offset, (uint32_t) encCircBuf->write_offset, 
         (uint32_t)session_obj->vocoder.mbox_config.encReclaimOffset );
#endif /* VS_PROFILING */

  if ( session_obj->vocoder.mbox_config.encReclaimOffset != encCircBuf->read_offset )
  {
    /* Re-claim Tx packet. */
    rc = apr_list_peak_head( &session_obj->vocoder.mbox_config.usedEncBufferQ,
                             ( ( apr_list_node_t** ) &encItem ) );
    if ( rc )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,"VS: usedEncBufferQ is empty" );
      return VS_EUNEXPECTED;
    }
    if( vs_platform_is_fusion )
    {
      device_addr = (uint64_t)encItem->buf.phyAddr;
      params.smem_addr = encItem->buf.phyAddr;
      params.mem_handle = session_obj->voicemem_handle;
      params.adsp_iova = NULL;
      params.pcie_iova = &host_iova;
      ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );
#if (!defined AVS_MPSS_TEST)
      req.device_addr = device_addr;
      req.host_addr  = host_iova;
      req.size = encItem->buf.size;
      req.options = 0;
      req.user_data = (void*)NULL;

#if defined (VS_PROFILING_DATA_PCIE)
  vs_data_begin_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
#endif /* VS_PROFILING_DATA_PCIE */

      ipa_dma_host_to_device( session_obj->ipa_ul_handle, &req);

#if defined (VS_PROFILING_DATA_PCIE)
  vs_data_end_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_4( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING-PCIe: READ_ENC_MAILBOX(): read encoder_packet: MAP_MEMORY: ADSP_to_MODEM: tick1_us=(%ld), tick2_us =(%ld), diff_us=(%ld), size=(%d)",
         vs_data_begin_ticks_in_us, vs_data_end_ticks_in_us, 
         (vs_data_end_ticks_in_us - vs_data_begin_ticks_in_us), req.size );
#endif /* VS_PROFILING_DATA_PCIE */

#else
      mmstd_memcpy( (void*)encItem->buf.phyAddr , encItem->buf.size,
                       (void*)host_iova, encItem->buf.size);
#endif   
    }    
    cmdPayload.voicemem_handle = session_obj->voicemem_handle;
    cmdPayload.virt_addr = (void*)encItem->buf.virtAddr;
    cmdPayload.size = encItem->buf.size;
    ( void ) voicemem_call( VOICEMEM_CMD_CACHE_INVALIDATE, &cmdPayload, sizeof( cmdPayload ) );
    *ret_buffer = (vs_pktexg_buffer_t*)encItem->buf.virtAddr;

    VS_MODULO_INCREMENT( session_obj->vocoder.mbox_config.encReclaimOffset,
                         sizeof(vss_ipktexg_mailbox_enc_request_t),
                         session_obj->vocoder.mbox_config.encCircBufDataSize );
    rc = VS_EOK;
  }
  else
  {
    *ret_buffer = NULL;
    rc = VS_ENOTEXIST;
  }

  return rc;
}

uint32_t vs_voc_action_put_mailbox_dec_request (
  vs_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  uint64_t virtAddr;
  uint64_t host_iova;
  vss_ipktexg_mailbox_voc_req_circ_buffer_t* decCircBuf;
  vss_ipktexg_mailbox_dec_request_t* decRequest;
  vs_shmem_block_list_item_t* decItem;
  voicemem_cmd_cache_flush_t cmdPayload;
#if (!defined AVS_MPSS_TEST)
  uint64_t device_addr;
  ipa_dma_req_t req;
#endif  
  voicemem_cmd_get_iova_t params;

  virtAddr = session_obj->vocoder.mbox_config.decCircBuf.virtAddr;
  decCircBuf = (vss_ipktexg_mailbox_voc_req_circ_buffer_t*)virtAddr;

#if VS_PROFILING
  MSG_4( MSG_SSID_DFLT, MSG_LEGACY_LOW,
         "VS: PUT_DEC_REQUEST: decCircBuf:[0x%08x] readOffset[%d] writeOffset[%d] reclaimOffset[%d]",
         decCircBuf, (uint32_t)decCircBuf->read_offset, (uint32_t)decCircBuf->write_offset, 
         (uint32_t)session_obj->vocoder.mbox_config.decReclaimOffset );
#endif /* VS_PROFILING */

  if( VS_IS_CURCULAR_BUF_FULL( decCircBuf, sizeof(vss_ipktexg_mailbox_dec_request_t),
                               session_obj->vocoder.mbox_config.decCircBufDataSize,
                               session_obj->vocoder.mbox_config.decReclaimOffset ) )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW, "VS: decCircBuffer is FULL!!");
    return VS_EOK;
  }

  rc = apr_list_remove_head( &session_obj->vocoder.mbox_config.freeDecBufferQ,
                             ( ( apr_list_node_t** ) &decItem ) );
  if ( rc )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,"VS: freeDecBufferQ is empty" );
    return VS_EOK;
  }
  apr_list_add_tail( &session_obj->vocoder.mbox_config.usedDecBufferQ, &decItem->link );

  /*  Populate decoder request & Queue the decoder request onto circular buffer. */
  decRequest = (vss_ipktexg_mailbox_dec_request_t*) ( (uint8_t*)decCircBuf +
                                                       sizeof(vss_ipktexg_mailbox_voc_req_circ_buffer_t) +
                                                       decCircBuf->write_offset );
  decRequest->timestamp_us = 0;

  if ( TRUE == vs_platform_is_fusion )
  {
    params.smem_addr = decItem->buf.phyAddr;
    params.mem_handle = session_obj->voicemem_handle;
    params.adsp_iova = &host_iova;
    params.pcie_iova = NULL;
    ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );
    decRequest->mem_address = host_iova;
  }
  else
  {
    decRequest->mem_address = decItem->buf.phyAddr;    
  }
  decRequest->mem_size = VS_MBOX_BUF_SIZE;

  /* Flush the decoder buffer. */
  cmdPayload.voicemem_handle = session_obj->voicemem_handle;
  cmdPayload.virt_addr = (void*)decItem->buf.virtAddr;
  cmdPayload.size = decItem->buf.size;
  ( void ) voicemem_call( VOICEMEM_CMD_CACHE_FLUSH, &cmdPayload, sizeof( cmdPayload ) );

  /* Flush the decoder circular buffer. */
  cmdPayload.voicemem_handle = session_obj->voicemem_handle;
  cmdPayload.virt_addr = (void*)decRequest;
  cmdPayload.size = sizeof(vss_ipktexg_mailbox_dec_request_t);
  ( void ) voicemem_call( VOICEMEM_CMD_CACHE_FLUSH, &cmdPayload, sizeof( cmdPayload ) );

  /* Increment circular buffer write offset. */
  VS_MODULO_INCREMENT( decCircBuf->write_offset, sizeof(vss_ipktexg_mailbox_dec_request_t),
                       session_obj->vocoder.mbox_config.decCircBufDataSize );
  /* Flush the decoder write offset. */
  cmdPayload.voicemem_handle = session_obj->voicemem_handle;
  cmdPayload.virt_addr = (void*)&decCircBuf->write_offset;
  cmdPayload.size = sizeof(uint64_t);
  ( void ) voicemem_call( VOICEMEM_CMD_CACHE_FLUSH, &cmdPayload, sizeof( cmdPayload ) );

  if( vs_platform_is_fusion )
  {
#if (!defined AVS_MPSS_TEST)
    /* Move the decoder buffer from device to host. */
    device_addr = (uint64_t) decItem->buf.phyAddr;
    params.smem_addr = device_addr;
    params.mem_handle = session_obj->voicemem_handle;
    params.adsp_iova = NULL;
    params.pcie_iova = &host_iova;
    ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );

    req.device_addr = device_addr;
    req.host_addr  = host_iova;
    req.size = decItem->buf.size;
    req.options = 0;
    req.user_data = (void*)NULL;

#if defined (VS_PROFILING_DATA_PCIE)
  vs_data_begin_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
#endif /* VS_PROFILING_DATA_PCIE */

    ipa_dma_device_to_host( session_obj->ipa_dl_handle, &req);

#if defined (VS_PROFILING_DATA_PCIE)
  vs_data_end_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_4( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING-PCIe: PUT_DEC_REQUEST(): write decoder_packet: MAP_MEMORY: MODEM_to_ADSP: tick1_us=(%ld), tick2_us =(%ld), diff_us=(%ld), size=(%d)",
         vs_data_begin_ticks_in_us, vs_data_end_ticks_in_us, 
         (vs_data_end_ticks_in_us - vs_data_begin_ticks_in_us), req.size );
#endif /* VS_PROFILING_DATA_PCIE */


    /* Move the decoding request from device to host. */
    device_addr = (uint64_t) qurt_lookup_physaddr( (qurt_addr_t)decRequest );
    params.smem_addr = device_addr;
    params.mem_handle = session_obj->voicemem_handle;
    params.adsp_iova = NULL;
    params.pcie_iova = &host_iova;
    ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );

    req.device_addr = device_addr;
    req.host_addr  = host_iova;
    req.size = sizeof(vss_ipktexg_mailbox_dec_request_t);
    req.options = 0;
    req.user_data = (void*)NULL;

#if defined (VS_PROFILING_DATA_PCIE)
  vs_data_begin_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
#endif /* VS_PROFILING_DATA_PCIE */

    ipa_dma_device_to_host( session_obj->ipa_dl_handle, &req );  

#if defined (VS_PROFILING_DATA_PCIE)
  vs_data_end_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_4( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING-PCIe: PUT_DEC_REQUEST(): write decoding_request: MAP_MEMORY: MODEM_to_ADSP: tick1_us=(%ld), tick2_us =(%ld), diff_us=(%ld), size=(%d)",
         vs_data_begin_ticks_in_us, vs_data_end_ticks_in_us, 
         (vs_data_end_ticks_in_us - vs_data_begin_ticks_in_us), req.size );
#endif /* VS_PROFILING_DATA_PCIE */

    /* Move the write offset update from device to host. */
    device_addr = (uint64_t) qurt_lookup_physaddr( (qurt_addr_t)(&decCircBuf->write_offset) );
    params.smem_addr = device_addr;
    params.mem_handle = session_obj->voicemem_handle;
    params.adsp_iova = NULL;
    params.pcie_iova = &host_iova;
    ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );

    req.device_addr = device_addr;
    req.host_addr  = host_iova;
    req.size = sizeof(uint64_t);
    req.options = 0;
    req.user_data = (void*)NULL;

#if defined (VS_PROFILING_DATA_PCIE)
  vs_data_begin_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
#endif /* VS_PROFILING_DATA_PCIE */

    ipa_dma_device_to_host( session_obj->ipa_dl_handle, &req );

#if defined (VS_PROFILING_DATA_PCIE)
  vs_data_end_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_4( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING-PCIe: PUT_DEC_REQUEST(): write decCircBuf->write_offset: MAP_MEMORY: MODEM_to_ADSP: tick1_us=(%ld), tick2_us =(%ld), diff_us=(%ld), size=(%d)",
         vs_data_begin_ticks_in_us, vs_data_end_ticks_in_us, 
         (vs_data_end_ticks_in_us - vs_data_begin_ticks_in_us), req.size );
#endif /* VS_PROFILING_DATA_PCIE */

#else
    params.smem_addr = ((uint64_t)decItem->buf.virtAddr );
    params.mem_handle = session_obj->voicemem_handle;
    params.adsp_iova = NULL;
    params.pcie_iova = &host_iova;
    ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );
    /* Move the decoder buffer from device to host. */
    mmstd_memcpy( (void*)host_iova, decItem->buf.size,
                        ((void*)decItem->buf.phyAddr ), decItem->buf.size );
                        
    params.smem_addr = (uint64_t)decRequest;
    params.mem_handle = session_obj->voicemem_handle;
    params.adsp_iova = NULL;
    params.pcie_iova = &host_iova;
    ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );
    /* Move the decoding request from device to host. */
    mmstd_memcpy( (void*)host_iova, sizeof(vss_ipktexg_mailbox_dec_request_t),
                    ((void*)decRequest ), sizeof(vss_ipktexg_mailbox_dec_request_t) );

    params.smem_addr = (uint64_t)(&decCircBuf->write_offset);
    params.mem_handle = session_obj->voicemem_handle;
    params.adsp_iova = NULL;
    params.pcie_iova = &host_iova;
    ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );
   /* Move the write offset update from device to host. */
    mmstd_memcpy( (void*)host_iova, sizeof(uint64_t),
                  (((void*)&decCircBuf->write_offset) ), sizeof(uint64_t) );
#endif
  }

  return rc;
}

uint32_t vs_voc_action_flush_enc_mailbox (
  vs_session_object_t* session_obj
)
{
  uint32_t rc = VS_EOK;
  uint64_t virtAddr;
  /* Host represents the APQ and device represents the SDX. 
   * So host IOVA represents the APQ DDR address and 
   * device address represent the SMEM address.
   */
  uint64_t host_iova;
  uint64_t device_addr;
  vss_ipktexg_mailbox_voc_req_circ_buffer_t* encCircBuf;
  vs_shmem_block_list_item_t* encItem;
  voicemem_cmd_cache_flush_t cmdPayload;
#if (!defined AVS_MPSS_TEST)
    ipa_dma_req_t req;
#endif  
  voicemem_cmd_get_iova_t params;

  virtAddr = session_obj->vocoder.mbox_config.encCircBuf.virtAddr;
  encCircBuf = (vss_ipktexg_mailbox_voc_req_circ_buffer_t*)virtAddr;

  if( vs_platform_is_fusion )
  {
#if (!defined AVS_MPSS_TEST)
    device_addr = (uint64_t) qurt_lookup_physaddr( (qurt_addr_t)(&encCircBuf->read_offset) );
    params.smem_addr = device_addr;
    params.mem_handle = session_obj->voicemem_handle;
    params.adsp_iova = NULL;
    params.pcie_iova = &host_iova;
    ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );

    /* Move the read offset update from device to host. */
    req.device_addr = device_addr;
    req.host_addr = host_iova;
    req.size = sizeof(uint64_t);
    req.options = 0;
    req.user_data = (void*)NULL;
#if defined (VS_PROFILING_DATA_PCIE)
  vs_data_begin_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
#endif /* VS_PROFILING_DATA_PCIE */

    ipa_dma_host_to_device( session_obj->ipa_ul_handle, &req);

#if defined (VS_PROFILING_DATA_PCIE)
  vs_data_end_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_4( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING-PCIe: FLUSH_ENC_MAILBOX(): read encCircBuf->read_offset: MAP_MEMORY: ADSP_to_Modem: tick1_us=(%ld), tick2_us =(%ld), diff_us=(%ld), size=(%d)",
         vs_data_begin_ticks_in_us, vs_data_end_ticks_in_us, 
         (vs_data_end_ticks_in_us - vs_data_begin_ticks_in_us), req.size );
#endif /* VS_PROFILING_DATA_PCIE */

#else
    device_addr = (uint64_t)&encCircBuf->read_offset;
    params.smem_addr = device_addr;
    params.mem_handle = session_obj->voicemem_handle;
    params.adsp_iova = NULL;
    params.pcie_iova = &host_iova;
    ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );

    mmstd_memcpy( (void*)&encCircBuf->read_offset , sizeof(uint64_t),
                  (void*)host_iova , sizeof(uint64_t) );
#endif
  }
  cmdPayload.voicemem_handle = session_obj->voicemem_handle;
  cmdPayload.virt_addr = (void*)&encCircBuf->read_offset;
  cmdPayload.size = sizeof(uint64_t);
  ( void ) voicemem_call( VOICEMEM_CMD_CACHE_INVALIDATE, &cmdPayload, sizeof( cmdPayload ) );

#if VS_PROFILING
  MSG_4( MSG_SSID_DFLT, MSG_LEGACY_LOW,
         "VS: FLUSH_ENC_MBOX: encCircBuf:[0x%08x] readOffset[%d] writeOffset[%d] reclaimOffset[%d]",
         encCircBuf, (uint32_t)encCircBuf->read_offset, (uint32_t) encCircBuf->write_offset, 
         (uint32_t)session_obj->vocoder.mbox_config.encReclaimOffset );
#endif /* VS_PROFILING */

  //Discard all the completed enc_request which are yet to be read.
  while ( session_obj->vocoder.mbox_config.encReclaimOffset != encCircBuf->read_offset )
  {
    /* discard Encoder packet. */
    rc = apr_list_remove_head( &session_obj->vocoder.mbox_config.usedEncBufferQ,
                               ( ( apr_list_node_t** ) &encItem ) );
    if ( rc )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,"VS: usedEncBufferQ is empty" );
      return VS_EUNEXPECTED;
    }
    apr_list_add_tail( &session_obj->vocoder.mbox_config.freeEncBufferQ, &encItem->link );

    VS_MODULO_INCREMENT( session_obj->vocoder.mbox_config.encReclaimOffset,
                         sizeof(vss_ipktexg_mailbox_enc_request_t),
                         session_obj->vocoder.mbox_config.encCircBufDataSize );

    // Requeue encoding request.
    ( void ) vs_voc_action_put_mailbox_enc_request ( session_obj );
  }

  return rc;
}

uint32_t vs_voc_action_flush_dec_mailbox (
  vs_session_object_t* session_obj
)
{
  uint32_t rc = VS_EOK;
  uint64_t virtAddr;
  /* Host represents the APQ and device represents the SDX. 
   * So host IOVA represents the APQ DDR address and 
   * device address represent the SMEM address.
   */
  uint64_t host_iova;
  uint64_t device_addr;
#if (!defined AVS_MPSS_TEST)
    ipa_dma_req_t req;
#endif  
  voicemem_cmd_get_iova_t params;
  vss_ipktexg_mailbox_voc_req_circ_buffer_t* decCircBuf;
  vs_shmem_block_list_item_t*decItem;
  voicemem_cmd_cache_flush_t cmdPayload;
  
  virtAddr = session_obj->vocoder.mbox_config.decCircBuf.virtAddr;
  decCircBuf = (vss_ipktexg_mailbox_voc_req_circ_buffer_t*)virtAddr;

#if VS_PROFILING
  MSG_4( MSG_SSID_DFLT, MSG_LEGACY_LOW,
         "VS: FLUSH_DEC_MBOX: decCircBuf:[0x%08x] readOffset[%d] writeOffset[%d] reclaimOffset[%d]",
         decCircBuf, (uint32_t)decCircBuf->read_offset, (uint32_t) decCircBuf->write_offset, 
         (uint32_t)session_obj->vocoder.mbox_config.decReclaimOffset );
#endif /* VS_PROFILING */

  //Discard all the completed dec_request which are yet to be rendered.
  while ( session_obj->vocoder.mbox_config.decReclaimOffset != decCircBuf->write_offset )
  {
    /* discard Encoder packet. */
    rc = apr_list_remove_head( &session_obj->vocoder.mbox_config.usedDecBufferQ,
                               ( ( apr_list_node_t** ) &decItem ) );
    if ( rc )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,"VS: usedEncBufferQ is empty" );
      return VS_EUNEXPECTED;
    }

    apr_list_add_tail( &session_obj->vocoder.mbox_config.freeDecBufferQ, &decItem->link );
    VS_MODULO_INCREMENT( session_obj->vocoder.mbox_config.decReclaimOffset,
                         sizeof(vss_ipktexg_mailbox_dec_request_t),
                         session_obj->vocoder.mbox_config.decCircBufDataSize );
  }

  decCircBuf->read_offset = decCircBuf->write_offset;
  cmdPayload.voicemem_handle = session_obj->voicemem_handle;
  cmdPayload.virt_addr = (void*)&decCircBuf->read_offset;
  cmdPayload.size = sizeof(uint64_t);
  ( void ) voicemem_call( VOICEMEM_CMD_CACHE_FLUSH, &cmdPayload, sizeof( cmdPayload ) );
  if( vs_platform_is_fusion )
  {
    /* Move the read offset update from device to host. */
#if (!defined AVS_MPSS_TEST)
    device_addr = (uint64_t) qurt_lookup_physaddr( (qurt_addr_t)(&decCircBuf->read_offset) );
    params.smem_addr = device_addr;
    params.mem_handle = session_obj->voicemem_handle;
    params.adsp_iova = NULL;
    params.pcie_iova = &host_iova;
    ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );

    req.device_addr = device_addr;
    req.host_addr = host_iova;
    req.size = sizeof(uint64_t);
    req.options = 0;
    req.user_data = (void*)NULL;

#if defined (VS_PROFILING_DATA_PCIE)
  vs_data_begin_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
#endif /* VS_PROFILING_DATA_PCIE */

    ipa_dma_device_to_host( session_obj->ipa_dl_handle, &req);

#if defined (VS_PROFILING_DATA_PCIE)
  vs_data_end_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_4( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING-PCIe: FLUSH_DEC_MAILBOX(): write decCircBuf->read_offset: MAP_MEMORY: MODEM_to_ADSP: tick1_us=(%ld), tick2_us =(%ld), diff_us=(%ld), size=(%d)",
         vs_data_begin_ticks_in_us, vs_data_end_ticks_in_us, 
         (vs_data_end_ticks_in_us - vs_data_begin_ticks_in_us), req.size );
#endif /* VS_PROFILING_DATA_PCIE */

#else
    device_addr = (uint64_t)&decCircBuf->read_offset;
    params.smem_addr = device_addr;
    params.mem_handle = session_obj->voicemem_handle;
    params.adsp_iova = NULL;
    params.pcie_iova = &host_iova;
    ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );

    mmstd_memcpy( (void*)host_iova  , sizeof(uint64_t),
                  (void*)&decCircBuf->read_offset , sizeof(uint64_t) );
#endif
  }
  return rc;
}

uint32_t vs_voc_action_initialize_mbox (
  vs_session_object_t* session_obj
)
{
  uint8_t index = 0;
  uint32_t shmem_offset = 0; uint64_t virtAddr = 0;
  uint64_t shmem_virt_addr_start = session_obj->mem_virt_addr_start;
  uint64_t shmem_phy_addr_start = session_obj->mem_phy_addr_start;
  vss_ipktexg_mailbox_voc_req_circ_buffer_t* encCircBuf = NULL;
  vss_ipktexg_mailbox_voc_req_circ_buffer_t* decCircBuf = NULL;
  voicemem_cmd_cache_flush_t cmdPayload;

  //Initialize HPCM shared memory.
  session_obj->hpcm.txReadBlk.size = VS_HPCM_BUF_SIZE;
  session_obj->hpcm.txReadBlk.virtAddr = shmem_virt_addr_start + shmem_offset;
  session_obj->hpcm.txReadBlk.phyAddr = shmem_phy_addr_start + shmem_offset;
  shmem_offset +=VS_ROUND_UP_ALIGN( VS_HPCM_BUF_SIZE, VS_CACHE_LINE_SIZE );
  session_obj->hpcm.txReadBuf.max_size = VS_HPCM_BUF_SIZE;
  session_obj->hpcm.txReadBuf.data = (uint8_t*)session_obj->hpcm.txReadBlk.virtAddr;

  session_obj->hpcm.txWriteBlk.size = VS_HPCM_BUF_SIZE;
  session_obj->hpcm.txWriteBlk.virtAddr = shmem_virt_addr_start + shmem_offset;
  session_obj->hpcm.txWriteBlk.phyAddr = shmem_phy_addr_start + shmem_offset;
  shmem_offset +=VS_ROUND_UP_ALIGN( VS_HPCM_BUF_SIZE, VS_CACHE_LINE_SIZE );
  session_obj->hpcm.txWriteBuf.max_size = VS_HPCM_BUF_SIZE;
  session_obj->hpcm.txWriteBuf.data = (uint8_t*)session_obj->hpcm.txWriteBlk.virtAddr;

  session_obj->hpcm.rxReadBlk.size = VS_HPCM_BUF_SIZE;
  session_obj->hpcm.rxReadBlk.virtAddr = shmem_virt_addr_start + shmem_offset;
  session_obj->hpcm.rxReadBlk.phyAddr = shmem_phy_addr_start + shmem_offset;
  shmem_offset +=VS_ROUND_UP_ALIGN( VS_HPCM_BUF_SIZE, VS_CACHE_LINE_SIZE );
  session_obj->hpcm.rxReadBuf.max_size = VS_HPCM_BUF_SIZE;
  session_obj->hpcm.rxReadBuf.data = (uint8_t*)session_obj->hpcm.rxReadBlk.virtAddr;

  session_obj->hpcm.rxWriteBlk.size = VS_HPCM_BUF_SIZE;
  session_obj->hpcm.rxWriteBlk.virtAddr = shmem_virt_addr_start + shmem_offset;
  session_obj->hpcm.rxWriteBlk.phyAddr = shmem_phy_addr_start + shmem_offset;
  shmem_offset +=VS_ROUND_UP_ALIGN( VS_HPCM_BUF_SIZE, VS_CACHE_LINE_SIZE );
  session_obj->hpcm.rxWriteBuf.max_size = VS_HPCM_BUF_SIZE;
  session_obj->hpcm.rxWriteBuf.data = (uint8_t*)session_obj->hpcm.rxWriteBlk.virtAddr;

  //Initialized shared memory for mailbox
  apr_list_init_v2( &session_obj->vocoder.mbox_config.usedEncBufferQ,
                    vs_cmdq_lock_fn, vs_cmdq_unlock_fn );
  apr_list_init_v2( &session_obj->vocoder.mbox_config.freeEncBufferQ,
                    vs_cmdq_lock_fn, vs_cmdq_unlock_fn );
  apr_list_init_v2( &session_obj->vocoder.mbox_config.usedDecBufferQ,
                    vs_cmdq_lock_fn, vs_cmdq_unlock_fn );
  apr_list_init_v2( &session_obj->vocoder.mbox_config.freeDecBufferQ,
                    vs_cmdq_lock_fn, vs_cmdq_unlock_fn );

  for( index = 0; index<VS_MAX_NUM_OF_MBOX_BUF_REQUESTS; index++ )
  {
    apr_list_init_node( &session_obj->vocoder.mbox_config.encBuffers[index].link );
    apr_list_add_tail( &session_obj->vocoder.mbox_config.freeEncBufferQ,
                       &session_obj->vocoder.mbox_config.encBuffers[index].link);

    apr_list_init_node( &session_obj->vocoder.mbox_config.decBuffers[index].link );
    apr_list_add_tail( &session_obj->vocoder.mbox_config.freeDecBufferQ,
                       &session_obj->vocoder.mbox_config.decBuffers[index].link);
  }

   //Carve out memory for Encoder circlar buffer.
  session_obj->vocoder.mbox_config.encCircBufDataSize = 
    ( VS_MAX_NUM_OF_MBOX_DATA_SLOTS * sizeof(vss_ipktexg_mailbox_enc_request_t) );
  session_obj->vocoder.mbox_config.encCircBuf.size = 
    session_obj->vocoder.mbox_config.encCircBufDataSize + sizeof(vss_ipktexg_mailbox_voc_req_circ_buffer_t);
  session_obj->vocoder.mbox_config.encCircBuf.virtAddr = shmem_virt_addr_start + shmem_offset;
  session_obj->vocoder.mbox_config.encCircBuf.phyAddr = shmem_phy_addr_start + shmem_offset;
  shmem_offset +=VS_ROUND_UP_ALIGN( session_obj->vocoder.mbox_config.encCircBuf.size,
                                    VS_CACHE_LINE_SIZE );

  //Carve out memory for Encoder buffers.
  for ( index = 0; index < VS_MAX_NUM_OF_MBOX_BUF_REQUESTS; index++ )
  {
    session_obj->vocoder.mbox_config.encBuffers[index].buf.size = VS_MBOX_BUF_SIZE;
    session_obj->vocoder.mbox_config.encBuffers[index].buf.virtAddr = shmem_virt_addr_start + shmem_offset;
    session_obj->vocoder.mbox_config.encBuffers[index].buf.phyAddr = shmem_phy_addr_start + shmem_offset;
    shmem_offset += VS_ROUND_UP_ALIGN( VS_MBOX_BUF_SIZE, VS_CACHE_LINE_SIZE );
  }

  //Carve out memory for Decoder circlar buffer.
  session_obj->vocoder.mbox_config.decCircBufDataSize = 
    ( VS_MAX_NUM_OF_MBOX_DATA_SLOTS * sizeof(vss_ipktexg_mailbox_dec_request_t) );
  session_obj->vocoder.mbox_config.decCircBuf.size = 
    session_obj->vocoder.mbox_config.decCircBufDataSize + sizeof(vss_ipktexg_mailbox_voc_req_circ_buffer_t);
  session_obj->vocoder.mbox_config.decCircBuf.virtAddr = shmem_virt_addr_start + shmem_offset;
  session_obj->vocoder.mbox_config.decCircBuf.phyAddr = shmem_phy_addr_start + shmem_offset;
  shmem_offset += VS_ROUND_UP_ALIGN( session_obj->vocoder.mbox_config.encCircBuf.size,
                                     VS_CACHE_LINE_SIZE );

  //Carve out memory for Decoder buffers.
  for ( index = 0; index < VS_MAX_NUM_OF_MBOX_BUF_REQUESTS; index++ )
  {
    session_obj->vocoder.mbox_config.decBuffers[index].buf.size = VS_MBOX_BUF_SIZE;
    session_obj->vocoder.mbox_config.decBuffers[index].buf.virtAddr = shmem_virt_addr_start + shmem_offset;
    session_obj->vocoder.mbox_config.decBuffers[index].buf.phyAddr = shmem_phy_addr_start + shmem_offset;
    shmem_offset += VS_ROUND_UP_ALIGN( VS_MBOX_BUF_SIZE, VS_CACHE_LINE_SIZE );
  }

  /* Clear Encoder circular buffer */
  virtAddr = session_obj->vocoder.mbox_config.encCircBuf.virtAddr;
  encCircBuf = (vss_ipktexg_mailbox_voc_req_circ_buffer_t*)virtAddr;
  encCircBuf->read_offset = 0;
  encCircBuf->write_offset = 0;
  session_obj->vocoder.mbox_config.encReclaimOffset = 0;

  cmdPayload.voicemem_handle = session_obj->memmap_handle;
  cmdPayload.virt_addr = (void*)session_obj->vocoder.mbox_config.encCircBuf.virtAddr;
  cmdPayload.size = session_obj->vocoder.mbox_config.encCircBuf.size;
  ( void ) voicemem_call( VOICEMEM_CMD_CACHE_FLUSH, &cmdPayload, sizeof( cmdPayload ) );
  
  /* Clear Decoder circular buffer */
  virtAddr = session_obj->vocoder.mbox_config.decCircBuf.virtAddr;
  decCircBuf = (vss_ipktexg_mailbox_voc_req_circ_buffer_t*)virtAddr;
  decCircBuf->read_offset = 0;
  decCircBuf->write_offset = 0;
  session_obj->vocoder.mbox_config.decReclaimOffset = 0;
  
  cmdPayload.voicemem_handle = session_obj->voicemem_handle;
  cmdPayload.virt_addr = (void*)session_obj->vocoder.mbox_config.decCircBuf.virtAddr;
  cmdPayload.size = session_obj->vocoder.mbox_config.decCircBuf.size;
  ( void ) voicemem_call( VOICEMEM_CMD_CACHE_FLUSH, &cmdPayload, sizeof( cmdPayload ) );

  for ( index = 0; index < VS_MAX_NUM_OF_MBOX_BUF_REQUESTS; index++ )
  {
    //Queue encoding request after mbox is intialized
    ( void ) vs_voc_action_put_mailbox_enc_request ( session_obj );
  }

  return VS_EOK;
}


uint32_t vs_voc_action_get_cvd_version ( 
   vs_session_object_t* session_obj
)
{
  uint32_t rc = VS_EOK;
  vs_simple_job_object_t* job_obj;

  rc = vs_create_simple_job_object( session_obj->header.handle, &job_obj );
  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_RESULT] = vs_voc_transition_result_rsp_fn;
  job_obj->fn_table[ VS_RESPONSE_FN_ENUM_CVD_VERSION] = vs_voc_get_cvd_version_rsp_fn;
  session_obj->session_ctrl.transition_job_handle = job_obj->header.handle;

  rc = __aprv2_cmd_alloc_send(
     vs_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
     vs_my_addr, ( (uint16_t) session_obj->header.handle ),
     vs_mvm_addr, APR_NULL_V,
     job_obj->header.handle, VSS_IVERSION_CMD_GET,
     NULL, 0 );
  if (rc)
  {
    VS_REPORT_FATAL_ON_ERROR(rc);
    session_obj->session_ctrl.status = rc;
    ( void ) vs_mem_free_object( ( vs_object_t* ) job_obj );
    rc = APR_EIMMEDIATE;
  }

  return rc;
}


