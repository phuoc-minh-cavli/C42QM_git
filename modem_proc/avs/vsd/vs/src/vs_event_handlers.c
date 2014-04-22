/**
  @file vs_event_handlers.c
  @brief This file contains the definitions of the internal functions of VS.
*/

/*
  Copyright (C) 2018-2019 QUALCOMM Technologies, Inc.
  All Rights Reserved.
  Confidential and Proprietary - Qualcomm Technologies, Inc.

  $Header: //components/rel/avs.mpss/8.0.c12/vsd/vs/src/vs_event_handlers.c#1 $
  $Author: pwbldsvc $
*/

/****************************************************************************
 * HEADER FILES
 ****************************************************************************/
#include "vs_i.h"


/****************************************************************************
 * VS EVENT HANDLERS
 ****************************************************************************/

uint32_t vs_stream_ready_event_handler (
  aprv2_packet_t* packet
)
{
  uint32_t rc = VS_EOK;
  vs_session_object_t* session_obj = NULL;

  for ( ;; )
  {
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "VS: VSS_ISTREAM_EVT_READY: src_port[0x%08x], dst_port[0x%08x], token[0x%08x]",
            packet->src_port, packet->dst_port, packet->token );

    /* Get the session object with respect to the handle */
    rc = vs_get_typed_object( packet->dst_port, VS_OBJECT_TYPE_ENUM_SESSION,
                            (vs_object_t**)&session_obj );
    if ( NULL == session_obj )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      break;
    }

    VS_ACQUIRE_LOCK( session_obj->session_lock );

    session_obj->is_stream_ready = TRUE;
    vs_common_send_event( session_obj->header.handle, VS_VOC_EVENT_READY, NULL, 0 );

    /* Reapply cached params. */
     if( NULL !=  session_obj->vocoder.set_cached_param_fn )
    {
      rc = session_obj->vocoder.set_cached_param_fn( session_obj );
    }

    VS_RELEASE_LOCK( session_obj->session_lock );

    break;
  }

  rc =  __aprv2_cmd_free( vs_apr_handle, packet );
  VS_REPORT_FATAL_ON_ERROR( rc );

  return VS_EOK;
}


uint32_t vs_stream_not_ready_event_handler (
  aprv2_packet_t* packet
)
{
  uint32_t rc = VS_EOK;
  vs_session_object_t* session_obj = NULL;

  for ( ;; )
  {
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "VS: STREAM_NOT_READY: src_port[0x%08x], dst_port[0x%08x], token[0x%08x]",
           packet->src_port, packet->dst_port, packet->token );

    /* Get the session object with respect to the handle */
    rc = vs_get_typed_object( packet->dst_port, VS_OBJECT_TYPE_ENUM_SESSION,
                              (vs_object_t**)&session_obj);
    if ( NULL == session_obj )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      break;
    }

    VS_ACQUIRE_LOCK( session_obj->session_lock );
    session_obj->is_stream_ready = FALSE;
    vs_common_send_event( session_obj->header.handle, VS_VOC_EVENT_NOT_READY, NULL, 0 ) ;
    VS_RELEASE_LOCK( session_obj->session_lock );

    break;
  }

  rc =  __aprv2_cmd_free( vs_apr_handle, packet );
  VS_REPORT_FATAL_ON_ERROR( rc );

  return VS_EOK;
}


uint32_t vs_stream_eamr_mode_change_event_handler (
  aprv2_packet_t* packet
)
{
  uint32_t rc = VS_EOK;
  vs_session_object_t* session_obj = NULL;
  vss_istream_evt_eamr_mode_changed_t eamr_rsp;

  for ( ;; )
  {
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "VS: EAMR_MODE_CHANGED: src_port[0x%08x] dst_port[0x%08x] token[0x%08x]",
           packet->src_port, packet->dst_port, packet->token );
    
    /* Get the session object with respect to the handle */
    rc = vs_get_typed_object( packet->dst_port, VS_OBJECT_TYPE_ENUM_SESSION,
                              (vs_object_t**)&session_obj );
    if ( NULL == session_obj )
    {
       VS_REPORT_FATAL_ON_ERROR( rc );
       break;
    }

    eamr_rsp.mode = APRV2_PKT_GET_PAYLOAD( vss_istream_evt_eamr_mode_changed_t, packet )->mode;
    if ( ( eamr_rsp.mode == VSS_ISTREAM_EAMR_MODE_NARROWBAND ) ||
         ( eamr_rsp.mode == VSS_ISTREAM_EAMR_MODE_WIDEBAND ) )
    {
      vs_common_send_event( session_obj->header.handle, VS_VOC_EVENT_OPERATING_MODE_UPDATE,
                            &eamr_rsp, sizeof( vs_voc_event_operating_mode_update_t ) );
    }
    else
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VS: EAMR_MODE_CHANGED: Invalid mode[0x%08x]", eamr_rsp.mode );
    }

    break;
  }

  rc =  __aprv2_cmd_free( vs_apr_handle, packet );
  VS_REPORT_FATAL_ON_ERROR( rc );

  return VS_EOK;
}

uint32_t vs_stream_dec_buf_request_event_handler (
  aprv2_packet_t* packet
)
{
  uint32_t rc = VS_EOK;
  vs_session_object_t* session_obj = NULL;

  for ( ;; )
  {
    /* Get the session object with respect to the handle */
    rc = vs_get_typed_object( packet->dst_port, VS_OBJECT_TYPE_ENUM_SESSION,
                              (vs_object_t**)&session_obj );
    if ( NULL == session_obj )
    {
       VS_REPORT_FATAL_ON_ERROR( rc );
       break;
    }

    VS_ACQUIRE_LOCK( session_obj->session_lock );

    vs_common_send_event( session_obj->header.handle, VS_PKTEXG_EVENT_DECODER_BUFFER_REQUEST, NULL, 0 ) ;

    VS_RELEASE_LOCK( session_obj->session_lock );

    break;
  }

  rc =  __aprv2_cmd_free( vs_apr_handle, packet );
  VS_REPORT_FATAL_ON_ERROR( rc );

  return VS_EOK;
}


uint32_t vs_stream_enc_buf_ready_event_handler (
  aprv2_packet_t* packet
)
{
  uint32_t rc = VS_EOK;
  vs_session_object_t* session_obj = NULL;

  for ( ;; )
  {
    /* Get the session object with respect to the handle */
    rc = vs_get_typed_object( packet->dst_port, VS_OBJECT_TYPE_ENUM_SESSION,
                              (vs_object_t**)&session_obj );
    if ( NULL == session_obj )
    {
       VS_REPORT_FATAL_ON_ERROR( rc );
       break;
    }

    VS_ACQUIRE_LOCK( session_obj->session_lock );

    ( void ) vs_common_send_event( session_obj->header.handle, VS_PKTEXG_EVENT_ENCODER_BUFFER_AVAILABLE, NULL, 0 ) ;

    VS_RELEASE_LOCK( session_obj->session_lock );

    break;
  }

  rc =  __aprv2_cmd_free( vs_apr_handle, packet );
  VS_REPORT_FATAL_ON_ERROR( rc );

  return VS_EOK;
}


uint32_t vs_stream_oobtty_char_notify_event_handler (
  aprv2_packet_t* packet
)
{
  uint32_t rc = VS_EOK;
  vs_session_object_t* session_obj = NULL;
  vs_oobtty_event_notify_char_t eventParam;
  vss_ittyoob_evt_tx_char_t* payload = NULL;

  for ( ;; )
  {
    /* Get the session object with respect to the handle */
    rc = vs_get_typed_object( packet->dst_port, VS_OBJECT_TYPE_ENUM_SESSION,
                              (vs_object_t**)&session_obj );
    if ( NULL == session_obj )
    {
       VS_REPORT_FATAL_ON_ERROR( rc );
       break;
    }

    payload = APRV2_PKT_GET_PAYLOAD( vss_ittyoob_evt_tx_char_t, packet );

    VS_ACQUIRE_LOCK( session_obj->session_lock );

    eventParam.tty_char = payload->tty_char;
    ( void ) vs_common_send_event( session_obj->header.handle,
                                   VS_OOBTTY_EVENT_NOTIFY_CHAR, &eventParam, sizeof(eventParam) ) ;

    VS_RELEASE_LOCK( session_obj->session_lock );

    break;
  }

  rc =  __aprv2_cmd_free( vs_apr_handle, packet );
  VS_REPORT_FATAL_ON_ERROR( rc );

  return VS_EOK;
}

uint32_t vs_path_delay_notify_event_handler (
  aprv2_packet_t* packet
)
{
  uint32_t rc = VS_EOK;
  vs_session_object_t* session_obj = NULL;
  vss_iavsync_evt_path_delay_t* payload;
  vs_voc_event_path_delay_t delay;
  
  for ( ;; )
  {
    /* Get the session object with respect to the handle */
    rc = vs_get_typed_object( packet->dst_port, VS_OBJECT_TYPE_ENUM_SESSION,
                              (vs_object_t**)&session_obj );
    if ( NULL == session_obj )
    {
       VS_REPORT_FATAL_ON_ERROR( rc );
       break;
    }

    payload = APRV2_PKT_GET_PAYLOAD( vss_iavsync_evt_path_delay_t, packet );
    session_obj->path_delay.tx_path_delay_us =  payload->tx_path_delay_us + VS_VOC_TIMING_ENC_OFFSET_MARGIN_V;
    session_obj->path_delay.rx_path_delay_us =  payload->rx_path_delay_us + VS_VOC_TIMING_DEC_OFFSET_MARGIN_V;
    
    delay.tx_path_delay_us =  session_obj->path_delay.tx_path_delay_us;
    delay.rx_path_delay_us =  session_obj->path_delay.rx_path_delay_us;
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "VS: PATH_DELAY_NOTIFICATION: tx_delay:[%d] rx_delay:[%d]", 
			  delay.tx_path_delay_us, delay.rx_path_delay_us );
    ( void ) vs_common_send_event( session_obj->header.handle,
                                   VS_VOC_EVENT_PATH_DELAY, &delay, sizeof(delay) ) ;
    break;
  }

  rc =  __aprv2_cmd_free( vs_apr_handle, packet );
  VS_REPORT_FATAL_ON_ERROR( rc );
  return VS_EOK;
}

uint32_t vs_stream_hpcm_notify_event_handler (
  aprv2_packet_t* packet
)
{
  uint32_t rc = VS_EOK;
  vs_session_object_t* session_obj = NULL;
  vs_hpcm_event_notify_buffer_t eventParam;
  vss_ivpcm_evt_notify_v2_t* payload = NULL;
  voicemem_cmd_cache_invalidate_t cmdPayload;
#if (!defined AVS_MPSS_TEST)   
  ipa_dma_req_t req;
#endif  
  voicemem_cmd_get_iova_t params;
  /* Host represents the APQ and device represents the SDX. 
   * So host IOVA represents the APQ DDR address and 
   * device address represent the SMEM address.
   */
  uint64_t host_iova;
  uint64_t device_addr;

  for ( ;; )
  {
    /* Get the session object with respect to the handle */
    rc = vs_get_typed_object( packet->dst_port, VS_OBJECT_TYPE_ENUM_SESSION,
                              (vs_object_t**)&session_obj );
    if ( NULL == session_obj )
    {
       VS_REPORT_FATAL_ON_ERROR( rc );
       break;
    }

    payload = APRV2_PKT_GET_PAYLOAD( vss_ivpcm_evt_notify_v2_t, packet );

    VS_ACQUIRE_LOCK( session_obj->session_lock );

    if( VSS_IVPCM_TAP_POINT_RX_DEFAULT == payload->tap_point )
    {
      eventParam.notify_mask = payload->notify_mask;
      eventParam.tap_point_id = payload->tap_point;
      eventParam.read_buffer = &session_obj->hpcm.rxReadBuf;
      eventParam.write_buffer = &session_obj->hpcm.rxWriteBuf;

      //outbuffer is filled
      if( payload->notify_mask & VSS_HPCM_MASK_READ_BUFFER )
      {
        eventParam.write_buffer->data_size = payload->request_buf_size;

        eventParam.read_buffer->data_size = payload->filled_out_size;
        eventParam.read_buffer->num_channels =  payload->num_out_channels;
        eventParam.read_buffer->sample_rate = payload->sampling_rate;

        if( vs_platform_is_fusion )
        {
          device_addr = session_obj->hpcm.rxReadBlk.phyAddr;
          params.smem_addr = device_addr;
          params.mem_handle = session_obj->voicemem_handle;
          params.adsp_iova = NULL;
          params.pcie_iova = &host_iova;
          ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );
#if (!defined AVS_MPSS_TEST)
          req.device_addr = device_addr;
          req.host_addr = host_iova;
          req.size = session_obj->hpcm.rxReadBlk.size;
          req.options = 0;
          req.user_data = ( void* )NULL;
          ipa_dma_host_to_device(session_obj->ipa_dl_handle, &req);
#else
          mmstd_memcpy( ( void* )session_obj->hpcm.rxReadBlk.phyAddr, session_obj->hpcm.rxReadBlk.size, 
                        ( void* )host_iova , session_obj->hpcm.rxReadBlk.size );
#endif

        }
        cmdPayload.voicemem_handle = session_obj->voicemem_handle;
        cmdPayload.virt_addr = (void*)session_obj->hpcm.rxReadBlk.virtAddr;
        cmdPayload.size = session_obj->hpcm.rxReadBlk.size;
        ( void ) voicemem_call( VOICEMEM_CMD_CACHE_INVALIDATE, &cmdPayload, sizeof( cmdPayload ) );
      }
    }
    else if( VSS_IVPCM_TAP_POINT_TX_DEFAULT == payload->tap_point )
    {
      eventParam.notify_mask = payload->notify_mask;
      eventParam.tap_point_id = payload->tap_point;
      eventParam.read_buffer = &session_obj->hpcm.txReadBuf;
      eventParam.write_buffer = &session_obj->hpcm.txWriteBuf;

      //outbuffer is filled
      if( payload->notify_mask & VSS_HPCM_MASK_READ_BUFFER )
      {
        eventParam.write_buffer->data_size = payload->request_buf_size;

        eventParam.read_buffer->data_size = payload->filled_out_size;
        eventParam.read_buffer->num_channels =  payload->num_out_channels;
        eventParam.read_buffer->sample_rate = payload->sampling_rate;
        if( vs_platform_is_fusion )
        {
          device_addr = session_obj->hpcm.txReadBlk.phyAddr;
          params.smem_addr = device_addr;
          params.mem_handle = session_obj->voicemem_handle;
          params.adsp_iova = NULL;
          params.pcie_iova = &host_iova;
          ( void )voicemem_call ( VOICEMEM_CMD_GET_IOVA, &params, sizeof(params) );
#if (!defined AVS_MPSS_TEST)
          req.device_addr = device_addr;
          req.host_addr = host_iova;
          req.size = session_obj->hpcm.txReadBlk.size;
          req.options = 0;
          req.user_data = ( void* )NULL;
          ipa_dma_host_to_device(session_obj->ipa_ul_handle, &req);
#else
          mmstd_memcpy( ( void* )session_obj->hpcm.txReadBlk.phyAddr, session_obj->hpcm.txReadBlk.size, 
                       ( void* ) host_iova , session_obj->hpcm.txReadBlk.size );
#endif

        }
        cmdPayload.voicemem_handle = session_obj->voicemem_handle;
        cmdPayload.virt_addr = (void*)session_obj->hpcm.txReadBlk.virtAddr;
        cmdPayload.size = session_obj->hpcm.txReadBlk.size;
        ( void ) voicemem_call( VOICEMEM_CMD_CACHE_INVALIDATE, &cmdPayload, sizeof( cmdPayload ) );
      }
    }
    else
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      break;
    }

    ( void ) vs_common_send_event( session_obj->header.handle,
                                   VS_HPCM_EVENT_NOTIFY_BUFFER, &eventParam, sizeof(eventParam) ) ;

    VS_RELEASE_LOCK( session_obj->session_lock );

    break;
  }

  rc =  __aprv2_cmd_free( vs_apr_handle, packet );
  VS_REPORT_FATAL_ON_ERROR( rc );

  return VS_EOK;
}


/*************************************************************
 * GENERIC RESPONSE handlers
 *************************************************************/

void vs_response_fn_trampoline (
  uint32_t fn_index,
  aprv2_packet_t* packet
)
{
  uint32_t rc = VS_EOK;
  vs_object_t* object = NULL;
  uint32_t msg_type;

  msg_type = APR_GET_FIELD( APRV2_PKT_MSGTYPE, packet->header );

  if ( msg_type == APRV2_PKT_MSGTYPE_EVENT_V )
  {
    rc = vs_get_object( packet->dst_port, &object );
  }
  else
  {
    rc = vs_get_object( packet->token, &object );
  }

  if ( NULL != object )
  {
    switch ( object->header.type )
    {
      case VS_OBJECT_TYPE_ENUM_HANDLE:
        object->handle.fn_table[ fn_index ]( packet );
        return;

      case VS_OBJECT_TYPE_ENUM_SIMPLE_JOB:
        object->simple_job.fn_table[ fn_index ]( packet );
        return;

      default:
        break;
    }
  }

  rc =  __aprv2_cmd_free( vs_apr_handle, packet );
  VS_REPORT_FATAL_ON_ERROR( rc );
}

/****************************************************************************
 * VS CUSTOM RESPONSE HANDLERS
 ****************************************************************************/

/**
 * This response function is associated to VS_OBJECT_TYPE_ENUM_SIMPLE_JOB
 * created for facilitating the vocoder state transition of a VS SESSION.
 *
 * Some VS commands needs CVD response to procees further with the
 * vocoder state machine, hence the ADSP/CVD timeout management and recovery
 * handling has to be done.
 */
void vs_voc_transition_result_rsp_fn (
  aprv2_packet_t* packet
)
{
  uint32_t rc = VS_EOK;
  vs_simple_job_object_t* simple_obj = NULL;
  vs_session_object_t* session_obj = NULL;
  aprv2_ibasic_rsp_result_t* rsp_result = NULL;

  for ( ;; )
  {
    rc = vs_get_typed_object( packet->token, VS_OBJECT_TYPE_ENUM_SIMPLE_JOB,
                              ( ( vs_object_t** ) &simple_obj ) );
    if ( NULL == simple_obj )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      break;
    }

    rsp_result = APRV2_PKT_GET_PAYLOAD( aprv2_ibasic_rsp_result_t, packet );
    simple_obj->status = rsp_result->status;
    simple_obj->is_completed = TRUE;
    rc = vs_get_typed_object( simple_obj->context_handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &session_obj ) );
    if ( NULL == session_obj )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      break;
    }

    session_obj->session_ctrl.status = simple_obj->status;
    if ( APR_EOK != simple_obj->status )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_voc_transition_result_rsp_fn(): command (0x%08x) failed "
             "with rc=(0x%08x)", rsp_result->opcode, rsp_result->status  );
    }
    else
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vs_voc_transition_result_rsp_fn(): command (0x%08x) rc=(0x%08x)",
             rsp_result->opcode, rsp_result->status  );
    }

#if defined (VS_PROFILING)
    vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "VS_PROFILING:transition_result_rsp_fn(): vs_cmd_ticks_in_us=(%ld)",
           vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

    /* free the simple job object. */
    ( void ) vs_mem_free_object( (vs_object_t*)simple_obj );

    break;
  }  

  /* Free the apr cmd response packet. */
  rc = __aprv2_cmd_free( vs_apr_handle, packet );
  VS_REPORT_FATAL_ON_ERROR( rc );

  vs_signal_run( );
  return;
}


/**
 * This response function is associated to VS_OBJECT_TYPE_ENUM_SIMPLE_JOB
 * created for facilitating the tracking responses for non-gating CVD APIs.
 */
void vs_voc_track_result_rsp_fn (
  aprv2_packet_t* packet
)
{
  uint32_t rc = VS_EOK;
  vs_simple_job_object_t* simple_obj = NULL;
  vs_session_object_t* session_obj = NULL;
  aprv2_ibasic_rsp_result_t* rsp_result = NULL;
  vs_common_event_cmd_response_t cmdResp;

  for ( ;; )
  {
    rc = vs_get_typed_object( packet->token, VS_OBJECT_TYPE_ENUM_SIMPLE_JOB,
                              ( ( vs_object_t** ) &simple_obj ) );
    if ( NULL == simple_obj )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      break;
    }

    rsp_result = APRV2_PKT_GET_PAYLOAD( aprv2_ibasic_rsp_result_t, packet );
    simple_obj->is_completed = TRUE;
    rc = vs_get_typed_object( simple_obj->context_handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              ( ( vs_object_t** ) &session_obj ) );
    if ( NULL == session_obj )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      break;
    }

    if ( APR_EOK != simple_obj->status )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VS: track result result failed for command[0x%08x] with rc[0x%08x",
             rsp_result->opcode, rsp_result->status  );
    }

    if( VSS_ITTYOOB_CMD_SEND_RX_CHAR == rsp_result->opcode )
    {
      cmdResp.cmd_id = VS_OOBTTY_CMD_PUSH_CHAR;
      cmdResp.status = simple_obj->status;
      cmdResp.token = NULL;
      ( void ) vs_common_send_event( session_obj->header.handle,
                                     VS_COMMON_EVENT_CMD_RESPONSE, &cmdResp, sizeof(cmdResp) );
    }

    /* free the simple job object. */
    ( void ) vs_mem_free_object( (vs_object_t*)simple_obj );

    break;
  }  

  /* Free the apr cmd response packet. */
  rc = __aprv2_cmd_free( vs_apr_handle, packet );
  VS_REPORT_FATAL_ON_ERROR( rc );
  vs_signal_run();
  return;
}

void vs_voc_mailbox_memory_config_rsp_fn (
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  vs_simple_job_object_t* simple_obj = NULL;
  vs_session_object_t* session_obj = NULL;
  vss_ipktexg_rsp_mailbox_memory_config_t* mbox_cfg = NULL;

  for ( ;; )
  {
    rc = vs_get_typed_object( packet->token, VS_OBJECT_TYPE_ENUM_SIMPLE_JOB,
                              (vs_object_t**)&simple_obj );
    if ( NULL == simple_obj )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      break;
    }

    mbox_cfg = APRV2_PKT_GET_PAYLOAD( vss_ipktexg_rsp_mailbox_memory_config_t, packet );
    simple_obj->is_completed = TRUE;
    rc = vs_get_typed_object( simple_obj->context_handle,
                              VS_OBJECT_TYPE_ENUM_SESSION,
                              (vs_object_t**)&session_obj );
    if ( NULL == session_obj )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      break;
    }
    session_obj->session_ctrl.status = VS_EOK;
    MSG_5( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "VS: MBOX_CFG_RSP: mem_address_adsp[MSW] = 0x%08x, mem_address_adsp[LSW] = 0x%08x, "
             "mem_address_pcie[MSW] = 0x%08x, mem_address_pcie[LSW] = 0x%08x,  size[%d]",
             (uint32_t)(mbox_cfg->mailbox_mem_address_adsp >> 32), (uint32_t)(mbox_cfg->mailbox_mem_address_adsp), 
             (uint32_t)(mbox_cfg->mailbox_mem_address_pcie >> 32), (uint32_t)(mbox_cfg->mailbox_mem_address_pcie), mbox_cfg->mem_size );
    
#if defined (VS_PROFILING)
    vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "VS_PROFILING:VSS_IPKTEXG_CMD_REQ_MAILBOX_MEMORY_CONFIG-RSP(): vs_cmd_ticks_in_us=(%ld)",
           vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

    rc = voicemem_call( VOICEMEM_CMD_SET_MAILBOX_MEMORY_CONFIG, mbox_cfg, 
                       sizeof(voicemem_cmd_set_mailbox_memory_config_t));
                       
    if ( rc == APR_EOK ) 
    {
      vs_adsp_mbox_memory_config_is_received = TRUE;
      session_obj->session_ctrl.status = VS_EOK;
    }
    else
    {    
      session_obj->session_ctrl.status = VS_EFAILED;
    }

    /* Free the simple job object. */
    ( void ) vs_mem_free_object( (vs_object_t*)simple_obj );
    break;
  }
  /* Free the apr cmd response packet. */
  rc = __aprv2_cmd_free( vs_apr_handle, packet );
  VS_REPORT_FATAL_ON_ERROR( rc );
  vs_signal_run();
  return;
}


void vs_voc_memory_map_result_rsp_fn (
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  vs_simple_job_object_t* simple_obj = NULL;
  vs_session_object_t* session_obj = NULL;
  vss_imemory_rsp_map_t* mapRsp = NULL;

  for ( ;; )
  {
    rc = vs_get_typed_object( packet->token, VS_OBJECT_TYPE_ENUM_SIMPLE_JOB,
                              (vs_object_t**)&simple_obj );
    if ( NULL == simple_obj )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      break;
    }

    mapRsp = APRV2_PKT_GET_PAYLOAD( vss_imemory_rsp_map_t, packet );
    simple_obj->is_completed = TRUE;
    rc = vs_get_typed_object( simple_obj->context_handle,
                              VS_OBJECT_TYPE_ENUM_SESSION,
                              (vs_object_t**)&session_obj );
    if ( NULL == session_obj )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      break;
    }

    session_obj->session_ctrl.status = VS_EOK;
    session_obj->memmap_handle = mapRsp->mem_handle;
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "VS: MEM_MAP_RSP: mem_handle[0x%08x]", mapRsp->mem_handle );

    break;
  }

#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:VSS_IMEMORY_CMD_MAP_PHYSICAL-RSP(): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  /* Free the simple job object. */
  ( void ) vs_mem_free_object( (vs_object_t*)simple_obj );

  /* Free the apr cmd response packet. */
  rc = __aprv2_cmd_free( vs_apr_handle, packet );
  VS_REPORT_FATAL_ON_ERROR( rc );

  vs_signal_run();
  return;
}


void vs_voc_get_avtimer_offset_rsp_fn (
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  vs_simple_job_object_t* simple_obj = NULL;
  vs_session_object_t* session_obj = NULL;
  vss_iavtimer_rsp_get_offset_t* Rsp = NULL;

  for ( ;; )
  {
    rc = vs_get_typed_object( packet->token, VS_OBJECT_TYPE_ENUM_SIMPLE_JOB,
                              (vs_object_t**)&simple_obj );
    if ( NULL == simple_obj )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      break;
    }

    Rsp = APRV2_PKT_GET_PAYLOAD( vss_iavtimer_rsp_get_offset_t, packet );
    simple_obj->is_completed = TRUE;
    rc = vs_get_typed_object( simple_obj->context_handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              (vs_object_t**)&session_obj );
    if ( NULL == session_obj )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      break;
    }

    session_obj->session_ctrl.status = VS_EOK;
    session_obj->vocoder.timing_config.avtimerOffsetUs = Rsp->offset_us;
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "VS: GET_AVTIMER_OFFSET_RSP: MSW[%d] LSW[%d]", (uint32_t)(Rsp->offset_us>>32),
           (uint32_t)Rsp->offset_us);

    break;
  }

#if defined (VS_PROFILING)
  vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
         "VS_PROFILING:VSS_IAVTIMER_CMD_GET_OFFSET-RSP(): vs_cmd_ticks_in_us=(%ld)",
         vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

  /* Free the simple job object. */
  ( void ) vs_mem_free_object( (vs_object_t*)simple_obj );

  /* Free the apr cmd response packet. */
  rc = __aprv2_cmd_free( vs_apr_handle, packet );
  VS_REPORT_FATAL_ON_ERROR( rc );

  vs_signal_run();
  return;
}


void vs_voc_create_mvm_session_result_rsp_fn (
  aprv2_packet_t* p_packet
)
{
  uint32_t rc = APR_EOK;
  vs_simple_job_object_t* simple_obj = NULL;
  vs_session_object_t* session_obj = NULL;
  aprv2_ibasic_rsp_result_t* rsp_result = NULL;

  for ( ;; )
  {
    if ( p_packet == APR_NULL_V ) 
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      break;
    }

    rc = vs_get_typed_object( p_packet->token, VS_OBJECT_TYPE_ENUM_SIMPLE_JOB,
                              ( ( vs_object_t** ) &simple_obj ) );
    if ( NULL == simple_obj )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      break;
    }

    rsp_result = APRV2_PKT_GET_PAYLOAD( aprv2_ibasic_rsp_result_t, p_packet );
    simple_obj->is_completed = TRUE;
    simple_obj->status = rsp_result->status;

    rc = vs_get_typed_object( simple_obj->context_handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              ( vs_object_t** )&session_obj );
    if ( NULL == session_obj )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      break;
    }

    session_obj->session_ctrl.status = simple_obj->status;
    session_obj->mvm_port = p_packet->src_port;

    if ( simple_obj->status != APR_EOK )
    {
      session_obj->mvm_port = APR_NULL_V;
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VS: MVM session creation failed with rc[0x%08x]", rsp_result->status  );
    }

    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "VS: MVM_PORT (0x%04x)  set for session_handle=(0x%08x)",
           session_obj->mvm_port, session_obj->header.handle );

#if defined (VS_PROFILING)
    vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "VS_PROFILING:VSS_IMVM_CMD_CREATE_FULL_CONTROL_SESSION-RSP(): vs_cmd_ticks_in_us=(%ld)",
           vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

    /* Free the simple job object. */
    ( void ) vs_mem_free_object( ( vs_object_t* ) simple_obj );

    break;
  }

  /* Free the APR command response packet. */
  rc = __aprv2_cmd_free( vs_apr_handle, p_packet );
  VS_REPORT_FATAL_ON_ERROR( rc );

  vs_signal_run();
  return;
}


void vs_voc_create_cvs_session_result_rsp_fn (
  aprv2_packet_t* p_packet
)
{
  uint32_t rc = APR_EOK;
  vs_simple_job_object_t* simple_obj = NULL;
  vs_session_object_t* session_obj = NULL;
  aprv2_ibasic_rsp_result_t* rsp_result = NULL;

  for ( ;; )
  {
    if ( p_packet == APR_NULL_V ) 
    {
      VS_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      break;
    }

    rc = vs_get_typed_object( p_packet->token, VS_OBJECT_TYPE_ENUM_SIMPLE_JOB,
                              ( ( vs_object_t** ) &simple_obj ) );
    if ( NULL == simple_obj )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      break;
    }

    rsp_result = APRV2_PKT_GET_PAYLOAD( aprv2_ibasic_rsp_result_t, p_packet );
    simple_obj->is_completed = TRUE;
    simple_obj->status = rsp_result->status;

    rc = vs_get_typed_object( simple_obj->context_handle, VS_OBJECT_TYPE_ENUM_SESSION,
                             (vs_object_t**) &session_obj );
    if ( NULL == session_obj )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      break;
    }

    session_obj->session_ctrl.status = rsp_result->status;
    session_obj->cvs_port = p_packet->src_port;

    if ( simple_obj->status != APR_EOK )
    {
      session_obj->cvs_port = APR_NULL_V;
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VS: CVS session creation failed with rc[0x%08x]", rsp_result->status  );
    }

    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "CVS_PORT (0x%04x)  set for session_handle=(0x%08x)",
           session_obj->cvs_port, session_obj->header.handle );

#if defined (VS_PROFILING)
    vs_cmd_ticks_in_us = (uint64_t)((qurt_sysclock_get_hw_ticks()) *10ull/192ull);
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "VS_PROFILING:VSS_ISTREAM_CMD_CREATE_FULL_CONTROL_SESSION-RSP(): vs_cmd_ticks_in_us=(%ld)",
           vs_cmd_ticks_in_us );
#endif /* VS_PROFILING */

    /* Free the simple job object. */
    ( void ) vs_mem_free_object( ( vs_object_t* ) simple_obj );

    break;
  }

  rc = __aprv2_cmd_free( vs_apr_handle, p_packet );
  VS_REPORT_FATAL_ON_ERROR( rc );

  vs_signal_run();
  return;
}


void vs_voc_get_cvd_version_rsp_fn (
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  vs_simple_job_object_t* simple_obj = NULL;
  vs_session_object_t* session_obj = NULL;
  vss_iversion_rsp_get_t* rsp = NULL;

  for ( ;; )
  {
    rc = vs_get_typed_object( packet->token, VS_OBJECT_TYPE_ENUM_SIMPLE_JOB,
                              (vs_object_t**)&simple_obj );
    if ( NULL == simple_obj )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      break;
    }

    rsp = APRV2_PKT_GET_PAYLOAD( vss_iversion_rsp_get_t, packet );
    simple_obj->is_completed = TRUE;
    rc = vs_get_typed_object( simple_obj->context_handle, VS_OBJECT_TYPE_ENUM_SESSION,
                              (vs_object_t**)&session_obj );
    if ( NULL == session_obj )
    {
      VS_REPORT_FATAL_ON_ERROR( rc );
      break;
    }

    session_obj->session_ctrl.status = VS_EOK;
    mmstd_memcpy( session_obj->cvd_version, sizeof( rsp->version ),
                rsp->version, sizeof( rsp->version ) );

    break;
  }

  /* Free the simple job object. */
  ( void ) vs_mem_free_object( (vs_object_t*)simple_obj );

  /* Free the apr cmd response packet. */
  rc = __aprv2_cmd_free( vs_apr_handle, packet );
  VS_REPORT_FATAL_ON_ERROR( rc );

  vs_signal_run();
  return;
}

