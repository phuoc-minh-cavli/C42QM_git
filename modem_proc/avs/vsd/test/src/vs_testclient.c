/*
   Copyright (C) 2016-2019 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   $Header: //components/rel/avs.mpss/8.0.c12/vsd/test/src/vs_testclient.c#2 $
   $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

/* CORE APIs. */
#include <stddef.h>
#include <string.h>
#include "err.h"
#include "msg.h"
#include "mmstd.h"

/* APR APIs. */
#include "apr_errcodes.h"
#include "apr_list.h"
#include "apr_objmgr.h"
#include "apr_lock.h"
#include "apr_event.h"
#include "apr_thread.h"
#include "apr_memmgr.h"
#include "aprv2_api_inline.h"

/* VSD APIs. */
#include "drv_api.h"
#include "vs.h"

/* SELF APIs. */
#include "vs_testclient.h"
#include "vs_test_i.h"

/* VOICELOG APIs. */
#include "voicelog_if.h"
#include "voicelog_utils.h"

#include "apr_timer.h"

#include "vss_common_public_if.h"


/*****************************************************************************
 * Global Variables                                                          *
 ****************************************************************************/
char_t vs_test_my_dns[] =  "qcom.audio.vstest";
static uint16_t vs_test_my_addr;
static uint32_t vs_test_apr_handle;

static apr_lock_t vs_test_int_lock;
static apr_lock_t vs_test_thread_lock;
static apr_event_t vs_test_control_event;

static apr_memmgr_type vs_test_heapmgr;
static uint8_t vs_test_heap_pool[ VS_TEST_HEAP_SIZE_V ];

static apr_objmgr_t vs_test_objmgr;
static apr_objmgr_object_t vs_test_object_table[ VS_TEST_MAX_OBJECTS_V ];

static vs_test_gating_control_t vs_test_gating_work_pkt_q;
static apr_list_t vs_test_nongating_work_pkt_q;
static apr_list_t vs_test_free_work_pkt_q;
static vs_test_work_item_t vs_test_work_pkts[ VS_TEST_NUM_WORK_PKTS_V ];

static apr_event_t vs_test_work_event;
static apr_thread_t vs_test_thread;
static uint8_t vs_test_task_stack[ VS_TEST_STACK_SIZE_V ];

static vs_test_session_object_t* vs_test_session_obj = NULL;
static apr_lock_t vs_test_global_lock;

static bool_t vs_test_is_initialized = FALSE; 

/****************************************************************************
 * COMMON INTERNAL ROUTINES                                                 *
 ****************************************************************************/

static void vs_test_int_lock_fn ( void )
{
  apr_lock_enter( vs_test_int_lock );
}


static void vs_test_int_unlock_fn ( void )
{
  apr_lock_leave( vs_test_int_lock );
}


static void vs_test_thread_lock_fn ( void )
{
  apr_lock_enter( vs_test_thread_lock );
}


static void vs_test_thread_unlock_fn ( void )
{
  apr_lock_leave( vs_test_thread_lock );
}


/****************************************************************************
 * VS_TEST CMDs & EVENTs PACKET QUEUING FUNCTIONS                               *
 ****************************************************************************/
static uint32_t vs_test_vs_media_id [] = {
  VS_VOCAMR_MEDIA_ID,
  VS_VOCAMRWB_MEDIA_ID,
  VS_VOCEFR_MEDIA_ID,
  VS_VOCFR_MEDIA_ID,
  VS_VOCHR_MEDIA_ID,
  VS_VOCEVRCA_MEDIA_ID,
  VS_VOCEVRCB_MEDIA_ID,
  VS_VOCEVRCWB_MEDIA_ID,
  VS_VOCEVRCNW_MEDIA_ID,
  VS_VOCEVRCNW2K_MEDIA_ID,
  VS_VOCQCELP13K_MEDIA_ID,
  VS_VOCG711ALAW_V2_MEDIA_ID,
  VS_VOCG711MULAW_V2_MEDIA_ID,
  VS_VOCEVS_MEDIA_ID,
  VSS_MEDIA_ID_PCM_8_KHZ,
  VSS_MEDIA_ID_PCM_16_KHZ,
  VSS_MEDIA_ID_PCM_32_KHZ,
  VSS_MEDIA_ID_PCM_48_KHZ,
  VS_VOCEAMR_MEDIA_ID,
  VS_VOCAMRWB_2_MEDIA_ID
};

static uint32_t vs_test_vs_frame_rate []={
  VS_TEST_FRAME_BLANK_RATE,
  VS_TEST_FRAME_EIGHTH_RATE,
  VS_TEST_FRAME_QUARTER_RATE,
  VS_TEST_FRAME_HALF_RATE,
  VS_TEST_FRAME_FULL_RATE,
  VS_TEST_FRAME_ERASURE_RATE,
  VS_TEST_FRAME_EIGHTH_RATE_NONCRITICAL
};

static uint32_t vs_test_vs_sr [] ={
  8000,
  16000,
  32000,
  64000
};

static uint32_t vs_test_set_voc_plc( 
  vs_test_session_object_t* session_obj 
)
{
  uint32_t rc = APR_EOK;
  vs_common_cmd_set_param_t setParamCmd;
  vs_param_voc_plc_t param;

  if ( APR_NULL_V == session_obj->vs_handle ) 
  {
    VS_TEST_REPORT_FATAL_ON_ERROR( VS_EHANDLE );
    return VS_EHANDLE;
  }

   param.plc = session_obj->plc;
   setParamCmd.handle = session_obj->vs_handle;
   setParamCmd.param_data = (void*)&param;
   setParamCmd.param_id = VS_PARAM_VOC_PLC;
   setParamCmd.size = sizeof( param );

   rc  = vs_call( VS_COMMON_CMD_SET_PARAM, ( void* )&setParamCmd, sizeof( setParamCmd ) );
   if ( rc != APR_EOK )
   {
     MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
            "vs_test_set_voc_plc(): failed to set plc_mode=(%d), vocoder=(0x%08x)",
            session_obj->plc, session_obj->vocoder_type );
   }

   return rc;
}

static uint32_t vs_test_vs_set_voc_media_type (
  vs_test_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_param_voc_media_id_t param;
  vs_common_cmd_set_param_t setParamCmd;

  if ( APR_NULL_V == session_obj->vs_handle ) 
  {
    VS_TEST_REPORT_FATAL_ON_ERROR( VS_EHANDLE );
    return VS_EHANDLE;
  }

  param.media_id =  session_obj->vocoder_type ;
  setParamCmd.handle = session_obj->vs_handle;
  setParamCmd.param_data = (void*)&param;
  setParamCmd.param_id = VS_PARAM_VOC_MEDIA_ID;
  setParamCmd.size = sizeof( param );
 
  rc  = vs_call( VS_COMMON_CMD_SET_PARAM, ( void* )&setParamCmd, sizeof( setParamCmd ) );
  if ( rc != APR_EOK )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_test_vs_set_voc_media_type(): failed to set mediaID[0x%08x]",
           session_obj->vocoder_type );
  }

  return rc;
}

 /* DTX Enable/Disable command */
static uint32_t vs_test_set_voc_dtx_mode( 
 vs_test_session_object_t* session_obj 
)
{
  uint32_t rc = APR_EOK;
  vs_common_cmd_set_param_t setParamCmd;
  vs_param_enc_dtx_t param;

  if ( APR_NULL_V == session_obj->vs_handle ) 
  {
    VS_TEST_REPORT_FATAL_ON_ERROR( VS_EHANDLE );
    return VS_EHANDLE;
  }

  param.dtx = (uint8_t) session_obj->dtx_mode;
  setParamCmd.handle = session_obj->vs_handle;
  setParamCmd.param_data = (void*)&param;
  setParamCmd.param_id = VS_PARAM_ENC_DTX;
  setParamCmd.size = sizeof( param );

  rc  = vs_call( VS_COMMON_CMD_SET_PARAM, ( void* )&setParamCmd, sizeof( setParamCmd ) );
  if ( rc != APR_EOK )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_test_set_voc_dtx_mode(): failed to set dtx_mode=(%d), vocoder=(0x%08x)",
           session_obj->dtx_mode, session_obj->vocoder_type );
  }

  return rc;
}

/* Adaptive DTX Enable / Fixed DTX Enable / DTX Disable command */
 static uint32_t vs_test_set_voc_dtx_mode_v2( 
  vs_test_session_object_t* session_obj 
 )
 {
   uint32_t rc = APR_EOK;
   vs_common_cmd_set_param_t setParamCmd;
   vs_param_enc_dtx_v2_t param;
 
   if ( APR_NULL_V == session_obj->vs_handle ) 
   {
     VS_TEST_REPORT_FATAL_ON_ERROR( VS_EHANDLE );
     return VS_EHANDLE;
   }
 
   param.dtx_mode =  session_obj->dtx_mode;
   param.version = 1;
   setParamCmd.handle = session_obj->vs_handle;
   setParamCmd.param_data = (void*)&param;
   setParamCmd.param_id = VS_PARAM_ENC_DTX_V2;
   setParamCmd.size = sizeof( param );
 
   rc  = vs_call( VS_COMMON_CMD_SET_PARAM, ( void* )&setParamCmd, sizeof( setParamCmd ) );
   if ( rc != APR_EOK )
   {
     MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
            "vs_test_set_voc_dtx_mode_v2(): failed to set dtx_mode=(%d), vocoder=(0x%08x)",
            session_obj->dtx_mode, session_obj->vocoder_type );
   }
 
   return rc;
 }


 static uint32_t vs_test_set_voc_operating_mode( 
  vs_test_session_object_t* session_obj 
 )
 {
   uint32_t rc = APR_EOK;
   vs_common_cmd_set_param_t setParamCmd;
   vs_param_enc_operating_mode_t param;

   if ( APR_NULL_V == session_obj->vs_handle ) 
   {
      VS_TEST_REPORT_FATAL_ON_ERROR( VS_EHANDLE );
      return VS_EHANDLE;
   }

   param.rate = session_obj->codec_mode;
   param.bandwidth = session_obj->bandwidth;
   setParamCmd.handle = session_obj->vs_handle;
   setParamCmd.param_data = (void*)&param;
   setParamCmd.param_id = VS_PARAM_ENC_OPERATING_MODE;
   setParamCmd.size = sizeof( param );

   rc  = vs_call( VS_COMMON_CMD_SET_PARAM, ( void* )&setParamCmd, sizeof( setParamCmd ) );
   if ( rc != APR_EOK )
   {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
          "vs_test_set_voc_operating_mode(): failed to set operating mode=(%d), vocoder=(0x%08x)",
            session_obj->codec_mode, session_obj->vocoder_type );
   }

   return rc;
 }

 static uint32_t vs_test_set_voc_min_max_rate (
   vs_test_session_object_t* session_obj
 )
 {
   uint32_t rc = APR_EOK;
   vs_param_enc_minmax_rate_t param;
   vs_common_cmd_set_param_t setParamCmd;

   if ( APR_NULL_V == session_obj->vs_handle ) 
   {
      VS_TEST_REPORT_FATAL_ON_ERROR( VS_EHANDLE );
      return VS_EHANDLE;
   }

   param.min_rate = session_obj->min_rate;
   param.max_rate = session_obj->max_rate;
   setParamCmd.handle = session_obj->vs_handle;
   setParamCmd.param_data = (void*)&param;
   setParamCmd.param_id = VS_PARAM_ENC_MINMAX_FRAME_RATE;
   setParamCmd.size = sizeof( param );

   rc  = vs_call( VS_COMMON_CMD_SET_PARAM, ( void* )&setParamCmd, sizeof( setParamCmd ) );

   if ( rc != APR_EOK )
   {
     MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
            "vs_test_set_voc_min_max_rate(): Failed!!!! vocoder_type = (0x%08x), "
            "min_rate = (0x%08x), max_rate=(0x%08x)", session_obj->vocoder_type,
            session_obj->min_rate, session_obj->max_rate );
   }
 
   return rc;
 }

static uint32_t vs_test_set_voc_enc_rate (
  vs_test_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_param_enc_rate_t param;
  vs_common_cmd_set_param_t setParamCmd;

  if ( APR_NULL_V == session_obj->vs_handle ) 
  {
    VS_TEST_REPORT_FATAL_ON_ERROR( VS_EHANDLE );
    return VS_EHANDLE;
  }

  param.rate = session_obj->avg_enc_rate ;
  setParamCmd.handle = session_obj->vs_handle;
  setParamCmd.param_data = (void*)&param;
  setParamCmd.param_id = VS_PARAM_ENC_RATE;
  setParamCmd.size = sizeof( param );

  rc  = vs_call( VS_COMMON_CMD_SET_PARAM, ( void* )&setParamCmd, sizeof( setParamCmd ) );
  VS_TEST_REPORT_FATAL_ON_ERROR( rc );

  return rc;
}

static uint32_t vs_test_set_max_tx_rx_sr (
  vs_test_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_param_max_pp_sample_rate_t param ;
  vs_common_cmd_set_param_t setParamCmd;

  if ( APR_NULL_V == session_obj->vs_handle ) 
  {
    VS_TEST_REPORT_FATAL_ON_ERROR( VS_EHANDLE );
    return VS_EHANDLE;
  }

  param.rx_pp_sample_rate = session_obj->max_rx_sr;
  param.tx_pp_sample_rate = session_obj->max_tx_sr;
  setParamCmd.handle = session_obj->vs_handle;
  setParamCmd.param_data = (void*)&param;
  setParamCmd.param_id = VS_PARAM_MAX_PP_SAMPLE_RATE;
  setParamCmd.size = sizeof( param );

  rc  = vs_call( VS_COMMON_CMD_SET_PARAM, ( void* )&setParamCmd, sizeof( setParamCmd ) );

  if ( rc != APR_EOK )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_test_set_max_tx_rx_sr(): failed with RC(0x%08x)", rc );
  }
  return rc;
}

static uint32_t vs_test_vocevs_channel_aware_mode_control (
  vs_test_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_param_enc_channel_aware_mode_t param;
  vs_common_cmd_set_param_t setParamCmd;

  if ( session_obj->channel_aware_mode_enabled == VS_TEST_VOCEVS_CHANNEL_AWARE_MODE_ENABLE_V )
  {
    param.fec_offset = session_obj->fec_offset;
    param.fer_rate = session_obj->fer_rate;
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "VS_TEST: EVS Channel_aware_mode_enable sent to VS with FRC Offset %d, FER Rate %d", 
           param.fec_offset, param.fer_rate );
    param.channel_awareness_mode = 1;
  }
  else if ( session_obj->channel_aware_mode_enabled == VS_TEST_VOCEVS_CHANNEL_AWARE_MODE_DISABLE_V )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "VS_TEST: EVS Channel_aware_mode_disable sent to VS" );
    param.channel_awareness_mode = 0;
  }
  else
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "vs_test_vocevs_channel_aware_mode_control: Not set/Invalid command!!" );
  }

  setParamCmd.handle = session_obj->vs_handle;
  setParamCmd.param_data = (void*)&param;
  setParamCmd.param_id = VS_PARAM_ENC_CHANNEL_AWARE_MODE;
  setParamCmd.size = sizeof( param );

  rc  = vs_call( VS_COMMON_CMD_SET_PARAM, ( void* )&setParamCmd, sizeof( setParamCmd ) );

  if ( rc != APR_EOK )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_test_vocevs_channel_aware_mode_control: failed enable/disable channel aware mode" );
  }
  return rc;
}

/* Set different vocoder property */
static uint32_t vs_test_set_voc_property_cmd_ctrl (
vs_test_gating_control_t* ctrl
)
{
  uint32_t rc = APR_EOK;
  aprv2_packet_t* packet = ctrl->packet;
  vs_test_session_object_t* session_obj = vs_test_session_obj;
  vs_test_loopback_cmd_t* cmd_params = NULL;

  for ( ;; )
  {
	if( packet == NULL )
    {
      VS_TEST_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }
	
    cmd_params = APRV2_PKT_GET_PAYLOAD( vs_test_loopback_cmd_t, packet );

    if( cmd_params == NULL )
    {
      VS_TEST_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "vs_test_set_voc_property_cmd_ctrl(): CommandID(%d) Payload(%d).", 
           cmd_params->payload1, cmd_params->payload2 );

    switch( cmd_params->payload1 )
    {
      case VS_TEST_VOC_PROPERTY_DTX:
        session_obj->dtx_mode = cmd_params->payload2;
        rc = vs_test_set_voc_dtx_mode( session_obj );
        VS_TEST_REPORT_FATAL_ON_ERROR( rc );
        break;

      case VS_TEST_VOC_PROPERTY_MEDIA_ID:
        if(cmd_params->payload2 < SIZE_OF_ARRAY(vs_test_vs_media_id))
        {
          session_obj->vocoder_type = vs_test_vs_media_id[ cmd_params->payload2 ];
          (void) vs_test_vs_set_voc_media_type(session_obj);
        }
        else  VS_TEST_REPORT_FATAL_ON_ERROR( VS_EBADPARAM );
        break;

      case VS_TEST_VOC_PROPERTY_MIN_MAX_RATE:
        if((cmd_params->payload2 < SIZE_OF_ARRAY(vs_test_vs_frame_rate) )
            && (cmd_params->payload3 < SIZE_OF_ARRAY(vs_test_vs_frame_rate) ))
        {
          session_obj->min_rate = vs_test_vs_frame_rate[ cmd_params->payload2 ];
          session_obj->max_rate = vs_test_vs_frame_rate[ cmd_params->payload3 ];
          (void) vs_test_set_voc_min_max_rate( session_obj );
        }
        else  VS_TEST_REPORT_FATAL_ON_ERROR( rc );
        break;

      case VS_TEST_VOC_PROPERTY_ENC_RATE:
        session_obj->avg_enc_rate = cmd_params->payload2;
        rc = vs_test_set_voc_enc_rate( session_obj );
        VS_TEST_REPORT_FATAL_ON_ERROR( rc );
        break;


      case VS_TEST_VOC_PROPERTY_OPERATING_BANDWIDTH:
        if(cmd_params->payload2 < SIZE_OF_ARRAY(vs_test_vs_sr) )
        {
          session_obj->codec_mode = cmd_params->payload2;
          session_obj->bandwidth = vs_test_vs_sr[ cmd_params->payload3 ];
          rc = vs_test_set_voc_operating_mode(session_obj);
        }
        else  VS_TEST_REPORT_FATAL_ON_ERROR( rc );
        break;

      case VS_TEST_VOC_PROPERTY_MAX_TX_RX_SR:
        if((cmd_params->payload2 < SIZE_OF_ARRAY(vs_test_vs_sr) )
            && (cmd_params->payload3 < SIZE_OF_ARRAY(vs_test_vs_sr) ))
        {
          session_obj->max_tx_sr = vs_test_vs_sr[ cmd_params->payload2 ];
          session_obj->max_rx_sr = vs_test_vs_sr[ cmd_params->payload3 ];
          rc = vs_test_set_max_tx_rx_sr( session_obj );
        }
        else  VS_TEST_REPORT_FATAL_ON_ERROR( rc );
        break;

      case VS_TEST_VOC_PROPERTY_CHANNEL_AWARE_MODE:
        session_obj->channel_aware_mode_enabled = cmd_params->payload2;
        session_obj->fec_offset = cmd_params->payload3;
        session_obj->fer_rate = cmd_params->payload4;
        rc = vs_test_vocevs_channel_aware_mode_control( session_obj );
        VS_TEST_REPORT_FATAL_ON_ERROR( rc );
        break;

      case VS_TEST_VOC_PROPERTY_PLC:
        session_obj->plc = cmd_params->payload2;
        rc= vs_test_set_voc_plc( session_obj );
        VS_TEST_REPORT_FATAL_ON_ERROR( rc );
        break;

      case VS_TEST_VOC_PROPERTY_DTX_CONFIG:
        session_obj->dtx_mode = cmd_params->payload2;
        rc = vs_test_set_voc_dtx_mode_v2( session_obj );
        VS_TEST_REPORT_FATAL_ON_ERROR( rc );
        break;

      default:
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "VS_TEST: paramID(%d) not supported.", cmd_params->payload1 );
        break;
    }

    break;
  }

  return VS_EOK;
}

/* This sets the cached vocder properties during the start_voice command */
 static uint32_t vs_test_set_cached_voc_property (
 vs_test_session_object_t* session_obj 
)
{
  uint32_t rc = APR_EOK;

  if( NULL == session_obj )
  {
    rc = VS_EUNEXPECTED;
    VS_TEST_REPORT_FATAL_ON_ERROR ( rc );
    return VS_EUNEXPECTED;
  }

  switch( session_obj->vocoder_type )
  {
    case VS_VOCAMR_MEDIA_ID:
    case VS_VOCAMRWB_MEDIA_ID:
    case VS_VOCEAMR_MEDIA_ID:
    case VS_VOCAMRWB_2_MEDIA_ID:
      rc |= vs_test_vs_set_voc_media_type( session_obj );
      rc |= vs_test_set_voc_enc_rate( session_obj );
      rc |= vs_test_set_voc_dtx_mode_v2( session_obj );
      break;
    
    case VS_VOCEVRCA_MEDIA_ID:
    case VS_VOCEVRCB_MEDIA_ID:
    case VS_VOCEVRCNW_MEDIA_ID:
    case VS_VOCEVRCNW2K_MEDIA_ID:
    case VS_VOCEVRCWB_MEDIA_ID:
    case VS_VOCQCELP13K_MEDIA_ID:
    {
      rc |= vs_test_vs_set_voc_media_type( session_obj );
      rc |= vs_test_set_voc_enc_rate( session_obj );
      rc |= vs_test_set_voc_min_max_rate( session_obj );
      rc |= vs_test_set_voc_dtx_mode_v2( session_obj );
    }
    break;
    
    case VS_VOCEVS_MEDIA_ID:
    {
      rc |= vs_test_vs_set_voc_media_type( session_obj );
      rc |= vs_test_set_voc_operating_mode( session_obj );
      rc |= vs_test_set_max_tx_rx_sr( session_obj );
      rc |= vs_test_vocevs_channel_aware_mode_control( session_obj );
      rc |= vs_test_set_voc_dtx_mode_v2( session_obj );
    }
    break;
    
    case VS_VOCEFR_MEDIA_ID:
    case VS_VOCFR_MEDIA_ID:
    case VS_VOCHR_MEDIA_ID:
     rc |= vs_test_vs_set_voc_media_type( session_obj );
     rc |= vs_test_set_voc_dtx_mode_v2( session_obj );
     break;

    case VS_VOCG711ALAW_V2_MEDIA_ID:
    case VS_VOCG711MULAW_V2_MEDIA_ID:
      rc |= vs_test_vs_set_voc_media_type( session_obj );
      rc |= vs_test_set_voc_dtx_mode_v2( session_obj );
      rc |= vs_test_set_voc_plc( session_obj );
      break;
    
    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "vs_test_set_cached_voc_property(): Invalid media ID=(0x%08x) ", 
              session_obj->vocoder_type );
      rc = APR_EBADPARAM;
      break;
  }

  return rc;
}


/**
 * Queues the apr packet.
 * In case of failure to queue a apr packet, packet shall be
 * freed by the caller.
 */
uint32_t vs_test_queue_work_packet (
  vs_test_work_item_queue_type_t queue_type,
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  vs_test_work_item_t* work_item = NULL;
  apr_list_t* work_queue = NULL;

  switch ( queue_type )
  {
   case VS_TEST_WORK_ITEM_QUEUE_TYPE_NONGATING:
     work_queue = &vs_test_nongating_work_pkt_q;
     break;

   case VS_TEST_WORK_ITEM_QUEUE_TYPE_GATING:
     work_queue = &vs_test_gating_work_pkt_q.cmd_q;
     break;

   default:
     rc = APR_EUNSUPPORTED;
     break;
  }

  for ( ;; )
  {
    /* Get a free command structure. */
    rc = apr_list_remove_head( &vs_test_free_work_pkt_q,
                               ( ( apr_list_node_t** ) &work_item ) );
    if ( rc )
    {
      rc = APR_ENORESOURCE;
      /* No free WORK packet structure is available. */
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_FATAL,
             "vs_test_queue_work_packet(): Ran out of WORK packets, rc=0x%08x, "
             "vs_test_state=%d",   rc, vs_test_is_initialized );
      break;
    }

    work_item->packet = packet;


    /* Add to incoming request work queue */
    rc = apr_list_add_tail( work_queue, &work_item->link );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_test_queue_work_packet() - ERROR: rc=0x%08x", rc );
      /* Add back to vs_test_free_work_pkt_q */
      work_item->packet = NULL;
      ( void ) apr_list_add_tail( &vs_test_free_work_pkt_q, &work_item->link );
    }
    else
    {
      /**
       * Signal appropriate thread.
       */
      vs_test_signal_run();
    }

    break;
  } /* for loop ends. */

  return rc;
}  /* vs_test_queue_work_packet() ends. */

static int32_t vs_test_isr_dispatch_fn (
  aprv2_packet_t* packet,
  void* dispatch_data
)
{
  uint32_t rc = VS_EOK;

  rc = vs_test_queue_work_packet( VS_TEST_WORK_ITEM_QUEUE_TYPE_NONGATING,packet );
  VS_TEST_REPORT_FATAL_ON_ERROR(rc);

  rc = __aprv2_cmd_free( vs_test_apr_handle, packet );
  VS_TEST_REPORT_FATAL_ON_ERROR(rc);

  return VS_EOK;
}

/****************************************************************************
 * VS_TEST CMDs/EVENTs PACKET PREPARE/DISPATCHER/FREE ROUTINES                  *
 ****************************************************************************/

/**
 * This is a common routine facilitating to prepare and
 * dispatches a CMD PKT.
 */
 uint32_t vs_test_prepare_and_dispatch_cmd_packet (
  void* session_context,
  uint32_t cmd_id,
  void* params,
  uint32_t size
)
{
  uint32_t rc = APR_EOK;
  aprv2_packet_t* packet = NULL;
  void* payload;

  for ( ;; )
  {
    rc = __aprv2_cmd_alloc_ext( vs_test_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                APR_NULL_V, APR_NULL_V,
                                APR_NULL_V, APR_NULL_V, 
                                (uint32_t)session_context, cmd_id,
                                size, &packet );

    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VS Testclient: APR packet allocation failed with rc=[0x%08x]", rc );
      break;
    }

    if ( ( size > 0 ) && ( params != NULL ) )
    {
      payload = APRV2_PKT_GET_PAYLOAD( void, packet);
      mmstd_memcpy( payload, size, params, size );
    }

    /* Queue the command packet for processing. */
    rc = vs_test_queue_work_packet( VS_TEST_WORK_ITEM_QUEUE_TYPE_NONGATING,
                                    packet );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_test_prepare_and_dispatch_cmd_packet() - cmd pkt queuing failed. "
             "rc=(0x%08x)", rc );
      rc = __aprv2_cmd_free( vs_test_apr_handle, packet );
      VS_TEST_REPORT_FATAL_ON_ERROR(rc);
    }
    else
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "vs_test_prepare_and_dispatch_cmd_packet() cmd pkt queued with "
             "cmd_id=(0x%08x)", cmd_id );
    }

    break;
  }

  return rc;
}


/****************************************************************************
 * VS_TEST OBJECT CREATION, DESTRUCTION AND INITIALISATION ROUTINES         *
 ****************************************************************************/

static uint32_t vs_test_gating_control_destroy (
  vs_test_gating_control_t* p_ctrl
)
{
  if ( p_ctrl == NULL )
  {
    return APR_EBADPARAM;
  }

  ( void ) apr_list_destroy( &p_ctrl->cmd_q );

  return APR_EOK;
} 

static uint32_t vs_test_mem_alloc_object (
  uint32_t size,
  vs_test_object_t** ret_object
)
{
  int32_t rc;
  vs_test_object_t* vs_test_obj;
  apr_objmgr_object_t* objmgr_obj;

  if ( ret_object == NULL )
  {
    return APR_EBADPARAM;
  }

  { /* Allocate memory for the VS_TEST object. */
    vs_test_obj = apr_memmgr_malloc( &vs_test_heapmgr, size );
    if ( vs_test_obj == NULL )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_test_mem_alloc_object(): Out of memory, requested size (%d)", size );
      return APR_ENORESOURCE;
    }

    /* Allocate a new handle for the vs test object. */
    rc = apr_objmgr_alloc_object( &vs_test_objmgr, &objmgr_obj );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
             "vs_test_mem_alloc_object(): Out of objects, rc = (0x%08X)", rc );
      apr_memmgr_free( &vs_test_heapmgr, vs_test_obj );
      return APR_ENORESOURCE;
    }

    /* Use the custom object type. */
    objmgr_obj->any.ptr = vs_test_obj;

    /* Initialize the base vs test object header. */
    vs_test_obj->header.handle = objmgr_obj->handle;
    vs_test_obj->header.type = VS_TEST_OBJECT_TYPE_ENUM_UNINITIALIZED;
  }

  *ret_object = vs_test_obj;

  return APR_EOK;
}

static uint32_t vs_test_mem_free_object (
  vs_test_object_t* object
)
{
  if ( object == NULL )
  {
    return APR_EBADPARAM;
  }

  /* Free the object memory and object handle. */
  ( void ) apr_objmgr_free_object( &vs_test_objmgr, object->header.handle );
  apr_memmgr_free( &vs_test_heapmgr, object );

  return APR_EOK;
}

static void vs_test_pktexg_timer_cb (
  void* session_context
)
{
  uint32_t rc = APR_EOK;
  vs_test_session_object_t* session_obj = (vs_test_session_object_t*)session_context;

  if ( NULL == session_obj )
  {
    VS_TEST_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return;
  }

  rc = apr_timer_start( session_obj->pktexg_timer, 20000000 );
  
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "VS_TEST: VSID=(0x%08x): VS_TEST_INTERNAL_EVENT_LOOPBACK_BUFFER queued",
         session_obj->vsid );
  rc = vs_test_prepare_and_dispatch_cmd_packet( session_context,VS_TEST_INTERNAL_EVENT_LOOPBACK_BUFFER, NULL, 0 );

  return ;
}

static uint32_t vs_test_create_session_object ( 
  vs_test_session_object_t** ret_session_obj )
{
  uint32_t rc = APR_EOK;
  vs_test_session_object_t* session_obj = NULL;

  if ( ret_session_obj == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = vs_test_mem_alloc_object( sizeof( vs_test_session_object_t ),
                             ( ( vs_test_object_t** ) &session_obj ) );
  if ( rc )
  {
    *ret_session_obj = NULL;
    return APR_ENORESOURCE;
  }

  { /* Initialize the simple job object. */
    session_obj->header.type = VS_TEST_OBJECT_TYPE_ENUM_SESSION;
    rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &session_obj->data_lock );
    VS_TEST_PANIC_ON_ERROR(rc);

    rc = apr_timer_create( &session_obj->pktexg_timer, vs_test_pktexg_timer_cb, session_obj );

    session_obj->vsid = VS_TEST_VSID1;
    session_obj->vocoder_type = VS_TEST_VOCODER_ID_UNDEFINED_V;
    session_obj->dtx_mode = FALSE;
    session_obj->codec_mode = VS_TEST_PROPERTY_NOT_SET_UINT8;
    session_obj->bandwidth = VS_TEST_PROPERTY_NOT_SET_UINT8;
    session_obj->channel_aware_mode_enabled = VS_TEST_PROPERTY_NOT_SET_UINT8;
    session_obj->max_tx_sr = VS_TEST_PROPERTY_NOT_SET_UINT8;
    session_obj->max_rx_sr = VS_TEST_PROPERTY_NOT_SET_UINT8;
    session_obj->fec_offset = VS_TEST_PROPERTY_NOT_SET_UINT8;
    session_obj->fer_rate = VS_TEST_PROPERTY_NOT_SET_UINT8;
    session_obj->avg_enc_rate = VS_TEST_PROPERTY_NOT_SET_UINT8;
    session_obj->min_rate = VS_TEST_PROPERTY_NOT_SET_UINT8;
    session_obj->max_rate = VS_TEST_PROPERTY_NOT_SET_UINT8;

    session_obj->vs_handle = APR_NULL_V;
    session_obj->vs_read_buf = NULL;
    session_obj->vs_write_buf = NULL;
    session_obj->is_vs_ready = FALSE;
  }

  *ret_session_obj = session_obj;
  return APR_EOK;
}

static uint32_t vs_test_create_simple_job_object (
  uint32_t parentjob_handle,
  vs_test_simple_job_object_t** ret_job_obj
)
{
  int32_t rc;
  vs_test_simple_job_object_t* vs_test_obj = NULL;

  if ( ret_job_obj == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = vs_test_mem_alloc_object( sizeof( vs_test_simple_job_object_t ),
                             ( ( vs_test_object_t** ) &vs_test_obj ) );
  if ( rc )
  {
    *ret_job_obj = NULL;
    return APR_ENORESOURCE;
  }

  { /* Initialize the simple job object. */
    vs_test_obj->header.type = VS_TEST_OBJECT_TYPE_ENUM_SIMPLE_JOB;
    vs_test_obj->context_handle = parentjob_handle;
    vs_test_obj->is_completed = 0;
  }

  *ret_job_obj = vs_test_obj;

  return APR_EOK;
}


/****************************************************************************
 * VS_TEST VS SESSION ROUTINES                                                  *
 ****************************************************************************/

static void vs_test_log_event_info(
  uint32_t event_id
)
{
  switch( event_id )
  {
   case VS_COMMON_EVENT_CMD_RESPONSE:
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "VS_TEST: VS_COMMON_EVENT_CMD_RESPONSE recieved" );
    }
    break;

   case VS_VOC_EVENT_NOT_READY:
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "VS_TEST: VS_VOC_EVENT_NOT_READY recieved" );
    }
    break;

   case VS_VOC_EVENT_READY:
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "VS_TEST: VS_VOC_EVENT_READY recieved" );
    }
    break;

   case VS_PKTEXG_EVENT_ENCODER_BUFFER_AVAILABLE:
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "VS_TEST: VS_PKTEXG_EVENT_ENCODER_BUFFER_AVAILABLE recieved" );
    }
    break;

   case VS_PKTEXG_EVENT_DECODER_BUFFER_REQUEST:
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "VS_TEST: VS_PKTEXG_EVENT_DECODER_BUFFER_REQUEST recieved" );
    }
    break;

   default:
     break;
  }

  return;
}

static uint32_t vs_test_vs_event_cb(
 uint32_t event_id,
 void* params,
 uint32_t size,
 void* session_context
)
{
  uint32_t rc = APR_EOK;

  if ( vs_test_is_initialized == FALSE ) return APR_EOK;

  switch ( event_id )
  {
   case VS_COMMON_EVENT_CMD_RESPONSE:
   case VS_VOC_EVENT_NOT_READY:
   case VS_VOC_EVENT_READY:
   case VS_PKTEXG_EVENT_ENCODER_BUFFER_AVAILABLE:
   case VS_PKTEXG_EVENT_DECODER_BUFFER_REQUEST:
   {
      (void) vs_test_log_event_info( event_id );
      rc = vs_test_prepare_and_dispatch_cmd_packet( session_context, event_id,
                                                  params, size );
   }
   break;

   default:
     MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
            "vs_test_vs_event_cb(): Unsupported event (%d)", event_id );
     rc = APR_EFAILED;
  }

  return rc;
} 

static uint32_t vs_test_vs_return_encoder_buffer (
  vs_test_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_pktexg_cmd_return_encoder_buffer_t returnEncBufCmd;

  if ( NULL == session_obj->vs_read_buf ) return VS_EOK;

  returnEncBufCmd.handle = session_obj->vs_handle;
  returnEncBufCmd.buffer = (void*)session_obj->vs_read_buf;

  rc = vs_call( VS_PKTEXG_CMD_RETURN_ENCODER_BUFFER, (void*)&returnEncBufCmd, sizeof( returnEncBufCmd ) );
  if ( rc )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_test_vs_return_encoder_buffer: Failed to return read buffer, buffer[0x%08x], rc[0x%08x]",
           session_obj->vs_read_buf, rc );
  }

  session_obj->vs_read_buf = NULL;
  return rc;
}

static uint32_t vs_test_vs_get_encoder_buffer (
  vs_test_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_pktexg_buffer_t* tempReadBuf = NULL;
  vs_pktexg_cmd_get_encoder_buffer_t read_buf_cmd;

  while( TRUE )
  {
    if ( APR_NULL_V == session_obj->vs_handle ) break;

    read_buf_cmd.handle = session_obj->vs_handle;
    read_buf_cmd.buffer = (void*)&tempReadBuf;

    rc = vs_call( VS_PKTEXG_CMD_GET_ENCODER_BUFFER, ( void* )&read_buf_cmd, sizeof( read_buf_cmd ) );
    if ( rc || ( NULL == tempReadBuf ) ) break;

    if( NULL != session_obj->vs_read_buf )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_test_vs_get_encoder_buffer(): returning previous encoder buffer" );
      ( void ) vs_test_vs_return_encoder_buffer( session_obj );
    }

    session_obj->vs_read_buf = tempReadBuf;
    tempReadBuf = NULL;
  }

  if( NULL == session_obj->vs_read_buf )
  {
     MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
       "vs_test_vs_get_encoder_buffer(): Failed to get read encoder buffer, rc[0x%08x]", rc );
  }

  return rc;
}

static uint32_t vs_test_vs_get_decoder_buffer (
  vs_test_session_object_t* session_obj
)
{
  uint32_t rc = VS_EFAILED;
  vs_pktexg_cmd_get_decoder_buffer_t getDecBufCmd;

  if ( APR_NULL_V == session_obj->vs_handle ) return rc;

  getDecBufCmd.handle = session_obj->vs_handle;
  getDecBufCmd.buffer = (void*)&session_obj->vs_write_buf;

  rc = vs_call( VS_PKTEXG_CMD_GET_DECODER_BUFFER, (void*)&getDecBufCmd, sizeof( getDecBufCmd ) );
  if ( rc )
  {
    session_obj->vs_write_buf = NULL;
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_test_vs_get_decoder_buffer(): Failed to get vs_write_buf,  rc[0x%08x]", rc );
  }

  return rc;
}

static uint32_t vs_test_vs_put_decoder_buffer (
  vs_test_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_pktexg_cmd_put_decoder_buffer_t putDecBufCmd;

  putDecBufCmd.handle = session_obj->vs_handle;
  putDecBufCmd.buffer = session_obj->vs_write_buf;

  rc = vs_call( VS_PKTEXG_CMD_PUT_DECODER_BUFFER, ( void* )&putDecBufCmd, sizeof( putDecBufCmd ) );
  if ( rc )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "vs_test_vs_put_decoder_buffer(): Failed to return vs_write_buf[0x%08x], rc[0x%08x]", session_obj->vs_write_buf, rc );
  }

  session_obj->vs_write_buf = NULL;
  return APR_EOK;
}

static uint32_t vs_test_process_loopback(aprv2_packet_t* packet)
{
  uint32_t rc = APR_EOK;
  vs_test_session_object_t* session_obj = NULL;
  session_obj = (vs_test_session_object_t*) packet->token;
  vs_pktexg_cmd_flush_buffers_t flushCmd;

  if ( NULL == session_obj )
  {
    VS_TEST_REPORT_FATAL_ON_ERROR(rc);
	return VS_EUNSUPPORTED;
  }

  for( ;; )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "vs_testclient: VSID=(0x%08x): Reading encoder buffer", session_obj->vsid );
    rc = vs_test_vs_get_encoder_buffer ( session_obj );
    if( NULL == session_obj->vs_read_buf )
    {
      VS_TEST_REPORT_FATAL_ON_ERROR( APR_EUNEXPECTED );
      break;
    }

    rc = vs_test_vs_get_decoder_buffer( session_obj );
    if( NULL == session_obj->vs_write_buf )
    {
      VS_TEST_REPORT_FATAL_ON_ERROR( APR_EUNEXPECTED );
      break;
    }

    /*looping back the packet */
    session_obj->vs_write_buf->media_id = session_obj->vs_read_buf->media_id;
    session_obj->vs_write_buf->frame_size = session_obj->vs_read_buf->frame_size;
    session_obj->vs_write_buf->status_mask = session_obj->vs_read_buf->status_mask;
    session_obj->vs_write_buf->timestamp_us = session_obj->vs_read_buf->timestamp_us;
    session_obj->vs_write_buf->minor_version = session_obj->vs_read_buf->minor_version;
    mmstd_memcpy( (uint8_t*)session_obj->vs_write_buf+ sizeof(vs_pktexg_buffer_t), session_obj->vs_write_buf->frame_size,
                   (uint8_t*)session_obj->vs_read_buf + sizeof(vs_pktexg_buffer_t), session_obj->vs_read_buf->frame_size );

    rc = vs_test_vs_return_encoder_buffer(session_obj);
    VS_TEST_REPORT_FATAL_ON_ERROR(rc);

    // Flush stale downlink data
    flushCmd.handle = session_obj->vs_handle;
    flushCmd.enc_flush = FALSE; flushCmd.dec_flush = TRUE;
    rc = vs_call( VS_PKTEXG_CMD_FLUSH_BUFFERS, &flushCmd, sizeof( flushCmd ) );
    VS_TEST_REPORT_FATAL_ON_ERROR(rc);

    rc = vs_test_vs_put_decoder_buffer( session_obj );
    VS_TEST_REPORT_FATAL_ON_ERROR(rc);

    break;
  }

  rc = __aprv2_cmd_free( vs_test_apr_handle, packet );
  VS_TEST_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}

static uint32_t vs_test_vs_close_session (
  vs_test_session_object_t* session_obj,
  void* client_context
)
{
  uint32_t rc = APR_EOK;
  vs_voc_cmd_close_t close_cmd;

  for ( ;; )
  {
    if ( session_obj->vs_handle == APR_NULL_V )
    {
      rc = APR_EOK;
      break;
    }

    close_cmd.handle = session_obj->vs_handle;
    close_cmd.token= client_context;

    rc = vs_call( VS_VOC_CMD_CLOSE, (void*)&close_cmd, sizeof( close_cmd ) );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_test_vs_close_session(): Failed to close VS session, " 
             "handle = (0x%08x), rc = (0x%08x)", session_obj->vs_handle, rc );
    }
    else
    {
      session_obj->vs_handle = APR_NULL_V;
      rc = APR_EPENDING;
    }

    break;
  }

  return rc;
}


static uint32_t vs_test_vs_open_session (
 vs_test_session_object_t* session_obj
)
{
  uint32_t rc;
  vs_voc_cmd_open_t open_cmd;

  for ( ;; )
  {
    /* Open VS session. */
    open_cmd.ret_handle = &session_obj->vs_handle;
    open_cmd.vsid = session_obj->vsid;
    open_cmd.network_id = VS_COMMON_NETWORK_ID_GSM;
    open_cmd.session_context = ( void* )session_obj;
    open_cmd.event_cb = vs_test_vs_event_cb;

    rc = vs_call( VS_VOC_CMD_OPEN, (void*)&open_cmd, sizeof( open_cmd ) );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_test_vs_open_session(): failed to open VS session, "
             "vsid = (0x%08x), rc = (0x%08x)", open_cmd.vsid, rc );
    }

    break;
  }

  return rc;
}


static uint32_t vs_test_vs_disable_vocoder (
  vs_test_session_object_t* session_obj,
  void* client_context
)
{
  uint32_t rc = APR_EOK;
  vs_voc_cmd_disable_t vs_disable_cmd;

  for ( ;; )
  {
    if ( session_obj->vs_handle == APR_NULL_V )
    {
      rc = APR_EOK;
      break;
    }

    vs_disable_cmd.handle = session_obj->vs_handle;
    vs_disable_cmd.token = client_context;
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "VS_TEST: VS_VOC_CMD_DISABLE: vocoder=(0x%08x)", session_obj->vocoder_type );
    rc = vs_call( VS_VOC_CMD_DISABLE, &vs_disable_cmd, sizeof(vs_disable_cmd) );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "VS_TEST: Failed to close VS session, handle[0x%08x], rc[0x%08x]", 
             session_obj->vs_handle, rc );
    }
    else
    {
      rc = APR_EPENDING;
    }
   
    break;
  }

  return rc;
}


static uint32_t vs_test_vs_enable_vocoder (
  vs_test_session_object_t* session_obj,
  void* client_context
)
{
  uint32_t rc = APR_EOK;
  vs_voc_cmd_enable_t vs_enable_cmd;

  for ( ;; )
  {
    if ( session_obj->vs_handle == APR_NULL_V )
    {
      rc = APR_EOK;
      break;
    }

    vs_enable_cmd.handle = session_obj->vs_handle;
    vs_enable_cmd.token= ( void* ) client_context;
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "VS_TEST: VS_VOC_CMD_ENABLE(): vocoder=(0x%08x)", session_obj->vocoder_type );
    rc = vs_call( VS_VOC_CMD_ENABLE, &vs_enable_cmd,
                  sizeof( vs_enable_cmd ) );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "vs_test_vs_enable_vocoder(): Failed to enable VS session, " 
             "handle = (0x%08x), rc = (0x%08x)", session_obj->vs_handle, rc );
    }
    else
    {
      rc = APR_EPENDING;
    }

    break;
  }/* For loop ends here. */

  return rc;
}

static uint32_t vs_test_vs_reset_vocoder (
vs_test_session_object_t* session_obj
)
{
   uint32_t rc = APR_EOK;
   if ( NULL == session_obj  )
   {
      return APR_EBADPARAM;
   }

   session_obj->vocoder_type = VS_TEST_VOCODER_ID_UNDEFINED_V;
   session_obj->dtx_mode = FALSE;
   session_obj->codec_mode = VS_TEST_PROPERTY_NOT_SET_UINT8;
   session_obj->bandwidth = VS_TEST_PROPERTY_NOT_SET_UINT8;
   session_obj->channel_aware_mode_enabled = VS_TEST_PROPERTY_NOT_SET_UINT8;
   session_obj->max_tx_sr = VS_TEST_PROPERTY_NOT_SET_UINT8;
   session_obj->max_rx_sr = VS_TEST_PROPERTY_NOT_SET_UINT8;
   session_obj->fec_offset = VS_TEST_PROPERTY_NOT_SET_UINT8;
   session_obj->fer_rate = VS_TEST_PROPERTY_NOT_SET_UINT8;
   session_obj->avg_enc_rate = VS_TEST_PROPERTY_NOT_SET_UINT8;
   session_obj->min_rate = VS_TEST_PROPERTY_NOT_SET_UINT8;
   session_obj->max_rate = VS_TEST_PROPERTY_NOT_SET_UINT8;

   session_obj->vs_read_buf = NULL;
   session_obj->vs_write_buf = NULL;

   MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW,"vs_test_vs_reset_vocoder(): vocoder has been reset");
   return rc;

}

/****************************************************************************
 * VS_TEST CMDs/EVENTs HANDLING ROUTINES                                        *
 ****************************************************************************/

static uint32_t vs_test_process_vs_cmd_response_event( 
 aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  vs_test_session_object_t* session_obj = NULL;
  vs_test_simple_job_object_t* simple_obj = NULL;
  vs_common_event_cmd_response_t* evt_params = NULL;

  for ( ;; )
  {
	if( packet == NULL )
    {
      VS_TEST_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }
	
    session_obj = ( vs_test_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      VS_TEST_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    evt_params = APRV2_PKT_GET_PAYLOAD( vs_common_event_cmd_response_t, packet );
    if( evt_params == NULL )
    {
      VS_TEST_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "vs_test_process_vs_cmd_response_event(): cmd response event recieved for "
           "VS cmd_id=(0x%08x), client_context=(0x%08x), status=(0x%08X)",
           evt_params->cmd_id, evt_params->token, evt_params->status );

    simple_obj = ( vs_test_simple_job_object_t* ) evt_params->token;
    if ( simple_obj == NULL ) break;

      simple_obj->is_completed = TRUE;
      simple_obj->status = evt_params->status;

    break;
  }

  rc = __aprv2_cmd_free( vs_test_apr_handle, packet );
  VS_TEST_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}


static uint32_t vs_test_process_vs_ready_event( 
 aprv2_packet_t* event_pkt
)
{
  uint32_t rc = APR_EOK;
  vs_test_session_object_t* session_obj = NULL;

  for ( ;; )
  {
    if( NULL == event_pkt )
    {
      VS_TEST_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }
    
    session_obj = ( vs_test_session_object_t* ) event_pkt->token;
    if( NULL == session_obj )
    {
      VS_TEST_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
         "vs_test_process_vs_ready_event(): vocoder session ready for packet "
         "exchange" );

    VS_TEST_ACQUIRE_LOCK( session_obj->data_lock );

    session_obj->is_vs_ready = TRUE;

    VS_TEST_RELEASE_LOCK( session_obj->data_lock );

    rc = apr_timer_start( session_obj->pktexg_timer, 20000000 );

    break;
  }

  rc = __aprv2_cmd_free( vs_test_apr_handle, event_pkt );
  VS_TEST_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}


static uint32_t vs_test_process_vs_not_ready_event( 
 aprv2_packet_t* event_pkt
)
{
  uint32_t rc = APR_EOK;
  vs_test_session_object_t* session_obj = NULL;
  vs_pktexg_cmd_flush_buffers_t flushCmd;

  for ( ;; )
  {
    if( event_pkt == NULL )
    {
      VS_TEST_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }
  
    session_obj = ( vs_test_session_object_t* ) event_pkt->token;
    if( session_obj == NULL )
    {
      VS_TEST_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    VS_TEST_ACQUIRE_LOCK( session_obj->data_lock );

    if( APR_NULL_V != session_obj->vs_handle )
    {
      flushCmd.handle = session_obj->vs_handle;
      flushCmd.enc_flush = TRUE;
      flushCmd.dec_flush = TRUE;
      rc = vs_call( VS_PKTEXG_CMD_FLUSH_BUFFERS, &flushCmd, sizeof( flushCmd ) );
      VS_TEST_REPORT_FATAL_ON_ERROR(rc);
    }

    session_obj->is_vs_ready = FALSE;
    VS_TEST_RELEASE_LOCK( session_obj->data_lock );

    break;
  }

  rc = __aprv2_cmd_free( vs_test_apr_handle, event_pkt );
  VS_TEST_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}

static uint32_t vs_test_process_loopback_start( 
 vs_test_gating_control_t* ctrl 
)
{
  uint32_t rc = APR_EOK;
  aprv2_packet_t* cmd_pkt = NULL;
  vs_test_session_object_t* session_obj = vs_test_session_obj;
  vs_test_simple_job_object_t* simple_obj = NULL;

  for ( ;; )
  {
    cmd_pkt = ctrl->packet;
    if( cmd_pkt == NULL )
    {
      VS_TEST_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    VS_TEST_ACQUIRE_LOCK( session_obj->data_lock );
    if ( ctrl->state == VS_TEST_GATING_CMD_STATE_EXECUTE )
    {
      rc = vs_test_set_cached_voc_property ( session_obj );
      VS_TEST_REPORT_FATAL_ON_ERROR(rc)

     /* Create the Simple job object to track CVD setup. */
     rc = vs_test_create_simple_job_object( session_obj->header.handle,
            ( vs_test_simple_job_object_t** ) &ctrl->rootjob_obj );
     if ( APR_EOK != rc )
     {
       MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
              "vs_test_process_loopback_start() - Failed to create simple job "
              "rc = (0X%08X).", rc );
       break;
     }
     simple_obj = &ctrl->rootjob_obj->simple_job;

     rc = vs_test_vs_enable_vocoder( session_obj, simple_obj );
    }
    else
    {
      simple_obj = &ctrl->rootjob_obj->simple_job;
      if( simple_obj->is_completed != TRUE )
      {
        rc = APR_EPENDING;
        break;
      }
    }

    if ( VS_EOK == rc )
    {
      ( void ) vs_test_mem_free_object ( ( vs_test_object_t*) simple_obj );
    }

    break;
  }

  if( session_obj != NULL )
  {
    VS_TEST_RELEASE_LOCK( session_obj->data_lock );
  }

  return rc;
}


static uint32_t vs_test_process_loopback_stop ( 
 vs_test_gating_control_t* ctrl 
)
{
  uint32_t rc = APR_EOK;
  aprv2_packet_t* cmd_pkt = NULL;
  vs_test_session_object_t* session_obj = vs_test_session_obj;
  vs_test_simple_job_object_t* simple_obj = NULL;

  for ( ;; )
  {
    cmd_pkt = ctrl->packet;
    if( cmd_pkt == NULL )
    {
      VS_TEST_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    VS_TEST_ACQUIRE_LOCK( session_obj->data_lock );

    if ( ctrl->state == VS_TEST_GATING_CMD_STATE_EXECUTE )
    {
      /* Create the Simple job object to track CVD setup. */
      rc = vs_test_create_simple_job_object( session_obj->header.handle,
             ( vs_test_simple_job_object_t** ) &ctrl->rootjob_obj );
      simple_obj= &ctrl->rootjob_obj->simple_job;

      rc = vs_test_vs_disable_vocoder( session_obj, simple_obj );
    }
    else
    {
      simple_obj= &ctrl->rootjob_obj->simple_job;
      if( simple_obj->is_completed != TRUE )
      {
        rc = APR_EPENDING;
        break;
      }
    }

    if ( APR_EOK == rc )
    {
      rc = apr_timer_stop( session_obj->pktexg_timer );
      ( void ) vs_test_mem_free_object ( ( vs_test_object_t*) simple_obj );
      ( void )vs_test_vs_reset_vocoder( session_obj );
    }

    break;
  }

  if( session_obj != NULL )
  {
    VS_TEST_RELEASE_LOCK( session_obj->data_lock );
  }

  return rc;
}

/****************************************************************************
 * NONGATING REQUEST( CMDs/EVENTs ) PROCESSING FUNCTIONS
 ****************************************************************************/

static void vs_test_task_process_nongating_work_items ( void )
{
  uint32_t rc = APR_EOK;
  vs_test_work_item_t* work_item = NULL;
  aprv2_packet_t* packet = NULL;

  while( apr_list_remove_head( &vs_test_nongating_work_pkt_q,
                               ( ( apr_list_node_t** ) &work_item ) ) == APR_EOK )
  {
    packet = work_item->packet;

    /* Add back to vs_free_work_pkt_q. */
    work_item->packet = NULL;
    ( void ) apr_list_add_tail( &vs_test_free_work_pkt_q, &work_item->link );

    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "vs_test_task_process_nongating_work_items(): Processing "
           "request_id=(0X%08X)", packet->opcode );

    switch( packet->opcode )
    {
     /**
      * Handling routine for nongating work-items should take of release the
      * memory allocated for the CMD/EVENT packets.
      */
     case VS_VOC_EVENT_READY:
       rc = vs_test_process_vs_ready_event( packet );
       break;
 
     case VS_VOC_EVENT_NOT_READY:
       rc = vs_test_process_vs_not_ready_event( packet );
       break;

     case VS_COMMON_EVENT_CMD_RESPONSE:
       rc = vs_test_process_vs_cmd_response_event( packet );
       break;

     case VS_TEST_INTERNAL_EVENT_LOOPBACK_BUFFER:
       rc = vs_test_process_loopback( packet);
       break;

     default:
       /* Add remaining work items to the gating work queue. */
       rc = vs_test_queue_work_packet ( VS_TEST_WORK_ITEM_QUEUE_TYPE_GATING,
                                         packet );
       if ( rc )
       {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "VS TEST: NON-GATING: cmd pkt queuing failed rc[0x%08x]", rc );
          rc = __aprv2_cmd_free( vs_test_apr_handle, packet );
         VS_TEST_REPORT_FATAL_ON_ERROR(rc);
       }
       break;
    }
  }

  return;
}

/****************************************************************************
 * GATING REQUEST( CMDs/EVENTs ) PROCESSING FUNCTIONS
 ****************************************************************************/
 
static void vs_test_task_process_gating_work_items ( void )
{
  uint32_t rc = APR_EOK;
  vs_test_work_item_t* work_item;
  vs_test_gating_control_t* ctrl = &vs_test_gating_work_pkt_q;

  for ( ;; )
  {
    switch ( ctrl->state )
    {
     case VS_TEST_GATING_CMD_STATE_FETCH:
       {
          /* Fetch the next gating command to execute. */
          rc = apr_list_remove_head( &ctrl->cmd_q,
                                     ( ( apr_list_node_t** ) &work_item ) );
          if ( rc ) return;

          if ( work_item->packet == NULL ) 
          {
            VS_TEST_PANIC_ON_ERROR ( APR_EUNEXPECTED );
          }

          MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
               "vs_test_task_process_gating_work_items(): "
               "VS_GATING_CMD_STATE_ENUM_FETCH" );

          ctrl->packet = work_item->packet;
          ctrl->state = VS_TEST_GATING_CMD_STATE_EXECUTE;


          /* Add the vs_work_item_t back to vs_free_work_pkt_q. */
          work_item->packet = NULL;
          ( void ) apr_list_add_tail( &vs_test_free_work_pkt_q, &work_item->link );
       }
       break;

     case VS_TEST_GATING_CMD_STATE_EXECUTE:
     case VS_TEST_GATING_CMD_STATE_CONTINUE:
       {
         /**
          * For Supported request_id, handler should take care of releasing 
          * memory allocated for packets.
          */
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
                 "vs_test_task_process_gating_work_items(): OPCODE=(0x%08x)",
                 ctrl->packet->opcode );

         switch ( ctrl->packet->opcode )
         {
          case VS_ITEST_CMD_LOOPBACK_START:
            rc = vs_test_process_loopback_start( ctrl );
            break;

          case VS_ITEST_CMD_LOOPBACK_STOP:
            rc = vs_test_process_loopback_stop( ctrl );
            break;

          case VS_ITEST_CMD_LOOPBACK_SET_VOC_PROPERTY:
            rc = vs_test_set_voc_property_cmd_ctrl( ctrl );
            break;

          default:
            {
              MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                     "vs_test_task_process_gating_work_items(): Unsupported "
                     "OPCODE=(0X%08X)", ctrl->packet->opcode );
              /* set to VS_EOK to fetch the next command in queue. */
              rc = APR_EOK;
            }
           break;
         }

         /* Evaluate the pending command completion status. */
         switch ( rc )
         {
          case APR_EOK:
            {
              MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                     "vs_test_task_process_gating_work_items(): request_id = "
                     "(0X%08X) processed successfully ", ctrl->packet->opcode );

              rc = __aprv2_cmd_free( vs_test_apr_handle, ctrl->packet );
              VS_TEST_REPORT_FATAL_ON_ERROR(rc)

              ctrl->packet = NULL;
              /* The current command is finished so fetch the next command. */
              ctrl->state = VS_TEST_GATING_CMD_STATE_FETCH;
            }
            break;
         
          case APR_EPENDING:
            /**
             * Assuming the current pending command control routine returns
             * APR_EPENDING the overall progress stalls until one or more
             * external events or responses are received.
             */
            ctrl->state = VS_TEST_GATING_CMD_STATE_CONTINUE;
            /**
             * Return from here so as to avoid unecessary looping till reponse
             * is recived.
             */
            return;
         
          default:
            VS_TEST_PANIC_ON_ERROR( APR_EUNEXPECTED );
            break;
         }
       }
       break;

     default:
      VS_TEST_PANIC_ON_ERROR( rc );
      break;
    }/* switch case ends. */
  }/* for loop ends. */

  return;
}

/****************************************************************************
 * VS_TEST TASK ROUTINES                                                        *
 ****************************************************************************/

 void vs_test_signal_run ( void )
{
  apr_event_signal( vs_test_work_event );
}

static int32_t vs_test_run ( void )
{
  vs_test_task_process_nongating_work_items( );
  vs_test_task_process_gating_work_items( );

  return APR_EOK;
}

static int32_t vs_test_worker_thread_fn (
  void* param
)
{
  int32_t rc;

  apr_event_create( &vs_test_work_event );
  apr_event_signal( vs_test_control_event );

  for ( ;; )
  {
    rc = apr_event_wait( vs_test_work_event );
    if ( rc ) break;

    vs_test_run( );
  }

  apr_event_destroy( vs_test_work_event );
  apr_event_signal( vs_test_control_event );

  return APR_EOK;
}


/****************************************************************************
 * VS_TEST BOOT-UP and POWER-DOWN ROUTINES                                      *
 ****************************************************************************/

static uint32_t vs_test_gating_control_init (
  vs_test_gating_control_t* p_ctrl
)
{
  uint32_t rc = APR_EOK;

  if ( p_ctrl == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = apr_list_init_v2( &p_ctrl->cmd_q,
                         vs_test_thread_lock_fn, vs_test_thread_unlock_fn );
  if ( rc )
  {
    return APR_EFAILED;
  }

  p_ctrl->state = VS_TEST_GATING_CMD_STATE_FETCH;
  p_ctrl->packet = NULL;
  p_ctrl->rootjob_obj = NULL;

  return APR_EOK;
}  /* end of vs_test_gating_control_init () */


/****************************************************************************
 * Init/De-init                                                             *
 ****************************************************************************/

static int32_t vs_test_init ( void )
{
  uint32_t rc = APR_EOK;
  uint32_t index;

  if ( TRUE == vs_test_is_initialized )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
          "vs_test_init(): Already Initialized " );
    return APR_EALREADY;
  }

  MSG( MSG_SSID_DFLT, MSG_LEGACY_LOW, "====== vs_test_init(): ======" );

  {  /* Initialize the locks. */
    rc = apr_lock_create( APR_LOCK_TYPE_INTERRUPT, &vs_test_int_lock );
    rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &vs_test_thread_lock );
    apr_event_create( &vs_test_control_event );
  }

  { /* Initialize the custom heap. */
    apr_memmgr_init_heap( &vs_test_heapmgr, ( ( void* ) &vs_test_heap_pool ),
                          sizeof( vs_test_heap_pool ), NULL, NULL );
  }

  { /* Initialize the object manager. */
    apr_objmgr_setup_params_t params;
  
    params.table = vs_test_object_table;
    params.total_bits = VS_TEST_HANDLE_TOTAL_BITS_V;
    params.index_bits = VS_TEST_HANDLE_INDEX_BITS_V;
    params.lock_fn = vs_test_int_lock_fn;
    params.unlock_fn = vs_test_int_unlock_fn;
    rc = apr_objmgr_construct( &vs_test_objmgr, &params );
  }

  { /* Initialize free and nongating work pkt queues. */
    rc = apr_list_init_v2( &vs_test_free_work_pkt_q, 
                           vs_test_int_lock_fn, vs_test_int_unlock_fn );
    for ( index = 0; index < VS_TEST_NUM_WORK_PKTS_V; ++index )
    {
      ( void ) apr_list_init_node( ( apr_list_node_t* ) &vs_test_work_pkts[index] );
      vs_test_work_pkts[index].packet = NULL;
      rc = apr_list_add_tail( &vs_test_free_work_pkt_q,
                              ( ( apr_list_node_t* ) &vs_test_work_pkts[index] ) );
    }
    rc = apr_list_init_v2( &vs_test_nongating_work_pkt_q,
                           vs_test_int_lock_fn, vs_test_int_unlock_fn );
  }

  { /* Initialize gating work pkt queue. */
    rc = vs_test_gating_control_init( &vs_test_gating_work_pkt_q );
  }

  { /* Initialize the global session lock. */
    rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &vs_test_global_lock );
    VS_TEST_PANIC_ON_ERROR ( rc );
  }

  rc = apr_thread_create( &vs_test_thread, "VS_TEST", 97,
                          vs_test_task_stack, VS_TEST_STACK_SIZE_V,
                          vs_test_worker_thread_fn , NULL );
  VS_TEST_PANIC_ON_ERROR( rc );

  apr_event_wait( vs_test_control_event );

  vs_test_is_initialized = TRUE;

  rc = __aprv2_cmd_register2(
         &vs_test_apr_handle, vs_test_my_dns, sizeof( vs_test_my_dns ), 0,
         vs_test_isr_dispatch_fn, NULL, &vs_test_my_addr );

  /* Create and initialize VS_TEST session object. */
  rc =  vs_test_create_session_object ( &vs_test_session_obj );
  VS_TEST_PANIC_ON_ERROR( rc );

  rc = vs_test_vs_open_session ( vs_test_session_obj );
  VS_TEST_PANIC_ON_ERROR( rc );

  return rc;
}

static int32_t vs_test_deinit ( void )
{
  uint32_t rc = APR_EOK;

  if ( ( NULL == vs_test_session_obj ) || ( FALSE ==  vs_test_is_initialized ) )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "vs_test_deinit(): Already De-initialized or NULL session object " );
    return APR_EUNEXPECTED;
  }

  /* Close VS session instances. */
  ( void ) vs_test_vs_close_session( vs_test_session_obj, NULL );
  
  /* Free session object. */
  ( void ) apr_lock_destroy( vs_test_session_obj->data_lock );
  ( void ) apr_timer_destroy (vs_test_session_obj->pktexg_timer );
  ( void ) vs_test_mem_free_object( (vs_test_object_t*)vs_test_session_obj );

  vs_test_is_initialized = FALSE;
  
  apr_event_signal_abortall( vs_test_work_event );
  apr_event_wait( vs_test_control_event );
  
  /* Release gating control structures */
  ( void ) vs_test_gating_control_destroy( &vs_test_gating_work_pkt_q );
  
  /* Release work queue */
  ( void ) apr_list_destroy( &vs_test_free_work_pkt_q );
  ( void ) apr_list_destroy( &vs_test_nongating_work_pkt_q );
  
  /* Deinitialize the object handle table. */
  apr_objmgr_destruct( &vs_test_objmgr );
  
  /* Deinitialize basic OS resources for staging the setup. */
  ( void ) apr_event_destroy( vs_test_control_event );
  ( void ) apr_lock_destroy( vs_test_int_lock );
  ( void ) apr_lock_destroy( vs_test_thread_lock );
  ( void ) apr_lock_destroy( vs_test_global_lock );
  
  return rc;
}

VS_EXTERNAL int32_t vs_test_call(
  uint32_t cmd_id,
  void* params,
  uint32_t size
)
{
  int32_t rc = VS_EOK;
  MSG_1( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
       "vs_test_call() - cmd_id (0x%08X)",cmd_id );
  switch ( cmd_id )
  {
   case DRV_CMDID_INIT:
     rc = vs_test_init();
     break;

   case VS_ITEST_CMD_LOOPBACK_START:
   case VS_ITEST_CMD_LOOPBACK_STOP:
   case VS_ITEST_CMD_LOOPBACK_SET_VOC_PROPERTY:
     /* Queue the command for testclient thread. */
     if (vs_test_is_initialized)
     {
         rc = vs_test_prepare_and_dispatch_cmd_packet( NULL, cmd_id, params, size );
     }
     else
     {
         MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
            "vs_test_call() - cmd_id (0x%08X) issued before test is initialized",cmd_id );
     }
     break;

   case DRV_CMDID_DEINIT:
    rc = vs_test_deinit();
    break;
    
   default:
     MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
            "vs_test_call() - Unsupported cmd_id (0x%08X)",cmd_id );
     rc = APR_EUNSUPPORTED;
     break;
  }

  return rc;

}  /* end of vs_client_call() */
