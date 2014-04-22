/*
   Copyright (C) 2015-2019 Qualcomm Technologies, Inc.
   All Rights Reserved.
   Confidential and Proprietary - Qualcomm Technologies, Inc.

   $Header: //components/rel/avs.mpss/8.0.c12/vsd/vadapter/src/wva_module.c#1 $
   $Author: pwbldsvc $
*/

/****************************************************************************
 * INCLUDE HEADER FILES                                                     *
 ****************************************************************************/

#include <stddef.h>
#include <string.h>
#include "err.h"
#include "msg.h"
#include "rcinit.h"
#include "mmstd.h"

/* APR APIs. */
#include "apr_errcodes.h"
#include "apr_list.h"
#include "apr_objmgr.h"
#include "apr_lock.h"
#include "apr_timer.h"
#include "apr_event.h"
#include "apr_thread.h"
#include "apr_memmgr.h"

/* WCDMA APIs. */
#include "wcdma_ext_api.h"

#ifdef FEATURE_SEGMENT_LOADING
#include "mcfg_seg_load.h"
//#include "IWCDMA.h"
#endif

/* VSD APIs. */
#include "drv_api.h"
#include "vs_task.h"
#include "voicecfg_api.h"
#include "voicecfg_items.h"
#include "voice_util_api.h"
#include "voicelog_if.h"
#include "voicelog_utils.h"
#include "vs.h"
#include "vagent.h"

/* SELF APIs. */
#include "wva_if.h"
#include "wva_i.h"

/*****************************************************************************
 * Defines                                                                   *
 ****************************************************************************/

/* Currently voice agent support max two subscription and max two VSID. */
#define WVA_MAX_NUM_OF_SESSIONS_V ( 2 )

#define WVA_ENC_PKT_MISS_THRESHOLD ( 2 )

/* Offset definitions related to uplink processing 
 *
 * Even CFN boudary allignes to HARD-VFR.
 * WCDMA reads uplink data froms DSM post every odd CFN at stmrtick 
 * count of 30 i.e. at offset of 12000 from VFR.
 *
 * Encoder delivers encoder packet at 9000 from VFR.
 * WVA triggers an UL timer to fire at 11000usec i.e. 1000usec prior to
 * WCDMA requesting same.
 *
 * SOFT-VFR delay is adjusted while evaluating timer value.
 */
#define WVA_ENC_OFFSET_US ( 9000 )
#define WVA_PUT_UL_PACKET_INTO_DSM_OFFSET_US ( 11000 )
#define WVA_WCDMA_READS_UL_PACKET_OFFSET_US ( 12000 )


/* Offset definition related to downlink processing 
 *
 * Even CFN boudary allignes to HARD-VFR.
 * WCDMA queue downlink data into DSM post every even CFN.
 *
 * WVA triggers an timer to get DL from DSM at 4300usec i.e.
 * 1000usec prior to vocoder requesting same. 
 *
 * SOFT-VFR delay is adjusted while evaluating timer value.
 */
#define WVA_GET_DL_PACKET_FROM_DSM_OFFSET_US ( 4300 )
#define WVA_DEC_REQ_OFFSET_US ( 5300 )
#define WVA_DEC_OFFSET_US ( 6300 )


/* The frame size in microseconds that each voice processing threads 
 * (vptx, vprx, encoder, decoder, decoder pp) operates on.
 */
#define WVA_VOICE_FRAME_SIZE_US_V ( 20000 ) 

#define WVA_VOICE_SAMPLE_RATE_UNDEFINED_V ( 0 ) 

#define WVA_VOICE_SAMPLE_RATE_NB_V ( 8000 ) 

#define WVA_VOICE_SAMPLE_RATE_WB_V ( 16000 ) 


/*****************************************************************************
 * Global Variables                                                          *
 ****************************************************************************/

/* WVA address is set at initialization. */
char_t wva_my_dns[] =  "qcom.audio.wva";
static uint16_t wva_my_addr;
static uint32_t wva_apr_handle;

static apr_lock_t wva_int_lock;
static apr_lock_t wva_thread_lock;
static apr_event_t wva_control_event;

static apr_memmgr_type wva_heapmgr;
static uint8_t wva_heap_pool[ WVA_HEAP_SIZE_V ];

static apr_objmgr_t wva_objmgr;
static apr_objmgr_object_t wva_object_table[ WVA_MAX_OBJECTS_V ];

static wva_gating_control_t wva_gating_work_pkt_q;
static apr_list_t wva_nongating_work_pkt_q;
static apr_list_t wva_free_work_pkt_q;
static wva_work_item_t wva_work_pkts[ WVA_NUM_WORK_PKTS_V ];


static apr_event_t wva_work_event;
static apr_thread_t wva_thread;
static uint8_t wva_task_stack[ WVA_TASK_STACK_SIZE ];

static wva_session_object_t* wva_session_obj_list[WVA_MAX_NUM_OF_SESSIONS_V];
static apr_lock_t wva_global_lock;

static bool_t wva_is_eamr_enabled = FALSE;
static bool_t wva_is_initialized = FALSE; 

#ifdef FEATURE_SEGMENT_LOADING
  interface_t *wva_ptr_W =  NULL;
#endif

/****************************************************************************
 * COMMON INTERNAL ROUTINES                                                 *
 ****************************************************************************/

static void wva_int_lock_fn ( void )
{
  apr_lock_enter( wva_int_lock );
}

static void wva_int_unlock_fn ( void )
{
  apr_lock_leave( wva_int_lock );
}

static void wva_thread_lock_fn ( void )
{
  apr_lock_enter( wva_thread_lock );
}

static void wva_thread_unlock_fn ( void )
{
  apr_lock_leave( wva_thread_lock );
}

static void wva_signal_run ( void )
{
  apr_event_signal( wva_work_event );
}

/****************************************************************************
 * WVA CMDs & EVENTs PACKET QUEUING FUNCTIONS                               *
 ****************************************************************************/

/**
 * Queues the aprv2_packet_t (queues both incoming and pending packets to 
 * corresponding queues). In case of failure to queue a apr packet, packet 
 * will be automatically freed.
 */
static uint32_t wva_queue_apr_packet (
  aprv2_packet_t* packet,
  apr_list_t* cmd_q
)
{
  uint32_t rc = VS_EOK;
  wva_work_item_t* work_item = NULL;
  
  if( packet == NULL )
  {
    WVA_REPORT_FATAL_ON_ERROR( APR_EUNEXPECTED );
    return VS_EFAILED;
  }

  for ( ;; )
  {
    /* Get a free command structure. */
    rc = apr_list_remove_head( &wva_free_work_pkt_q, (apr_list_node_t**)&work_item );
    if ( NULL == work_item )
    {
      WVA_REPORT_FATAL_ON_ERROR(VS_ENORESOURCE);
      break;
    }

    work_item->packet = packet;
    rc = apr_list_add_tail( cmd_q, &work_item->link );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "WVA: error in queuing APR packet: opcode[0x%08x], rc[0x%08x]", packet->opcode, rc );
      work_item->packet = NULL;
      ( void ) apr_list_add_tail( &wva_free_work_pkt_q, &work_item->link );
    }
    else
    {
      /**
       * Signal appropriate thread.
       */
      wva_signal_run( );
    }

    break;
  } /* for loop ends. */

  return rc;
}

static int32_t wva_isr_dispatch_fn (
  aprv2_packet_t* packet,
  void* dispatch_data
)
{
  uint32_t rc = VS_EOK;

  rc = wva_queue_apr_packet( packet, &wva_nongating_work_pkt_q );
  WVA_REPORT_FATAL_ON_ERROR(rc);

  rc = __aprv2_cmd_free( wva_apr_handle, packet );
  WVA_REPORT_FATAL_ON_ERROR(rc);

  return VS_EOK;
}

/****************************************************************************
 * WVA CMDs/EVENTs PACKET PREPARE/DISPATCHER/FREE ROUTINES                  *
 ****************************************************************************/

static uint32_t wva_prepare_and_dispatch_cmd_packet (
  void* session_context,
  uint32_t cmd_id,
  void* cmd_params,
  uint32_t params_size
)
{
  uint32_t rc = VS_EOK;
  aprv2_packet_t* packet = NULL;
  void* payload;

  for ( ;; )
  {
    rc = __aprv2_cmd_alloc_ext( wva_apr_handle, APRV2_PKT_MSGTYPE_SEQCMD_V,
                                APR_NULL_V, APR_NULL_V,
                                APR_NULL_V, APR_NULL_V, 
                                (uint32_t)session_context, cmd_id,
                                params_size, &packet );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "WVA: APR packet allocation failed with rc=[0x%08x]", rc );
      break;
    }

    if ( ( params_size > 0 ) && ( cmd_params != NULL ) )
    {
      payload = APRV2_PKT_GET_PAYLOAD( void, packet);
      mmstd_memcpy( payload, params_size, cmd_params, params_size );
    }

    /* Queue the command packet for processing. */
    rc = wva_queue_apr_packet( packet, &wva_nongating_work_pkt_q );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "WVA: APR packet queuing failed. rc[0x%08x]", rc );
      rc = __aprv2_cmd_free( wva_apr_handle, packet );
      WVA_REPORT_FATAL_ON_ERROR(rc);
    }

    break;
  }

  return rc;
}

static void wva_pktexg_timer_cb (
  void* session_context
)
{
  uint32_t rc = APR_EOK;
  wva_session_object_t* session_obj = (wva_session_object_t*)session_context;

  if ( NULL == session_obj )
  {
    WVA_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return;
  }

  if( session_obj->cfn_value % 2 )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "WVA: VSID=(0x%08x): WVA_INTERNAL_EVENT_DELIVER_UL_BUFFER recieved", session_obj->vsid );
    rc = wva_prepare_and_dispatch_cmd_packet( 
           session_context, WVA_INTERNAL_EVENT_DELIVER_UL_BUFFER, NULL, 0 );
  }
  else
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "WVA: VSID=(0x%08x): WVA_INTERNAL_EVENT_DELIVER_DL_BUFFER recieved",
           session_obj->vsid );
    rc = wva_prepare_and_dispatch_cmd_packet( 
           session_context, WVA_INTERNAL_EVENT_DELIVER_DL_BUFFER, NULL, 0 );
  }

  return;
}

/****************************************************************************
 * WVA OBJECT CREATION, DESTRUCTION AND INITIALISATION ROUTINES             *
 ****************************************************************************/

static uint32_t wva_mem_alloc_object (
  uint32_t size,
  wva_object_t** ret_object
)
{
  int32_t rc;
  wva_object_t* wva_obj;
  apr_objmgr_object_t* objmgr_obj;

  if ( ret_object == NULL )
  {
    return APR_EBADPARAM;
  }

  { /* Allocate memory for the WVA object. */
    wva_obj = apr_memmgr_malloc( &wva_heapmgr, size );
    if ( wva_obj == NULL )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "wva_mem_alloc_object(): Out of memory, requested size (%d)", size );
      return APR_ENORESOURCE;
    }

    /* Allocate a new handle for the MVS object. */
    rc = apr_objmgr_alloc_object( &wva_objmgr, &objmgr_obj );
    if ( rc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
             "wva_mem_alloc_object(): Out of objects, rc = (0x%08X)", rc );
      apr_memmgr_free( &wva_heapmgr, wva_obj );
      return APR_ENORESOURCE;
    }

    /* Use the custom object type. */
    objmgr_obj->any.ptr = wva_obj;

    /* Initialize the base MVS object header. */
    wva_obj->header.handle = objmgr_obj->handle;
    wva_obj->header.type = WVA_OBJECT_TYPE_ENUM_UNINITIALIZED;
  }

  *ret_object = wva_obj;

  return APR_EOK;
}

static uint32_t wva_mem_free_object (
  wva_object_t* object
)
{
  if ( object == NULL )
  {
    return APR_EBADPARAM;
  }

  /* Free the object memory and object handle. */
  ( void ) apr_objmgr_free_object( &wva_objmgr, object->header.handle );
  apr_memmgr_free( &wva_heapmgr, object );

  return APR_EOK;
}

static uint32_t wva_create_session_object ( 
  wva_session_object_t** ret_session_obj )
{
  uint32_t rc = APR_EOK;
  wva_session_object_t* session_obj = NULL;

  if ( ret_session_obj == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = wva_mem_alloc_object( sizeof(wva_session_object_t), (wva_object_t**)&session_obj );
  if ( rc )
  {
    *ret_session_obj = NULL;
    return APR_ENORESOURCE;
  }

  { /* Initialize the simple job object. */
    session_obj->header.type = WVA_OBJECT_TYPE_ENUM_SESSION;
    session_obj->asid = SYS_MODEM_AS_ID_NONE;
    session_obj->vsid = WVA_VSID_UNDEFINED_V;

    rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &session_obj->data_lock );
    WVA_PANIC_ON_ERROR(rc);

    ( void ) voice_dsm_amr_q_init ( &session_obj->ul_queue, WVA_AMR_DSM_Q_LEN );
    ( void ) voice_dsm_amr_q_init ( &session_obj->dl_queue, WVA_AMR_DSM_Q_LEN );
    mmstd_memset ( &session_obj->ul_chan_state, 0, 
                   sizeof( session_obj->ul_chan_state ) );
    mmstd_memset ( &session_obj->dl_chan_state, 0, 
                   sizeof( session_obj->dl_chan_state ) );

    rc = apr_timer_create( &session_obj->pktexg_timer, wva_pktexg_timer_cb, session_obj );
    WVA_PANIC_ON_ERROR( rc );

    session_obj->wcdma_handle = APR_NULL_V;
    session_obj->is_wcdma_ready = FALSE;
    session_obj->vocoder_type = WVA_VOCODER_ID_UNDEFINED_V;
    session_obj->codec_mode = WVA_CODEC_MODE_UNDEFINED;
    session_obj->dtx_mode = FALSE;

    session_obj->is_resource_granted = FALSE;
    session_obj->vocoder_state = VOICELOG_IEVENT_VOCODER_STATE_RELEASED;
    session_obj->enc_packet_miss_count = 0;

    session_obj->vs_handle = APR_NULL_V;
    session_obj->vs_read_buf = NULL;
    session_obj->vs_write_buf = NULL;
    session_obj->is_vs_ready = FALSE;
  }

  *ret_session_obj = session_obj;

  return APR_EOK;
}

static uint32_t wva_create_simple_job_object (
  uint32_t parentjob_handle,
  wva_simple_job_object_t** ret_job_obj
)
{
  int32_t rc;
  wva_simple_job_object_t* wva_obj = NULL;

  if ( ret_job_obj == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = wva_mem_alloc_object( sizeof( wva_simple_job_object_t ), (wva_object_t**) &wva_obj );
  if ( rc )
  {
    *ret_job_obj = NULL;
    return APR_ENORESOURCE;
  }

  { /* Initialize the simple job object. */
    wva_obj->header.type = WVA_OBJECT_TYPE_ENUM_SIMPLE_JOB;
    wva_obj->context_handle = parentjob_handle;
    wva_obj->is_completed = 0;
  }

  *ret_job_obj = wva_obj;

  return APR_EOK;
}


/****************************************************************************
 * WVA WCDMA <> VS MAPPING  ROUTINES                                          *
 ****************************************************************************/

static uint32_t wva_map_vocamr_codec_mode_wcdma_to_vs( 
  uint32_t wcdma_codec_mode,
  uint8_t* vs_codec_mode
)
{
  uint32_t rc = APR_EOK;

  switch ( wcdma_codec_mode )
  {
   case WCDMA_IVOCAMR_CODEC_MODE_0475:
     *vs_codec_mode = VS_VOCAMR_CODEC_MODE_0475;
     break;

   case WCDMA_IVOCAMR_CODEC_MODE_0515:
     *vs_codec_mode = VS_VOCAMR_CODEC_MODE_0515;
     break;

   case WCDMA_IVOCAMR_CODEC_MODE_0590:
     *vs_codec_mode = VS_VOCAMR_CODEC_MODE_0590;
     break;

   case WCDMA_IVOCAMR_CODEC_MODE_0670:
     *vs_codec_mode = VS_VOCAMR_CODEC_MODE_0670;
     break;

   case WCDMA_IVOCAMR_CODEC_MODE_0740:
     *vs_codec_mode = VS_VOCAMR_CODEC_MODE_0740;
     break;

   case WCDMA_IVOCAMR_CODEC_MODE_0795:
     *vs_codec_mode = VS_VOCAMR_CODEC_MODE_0795;
     break;

   case WCDMA_IVOCAMR_CODEC_MODE_1020:
     *vs_codec_mode = VS_VOCAMR_CODEC_MODE_1020;
     break;

   case WCDMA_IVOCAMR_CODEC_MODE_1220:
     *vs_codec_mode = VS_VOCAMR_CODEC_MODE_1220;
     break;
  
   default:
     rc = APR_EBADPARAM;
     *vs_codec_mode = 0xFF;
     break;
  }

  return rc;
}

static uint32_t wva_map_vocamrwb_codec_mode_wcdma_to_vs( 
  uint32_t wcdma_codec_mode,
  uint8_t* vs_codec_mode
)
{
  uint32_t rc = APR_EOK;

  switch ( wcdma_codec_mode )
  {
   case WCDMA_IVOCAMRWB_CODEC_MODE_0660:
     *vs_codec_mode = VS_VOCAMRWB_CODEC_MODE_0660;
     break;
  
   case WCDMA_IVOCAMRWB_CODEC_MODE_0885:
     *vs_codec_mode = VS_VOCAMRWB_CODEC_MODE_0885;
     break;
  
   case WCDMA_IVOCAMRWB_CODEC_MODE_1265:
     *vs_codec_mode = VS_VOCAMRWB_CODEC_MODE_1265;
     break;
  
   case WCDMA_IVOCAMRWB_CODEC_MODE_1425:
     *vs_codec_mode = VS_VOCAMRWB_CODEC_MODE_1425;
     break;
    
   case WCDMA_IVOCAMRWB_CODEC_MODE_1585:
     *vs_codec_mode = VS_VOCAMRWB_CODEC_MODE_1585;
     break;
    
   case WCDMA_IVOCAMRWB_CODEC_MODE_1825:
     *vs_codec_mode = VS_VOCAMRWB_CODEC_MODE_1825;
     break;
    
   case WCDMA_IVOCAMRWB_CODEC_MODE_1985:
     *vs_codec_mode = VS_VOCAMRWB_CODEC_MODE_1985;
     break;
   
   case WCDMA_IVOCAMRWB_CODEC_MODE_2305:
     *vs_codec_mode = VS_VOCAMRWB_CODEC_MODE_2305;
     break;
  
   case WCDMA_IVOCAMRWB_CODEC_MODE_2385:
     *vs_codec_mode = VS_VOCAMRWB_CODEC_MODE_2385;
     break;
  
   default:
     rc = APR_EBADPARAM;
     *vs_codec_mode = 0xFF;
     break;
  }

  return rc;
}

static uint32_t wva_map_vocoder_type_wcdma_to_vs(
  uint32_t wcdma_vocoder_type
)
{
  uint32_t vs_media_id = 0xFFFFFFFF;

  switch ( wcdma_vocoder_type )
  {
   case WCDMA_IVOCODER_ID_AMR:
     vs_media_id = (TRUE== wva_is_eamr_enabled)?VS_VOCEAMR_MEDIA_ID : VS_VOCAMR_MEDIA_ID;
     break;

   case WCDMA_IVOCODER_ID_AMRWB:
     vs_media_id = VS_VOCAMRWB_MEDIA_ID;
     break;
  }

  return vs_media_id;
}


static uint32_t wva_update_chan_state (
  voice_amr_chan_state_t* chan_state,
  uint32_t chan_class
)
{
  uint32_t rc = APR_EOK;

  switch ( chan_class )
  {
   case WCDMA_ICOMMON_CHAN_CLASS_TYPE_ABC:
     chan_state->has_chan_c = TRUE;
     /* fall through */

   case WCDMA_ICOMMON_CHAN_CLASS_TYPE_AB:
     chan_state->has_chan_b = TRUE;
     /* fall through */

   case WCDMA_ICOMMON_CHAN_CLASS_TYPE_A:
     chan_state->has_chan_a = TRUE;
     break;

   case WCDMA_ICOMMON_CHAN_CLASS_TYPE_NONE:
    {
      chan_state->has_chan_a = FALSE;
      chan_state->has_chan_b = FALSE;
      chan_state->has_chan_c = FALSE;
    }
    break;

   default:
     rc = APR_EBADPARAM;
     break;
  }

  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "wva_update_chan_state(): Invalid channel class=(0x%08x)", 
           chan_class );
  }

  return rc;
}

static void wva_downlink_channel_data_available (
  uint8_t lc_id,
  uint8_t n_unit,
  void* session_context 
)
{
  wva_session_object_t* session_obj = NULL;
  session_obj = ( wva_session_object_t* ) session_context;

  for ( ;; )
  {
    if( lc_id == session_obj->dl_chan_state.lcc )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "wva_downlink_channel_data_available(): logical channel C data "
             "available lc_id =(%d)", lc_id );
      break;
    }

    if( lc_id == session_obj->dl_chan_state.lcb )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "wva_downlink_channel_data_available(): logical channel B data "
             "available lc_id =(%d)", lc_id );
      break;
    }

    if( lc_id == session_obj->dl_chan_state.lca )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
             "wva_downlink_channel_data_available(): logical channel A data "
             "available lc_id =(%d)", lc_id );
      session_obj->is_dl_buffer_ready = TRUE;
      break;
    }

    break;
  }

  return;
}

/****************************************************************************
 * WVA COMMON ROUTINES                                                      *
 ****************************************************************************/

static void wva_publish_vocoder_state (
  wva_session_object_t* session_obj,
  uint32_t vocoder_state
)
{
  if( ( NULL != session_obj ) &&
      ( vocoder_state != session_obj->vocoder_state ) )
  {
    /* Publish vocoder state for voice call on a subscription. */
   session_obj->vocoder_state = vocoder_state;
   voicelog_event_report( session_obj->asid, session_obj->vsid,
                          session_obj->vocoder_state, VOICELOG_ICOMMON_NETWORK_ID_WCDMA,
                          wva_map_vocoder_type_wcdma_to_vs( session_obj->vocoder_type ) );
  }

  return;
}

static uint32_t wva_log_vocoder_frame ( 
  wva_session_object_t* session_obj,
  uint32_t log_code,
  uint8_t frameHeader,
  uint8_t* frame,
  uint32_t frameSize
)
{
  voicelog_ipacket_cmd_commit_data_t log_cmd_param; 

  log_cmd_param.version = VOICELOG_IPACKET_VOCODER_DATA_VERSION_V;
  log_cmd_param.log_code = log_code;
  log_cmd_param.vsid = session_obj->vsid;
  log_cmd_param.network_id = VOICELOG_ICOMMON_NETWORK_ID_WCDMA;
  log_cmd_param.timestamp = 0;
  log_cmd_param.tap_point_id = VOICELOG_IPACKET_TAP_POINT_ID_VOICE_ADAPTER;

  log_cmd_param.media_id = wva_map_vocoder_type_wcdma_to_vs ( session_obj->vocoder_type );
  log_cmd_param.frame_header = frameHeader;
  log_cmd_param.frame = frame;
  log_cmd_param.frame_size = frameSize;
  log_cmd_param.data_extension = NULL;

  /* Log data. */
  ( void ) voicelog_call ( VOICELOG_IPACKET_CMD_COMMIT_DATA, (void*)&log_cmd_param, 
                           sizeof( voicelog_ipacket_cmd_commit_data_t ) );

  return APR_EOK;
}

static uint32_t wva_get_dl_vocoder_packet ( 
  wva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_pktexg_buffer_t* vsBuffer = NULL;
  uint8_t* frame = NULL;
  vs_vocamr_frame_header_t* amrFrameHeader = NULL;
  vs_vocamrwb_frame_header_t* amrwbFrameHeader = NULL;

  vsBuffer = session_obj->vs_write_buf;
  vsBuffer->media_id = wva_map_vocoder_type_wcdma_to_vs( session_obj->vocoder_type );
  frame = ((uint8_t*)vsBuffer) + sizeof( vs_pktexg_buffer_t );

  rc = voice_amr_dl_processing( vsBuffer, &session_obj->dl_queue );
  if ( rc ) return rc;

  if ( ( VS_VOCAMR_MEDIA_ID == vsBuffer->media_id ) ||
       ( VS_VOCEAMR_MEDIA_ID == vsBuffer->media_id ) )
  {
    amrFrameHeader = (vs_vocamr_frame_header_t*)frame;
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "WVA: (E)AMR: GET_DL_PACKET: FrameType=(%d), CodecMode=(%d)",
           amrFrameHeader->frame_type, amrFrameHeader->codec_mode );
  }

  if ( VS_VOCAMRWB_MEDIA_ID == vsBuffer->media_id )
  {
    amrwbFrameHeader = (vs_vocamrwb_frame_header_t*)frame;
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "WVA: AMR-WB: GET_DL_PACKET: FrameType=(%d), CodecMode=(%d)",
           amrwbFrameHeader->frame_type, amrwbFrameHeader->codec_mode );
  }

  return rc;
}

static uint32_t wva_get_no_data_vocoder_packet ( 
  wva_session_object_t* session_obj
)
{
  uint8_t* frame = NULL;
  vs_vocamr_frame_header_t* amrFrameHeader = NULL;
  vs_vocamrwb_frame_header_t* amrwbFrameHeader = NULL;
  uint8_t codec_mode;
  vs_pktexg_buffer_t* vsBuffer = NULL;

  vsBuffer = session_obj->vs_write_buf;
  vsBuffer->media_id = wva_map_vocoder_type_wcdma_to_vs( session_obj->vocoder_type );
  frame = ((uint8_t*)vsBuffer) + sizeof( vs_pktexg_buffer_t );

  if ( ( VS_VOCAMR_MEDIA_ID== vsBuffer->media_id ) ||
       ( VS_VOCEAMR_MEDIA_ID== vsBuffer->media_id ) )
  {
    wva_map_vocamr_codec_mode_wcdma_to_vs( session_obj->codec_mode, &codec_mode);
    amrFrameHeader = (vs_vocamr_frame_header_t*)frame;
    amrFrameHeader->codec_mode = (vs_vocamr_codec_mode_t)codec_mode;
    amrFrameHeader->frame_type = VS_VOCAMR_FRAME_TYPE_NO_DATA;
    vsBuffer->frame_size = 1;
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "WVA: (E)AMR: GET_DL_PACKET: FrameType=(%d), CodecMode=(%d)",
           amrFrameHeader->frame_type, amrFrameHeader->codec_mode );
  }

  if ( VS_VOCAMRWB_MEDIA_ID == vsBuffer->media_id )
  {
    wva_map_vocamrwb_codec_mode_wcdma_to_vs( session_obj->codec_mode, &codec_mode);
    amrwbFrameHeader = (vs_vocamrwb_frame_header_t*)frame;
    amrwbFrameHeader->codec_mode = (vs_vocamrwb_codec_mode_t)codec_mode;
    amrwbFrameHeader->frame_type = VS_VOCAMRWB_FRAME_TYPE_NO_DATA;
    vsBuffer->frame_size = 1;
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "WVA: AMR-WB: GET_DL_PACKET: FrameType=(%d), CodecMode=(%d)",
           amrwbFrameHeader->frame_type, amrwbFrameHeader->codec_mode );
  }

  return VS_EOK;
}

static uint32_t wva_deliver_ul_silence_vocoder_packet ( 
  wva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  uint8_t frame[VS_VOCAMRWB_MAX_PACKET_DATA_LENGTH+1];
  uint32_t frameSize=0;
  uint32_t vs_media_id; uint8_t codec_mode;
  vs_vocamr_frame_header_t* amrFrameHeader = NULL;
  vs_vocamrwb_frame_header_t* amrwbFrameHeader = NULL;

  vs_media_id = wva_map_vocoder_type_wcdma_to_vs( session_obj->vocoder_type );

  if ( ( vs_media_id == VS_VOCAMR_MEDIA_ID ) ||
       ( vs_media_id == VS_VOCEAMR_MEDIA_ID ) )
  {
    ( void ) wva_map_vocamr_codec_mode_wcdma_to_vs( session_obj->codec_mode, &codec_mode );
    amrFrameHeader = (vs_vocamr_frame_header_t*)&frame[0];
    amrFrameHeader->frame_type = VS_VOCAMR_FRAME_TYPE_SPEECH_GOOD;
    amrFrameHeader->codec_mode = ( vs_vocamr_codec_mode_t ) codec_mode;
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "WVA: (E)AMR: DELIVER_UL_PACKET: sending silence frame FrameType=(%d), CodecMode=(%d)",
           amrFrameHeader->frame_type, amrFrameHeader->codec_mode );
  }

  if( vs_media_id == VS_VOCAMRWB_MEDIA_ID )
  {
    ( void ) wva_map_vocamrwb_codec_mode_wcdma_to_vs( session_obj->codec_mode, &codec_mode );
    amrwbFrameHeader = (vs_vocamrwb_frame_header_t*)&frame[0];
    amrwbFrameHeader->frame_type = VS_VOCAMRWB_FRAME_TYPE_SPEECH_GOOD;
    amrwbFrameHeader->codec_mode = ( vs_vocamrwb_codec_mode_t ) codec_mode;
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "WVA: AMR-WB: DELIVER_UL_PACKET: sending silence frame FrameType=(%d), CodecMode=(%d)",
           amrwbFrameHeader->frame_type, amrwbFrameHeader->codec_mode );
  }

  /* Get the silence frame vectors from voice utils. */
  rc = voice_util_get_homing_frame( vs_media_id, (void*)&frame[0], &frame[1],
                                    VS_VOCAMRWB_MAX_PACKET_DATA_LENGTH, &frameSize );
  if ( rc ) return VS_EFAILED;

  /* Deliver the vocoder data to W-RLC via DSM queue. */
  rc = voice_amr_ul_processing( vs_media_id, &frame[0], &session_obj->ul_chan_state, &session_obj->ul_queue );

  ( void ) wva_log_vocoder_frame( session_obj, VOICELOG_IPACKET_CODE_UL_VOCODER_PACKET,
                                  frame[0], &frame[1], frameSize );

  return rc;
}

static uint32_t wva_deliver_ul_vocoder_packet ( 
  wva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_pktexg_buffer_t* vs_buffer = NULL;
  uint8_t* frame = NULL;
  vs_vocamr_frame_header_t* amrFrameHeader = NULL;
  vs_vocamrwb_frame_header_t* amrwbFrameHeader = NULL;

  vs_buffer = session_obj->vs_read_buf;
  frame = ((uint8_t*)vs_buffer) + sizeof( vs_pktexg_buffer_t );

  if ( ( VS_VOCAMR_MEDIA_ID == vs_buffer->media_id ) ||
       ( VS_VOCEAMR_MEDIA_ID == vs_buffer->media_id ) )
  {
    amrFrameHeader = (vs_vocamr_frame_header_t*) frame;
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "WVA: (E)AMR: DELIVER_UL_PACKET: FrameType=(%d), CodecMode=(%d)",
           amrFrameHeader->frame_type, amrFrameHeader->codec_mode );
    rc = voice_amr_ul_processing( vs_buffer->media_id, frame,
                                  &session_obj->ul_chan_state, &session_obj->ul_queue );
    return VS_EOK;
  }

  if ( VS_VOCAMRWB_MEDIA_ID == vs_buffer->media_id )
  {
    frame = ((uint8_t*)vs_buffer) + sizeof( vs_pktexg_buffer_t );
    amrwbFrameHeader = (vs_vocamrwb_frame_header_t*) frame;
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "WVA: AMR-WB: DELIVER_UL_PACKET: FrameType=(%d), CodecMode=(%d)",
           amrwbFrameHeader->frame_type, amrwbFrameHeader->codec_mode );
    rc = voice_amr_ul_processing( vs_buffer->media_id, frame,
                                  &session_obj->ul_chan_state, &session_obj->ul_queue );
    return VS_EOK;
  }

  return rc;
}

static void wva_log_event_info (
  void* session_context,
  uint32_t event_id
)
{
  wva_session_object_t* session_obj = ( wva_session_object_t* ) session_context;
 
  if ( session_context == NULL ) return;
 
   switch( event_id )
   {
     case VS_VOC_EVENT_OPERATING_MODE_UPDATE:
     case VS_COMMON_EVENT_CMD_RESPONSE:
     break;

     case VS_VOC_EVENT_NOT_READY:
       MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
              "WVA: ASID=(%d): VSID=(0x%08x): VS_VOC_EVENT_NOT_READY recieved",
             session_obj->asid, session_obj->vsid );
       break;

     case VS_VOC_EVENT_READY:
       MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "WVA: ASID=(%d): VSID=(0x%08x): VS_VOC_EVENT_READY recieved",
             session_obj->asid, session_obj->vsid );
       break;

     case WCDMA_IVOICE_EVENT_SET_LOGICAL_CHANNELS:
       MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
              "WVA: ASID=(%d): VSID=(0x%08x): WCDMA_IVOICE_EVENT_SET_LOGICAL_CHANNELS recieved",
              session_obj->asid, session_obj->vsid );
       break;
 
     case WCDMA_IVOICE_EVENT_REQUEST_START:
       MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "WVA: ASID=(%d): VSID=(0x%08x): WCDMA_IVOICE_EVENT_REQUEST_START recieved",
             session_obj->asid, session_obj->vsid );
       break;
 
     case WCDMA_IVOICE_EVENT_REQUEST_STOP:
       MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
              "WVA: ASID=(%d): VSID=(0x%08x): WCDMA_IVOICE_EVENT_REQUEST_STOP recieved",
              session_obj->asid, session_obj->vsid );
       break;
 
     case WCDMA_IVOICE_EVENT_SELECT_OWNER:
       MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
              "WVA: ASID=(%d): VSID=(0x%08x): WCDMA_IVOICE_EVENT_SELECT_OWNER recieved",
              session_obj->asid, session_obj->vsid );
       break;
 
     case WCDMA_IVOICE_EVENT_REQUEST_CODEC_MODE:
       MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
              "WVA: ASID=(%d): VSID=(0x%08x): WCDMA_IVOICE_EVENT_REQUEST_CODEC_MODE recieved",
              session_obj->asid, session_obj->vsid );
       break;
 
     case WCDMA_IVOICE_EVENT_REQUEST_SCR_MODE:
       MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
              "WVA: ASID=(%d): VSID=(0x%08x): WCDMA_IVOICE_EVENT_REQUEST_SCR_MODE recieved",
              session_obj->asid, session_obj->vsid );
       break;

     case WCDMA_IVOICEL1_EVENT_VFR_NOTIFICATION:
     default:
       break;
   }

   return;
}



/****************************************************************************
 * WVA VS SESSION ROUTINES                                                  *
 ****************************************************************************/
static uint32_t wva_vs_event_cb(
  uint32_t event_id,
  void* params,
  uint32_t size,
  void* session_context
)
{
  uint32_t rc = APR_EOK;

  if ( wva_is_initialized == FALSE ) return APR_EOK;

  switch ( event_id )
  {
    case VS_COMMON_EVENT_CMD_RESPONSE:
    case VS_VOC_EVENT_NOT_READY:
    case VS_VOC_EVENT_READY:
    case VS_VOC_EVENT_OPERATING_MODE_UPDATE:
    {
      (void) wva_log_event_info( session_context, event_id );
      rc = wva_prepare_and_dispatch_cmd_packet( 
             session_context, event_id, params, size );
    }
    break;

    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
             "wva_vs_event_cb(): Unsupported event (%d)", event_id );
      rc = APR_EFAILED;
  }

  return rc;
}

static uint32_t wva_vs_set_voc_codec_mode (
  wva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_param_enc_rate_t param;
  vs_common_cmd_set_param_t setParamCmd;
  uint8_t codec_mode;

  switch ( session_obj->vocoder_type )
  {
     case WCDMA_IVOCODER_ID_AMR:
     {
       rc = wva_map_vocamr_codec_mode_wcdma_to_vs( session_obj->codec_mode, &codec_mode );
       if ( rc )
       {
         MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                "WVA: AMR: Unsupported CodecMode=(0x%08x)", session_obj->codec_mode );
       }
       else
       {
         MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
                "WVA: AMR: CodecMode=(%d) sent to VS", codec_mode );
       }
     }
     break;

     case WCDMA_IVOCODER_ID_AMRWB:
     {
       rc = wva_map_vocamrwb_codec_mode_wcdma_to_vs ( session_obj->codec_mode, &codec_mode );
       if ( rc )
       {
         MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                "WVA: AMR-WB: Unsupported CodecMode=(0x%08x)", session_obj->codec_mode );
       }
       else
       {
         MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
                "WVA: AMR-WB: CodecMode=(%d) sent to VS", codec_mode );
       }
     }
     break;

     default:
       MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
              "WVA: set codec mode for Unsupported vocoder=(0x%08x), ",
              session_obj->vocoder_type );
       return VS_EFAILED;
    }

  if ( NULL == session_obj->vs_handle )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "WVA: set codec mode VS session handle is not yet opened");
    return VS_EFAILED;
  }

  param.rate = codec_mode;
  setParamCmd.handle = session_obj->vs_handle;
  setParamCmd.param_data = (void*)&param;
  setParamCmd.param_id = VS_PARAM_ENC_RATE;
  setParamCmd.size = sizeof( param );

  rc  = vs_call( VS_COMMON_CMD_SET_PARAM, ( void* )&setParamCmd, sizeof( setParamCmd ) );
  if ( rc != APR_EOK )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "WVA: failed to set codec_mode=(%d), vocoder=(0x%08x)",
           session_obj->codec_mode, session_obj->vocoder_type );
  }

  return rc;
}


static uint32_t wva_vs_set_voc_dtx_mode (
  wva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_param_enc_dtx_v2_t param;
  vs_common_cmd_set_param_t setParamCmd;

  switch ( session_obj->vocoder_type )
  {
    case WCDMA_IVOCODER_ID_AMR:
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "WVA: AMR dtx_mode=(%d) sent to VS", session_obj->dtx_mode );
    }
    break;

    case WCDMA_IVOCODER_ID_AMRWB:
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "WVA: AMR-WB dtx_mode=(%d) sent to VS", session_obj->dtx_mode );
    }
    break;

    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "wva_set_voc_dtx_mode(): Unsupported vocoder=(0x%08x), ",
             session_obj->vocoder_type );
      return VS_EFAILED;
  }

  param.dtx_mode = session_obj->dtx_mode;
  param.version = 1;
  setParamCmd.handle = session_obj->vs_handle;
  setParamCmd.param_data = (void*)&param;
  setParamCmd.param_id = VS_PARAM_ENC_DTX_V2;
  setParamCmd.size = sizeof( param );

  rc  = vs_call( VS_COMMON_CMD_SET_PARAM, ( void* )&setParamCmd, sizeof( setParamCmd ) );
  if ( rc != APR_EOK )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "WVA: failed to set dtx_mode=(%d), vocoder=(0x%08x)",
           session_obj->dtx_mode, session_obj->vocoder_type );
  }

  return rc;
}

static void wva_vs_set_voc_media_type (
  wva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_param_voc_media_id_t param;
  vs_common_cmd_set_param_t setParamCmd;

  param.media_id = wva_map_vocoder_type_wcdma_to_vs( session_obj->vocoder_type );
  setParamCmd.handle = session_obj->vs_handle;
  setParamCmd.param_data = (void*)&param;
  setParamCmd.param_id = VS_PARAM_VOC_MEDIA_ID;
  setParamCmd.size = sizeof( param );

  rc  = vs_call( VS_COMMON_CMD_SET_PARAM, ( void* )&setParamCmd, sizeof( setParamCmd ) );
  if ( rc != APR_EOK )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "WVA: failed to set mediaID[0x%08x]", session_obj->vocoder_type );
  }

  return;
}


static void wva_vs_set_voc_timing_offsets (
  wva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_param_voc_timing_offsets_t timingParam;
  vs_common_cmd_set_param_t setParamCmd;

  timingParam.enc_offset = WVA_ENC_OFFSET_US;
  timingParam.dec_req_offset = WVA_DEC_REQ_OFFSET_US;
  timingParam.dec_offset = WVA_DEC_OFFSET_US;
  timingParam.vfr_mode = 1;

  setParamCmd.handle = session_obj->vs_handle;
  setParamCmd.param_data = (void*)&timingParam;
  setParamCmd.param_id = VS_PARAM_VOC_TIMING_OFFSETS;
  setParamCmd.size = sizeof( timingParam );
  rc  = vs_call( VS_COMMON_CMD_SET_PARAM, ( void* )&setParamCmd, sizeof( setParamCmd ) );
  if ( rc )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "WVA: failed to set timing Offsets" );
  }

  return;
}

static uint32_t wva_vs_get_encoder_buffer (
  wva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_pktexg_cmd_get_encoder_buffer_t getEncBufCmd;

  getEncBufCmd.handle = session_obj->vs_handle;
  getEncBufCmd.buffer = (void*)&session_obj->vs_read_buf;

  rc = vs_call( VS_PKTEXG_CMD_GET_ENCODER_BUFFER, (void*)&getEncBufCmd, sizeof( getEncBufCmd ) );
  if ( rc )
  {
    session_obj->vs_read_buf = NULL;
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "WVA: Failed to get read vs_buffer, vs_read_buf[0x%08x], rc[0x%08x]",
           session_obj->vs_read_buf, rc );
  }

  return rc;
}


static uint32_t wva_vs_return_encoder_buffer (
  wva_session_object_t* session_obj
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
           "WVA: Failed to return read buffer, buffer[0x%08x], rc[0x%08x]",
           session_obj->vs_read_buf, rc );
  }

  session_obj->vs_read_buf = NULL;
  return rc;
}

static uint32_t wva_vs_get_decoder_buffer (
  wva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  vs_pktexg_cmd_get_decoder_buffer_t getDecBufCmd;

  getDecBufCmd.handle = session_obj->vs_handle;
  getDecBufCmd.buffer = (void*)&session_obj->vs_write_buf;

  rc = vs_call( VS_PKTEXG_CMD_GET_DECODER_BUFFER, (void*)&getDecBufCmd, sizeof( getDecBufCmd ) );
  if ( rc )
  {
    session_obj->vs_write_buf = NULL;
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "WVA: Failed to get vs_write_buf,  rc[0x%08x]", rc );
  }

  return rc;
}


static uint32_t wva_vs_put_decoder_buffer (
  wva_session_object_t* session_obj
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
           "WVA: Failed to return vs_write_buf[0x%08x], rc[0x%08x]", session_obj->vs_write_buf, rc );
  }

  session_obj->vs_write_buf = NULL;
  return APR_EOK;
}


static uint32_t wva_vs_open_vocoder_session (
 wva_session_object_t* session_obj
)
{
  uint32_t rc;
  vs_voc_cmd_open_t open_cmd;

  /* Open VS session. */
  open_cmd.ret_handle = &session_obj->vs_handle;
  open_cmd.vsid = session_obj->vsid;
  open_cmd.network_id = VS_COMMON_NETWORK_ID_WCDMA;
  open_cmd.session_context = ( void* )session_obj;
  open_cmd.event_cb = wva_vs_event_cb;

  rc = vs_call( VS_VOC_CMD_OPEN, (void*)&open_cmd, sizeof( open_cmd ) );
  if ( rc )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "WVA: failed to open VS session rc = (0x%08x)", rc );
  }

  return rc;
}


static uint32_t wva_vs_enable_vocoder (
  wva_session_object_t* session_obj,
  void* token
)
{
  uint32_t rc = APR_EOK;
  vs_voc_cmd_enable_t enableCmd;

  for ( ;; )
  {
    if ( session_obj->vs_handle == APR_NULL_V )
    {
      rc = APR_EOK;
      break;
    }

    /* Update Vocoder Session Number for vocoder logging purpose. */
    ( void ) voicelog_session_update ( session_obj->vsid,
                                       VOICELOG_INFO_ENUM_VOCODER_SESSION_NUMBER);

    MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "WVA: Enabling VS!!!" );

    enableCmd.handle = session_obj->vs_handle;
    enableCmd.token = token;
    rc = vs_call( VS_VOC_CMD_ENABLE, &enableCmd, sizeof( enableCmd ) );
    if ( rc )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "WVA: Failed to enable VS session, handle[0x%08x], rc[0x%08x]",
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


static uint32_t wva_vs_close_vocoder_session (
  wva_session_object_t* session_obj,
  void* token
)
{
  uint32_t rc = APR_EOK;
  vs_voc_cmd_close_t closeCmd;

  if ( session_obj->vs_handle == APR_NULL_V ) return VS_EOK;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "WVA: Closing VS!!!" );

  closeCmd.handle = session_obj->vs_handle;
  closeCmd.token = token;
  rc = vs_call( VS_VOC_CMD_CLOSE, (void*)&closeCmd, sizeof( closeCmd ) );
  if ( rc )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "WVA: Failed to close VS session, handle[0x%08x], rc[0x%08x]",
           session_obj->vs_handle, rc );
  }
  else
  {
    session_obj->vs_handle = APR_NULL_V;
    rc = APR_EPENDING;
  }

  return rc;
}



/****************************************************************************
 * WVA WCDMA SESSION ROUTINES                                                  *
 ****************************************************************************/

static uint32_t wva_wcdma_event_cb(
 sys_modem_as_id_e_type asid,
 uint32_t event_id,
 void* params,
 uint32_t size
)
{
  uint32_t rc = APR_EOK;
  void* session_context = NULL;

  if ( ( asid < SYS_MODEM_AS_ID_1 ) || ( asid > SYS_MODEM_AS_ID_2 ) )
  {
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "wva_wcdma_event_cb(): ASID=(%d) not supported", asid );
    return APR_EBADPARAM;
  }

  session_context = ( void* ) wva_session_obj_list[ asid ];

  switch ( event_id )
  {
   case WCDMA_IVOICE_EVENT_REQUEST_START:
   case WCDMA_IVOICE_EVENT_REQUEST_STOP:
   case WCDMA_IVOICE_EVENT_SELECT_OWNER:
   case WCDMA_IVOICE_EVENT_SET_LOGICAL_CHANNELS:
   case WCDMA_IVOICE_EVENT_REQUEST_CODEC_MODE:
   case WCDMA_IVOICE_EVENT_REQUEST_SCR_MODE:
   case WCDMA_IVOICEL1_EVENT_VFR_NOTIFICATION:
    {
      ( void ) wva_log_event_info( session_context, event_id );
      rc = wva_prepare_and_dispatch_cmd_packet( session_context, event_id, params, size );
    }
    break;

   default:
     MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
            "wva_wcdma_event_cb(): Unsupported event (%d)", event_id );
     rc = APR_EFAILED;
  }

  return rc;
}

static uint32_t wva_wcdma_open_session (
 wva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  wcdma_ivoice_cmd_open_t open_cmd;

  if( NULL == session_obj )
  {
    WVA_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return APR_EBADPARAM;
  }

  open_cmd.ret_handle = &session_obj->wcdma_handle;
  open_cmd.asid = session_obj->asid;
  open_cmd.event_cb = wva_wcdma_event_cb;

#ifdef FEATURE_SEGMENT_LOADING
  wva_ptr_W = get_wcdma_interface();
  if ( wva_ptr_W != NULL )
  {
    wcdma_ext_audio_api( WCDMA_VOICE_CMD_OPEN, &open_cmd, sizeof( open_cmd ) );
  }
  else
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "wva_wcdma_open_session(): WCDMA SEGMENT NOT LOADED!!");
  }
#else
  rc = wcdma_ext_audio_api( WCDMA_VOICE_CMD_OPEN, &open_cmd, sizeof( open_cmd ) );
#endif

  if ( ( APR_EOK != rc ) && ( APR_EUNSUPPORTED != rc ) ) 
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "WVA: Failed to open WCDMA session for asid[%d]) with rc[0x%08x]",
           open_cmd.asid, rc );
  }
  else
  {
    rc = VS_EOK;
  }

  return rc;
}

static uint32_t wva_wcdma_close_session (
  wva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  wcdma_ivoice_cmd_close_t close_cmd;

  if( NULL == session_obj )
  {
    WVA_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return APR_EBADPARAM;
  }

  close_cmd.handle = session_obj->wcdma_handle;

#ifdef FEATURE_SEGMENT_LOADING
  wva_ptr_W = get_wcdma_interface();
  if ( wva_ptr_W != NULL )
  {
    rc = wcdma_ext_audio_api( WCDMA_VOICE_CMD_CLOSE, &close_cmd, sizeof( close_cmd ) );
  }
  else
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "wva_wcdma_close_session(): WCDMA SEGMENT NOT LOADED!!");
  }

#else
  rc = wcdma_ext_audio_api( WCDMA_VOICE_CMD_CLOSE, &close_cmd, sizeof( close_cmd ) );
#endif

  if ( rc )
  {
   MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
          "WVA: failed to close WCDMA session for asid[%d], rc[0x%08x]",
          session_obj->asid, rc );
  }

  return rc;
}


static uint32_t wva_wcdma_start_session (
  wva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  wcdma_ivoice_cmd_start_t start_cmd;

  if( NULL == session_obj )
  {
    WVA_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return APR_EBADPARAM;
  }

  start_cmd.handle = session_obj->wcdma_handle;
  rc = wcdma_ext_audio_api ( WCDMA_VOICE_CMD_START, &start_cmd, sizeof( start_cmd ) );
  if ( rc )
  {
    MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
          "WVA: failed to start WCDMA session for asid[%d], rc[0x%08x]",
          session_obj->asid, rc );
  }

  return rc;
}

static uint32_t wva_wcdma_stop_session (
  wva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  wcdma_ivoice_cmd_stop_t stop_cmd;

  if( NULL == session_obj )
  {
    WVA_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
    return APR_EBADPARAM;
  }

  stop_cmd.handle = session_obj->wcdma_handle;
  rc = wcdma_ext_audio_api ( WCDMA_VOICE_CMD_STOP, &stop_cmd, sizeof( stop_cmd ) );
  if ( rc )
  {
   MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
          "WVA: failed to stop WCDMA session for asid[%d], rc[0x%08x]",
          session_obj->asid, rc );
  }

  return rc;
}

static uint32_t wva_wcdma_set_vfr_notification (
  wva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  wcdma_ivoicel1_cmd_set_vfr_notification_t set_vfr_cmd;

  if( NULL == session_obj )
  {
    MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
         "wva_wcdma_set_vfr_notification(): subs_obj is NULL" );
    return APR_EBADPARAM;
  }

  set_vfr_cmd.asid = session_obj->asid;
  set_vfr_cmd.enable_flag = session_obj->is_wcdma_ready;
  rc = wcdma_ext_audio_api ( WCDMA_VOICE_CMD_SET_VFR_NOTIFICATION, &set_vfr_cmd, sizeof( set_vfr_cmd ) );
  if ( rc )
  {
    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "WVA: failed to set VFR Notification enableFlag[%d], asid[%d], rc[0x%08x]",
           set_vfr_cmd.enable_flag, set_vfr_cmd.asid, rc );
  }

  return rc;
}


static uint32_t wva_wcdma_deregister_ul_logical_channels (
  wcdma_ivoice_event_set_logical_channels_t* chan_info,
  wva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  uint8_t nchan = 0;
  voice_amr_chan_state_t* chan_state;
  voice_amr_dsm_queue_t* ul_queue = NULL;
  wcdma_cmd_l2_ul_cmd_buffer_t ul_dereg_cmd_buf;

  for( ;; )
  {
    chan_state = &session_obj->ul_chan_state;
    ul_queue = &session_obj->ul_queue;

#ifndef WINSIM
    /* De-register logical channels if necessary
     *
     * L2 Layer works on command buffer, Get the command packet using 
     * WCDMA_GET_L2_UL_CMD_BUFFER, fill it with Command header 
     * (cmd_hdr.cmd_id) =  RLC_UL_DEREGISTER_SRVC_REQ and fill appropriate 
     * cmd_data.(ul_dereg) members and dispatch it WCDMA_PUT_L2_UL_CMD
     */
     ul_dereg_cmd_buf.service = NULL;
      rc  = wcdma_ext_audio_api( WCDMA_GET_L2_UL_CMD_BUFFER, &ul_dereg_cmd_buf,
                                 sizeof( ul_dereg_cmd_buf ) );
      if( ( rc != WCDMA_STATUS_GOOD ) || ( ul_dereg_cmd_buf.service == NULL ) )
      {
        MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
               "Couldn't get WCMDA L2 uplink command buffer for dereg. "
               "rc = (0x%08X)", rc );
        break;
      }
  
      /*<- command id for deregister these channels */
      ul_dereg_cmd_buf.service->cmd_hdr.cmd_id = RLC_UL_DEREGISTER_SRVC_REQ;
      ul_dereg_cmd_buf.service->cmd_hdr.as_id = session_obj->asid;
 
      /* deregister class A channel if necessary */
      if ( ( chan_state->has_chan_a == FALSE ) || 
           ( chan_state->lca != chan_info->lc_class_a ) )
      {
        if ( chan_state->lca != 0 )
        {
          ul_dereg_cmd_buf.service->cmd_data.ul_dereg.rlc_id[nchan] = chan_state->lca;
          nchan++;
          chan_state->lca = 0;
        }
      }
 
      /* deregister class B channel if necessary */
      if ( ( chan_state->has_chan_b == FALSE ) || 
           ( chan_state->lcb != chan_info->lc_class_b ) )
      {
        if ( chan_state->lcb != 0 )
        {
         ul_dereg_cmd_buf.service->cmd_data.ul_dereg.rlc_id[nchan] = chan_state->lcb;
         nchan++;
         chan_state->lcb = 0;
        }
      }
  
      /* deregister class C channel if necessary */
      if ( ( chan_state->has_chan_c == FALSE ) || 
           ( chan_state->lcc != chan_info->lc_class_c ) )
      {
        if ( chan_state->lcc != 0 )
        {
          ul_dereg_cmd_buf.service->cmd_data.ul_dereg.rlc_id[nchan] = chan_state->lcc;
          nchan++;
          chan_state->lcc = 0;
        }
      }

      if( nchan > 0 )
      {
        ( void ) voice_dsm_amr_q_empty( &session_obj->ul_queue );
         MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
              "DSM UL queue empty during de-registration" );
      }

      ul_dereg_cmd_buf.service->cmd_data.ul_dereg.nchan = nchan;

      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED, 
             "wva_wcdma_deregister_ul_logical_channels(): START: lca=(%d), "
             "lcb=(%d), lcc=(%d)", chan_state->lca, chan_state->lcb, chan_state->lcc );

      rc  = wcdma_ext_audio_api( WCDMA_PUT_L2_UL_CMD, &ul_dereg_cmd_buf,
                                 sizeof( ul_dereg_cmd_buf ) );
      if( rc == WCDMA_STATUS_GOOD )
      {
        MSG_4( MSG_SSID_DFLT, MSG_LEGACY_MED,
               "WCDMA L2 UL De-registration successful. Number of WCDMA L2 "
               "UL channels, nchan = (%d), lca=(%d), lcb=(%d), lcc=(%d)",
               nchan, chan_state->lca, chan_state->lcb, chan_state->lcc );
      }
      else
      {
        MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
               "WCDMA L2 UL De-registration failed, rc = (0x%08X). Number of "
               "WCDMA L2 uplink logical channels, nchan = (%d).", rc, nchan );
      }
#endif

    break;
  }

  return rc;
}

static uint32_t wva_wcdma_register_ul_logical_channels (
  wcdma_ivoice_event_set_logical_channels_t* chan_info,
  wva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  uint8_t nchan = 0;
  voice_amr_chan_state_t* chan_state;
  voice_amr_dsm_queue_t* ul_queue = NULL;
  wcdma_cmd_l2_ul_cmd_buffer_t ul_reg_cmd_buf;

  for( ;; )
  {
    chan_state = &session_obj->ul_chan_state;
    ul_queue = &session_obj->ul_queue;
  
#ifndef WINSIM

   /* Register logical channels if necessary,
    *
    * L2 Layer works on command buffer. Get the command packet using 
    * WCDMA_GET_L2_UL_CMD_BUFFER; Fill it with Command header 
    * (cmd_hdr.cmd_id) =  RLC_UL_REGISTER_SRVC_REQ  fill appropriate 
    * cmd_data.(ul_reg) members and dispatch it WCDMA_PUT_L2_UL_CMD */

   ul_reg_cmd_buf.service = NULL;
    rc  = wcdma_ext_audio_api( WCDMA_GET_L2_UL_CMD_BUFFER, &ul_reg_cmd_buf,
                               sizeof( ul_reg_cmd_buf ) );
    if( ( rc != WCDMA_STATUS_GOOD ) || ( ul_reg_cmd_buf.service == NULL ) )
     {
       MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
              "Couldn't get WCMDA L2 uplink command buffer for reg. "
              "rc = (0x%08X)", rc );
       return APR_EFAILED;
    }
    
    /*<- command id for Register these channels */
    ul_reg_cmd_buf.service->cmd_hdr.cmd_id = RLC_UL_REGISTER_SRVC_REQ;
    ul_reg_cmd_buf.service->cmd_hdr.as_id = session_obj->asid;
   
    /* Register class A channel if necessary */
    if ( ( chan_state->has_chan_a == TRUE ) && ( chan_info->lc_class_a > 0 ) )
    {
      chan_state->lca = chan_info->lc_class_a;
      ul_reg_cmd_buf.service->cmd_data.ul_reg.rlc_data[nchan].lc_id = chan_state->lca;
      ul_reg_cmd_buf.service->cmd_data.ul_reg.rlc_data[nchan].ul_wm_ptr = &ul_queue->wm_a;
      nchan++;
    }
   
    /* Register class B channel if necessary */
    if ( ( chan_state->has_chan_b == TRUE ) && ( chan_info->lc_class_b > 0 ) )
    {
      chan_state->lcb = chan_info->lc_class_b;
      ul_reg_cmd_buf.service->cmd_data.ul_reg.rlc_data[nchan].lc_id = chan_state->lcb;
      ul_reg_cmd_buf.service->cmd_data.ul_reg.rlc_data[nchan].ul_wm_ptr = &ul_queue->wm_b;
      nchan++;
    }
   
    /* Register class C channel if necessary */
    if ( ( chan_state->has_chan_c == TRUE ) && ( chan_info->lc_class_c > 0 ) )
    {
      chan_state->lcc = chan_info->lc_class_c;
      ul_reg_cmd_buf.service->cmd_data.ul_reg.rlc_data[nchan].lc_id = chan_state->lcc;
      ul_reg_cmd_buf.service->cmd_data.ul_reg.rlc_data[nchan].ul_wm_ptr = &ul_queue->wm_c;
      nchan++;
    }
   
    if( nchan > 0 )
    {
      ( void ) voice_dsm_amr_q_empty( &session_obj->ul_queue );
       MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
            "DSM UL queue empty during registration" );
    }

    ul_reg_cmd_buf.service->cmd_data.ul_dereg.nchan = nchan;

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED, 
           "wva_wcdma_register_ul_logical_channels(): START: lca=(%d), lcb=(%d), "
           "lcc=(%d)", chan_state->lca, chan_state->lcb, chan_state->lcc );

    rc  = wcdma_ext_audio_api( WCDMA_PUT_L2_UL_CMD, &ul_reg_cmd_buf,
                               sizeof( ul_reg_cmd_buf ) );
    if( rc == WCDMA_STATUS_GOOD )
    {
      MSG_4( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "WCDMA L2 UL Registration successful. Number of WCDMA L2 "
             "UL channels, nchan = (%d), lca=(%d), lcb=(%d), lcc=(%d) ",
              nchan, chan_state->lca, chan_state->lcb, chan_state->lcc );
    }
    else
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
             "WCDMA L2 UL Registration failed, rc = (0x%08X). Number of "
             "WCDMA L2 uplink logical channels, nchan = (%d).", rc, nchan );
    }
#endif
    break;
  }

  return rc;
}


static uint32_t wva_wcdma_deregister_dl_logical_channels (
  wcdma_ivoice_event_set_logical_channels_t* chan_info,
  wva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  uint8_t nchan = 0;
  voice_amr_chan_state_t* chan_state;
  voice_amr_dsm_queue_t* dl_queue = NULL;
  wcdma_cmd_l2_dl_cmd_buffer_t dl_dereg_cmd_buf;

  for( ;; )
  {
    chan_state = &session_obj->dl_chan_state;
    dl_queue = &session_obj->dl_queue;

#ifndef WINSIM
    /* De-register logical channels if necessary
     *
     * L2 Layer works on command buffer, Get the command packet using 
     * WCDMA_GET_L2_DL_CMD_BUFFER, fill it with Command header 
     * (cmd_hdr.cmd_id) =  RLC_DL_DEREGISTER_SRVC_REQ and fill appropriate 
     * cmd_data.(ul_dereg) members and dispatch it WCDMA_PUT_L2_DL_CMD
     */

     dl_dereg_cmd_buf.service = NULL;
     rc  = wcdma_ext_audio_api( WCDMA_GET_L2_DL_CMD_BUFFER, &dl_dereg_cmd_buf,
                                sizeof( dl_dereg_cmd_buf ) );
     if( ( rc != WCDMA_STATUS_GOOD ) || ( dl_dereg_cmd_buf.service == NULL ) )
     {
       MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
              "Couldn't get WCMDA L2 downlink command buffer for dereg. "
              "rc = (0x%08X)", rc );
       break;
     }

    /*<- command id for deregister these channels */
     dl_dereg_cmd_buf.service->cmd_hdr.cmd_id = RLC_DL_DEREGISTER_SRVC_REQ;
     dl_dereg_cmd_buf.service->cmd_hdr.as_id = session_obj->asid;
    
    
     /* deregister class A channel if necessary */
     if ( ( chan_state->has_chan_a == FALSE ) || 
          ( chan_state->lca != chan_info->lc_class_a ) )
     {
       if ( chan_state->lca != 0 )
       {
         dl_dereg_cmd_buf.service->cmd_data.dl_dereg.rlc_id[nchan] = chan_state->lca;
         nchan++;
         chan_state->lca = 0;
       }
    
     }
    
     /* deregister class B channel if necessary */
     if ( ( chan_state->has_chan_b == FALSE ) || 
          ( chan_state->lcb != chan_info->lc_class_b ) )
     {
       if ( chan_state->lcb != 0 )
       {
        dl_dereg_cmd_buf.service->cmd_data.dl_dereg.rlc_id[nchan] = chan_state->lcb;
        nchan++;
        chan_state->lcb = 0;
       }
    
     }
    
     /* deregister class C channel if necessary */
     if ( ( chan_state->has_chan_c == FALSE ) || 
          ( chan_state->lcc != chan_info->lc_class_c ) )
     {
       if ( chan_state->lcc != 0 )
       {
         dl_dereg_cmd_buf.service->cmd_data.dl_dereg.rlc_id[nchan] = chan_state->lcc;
         nchan++;
         chan_state->lcc = 0;
       }
    
     }

     if( nchan > 0 )
     {
       ( void ) voice_dsm_amr_q_empty( &session_obj->dl_queue );
         MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
              "DSM DL queue empty during de-registration" );
     }

     MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED, 
            "wva_wcdma_deregister_dl_logical_channels(): START: lca=(%d), "
            "lcb=(%d), lcc=(%d)", chan_state->lca, chan_state->lcb,
            chan_state->lcc );

     dl_dereg_cmd_buf.service->cmd_data.dl_dereg.nchan = nchan;
     rc  = wcdma_ext_audio_api( WCDMA_PUT_L2_DL_CMD, &dl_dereg_cmd_buf,
                                sizeof( dl_dereg_cmd_buf ) );
     if( rc == WCDMA_STATUS_GOOD )
     {
       MSG_4( MSG_SSID_DFLT, MSG_LEGACY_MED,
              "WCDMA L2 DL De-registration successful. Number of WCDMA L2 "
              "DL channels, nchan = (%d), lca=(%d), lcb=(%d), lcc=(%d)",
               nchan, chan_state->lca, chan_state->lcb, chan_state->lcc );
     }
     else
     {
       MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
              "WCDMA L2 DL De-registration failed, rc = (0x%08X). Number of "
              "WCDMA L2 downlink logical channels, nchan = (%d).", rc, nchan );
     }
#endif

    break;
  }

  return rc;
}

static uint32_t wva_wcdma_register_dl_logical_channels (
  wcdma_ivoice_event_set_logical_channels_t* chan_info,
  wva_session_object_t* session_obj
)
{
  uint32_t rc = APR_EOK;
  uint8_t nchan = 0;
  voice_amr_chan_state_t* chan_state;
  voice_amr_dsm_queue_t* dl_queue = NULL;
  wcdma_cmd_l2_dl_cmd_buffer_t dl_reg_cmd_buf;

  for( ;; )
  {
    chan_state = &session_obj->dl_chan_state;
    dl_queue = &session_obj->dl_queue;
#ifndef WINSIM

    dl_reg_cmd_buf.service = NULL;
    rc  = wcdma_ext_audio_api( WCDMA_GET_L2_DL_CMD_BUFFER, &dl_reg_cmd_buf,
                               sizeof( dl_reg_cmd_buf ) );
    if( ( rc != WCDMA_STATUS_GOOD ) || ( dl_reg_cmd_buf.service == NULL ) )
     {
       MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
              "Couldn't get WCMDA L2 downlink command buffer for reg. "
              "rc = (0x%08X)", rc );
       return APR_EFAILED;
    }
    
    /*<- command id for Register these channels */
    dl_reg_cmd_buf.service->cmd_hdr.cmd_id = RLC_DL_REGISTER_SRVC_REQ;
    dl_reg_cmd_buf.service->cmd_hdr.as_id = session_obj->asid;
    
    /* Register class A channel if necessary */
    if ( ( chan_state->has_chan_a == TRUE ) && ( chan_info->lc_class_a > 0 ) )
    {
      chan_state->lca = chan_info->lc_class_a;
      dl_reg_cmd_buf.service->cmd_data.dl_reg.rlc_data[nchan].lc_id = chan_state->lca;
      dl_reg_cmd_buf.service->cmd_data.dl_reg.rlc_data[nchan].dl_wm_ptr = &dl_queue->wm_a;
      dl_reg_cmd_buf.service->cmd_data.dl_reg.rlc_data[nchan].context= TRUE;
      dl_reg_cmd_buf.service->cmd_data.dl_reg.rlc_data[nchan].rlc_post_rx_func_ptr_para
        = (void*)session_obj;
      dl_reg_cmd_buf.service->cmd_data.dl_reg.rlc_data[nchan].rlc_post_rx_proc_func_ptr 
        = wva_downlink_channel_data_available;
      nchan++;
    }
    
    /* Register class B channel if necessary */
    if ( ( chan_state->has_chan_b == TRUE ) && ( chan_info->lc_class_b > 0 ) )
    {
      chan_state->lcb = chan_info->lc_class_b;
      dl_reg_cmd_buf.service->cmd_data.dl_reg.rlc_data[nchan].lc_id = chan_state->lcb;
      dl_reg_cmd_buf.service->cmd_data.dl_reg.rlc_data[nchan].dl_wm_ptr = &dl_queue->wm_b;
      dl_reg_cmd_buf.service->cmd_data.dl_reg.rlc_data[nchan].context= TRUE;
      dl_reg_cmd_buf.service->cmd_data.dl_reg.rlc_data[nchan].rlc_post_rx_func_ptr_para
        = (void*)session_obj;
      dl_reg_cmd_buf.service->cmd_data.dl_reg.rlc_data[nchan].rlc_post_rx_proc_func_ptr 
        = wva_downlink_channel_data_available;
      nchan++;
    }
    
    /* Register class C channel if necessary */
    if ( ( chan_state->has_chan_c == TRUE ) && ( chan_info->lc_class_c > 0 ) )
    {
      chan_state->lcc = chan_info->lc_class_c;
      dl_reg_cmd_buf.service->cmd_data.dl_reg.rlc_data[nchan].lc_id = chan_state->lcc;
      dl_reg_cmd_buf.service->cmd_data.dl_reg.rlc_data[nchan].dl_wm_ptr = &dl_queue->wm_c;
      dl_reg_cmd_buf.service->cmd_data.dl_reg.rlc_data[nchan].context= TRUE;
      dl_reg_cmd_buf.service->cmd_data.dl_reg.rlc_data[nchan].rlc_post_rx_func_ptr_para
        = (void*)session_obj;
      dl_reg_cmd_buf.service->cmd_data.dl_reg.rlc_data[nchan].rlc_post_rx_proc_func_ptr 
        = wva_downlink_channel_data_available;
      nchan++;
    }
    
    if( nchan > 0 )
    {
      ( void ) voice_dsm_amr_q_empty( &session_obj->dl_queue );
         MSG( MSG_SSID_DFLT, MSG_LEGACY_MED,
              "DSM DL queue empty during registration" );
    }

    dl_reg_cmd_buf.service->cmd_data.dl_dereg.nchan = nchan;

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "wva_wcdma_register_dl_logical_channels(): START: lca=(%d), lcb=(%d), "
           "lcc=(%d)", chan_state->lca, chan_state->lcb, chan_state->lcc );

    rc  = wcdma_ext_audio_api( WCDMA_PUT_L2_DL_CMD, &dl_reg_cmd_buf,
                               sizeof( dl_reg_cmd_buf ) );
    if( rc == WCDMA_STATUS_GOOD )
    {
     MSG_4( MSG_SSID_DFLT, MSG_LEGACY_MED,
            "WCDMA L2 DL Registration successful. Number of WCDMA L2 "
            "DL channels, nchan = (%d), lca=(%d), lcb=(%d), lcc=(%d)",
             nchan, chan_state->lca, chan_state->lcb, chan_state->lcc );
    }
    else
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR, 
             "WCDMA L2 DL Registration failed, rc = (0x%08X). Number of "
             "WCDMA L2 downlink logical channels, nchan = (%d).", rc, nchan );
    }
#endif

    break;
  }

  return rc;
}


/****************************************************************************
 * WVA CMDs/EVENTs HANDLING ROUTINES                                        *
 ****************************************************************************/

static uint32_t wva_process_vs_cmd_response_event( 
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  wva_session_object_t* session_obj = NULL;
  wva_simple_job_object_t* simple_obj = NULL;
  vs_common_event_cmd_response_t* evt_params = NULL;

  for ( ;; )
  {
    if( NULL == packet )
    {
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      rc = APR_EUNEXPECTED;
      break;
    }

    session_obj = ( wva_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    evt_params = APRV2_PKT_GET_PAYLOAD( vs_common_event_cmd_response_t, packet );
    if ( NULL == evt_params )
    {
      WVA_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rc = APR_EFAILED;
      break;
    }

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
           "WVA: cmd response recieved for cmd_id[0x%08x], token[0x%08x], status[0x%08X]",
           evt_params->cmd_id, evt_params->token, evt_params->status );

    simple_obj = ( wva_simple_job_object_t* ) evt_params->token;
    if ( simple_obj == NULL ) break;

    simple_obj->is_completed = TRUE;
    simple_obj->status = evt_params->status;

    break;
  }

  rc = __aprv2_cmd_free( wva_apr_handle, packet );
  WVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}

static uint32_t wva_process_vs_ready_event( 
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  wva_session_object_t* session_obj = NULL;

  for ( ;; )
  {
    if( NULL == packet )
    {
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      rc = APR_EUNEXPECTED;
      break;
    }

    session_obj = ( wva_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    WVA_ACQUIRE_LOCK( session_obj->data_lock );

    session_obj->is_vs_ready = TRUE;
    wva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_READY );

    WVA_RELEASE_LOCK( session_obj->data_lock );

    break;
  }

  rc = __aprv2_cmd_free( wva_apr_handle, packet );
  WVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}


static uint32_t wva_process_vs_not_ready_event( 
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  wva_session_object_t* session_obj = NULL;
  vs_pktexg_cmd_flush_buffers_t flushCmd;

  for ( ;; )
  {
    if( NULL == packet )
    {
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      rc = APR_EUNEXPECTED;
      break;
    }

    session_obj = ( wva_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    WVA_ACQUIRE_LOCK( session_obj->data_lock );

    session_obj->is_vs_ready = FALSE;
    wva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_NOT_READY );

    if( APR_NULL_V != session_obj->vs_handle )
    {
      flushCmd.handle = session_obj->vs_handle;
      flushCmd.enc_flush = TRUE;
      flushCmd.dec_flush = TRUE;
      rc = vs_call( VS_PKTEXG_CMD_FLUSH_BUFFERS, &flushCmd, sizeof( flushCmd ) );
      WVA_REPORT_FATAL_ON_ERROR(rc);
    }

    WVA_RELEASE_LOCK( session_obj->data_lock );

    break;
  }

  rc = __aprv2_cmd_free( wva_apr_handle, packet );
  WVA_REPORT_FATAL_ON_ERROR(rc);

  return VS_EOK;
}


static uint32_t wva_process_vs_eamr_rate_change_event( 
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  wva_session_object_t* session_obj = NULL;
  vs_voc_event_operating_mode_update_t* evt_params = NULL;
  wcdma_ivoice_cmd_send_sample_rate_t sample_rate_cmd;

  for ( ;; )
  {
    if( NULL == packet )
    {
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      rc = APR_EUNEXPECTED;
      break;
    }

    session_obj = ( wva_session_object_t* ) packet->token;
    if( session_obj == NULL )
    {
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    evt_params = APRV2_PKT_GET_PAYLOAD( vs_voc_event_operating_mode_update_t, packet );
    if( evt_params == NULL )
    {
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    if( session_obj->vocoder_type != WCDMA_IVOCODER_ID_AMR )
    {
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "wva_process_vs_eamr_rate_change_event(): Not Applicable for "
             "vocoder_type=0x%08x", session_obj->vocoder_type );
      break;
    }

    switch ( evt_params->rx_mode )
    {
      case VS_VOC_BANDWIDTH_NB:
        sample_rate_cmd.sample_rate = WVA_VOICE_SAMPLE_RATE_NB_V;
        break;

      case VS_VOC_BANDWIDTH_WB:
        sample_rate_cmd.sample_rate = WVA_VOICE_SAMPLE_RATE_WB_V;
        break;

      default:
        sample_rate_cmd.sample_rate = WVA_VOICE_SAMPLE_RATE_UNDEFINED_V;
        break;
    }

    if( ( evt_params->rx_mode == VS_VOC_BANDWIDTH_NB ) ||
        ( evt_params->rx_mode == VS_VOC_BANDWIDTH_WB ) )
    {
      sample_rate_cmd.handle = session_obj->wcdma_handle;
      sample_rate_cmd.vocoder_id = session_obj->vocoder_type;
      rc = wcdma_ext_audio_api( WCDMA_VOICE_CMD_SEND_SAMPLE_RATE,
                                &sample_rate_cmd, sizeof( sample_rate_cmd ) );
      MSG_3( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "WVA: sample rate change notification for asid[%d] rx_mode[%d] sample_rate[%d]",
             session_obj->asid, evt_params->rx_mode, sample_rate_cmd.sample_rate );
    }

    break;
  }

  rc = __aprv2_cmd_free( wva_apr_handle, packet );
  WVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}

static uint32_t wva_process_wcdma_scr_mode_event( 
  aprv2_packet_t* packet 
)
{
  uint32_t rc = APR_EOK;
  wva_session_object_t* session_obj = NULL;
  wcdma_ivoice_event_request_scr_mode_t* evt_params = NULL;

  for ( ;; )
  {
    if( NULL == packet )
    {
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( wva_session_object_t* ) packet->token;
    if ( session_obj == NULL )
    {
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    evt_params = APRV2_PKT_GET_PAYLOAD( wcdma_ivoice_event_request_scr_mode_t, packet );
    if ( NULL == evt_params )
    {
      WVA_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rc = APR_EFAILED;
      break;
    }

    WVA_ACQUIRE_LOCK( session_obj->data_lock );

    session_obj->dtx_mode = evt_params->enable_flag;
    ( void ) wva_vs_set_voc_dtx_mode ( session_obj );

    WVA_RELEASE_LOCK( session_obj->data_lock );

    break;
  }

  rc = __aprv2_cmd_free( wva_apr_handle, packet );
  WVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}


static uint32_t wva_process_wcdma_select_owner_event( 
  aprv2_packet_t* packet 
)
{
  uint32_t rc = APR_EOK;
  wva_session_object_t* session_obj = NULL;
  vagent_ivoice_cmd_resource_request_t requestResourceCmd;

  for ( ;; )
  {
    if( NULL == packet )
    {
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( wva_session_object_t* ) packet->token;
    if ( session_obj == NULL )
    {
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    WVA_ACQUIRE_LOCK( session_obj->data_lock );

    /* FOR IRAT-HO Scenrios,request voice agent for resource grant. */
    if ( session_obj->is_resource_granted == FALSE )
    {
      requestResourceCmd.asid = session_obj->asid;
      requestResourceCmd.client_id = VAGENT_VOICE_CLIENT_WCDMA;
      rc = vagent_call( VAGENT_IVOICE_CMD_RESOURCE_REQUEST, (void*)&requestResourceCmd,
                        sizeof(requestResourceCmd) );
      WVA_REPORT_FATAL_ON_ERROR(rc);

      wva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_REQUESTED );
    }

    WVA_RELEASE_LOCK( session_obj->data_lock );

    break;
  }

  rc = __aprv2_cmd_free( wva_apr_handle, packet );
  WVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}

static uint32_t wva_process_wcdma_codec_mode_event( 
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  wva_session_object_t* session_obj = NULL;
  wcdma_ivoice_event_request_codec_mode_t* evt_params = NULL;

  for ( ;; )
  {
    if( NULL == packet )
    {
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( wva_session_object_t* ) packet->token;
    if ( session_obj == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "wva_process_wcdma_codec_mode_event(): subs_obj is NULL" );
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    evt_params = APRV2_PKT_GET_PAYLOAD( wcdma_ivoice_event_request_codec_mode_t, packet );
    if ( NULL == evt_params )
    {
      WVA_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rc = APR_EFAILED;
      break;
    }

    WVA_ACQUIRE_LOCK( session_obj->data_lock );

    session_obj->codec_mode = evt_params->codec_mode;
    ( void ) wva_vs_set_voc_codec_mode ( session_obj );

    WVA_RELEASE_LOCK( session_obj->data_lock );

    break;
  }

  rc = __aprv2_cmd_free( wva_apr_handle, packet );
  WVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}


static uint32_t wva_process_wcdma_set_logical_channels_event( 
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  uint32_t chan_class;
  voice_amr_chan_state_t* chan_state = NULL;
  wva_session_object_t* session_obj = NULL;
  wcdma_ivoice_event_set_logical_channels_t* evt_params = NULL;

  for ( ;; )
  {
    if( NULL == packet )
    {
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( wva_session_object_t* ) packet->token;
    if ( session_obj == NULL )
    {
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      rc = APR_EFAILED;
      break;
    }

    evt_params = APRV2_PKT_GET_PAYLOAD( wcdma_ivoice_event_set_logical_channels_t, packet );
    if ( NULL == evt_params )
    {
      WVA_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rc = APR_EFAILED;
      break;
    }

    switch ( evt_params->direction )
    {
      /* Register/Dereg Downlink logical channels. */
      case FALSE:
      {
        /* Update the channle state for DSM queue. */
        chan_class = evt_params->class_type;
        chan_state = &session_obj->dl_chan_state;
        rc = wva_update_chan_state( chan_state, chan_class );
        if( rc ) break;

        ( void ) wva_wcdma_deregister_dl_logical_channels( evt_params, session_obj );
        ( void ) wva_wcdma_register_dl_logical_channels( evt_params, session_obj );
      }
      break;

      /* Register/Dereg Downlink logical channels. */
      case TRUE:
      {
        /* Update the channle state for DSM queue. */
        chan_class = evt_params->class_type;
        chan_state = &session_obj->ul_chan_state;
        rc = wva_update_chan_state( chan_state, chan_class );
        if( rc ) break;

        ( void ) wva_wcdma_deregister_ul_logical_channels( evt_params, session_obj );
        ( void ) wva_wcdma_register_ul_logical_channels( evt_params, session_obj );
      }
      break;
    }

    break;
  }

  rc = __aprv2_cmd_free( wva_apr_handle, packet );
  WVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}


static uint32_t wva_process_wcdma_l1_vfr_event( 
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  int8_t timetick_diff = 0;
  int64_t timing_offset_us = 0;
  wva_session_object_t* session_obj = NULL;
  wcdma_ivoicel1_event_vfr_notification_t* evt_params = NULL;

  for ( ;; )
  {
    if( NULL == packet )
    {
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    session_obj = ( wva_session_object_t* ) packet->token;
    if ( session_obj == NULL )
    {
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    evt_params = APRV2_PKT_GET_PAYLOAD( wcdma_ivoicel1_event_vfr_notification_t, packet );
    if ( NULL == evt_params )
    {
      WVA_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rc = APR_EFAILED;
      break;
    }

    MSG_3( MSG_SSID_DFLT, MSG_LEGACY_LOW,
           "wva_process_wcdma_l1_vfr_event(): vfr_state=(%d), cfn_value=(%d), "
           "stmr_tick_count=(%d)", evt_params->vfr_state, evt_params->cfn_value,
           evt_params->stmr_tick_count );

    if ( ( evt_params->cfn_value % 2 ) == 0 )
    {
      /* Calculate the timing offset in stmr timer tick count.*/
      timetick_diff = ( int8_t ) evt_params->stmr_tick_count;

      /* Converting ticks to microsec. 150 ticks equals 10 milli-sec, or 150 ticks 
       * equals 10000 micro-sec or 1 tick equals 1000/15 or 200/3 microsec or 
       * 66.666 microsec . */
      timing_offset_us = (int64_t) (WVA_GET_DL_PACKET_FROM_DSM_OFFSET_US );
      timing_offset_us -= ( int64_t )( ( 1.0 * timetick_diff ) * 66.66 );
    }
    else
    {
      /* Calculate the timing offset in stmr timer tick count.*/
      timetick_diff = ( int8_t ) evt_params->stmr_tick_count;

      /* Converting ticks to microsec. 150 ticks equals 10 milli-sec, or 150 ticks 
       * equals 10000 micro-sec or 1 tick equals 1000/15 or 200/3 microsec or 
       * 66.666 microsec. */
      timing_offset_us = WVA_WCDMA_READS_UL_PACKET_OFFSET_US - WVA_PUT_UL_PACKET_INTO_DSM_OFFSET_US;
      timing_offset_us -= ( int64_t )( ( 1.0 * timetick_diff ) * 66.66 );
    }

    if ( timing_offset_us <= 0 )
    {
      WVA_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      break;
    }

    session_obj->cfn_value = evt_params->cfn_value;
    rc = apr_timer_start( session_obj->pktexg_timer, (timing_offset_us*1000) );
    WVA_REPORT_FATAL_ON_ERROR(rc);

    break;
  }

  rc = __aprv2_cmd_free( wva_apr_handle, packet );
  WVA_REPORT_FATAL_ON_ERROR(rc);

  return rc;
}


static uint32_t wva_process_deliver_dl_buffer_event( 
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  wva_session_object_t* session_obj = NULL;
  vs_pktexg_cmd_flush_buffers_t flushCmd;
  uint8_t* frameHeader;

  for ( ;; )
  {
    if( NULL == packet )
    {
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      rc = APR_EUNEXPECTED;
      break;
    }

    session_obj = ( wva_session_object_t* ) packet->token;
    if ( session_obj == NULL )
    {
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      rc = APR_EFAILED;
      break;
    }

    if( FALSE == session_obj->is_vs_ready )
    {
      ( void ) voice_dsm_amr_q_empty( &session_obj->dl_queue );
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "WVA: droping DL vocoder packet as is_vs_ready[%d]", session_obj->is_vs_ready );
      rc = APR_EFAILED;
      break;
    }

    rc = wva_vs_get_decoder_buffer( session_obj );
    if ( NULL == session_obj->vs_write_buf )
    {
      ( void ) voice_dsm_amr_q_empty( &session_obj->dl_queue );
      WVA_REPORT_FATAL_ON_ERROR ( APR_ENEEDMORE );
      rc = APR_EFAILED;
      break;
    }

    if( TRUE == session_obj->is_dl_buffer_ready )
    {
      /* Get the vocoder data from the DSM queue. */
      rc = wva_get_dl_vocoder_packet( session_obj );
    }
    else
    {
      rc = wva_get_no_data_vocoder_packet( session_obj );
    }
    session_obj->is_dl_buffer_ready = FALSE;

    if ( rc ) 
    {
      WVA_REPORT_FATAL_ON_ERROR(rc);
      break;
    }

    frameHeader = ( (uint8_t*)session_obj->vs_write_buf + sizeof(vs_pktexg_buffer_t) );
    ( void ) wva_log_vocoder_frame( session_obj, VOICELOG_IPACKET_CODE_DL_VOCODER_PACKET,
                                    *frameHeader, frameHeader+1, session_obj->vs_write_buf->frame_size-1 );

    flushCmd.handle = session_obj->vs_handle;
    flushCmd.enc_flush = FALSE; flushCmd.dec_flush = TRUE;
    rc = vs_call( VS_PKTEXG_CMD_FLUSH_BUFFERS, &flushCmd, sizeof( flushCmd ) );
    WVA_REPORT_FATAL_ON_ERROR(rc);

    /* Pass the vocoder buffer to VS from rendering. */
    rc = wva_vs_put_decoder_buffer( session_obj );
    break;
  }

  rc = __aprv2_cmd_free( wva_apr_handle, packet );
  WVA_REPORT_FATAL_ON_ERROR(rc)

  return rc;
}


static uint32_t wva_process_deliver_ul_buffer_event( 
  aprv2_packet_t* packet
)
{
  uint32_t rc = APR_EOK;
  uint8_t* frameHeader;
  wva_session_object_t* session_obj = NULL;

  for ( ;; )
  {
    if( NULL == packet )
    {
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      rc = APR_EUNEXPECTED;
      break;
    }

    session_obj = ( wva_session_object_t* ) packet->token;
    if ( session_obj == NULL )
    {
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
       rc = APR_EFAILED;
      break;
    }

    if(TRUE == session_obj->is_vs_ready)
    {
      rc = wva_vs_get_encoder_buffer ( session_obj );
    }
	
    if ( NULL != session_obj->vs_read_buf )
    {
      session_obj->enc_packet_miss_count = 0;
      ( void ) wva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_RUNNING );
      ( void ) wva_deliver_ul_vocoder_packet( session_obj );
      frameHeader = ( (uint8_t*)session_obj->vs_read_buf + sizeof(vs_pktexg_buffer_t) );
      ( void ) wva_log_vocoder_frame( session_obj, VOICELOG_IPACKET_CODE_UL_VOCODER_PACKET,
                                      *frameHeader, frameHeader+1, session_obj->vs_read_buf->frame_size-1);
      ( void ) wva_vs_return_encoder_buffer( session_obj );
    }
    else if( session_obj->enc_packet_miss_count >= WVA_ENC_PKT_MISS_THRESHOLD )
    {
      ( void ) wva_deliver_ul_silence_vocoder_packet( session_obj );
      break;
    }
    else
    {
      session_obj->enc_packet_miss_count++;
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "WVA: UL vocoder Packet not transfered to WCDMA is_vs_ready[%d] readBuf[0x%08x]",
             session_obj->is_vs_ready, session_obj->vs_read_buf );
    }

    break;
  }

  rc = __aprv2_cmd_free( wva_apr_handle, packet );
  WVA_REPORT_FATAL_ON_ERROR(rc)

  return rc;
}


static uint32_t wva_process_wcdma_vocoder_start_event( 
 wva_gating_control_t* ctrl 
)
{
  uint32_t rc = APR_EOK;
  aprv2_packet_t* packet = ctrl->packet;
  wva_session_object_t* session_obj = NULL;
  wcdma_ivoice_event_request_start_t* evt_params = NULL;
  wva_simple_job_object_t* simple_obj = NULL;
  vagent_ivoice_cmd_resource_request_t requestResourceCmd;

  for ( ;; )
  {
    if( NULL == packet )
    {
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      rc = APR_EUNEXPECTED;
      break;
    }

    session_obj = ( wva_session_object_t* ) packet->token;
    if ( session_obj == NULL )
    {
      MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "wva_process_wcdma_codec_mode_event(): subs_obj is NULL" );
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      break;
    }

    evt_params = APRV2_PKT_GET_PAYLOAD( wcdma_ivoice_event_request_start_t, packet );
    if ( NULL == evt_params )
    {
      WVA_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rc = APR_EFAILED;
      break;
    }

    WVA_ACQUIRE_LOCK( session_obj->data_lock );

    if ( ctrl->state == WVA_GATING_CMD_STATE_EXECUTE )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_MED,
             "wva_process_wcdma_vocoder_start_event() - configured vocoder "
             "id = (0x%08x), requested vocoder id = (0x%08x)",
             session_obj->vocoder_type, evt_params->vocoder_id );

      if ( session_obj->is_wcdma_ready == TRUE )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "wva_process_wcdma_vocoder_start_event() - WCDMA already sent IVOICE_EVENT_REQUEST_START");
        break;
      }

      if ( evt_params->vocoder_id != WCDMA_IVOCODER_ID_NONE )
      {
        /* During Inter-frequency HHO scenrios, reuse the vocoder_type 
           set initially by WRRC. */
        session_obj->vocoder_type = evt_params->vocoder_id;
      }

      /* Vaildate the vocoder_type and check if the Logical channel are 
         configured to adapter by WRRC, this is to avoid multiple start/stop 
         from WL1 during add/drop state machine. */
      if ( ( session_obj->vocoder_type == WVA_VOCODER_ID_UNDEFINED_V ) ||
           ( session_obj->ul_chan_state.has_chan_a == FALSE ) ||
           ( session_obj->dl_chan_state.has_chan_a == FALSE ) )
      {
         MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
                "wva_process_wcdma_vocoder_start_event() - Invalid vocoder "
                "type = (0x%08x) configured, ul_dsm_state=%d, dl_dsm_state=%d",
                session_obj->vocoder_type, session_obj->ul_chan_state.has_chan_a,
                session_obj->dl_chan_state.has_chan_a );
         break;
      }

      /* Update the WCDMA state. */
      session_obj->is_wcdma_ready = TRUE;
      session_obj->enc_packet_miss_count = 0;

      if ( session_obj->is_resource_granted == FALSE )
      {
        /* Request for voice resource. */
        requestResourceCmd.asid = session_obj->asid;
        requestResourceCmd.client_id = VAGENT_VOICE_CLIENT_WCDMA;
        rc = vagent_call( VAGENT_IVOICE_CMD_RESOURCE_REQUEST, (void*)&requestResourceCmd,
                          sizeof(requestResourceCmd) );
        WVA_REPORT_FATAL_ON_ERROR(rc);

        wva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_REQUESTED );
        break;
      }

      rc = wva_vs_open_vocoder_session( session_obj );
      WVA_PANIC_ON_ERROR(rc);

      /* Set cached vocoder properties. */
      wva_vs_set_voc_media_type( session_obj );
      wva_vs_set_voc_dtx_mode( session_obj );
      wva_vs_set_voc_codec_mode( session_obj );
      wva_vs_set_voc_timing_offsets( session_obj );
      /* Create the Simple job object to track VS_ENABLE. */
      rc = wva_create_simple_job_object( session_obj->header.handle,
             ( wva_simple_job_object_t** ) &ctrl->rootjob_obj );
      simple_obj = &ctrl->rootjob_obj->simple_job;
      
      rc = wva_vs_enable_vocoder( session_obj, simple_obj );
    }
    else
    {
      simple_obj = &ctrl->rootjob_obj->simple_job;
      if( simple_obj->is_completed != TRUE )
      {
        rc = APR_EPENDING;
        break;
      }

      ( void ) wva_wcdma_start_session( session_obj );
      ( void ) wva_wcdma_set_vfr_notification( session_obj );
    }

    break;
  }

  /* If the return code is not APR_EPENDING it confirm that nothing is left for
   * hence memory associated to CMD packet, EVENT packet and JOB object created
   * shall be relesed. Control shall not return to this handler for a perticular
   * Insance of event. */
  if ( APR_EPENDING != rc ) 
  {
    rc = APR_EOK;

    /* Free CMD/EVT packet memory. */
    ( void ) wva_mem_free_object ( ( wva_object_t*) simple_obj );
  }

  if ( session_obj != NULL )
  {
    WVA_RELEASE_LOCK( session_obj->data_lock );
  }

  return rc;
}


static uint32_t wva_process_wcdma_vocoder_stop_event( 
 wva_gating_control_t* ctrl 
)
{
  uint32_t rc = APR_EOK;
  aprv2_packet_t* packet = ctrl->packet;
  wva_session_object_t* session_obj = NULL;
  wva_simple_job_object_t* simple_obj = NULL;
  vagent_ivoice_cmd_resource_release_t releaseResourceCmd;

  for ( ;; )
  {
    if( NULL == packet )
    {
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      rc = APR_EUNEXPECTED;
      break;
    }
 
    session_obj = ( wva_session_object_t* ) packet->token;
    if ( session_obj == NULL )
    {
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      rc = APR_EUNEXPECTED;
      break;
    }

    WVA_ACQUIRE_LOCK( session_obj->data_lock );

    if ( ctrl->state == WVA_GATING_CMD_STATE_EXECUTE )
    {
      /* If the WRRC de-registeres the logical channles, update the session
         vocoder type as undefined, to ignore multiple start/stopn from WL1
         during add/drop state machine. */
      if ( ( session_obj->ul_chan_state.has_chan_a == FALSE ) &&
           ( session_obj->dl_chan_state.has_chan_a == FALSE ) )
      {
        session_obj->vocoder_type = WVA_VOCODER_ID_UNDEFINED_V;
        MSG_3( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
               "wva_process_wcdma_vocoder_stop_event(): Invalidate configured "
               "type = (0x%08x), ul_dsm_state=%d, dl_dsm_state=%d",
               session_obj->vocoder_type, session_obj->ul_chan_state.has_chan_a,
               session_obj->dl_chan_state.has_chan_a );
      }

      if ( FALSE == session_obj->is_wcdma_ready )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
             "wcdma_vocoder_stop_event(PROCESS): Vocoder not started yet" );
        break;
      }

      /* Update the WCDMA state. */
      session_obj->is_wcdma_ready = FALSE;
      if ( TRUE == session_obj->is_resource_granted )
      {
        /* Create the Simple job object to track CVD setup. */
        rc = wva_create_simple_job_object( session_obj->header.handle,
               ( wva_simple_job_object_t** ) &ctrl->rootjob_obj );
        simple_obj = &ctrl->rootjob_obj->simple_job;

        rc = wva_vs_close_vocoder_session( session_obj, simple_obj );
      }
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

    break;
  }

  if ( APR_EOK == rc )
  {
    /* Send voice resource released event to voice agent.
     * "REQUEST_STOP sent by W-RRC without sending REQUEST_START" 
     * This can happen if W-RRC called SELECT_OWNER during interRAT 
     * handover start. However the handover failed afterwards. So, W-RRC 
     * did not call REQUEST_START, instead directly called REQUEST_STOP 
     * to indicate WVA that it no longer required vocoder. 
     */
     session_obj->is_resource_granted = FALSE;
     releaseResourceCmd.asid = session_obj->asid;
     releaseResourceCmd.client_id = VAGENT_VOICE_CLIENT_WCDMA;
     rc = vagent_call( VAGENT_IVOICE_CMD_RESOURCE_RELEASE, (void*)&releaseResourceCmd,
                       sizeof(releaseResourceCmd) );
     WVA_REPORT_FATAL_ON_ERROR(rc);

     wva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_RELEASED );
  }

  /* If the return code is not APR_EPENDING it confirm that nothing is left for
   * hence memory associated to CMD packet, EVENT packet and JOB object created
   * shall be relesed. Control shall not return to this handler for a perticular
   * Insance of event. */
  if ( APR_EPENDING != rc ) 
  {
    rc = APR_EOK;
    /* Send STOP and VFR Disable notfication to WCDMA. */
    ( void ) wva_wcdma_stop_session ( session_obj );
    ( void ) wva_wcdma_set_vfr_notification ( session_obj );

    /* Free CMD/EVT packet memory. */
    ( void ) wva_mem_free_object ( ( wva_object_t*) simple_obj );
  }

  if ( session_obj != NULL )
  {
    WVA_RELEASE_LOCK( session_obj->data_lock );
  }

  return rc;
}

static uint32_t wva_va_event_cb (
  uint32_t event_id,
  void* params,
  uint32_t size,
  void* client_data
 )

{
  uint32_t rc = APR_EOK;

  switch ( event_id )
  {
    case VAGENT_IVOICE_EVENT_RESOURCE_GRANT:
    case VAGENT_IVOICE_EVENT_RESOURCE_REVOKE:
      rc = wva_prepare_and_dispatch_cmd_packet( NULL, event_id, params, size );
      break;

    default:
      MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR, "WVA: Unsupported EventId=(%d)", event_id );
      rc = APR_EFAILED;
  }

  return rc;
}

static uint32_t wva_process_resource_grant_cmd( 
 wva_gating_control_t* ctrl 
)
{
  uint32_t rc = APR_EOK;
  aprv2_packet_t* packet = ctrl->packet;
  wva_session_object_t* session_obj = NULL;
  vagent_ivoice_event_resource_grant_t* event_params = NULL;
  wva_simple_job_object_t* simple_obj = NULL;

  for ( ;; )
  {
    if( NULL == packet )
    {
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      rc = APR_EUNEXPECTED;
      break;
   }

    event_params = APRV2_PKT_GET_PAYLOAD( vagent_ivoice_event_resource_grant_t, packet );
    if ( NULL == event_params )
    {
      WVA_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rc = APR_EFAILED;
      break;
    }

    session_obj = wva_session_obj_list[event_params->asid];
    if( session_obj == NULL )
    {
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      rc = APR_EOK;
      break;
    }

    WVA_ACQUIRE_LOCK( session_obj->data_lock );

    if ( ctrl->state == WVA_GATING_CMD_STATE_EXECUTE )
    {
      session_obj->is_resource_granted = TRUE;
      session_obj->vsid = event_params->vsid;
      wva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_GRANTED );

      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "WVA: ASID[%d]: VSID[0x%08x]: VAGENT_IVOICE_EVENT_RESOURCE_GRANT recieved",
             session_obj->asid, session_obj->vsid );

      if ( session_obj->is_wcdma_ready == FALSE )
      {
        MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "wva_process_resource_grant_cmd() - IVOICE_EVENT_REQUEST_START not "
             "received from WCDMA" );
        break;
      }

      rc = wva_vs_open_vocoder_session( session_obj );
      WVA_PANIC_ON_ERROR(rc);

      /* Set cached vocoder properties. */
      wva_vs_set_voc_media_type( session_obj );
      wva_vs_set_voc_dtx_mode( session_obj );
      wva_vs_set_voc_codec_mode( session_obj );
      wva_vs_set_voc_timing_offsets( session_obj );

      /* Create the Simple job object to track VS_ENABLE. */
      rc = wva_create_simple_job_object( session_obj->header.handle,
             ( wva_simple_job_object_t** ) &ctrl->rootjob_obj );
      simple_obj = &ctrl->rootjob_obj->simple_job;

      rc = wva_vs_enable_vocoder( session_obj, simple_obj );
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

    if ( rc == APR_EOK )
    {
      ( void ) wva_wcdma_start_session( session_obj );
      ( void ) wva_wcdma_set_vfr_notification( session_obj );
      ( void ) wva_mem_free_object ( ( wva_object_t*) simple_obj );
    }

    break;
  }

  if ( session_obj != NULL )
  {
    WVA_RELEASE_LOCK( session_obj->data_lock );
  }

  return rc;
}


static uint32_t wva_process_resource_revoke_cmd( 
 wva_gating_control_t* ctrl 
)
{
  uint32_t rc = APR_EOK;
  aprv2_packet_t* packet = ctrl->packet;
  wva_session_object_t* session_obj = NULL;
  vagent_ivoice_event_resource_revoke_t* event_params = NULL;
  wva_simple_job_object_t* simple_obj = NULL;
  vagent_ivoice_cmd_resource_release_t releaseResourceCmd;

  for ( ;; )
  {
    if( NULL == packet )
    {
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      rc = APR_EUNEXPECTED;
      break;
    }

    event_params = APRV2_PKT_GET_PAYLOAD( vagent_ivoice_event_resource_revoke_t, packet );
    if ( NULL == event_params )
    {
      WVA_REPORT_FATAL_ON_ERROR( VS_EUNEXPECTED );
      rc = APR_EFAILED;
      break;
    }

    session_obj = wva_session_obj_list[event_params->asid];
    if( session_obj == NULL )
    {
      WVA_REPORT_FATAL_ON_ERROR ( APR_EUNEXPECTED );
      rc = APR_EOK;
      break;
    }

    WVA_ACQUIRE_LOCK( session_obj->data_lock );

    if ( ctrl->state == WVA_GATING_CMD_STATE_EXECUTE )
    {
      MSG_2( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
             "WVA: ASID[%d]: VSID[0x%08x]: VAGENT_IVOICE_EVENT_RESOURCE_REVOKE recieved",
             session_obj->asid, session_obj->vsid );

      /* Create the Simple job object to track VS_DISABLE. */
      rc = wva_create_simple_job_object( session_obj->header.handle,
             ( wva_simple_job_object_t** ) &ctrl->rootjob_obj );
      simple_obj= &ctrl->rootjob_obj->simple_job;

      rc = wva_vs_close_vocoder_session( session_obj, simple_obj );
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

    if ( VS_EOK == rc )
    {
      session_obj->is_resource_granted = FALSE;
      releaseResourceCmd.asid = session_obj->asid;
      releaseResourceCmd.client_id = VAGENT_VOICE_CLIENT_WCDMA;
      rc = vagent_call( VAGENT_IVOICE_CMD_RESOURCE_RELEASE, (void*)&releaseResourceCmd,
                        sizeof(releaseResourceCmd) );
      WVA_REPORT_FATAL_ON_ERROR(rc);

      ( void ) wva_publish_vocoder_state( session_obj, VOICELOG_IEVENT_VOCODER_STATE_RELEASED );
      ( void ) wva_mem_free_object ( ( wva_object_t*) simple_obj );
    }

    break;
  }

  if ( session_obj != NULL )
  {
    WVA_RELEASE_LOCK( session_obj->data_lock );
  }

  return rc;
}


/****************************************************************************
 * NONGATING REQUEST( CMDs/EVENTs ) PROCESSING FUNCTIONS
 ****************************************************************************/

static void wva_task_process_nongating_work_items ( void )
{
  uint32_t rc = APR_EOK;
  wva_work_item_t* work_item = NULL;
  aprv2_packet_t* packet = NULL;

  while( apr_list_remove_head( &wva_nongating_work_pkt_q,
                               ( ( apr_list_node_t** ) &work_item ) ) == APR_EOK )
  {
    packet = work_item->packet;
    work_item->packet = NULL;
    ( void ) apr_list_add_tail( &wva_free_work_pkt_q, &work_item->link );

    switch( packet->opcode )
    {
      /* Handling routine for nongating work-items should take of release the
       * memory allocated for the CMD/EVENT packets.*/
      case WCDMA_IVOICE_EVENT_REQUEST_CODEC_MODE:
        rc = wva_process_wcdma_codec_mode_event( packet );
        break;

      case WCDMA_IVOICE_EVENT_REQUEST_SCR_MODE:
        rc = wva_process_wcdma_scr_mode_event( packet );
        break;

      case WCDMA_IVOICE_EVENT_SELECT_OWNER:
        rc = wva_process_wcdma_select_owner_event( packet );
        break;
 
      case WCDMA_IVOICE_EVENT_SET_LOGICAL_CHANNELS:
        rc = wva_process_wcdma_set_logical_channels_event( packet );
        break;
 
      case WCDMA_IVOICEL1_EVENT_VFR_NOTIFICATION:
        rc = wva_process_wcdma_l1_vfr_event( packet );
        break;

      case WVA_INTERNAL_EVENT_DELIVER_DL_BUFFER:
        rc = wva_process_deliver_dl_buffer_event( packet );
        break;
 
      case WVA_INTERNAL_EVENT_DELIVER_UL_BUFFER:
        rc = wva_process_deliver_ul_buffer_event( packet );
        break;
 
      case VS_COMMON_EVENT_CMD_RESPONSE:
        rc = wva_process_vs_cmd_response_event( packet );
        break;
 
      case VS_VOC_EVENT_OPERATING_MODE_UPDATE:
        rc = wva_process_vs_eamr_rate_change_event( packet );
        break;

      case VS_VOC_EVENT_READY:
        rc = wva_process_vs_ready_event( packet );
        break;
      
      case VS_VOC_EVENT_NOT_READY:
        rc = wva_process_vs_not_ready_event( packet );
        break;

      default:
        /* Add remaining work items to the gating work queue. */
        rc = wva_queue_apr_packet ( packet, &wva_gating_work_pkt_q.cmd_q );
        if ( rc )
        {
          MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                 "WVA: NON-GATING: cmd pkt queuing failed rc[0x%08x]", rc );
          rc = __aprv2_cmd_free( wva_apr_handle, packet );
          WVA_REPORT_FATAL_ON_ERROR(rc);
        }
        break;
    }
  }

  return;
}

/****************************************************************************
 * GATING REQUEST( CMDs/EVENTs ) PROCESSING FUNCTIONS
 ****************************************************************************/
 
static void wva_task_process_gating_work_items ( void )
{
  uint32_t rc = APR_EOK;
  wva_work_item_t* work_item;
  wva_gating_control_t* ctrl = &wva_gating_work_pkt_q;

  for ( ;; )
  {
    switch ( ctrl->state )
    {
      case WVA_GATING_CMD_STATE_FETCH:
      {
         /* Fetch the next gating command to execute. */
         rc = apr_list_remove_head( &ctrl->cmd_q,
                                    ( ( apr_list_node_t** ) &work_item ) );
         if ( rc ) return;

         ctrl->packet = work_item->packet;
         ctrl->state = WVA_GATING_CMD_STATE_EXECUTE;

         if ( ctrl->packet == NULL )
         {
           WVA_PANIC_ON_ERROR ( APR_EUNEXPECTED );
         }

         /* Add the vs_work_item_t back to vs_free_work_pkt_q. */
         work_item->packet = NULL;
         ( void ) apr_list_add_tail( &wva_free_work_pkt_q, &work_item->link );
       }
       break;

       case WVA_GATING_CMD_STATE_EXECUTE:
       case WVA_GATING_CMD_STATE_CONTINUE:
       {
         if ( WVA_GATING_CMD_STATE_EXECUTE == ctrl->state )
         {
           MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
                  "WVA: PROCESS_GATING_WORK_ITEM: OPCODE:(0X%08X) STARTED",
                  ctrl->packet->opcode );
         }

         /* For Supported request_id, handler should take care of releasing 
          * memory allocated for packets.*/
         switch ( ctrl->packet->opcode )
         {
          case WCDMA_IVOICE_EVENT_REQUEST_START:
            rc = wva_process_wcdma_vocoder_start_event( ctrl );
            break;

          case WCDMA_IVOICE_EVENT_REQUEST_STOP:
            rc = wva_process_wcdma_vocoder_stop_event( ctrl );
            break;

          case VAGENT_IVOICE_EVENT_RESOURCE_GRANT:
            rc = wva_process_resource_grant_cmd( ctrl );
            break;

          case VAGENT_IVOICE_EVENT_RESOURCE_REVOKE:
            rc = wva_process_resource_revoke_cmd( ctrl );
            break;

          default:
            /* set to VS_EOK to fetch the next command in queue. */
            rc = APR_EOK;
            MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
                     "WVA: PROCESS_GATING_WORK_ITEM: OPCODE:(0X%08X) UNSUPPORTED",
                     ctrl->packet->opcode );
            break;
         }

         /* Evaluate the pending command completion status. */
         switch ( rc )
         {
           case APR_EOK:
           {
             MSG_1( MSG_SSID_DFLT, MSG_LEGACY_LOW,
                    "WVA: PROCESS_GATING_WORK_ITEM: OPCODE:(0X%08X) ENDED",
                    ctrl->packet->opcode );
             rc = __aprv2_cmd_free( wva_apr_handle, ctrl->packet );
             WVA_REPORT_FATAL_ON_ERROR(rc)

             ctrl->packet = NULL;
             /* The current command is finished so fetch the next command. */
             ctrl->state = WVA_GATING_CMD_STATE_FETCH;
           }
           break;

           case APR_EPENDING:
             /**
              * Assuming the current pending command control routine returns
              * APR_EPENDING the overall progress stalls until one or more
              * external events or responses are received.
              */
             ctrl->state = WVA_GATING_CMD_STATE_CONTINUE;
             /**
              * Return from here so as to avoid unecessary looping till reponse
              * is recived.
              */
             return;

           default:
             WVA_PANIC_ON_ERROR( APR_EUNEXPECTED );
             break;
         }
       }
       break;

       default:
         WVA_PANIC_ON_ERROR( rc );
         break;
    }
  }/* for loop ends. */

  return;
}

/****************************************************************************
 * WVA TASK ROUTINES                                                        *
 ****************************************************************************/

static int32_t wva_run ( void )
{
  wva_task_process_nongating_work_items( );
  wva_task_process_gating_work_items( );

  return APR_EOK;
}

static int32_t wva_worker_thread_fn (
  void* param
)
{
  int32_t rc;

  apr_event_create( &wva_work_event );
  apr_event_signal( wva_control_event );

  for ( ;; )
  {
    rc = apr_event_wait( wva_work_event );
    if ( rc ) break;

    wva_run( );
  }

  apr_event_destroy( wva_work_event );
  apr_event_signal( wva_control_event );

  return APR_EOK;
}


/****************************************************************************
 * WVA BOOT-UP and POWER-DOWN ROUTINES                                      *
 ****************************************************************************/

static uint32_t wva_gating_control_init (
  wva_gating_control_t* p_ctrl
)
{
  uint32_t rc = APR_EOK;

  if ( p_ctrl == NULL )
  {
    return APR_EBADPARAM;
  }

  rc = apr_list_init_v2( &p_ctrl->cmd_q,
                         wva_thread_lock_fn, wva_thread_unlock_fn );
  if ( rc )
  {
    return APR_EFAILED;
  }

  p_ctrl->state = WVA_GATING_CMD_STATE_FETCH;
  p_ctrl->packet = NULL;
  p_ctrl->rootjob_obj = NULL;

  return APR_EOK;
}  /* end of wva_gating_control_init () */

static uint32_t wva_gating_control_destroy (
  wva_gating_control_t* p_ctrl
)
{
  if ( p_ctrl == NULL )
  {
    return APR_EBADPARAM;
  }

  ( void ) apr_list_destroy( &p_ctrl->cmd_q );

  return APR_EOK;
}  /* end of vs_gating_control_destroy () */


/****************************************************************************
 * POWER UP/DOWN ROUTINES                                                    *
 ****************************************************************************/

static int32_t wva_init ( void )
{
  uint32_t rc = APR_EOK;
  uint32_t index;
  RCINIT_INFO info_handle = NULL;
  RCINIT_PRIO priority = 0;
  unsigned long stack_size = 0;
  voicecfg_cmd_get_bool_item_t item;

  MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH, "====== wva_init(): ======" );

  {  /* Initialize the locks. */
    rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &wva_int_lock );
    rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &wva_thread_lock );
    apr_event_create( &wva_control_event );
  }

  { /* Initialize the custom heap. */
    apr_memmgr_init_heap( &wva_heapmgr, ( ( void* ) &wva_heap_pool ),
                          sizeof( wva_heap_pool ), NULL, NULL );
  }

  { /* Initialize the object manager. */
    apr_objmgr_setup_params_t params;
  
    params.table = wva_object_table;
    params.total_bits = WVA_HANDLE_TOTAL_BITS_V;
    params.index_bits = WVA_HANDLE_INDEX_BITS_V;
    params.lock_fn = wva_int_lock_fn;
    params.unlock_fn = wva_int_unlock_fn;
    rc = apr_objmgr_construct( &wva_objmgr, &params );
  }

  { /* Initialize free and nongating work pkt queues. */
    rc = apr_list_init_v2( &wva_free_work_pkt_q, 
                           wva_int_lock_fn, wva_int_unlock_fn );
    for ( index = 0; index < WVA_NUM_WORK_PKTS_V; ++index )
    {
      ( void ) apr_list_init_node( ( apr_list_node_t* ) &wva_work_pkts[index] );
      wva_work_pkts[index].packet = NULL;
      rc = apr_list_add_tail( &wva_free_work_pkt_q,
                              ( ( apr_list_node_t* ) &wva_work_pkts[index] ) );
    }

    rc = apr_list_init_v2( &wva_nongating_work_pkt_q,
                           wva_int_lock_fn, wva_int_unlock_fn );
  }

  { /* Initialize gating work pkt queue. */
    rc = wva_gating_control_init( &wva_gating_work_pkt_q );
  }

  { /* Initialize the global session lock. */
    rc = apr_lock_create( APR_LOCK_TYPE_MUTEX, &wva_global_lock );
    WVA_PANIC_ON_ERROR( rc );
  }

  {
    mmstd_memset ( wva_task_stack, 0xFA, WVA_TASK_STACK_SIZE );
  }
  { /* Create the wva task worker thread. */
#if ( !defined AVS_MPSS_TEST )
    info_handle = rcinit_lookup( WVA_TASK_NAME );
    if ( NULL != info_handle ) 
    {
      priority = rcinit_lookup_prio_info( info_handle );
      stack_size = rcinit_lookup_stksz_info( info_handle );
    }
#endif
   
    if ( NULL == info_handle ) 
    {
      /* Use the default priority & stack_size*/
      MSG( MSG_SSID_DFLT, MSG_LEGACY_HIGH,
           "wva_init(): WVA task not registered with RCINIT" );
      priority = WVA_TASK_PRIORITY;
      stack_size = WVA_TASK_STACK_SIZE;
    }

    if ( ( priority > 255 ) || ( stack_size == 0 ) ) 
    {
      ERR_FATAL( "wva_init(): Invalid priority: %d or stack size: %d",
                 priority, stack_size, 0 );
    }

    rc = apr_thread_create( &wva_thread, WVA_TASK_NAME, TASK_PRIORITY(priority),
                            wva_task_stack, stack_size, 
                            wva_worker_thread_fn , NULL );
    WVA_PANIC_ON_ERROR( rc );

    apr_event_wait( wva_control_event );
  }

  wva_is_initialized = TRUE;

  item.id = VOICECFG_CFG_IS_3G_EAMR_ENABLED;
  item.ret_value = FALSE;
  rc =  voicecfg_call( VOICECFG_CMD_GET_BOOL_ITEM, &item, sizeof( item ) );
  if ( ! rc  && item.ret_value == TRUE )
  {
    wva_is_eamr_enabled = TRUE;
  }

  return rc;
}


static int32_t wva_postinit ( void )
{
  uint32_t rc = APR_EOK;
  uint32_t index =0;
  vagent_ivoice_cmd_register_t registerCmd;

  rc = __aprv2_cmd_register2(
         &wva_apr_handle, wva_my_dns, sizeof( wva_my_dns ), 0,
         wva_isr_dispatch_fn, NULL, &wva_my_addr );
  WVA_PANIC_ON_ERROR(rc);

  /* Initialize the mapping info and open onex voice instance. */
  for( index = 0; index < WVA_MAX_NUM_OF_SESSIONS_V; ++index )
  {
    /* Create and initialize WVA session object. */
    rc =  wva_create_session_object ( &wva_session_obj_list[index] );
    WVA_PANIC_ON_ERROR( rc );
    wva_session_obj_list[index]->asid = ( sys_modem_as_id_e_type ) index;

    /* Open CDMA voice session instance. */
    rc = wva_wcdma_open_session( wva_session_obj_list[index] );
    WVA_PANIC_ON_ERROR (rc);
  }

  /* Registring with voice agent for resource. */
  registerCmd.client_id = VAGENT_VOICE_CLIENT_WCDMA;
  registerCmd.client_cb = wva_va_event_cb;
  registerCmd.client_data = NULL;
  rc = vagent_call( VAGENT_IVOICE_CMD_REGISTER, (void*)&registerCmd, sizeof(registerCmd) );
  WVA_PANIC_ON_ERROR (rc);

  return rc;
}


static int32_t wva_predeinit ( void )
{
  uint32_t rc = APR_EOK;
  uint32_t index =0;

  /* Close onex Session instance. */
  for( index = 0; index < WVA_MAX_NUM_OF_SESSIONS_V; ++index )
  {
    /* WVA CLOSE's CDMA session instance for all available ASID. */
    ( void ) wva_wcdma_close_session( wva_session_obj_list[index] );

    /* Free WVA session object for all VSID. */
    ( void ) apr_timer_destroy ( wva_session_obj_list[index]->pktexg_timer );
    ( void ) apr_lock_destroy( wva_session_obj_list[index]->data_lock );
    ( void ) wva_mem_free_object( (wva_object_t*)wva_session_obj_list[index] );
  }

  return rc;
}


static int32_t wva_deinit ( void )
{
  uint32_t rc = APR_EOK;

  wva_is_initialized = FALSE;

  apr_event_signal_abortall( wva_work_event );
  apr_event_wait( wva_control_event );


  /* Release gating control structures */
  ( void ) wva_gating_control_destroy( &wva_gating_work_pkt_q );

  /* Release work queue */
  ( void ) apr_list_destroy( &wva_free_work_pkt_q );
  ( void ) apr_list_destroy( &wva_nongating_work_pkt_q );


  /* Deinitialize the object handle table. */
  ( void ) apr_objmgr_destruct( &wva_objmgr );

  /* Deinitialize basic OS resources for staging the setup. */
  ( void ) apr_event_destroy( wva_control_event );
  ( void ) apr_lock_destroy( wva_int_lock );
  ( void ) apr_lock_destroy( wva_thread_lock );
  ( void ) apr_lock_destroy( wva_global_lock );

  return rc;
}


WVA_EXTERNAL uint32_t wva_call (
  uint32_t cmd_id,
  void* params,
  uint32_t size
)
{
  uint32_t rc;

  if ( ( FALSE == wva_is_initialized ) && ( DRV_CMDID_INIT != cmd_id ) )
  {
    WVA_REPORT_FATAL_ON_ERROR( APR_ENOSERVICE );
    return APR_ENOSERVICE;
  }

  switch ( cmd_id )
  {
  case DRV_CMDID_INIT:
    rc = wva_init( );
    break;

  case DRV_CMDID_POSTINIT:
    rc = wva_postinit( );
    break;

  case DRV_CMDID_PREDEINIT:
    rc = wva_predeinit( );
    break;

  case DRV_CMDID_DEINIT:
    rc = wva_deinit( );
    break;

  default:
    MSG_1( MSG_SSID_DFLT, MSG_LEGACY_ERROR,
           "wva_call(): Unsupported cmd ID (0x%08x)", cmd_id );
    rc = APR_EUNSUPPORTED;
    break;
  }

  return rc;
}

